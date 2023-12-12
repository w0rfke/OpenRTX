/***************************************************************************
 *   Copyright (C) 2023 by Federico Amedeo Izzo IU2NUO,                    *
 *                         Niccolò Izzo IU2KIN                             *
 *                         Frederik Saraci IU2NRO                          *
 *                         Silvano Seva IU2KWO                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

#include <interfaces/nvmem.h>
#include <nvmem_access.h>
#include <rtxlink_dat.h>
#include <rtxlink.h>
#include <errno.h>

#define ACK   (0x06)  // ACKnowledge, receive OK
#define NAK   (0x15)  // Negative ACKnowledge, receiver ERROR, retry

static enum DatStatus        status = RTXLINK_DAT_IDLE;
static const struct nvmArea *memArea;
static uint8_t               blockCnt;
static size_t                curAddr;

static void datProtocolHandler(const uint8_t *data, size_t len)
{
    uint8_t outData[1026];
    size_t  outSize;

    switch(status)
    {
        case RTXLINK_DAT_READ:
        {
            size_t toRead = memArea->size - (curAddr - memArea->startAddr);
            if(toRead > 1024)
                toRead = 1024;

            nvmArea_read(memArea, curAddr, &outData[2], toRead);
            outData[0] = blockCnt;
            outData[1] = blockCnt ^ 0xFF;
            outSize = toRead + 2;

            if(data[0] == ACK)
            {
                // Previous frame received correctly, prepare the next
                blockCnt += 1;
                curAddr  += toRead;
            }
        }
            break;

        case RTXLINK_DAT_WRITE:
        {
            // Set up a NACK response, overridden to ACK if everything goes well
            outData[0] = NAK;
            outSize    = 1;

            // Check sequence numbers
            uint8_t bNum  = data[0];
            uint8_t ibNum = data[1];

            if(((bNum ^ ibNum) != 0xFF) || (bNum != blockCnt))
                break;

            int ret = nvmArea_write(memArea, curAddr, &data[2], len - 2);
            if(ret < 0)
                break;

            // Success: prepare next block number, update address and send ACK
            blockCnt  += 1;
            curAddr   += (len - 2);
            outData[0] = ACK;
        }
            break;

        default:
            outData[0] = NAK;
            outSize    = 1;
            break;
    }

    rtxlink_send(RTXLINK_FRAME_DAT, outData, outSize);
}


int dat_readNvmArea(const struct nvmArea *area)
{
    if(status != RTXLINK_DAT_IDLE)
        return -EBUSY;

    status   = RTXLINK_DAT_READ;
    memArea  = area;
    curAddr  = memArea->startAddr;
    blockCnt = 0;

    rtxlink_setProtocolHandler(RTXLINK_FRAME_DAT, datProtocolHandler);
    return 0;
}

int dat_writeNvmArea(const struct nvmArea *area)
{
    if(status != RTXLINK_DAT_IDLE)
        return -EBUSY;

    status = RTXLINK_DAT_WRITE;
    memArea  = area;
    curAddr  = memArea->startAddr;
    blockCnt = 0;

    rtxlink_setProtocolHandler(RTXLINK_FRAME_DAT, datProtocolHandler);

    uint8_t ready = ACK;
    rtxlink_send(RTXLINK_FRAME_DAT, &ready, 1);

    return 0;
}

enum DatStatus dat_getStatus()
{
    return status;
}

void dat_reset()
{
    rtxlink_removeProtocolHandler(RTXLINK_FRAME_DAT);
    status = RTXLINK_DAT_IDLE;
}

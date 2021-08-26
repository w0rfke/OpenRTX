/***************************************************************************
 *   Copyright (C) 2021 by Federico Amedeo Izzo IU2NUO,                    *
 *                         Niccolò Izzo IU2KIN,                            *
 *                         Frederik Saraci IU2NRO,                         *
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

#include <interfaces/audio_stream.h>
#include <interfaces/audio_path.h>
#include <interfaces/platform.h>
#include <interfaces/delays.h>
#include <memory_profiling.h>
#include <interfaces/audio.h>
#include <codec2.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dsp.h>

static const size_t audioBufSize = 320;
static const size_t dataBufSize  = 8*1024;

void *mic_task(void *arg)
{
    struct CODEC2 *codec2 = codec2_create(CODEC2_MODE_3200);
    int16_t *audioBuf = ((int16_t *) malloc(audioBufSize * sizeof(int16_t)));
    uint8_t *dataBuf  = ((uint8_t *) malloc(dataBufSize  * sizeof(uint8_t)));
    memset(dataBuf, 0x00, dataBufSize);

    audio_enableMic();
    delayMs(500);

    streamId id = inputStream_start(SOURCE_MIC, PRIO_RX, audioBuf, audioBufSize,
                                    BUF_CIRC_DOUBLE, 8000);

    platform_ledOn(GREEN);

    int16_t audio[160] = {0};
    size_t pos = 0;
    while(pos < dataBufSize)
    {
        dataBlock_t data = inputStream_getData(id);
        memcpy(audio, data.data, 160);

        dsp_dcRemoval(audio, 160);
        for(size_t i = 0; i < 160; i++)
        {
            audio[i] <<= 3;
        }

        codec2_encode(codec2, dataBuf + pos, audio);
        pos += 8;
    }

    platform_ledOff(GREEN);
    delayMs(10000);
    platform_ledOn(RED);

    for(size_t i = 0; i < dataBufSize; i++)
    {
        iprintf("%02x ", dataBuf[i]);
    }

    platform_ledOff(RED);

    while(1) ;

    return 0;
}

int main()
{
    platform_init();

    // Create mic input thread
    pthread_t      mic_thread;
    pthread_attr_t mic_attr;

    pthread_attr_init(&mic_attr);
    pthread_attr_setstacksize(&mic_attr, 20 * 1024);
    pthread_create(&mic_thread, &mic_attr, mic_task, NULL);

    while(1) ;

    return 0;
}


/***************************************************************************
 *   Copyright (C) 2020 - 2023 by Federico Amedeo Izzo IU2NUO,             *
 *                                Niccolò Izzo IU2KIN                      *
 *                                Frederik Saraci IU2NRO                   *
 *                                Silvano Seva IU2KWO                      *
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

//#include <hwconfig.h>
//#include <pthread.h>
#include <ui.h>
#include <state.h>
#include <threads.h>
#include <graphics.h>
#include "platform.h"
#include "delays.h"
#include "radio.h"
#include <event.h>
#include <rtx.h>
#include <string.h>
#include <utils.h>
#include <input.h>
#include <backup.h>
#ifdef CONFIG_GPS
#include <peripherals/gps.h>
#include <gps.h>
#endif
#include <voicePrompts.h>
//#include "openrtx.h"
#include "ST7735S.h"

//added for debugging functions
#include "string.h" //For Huart1 strlen
#include <stdio.h>
extern UART_HandleTypeDef huart1;

bool ui_threadFunc_start = 0;
bool rtx_threadFunc_start = 0;


#if defined(PLATFORM_TTWRPLUS)
#include <pmu.h>
#endif

/* Mutex for concurrent access to RTX state variable */
//pthread_mutex_t rtx_mutex;

/**
 * \internal Thread managing user input and UI
 */
void *ui_threadFunc(void *arg)
{
    (void) arg;
    kbd_msg_t   kbd_msg;
    rtxStatus_t rtx_cfg = { 0 };
    bool        sync_rtx = true;
    long long   time     = 0;

		if (ui_threadFunc_start == 0) {
			// Load initial state and update the UI
			ui_saveState();
			ui_updateGUI();

			// Keep the splash screen for one second  before rendering the new UI screen
			sleepFor(1u, 0u);
			gfx_render();
			ui_threadFunc_start = 1;
		}
		//const char *message; message = "I'm here inside ui loop\r\n"; HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
    //while(state.devStatus != SHUTDOWN)
		//break out loop to run other 'threads'

		if (state.devStatus != SHUTDOWN)
    {
        time = HAL_GetTick();
        if(input_scanKeyboard(&kbd_msg))
        {
  			//char message2[50];
				//sprintf(message2, "kbd_msg.value: %u\n\r", kbd_msg.value); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);					
					
            ui_pushEvent(EVENT_KBD, kbd_msg.value);
            //gfx_clearScreen();
        }

			

        //pthread_mutex_lock(&state_mutex);   // Lock r/w access to radio state
        ui_updateFSM(&sync_rtx);            // Update UI FSM
			
        ui_saveState();                     // Save local state copy
        //pthread_mutex_unlock(&state_mutex); // Unlock r/w access to radio state

        //vp_tick();                           // continue playing voice prompts in progress if any.

        // If synchronization needed take mutex and update RTX configuration
        if(sync_rtx)
        {
            //pthread_mutex_lock(&rtx_mutex);
            rtx_cfg.opMode      = state.channel.mode;
            rtx_cfg.bandwidth   = state.channel.bandwidth;
            rtx_cfg.rxFrequency = state.channel.rx_frequency;
            rtx_cfg.txFrequency = state.channel.tx_frequency;
            rtx_cfg.txPower     = state.channel.power;
            rtx_cfg.sqlLevel    = state.settings.sqlLevel;
            rtx_cfg.rxToneEn    = state.channel.fm.rxToneEn;
            rtx_cfg.rxTone      = ctcss_tone[state.channel.fm.rxTone];
            rtx_cfg.txToneEn    = state.channel.fm.txToneEn;
            rtx_cfg.txTone      = ctcss_tone[state.channel.fm.txTone];
            rtx_cfg.toneEn      = state.tone_enabled;
            #ifdef PLATFORM_A36PLUS
            rtx_cfg.modulation  = state.settings.rx_modulation;
            #endif

            // Enable Tx if channel allows it and we are in UI main screen
            rtx_cfg.txDisable = state.channel.rx_only || state.txDisable;

            // Copy new M17 CAN, source and destination addresses
            rtx_cfg.can = state.settings.m17_can;
            rtx_cfg.canRxEn = state.settings.m17_can_rx;
            strncpy(rtx_cfg.source_address,      state.settings.callsign, 10);
            strncpy(rtx_cfg.destination_address, state.settings.m17_dest, 10);

            //pthread_mutex_unlock(&rtx_mutex);

            rtx_configure(&rtx_cfg);
            sync_rtx = false;
        }

        // Update UI and render on screen, if necessary
        #ifndef CONFIG_GFX_NOFRAMEBUF
        if(ui_updateGUI() == true)
        {
            gfx_render();
        }
        #else
        ui_updateGUI();
        #endif


        // 40Hz update rate for keyboard and UI
        time += 25;
        //sleepUntil(time);
				
    }
    //ui_terminate();
    //gfx_terminate();

    return NULL;
}

//Move Up
/**
 * \internal Thread for RTX management.
 */
void *rtx_threadFunc(void *arg)
{
    (void) arg;

		if (rtx_threadFunc_start == 0) {
			rtx_init();
			rtx_threadFunc_start = 1;
		}

    //while(state.devStatus == RUNNING)
	  //no threads, break out loop
	  if(state.devStatus == RUNNING)
    {
        rtx_task();
    }

    //rtx_terminate();

    return NULL;
}


/**
 * \internal Thread managing the device and update the global state variable.
 */
void *main_thread(void *arg)
{
    (void) arg;

    long long time     = 0;
	    while(state.devStatus != SHUTDOWN)
    {
			  time = HAL_GetTick();

        #if defined(PLATFORM_TTWRPLUS)
        pmu_handleIRQ();
        #endif

        // Check if power off is requested
        //pthread_mutex_lock(&state_mutex);
        if(platform_pwrButtonStatus() == false)
            state.devStatus = SHUTDOWN;

        //pthread_mutex_unlock(&state_mutex);

        // Run GPS task
        #if defined(CONFIG_GPS) && !defined(MD3x0_ENABLE_DBG)
        gps_task();
        #endif

        // Run state update task
        state_task();


        // Run this loop once every 5ms
        time += 5;
        //sleepUntil(time);
				//Since we don't have thread hande all in sequentially?
				rtx_threadFunc(NULL);
				ui_threadFunc(NULL);
				//const char *message; message = "I'm here inside Main loop\r\n"; HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
    }

    #if defined(CONFIG_GPS)
    gps_terminate();
    #endif

    return NULL;
}


/**
 * \internal This function creates all the system tasks and mutexes.
 */
//void create_threads()
//{
//    // Create RTX state mutex
//    pthread_mutex_init(&rtx_mutex, NULL);
//
//    // Create rtx radio thread
//    pthread_attr_t rtx_attr;
//    pthread_attr_init(&rtx_attr);
//
//    #ifndef __ZEPHYR__
//    pthread_attr_setstacksize(&rtx_attr, RTX_TASK_STKSIZE);
//    #else
//    void *rtx_thread_stack = malloc(RTX_TASK_STKSIZE * sizeof(uint8_t));
//    pthread_attr_setstack(&rtx_attr, rtx_thread_stack, RTX_TASK_STKSIZE);
//    #endif
//
//    #ifdef _MIOSIX
//    // Max priority for RTX thread when running with miosix rtos
//    struct sched_param param;
//    param.sched_priority = sched_get_priority_max(0);
//    pthread_attr_setschedparam(&rtx_attr, &param);
//    #endif
//
//    pthread_t rtx_thread;
//    pthread_create(&rtx_thread, &rtx_attr, rtx_threadFunc, NULL);
//
//    // Create UI thread
//    pthread_attr_t ui_attr;
//    pthread_attr_init(&ui_attr);
//
//    #ifndef __ZEPHYR__
//    pthread_attr_setstacksize(&ui_attr, UI_TASK_STKSIZE);
//    #else
//    void *ui_thread_stack = malloc(UI_TASK_STKSIZE * sizeof(uint8_t));
//    pthread_attr_setstack(&ui_attr, ui_thread_stack, UI_TASK_STKSIZE);
//    #endif
//
//    pthread_t ui_thread;
//    pthread_create(&ui_thread, &ui_attr, ui_threadFunc, NULL);
//}

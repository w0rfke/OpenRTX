/***************************************************************************
 *   Copyright (C) 2022 - 2023 by Federico Amedeo Izzo IU2NUO,             *
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

#include "platform.h"
#include "keyboard.h"
#include "display.h"
#include "delays.h"
#include "cps_io.h"
//#include <peripherals/gps.h>
//#include <voicePrompts.h>
#include <graphics.h>
#include <openrtx.h>
#include <threads.h>
#include <state.h>
#include <ui.h>
#ifdef PLATFORM_LINUX
#include <stdlib.h>
#endif
#include "ST7735S.h"

extern void *main_thread(void *arg);

void openrtx_init()
{
    state.devStatus = STARTUP;

    platform_init();    // Initialize low-level platform drivers
    state_init();       // Initialize radio state
    gfx_init();         // Initialize display and graphics driver
			//display_colorWindow565(0, 0, CONFIG_SCREEN_HEIGHT, CONFIG_SCREEN_WIDTH , 0xF800);
    kbd_init();         // Initialize keyboard driver
    ui_init();          // Initialize user interface
    //vp_init();          // Initialize voice prompts
    #ifdef CONFIG_SCREEN_CONTRAST
    display_setContrast(state.settings.contrast);
    #endif
/*
    // Load codeplug from nonvolatile memory, create a new one in case of failure.
    if(cps_open(NULL) < 0)
    {
        cps_create(NULL);
        if(cps_open(NULL) < 0)
        {
            // Unrecoverable error
            #ifdef PLATFORM_LINUX
            exit(-1);
            #else
            // TODO: implement error handling for non-linux targets
            while(1) ;
            #endif
        }
    }
*/
    // Display splash screen, turn on backlight after a suitable time to
    // hide random pixels during render process
		
		//color full screen red, so that we can see the edges for a36plus
		display_colorWindow565(0, 0, 162, 132 , 0xF800);
		//draw blue where the screen SHOULD be, if we see blue something is wrong
		display_colorWindow565(0, 0, 132, 128 , 0x00F8);
    ui_drawSplashScreen();
/* //debug issue with drawrect
		color_t red =   {255,   255,   255, 255};
		point_t pos = {1,1};	
	  gfx_drawRect(pos, 50, 50, red, false);
*/		
		
    gfx_render();
    //sleepFor(3u, 30u);
    //display_setBacklightLevel(state.settings.brightness);
    sleepFor(0, 500u);
    gfx_clearScreen();

    #if defined(CONFIG_GPS)
    // Detect and initialise GPS
    state.gpsDetected = gps_detect(1000);
    if(state.gpsDetected) gps_init(9600);
    #endif
}

void *openrtx_run()
{
    state.devStatus = RUNNING;

    // Start the OpenRTX threads
    //create_threads();

    // Jump to the device management thread
    main_thread(NULL);

    // Device thread terminated, complete shutdown sequence
    state_terminate();
    platform_terminate();

    return NULL;
}

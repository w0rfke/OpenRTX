/***************************************************************************
 *   Copyright (C) 2023 by Federico Amedeo Izzo IU2NUO,                    *
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

#ifndef PINMAP_H
#define PINMAP_H

//#include <gd32f3x0.h>

// LCD display
//#define LCD_CLK GPIOB, 13
//#define LCD_DAT GPIOB, 15
//#define LCD_PWR GPIOB, 7
#define LCD_RST GPIOB, 12
#define LCD_CS GPIOB, 14
#define LCD_DC GPIOB, 13

// LEDs
#define GREEN_LED GPIOA, 14
#define RED_LED GPIOA, 13

// Analog inputs
//#define AIN_VBAT GPIOA, 1

// Push-to-talk
#define PTT_SW GPIOA, 12

// Keyboard and side keys
#define KBD_K0 GPIOD, 8
#define KBD_K1 GPIOD, 9
#define KBD_K2 GPIOD, 10
#define KBD_K3 GPIOD, 11
#define KBD_DB0 GPIOD, 12
#define KBD_DB2 GPIOD, 13
#define KBD_DB3 GPIOD, 14
#define KBD_DB1 GPIOD, 15

// External flash
#define FLASH_SDI GPIOC, 2
#define FLASH_SDO GPIOC, 3
#define FLASH_CLK GPIOB, 10
#define FLASH_CS GPIOC, 1

// BK1080
#define BK1080_CLK GPIOC, 8
#define BK1080_DAT GPIOB, 10  // Shared with external flash_SCK
#define BK1080_EN GPIOC, 9

// BK4819

#define BK4819_CLK GPIOB, 10
#define BK4819_DAT GPIOC, 12
#define BK4819_CS GPIOC, 11

// Audio control
#define MIC_SPK_EN GPIOA, 11

// RF stage
#define RF_AM_AGC GPIOA, 8
#define RFV3R_EN GPIOC, 13

#define RFV3T_EN GPIOC, 15
#define RFU3R_EN GPIOC, 14
//
//// Power button
#define PWR_SW GPIOB,0

#endif

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

#include <peripherals/gpio.h>
#include <interfaces/platform.h>
#include <platform/drivers/baseband/bk4819.h>
#include <hwconfig.h>
#include <platform/drivers/display/ST7735S_a36plus.h>
//#include <platform/drivers/ADC/ADC0_A36plus.h>
//#include "gd32f3x0_rtc.h"
//#include "gd32f3x0_pmu.h"

static const hwInfo_t hwInfo =
{
    .vhf_maxFreq = 200,
    .vhf_minFreq = 108,
    .vhf_band    = 1,
    .uhf_maxFreq = 650,
    .uhf_minFreq = 200,
    .uhf_band    = 1,
    .hw_version  = 0,
    .name        = "A36Plus F401"
};


static void MX_SPI1_Init(void)
{
  /* Enable Peripheral clock for SPI1 */
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // Enable SPI1 clock (APB2 bus)

  /* Configure GPIOA Pins 5 (SCK) and 7 (MOSI) */
  GPIOA->MODER &= ~(GPIO_MODER_MODE5_Msk | GPIO_MODER_MODE7_Msk); // Clear the mode bits for PA5 (SCK) and PA7 (MOSI)
  GPIOA->MODER |= (GPIO_MODER_MODE5_1 | GPIO_MODER_MODE7_1); // Set PA5 and PA7 to Alternate Function mode

  GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_5 | GPIO_OTYPER_OT_7); // Set PA5 and PA7 to Push-Pull
  GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED7); // Set high speed for PA5 and PA7
  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD5_Msk | GPIO_PUPDR_PUPD7_Msk); // No pull-up/pull-down for PA5 and PA7

  GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL5_Msk | GPIO_AFRL_AFSEL7_Msk); // Clear alternate function for PA5 and PA7
  GPIOA->AFR[0] |= (5 << GPIO_AFRL_AFSEL5_Pos) | (5 << GPIO_AFRL_AFSEL7_Pos); // Set PA5 and PA7 to AF5 (SPI1)

  /* Configure SPI1 parameters */

  /* Reset SPI1 CR1 register */
  SPI1->CR1 = 0;

  /* Set the configuration for SPI1 */
  SPI1->CR1 |= SPI_CR1_MSTR           // Master mode
            | SPI_CR1_BR_1            // Baud rate prescaler DIV4 (BR[2:0] = 010)
            | SPI_CR1_MSB             // MSB first
            | SPI_CR1_SSM             // Software NSS management
            | SPI_CR1_SSI             // Internal slave select high
            | SPI_CR1_CPOL            // Clock polarity low
            | SPI_CR1_CPHA            // Clock phase second edge
            | SPI_CR1_DFF;            // 8-bit data frame format

  /* Enable SPI1 */
  SPI1->CR1 |= SPI_CR1_SPE;  // Set the SPE bit to enable the SPI peripheral
}

void spi_config(void)
{
//    lcd_spi_config();
    MX_SPI1_Init();    //SPI1 is for LCD
}

void platform_init()
{
    // Configure GPIOs
    gpio_setMode(GREEN_LED, OUTPUT);
    gpio_setMode(RED_LED,   OUTPUT);
    gpio_setMode(PTT_SW,    INPUT_PULL_UP);
    spi_config();
    //backlight_init();
    //nvm_init();         // Initialize nonvolatile memory
    //rtc_initialize();   // Initialize the RTC peripheral
    gpio_setMode(AIN_VBAT, ANALOG);
    //adc0_init(); //Tris DO Later
}


void platform_terminate()
{
    // Shut down LED
    gpio_clearPin(GREEN_LED);
    gpio_clearPin(RED_LED);
    //backlight_terminate();
    
}

uint16_t platform_getVbat()
{
    // Return the ADC reading from AIN_VBAT
    //return adc0_getMeasurement(0);
}

uint8_t platform_getMicLevel()
{
    return bk4819_get_mic_level();
}

uint8_t platform_getVolumeLevel()
{
    return 0;
}

int8_t platform_getChSelector()
{
    return 0;
}

bool platform_getPttStatus()
{
    // PTT is active low
    return (gpio_readPin(PTT_SW) ? false : true);
}

bool platform_pwrButtonStatus()
{
    return !gpio_readPin(PWR_SW);
}

void platform_ledOn(led_t led)
{
    switch(led)
    {
        case GREEN:
            gpio_setPin(GREEN_LED);
            break;

        case RED:
            gpio_setPin(RED_LED);
            break;

        default:
            break;
    }
}

void platform_ledOff(led_t led)
{
    switch(led)
    {
        case GREEN:
            gpio_clearPin(GREEN_LED);
            break;

        case RED:
            gpio_clearPin(RED_LED);
            break;

        default:
            break;
    }
}

// TIMER0 interrupt handler
void TIMER0_IRQHandler()
{
//    if (timer_interrupt_flag_get(TIMER0, TIMER_INT_UP) != RESET)
//    {
//        // Clear the interrupt flag
//        timer_interrupt_flag_clear(TIMER0, TIMER_INT_UP);
//        // Change the duty cycle
//        timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, rand() % 256);
//    }
}

void platform_beepStart(uint16_t freq)
{
//    // Enable necessary peripherals
//    rcu_periph_clock_enable(RCU_GPIOA);
//    // Disable UART to make sure it doesn't interfere with the speaker
//    //usart_deinit(USART0);
//    rcu_periph_clock_disable(RCU_USART0);
//    rcu_periph_clock_enable(RCU_TIMER0);
//
//    // Configure GPIO pin A11 for alternate function (TIMER1_CH4)
//    gpio_af_set(GPIOA, GPIO_AF_2, GPIO_PIN_11);
//    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
//    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
//
//
//    // Configure Timer for PWM
//    timer_oc_parameter_struct timer_ocinitpara;
//    timer_parameter_struct timer_initpara;
//
//    // Deinitialize TIMER0
//    timer_deinit(TIMER0);
//
//    // Initialize TIMER0 parameters
//    timer_initpara.prescaler = 84; // Prescaler to get 1us per tick (assuming 84MHz system clock)
//    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
//    timer_initpara.counterdirection = TIMER_COUNTER_UP;
//    // period = 1 / (freq * 1us) - 1
//    timer_initpara.period = (uint32_t)(1.0f / ((float)freq * 0.00000101f));
//    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
//    timer_init(TIMER0, &timer_initpara);
//
//    // Initialize TIMER0 Channel 3 parameters
//    timer_ocinitpara.outputstate = TIMER_CCX_ENABLE;
//    //timer_ocinitpara.outputnstate = TIMER_CCXN_ENABLE;
//    timer_ocinitpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
//    //timer_ocinitpara.ocnpolarity = TIMER_OCN_POLARITY_LOW;
//    timer_ocinitpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
//    //timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_HIGH;
//    timer_channel_output_config(TIMER0, TIMER_CH_3, &timer_ocinitpara);
//    timer_primary_output_config(TIMER0, ENABLE);
//
//    // Set PWM mode and duty cycle
//    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, 160); // 50% duty cycle
//    timer_channel_output_mode_config(TIMER0, TIMER_CH_3, TIMER_OC_MODE_PWM0);
//    timer_channel_output_shadow_config(TIMER0, TIMER_CH_3, TIMER_OC_SHADOW_DISABLE);
//    timer_auto_reload_shadow_enable(TIMER0);
//
//    // Enable TIMER0
//    timer_enable(TIMER0);
//
//    // Enable the TIMER0 interrupt
//    timer_interrupt_enable(TIMER0, TIMER_INT_UP);
}



void platform_beepStop()
{
    // Disable the speaker and stop the timer
//    timer_disable(TIMER0);
//    rcu_periph_clock_enable(RCU_USART0);
}


// Helper function to convert BCD to normal numbers
static uint8_t bcd2dec(uint8_t bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

datetime_t platform_getCurrentTime()
{
    // Initialize the RTC peripheral
//    rtc_parameter_struct rtc_init_struct;
//    rtc_current_time_get(&rtc_init_struct);
    datetime_t t;
    // rtc_parameter_struct stores stuff in BCD
    // so, convert
//    t.year = bcd2dec(rtc_init_struct.rtc_year);
//    t.month = bcd2dec(rtc_init_struct.rtc_month);
//    t.date = bcd2dec(rtc_init_struct.rtc_date);
//    t.day = bcd2dec(rtc_init_struct.rtc_day_of_week);
//    t.hour = bcd2dec(rtc_init_struct.rtc_hour);
//    t.minute = bcd2dec(rtc_init_struct.rtc_minute);
//    t.second = bcd2dec(rtc_init_struct.rtc_second);
    return t;
}

void platform_setTime(datetime_t t)
{

//    rtc_parameter_struct rtc_init_struct;
//    rtc_current_time_get(&rtc_init_struct);
//    rtc_init_struct.rtc_year = (t.year / 10) << 4 | (t.year % 10);
//    rtc_init_struct.rtc_month = (t.month / 10) << 4 | (t.month % 10);
//    rtc_init_struct.rtc_date = (t.day / 10) << 4 | (t.day % 10);
//    rtc_init_struct.rtc_hour = (t.hour / 10) << 4 | (t.hour % 10);
//    rtc_init_struct.rtc_minute = (t.minute / 10) << 4 | (t.minute % 10);
//    rtc_init_struct.rtc_second = (t.second / 10) << 4 | (t.second % 10);
//    rtc_init(&rtc_init_struct);
}

const hwInfo_t *platform_getHwInfo()
{
    return &hwInfo;
}

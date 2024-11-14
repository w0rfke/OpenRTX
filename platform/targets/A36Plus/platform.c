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
#include <platform/drivers/ADC/ADC0_A36plus.h>
#include "gd32f3x0_rtc.h"
#include "gd32f3x0_pmu.h"
#include "state.h"

static const hwInfo_t hwInfo =
{
    .vhf_maxFreq = 200,
    .vhf_minFreq = 136,
    .vhf_band    = 1,
    .uhf_maxFreq = 650,
    .uhf_minFreq = 200,
    .uhf_band    = 1,
    .hw_version  = 0,
    .name        = "A36Plus"
};

extern state_t state;

static void lcd_spi_config(void)
{
    rcu_periph_clock_enable(LCD_GPIO_RCU);
    gpio_af_set(LCD_GPIO_PORT, GPIO_AF_0, LCD_GPIO_SCK_PIN | LCD_GPIO_SDA_PIN);
    gpio_mode_set(LCD_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LCD_GPIO_SCK_PIN | LCD_GPIO_SDA_PIN);
    gpio_output_options_set(LCD_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_GPIO_SCK_PIN | LCD_GPIO_SDA_PIN);

    gpio_mode_set(LCD_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LCD_GPIO_RST_PIN | LCD_GPIO_CS_PIN | LCD_GPIO_WR_PIN | LCD_GPIO_LIGHT_PIN);
    gpio_output_options_set(LCD_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_GPIO_RST_PIN | LCD_GPIO_CS_PIN | LCD_GPIO_WR_PIN | LCD_GPIO_LIGHT_PIN);
    spi_parameter_struct spi_init_struct;
    /* deinitialize SPI and the parameters */
    spi_i2s_deinit(SPI1);

    spi_struct_para_init(&spi_init_struct);

    /* configure SPI1 parameter */
    spi_init_struct.nss = SPI_NSS_SOFT;
    spi_init_struct.prescale = SPI_PSC_2;
    spi_init_struct.endian = SPI_ENDIAN_MSB;
    spi_init_struct.device_mode = SPI_MASTER;
    spi_init_struct.frame_size = SPI_FRAMESIZE_8BIT;
    spi_init_struct.trans_mode = SPI_TRANSMODE_BDTRANSMIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init(SPI1, &spi_init_struct);
    spi_enable(SPI1);
}

void spi_config(void)
{
    rcu_periph_clock_enable(RCU_SPI0);
    rcu_periph_clock_enable(RCU_SPI1);
    lcd_spi_config();
}

void platform_init()
{
    // Configure GPIOs
    // gpio_setMode(GREEN_LED, OUTPUT);
    // gpio_setMode(RED_LED,   OUTPUT);
    gpio_setMode(PTT_SW,    INPUT_PULL_UP);
    spi_config();
    backlight_init();
    timer_config();
    nvm_init();         // Initialize nonvolatile memory
    //rtc_initialize();   // Initialize the RTC peripheral
    gpio_setMode(AIN_VBAT, ANALOG);
    adc0_init();
}


void platform_terminate()
{
    // Shut down LED
    gpio_clearPin(GREEN_LED);
    gpio_clearPin(RED_LED);
    backlight_terminate();
    
}

uint16_t platform_getVbat()
{
    // Return the ADC reading from AIN_VBAT
    return adc0_getMeasurement(0);
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

// #define SINE_TABLE_SIZE 256
// #define PWM_FREQUENCY 440 // 1 kHz PWM frequency
// #include <math.h>
// #include <gd32f3x0_timer.h>
// #include <gd32f3x0_dma.h>

// static uint16_t sineTable[SINE_TABLE_SIZE];

// void generateSineTable()                        
// {
//     for (int i = 0; i < SINE_TABLE_SIZE; i++)
//     {
//         sineTable[i] = (uint16_t)(sin(2 * M_PI * i / SINE_TABLE_SIZE) * 0x400);
//     }
// }

#include "gd32f3x0_timer.h"
#include "gd32f3x0_rcu.h"
#include "gd32f3x0_misc.h"
#include "bk4819.h"

volatile uint8_t toggle = 0;

void timer_config(void)
{
    // Enable the clock for TIMER1
    rcu_periph_clock_enable(RCU_TIMER1);

    // Timer configuration
    timer_parameter_struct timer_initpara;
    timer_deinit(TIMER1);
    timer_initpara.prescaler = 95; // Prescaler value
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = 832; // Auto-reload value
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_init(TIMER1, &timer_initpara);

    // Enable the TIMER interrupt
    timer_interrupt_enable(TIMER1, TIMER_INT_UP);
    nvic_irq_enable(TIMER1_IRQn, 0, 0);
}

// An attempt to send an HDLC frame containing an AX.25 packet
const char message[] = {0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x39, 0x29, 0x76, 0x05, 0x02, 0x02, 0x07, 0x39, 0x76, 0x19, 0x51, 0x59, 0x02, 0x86, 0xC0, 0x0F, 0x2A, 0x16, 0xA6, 0x04, 0x8E, 0xAE, 0x96, 0xC6, 0xD6, 0x04, 0x46, 0x4E, 0xF6, 0xEE, 0x76, 0x04, 0x66, 0xF6, 0x1E, 0x04, 0x56, 0xAE, 0xB6, 0x0E, 0xCE, 0x04, 0xF6, 0x6E, 0xA6, 0x4E, 0x04, 0x2E, 0x16, 0xA6, 0x04, 0x36, 0x86, 0x5E, 0x9E, 0x04, 0x26, 0xF6, 0xE6, 0x75, 0xF3, 0x3F, 0x00};
volatile uint32_t idx = 0;

void TIMER1_IRQHandler(void)
{
    if (timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP) != RESET)
    {
        // Clear the interrupt flag
        timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);

        // Send the next character bit by bit
        if (idx < strlen(message) * 8)
        {
            // Get the next character
            char c = message[idx/8];

            // Send the bit
            if (c & (1 << idx%8))
            {
                // Write 1200Hz tone
                WriteRegister(0x71, ((1200*1032444u)) /100000u);
            }
            else
            {
                // Write 2200Hz tone
                WriteRegister(0x71, ((2200*1032444u) ) /100000u);
            }
            idx++;
        }
        else    
        {
            WriteRegister(0x71, 0);
            idx++;
            if(idx >= strlen(message)*8 + 1200)
                idx = 0;
        }
    }
}

void platform_beepStart(uint16_t freq)
{
    // Turn off the bk4819
    bk4819_rtx_off();
    usart0_IRQwrite("Beep started\r\n");
    // rtx_terminate();
    // Turn on the speaker
    BK4819_BeepStart(freq, false);
    delayMs(1000);
    gpio_setPin(MIC_SPK_EN);
    //gpio_clearPin(MIC_SPK_EN);
    // Start the timer
    timer_enable(TIMER1);


    // // Generate the sine wave lookup table
    // generateSineTable();

    // // Configure DMA to transfer sine table to PWM duty cycle register
    // rcu_periph_clock_enable(RCU_DMA);
    // dma_flag_clear(4,1);
    // dma_parameter_struct dma_init_struct;
    // dma_deinit(DMA_CH4);
    // dma_init_struct.memory_addr = (uint32_t)sineTable;
    // dma_init_struct.memory_width = 0x400;
    // dma_init_struct.periph_addr = TIMER_CH3CV(TIMER0);
    // dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    // dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    // dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    // dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    // dma_init_struct.number = 0x100;
    // dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    // dma_init(DMA_CH4, &dma_init_struct);
    // dma_circulation_enable(DMA_CH4);
    // dma_memory_to_memory_disable(DMA_CH4);
    // dma_interrupt_enable(DMA_CH4, DMA_INT_FTF);
    // dma_interrupt_flag_clear(DMA_CH4, DMA_INTF_GIF);
    // dma_channel_disable(DMA_CH4);
    // gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    // gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    // gpio_af_set(GPIOA, GPIO_AF_2, GPIO_PIN_11);
    // // Configure Timer for PWM
    // rcu_periph_clock_enable(RCU_TIMER0);
    // timer_oc_parameter_struct timer_ocinitpara;
    // timer_parameter_struct timer_initpara;
    // timer_deinit(TIMER0);
    // timer_initpara.prescaler = 0x0fec;
    // timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    // timer_initpara.counterdirection = TIMER_COUNTER_UP;
    // timer_initpara.period = SINE_TABLE_SIZE - 1;
    // timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    // timer_init(TIMER0, &timer_initpara);
    // timer_ocinitpara.outputstate = TIMER_CCX_ENABLE;
    // timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    // timer_ocinitpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
    // timer_ocinitpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    // timer_channel_output_config(TIMER0, TIMER_CH_3, &timer_ocinitpara);
    // timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, 0x400);
    // timer_channel_output_mode_config(TIMER0, TIMER_CH_3, TIMER_OC_MODE_PWM0);
    // timer_channel_output_shadow_config(TIMER0, TIMER_CH_3, TIMER_OC_SHADOW_DISABLE);
    // timer_primary_output_config(TIMER0, ENABLE);
    // timer_interrupt_enable(TIMER0, TIMER_DMAINTEN_UPDEN);
    // timer_auto_reload_shadow_enable(TIMER0);
    // timer_disable(TIMER0);
    // nvic_irq_enable(11,0,3);
    // dma_channel_enable(4);
    // dma_interrupt_enable(4,2);
    // dma_interrupt_flag_clear(4,1);

    // Turn off the speaker
    //gpio_clearPin(MIC_SPK_EN);
    //bk4819_rx_on();
}

void platform_beepStop()
{
    // HACK: Disabled so APRS speaker output can work
    return;
    gpio_setPin(MIC_SPK_EN);
    BK4819_BeepStop();
    // Turn off the speaker
    usart0_IRQwrite("Beep stopped\r\n");
    //bk4819_rx_on();
}

// Helper function to convert BCD to normal numbers
static uint8_t bcd2dec(uint8_t bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

datetime_t platform_getCurrentTime()
{
    // Initialize the RTC peripheral
    rtc_parameter_struct rtc_init_struct;
    rtc_current_time_get(&rtc_init_struct);
    datetime_t t;
    // rtc_parameter_struct stores stuff in BCD
    // so, convert
    t.year = bcd2dec(rtc_init_struct.rtc_year);
    t.month = bcd2dec(rtc_init_struct.rtc_month);
    t.date = bcd2dec(rtc_init_struct.rtc_date);
    t.day = bcd2dec(rtc_init_struct.rtc_day_of_week);
    t.hour = bcd2dec(rtc_init_struct.rtc_hour);
    t.minute = bcd2dec(rtc_init_struct.rtc_minute);
    t.second = bcd2dec(rtc_init_struct.rtc_second);
    return t;
}

void platform_setTime(datetime_t t)
{
    rtc_parameter_struct rtc_init_struct;
    rtc_current_time_get(&rtc_init_struct);
    rtc_init_struct.rtc_year = (t.year / 10) << 4 | (t.year % 10);
    rtc_init_struct.rtc_month = (t.month / 10) << 4 | (t.month % 10);
    rtc_init_struct.rtc_date = (t.day / 10) << 4 | (t.day % 10);
    rtc_init_struct.rtc_hour = (t.hour / 10) << 4 | (t.hour % 10);
    rtc_init_struct.rtc_minute = (t.minute / 10) << 4 | (t.minute % 10);
    rtc_init_struct.rtc_second = (t.second / 10) << 4 | (t.second % 10);
    rtc_init(&rtc_init_struct);
}

const hwInfo_t *platform_getHwInfo()
{
    return &hwInfo;
}

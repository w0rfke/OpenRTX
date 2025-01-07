#ifndef ST7735S_H
#define ST7735S_H

#include <stdint.h>  // For uint8_t, uint16_t
#include "stm32f4xx_ll_gpio.h"  // STM32 Low-Level GPIO functions
#include "stm32f401xe.h" // STM32F4xx definitions
#include "platform.h"
#include "graphics.h"

// Screen dimensions and pixel format
#define CONFIG_SCREEN_WIDTH  132
#define CONFIG_SCREEN_HEIGHT 128

#define FONT_UBUNTU_REGULAR
#define CONFIG_PIX_FMT_RGB565
#define CONFIG_GFX_NOFRAMEBUF
#define CONFIG_BAT_LIPO_2S

#define vsniprintf(w, x, y, z) vsnprintf(w, x, y, z) //ARMCLANG does not seem to find vsniprintf
//#define sniprintf(x, y, z) snprintf(x, y, z) //ARMCLANG does not seem to find sniprintf
#define sniprintf(x, y, ...) snprintf(x, y, __VA_ARGS__)


static const hwInfo_t hwInfo =
{
    .vhf_maxFreq = 200,
    .vhf_minFreq = 108,
    .vhf_band    = 1,
    .uhf_maxFreq = 650,
    .uhf_minFreq = 200,
    .uhf_band    = 1,
    .hw_version  = 0,
    .name        = "A36P_SIM"
};

enum Mod17Flags
{
    MOD17_FLAGS_HMI_PRESENT = 1,
    MOD17_FLAGS_SOFTPOT     = 2
};





// Macros for controlling the pins connected to the TFT display
#define TFT_CS_H()   LL_GPIO_SetOutputPin(GPIOB, LCD_CS_Pin)  // Chip Select High
#define TFT_CS_L()   LL_GPIO_ResetOutputPin(GPIOB, LCD_CS_Pin)  // Chip Select Low
#define TFT_DC_D()   LL_GPIO_SetOutputPin(GPIOB, LCD_DC_Pin)  // Data/Command Select to Data
#define TFT_DC_C()   LL_GPIO_ResetOutputPin(GPIOB, LCD_DC_Pin)  // Data/Command Select to Command
#define TFT_RES_H()  LL_GPIO_SetOutputPin(GPIOB, LCD_RST_Pin)  // Reset High
#define TFT_RES_L()  LL_GPIO_ResetOutputPin(GPIOB, LCD_RST_Pin)  // Reset Low
#define TFT_BL_H()   LL_GPIO_SetOutputPin(GPIOB, LCD_BL_Pin)  // Backlight On
#define TFT_BL_L()   LL_GPIO_ResetOutputPin(GPIOB, LCD_BL_Pin)  // Backlight Off

//Same function names as OpenRTX
//#define gpio_setPin(x)  	LL_GPIO_SetOutputPin(GPIOB, x)
//#define gpio_clearPin(x)  LL_GPIO_ResetOutputPin(GPIOB, x)
//#define gpio_setPin(x, y)  	LL_GPIO_SetOutputPin(x, y)
//#define gpio_clearPin(x, y)  LL_GPIO_ResetOutputPin(x, y)
//#define delayMs(x) 				HAL_Delay(x);

// ST7735S Command Set (Enum for better readability)
enum ST7735S_command {
    ST7735S_CMD_NOP       = 0x00U,
    ST7735S_CMD_SWRESET   = 0x01U,
    ST7735S_CMD_RDDID     = 0x04U,
    ST7735S_CMD_RDDST     = 0x09U,
    ST7735S_CMD_RDDPM     = 0x0AU,
    ST7735S_CMD_RDDMADCTL = 0x0BU,
    ST7735S_CMD_RDDCOLMOD = 0x0CU,
    ST7735S_CMD_RDDIM     = 0x0DU,
    ST7735S_CMD_RDDSM     = 0x0EU,
    ST7735S_CMD_RDDSDR    = 0x0FU,
    ST7735S_CMD_SLPIN     = 0x10U,
    ST7735S_CMD_SLPOUT    = 0x11U,
    ST7735S_CMD_PTLON     = 0x12U,
    ST7735S_CMD_NORON     = 0x13U,
    ST7735S_CMD_INVOFF    = 0x20U,
    ST7735S_CMD_INVON     = 0x21U,
    ST7735S_CMD_GAMSET    = 0x26U,
    ST7735S_CMD_DISPOFF   = 0x28U,
    ST7735S_CMD_DISPON    = 0x29U,
    ST7735S_CMD_CASET     = 0x2AU,
    ST7735S_CMD_RASET     = 0x2BU,
    ST7735S_CMD_RAMWR     = 0x2CU,
    ST7735S_CMD_RGBSET    = 0x2DU,
    ST7735S_CMD_RAMRD     = 0x2EU,
    ST7735S_CMD_PTLAR     = 0x30U,
    ST7735S_CMD_SCRLAR    = 0x33U,
    ST7735S_CMD_TEOFF     = 0x34U,
    ST7735S_CMD_TEON      = 0x35U,
    ST7735S_CMD_MADCTL    = 0x36U,
    ST7735S_CMD_VSCSAD    = 0x37U,
    ST7735S_CMD_IDMOFF    = 0x38U,
    ST7735S_CMD_IDMON     = 0x39U,
    ST7735S_CMD_COLMOD    = 0x3AU,
    ST7735S_CMD_RDID1     = 0xDAU,
    ST7735S_CMD_RDID2     = 0xDBU,
    ST7735S_CMD_RDID3     = 0xDCU,
    ST7735S_CMD_FRMCTR1   = 0xB1U,
    ST7735S_CMD_FRMCTR2   = 0xB2U,
    ST7735S_CMD_FRMCTR3   = 0xB3U,
    ST7735S_CMD_INVCTR    = 0xB4U,
    ST7735S_CMD_PWCTR1    = 0xC0U,
    ST7735S_CMD_PWCTR2    = 0xC1U,
    ST7735S_CMD_PWCTR3    = 0xC2U,
    ST7735S_CMD_PWCTR4    = 0xC3U,
    ST7735S_CMD_PWCTR5    = 0xC4U,
    ST7735S_CMD_VMCTR1    = 0xC5U,
    ST7735S_CMD_VMOFCTR   = 0xC7U,
    ST7735S_CMD_WRID2     = 0xD1U,
    ST7735S_CMD_WRID3     = 0xD2U,
    ST7735S_CMD_NVFCTR1   = 0xD9U,
    ST7735S_CMD_NVFCTR2   = 0xDEU,
    ST7735S_CMD_NVFCTR3   = 0xDFU,
    ST7735S_CMD_GMCTRP1   = 0xE0U,
    ST7735S_CMD_GMCTRN1   = 0xE1U,
    ST7735S_CMD_GCV       = 0xFCU,
};

// Function Prototypes
void display_init(void);  // Function to initialize the display
void display_colorWindow565(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);  // Function to set a color window
void display_drawBuffer(uint16_t x, uint16_t y, uint16_t height, uint16_t width, uint16_t *buffer);
//void display_drawEncodedBuffer(point_t start, uint16_t height, uint16_t width, uint8_t *buffer);
void display_drawEncodedBuffer(uint8_t *buffer);
void display_drawEncodedBuffer_area(uint8_t *buffer, rect_area_t *area);
  
void display_drawDecodedBuffer(uint16_t *buffer);
void display_sendRawPixel(uint16_t data);
void display_setWindow(uint16_t x, uint16_t y, uint16_t height, uint16_t width);
void display_sendRawPixel(uint16_t data);
void display_scroll(uint8_t line);
void display_defineScrollArea(uint16_t x, uint16_t x2);

static inline void ST7735_sendCommand(uint8_t command, uint8_t numArgs, uint8_t *args);  // Function to execute a command on the display

// Helper Inline Functions
static inline void sendByte(uint8_t data);  // Function to send a byte over SPI
static inline void sendShort(uint16_t val);  // Function to send a 16-bit value over SPI
static inline void sendCommand(uint8_t command);  // Function to send a command
static inline void sendData(uint8_t data);  // Function to send data

//debug functions
void display_contents_EncodedBuffer(uint8_t *buffer);


#endif  // ST7735S_H
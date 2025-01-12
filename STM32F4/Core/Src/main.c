/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "../inc/main.h"
#include "../inc/ST7735S.h"
#include "openrtx.h"
#include "../../../platform/targets/A36Plus/pinmap.h"


//added for debugging functions right into main
#include "string.h" //For Huart1 strlen
#include "../../../openrtx/include/core/state.h"
#include "../../../openrtx/include/core/graphics.h"
#include "../../../openrtx/include/interfaces/keyboard.h"
#include "../../../openrtx/include/core/ui.h"
#include "../../../openrtx/include/interfaces/delays.h"
#include <stdlib.h>

//added for debugging functions
#include "string.h" //For Huart1 strlen
#include <stdio.h>

// Global constant array for S-meter block widths - +10 and +20 need bigger width
//this is for screen width = 132 Pixels
static const uint8_t s_meter_widths[] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 13, 13};


#define color_black        (color_t){0, 0, 0, 255}
#define color_red         (color_t){255, 0, 0, 255}
#define color_grey        (color_t){60, 60, 60, 255}
#define color_white        (color_t){255, 255, 255, 255}
#define yellow_fab413      (color_t){250, 180, 19, 255}
    
    
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
typedef struct
{
    uint16_t b : 5;
    uint16_t g : 6;
    uint16_t r : 5;
}
rgb565_t;

static rgb565_t _true2highColor(color_t true_color)
{
    rgb565_t high_color;
    high_color.r = true_color.r >> 3;
    high_color.g = true_color.g >> 2;
    high_color.b = true_color.b >> 3;

    return high_color;
}

    uint16_t static_buffer2[2600] = {0};

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
//inline static void gfx_smeter(point_t start, color_t s[], uint16_t s_level);
static void gfx_smeter(point_t start, color_t s[], uint16_t s_level, uint16_t s_level_prev);
static void gfx_smeter_partial(point_t start, color_t s[], uint8_t s_level, uint16_t **smeter_buffer);
static void gfx_smeter_all(point_t start, color_t s[], uint8_t s_level, uint16_t **smeter_buffer);
static void gfx_smeterFillBuff(uint16_t **smeter_buffer);
static uint16_t calculateSquelchBarWidth(uint8_t sqlLevel);
static void drawSquelchBars(point_t start, uint8_t sqlLevel);
static inline void EncodedBuffertoBuffer(uint8_t *EncBuffer, uint16_t *Buffer);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();

  //Simulate openrtx without miosix
  //openrtx_init();
  //openrtx_run();
  
  
  //Manual Function testing
  
    platform_init();    // Initialize low-level platform drivers
    state_init();       // Initialize radio state
    gfx_init();
    kbd_init();
    ui_init();
    
    //display_colorWindow565(0, 0, 162, 132 , 0xF800); //RED
    display_colorWindow565(0, 0, 162, 132 , 0x00F8);
    //display_colorWindow565(0, 0, 132, 128 , 0x00F8);
    //draw black where the screen SHOULD be
    display_colorWindow565(0, 0, 132, 128 , 0x00);
    //ui_drawSplashScreen();
    

    
    
    #if CONFIG_SCREEN_HEIGHT > 64
    static const point_t    logo_orig = {0, (CONFIG_SCREEN_HEIGHT / 2) - 6};
    static const point_t    call_orig = {0, CONFIG_SCREEN_HEIGHT - 8};
    static const fontSize_t logo_font = FONT_SIZE_12PT;
    static const fontSize_t call_font = FONT_SIZE_8PT;
    #else
    static const point_t    logo_orig = {0, 19};
    static const point_t    call_orig = {0, CONFIG_SCREEN_HEIGHT - 8};
    static const fontSize_t logo_font = FONT_SIZE_8PT;
    static const fontSize_t call_font = FONT_SIZE_6PT;
    #endif
    

    
    //yellow orange gradient
    color_t s[11] = {
    {0xff, 0xff, 0xff, 255},
    {0xff, 0xfe, 0xf7, 255},
    {0xff, 0xfd, 0xf0, 255},
    {0xff, 0xfb, 0xe6, 255},
    {0xff, 0xf7, 0xd4, 255},
    {0xff, 0xf5, 0xc4, 255},
    {0xff, 0xf3, 0xb0, 255},
    {0xff, 0xee, 0x95, 255},
    {0xff, 0xd6, 0x67, 255},
    {0xff, 0x66, 0x66, 255},
    {0xff, 0x66, 0x66, 255}
    };

    volatile bool flag = true; //so that forever while loops does not get optimized
        
    uint16_t *smeter_buffer[11]= {NULL};  // Array of 11 pointers, initially NULL
    uint8_t *encoded_buffer[11]= {NULL};
    uint16_t *decoded_buffer[1]= {NULL};
    

    /*
    //test code for writing s-meter, full rewrite+print text
    point_t start;
    start.x=1;
    start.y=120; 
    point_t logo_origin;
    int8_t s_level=11;
    //Quite slow rewriting everything.
    while (1){
      for (s_level=0; s_level<=11; s_level++){
        gfx_smeter(start, s, s_level);
      }
      for (s_level=11; s_level>=0; s_level--){
        gfx_smeter(start, s, s_level);
      }
    }
    
    //end text code s-meter rewrite
    */

    point_t start;
    start.x=1;
    start.y=10; 
    
    
    start.x=1;
    start.y=111;   
    rssi_t rssi = -52  ; //typedef int32_t rssi_t;

    uint8_t squelch = 2; //0-15??
    uint8_t volume=20; //0-255? mic level
    
    //gfx_drawSmeter(start, (CONFIG_SCREEN_WIDTH-2), 14, rssi, squelch, volume, true, yellow_fab413);
    //while(1);
    

    

//s_meter test partial writing direct without buffer  
    /*
        while (flag){
    int s_level_prev = 11;
    for (int s_level=0; s_level<=11; s_level++){
      gfx_smeter(start,s, s_level, s_level_prev);  
      s_level_prev = s_level;
      HAL_Delay(17);
    }
    
    for (int s_level=11; s_level>=0; s_level--){
      gfx_smeter(start,s, s_level, s_level_prev);  
      s_level_prev = s_level;
      HAL_Delay(17);
    }
  }  
*/

   /*
//White red
    color_t s[11] = {
    {0xff, 0xff, 0xff, 255},
    {0xff, 0xff, 0xff, 255},
    {0xff, 0xff, 0xff, 255},
    {0xff, 0xff, 0xff, 255},
    {0xff, 0xff, 0xff, 255},
    {0xff, 0xff, 0xff, 255},
    {0xff, 0xff, 0xff, 255},
    {0xff, 0xff, 0xff, 255},
    {0xff, 0xff, 0xff, 255},
    {0xf2, 0x3a, 0x3a, 255},
    {0xf2, 0x3a, 0x3a, 255}
    };
*/

  /*  
s[0] = (color_t) {0xff, 0xff, 0xff, 255};
s[1] = (color_t) {0xff, 0xff, 0xff, 255};
s[2] = (color_t) {0xff, 0xff, 0xff, 255};
s[3] = (color_t) {0xff, 0xff, 0xff, 255};
s[4] = (color_t) {0xff, 0xff, 0xff, 255};
s[5] = (color_t) {0xff, 0xff, 0xff, 255};
s[6] = (color_t) {0xff, 0xff, 0xff, 255};
s[7] = (color_t) {0xff, 0xff, 0xff, 255};
s[8] = (color_t) {0xff, 0xff, 0xff, 255};
s[9] = (color_t) {0xf2, 0x3a, 0x3a, 255};
s[10] = (color_t){0xf2, 0x3a, 0x3a, 255};
  */  
  
  
    //prepare intermediate buffer for the smeter  
    for(int i = 0; i<11; i++) {
      smeter_buffer[i] = (uint16_t*)malloc(s_meter_widths[i] * 7 * sizeof(uint16_t));  // 16-bit color per pixel, 7px high
    }


    //prepare encoded buffer for the smeter..also used for other tests below, as it is big enough
    //for(int i = 0; i<11; i++) {
    //  encoded_buffer[i] = (uint8_t*)malloc(510);  // 16-bit color per pixel, 7px high
    //}
    //encoded_buffer[0] = (uint8_t*)malloc(510);  // 16-bit color per pixel, 7px high
    uint8_t static_buffer[520] = {0};
    encoded_buffer[0] = static_buffer;
    
    //uint16_t static_buffer2[2600] = {0}; move to global
    //decoded_buffer[0] = (uint16_t*)malloc(2600);  // 16-bit color per pixel, 7px high
    decoded_buffer[0] = static_buffer2;  // 16-bit color per pixel, 7px high
    
    //function to create all 11 smeter blocks to an uncmpressed buffer
    //gfx_smeterFillBuff(smeter_buffer);
    //now compress this buffer...
    for(int i = 0; i<11; i++) {
      //encode each uncompressed buffer to compressed one
    //  encode_buffer(smeter_buffer[i], (s_meter_widths[i] * 7 * sizeof(uint8_t)), encoded_buffer[i]);
    }
    //sprintf(message2, "Time for encoding 11 smeter buffers\n\r"); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
        
/*
    start.x=1;
    //write all 11 smeter blocks
    for (int i=0; i<11; i++) {
        display_drawBuffer(start.x, start.y, s_meter_widths[i], 7, smeter_buffer[i]);
        //display_drawEncodedBuffer(start, s_meter_widths[i], 7, encoded_buffer[i]);
        start.x+=s_meter_widths[i]+1;
        HAL_Delay(10);
    }
  while(1)
    ;
*/
    
    point_t logo_origin;
    //int8_t s_level=11;
    uint8_t s_level=11;
    //Quite slow rewriting everything.
    //while (1){
    //  for (s_level=0; s_level<=11; s_level++){
  //      gfx_smeter2(start, s, s_level, smeter_buffer);
    
    
    //  }
    //  for (s_level=11; s_level>=0; s_level--){
    //    gfx_smeter2(start, s, s_level, smeter_buffer);
    //  }
    //}
    
    
    //for(int j = 0; j<50; j++){
    
    
    //write test to Huart1
    start.x=1;
    start.y=1;

    static char text[32];


//    sprintf(text, "A/ 430.8821#@$^<>=*Avbfsdhsfghf");
    color_t text_color = yellow_fab413;
    
    //gfx_printBuffer(start, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, text);                             

    
     memset(encoded_buffer[0], 0x7F, 510);

    //This is testing the speed between creating/writing CRLE buffer vs original gfx_printBuffer
    point_t encoded_buffer_size;
    char message2[100];
    uint16_t colors[] = {0x00F8, 0xFFFF};
    //sprintf(text, "A/ 430.8815#@\n$^)g*j//<>j*AB\n1");
    sprintf(text, "Rx 430.47500");
    encoded_buffer_size = gfx_printtoBufferCRLE(start, FONT_SIZE_6PT, TEXT_ALIGN_LEFT, text, encoded_buffer[0], colors , 2);
    display_drawEncodedBuffer(encoded_buffer[0]);
    //display_contents_EncodedBuffer(encoded_buffer[0]);
    
    sprintf(text, "Rx 430.48125");
    rect_area_t box = gfx_compare_CrleBuffer(start, FONT_SIZE_6PT, TEXT_ALIGN_LEFT, text, encoded_buffer[0], colors , 2);
    encoded_buffer_size = gfx_printtoBufferCRLE(start, FONT_SIZE_6PT, TEXT_ALIGN_LEFT, text, encoded_buffer[0], colors , 2);
    //display_contents_EncodedBuffer(encoded_buffer[0]);
    sprintf(message2, "box.start.x: %i, box.start.y: %i, box.w: %i, box.h: %i\n\r", box.start.x, box.start.y, box.w, box.h); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
    display_drawEncodedBuffer_area(encoded_buffer[0], box); 

    sprintf(text, "Rx 430.47500");
    //start.y = 10;
    //encoded_buffer_size = gfx_printtoBufferCRLE(start, FONT_SIZE_6PT, TEXT_ALIGN_LEFT, text, encoded_buffer[0], colors , 2);
    //display_drawEncodedBuffer(encoded_buffer[0]);    
    
    
    display_drawEncodedBuffer(encoded_buffer[0]);
    for (uint16_t i = 0 ; i < 100000 ; i+=625) {
        sprintf(text, "Rx 430.%05u", i);
 //HAL_UART_Transmit(&huart1, (uint8_t *)text, strlen(text), HAL_MAX_DELAY);
         box = gfx_compare_CrleBuffer(start, FONT_SIZE_6PT, TEXT_ALIGN_LEFT, text, encoded_buffer[0], colors , 2);
          //sprintf(message2, "box.start.x: %i, box.start.y: %i, box.w: %i, box.h: %i\n\r", box.start.x, box.start.y, box.w, box.h); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
        encoded_buffer_size = gfx_printtoBufferCRLE(start, FONT_SIZE_6PT, TEXT_ALIGN_LEFT, text, encoded_buffer[0], colors , 2);
        display_drawEncodedBuffer_area(encoded_buffer[0], box);

        //display_drawEncodedBuffer(encoded_buffer[0]);
    }
while (1) ;

    encoded_buffer_size = gfx_printtoBufferCRLE(start, FONT_SIZE_6PT, TEXT_ALIGN_LEFT, text, encoded_buffer[0], colors , 2);
    display_drawEncodedBuffer(encoded_buffer[0]);
    //display_contents_EncodedBuffer(encoded_buffer[0]);
    
    //display_drawEncodedBuffer(encoded_buffer[0]);
    sprintf(text, "B/ 431.9815#z\n$^)g*K//<>j*AB\n1");
        
    //uint32_t tickstart = HAL_GetTick();
    //for (int i=0; i<1000; i++){
    //    gfx_compare_CrleBuffer(start, FONT_SIZE_6PT, TEXT_ALIGN_LEFT, text, encoded_buffer[0], colors , 2);
    //}
    //uint32_t tickend = HAL_GetTick();   
    //sprintf(message2, "Comparing Buffer 100x: %i ms\n\r", tickend - tickstart); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
//    rect_area_t box = gfx_compare_CrleBuffer(start, FONT_SIZE_6PT, TEXT_ALIGN_LEFT, text, encoded_buffer[0], colors , 2);
    sprintf(message2, "box.start.x: %i, box.start.y: %i, box.w: %i, box.h: %i\n\r", box.start.x, box.start.y, box.w, box.h); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
    encoded_buffer_size = gfx_printtoBufferCRLE(start, FONT_SIZE_6PT, TEXT_ALIGN_LEFT, text, encoded_buffer[0], colors , 2);
  
    uint32_t tickstart = HAL_GetTick();
    for (int i=0; i<1000; i++){  
        display_drawEncodedBuffer_area(encoded_buffer[0], box);
        //display_drawEncodedBuffer(encoded_buffer[0]);
        //rect_area_t box = gfx_compare_CrleBuffer(start, FONT_SIZE_6PT, TEXT_ALIGN_LEFT, text, encoded_buffer[0], colors , 2);  
    }
    uint32_t tickend = HAL_GetTick();   
    sprintf(message2, "Comparing Buffer 100x: %i ms\n\r", tickend - tickstart); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);    
    display_drawEncodedBuffer_area(encoded_buffer[0], box);  
    
    while(1);


    encoded_buffer_size = gfx_printtoBufferCRLE(start, FONT_SIZE_6PT, TEXT_ALIGN_LEFT, text, encoded_buffer[0], colors , 2);
    display_drawEncodedBuffer(encoded_buffer[0]);
    //display_contents_EncodedBuffer(encoded_buffer[0]);
    
    
    
    
 
   /*
    uint32_t tickstart = HAL_GetTick();
    for (int i=0; i<100; i++){
        encoded_buffer_size = gfx_printtoBufferCRLE(start, FONT_SIZE_6PT, TEXT_ALIGN_LEFT, text, encoded_buffer[0], colors , 2);
    }
    uint32_t tickend = HAL_GetTick();
    sprintf(message2, "Encoding Buffer 100x: %i ms\n\r", tickend - tickstart); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
    
    tickstart = HAL_GetTick();
    for (int i=0; i<100; i++){
        //sprintf(text, "A/ 430.8815#@$^<>j*A");
        //encoded_buffer_size = gfx_printtoBufferCRLE(start, FONT_SIZE_6PT, TEXT_ALIGN_LEFT, text, encoded_buffer[0], colors , 2);
        display_drawEncodedBuffer(encoded_buffer[0]);
    }
    tickend = HAL_GetTick();
    sprintf(message2, "decoding+drawing buffer 100x: %i ms\n\r", tickend - tickstart); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
    sprintf(message2, "Bytes needed for this example: 507 Bytes\n\r"); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
    sprintf(message2, "Uncompressed framebuffer for this size: %i Bytes\r\n\r\n", encoded_buffer_size.x*encoded_buffer_size.y*2 ); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
    

    start.y=40;
    start.x=1;
    encoded_buffer_size = gfx_printtoBufferCRLE(start, FONT_SIZE_6PT, TEXT_ALIGN_LEFT, text, encoded_buffer[0], colors , 2);
    EncodedBuffertoBuffer(encoded_buffer[0], decoded_buffer[0]);
    
    tickstart = HAL_GetTick();
    for (int i=0; i<100; i++){
        //sprintf(text, "A/ 430.8821#@$^<>j*AB");
        //sprintf(text, "A/ 430.8821#@\n$^')g*j//<>j*AB\n1");
        //gfx_printBuffer2(start, FONT_SIZE_6PT, TEXT_ALIGN_LEFT, text_color, text);
        display_drawDecodedBuffer(decoded_buffer[0]);
    }
    tickend = HAL_GetTick();
    //sprintf(message2, "gfx_printBuffer + wipe area behind 100x: %i ms\n\r", tickend - tickstart); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
    sprintf(message2, "Drawing directly from memory 100x: %i ms\n\r", tickend - tickstart); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
    //END testing the speed between creating/writing CRLE buffer vs original gfx_printBuffer

//
                                                    
    //gfx_printError("test", 0);
 */
    
    start.y=120;
    //gfx_printBuffer2(start, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, text);

/*
//other gradient test:
s[0] = (color_t) {0x9e, 0xc8, 0xff, 255};
s[1] = (color_t) {0x74, 0xdd, 0xff, 255};
s[2] = (color_t) {0x86, 0xff, 0xf3, 255};
s[3] = (color_t) {0xa2, 0xff, 0xbe, 255};
s[4] = (color_t) {0xec, 0xff, 0xa7, 255};
s[5] = (color_t) {0xfd, 0xe6, 0x9b, 255};
s[6] = (color_t) {0xfa, 0xc8, 0x7d, 255};
//s[7] = (color_t) {0xf8, 0xa7, 0x63, 255};
s[7] = (color_t) {0xf8, 0xa7, 0x63, 255};
s[8] = (color_t) {0xf5, 0x7D, 0x4a, 255};
s[9] = (color_t) {0xf2, 0x3a, 0x3a, 255};
s[10] = (color_t){0xf2, 0x3a, 0x3a, 255};
*/
    
    
    start.y=19;
    start.x=0;
//    uint16_t colors[] = {0x0000, 0xFFFF};
    //sprintf(text, "435.5250g@sdf/selABC");
    //gfx_printtoBufferCRLE(start, FONT_SIZE_6PT, TEXT_ALIGN_CENTER, text, encoded_buffer[0], colors , 2);  
    //display_drawEncodedBuffer(encoded_buffer[0]);    


    start.y=75;
    start.x=20;
    //uint16_t colors[] = {0x0000, 0xFFFF};
    sprintf(text, "435.5250");
    gfx_printtoBufferCRLE(start, FONT_SIZE_8PT, TEXT_ALIGN_LEFT, text, encoded_buffer[0], colors , 2);
    display_drawEncodedBuffer(encoded_buffer[0]); 
    
     
    start.y=100;
    start.x=20;
    //uint16_t colors[] = {0x0000, 0xFFFF};
    sprintf(text, "435.52500");
    gfx_printtoBufferCRLE(start, FONT_SIZE_6PT, TEXT_ALIGN_LEFT, text, encoded_buffer[0], colors , 2);
    display_drawEncodedBuffer(encoded_buffer[0]);    
    
    start.y=101;
    start.x=100;
    //colors[1] = 0xF800;
    sprintf(text, "FM");
    gfx_printtoBufferCRLE(start, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text, encoded_buffer[0], colors , 2);  
    display_drawEncodedBuffer(encoded_buffer[0]);
    

    
    start.y=114;
    start.x=119;  
    //colors[1] = 0x94b2;
    //sprintf(text, "SQL");
    //gfx_printtoBufferCRLE(start, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text, encoded_buffer[0], colors , 2);    
    //display_drawEncodedBuffer(encoded_buffer[0]);
    
    uint16_t SquelchBarWidth = calculateSquelchBarWidth(0);
    //gfx_drawRect2(start, SquelchBarWidth, 2, yellow_fab413, true, yellow_fab413);
    start.y=120; 

    //show s0 first.    
    gfx_smeter_all(start, s, 0, smeter_buffer);
    start.y=120; 
    start.x=3;
    for (int i=0; i<11; i++) {
        display_drawBuffer(start.x, start.y, s_meter_widths[i], 7, smeter_buffer[i]);
        start.x+=s_meter_widths[i]+1;
    }
    HAL_Delay(100);
    while(flag){
        start.y=120; 
        start.x=3;
        for (int i=0; i<11; i++) {
          gfx_smeter_all(start, s, i+1, smeter_buffer);
          if (i>0) {
            display_drawBuffer(start.x-s_meter_widths[i-1]-1, start.y, s_meter_widths[i-1], 7, smeter_buffer[i-1]);
          }
          display_drawBuffer(start.x, start.y, s_meter_widths[i], 7, smeter_buffer[i]);
          start.x+=s_meter_widths[i]+1;
          HAL_Delay(100);
        }
        HAL_Delay(100);
        start.y=116;
        start.x=3;
        for (int j=1; j<16; j++){
            //drawSquelchBars(start, j);
            //HAL_Delay(50);
        }
/*
        //first try squelch bar scaled to smeter
        for (int j=1; j<16; j++){
            SquelchBarWidth = calculateSquelchBarWidth(j);
            char message2[100];
            //sprintf(message2, "SquelchBarWidth: %i\n\r", SquelchBarWidth); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
            start.x=3;
            start.y=112;
            gfx_drawRect2(start, SquelchBarWidth, 2, yellow_fab413, true, yellow_fab413);
            HAL_Delay(200);
        }
*/
        
        start.y=120;       
        
        start.x=116;
        for (int i=10; i>=0; i--) {
          gfx_smeter_all(start, s, i+1, smeter_buffer);
          display_drawBuffer(start.x, start.y, s_meter_widths[i], 7, smeter_buffer[i]);
          if (i<10) {
            display_drawBuffer(start.x+s_meter_widths[i]+1, start.y, s_meter_widths[i+1], 7, smeter_buffer[i+1]);
          }
          if (i > 0) {
            start.x -= s_meter_widths[i - 1] + 1;  // Only access s_meter_widths[i - 1] if i > 0
          }
          HAL_Delay(100);
        }
        gfx_smeter_all(start, s, 0, smeter_buffer);
        display_drawBuffer(3, start.y, s_meter_widths[0], 7, smeter_buffer[0]);
        HAL_Delay(100);
        
        //gfx_smeter2(start, s, 11, smeter_buffer);
        //HAL_Delay(17);
      }

      //}
        while (1);
    
    
    //end text code s-meter rewrite
    
    
    
//keep test colors for now, gradient blue to red  
/*
s[0] = (color_t) {0x9e, 0xc8, 0xff, 255};
s[1] = (color_t) {0x74, 0xdd, 0xff, 255};
s[2] = (color_t) {0x86, 0xff, 0xf3, 255};
s[3] = (color_t) {0xa2, 0xff, 0xbe, 255};
s[4] = (color_t) {0xec, 0xff, 0xa7, 255};
s[5] = (color_t) {0xfd, 0xe6, 0x9b, 255};
s[6] = (color_t) {0xfa, 0xc8, 0x7d, 255};
//s[7] = (color_t) {0xf8, 0xa7, 0x63, 255};
s[7] = (color_t) {0xf8, 0xa7, 0x63, 255};
s[8] = (color_t) {0xf5, 0x7D, 0x4a, 255};
s[9] = (color_t) {0xf2, 0x3a, 0x3a, 255};
s[10] = (color_t){0xf2, 0x3a, 0x3a, 255};
*/
    

    
    
    //gfx_drawRect2(start, -50, -50, yellow_fab413, true, color_white);
    
    //gfx_drawRect(start, 8, 2, yellow_fab413, false);
    
    //start.x=30;  
    //gfx_drawRect(start, 50, 50, yellow_fab413, false);
  
    //gfx_render();
    //sleepFor(0u, 30u);
    //sleepFor(0, 500u);
    //gfx_clearScreen();
    
    
        //START RSSI TEST CODE
    //point_t start;
    start.x=1;
    start.y=111;   
 //   rssi_t rssi = -52  ; //typedef int32_t rssi_t;

//    uint8_t squelch = 2; //0-15??
//    uint8_t volume=20; //0-255? mic level
    
    //gfx_drawSmeter(start, (CONFIG_SCREEN_WIDTH-2), 14, rssi, squelch, volume, true, yellow_fab413);

    start.y=66;
    uint8_t level = 130;

    //gfx_drawSmeterLevel(start, (CONFIG_SCREEN_WIDTH-2), 18, rssi, level, volume, true);
    //END RSSI TEST CODE
  


  
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    __NOP();
    HAL_Delay(1000);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}



static inline void EncodedBuffertoBuffer(uint8_t *EncBuffer, uint16_t *buffer){
    // Read header information
    point_t start;
    uint8_t num_color_bits = EncBuffer[COLOR_BITS_B0];
    start.x = buffer[0] = EncBuffer[START_X_LSB_B1] | (EncBuffer[START_X_MSB_B2] << 8);
    start.y = buffer[1] = EncBuffer[START_Y_LSB_B3] | (EncBuffer[START_Y_MSB_B4] << 8);
    uint16_t width = buffer[3] = EncBuffer[WIDTH_LSB_B5] | (EncBuffer[WIDTH_MSB_B6] << 8);
    uint16_t height = buffer[4] = EncBuffer[HEIGHT_LSB_B7] | (EncBuffer[HEIGHT_MSB_B8] << 8);
    uint8_t max_colors = 1 << num_color_bits;

    // Initialize color map from the header
    uint16_t color_map[max_colors];
    for (int i = 0; i < max_colors; i++) {
        color_map[i] = EncBuffer[COLOR_START_B9 + 2 * i] | (EncBuffer[COLOR_START_B9 + 2 * i + 1] << 8);
    }

    size_t decoded_index = 0;
    size_t buffer_index = 5;
    uint8_t byte;
    uint8_t color_bits;
    uint8_t run_length;
    int encoded_index = COLOR_START_B9 + (max_colors * 2);

    const size_t num_pixels = width * height; // Total number of pixels
    // Calculate the bit masks based on the number of color bits
    uint8_t color_mask = (1 << num_color_bits) - 1;
    uint8_t run_length_shift = 8 - num_color_bits;


    // Send the pixel data from the buffer to the display
    while (decoded_index < num_pixels) {
        byte = EncBuffer[encoded_index++];
        color_bits = (byte >> run_length_shift) & color_mask;  // Extract the color bits
        run_length = (byte & ((1 << run_length_shift) - 1)) + 1;   // Extract the run length bits and adjust (1 to max run length)

        // Fill the decoded buffer with the color for the run length
        while (run_length > 0 && decoded_index < num_pixels) {
            //sendShort(color_map[color_bits]);
            buffer[buffer_index++] = color_map[color_bits];
            run_length--;
            decoded_index++;
        }
    }
}




static void gfx_smeter(point_t start, color_t s[], uint16_t s_level, uint16_t s_level_prev)
{
    start.x = 1;
    point_t logo_origin;
    color_t rect_bck_color;
    color_t text_color;

    int min_level = (s_level < s_level_prev) ? s_level : s_level_prev;
    int max_level = (s_level > s_level_prev) ? s_level : s_level_prev;

    for (int i = 1; i <= 11; i++) {
        // Skip levels that don't need updating
        if (i >= min_level && i <= max_level) {
            // Previous level is greater: turn off extra bars
            if (s_level_prev > s_level) {
                rect_bck_color = color_black;
                text_color = (i < 10) ? color_white : color_red;
            } 
            // Previous level is smaller: turn on new bars
            else {
                rect_bck_color = s[i-1];
                text_color = color_black;
            }

            gfx_drawRect2(start, s_meter_widths[i-1], 7, rect_bck_color, true, rect_bck_color);
            logo_origin.x = start.x + 4;
            logo_origin.y = start.y + 6;

            if (i < 10) {
                gfx_print(logo_origin, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, "%i", i);
            } else {
                logo_origin.x -= 2;
                gfx_print(logo_origin, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, "+%i", (i - 9) * 10);
            }
        }
        // Advance the start position horizontally
        start.x += s_meter_widths[i-1] + 1;
    }
}



//Previous version without s_level_prev
/*
static void gfx_smeter(point_t start, color_t s[], uint16_t s_level)
  {
    //point_t start;
    start.x=1;
    //start.y=120; 
    point_t logo_origin;
    //uint8_t s_level=11;
    
    color_t rect_bck_color;
    color_t text_color;
    for (int i=0; i<11; i++) {
        if (i < s_level) {
            rect_bck_color = s[i];        
            text_color = color_black;
        } else {
            rect_bck_color = color_black;
            text_color = (i < 9) ? color_white : color_red;
        }
        gfx_drawRect2(start, s_meter_widths[i], 7, rect_bck_color, true, rect_bck_color);
        logo_origin.x = start.x+4;
        logo_origin.y=start.y+6;
        if (i<9) {
            gfx_print(logo_origin, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, "%i", i+1);
        } else {
            logo_origin.x-=2;
            gfx_print(logo_origin, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, "+%i", (i-8)*10);          
        }
        start.x+=s_meter_widths[i]+1;
    }
  }
  */
//END Previous version without s_level_prev


/*
static void gfx_smeterFillBuff2(uint16_t **smeter_buffer)
{
  
      color_t s[11] = {
    {0xff, 0xff, 0xff, 255},
    {0xff, 0xfe, 0xf7, 255},
    {0xff, 0xfd, 0xf0, 255},
    {0xff, 0xfb, 0xe6, 255},
    {0xff, 0xf7, 0xd4, 255},
    {0xff, 0xf5, 0xc4, 255},
    {0xff, 0xf3, 0xb0, 255},
    {0xff, 0xee, 0x95, 255},
    {0xff, 0xd6, 0x67, 255},
    {0xff, 0x66, 0x66, 255},
    {0xff, 0x66, 0x66, 255}
    };
      
    point_t start;
    start.x=1;
    //start.y=120; 
    point_t logo_origin;
    //uint8_t s_level=11;
    int s_level=0;
    color_t rect_bck_color;
    color_t text_color;
  static char text[32];
  char message2[100];
    for (int i=0; i<11; i++) {
        // Dynamically allocate memory for the buffer
      //for testing allocate in parent function
            //smeter_buffer[i] = (uint16_t*)malloc(s_meter_widths[i] * 7 * sizeof(uint16_t));  // 16-bit color per pixel, 7px high
            //if (smeter_buffer[i] == NULL) {
            //  sprintf(message2, "smeter_buffer[%i]: NULL\n\rs_meter_widths[i]: %i, sizeof(uint16_t): %i ", i,s_meter_widths[i] ,sizeof(uint16_t)); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
            //}
          if (i < s_level) {
              rect_bck_color = s[i];        
              text_color = color_black;
          } else {
              rect_bck_color = color_black;
              text_color = (i < 9) ? color_white : color_red;
          }
          rgb565_t color565 = _true2highColor(rect_bck_color);
          int count = 0;
          for (int j = 0; j < (s_meter_widths[i] * 7*2); j++) {
              smeter_buffer[i][j] = (color565.r << 11) | (color565.g << 5) | (color565.b);  // Fill each pixel in the buffer with the color
              count++;
             //sprintf(message2, "smeter_buffer[%i][%i]: %u\n\r", i, j, smeter_buffer[i][j]); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
          }
          //sprintf(message2, "smeter_buffer[%i] count=%i\n\r", i, count); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
          
          //display_setPixel(pos.x, pos.y, (color565.r << 11) | (color565.g << 5) | (color565.b));
        
         //logo - fix later
          logo_origin.x = 3;
          logo_origin.y=6;
          if (i<9) {
              //gfx_print(logo_origin, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, "%i", i+1);
              sprintf(text, "%i", i+1);
              gfx_printToBuffer(logo_origin, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, text, smeter_buffer[i], s_meter_widths[i]);
          } else {
              logo_origin.x-=2;
              //gfx_print(logo_origin, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, "+%i", (i-8)*10);          
              sprintf(text, "+%i", (i-8)*10);
              gfx_printToBuffer(logo_origin, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, text, smeter_buffer[i], s_meter_widths[i]);
        }
          start.x+=s_meter_widths[i]+1;
      }
  }  
*/



// Function to calculate s_level and base_rssi based on squelch
inline static void calculateSquelchDetails(rssi_t squelch, int *s_level, rssi_t *base_rssi) {
    if (squelch >= -73) {
        *s_level = 9 + (squelch + 73) / 10;  // S9 to S11 (10 dB per step)
        *base_rssi = -73 + (*s_level - 9) * 10; 
    } else {
        *s_level = (squelch + 127) / 6;  // S0 to S9 (6 dB per step)
        *base_rssi = -127 + *s_level * 6;
    }
}




static void drawSquelchBars(point_t start, uint8_t sqlLevel) {
    rssi_t squelch = -127 + (sqlLevel * 66) / 15;  // Convert sqlLevel (0 to 15) into RSSI (-127.0dbm to -61dbm)
    int16_t previous_width = -1;   //starting at -1 because we don't want a pixel substracted from the first block.

    for (int i = 1; i <= 15; i++) { // Loop through all squelch levels (0 to 15) - To draw all the blocks
      
        rssi_t squelch_calc = -127 + (i * 66) / 15; 
        rssi_t base_rssi;
        int s_level;

        // Calculate s_level and base_rssi for the current squelch level
        calculateSquelchDetails(squelch_calc, &s_level, &base_rssi);
      
        // Calculate the cumulative width up to the corresponding s_level
        uint16_t current_width = 0;
        for (int j = 0; j < s_level; j++) {
            current_width += s_meter_widths[j]+1;  // Include 1px spacing between the s_level blocks
        }

        // Orange shows the current level sql, grey the next possible levels
        color_t color = (squelch_calc <= squelch) ? yellow_fab413 : (color_t){100, 100, 100, 255};
        //orange with a little less saturation:
        //color_t color = (squelch_calc <= squelch) ? (color_t){0xef, 0x9e, 0x29, 255} : (color_t){100, 100, 100, 255};

        // Calculate the # of pixels that we surpass the s_level block
        rssi_t dB_offset = squelch_calc - base_rssi;
        int fractional_pixels = (dB_offset * s_meter_widths[s_level]) / (s_level < 9 ? 6 : 10);
        current_width += fractional_pixels;

        // Print the rectangle for the current squelch level
        gfx_drawRect2(start, current_width - previous_width-1, 2, color, true, color);
        point_t start2;
        start2.y = start.y;
        start2.x = start.x +current_width - previous_width-1;
        gfx_drawRect2(start2, 1, 2, (color_t){0x30, 0x32, 0x32}, true, color); //temporary, draw a pixel in between that is not black, less contrast.
        start.x += current_width - previous_width;
        // Update the previous width for the next loop iteration
        previous_width = current_width;
    }
}





//version 1
static uint16_t calculateSquelchBarWidth(uint8_t sqlLevel) {
    rssi_t squelch = -127 + (sqlLevel * 66) / 15;  // This turns squelch (0 to 15) into RSSI (-127.0dbm to -61dbm)
    rssi_t base_rssi;
    int s_level;
    uint16_t sql_bar_width = 0;

    // Determine s_level and base_rssi, based on Squelch in dBm
     //Needed to scale squelch on the same scale.
    if (squelch >= -73) {
        s_level = 9 + (squelch + 73) / 10;  // S9 to S11 (10 dB per step)
        base_rssi = -73 + (s_level - 9) * 10; 
    } else {
        s_level = (squelch + 127) / 6;  // S0 to S9 (6 dB per step)
        base_rssi = -127 + s_level * 6;
    }

    // Calculate cumulative width up to the calculated s_level
    for (int i = 1; i <= s_level; i++) {
        sql_bar_width += s_meter_widths[i - 1] + 1;  // Include 1px spacing
    }

    // calculate additional pixel offset beyond the s-level
    rssi_t dB_offset = squelch - base_rssi;
    int fractional_pixels = (dB_offset * s_meter_widths[s_level]) / (s_level < 9 ? 6 : 10);
    sql_bar_width += fractional_pixels;
/////
//char message2[100];
//sprintf(message2, "dB_offset: %i, s-level:%i\n\r", dB_offset, s_level); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
//sprintf(message2, "fractional_pixels: %i\n\r", fractional_pixels); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);    

//      char message2[100];
//      sprintf(message2, "sql_bar_width:%i, squelch:%i\n\r", sql_bar_width, squelch); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
    
    return sql_bar_width;  // Return the computed width of the squelch bar
}





static void gfx_smeterFillBuff(uint16_t **smeter_buffer)
{
    point_t start;
    start.x=1;
    //start.y=120; 
    point_t logo_origin;
    //uint8_t s_level=11;
    int s_level=0;
    color_t rect_bck_color;
    color_t text_color;
  static char text[32];
  char message2[100];
    for (int i=0; i<11; i++) {
        // Dynamically allocate memory for the buffer
      //for testing allocate in parent function
            //smeter_buffer[i] = (uint16_t*)malloc(s_meter_widths[i] * 7 * sizeof(uint16_t));  // 16-bit color per pixel, 7px high
            //if (smeter_buffer[i] == NULL) {
            //  sprintf(message2, "smeter_buffer[%i]: NULL\n\rs_meter_widths[i]: %i, sizeof(uint16_t): %i ", i,s_meter_widths[i] ,sizeof(uint16_t)); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
            //}
          if (i < s_level) {
              rect_bck_color = color_white;        
              text_color = color_black;
          } else {
              rect_bck_color = color_white;
              text_color = color_black;
          }
          rgb565_t color565 = _true2highColor(rect_bck_color);
          int count = 0;
          for (int j = 0; j < (s_meter_widths[i] * 7*2); j++) {
              smeter_buffer[i][j] = (color565.r << 11) | (color565.g << 5) | (color565.b);  // Fill each pixel in the buffer with the color
              count++;
             //sprintf(message2, "smeter_buffer[%i][%i]: %u\n\r", i, j, smeter_buffer[i][j]); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
          }
          //sprintf(message2, "smeter_buffer[%i] count=%i\n\r", i, count); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
          
          //display_setPixel(pos.x, pos.y, (color565.r << 11) | (color565.g << 5) | (color565.b));
        
         //logo - fix later
          logo_origin.x = 3;
          logo_origin.y=6;
          if (i<9) {
              //gfx_print(logo_origin, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, "%i", i+1);
              sprintf(text, "%i", i+1);
              gfx_printToBuffer(logo_origin, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, text, smeter_buffer[i], s_meter_widths[i]);
          } else {
              logo_origin.x-=2;
              //gfx_print(logo_origin, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, "+%i", (i-8)*10);          
              sprintf(text, "+%i", (i-8)*10);
              gfx_printToBuffer(logo_origin, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, text, smeter_buffer[i], s_meter_widths[i]);
          }
          start.x+=s_meter_widths[i]+1;
      }
  }  


//Version that shows gradient  
//smeter version that shows all numbers from 1 to +20
static void gfx_smeter_all(point_t start, color_t s[], uint8_t s_level, uint16_t **smeter_buffer)
{
    //point_t start;
    start.x=1;
    //start.y=120; 
    point_t logo_origin;
    //uint8_t s_level=11;
    
    color_t rect_bck_color;
    color_t text_color;
  static char text[32];
  char message2[100]; 
  //s_level 0 
    for (int i=0; i<11; i++) {
          if (i < s_level) {
              rect_bck_color = s[i];  
              if (i == 0 || i == 2 || i == 5 || i == 8) {
                text_color = yellow_fab413;
                     text_color.r = text_color.r * 3 / 4;
                     text_color.g = text_color.g * 3 / 4;
                     text_color.b = text_color.b * 3 / 4;
  
              } else {
                  text_color = color_white;
                  //text_color = s[i];
                  text_color.r /= 3;
                  text_color.g /= 3;
                  text_color.b /= 3;
              }

            
              //if (s_level == (i+1) && s_level != 0) {
              if (s_level == (i+1) && s_level != 0) {
                   if (i == 0 || i == 2 || i == 5 || i == 8) {
                     text_color = yellow_fab413;
                     //text_color.r = text_color.r * 2 / 3;
                     //text_color.g = text_color.g * 2 / 3;
                     //text_color.b = text_color.b * 2 / 3;
                                       text_color.r /= 2;
                  text_color.g /= 2;
                  text_color.b /= 2;
                     
                   } else {
                    //text_color = color_black;
                    text_color = (color_t){0xd9,0xab,0x41,255};
                    text_color.r /= 4;
                    text_color.g /= 4;
                    text_color.b /= 4;
                    text_color = color_black;                                
                  }
              }
              
          } else {
              //rect_bck_color = color_black;
            if (i > 8) {
              rect_bck_color = (color_t){ 0x33, 0x33, 0x33, 0xff};
            }
            else 
            {
              rect_bck_color = color_black;
            }
            if (i == 0 || i == 2 || i == 5 || i == 8) {
              //text_color = yellow_fab413;
              text_color = (color_t){ 0xd7, 0x8f, 0x27, 0xff};
            } else {
             // Dark grey
              //rect_bck_color = (color_t){ 0x33, 0x33, 0x33, 0xff};

              
              //trying dynamic non black dark color

              //text_color = (i < 9) ? color_white : color_red;

              //text_color = (i < 9) ? (color_t){150,150,150,255} : (color_t){0xb3, 0x52, 0x52, 255};
              //orange text color: d9ab41
              //text_color = (i < 9) ? (color_t){0xd9,0xab,0x41,255} : (color_t){0xb3, 0x52, 0x52, 255};
              //text_color = (i < 9) ? (color_t){0xb3,0x9b,0x6b,255} : (color_t){0xb3, 0x52, 0x52, 255};
              //white below, red above s9
              text_color = (i < 9) ? (color_t){0xff,0xff,0xff,255} : (color_t){0xb3, 0x52, 0x52, 255};
            }
          }

          rgb565_t color565 = _true2highColor(rect_bck_color);
          //int count = 0;
          for (int j = 0; j < (s_meter_widths[i] * 7*2); j++) {
              smeter_buffer[i][j] = (color565.r << 11) | (color565.g << 5) | (color565.b);  // Fill each pixel in the buffer with the color
              //count++;
             //sprintf(message2, "smeter_buffer[%i][%i]: %u\n\r", i, j, smeter_buffer[i][j]); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
          }
          //sprintf(message2, "smeter_buffer[%i] count=%i\n\r", i, count); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
          
          //display_setPixel(pos.x, pos.y, (color565.r << 11) | (color565.g << 5) | (color565.b));
        
          logo_origin.x = 3;
          logo_origin.y=6;
          if (i<9) {
              //gfx_print(logo_origin, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, "%i", i+1);
              sprintf(text, "%i", i+1);
              gfx_printToBuffer(logo_origin, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, text, smeter_buffer[i], s_meter_widths[i]);
          } else {
              logo_origin.x-=2;
              //gfx_print(logo_origin, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, "+%i", (i-8)*10);          
              sprintf(text, "+%i", (i-8)*10);
              gfx_printToBuffer(logo_origin, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, text, smeter_buffer[i], s_meter_widths[i]);
        }
          start.x+=s_meter_widths[i]+1;
      }
  }  

//Version of smeter that only shows 1/3/6/9/+10/+20 - less cluttered better?
static void gfx_smeter_partial(point_t start, color_t s[], uint8_t s_level, uint16_t **smeter_buffer)
{
    //point_t start;
    start.x=1;
    //start.y=120; 
    point_t logo_origin;
    //uint8_t s_level=11;
    
    color_t rect_bck_color;
    color_t text_color;
  static char text[32];
  char message2[100]; 
  //s_level 0 
    for (int i=0; i<11; i++) {
          if (i < s_level) {
              rect_bck_color = s[i];        
              //text_color = (color_t){200,200,200,255};
              text_color = s[i];
            //only show certain values - and also filter out the number right above or below
              //if ((i == 0 || i == 2 || i == 5 || i == 8 || i == 9 || i == 10) && !((i == (s_level-2)))) {
              if ((i == 0 || i == 2 || i == 5 || i == 8 || i == 9 || i == 10) ) {
                  text_color.r = text_color.r * 2 / 3;
                  text_color.g = text_color.g * 2 / 3;
                  text_color.b = text_color.b * 2 / 3;
              }
            
              if (s_level == (i+1) && s_level != 0) {
                  //text_color = color_black;
                  text_color = (color_t){0xd9,0xab,0x41,255};
                  text_color.r = text_color.r / 3;
                  text_color.g = text_color.g / 3;
                  text_color.b = text_color.b / 3;
              }
              
          } else {
              //rect_bck_color = color_black;
              //rect_bck_color = (color_t){ 0x29, 0x028, 0x029, 0xff};
              rect_bck_color = (color_t){ 0x31, 0x31, 0x31, 0xff};
             //trying dynamic non black dark color

              text_color = rect_bck_color;
              //text_color = (i < 9) ? color_white : color_red;
                          //only show certain values
              if (i == 0 || i == 2 || i == 5 || i == 8 || i == 9 || i == 10) {
                  
                  //gray text color:
                  //text_color = (i < 9) ? (color_t){150,150,150,255} : (color_t){0xb3, 0x52, 0x52, 255};
                  //orange text color: d9ab41
                  //text_color = (i < 9) ? (color_t){0xd9,0xab,0x41,255} : (color_t){0xb3, 0x52, 0x52, 255};
                  text_color = (i < 9) ? (color_t){0xb3,0x9b,0x6b,255} : (color_t){0xb3, 0x52, 0x52, 255};
              }
          }

          rgb565_t color565 = _true2highColor(rect_bck_color);
          //int count = 0;
          for (int j = 0; j < (s_meter_widths[i] * 7*2); j++) {
              smeter_buffer[i][j] = (color565.r << 11) | (color565.g << 5) | (color565.b);  // Fill each pixel in the buffer with the color
              //count++;
             //sprintf(message2, "smeter_buffer[%i][%i]: %u\n\r", i, j, smeter_buffer[i][j]); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
          }
          //sprintf(message2, "smeter_buffer[%i] count=%i\n\r", i, count); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
          
          //display_setPixel(pos.x, pos.y, (color565.r << 11) | (color565.g << 5) | (color565.b));
        
          logo_origin.x = 3;
          logo_origin.y=6;
          if (i<9) {
              //gfx_print(logo_origin, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, "%i", i+1);
              sprintf(text, "%i", i+1);
              gfx_printToBuffer(logo_origin, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, text, smeter_buffer[i], s_meter_widths[i]);
          } else {
              logo_origin.x-=2;
              //gfx_print(logo_origin, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, "+%i", (i-8)*10);          
              sprintf(text, "+%i", (i-8)*10);
              gfx_printToBuffer(logo_origin, FONT_SIZE_5PT, TEXT_ALIGN_LEFT, text_color, text, smeter_buffer[i], s_meter_widths[i]);
        }
          start.x+=s_meter_widths[i]+1;
      }
  }  


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE2);
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_8, 84, LL_RCC_PLLP_DIV_2);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  while (LL_PWR_IsActiveFlag_VOS() == 0)
  {
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_SetSystemCoreClock(84000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
  LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  LL_ADC_InitTypeDef ADC_InitStruct = {0};
  LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
  LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**ADC1 GPIO Configuration
  PA1   ------> ADC1_IN1
  */
  GPIO_InitStruct.Pin = AIN_VBAT_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(AIN_VBAT_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
  ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
  ADC_InitStruct.SequencersScanMode = LL_ADC_SEQ_SCAN_DISABLE;
  LL_ADC_Init(ADC1, &ADC_InitStruct);
  ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
  ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
  ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
  ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
  ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
  LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
  LL_ADC_REG_SetFlagEndOfConversion(ADC1, LL_ADC_REG_FLAG_EOC_UNITARY_CONV);
  ADC_CommonInitStruct.CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV4;
  LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &ADC_CommonInitStruct);

  /** Configure Regular Channel
  */
  LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_1);
  LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_3CYCLES);
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**SPI1 GPIO Configuration
  PA5   ------> SPI1_SCK
  PA7   ------> SPI1_MOSI
  */
  GPIO_InitStruct.Pin = LCD_CLK_Pin|LCD_DAT_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 10;
  LL_SPI_Init(SPI1, &SPI_InitStruct);
  LL_SPI_Enable(SPI1);  // Enable SPI1 using LL library function
  //LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  LL_SPI_InitTypeDef SPI_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  /**SPI2 GPIO Configuration
  PC2   ------> SPI2_MISO
  PC3   ------> SPI2_MOSI
  PB10   ------> SPI2_SCK
  */
  GPIO_InitStruct.Pin = FLASH_SDI_Pin|FLASH_SDO_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = FLASH_CLK_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(FLASH_CLK_GPIO_Port, &GPIO_InitStruct);

  /* SPI2 parameter configuration*/
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 10;
  LL_SPI_Init(SPI2, &SPI_InitStruct);
//LL_SPI_SetStandard(SPI2, LL_SPI_PROTOCOL_MOTOROLA);
  LL_SPI_Enable(SPI2);  // Enable SPI1 using LL library function
  

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 57600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);

  /**/
  LL_GPIO_ResetOutputPin(GPIOC, RFV3R_EN_Pin|RFU3R_EN_Pin|RFV3T_EN_Pin|FLASH_CS_Pin
                          |BK1080_CLK_Pin|BK1080_EN_Pin|BK4819_CS_Pin);

  /**/
  LL_GPIO_ResetOutputPin(GPIOA, LCD_DC_Pin|RF_AM_AGC_Pin|MIC_SPK_EN_Pin|RED_LED_Pin
                          |GREEN_LED_Pin);

  /**/
  LL_GPIO_ResetOutputPin(GPIOB, PWR_SW_Pin|LCD_RST_Pin|LCD_DCB13_Pin|LCD_CS_Pin);

  /**/
  GPIO_InitStruct.Pin = RFV3R_EN_Pin|RFU3R_EN_Pin|RFV3T_EN_Pin|FLASH_CS_Pin
                          |BK1080_CLK_Pin|BK1080_EN_Pin|BK4819_CS_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LCD_DC_Pin|RF_AM_AGC_Pin|MIC_SPK_EN_Pin|RED_LED_Pin
                          |GREEN_LED_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = PWR_SW_Pin|LCD_RST_Pin|LCD_DCB13_Pin|LCD_CS_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = KBD_K0_Pin|KBD_K1_Pin|KBD_K2_Pin|KBD_K3_Pin
                          |KBD_DB0_Pin|KBD_DB1_Pin|KBD_DB3_Pin|KBD_DB4_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = PTT_SW_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(PTT_SW_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

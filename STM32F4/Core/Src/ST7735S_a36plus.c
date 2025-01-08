#define A36

//#include <stddef.h>
#include <stdint.h>

#include "main.h"
#include "ST7735S.h"
#include "pinmap.h"
#include "gpio-native.h"
#include "stm32f401xe.h"
#include "stm32f4xx_ll_spi.h"
#include "openrtxfunctions.h"
#include "graphics.h"

//added for debugging functions
#include "string.h" //For Huart1 strlen
#include <stdio.h>
extern UART_HandleTypeDef huart1;

static inline void sendByte(uint8_t data)
{
  // Ensure SPI is enabled
  //  if ((SPI1->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE) {
  //      LL_SPI_Enable(SPI1);  // Enable SPI1 using LL library function
  //  }
		// Wait until SPI1 is ready to transmit (TXE bit is set)
    while (!(SPI1->SR & SPI_SR_TXE));  // Wait until TX buffer is empty

    // Write the data to the SPI data register (DR)
    SPI1->DR = data;  // Directly write to the SPI1 data register
		
    // Wait until the transmission is complete (BSY flag is cleared)
    while (SPI1->SR & SPI_SR_BSY);  // Wait until SPI is no longer busy

    // Optionally, ensure TXE flag is cleared before finishing
    while (!(SPI1->SR & SPI_SR_TXE));  // Ensure TXE is cleared
}


static inline void sendShort(uint16_t val)
{
	  //Handle CS/DC Outside, minimize flips
    sendByte((val >> 8) & 0xFF);
    sendByte( val       & 0xFF);
}
/*
static inline void sendCommand(uint8_t command)
{
    gpio_clearPin(LCD_DC); //only DC to command, handle CS elsewhere
    sendByte(command);
}
*/
/*
static inline void sendData(uint8_t data)
{
    gpio_setPin(LCD_DC); //only DC to Data, handle CS elsewhere
    sendByte(data);

}*/


static inline void ST7735_sendCommand(uint8_t command, uint8_t numArgs, uint8_t *args) {
	gpio_clearPin(LCD_CS);
	gpio_clearPin(LCD_DC);	
    sendByte(command);
    if (numArgs){
	    gpio_setPin(LCD_DC);
			  // Process arguments, 'numArg' times.
        for (uint8_t i = 0; i < numArgs; i++) {
            sendByte(args[i]); // Send each argument
        }
		}
    //gpio_setPin(LCD_DC); //
    if (command != ST7735S_CMD_RAMWR && command != ST7735S_CMD_SCRLAR) { //for ramwr.. keep CS Low because display data will follow
		    gpio_setPin(LCD_CS);			
		}
}


void display_setWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
    //Send display area to write to LCD controller + prepare for write. Pixel data needs to follow after this function
    //handle CS/DC before and after, minimize flips
    #ifdef A36
    	x+=28; 	//Shift 28 pixels right for A36Plus
			//only for proteus
      if (width==162) { x = 0; }
    #endif
	
    uint8_t args[4];
    args[0] = ((x >> 8) & 0xFF);
    args[1] = ( x       & 0xFF);
    args[2] = (((x + width - 1) >> 8) & 0xFF);
    args[3] = ( (x + width - 1)       & 0xFF);
    ST7735_sendCommand(ST7735S_CMD_CASET, 4, args);

    args[0] = ((y >> 8) & 0xFF);
    args[1] = ( y       & 0xFF);
    args[2] = (((y + height - 1) >> 8) & 0xFF);
    args[3] = ( (y + height - 1)       & 0xFF);
    ST7735_sendCommand(ST7735S_CMD_RASET, 4, args);
    ST7735_sendCommand(ST7735S_CMD_RAMWR, 0, 0);
}

void display_colorWindow565(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) //send range of pixels in same color
{
    gpio_clearPin(LCD_CS);
    gpio_clearPin(LCD_DC);
    display_setWindow(x, y, width,height); 	//RASET, CASET, RAMWR

    const size_t num_pixels = (height) * (width); // Total number of pixels, each pixel is 2 Bytes
    gpio_setPin(LCD_DC); //get ready to send data

    for (size_t i = 0; i < num_pixels*2; i++) {
        sendShort(color);
    }
    gpio_setPin(LCD_CS);
    //gpio_clearPin(LCD_DC); //we clear DC before sending commands anyway
}

void display_drawBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t *buffer) {
    gpio_clearPin(LCD_CS);
    gpio_clearPin(LCD_DC);
    display_setWindow(x, y, width, height); //RASET, CASET, RAMWR

    const size_t num_pixels = height * width; // Total number of pixels (each pixel is 2 bytes)
    gpio_setPin(LCD_DC);
	
  //char message2[100];
	//sprintf(message2, "height: %u, width:%u, numpixel=%un\r", height,width, num_pixels); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
    // Send the pixel data from the buffer to the display
    for (size_t i = 0; i < num_pixels; i++) {
        sendShort(buffer[i]);  // Send 16-bit rgb565 for each pixel in the buffer
    }
    gpio_setPin(LCD_CS);         // Deselect the LCD chip
}

/**
 * @brief Draws an encoded framebuffer to the display using Color Run-Length Encoding (CRLE).
 *
 * This function reads the header information from the encoded buffer to determine the
 * starting point, width, height, and color mapping. It then decodes the pixel data and
 * sends it to the display controller.
 *
 * @param buffer: A pointer to the buffer containing the encoded pixel data, including the header.
 */
//Version 2 - Unrolled a loop to send 8 Pixels per loop - speed increase, only 0.01ms slower then Uncompressed
void display_drawEncodedBuffer(uint8_t *buffer) {
    // Read header information
    const uint8_t num_color_bits = buffer[COLOR_BITS_B0];
    const point_t buffer_start = {
        .x = *(uint16_t*)&buffer[START_X_LSB_B1],
        .y = *(uint16_t*)&buffer[START_Y_LSB_B3]
    };
    const uint16_t buffer_width = *(uint16_t*)&buffer[WIDTH_LSB_B5];
    const uint16_t buffer_height = *(uint16_t*)&buffer[HEIGHT_LSB_B7];
    const uint8_t max_colors = 1 << num_color_bits;

    // Initialize color map from the header
    uint16_t color_map[max_colors];
    const uint8_t *color_data = &buffer[COLOR_START_B9];
    for (int i = 0; i < max_colors; i++) {
        color_map[i] = *(uint16_t*)&color_data[i * 2];
    }
    int encoded_index = COLOR_START_B9 + (max_colors * 2);

    const size_t num_pixels = buffer_width * buffer_height; // Total number of pixels
    const uint8_t run_length_shift = 8 - num_color_bits;

    // Start communication with the LCD controller
    gpio_clearPin(LCD_CS);
    gpio_clearPin(LCD_DC);
    display_setWindow(buffer_start.x, buffer_start.y, buffer_width, buffer_height); // RASET, CASET, RAMWR
    gpio_setPin(LCD_DC);

    // Send the pixel data from the buffer to the display
    uint32_t pixels_remaining = num_pixels;
    
    while (pixels_remaining > 0) {
        uint8_t byte = buffer[encoded_index++];
        uint8_t color_bits = byte >> run_length_shift;           // Extract the color bits
        uint8_t run_length = (byte & ((1 << run_length_shift) - 1)) + 1;   // Extract the run length bits and adjust (1 to max run length)
        uint16_t color = color_map[color_bits];
      
        // Unrolled loop for sending pixels (8 pixels at a time when possible)
        while (run_length >= 8 && pixels_remaining >= 8) {
            sendShort(color);
            sendShort(color);
            sendShort(color);
            sendShort(color);
            sendShort(color);
            sendShort(color);
            sendShort(color);
            sendShort(color);
            run_length -= 8;
            pixels_remaining -= 8;
        }
        
        // Handle remaining pixels
        while (run_length > 0 && pixels_remaining > 0) {
            sendShort(color);
            run_length--;
            pixels_remaining--;
        }
    }
    gpio_setPin(LCD_CS);
}


/**
 * @brief Displays an encoded framebuffer within a specified area using Color Run-Length Encoding (CRLE).
 *
 * This function extracts header information from the encoded buffer to determine the
 * starting coordinates, dimensions, and color mapping. It will not display the full buffer, but it will
 * take area into account, which contains relative x,y start coordinates within the buffer and width and height.
 * So it will decode the pixel data and only transmit the pixels withing the given area to the display controller.
 *
 * @param buffer: A pointer to the buffer containing the encoded pixel data, including the header.
 * @param area:   A pointer to the rect_area_t structure defining the area to draw within.
 */
//Version2, much faster, because we try to jump to target_pixel in a separate loop
//To remember: keep current_pixel up to date after reading a byte, and do not modify it for other purposes.
void display_drawEncodedBuffer_area(uint8_t *buffer, rect_area_t area) {
    // Read header information
    const uint8_t num_color_bits = buffer[COLOR_BITS_B0];
    const point_t buffer_start = {
        .x = *(uint16_t*)&buffer[START_X_LSB_B1],
        .y = *(uint16_t*)&buffer[START_Y_LSB_B3]
    };
    const uint16_t buffer_width = *(uint16_t*)&buffer[WIDTH_LSB_B5];
    const uint16_t buffer_height = *(uint16_t*)&buffer[HEIGHT_LSB_B7];
    const uint8_t max_colors = 1 << num_color_bits;

    // Initialize color map from the header
    uint16_t color_map[max_colors];
    const uint8_t *color_data = &buffer[COLOR_START_B9];
    for (int i = 0; i < max_colors; i++) {
        color_map[i] = *(uint16_t*)&color_data[i * 2];
    }
    int encoded_index = COLOR_START_B9 + (max_colors * 2);
    //temp for test
    color_map[0] = 0xf800; //red
    
    const uint8_t run_length_shift = 8 - num_color_bits;
    
    // Calculate start pixel
    uint32_t target_pixel = (area.start.y) * buffer_width + area.start.x;  // First pixel to print
    uint32_t current_pixel = 0;
    uint16_t current_line = 0;
    
    // Start communication with the LCD controller
    gpio_clearPin(LCD_CS);
    gpio_clearPin(LCD_DC);
    display_setWindow(buffer_start.x + area.start.x, buffer_start.y + area.start.y, area.w, area.h);
    gpio_setPin(LCD_DC);
char message2[100] ;
//sprintf(message2, "buffer_width:%u\r\n",buffer_width); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);      
    // Send the pixel data from the buffer to the display
    while (current_line < area.h) {
        uint16_t color;
        uint8_t run_length;
        while (current_pixel <= target_pixel) {
            uint8_t byte = buffer[encoded_index++]; 
            run_length = (byte & ((1 << run_length_shift) - 1)) + 1;
          
//sprintf(message2, "run_length:%u\r\n",run_length); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
          
            if (current_pixel + run_length > target_pixel) {
                uint8_t color_bits = byte >> run_length_shift;
                color = color_map[color_bits];
                break;
            } else {
                current_pixel += run_length;
            }
        }
        current_pixel += run_length;
        run_length =  current_pixel - target_pixel;
        //sprintf(message2, "target_pixel:%u, encoded_index: %u, current_pixel: %u, run_length: %u, color: %u\r\n",target_pixel, encoded_index, current_pixel, run_length, color); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY); 
        uint16_t pixels_to_print = area.w;

        while (run_length > 0 && pixels_to_print > 0) {
            // We're at the start of our area
            sendShort(color);
            pixels_to_print--;
            run_length--;
            if (run_length == 0 && pixels_to_print > 0) {
               // Need new byte to continue the line
                uint8_t byte = buffer[encoded_index++];
                uint8_t color_bits = byte >> run_length_shift;
                run_length = (byte & ((1 << run_length_shift) - 1)) + 1;
                color = color_map[color_bits];
                current_pixel += run_length;
            }
        }
        // Finished line, setup for next
        current_line++;
        target_pixel += buffer_width;
    }
    gpio_setPin(LCD_CS);
}


void display_drawDecodedBuffer(uint16_t *buffer) {
    // Read header information
      point_t start = {
        .x = buffer[0],
        .y = buffer[1]
    };
    //consider using rect_area_t
    uint16_t width = buffer[3];
    uint16_t height = buffer[4];
    //uint8_t max_colors = 1 << num_color_bits;
    if (width == 0 || height == 0) {
        return;  // Invalid dimensions
    }
 
    // Initialize color map from the header
    //uint16_t color_map[max_colors];
    //for (int i = 0; i < max_colors; i++) {
    //    color_map[i] = buffer[COLOR_START_B9 + 2 * i] | (buffer[COLOR_START_B9 + 2 * i + 1] << 8);
    //}

    size_t decoded_index = 0;
    uint8_t byte;
    uint8_t color_bits;
    uint8_t run_length;
    int encoded_index = 5;

    const size_t num_pixels = width * height; // Total number of pixels
    // Calculate the bit masks based on the number of color bits
    //uint8_t color_mask = (1 << num_color_bits) - 1;
    //uint8_t run_length_shift = 8 - num_color_bits;

    // Start communication with the LCD controller
    gpio_clearPin(LCD_CS);
    gpio_clearPin(LCD_DC);
    display_setWindow(start.x, start.y, width, height); // RASET, CASET, RAMWR
    gpio_setPin(LCD_DC);

    // Send the pixel data from the buffer to the display
    while (decoded_index++ < num_pixels) {
        //byte = buffer[encoded_index++];
        //color_bits = (byte >> run_length_shift) & color_mask;  // Extract the color bits
        //run_length = (byte & ((1 << run_length_shift) - 1)) + 1;   // Extract the run length bits and adjust (1 to max run length)

        // Fill the decoded buffer with the color for the run length
        //while (run_length > 0 && decoded_index < num_pixels) {
            //sendShort(color_map[color_bits]);
            sendShort(buffer[encoded_index++]);
            //run_length--;
        //}
    }
    gpio_setPin(LCD_CS);
}


/* //TRIS previous version V1 display_drawEncodedBuffer
void display_drawEncodedBuffer(point_t start, uint16_t width, uint16_t height, uint8_t *buffer) {
    uint16_t color_map[2] = {0x0000, 0xFFFF};  // 00 -> Black, 01 -> White
    size_t decoded_index = 0;
		uint8_t byte;
		uint8_t color_bits;
		uint8_t run_length;
    
		gpio_clearPin(LCD_CS);
    gpio_clearPin(LCD_DC);
		display_setWindow(start.x, start.y, width, height); //RASET, CASET, RAMWR

    const size_t num_pixels = height * width; // Total number of pixels (each pixel is 2 bytes)
    gpio_setPin(LCD_DC);
	
  //char message2[100];
	//sprintf(message2, "height: %u, width:%u, numpixel=%un\r", height,width, num_pixels); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
    // Send the pixel data from the buffer to the display
    for (size_t i = 0; decoded_index < num_pixels; i++) {
        byte = buffer[i];
        color_bits = (byte >> 6) & 0x03;  // Extract the first 2 bits
        run_length = (byte & 0x3F) + 1;   // Extract the last 6 bits and adjust (1 to 64)

        // Fill the decoded buffer with the color for the run length
        for (uint8_t j = 0; j < run_length && decoded_index < num_pixels; j++) {
					sendShort(color_map[color_bits]);
					decoded_index++;
        }
    }
    gpio_setPin(LCD_CS);
}
//END TRIS previous version display_drawEncodedBuffer
*/

void display_init(void)
{
    // Reset display controller
		//gpio_setPin(LCD_CS);
		//delayMs(1);
	  gpio_clearPin(LCD_CS);
    gpio_clearPin(LCD_RST);
    delayMs(11);
    gpio_setPin(LCD_RST);
	  gpio_setPin(LCD_CS);
	  delayMs(121);
	
    ST7735_sendCommand(ST7735S_CMD_SLPOUT, 0, 0);
	  delayMs(121);
	
    ST7735_sendCommand(ST7735S_CMD_COLMOD, 1, (uint8_t []){0x05});
		#ifdef A36
		//Only for rotated screen in Proteus:
		//ST7735_sendCommand(ST7735S_CMD_MADCTL, 1, (uint8_t []){0xc8}); // Talkpod Original
		ST7735_sendCommand(ST7735S_CMD_MADCTL, 1, (uint8_t []){0xA0}); // RGB=0
		//ST7735_sendCommand(ST7735S_CMD_MADCTL, 1, (uint8_t []){0xA8}); // RGB=1 
		#endif
		//ST7735_sendCommand(ST7735S_CMD_GMCTRN1, 16, (uint8_t []){0x04, 0x16, 0x06, 0x0D, 0x2D, 0x26, 0x23, 0x27, 0x27, 0x25, 0x2D, 0x3B, 0x00, 0x01, 0x04, 0x13});
}

void *display_getFrameBuffer()
{
    return NULL;
}

void display_terminate()
{

}

void display_renderRows(uint8_t startRow, uint8_t endRow, void *fb)
{
    (void) startRow;
    (void) endRow;
    (void) fb;
}

void display_render(void *fb)
{
    (void) fb;
}

void display_sendRawPixel(uint16_t data)
{
    sendShort(data);
	//There is NO CS to LOW at the end, UI_MENU spectrum might be broken.. to check
}

void display_defineScrollArea(uint16_t x, uint16_t x2)
{

	/* tfa: top fixed area: nr of line from top of the frame mem and display) */
	uint16_t tfa = 160 - x2 + 1;
	/* vsa: height of the vertical scrolling area in nr of line of the frame mem
	   (not the display) from the vertical scrolling address. the first line appears
	   immediately after the bottom most line of the top fixed area. */
	uint16_t vsa = x2 - x + 1;
	/* bfa: bottom fixed are in nr of lines from bottom of the frame memory and display */
	uint16_t bfa = x + 1;

	if (tfa + vsa + bfa < 162)
		return;

	/* reset mv */
	//sendCommand(ST7735S_CMD_MADCTL);
	//sendShort(0xC8 & ~(1 << 5));
	ST7735_sendCommand(ST7735S_CMD_VSCSAD, 0, 0);
	gpio_setPin(LCD_DC);
	sendShort(tfa);
	sendShort(vsa);
	sendShort(bfa);
        gpio_clearPin(LCD_CS);
}

void display_scroll(uint8_t line)
{
	ST7735_sendCommand(ST7735S_CMD_VSCSAD, 0, 0);
	gpio_setPin(LCD_DC);
	sendShort(line);
  gpio_clearPin(LCD_CS);
}

void display_clearWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	display_colorWindow565(x, y, width, height, 0x00); //Black
}

void display_fill(uint32_t color)
{
	display_colorWindow565(0, 0, CONFIG_SCREEN_WIDTH, CONFIG_SCREEN_HEIGHT, color); //color in Arg
}

void display_setPixel(uint16_t x, uint16_t y, uint32_t color)
{
	display_colorWindow565(x, y, 1, 1, color); // Width & Height = 1 to write 1 pixel
	//setPosition(x+28, CONFIG_SCREEN_HEIGHT - y);
  //sendCommand(ST7735S_CMD_RAMWR);
  //sendShort(color & 0x0000FFFF);
	
	
}

void display_setBacklightLevel(uint8_t level)
{
     if(level > 100)
        level = 100;

    uint32_t pwmLevel = (level / 100.0) * 255;
    //TIMER_CH0CV(TIMER16) = pwmLevel;
}


//Debug functions

void display_contents_EncodedBuffer(uint8_t *buffer) {
    // Read header information
    const uint8_t num_color_bits = buffer[COLOR_BITS_B0];
    const point_t start = {
        .x = *(uint16_t*)&buffer[START_X_LSB_B1],
        .y = *(uint16_t*)&buffer[START_Y_LSB_B3]
    };
    const uint16_t width = *(uint16_t*)&buffer[WIDTH_LSB_B5];
    const uint16_t height = *(uint16_t*)&buffer[HEIGHT_LSB_B7];
    const uint8_t max_colors = 1 << num_color_bits;
    const size_t num_pixels = width * height; // Total number of pixels to process
    
    // Initialize color map from the header
    uint16_t color_map[max_colors];
    const uint8_t *color_data = &buffer[COLOR_START_B9];
    for (int i = 0; i < max_colors; i++) {
        color_map[i] = *(uint16_t*)&color_data[i * 2];
    }
    
    int encoded_index = COLOR_START_B9 + (max_colors * 2);
    
    // Calculate the bit masks based on the number of color bits
    uint8_t run_length_shift = 8 - num_color_bits;
    
    // Print header information
    char message[100];
    sprintf(message, "Image size: %dx%d (%d pixels)\nNumber of color bits: %d\r\n", 
            width, height, num_pixels, num_color_bits);
    HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
    
    // Keep track of processed pixels
    uint32_t pixels_processed = 0;
    
    // Process until we've handled all pixels
    while (pixels_processed < num_pixels) {
        uint8_t byte = buffer[encoded_index];
        uint8_t color_bits = byte >> run_length_shift;           // Extract the color bits
        uint8_t run_length = (byte & ((1 << run_length_shift) - 1)) + 1;   // Extract the run length bits and adjust
        uint16_t color = color_map[color_bits];
        
        // Format: byte_number(3chars) color(hex) run_length
        sprintf(message, "%03d %04d 0x%04X %d\r\n", encoded_index - 17, pixels_processed, color, run_length);
        HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
        
        pixels_processed += run_length;
        encoded_index++;
    }
    
    // Print summary
    sprintf(message, "\nTotal pixels processed: %u\nBytes read: %d\n", pixels_processed, encoded_index - (COLOR_START_B9 + (max_colors * 2)));
    HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
}
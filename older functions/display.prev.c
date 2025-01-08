//Version 1 - First version to replace  "2x int8_t read" with (uint16_t*) casting
//            To get variables from header.
//			  No unrolled loop for sending pixels yet (slightly slower)
/**
 * @brief Draws an encoded framebuffer to the display using Color Run-Length Encoding (CRLE).
 *
 * This function reads the header information from the encoded buffer to determine the
 * starting point, width, height, and color mapping. It then decodes the pixel data and
 * sends it to the display controller.
 *
 * @param buffer: A pointer to the buffer containing the encoded pixel data, including the header.
 */
void display_drawEncodedBuffer(uint8_t *buffer) __attribute__((optimize("O3"))) {
    // Read header information
    //point_t start;
    const uint8_t num_color_bits = buffer[COLOR_BITS_B0];
    //start.x = buffer[START_X_LSB_B1] | (buffer[START_X_MSB_B2] << 8);
    //start.y = buffer[START_Y_LSB_B3] | (buffer[START_Y_MSB_B4] << 8);
    const point_t start = {
        .x = *(uint16_t*)&buffer[START_X_LSB_B1],
        .y = *(uint16_t*)&buffer[START_Y_LSB_B3]
    };
    //uint16_t width = buffer[WIDTH_LSB_B5] | (buffer[WIDTH_MSB_B6] << 8);
    //uint16_t height = buffer[HEIGHT_LSB_B7] | (buffer[HEIGHT_MSB_B8] << 8);
    const uint16_t width = *(uint16_t*)&buffer[WIDTH_LSB_B5];
    const uint16_t height = *(uint16_t*)&buffer[HEIGHT_LSB_B7];
    const uint8_t max_colors = 1 << num_color_bits;

    // Initialize color map from the header
    uint16_t color_map[max_colors];
    const uint8_t *color_data = &buffer[COLOR_START_B9];
    for (int i = 0; i < max_colors; i++) {
        //color_map[i] = buffer[COLOR_START_B9 + 2 * i] | (buffer[COLOR_START_B9 + 2 * i + 1] << 8);
        color_map[i] = *(uint16_t*)&color_data[i * 2];
    }

    size_t decoded_index = 0;
    //uint8_t byte;
    //uint8_t color_bits;
    //uint8_t run_length;
    int encoded_index = COLOR_START_B9 + (max_colors * 2);

    const size_t num_pixels = width * height; // Total number of pixels
    // Calculate the bit masks based on the number of color bits
    uint8_t color_mask = (1 << num_color_bits) - 1;
    uint8_t run_length_shift = 8 - num_color_bits;

    // Start communication with the LCD controller
    gpio_clearPin(LCD_CS);
    gpio_clearPin(LCD_DC);
    display_setWindow(start.x, start.y, width, height); // RASET, CASET, RAMWR
    gpio_setPin(LCD_DC);

    // Send the pixel data from the buffer to the display
    while (decoded_index < num_pixels) {
        uint8_t byte = buffer[encoded_index++];
        uint8_t color_bits = byte >> run_length_shift;           // Extract the color bits
        uint8_t run_length = (byte & ((1 << run_length_shift) - 1)) + 1;   // Extract the run length bits and adjust (1 to max run length)
        uint16_t color = color_map[color_bits];
      
        // Fill the decoded buffer with the color for the run length
        while (run_length > 0 && decoded_index < num_pixels) {
            //sendShort(color_map[color_bits]);
            sendShort(color);
            run_length--;
            decoded_index++;
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
//Version1 - no optimization to get to target_pixel
void display_drawEncodedBuffer_area(uint8_t *buffer, rect_area_t *area) {
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
    uint32_t target_pixel = (area->start.y) * buffer_width + area->start.x;  // First pixel to print
    uint32_t current_pixel = 0;
    uint16_t current_line = 0;
    
    // Start communication with the LCD controller
    gpio_clearPin(LCD_CS);
    gpio_clearPin(LCD_DC);
    display_setWindow(buffer_start.x + area->start.x, buffer_start.y + area->start.y, area->w, area->h);
    gpio_setPin(LCD_DC);

    // Send the pixel data from the buffer to the display
    while (current_line < area->h) {
        uint8_t byte = buffer[encoded_index++];
        uint8_t color_bits = byte >> run_length_shift;
        uint8_t run_length = (byte & ((1 << run_length_shift) - 1)) + 1;
        uint16_t color = color_map[color_bits];
        
        while (run_length > 0) {
            if (current_pixel == target_pixel) {
                // We're at the start of our area for this line
                uint16_t pixels_to_print = area->w;
                while (pixels_to_print > 0) {
                    sendShort(color);
                    pixels_to_print--;
                    run_length--;
                    current_pixel++;
                    
                    if (run_length == 0 && pixels_to_print > 0) {
                        // Need new byte to continue the line
                        byte = buffer[encoded_index++];
                        color_bits = byte >> run_length_shift;
                        run_length = (byte & ((1 << run_length_shift) - 1)) + 1;
                        color = color_map[color_bits];
                    }
                }
                // Finished line, setup for next
                current_line++;
                //target_pixel = ((area->start.y + current_line) * buffer_width) + area->start.x;
                target_pixel += buffer_width;
            } else {
                // Skip pixels until we reach our target
                current_pixel++;
                run_length--;
            }
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
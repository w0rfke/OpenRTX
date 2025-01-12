//Version2 - mostly works, unless under high pressure in a for loop (cache/timing issues?)..there can be some artifacts.
//still todo: width/height in function call, draw border? color bits in function call
//still todo: set boundary conditions?
point_t gfx_printtoBufferCRLE(point_t start, fontSize_t size, textAlign_t alignment, const char *buf, uint8_t *encoded_buffer, uint16_t *colors, uint8_t num_colors) {
  alignment = 0;
  
    uint8_t num_color_bits = 2; //Need to be included in function call
    const GFXfont f = fonts[size];
    const uint8_t font_height = font_detail[size].font_size;
    const uint8_t total_font_height = font_detail[size].font_height;
    const size_t len = strlen(buf);
    const uint8_t color_shift_left = 8 - num_color_bits;
    const uint8_t max_run_length = (1 << color_shift_left);
    const uint8_t max_colors = 1 << num_color_bits;
    uint16_t background_color = 0;
    uint16_t foreground_color = 1;
    uint16_t encoded_index = COLOR_START_B9 + (max_colors * 2);  //define position of first data byte = end of header
    // Encoding variables
    uint16_t current_color = 0;
    uint16_t run_length = 0;
    uint8_t last_char_index = 0;          //keep last character index so we can process additional lines if needed
    uint16_t line_size = 0;
    uint16_t combined_buffer_height = total_font_height; //will containt the total height at the end
    uint8_t additional_write_lines = 0; //in case of newline or wordwrap
     
    //If we don't have buffer width & height: automatic sizing to text
    //uint16_t width = 110; uint16_t height = 25;
    uint16_t width = 0; uint16_t height = 0;
    
    uint16_t x_max;
    if (width == 0) {
        x_max = CONFIG_SCREEN_WIDTH - start.x;
    } else {
        x_max = line_size = width;
    }
    volatile uint16_t y_max = (height == 0) ? CONFIG_SCREEN_HEIGHT - start.y : height;

    //Process how many additional lines to be written  - get_line_size is not useful here
    uint16_t reset_x = 0, reset_y = 0;
    for (unsigned i = 0; i < len; i++) {
        char c = buf[i];
        GFXglyph glyph = f.glyph[c - f.first];
        //uint8_t h = glyph.height;
        //int8_t yo = glyph.yOffset;

        // Handle newline and carriage return
        if (c == '\n' || c == '\r') {
            if (c == '\r' && buf[i + 1] == '\n') {	//treating \r and \r\n the same ; buf[i+1] could point to null in null ended string.
                i++;
            }
            reset_x = 0;
            if ((reset_y += f.yAdvance) > y_max)
                break;
            additional_write_lines++;
            continue;
        }

        // Handle wrap around
        if (reset_x + glyph.xAdvance > x_max) {
            reset_x = 0;
            if ((reset_y += f.yAdvance) > y_max)
                break;
            additional_write_lines++;
            continue;
        }
        //if we don't have newline situation, then add up the glyphs
        reset_x += glyph.xAdvance;
        line_size = (reset_x > line_size) ? reset_x : line_size;
    }
    start.x = get_reset_x(alignment, line_size, start.x);
    //calculate height or use from height
    y_max = (height == 0) ? ((f.yAdvance * additional_write_lines) + total_font_height) : height;
    //   char message2[100];
    //sprintf(message2, "y_max: %i,\n\r", y_max); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);

    // Initialize the encoded buffer header    
    encoded_buffer[COLOR_BITS_B0] = num_color_bits;
    //add start position to header:
    *(uint16_t*)&encoded_buffer[START_X_LSB_B1] = start.x;
    *(uint16_t*)&encoded_buffer[START_Y_LSB_B3] = start.y;

    //set total height/width in the buffer header
    *(uint16_t*)&encoded_buffer[WIDTH_LSB_B5] = line_size;
    *(uint16_t*)&encoded_buffer[HEIGHT_LSB_B7] = total_font_height;

    // Ensure we do not exceed the provided number of colors
    for (int i = 0; i < max_colors && i < num_colors; i++) {
        *(uint16_t*)&encoded_buffer[COLOR_START_B9 + 2 * i] = colors[i];
    }

    int16_t yy_max = total_font_height - font_height;
    //limit yy repetitions within y_max height
    if (combined_buffer_height > y_max) {
        yy_max = y_max - font_height;
        combined_buffer_height = y_max;
    }
    reset_y = combined_buffer_height;

    for (uint8_t line = 0; line <= additional_write_lines; line++) {
        size_t start_char = last_char_index; //save so that we can start at the right character after newline/wordwrap      
        for (int16_t yy = -font_height; yy < yy_max; yy++) {
            current_color = background_color;
            // Background padding before writing chars in case of center/right aligment
            uint16_t padding_before = 0;
            if (alignment != TEXT_ALIGN_LEFT) {
                uint16_t line_width = get_line_size2(f, reset_x, &buf[start_char], len - start_char, x_max);
                padding_before = (alignment == TEXT_ALIGN_CENTER) ? (line_size - line_width) / 2 : (line_size - line_width);
            }
            if (padding_before) {
                //current_color = background_color;
                run_length += padding_before;
                while (run_length >= max_run_length) {
                    encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (max_run_length - 1);
                    run_length -= max_run_length;
                }
            }
            reset_x = 0;
            for (unsigned i = start_char; i < len; i++) {
                char c = buf[i];
                GFXglyph glyph = f.glyph[c - f.first];
                uint8_t *bitmap = f.bitmap;

                uint16_t bo = glyph.bitmapOffset;
                uint8_t w = glyph.width, h = glyph.height;
                int8_t xo = glyph.xOffset, yo = glyph.yOffset;

                uint8_t xx, bits = 0, bit = 0;

                if (c == '\n' || c == '\r') {                   //Newline
                    if (c == '\r' && buf[i + 1] == '\n') {	      //treating \r and \r\n the same ; buf[i+1] could point to null in null ended string.
                        i++;
                    }
                    i++;
                    last_char_index = i;
                    break;
                } else if (reset_x + glyph.xAdvance > x_max) {  //Wordwrap
                    last_char_index = i;
                    break;
                }
                // Background padding: write "glyph.xAdvance" times black pixels for lines without data
                if (yy < yo || yy >= yo + h) {
                    if (current_color != background_color && run_length > 0) { //Encode if we still have run_length of other color.
                        encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (run_length - 1);
                        run_length = 0;
                        current_color = background_color;
                    }
                    run_length += glyph.xAdvance;
                    while (run_length >= max_run_length)
                    {
                        encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (max_run_length - 1);
                        run_length -= max_run_length;
                    }
                } else {
                    // Draw bitmap and handle padding
                    int16_t bit_offset = (yy - yo) * w;  // Total bit offset
                    bo += bit_offset / 8; // For each line jump to the correct byte in bitmap
                    bits = bitmap[bo++];
                    bit = ((unsigned)bit_offset) % 8;
                    bits <<= bit;

                    for (xx = 0; xx < glyph.xAdvance; xx++) {
                        uint16_t pixel_color;
                        if ((xx < xo) || (xx >= (w + xo))) {   //Handle background padding: xoffset before char and "xAdvance - width" after char               
                            pixel_color = background_color;  					// This is background padding, don't shift "bits"
                        } else {
                            pixel_color = ((bits & 0x80) >> 7) ? foreground_color : background_color;
                            bits <<= 1;       // Shift the bits left to process the next pixel
                            bit++;
                        }

                        // Check if the pixel color is the same as the previous one
                        if (pixel_color == current_color) {
                            run_length++;                     //Same pixel color, one more repetition
                            if (run_length == max_run_length) {
                                // Encode the run
                                encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (max_run_length - 1);
                                run_length = 0;
                            }
                        } else {
                            // Color Change - Encode the previous run if needed
                            if (run_length > 0) {
                                encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (run_length - 1);
                            }
                            current_color = pixel_color;
                            run_length = 1;
                        }
                        if (!(bit & 7)) {
                            bits = bitmap[bo++];  // Load the next byte from the bitmap
                        }
                    }
                }
                reset_x += glyph.xAdvance;
            }

            // Handle background padding when line_size is longer then the characters written
            if (line_size > (reset_x + padding_before)) {
                if (current_color != background_color && run_length > 0) { //Encode first if we still have run_length of other color.
                    encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (run_length - 1);
                    run_length = 0;
                    current_color = background_color;
                }
                run_length += line_size - reset_x - padding_before;   //Add all padding at once
                while (run_length >= max_run_length) {
                    encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (max_run_length - 1);
                    run_length -= max_run_length;
                }
            }
            //Start a new encoded byte when a new line starts. That will make compare easier. This will slightly increase size.
            if (run_length > 0) {
                encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (run_length - 1);
                run_length = 0;
            }
        }

        if ((line < additional_write_lines) || (combined_buffer_height < y_max)) {
            int8_t extra_pixel_lines;
            if (line < additional_write_lines) { //Prepare for next line - skip the last cycle so that encoded_buffer[HEIGHT] is correct
                reset_y += f.yAdvance;
                extra_pixel_lines = f.yAdvance - total_font_height; //Empty pixel line(s) between the printed lines
                //check vertical screen boundary
                if (reset_y >= y_max) {
                    yy_max = y_max - (f.yAdvance * (line + 1)) - font_height;
                    combined_buffer_height = y_max;
                } else {
                    combined_buffer_height += extra_pixel_lines + yy_max + font_height;  //update height in header  
                }
                //run_length += extra_pixel_lines * line_size;
            } else {  //if height is defined, add padding after the last line
                //run_length = line_size *  (y_max - combined_buffer_height);
				extra_pixel_lines = y_max-combined_buffer_height;
                combined_buffer_height = y_max;
            }
            for (int k = 0 ; k < extra_pixel_lines ; k++) {
                current_color = background_color;
                run_length = line_size;
                while (run_length >= max_run_length) {
                    encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (max_run_length - 1);
                    run_length -= max_run_length;
                }
                if (run_length > 0) { //Encode first if we still have run_length of other color.
                    encoded_buffer[encoded_index++] = ( current_color << color_shift_left) | (run_length - 1);
                    run_length = 0;
                    //current_color = background_color;
                }
            }
        }
    }
    *(uint16_t*)&encoded_buffer[HEIGHT_LSB_B7] = combined_buffer_height;

    // Calculate text size
	return (point_t){line_size, combined_buffer_height};
}


//END Tris Test Printing to encoded buffer right away


//TRIS version with wiping screen behind the buffer characters
point_t gfx_printBuffer2(point_t start, fontSize_t size, textAlign_t alignment,
                        color_t color, const char *buf)
{
    GFXfont f = fonts[size];
    size_t len = strlen(buf);
    uint8_t font_height = font_detail[size].font_size;
    uint8_t total_font_height = font_detail[size].font_height;
    
    //uint16_t line_size = get_line_size(f, start.x, buf, len);
    uint16_t x_max = CONFIG_SCREEN_WIDTH - start.x;
    uint16_t line_size = 0;
  	//uint8_t additional_write_lines = 0; //in case of newline or wrap around

    //Process how many additional lines to be written  - get_line_size is not useful here
    uint16_t reset_x = 0;
    for (unsigned i = 0; i < len; i++) {
        char c = buf[i];
        GFXglyph glyph = f.glyph[c - f.first];
        uint8_t h = glyph.height;
        int8_t yo = glyph.yOffset;

        // Handle newline and carriage return
        if (c == '\n' || c == '\r') {
            if (c == '\r' && buf[i+1] == '\n') {	//treating \r and \r\n the same ; buf[i+1] could point to null in null ended string.
                i++;
            }
            reset_x = 0;
            //additional_write_lines++;
            continue;
        }

        // Handle wrap around
        if (reset_x + glyph.xAdvance > x_max) {
            reset_x = 0;
            //additional_write_lines++;
            continue;
        }
        //if we don't have newline situation, then add up the glyphs
        reset_x += glyph.xAdvance;
        line_size = (reset_x > line_size) ? reset_x : line_size;	 
    }
    start.x = reset_x = get_reset_x(alignment, line_size, start.x);
    
    reset_x = get_reset_x(alignment, line_size, start.x);
    //clear screen behind the area where we will print the first line
    display_colorWindow565(reset_x, start.y-font_height,line_size,f.yAdvance , 0xF8);
    //display_colorWindow565(1, 33,131, 9, 0xF8);
  
    //TRIS FIX: Keeping saved_start_x for positioning new line
    start.x = reset_x;
    // Save initial start.y value to calculate vertical size
    uint16_t saved_start_y = start.y;
    uint16_t line_h = 0;

     //if we have newline or wrap around, we need clear screen behind it fist
     bool writeanotherline = 0;
    /* For each char in the string */
    for(unsigned i = 0; i < len; i++)
      {
        char c = buf[i];
        GFXglyph glyph = f.glyph[c - f.first];
        uint8_t *bitmap = f.bitmap;

        uint16_t bo = glyph.bitmapOffset;
        uint8_t w = glyph.width, h = glyph.height;
        int8_t xo = glyph.xOffset,
               yo = glyph.yOffset;
        uint8_t xx, yy, bits = 0, bit = 0;
        line_h = total_font_height;
    

        // Handle newline and carriage return
        if (c == '\n')
        {
            if(alignment!=TEXT_ALIGN_CENTER)
            {
                start.x = reset_x;
            }
            else
            {
                //Move up, before line size  
                start.x = get_reset_x(alignment, line_size, reset_x);
                line_size = get_line_size(f, start.x, &buf[i+1], len-(i+1));
                //TRIS - FIX: replaced start.x by reset_x, which contains the intial value
                //start.x = reset_x = get_reset_x(alignment, line_size, reset_x);
            }
            start.y += f.yAdvance;
            writeanotherline = 1;
            continue;
        }
        else if (c == '\r')
        {
            start.x = reset_x;
            continue;
        }

        // Handle wrap around
        //if ((start.x + glyph.xAdvance) > (CONFIG_SCREEN_WIDTH - reset_x))  //bug fix?
        if (start.x + glyph.xAdvance > CONFIG_SCREEN_WIDTH)
        {
             //Move up, before line size  
             start.x = get_reset_x(alignment, line_size, reset_x);
             line_size = get_line_size(f, start.x, &buf[i], len-(i));
            //TRIS - FIX: replaced start.x by reset_x, which contains the intial value
            //start.x = reset_x = get_reset_x(alignment, line_size, reset_x); 
            start.y += f.yAdvance;
            writeanotherline = 1;
        }
    
        if (writeanotherline) {
            display_colorWindow565(reset_x, start.y - font_height,line_size,f.yAdvance , 0xF8);
            writeanotherline = 0;
        }

        // Draw bitmap
        for (yy = 0; yy < h; yy++)
        {
            for (xx = 0; xx < w; xx++)
            {
                if (!(bit++ & 7))
                {
                    bits = bitmap[bo++];
                }

                if (bits & 0x80)
                {
                    if (start.y + yo + yy < CONFIG_SCREEN_HEIGHT &&
                        start.x + xo + xx < CONFIG_SCREEN_WIDTH &&
                        start.y + yo + yy > 0 &&
                        start.x + xo + xx > 0)
                    {
                        point_t pos;
                        pos.x = start.x + xo + xx;
                        pos.y = start.y + yo + yy;
                        gfx_setPixel(pos, color);

                    }
                }

                bits <<= 1;
            }
        }

        start.x += glyph.xAdvance;
    }
    // Calculate text size
    point_t text_size = {0, 0};
    text_size.x = line_size;
    text_size.y = (saved_start_y - start.y) + line_h;
    return text_size;
}





//Version1, lots of things still not working
point_t gfx_printtoBufferCRLE(point_t start, fontSize_t size, textAlign_t alignment, const char *buf, uint8_t *encoded_buffer, uint16_t *colors, uint8_t num_colors) {
    GFXfont f = fonts[size];
    size_t len = strlen(buf);

    // Compute size of the first row in pixels
    uint16_t line_size = get_line_size(f, buf, len);
    uint16_t reset_x = start.x = get_reset_x(alignment, line_size, start.x);
    //uint16_t saved_start_x = start.x = reset_x;
    uint8_t font_height = font_sizes[size];
	uint8_t extra_font_height = 0;
    // Save initial start.y value to calculate vertical size
    uint16_t saved_start_y = start.y;
    uint16_t line_h = 0;
    int16_t yy;

    // Initialize the encoded buffer header
    uint8_t num_color_bits = 2; // Tris for testing
    encoded_buffer[COLOR_BITS_B0] = num_color_bits;
    uint8_t max_colors = 1 << num_color_bits;
    int encoded_index = COLOR_START_B9 + (max_colors * 2);

    encoded_buffer[START_X_LSB_B1] = start.x & 0xFF;
    encoded_buffer[START_X_MSB_B2] = (start.x >> 8) & 0xFF;
    encoded_buffer[START_Y_LSB_B3] = start.y & 0xFF;
    encoded_buffer[START_Y_MSB_B4] = (start.y >> 8) & 0xFF;
    encoded_buffer[WIDTH_LSB_B5] = line_size & 0xFF;
    encoded_buffer[WIDTH_MSB_B6] = (line_size >> 8) & 0xFF;
    //encoded_buffer[HEIGHT_LSB_B7] = (font_height) & 0xFF;
    //encoded_buffer[HEIGHT_MSB_B8] = (font_height >> 8) & 0xFF;


    // Ensure we do not exceed the provided number of colors
    for (int i = 0; i < max_colors && i < num_colors; i++) { 
        encoded_buffer[COLOR_START_B9 + 2*i] = colors[i] & 0xFF;               //first color is used as background. corresponds to current_color==0
        encoded_buffer[COLOR_START_B9 + 2*i + 1] = (colors[i] >> 8) & 0xFF;   //second color is used as text color. corresponds to current_color==1
    }

    uint16_t current_color = 0;  // Start with the background color (black)
    uint8_t run_length = 0;
    uint8_t color_shift_left = 8 - num_color_bits;
    uint8_t max_run_length = (1 << color_shift_left) - 1;
    //Font height comes from array font_size, but can be increased if we encounter characters that go lower then yo==0
		// For each char in the string
    for (unsigned i = 0; i < len; i++) {
        char c = buf[i];
        GFXglyph glyph = f.glyph[c - f.first];
        //uint8_t *bitmap = f.bitmap;
        //uint16_t bo = glyph.bitmapOffset;
        //uint8_t w = glyph.width, h = glyph.height;
        uint8_t h = glyph.height;
        //int8_t xo = glyph.xOffset, yo = glyph.yOffset;
        int8_t yo = glyph.yOffset;
        //start.y = saved_start_y;


        //uint8_t xx, bits = 0, bit = 0;
        line_h = h;
			
			  //handle newline cr and wrap around later.

        // Handle newline and carriage return
        if (c == '\n') {
            if (alignment != TEXT_ALIGN_CENTER) {
                start.x = reset_x;
            } else {
                line_size = get_line_size(f, &buf[i + 1], len - (i + 1));
                start.x = reset_x = get_reset_x(alignment, line_size, reset_x);
            }
       //   start.y += f.yAdvance;
            continue;
        } else if (c == '\r') {
      //    start.x = reset_x;
            continue;
        }

        // Handle wrap around
        if (start.x + glyph.xAdvance > CONFIG_SCREEN_WIDTH) {
            line_size = get_line_size(f, buf, len);
  //         start.x = reset_x = get_reset_x(alignment, line_size, reset_x);
  //         start.y += f.yAdvance;
        }
				//if the char goes lower then baseline (comma, y, etc) foresee extra lines.
				if ((h+yo) > extra_font_height) {
				     extra_font_height = (h+yo);
				}
    }
		//add total height to the buffer header:
		encoded_buffer[HEIGHT_LSB_B7] = (font_height+extra_font_height) & 0xFF;
    encoded_buffer[HEIGHT_MSB_B8] = ((font_height+extra_font_height) >> 8) & 0xFF;
		
    for (yy = -font_height; yy < extra_font_height; yy++) {
        // For each char in the string
        for (unsigned i = 0; i < len; i++) {
            char c = buf[i];
            GFXglyph glyph = f.glyph[c - f.first];
            uint8_t *bitmap = f.bitmap;

            uint16_t bo = glyph.bitmapOffset;
            uint8_t w = glyph.width, h = glyph.height;
            int8_t xo = glyph.xOffset, yo = glyph.yOffset;
            start.y = saved_start_y;

            uint8_t xx, bits = 0, bit = 0;
            line_h = h;

            // Ensure we write the necessary amount of black pixels for lines without data
			if (yy < yo || yy >= yo + h) {
                // For characters that are fully black pixels (no data to draw)
                for (xx = 0; xx < glyph.xAdvance; xx++) {
                    if (current_color == 0) {
                        run_length++;
                        if (run_length == max_run_length) {
                            encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (max_run_length - 1);
                            run_length = 0;
                        }
                    } else {
                        if (run_length > 0) {
                            encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (run_length - 1);
                        }
                        current_color = 0;
                        run_length = 1;
                    }
                }
							
            } else {
							
                // Draw bitmap and handle padding
                int16_t bit_offset = (yy - yo) * w;  // Total bit offset
                bo += bit_offset / 8; // For each line (that exists) go one byte further in the buffer
                bits = bitmap[bo++];
                bit = (bit_offset) % 8;
                bits <<= bit;

                for (xx = 0; xx < glyph.xAdvance; xx++) {
                    uint8_t pixel_color;

                    // Handle case where the pixel exceeds the width of the character and is padding
                    if (xx >= w) {
                        pixel_color = 0;  // This is black (background) padding pixel						
                    } else {
                        pixel_color = (bits & 0x80) >> 7;  // 1 for text (white), 0 for background (black)
                    }

                    // Check if the pixel color is the same as the previous one
                    if (pixel_color == current_color) {
                        run_length++;
                        if (run_length == max_run_length) {
                            // Encode the run
                            encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (max_run_length - 1);
                            run_length = 0;
                        }
                    } else {
                        // Encode the previous run if needed
                        if (run_length > 0) {
                            encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (run_length - 1);
                        }
                        current_color = pixel_color;
                        run_length = 1;
                    }

                    // Shift the bits left to process the next pixel
                    bits <<= 1;
                    //bit++;

                    // If bit%8 == 0
                    if (!(++bit & 7)) {
                        bits = bitmap[bo++];  // Load the next byte from the bitmap
                    }
                }
            }
            start.x += glyph.xAdvance;
        }
        start.x = reset_x;
    }

    // Encode the final run (if there are remaining pixels)
    if (run_length > 0) {
        encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (run_length - 1);
    }
//char message2[100];
//sprintf(message2, "size of buffer+header: %i\n\r", encoded_index); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);			
	  
		
    // Calculate text size
    point_t text_size = {0, 0};
    text_size.x = line_size;
    text_size.y = (saved_start_y - start.y) + line_h;

    return text_size;
}

//Version1 - has unneeded vars in function call: startx and length can actually be taken care of locally as well.
static inline uint16_t get_line_size2(GFXfont f, uint16_t startx, const char *text, uint16_t length, uint16_t x_max)
{
    uint16_t line_size = 0;
    for(unsigned i = 0; i < length && text[i] != '\n' && text[i] != '\r'; i++)
    {
        GFXglyph glyph = f.glyph[text[i] - f.first];
        if ((line_size + glyph.xAdvance) <= x_max)   //Correct to <= instead of <
            line_size += glyph.xAdvance;
        else
            break;
    }
    return line_size;
}


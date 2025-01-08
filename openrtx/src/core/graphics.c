/***************************************************************************
 *   Copyright (C) 2020 - 2023 by Federico Amedeo Izzo IU2NUO,             *
 *                                Niccolò Izzo IU2KIN,                     *
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

/**
 * This source file provides an  implementation for the graphics.h interface
 * It is suitable for both color, grayscale and B/W display
 */



#include "display.h"
#include "ST7735S.h"

#include "arm_acle.h"

//#include <hwconfig.h>
#include "graphics.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <stdio.h>
#include <math.h>

#include <gfxfont.h>
#include <TomThumb.h>
#include <FreeSans6pt7b.h>
#include <FreeSans8pt7b.h>
#include <FreeSans9pt7b.h>
#include <FreeSans10pt7b.h>
#include <FreeSans12pt7b.h>
#include <FreeSans16pt7b.h>
#include <FreeSans18pt7b.h>
#include <FreeSans24pt7b.h>
#include <UbuntuRegular6pt7b.h>
#include <UbuntuRegular8pt7b.h>
#include <UbuntuRegular9pt7b.h>
#include <UbuntuRegular10pt7b.h>
#include <UbuntuRegular12pt7b.h>
#include <UbuntuRegular16pt7b.h>
#include <UbuntuRegular18pt7b.h>
#include <UbuntuRegular24pt7b.h>

//Tris Fonts
#include <TomThumbPlus V2.h>
#include <Terminus Mono 7PX W5 H9.h>
#include <Terminus Mono 7PX W6 H9.h>
#include <Proggy 5x7.h>


#include <Symbols5pt7b.h>
#include <Symbols6pt7b.h>
#include <Symbols8pt7b.h>

//added for debugging functions
#include "string.h" //For Huart1 strlen
extern UART_HandleTypeDef huart1;
#include "stm32f401xe.h"

// Variable swap macro
#define DEG_RAD  0.017453292519943295769236907684886
#define SIN(x) sinf((x) * DEG_RAD)
#define COS(x) cosf((x) * DEG_RAD)

/**
 * Fonts, ordered by the fontSize_t enum.
 */
 
/* //ORIGINAL
static const GFXfont fonts[] = { TomThumb,            // 5pt
    #if defined FONT_FREE_SANS
                                 FreeSans6pt7b,       // 6pt
                                 FreeSans8pt7b,       // 8pt
                                 FreeSans9pt7b,       // 9pt
                                 FreeSans10pt7b,      // 10pt
                                 FreeSans12pt7b,      // 12pt
                                 FreeSans16pt7b,      // 16pt
    #elif defined FONT_UBUNTU_REGULAR
                                 UbuntuRegular6pt7b,  // 6pt
                                 UbuntuRegular8pt7b,  // 8pt
                                 UbuntuRegular9pt7b,  // 9pt
                                 UbuntuRegular10pt7b, // 10pt
                                 UbuntuRegular12pt7b, // 12pt
                                 UbuntuRegular16pt7b, // 16pt
    #else
    #error Unsupported font family!
    #endif
                                 Symbols5pt7b,      // 5pt
                                 Symbols6pt7b,      // 6pt
                                 Symbols8pt7b       // 8pt
                               };
*/

//TRIS Fonts
static const GFXfont fonts[] = { TomThumbPlus,            // 5pt
    #if defined FONT_FREE_SANS
                                 FreeSans6pt7b,       // 6pt
                                 FreeSans8pt7b,       // 8pt
                                 FreeSans9pt7b,       // 9pt
                                 FreeSans10pt7b,      // 10pt
                                 FreeSans12pt7b,      // 12pt
                                 FreeSans16pt7b,      // 16pt
    #elif defined FONT_UBUNTU_REGULAR
                                 Proggy7Px,  // 6pt
                                 TerminusMono7px,  // 8pt
                                 UbuntuRegular9pt7b,  // 9pt
                                 UbuntuRegular10pt7b, // 10pt
                                 UbuntuRegular12pt7b, // 12pt
                                 UbuntuRegular16pt7b, // 16pt
    #else
    #error Unsupported font family!
    #endif
                                 Symbols5pt7b,      // 5pt
                                 Symbols6pt7b,      // 6pt
                                 Symbols8pt7b       // 8pt
                               };

typedef struct {
    uint8_t font_size;  // Height above the baseline
    uint8_t font_height; // Total height including below baseline,for characters like "j" and comma.
} Font_detail;


                               
const Font_detail font_detail[] = {
    {5, 5}, // FONT_SIZE_5PT
    {7, 9},// FONT_SIZE_6PT -- the chars themselves are 8px, but start a line lower, so at least 9 is needed. 11 is for the / sign which is too high
    //Fonts below need sizes updated!
    {12,15},  // FONT_SIZE_8PT
    {13,15}, // FONT_SIZE_9PT
    {13,15}, // FONT_SIZE_10PT
    {12,15}, // FONT_SIZE_12PT
    {16,15} // FONT_SIZE_16PT
};


#ifdef CONFIG_PIX_FMT_RGB565

/* This specialization is meant for an RGB565 little endian pixel format.
 * Thus, to accomodate for the endianness, the fields in struct rgb565_t have to
 * be written in reversed order.
 *
 * For more details about endianness and bitfield structs see the following web
 * page: http://mjfrazer.org/mjfrazer/bitfields/
 */

#define PIXEL_T rgb565_t
#define FB_SIZE (CONFIG_SCREEN_HEIGHT * CONFIG_SCREEN_WIDTH)

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

#elif defined CONFIG_PIX_FMT_BW

/**
 * This specialization is meant for black and white pixel format.
 * It is suitable for monochromatic displays with 1 bit per pixel,
 * it will have RGB and grayscale counterparts
 */

#define PIXEL_T uint8_t
#define FB_SIZE (((CONFIG_SCREEN_HEIGHT * CONFIG_SCREEN_WIDTH) / 8 ) + 1)

typedef enum
{
    WHITE = 0,
    BLACK = 1,
}
bw_t;

static bw_t _color2bw(color_t true_color)
{
    if(true_color.r == 0 &&
       true_color.g == 0 &&
       true_color.b == 0)
        return WHITE;
    else
        return BLACK;
}

#else
#error Please define a pixel format type into hwconfig.h or meson.build
#endif

#ifndef CONFIG_GFX_NOFRAMEBUF
static PIXEL_T __attribute__((section(".bss.fb"))) framebuffer[FB_SIZE];
#endif
static char text[32];


void gfx_init()
{
    display_init();
    // Clear text buffer
    memset(text, 0x00, 32);
}

void gfx_terminate()
{
    //display_terminate();
}

void gfx_renderRows(uint8_t startRow, uint8_t endRow)
{
#ifdef CONFIG_GFX_NOFRAMEBUF
    (void) startRow;
    (void) endRow;
#else
    display_renderRows(startRow, endRow, framebuffer);
#endif
}

void gfx_render()
{
#ifndef CONFIG_GFX_NOFRAMEBUF
    display_render(framebuffer);
#endif
}

void gfx_clearRows(uint8_t startRow, uint8_t endRow)
{
    if(endRow < startRow)
        return;

#ifndef CONFIG_GFX_NOFRAMEBUF
    uint16_t start = startRow * CONFIG_SCREEN_WIDTH * sizeof(PIXEL_T);
    uint16_t height = endRow - startRow * CONFIG_SCREEN_WIDTH * sizeof(PIXEL_T);
    // Set the specified rows to 0x00 = make the screen black
    memset(framebuffer + start, 0x00, height);
#endif
}

void gfx_clearScreen()
{
    // Set the whole framebuffer to 0x00 = make the screen black
    #ifdef CONFIG_GFX_NOFRAMEBUF
    display_fill(0x00);
    #else
    memset(framebuffer, 0x00, FB_SIZE * sizeof(PIXEL_T));
    #endif
}

void gfx_fillScreen(color_t color)
{
    // Convert color to high color
    #ifdef CONFIG_PIX_FMT_RGB565
    rgb565_t high_color = _true2highColor(color);
    display_fill((high_color.r << 11) | (high_color.g << 5) | high_color.b);
    #elif defined CONFIG_PIX_FMT_BW
    display_fill(_color2bw(color));
    #endif
}

#ifdef PLATFORM_A36PLUS
// Set window: x, y, width, height
void gfx_setWindow(uint16_t x, uint16_t y, uint16_t height, uint16_t width)
{
    return;
    display_setWindow(x, y, height, width);
}

void gfx_clearWindow(uint16_t x, uint16_t y, uint16_t height, uint16_t width)
{
    //return;
    display_clearWindow(x, y, height, width);
    // Restore window
    display_setWindow(0,0,CONFIG_SCREEN_HEIGHT, CONFIG_SCREEN_WIDTH+28);
}
#else
// empty declarations
void gfx_setWindow(uint16_t x, uint16_t y, uint16_t height, uint16_t width) {}
void gfx_clearWindow(uint16_t x, uint16_t y, uint16_t height, uint16_t width) {}
#endif

inline void gfx_setPixel(point_t pos, color_t color)
{
    if (pos.x >= CONFIG_SCREEN_WIDTH || pos.y >= CONFIG_SCREEN_HEIGHT ||
        pos.x < 0 || pos.y < 0)
        return; // off the screen

#ifdef CONFIG_PIX_FMT_RGB565

    #ifdef CONFIG_GFX_NOFRAMEBUF
    rgb565_t pixel = _true2highColor(color);
    display_setPixel(pos.x, pos.y, (pixel.r << 11) | (pixel.g << 5) | (pixel.b));
    #else
    // Blend old pixel value and new one
    if (color.alpha < 255)
    {
        uint8_t alpha = color.alpha;
        rgb565_t new_pixel = _true2highColor(color);
        rgb565_t old_pixel = framebuffer[pos.x + pos.y*CONFIG_SCREEN_WIDTH];
        rgb565_t pixel;
        pixel.r = ((255-alpha)*old_pixel.r+alpha*new_pixel.r)/255;
        pixel.g = ((255-alpha)*old_pixel.g+alpha*new_pixel.g)/255;
        pixel.b = ((255-alpha)*old_pixel.b+alpha*new_pixel.b)/255;
        framebuffer[pos.x + pos.y*CONFIG_SCREEN_WIDTH] = pixel;
    }
    else
    {
        framebuffer[pos.x + pos.y*CONFIG_SCREEN_WIDTH] = _true2highColor(color);
    }
    #endif

#elif defined CONFIG_PIX_FMT_BW

    #ifdef CONFIG_GFX_NOFRAMEBUF
    display_setPixel(pos.x, pos.y, _color2bw(color));
    #else
    // Ignore more than half transparent pixels
    if (color.alpha >= 128)
    {
        uint16_t cell = (pos.x + pos.y*CONFIG_SCREEN_WIDTH) / 8;
        uint16_t elem = (pos.x + pos.y*CONFIG_SCREEN_WIDTH) % 8;
        framebuffer[cell] &= ~(1 << elem);
        framebuffer[cell] |= (_color2bw(color) << elem);
    }
    #endif

#endif
}

void gfx_drawLine(point_t start, point_t end, color_t color)
{
    int16_t steep = abs(end.y - start.y) > abs(end.x - start.x);

    if (steep)
    {
        uint16_t tmp;
        // Swap start.x and start.y
        tmp = start.x;
        start.x = start.y;
        start.y = tmp;
        // Swap end.x and end.y
        tmp = end.x;
        end.x = end.y;
        end.y = tmp;
    }

    if (start.x > end.x)
    {
        uint16_t tmp;
        // Swap start.x and end.x
        tmp = start.x;
        start.x = end.x;
        end.x = tmp;
        // Swap start.y and end.y
        tmp = start.y;
        start.y = end.y;
        end.y = tmp;
    }

    int16_t dx, dy;
    dx = end.x - start.x;
    dy = abs(end.y - start.y);

    int16_t err = dx >> 1;
    int16_t ystep;

    if (start.y < end.y)
        ystep = 1;
    else
        ystep = -1;

    for (; start.x<=end.x; start.x++)
    {
        point_t pos = {start.y, start.x};
        if (steep)
            gfx_setPixel(pos, color);
        else
            gfx_setPixel(start, color);

        err -= dy;
        if (err < 0)
        {
            start.y += ystep;
            err += dx;
        }
    }
}


//same as gfx_drawRect2, but we need to specify fill_color, which can be the same as color to behave like gfx_drawRect
void gfx_drawRect2(point_t start, int16_t width, int16_t height, color_t color, bool fill, color_t fill_color)
{   
    if (width == 0) return;
    if (height == 0) return;

    // Handle negative width
    volatile int16_t x_max = start.x;  // Set x_max to the start.x initially
    if (width < 0) {
        start.x += width+1;   // Move the starting point left by width
    } else {
        x_max += width - 1;  // Move the ending point right by width
    }

    // Handle negative height
    volatile int16_t y_max = start.y;  // Set y_max to the start.y initially
    if (height < 0) {
        start.y += height+1;    // Move the starting point up by height
    } else {
        y_max += height - 1;  // Move the ending point down by height
    }

    // Clamp start and max values to screen boundaries
    if (start.x < 0) start.x = 0;
    if (start.y < 0) start.y = 0;
    if (x_max > CONFIG_SCREEN_WIDTH-1) x_max = CONFIG_SCREEN_WIDTH - 1;
    if (y_max >= CONFIG_SCREEN_HEIGHT-1) y_max = CONFIG_SCREEN_HEIGHT - 1;

    //char message2[50];
    //sprintf(message2, "start.x: %u\n\r", state.devStatus); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
    
    // Loop over the rectangle area directly from start to max
    for (int16_t y = start.y; y <= y_max; y++) {
        for (int16_t x = start.x; x <= x_max; x++) {
            bool on_perimeter = (y == start.y || y == y_max || x == start.x || x == x_max);

            if (on_perimeter) {
                point_t pos = {x, y};
                gfx_setPixel(pos, color);  // Draw the pixel
            }
            else if(fill)
            {
                point_t pos = {x, y};
                gfx_setPixel(pos, fill_color); // Draw the pixel
            }  
        }
    }
}
/* Previous Version
void gfx_drawRect2(point_t start, uint16_t width, uint16_t height, color_t color, bool fill, color_t fill_color)
{   
    if (width == 0) return;
    if (height == 0) return;

    volatile uint16_t x_max = start.x + width - 1; // Optimization -O3 does not work without volatile
    volatile uint16_t y_max = start.y + height - 1;

    if (x_max > (CONFIG_SCREEN_WIDTH - 1)) x_max = CONFIG_SCREEN_WIDTH - 1;
    if (y_max > (CONFIG_SCREEN_HEIGHT - 1)) y_max = CONFIG_SCREEN_HEIGHT - 1;

    for (int16_t y = start.y; y <= y_max; y++) {
        for (int16_t x = start.x; x <= x_max; x++) {
            // Check for perimeter: Top, Bottom, Left, Right
            bool on_perimeter = (y == start.y || y == y_max || x == start.x || x == x_max);

            // Draw the perimeter with the given color
            if (on_perimeter) {
                point_t pos = {x, y};
                gfx_setPixel(pos, color);
            } 
            // If fill is true, draw the inner area with the fill color
            else if (fill) {
                point_t pos = {x, y};
                gfx_setPixel(pos, fill_color);
            }
        }
    }
}
*/

void gfx_drawRect(point_t start, uint16_t width, uint16_t height, color_t color, bool fill)
{   
    if(width == 0) return;
    if(height == 0) return;
  
    volatile  uint16_t x_max = start.x + width - 1; // Optimization -O3 does not work without volatile
    volatile  uint16_t y_max = start.y + height - 1;

    if(x_max > (CONFIG_SCREEN_WIDTH - 1)) x_max = CONFIG_SCREEN_WIDTH - 1;
    if(y_max > (CONFIG_SCREEN_HEIGHT - 1)) y_max = CONFIG_SCREEN_HEIGHT - 1;

    for(int16_t y = start.y; y <= y_max; y++)
    {
        for(int16_t x = start.x; x <= x_max; x++)
        {
            // Direct check for the perimeter: Top, Bottom, Left, Right
            bool on_perimeter = (y == start.y || y == y_max || x == start.x || x == x_max);

            // Draw the pixel if fill is true, or if we're on the perimeter
            if(fill || on_perimeter)
            {
                point_t pos = {x, y};
                gfx_setPixel(pos, color); // Draw the pixel
            }          
        }
    }
}


void gfx_drawCircle(point_t start, uint16_t r, color_t color)
{
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    point_t pos = start;
    pos.y += r;
    gfx_setPixel(pos, color);
    pos.y -= 2 * r;
    gfx_setPixel(pos, color);
    pos.y += r;
    pos.x += r;
    gfx_setPixel(pos, color);
    pos.x -= 2 * r;
    gfx_setPixel(pos, color);

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }

        x++;
        ddF_x += 2;
        f += ddF_x;

        pos.x = start.x + x;
        pos.y = start.y + y;
        gfx_setPixel(pos, color);
        pos.x = start.x - x;
        pos.y = start.y + y;
        gfx_setPixel(pos, color);
        pos.x = start.x + x;
        pos.y = start.y - y;
        gfx_setPixel(pos, color);
        pos.x = start.x - x;
        pos.y = start.y - y;
        gfx_setPixel(pos, color);
        pos.x = start.x + y;
        pos.y = start.y + x;
        gfx_setPixel(pos, color);
        pos.x = start.x - y;
        pos.y = start.y + x;
        gfx_setPixel(pos, color);
        pos.x = start.x + y;
        pos.y = start.y - x;
        gfx_setPixel(pos, color);
        pos.x = start.x - y;
        pos.y = start.y - x;
        gfx_setPixel(pos, color);
    }
}

void gfx_drawHLine(int16_t y, uint16_t height, color_t color)
{
    point_t start = {0, y};
    gfx_drawRect(start, CONFIG_SCREEN_WIDTH, height, color, 1);
}

void gfx_drawVLine(int16_t x, uint16_t width, color_t color)
{
    point_t start = {x, 0};
    gfx_drawRect(start, width, CONFIG_SCREEN_HEIGHT, color, 1);
}

/**
 * Compute the pixel size of the first text line
 * @param f: font used as the source of glyphs
 * @param text: the input text
 * @param length: the length of the input text, used for boundary checking
 *        line_size goes from "1 to CONFIG_SCREEN_WIDTH", not "0 to CONFIG_SCREEN_WIDTH - 1"
  */
static inline uint16_t get_line_size(GFXfont f, uint16_t startx, const char *text, uint16_t length)
{
    uint16_t line_size = 0;
    uint16_t x_max = CONFIG_SCREEN_WIDTH - startx;   //start.x needs to be taken into account for screen boundary
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


static inline uint16_t get_line_size2(GFXfont f, uint16_t startx, const char *text, uint16_t length, uint16_t x_max)
{
    uint16_t line_size = 0;
    //uint16_t x_max = width - startx;   //start.x needs to be taken into account for screen boundary
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

/**
 * Compute the start x coordinate of a new line of given pixel size
 * @param alinment: enum representing the text alignment
 * @param line_size: the size of the current text line in pixels
 */
static inline uint16_t get_reset_x(textAlign_t alignment, uint16_t line_size,
                                                          uint16_t startx)
{
    switch(alignment)
    {
        case TEXT_ALIGN_LEFT:
            return startx;
        case TEXT_ALIGN_CENTER:
            return (CONFIG_SCREEN_WIDTH - line_size - startx)/2; //Center alignment, take start.x into account if start.x <> 0
        case TEXT_ALIGN_RIGHT:
            return CONFIG_SCREEN_WIDTH - line_size - startx;
    }

    return 0;
}

uint8_t gfx_getFontHeight(fontSize_t size)
{
    GFXfont f = fonts[size];
    GFXglyph glyph = f.glyph['|' - f.first];
    return glyph.height;
}



         //TRIS COMPARE FUNCTION
typedef struct {
    uint8_t *encoded_buffer;              // Pointer to the current byte in the encoded buffer
    uint16_t *encoded_index;             // Pointer to the encoded index
    uint16_t *color_map_encoded;
    uint16_t *encoded_pixel_index;         // Pointer to the encoded pixel index
    uint16_t *current_pixel_index;            // Pointer to the temporary pixel index
    uint16_t *current_color;             // Pointer to the current color
    int16_t *line_start_change_x;     // Pointer to track the start of changes on the line
    int16_t *line_end_change_x;       // Pointer to track the end of changes on the line
    uint16_t *run_length;                // Pointer to the run length
} BufferProcessingState;

 uint16_t count = 0;

void gfx_crle_run_differences(BufferProcessingState *state, uint8_t num_color_bits) {
    uint16_t encoded_index = *state->encoded_index;
    uint16_t current_pixel_idx = *state->current_pixel_index;
    const uint8_t color_shift_left = 8 - num_color_bits;
    //const uint8_t max_run_length = (1 << color_shift_left);
    
    uint8_t colorbits = state->encoded_buffer[encoded_index] >> color_shift_left;
    uint16_t run_length_encoded = (state->encoded_buffer[encoded_index] & ((1 << color_shift_left) - 1)) + 1;
  __dsb(0xF);
    
    const uint16_t run_length = (*state->run_length > 64) ? (64) : (*state->run_length);
    const uint16_t current_pixel_max = current_pixel_idx + run_length;

    uint8_t pixel_offset = *state->encoded_pixel_index - current_pixel_idx;
    uint8_t lowest_run_length;
    
    if (pixel_offset) {
        lowest_run_length = ((pixel_offset < run_length) ? pixel_offset : run_length);
    } else {
        lowest_run_length = ((run_length_encoded < run_length) ? run_length_encoded : run_length);
    }
    
    // Check for color change and track first/last change
    if (*state->current_color != state->color_map_encoded[colorbits]) {
        if (*state->line_start_change_x == -1) {
            *state->line_start_change_x = current_pixel_idx;
        }
        *state->line_end_change_x = current_pixel_idx + lowest_run_length;
    }

    current_pixel_idx += lowest_run_length;
    if (pixel_offset == 0) {
        *state->encoded_pixel_index += run_length_encoded;
    }

    uint8_t current_remaining = current_pixel_max - current_pixel_idx;
    while (current_remaining) {
        encoded_index++;
        colorbits = state->encoded_buffer[encoded_index] >> color_shift_left;
        __dsb(0xF);
        run_length_encoded = (state->encoded_buffer[encoded_index] & ((1 << color_shift_left) - 1)) + 1;
        lowest_run_length = ((run_length_encoded < current_remaining) ? run_length_encoded : current_remaining);
        
        // Check for color change and track first/last change
        if (*state->current_color != state->color_map_encoded[colorbits]) {
            if (*state->line_start_change_x == -1) {
                *state->line_start_change_x = current_pixel_idx;
            }
            *state->line_end_change_x = current_pixel_idx + lowest_run_length;
        }

        current_pixel_idx += lowest_run_length;
        *state->encoded_pixel_index += run_length_encoded;
        current_remaining = current_pixel_max - current_pixel_idx;
    }
    if (*state->encoded_pixel_index <= current_pixel_idx) {
       encoded_index++;
    }
    *state->encoded_index = encoded_index;
    *state->current_pixel_index = current_pixel_idx;
}


rect_area_t gfx_compare_CrleBuffer(point_t start, fontSize_t size, textAlign_t alignment, const char *buf, uint8_t *encoded_buffer, uint16_t *colors, uint8_t num_colors) {
  alignment = 0;
  
    uint8_t num_color_bits = 2; //Need to be included in function call
    const GFXfont f = fonts[size];
    const uint8_t font_height = font_detail[size].font_size;
    const uint8_t total_font_height = font_detail[size].font_height;
    const size_t len = strlen(buf);
    const uint8_t color_shift_left = 8 - num_color_bits;
    const uint8_t max_run_length = (1 << color_shift_left);
    const uint8_t max_colors = 1 << num_color_bits;
    uint16_t background_color = colors[0];
    uint16_t foreground_color = colors[1];
    uint16_t encoded_index = COLOR_START_B9 + (max_colors * 2);  //define position of first data byte = end of header
    // Encoding variables
    uint16_t current_color = 0;
    uint16_t run_length = 0;
    uint8_t last_char_index = 0;          //keep last character index so we can process additional lines if needed
    //For comparing
    uint16_t encoded_pixel_index;
    uint16_t current_pixel_index;
    //int16_t buff_start_change_x = -1;    // The first X position (lowest value) across all lines where a pixel has changed. -1 indicates no changes.
    //int16_t buff_end_change_x = 0;       // The last X position (highest value) across all lines where a pixel has changed. 0 indicates no changes (if no changes have been tracked).
    //int16_t buff_start_change_y = -1;    // The first X position (lowest value) across all lines where a pixel has changed. -1 indicates no changes.
    //int16_t buff_end_change_y = 0;       // The last X position (highest value) across all lines where a pixel has changed. 0 indicates no changes (if no changes have been tracked).
    int16_t track_line = 0;
    
    rect_points_t buff = {
    .start = {-1, -1},  // Initialize start point with (-1, -1)
    .end = {0, 0}       // Initialize end point with (0, 0)
    };
    
    int16_t line_start_change_x;           //first-last change per line
    int16_t line_end_change_x;
    
          char message2[100];
 //         sprintf(message2, "background_color: %u, foreground_color: %u \n\r", background_color, foreground_color); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
    
    // Colors from already encoded buffer
    uint16_t color_map_encoded[1 << encoded_buffer[COLOR_BITS_B0]];  // size = 1 << num_color_bits
    // Initialize color map from the header (no need for extra variables)
    for (int i = 0; i < (1 << encoded_buffer[COLOR_BITS_B0]); i++) {  // max_colors = 1 << num_color_bits
        //color_map_encoded[i] = encoded_buffer[COLOR_START_B9 + 2 * i] | (encoded_buffer[COLOR_START_B9 + 2 * i + 1] << 8);
        color_map_encoded[i] = *(uint16_t*)&encoded_buffer[COLOR_START_B9 + 2 * i];
    }
    
     BufferProcessingState buffer_state = {
    .encoded_buffer = encoded_buffer,            // Direct reference to the entire buffer (no need to dereference)
    .encoded_index = &encoded_index,           // Pointer to the encoded index
    .color_map_encoded = color_map_encoded,           // Pointer to the encoded color map
    .encoded_pixel_index = &encoded_pixel_index, // Pointer to the encoded pixel index
    .current_pixel_index = &current_pixel_index,       // Pointer to the temporary pixel index
    .current_color = &current_color,           // Pointer to the current color
    .line_start_change_x = &line_start_change_x,  // Pointer to the start of changes
    .line_end_change_x = &line_end_change_x,    // Pointer to the end of changes
    .run_length = &run_length                  // Pointer to the run length
    };


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
        uint8_t h = glyph.height;
        int8_t yo = glyph.yOffset;

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
            //Reset variables to find the changes
            line_start_change_x = -1;      //first-last change per line
            line_end_change_x = 0;
            encoded_pixel_index = 0;
            current_pixel_index = 0;

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
                    //encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (max_run_length - 1);
                    gfx_crle_run_differences(&buffer_state, num_color_bits);
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
                        //encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (run_length - 1);
                        gfx_crle_run_differences(&buffer_state, num_color_bits);
                        run_length = 0;
                        current_color = background_color;
                    }
                    run_length += glyph.xAdvance;
                    while (run_length >= max_run_length)
                    {
                        //encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (max_run_length - 1);
                        gfx_crle_run_differences(&buffer_state, num_color_bits);
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
                                //encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (max_run_length - 1);
                                gfx_crle_run_differences(&buffer_state, num_color_bits);
                                run_length = 0;
                            }
                        } else {
                            // Color Change - Encode the previous run if needed
                            if (run_length > 0) {
                                //encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (run_length - 1);
                                gfx_crle_run_differences(&buffer_state, num_color_bits);
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
                    //encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (run_length - 1);
                    gfx_crle_run_differences(&buffer_state, num_color_bits);
                    run_length = 0;
                    current_color = background_color;
                }
                run_length += line_size - reset_x - padding_before;   //Add all padding at once
                while (run_length >= max_run_length) {
                    //encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (max_run_length - 1);
                    gfx_crle_run_differences(&buffer_state, num_color_bits);
                    run_length -= max_run_length;
                }
            }
            //Start a new encoded byte when a new line starts. That will make compare easier. This will slightly increase size.
            if (run_length > 0) {
                //encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (run_length - 1);
                gfx_crle_run_differences(&buffer_state, num_color_bits);
                run_length = 0;
            }
char message2[100];
sprintf(message2, "  ---l: %i,yy: %i,s_x: %i,e_x: %i \n\r", line, yy, line_start_change_x, line_end_change_x); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
            // Update the start and end values based on changes
            if (line_start_change_x != -1) { 
                // Track the minimum (first) change X position across the line
                if (buff.start.x == -1 || line_start_change_x < buff.start.x) {
                    buff.start.x = line_start_change_x;
                }
            }
            // Always track the last changed X position across the line
            buff.end.x = (line_end_change_x > buff.end.x) ? line_end_change_x : buff.end.x;
            if (line_start_change_x != -1) {
                buff.end.y = track_line + 1;
                buff.start.y = (buff.start.y == -1) ? track_line : buff.start.y;
            }
            track_line++;
        }

        if ((line < additional_write_lines) || (combined_buffer_height < y_max)) {
            //Encode remaining pixels, if not background
            //if (current_color != background_color && run_length > 0) { //Encode first if we still have run_length of other color.
            //for comparison it's better to end the previous lines and start a new byte if needed:
            //if (run_length > 0) {
                //encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (run_length - 1);
            //    gfx_crle_run_differences(&buffer_state, num_color_bits);
            //    run_length = 0;
            //}
            //current_color = background_color;
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
                //Reset variables to find the changes
                line_start_change_x = -1;
                line_end_change_x = 0;
                encoded_pixel_index = 0;
                current_pixel_index = 0;
                current_color = background_color;
                run_length = line_size;
//sprintf(message2, "     ----------extra:run_length: %u, line_start_change_x: %i, line_end_change_x: %i \n\r\n\r", run_length, line_start_change_x, line_end_change_x); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
                while (run_length >= max_run_length) {
                    //encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (max_run_length - 1);
                    gfx_crle_run_differences(&buffer_state, num_color_bits);
                    run_length -= max_run_length;
                }
                if (run_length > 0) {
                    //encoded_buffer[encoded_index++] = (current_color << color_shift_left) | (run_length - 1);
                    gfx_crle_run_differences(&buffer_state, num_color_bits);
                    run_length = 0;
                }
     //sprintf(message2, "     ----------extra: run_length: %u, line_start_change_x: %i, line_end_change_x: %i \n\r\n\r", run_length, line_start_change_x, line_end_change_x); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
                // Update the start and end values based on changes
                if (line_start_change_x != -1) { 
                    // Track the minimum (first) change X position across the line
                    if (buff.start.x == -1 || line_start_change_x < buff.start.x) {
                        buff.start.x = line_start_change_x;
                    }
                }
                // Always track the last changed X position across the line
                buff.end.x = (line_end_change_x > buff.end.x) ? line_end_change_x : buff.end.x;
                if (line_start_change_x != -1) {
                    buff.end.y = track_line + 1;
                    buff.start.y = (buff.start.y == -1) ? track_line : buff.start.y;
                }
                track_line++;
            }
        }
    }
    *(uint16_t*)&encoded_buffer[HEIGHT_LSB_B7] = combined_buffer_height;
    
//    char message2[100];
//sprintf(message2, "buff.start.x: %i, buff.end.x: %i, buff.start.y: %i, buff.end.y: %i\n\r", buff.start.x, buff.end.x, buff.start.y, buff.end.y); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);

    // Calculate text size
    return (rect_area_t){buff.start.x, buff.start.y, buff.end.x-buff.start.x, buff.end.y-buff.start.y};
}
//END TRIS COMPARE FUNCTION



//Tris.. trying inline function for encoding buffer

//END Tris.. trying inline function for encoding buffer


//Tris Test Printing to encoded buffer right away

//updated with dynamic for- and background colors
//Also test updated logic due to reseting to new byte for new line

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
        uint8_t h = glyph.height;
        int8_t yo = glyph.yOffset;

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
//END TRIS version with wiping screen behind the buffer characters
//END TRIS version with wiping screen behind the buffer characters



//Tris Test printing to buffer, instead of screen - Large uncompressed buffer, 2byte per pixel (RGB565)
point_t gfx_printToBuffer(point_t start, fontSize_t size, textAlign_t alignment,
                        color_t color, const char *buf, uint16_t *smeter_buffer, uint16_t buffer_width)
{
    GFXfont f = fonts[size];
    size_t len = strlen(buf);
  
    // Compute size of the first row in pixels
    uint16_t line_size = get_line_size(f, start.x, buf, len);
    uint16_t reset_x = get_reset_x(alignment, line_size, start.x);
    start.x = reset_x;
    uint8_t font_height = font_detail[size].font_size;

    // Save initial start.y value to calculate vertical size
    uint16_t saved_start_y = start.y;
    uint16_t line_h = 0;

    /* For each char in the string */
    for (unsigned i = 0; i < len; i++)
    {
        char c = buf[i];
        GFXglyph glyph = f.glyph[c - f.first];
        uint8_t *bitmap = f.bitmap;

        uint16_t bo = glyph.bitmapOffset;
        uint8_t w = glyph.width, h = glyph.height;
        int8_t xo = glyph.xOffset,
              yo = glyph.yOffset;
        uint8_t xx, yy, bits = 0, bit = 0;
        line_h = h;

        // Handle newline and carriage return
        if (c == '\n')
        {
            if (alignment != TEXT_ALIGN_CENTER)
            {
                start.x = reset_x;
            }
            else
            {
                line_size = get_line_size(f, start.x, &buf[i + 1], len - (i + 1));
                start.x = reset_x = get_reset_x(alignment, line_size, start.x);
            }
            start.y += f.yAdvance;
            continue;
        }
        else if (c == '\r')
        {
            start.x = reset_x;
            continue;
        }

        // Handle wrap around
        if (start.x + glyph.xAdvance > buffer_width)
        {
            // Compute size of the first row in pixels
          
            //BUG?? line_size does not take start.x into account..
            line_size = get_line_size(f, start.x, buf, len);
            start.x = reset_x = get_reset_x(alignment, line_size, start.x);
            start.y += f.yAdvance;
        }

        // Convert color to RGB565 (only once)
        uint16_t r = (color.r >> 3) & 0x1F;  // Red: 5 bits (0-31)
        uint16_t g = (color.g >> 2) & 0x3F;  // Green: 6 bits (0-63)
        uint16_t b = (color.b >> 3) & 0x1F;  // Blue: 5 bits (0-31)
        uint16_t rgb565 = (r << 11) | (g << 5) | b;  // Combine into RGB565 format

        // Draw bitmap to smeter_buffer
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
          
                        uint16_t buffer_idx = pos.y * buffer_width + pos.x;
                        // Store the RGB565 color in the buffer
                        //char message2[100];
                        //sprintf(message2, "pos.x:%i, pos.y:%i, idx:%i \n\r", pos.x,pos.y, buffer_idx); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
                        smeter_buffer[buffer_idx] = rgb565;
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
//END Tris Test printing to buffer, instead of screen - Large no compression buffer


//Tris New function encode_buffer - encode from intermediary uncompressed buffer for s_meter test
void encode_buffer(uint16_t *smeter_buffer, uint32_t buffer_size, uint8_t *encoded_buffer) {
    int encoded_index = 0;

    uint16_t current_color = smeter_buffer[0];  // Starting color to track
    uint8_t run_length = 1;  // Start with the first pixel already counted
  
    uint8_t color_to_bits[2] = {0b00, 0b01};  // Only black (0x0000) and white (0xFFFF) are used

    for (int j = 1; j < buffer_size; j++) {  // iterate over each pixel
        if (smeter_buffer[j] == current_color) {
            run_length++;
            // If the run length exceeds the maximum allowable count, encode and reset
            if (run_length == 64) {
                // Store the current run
                encoded_buffer[encoded_index++] = (color_to_bits[current_color == 0x0000 ? 0 : 1] << 6) | (64-1); //63 is FFFFFF
                run_length = 1;
            }
        } else {
            // Encode the current run
            encoded_buffer[encoded_index++] = (color_to_bits[current_color == 0x0000 ? 0 : 1] << 6) | (run_length-1);
            // Start a new run for the new color
            current_color = smeter_buffer[j];
            run_length = 1;
        }
    }
    // Encode the final run (if there are remaining pixels)
    if (run_length > 0) {
        encoded_buffer[encoded_index++] = (color_to_bits[current_color == 0x0000 ? 0 : 1] << 6) | (run_length-1);
    }
            char message2[50];
    //sprintf(message2, "encoded_index(nr of bytes): %i\n\r", encoded_index); HAL_UART_Transmit(&huart1, (uint8_t *)message2, strlen(message2), HAL_MAX_DELAY);
}
//END - Tris New function encode_buffer - encode from intermediary uncompressed buffer for s_meter test





point_t gfx_printBuffer(point_t start, fontSize_t size, textAlign_t alignment,
                        color_t color, const char *buf)
{
    GFXfont f = fonts[size];

    size_t len = strlen(buf);

    // Compute size of the first row in pixels
    uint16_t line_size = get_line_size(f, start.x, buf, len);
    uint16_t reset_x = get_reset_x(alignment, line_size, start.x);
    //TRIS FIX: Keeping saved_start_x for positioning new line
    start.x = reset_x;
    // Save initial start.y value to calculate vertical size
    uint16_t saved_start_y = start.y;
    uint16_t line_h = 0;

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
        line_h = h;

        // Handle newline and carriage return
        if (c == '\n')
        {
          if(alignment!=TEXT_ALIGN_CENTER)
          {
            start.x = reset_x;
          }
          else
          {
            line_size = get_line_size(f, start.x, &buf[i+1], len-(i+1));
            //TRIS - FIX: replaced start.x by reset_x, that contains the intial value
            start.x = reset_x = get_reset_x(alignment, line_size, reset_x);
          }
          start.y += f.yAdvance;
          continue;
        }
        else if (c == '\r')
        {
          start.x = reset_x;
          continue;
        }

        // Handle wrap around
        //if (start.x + glyph.xAdvance > CONFIG_SCREEN_WIDTH)  //bug?
        if (start.x + glyph.xAdvance > CONFIG_SCREEN_WIDTH)
        {
            // Compute size of the first row in pixels
            //BUG, the next line_length is the remainer of the pixels..some alignments will break
            //line_size = get_line_size(f, start.x, buf, len);
            line_size = get_line_size(f, start.x, &buf[i], len-(i));
            //TRIS - FIX: replaced start.x by reset_x, that contains the intial value
            start.x = reset_x = get_reset_x(alignment, line_size, reset_x); 
            start.y += f.yAdvance;
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
    
    //only correct for maximum 1 line
    text_size.x = line_size;
    text_size.y = (saved_start_y - start.y) + line_h;
    return text_size;
}

point_t gfx_print(point_t start, fontSize_t size, textAlign_t alignment,
                  color_t color, const char *fmt, ... )
{
    // Get format string and arguments from var char
    va_list ap;
    va_start(ap, fmt);
    vsniprintf(text, sizeof(text)-1, fmt, ap);
    va_end(ap);

    return gfx_printBuffer(start, size, alignment, color, text);
}

point_t gfx_printLine(uint8_t cur, uint8_t tot, int16_t startY, int16_t endY,
                      int16_t startX, fontSize_t size, textAlign_t alignment,
                      color_t color, const char* fmt, ... )
{
    // Get format string and arguments from var char
    va_list ap;
    va_start(ap, fmt);
    vsniprintf(text, sizeof(text)-1, fmt, ap);
    va_end(ap);

    // Estimate font height by reading the gliph | height
    uint8_t fontH = gfx_getFontHeight(size);

    // If endY is 0 set it to default value = CONFIG_SCREEN_HEIGHT
    if(endY == 0) endY = CONFIG_SCREEN_HEIGHT;

    // Calculate print coordinates
    int16_t height = endY - startY;
    // to print 2 lines we need 3 padding spaces
    int16_t gap = (height - (fontH * tot)) / (tot + 1);
    // We need a gap and a line height for each line
    int16_t printY = startY + (cur * (gap + fontH));

    point_t start = {startX, printY};
    return gfx_printBuffer(start, size, alignment, color, text);
}

// Print an error message to the center of the screen, surronded by a red (when possible) box
void gfx_printError(const char *text, fontSize_t size)
{
    // 3 px box padding
    uint16_t box_padding = 16;
    color_t white = {255, 255, 255, 255};
    color_t red =   {255,   0,   0, 255};
    point_t start = {0, CONFIG_SCREEN_HEIGHT/2 + 5};

    // Print the error message
    point_t text_size = gfx_print(start, size, TEXT_ALIGN_CENTER, white, text);
    text_size.x += box_padding;
    text_size.y += box_padding;
    point_t box_start = {0, 0};
    box_start.x = (CONFIG_SCREEN_WIDTH / 2) - (text_size.x / 2);
    box_start.y = (CONFIG_SCREEN_HEIGHT / 2) - (text_size.y / 2);
    // Draw the error box
    gfx_drawRect(box_start, text_size.x, text_size.y, red, false);
}

point_t gfx_drawSymbol(point_t start, symbolSize_t size, textAlign_t alignment,
                       color_t color, symbol_t symbol)
{
    /*
     * Symbol tables come immediately after fonts in the general font table.
     * But, to prevent errors where symbol size is used instead of font size and
     * vice-versa, their enums are separate. The trickery below is used to put
     * together again the two enums in a single consecutive index.
     *
     * TODO: improve this.
     */
    int symSize = size + FONT_SIZE_NUM;
    char buffer[2] = {0};

    buffer[0] = (char) symbol;
    return gfx_printBuffer(start, symSize, alignment, color, buffer);
}

/*
 * Function to draw battery of arbitrary size
 * starting coordinates are relative to the top left point.
 *
 *  ****************       |
 * *                *      |
 * *  *******       *      |
 * *  *******       **     |
 * *  *******       **     | <-- Height (px)
 * *  *******       *      |
 * *                *      |
 *  ****************       |
 *
 * __________________
 *
 * ^
 * |
 *
 * Width (px)
 *
 */
void gfx_drawBattery(point_t start, uint16_t width, uint16_t height,
                                                    uint8_t percentage)
{
    color_t white =  {255, 255, 255, 255};
    color_t black =  {0,   0,   0  , 255};

    // Cap percentage to 1
    percentage = (percentage > 100) ? 100 : percentage;

#ifdef CONFIG_PIX_FMT_RGB565
    color_t green =  {0,   255, 0  , 255};
    color_t yellow = {250, 180, 19 , 255};
    color_t red =    {255, 0,   0  , 255};

    // Select color according to percentage
    color_t bat_color = yellow;
    if (percentage < 30)
        bat_color = red;
    else if (percentage > 60)
        bat_color = green;
#elif defined CONFIG_PIX_FMT_BW
    color_t bat_color = white;
#endif

    // Draw the battery outline
    gfx_drawRect(start, width, height, white, false);

    // Draw the battery fill
    point_t fill_start;
    fill_start.x = start.x + 2;
    fill_start.y = start.y + 2;
    int fillWidth = ((width - 4) * percentage) / 100;
    gfx_drawRect(fill_start, fillWidth, height - 4, bat_color, true);

    // Round corners
    point_t top_left     = start;
    point_t top_right    = start;
    point_t bottom_left  = start;
    point_t bottom_right = start;

    top_right.x    += width - 1;
    bottom_left.y  += height - 1;
    bottom_right.x += width - 1;
    bottom_right.y += height - 1;

    gfx_setPixel(top_left, black);
    gfx_setPixel(top_right, black);
    gfx_setPixel(bottom_left, black);
    gfx_setPixel(bottom_right, black);

    // Draw the button
    point_t button_start;
    point_t button_end;

    button_start.x = start.x + width;
    button_start.y = start.y + height / 2 - (height / 8) - 1 + (height % 2);
    button_end.x   = start.x + width;
    button_end.y   = start.y + height / 2 + (height / 8);
    gfx_drawLine(button_start, button_end, white);
}

/*
 * Function to draw RSSI-meter of arbitrary size
 * starting coordinates are relative to the top left point.
 *
 * *     *     *     *     *     *     *     *      *      *      *|
 * ***********                 <-- Volume (optional)               |
 * ***************             <-- Squelch                         |
 * ***************                                                 |
 * ******************************************                      |
 * ******************************************    <- RSSI           |
 * ******************************************                      |  <-- Height (px)
 * ******************************************                      |
 * 1     2     3     4     5     6     7     8     9     +10    +20|
 * _________________________________________________________________
 *
 * ^
 * |
 *
 * Width (px)
 *
 */
void gfx_drawSmeter(point_t start, uint16_t width, uint16_t height, rssi_t rssi,
                    uint8_t squelch, uint8_t volume, bool drawVolume, color_t color)
{
    color_t white =  {255, 255, 255, 255};
    color_t yellow = {250, 180, 19 , 255};
    color_t red =    {255, 0,   0  , 255};

    fontSize_t font = FONT_SIZE_5PT;
    uint8_t font_height =  gfx_getFontHeight(font);
    uint16_t bar_height = (height - 3 - font_height);

    uint16_t bar_height_divider = drawVolume ? 7 : 6;

    // S-level marks and numbers
    for(int i = 0; i < 12; i++)
    {
        color_t color = (i % 3 == 0) ? yellow : white;
        color = (i > 9) ? red : color;
        point_t pixel_pos = start;
        pixel_pos.x += i * (width - 1) / 11;
        gfx_setPixel(pixel_pos, color);
        pixel_pos.y += height;
        if (i == 10) {
            pixel_pos.x -= 8;
            gfx_print(pixel_pos, font, TEXT_ALIGN_LEFT, color, "+%d", i);
        }
        else if(i == 11){
            pixel_pos.x -= 10;
            gfx_print(pixel_pos, font, TEXT_ALIGN_LEFT, red, "+20");
        }
        else
            gfx_print(pixel_pos, font, TEXT_ALIGN_LEFT, color, "%d", i);
        if (i == 10) {
            pixel_pos.x += 8;
        }
    }

    uint16_t volume_height = drawVolume ? bar_height / bar_height_divider : 0;
    if (drawVolume)
    {
        // Speaker Volume Bar
        uint16_t volume_width = (width * volume) / 255;
        point_t volume_pos = {start.x, (uint8_t) (start.y + 2)};
        gfx_drawRect(volume_pos, volume_width, volume_height, white, true);
    }

    // Squelch bar
    uint16_t squelch_height = bar_height * 2 / bar_height_divider ;
    uint16_t squelch_width = (width * squelch) / 15;
    point_t squelch_pos = {start.x, (uint8_t) (start.y + 2 + volume_height)};
    gfx_drawRect(squelch_pos, squelch_width, squelch_height, color, true);

    // RSSI bar
    int16_t s_level;
    if (rssi >= -53) { s_level = 12; }           // rssi >= -53dB set s_level to "11" (S9 + 20 dB)
    else if (rssi >= -73) {
        s_level =  (rssi_t)(163 + rssi) / 10;   // Increase s_level /10dB instead of /6dB for > S9
    }
    else if (rssi < -121) { s_level = 0; }     // s_level should not be negative + avoid overflow if rssi (int32_t) =< -196741
    else {
        s_level =  (rssi_t)(127 + rssi) / 6;    // 6dB increase per S-Point
    }
    uint16_t rssi_height = bar_height * 4 / bar_height_divider;
    uint16_t rssi_width = (s_level * (width - 1) / 11);
    point_t rssi_pos = { start.x, (uint8_t) (start.y + 2 + squelch_height + volume_height)};
    extern color_t spectrum_getColorFromLevel(uint16_t Level);
    for (int i = 0; i < s_level*2; i++) {
        uint16_t fragment_width = (width - 1) / 22; //will never work due to integer division, it will be rounded down and 1x x 22 times -> never shows full s_level 
        point_t fragment_pos = {start.x + i * fragment_width, rssi_pos.y};
        gfx_drawRect(fragment_pos, fragment_width, rssi_height, spectrum_getColorFromLevel((80+6*i)/4), true);
    }
}

/*
 * Function to draw RSSI-meter with level-meter of arbitrary size
 * Version without squelch bar for digital protocols
 * starting coordinates are relative to the top left point.
 *
 * ******************************************    <- volume         |
 * *             *                *               *               *|
 * ******************************************                      |
 * ******************************************    <- level          |
 * ******************************************                      |
 * ******************************************                      |
 * *             *                *               *               *|
 * ******************************************                      |  <-- Height (px)
 * ******************************************    <- RSSI           |
 * ******************************************                      |
 * ******************************************                      |
 * 1     2     3     4     5     6     7     8     9     +10    +20|
 * _________________________________________________________________
 *
 * ^
 * |
 *
 * Width (px)
 *
 */
void gfx_drawSmeterLevel(point_t start, uint16_t width, uint16_t height, rssi_t rssi,
                         uint8_t level, uint8_t volume, bool drawVolume)
{
    color_t red =    {255, 0,   0  , 255};
    color_t green =  {0,   255,   0, 255};
    color_t white =  {255, 255, 255, 255};
    color_t yellow = {250, 180, 19 , 255};

    fontSize_t font = FONT_SIZE_5PT;
    uint8_t font_height =  gfx_getFontHeight(font);
    uint16_t bar_height = (height - 6 - font_height);
    uint16_t bar_height_divider = drawVolume ? 7 : 6;

    uint16_t volume_height = drawVolume ? bar_height / bar_height_divider : 0;
    if (drawVolume)
    {
        // Speaker Volume Bar
        uint16_t volume_width = (width * volume) / 255;
        point_t volume_pos = start;
        gfx_drawRect(volume_pos, volume_width, volume_height, white, true);
    }

    // Level meter marks
    #if 0
    for(int i = 0; i <= 4; i++)
    {
        point_t pixel_pos =  {start.x, (uint8_t) (start.y + volume_height)};
        pixel_pos.x += i * (width - 1) / 4;
        gfx_setPixel(pixel_pos, white);
        pixel_pos.y += ((bar_height / bar_height_divider * 3) + 3);
        gfx_setPixel(pixel_pos, white);
    }
    #endif
    // Level bar
    uint16_t level_height = bar_height * 3 / bar_height_divider;
    uint16_t level_width = (width * level) / 255;
    point_t level_pos = { start.x, (uint8_t) (start.y + 2 + volume_height)};
    gfx_drawRect(level_pos, level_width, level_height, green, true);
    // RSSI bar
    int16_t s_level;
    if (rssi >= -53) { s_level = 11; }          // rssi >= -53dB set s_level to "11" (S9 + 20 dB)
    else if (rssi >= -73) {
        s_level =  (rssi_t)(163 + rssi) / 10;   // Increase s_level /10dB instead of /6dB for > S9
    }
    else if (rssi < -121) { s_level = 0; }     // s_level should not be negative + avoid overflow if rssi (int32_t) =< -196741
    else {
        s_level =  (rssi_t)(127 + rssi) / 6;    // 6dB increase per S-Point
    }
    uint16_t rssi_height = bar_height * 3 / bar_height_divider;
    uint16_t rssi_width = (s_level * (width - 1) / 11);
    point_t rssi_pos = {start.x, (uint8_t) (start.y + 5 + level_height + volume_height)};
    gfx_drawRect(rssi_pos, rssi_width, rssi_height, white, true);
    // S-level marks and numbers
    for(int i = 0; i < 12; i++)
    {
        color_t color = (i % 3 == 0) ? yellow : white;
        color = (i > 9) ? red : color;
        point_t pixel_pos = start;
        pixel_pos.x += i * (width - 1) / 11;
        pixel_pos.y += height;

        if (i == 10) {
            pixel_pos.x -= 8;
            gfx_print(pixel_pos, font, TEXT_ALIGN_LEFT, color, "+%d", i);
        }
        else if(i == 11){
            pixel_pos.x -= 10;
            gfx_print(pixel_pos, font, TEXT_ALIGN_LEFT, red, "+20");
        }
        else
            gfx_print(pixel_pos, font, TEXT_ALIGN_LEFT, color, "%d", i);
        if (i == 10) {
            pixel_pos.x += 8;
        }
    }
}

/*
 * Function to draw GPS satellites snr bar graph of arbitrary size
 * starting coordinates are relative to the top left point.
 *
 * ****            |
 * ****      ****  |
 * **** **** ****  |  <-- Height (px)
 * **** **** ****  |
 * **** **** ****  |
 * **** **** ****  |
 *                 |
 *  N    N+1  N+2  |
 * __________________
 *
 * ^
 * |
 *
 * Width (px)
 *
 */
void gfx_drawGPSgraph(point_t start,
                      uint16_t width,
                      uint16_t height,
                      gpssat_t *sats,
                      uint32_t active_sats)
{
    color_t white =  {255, 255, 255, 255};
    color_t yellow = {250, 180, 19 , 255};

    // SNR Bars and satellite identifiers
    uint8_t bar_width = (width - 26) / 12;
    uint8_t bar_height = 1;
    for(int i = 0; i < 12; i++)
    {
        bar_height = (height - 8) * sats[i].snr / 100 + 1;
        point_t bar_pos = start;
        bar_pos.x += 2 + i * (bar_width + 2);
        bar_pos.y += (height - 8) - bar_height;
        color_t bar_color = (active_sats & 1 << (sats[i].id - 1)) ? yellow : white;
        gfx_drawRect(bar_pos, bar_width, bar_height, bar_color, true);
        point_t id_pos = {bar_pos.x, (uint8_t) (start.y + height)};
        gfx_print(id_pos, FONT_SIZE_5PT, TEXT_ALIGN_LEFT,
                  bar_color, "%2d ", sats[i].id);
    }
    uint8_t bars_width = 9 + 11 * (bar_width + 2);
    point_t left_line_end    = start;
    point_t right_line_start = start;
    point_t right_line_end   = start;

    left_line_end.y    += height - 9;
    right_line_start.x += bars_width;
    right_line_end.x   += bars_width;
    right_line_end.y   += height - 9;

    gfx_drawLine(start, left_line_end, white);
    gfx_drawLine(right_line_start, right_line_end, white);
}

void gfx_drawGPScompass(point_t start,
                        uint16_t radius,
                        float deg,
                        bool active)
{
    color_t white =  {255, 255, 255, 255};
    color_t black =  {  0,   0,   0, 255};
    color_t yellow = {250, 180, 19 , 255};

    // Compass circle
    point_t circle_pos = start;
    circle_pos.x += radius + 1;
    circle_pos.y += radius + 3;
    gfx_drawCircle(circle_pos, radius, white);
    point_t n_box = {(uint8_t)(start.x + radius - 5), start.y};
    gfx_drawRect(n_box, 13, 13, black, true);
    float needle_radius = radius - 4;
    if (active)
    {
        // Needle
        deg -= 90.0f;
        point_t p1 = {(uint8_t)(circle_pos.x + needle_radius * COS(deg)),
                      (uint8_t)(circle_pos.y + needle_radius * SIN(deg))};
        point_t p2 = {(uint8_t)(circle_pos.x + needle_radius * COS(deg + 145.0f)),
                      (uint8_t)(circle_pos.y + needle_radius * SIN(deg + 145.0f))};
        point_t p3 = {(uint8_t)(circle_pos.x + needle_radius / 2 * COS(deg + 180.0f)),
                      (uint8_t)(circle_pos.y + needle_radius / 2 * SIN(deg + 180.0f))};
        point_t p4 = {(uint8_t)(circle_pos.x + needle_radius * COS(deg - 145.0f)),
                      (uint8_t)(circle_pos.y + needle_radius * SIN(deg - 145.0f))};
        gfx_drawLine(p1, p2, yellow);
        gfx_drawLine(p2, p3, yellow);
        gfx_drawLine(p3, p4, yellow);
        gfx_drawLine(p4, p1, yellow);
    }
    // North indicator
    point_t n_pos = {(uint8_t)(start.x + radius - 3),
                     (uint8_t)(start.y + 7)};
    gfx_print(n_pos, FONT_SIZE_6PT, TEXT_ALIGN_LEFT, white, "N");
}

void gfx_plotData(point_t start, uint16_t width, uint16_t height,
                  const int16_t *data, size_t len)
{
    uint16_t horizontal_pos = start.x;
    color_t white = {255, 255, 255, 255};
    point_t prev_pos = {0, 0};
    point_t pos = {0, 0};
    bool first_iteration = true;
    for (size_t i = 0; i < len; i++)
    {
        horizontal_pos++;
        if (horizontal_pos > (start.x + width))
            break;
        pos.x = horizontal_pos;
        pos.y = start.y + (height / 2)
              + ((data[i] * 4) / (2 * SHRT_MAX) * height);
        if (pos.y > CONFIG_SCREEN_HEIGHT)
            pos.y = CONFIG_SCREEN_HEIGHT;
        if (!first_iteration)
            gfx_drawLine(prev_pos, pos, white);
        prev_pos = pos;
        if (first_iteration)
            first_iteration = false;
    }
}

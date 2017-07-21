/*
 * File:       graphics.h
 * Date:       November 19, 2016
 * Author:     Craig Hollinger
 *
 * Public interface for graphics.c.
 
 * This is a collection of simple line drawing functions for a monochrome
 * graphic display of up to 256 pixels wide by 256 pixels high.  The pixels are
 * normally addressed with the upper-left part of the display containing the
 * first pixel.
 *
 * It is assumed the display is byte addressable and each byte contains 8 pixels
 * arranged in a vertical line.  The LS Bit is the upper-most pixel of the line,
 * the MS Bit is the lower.  AS the bytes are sent to the display, they are
 * written from left to right.
 *
 * This type of display cannot be read, so any graphics displayed need to be
 * stored in a local data area in RAM.  The pointer graphics_frame points to
 * this area.  All these functions act on this chunk of RAM and not on the
 * display directly.  The RAM must be uploaded to the display for any changes
 * to take effect.  The upload function is not part of this software module, it
 * will be found in the module associated with the display.  Displays will have
 * different interfaces to the microcontroller: I2C, SPI, UART, parallel, etc.
 *
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * or the GNU Lesser General Public License version 3, both as
 * published by the Free Software Foundation.
 */
#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_ 1

/* Colours of the display pixels.  Since this is a monochrome display, the only
   colours are black (dark) or white (lit).  Inverse just toggles the pixel. */
enum
{
  GRAPHICS_COLOUR_BLACK,
  GRAPHICS_COLOUR_WHITE,
  GRAPHICS_COLOUR_INVERSE,
  GRAPHICS_COLOUR_MAX
};

/* Display rotation in increments of 90 degrees clockwise. */
enum
{
  GRAPHICS_ROTATION_0,
  GRAPHICS_ROTATION_90,
  GRAPHICS_ROTATION_180,
  GRAPHICS_ROTATION_270,
  GRAPHICS_ROTATION_MAX
};

/*
 * graphics_init()
 *
 * Initialize the graphics system and allocate some memory for the graphics
 * frame.
 *
 * It is up to the user to ensure there is enough RAM in the microcontroller
 * to contain all the bytes needed to represent the chosen display.  For the
 * best security, the calling function should check the returned value.  If
 * zero, then malloc could not allocate enough memory and if the pointer is used
 * the program would likely crash.
 */
uint8_t graphics_init(uint8_t max_x, uint8_t max_y);

/*
 * graphics_exit()
 *
 * De-allocate the graphics memory.
 */
void graphics_exit(void);

/*
 * graphics_clear()
 *
 * Write the background colour to the local graphics memory.
 */
void graphics_clear(void);

/*
 * graphics_get_frame()
 *
 * Returns a pointer to the area of RAM containing the pixel data of the
 * display.
 */
uint8_t *graphics_get_frame(void);

/*
 * graphics_set_cursor()
 *
 * Place the cursor where the next graphic will be drawn.
 */
void graphics_set_cursor(uint8_t x, uint8_t y);

/*
 * graphics_set_text_size()
 *
 * Set the text magnification value.  No check is done here to see if the value
 * makes any sense.  If it is too large, unexpected results are likely to occur
 * when text characters are drawn.
 */
void graphics_set_text_size(uint8_t size);

/*
 * graphics_set_rotation()
 *
 * Set the rotation of text characters on the display.  This can only be one of
 * four different rotations.
 */
void graphics_set_rotation(uint8_t rotation);

/*
 * graphics_set_fg_colour()
 *
 * Set the foreground colour.
 */
void graphics_set_fg_colour(uint8_t colour);

/*
 * graphics_set_bg_colour()
 *
 * Set the background colour.
 */
void graphics_set_bg_colour(uint8_t colour);

/*
 * graphics_get_max_x()
 *
 * Return the value of the maximum x dimension of the display.
 */
uint8_t graphics_get_max_x(void)

/*
 * graphics_get_max_y()
 *
 * Return the value of the maximum y dimension of the display.
 */
uint8_t graphics_get_max_y(void)

/*
 * graphics_putChar()
 *
 * Draw the text character in graphics memory.  The character will be scaled up
 * by the value in graphics_text_size.  The location where the character is to
 * be drawn is determined by the values in graphics_cursor_x and y.  The font is
 * based on the 5x7 font in the array font5x7[][].  A column of blank pixels is
 * written to the right to space the characters if they are written in a string.
 *
 * Also, the local variables graphics_cursor_x and y are updated as this
 * function exits so that the putStr functions can write the string with
 * correctly spaced characters.
 */
void graphics_putChar(unsigned char c);

/*
 * graphics_putStr()
 *
 * Send a null terminated text string stored in RAM to the graphics buffer at
 * the position set by graphics_set_graphics_cursor() with rotation set by
 * set_graphics_rotation().
 */
void graphics_putStr(char str[]);

/*
 * graphics_putStrP()
 *
 * Draw a null terminated text string stored in program memory to the graphics
 * buffer at the positiion set by set_graphics_cursor() with rotation set by
 * set_graphics_rotation().
 */
void graphics_putStrP(const char *addr);

/*
 * graphics_draw_line()
 *
 * This function is based on Bresenham's Algorithm as described in Wikipedia
 * https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm.
 * The description is incomplete as there are 8 different ways a line can be
 * drawn and the algorithm presented only works with one.  Xiaolin Wu's line
 * algorithm, also from Wikipedia
 * https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm,
 * manipulates the endpoints of the line in the beginning of the function to
 * account for these scenarios.
 *
 * The code here adapts both algorithms and simplifies Xiaolin Wu's by not
 * drawing the additional pixels drawn with intensities varying according to
 * their distance from the line.
 *
 * The origin of the Cartesian plane the line is drawn on is assumed to be in
 * the upper left corner.  The x coordinate increases to the right and the 
 * y coordinate increases downward.
 *
 * x0, y0: start coordinates
 * x1, y1: end coordinates
 */
void graphics_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

/*
 * graphics_draw_circle()
 *
 * Draw a circle of radius r with center at x0, y0.
 *
 * This uses the midpoint circle algorithm and is taken directly from Wikipedea:
 *
 *  https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
 */
void graphics_draw_circle(uint8_t x0, uint8_t y0, uint8_t r);

/*
 * graphics_draw_filled_circle()
 *
 * This method doesn't draw a completely filled-in circle.  Still a work in
 * progress.
 */
void graphics_draw_filled_circle(uint8_t x0, uint8_t y0, uint8_t r);

/*
 * graphics_draw_rectangle()
 *
 * Draw a rectangle with the coordinates given.  Each pair of coordinates is
 * an opposite corner of the rectangle.  Simply draw four lines.
 */
void graphics_draw_rectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

/*
 * graphics_draw_filled_rectangle()
 *
 * Draw a filled rectangle with the coordinates given.  Each pair of coordinates
 * is an opposite corner of the rectangle.  Draw a series of lines from top to
 * bottom of rectangle.
 */
void graphics_draw_filled_rectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

#endif

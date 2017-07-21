/*
 * File:       ssd1306_i2c.h
 * Date:       July 20, 2017
 * Author:     Craig Hollinger
 *
 * Public interface for ssd1306_i2c.c display driver.   
 *
 * Driver software for ssd1306 display driver.  There are several displays
 * using this driver with different resolutions.  All have a 128-bit horizontal
 * resolution, varying only in the vertical resolution (16, 32, or 64 bit).
 * The file ssd1306_i2c.h contains the definitions for the horizontal and
 * vertical resolution.  Currently it defines the vertical resolution to be
 * 64-bits.  If a display with different resolution is used, the file will have
 * to be modified.
 *
 * Displays of this type come with different interfaces to a microcontroller:
 * I2C, SPI, UART, parallel, etc.  This code assumes the display interfaces is
 * the I2C bus.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * or the GNU Lesser General Public License version 3, both as
 * published by the Free Software Foundation.
 */
#ifndef _SSD1306_H_
#define _SSD1306_H_ 1

/* Dimensions of the display. */
#define SSD1306_GRAPHICS_MAX_X 128
/* This is the vertical resolution of the display, will need to be changed if a
   display with different resolution is used. */
#define SSD1306_GRAPHICS_MAX_Y 64

/* These definitions don't need changing if the display is changed. */
#define SSD1306_COL_MAX SSD1306_GRAPHICS_MAX_X
#define SSD1306_PAGE_MAX (SSD1306_GRAPHICS_MAX_Y / 8)
#define SSD1306_TEXT_MAX_X (SSD1306_GRAPHICS_MAX_X / 6)
#define SSD1306_TEXT_MAX_Y (SSD1306_GRAPHICS_MAX_Y / 8)

/* Display scrolling parameters. */
#define SSD1306_SCROLL_SPEED_2    0x07
#define SSD1306_SCROLL_SPEED_3    0x04
#define SSD1306_SCROLL_SPEED_4    0x05
#define SSD1306_SCROLL_SPEED_5    0x00
#define SSD1306_SCROLL_SPEED_25   0x06
#define SSD1306_SCROLL_SPEED_64   0x01
#define SSD1306_SCROLL_SPEED_128  0x02
#define SSD1306_SCROLL_SPEED_256  0x03

/*
 * ssd1306_i2c_init()
 *
 * Startup the I2C interface.  Initialize the display LCD controller.
 *
 * The initialization sequence for the controller is taken directly from the
 * manual and sets all registers to their reset values.
 */
void ssd1306_i2c_init(void);

/*
 * ssd1306_i2c_clear()
 *
 * Clear the LCD controller buffer by writing 0 to each RAM location. Reset the
 * row and column address pointers to 0 when done.
 */
void ssd1306_i2c_clear(void);

/*
 * ssd1306_i2c_putChar()
 *
 * Send character c to the LCD display at the character position set by 
 * ssd1306_i2c_setCursor().
 *
 * The characters are 5 columns wide and 8 rows high.  An additional column
 * of blank pixels is written to the right to space the characters.  
 *
 * The characters are written directly to the display, not to local graphics
 * RAM.  Graphics functions such as text size and rotation are not available.
 *
 * Depending on the display used, the screen is made up of up to 8 rows (8
 * pixels high) and 128 columns.  This function divides the horizontal columns
 * into sets of 6 columns which give 21 characters across.  This method uses up
 * 126 pixels across the display and leaves the last two on the right unused.
 */
void ssd1306_i2c_putChar(unsigned char c);

/*
 * ssd1306_i2c_set_text_cursor()
 *
 * Place the cursor where the next character will be written.  col and row are
 * assumed to be character positions not exact pixel locations: the display is
 * 21 characters wide and has up to 8 character rows depending on the display
 * type.
 */
void ssd1306_i2c_set_text_cursor(uint8_t col, uint8_t row);

/*
 * ssd1306_i2c_dspl_on()
 *
 * Turn on the display - light up all pixels that are 1.
 */
void ssd1306_i2c_dspl_on(void);

/*
 * ssd1306_i2c_dspl_off()
 *
 * Turn off the display - turn off all pixels.
 */
void ssd1306_i2c_dspl_off();

/*
 * ssd1306_i2c_dspl_inv()
 *
 * Invert the pixels on the display, that is turn off pixels that are lit
 * (1-bit) and pixels that are dark (0-bit) to lit.
 */
void ssd1306_i2c_dspl_inv(void);

/*
 * ssd1306_i2c_dspl_norm()
 *
 * Return the display to normal mode: 1-bits are lit and 0-bits are dark.
 */
void ssd1306_i2c_dspl_norm(void);

/*
 * ssd1306_i2c_flip_normal()
 *
 * Return display to normal vertical orientation with top at the ribbon.  For
 * this to take effect properly, the display data will have to be re-written.
 */
void ssd1306_i2c_flip_normal(void);

/*
 * ssd1306_i2c_flip_vertical()
 *
 * Flip the display vertically so that the bottom is at the ribbon.  For this to
 * take effect properly, the display data will have to be re-written.
 */
void ssd1306_i2c_flip_vertical(void);

/*
 * ssd1306_horiz_scroll_left()
 *
 * Setup the display to scroll horizontally to the left.  For scrolling to take
 * effect, the ssd1306_start_scroll() function must be called.
 *
 * top: top page of scrolling display area (0 - 7)
 * bot: bottom page of scrolling display area (0 - 7)
 * speed: scrolling speed (0 - 7)
 */
void ssd1306_horiz_scroll_left(uint8_t top, uint8_t bot, uint8_t speed);

/*
 * ssd1306_horiz_scroll_right()
 *
 * Setup the display to scroll horizontally to the right.  For scrolling to take
 * effect, the ssd1306_start_scroll() function must be called.
 *
 * top: top page of scrolling display area (0 - 7)
 * bot: bottom page of scrolling display area (0 - 7)
 * speed: scrolling speed (0 - 7)
 */
void ssd1306_horiz_scroll_right(uint8_t top, uint8_t bot, uint8_t speed);

/*
 * ssd1306_vert_horiz_scroll_right()
 *
 * Setup the display to scroll vertically and horizontally to the right.  For
 * scrolling to take effect, the ssd1306_start_scroll() function must be called.
 *
 * top: top page of scrolling display area (0 - 7)
 * bot: bottom page of scrolling display area (0 - 7)
 * speed: scrolling speed (0 - 7)
 * offset: vertical scrolling offset
 */
void ssd1306_vert_horiz_scroll_right(uint8_t top, uint8_t bot, uint8_t speed, uint8_t offset);

/*
 * ssd1306_vert_horiz_scroll_left()
 *
 * Setup the display to scroll vertically and horizontally to the left.  For
 * scrolling to take effect, the ssd1306_start_scroll() function must be called.
 *
 * top: top page of scrolling display area (0 - 7)
 * bot: bottom page of scrolling display area (0 - 7)
 * speed: scrolling speed (0 - 7)
 * offset: vertical scrolling offset
 */
void ssd1306_vert_horiz_scroll_left(uint8_t top, uint8_t bot, uint8_t speed, uint8_t offset);

/*
 * ssd1306_start_scroll()
 *
 * Start the display scrolling.  Once the display is scrolling, no changes can
 * be made.
 */
void ssd1306_start_scroll(void);

/*
 * ssd1306_stop_scroll()
 *
 * Stop the display scrolling.
 *
 * This command must be issued before any scrolling parameters are sent to the
 * display, otherwise no changes will be made.
 */
void ssd1306_stop_scroll(void);

/*
 * ssd1306_i2c_putStr()
 *
 * Send a null terminated string stored in RAM to the LCD display at the
 * position set by ssd1306_i2c_set_text_cursor().  The data is sent directly to
 * the display and not to local graphics RAM.  Changes to text size and
 * rotation are not possible with this function.
 */
void ssd1306_i2c_putStr(char str[]);

/*
 * ssd1306_i2c_putStrP()
 *
 * Send a null terminated string stored in program memory to the LCD display
 * at the positiion set by ssd1306_i2c_set_text_cursor().  The data is sent
 * directly to the display and not to local graphics RAM.  Changes to text size
 * and rotation are not possible with this function.
 */
void ssd1306_i2c_putStr_P(const char *addr);

/*
 * graphics_i2c_update()
 *
 * Send the entire local graphics memory to the display.
 */
void ssd1306_i2c_graphics_update(void);

#endif

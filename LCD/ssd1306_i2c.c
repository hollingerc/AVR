/*
 * File:       ssd1306_i2c.c
 * Date:       July 20, 2017
 * Author:     Craig Hollinger
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
 * The text font comes from the file 'font5x7.c' which places the data in FLASH
 * memory.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * or the GNU Lesser General Public License version 3, both as
 * published by the Free Software Foundation.
 */
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "i2c/i2c.h"
#include "graphics/font5x7.h"
#include "graphics/graphics.h"
#include "ssd1306/ssd1306_i2c.h"

/*
 * Device slave address
 */
#define SSD1306SlaveAdrs    0x3c

/* Set the display contrast.  The second byte contains the contrast level -
 * increasing value increases the contrast.
 *
 * reset = 0x7f
 */
#define SSD1306_SET_CONTRAST       0x81
#define SSD1306_SET_CONTRAST_RESET 0x7f

/* Entire display on/off.  If on, display RAM contents.  If off, display
 * nothing.
 *
 * reset = 0xa4
 */
#define SSD1306_ENTIRE_DSPL_RAM 0xa4
#define SSD1306_ENTIRE_DSPL_ON  0xa5

/* Display normal or inverse.
 *
 * reset = 0xa6
 */
#define SSD1306_DISPLAY_NORM 0xa6
#define SSD1306_DISPLAY_INV  0xa7

/* Display on or off control.  When off, display is in sleep mode.
 *
 * reset = 0xae (sleep)
 */
#define SSD1306_DISPLAY_OFF 0xae
#define SSD1306_DISPLAY_ON  0xaf

/* Set the lower nibble of the column start address register for Page
 * Addressing Mode. The lower 4 bits contain the address.
 *
 * This command is only for Page Addressing Mode.
 *
 * reset = 0x00
 */
#define SSD1306_COL_ADRS_LO 0x00

/* Set the upper nibble of the column start address register for Page
 * Addressing Mode. The lower 4 bits contain the address.
 *
 * This command is used only for Page Addressing Mode.
 *
 * reset = 0x10
 */
#define SSD1306_COL_ADRS_HI 0x10

/* Set the memory addressing mode.  The second byte contains the two bits
 * that set the mode.
 *
 * reset = 0x02
 */
#define SSD1306_MEM_ADRS_MODE      0x20
#define SSD1306_MEM_ADRS_MODE_HORZ 0x00
#define SSD1306_MEM_ADRS_MODE_VERT 0x01
#define SSD1306_MEM_ADRS_MODE_PAGE 0x02

/* Set the column start and end addresses.  The second byte contains 7-bit
 * start address and the third byte contains the 7-bit end address.
 *
 * reset = 0x00, 0x7f
 */
#define SSD1306_SET_COL_ADRS 0x21

/* Set the page start and end addresses.  The second byte contains 7-bit
 * start address and the third byte contains the 7-bit end address.
 *
 * reset = 0x00, 0x07
 */
#define SSD1306_SET_PAGE_ADRS 0x22

/* Set the page start address (0-7).  The lower three bits contain the
 * address.
 */
#define SSD1306_SET_PAGE_ADRS_PAGE_MODE 0xb0
#define PAGE0 0
#define PAGE1 1
#define PAGE2 2
#define PAGE3 3
#define PAGE4 4
#define PAGE5 5
#define PAGE6 6
#define PAGE7 7

/* Set display start line (0-63).  The lower six bits contain the 
 * address.
 *
 * reset = 0x00
 */
#define SSD1306_SET_START_LINE       0x40
#define SSD1306_SET_START_LINE_RESET 0x00

/* Segment re-map.  If LSB = 0, address 0 is mapped to SEG0.  If LSB = 1,
 * address 127 is mapped to SEG0.
 *
 * reset = 0xa0
 */
#define SSD1306_SEG_REMAP_0   0xa0
#define SSD1306_SEG_REMAP_127 0xa1

/* Set multiplex ratio.  The second byte contains the 6-bit multiplex
 * ratio.  The range is 15-63 (0-14 are invalid).
 *
 * reset = 0x3f
 */
#define SSD1306_SET_MUX_RATIO       0xa8
#define SSD1306_SET_MUX_RATIO_RESET 0x3f

/* Set COM Output scan direction.
 *
 * reset = 0xc0
 */
#define SSD1306_SET_COM_SCAN_NORM 0xc0
#define SSD1306_SET_COM_SCAN_RMAP 0xc8

/* Set display offset (vertical shift by COM).  The second byte contains
 * the shift (0-64)
 *
 * reset = 0x00
 */
#define SSD1306_DSPL_OFFSET       0xd3
#define SSD1306_DSPL_OFFSET_RESET 0x00

/* Set COM pins hardware configuration.  Bits 5&4 of the second byte set the
 * configuration.
 *
 * reset = 0b00010010 (Alt)
 */
#define SSD1306_SET_COM_CONFIG         0xda
#define SSD1306_SET_COM_CONFIG_SEQ     0b00000010
#define SSD1306_SET_COM_CONFIG_ALT     0b00010010
#define SSD1306_SET_COM_CONFIG_MAP_DIS 0b00000010
#define SSD1306_SET_COM_CONFIG_MAP_EN  0b00100010

/* Set display clock divide ratio and frequency.  The lower nibble of the
 * second byte sets the divide ratio.  The upper nibble sets the frequency.  The
 * frequency increases as the value increases.
 *
 * reset = 0x80
 */
#define SSD1306_SET_CLOCK_FREQ       0xd5
#define SSD1306_SET_CLOCK_FREQ_RESET 0x80

/* Set pre-charge period.  The lower nibble of the second byte sets the period
 * of phase 1.  The upper nibble sets the period of phase 2.
 *
 * reset = 0x22
 */
#define SSD1306_SET_PRE_CHARGE 0xd9

/* Set Vcom de-select level.  Bits 6:4 of the second byte sets the level.  The
 * levels are:
 *
 * 0.65 * Vcc
 * 0.77 * Vcc
 * 0.83 * Vcc
 *
 * reset = 0b00100000
 */
#define SSD1306_SET_VCOM_DE_SELECT    0xdb
#define SSD1306_SET_VCOM_DE_SELECT_65 0b00000000
#define SSD1306_SET_VCOM_DE_SELECT_77 0b00100000
#define SSD1306_SET_VCOM_DE_SELECT_83 0b00110000

/* Charge pump setting.  Bit 2 of the second byte enables/disable the pump.
 *
 * reset = 0b00010000
 */
#define SSD1306_CHARGE_PUMP     0x8d
#define SSD1306_CHARGE_PUMP_DIS 0b00010000
#define SSD1306_CHARGE_PUMP_EN  0b00010100

/* Display scrolling commands. */
#define SSD1306_SET_HORIZ_RIGHT_SCROLL  0x26
#define SSD1306_SET_HORIZ_LEFT_SCROLL   0x27
#define SSD1306_SET_VERT_RIGHT_SCROLL   0x29
#define SSD1306_SET_VERT_LEFT_SCROLL    0x2a
#define SSD1306_STOP_SCROLL             0x2e
#define SSD1306_START_SCROLL            0x2f
#define SSD1306_SET_VERT_SCROLL_AREA    0xa3

/* These bytes are sent following the Slave Address to indicate if the 
 * following bytes are data or a command. */
#define SSD1306_I2C_DATA     0b01000000
#define SSD1306_I2C_COMMAND  0b00000000
#define SSD1306_I2C_CONTINUE 0b10000000

/* ssd1306_i2c_command()
 *
 * Send a command to the display.
 */
void ssd1306_command(uint8_t cmd)
{
  i2c_start();
  i2c_putchar((SSD1306SlaveAdrs<<1) & 0b11111110);
  i2c_putchar(SSD1306_I2C_COMMAND);
  i2c_putchar(cmd);
  i2c_stop();
}

/*
 * ssd1306_i2c_init()
 *
 * Startup the I2C interface.  Initialize the display LCD controller.
 *
 * The initialization sequence for the controller is taken directly from the
 * manual and sets all registers to their reset values.
 */
void ssd1306_i2c_init(void)
{
  uint8_t buf[2];

/* set Mux Ratio to reset value */
  buf[0] = SSD1306_SET_MUX_RATIO;
  buf[1] = SSD1306_SET_MUX_RATIO_RESET;
  i2c_write(SSD1306SlaveAdrs, 2, SSD1306_I2C_COMMAND, buf);

/* set Display Offset to reset value */
  buf[0] = SSD1306_DSPL_OFFSET;
  buf[1] = SSD1306_DSPL_OFFSET_RESET;
  i2c_write(SSD1306SlaveAdrs, 2, SSD1306_I2C_COMMAND, buf);

/* set Display Start Line to reset value */
  buf[0] = SSD1306_SET_START_LINE + SSD1306_SET_START_LINE_RESET;
  i2c_write(SSD1306SlaveAdrs, 1, SSD1306_I2C_COMMAND, buf);

/* set Segment Re-map to reset value, flips display left to right
   Doesn't affect data already in RAM, takes effect on next write to display */
  buf[0] = SSD1306_SEG_REMAP_0; // reset value
//  buf[0] = SSD1306_SEG_REMAP_127;
  i2c_write(SSD1306SlaveAdrs, 1, SSD1306_I2C_COMMAND, buf);

/* set COM Scan Direction to reset value, flips display vertically */
  buf[0] = SSD1306_SET_COM_SCAN_NORM; // normal display (reset), top at ribbon
//  buf[0] = SSD1306_SET_COM_SCAN_RMAP; // flips display, top at pins
  i2c_write(SSD1306SlaveAdrs, 1, SSD1306_I2C_COMMAND, buf);

/* set COM Pins Configuration to reset value */
  buf[0] = SSD1306_SET_COM_CONFIG;
  buf[1] = SSD1306_SET_COM_CONFIG_ALT;
  i2c_write(SSD1306SlaveAdrs, 2, SSD1306_I2C_COMMAND, buf);

/* set Display Contrast to reset value */
  buf[0] = SSD1306_SET_CONTRAST;
  buf[1] = SSD1306_SET_CONTRAST_RESET;
  i2c_write(SSD1306SlaveAdrs, 2, SSD1306_I2C_COMMAND, buf);

/* set Display On/RAM to reset value */
  buf[0] = SSD1306_ENTIRE_DSPL_RAM;
  i2c_write(SSD1306SlaveAdrs, 1, SSD1306_I2C_COMMAND, buf);

/* set Display Normal/Inverse to reset value */
  buf[0] = SSD1306_DISPLAY_NORM;
  i2c_write(SSD1306SlaveAdrs, 1, SSD1306_I2C_COMMAND, buf);

/* set Oscillator Frequency to reset value */
  buf[0] = SSD1306_SET_CLOCK_FREQ;
  buf[1] = SSD1306_SET_CLOCK_FREQ_RESET;
  i2c_write(SSD1306SlaveAdrs, 2, SSD1306_I2C_COMMAND, buf);

/* enable the Charge Pump */
  buf[0] = SSD1306_CHARGE_PUMP;
  buf[1] = SSD1306_CHARGE_PUMP_EN;
  i2c_write(SSD1306SlaveAdrs, 2, SSD1306_I2C_COMMAND, buf);

/* turn On the display */
  buf[0] = SSD1306_DISPLAY_ON;
  i2c_write(SSD1306SlaveAdrs, 1, SSD1306_I2C_COMMAND, buf);

/* set the Memory Address Mode to Horizontal Mode */
  buf[0] = SSD1306_MEM_ADRS_MODE;
  buf[1] = SSD1306_MEM_ADRS_MODE_HORZ;
  i2c_write(SSD1306SlaveAdrs, 2, SSD1306_I2C_COMMAND, buf);

/* set the Page and Column Address pointers */
	buf[0] = SSD1306_SET_COL_ADRS;
	buf[1] = 0;
	buf[2] = SSD1306_GRAPHICS_MAX_X - 1;
	i2c_write(SSD1306SlaveAdrs, 3, SSD1306_I2C_COMMAND, buf);
	buf[0] = SSD1306_SET_PAGE_ADRS;
	buf[1] = 0;
	buf[2] = (SSD1306_GRAPHICS_MAX_Y / 8) - 1;
	i2c_write(SSD1306SlaveAdrs, 3, SSD1306_I2C_COMMAND, buf);

/* clear the display memory and reset the row and column pointers */
  ssd1306_i2c_clear();

}/* end ssd1306_i2c_init() */

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
void ssd1306_i2c_putChar(unsigned char c)
{
  uint8_t i, buf[6];

/* If c addresses data beyond font5x7[][], exit. */
  if((c > FONT5X7_MAX) || (c < FONT5X7_MIN))
  {
    return;
  }

/* adjust c to skip non-printing characters */
  c -= 0x20;

  for (i = 0; i < 5; i++)
  {
    buf[i] = pgm_read_byte_near((PGM_P)&font5x7[c][i]);
  }
  buf[i] = 0;
  i2c_write(SSD1306SlaveAdrs, 6, SSD1306_I2C_DATA, buf);

}/* end ssd1306_i2c_putChar() */

/*
 * ssd1306_i2c_set_text_cursor()
 *
 * Place the cursor where the next character will be written.  col and row are
 * assumed to be character positions not exact pixel locations: the
 * display is 21 characters wide and has up to 8 character rows depending on the
 * display type.
 */
void ssd1306_i2c_set_text_cursor(uint8_t col, uint8_t row)
{
  uint8_t buf[3];

/* Ensure the new position will be on the display. If not, exit. */
  if((col >= SSD1306_TEXT_MAX_X) || (row >= SSD1306_TEXT_MAX_Y))
  {
    return;
  }

  col *= 6;

/* set the Page and Column Address pointers */
	buf[0] = SSD1306_SET_COL_ADRS;
	buf[1] = col;
	buf[2] = SSD1306_GRAPHICS_MAX_X - 1;
	i2c_write(SSD1306SlaveAdrs, 3, SSD1306_I2C_COMMAND, buf);
	buf[0] = SSD1306_SET_PAGE_ADRS;
	buf[1] = row;
	buf[2] = (SSD1306_GRAPHICS_MAX_Y / 8) - 1;
	i2c_write(SSD1306SlaveAdrs, 3, SSD1306_I2C_COMMAND, buf);

}/* end ssd1306_i2c_set_text_cursor() */

/*
 * ssd1306_i2c_clear()
 *
 * Clear the LCD controller buffer by writing 0 to each RAM location. Reset the
 * row and column address pointers to 0 when done.
 */
void ssd1306_i2c_clear(void)
{
	uint8_t buf[3];

	/* set the Page and Column Address pointers */
  buf[0] = SSD1306_SET_COL_ADRS;
  buf[1] = 0;
  buf[2] = SSD1306_GRAPHICS_MAX_X - 1;
  i2c_write(SSD1306SlaveAdrs, 3, SSD1306_I2C_COMMAND, buf);
	buf[0] = SSD1306_SET_PAGE_ADRS;
	buf[1] = 0;
	buf[2] = (SSD1306_GRAPHICS_MAX_Y / 8) - 1;
  i2c_write(SSD1306SlaveAdrs, 3, SSD1306_I2C_COMMAND, buf);

  /* fill display RAM with 0 */
  i2c_start();
  i2c_putchar((SSD1306SlaveAdrs<<1) & 0b11111110);
  i2c_putchar(SSD1306_I2C_DATA);
	for(uint16_t i = 0; i < (SSD1306_GRAPHICS_MAX_X * SSD1306_GRAPHICS_MAX_Y / 8); i++)
  {
  	i2c_putchar(0);
	}
  i2c_stop();

  /* reset the address pointers */
  buf[0] = SSD1306_SET_COL_ADRS;
  buf[1] = 0;
  buf[2] = SSD1306_GRAPHICS_MAX_X - 1;
  i2c_write(SSD1306SlaveAdrs, 3, SSD1306_I2C_COMMAND, buf);
  buf[0] = SSD1306_SET_PAGE_ADRS;
  buf[1] = 0;
  buf[2] = (SSD1306_GRAPHICS_MAX_Y / 8) - 1;
  i2c_write(SSD1306SlaveAdrs, 3, SSD1306_I2C_COMMAND, buf);

}/* end ssd1306_i2c_clear() */

/*
 * ssd1306_i2c_dspl_on()
 *
 * Turn on the display - light up all pixels that are 1.
 */
void ssd1306_i2c_dspl_on(void)
{
  uint8_t buf;
  
  buf = SSD1306_DISPLAY_ON;
  i2c_write(SSD1306SlaveAdrs, 1, SSD1306_I2C_COMMAND, &buf);

}/* end ssd1306_i2c_dspl_on() */

/*
 * ssd1306_i2c_dspl_off()
 *
 * Turn off the display - turn off all pixels.
 */
void ssd1306_i2c_dspl_off(void)
{
  uint8_t buf;
  
  buf = SSD1306_DISPLAY_OFF;
  i2c_write(SSD1306SlaveAdrs, 1, SSD1306_I2C_COMMAND, &buf);

}/* end ssd1306_i2c_dspl_off() */

/*
 * ssd1306_i2c_dspl_inv()
 *
 * Invert the pixels on the display, that is turn off pixels that are lit
 * (1-bit) and pixels that are dark (0-bit) to lit.
 */
void ssd1306_i2c_dspl_inv(void)
{
  uint8_t buf;
  
  buf = SSD1306_DISPLAY_INV;
  i2c_write(SSD1306SlaveAdrs, 1, SSD1306_I2C_COMMAND, &buf);

}/* end ssd1306_i2c_dspl_inv() */

/*
 * ssd1306_i2c_dspl_norm()
 *
 * Return the display to normal mode: 1-bits are lit and 0-bits are dark.
 */
void ssd1306_i2c_dspl_norm(void)
{
  uint8_t buf;
  
  buf = SSD1306_DISPLAY_NORM;
  i2c_write(SSD1306SlaveAdrs, 1, SSD1306_I2C_COMMAND, &buf);

}/* end ssd1306_i2c_dspl_norm() */

/*
 * ssd1306_i2c_flip_normal()
 *
 * Return display to normal vertical orientation with top at the ribbon.  For
 * this to take effect properly, the display data will have to be re-written.
 */
void ssd1306_i2c_flip_normal(void)
{
  uint8_t buf;

  buf = SSD1306_SEG_REMAP_0; // reset value
  i2c_write(SSD1306SlaveAdrs, 1, SSD1306_I2C_COMMAND, &buf);
  buf = SSD1306_SET_COM_SCAN_NORM; // normal display (reset), top at ribbon
  i2c_write(SSD1306SlaveAdrs, 1, SSD1306_I2C_COMMAND, &buf);

}/* end ssd1306_i2c_flip_normal() */

/*
 * ssd1306_i2c_flip_vertical()
 *
 * Flip the display vertically so that the bottom is at the ribbon.  For this to
 * take effect properly, the display data will have to be re-written.
 */
void ssd1306_i2c_flip_vertical(void)
{
  uint8_t buf;

  buf = SSD1306_SEG_REMAP_127;
  i2c_write(SSD1306SlaveAdrs, 1, SSD1306_I2C_COMMAND, &buf);
  buf = SSD1306_SET_COM_SCAN_RMAP;
  i2c_write(SSD1306SlaveAdrs, 1, SSD1306_I2C_COMMAND, &buf);

}/* end ssd1306_i2c_flip_vertical() */

/*
 * set_horiz_scroll()
 *
 * Setup the horizontal scrolling area and direction.  When turned on, the image
 * will scroll across the screen either to the left or to the right.
 *
 * cmd: sets the direction, can only be 0x26 (right) or 0x27 (left)
 * top: top of horizontal section of screen, can only be 0-7
 * bot: bottom of horizontal section of screen, can only be 0-7
 * speed: sets the scrolling speed, can only be 0-7
 */
void set_horiz_scroll(uint8_t cmd, uint8_t top, uint8_t bot, uint8_t speed)
{
  uint8_t buf[7];

  if((cmd != SSD1306_SET_HORIZ_RIGHT_SCROLL) && (cmd != SSD1306_SET_HORIZ_LEFT_SCROLL))
  {
    return;
  }
  if((top > 7) || (bot > 7) || (top > bot) || (speed > 7))
  {
    return;
  }
  buf[0] = cmd;
  buf[1] = 0;
  buf[2] = top;
  buf[3] = speed;
  buf[4] = bot;
  buf[5] = 0;
  buf[6] = 0xff;
  i2c_write(SSD1306SlaveAdrs, 7, SSD1306_I2C_COMMAND, buf);

}/* end set_horiz_scroll() */

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
void ssd1306_horiz_scroll_left(uint8_t top, uint8_t bot, uint8_t speed)
{
  set_horiz_scroll(SSD1306_SET_HORIZ_LEFT_SCROLL, top, bot, speed);

}/* end ssd1306_horiz_scroll_left() */

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
void ssd1306_horiz_scroll_right(uint8_t top, uint8_t bot, uint8_t speed)
{
  set_horiz_scroll(SSD1306_SET_HORIZ_RIGHT_SCROLL, top, bot, speed);

}/* end ssd1306_horiz_scroll_right() */

/*
 * set_vert_horiz_scroll()
 *
 * Setup the vertical/horizontal scrolling area and direction.  When turned on,
 * the image will scroll across the screen either to the left or to the right
 * and up vertically at the same time.
 *
 * cmd: sets the direction, can only be 0x29 (vertical/right) or
 *      0x2a (vertical/left)
 * top: top of horizontal section of screen, can only be 0-7
 * bot: bottom of horizontal section of screen, can only be 0-7
 * speed: sets the scrolling speed, can only be 0-7
 * offset: vertical offset (0 - 63)
 */
void set_vert_horiz_scroll(uint8_t cmd, uint8_t top, uint8_t bot, uint8_t speed, uint8_t offset)
{
  uint8_t buf[6];

  if((cmd != SSD1306_SET_VERT_RIGHT_SCROLL) && (cmd != SSD1306_SET_VERT_LEFT_SCROLL))
  {
    return;
  }
  if((top > 7) || (bot > 7) || (top > bot) || (speed > 7) || (offset > 63))
  {
    return;
  }
  buf[0] = cmd;
  buf[1] = 0;
  buf[2] = top;
  buf[3] = speed;
  buf[4] = bot;
  buf[5] = offset;
  i2c_write(SSD1306SlaveAdrs, 6, SSD1306_I2C_COMMAND, buf);

}/* end set_vert_horiz_scroll() */

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
void ssd1306_vert_horiz_scroll_right(uint8_t top, uint8_t bot, uint8_t speed, uint8_t offset)
{
  set_vert_horiz_scroll(SSD1306_SET_VERT_RIGHT_SCROLL, top, bot, speed, offset);

}/* end ssd1306_vert_horiz_scroll_right() */

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
void ssd1306_vert_horiz_scroll_left(uint8_t top, uint8_t bot, uint8_t speed, uint8_t offset)
{
  set_vert_horiz_scroll(SSD1306_SET_VERT_LEFT_SCROLL, top, bot, speed, offset);

}/* end ssd1306_vert_horiz_scroll_left() */

/*
 * ssd1306_start_scroll()
 *
 * Start the display scrolling.
 */
void ssd1306_start_scroll(void)
{
  uint8_t buf = SSD1306_START_SCROLL;
  
  i2c_write(SSD1306SlaveAdrs, 1, SSD1306_I2C_COMMAND, &buf);

}/* end ssd1306_start_scroll() */

/*
 * ssd1306_stop_scroll()
 *
 * Stop the display scrolling.
 *
 * This command must be issued before any scrolling parameters are sent to the
 * display, otherwise no changes will be made.
 */
void ssd1306_stop_scroll(void)
{
  uint8_t buf = SSD1306_STOP_SCROLL;
  
  i2c_write(SSD1306SlaveAdrs, 1, SSD1306_I2C_COMMAND, &buf);

}/* end ssd1306_stop_scroll() */

/*
 * ssd1306_i2c_putStr()
 *
 * Send a null terminated string stored in RAM to the LCD display at the
 * position set by ssd1306_i2c_set_text_cursor().  The data is sent directly to
 * the display and not to local graphics RAM.  Changes to text size and
 * rotation are not possible with this function.
 */
void ssd1306_i2c_putStr(char str[])
{

  char c;
  uint8_t i = 0;

  while ((c = str[i++]) != 0)
  {
    ssd1306_i2c_putChar(c);
  }

}/* end ssd1306_i2c_putStr() */

/*
 * ssd1306_i2c_putStrP()
 *
 * Send a null terminated string stored in program memory to the LCD display
 * at the positiion set by ssd1306_i2c_set_text_cursor().  The data is sent
 * directly to the display and not to local graphics RAM.  Changes to text size
 * and rotation are not possible with this function.
 */
void ssd1306_i2c_putStr_P(const char *addr)
{

  char c;

  while ((c = pgm_read_byte(addr++)) != 0)
  {
    ssd1306_i2c_putChar(c);
  }

}/* end ssd1306_i2c_putStrP() */

/*
 * graphics_i2c_update()
 *
 * Send the entire local graphics memory to the display.
 */
void ssd1306_i2c_graphics_update(void)
{
	uint8_t buf[3],
          *graphics_frame;

  graphics_frame = graphics_get_frame();

  if(graphics_frame == 0)
  {
    return;
  }

/* set the Page and Column Address pointers */
  buf[0] = SSD1306_SET_COL_ADRS;
  buf[1] = 0;
  buf[2] = SSD1306_GRAPHICS_MAX_X - 1;
  i2c_write(SSD1306SlaveAdrs, 3, SSD1306_I2C_COMMAND, buf);
  buf[0] = SSD1306_SET_PAGE_ADRS;
  buf[1] = 0;
  buf[2] = (SSD1306_GRAPHICS_MAX_Y / 8) - 1;
  i2c_write(SSD1306SlaveAdrs, 3, SSD1306_I2C_COMMAND, buf);

  i2c_start();
  i2c_putchar((SSD1306SlaveAdrs<<1) & 0b11111110);
  i2c_putchar(SSD1306_I2C_DATA);
  for(uint16_t i = 0; i < (SSD1306_GRAPHICS_MAX_X * SSD1306_GRAPHICS_MAX_Y / 8); i++)
  {
      i2c_putchar(graphics_frame[i]);
	}
  i2c_stop();

/* reset the address pointers */
  buf[0] = SSD1306_SET_COL_ADRS;
  buf[1] = 0;
  buf[2] = SSD1306_GRAPHICS_MAX_X - 1;
  i2c_write(SSD1306SlaveAdrs, 3, SSD1306_I2C_COMMAND, buf);
  buf[0] = SSD1306_SET_PAGE_ADRS;
  buf[1] = 0;
  buf[2] = (SSD1306_GRAPHICS_MAX_Y / 8) - 1;
  i2c_write(SSD1306SlaveAdrs, 3, SSD1306_I2C_COMMAND, buf);

}/* end graphics_i2c_update() */

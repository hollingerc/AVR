/*
 * File:       graphics.c
 * Date:       July 20, 2017
 * Author:     Craig Hollinger
 *
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
 * The font for text characters is contained in its own module (font5x7.c) and
 * is interfaced with font5x7.h.  If a different font is desired, a different
 * font module can be used and this file modified accordingly.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * or the GNU Lesser General Public License version 3, both as
 * published by the Free Software Foundation.
 */
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "graphics/graphics.h"
#include "graphics/font5x7.h"

/* This points to the RAM area that would store a copy of the display.  This
   area needs to be big enough to store the data for the chosen display. */
uint8_t *graphics_frame = 0;

/* Local variables.  Their names should be enough description. */
uint8_t graphics_cursor_x = 0,
        graphics_cursor_y = 0,
        graphics_max_x = 0,
        graphics_max_y = 0,
        graphics_text_size = 1,
        fg_colour = GRAPHICS_COLOUR_WHITE,
        bg_colour = GRAPHICS_COLOUR_BLACK,
        graphics_rotation = GRAPHICS_ROTATION_0;

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
uint8_t graphics_init(uint8_t max_x, uint8_t max_y)
{
  graphics_frame = (uint8_t *)malloc(max_x * max_y / 8);

  if(graphics_frame == 0){
    return(1);
  }

  graphics_max_x = max_x;
  graphics_max_y = max_y;
  graphics_cursor_x = 0;
  graphics_cursor_y = 0;
  graphics_text_size = 1;
  fg_colour = GRAPHICS_COLOUR_WHITE;
  bg_colour = GRAPHICS_COLOUR_BLACK;

  graphics_clear();

  return(0);

}/* end graphics_init() */

/*
 * graphics_exit()
 *
 * De-allocate the graphics memory.
 */
void graphics_exit(void)
{
  if(graphics_frame != 0){
    free(graphics_frame);
  }

  graphics_frame = 0;

}/* end graphics_exit() */

/*
 * graphics_get_frame()
 *
 * Returns a pointer to the area of RAM containing the pixel data of the
 * display.
 */
uint8_t *graphics_get_frame(void)
{
  return(graphics_frame);

}/* end graphics_get_frame() */

/*
 * swap()
 *
 * Swap a and b.
 */
void swap(uint8_t *a, uint8_t *b)
{
  uint8_t temp;
  
  temp = *a;
  *a = *b;
  *b = temp;
}

/*
 * graphics_clear()
 *
 * Write the background colour to the local graphics memory.
 */
void graphics_clear(void)
{

  if(graphics_frame != 0)
  {
    for(uint16_t i = 0; i < (graphics_max_x * graphics_max_y / 8); i++)
    {
      graphics_frame[i] = bg_colour;
    }
  }  

}/* end graphics_clear() */

/*
 * graphics_set_cursor()
 *
 * Place the cursor where the next graphic will be drawn.
 */
void graphics_set_cursor(uint8_t x, uint8_t y)
{
/* Ensure the new position will be on the display. If not, exit. */
  if((x >= graphics_max_x) || (y >= graphics_max_y))
  {
    return;
  }

  graphics_cursor_x = x;
  graphics_cursor_y = y;

}/* end graphics_set_cursor() */

/*
 * graphics_set_text_size()
 *
 * Set the text magnification value.  No check is done here to see if the value
 * makes any sense.  If it is too large, unexpected results are likely to occur
 * when text characters are drawn.
 */
void graphics_set_text_size(uint8_t size)
{
  if(size == 0)
  {
    return;
  }

  graphics_text_size = size;

}/* end graphics_set_text_size() */

/*
 * graphics_set_rotation()
 *
 * Set the rotation of text characters on the display.  This can only be one of
 * four different rotations.
 */
void graphics_set_rotation(uint8_t rotation)
{
  if(rotation >= GRAPHICS_ROTATION_MAX)
  {
    return;
  }
  graphics_rotation = rotation;

}/* end graphics_set_rotation() */

/*
 * graphics_set_fg_colour()
 *
 * Set the foreground colour.
 */
void graphics_set_fg_colour(uint8_t colour)
{
  if(colour >= GRAPHICS_COLOUR_MAX)
  {
    return;
  }

  fg_colour = colour;

}/* end graphics_set_fg_colour() */

/*
 * graphics_set_bg_colour()
 *
 * Set the background colour.
 */
void graphics_set_bg_colour(uint8_t colour)
{
  if(colour >= GRAPHICS_COLOUR_MAX)
  {
    return;
  }

  bg_colour = colour;

}/* end graphics_set_fg_colour() */

/*
 * graphics_get_max_x()
 *
 * Return the value of the maximum x dimension of the display.
 */
uint8_t graphics_get_max_x(void)
{
  return(graphics_max_x);

}/* end graphics_get_max_x() */

/*
 * graphics_get_max_y()
 *
 * Return the value of the maximum y dimension of the display.
 */
uint8_t graphics_get_max_y(void)
{
  return(graphics_max_y);

}/* end graphics_get_max_y() */

/*
 * plot_pixel()
 *
 * Set one bit in graphics_frame[].  
 *
 * Since the display cannot be read, a copy of what is displayed on the screen
 * is kept in RAM in graphics_frame[].  Once the display memory has been
 * updated, the whole array can be written to the display.
 *
 * x, y: location of the pixel
 * colour: pixel colour (either on or off)
 */
void plot_pixel(uint8_t x, uint8_t y, uint8_t colour)
{
  if((x >= graphics_max_x) || (y >= graphics_max_y) ||
     (graphics_frame == 0) || (colour >= GRAPHICS_ROTATION_MAX))
  {
    return;
  }
  switch(colour)
  {
    case GRAPHICS_COLOUR_BLACK:
      graphics_frame[(y / 8 * graphics_max_x) + x] &= ~(1 << (y & 0b00000111));
      break;

    case GRAPHICS_COLOUR_WHITE:
      graphics_frame[(y / 8 * graphics_max_x) + x] |= (1 << (y & 0b00000111));
      break;

    case GRAPHICS_COLOUR_INVERSE:
      graphics_frame[(y / 8 * graphics_max_x) + x] ^= (1 << (y & 0b00000111));
      break;

    default:
      break;
  }    
}/* end plot_pixel() */

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
void graphics_putChar(unsigned char c)
{
  uint8_t i, j, k, l, character, x, y;

/* If c addresses data beyond font5x7[][], exit. */
  if((c > FONT5X7_MAX) || (c < FONT5X7_MIN))
  {
    return;
  }

  x = graphics_cursor_x;
  y = graphics_cursor_y;

/* adjust c to skip non-printing characters */
  c -= 0x20;

/* this loop is for each of the bytes making up the font */
  for (i = 0; i < 5; i++)
  {
    character = pgm_read_byte_near((PGM_P)&font5x7[c][i]);

  /* this loop repeats for each column of pixels */
    for(j = 0; j < graphics_text_size; j++)
    {
    /* this loop repeats for each bit in the byte from the font5x7 array */
      for(k = 0; k < 8; k++)
      {
      /* this loop repeats for each column pixel */
        for(l = 0; l < graphics_text_size; l++)
        {
          if((character & 0x01) == 0)
          {
            plot_pixel(x, y, bg_colour);
          }
          else
          {
            plot_pixel(x, y, fg_colour);

          }/* end if((character & 0x01) == 0) */

          switch(graphics_rotation)
          {
            case GRAPHICS_ROTATION_0:
              y++;
              break;
            case GRAPHICS_ROTATION_90:
              x--;
              break;
            case GRAPHICS_ROTATION_180:
              y--;
              break;
            case GRAPHICS_ROTATION_270:
              x++;
              break;
            default:
              break;
          }/* end switch(graphics_rotation) */

        }/* end for(l = 0; l < graphics_text_size; l++) */

      /* rotate the character to the next pixel */
        character >>= 1;

      }/* end for(k = 0; k < 8; k++) */

    /* refresh the character after rotation */
      character = pgm_read_byte_near((PGM_P)&font5x7[c][i]);

      switch(graphics_rotation)
      {
        case GRAPHICS_ROTATION_0:
          x++;
          y = graphics_cursor_y;
          break;
        case GRAPHICS_ROTATION_90:
          y++;
          x = graphics_cursor_x;
          break;
        case GRAPHICS_ROTATION_180:
          x--;
          y = graphics_cursor_y;
          break;
        case GRAPHICS_ROTATION_270:
          y--;
          x = graphics_cursor_x;
          break;
          default:
            break;
      }/* end switch(graphics_rotation) */

    }/* end for(j = 0; j < graphics_text_size; j++) */

  }/* end for (i = 0; i < 5; i++) */

/* draw a column of blank pixels to the right of the character for separation */
  for(i = 0; i < graphics_text_size; i++)
  {
    switch(graphics_rotation)
    {
      case GRAPHICS_ROTATION_0:
      case GRAPHICS_ROTATION_180:
        y = graphics_cursor_y;
        break;
      case GRAPHICS_ROTATION_90:
      case GRAPHICS_ROTATION_270:
        x = graphics_cursor_x;
        break;
      default:
        break;
    }/* end switch(graphics_rotation) */

    for(j = 0; j < (8 * graphics_text_size); j++)
    {
      plot_pixel(x, y, bg_colour);

      switch(graphics_rotation)
      {
        case GRAPHICS_ROTATION_0:
          y++;
          break;
        case GRAPHICS_ROTATION_90:
          x--;
          break;
        case GRAPHICS_ROTATION_180:
          y--;
          break;
        case GRAPHICS_ROTATION_270:
          x++;
          break;
        default:
          break;
      }/* end switch(graphics_rotation) */

    }/* end for(j = 0; j < (8 * graphics_text_size); j++) */

    switch(graphics_rotation)
    {
      case GRAPHICS_ROTATION_0:
        x++;
        break;
      case GRAPHICS_ROTATION_90:
        y++;
        break;
      case GRAPHICS_ROTATION_180:
        x--;
        break;
      case GRAPHICS_ROTATION_270:
        y--;
        break;
      default:
        break;
    }/* end switch(graphics_rotation) */

  }/* end for(i = 0; i < graphics_text_size; i++) */

/* advance the x pixel location in anticipation of writing a string */
  switch(graphics_rotation)
  {
    case GRAPHICS_ROTATION_0:
    case GRAPHICS_ROTATION_180:
      graphics_cursor_x = x;
      break;
    case GRAPHICS_ROTATION_90:
    case GRAPHICS_ROTATION_270:
      graphics_cursor_y = y;
      break;
    default:
      break;
  }/* end switch(graphics_rotation) */

}/* end graphics_putChar() */

/*
 * graphics_putStr()
 *
 * Send a null terminated text string stored in RAM to the graphics buffer at
 * the position set by graphics_set_graphics_cursor() with rotation set by
 * set_graphics_rotation().
 */
void graphics_putStr(char str[])
{
  char c;
  uint8_t i = 0;

  while ((c = str[i++]) != 0)
  {
    graphics_putChar(c);
  }

}/* end graphics_putStr() */

/*
 * graphics_putStrP()
 *
 * Draw a null terminated text string stored in program memory to the graphics
 * buffer at the positiion set by set_graphics_cursor() with rotation set by
 * set_graphics_rotation().
 */
void graphics_putStrP(const char *addr)
{
  char c;

  while ((c = pgm_read_byte(addr++)) != 0)
  {
    graphics_putChar(c);
  }

}/* end graphics_putStrP() */

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
void graphics_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  uint8_t steep;
  int16_t dx, dy,
          err,
          ystep;

  if(abs(y1 - y0) > abs(x1 - x0))
  {
    steep = 1;
  }
  else
  {
    steep = 0;
  }/* end if(abs(y1 - y0) > abs(x1 - x0)) */

  if (steep == 1)
  {
    swap(&x0, &y0);
    swap(&x1, &y1);
  }

  if(x0 > x1)
  {
    swap(&x0, &x1);
    swap(&y0, &y1);
  }/* end if(x0 > x1) */

  dx = x1 - x0;
  dy = abs(y1 - y0);
  err = dx / 2;

  if (y0 < y1)
  {
    ystep = 1;
  }
  else
  {
    ystep = -1;
  }/* end if (y0 < y1) */

  for (; x0 <= x1; x0++)
  {
    if(steep == 1)
    {
      plot_pixel(y0, x0, fg_colour);
    }
    else
    {
      plot_pixel(x0, y0, fg_colour);
    }/* end if(steep == 1) */

    err -= dy;

    if(err < 0)
    {
      y0 += ystep;
      err += dx;
    }/* end if(err < 0) */

  }/* end for (; x0 <= x1; x0++) */

}/* end graphics_draw_line() */

/*
 * graphics_draw_circle()
 *
 * Draw a circle of radius r with center at x0, y0.
 *
 * This uses the midpoint circle algorithm and is taken directly from Wikipedea:
 *
 *  https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
 */
void graphics_draw_circle(uint8_t x0, uint8_t y0, uint8_t r)
{
  int16_t f,
          ddF_x, ddF_y,
          x, y;

  f = 1 - r;
  ddF_x = 1;
  ddF_y = -2 * r;
  x = 0;
  y = r;
  
  plot_pixel(x0, y0 + r, fg_colour);
  plot_pixel(x0, y0 - r, fg_colour);
  plot_pixel(x0 + r, y0, fg_colour);
  plot_pixel(x0 - r, y0, fg_colour);

  while(x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }/* end if (f >= 0) */

    x++;
    ddF_x += 2;
    f += ddF_x;
  
    plot_pixel(x0 + x, y0 + y, fg_colour);
    plot_pixel(x0 - x, y0 + y, fg_colour);
    plot_pixel(x0 + x, y0 - y, fg_colour);
    plot_pixel(x0 - x, y0 - y, fg_colour);
    plot_pixel(x0 + y, y0 + x, fg_colour);
    plot_pixel(x0 - y, y0 + x, fg_colour);
    plot_pixel(x0 + y, y0 - x, fg_colour);
    plot_pixel(x0 - y, y0 - x, fg_colour);

  }/* end while(x < y) */

}/* end graphics_draw_circle() */

/*
 * graphics_draw_filled_circle()
 *
 * This method doesn't draw a completely filled-in circle.  Still a work in
 * progress.
 */
void graphics_draw_filled_circle(uint8_t x0, uint8_t y0, uint8_t r)
{
  for(uint8_t i = 0; i <= r; i++)
  {
    graphics_draw_circle(x0, y0, i);
  }
}/* end graphics_draw_filled_circle() */

/*
 * graphics_draw_rectangle()
 *
 * Draw a rectangle with the coordinates given.  Each pair of coordinates is
 * an opposite corner of the rectangle.  Simply draw four lines.
 */
void graphics_draw_rectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  graphics_draw_line(x0, y0, x1, y0);
  graphics_draw_line(x1, y0, x1, y1);
  graphics_draw_line(x1, y1, x0, y1);
  graphics_draw_line(x0, y1, x0, y0);

}/* end graphics_draw_rectangle() */

/*
 * graphics_draw_filled_rectangle()
 *
 * Draw a filled rectangle with the coordinates given.  Each pair of coordinates
 * is an opposite corner of the rectangle.  Draw a series of lines from top to
 * bottom of rectangle.
 */
void graphics_draw_filled_rectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
 {
    uint8_t i, len;

    if(y0 > y1)
    {
      swap(&y0, &y1);

    }/* end if(y0 > y1) */

    len = y1 - y0;
   
    for(i = 0; i < len; i++)
    {
      graphics_draw_line(x0, (y0 + i), x1, (y0 + i));

    }/* end for(i = 0; i < len; i++) */
 
 }/* end graphics_draw_filled_rectangle() */
 

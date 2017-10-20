/*
 * File:       spi.c
 * Date:       November 19, 2014
 * Author:     Craig Hollinger
 *
 * Description:
 *
 * Simple drivers for the SPI peripheral.  Used only in SPI Master mode.
 *
 * This file is free software; you can redistribute it and/or modify it under
 * the terms of either the GNU General Public License version 3 or the GNU
 * Lesser General Public License version 3, both as published by the Free
 * Software Foundation.
 */
#include <avr/io.h>
#include "spi\spi.h"

/*
 * spi_init()
 *
 * Initialize the SPI peripheral by writing the input parameters to the control
 * and status registers.
 */
void spi_init(uint8_t spcr, uint8_t spsr)
{
/* Set SS to high so a connected chip will be "deselected" by default.
   Warning: if the SS pin ever becomes a LOW INPUT then SPI automatically
   switches to Slave, so the data direction of the SS pin MUST be kept as
   OUTPUT.

   When the SS pin is set as OUTPUT, it can be used as a general purpose output
   port (it doesn't influence SPI operations). */
  PORTB |= _BV(PORTB2);
  DDRB |= _BV(PORTB2);

/* Set direction register for SCK and MOSI pins to output. MISO pin is
   automatically overridden to INPUT by the SPI module. */
  DDRB |= _BV(PORTB5);/* SCK */
  DDRB |= _BV(PORTB3);/* MOSI */

/* update the control and status registers */
  SPCR = spcr;
  SPSR = spsr;

} /* end spi_init() */

/*
 * spi_end()
 *
 * Turn off the SPI peripheral (saves power).
 */
void spi_end()
{
  SPCR &= ~_BV(SPE);

}/* end spi_end() */

/*
 * spi_bitOrder()
 *
 * Set the bit order of SPI transmission: if order is 0, MSB first, otherwise
 * LSB first.
 */
void spi_bitOrder(uint8_t order)
{

  if(order)
  {
    SPCR |= _BV(DORD);// LSB first
  }
  else
  {
    SPCR &= ~_BV(DORD);// MSB first
  }

}/* end spi_bitOrder() */

/*
 * spi_dataMode()
 *
 * Set the data transmission SPI mode.
 */
void spi_dataMode(uint8_t mode)
{

  SPCR = ((SPCR & 0b11110011) | (mode & 0b00001100));

}/* spi_dataMode() */

/*
 * spi_clockRate()
 *
 * Set the SPI clock rate.
 */
void spi_clockRate(uint8_t rate, uint8_t speed)
{

  if(speed)
  {
    SPSR |= _BV(SPI2X);
  }
  else
  {
    SPSR &= ~_BV(SPI2X);
  }
  SPCR &= 0b11111100;
  SPCR |= (rate & 0b00000011);

}/* end spi_clockRate() */

/*
 * spi_transfer()
 *
 * Initiate an SPI transfer by putting the byte to be transmitted into the SPI
 * data register.  Return the byte received (if any).  This function blocks
 * until all 8-bits are clocked in.
 */
uint8_t spi_transfer(uint8_t data)
{

  SPDR = data;

  while ((SPSR & _BV(SPIF)) == 0);/* wait until transmission finishes */
  return SPDR;

}/* end spi_transfer() */

/*
 * spi_enableInterupt()
 *
 * Enable the SPI data transfer complete interrupt.
 */
void spi_enableInterrupt()
{

  SPCR |= _BV(SPIE);

}/* end spi_enableInterrupt() */

/*
 * spi_disableInterrupt()
 *
 * Disable the SPI data transfer complete interrupt.
 */
void spi_disableInterrupt()
{

  SPCR &= ~_BV(SPIE);

}/* end spi_disableInterrupt() */



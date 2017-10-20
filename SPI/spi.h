/*
 * File:       spi.h
 * Date:       November 19, 2014
 * Author:     Craig Hollinger
 *
 * Public interface for SPI Master library.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * or the GNU Lesser General Public License version 3, both as
 * published by the Free Software Foundation.
 */
#ifndef _SPI_H_
#define _SPI_H_ 1

/* SPI Control Register
 */
/* SPI interrupt enable */
#define SPI_SPCR_SPIE     0b10000000
/* SPI enable */
#define SPI_SPCR_SPE      0b01000000
/* SPI data order, MSB first (reset) */
#define SPI_SPCR_DORD_MSB 0b00000000
#define SPI_SPCR_DORD_LSB 0b00100000
/* SPI master/slave select */
#define SPI_SPCR_MSTR     0b00010000
#define SPI_SPCR_SLAV     0b00000000
/* SPI clock polarity and phase */
#define SPI_SPCR_MODE0    0b00000000
#define SPI_SPCR_MODE1    0b00000100
#define SPI_SPCR_MODE2    0b00001000
#define SPI_SPCR_MODE3    0b00001100
/* SPI clock rate */
#define SPI_SPCR_DIV4     0b00000000
#define SPI_SPCR_DIV16    0b00000001
#define SPI_SPCR_DIV64    0b00000010
#define SPI_SPCR_DIV128   0b00000011
/* these require SPI2X to be set */
#define SPI_SPCR_DIV2     0b00000000
#define SPI_SPCR_DIV8     0b00000001
#define SPI_SPCR_DIV32    0b00000010
#define SPI_SPCR_DIV64X   0b00000011

/* SPI Status Register */
#define SPI_SPSR_SPI1X    0b00000000
#define SPI_SPSR_SPI2X    0b00000001

#define SPI_DORD_MSB 0
#define SPI_DORD_LSB 1

#define SPI_DBLSPD_TRUE 1
#define SPI_DBLSPD_FALSE  0

uint8_t spi_transfer(uint8_t c);
void spi_enableInterrupt();
void spi_disableInterrupt();
void spi_init(uint8_t spcr, uint8_t spsr);
void spi_end();
void spi_bitOrder(uint8_t order);
void spi_dataMode(uint8_t mode);
void spi_clockRate(uint8_t rate, uint8_t speed);

#endif /* _SPI_H_ */

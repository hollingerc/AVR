/*
 * File:           i2c.h
 * Date:           December 3, 2009
 * Author:         Craig Hollinger
 *
 * Public interface for i2c.c.  
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */
#ifndef _I2C_H_
#define _I2C_H_ 1

#include <stdint.h>

void i2c_init(unsigned long i2c_speed);
uint8_t i2c_start(void);
void i2c_stop(void);
uint8_t i2c_putchar(uint8_t c);
uint8_t i2c_getchar_ack(void);
uint8_t i2c_getchar_nack(void);
uint8_t i2c_write(uint8_t SlvAdrs, uint8_t len, uint8_t adrs, uint8_t *buf);
uint8_t i2c_read(uint8_t SlvAdrs, uint8_t len, uint8_t adrs, uint8_t *buf);

#endif /* _I2C_H_ */

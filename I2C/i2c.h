/*
 * File:    i2c.h
 * Date:    May 22, 2011
 * Author:  Craig Hollinger
 *
 * Public interface for i2c.c.  
 *
 * The .c file contains driver routines to control the processor's peripheral
 * I2C interface.  All these functions are blocking, they won't return until
 * the TWI has completed it's current function, or an error has occurred.
 *
 * Some of these functions are based on ideas from example software included
 * with the WinAVR compiler.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */
#ifndef _I2C_H_
#define _I2C_H_ 1

#include <stdint.h>

/*
 * i2c_init()
 *
 * Set up the TWI module in the processor for I2C master mode.  SCL rate will 
 * be a maximum of 400kHz if the CPU frequency is > 1.6MHz.
 */
void i2c_init(unsigned long i2c_speed);

/*
 * i2c_start()
 *
 * Generate a Start condition on the I2C bus.  Returns an error code if the bus 
 * is not idle.
 */
uint8_t i2c_start(void);

/*
 * i2c_stop()
 *
 * Generate a Stop condition on the I2C bus.
 */
void i2c_stop(void);

/*
 * i2c_putchar()
 *
 * Put a byte on the I2C bus.  Returns an error code if the bus is not idle.
 */
uint8_t i2c_putchar(uint8_t c);

/*
 * i2c_getchar_ack()
 *
 * Receive a byte from then put an Acknowledge on the I2C bus.
 */
uint8_t i2c_getchar_ack(void);

/*
 * i2c_getchar_nack()
 *
 * Receive a byte from then put a Not Acknowledge on the I2C bus.
 */
uint8_t i2c_getchar_nack(void);

/*
 * i2c_write()
 *
 * Write a number of data bytes the I2C slave device.
 *
 * The steps below are followed:
 *
 *       1. Apply a Start condition on the bus
 *       2. Put the slave device address with bit-0 = 0 onto the bus
 *       3. Put the byte address to write to on the bus 
 *       4. Put a data byte onto the bus 
 *       5. Repeat step 5 for all data bytes 
 *       6. Apply a Stop condition on the bus 
 */
uint8_t i2c_write(uint8_t SlvAdrs, uint8_t len, uint8_t adrs, uint8_t *buf);

/*
 * i2c_read()
 *
 * Random read.  Read len data bytes from the I2C slave device to buf starting at
 * the device word address.
 *
 * The steps below are followed:
 *
 *   1.  Apply a Start condition on the bus
 *   2.  Put the Slave Device address with bit0 = 0 (write) onto the bus
 *   3.  Put the byte address to read from on the bus
 *   4.  Apply a Repeated Start condition on the bus
 *   5.  Receive a data byte from the bus and apply an Acknowledge
 *   6.  Repeat step 5 for all data bytes except last one
 *   7.  Receive the last data byte from the bus and apply a Not Acknowledge
 *   8.  Apply a Stop condition on the bus 
 */
uint8_t i2c_read(uint8_t SlvAdrs, uint8_t len, uint8_t adrs, uint8_t *buf);

#endif /* _I2C_H_ */

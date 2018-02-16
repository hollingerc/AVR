/*
 * pbmxkeys.h
 *
 * Created: 2015-08-09
 *  Author: Craig Hollinger
 *
 * These functions process up to four momentary contact pushbutton keys that
 * are multiplexed with another device.  This device must use the IO pins as
 * output only.  The function pbkeys_run() is called at regular intervals
 * and the IO pins are switched from output to input to look for a pressed key.
 * Prior to the function returning, the IO pins are switched back to output.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * or the GNU Lesser General Public License version 3, both as
 * published by the Free Software Foundation.
 */ 
#ifndef _PBMXKEYS_H_
#define _PBMXKEYS_H_ 1

#define PBKEYS_UPPER 0xf0
#define PBKEYS_LOWER 0x0f
#define PBKEYS_KEY1_PRSD 0b00000001
#define PBKEYS_KEY2_PRSD 0b00000010
#define PBKEYS_KEY3_PRSD 0b00000100
#define PBKEYS_KEY4_PRSD 0b00001000
#define PBKEYS_KEY1_HELD 0b00010000
#define PBKEYS_KEY2_HELD 0b00100000
#define PBKEYS_KEY3_HELD 0b01000000
#define PBKEYS_KEY4_HELD 0b10000000

void pbmxkeys_init(volatile uint8_t *keyPort, unsigned char keyMask);
void pbmxkeys_run(void);
void pbmxkeys_clear(void);
unsigned char pbmxkeys_get_flags(void);

#endif /* _PBMXKEYS_H_ */

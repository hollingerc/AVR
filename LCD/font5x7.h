/*
 * File:       font5x7.h
 * Date:       November 29, 2016
 * Author:     Craig Hollinger
 *
 * Table of data that represent the printable ASCII characters.
 * Each row is a character, each byte in the row is a column of pixels on the
 * display which produces a typical 5x8 character.  This table is stored in
 * FLASH memory in order to save RAM (480 bytes).
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * or the GNU Lesser General Public License version 3, both as
 * published by the Free Software Foundation.
 */
#ifndef _FONT5X7_H_
#define _FONT5X7_H_ 1

#define FONT5X7_MIN (0x20)
#define FONT5X7_MAX (0x7e)

extern const char font5x7[][5];

#endif /* _FONT5X7_H_ */

/*
 * PiEEPROM.h - AVR EEPROM like implementation for Raspberry Pi
 *
 * Copyright (C) 2014 Tomas Hozza <thozza@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef __PiEEPROM_H__
#define __PiEEPROM_H__ 1

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EEPROM_SIZE 1024    // 1KB of EEPROM memory

extern uint8_t _eeprom[EEPROM_SIZE];

int eeprom_is_ready();
/**
 * Loops until the eeprom is no longer busy.
 * Returns Nothing.
 */ 	 
#define eeprom_busy_wait() do {} while (!eeprom_is_ready())


/**
 * Read one byte from EEPROM address __p.
 */
uint8_t eeprom_read_byte (const uint8_t *__p);

/**
 * Read one 16-bit word (little endian) from EEPROM address __p.
 */
uint16_t eeprom_read_word (const uint16_t *__p);

/**
 * Read one 32-bit double word (little endian) from EEPROM address __p.
 */
uint32_t eeprom_read_dword (const uint32_t *__p);

/**
 * Read one float value (little endian) from EEPROM address __p.
 */
float eeprom_read_float (const float *__p);

/**
 * Read a block of __n bytes from EEPROM address __src to SRAM __dst.
 */
void eeprom_read_block (void *__dst, const void *__src, size_t __n);



/**
 * Write a byte __value to EEPROM address __p.
 */
void eeprom_write_byte (uint8_t *__p, uint8_t __value);

/**
 * Write a word __value to EEPROM address __p.
 */
void eeprom_write_word (uint16_t *__p, uint16_t __value);

/**
 * Write a 32-bit double word __value to EEPROM address __p.
 */
void eeprom_write_dword (uint32_t *__p, uint32_t __value);

/**
 * Write a float __value to EEPROM address __p.
 */
void eeprom_write_float (float *__p, float __value);

/**
 * Write a block of __n bytes to EEPROM address __dst from __src.
 * The argument order is mismatch with common functions like strcpy().
 */
void eeprom_write_block (const void *__src, void *__dst, size_t __n);



/**
 * Update a byte __value to EEPROM address __p.
 */
#define eeprom_update_byte eeprom_write_byte

/**
 * Update a word __value to EEPROM address __p.
 */
#define eeprom_update_word eeprom_write_word

/**
 * Update a 32-bit double word __value to EEPROM address __p.
 */
#define eeprom_update_dword eeprom_write_dword

/**
 * Update a float __value to EEPROM address __p.
 */
#define eeprom_update_float eeprom_write_float

/**
 * Update a block of __n bytes to EEPROM address __dst from __src.
 * The argument order is mismatch with common functions like strcpy().
 */
#define eeprom_update_block eeprom_write_block

#ifdef __cplusplus
}
#endif

#endif /* __PiEEPROM_H__ */

/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2013 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _avr_functions_h_
#define _avr_functions_h_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

void eeprom_initialize(void);
uint8_t eeprom_read_byte(const uint8_t *addr) __attribute__ ((pure));
uint16_t eeprom_read_word(const uint16_t *addr) __attribute__ ((pure));
uint32_t eeprom_read_dword(const uint32_t *addr) __attribute__ ((pure));
void eeprom_read_block(void *buf, const void *addr, uint32_t len);
void eeprom_write_byte(uint8_t *addr, uint8_t value);
void eeprom_write_word(uint16_t *addr, uint16_t value);
void eeprom_write_dword(uint32_t *addr, uint32_t value);
void eeprom_write_block(const void *buf, void *addr, uint32_t len);
int eeprom_is_ready(void);
#define eeprom_busy_wait() do {} while (!eeprom_is_ready())

static inline float eeprom_read_float(const float *addr) __attribute__((pure, always_inline, unused));
static inline float eeprom_read_float(const float *addr)
{
	union {float f; uint32_t u32;} u;
	u.u32 = eeprom_read_dword((const uint32_t *)addr);
	return u.f;
}
static inline void eeprom_write_float(float *addr, float value) __attribute__((always_inline, unused));
static inline void eeprom_write_float(float *addr, float value)
{
	union {float f; uint32_t u32;} u;
	u.f = value;
	eeprom_write_dword((uint32_t *)addr, u.u32);
}
static inline void eeprom_update_byte(uint8_t *addr, uint8_t value) __attribute__((always_inline, unused));
static inline void eeprom_update_byte(uint8_t *addr, uint8_t value)
{
	eeprom_write_byte(addr, value);
}
static inline void eeprom_update_word(uint16_t *addr, uint16_t value) __attribute__((always_inline, unused));
static inline void eeprom_update_word(uint16_t *addr, uint16_t value)
{
	eeprom_write_word(addr, value);
}
static inline void eeprom_update_dword(uint32_t *addr, uint32_t value) __attribute__((always_inline, unused));
static inline void eeprom_update_dword(uint32_t *addr, uint32_t value)
{
	eeprom_write_dword(addr, value);
}
static inline void eeprom_update_float(float *addr, float value) __attribute__((always_inline, unused));
static inline void eeprom_update_float(float *addr, float value)
{
	union {float f; uint32_t u32;} u;
	u.f = value;
	eeprom_write_dword((uint32_t *)addr, u.u32);
}
static inline void eeprom_update_block(const void *buf, void *addr, uint32_t len) __attribute__((always_inline, unused));
static inline void eeprom_update_block(const void *buf, void *addr, uint32_t len)
{
	eeprom_write_block(buf, addr, len);
}


char * ultoa(unsigned long val, char *buf, int radix);
char * ltoa(long val, char *buf, int radix);

#if defined(_NEWLIB_VERSION) && (__NEWLIB__ < 2 || __NEWLIB__ == 2 && __NEWLIB_MINOR__ < 2)
static inline char * utoa(unsigned int val, char *buf, int radix) __attribute__((always_inline, unused));
static inline char * utoa(unsigned int val, char *buf, int radix) { return ultoa(val, buf, radix); }
static inline char * itoa(int val, char *buf, int radix) __attribute__((always_inline, unused));
static inline char * itoa(int val, char *buf, int radix) { return ltoa(val, buf, radix); }
#endif

char * dtostrf(float val, int width, unsigned int precision, char *buf);


#ifdef __cplusplus
}
#endif
#endif

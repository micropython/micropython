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

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include "kinetis.h"

// uncomment to enable 9 bit formats
//#define SERIAL_9BIT_SUPPORT


#define SERIAL_7E1 0x02
#define SERIAL_7O1 0x03
#define SERIAL_8N1 0x00
#define SERIAL_8N2 0x04
#define SERIAL_8E1 0x06
#define SERIAL_8O1 0x07
#define SERIAL_7E1_RXINV 0x12
#define SERIAL_7O1_RXINV 0x13
#define SERIAL_8N1_RXINV 0x10
#define SERIAL_8N2_RXINV 0x14
#define SERIAL_8E1_RXINV 0x16
#define SERIAL_8O1_RXINV 0x17
#define SERIAL_7E1_TXINV 0x22
#define SERIAL_7O1_TXINV 0x23
#define SERIAL_8N1_TXINV 0x20
#define SERIAL_8N2_TXINV 0x24
#define SERIAL_8E1_TXINV 0x26
#define SERIAL_8O1_TXINV 0x27
#define SERIAL_7E1_RXINV_TXINV 0x32
#define SERIAL_7O1_RXINV_TXINV 0x33
#define SERIAL_8N1_RXINV_TXINV 0x30
#define SERIAL_8N2_RXINV_TXINV 0x34
#define SERIAL_8E1_RXINV_TXINV 0x36
#define SERIAL_8O1_RXINV_TXINV 0x37
#ifdef SERIAL_9BIT_SUPPORT
#define SERIAL_9N1 0x84
#define SERIAL_9E1 0x8E
#define SERIAL_9O1 0x8F
#define SERIAL_9N1_RXINV 0x94
#define SERIAL_9E1_RXINV 0x9E
#define SERIAL_9O1_RXINV 0x9F
#define SERIAL_9N1_TXINV 0xA4
#define SERIAL_9E1_TXINV 0xAE
#define SERIAL_9O1_TXINV 0xAF
#define SERIAL_9N1_RXINV_TXINV 0xB4
#define SERIAL_9E1_RXINV_TXINV 0xBE
#define SERIAL_9O1_RXINV_TXINV 0xBF
#endif
// Teensy LC and 3.5 and 3.6 Uarts have 1/2 bit stop setting
#if defined(__MK64FX512__) || defined(__MK66FX1M0__) || defined(KINETISL)
#define SERIAL_2STOP_BITS 0x100
#define SERIAL_8E2 (SERIAL_8E1 | SERIAL_2STOP_BITS)
#define SERIAL_8O2 (SERIAL_8O1 | SERIAL_2STOP_BITS)
#define SERIAL_8E2_RXINV (SERIAL_8E1_RXINV | SERIAL_2STOP_BITS)
#define SERIAL_8O2_RXINV (SERIAL_8O1_RXINV | SERIAL_2STOP_BITS)
#define SERIAL_8E2_TXINV (SERIAL_8E1_TXINV | SERIAL_2STOP_BITS)
#define SERIAL_8O2_TXINV (SERIAL_8O1_TXINV | SERIAL_2STOP_BITS)
#define SERIAL_8E2_RXINV_TXINV (SERIAL_8E1_RXINV_TXINV | SERIAL_2STOP_BITS)
#define SERIAL_8O2_RXINV_TXINV (SERIAL_8O1_RXINV_TXINV | SERIAL_2STOP_BITS)
#endif
// bit0: parity, 0=even, 1=odd
// bit1: parity, 0=disable, 1=enable
// bit2: mode, 1=9bit, 0=8bit
// bit3: mode10: 1=10bit, 0=8bit
// bit4: rxinv, 0=normal, 1=inverted
// bit5: txinv, 0=normal, 1=inverted
// bit6: unused
// bit7: actual data goes into 9th bit


#if defined(KINETISK)
#define BAUD2DIV(baud)  (((F_CPU * 2) + ((baud) >> 1)) / (baud))
#define BAUD2DIV2(baud) (((F_CPU * 2) + ((baud) >> 1)) / (baud))
#define BAUD2DIV3(baud) (((F_BUS * 2) + ((baud) >> 1)) / (baud))
#elif defined(KINETISL)

#if F_CPU <= 2000000
#define BAUD2DIV(baud)  (((F_PLL / 16 ) + ((baud) >> 1)) / (baud))
#elif F_CPU <= 16000000
#define BAUD2DIV(baud)  (((F_PLL / (F_PLL / 1000000)) + ((baud) >> 1)) / (baud))
#else
#define BAUD2DIV(baud)  (((F_PLL / 2 / 16) + ((baud) >> 1)) / (baud))
#endif

#define BAUD2DIV2(baud) (((F_BUS / 16) + ((baud) >> 1)) / (baud))
#define BAUD2DIV3(baud) (((F_BUS / 16) + ((baud) >> 1)) / (baud))
#endif


// C language implementation
//
#ifdef __cplusplus
extern "C" {
#endif
void serial_begin(uint32_t divisor);
void serial_format(uint32_t format);
void serial_end(void);
void serial_set_transmit_pin(uint8_t pin);
void serial_set_rx(uint8_t pin);
void serial_set_tx(uint8_t pin, uint8_t opendrain);
int serial_set_rts(uint8_t pin);
int serial_set_cts(uint8_t pin);
void serial_putchar(uint32_t c);
void serial_write(const void *buf, unsigned int count);
void serial_flush(void);
int serial_write_buffer_free(void);
int serial_available(void);
int serial_getchar(void);
int serial_peek(void);
void serial_clear(void);
void serial_print(const char *p);
void serial_phex(uint32_t n);
void serial_phex16(uint32_t n);
void serial_phex32(uint32_t n);

void serial2_begin(uint32_t divisor);
void serial2_format(uint32_t format);
void serial2_end(void);
void serial2_set_transmit_pin(uint8_t pin);
void serial2_set_rx(uint8_t pin);
void serial2_set_tx(uint8_t pin, uint8_t opendrain);
int serial2_set_rts(uint8_t pin);
int serial2_set_cts(uint8_t pin);
void serial2_putchar(uint32_t c);
void serial2_write(const void *buf, unsigned int count);
void serial2_flush(void);
int serial2_write_buffer_free(void);
int serial2_available(void);
int serial2_getchar(void);
int serial2_peek(void);
void serial2_clear(void);

void serial3_begin(uint32_t divisor);
void serial3_format(uint32_t format);
void serial3_end(void);
void serial3_set_transmit_pin(uint8_t pin);
void serial3_set_rx(uint8_t pin);
void serial3_set_tx(uint8_t pin, uint8_t opendrain);
int serial3_set_rts(uint8_t pin);
int serial3_set_cts(uint8_t pin);
void serial3_putchar(uint32_t c);
void serial3_write(const void *buf, unsigned int count);
void serial3_flush(void);
int serial3_write_buffer_free(void);
int serial3_available(void);
int serial3_getchar(void);
int serial3_peek(void);
void serial3_clear(void);

void serial4_begin(uint32_t divisor);
void serial4_format(uint32_t format);
void serial4_end(void);
void serial4_set_transmit_pin(uint8_t pin);
void serial4_set_rx(uint8_t pin);
void serial4_set_tx(uint8_t pin, uint8_t opendrain);
int serial4_set_rts(uint8_t pin);
int serial4_set_cts(uint8_t pin);
void serial4_putchar(uint32_t c);
void serial4_write(const void *buf, unsigned int count);
void serial4_flush(void);
int serial4_write_buffer_free(void);
int serial4_available(void);
int serial4_getchar(void);
int serial4_peek(void);
void serial4_clear(void);

void serial5_begin(uint32_t divisor);
void serial5_format(uint32_t format);
void serial5_end(void);
void serial5_set_transmit_pin(uint8_t pin);
void serial5_set_rx(uint8_t pin);
void serial5_set_tx(uint8_t pin, uint8_t opendrain);
int serial5_set_rts(uint8_t pin);
int serial5_set_cts(uint8_t pin);
void serial5_putchar(uint32_t c);
void serial5_write(const void *buf, unsigned int count);
void serial5_flush(void);
int serial5_write_buffer_free(void);
int serial5_available(void);
int serial5_getchar(void);
int serial5_peek(void);
void serial5_clear(void);

void serial6_begin(uint32_t divisor);
void serial6_format(uint32_t format);
void serial6_end(void);
void serial6_set_transmit_pin(uint8_t pin);
void serial6_set_rx(uint8_t pin);
void serial6_set_tx(uint8_t pin, uint8_t opendrain);
int serial6_set_rts(uint8_t pin);
int serial6_set_cts(uint8_t pin);
void serial6_putchar(uint32_t c);
void serial6_write(const void *buf, unsigned int count);
void serial6_flush(void);
int serial6_write_buffer_free(void);
int serial6_available(void);
int serial6_getchar(void);
int serial6_peek(void);
void serial6_clear(void);

#ifdef __cplusplus
}
#endif


// C++ interface
//
#ifdef __cplusplus
#include "Stream.h"
class HardwareSerial : public Stream
{
public:
	virtual void begin(uint32_t baud) { serial_begin(BAUD2DIV(baud)); }
	virtual void begin(uint32_t baud, uint32_t format) {
					  serial_begin(BAUD2DIV(baud));
					  serial_format(format); }
	virtual void end(void)		{ serial_end(); }
	virtual void transmitterEnable(uint8_t pin) { serial_set_transmit_pin(pin); }
	virtual void setRX(uint8_t pin) { serial_set_rx(pin); }
	virtual void setTX(uint8_t pin, bool opendrain=false) { serial_set_tx(pin, opendrain); }
	virtual bool attachRts(uint8_t pin) { return serial_set_rts(pin); }
	virtual bool attachCts(uint8_t pin) { return serial_set_cts(pin); }
	virtual int available(void)     { return serial_available(); }
	virtual int peek(void)          { return serial_peek(); }
	virtual int read(void)          { return serial_getchar(); }
	virtual void flush(void)        { serial_flush(); }
	virtual void clear(void)	{ serial_clear(); }
	virtual int availableForWrite(void) { return serial_write_buffer_free(); }
	virtual size_t write(uint8_t c) { serial_putchar(c); return 1; }
	virtual size_t write(unsigned long n)   { return write((uint8_t)n); }
	virtual size_t write(long n)            { return write((uint8_t)n); }
	virtual size_t write(unsigned int n)    { return write((uint8_t)n); }
	virtual size_t write(int n)             { return write((uint8_t)n); }
	virtual size_t write(const uint8_t *buffer, size_t size)
					{ serial_write(buffer, size); return size; }
        virtual size_t write(const char *str)	{ size_t len = strlen(str);
					  serial_write((const uint8_t *)str, len);
					  return len; }
	virtual size_t write9bit(uint32_t c)	{ serial_putchar(c); return 1; }
	operator bool()			{ return true; }
};
extern HardwareSerial Serial1;
extern void serialEvent1(void);

class HardwareSerial2 : public HardwareSerial
{
public:
	virtual void begin(uint32_t baud) { serial2_begin(BAUD2DIV2(baud)); }
	virtual void begin(uint32_t baud, uint32_t format) {
					  serial2_begin(BAUD2DIV2(baud));
					  serial2_format(format); }
	virtual void end(void)		{ serial2_end(); }
	virtual void transmitterEnable(uint8_t pin) { serial2_set_transmit_pin(pin); }
	virtual void setRX(uint8_t pin) { serial2_set_rx(pin); }
	virtual void setTX(uint8_t pin, bool opendrain=false) { serial2_set_tx(pin, opendrain); }
	virtual bool attachRts(uint8_t pin) { return serial2_set_rts(pin); }
	virtual bool attachCts(uint8_t pin) { return serial2_set_cts(pin); }
	virtual int available(void)     { return serial2_available(); }
	virtual int peek(void)          { return serial2_peek(); }
	virtual int read(void)          { return serial2_getchar(); }
	virtual void flush(void)        { serial2_flush(); }
	virtual void clear(void)	{ serial2_clear(); }
	virtual int availableForWrite(void) { return serial2_write_buffer_free(); }
	virtual size_t write(uint8_t c) { serial2_putchar(c); return 1; }
	virtual size_t write(unsigned long n)   { return write((uint8_t)n); }
	virtual size_t write(long n)            { return write((uint8_t)n); }
	virtual size_t write(unsigned int n)    { return write((uint8_t)n); }
	virtual size_t write(int n)             { return write((uint8_t)n); }
	virtual size_t write(const uint8_t *buffer, size_t size)
					{ serial2_write(buffer, size); return size; }
        virtual size_t write(const char *str)	{ size_t len = strlen(str);
					  serial2_write((const uint8_t *)str, len);
					  return len; }
	virtual size_t write9bit(uint32_t c)	{ serial2_putchar(c); return 1; }
	operator bool()			{ return true; }
};
extern HardwareSerial2 Serial2;
extern void serialEvent2(void);

class HardwareSerial3 : public HardwareSerial
{
public:
	virtual void begin(uint32_t baud) { serial3_begin(BAUD2DIV3(baud)); }
	virtual void begin(uint32_t baud, uint32_t format) {
					  serial3_begin(BAUD2DIV3(baud));
					  serial3_format(format); }
	virtual void end(void)          { serial3_end(); }
	virtual void transmitterEnable(uint8_t pin) { serial3_set_transmit_pin(pin); }
	virtual void setRX(uint8_t pin) { serial3_set_rx(pin); }
	virtual void setTX(uint8_t pin, bool opendrain=false) { serial3_set_tx(pin, opendrain); }
	virtual bool attachRts(uint8_t pin) { return serial3_set_rts(pin); }
	virtual bool attachCts(uint8_t pin) { return serial3_set_cts(pin); }
	virtual int available(void)     { return serial3_available(); }
	virtual int peek(void)          { return serial3_peek(); }
	virtual int read(void)          { return serial3_getchar(); }
	virtual void flush(void)        { serial3_flush(); }
	virtual void clear(void)	{ serial3_clear(); }
	virtual int availableForWrite(void) { return serial3_write_buffer_free(); }
	virtual size_t write(uint8_t c) { serial3_putchar(c); return 1; }
	virtual size_t write(unsigned long n)   { return write((uint8_t)n); }
	virtual size_t write(long n)            { return write((uint8_t)n); }
	virtual size_t write(unsigned int n)    { return write((uint8_t)n); }
	virtual size_t write(int n)             { return write((uint8_t)n); }
	virtual size_t write(const uint8_t *buffer, size_t size)
					{ serial3_write(buffer, size); return size; }
        virtual size_t write(const char *str)	{ size_t len = strlen(str);
					  serial3_write((const uint8_t *)str, len);
					  return len; }
	virtual size_t write9bit(uint32_t c)	{ serial3_putchar(c); return 1; }
	operator bool()			{ return true; }
};
extern HardwareSerial3 Serial3;
extern void serialEvent3(void);

class HardwareSerial4 : public HardwareSerial
{
public:
	virtual void begin(uint32_t baud) { serial4_begin(BAUD2DIV3(baud)); }
	virtual void begin(uint32_t baud, uint32_t format) {
					  serial4_begin(BAUD2DIV3(baud));
					  serial4_format(format); }
	virtual void end(void)          { serial4_end(); }
	virtual void transmitterEnable(uint8_t pin) { serial4_set_transmit_pin(pin); }
	virtual void setRX(uint8_t pin) { serial4_set_rx(pin); }
	virtual void setTX(uint8_t pin, bool opendrain=false) { serial4_set_tx(pin, opendrain); }
	virtual bool attachRts(uint8_t pin) { return serial4_set_rts(pin); }
	virtual bool attachCts(uint8_t pin) { return serial4_set_cts(pin); }
	virtual int available(void)     { return serial4_available(); }
	virtual int peek(void)          { return serial4_peek(); }
	virtual int read(void)          { return serial4_getchar(); }
	virtual void flush(void)        { serial4_flush(); }
	virtual void clear(void)	{ serial4_clear(); }
	virtual int availableForWrite(void) { return serial4_write_buffer_free(); }
	virtual size_t write(uint8_t c) { serial4_putchar(c); return 1; }
	virtual size_t write(unsigned long n)   { return write((uint8_t)n); }
	virtual size_t write(long n)            { return write((uint8_t)n); }
	virtual size_t write(unsigned int n)    { return write((uint8_t)n); }
	virtual size_t write(int n)             { return write((uint8_t)n); }
	virtual size_t write(const uint8_t *buffer, size_t size)
					{ serial4_write(buffer, size); return size; }
        virtual size_t write(const char *str)	{ size_t len = strlen(str);
					  serial4_write((const uint8_t *)str, len);
					  return len; }
	virtual size_t write9bit(uint32_t c)	{ serial4_putchar(c); return 1; }
	operator bool()			{ return true; }
};
extern HardwareSerial4 Serial4;
extern void serialEvent4(void);

class HardwareSerial5 : public HardwareSerial
{
public:
	virtual void begin(uint32_t baud) { serial5_begin(BAUD2DIV3(baud)); }
	virtual void begin(uint32_t baud, uint32_t format) {
					  serial5_begin(BAUD2DIV3(baud));
					  serial5_format(format); }
	virtual void end(void)          { serial5_end(); }
	virtual void transmitterEnable(uint8_t pin) { serial5_set_transmit_pin(pin); }
	virtual void setRX(uint8_t pin) { serial5_set_rx(pin); }
	virtual void setTX(uint8_t pin, bool opendrain=false) { serial5_set_tx(pin, opendrain); }
	virtual bool attachRts(uint8_t pin) { return serial5_set_rts(pin); }
	virtual bool attachCts(uint8_t pin) { return serial5_set_cts(pin); }
	virtual int available(void)     { return serial5_available(); }
	virtual int peek(void)          { return serial5_peek(); }
	virtual int read(void)          { return serial5_getchar(); }
	virtual void flush(void)        { serial5_flush(); }
	virtual void clear(void)	{ serial5_clear(); }
	virtual int availableForWrite(void) { return serial5_write_buffer_free(); }
	virtual size_t write(uint8_t c) { serial5_putchar(c); return 1; }
	virtual size_t write(unsigned long n)   { return write((uint8_t)n); }
	virtual size_t write(long n)            { return write((uint8_t)n); }
	virtual size_t write(unsigned int n)    { return write((uint8_t)n); }
	virtual size_t write(int n)             { return write((uint8_t)n); }
	virtual size_t write(const uint8_t *buffer, size_t size)
					{ serial5_write(buffer, size); return size; }
        virtual size_t write(const char *str)	{ size_t len = strlen(str);
					  serial5_write((const uint8_t *)str, len);
					  return len; }
	virtual size_t write9bit(uint32_t c)	{ serial5_putchar(c); return 1; }
	operator bool()			{ return true; }
};
extern HardwareSerial5 Serial5;
extern void serialEvent5(void);

class HardwareSerial6 : public HardwareSerial
{
public:
#if defined(__MK66FX1M0__)	// For LPUART just pass baud straight in. 
	virtual void begin(uint32_t baud) { serial6_begin(baud); }
	virtual void begin(uint32_t baud, uint32_t format) {
					  serial6_begin(baud);
					  serial6_format(format); }
#else
	virtual void begin(uint32_t baud) { serial6_begin(BAUD2DIV3(baud)); }
	virtual void begin(uint32_t baud, uint32_t format) {
					  serial6_begin(BAUD2DIV3(baud));
					  serial6_format(format); }
#endif
	virtual void end(void)          { serial6_end(); }
	virtual void transmitterEnable(uint8_t pin) { serial6_set_transmit_pin(pin); }
	virtual void setRX(uint8_t pin) { serial6_set_rx(pin); }
	virtual void setTX(uint8_t pin, bool opendrain=false) { serial6_set_tx(pin, opendrain); }
	virtual bool attachRts(uint8_t pin) { return serial6_set_rts(pin); }
	virtual bool attachCts(uint8_t pin) { return serial6_set_cts(pin); }
	virtual int available(void)     { return serial6_available(); }
	virtual int peek(void)          { return serial6_peek(); }
	virtual int read(void)          { return serial6_getchar(); }
	virtual void flush(void)        { serial6_flush(); }
	virtual void clear(void)	{ serial6_clear(); }
	virtual int availableForWrite(void) { return serial6_write_buffer_free(); }
	virtual size_t write(uint8_t c) { serial6_putchar(c); return 1; }
	virtual size_t write(unsigned long n)   { return write((uint8_t)n); }
	virtual size_t write(long n)            { return write((uint8_t)n); }
	virtual size_t write(unsigned int n)    { return write((uint8_t)n); }
	virtual size_t write(int n)             { return write((uint8_t)n); }
	virtual size_t write(const uint8_t *buffer, size_t size)
					{ serial6_write(buffer, size); return size; }
        virtual size_t write(const char *str)	{ size_t len = strlen(str);
					  serial6_write((const uint8_t *)str, len);
					  return len; }
	virtual size_t write9bit(uint32_t c)	{ serial6_putchar(c); return 1; }
	operator bool()			{ return true; }
};
extern HardwareSerial6 Serial6;
extern void serialEvent6(void);




#endif
#endif

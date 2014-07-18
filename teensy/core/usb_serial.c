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

#include "usb_dev.h"
#include "usb_serial.h"
#include "core_pins.h" // for yield()
//#include "HardwareSerial.h"
#include <string.h> // for memcpy()

// defined by usb_dev.h -> usb_desc.h
#if defined(CDC_STATUS_INTERFACE) && defined(CDC_DATA_INTERFACE)

uint32_t usb_cdc_line_coding[2];
volatile uint8_t usb_cdc_line_rtsdtr=0;
volatile uint8_t usb_cdc_transmit_flush_timer=0;

static usb_packet_t *rx_packet=NULL;
static usb_packet_t *tx_packet=NULL;
static volatile uint8_t tx_noautoflush=0;

#define TRANSMIT_FLUSH_TIMEOUT	5   /* in milliseconds */

// get the next character, or -1 if nothing received
int usb_serial_getchar(void)
{
	unsigned int i;
	int c;

	if (!rx_packet) {
		if (!usb_configuration) return -1;
		rx_packet = usb_rx(CDC_RX_ENDPOINT);
		if (!rx_packet) return -1;
	}
	i = rx_packet->index;
	c = rx_packet->buf[i++];
	if (i >= rx_packet->len) {
		usb_free(rx_packet);
		rx_packet = NULL;
	} else {
		rx_packet->index = i;
	}
	return c;
}

// peek at the next character, or -1 if nothing received
int usb_serial_peekchar(void)
{
	if (!rx_packet) {
		if (!usb_configuration) return -1;
		rx_packet = usb_rx(CDC_RX_ENDPOINT);
		if (!rx_packet) return -1;
	}
	if (!rx_packet) return -1;
	return rx_packet->buf[rx_packet->index];
}

// number of bytes available in the receive buffer
int usb_serial_available(void)
{
	int count;
	count = usb_rx_byte_count(CDC_RX_ENDPOINT);
	if (rx_packet) count += rx_packet->len - rx_packet->index;
	return count;
}

// read a block of bytes to a buffer
int usb_serial_read(void *buffer, uint32_t size)
{
	uint8_t *p = (uint8_t *)buffer;
	uint32_t qty, count=0;

	while (size) {
		if (!usb_configuration) break;
		if (!rx_packet) {
			rx:
			rx_packet = usb_rx(CDC_RX_ENDPOINT);
			if (!rx_packet) break;
			if (rx_packet->len == 0) {
				usb_free(rx_packet);
				goto rx;
			}
		}
		qty = rx_packet->len - rx_packet->index;
		if (qty > size) qty = size;
		memcpy(p, rx_packet->buf + rx_packet->index, qty);
		p += qty;
		count += qty;
		size -= qty;
		rx_packet->index += qty;
		if (rx_packet->index >= rx_packet->len) {
			usb_free(rx_packet);
			rx_packet = NULL;
		}
	}
	return count;
}

// discard any buffered input
void usb_serial_flush_input(void)
{
	usb_packet_t *rx;

	if (!usb_configuration) return;
	if (rx_packet) {
		usb_free(rx_packet);
		rx_packet = NULL;
	}
	while (1) {
		rx = usb_rx(CDC_RX_ENDPOINT);
		if (!rx) break;
		usb_free(rx);
	}
}

// Maximum number of transmit packets to queue so we don't starve other endpoints for memory
#define TX_PACKET_LIMIT 8

// When the PC isn't listening, how long do we wait before discarding data?  If this is
// too short, we risk losing data during the stalls that are common with ordinary desktop
// software.  If it's too long, we stall the user's program when no software is running.
#define TX_TIMEOUT_MSEC 70

#if F_CPU == 168000000
  #define TX_TIMEOUT (TX_TIMEOUT_MSEC * 1100)
#elif F_CPU == 144000000
  #define TX_TIMEOUT (TX_TIMEOUT_MSEC * 932)
#elif F_CPU == 120000000
  #define TX_TIMEOUT (TX_TIMEOUT_MSEC * 764)
#elif F_CPU == 96000000
  #define TX_TIMEOUT (TX_TIMEOUT_MSEC * 596)
#elif F_CPU == 72000000
  #define TX_TIMEOUT (TX_TIMEOUT_MSEC * 512)
#elif F_CPU == 48000000
  #define TX_TIMEOUT (TX_TIMEOUT_MSEC * 428)
#elif F_CPU == 24000000
  #define TX_TIMEOUT (TX_TIMEOUT_MSEC * 262)
#endif

// When we've suffered the transmit timeout, don't wait again until the computer
// begins accepting data.  If no software is running to receive, we'll just discard
// data as rapidly as Serial.print() can generate it, until there's something to
// actually receive it.
static uint8_t transmit_previous_timeout=0;


// transmit a character.  0 returned on success, -1 on error
int usb_serial_putchar(uint8_t c)
{
	return usb_serial_write(&c, 1);
}


int usb_serial_write(const void *buffer, uint32_t size)
{
	uint32_t len;
	uint32_t wait_count;
	const uint8_t *src = (const uint8_t *)buffer;
	uint8_t *dest;

	tx_noautoflush = 1;
	while (size > 0) {
		if (!tx_packet) {
			wait_count = 0;
			while (1) {
				if (!usb_configuration) {
					tx_noautoflush = 0;
					return -1;
				}
				if (usb_tx_packet_count(CDC_TX_ENDPOINT) < TX_PACKET_LIMIT) {
					tx_noautoflush = 1;
					tx_packet = usb_malloc();
					if (tx_packet) break;
					tx_noautoflush = 0;
				}
				if (++wait_count > TX_TIMEOUT || transmit_previous_timeout) {
					transmit_previous_timeout = 1;
					return -1;
				}
				yield();
			}
		}
		transmit_previous_timeout = 0;
		len = CDC_TX_SIZE - tx_packet->index;
		if (len > size) len = size;
		dest = tx_packet->buf + tx_packet->index;
		tx_packet->index += len;
		size -= len;
		while (len-- > 0) *dest++ = *src++;
		if (tx_packet->index >= CDC_TX_SIZE) {
			tx_packet->len = CDC_TX_SIZE;
			usb_tx(CDC_TX_ENDPOINT, tx_packet);
			tx_packet = NULL;
		}
		usb_cdc_transmit_flush_timer = TRANSMIT_FLUSH_TIMEOUT;
	}
	tx_noautoflush = 0;
	return 0;
}

void usb_serial_flush_output(void)
{
	if (!usb_configuration) return;
	tx_noautoflush = 1;
	if (tx_packet) {
		usb_cdc_transmit_flush_timer = 0;
		tx_packet->len = tx_packet->index;
		usb_tx(CDC_TX_ENDPOINT, tx_packet);
		tx_packet = NULL;
	} else {
		usb_packet_t *tx = usb_malloc();
		if (tx) {
			usb_cdc_transmit_flush_timer = 0;
			usb_tx(CDC_TX_ENDPOINT, tx);
		} else {
			usb_cdc_transmit_flush_timer = 1;
		}
	}
	tx_noautoflush = 0;
}

void usb_serial_flush_callback(void)
{
	if (tx_noautoflush) return;
	if (tx_packet) {
		tx_packet->len = tx_packet->index;
		usb_tx(CDC_TX_ENDPOINT, tx_packet);
		tx_packet = NULL;
	} else {
		usb_packet_t *tx = usb_malloc();
		if (tx) {
			usb_tx(CDC_TX_ENDPOINT, tx);
		} else {
			usb_cdc_transmit_flush_timer = 1;
		}
	}
}









#endif // CDC_STATUS_INTERFACE && CDC_DATA_INTERFACE

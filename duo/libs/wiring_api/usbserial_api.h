#ifndef _USBSERIAL_API_H
#define _USBSERIAL_API_H


void usbserial_begin(unsigned long baudrate);
void usbserial_end(void);
int usbserial_available(void);
unsigned char usbserial_read(void);
int usbserial_peek(void);
void usbserial_flush(void);
void usbserial_putc(unsigned char c);


#endif

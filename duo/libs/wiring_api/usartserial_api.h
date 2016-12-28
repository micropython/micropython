#ifndef _USARTSERIAL_API_H
#define _USARTSERIAL_API_H


void usartserial1_begin(unsigned long baudrate);
void usartserial1_end(void);
int usartserial1_availableForWrite(void);
int usartserial1_available(void);
void usartserial1_halfduplex(bool enable);
int usartserial1_peek(void);
unsigned char usartserial1_read(void);
void usartserial1_flush(void);
void usartserial1_putc(unsigned char c);
bool usartserial1_isenable(void);


void usartserial2_begin(unsigned long baudrate);
void usartserial2_end(void);
int usartserial2_availableForWrite(void);
int usartserial2_available(void);
void usartserial2_halfduplex(bool enable);
int usartserial2_peek(void);
unsigned char usartserial2_read(void);
void usartserial2_flush(void);
void usartserial2_putc(unsigned char c);
bool usartserial2_isenable(void);


#endif

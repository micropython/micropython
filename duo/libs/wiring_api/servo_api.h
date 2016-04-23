#ifndef _SERVO_API_H
#define _SERVO_API_H


bool servo_attach(uint16_t pin);
bool servo_attached(uint16_t pin);
int servo_attachedPin(uint16_t pin);
bool servo_detach(uint16_t pin);
void servo_write(uint16_t pin, int angle);
int servo_read(uint16_t pin);
void servo_writeMicroseconds(uint16_t pin, uint16_t pulseWidth);
uint16_t servo_readMicroseconds(uint16_t pin);
void servo_setTrim(uint16_t pin, int trim);


#endif

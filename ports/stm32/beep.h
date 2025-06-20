#ifndef __BEEP_H__
#define __BEEP_H__

#define BEEP_CLICK      0
#define BEEP_BEEP       1
#define BEEP_ALARM      2


#define OUT_INACT       0x00            // BELL-Output = "inactive"
#define OUT_LOW         0x55            // BELL-Output = "soll=LOW"
#define OUT_HIGH        (uint8_t)(~OUT_LOW)        // BELL-Output = "soll=high"

#include "py/mphal.h"
#include "py/runtime.h"

void beep_init(void);

// Returns OUT_
uint8_t beep_process(void);

void beep_on(uint8_t type);
void beep_off(uint8_t type);

void beep_set_params(uint16_t* params);

#endif
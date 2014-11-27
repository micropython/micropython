#include <stdio.h>
#include <stdint.h>

#include "mpconfig.h"

#include "Arduino.h"

#include MICROPY_HAL_H

uint32_t HAL_GetTick(void) {
  return millis();
}

void HAL_Delay(uint32_t Delay) {
  delay(Delay);
}

void mp_hal_set_interrupt_char(int c) {
  // The teensy 3.1 usb stack doesn't currently have the notion of generating
  // an exception when a certain character is received. That just means that
  // you can't press Control-C and get your python script to stop.
}

#include <stdio.h>
#include <stdint.h>

#include "mpconfig.h"

#include "Arduino.h"

#include MICROPY_HAL_H

uint32_t HAL_GetTick(void) {
  return micros();
}

void HAL_Delay(uint32_t Delay) {
  delay(Delay);
}

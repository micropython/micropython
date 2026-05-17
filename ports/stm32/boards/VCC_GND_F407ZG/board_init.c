#include "py/mphal.h"

void VCC_GND_F407ZG_board_early_init(void) {
    // set SPI flash CS pin high
    mp_hal_pin_output(pin_C4);
    mp_hal_pin_write(pin_C4, 1);
}

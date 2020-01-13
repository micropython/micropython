#include "py/mphal.h"

void MCUDev_F407VE_board_early_init(void) {
    // set SPI flash CS pin high
    mp_hal_pin_output(pin_B0);
    mp_hal_pin_write(pin_B0, 1);
}

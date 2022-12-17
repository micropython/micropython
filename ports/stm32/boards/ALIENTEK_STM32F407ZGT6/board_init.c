#include "py/mphal.h"

void ALIENTEK_STM32_F407ZG_board_early_init(void) {
    // set SPI flash CS pin high
    mp_hal_pin_output(pin_B14);
    mp_hal_pin_write(pin_B14, 1);
}

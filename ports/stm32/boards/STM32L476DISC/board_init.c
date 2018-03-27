#include "py/mphal.h"

void STM32L476DISC_board_early_init(void) {
    // set SPI flash WP and HOLD pins high
    mp_hal_pin_output(&pin_E14);
    mp_hal_pin_output(&pin_E15);
    mp_hal_pin_write(&pin_E14, 1);
    mp_hal_pin_write(&pin_E15, 1);
}

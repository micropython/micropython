#include "py/mphal.h"

void VCC_GND_STM32H743VI_board_early_init(void) {
    // set SPI and QSPI flashes CS pin high
    mp_hal_pin_output(pin_B10);
    mp_hal_pin_write(pin_B10, 1);
    mp_hal_pin_output(pin_B12);
    mp_hal_pin_write(pin_B12, 1);
}

#include "py/mphal.h"
#include "storage.h"

#ifdef MICROPY_F469DISC_USE_SOFTSPI

// SPI flash WP and HOLD pins on STM32F469I-DISCO
#define SPIFLASH_WP                 (pin_F7)
#define SPIFLASH_HOLD               (pin_F6)

void STM32F469DISC_board_early_init(void) {
    // set SPI flash WP and HOLD pins high
    mp_hal_pin_output(SPIFLASH_WP);
    mp_hal_pin_output(SPIFLASH_HOLD);
    mp_hal_pin_write(SPIFLASH_WP, 1);
    mp_hal_pin_write(SPIFLASH_HOLD, 1);
}

#else // MICROPY_F469DISC_USE_SOFTSPI

void STM32F469DISC_board_early_init(void) {
}

#endif // MICROPY_F469DISC_USE_SOFTSPI

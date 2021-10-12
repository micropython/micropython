#include "py/mphal.h"

void board_early_init(void) {

#if !MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
    // set external SPI flash CS pin high
    mp_hal_pin_output(MICROPY_HW_SPIFLASH_CS);
    mp_hal_pin_write(MICROPY_HW_SPIFLASH_CS, 1);
#endif

}

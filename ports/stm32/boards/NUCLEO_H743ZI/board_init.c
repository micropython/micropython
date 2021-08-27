#include "py/mphal.h"

void NUCLEO_H743ZI_board_early_init(void) {
    // Turn off the USB switch
    mp_hal_pin_output(pyb_pin_OTG_FS_POWER);
    mp_hal_pin_low(pyb_pin_OTG_FS_POWER);
}

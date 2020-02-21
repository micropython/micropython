#include "py/mphal.h"

void NUCLEO_H743ZI2_board_early_init(void) {
    // Turn off the USB switch
    #define USB_FS_PWR_EN pin_D10
    mp_hal_pin_output(USB_FS_PWR_EN);
    mp_hal_pin_high(USB_FS_PWR_EN);
}

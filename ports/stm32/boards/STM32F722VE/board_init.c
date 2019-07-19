#include "py/mphal.h"

void STM32F722VE_board_early_init(void) {
    // Turn off the USB switch
    #define USB_PowerSwitchOn pin_D10
    mp_hal_pin_output(USB_PowerSwitchOn);
    mp_hal_pin_high(USB_PowerSwitchOn);
}

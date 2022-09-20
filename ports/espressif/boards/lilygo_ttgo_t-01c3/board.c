#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/board.h"

#include "components/driver/include/driver/gpio.h"

void board_init(void) {
    // Debug UART
    #ifdef DEBUG
    common_hal_never_reset_pin(&pin_GPIO20);
    common_hal_never_reset_pin(&pin_GPIO21);
    #endif
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.

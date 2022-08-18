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

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {
}

void board_deinit(void) {
}

#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/board.h"

#include "components/driver/include/driver/gpio.h"

void board_init(void) {
}

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {
}

void board_deinit(void) {
}

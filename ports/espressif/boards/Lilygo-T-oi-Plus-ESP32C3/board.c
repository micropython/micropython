#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/board.h"

#include "components/driver/include/driver/gpio.h"

void board_init(void) {
}

bool board_requests_safe_mode(void) {
    return false;
}

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    // Pull LED down on reset rather than the default up
    if (pin_number == MICROPY_HW_LED_STATUS->number) {
        gpio_config_t cfg = {
            .pin_bit_mask = BIT64(pin_number),
            .mode = GPIO_MODE_DISABLE,
            .pull_up_en = false,
            .pull_down_en = true,
            .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&cfg);
        return true;
    }
    return false;
}

void reset_board(void) {
}

void board_deinit(void) {
}

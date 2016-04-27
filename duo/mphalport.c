#include <errno.h>
#include <string.h>

#include "py/mpstate.h"
#include "py/mphal.h"
#include "stm32f2xx_hal_def.h"

// this table converts from HAL_StatusTypeDef to POSIX errno
const byte mp_hal_status_to_errno_table[4] = {
    [HAL_OK] = 0,
    [HAL_ERROR] = EIO,
    [HAL_BUSY] = EBUSY,
    [HAL_TIMEOUT] = ETIMEDOUT,
};

NORETURN void mp_hal_raise(HAL_StatusTypeDef status) {
    nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(mp_hal_status_to_errno_table[status])));
}

void mp_hal_set_interrupt_char(int c) {

}

int mp_hal_stdin_rx_chr(void) {
    return 0;
}

void mp_hal_stdout_tx_str(const char *str) {

}

void mp_hal_stdout_tx_strn(const char *str, size_t len) {

}

void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {

}

void mp_hal_gpio_clock_enable(GPIO_TypeDef *gpio) {

}

void mp_hal_gpio_config(GPIO_TypeDef *gpio, uint32_t pin, uint32_t mode, uint32_t pull, uint32_t alt) {

}

bool mp_hal_gpio_set_af(const pin_obj_t *pin, GPIO_InitTypeDef *init, uint8_t fn, uint8_t unit) {
    return true;
}

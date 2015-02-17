// We use the ST Cube HAL library for most hardware peripherals
#include <stm32f4xx_hal.h>

// Basic GPIO functions
#define GPIO_read_pin(gpio, pin)        (((gpio)->IDR >> (pin)) & 1)
#define GPIO_set_pin(gpio, pin_mask)    (((gpio)->BSRRL) = (pin_mask))
#define GPIO_clear_pin(gpio, pin_mask)  (((gpio)->BSRRH) = (pin_mask))
#define GPIO_read_output_pin(gpio, pin) (((gpio)->ODR >> (pin)) & 1)

extern const byte mp_hal_status_to_errno_table[4];

NORETURN void mp_hal_raise(HAL_StatusTypeDef status);
void mp_hal_set_interrupt_char(int c); // -1 to disable

int mp_hal_stdin_rx_chr(void);
void mp_hal_stdout_tx_str(const char *str);
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len);
void mp_hal_stdout_tx_strn_cooked(const char *str, mp_uint_t len);

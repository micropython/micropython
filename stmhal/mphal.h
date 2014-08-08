// We use the ST Cube HAL library for most hardware peripherals
#include <stm32f4xx_hal.h>

// Basic GPIO functions
#define GPIO_read_pin(gpio, pin)        (((gpio)->IDR >> (pin)) & 1)
#define GPIO_set_pin(gpio, pin_mask)    (((gpio)->BSRRL) = (pin_mask))
#define GPIO_clear_pin(gpio, pin_mask)  (((gpio)->BSRRH) = (pin_mask))
#define GPIO_read_output_pin(gpio, pin) (((gpio)->ODR >> (pin)) & 1)

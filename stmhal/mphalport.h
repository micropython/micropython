// We use the ST Cube HAL library for most hardware peripherals
#include STM32_HAL_H

// The unique id address differs per MCU.  Ideally this define should
// go in some MCU-specific header, but for now it lives here.
#if defined(MCU_SERIES_F4)
#define MP_HAL_UNIQUE_ID_ADDRESS (0x1fff7a10)
#elif defined(MCU_SERIES_F7)
#define MP_HAL_UNIQUE_ID_ADDRESS (0x1ff0f420)
#else
#error mphalport.h: Unrecognized MCU_SERIES
#endif

// Basic GPIO functions
#define GPIO_read_pin(gpio, pin)        (((gpio)->IDR >> (pin)) & 1)
#if defined(MCU_SERIES_F7)
#define GPIO_set_pin(gpio, pin_mask)    (((gpio)->BSRR) = (pin_mask))
#define GPIO_clear_pin(gpio, pin_mask)  (((gpio)->BSRR) = ((pin_mask) << 16))
#else
#define GPIO_set_pin(gpio, pin_mask)    (((gpio)->BSRRL) = (pin_mask))
#define GPIO_clear_pin(gpio, pin_mask)  (((gpio)->BSRRH) = (pin_mask))
#endif
#define GPIO_read_output_pin(gpio, pin) (((gpio)->ODR >> (pin)) & 1)

void mp_hal_gpio_clock_enable(GPIO_TypeDef *gpio);

extern const unsigned char mp_hal_status_to_errno_table[4];

NORETURN void mp_hal_raise(HAL_StatusTypeDef status);
void mp_hal_set_interrupt_char(int c); // -1 to disable

#define mp_hal_delay_ms HAL_Delay
#define mp_hal_ticks_ms HAL_GetTick

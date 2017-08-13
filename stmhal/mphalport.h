// We use the ST Cube HAL library for most hardware peripherals
#include STM32_HAL_H
#include "pin.h"

// The unique id address differs per MCU.  Ideally this define should
// go in some MCU-specific header, but for now it lives here.
#if defined(MCU_SERIES_F4)
#define MP_HAL_UNIQUE_ID_ADDRESS (0x1fff7a10)
#elif defined(MCU_SERIES_F7)
#define MP_HAL_UNIQUE_ID_ADDRESS (0x1ff0f420)
#elif defined(MCU_SERIES_L4)
#define MP_HAL_UNIQUE_ID_ADDRESS (0x1fff7590)
#else
#error mphalport.h: Unrecognized MCU_SERIES
#endif

// Basic GPIO functions
#define GPIO_read_pin(gpio, pin)        (((gpio)->IDR >> (pin)) & 1)
#if defined(MCU_SERIES_F7) || defined(MCU_SERIES_L4)
#define GPIO_set_pin(gpio, pin_mask)    (((gpio)->BSRR) = (pin_mask))
#define GPIO_clear_pin(gpio, pin_mask)  (((gpio)->BSRR) = ((pin_mask) << 16))
#else
#define GPIO_set_pin(gpio, pin_mask)    (((gpio)->BSRRL) = (pin_mask))
#define GPIO_clear_pin(gpio, pin_mask)  (((gpio)->BSRRH) = (pin_mask))
#endif
#define GPIO_read_output_pin(gpio, pin) (((gpio)->ODR >> (pin)) & 1)

void mp_hal_gpio_clock_enable(GPIO_TypeDef *gpio);
void mp_hal_gpio_config(GPIO_TypeDef *gpio, uint32_t pin, uint32_t mode, uint32_t pull, uint32_t alt);
bool mp_hal_gpio_set_af(const pin_obj_t *pin, GPIO_InitTypeDef *init, uint8_t fn, uint8_t unit);

extern const unsigned char mp_hal_status_to_errno_table[4];

NORETURN void mp_hal_raise(HAL_StatusTypeDef status);
void mp_hal_set_interrupt_char(int c); // -1 to disable

#define mp_hal_delay_ms HAL_Delay
#define mp_hal_ticks_ms HAL_GetTick

// needed for machine.I2C
#include "stmhal/systick.h"
#define mp_hal_delay_us_fast(us) sys_tick_udelay(us)

// C-level pin HAL
#include "stmhal/pin.h"
#define mp_hal_pin_obj_t pin_obj_t*
#define mp_hal_get_pin_obj(o) (pin_obj_t*)pin_find(o)
#define mp_hal_pin_open_drain(p) mp_hal_gpio_config((p)->gpio, (p)->pin, 5, 0, 0)
#define mp_hal_pin_od_low(p) GPIO_clear_pin((p)->gpio, (p)->pin_mask)
#define mp_hal_pin_od_high(p) GPIO_set_pin((p)->gpio, (p)->pin_mask)
#define mp_hal_pin_read(p) GPIO_read_pin((p)->gpio, (p)->pin)

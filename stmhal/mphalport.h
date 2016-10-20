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

extern const unsigned char mp_hal_status_to_errno_table[4];

NORETURN void mp_hal_raise(HAL_StatusTypeDef status);
void mp_hal_set_interrupt_char(int c); // -1 to disable

// timing functions

#include "stmhal/systick.h"

#define mp_hal_delay_ms HAL_Delay
#define mp_hal_delay_us(us) sys_tick_udelay(us)
#define mp_hal_delay_us_fast(us) sys_tick_udelay(us)
#define mp_hal_ticks_ms HAL_GetTick
#define mp_hal_ticks_us() sys_tick_get_microseconds()

extern bool mp_hal_ticks_cpu_enabled;
void mp_hal_ticks_cpu_enable(void);
static inline mp_uint_t mp_hal_ticks_cpu(void) {
    if (!mp_hal_ticks_cpu_enabled) {
        mp_hal_ticks_cpu_enable();
    }
    return DWT->CYCCNT;
}

// C-level pin HAL

#include "stmhal/pin.h"

#define mp_hal_pin_obj_t const pin_obj_t*
#define mp_hal_get_pin_obj(o)   pin_find(o)
#define mp_hal_pin_input(p)     mp_hal_pin_config((p), 0, 0, 0)
#define mp_hal_pin_output(p)    mp_hal_pin_config((p), 1, 0, 0)
#define mp_hal_pin_open_drain(p) mp_hal_pin_config((p), 5, 0, 0)
#if defined(MCU_SERIES_F7) || defined(MCU_SERIES_L4)
#define mp_hal_pin_high(p)      (((p)->gpio->BSRR) = (p)->pin_mask)
#define mp_hal_pin_low(p)       (((p)->gpio->BSRR) = ((p)->pin_mask << 16))
#else
#define mp_hal_pin_high(p)      (((p)->gpio->BSRRL) = (p)->pin_mask)
#define mp_hal_pin_low(p)       (((p)->gpio->BSRRH) = (p)->pin_mask)
#endif
#define mp_hal_pin_od_low(p)    mp_hal_pin_low(p)
#define mp_hal_pin_od_high(p)   mp_hal_pin_high(p)
#define mp_hal_pin_read(p)      (((p)->gpio->IDR >> (p)->pin) & 1)
#define mp_hal_pin_write(p, v)  do { if (v) { mp_hal_pin_high(p); } else { mp_hal_pin_low(p); } } while (0)

void mp_hal_gpio_clock_enable(GPIO_TypeDef *gpio);
void mp_hal_pin_config(mp_hal_pin_obj_t pin, uint32_t mode, uint32_t pull, uint32_t alt);
bool mp_hal_pin_set_af(mp_hal_pin_obj_t pin, GPIO_InitTypeDef *init, uint8_t fn, uint8_t unit);

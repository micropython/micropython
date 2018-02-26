// We use the ST Cube HAL library for most hardware peripherals
#include STM32_HAL_H
#include "pin.h"

extern const unsigned char mp_hal_status_to_errno_table[4];

NORETURN void mp_hal_raise(HAL_StatusTypeDef status);
void mp_hal_set_interrupt_char(int c); // -1 to disable

// timing functions

#include "irq.h"

#define mp_hal_quiet_timing_enter() raise_irq_pri(1)
#define mp_hal_quiet_timing_exit(irq_state) restore_irq_pri(irq_state)
#define mp_hal_delay_us_fast(us) mp_hal_delay_us(us)

extern bool mp_hal_ticks_cpu_enabled;
void mp_hal_ticks_cpu_enable(void);
static inline mp_uint_t mp_hal_ticks_cpu(void) {
    if (!mp_hal_ticks_cpu_enabled) {
        mp_hal_ticks_cpu_enable();
    }
    return DWT->CYCCNT;
}

// C-level pin HAL

#include "pin.h"

#define MP_HAL_PIN_FMT                  "%q"
#define MP_HAL_PIN_MODE_INPUT           (0)
#define MP_HAL_PIN_MODE_OUTPUT          (1)
#define MP_HAL_PIN_MODE_ALT             (2)
#define MP_HAL_PIN_MODE_ANALOG          (3)
#define MP_HAL_PIN_MODE_OPEN_DRAIN      (5)
#define MP_HAL_PIN_MODE_ALT_OPEN_DRAIN  (6)
#define MP_HAL_PIN_PULL_NONE            (GPIO_NOPULL)
#define MP_HAL_PIN_PULL_UP              (GPIO_PULLUP)
#define MP_HAL_PIN_PULL_DOWN            (GPIO_PULLDOWN)

#define mp_hal_pin_obj_t const pin_obj_t*
#define mp_hal_get_pin_obj(o)   pin_find(o)
#define mp_hal_pin_name(p)      ((p)->name)
#define mp_hal_pin_input(p)     mp_hal_pin_config((p), MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_NONE, 0)
#define mp_hal_pin_output(p)    mp_hal_pin_config((p), MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_NONE, 0)
#define mp_hal_pin_open_drain(p) mp_hal_pin_config((p), MP_HAL_PIN_MODE_OPEN_DRAIN, MP_HAL_PIN_PULL_NONE, 0)
#define mp_hal_pin_high(p)      (((p)->gpio->BSRR) = (p)->pin_mask)
#define mp_hal_pin_low(p)       (((p)->gpio->BSRR) = ((p)->pin_mask << 16))
#define mp_hal_pin_od_low(p)    mp_hal_pin_low(p)
#define mp_hal_pin_od_high(p)   mp_hal_pin_high(p)
#define mp_hal_pin_read(p)      (((p)->gpio->IDR >> (p)->pin) & 1)
#define mp_hal_pin_write(p, v)  do { if (v) { mp_hal_pin_high(p); } else { mp_hal_pin_low(p); } } while (0)

void mp_hal_gpio_clock_enable(GPIO_TypeDef *gpio);
void mp_hal_pin_config(mp_hal_pin_obj_t pin, uint32_t mode, uint32_t pull, uint32_t alt);
bool mp_hal_pin_config_alt(mp_hal_pin_obj_t pin, uint32_t mode, uint32_t pull, uint8_t fn, uint8_t unit);

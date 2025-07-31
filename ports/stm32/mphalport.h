// We use the ST Cube HAL library for most hardware peripherals
#include STM32_HAL_H
#include "pin.h"

extern uint8_t mp_hal_unique_id_address[12];

// F0-1.9.0+F4-1.16.0+F7-1.7.0+G0-1.5.1+G4-1.3.0+H7-1.6.0+L0-1.11.2+L4-1.17.0+WB-1.10.0+WL-1.1.0
#if defined(STM32F0)
#define MICROPY_PLATFORM_VERSION "HAL1.9.0"
#elif defined(STM32F4)
#define MICROPY_PLATFORM_VERSION "HAL1.16.0"
#elif defined(STM32F7)
#define MICROPY_PLATFORM_VERSION "HAL1.7.0"
#elif defined(STM32G0)
#define MICROPY_PLATFORM_VERSION "HAL1.5.1"
#elif defined(STM32G4)
#define MICROPY_PLATFORM_VERSION "HAL1.3.0"
#elif defined(STM32H5)
#define MICROPY_PLATFORM_VERSION "HAL1.0.0"
#elif defined(STM32H7)
#define MICROPY_PLATFORM_VERSION "HAL1.11.0"
#elif defined(STM32L0)
#define MICROPY_PLATFORM_VERSION "HAL1.11.2"
#elif defined(STM32L1)
#define MICROPY_PLATFORM_VERSION "HAL1.10.3"
#elif defined(STM32L4)
#define MICROPY_PLATFORM_VERSION "HAL1.17.0"
#elif defined(STM32WB)
#define MICROPY_PLATFORM_VERSION "HAL1.10.0"
#elif defined(STM32WL)
#define MICROPY_PLATFORM_VERSION "HAL1.1.0"
#endif

extern const unsigned char mp_hal_status_to_errno_table[4];

static inline int mp_hal_status_to_neg_errno(HAL_StatusTypeDef status) {
    return -mp_hal_status_to_errno_table[status];
}

NORETURN void mp_hal_raise(HAL_StatusTypeDef status);
void mp_hal_set_interrupt_char(int c); // -1 to disable

// Atomic section helpers.

#include "irq.h"

#define MICROPY_BEGIN_ATOMIC_SECTION()     disable_irq()
#define MICROPY_END_ATOMIC_SECTION(state)  enable_irq(state)

// For regular code that wants to prevent "background tasks" from running.
// These background tasks (LWIP, Bluetooth) run in PENDSV context.
#define MICROPY_PY_PENDSV_ENTER   uint32_t atomic_state = raise_irq_pri(IRQ_PRI_PENDSV);
#define MICROPY_PY_PENDSV_REENTER atomic_state = raise_irq_pri(IRQ_PRI_PENDSV);
#define MICROPY_PY_PENDSV_EXIT    restore_irq_pri(atomic_state);

// Prevent the "lwIP task" from running.
#define MICROPY_PY_LWIP_ENTER   MICROPY_PY_PENDSV_ENTER
#define MICROPY_PY_LWIP_REENTER MICROPY_PY_PENDSV_REENTER
#define MICROPY_PY_LWIP_EXIT    MICROPY_PY_PENDSV_EXIT

// Timing functions.

#if __CORTEX_M == 0
// Don't have raise_irq_pri on Cortex-M0 so keep IRQs enabled to have SysTick timing
#define mp_hal_quiet_timing_enter() (1)
#define mp_hal_quiet_timing_exit(irq_state) (void)(irq_state)
#else
#define mp_hal_quiet_timing_enter() raise_irq_pri(1)
#define mp_hal_quiet_timing_exit(irq_state) restore_irq_pri(irq_state)
#endif
#define mp_hal_delay_us_fast(us) mp_hal_delay_us(us)

void mp_hal_ticks_cpu_enable(void);
static inline mp_uint_t mp_hal_ticks_cpu(void) {
    #if __CORTEX_M == 0
    return 0;
    #else
    if (!(DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)) {
        mp_hal_ticks_cpu_enable();
    }
    return DWT->CYCCNT;
    #endif
}

// C-level pin HAL

#include "pin.h"

#define MP_HAL_PIN_FMT                  "%q"
#define MP_HAL_PIN_MODE_INPUT           (0)
#define MP_HAL_PIN_MODE_OUTPUT          (1)
#define MP_HAL_PIN_MODE_ALT             (2)
#define MP_HAL_PIN_MODE_ANALOG          (3)
#if defined(GPIO_ASCR_ASC0)
#define MP_HAL_PIN_MODE_ADC             (11)
#else
#define MP_HAL_PIN_MODE_ADC             (3)
#endif
#define MP_HAL_PIN_MODE_OPEN_DRAIN      (5)
#define MP_HAL_PIN_MODE_ALT_OPEN_DRAIN  (6)
#define MP_HAL_PIN_PULL_NONE            (GPIO_NOPULL)
#define MP_HAL_PIN_PULL_UP              (GPIO_PULLUP)
#define MP_HAL_PIN_PULL_DOWN            (GPIO_PULLDOWN)
#define MP_HAL_PIN_SPEED_LOW            (GPIO_SPEED_FREQ_LOW)
#define MP_HAL_PIN_SPEED_MEDIUM         (GPIO_SPEED_FREQ_MEDIUM)
#define MP_HAL_PIN_SPEED_HIGH           (GPIO_SPEED_FREQ_HIGH)
#define MP_HAL_PIN_SPEED_VERY_HIGH      (GPIO_SPEED_FREQ_VERY_HIGH)

#define mp_hal_pin_obj_t const machine_pin_obj_t *
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
#define mp_hal_pin_write(p, v)  ((v) ? mp_hal_pin_high(p) : mp_hal_pin_low(p))
#define mp_hal_pin_interrupt(pin, handler, trigger, hard) extint_register_pin(pin, trigger, hard, handler)

enum mp_hal_pin_interrupt_trigger {
    MP_HAL_PIN_TRIGGER_NONE,
    MP_HAL_PIN_TRIGGER_FALL = GPIO_MODE_IT_FALLING,
    MP_HAL_PIN_TRIGGER_RISE = GPIO_MODE_IT_RISING,
};

void mp_hal_gpio_clock_enable(GPIO_TypeDef *gpio);
void mp_hal_pin_config(mp_hal_pin_obj_t pin, uint32_t mode, uint32_t pull, uint32_t alt);
bool mp_hal_pin_config_alt(mp_hal_pin_obj_t pin, uint32_t mode, uint32_t pull, uint8_t fn, uint8_t unit);
void mp_hal_pin_config_speed(mp_hal_pin_obj_t pin_obj, uint32_t speed);
void extint_register_pin(const machine_pin_obj_t *pin, uint32_t mode, bool hard_irq, mp_obj_t callback_obj);

mp_obj_base_t *mp_hal_get_spi_obj(mp_obj_t spi_in);

enum {
    MP_HAL_MAC_WLAN0 = 0,
    MP_HAL_MAC_WLAN1,
    MP_HAL_MAC_BDADDR,
    MP_HAL_MAC_ETH0,
};

void mp_hal_generate_laa_mac(int idx, uint8_t buf[6]);
void mp_hal_get_mac(int idx, uint8_t buf[6]);
void mp_hal_get_mac_ascii(int idx, size_t chr_off, size_t chr_len, char *dest);

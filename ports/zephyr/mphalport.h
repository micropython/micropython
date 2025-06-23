#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "shared/runtime/interrupt_char.h"

#define MICROPY_BEGIN_ATOMIC_SECTION irq_lock
#define MICROPY_END_ATOMIC_SECTION irq_unlock

void mp_hal_init(void);
void mp_hal_wait_sem(struct k_sem *sem, uint32_t timeout_ms);

static inline mp_uint_t mp_hal_ticks_us(void) {
    return k_cyc_to_ns_floor64(k_cycle_get_32()) / 1000;
}

static inline mp_uint_t mp_hal_ticks_ms(void) {
    return k_uptime_get();
}

static inline mp_uint_t mp_hal_ticks_cpu(void) {
    // ticks_cpu() is defined as using the highest-resolution timing source
    // in the system. This is usually a CPU clock, but doesn't have to be,
    // here we just use Zephyr hi-res timer.
    return k_cycle_get_32();
}

static inline void mp_hal_delay_us(mp_uint_t delay) {
    k_busy_wait(delay);
}

static inline void mp_hal_delay_ms(mp_uint_t delay) {
    mp_hal_wait_sem(NULL, delay);
}

static inline uint64_t mp_hal_time_ns(void) {
    return k_ticks_to_ns_near64(k_uptime_ticks());
}

#define mp_hal_delay_us_fast(us)   (mp_hal_delay_us(us))

// C-level pin HAL

#include "modmachine.h"

#define MP_HAL_PIN_FMT "%u"
#define mp_hal_pin_obj_t const machine_pin_obj_t *

mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t pin_in);

static inline unsigned int mp_hal_pin_name(mp_hal_pin_obj_t pin) {
    // TODO make it include the port
    return pin->pin;
}

static inline void mp_hal_pin_input(mp_hal_pin_obj_t pin) {
    (void)gpio_pin_configure(pin->port, pin->pin, GPIO_INPUT);
}

static inline void mp_hal_pin_output(mp_hal_pin_obj_t pin) {
    if (gpio_pin_configure(pin->port, pin->pin, GPIO_OUTPUT | GPIO_INPUT) == -ENOTSUP) {
        // If GPIO_OUTPUT|GPIO_INPUT is not supported (eg frdm_k64f) then try just GPIO_OUTPUT.
        (void)gpio_pin_configure(pin->port, pin->pin, GPIO_OUTPUT);
    }
}

static inline void mp_hal_pin_open_drain(mp_hal_pin_obj_t pin) {
    (void)gpio_pin_configure(pin->port, pin->pin, GPIO_OUTPUT | GPIO_INPUT | GPIO_OPEN_DRAIN);
}

static inline int mp_hal_pin_read(mp_hal_pin_obj_t pin) {
    return gpio_pin_get_raw(pin->port, pin->pin);
}

static inline void mp_hal_pin_write(mp_hal_pin_obj_t pin, int v) {
    (void)gpio_pin_set_raw(pin->port, pin->pin, v);
}

static inline void mp_hal_pin_od_low(mp_hal_pin_obj_t pin) {
    (void)gpio_pin_set_raw(pin->port, pin->pin, 0);
}

static inline void mp_hal_pin_od_high(mp_hal_pin_obj_t pin) {
    (void)gpio_pin_set_raw(pin->port, pin->pin, 1);
}

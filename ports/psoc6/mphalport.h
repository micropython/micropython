// std includes
#include <stdio.h>
#include <unistd.h>


// micropython includes
#include "py/mpconfig.h"
#include "py/runtime.h"


// MTB includes
#include "cyhal.h"


// port-specific includes

#define MICROPY_BEGIN_ATOMIC_SECTION()      (0)
#define MICROPY_END_ATOMIC_SECTION(state)   {(void)state;}

#define MP_HAL_PIN_FMT   "%u"
#define mp_hal_pin_obj_t uint

#define mp_hal_delay_us_fast mp_hal_delay_us


extern __attribute__((weak)) int _write(int fd, const char *ptr, int len);


void mp_hal_delay_us(mp_uint_t us);
void mp_hal_delay_us_fast(mp_uint_t us);
void mp_hal_delay_ms(mp_uint_t ms);

uint64_t mp_hal_time_ns(void);

mp_uint_t mp_hal_ticks_us(void);
mp_uint_t mp_hal_ticks_ms(void);
mp_uint_t mp_hal_ticks_cpu(void);


void mp_hal_pin_od_low(mp_hal_pin_obj_t pin);
void mp_hal_pin_od_high(mp_hal_pin_obj_t pin);
void mp_hal_pin_open_drain(mp_hal_pin_obj_t pin);


int mp_hal_pin_read(mp_hal_pin_obj_t pin);
uint8_t mp_hal_pin_name(mp_hal_pin_obj_t pin);
mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t obj);

void mp_hal_pin_write(mp_hal_pin_obj_t pin, uint8_t polarity);
void mp_hal_pin_output(mp_hal_pin_obj_t pin);
void mp_hal_pin_input(mp_hal_pin_obj_t pin);

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags);

int mp_hal_stdin_rx_chr(void);

mp_uint_t begin_atomic_section();
void end_atomic_section(mp_uint_t state);


static inline void mp_hal_pin_config(mp_hal_pin_obj_t pin, uint32_t mode, uint32_t pull, uint32_t alt) {
    printf("mp_hal_pin_config %d   mode : %ld   pull : %ld   alt : %ld\n", pin, mode, pull, alt);
    cyhal_gpio_configure(pin, mode, pull);

    // assert((mode == MP_HAL_PIN_MODE_INPUT || mode == MP_HAL_PIN_MODE_OUTPUT) && alt == 0);
    // gpio_set_dir(pin, mode);
    // gpio_set_pulls(pin, pull == MP_HAL_PIN_PULL_UP, pull == MP_HAL_PIN_PULL_DOWN);
}

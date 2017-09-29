#include <string.h>

#include "lib/mp-readline/readline.h"
#include "lib/utils/interrupt_char.h"
#include "py/mphal.h"
#include "py/mpstate.h"
#include "py/runtime.h"
#include "py/smallint.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/time/__init__.h"
#include "supervisor/shared/autoreload.h"

#include "hal/include/hal_atomic.h"
#include "hal/include/hal_delay.h"
#include "hal/include/hal_gpio.h"
#include "hal/include/hal_sleep.h"

#include "mpconfigboard.h"
#include "mphalport.h"
#include "reset.h"
#include "tick.h"
#include "usb.h"

extern struct usart_module usart_instance;

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        #ifdef MICROPY_VM_HOOK_LOOP
            MICROPY_VM_HOOK_LOOP
        #endif
        // if (reload_next_character) {
        //     return CHAR_CTRL_D;
        // }
        if (usb_bytes_available()) {
            #ifdef MICROPY_HW_LED_RX
            gpio_toggle_pin_level(MICROPY_HW_LED_RX);
            #endif
            return usb_read();
        }
    }
}

void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    #ifdef MICROPY_HW_LED_TX
    gpio_toggle_pin_level(MICROPY_HW_LED_TX);
    #endif

    usb_write(str, len);
}

void mp_hal_delay_ms(mp_uint_t delay) {
    uint64_t start_tick = ticks_ms;
    uint64_t duration = 0;
    while (duration < delay) {
        #ifdef MICROPY_VM_HOOK_LOOP
            MICROPY_VM_HOOK_LOOP
        #endif
        // Check to see if we've been CTRL-Ced by autoreload or the user.
        if(MP_STATE_VM(mp_pending_exception) == MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_kbd_exception))) {
            break;
        }
        duration = (ticks_ms - start_tick);
        // TODO(tannewt): Go to sleep for a little while while we wait.
    }
}

void mp_hal_delay_us(mp_uint_t delay) {
    tick_delay(delay);
}

void mp_hal_disable_all_interrupts(void) {
    common_hal_mcu_disable_interrupts();
}

void mp_hal_enable_all_interrupts(void) {
    common_hal_mcu_enable_interrupts();
}

/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024-2025 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"

#if MICROPY_PY_NETWORK_CYW43

#include "lib/cyw43-driver/src/cyw43.h"
#include "pico/unique_id.h"

// Provided by mpnetworkport.c.
void cyw43_irq_init(void);
void cyw43_irq_deinit(void);

#if CYW43_PIN_WL_DYNAMIC
// Provided by the pico-sdk (cyw43_bus_pio_spi.c).
int cyw43_set_pins_wl(uint pins[CYW43_PIN_INDEX_WL_COUNT]);
#endif

#if CYW43_PIO_CLOCK_DIV_DYNAMIC
// Provided by the pico-sdk (cyw43_bus_pio_spi.c).
void cyw43_set_pio_clkdiv_int_frac8(uint32_t clock_div_int, uint8_t clock_div_frac8);
#endif

static bool cyw43_init_done;

// Bring the cyw43 chip up.  This is deferred until first use (constructing a
// WLAN, BLE or external Pin object, or a cyw43.CYW43) so that the bus pins are
// not driven until the chip is actually needed.
static void rp2_cyw43_start(void) {
    if (cyw43_init_done) {
        return;
    }
    cyw43_init(&cyw43_state);
    cyw43_irq_init();
    cyw43_post_poll_hook(); // enable the irq
    cyw43_init_done = true;

    // The MAC isn't loaded from OTP yet, so use the unique id to generate the
    // default AP ssid.
    uint8_t buf[8];
    memcpy(&buf[0], "PICO", 4);
    static const char hexchr[16] = "0123456789ABCDEF";
    pico_unique_board_id_t pid;
    pico_get_unique_board_id(&pid);
    buf[4] = hexchr[pid.id[7] >> 4];
    buf[5] = hexchr[pid.id[6] & 0xf];
    buf[6] = hexchr[pid.id[5] >> 4];
    buf[7] = hexchr[pid.id[4] & 0xf];
    cyw43_wifi_ap_set_ssid(&cyw43_state, 8, buf);
    cyw43_wifi_ap_set_auth(&cyw43_state, CYW43_AUTH_WPA2_AES_PSK);
    cyw43_wifi_ap_set_password(&cyw43_state, 8, (const uint8_t *)"picoW123");
}

// Ensure the chip is running with the current (default or previously set)
// configuration.  Called from the WLAN, BLE and external Pin constructors.
void rp2_cyw43_ensure_init(void) {
    rp2_cyw43_start();
}

// Whether the chip has been brought up.  Used to decide if the bus pins should
// be reserved from a soft reset.
bool rp2_cyw43_is_initialised(void) {
    return cyw43_init_done;
}

// The shared WLAN and BLE constructors call these weak hooks so that a chip
// enabled at build time is brought up on first use.
void network_cyw43_init(void) {
    rp2_cyw43_ensure_init();
}

void bluetooth_ble_obj_init(void) {
    rp2_cyw43_ensure_init();
}

#if CYW43_PIN_WL_DYNAMIC || CYW43_PIO_CLOCK_DIV_DYNAMIC

// cyw43.CYW43 is the single point at which the bus pins and PIO clock divisor
// used to talk to the chip are configured.  Building one on a board wired to an
// external cyw43 module (e.g. an RM2) replaces the compile-time defaults, and
// the standard network.WLAN, bluetooth.BLE and Pin('WL_GPIO...') objects then
// work as usual.

typedef struct _rp2_cyw43_obj_t {
    mp_obj_base_t base;
} rp2_cyw43_obj_t;

extern const mp_obj_type_t rp2_cyw43_type;

static const rp2_cyw43_obj_t rp2_cyw43_obj = { { &rp2_cyw43_type } };

#if CYW43_PIN_WL_DYNAMIC
// Return the gpio for a pin argument, falling back to another argument and
// finally to NUM_BANK0_GPIOS which means "leave the current pin unchanged".
static uint rp2_cyw43_arg_pin(const mp_arg_val_t *arg, const mp_arg_val_t *fallback) {
    if (arg->u_obj != MP_OBJ_NULL) {
        return mp_hal_get_pin_obj(arg->u_obj);
    }
    if (fallback != NULL && fallback->u_obj != MP_OBJ_NULL) {
        return mp_hal_get_pin_obj(fallback->u_obj);
    }
    return NUM_BANK0_GPIOS;
}

// Tear the chip down so that its bus pins can be reconfigured.  A bringup that
// failed (wrong pins, or no module attached) leaves the PIO bus claimed but
// does not satisfy cyw43_deinit() (which only runs once the chip is fully up),
// so also release the low-level bus directly.  Both are safe no-ops otherwise.
static void rp2_cyw43_stop(void) {
    if (!cyw43_init_done) {
        return;
    }
    cyw43_irq_deinit();
    cyw43_deinit(&cyw43_state);
    cyw43_ll_deinit(&cyw43_state.cyw43_ll);
    cyw43_init_done = false;
}

// Apply a set of bus pins, filling unspecified entries (NUM_BANK0_GPIOS) from
// the current configuration.  If the pins actually change the chip is stopped
// first, so this recovers a chip whose previous bringup failed and left the
// bus claimed, rather than reporting it as in use.
static void rp2_cyw43_configure_pins(uint pins[CYW43_PIN_INDEX_WL_COUNT]) {
    bool changed = false;
    for (size_t i = 0; i < CYW43_PIN_INDEX_WL_COUNT; ++i) {
        if (pins[i] == NUM_BANK0_GPIOS) {
            pins[i] = cyw43_get_pin_wl(i);
        }
        changed = changed || pins[i] != cyw43_get_pin_wl(i);
    }
    if (!changed) {
        return;
    }
    rp2_cyw43_stop();
    if (cyw43_set_pins_wl(pins) != PICO_OK) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid cyw43 pins"));
    }
}
#endif // CYW43_PIN_WL_DYNAMIC

static mp_obj_t rp2_cyw43_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum {
        #if CYW43_PIN_WL_DYNAMIC
        ARG_pin_on, ARG_pin_out, ARG_pin_in, ARG_pin_wake, ARG_pin_clock, ARG_pin_cs, ARG_pin_dat,
        #endif
        #if CYW43_PIO_CLOCK_DIV_DYNAMIC
        ARG_div_int, ARG_div_frac,
        #endif
    };
    static const mp_arg_t allowed_args[] = {
        #if CYW43_PIN_WL_DYNAMIC
        { MP_QSTR_pin_on, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_pin_out, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_pin_in, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_pin_wake, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_pin_clock, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_pin_cs, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_pin_dat, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        #endif
        #if CYW43_PIO_CLOCK_DIV_DYNAMIC
        { MP_QSTR_div_int, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_div_frac, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = 0 } },
        #endif
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Configure the pins and clock before the chip is brought up, so that the
    // bus and its host wake irq are set up on the correct pins from the start.
    #if CYW43_PIN_WL_DYNAMIC
    uint pins[CYW43_PIN_INDEX_WL_COUNT];
    pins[CYW43_PIN_INDEX_WL_REG_ON] = rp2_cyw43_arg_pin(&args[ARG_pin_on], NULL);
    pins[CYW43_PIN_INDEX_WL_DATA_OUT] = rp2_cyw43_arg_pin(&args[ARG_pin_out], &args[ARG_pin_dat]);
    pins[CYW43_PIN_INDEX_WL_DATA_IN] = rp2_cyw43_arg_pin(&args[ARG_pin_in], &args[ARG_pin_dat]);
    pins[CYW43_PIN_INDEX_WL_HOST_WAKE] = rp2_cyw43_arg_pin(&args[ARG_pin_wake], &args[ARG_pin_dat]);
    pins[CYW43_PIN_INDEX_WL_CLOCK] = rp2_cyw43_arg_pin(&args[ARG_pin_clock], NULL);
    pins[CYW43_PIN_INDEX_WL_CS] = rp2_cyw43_arg_pin(&args[ARG_pin_cs], NULL);
    rp2_cyw43_configure_pins(pins);
    #endif

    #if CYW43_PIO_CLOCK_DIV_DYNAMIC
    if (args[ARG_div_int].u_int > 0) {
        cyw43_set_pio_clkdiv_int_frac8(args[ARG_div_int].u_int, (uint8_t)args[ARG_div_frac].u_int);
    }
    #endif

    // Bring the chip up (if not already) using the configured pins and clock.
    rp2_cyw43_start();

    (void)type;
    return MP_OBJ_FROM_PTR(&rp2_cyw43_obj);
}

MP_DEFINE_CONST_OBJ_TYPE(
    rp2_cyw43_type,
    MP_QSTR_CYW43,
    MP_TYPE_FLAG_NONE,
    make_new, rp2_cyw43_make_new
    );

static const mp_rom_map_elem_t rp2_cyw43_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_cyw43) },
    { MP_ROM_QSTR(MP_QSTR_CYW43), MP_ROM_PTR(&rp2_cyw43_type) },
};
static MP_DEFINE_CONST_DICT(rp2_cyw43_module_globals, rp2_cyw43_module_globals_table);

const mp_obj_module_t rp2_cyw43_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&rp2_cyw43_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_cyw43, rp2_cyw43_module);

#endif // CYW43_PIN_WL_DYNAMIC || CYW43_PIO_CLOCK_DIV_DYNAMIC

#endif // MICROPY_PY_NETWORK_CYW43

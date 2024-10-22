#include "py/runtime.h"
#include "extmod/network_cyw43.h"
#include "extmod/modnetwork.h"
#include "lib/cyw43-driver/src/cyw43.h"
#include "pico/unique_id.h"

void cyw43_irq_deinit(void);
void cyw43_irq_init(void);

#if CYW43_PIN_WL_DYNAMIC
// Defined in cyw43_bus_pio_spi.c
int cyw43_set_pins_wl(uint pins[CYW43_PIN_INDEX_WL_COUNT]);
#endif

#if CYW43_PIO_CLOCK_DIV_DYNAMIC
// Defined in cyw43_bus_pio_spi.c
void cyw43_set_pio_clkdiv_int_frac8(uint32_t clock_div_int, uint8_t clock_div_frac8);
#endif

// enums for the arguments needed to initialise cyw43
#define CYW43_INIT_ARG_ENUM \
    ARG_pin_on, ARG_pin_out, ARG_pin_in, ARG_pin_wake, ARG_pin_clock, ARG_pin_cs, ARG_pin_dat, ARG_div_int, ARG_div_frac

// Builds an array of pins needed by cyw43_set_pins_wl from function arguments
// order defined by cyw43_pin_index_t
#if CYW43_PIN_WL_DYNAMIC
#define SET_CYW43_PIN_ARG(ARG_ENUM, DEFAULT) args[ARG_ENUM].u_obj != MP_OBJ_NULL ? mp_hal_get_pin_obj(args[ARG_ENUM].u_obj) : (DEFAULT)
#define CYW43_PIN_ARRAY \
    SET_CYW43_PIN_ARG(ARG_pin_on, NUM_BANK0_GPIOS), \
    SET_CYW43_PIN_ARG(ARG_pin_out, SET_CYW43_PIN_ARG(ARG_pin_dat, NUM_BANK0_GPIOS)), \
    SET_CYW43_PIN_ARG(ARG_pin_in, SET_CYW43_PIN_ARG(ARG_pin_dat, NUM_BANK0_GPIOS)), \
    SET_CYW43_PIN_ARG(ARG_pin_wake, SET_CYW43_PIN_ARG(ARG_pin_dat, NUM_BANK0_GPIOS)), \
    SET_CYW43_PIN_ARG(ARG_pin_clock, NUM_BANK0_GPIOS), \
    SET_CYW43_PIN_ARG(ARG_pin_cs, NUM_BANK0_GPIOS),
#else
#define CYW43_PIN_ARRAY 0
#endif

static void rp2_cyw43_init(uint *pins, uint32_t clock_div_int, uint8_t clock_div_frac8) {
    static bool cyw43_init_done;
    if (!cyw43_init_done) {
        cyw43_init(&cyw43_state);
        cyw43_irq_init();
        cyw43_post_poll_hook(); // enable the irq
        cyw43_init_done = true;
        uint8_t buf[8];
        memcpy(&buf[0], "PICO", 4);

        // Use unique id to generate the default AP ssid.
        const char hexchr[16] = "0123456789ABCDEF";
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

    #if CYW43_PIN_WL_DYNAMIC
    assert(pins);
    // Set unspecified pins
    for (int i = 0; i < CYW43_PIN_INDEX_WL_COUNT; i++) {
        if (pins[i] == NUM_BANK0_GPIOS) {
            pins[i] = cyw43_get_pin_wl(i);
        }
    }
    // check if the pins have actually changed
    for (int i = 0; i < CYW43_PIN_INDEX_WL_COUNT; i++) {
        if (pins[i] != cyw43_get_pin_wl(i)) {
            // re-initialise cyw43. This can fail if the pins are invalid (gpio base is incompatible) or the pio is in use
            int error = cyw43_set_pins_wl(pins);
            if (error == PICO_ERROR_RESOURCE_IN_USE) {
                mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("wifi in use"));
            } else if (error != PICO_OK) {
                mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("wifi pins invalid"));
            }
            cyw43_irq_deinit();
            cyw43_irq_init();
            break;
        }
    }
    #endif // CYW43_PIN_WL_DYNAMIC
    #if CYW43_PIO_CLOCK_DIV_DYNAMIC
    // set the pio clock divisor - has no effect if the pio is in use
    if (clock_div_int > 0) {
        cyw43_set_pio_clkdiv_int_frac8(clock_div_int, clock_div_frac8);
    }
    #endif // CYW43_PIO_CLOCK_DIV_DYNAMIC
}

static void cyw43_obj_init(mp_arg_val_t *args) {
    assert(args);
    enum { CYW43_INIT_ARG_ENUM };
    uint pins[] = {
        CYW43_PIN_ARRAY
    };

    uint32_t clock_div_int = 0;
    uint8_t clock_div_frac8 = 0;
    #if CYW43_PIO_CLOCK_DIV_DYNAMIC
    clock_div_int = args[ARG_div_int].u_int;
    clock_div_frac8 = (uint8_t)args[ARG_div_frac].u_int;
    #endif

    rp2_cyw43_init(pins, clock_div_int, clock_div_frac8);
}

void network_cyw43_obj_init(mp_arg_val_t *args) {
    cyw43_obj_init(args);
}

void bluetooth_ble_obj_init(mp_arg_val_t *args) {
    cyw43_obj_init(args);
}

// perform extra initialisation in machine_pin_obj_init_helper
void machine_pin_ext_obj_init(mp_arg_val_t *args) {
    cyw43_obj_init(args);
}

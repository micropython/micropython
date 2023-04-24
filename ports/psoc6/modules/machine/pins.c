// std includes
#include <stdlib.h>

// mpy include
#include "py/runtime.h"

#include "pins.h"

// Function definitions
// helper function to translate pin_name(string) into machine_pin_obj_t index.
int pin_find(mp_obj_t pin) {
    int wanted_pin = -1;
    if (mp_obj_is_small_int(pin)) {
        // Pin defined by the index of pin table
        wanted_pin = mp_obj_get_int(pin);
    } else if (mp_obj_is_str(pin)) {
        // Search by name
        size_t slen;
        const char *s = mp_obj_str_get_data(pin, &slen);
        for (int i = 0; i < MP_ARRAY_SIZE(machine_pin_obj); i++) {
            if (slen == strlen(machine_pin_obj[i].pin_name) && strncmp(s, machine_pin_obj[i].pin_name, slen) == 0) {
                wanted_pin = i;
                break;
            }
        }
    }

    if (!(0 <= wanted_pin && wanted_pin < MP_ARRAY_SIZE(machine_pin_obj))) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin: Pin not defined!"));
    }

    return wanted_pin;
}

// helper function to translate pin_name(string) into machine_pin_obj_t->pin_addr
int pin_addr_by_name(mp_obj_t pin) {
    if (mp_obj_is_str(pin)) {
        return machine_pin_obj[pin_find(pin)].pin_addr;
    } else {
        return -1; // expecting a str as input
    }
}


// Pin object instantiations on the basis of selected board (chip package)
// Generated manually w.r.t cyhal_gpio_psoc6_02_124_bga_t enum for the package present in the CYPROTO-062-4343 target
// TODO: automate this generation using a python script, same as other ports.
const machine_pin_obj_t machine_pin_obj[] = {
    {{&machine_pin_type}, PIN_P0_0, "P0_0"},
    {{&machine_pin_type}, PIN_P0_1, "P0_1"},
    {{&machine_pin_type}, PIN_P0_2, "P0_2"},
    {{&machine_pin_type}, PIN_P0_3, "P0_3"},
    {{&machine_pin_type}, PIN_P0_4, "P0_4"},
    {{&machine_pin_type}, PIN_P0_5, "P0_5"},

    {{&machine_pin_type}, PIN_P1_0, "P1_0"},
    {{&machine_pin_type}, PIN_P1_1, "P1_1"},
    {{&machine_pin_type}, PIN_P1_2, "P1_2"},
    {{&machine_pin_type}, PIN_P1_3, "P1_3"},
    {{&machine_pin_type}, PIN_P1_4, "P1_4"},
    {{&machine_pin_type}, PIN_P1_5, "P1_5"},

    {{&machine_pin_type}, PIN_P2_0, "P2_0"},
    {{&machine_pin_type}, PIN_P2_1, "P2_1"},
    {{&machine_pin_type}, PIN_P2_2, "P2_2"},
    {{&machine_pin_type}, PIN_P2_3, "P2_3"},
    {{&machine_pin_type}, PIN_P2_4, "P2_4"},
    {{&machine_pin_type}, PIN_P2_5, "P2_5"},
    {{&machine_pin_type}, PIN_P2_6, "P2_6"},
    {{&machine_pin_type}, PIN_P2_7, "P2_7"},

    {{&machine_pin_type}, PIN_P3_0, "P3_0"},
    {{&machine_pin_type}, PIN_P3_1, "P3_1"},
    {{&machine_pin_type}, PIN_P3_2, "P3_2"},
    {{&machine_pin_type}, PIN_P3_3, "P3_3"},
    {{&machine_pin_type}, PIN_P3_4, "P3_4"},
    {{&machine_pin_type}, PIN_P3_5, "P3_5"},

    {{&machine_pin_type}, PIN_P4_0, "P4_0"},
    {{&machine_pin_type}, PIN_P4_1, "P4_1"},

    {{&machine_pin_type}, PIN_P5_0, "P5_0"},
    {{&machine_pin_type}, PIN_P5_1, "P5_1"},
    {{&machine_pin_type}, PIN_P5_2, "P5_2"},
    {{&machine_pin_type}, PIN_P5_3, "P5_3"},
    {{&machine_pin_type}, PIN_P5_4, "P5_4"},
    {{&machine_pin_type}, PIN_P5_5, "P5_5"},
    {{&machine_pin_type}, PIN_P5_6, "P5_6"},
    {{&machine_pin_type}, PIN_P5_7, "P5_7"},

    {{&machine_pin_type}, PIN_P6_0, "P6_0"},
    {{&machine_pin_type}, PIN_P6_1, "P6_1"},
    {{&machine_pin_type}, PIN_P6_2, "P6_2"},
    {{&machine_pin_type}, PIN_P6_3, "P6_3"},
    {{&machine_pin_type}, PIN_P6_4, "P6_4"},
    {{&machine_pin_type}, PIN_P6_5, "P6_5"},
    {{&machine_pin_type}, PIN_P6_6, "P6_6"},
    {{&machine_pin_type}, PIN_P6_7, "P6_7"},

    {{&machine_pin_type}, PIN_P7_0, "P7_0"},
    {{&machine_pin_type}, PIN_P7_1, "P7_1"},
    {{&machine_pin_type}, PIN_P7_2, "P7_2"},
    {{&machine_pin_type}, PIN_P7_3, "P7_3"},
    {{&machine_pin_type}, PIN_P7_4, "P7_4"},
    {{&machine_pin_type}, PIN_P7_5, "P7_5"},
    {{&machine_pin_type}, PIN_P7_6, "P7_6"},
    {{&machine_pin_type}, PIN_P7_7, "P7_7"},

    {{&machine_pin_type}, PIN_P8_0, "P8_0"},
    {{&machine_pin_type}, PIN_P8_1, "P8_1"},
    {{&machine_pin_type}, PIN_P8_2, "P8_2"},
    {{&machine_pin_type}, PIN_P8_3, "P8_3"},
    {{&machine_pin_type}, PIN_P8_4, "P8_4"},
    {{&machine_pin_type}, PIN_P8_5, "P8_5"},
    {{&machine_pin_type}, PIN_P8_6, "P8_6"},
    {{&machine_pin_type}, PIN_P8_7, "P8_7"},

    {{&machine_pin_type}, PIN_P9_0, "P9_0"},
    {{&machine_pin_type}, PIN_P9_1, "P9_1"},
    {{&machine_pin_type}, PIN_P9_2, "P9_2"},
    {{&machine_pin_type}, PIN_P9_3, "P9_3"},
    {{&machine_pin_type}, PIN_P9_4, "P9_4"},
    {{&machine_pin_type}, PIN_P9_5, "P9_5"},
    {{&machine_pin_type}, PIN_P9_6, "P9_6"},
    {{&machine_pin_type}, PIN_P9_7, "P9_7"},

    {{&machine_pin_type}, PIN_P10_0, "P10_0"},
    {{&machine_pin_type}, PIN_P10_1, "P10_1"},
    {{&machine_pin_type}, PIN_P10_2, "P10_2"},
    {{&machine_pin_type}, PIN_P10_3, "P10_3"},
    {{&machine_pin_type}, PIN_P10_4, "P10_4"},
    {{&machine_pin_type}, PIN_P10_5, "P10_5"},
    {{&machine_pin_type}, PIN_P10_6, "P10_6"},
    {{&machine_pin_type}, PIN_P10_7, "P10_7"},

    {{&machine_pin_type}, PIN_P11_0, "P11_0"},
    {{&machine_pin_type}, PIN_P11_1, "P11_1"},
    {{&machine_pin_type}, PIN_P11_2, "P11_2"},
    {{&machine_pin_type}, PIN_P11_3, "P11_3"},
    {{&machine_pin_type}, PIN_P11_4, "P11_4"},
    {{&machine_pin_type}, PIN_P11_5, "P11_5"},
    {{&machine_pin_type}, PIN_P11_6, "P11_6"},
    {{&machine_pin_type}, PIN_P11_7, "P11_7"},

    {{&machine_pin_type}, PIN_P12_0, "P12_0"},
    {{&machine_pin_type}, PIN_P12_1, "P12_1"},
    {{&machine_pin_type}, PIN_P12_2, "P12_2"},
    {{&machine_pin_type}, PIN_P12_3, "P12_3"},
    {{&machine_pin_type}, PIN_P12_4, "P12_4"},
    {{&machine_pin_type}, PIN_P12_5, "P12_5"},
    {{&machine_pin_type}, PIN_P12_6, "P12_6"},
    {{&machine_pin_type}, PIN_P12_7, "P12_7"},

    {{&machine_pin_type}, PIN_P13_0, "P13_0"},
    {{&machine_pin_type}, PIN_P13_1, "P13_1"},
    {{&machine_pin_type}, PIN_P13_2, "P13_2"},
    {{&machine_pin_type}, PIN_P13_3, "P13_3"},
    {{&machine_pin_type}, PIN_P13_4, "P13_4"},
    {{&machine_pin_type}, PIN_P13_5, "P13_5"},
    {{&machine_pin_type}, PIN_P13_6, "P13_6"},
    {{&machine_pin_type}, PIN_P13_7, "P13_7"},

    {{&machine_pin_type}, PIN_USBDP, "USBDP"},
    {{&machine_pin_type}, PIN_USBDM, "USBDM"},
};

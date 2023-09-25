// std includes
#include <stdlib.h>

// mpy include
#include "py/runtime.h"

#include "pins.h"

extern const uint8_t machine_pin_num_of_cpu_pins;

MP_DEFINE_CONST_OBJ_TYPE(
    pin_cpu_pins_obj_type,
    MP_QSTR_cpu,
    MP_TYPE_FLAG_NONE,
    locals_dict, &pin_cpu_pins_locals_dict
    );

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
        for (int i = 0; i < machine_pin_num_of_cpu_pins; i++) {
            if (slen == strlen(machine_pin_obj[i].pin_name) && strncmp(s, machine_pin_obj[i].pin_name, slen) == 0) {
                wanted_pin = i;
                break;
            }
        }
    }

    if (!(0 <= wanted_pin && wanted_pin < machine_pin_num_of_cpu_pins)) {
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


#include "py/mphal.h"
#include "machine_pin_phy.h"


// Function definitions
// helper function to translate pin_name(string) into machine_pin_io_obj_t index.
int pin_find(mp_obj_t pin) {
    int wanted_pin = -1;
    if (mp_obj_is_small_int(pin)) {
        int pin_addr = mp_obj_get_int(pin);
        for (int i = 0; i < machine_pin_num_of_cpu_pins; i++) {
            if (machine_pin_phy_obj[i].addr == pin_addr) {
                wanted_pin = i;
                break;
            }
        }
    } else if (mp_obj_is_str(pin)) {
        // Search by name
        size_t slen;
        const char *s = mp_obj_str_get_data(pin, &slen);
        for (int i = 0; i < machine_pin_num_of_cpu_pins; i++) {
            if (slen == strlen(machine_pin_phy_obj[i].name) && strncmp(s, machine_pin_phy_obj[i].name, slen) == 0) {
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

mp_obj_t pin_name_by_addr(mp_obj_t pin) {
    if (mp_obj_is_int(pin)) {
        const char *name = machine_pin_phy_obj[pin_find(pin)].name;
        return mp_obj_new_str(name, strlen(name));
    } else {
        return NULL; // expecting a int as input
    }
}

// helper function to translate pin_name(string) into machine_pin_io_obj_t->pin_addr or pin_obj to machine_pin_io_obj_t->pin_addr
int pin_addr_by_name(mp_obj_t pin) {
    if (mp_obj_is_str(pin)) {
        return machine_pin_phy_obj[pin_find(pin)].addr;
    } else {
        return pin_fetch_address(pin);
    }
}

#define pin_phy_assert_null(x) { if (x == NULL) { return NULL; } }

machine_pin_phy_obj_t *pin_phy_find_by_name(mp_obj_t pin_name) {
    size_t slen;
    const char *s = mp_obj_str_get_data(pin_name, &slen);
    for (int i = 0; i < machine_pin_num_of_cpu_pins; i++) {
        if (slen == strlen(machine_pin_phy_obj[i].name) && strncmp(s, machine_pin_phy_obj[i].name, slen) == 0) {
            return &machine_pin_phy_obj[i];
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("invalid pin: Pin not defined!"));
    return NULL;
}

bool pin_phy_is_alloc(machine_pin_phy_obj_t *pin_phy) {
    if (pin_phy->allocated_func != PIN_PHY_FUNC_NONE) {
        return true;
    }
    return false;
}

machine_pin_phy_obj_t *pin_phy_alloc(mp_obj_t pin_name, machine_pin_phy_func_t func) {
    machine_pin_phy_obj_t *pin_phy = pin_phy_find_by_name(pin_name);
    pin_phy_assert_null(pin_phy);

    pin_phy->allocated_func = func;

    return pin_phy;
}

machine_pin_phy_obj_t *pin_phy_realloc(mp_obj_t pin_name, machine_pin_phy_func_t func) {
    machine_pin_phy_obj_t *pin_phy = pin_phy_find_by_name(pin_name);
    pin_phy_assert_null(pin_phy);

    if (pin_phy_is_alloc(pin_phy)) {
        pin_phy_free(pin_phy);
    }

    pin_phy->allocated_func = func;

    return pin_phy;
}

void pin_phy_free(machine_pin_phy_obj_t *pin_phy) {
    pin_phy->allocated_func = PIN_PHY_FUNC_NONE;
}


void mod_pin_phy_deinit(void) {
    for (int i = 0; i < machine_pin_num_of_cpu_pins; i++) {
        pin_phy_free(&machine_pin_phy_obj[i]);
    }
}

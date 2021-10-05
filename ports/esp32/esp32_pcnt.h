#pragma once

#include <driver/gpio.h>
#include "driver/pcnt.h"

#define _INT16_MAX (32766)
#define _INT16_MIN (-32766)

enum edgeKind {
    RAISE = 1,
    FALL = 2
};

enum clockMultiplier {
    X1 = 1,
    X2 = 2,
    X4 = 4
};

typedef struct _pcnt_PCNT_obj_t {
    mp_obj_base_t base;
    gpio_num_t aPinNumber;
    gpio_num_t bPinNumber;

    pcnt_config_t r_enc_config;
    bool attached;
    pcnt_unit_t unit;
    volatile int64_t count;
} pcnt_PCNT_obj_t;

extern int machine_pin_get_id(mp_obj_t pin_in);

#pragma once

#pragma once

#include <driver/gpio.h>
#include "driver/pcnt.h"

#define INT16_ROLL 32767 // 1000 // 32000 //

#define FILTER_MAX 1023

enum edgeKind {
    RISING = 0x1,
    FALLING = 0x2
};

#define COUNTER_UP   (-2)
#define COUNTER_DOWN (-4)

typedef struct _mp_pcnt_obj_t {
    mp_obj_base_t base;
    int unit;

    int aPinNumber;
    int bPinNumber;

    volatile int64_t counter;

    int64_t match1;
    int64_t match2;
    int64_t counter_match1;
    int64_t counter_match2;
    mp_obj_t handler_match1;
    mp_obj_t handler_match2;
    mp_obj_t handler_zero;
    int status;

    int filter;
    enum edgeKind edge; // Counter only
    int8_t x124; // Encoder only
    float scale;
} mp_pcnt_obj_t;

//extern int machine_pin_get_id(mp_obj_t pin_in);

#pragma once

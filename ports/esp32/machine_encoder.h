#ifndef MICROPY_INCLUDED_MACHINE_ENCODER_H
#define MICROPY_INCLUDED_MACHINE_ENCODER_H

// #define USE_INT64
#ifdef USE_INT64
typedef int64_t counter_t;
#else
typedef int32_t counter_t;
#endif

#define INT16_ROLL ((counter_t)32767)

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

    volatile counter_t counter;

    counter_t match1;
    mp_obj_t handler_match1;
    mp_obj_t handler_zero;
    int status;

    int filter;
    enum edgeKind edge; // Counter only
    int8_t x124; // Encoder: multiplier 124 // Counter: 0 is 'direction=' keyword used, -1 is '_src=' keyword used
} mp_pcnt_obj_t;

#endif // MICROPY_INCLUDED_MACHINE_ENCODER_H

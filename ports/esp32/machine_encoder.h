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

enum edge_bit_mask {
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

    volatile counter_t counter; // the absolute value of the counter from the 0 (in constructor) or from the set value(x) method
    volatile counter_t counter_accum; // = counter - match // relative value of counter from the absolute value to the match value;  it is updated after match events

    counter_t match; // match value
    counter_t counter_match; // (match - counter) - (match - counter) % INT16_ROLL
    mp_obj_t handler_match;
    mp_obj_t handler_zero;
    mp_obj_t handler_roll_over;
    mp_obj_t handler_roll_under;
    uint32_t event_status;

    int filter;
    enum edge_bit_mask edge; // Counter only
    int8_t x124; // Encoder: multiplier 1, 2 or 4 // Counter: 0 is 'direction=' keyword used, -1 is '_src=' keyword used
} mp_pcnt_obj_t;

#endif // MICROPY_INCLUDED_MACHINE_ENCODER_H

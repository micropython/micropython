#ifndef MICROPY_INCLUDED_MACHINE_ENCODER_H
#define MICROPY_INCLUDED_MACHINE_ENCODER_H

// #define USE_INT64
#ifdef USE_INT64
typedef int64_t counter_t;
#else
typedef int32_t counter_t;
#endif

#define INT16_ROLL 32767

#define FILTER_MAX 1023

enum edge_bit_mask {
    RISING = 0x1,
    FALLING = 0x2
};

#define COUNTER_UP   (1)
#define COUNTER_DOWN (0)

typedef struct _mp_pcnt_obj_t {
    mp_obj_base_t base;
    int unit;

    int aPinNumber;
    int bPinNumber;

    int16_t count_max; // [1..INT16_ROLL]
    int16_t count_min; // [-INT16_ROLL..0]

    volatile counter_t counter;
    volatile counter_t counter_offset;
    counter_t match;
    counter_t counter_match; // (match - total_counter) - (match - total_counter) % INT16_ROLL

    volatile uint32_t mp_irq_flags;    // user IRQ active IRQ flags is event_status
    uint32_t mp_irq_trigger;  // user IRQ trigger mask
    mp_irq_obj_t *mp_irq_obj; // user IRQ object

    int16_t filter;
    enum edge_bit_mask edge; // Counter only
    union {
        int8_t phases;                // Encoder: multiplier 1, 2 or 4
        int8_t direction_reverse_src; // Counter: -1 is '_src=Pin()' keyword used,
                                      //           1 is 'direction=Pin()' keyword used,
                                      //           0 is 'direction=Counter.UP/Counter.DOWN' constants in keyword used
    };
} mp_pcnt_obj_t;

#endif // MICROPY_INCLUDED_MACHINE_ENCODER_H

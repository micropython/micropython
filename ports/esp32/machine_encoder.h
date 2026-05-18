#ifndef MICROPY_INCLUDED_MACHINE_ENCODER_H
#define MICROPY_INCLUDED_MACHINE_ENCODER_H

#define USE_CYCLES 1 // if 1, use `cycles` and `counter_offset` to calculate the counter value. If 0, use `counter_accum` and `counter_offset` to calculate the counter value
#define USE_MAX_MIN 1 // if 1, allow user to set `count_max` and `count_min` to values other than INT16_ROLL and -INT16_ROLL
#define USE_MAX_MIN_CALLBACKS 1 // if 1, support user callbacks for max/min events
#define USE_DEFAULT_SERVICE   0 // 0 - works // 1 - increases memory usage
#define INIT_SAVE_COUNTER     0 // 1 // 0-according to MIMXRT
#define USE_FUNC_filter_ns    0
#define USE_FUNC_pause_resume 0
#define USE_FUNC_cycles       1 // pcnt.cycles() == (pcnt.value() / (pcnt.count_max - pcnt.count_min))

#define USE_INT64 0
#if USE_INT64
typedef int64_t counter_t;
#else
typedef int32_t counter_t;
#endif

#define INT16_ROLL 32767 // default for `count_max` and `count_min`
#define FILTER_MAX 1023

enum edge_bit_mask {
    RISING = 0x1,
    FALLING = 0x2
};

#define PCNT_UNIT_ZERO_CROSS_MASK 3

#define COUNTER_UP   1 // level of the direction signal for counting up, used when `direction_reverse_src` is 0
#define COUNTER_DOWN 0

typedef struct _mp_pcnt_obj_t {
    mp_obj_base_t base;

    counter_t match;
    volatile counter_t counter_offset;
    #if USE_CYCLES
    volatile int32_t cycles;
    volatile int32_t cycles_match;
    #else
    volatile counter_t counter_accum;
    volatile counter_t counter_match; // (match - total_counter) - (match - total_counter) % INT16_ROLL
    #endif

    volatile uint32_t mp_irq_flags;   // user IRQ active IRQ flags is event_status
    volatile uint32_t mp_irq_trigger; // user IRQ trigger mask
    mp_irq_obj_t *mp_irq_obj;         // user IRQ object

    int16_t count_max; // [1..INT16_ROLL]
    // int16_t count_min; // [-INT16_ROLL..-1] is -count_max

    int16_t filter;

    int8_t unit;
    int8_t aPinNumber; // phase A pin for Encoder, src pin for pulses for Counter
    int8_t bPinNumber; // phase B pin for Encoder, _src pin for pulses DOWN or direction pin for Counter

    volatile pcnt_evt_type_t schedule;
    enum edge_bit_mask edge;          // Counter only
    union {
        int8_t phases;                // Encoder: multiplier 1, 2 or 4
        int8_t direction_reverse_src; // Counter: -1 is '_src=Pin()' keyword used,
                                      //           1 is 'direction=Pin()' keyword used,
                                      //           0 is 'direction=Counter.UP/Counter.DOWN' constants in keyword used
    };
} mp_pcnt_obj_t;

#endif // MICROPY_INCLUDED_MACHINE_ENCODER_H

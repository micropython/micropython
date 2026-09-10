/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 MASSDRIVER (massdriver.space)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifdef CONFIG_MICROPY_RELATED_PINS

enum mp_related_pins_kind {
    RELATED_PINS_KIND_INVALID = 0,
    RELATED_PINS_KIND_INT = 1,
    RELATED_PINS_KIND_STR,
    RELATED_PINS_KIND_ARBITRARY,
    RELATED_PINS_KIND_MAX
};

typedef struct _mp_related_pins_int_pair_t {
    int mpy_start;
    size_t mpy_cnt;
    uint8_t zephyr_start;
    uint8_t zephyr_cnt;
} mp_related_pins_int_pair_t;

typedef struct _mp_related_pins_str_pair_t {
    /* Forced to generate live, this is too complicated to do in preprocessor */
    const char *generator;
    mp_related_pins_int_pair_t int_range;
} mp_related_pins_str_pair_t;

typedef struct _mp_related_pins_arbitrary_pair_t {
    const char *mpy;
    uint8_t zephyr;
} mp_related_pins_arbitrary_pair_t;

typedef struct _mp_related_pins_t {
    const struct device *dev;
    enum mp_related_pins_kind kind;
    size_t cnt;
    const void *pairs;
} mp_related_pins_t;

#define RELATED_PINS_INT_CHECK(node) \
    BUILD_ASSERT(DT_PROP_LEN(node, gpio_out_ranges) == DT_PROP_LEN(node, gpio_in_ranges), \
        "gpio-in-ranges length must match gpio-out-ranges length"); \
    BUILD_ASSERT((DT_PROP_LEN(node, gpio_out_ranges) & 0x1) == 0, \
        "gpio-in-ranges and gpio-out-ranges must be arrays of pairs");

#define RELATED_PINS_STR_CHECK(node) \
    BUILD_ASSERT(DT_PROP_LEN(node, gpio_out_ranges) == DT_PROP_LEN(node, gpio_in_ranges), \
        "gpio-in-ranges length must match gpio-out-ranges length"); \
    BUILD_ASSERT((DT_PROP_LEN(node, gpio_out_ranges) & 0x1) == 0, \
        "gpio-in-ranges and gpio-out-ranges must be arrays of pairs");

#define RELATED_PINS_ARBITRARY_CHECK(node) \
    BUILD_ASSERT(DT_PROP_LEN(node, gpio_strings) == DT_PROP_LEN(node, gpio_ids), \
        "gpio-strings length must match gpio-ids length");

DT_FOREACH_STATUS_OKAY(micropython_related_pins_integer_generator, RELATED_PINS_INT_CHECK)
DT_FOREACH_STATUS_OKAY(micropython_related_pins_string_generator, RELATED_PINS_STR_CHECK)
DT_FOREACH_STATUS_OKAY(micropython_related_pins_arbitrary, RELATED_PINS_ARBITRARY_CHECK)

/* Macro magic references drivers/gpio.h from Zephyr */

#define RELATED_PINS_INT_GEN_PAIRS_IMPL(node, odd_it, even_it)                                     \
    {                                                                                              \
        .mpy_start = DT_PROP_BY_IDX(node, gpio_out_ranges, even_it),                               \
        .mpy_cnt = DT_PROP_BY_IDX(node, gpio_out_ranges, odd_it),                                  \
        .zephyr_start = DT_PROP_BY_IDX(node, gpio_in_ranges, even_it),                             \
        .zephyr_cnt = DT_PROP_BY_IDX(node, gpio_in_ranges, odd_it),                                \
    },

#define RELATED_PINS_INT_GEN_PAIRS(odd_it, node)                                                   \
    IF_ENABLED(DT_PROP_HAS_IDX(node, gpio_out_ranges, odd_it),                                     \
        (RELATED_PINS_INT_GEN_PAIRS_IMPL(node, odd_it,                                             \
            GET_ARG_N(odd_it, Z_SPARSE_LIST_EVEN_NUMBERS))))

#define RELATED_PINS_INT_GEN(node)                                                                 \
    {                                                                                              \
        .dev = DEVICE_DT_GET(DT_PROP(node, gpio_device)),                                          \
        .kind = RELATED_PINS_KIND_INT,                                                             \
        .cnt = DT_PROP_LEN(node, gpio_out_ranges) / 2,                                             \
        .pairs = (mp_related_pins_int_pair_t[])                                                    \
            {                                                                                      \
                FOR_EACH_FIXED_ARG(                                                                \
                    RELATED_PINS_INT_GEN_PAIRS,                                                    \
                    (),                                                                            \
                    node,                                                                          \
                    LIST_DROP_EMPTY(Z_SPARSE_LIST_ODD_NUMBERS)                                     \
                )                                                                                  \
            },                                                                                     \
    },

#define RELATED_PINS_STR_GEN_PAIRS_IMPL(node, odd_it, even_it)                                     \
    {                                                                                              \
        .generator = DT_PROP(node, generator_expression),                                          \
        .int_range.mpy_start = DT_PROP_BY_IDX(node, gpio_out_ranges, even_it),                     \
        .int_range.mpy_cnt = DT_PROP_BY_IDX(node, gpio_out_ranges, odd_it),                        \
        .int_range.zephyr_start = DT_PROP_BY_IDX(node, gpio_in_ranges, even_it),                   \
        .int_range.zephyr_cnt = DT_PROP_BY_IDX(node, gpio_in_ranges, odd_it),                      \
    },

#define RELATED_PINS_STR_GEN_PAIRS(odd_it, node)                                                   \
    IF_ENABLED(DT_PROP_HAS_IDX(node, gpio_out_ranges, odd_it),                                     \
        (RELATED_PINS_STR_GEN_PAIRS_IMPL(node, odd_it,                                             \
            GET_ARG_N(odd_it, Z_SPARSE_LIST_EVEN_NUMBERS))))

#define RELATED_PINS_STR_GEN(node)                                                                 \
    {                                                                                              \
        .dev = DEVICE_DT_GET(DT_PROP(node, gpio_device)),                                          \
        .kind = RELATED_PINS_KIND_STR,                                                             \
        .cnt = DT_PROP_LEN(node, gpio_out_ranges) / 2,                                             \
        .pairs = (mp_related_pins_str_pair_t[])                                                    \
            {                                                                                      \
                FOR_EACH_FIXED_ARG(                                                                \
                    RELATED_PINS_STR_GEN_PAIRS,                                                    \
                    (),                                                                            \
                    node,                                                                          \
                    LIST_DROP_EMPTY(Z_SPARSE_LIST_ODD_NUMBERS)                                     \
                )                                                                                  \
            },                                                                                     \
    },

#define RELATED_PINS_ARBITRARY_GEN_PAIRS(id, node)                                                 \
    {                                                                                              \
        .mpy = DT_PROP_BY_IDX(node, gpio_strings, id),                                             \
        .zephyr = DT_PROP_BY_IDX(node, gpio_ids, id),                                              \
    },

#define RELATED_PINS_ARBITRARY_GEN(node)                                                           \
    {                                                                                              \
        .dev = DEVICE_DT_GET(DT_PROP(node, gpio_device)),                                          \
        .kind = RELATED_PINS_KIND_ARBITRARY,                                                       \
        .cnt = DT_PROP_LEN(node, gpio_strings),                                                    \
        .pairs = (mp_related_pins_arbitrary_pair_t[])                                              \
            {                                                                                      \
                LISTIFY(                                                                           \
                    DT_PROP_LEN(node, gpio_strings),                                               \
                    RELATED_PINS_ARBITRARY_GEN_PAIRS,                                              \
                    (),                                                                            \
                    node                                                                           \
                )                                                                                  \
            },                                                                                     \
    },

static const mp_related_pins_t mp_related_pins[] = {
    DT_FOREACH_STATUS_OKAY(micropython_related_pins_integer_generator, RELATED_PINS_INT_GEN)
    DT_FOREACH_STATUS_OKAY(micropython_related_pins_string_generator, RELATED_PINS_STR_GEN)
    DT_FOREACH_STATUS_OKAY(micropython_related_pins_arbitrary, RELATED_PINS_ARBITRARY_GEN)
};

static const machine_pin_obj_t machine_pin_get_related_integer(const int pin) {
    for (size_t i = 0; i < ARRAY_SIZE(mp_related_pins); i++) {
        if (mp_related_pins[i].kind == RELATED_PINS_KIND_INT) {
            for (size_t j = 0; j < mp_related_pins[i].cnt; j++) {
                const mp_related_pins_int_pair_t *pair =
                    &((mp_related_pins_int_pair_t *)mp_related_pins[i].pairs)[j];
                if (pin >= pair->mpy_start) {
                    if (pin - pair->mpy_start < pair->mpy_cnt) {
                        return (machine_pin_obj_t) {
                                .port = mp_related_pins[i].dev,
                                .pin = pair->zephyr_start + (pin - pair->mpy_start),
                        };
                    }
                }
            }
        }
    }
    return (machine_pin_obj_t) { .port = NULL };
}

static const machine_pin_obj_t machine_pin_get_related_str(const char *str) {
    for (size_t i = 0; i < ARRAY_SIZE(mp_related_pins); i++) {
        if (mp_related_pins[i].kind == RELATED_PINS_KIND_STR) {
            for (size_t j = 0; j < mp_related_pins[i].cnt; j++) {
                int pin;
                const mp_related_pins_str_pair_t *pair =
                    &((mp_related_pins_str_pair_t *)mp_related_pins[i].pairs)[j];
                int ret = sscanf(str, pair->generator, &pin);
                if (ret != 1) {
                    mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("couldn't parse pin"));
                }
                if (pin >= pair->int_range.mpy_start) {
                    if (pin - pair->int_range.mpy_start < pair->int_range.mpy_cnt) {
                        return (machine_pin_obj_t) {
                                .port = mp_related_pins[i].dev,
                                .pin = pair->int_range.zephyr_start
                                    + (pin - pair->int_range.mpy_start),
                        };
                    }
                }
            }
        } else if (mp_related_pins[i].kind == RELATED_PINS_KIND_ARBITRARY) {
            for (size_t j = 0; j < mp_related_pins[i].cnt; j++) {
                const mp_related_pins_arbitrary_pair_t *pair =
                    &((mp_related_pins_arbitrary_pair_t *)mp_related_pins[i].pairs)[j];
                int ret = strcmp(pair->mpy, str);
                if (ret == 0) {
                    return (machine_pin_obj_t) {
                            .port = mp_related_pins[i].dev,
                            .pin = pair->zephyr,
                    };
                }
            }
        }
    }
    return (machine_pin_obj_t) { .port = NULL };
}

static const machine_pin_obj_t machine_pin_get_related(mp_obj_t arg) {
    if (mp_obj_is_int(arg)) {
        return machine_pin_get_related_integer(mp_obj_get_int(arg));
    }
    return machine_pin_get_related_str(mp_obj_str_get_str(arg));
}

#endif // CONFIG_MICROPY_RELATED_PINS

/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
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

#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "nrf_gpio.h"
#include "microbitimage.h"
#include "microbitdisplay.h"
#include "iters.h"
#include "ticker.h"

#define min(a,b) (((a)<(b))?(a):(b))

void microbit_display_show(microbit_display_obj_t *display, microbit_image_obj_t *image) {
    mp_int_t w = min(imageWidth(image), 5);
    mp_int_t h = min(imageHeight(image), 5);
    mp_int_t x = 0;
    mp_int_t brightnesses = 0;
    for (; x < w; ++x) {
        mp_int_t y = 0;
        for (; y < h; ++y) {
            uint8_t pix = imageGetPixelValue(image, x, y);
            display->image_buffer[x][y] = pix;
            brightnesses |= (1 << pix);
        }
        for (; y < 5; ++y) {
            display->image_buffer[x][y] = 0;
        }
    }
    for (; x < 5; ++x) {
        for (mp_int_t y = 0; y < 5; ++y) {
            display->image_buffer[x][y] = 0;
        }
    }
    display->brightnesses = brightnesses;
}

#define DEFAULT_PRINT_SPEED 400


mp_obj_t microbit_display_show_func(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    // Cancel any animations.
    MP_STATE_PORT(async_data)[0] = NULL;
    MP_STATE_PORT(async_data)[1] = NULL;

    static const mp_arg_t show_allowed_args[] = {
        { MP_QSTR_image,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_delay,    MP_ARG_INT, {.u_int = DEFAULT_PRINT_SPEED} },
        { MP_QSTR_clear,     MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_wait,     MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_loop,     MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    // Parse the args.
    microbit_display_obj_t *self = (microbit_display_obj_t*)pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(show_allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(show_allowed_args), show_allowed_args, args);

    mp_obj_t image = args[0].u_obj;
    mp_int_t delay = args[1].u_int;
    bool clear = args[2].u_bool;
    bool wait = args[3].u_bool;
    bool loop = args[4].u_bool;

    if (mp_obj_is_str(image)) {
        // arg is a string object
        size_t len;
        const char *str = mp_obj_str_get_data(image, &len);
        if (len == 0) {
            // There are no chars; do nothing.
            return mp_const_none;
        } else if (len == 1) {
            if (!clear && !loop) {
                // A single char; convert to an image and print that.
                image = microbit_image_for_char(str[0]);
                goto single_image_immediate;
            }
        }
        image = microbit_string_facade(image);
    } else if (mp_obj_get_type(image) == &microbit_image_type) {
        if (!clear && !loop) {
            goto single_image_immediate;
        }
        image = mp_obj_new_tuple(1, &image);
    }
    // iterable:
    if (args[4].u_bool) { /*loop*/
        image = microbit_repeat_iterator(image);
    }
    microbit_display_animate(self, image, delay, clear, wait);
    return mp_const_none;

single_image_immediate:
    microbit_display_show(self, (microbit_image_obj_t *)image);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(microbit_display_show_obj, 1, microbit_display_show_func);

static uint8_t async_mode;
static mp_obj_t async_iterator = NULL;
// Record if an error occurs in async animation. Unfortunately there is no way to report this.
static volatile bool wakeup_event = false;
static mp_uint_t async_delay = 1000;
static mp_uint_t async_tick = 0;
static bool async_clear = false;


bool microbit_display_active_animation(void) {
    return async_mode == ASYNC_MODE_ANIMATION;
}

STATIC void async_stop(void) {
    async_iterator = NULL;
    async_mode = ASYNC_MODE_STOPPED;
    async_tick = 0;
    async_delay = 1000;
    async_clear = false;
    MP_STATE_PORT(async_data)[0] = NULL;
    MP_STATE_PORT(async_data)[1] = NULL;
    wakeup_event = true;
}

STATIC void wait_for_event(void) {
    while (!wakeup_event) {
        // allow CTRL-C to stop the animation
        if (MP_STATE_THREAD(mp_pending_exception) != MP_OBJ_NULL) {
            async_stop();
            return;
        }
        __WFI();
    }
    wakeup_event = false;
}

typedef struct {
    uint8_t x;
    uint8_t y;
} DisplayPoint;

#define NO_CONN 0

#define ROW_COUNT 3
#define COLUMN_COUNT 9

static const DisplayPoint display_map[COLUMN_COUNT][ROW_COUNT] = {
    {{0,0}, {4,2}, {2,4}},
    {{2,0}, {0,2}, {4,4}},
    {{4,0}, {2,2}, {0,4}},
    {{4,3}, {1,0}, {0,1}},
    {{3,3}, {3,0}, {1,1}},
    {{2,3}, {3,4}, {2,1}},
    {{1,3}, {1,4}, {3,1}},
    {{0,3}, {NO_CONN,NO_CONN}, {4,1}},
    {{1,2}, {NO_CONN,NO_CONN}, {3,2}}
};

#define MIN_COLUMN_PIN 4
#define COLUMN_PINS_MASK 0x1ff0
#define MIN_ROW_PIN 13
#define MAX_ROW_PIN 15
#define ROW_PINS_MASK 0xe000

static inline void displaySetPinsForRow(microbit_display_obj_t * p_display, uint8_t brightness) {
    if (brightness == 0) {
        nrf_gpio_port_out_clear(NRF_GPIO, COLUMN_PINS_MASK & ~p_display->pins_for_brightness[brightness]);
    } else {
        nrf_gpio_pin_set(p_display->pins_for_brightness[brightness]);
    }
}

/* This is the primary PWM driver/display driver.  It will operate on one row
 * (9 pins) per invocation.  It will turn on LEDs with maximum brightness,
 * then let the "callback" callback turn off the LEDs as appropriate for the
 * required brightness level.
 *
 * For each row
 *   Turn off all the LEDs in the previous row
 *     Set the column bits high (off)
 *     Set the row strobe low (off)
 *   Turn on all the LEDs in the current row that have maximum brightness
 *     Set the row strobe high (on)
 *     Set some/all column bits low (on)
 *   Register the PWM callback
 *   For each callback start with brightness 0
 *     If brightness 0
 *       Turn off the LEDs specified at this level
 *     Else
 *       Turn on the LEDs specified at this level
 *     If brightness max
 *       Disable the PWM callback
 *     Else
 *       Re-queue the PWM callback after the appropriate delay
 */
static void displayAdvanceRow(microbit_display_obj_t * p_display) {
    /* Clear all of the column bits */
    nrf_gpio_port_out_set(NRF_GPIO, COLUMN_PINS_MASK);
    /* Clear the strobe bit for this row */
    nrf_gpio_pin_clear(p_display->strobe_row + MIN_ROW_PIN);

    /* Move to the next row.  Before this, "this row" refers to the row
     * manipulated by the previous invocation of this function.  After this,
     * "this row" refers to the row manipulated by the current invocation of
     * this function. */
    p_display->strobe_row++;

    // Reset the row counts and bit mask when we have hit the max.
    if (p_display->strobe_row == ROW_COUNT) {
        p_display->strobe_row = 0;
    }

    // Set pin for this row.
    // Prepare row for rendering.
    for (int i = 0; i <= MAX_BRIGHTNESS; i++) {
        p_display->pins_for_brightness[i] = 0;
    }
    for (int i = 0; i < COLUMN_COUNT; i++) {
        int x = display_map[i][p_display->strobe_row].x;
        int y = display_map[i][p_display->strobe_row].y;
        int brightness = microbit_display_obj.image_buffer[x][y];
        p_display->pins_for_brightness[brightness] |= (1<<(i+MIN_COLUMN_PIN));
        (void)brightness;
    }
    /* Enable the strobe bit for this row */
    nrf_gpio_pin_set(p_display->strobe_row + MIN_ROW_PIN);
    /* Enable the column bits for all pins that need to be on. */
    nrf_gpio_port_out_clear(NRF_GPIO, p_display->pins_for_brightness[MAX_BRIGHTNESS]);
}

static const uint16_t render_timings[] =
// The scale is (approximately) exponential,
// each step is approx x1.9 greater than the previous.
{   0, // Bright, Ticks Duration, Relative power
    2,   //   1,   2,     32us,     inf
    2,   //   2,   4,     64us,     200%
    4,   //   3,   8,     128us,    200%
    7,   //   4,   15,    240us,    187%
    13,  //   5,   28,    448us,    187%
    25,  //   6,   53,    848us,    189%
    49,  //   7,   102,   1632us,   192%
    97,  //   8,   199,   3184us,   195%
// Always on  9,   375,   6000us,   188%
};

#define DISPLAY_TICKER_SLOT 1

/* This is the PWM callback.  It is registered by the animation callback and
 * will unregister itself when all of the brightness steps are complete. */
static int32_t callback(void) {
    microbit_display_obj_t *display = &microbit_display_obj;
    mp_uint_t brightness = display->previous_brightness;
    displaySetPinsForRow(display, brightness);
    brightness += 1;
    if (brightness == MAX_BRIGHTNESS) {
        clear_ticker_callback(DISPLAY_TICKER_SLOT);
        return -1;
    }
    display->previous_brightness = brightness;
    // Return interval (in 16us ticks) until next callback
    return render_timings[brightness];
}

static void draw_object(mp_obj_t obj) {
    microbit_display_obj_t *display = (microbit_display_obj_t*)MP_STATE_PORT(async_data)[0];
    if (obj == MP_OBJ_STOP_ITERATION) {
        if (async_clear) {
            microbit_display_show(&microbit_display_obj, BLANK_IMAGE);
            async_clear = false;
        } else {
            async_stop();
        }
    } else if (mp_obj_get_type(obj) == &microbit_image_type) {
        microbit_display_show(display, (microbit_image_obj_t *)obj);
    } else if (mp_obj_is_str(obj)) {
        size_t len;
        const char *str = mp_obj_str_get_data(obj, &len);
        if (len == 1) {
            microbit_display_show(display, microbit_image_for_char(str[0]));
        } else {
            async_stop();
        }
    } else {
        mp_sched_exception(mp_obj_new_exception_msg(&mp_type_TypeError, MP_ERROR_TEXT("not an image.")));
        async_stop();
    }
}

static void microbit_display_update(void) {
    async_tick += MILLISECONDS_PER_MACRO_TICK;
    if (async_tick < async_delay) {
        return;
    }
    async_tick = 0;
    switch (async_mode) {
        case ASYNC_MODE_ANIMATION:
        {
            if (MP_STATE_PORT(async_data)[0] == NULL || MP_STATE_PORT(async_data)[1] == NULL) {
                async_stop();
                break;
            }
            /* WARNING: We are executing in an interrupt handler.
             * If an exception is raised here then we must hand it to the VM. */
            mp_obj_t obj;
            nlr_buf_t nlr;
            gc_lock();
            if (nlr_push(&nlr) == 0) {
                obj = mp_iternext_allow_raise(async_iterator);
                nlr_pop();
                gc_unlock();
            } else {
                gc_unlock();
                if (!mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(((mp_obj_base_t*)nlr.ret_val)->type),
                    MP_OBJ_FROM_PTR(&mp_type_StopIteration))) {
                    // An exception other than StopIteration, so set it for the VM to raise later
                    // If memory error, write an appropriate message.
                    if (mp_obj_get_type(nlr.ret_val) == &mp_type_MemoryError) {
                        mp_printf(&mp_plat_print, "Allocation in interrupt handler");
                    }
                    mp_sched_exception(MP_OBJ_FROM_PTR(nlr.ret_val));
                }
                obj = MP_OBJ_STOP_ITERATION;
            }
            draw_object(obj);
            break;
        }
        case ASYNC_MODE_CLEAR:
            microbit_display_show(&microbit_display_obj, BLANK_IMAGE);
            async_stop();
            break;
    }
}

#define GREYSCALE_MASK ((1<<MAX_BRIGHTNESS)-2)

/* This is the top-level animation/display registered callback. */
void microbit_display_tick(void) {
    /* Do nothing if the display is not active. */
    if (!microbit_display_obj.active) {
        return;
    }

    displayAdvanceRow(&microbit_display_obj);

    microbit_display_update();
    microbit_display_obj.previous_brightness = 0;
    if (microbit_display_obj.brightnesses & GREYSCALE_MASK) {
        set_ticker_callback(DISPLAY_TICKER_SLOT, callback, 1800);
    }
}


void microbit_display_animate(microbit_display_obj_t *self, mp_obj_t iterable, mp_int_t delay, bool clear, bool wait) {
    // Reset the repeat state.
    MP_STATE_PORT(async_data)[0] = NULL;
    MP_STATE_PORT(async_data)[1] = NULL;
    async_iterator = mp_getiter(iterable, NULL);
    async_delay = delay;
    async_clear = clear;
    MP_STATE_PORT(async_data)[0] = self; // so it doesn't get GC'd
    MP_STATE_PORT(async_data)[1] = async_iterator;
    wakeup_event = false;
    mp_obj_t obj = mp_iternext_allow_raise(async_iterator);
    draw_object(obj);
    async_tick = 0;
    async_mode = ASYNC_MODE_ANIMATION;
    if (wait) {
        wait_for_event();
    }
}


// Delay in ms in between moving display one column to the left.
#define DEFAULT_SCROLL_SPEED       150

void microbit_display_scroll(microbit_display_obj_t *self, const char* str, bool wait) {
    mp_obj_t iterable = scrolling_string_image_iterable(str, strlen(str), NULL, false, false);
    microbit_display_animate(self, iterable, DEFAULT_SCROLL_SPEED, false, wait);
}


mp_obj_t microbit_display_scroll_func(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t scroll_allowed_args[] = {
        { MP_QSTR_text, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_delay, MP_ARG_INT, {.u_int = DEFAULT_SCROLL_SPEED} },
        { MP_QSTR_wait, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_monospace, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_loop, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    // Parse the args.
    microbit_display_obj_t *self = (microbit_display_obj_t*)pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(scroll_allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(scroll_allowed_args), scroll_allowed_args, args);
    size_t len;
    const char* str = mp_obj_str_get_data(args[0].u_obj, &len);
    mp_obj_t iterable = scrolling_string_image_iterable(str, len, args[0].u_obj, args[3].u_bool /*monospace?*/, args[4].u_bool /*loop*/);
    microbit_display_animate(self, iterable, args[1].u_int /*delay*/, false/*clear*/, args[2].u_bool/*wait?*/);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(microbit_display_scroll_obj, 1, microbit_display_scroll_func);

mp_obj_t microbit_display_on_func(mp_obj_t obj) {
    microbit_display_obj_t *self = (microbit_display_obj_t*)obj;
    /* Try to reclaim the pins we need */
/*
    microbit_obj_pin_fail_if_cant_acquire(&microbit_p3_obj);
    microbit_obj_pin_fail_if_cant_acquire(&microbit_p4_obj);
    microbit_obj_pin_fail_if_cant_acquire(&microbit_p6_obj);
    microbit_obj_pin_fail_if_cant_acquire(&microbit_p7_obj);
    microbit_obj_pin_fail_if_cant_acquire(&microbit_p9_obj);
    microbit_obj_pin_fail_if_cant_acquire(&microbit_p10_obj);
    microbit_obj_pin_acquire(&microbit_p3_obj, microbit_pin_mode_display);
    microbit_obj_pin_acquire(&microbit_p4_obj, microbit_pin_mode_display);
    microbit_obj_pin_acquire(&microbit_p6_obj, microbit_pin_mode_display);
    microbit_obj_pin_acquire(&microbit_p7_obj, microbit_pin_mode_display);
    microbit_obj_pin_acquire(&microbit_p9_obj, microbit_pin_mode_display);
    microbit_obj_pin_acquire(&microbit_p10_obj, microbit_pin_mode_display);
*/
    /* Make sure all pins are in the correct state */
    microbit_display_init();
    /* Re-enable the display loop.  This will resume any animations in
     * progress and display any static image. */
    self->active = true;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(microbit_display_on_obj, microbit_display_on_func);

mp_obj_t microbit_display_off_func(mp_obj_t obj) {
    microbit_display_obj_t *self = (microbit_display_obj_t*)obj;
    /* Disable the display loop.  This will pause any animations in progress.
     * It will not prevent a user from attempting to modify the state, but
     * modifications will not appear to have any effect until the display loop
     * is re-enabled. */
    self->active = false;
    /* Disable the row strobes, allowing the columns to be used freely for
     * GPIO. */
    nrf_gpio_port_out_clear(0, ROW_PINS_MASK);
    /* Free pins for other uses */
/*
    microbit_obj_pin_free(&microbit_p3_obj);
    microbit_obj_pin_free(&microbit_p4_obj);
    microbit_obj_pin_free(&microbit_p6_obj);
    microbit_obj_pin_free(&microbit_p7_obj);
    microbit_obj_pin_free(&microbit_p9_obj);
    microbit_obj_pin_free(&microbit_p10_obj);
*/
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(microbit_display_off_obj, microbit_display_off_func);

mp_obj_t microbit_display_is_on_func(mp_obj_t obj) {
    microbit_display_obj_t *self = (microbit_display_obj_t*)obj;
    if (self->active) {
        return mp_const_true;
    }
    else {
        return mp_const_false;
    }
}
MP_DEFINE_CONST_FUN_OBJ_1(microbit_display_is_on_obj, microbit_display_is_on_func);

void microbit_display_clear(void) {
    // Reset repeat state, cancel animation and clear screen.
    wakeup_event = false;
    async_mode = ASYNC_MODE_CLEAR;
    async_tick = async_delay - MILLISECONDS_PER_MACRO_TICK;
    wait_for_event();
}

mp_obj_t microbit_display_clear_func(mp_obj_t self_in) {
    microbit_display_clear();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(microbit_display_clear_obj, microbit_display_clear_func);

void microbit_display_set_pixel(microbit_display_obj_t *display, mp_int_t x, mp_int_t y, mp_int_t bright) {
    if (x < 0 || y < 0 || x > 4 || y > 4) {
        mp_raise_ValueError(MP_ERROR_TEXT("index out of bounds."));
    }
    if (bright < 0 || bright > MAX_BRIGHTNESS) {
        mp_raise_ValueError(MP_ERROR_TEXT("brightness out of bounds."));
    }
    display->image_buffer[x][y] = bright;
    display->brightnesses |= (1 << bright);
}

STATIC mp_obj_t microbit_display_set_pixel_func(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args;
    microbit_display_obj_t *self = (microbit_display_obj_t*)args[0];
    microbit_display_set_pixel(self, mp_obj_get_int(args[1]), mp_obj_get_int(args[2]), mp_obj_get_int(args[3]));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(microbit_display_set_pixel_obj, 4, 4, microbit_display_set_pixel_func);

mp_int_t microbit_display_get_pixel(microbit_display_obj_t *display, mp_int_t x, mp_int_t y) {
    if (x < 0 || y < 0 || x > 4 || y > 4) {
        mp_raise_ValueError(MP_ERROR_TEXT("index out of bounds."));
    }
    return display->image_buffer[x][y];
}

STATIC mp_obj_t microbit_display_get_pixel_func(mp_obj_t self_in, mp_obj_t x_in, mp_obj_t y_in) {
    microbit_display_obj_t *self = (microbit_display_obj_t*)self_in;
    return MP_OBJ_NEW_SMALL_INT(microbit_display_get_pixel(self, mp_obj_get_int(x_in), mp_obj_get_int(y_in)));
}
MP_DEFINE_CONST_FUN_OBJ_3(microbit_display_get_pixel_obj, microbit_display_get_pixel_func);

STATIC const mp_rom_map_elem_t microbit_display_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_get_pixel),  MP_ROM_PTR(&microbit_display_get_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pixel),  MP_ROM_PTR(&microbit_display_set_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&microbit_display_show_obj) },
    { MP_ROM_QSTR(MP_QSTR_scroll), MP_ROM_PTR(&microbit_display_scroll_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&microbit_display_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_on),  MP_ROM_PTR(&microbit_display_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off),  MP_ROM_PTR(&microbit_display_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_on),  MP_ROM_PTR(&microbit_display_is_on_obj) },
};

STATIC MP_DEFINE_CONST_DICT(microbit_display_locals_dict, microbit_display_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    microbit_display_type,
    MP_QSTR_MicroBitDisplay,
    MP_TYPE_FLAG_NONE,
    locals_dict, &microbit_display_locals_dict
    );

microbit_display_obj_t microbit_display_obj = {
    {&microbit_display_type},
    {{ 0, }},
    .previous_brightness = 0,
    .active = 1,
    .strobe_row = 0,
    .brightnesses = 0,
    .pins_for_brightness = { 0 },
};

void microbit_display_init(void) {
    //  Set pins as output.
    for (int i = MIN_COLUMN_PIN; i <= MAX_ROW_PIN; i++) {
        nrf_gpio_cfg_output(i);
    }
}

MP_REGISTER_ROOT_POINTER(void *async_data[2]);

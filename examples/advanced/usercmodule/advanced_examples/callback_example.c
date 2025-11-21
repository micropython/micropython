/**
 * Callback and Event Handling Example
 *
 * Demonstrates:
 * - Storing and calling Python callbacks from C
 * - Event-driven programming patterns
 * - Interrupt-safe callback queueing
 * - Multiple callback registration
 */

#include "py/runtime.h"
#include "py/mphal.h"

//=============================================================================
// Example 1: Event Manager with Multiple Callbacks
//=============================================================================

#define MAX_CALLBACKS 8

typedef struct _event_manager_obj_t {
    mp_obj_base_t base;
    mp_obj_t callbacks[MAX_CALLBACKS];
    uint8_t callback_count;
    uint32_t event_count;
} event_manager_obj_t;

static mp_obj_t event_manager_make_new(const mp_obj_type_t *type, size_t n_args,
                                         size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    event_manager_obj_t *self = mp_obj_malloc(event_manager_obj_t, type);

    for (int i = 0; i < MAX_CALLBACKS; i++) {
        self->callbacks[i] = mp_const_none;
    }
    self->callback_count = 0;
    self->event_count = 0;

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t event_manager_on(mp_obj_t self_in, mp_obj_t callback) {
    event_manager_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (!mp_obj_is_callable(callback)) {
        mp_raise_TypeError(MP_ERROR_TEXT("callback must be callable"));
    }

    if (self->callback_count >= MAX_CALLBACKS) {
        mp_raise_msg(&mp_type_RuntimeError,
                     MP_ERROR_TEXT("too many callbacks"));
    }

    // Store callback
    self->callbacks[self->callback_count++] = callback;

    return mp_obj_new_int(self->callback_count - 1);  // Return callback ID
}
static MP_DEFINE_CONST_FUN_OBJ_2(event_manager_on_obj, event_manager_on);

static mp_obj_t event_manager_off(mp_obj_t self_in, mp_obj_t id_in) {
    event_manager_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t id = mp_obj_get_int(id_in);

    if (id < 0 || id >= self->callback_count) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid callback ID"));
    }

    self->callbacks[id] = mp_const_none;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(event_manager_off_obj, event_manager_off);

static mp_obj_t event_manager_emit(size_t n_args, const mp_obj_t *args) {
    event_manager_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    // Collect event data arguments (if any)
    mp_obj_t event_args[n_args - 1];
    for (size_t i = 1; i < n_args; i++) {
        event_args[i - 1] = args[i];
    }

    self->event_count++;

    // Call all registered callbacks
    for (int i = 0; i < self->callback_count; i++) {
        if (self->callbacks[i] != mp_const_none) {
            // Call callback with event arguments
            if (n_args == 1) {
                mp_call_function_0(self->callbacks[i]);
            } else {
                mp_call_function_n_kw(self->callbacks[i], n_args - 1, 0, event_args);
            }
        }
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(event_manager_emit_obj, 1, MP_OBJ_FUN_ARGS_MAX, event_manager_emit);

static mp_obj_t event_manager_count(mp_obj_t self_in) {
    event_manager_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->event_count);
}
static MP_DEFINE_CONST_FUN_OBJ_1(event_manager_count_obj, event_manager_count);

static const mp_rom_map_elem_t event_manager_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&event_manager_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&event_manager_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_emit), MP_ROM_PTR(&event_manager_emit_obj) },
    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&event_manager_count_obj) },
};
static MP_DEFINE_CONST_DICT(event_manager_locals_dict, event_manager_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    event_manager_type,
    MP_QSTR_EventManager,
    MP_TYPE_FLAG_NONE,
    make_new, event_manager_make_new,
    locals_dict, &event_manager_locals_dict
);

//=============================================================================
// Example 2: Timer with Callback (simulated)
//=============================================================================

typedef struct _timer_obj_t {
    mp_obj_base_t base;
    mp_obj_t callback;
    uint32_t interval_ms;
    uint32_t last_tick;
    uint32_t call_count;
    bool active;
} timer_obj_t;

static mp_obj_t timer_make_new(const mp_obj_type_t *type, size_t n_args,
                                 size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    timer_obj_t *self = mp_obj_malloc(timer_obj_t, type);

    self->interval_ms = mp_obj_get_int(args[0]);
    self->callback = mp_const_none;
    self->last_tick = mp_hal_ticks_ms();
    self->call_count = 0;
    self->active = false;

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t timer_callback(mp_obj_t self_in, mp_obj_t callback) {
    timer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (callback != mp_const_none && !mp_obj_is_callable(callback)) {
        mp_raise_TypeError(MP_ERROR_TEXT("callback must be callable or None"));
    }

    self->callback = callback;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(timer_callback_obj, timer_callback);

static mp_obj_t timer_start(mp_obj_t self_in) {
    timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->active = true;
    self->last_tick = mp_hal_ticks_ms();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(timer_start_obj, timer_start);

static mp_obj_t timer_stop(mp_obj_t self_in) {
    timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->active = false;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(timer_stop_obj, timer_stop);

// This would be called periodically (e.g., from a timer interrupt or main loop)
static mp_obj_t timer_tick(mp_obj_t self_in) {
    timer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (!self->active || self->callback == mp_const_none) {
        return mp_const_false;
    }

    uint32_t now = mp_hal_ticks_ms();
    if (now - self->last_tick >= self->interval_ms) {
        self->last_tick = now;
        self->call_count++;

        // Call the callback
        mp_call_function_0(self->callback);

        return mp_const_true;
    }

    return mp_const_false;
}
static MP_DEFINE_CONST_FUN_OBJ_1(timer_tick_obj, timer_tick);

static const mp_rom_map_elem_t timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_callback), MP_ROM_PTR(&timer_callback_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&timer_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&timer_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_tick), MP_ROM_PTR(&timer_tick_obj) },
};
static MP_DEFINE_CONST_DICT(timer_locals_dict, timer_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    timer_type,
    MP_QSTR_CallbackTimer,
    MP_TYPE_FLAG_NONE,
    make_new, timer_make_new,
    locals_dict, &timer_locals_dict
);

//=============================================================================
// Example 3: Interrupt-Safe Event Queue
//=============================================================================

#define EVENT_QUEUE_SIZE 16

typedef struct _event_queue_obj_t {
    mp_obj_base_t base;
    mp_obj_t callback;
    volatile uint32_t events[EVENT_QUEUE_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
    volatile uint8_t count;
} event_queue_obj_t;

static mp_obj_t event_queue_make_new(const mp_obj_type_t *type, size_t n_args,
                                       size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    event_queue_obj_t *self = mp_obj_malloc(event_queue_obj_t, type);

    self->callback = mp_const_none;
    self->head = 0;
    self->tail = 0;
    self->count = 0;

    return MP_OBJ_FROM_PTR(self);
}

// This would be called from an interrupt handler
static void event_queue_push_isr(event_queue_obj_t *self, uint32_t event) {
    if (self->count < EVENT_QUEUE_SIZE) {
        self->events[self->head] = event;
        self->head = (self->head + 1) % EVENT_QUEUE_SIZE;
        self->count++;
    }
}

static mp_obj_t event_queue_process(mp_obj_t self_in) {
    event_queue_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t processed = 0;

    while (self->count > 0) {
        // Get event from queue (disable interrupts during access)
        mp_uint_t state = MICROPY_BEGIN_ATOMIC_SECTION();

        uint32_t event = self->events[self->tail];
        self->tail = (self->tail + 1) % EVENT_QUEUE_SIZE;
        self->count--;

        MICROPY_END_ATOMIC_SECTION(state);

        // Process event with callback (interrupts enabled)
        if (self->callback != mp_const_none) {
            mp_call_function_1(self->callback, mp_obj_new_int(event));
        }

        processed++;
    }

    return mp_obj_new_int(processed);
}
static MP_DEFINE_CONST_FUN_OBJ_1(event_queue_process_obj, event_queue_process);

static mp_obj_t event_queue_set_callback(mp_obj_t self_in, mp_obj_t callback) {
    event_queue_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (callback != mp_const_none && !mp_obj_is_callable(callback)) {
        mp_raise_TypeError(MP_ERROR_TEXT("callback must be callable or None"));
    }

    self->callback = callback;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(event_queue_set_callback_obj, event_queue_set_callback);

static mp_obj_t event_queue_pending(mp_obj_t self_in) {
    event_queue_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->count);
}
static MP_DEFINE_CONST_FUN_OBJ_1(event_queue_pending_obj, event_queue_pending);

static const mp_rom_map_elem_t event_queue_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_process), MP_ROM_PTR(&event_queue_process_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_callback), MP_ROM_PTR(&event_queue_set_callback_obj) },
    { MP_ROM_QSTR(MP_QSTR_pending), MP_ROM_PTR(&event_queue_pending_obj) },
};
static MP_DEFINE_CONST_DICT(event_queue_locals_dict, event_queue_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    event_queue_type,
    MP_QSTR_EventQueue,
    MP_TYPE_FLAG_NONE,
    make_new, event_queue_make_new,
    locals_dict, &event_queue_locals_dict
);

//=============================================================================
// Module Definition
//=============================================================================

static const mp_rom_map_elem_t callback_example_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_callback_example) },
    { MP_ROM_QSTR(MP_QSTR_EventManager), MP_ROM_PTR(&event_manager_type) },
    { MP_ROM_QSTR(MP_QSTR_CallbackTimer), MP_ROM_PTR(&timer_type) },
    { MP_ROM_QSTR(MP_QSTR_EventQueue), MP_ROM_PTR(&event_queue_type) },
};
static MP_DEFINE_CONST_DICT(callback_example_module_globals, callback_example_module_globals_table);

const mp_obj_module_t callback_example_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&callback_example_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_callback_example, callback_example_user_cmodule);

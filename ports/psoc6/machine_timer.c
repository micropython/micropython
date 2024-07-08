// mpy includes
#include "py/runtime.h"
#include "py/mphal.h"


// MTB includes
#include "cybsp.h"
#include "cyhal.h"


#define TIMER_MODE_ONE_SHOT (0)
#define TIMER_MODE_PERIODIC (1)

typedef struct _machine_timer_obj_t {
    mp_obj_base_t base;
    cyhal_timer_t timer_obj;
    uint8_t timer_id;
    uint32_t mode;
    uint32_t period;
    uint32_t freq;
    mp_obj_t callback;
} machine_timer_obj_t;

const mp_obj_type_t machine_timer_type;

static void isr_timer(void *callback_arg, cyhal_timer_event_t event) {
    machine_timer_obj_t *self = callback_arg;
    mp_sched_schedule(self->callback, MP_OBJ_FROM_PTR(self));
}


static void machine_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    qstr mode = self->mode == TIMER_MODE_ONE_SHOT ? MP_QSTR_ONE_SHOT : MP_QSTR_PERIODIC;
    mp_printf(print, "Timer(mode=%q, period=%u, tick_hz=%u)", mode, self->period, self->freq);
}

static mp_obj_t machine_timer_init_helper(machine_timer_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_callback, ARG_period, ARG_freq, };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIMER_MODE_ONE_SHOT} },
        { MP_QSTR_callback,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_period,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 9999u} },
        { MP_QSTR_freq,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_rom_obj = MP_ROM_NONE} },
    };
    // Parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    float period;
    self->mode = args[ARG_mode].u_int;

    if (args[ARG_freq].u_obj != mp_const_none) {
        self->freq = args[ARG_freq].u_int;
        period = 1.0f / (float)(args[ARG_freq].u_int);   // Frequency to period conversion
    } else {
        self->period = args[ARG_period].u_int;
        period = (float)args[ARG_period].u_int / 1000.0f;   // ms to s conversion
    }

    if (args[ARG_callback].u_obj != mp_const_none) {
        self->callback = args[ARG_callback].u_obj;
    }


    uint32_t period_hal;        // Period/count input for the PSoC6 HAL timer configuration
    uint32_t fz_hal = 1000000;  // Frequency for the PSoC timer clock is fixed as 1 MHz
    period_hal = (uint32_t)(period * fz_hal) - 1; // Overflow Period = (Period + 1)/ frequency ;period = (overflow period * frequency)-1

    // Adjust the frequency & recalculate the period if period/count is  greater than the maximum overflow value for a 32 bit timer ie; 2^32
    while (period_hal > 4294967296) {
        fz_hal = fz_hal / 10;  // Reduce the fz_hal value by 10%
        period_hal = (uint32_t)(period * fz_hal) - 1;  // Recalculate Period input for the PSoC6 HAL timer configuration
    }

    // Timer initialisation of port
    cy_rslt_t rslt;

    const cyhal_timer_cfg_t timer_cfg =
    {
        .compare_value = 0,                 /* Timer compare value, not used */
        .period = period_hal,               /* Defines the timer period */
        .direction = CYHAL_TIMER_DIR_UP,    /* Timer counts up */
        .is_compare = false,                /* Don't use compare mode */
        .is_continuous = self->mode,        /* Run the timer */
        .value = 0                          /* Initial value of counter */
    };

    /* Initialize the timer object. Does not use pin output ('pin' is NC) and
     * does not use a pre-configured clock source ('clk' is NULL). */

    rslt = cyhal_timer_init(&self->timer_obj, NC, NULL);
    CY_ASSERT(CY_RSLT_SUCCESS == rslt);

    /* Apply timer configuration such as period, count direction, run mode, etc. */
    rslt = cyhal_timer_configure(&self->timer_obj, &timer_cfg);

    /* Set the frequency of timer to Defined frequency */
    rslt = cyhal_timer_set_frequency(&self->timer_obj, fz_hal);

    /* Assign the ISR to execute on timer interrupt */
    cyhal_timer_register_callback(&self->timer_obj, isr_timer, self);

    /* Set the event on which timer interrupt occurs and enable it */
    cyhal_timer_enable_event(&self->timer_obj, CYHAL_TIMER_IRQ_TERMINAL_COUNT, 3, true);

    /* Start the timer with the configured settings */
    rslt = cyhal_timer_start(&self->timer_obj);

    CY_ASSERT(CY_RSLT_SUCCESS == rslt);

    return mp_const_none;
}

static mp_obj_t machine_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    machine_timer_obj_t *self = mp_obj_malloc(machine_timer_obj_t, &machine_timer_type);

    self->base.type = &machine_timer_type;

    mp_int_t id = 0;
    if (n_args > 0) {
        id = mp_obj_get_int(args[0]);
        --n_args;
        ++args;
    }
    // Get timer id
    if (id != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Specified Timer doesn't exist. Currently only id 0 is configured"));
    }

    if (n_args > 0 || n_kw > 0) {
        // Start the timer
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_timer_init_helper(self, n_args, args, &kw_args);
    }

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_timer_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    return machine_timer_init_helper(self, n_args - 1, args + 1, kw_args);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_timer_init_obj, 1, machine_timer_init);

static mp_obj_t machine_timer_deinit(mp_obj_t self_in) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    cyhal_timer_free(&self->timer_obj);
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_deinit_obj, machine_timer_deinit);

static const mp_rom_map_elem_t machine_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&machine_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_timer_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_timer_deinit_obj) },

    { MP_ROM_QSTR(MP_QSTR_ONE_SHOT), MP_ROM_INT(TIMER_MODE_ONE_SHOT) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(TIMER_MODE_PERIODIC) },
};

static MP_DEFINE_CONST_DICT(machine_timer_locals_dict, machine_timer_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_timer_type,
    MP_QSTR_Timer,
    MP_TYPE_FLAG_NONE,
    make_new, machine_timer_make_new,
    print, machine_timer_print,
    locals_dict, &machine_timer_locals_dict
    );

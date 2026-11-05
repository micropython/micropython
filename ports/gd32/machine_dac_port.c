// machine_dac_port.c - machine.DAC for GD32F4xx
// DAC has no extmod layer, so this file defines the type and registers
// machine_dac_type via MICROPY_PY_MACHINE_EXTRA_GLOBALS.

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#include "gd32f4xx.h"
#include "pin_defs.h"

typedef struct _machine_dac_obj_t {
    mp_obj_base_t base;
    uint8_t  unit;     // 0 = DAC0 (PA4), 1 = DAC1 (PA5)
    int16_t  pin;
} machine_dac_obj_t;

static machine_dac_obj_t machine_dac_obj[2];
static bool machine_dac_ready = false;

// PA4 -> DAC0 (out0), PA5 -> DAC1 (out1)
static int pin_to_dac(uint8_t pin_enc) {
    uint8_t port = PIN_PORT(pin_enc);
    uint8_t num = PIN_NUM(pin_enc);
    if (port == 0 && num == 4) {
        return 0;
    }
    if (port == 0 && num == 5) {
        return 1;
    }
    return -1;
}

static void dac_prepare(void) {
    if (machine_dac_ready) {
        return;
    }
    rcu_periph_clock_enable(RCU_DAC);
    dac_deinit(DAC0);
    dac_trigger_disable(DAC0, DAC_OUT0);
    dac_trigger_disable(DAC0, DAC_OUT1);
    dac_wave_mode_config(DAC0, DAC_OUT0, DAC_WAVE_DISABLE);
    dac_wave_mode_config(DAC0, DAC_OUT1, DAC_WAVE_DISABLE);
    dac_output_buffer_enable(DAC0, DAC_OUT0);
    dac_output_buffer_enable(DAC0, DAC_OUT1);
    dac_enable(DAC0, DAC_OUT0);
    dac_enable(DAC0, DAC_OUT1);
    mp_hal_delay_us(10);
    machine_dac_ready = true;
}

// ---- type ----
static void machine_dac_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_dac_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "DAC(%u)", self->unit);
}

static mp_obj_t machine_dac_make_new(const mp_obj_type_t *type, size_t n_args,
                                     size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    mp_int_t pin_enc = mp_obj_get_int(args[0]);
    int unit = pin_to_dac((uint8_t)pin_enc);
    if (unit < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("DAC available on PA4, PA5"));
    }

    machine_dac_obj_t *self = &machine_dac_obj[unit];
    self->base.type = type;
    self->unit = (uint8_t)unit;
    self->pin = (int16_t)pin_enc;

    // configure the pin as analog
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, pin_hal_mask(PIN_NUM((uint8_t)pin_enc)));

    dac_prepare();
    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_dac_write(mp_obj_t self_in, mp_obj_t value_in) {
    machine_dac_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t value = mp_obj_get_int(value_in);
    if (value < 0) {
        value = 0;
    }
    if (value > 4095) {
        value = 4095;
    }
    dac_prepare();
    dac_data_set(DAC0, self->unit == 0 ? DAC_OUT0 : DAC_OUT1,
                 DAC_ALIGN_12B_R, (uint16_t)value);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_dac_write_obj, machine_dac_write);

static mp_obj_t machine_dac_read(mp_obj_t self_in) {
    machine_dac_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(
        dac_output_value_get(DAC0, self->unit == 0 ? DAC_OUT0 : DAC_OUT1));
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_dac_read_obj, machine_dac_read);

static const mp_rom_map_elem_t machine_dac_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&machine_dac_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_read),  MP_ROM_PTR(&machine_dac_read_obj) },
};
static MP_DEFINE_CONST_DICT(machine_dac_locals_dict, machine_dac_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_dac_type,
    MP_QSTR_DAC,
    MP_TYPE_FLAG_NONE,
    make_new, machine_dac_make_new,
    print, machine_dac_print,
    locals_dict, &machine_dac_locals_dict
    );

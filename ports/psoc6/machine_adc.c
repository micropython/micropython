
#include <stdbool.h>
#include "py/runtime.h"
#include "py/mphal.h"

// port-specific includes
#include "modmachine.h"
#include "machine_adc.h"
#include "machine_adcblock.h"

#include "cybsp.h"
#include "cyhal.h"
#include "cy_pdl.h"

extern machine_adcblock_obj_t *adc_block_obj_find(mp_obj_t pin);
extern machine_adcblock_obj_t *adc_block_obj_init(mp_obj_t pin);
extern machine_adc_obj_t *adc_block_channel_find(machine_adcblock_obj_t *adc_block, mp_obj_t pin);
extern machine_adc_obj_t *adc_block_channel_alloc(machine_adcblock_obj_t *adc_block, mp_obj_t pin);
extern void adc_block_channel_free(machine_adcblock_obj_t *acd_block, machine_adc_obj_t *channel);

void adc_obj_init(machine_adc_obj_t *adc, machine_adcblock_obj_t *adc_block, mp_obj_t pin_name, uint32_t sampling_time) {
    machine_pin_phy_obj_t *adc_pin_phy = pin_phy_realloc(pin_name, PIN_PHY_FUNC_ADC);

    const cyhal_adc_channel_config_t channel_config =
    {
        .enable_averaging = false,
        .min_acquisition_ns = sampling_time,
        .enabled = true
    };

    cy_rslt_t status = cyhal_adc_channel_init_diff(&(adc->adc_chan_obj), &(adc_block->adc_obj), adc_pin_phy->addr, CYHAL_ADC_VNEG, &channel_config);
    if (status != CY_RSLT_SUCCESS) {
        mp_raise_TypeError(MP_ERROR_TEXT("ADC Channel Initialization failed!"));
    }

    adc->pin_phy = adc_pin_phy;
    adc->block = adc_block;
    adc->sample_ns = sampling_time;
}

void adc_obj_deinit(machine_adc_obj_t *adc) {
    cyhal_adc_channel_free(&(adc->adc_chan_obj));
    pin_phy_free(adc->pin_phy);
}

machine_adc_obj_t *machine_adc_make_init(uint32_t sampling_time, mp_obj_t pin_name) {
    machine_adc_obj_t *adc;

    machine_adcblock_obj_t *adc_block = adc_block_obj_find(pin_name);
    if (adc_block == NULL) {
        adc_block = adc_block_obj_init(pin_name);
    } else {
        adc = adc_block_channel_find(adc_block, pin_name);
        if (adc != NULL) {
            adc->sample_ns = sampling_time;
            return adc;
        }
    }

    adc = adc_block_channel_alloc(adc_block, pin_name);
    adc_obj_init(adc, adc_block, pin_name, sampling_time);

    return adc;
}

// Helper function to get resolution
uint8_t adc_get_resolution(machine_adc_obj_t *adc) {
    printf("ADC bits: %d", adc->block->bits);
    return adc->block->bits;
}

// machine_adc_print()
static void machine_adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<ADC Pin=%u, ADCBlock_id=%d, sampling_time_ns=%ld>", self->pin_phy->addr, self->block->id, self->sample_ns);
}

// ADC(id)
static mp_obj_t machine_adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, 6, true);
    enum {ARG_sample_ns};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample_ns, MP_ARG_KW_ONLY | MP_ARG_INT,   {.u_int = DEFAULT_ADC_ACQ_NS} },
    };

    // Parse the arguments.
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, all_args + 1, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get user input sampling time
    uint32_t sampling_time = args[ARG_sample_ns].u_int;

    machine_adc_obj_t *o = machine_adc_make_init(sampling_time, all_args[0]);

    return MP_OBJ_FROM_PTR(o);
}

// deinit()
static mp_obj_t machine_adc_deinit(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    adc_obj_deinit(self);
    adc_block_channel_free(self->block, self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_deinit_obj, machine_adc_deinit);

// block()
static mp_obj_t machine_adc_block(mp_obj_t self_in) {
    const machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_FROM_PTR(self->block);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_block_obj, machine_adc_block);

// read_u16()
static mp_obj_t machine_adc_read_u16(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int32_t adc_raw_result = cyhal_adc_read(&(self->adc_chan_obj));
    mp_int_t bits = (mp_int_t)adc_get_resolution(self);
    mp_uint_t u16 = adc_raw_result << (16 - bits);
    return MP_OBJ_NEW_SMALL_INT(u16);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_read_u16_obj, machine_adc_read_u16);

// read_uv
static mp_obj_t machine_adc_read_uv(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(cyhal_adc_read_uv(&(self->adc_chan_obj)));
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_read_uv_obj, machine_adc_read_uv);


static const mp_rom_map_elem_t machine_adc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_adc_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_u16), MP_ROM_PTR(&machine_adc_read_u16_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_uv), MP_ROM_PTR(&machine_adc_read_uv_obj) },
    { MP_ROM_QSTR(MP_QSTR_block), MP_ROM_PTR(&machine_adc_block_obj) },
};
static MP_DEFINE_CONST_DICT(machine_adc_locals_dict, machine_adc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_adc_type,
    MP_QSTR_ADC,
    MP_TYPE_FLAG_NONE,
    make_new, machine_adc_make_new,
    print, machine_adc_print,
    locals_dict, &machine_adc_locals_dict
    );

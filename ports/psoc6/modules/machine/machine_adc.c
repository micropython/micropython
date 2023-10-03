
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

#define IS_GPIO_VALID_ADC_PIN(gpio) ((gpio == CYHAL_NC_PIN_VALUE) || ((gpio >= 80) && (gpio <= 87)))

extern int16_t adc_block_get_block_id_for_pin(uint32_t pin);
extern machine_adc_obj_t *adc_block_retrieve_adc_obj_for_pin(machine_adcblock_obj_t *adc_block, uint32_t pin);
extern machine_adcblock_obj_t *adc_block_init_helper(uint8_t adc_id, uint8_t bits);
extern machine_adc_obj_t *adc_block_allocate_new_channel_for_pin(machine_adcblock_obj_t *adc_block, uint32_t pin);

/******************************************************************************/
// MicroPython bindings for machine.ADC

const mp_obj_type_t machine_adc_type;

// Private helper function to check if ADC Block ID is valid
static inline bool _is_gpio_valid_adc_pin(uint32_t pin) {
    if ((pin == CYHAL_NC_PIN_VALUE) || ((pin >= 80) && (pin <= 87))) {
        return true;
    }

    return false;
}

// Public helper function to handle new ADC channel creation and initialization
machine_adc_obj_t *adc_create_and_init_new_channel_obj(machine_adcblock_obj_t *adc_block, uint32_t pin, uint32_t sampling_time) {

    machine_adc_obj_t *adc_channel = adc_block_allocate_new_channel_for_pin(adc_block, pin);
    const cyhal_adc_channel_config_t channel_config =
    {
        .enable_averaging = false,
        .min_acquisition_ns = sampling_time,
        .enabled = true
    };

    cy_rslt_t status = cyhal_adc_channel_init_diff(&(adc_channel->adc_chan_obj), &(adc_block->adc_block_obj), pin, CYHAL_ADC_VNEG, &channel_config);
    if (status != CY_RSLT_SUCCESS) {
        mp_raise_TypeError(MP_ERROR_TEXT("ADC Channel Initialization failed!"));
    }

    adc_channel->pin = pin;
    adc_channel->block = adc_block;
    adc_channel->sample_ns = sampling_time;

    return adc_channel;
}

// Main helper function to create and initialize ADC
machine_adc_obj_t *adc_init_helper(uint32_t sampling_time, uint32_t pin) {
    if (!_is_gpio_valid_adc_pin(pin)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid ADC Pin"));
    }

    int16_t adc_block_id = adc_block_get_block_id_for_pin(pin);

    if (adc_block_id == -1) {
        mp_raise_ValueError(MP_ERROR_TEXT("No associated ADC Block for specified pin!"));
    }

    // Initialize ADC Block
    machine_adcblock_obj_t *adc_block = adc_block_init_helper(adc_block_id, DEFAULT_ADC_BITS);

    // Retrieve associated channel (ADC obj) for pin
    machine_adc_obj_t *o = adc_block_retrieve_adc_obj_for_pin(adc_block, pin);

    if (o == NULL) {
        o = adc_create_and_init_new_channel_obj(adc_block, pin, sampling_time);
    }

    return o;
}

// machine_adc_print()
STATIC void machine_adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<ADC Pin=%u, ADCBlock_id=%d, sampling_time_ns=%ld>", self->pin, self->block->id, self->sample_ns);
}

// ADC(id)
STATIC mp_obj_t machine_adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
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

    machine_pin_io_obj_t *adc_pin_obj = MP_OBJ_TO_PTR(all_args[0]);

    // Get user input sampling time
    uint32_t sampling_time = args[ARG_sample_ns].u_int;

    machine_adc_obj_t *o = adc_init_helper(sampling_time, adc_pin_obj->pin_addr);

    return MP_OBJ_FROM_PTR(o);
}

// block()
STATIC mp_obj_t machine_adc_block(mp_obj_t self_in) {
    const machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_FROM_PTR(self->block);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_block_obj, machine_adc_block);

// read_u16()
STATIC mp_obj_t machine_adc_read_u16(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(cyhal_adc_read_u16(&(self->adc_chan_obj)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_read_u16_obj, machine_adc_read_u16);

// read_uv
STATIC mp_obj_t machine_adc_read_uv(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(cyhal_adc_read_uv(&(self->adc_chan_obj)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_read_uv_obj, machine_adc_read_uv);

STATIC const mp_rom_map_elem_t machine_adc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read_u16), MP_ROM_PTR(&machine_adc_read_u16_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_uv), MP_ROM_PTR(&machine_adc_read_uv_obj) },
    { MP_ROM_QSTR(MP_QSTR_block), MP_ROM_PTR(&machine_adc_block_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_adc_locals_dict, machine_adc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_adc_type,
    MP_QSTR_ADC,
    MP_TYPE_FLAG_NONE,
    make_new, machine_adc_make_new,
    print, machine_adc_print,
    locals_dict, &machine_adc_locals_dict
    );

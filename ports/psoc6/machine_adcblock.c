#include <stdio.h>
#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"

#include "machine_adcblock.h"
#include "machine_adc.h"

extern void adc_obj_init(machine_adc_obj_t *adc, machine_adcblock_obj_t *adc_block, mp_obj_t pin_name, uint32_t sampling_time);
extern void adc_obj_deinit(machine_adc_obj_t *adc);

machine_adcblock_obj_t *adc_block[MAX_BLOCKS] = {NULL};

/*#if defined(CY8C624ABZI_S2D44)
const adc_block_channel_pin_map_t adc_block_pin_map[] = {
    {ADCBLOCK0, 0, CYHAL_GET_GPIO(CYHAL_PORT_10, 0)},
    {ADCBLOCK0, 1, CYHAL_GET_GPIO(CYHAL_PORT_10, 1)},
    {ADCBLOCK0, 2, CYHAL_GET_GPIO(CYHAL_PORT_10, 2)},
    {ADCBLOCK0, 3, CYHAL_GET_GPIO(CYHAL_PORT_10, 3)},
    {ADCBLOCK0, 4, CYHAL_GET_GPIO(CYHAL_PORT_10, 4)},
    {ADCBLOCK0, 5, CYHAL_GET_GPIO(CYHAL_PORT_10, 5)}
};
#elif defined(CYBLE_416045_02_device)*/
const adc_block_channel_pin_map_t adc_block_pin_map[] = {
    {ADCBLOCK0, 0, CYHAL_GET_GPIO(CYHAL_PORT_10, 0)},
    {ADCBLOCK0, 1, CYHAL_GET_GPIO(CYHAL_PORT_10, 1)},
    {ADCBLOCK0, 2, CYHAL_GET_GPIO(CYHAL_PORT_10, 2)},
    {ADCBLOCK0, 3, CYHAL_GET_GPIO(CYHAL_PORT_10, 3)},
    {ADCBLOCK0, 4, CYHAL_GET_GPIO(CYHAL_PORT_10, 4)},
    {ADCBLOCK0, 5, CYHAL_GET_GPIO(CYHAL_PORT_10, 5)},
    {ADCBLOCK0, 6, CYHAL_GET_GPIO(CYHAL_PORT_10, 6)},
    {ADCBLOCK0, 7, CYHAL_GET_GPIO(CYHAL_PORT_10, 7)}
};
// #endif
/******************************************************************************/
// MicroPython bindings for machine.ADC

// Private helper function to get channel number for provided pin
int16_t _get_adc_channel_number_for_pin(uint32_t pin) {
    for (int i = 0; i < (sizeof(adc_block_pin_map) / sizeof(adc_block_pin_map[0])); i++)
    {
        if (pin == adc_block_pin_map[i].pin) {
            return adc_block_pin_map[i].channel;
        }
    }
    return -1;
}

// Private helper function to get pin number for provided channel no.
int16_t _get_adc_pin_number_for_channel(uint16_t channel) {
    for (int i = 0; i < (sizeof(adc_block_pin_map) / sizeof(adc_block_pin_map[0])); i++)
    {
        if (channel == adc_block_pin_map[i].channel) {
            return adc_block_pin_map[i].pin;
        }
    }
    return -1;
}

// Public helper function to get adc block id associated to input pin
int16_t _get_block_id_for_pin(uint32_t pin) {
    for (int i = 0; i < (sizeof(adc_block_pin_map) / sizeof(adc_block_pin_map[0])); i++)
    {
        if (pin == adc_block_pin_map[i].pin) {
            return adc_block_pin_map[i].block_id;
        }
    }
    return -1;
}

// Private helper function to check if ADC Block ID is valid
static inline bool _is_valid_id(uint8_t adc_id) {
    if (adc_id >= MAX_BLOCKS) {
        return false;
    }

    return true;
}

static inline machine_adcblock_obj_t *_adc_block_obj_find(uint8_t adc_block_id) {
    if (!_is_valid_id(adc_block_id)) {
        mp_raise_TypeError(MP_ERROR_TEXT("Specified ADC id not supported"));
    }

    for (uint8_t i = 0; i < MAX_BLOCKS; i++)
    {
        if (adc_block[i] != NULL) {
            if (adc_block[i]->id == adc_block_id) {
                return adc_block[i];
            }
        }
    }

    return NULL;
}

static inline machine_adcblock_obj_t *_adc_block_obj_alloc() {
    for (uint8_t i = 0; i < MAX_BLOCKS; i++)
    {
        if (adc_block[i] == NULL) {
            adc_block[i] = mp_obj_malloc(machine_adcblock_obj_t, &machine_adcblock_type);
            return adc_block[i];
        }
    }

    return NULL;
}

static inline void _adc_block_obj_free(machine_adcblock_obj_t *adc_block_ptr) {
    for (uint8_t i = 0; i < MAX_BLOCKS; i++)
    {
        if (adc_block[i] == adc_block_ptr) {
            adc_block[i] = NULL;
        }
    }
}

machine_adc_obj_t *adc_block_channel_alloc(machine_adcblock_obj_t *adc_block, mp_obj_t pin) {
    int pin_addr = mp_hal_get_pin_obj(pin);
    int16_t adc_channel_no = _get_adc_channel_number_for_pin(pin_addr);
    adc_block->channel[adc_channel_no] = mp_obj_malloc(machine_adc_obj_t, &machine_adc_type);
    return adc_block->channel[adc_channel_no];
}

void adc_block_channel_free(machine_adcblock_obj_t *adc_block, machine_adc_obj_t *adc) {
    for (uint8_t i = 0; i < ADC_BLOCK_CHANNEL_MAX; i++) {
        if (adc_block->channel[i] == adc) {
            adc_block->channel[i] = NULL;
        }
    }
}

// Private helper function to get any associated pin to the given ADC Block id
static inline uint32_t _adc_block_get_any_pin(uint16_t adc_block_id) {
    for (uint8_t i = 0; i < MAX_CHANNELS; i++) {
        if (adc_block_pin_map[i].block_id == adc_block_id) {
            return adc_block_pin_map[i].pin;
        }
    }

    return 0; // there should always an associated pin for block...
}

// Private helper function to create and initialize new ADC Block
static void _adc_block_obj_init(machine_adcblock_obj_t *adc_block, uint16_t adc_block_id, uint8_t bits) {
    cy_rslt_t status = cyhal_adc_init(&(adc_block->adc_obj), _adc_block_get_any_pin(adc_block_id), NULL);
    if (status != CY_RSLT_SUCCESS) {
        mp_raise_TypeError(MP_ERROR_TEXT("ADC Initialization failed!"));
    }
    const cyhal_adc_config_t adc_config = {
        .continuous_scanning = false, // Continuous Scanning is disabled
        .average_count = 1,           // Average count disabled
        .vref = CYHAL_ADC_REF_VDDA,   // VREF for Single ended channel set to VDDA
        .vneg = CYHAL_ADC_VNEG_VSSA,  // VNEG for Single ended channel set to VSSA
        .resolution = 12u,          // 12-bit resolution
        .ext_vref = NC,             // No connection
        .bypass_pin = NC
    };                              // No connection

    status = cyhal_adc_configure(&(adc_block->adc_obj), &adc_config);
    if (status != CY_RSLT_SUCCESS) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("ADC configuration update failed. Error: %ld ! \n"), status);
    }

    adc_block->id = adc_block_id;
    adc_block->bits = bits;
}

static void _adc_block_obj_deinit(machine_adcblock_obj_t *adc_block) {
    // Includes all channels deinitialization and deallocation
    for (uint8_t i = 0; i < ADC_BLOCK_CHANNEL_MAX; i++) {
        if (adc_block->channel[i] != NULL) {
            adc_obj_deinit(adc_block->channel[i]);
            adc_block_channel_free(adc_block, adc_block->channel[i]);
        }
    }
    cyhal_adc_free(&(adc_block->adc_obj));
}

machine_adcblock_obj_t *machine_adcblock_make_init(uint8_t adc_id, uint8_t bits) {
    machine_adcblock_obj_t *adc_block = _adc_block_obj_find(adc_id);

    // No existing adc_block for given id. Create new one.
    if (adc_block == NULL) {
        adc_block = _adc_block_obj_alloc();
        if (adc_block == NULL) {
            mp_raise_TypeError(MP_ERROR_TEXT("ADC Blocks not available. Fully allocated"));
        }
        _adc_block_obj_init(adc_block, adc_id, bits);
    }

    return adc_block;
}

machine_adcblock_obj_t *adc_block_obj_find(mp_obj_t pin) {
    int pin_addr = mp_hal_get_pin_obj(pin);
    int adc_block_id = _get_block_id_for_pin(pin_addr);

    if (adc_block_id == -1) {
        mp_raise_ValueError(MP_ERROR_TEXT("No associated ADC Block for specified pin!"));
    }

    return _adc_block_obj_find(adc_block_id);
}

machine_adcblock_obj_t *adc_block_obj_init(mp_obj_t pin) {
    int pin_addr = mp_hal_get_pin_obj(pin);
    int adc_block_id = _get_block_id_for_pin(pin_addr);

    if (adc_block_id == -1) {
        mp_raise_ValueError(MP_ERROR_TEXT("No associated ADC Block for specified pin!"));
    }

    return machine_adcblock_make_init(adc_block_id, DEFAULT_ADC_BITS);
}

machine_adc_obj_t *adc_block_channel_find(machine_adcblock_obj_t *adc_block, mp_obj_t pin) {
    int pin_addr = mp_hal_get_pin_obj(pin);
    int16_t adc_channel_no = _get_adc_channel_number_for_pin(pin_addr);
    return adc_block->channel[adc_channel_no];
}

// machine_adcblock_print()
static void machine_adcblock_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_adcblock_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ADCBlock(%u, bits=%u)", self->id, self->bits);
}

// ADCBlock constructor
static mp_obj_t machine_adcblock_make_new(const mp_obj_type_t *type, size_t n_pos_args, size_t n_kw_args, const mp_obj_t *all_args) {
    mp_arg_check_num(n_pos_args, n_kw_args, 1, MP_OBJ_FUN_ARGS_MAX, true);

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw_args, all_args + n_pos_args);

    // Get ADC ID
    uint8_t adc_id = mp_obj_get_int(all_args[0]);

    // Get ADC bits
    enum { ARG_bits };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_ADC_BITS} }
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_pos_args - 1, all_args + 1, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    uint8_t bits = args[ARG_bits].u_int;
    if (bits != DEFAULT_ADC_BITS) {
        mp_raise_TypeError(MP_ERROR_TEXT("Invalid bits. Current ADC configuration supports only 11 bits resolution!"));
    }

    return MP_OBJ_FROM_PTR(machine_adcblock_make_init(adc_id, bits));
}

// ADCBlock deinit()
static mp_obj_t machine_adcblock_deinit(mp_obj_t self_in) {
    machine_adcblock_obj_t *self = MP_OBJ_TO_PTR(self_in);
    _adc_block_obj_deinit(self);
    _adc_block_obj_free(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_adcblock_deinit_obj, machine_adcblock_deinit);

// ADCBlock connect()
static mp_obj_t machine_adcblock_connect(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_adcblock_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    uint8_t channel = -1;
    uint32_t pin = 0;
    mp_obj_t pin_name;

    if (n_pos_args == 2) {
        // If channel only specified : If mp_obj_is_int is true, then it is channel
        if (mp_obj_is_int(pos_args[1])) {
            channel = mp_obj_get_int(pos_args[1]);
            pin = _get_adc_pin_number_for_channel(channel);
            pin_name = pin_name_by_addr(mp_obj_new_int(pin));
        } else {
            pin_name = pos_args[1];
        }
    } else if (n_pos_args == 3) {
        channel = mp_obj_get_int(pos_args[1]);
        uint32_t exp_pin = _get_adc_pin_number_for_channel(channel);

        pin_name = pos_args[2];
        pin = mp_hal_get_pin_obj(pin_name);
        if (exp_pin != pin) {
            mp_raise_TypeError(MP_ERROR_TEXT("Wrong pin specified for the mentioned channel"));
        }
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("Too many positional args"));
    }

    machine_adc_obj_t *adc = adc_block_channel_find(self, pin_name);
    if (adc == NULL) {
        adc = adc_block_channel_alloc(self, pin_name);
        adc_obj_init(adc, self, pin_name, DEFAULT_ADC_ACQ_NS);
    }

    return adc;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_adcblock_connect_obj, 2, machine_adcblock_connect);

void mod_adcblock_deinit(void) {
    for (uint8_t i = 0; i < MAX_BLOCKS; i++) {
        if (adc_block[i] != NULL) {
            machine_adcblock_deinit(adc_block[i]);
        }
    }
}


static const mp_rom_map_elem_t machine_adcblock_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),  MP_ROM_PTR(&machine_adcblock_deinit_obj)},
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&machine_adcblock_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__),     MP_ROM_PTR(&machine_adcblock_deinit_obj) },
};
static MP_DEFINE_CONST_DICT(machine_adcblock_locals_dict, machine_adcblock_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_adcblock_type,
    MP_QSTR_ADCBlock,
    MP_TYPE_FLAG_NONE,
    make_new, machine_adcblock_make_new,
    print, machine_adcblock_print,
    locals_dict, &machine_adcblock_locals_dict
    );

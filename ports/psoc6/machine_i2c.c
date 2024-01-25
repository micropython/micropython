// std includes
#include <stdio.h>
#include <string.h>


// mpy includes
#include "extmod/modmachine.h"
#include "py/runtime.h"
#include "py/mphal.h"


// MTB includes
#include "cybsp.h"


// port-specific includes
#include "modmachine.h"
#include "machine_pin_phy.h"
#include "mplogger.h"

#define MAX_I2C              10 // TODO: Change as per bsp?
#define DEFAULT_I2C_FREQ     (400000)
#define PSOC_I2C_MASTER_MODE (CYHAL_I2C_MODE_MASTER)

#define i2c_assert_raise_val(msg, ret)   if (ret != CY_RSLT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

typedef struct _machine_i2c_obj_t {
    mp_obj_base_t base;
    cyhal_i2c_t i2c_obj;
    machine_pin_phy_obj_t *scl;
    machine_pin_phy_obj_t *sda;
    cyhal_i2c_cfg_t cfg;
} machine_i2c_obj_t;

machine_i2c_obj_t *i2c_obj[MAX_I2C] = { NULL };

static inline machine_i2c_obj_t *i2c_obj_alloc() {
    for (uint8_t i = 0; i < MAX_I2C; i++)
    {
        if (i2c_obj[i] == NULL) {
            i2c_obj[i] = mp_obj_malloc(machine_i2c_obj_t, &machine_i2c_type);
            return i2c_obj[i];
        }
    }

    return NULL;
}

static inline void i2c_obj_free(machine_i2c_obj_t *i2c_obj_ptr) {
    for (uint8_t i = 0; i < MAX_I2C; i++)
    {
        if (i2c_obj[i] == i2c_obj_ptr) {
            i2c_obj[i] = NULL;
        }
    }
}

STATIC void i2c_init(machine_i2c_obj_t *machine_i2c_obj, uint32_t freq_hz, int16_t slave_addr) {
    // Define the I2C master configuration structure

    // Set cgf frequency
    machine_i2c_obj->cfg.frequencyhal_hz = freq_hz;

    // Set mode master or slave
    if (slave_addr <= 0) {
        machine_i2c_obj->cfg.is_slave = CYHAL_I2C_MODE_MASTER;
        machine_i2c_obj->cfg.address = 0;
    } else {
        machine_i2c_obj->cfg.is_slave = CYHAL_I2C_MODE_SLAVE;
        machine_i2c_obj->cfg.address = slave_addr;
    }

    // Initialize I2C master, set the SDA and SCL pins and assign a new clock
    cy_rslt_t result = cyhal_i2c_init(&machine_i2c_obj->i2c_obj, machine_i2c_obj->sda->addr, machine_i2c_obj->scl->addr, NULL);
    i2c_assert_raise_val("I2C initialisation failed with return code %lx !", result);

    result = cyhal_i2c_configure(&machine_i2c_obj->i2c_obj, &(machine_i2c_obj->cfg));
    i2c_assert_raise_val("I2C initialisation failed with return code %lx !", result);
}

STATIC inline void i2c_sda_alloc(machine_i2c_obj_t *i2c_obj, mp_obj_t pin_name) {
    machine_pin_phy_obj_t *sda = pin_phy_realloc(pin_name, PIN_PHY_FUNC_I2C);

    if (sda == NULL) {
        size_t slen;
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SDA pin (%s) not found !"), mp_obj_str_get_data(pin_name, &slen));
    }

    i2c_obj->sda = sda;
}

STATIC inline void i2c_sda_free(machine_i2c_obj_t *i2c_obj) {
    pin_phy_free(i2c_obj->sda);
}

STATIC inline void i2c_scl_alloc(machine_i2c_obj_t *i2c_obj, mp_obj_t pin_name) {
    machine_pin_phy_obj_t *scl = pin_phy_realloc(pin_name, PIN_PHY_FUNC_I2C);

    if (scl == NULL) {
        size_t slen;
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SCL pin (%s) not found !"), mp_obj_str_get_data(pin_name, &slen));
    }

    i2c_obj->scl = scl;
}

STATIC inline void i2c_scl_free(machine_i2c_obj_t *i2c_obj) {
    pin_phy_free(i2c_obj->scl);
}

STATIC void machine_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2C(freq=%u, scl=%u, sda=%u)", self->cfg.frequencyhal_hz, self->scl->addr, self->sda->addr);
}

STATIC void machine_i2c_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_freq, ARG_scl, ARG_sda, ARG_addr };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = DEFAULT_I2C_FREQ} },
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_addr, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}}
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // get scl & sda pins & configure them
    if (args[ARG_scl].u_obj != mp_const_none) {
        i2c_scl_alloc(self, args[ARG_scl].u_obj);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("SCL pin must be provided"));
    }

    if (args[ARG_sda].u_obj != mp_const_none) {
        i2c_sda_alloc(self, args[ARG_sda].u_obj);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("SDA pin must be provided"));
    }

    // initialise I2C at cyhal level
    i2c_init(self, args[ARG_freq].u_int, args[ARG_addr].u_int);

}

mp_obj_t machine_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 4, true);

    machine_i2c_obj_t *self = i2c_obj_alloc();
    if (self == NULL) {
        return mp_const_none;
    }

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    machine_i2c_init((mp_obj_base_t *)self, n_args, args, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

STATIC void machine_i2c_deinit(mp_obj_base_t *self_in) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    cyhal_i2c_free(&(self->i2c_obj));
    i2c_sda_free(self);
    i2c_scl_free(self);
    i2c_obj_free(self);
}

STATIC int machine_i2c_transfer(mp_obj_base_t *self_in, uint16_t addr, size_t len, uint8_t *buf, unsigned int flags) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    cy_rslt_t result = CY_RSLT_SUCCESS;
    bool send_stop = (flags & MP_MACHINE_I2C_FLAG_STOP) ? true : false;
    uint32_t timeout = 0;

    if ((flags & MP_MACHINE_I2C_FLAG_READ) == MP_MACHINE_I2C_FLAG_READ) {
        result = cyhal_i2c_master_read(&self->i2c_obj, addr, buf, len, timeout, send_stop);
        i2c_assert_raise_val("cyhal_i2c_master_read failed with return code %lx !", result);

        return len;
    } else {
        // handle scan type bus checks and send stop
        if (buf == NULL) {
            result = cyhal_i2c_master_write(&self->i2c_obj, addr, buf, 0, 50, send_stop);

            if ((result != CY_RSLT_SUCCESS)) {
                // these 2 errors occur if nothing is attached to sda/scl, but they are pulled-up (0xaa2004) or not pulled-up (0xaa2003).
                // In the latter case, due to not reaction at all the timeout has to expire. Latency is therefore high.
                if (result != 0xaa2004 && result != 0xaa2003) {
                    i2c_assert_raise_val("cyhal_i2c_master_write failed with return code %lx !", result);
                }

                return 1;
            }

            return len;
        } else {
            result = cyhal_i2c_master_write(&self->i2c_obj, addr, buf, len, timeout, send_stop);
            i2c_assert_raise_val("cyhal_i2c_master_write failed with return code %lx !", result);
        }

        return len;
    }
}

// configure slave buffers
// i2c_master writes
// i2c_slave reads from write buffer
// i2c_slave does something about it
// i2c_master reads
// i2c_slave needs needs to have something there is the buffer

STATIC mp_obj_t machine_i2c_slave_configure_receive_buffer(mp_obj_t self_in /*pass a buffer*/) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    (void)self->scl;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_i2c_slave_conf_rx_buffer_obj, machine_i2c_slave_configure_receive_buffer);

// STATIC mp_obj_t machine_i2c_slave_configure_transmit_buffer(mp_obj_t self_in /*pass a buffer*/) {
//     machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);

//     return mp_const_none;
// }
// machine_i2c_slave_configure_rx_buffer() {

// }

// machine_i2c_skave_configure_tx_buffer() {

// }

// STATIC mp_obj_t machine_i2c_slave_configure_transmit_buffer(mp_obj_t self_in /*pass a buffer*/) {
//     machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);

//     return mp_const_none;
// }

// machine_i2c_slave_register_callback(callback_function) {

// }

// machine_i2c_slave_tx_register_callback(callback_function) {

// }




STATIC const mp_machine_i2c_p_t machine_i2c_p = {
    .init = machine_i2c_init,
    .deinit = machine_i2c_deinit,
    .transfer_single = machine_i2c_transfer,
    .transfer = mp_machine_i2c_transfer_adaptor
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2c_type,
    MP_QSTR_I2C,
    MP_TYPE_FLAG_NONE,
    make_new, machine_i2c_make_new,
    print, machine_i2c_print,
    protocol, &machine_i2c_p,
    locals_dict, &mp_machine_i2c_locals_dict
    );

#if MICROPY_PY_MACHINE_I2C_SLAVE

STATIC const mp_rom_map_elem_t machine_i2c_slave_locals_dict_table[] = {
    // Functions
    { MP_ROM_QSTR(MP_QSTR_slave_conf_rx_buffer),         MP_ROM_PTR(&machine_i2c_slave_conf_rx_buffer_obj) },

    // Const
    { MP_ROM_QSTR(MP_QSTR_RD_EVENT),                     MP_ROM_INT(CYHAL_I2C_SLAVE_READ_EVENT) },
    { MP_ROM_QSTR(MP_QSTR_WR_EVENT),                     MP_ROM_INT(CYHAL_I2C_SLAVE_WRITE_EVENT) },
    { MP_ROM_QSTR(MP_QSTR_RD_BUF_IN_FIFO_EVENT),         MP_ROM_INT(CYHAL_I2C_SLAVE_RD_IN_FIFO_EVENT) },
    { MP_ROM_QSTR(MP_QSTR_RD_BUF_EMPTY_EVENT),           MP_ROM_INT(CYHAL_I2C_SLAVE_RD_BUF_EMPTY_EVENT) },
    { MP_ROM_QSTR(MP_QSTR_RD_CMPLT_EVENT),               MP_ROM_INT(CYHAL_I2C_SLAVE_RD_CMPLT_EVENT) },
    { MP_ROM_QSTR(MP_QSTR_WR_CMPLT_EVENT),               MP_ROM_INT(CYHAL_I2C_SLAVE_WR_CMPLT_EVENT) },
    { MP_ROM_QSTR(MP_QSTR_ERR_EVENT),                    MP_ROM_INT(CYHAL_I2C_SLAVE_ERR_EVENT) },
};
STATIC MP_DEFINE_CONST_DICT(machine_i2c_slave_locals_dict, machine_i2c_slave_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2c_slave_type,
    MP_QSTR_I2CSlave,
    MP_TYPE_FLAG_NONE,
    make_new, machine_i2c_make_new,
    print, machine_i2c_print,
    locals_dict, &machine_i2c_slave_locals_dict
    );
#endif

void mod_i2c_deinit() {
    for (uint8_t i = 0; i < MAX_I2C; i++) {
        if (i2c_obj[i] != NULL) {
            machine_i2c_deinit((mp_obj_base_t *)(i2c_obj[i]));
        }
    }
}

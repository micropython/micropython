// std includes
#include <stdio.h>
#include <string.h>

// mpy includes
#include "py/runtime.h"
#include "extmod/modmachine.h"
#include "py/mperrno.h"

// MTB includes
#include "cybsp.h"
#include "cyhal.h"


// port-specific includes
#include "modmachine.h"
#include "machine_pin_phy.h"
#include "mplogger.h"

#define MAX_SPI              10 // TODO: Change as per bsp?

#define DEFAULT_SPI_BAUDRATE    (1000000)
#define DEFAULT_SPI_POLARITY    (0)
#define DEFAULT_SPI_PHASE       (0)
#define DEFAULT_SPI_BITS        (8)
#define DEFAULT_SPI_FIRSTBIT    (0) // msb


typedef struct _machine_spi_obj_t {
    mp_obj_base_t base;
    cyhal_spi_t spi_obj;
    uint8_t spi_id;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
    uint8_t firstbit;
    machine_pin_phy_obj_t *sck;
    machine_pin_phy_obj_t *mosi;
    machine_pin_phy_obj_t *miso;
    uint32_t baudrate;
} machine_spi_obj_t;

machine_spi_obj_t *spi_obj[MAX_SPI] = { NULL };

static inline machine_spi_obj_t *spi_obj_alloc() {
    for (uint8_t i = 0; i < MAX_SPI; i++)
    {
        if (spi_obj[i] == NULL) {
            spi_obj[i] = mp_obj_malloc(machine_spi_obj_t, &machine_spi_type);
            return spi_obj[i];
        }
    }

    return NULL;
}

static inline void spi_obj_free(machine_spi_obj_t *spi_obj_ptr) {
    for (uint8_t i = 0; i < MAX_SPI; i++)
    {
        if (spi_obj[i] == spi_obj_ptr) {
            spi_obj[i] = NULL;
        }
    }
}

STATIC inline void spi_sck_alloc(machine_spi_obj_t *spi_obj, mp_obj_t pin_name) {
    machine_pin_phy_obj_t *sck = pin_phy_realloc(pin_name, PIN_PHY_FUNC_SPI);

    if (sck == NULL) {
        size_t slen;
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SCK pin (%s) not found !"), mp_obj_str_get_data(pin_name, &slen));
    }

    spi_obj->sck = sck;
}

STATIC inline void spi_sck_free(machine_spi_obj_t *spi_obj) {
    pin_phy_free(spi_obj->sck);
}

STATIC inline void spi_mosi_alloc(machine_spi_obj_t *spi_obj, mp_obj_t pin_name) {
    machine_pin_phy_obj_t *mosi = pin_phy_realloc(pin_name, PIN_PHY_FUNC_SPI);

    if (mosi == NULL) {
        size_t slen;
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("MOSI pin (%s) not found !"), mp_obj_str_get_data(pin_name, &slen));
    }

    spi_obj->mosi = mosi;
}

STATIC inline void spi_mosi_free(machine_spi_obj_t *spi_obj) {
    pin_phy_free(spi_obj->mosi);
}

STATIC inline void spi_miso_alloc(machine_spi_obj_t *spi_obj, mp_obj_t pin_name) {
    machine_pin_phy_obj_t *miso = pin_phy_realloc(pin_name, PIN_PHY_FUNC_SPI);

    if (miso == NULL) {
        size_t slen;
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("MISO pin (%s) not found !"), mp_obj_str_get_data(pin_name, &slen));
    }

    spi_obj->miso = miso;
}

STATIC inline void spi_miso_free(machine_spi_obj_t *spi_obj) {
    pin_phy_free(spi_obj->miso);
}

// Function to select the mode
STATIC cyhal_spi_mode_t mode_select(uint8_t firstbit, uint8_t polarity, uint8_t phase) {

    cyhal_spi_mode_t mode;
    if (firstbit == 1) {
        if (polarity == 0) {
            if (phase == 0) {
                mode = CYHAL_SPI_MODE_00_LSB;
            } else {
                mode = CYHAL_SPI_MODE_01_LSB;
            }
        } else {
            if (phase == 0) {
                mode = CYHAL_SPI_MODE_10_LSB;
            } else {
                mode = CYHAL_SPI_MODE_11_LSB;
            }
        }
    } else {
        if (polarity == 0) {
            if (phase == 0) {
                mode = CYHAL_SPI_MODE_00_MSB;
            } else {
                mode = CYHAL_SPI_MODE_01_MSB;
            }
        } else {
            if (phase == 0) {
                mode = CYHAL_SPI_MODE_10_MSB;
            } else {
                mode = CYHAL_SPI_MODE_11_MSB;
            }
        }
    }
    return mode;
}

STATIC void machine_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPI(id=%u, baudrate=%u, polarity=%u, phase=%u, bits=%u, firstbit=%u, sck=%d, mosi=%d, miso=%d)",
        self->spi_id, self->baudrate, self->polarity,
        self->phase, self->bits, self->firstbit,
        self->sck->addr, self->mosi->addr, self->miso->addr);
}

mp_obj_t machine_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_sck, ARG_mosi, ARG_miso };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = DEFAULT_SPI_BAUDRATE} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_POLARITY} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_PHASE} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_BITS} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_FIRSTBIT} },
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get the SPI bus id.
    // There are no separate spi blocks. Now id is used to identify the master mode
    int spi_id = mp_obj_get_int(args[ARG_id].u_obj);

    if (spi_id != 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SPI(%d) doesn't exist"), spi_id);
    }

    // Get static peripheral object.
    machine_spi_obj_t *self = spi_obj_alloc();

    // set baudrate if provided
    if (args[ARG_baudrate].u_int != -1) {
        self->baudrate = args[ARG_baudrate].u_int;
    }

    // set polarity(CPOL) if provided
    if (args[ARG_polarity].u_int != -1) {
        self->polarity = args[ARG_polarity].u_int;
    }

    // set phase(CPHA) if provided
    if (args[ARG_phase].u_int != -1) {
        self->phase = args[ARG_phase].u_int;
    }

    // set bits if provided
    if (args[ARG_bits].u_int != -1) {
        self->bits = args[ARG_bits].u_int;
    }

    // set firstbit if provided(LSB or MSB first)
    if (args[ARG_firstbit].u_int != -1) {
        self->firstbit = args[ARG_firstbit].u_int;
    }

    // Set SCK/MOSI/MISO pins if configured.
    if (args[ARG_sck].u_obj != mp_const_none) {
        spi_sck_alloc(self, args[ARG_sck].u_obj);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("SCK pin must be provided"));
    }

    if (args[ARG_mosi].u_obj != mp_const_none) {
        spi_mosi_alloc(self, args[ARG_mosi].u_obj);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("MOSI pin must be provided"));
    }

    if (args[ARG_miso].u_obj != mp_const_none) {
        spi_miso_alloc(self, args[ARG_miso].u_obj);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("MISO pin must be provided"));
    }

    if (n_args > 1 || n_kw > 0) {
        cyhal_spi_mode_t mode = mode_select(self->firstbit, self->polarity, self->phase);
        // set the baudrate
        cyhal_spi_set_frequency(&self->spi_obj, self->baudrate);
        // Initialise the SPI peripheral if any arguments given, or it was not initialised previously.
        cy_rslt_t result = cyhal_spi_init(&self->spi_obj, self->mosi->addr, self->miso->addr, self->sck->addr, NC, NULL, self->bits, mode, false);
        if (result != CY_RSLT_SUCCESS) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SPI initialisation failed with return code %lx !"), result);
        }
    }
    return MP_OBJ_FROM_PTR(self);
}

STATIC void machine_spi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    // Parse the arguments.
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Reconfigure the baudrate if requested.
    if (args[ARG_baudrate].u_int != -1) {
        self->baudrate = args[ARG_baudrate].u_int;
        cyhal_spi_set_frequency(&self->spi_obj, self->baudrate);
    }

    if (args[ARG_polarity].u_int != -1) {
        self->polarity = args[ARG_polarity].u_int;
    }

    if (args[ARG_phase].u_int != -1) {
        self->phase = args[ARG_phase].u_int;
    }

    if (args[ARG_bits].u_int != -1) {
        self->bits = args[ARG_bits].u_int;
    }

    if (args[ARG_firstbit].u_int != -1) {
        self->firstbit = args[ARG_firstbit].u_int;
    }

    cyhal_spi_mode_t mode = mode_select(self->firstbit, self->polarity, self->phase);

    // since it's reinitialising, first it should be deinitialised & initilased with new parameters
    cyhal_spi_free(&self->spi_obj);

    cy_rslt_t result = cyhal_spi_init(&self->spi_obj, self->mosi->addr, self->miso->addr, self->sck->addr, NC, NULL, self->bits, mode, false);
    if (result != CY_RSLT_SUCCESS) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SPI initialisation failed with return code %lx !"), result);
    }
}

STATIC void machine_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;
    cy_rslt_t result;

    if (dest == NULL) {
        for (int i = 0; i < len; i++)
        {
            result = cyhal_spi_send(&self->spi_obj, src[i]);
            if (result != CY_RSLT_SUCCESS) {
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("cyhal_spi_send failed with return code %lx !"), result);
            }
        }
    } else if (src == NULL) {
        for (int i = 0; i < len; i++)
        {
            result = cyhal_spi_recv(&self->spi_obj, (uint32_t *)(dest + i));
            if (result != CY_RSLT_SUCCESS) {
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("cyhal_spi_recv failed with return code %lx !"), result);
            }
        }
    } else {
        result = cyhal_spi_transfer(&self->spi_obj, src, len, dest, len, 0xFF);
        if (result != CY_RSLT_SUCCESS) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("cyhal_spi_transf failed with return code %lx !"), result);
        }
    }

}

STATIC void machine_spi_deinit(mp_obj_base_t *self_in) {
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;
    cyhal_spi_free(&self->spi_obj);
    spi_sck_free(self);
    spi_mosi_free(self);
    spi_miso_free(self);
    spi_obj_free(self);
}

STATIC const mp_machine_spi_p_t machine_spi_p = {
    .init = machine_spi_init,
    .deinit = machine_spi_deinit,
    .transfer = machine_spi_transfer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, machine_spi_make_new,
    print, machine_spi_print,
    protocol, &machine_spi_p,
    locals_dict, &mp_machine_spi_locals_dict
    );

void mod_spi_deinit() {
    for (uint8_t i = 0; i < MAX_SPI; i++) {
        if (spi_obj[i] != NULL) {
            machine_spi_deinit((mp_obj_base_t *)(spi_obj[i]));
        }
    }
}

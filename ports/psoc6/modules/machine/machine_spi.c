// std includes
#include <stdio.h>
#include <string.h>

// mpy includes
#include "py/runtime.h"
#include "extmod/machine_spi.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/machine_spi.h"

// MTB includes
#include "cybsp.h"
#include "cyhal.h"


// port-specific includes
#include "drivers/machine/psoc6_gpio.h"
#include "modmachine.h"
#include "mplogger.h"
#include "pins.h"

#define MICROPY_HW_SPI_SCK  P9_2
#define MICROPY_HW_SPI_MOSI P9_0
#define MICROPY_HW_SPI_MISO P9_1


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
    uint8_t sck;
    uint8_t mosi;
    uint8_t miso;
    uint32_t baudrate;
} machine_spi_obj_t;

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
        self->sck, self->mosi, self->miso);
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
    machine_spi_obj_t *self = mp_obj_malloc(machine_spi_obj_t, &machine_spi_type);

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
        int sck = mp_hal_get_pin_obj(args[ARG_sck].u_obj);

        if (self->sck == -1) {
            size_t slen;
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SCK pin (%s) not found !"), mp_obj_str_get_data(args[ARG_sck].u_obj, &slen));
        }
        self->sck = sck;
    } else {
        self->sck = MICROPY_HW_SPI_SCK;
    }

    if (args[ARG_mosi].u_obj != mp_const_none) {
        int mosi = mp_hal_get_pin_obj(args[ARG_mosi].u_obj);

        if (self->mosi == -1) {
            size_t slen;
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("MOSI pin (%s) not found !"), mp_obj_str_get_data(args[ARG_mosi].u_obj, &slen));
        }
        self->mosi = mosi;
    } else {
        self->mosi = MICROPY_HW_SPI_MOSI;
    }

    if (args[ARG_miso].u_obj != mp_const_none) {
        int miso = mp_hal_get_pin_obj(args[ARG_miso].u_obj);

        if (self->miso == -1) {
            size_t slen;
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("MISO pin (%s) not found !"), mp_obj_str_get_data(args[ARG_miso].u_obj, &slen));
        }
        self->miso = miso;
    } else {
        self->miso = MICROPY_HW_SPI_MISO;

    }

    if (n_args > 1 || n_kw > 0) {
        cyhal_spi_mode_t mode = mode_select(self->firstbit, self->polarity, self->phase);
        // set the baudrate
        cyhal_spi_set_frequency(&self->spi_obj, self->baudrate);
        // Initialise the SPI peripheral if any arguments given, or it was not initialised previously.
        cy_rslt_t result = cyhal_spi_init(&self->spi_obj, self->mosi, self->miso, self->sck, NC, NULL, self->bits, mode, false);
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

    cy_rslt_t result = cyhal_spi_init(&self->spi_obj, self->mosi, self->miso, self->sck, NC, NULL, self->bits, mode, false);
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

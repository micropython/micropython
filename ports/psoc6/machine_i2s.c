#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/misc.h"
#include "py/stream.h"
#include "py/objstr.h"

// MTB includes
#include "cybsp.h"
#include "cyhal.h"

// port-specific includes
#include "extmod/modmachine.h"
#include "modmachine.h"
#include "mplogger.h"
#include <FreeRTOS.h>

#if MICROPY_PY_MACHINE_PSOC6_I2S

/* HFCLK1 Clock Divider */
 #define HFCLK1_CLK_DIVIDER  4u

/* Audio Subsystem Clock. Typical values depends on the desired sample rate:
 * 8KHz / 16 KHz / 32 KHz / 48 KHz    : 98.304 MHz
 * 22.05 KHz / 44.1 KHz               : 90.000 MHz
 */

/* Clock Settings */
#define AUDIO_SYS_CLOCK_HZ    98000000u   /* in Hz (Ideally 98.304 MHz) */
#define AUDIO_SYS_CLOCK_HZ_1  90000000u

uint32_t Audio_sys_clock_fz;
uint8_t clock_set;

void clock_init(void);

cyhal_clock_t audio_clock;
cyhal_clock_t pll_clock;
cyhal_clock_t system_clock;
cyhal_clock_t hf0_clock;

typedef enum {
    RX,
    TX
} i2s_mode_t;

typedef enum {
    MONO,
    STEREO
} format_t;

typedef enum {
    BLOCKING,
    NON_BLOCKING,
    ASYNCIO
} io_mode_t;


typedef struct _machine_i2s_obj_t {
    mp_obj_base_t base;
    cyhal_i2s_t i2s_obj;
    uint8_t i2s_id;
    mp_hal_pin_obj_t sck;
    mp_hal_pin_obj_t ws;
    mp_hal_pin_obj_t sd;
    i2s_mode_t mode;
    int8_t bits;
    format_t format;
    int32_t rate;
    int32_t ibuf;
    mp_obj_t callback_for_non_blocking;
    io_mode_t io_mode;
} machine_i2s_obj_t;


static void machine_i2s_init_helper(machine_i2s_obj_t *self, size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    enum {
        ARG_sck,
        ARG_ws,
        ARG_sd,
        ARG_mode,
        ARG_bits,
        ARG_format,
        ARG_rate,
        ARG_ibuf,
    };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_ws,       MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_sd,       MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mode,     MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = -1} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = -1} },
        { MP_QSTR_format,   MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = -1} },
        { MP_QSTR_rate,     MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = -1} },
        { MP_QSTR_ibuf,     MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = -1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_pos_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // are Pins valid?
    mp_hal_pin_obj_t sck = args[ARG_sck].u_obj == MP_OBJ_NULL ? -1 : mp_hal_get_pin_obj(args[ARG_sck].u_obj);
    mp_hal_pin_obj_t ws = args[ARG_ws].u_obj == MP_OBJ_NULL ? -1 : mp_hal_get_pin_obj(args[ARG_ws].u_obj);
    mp_hal_pin_obj_t sd = args[ARG_sd].u_obj == MP_OBJ_NULL ? -1 : mp_hal_get_pin_obj(args[ARG_sd].u_obj);


    // is Mode valid?
    i2s_mode_t i2s_mode = args[ARG_mode].u_int;
    if ((i2s_mode != RX) &&
        (i2s_mode != TX)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid mode"));
    }

    // is Bits valid?
    int8_t i2s_bits = args[ARG_bits].u_int;
    if ((i2s_bits != 16) &&
        (i2s_bits != 32)
        ) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid bits"));
    }

    // is Format valid?
    format_t i2s_format = args[ARG_format].u_int;
    if ((i2s_format != MONO) &&
        (i2s_format != STEREO)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid format"));
    }


    self->sck = sck;
    self->ws = ws;
    self->sd = sd;
    self->mode = i2s_mode;
    self->bits = i2s_bits;
    self->format = i2s_format;
    self->rate = args[ARG_rate].u_int;
    if (self->rate == (8000 || 16000 || 32000 || 48000)) {
        Audio_sys_clock_fz = AUDIO_SYS_CLOCK_HZ;
    } else {
        Audio_sys_clock_fz = AUDIO_SYS_CLOCK_HZ_1;
    }
    self->ibuf = args[ARG_ibuf].u_int;
    self->callback_for_non_blocking = MP_OBJ_NULL;
    self->io_mode = BLOCKING;

    if (clock_set == 0) {
        clock_init();
        cyhal_system_delay_ms(1);
    }


    if (i2s_mode == TX) {  // TX is a master
        cyhal_i2s_pins_t tx_pins = { .sck = self->sck, .ws = self->ws, .data = self->sd, .mclk = NC };
        cyhal_i2s_config_t tx_config =
        {
            .is_tx_slave = false,
            .is_rx_slave = false,
            .mclk_hz = 0,
            .channel_length = 32,
            .word_length = self->bits,
            .sample_rate_hz = self->rate,
        };
        cy_rslt_t result = cyhal_i2s_init(&self->i2s_obj, &tx_pins, NULL, &tx_config, &audio_clock);
        if (result != CY_RSLT_SUCCESS) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2S tx initialisation failed with return code %lx !"), result);
        }
    } else { // Rx is slave
        cyhal_i2s_pins_t rx_pins = { .sck = self->sck, .ws = self->ws, .data = self->sd, .mclk = NC };
        cyhal_i2s_config_t rx_config =
        {
            .is_tx_slave = false,
            .is_rx_slave = true,
            .mclk_hz = 0,
            .channel_length = 32,
            .word_length = self->bits,
            .sample_rate_hz = self->rate,
        };
        cy_rslt_t result = cyhal_i2s_init(&self->i2s_obj, NULL, &rx_pins, &rx_config, &audio_clock);
        if (result != CY_RSLT_SUCCESS) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2S rx initialisation failed with return code %lx !"), result);
        }
    }

}

static mp_obj_t machine_i2s_deinit(mp_obj_t self_in) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    cyhal_i2s_free(&self->i2s_obj);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_i2s_deinit_obj, machine_i2s_deinit);

static machine_i2s_obj_t *mp_machine_i2s_make_new_instance(mp_int_t i2s_id) {

    machine_i2s_obj_t *self;
    if (MP_STATE_PORT(machine_i2s_obj[i2s_id]) == NULL) {
        self = mp_obj_malloc(machine_i2s_obj_t, &machine_i2s_type);
        MP_STATE_PORT(machine_i2s_obj[i2s_id]) = self;
        self->i2s_id = i2s_id;
    } else {
        self = MP_STATE_PORT(machine_i2s_obj[i2s_id]);
        machine_i2s_deinit(MP_OBJ_FROM_PTR(self));
    }

    return self;
}


static void machine_i2s_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2S(id=%u,\n"
        "sck="MP_HAL_PIN_FMT ",\n"
        "ws="MP_HAL_PIN_FMT ",\n"
        "sd="MP_HAL_PIN_FMT ",\n"
        "mode=%u,\n"
        "bits=%u, format=%u,\n"
        "rate=%d, ibuf=%d)",
        self->i2s_id,
        mp_hal_pin_name(self->sck),
        mp_hal_pin_name(self->ws),
        mp_hal_pin_name(self->sd),
        self->mode,
        self->bits, self->format,
        self->rate, self->ibuf
        );
}

static mp_obj_t machine_i2s_make_new(const mp_obj_type_t *type, size_t n_pos_args, size_t n_kw_args, const mp_obj_t *args) {
    mp_arg_check_num(n_pos_args, n_kw_args, 1, MP_OBJ_FUN_ARGS_MAX, true);

    mp_int_t i2s_id = mp_obj_get_int(args[0]);
    if ((i2s_id != 0) && (i2s_id != 1)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid id"));
    }
    machine_i2s_obj_t *self = mp_machine_i2s_make_new_instance(i2s_id);

    // machine_i2s_obj_t *self = mp_obj_malloc(machine_i2s_obj_t, &machine_i2s_type);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw_args, args + n_pos_args);
    machine_i2s_init_helper(self, n_pos_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_i2s_init(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_i2s_obj_t *self = pos_args[0];
    machine_i2s_deinit(self);
    machine_i2s_init_helper(self, n_pos_args - 1, pos_args + 1, kw_args);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2s_init_obj, 1, machine_i2s_init);

// placeholder function not complete
static mp_obj_t machine_i2s_irq(mp_obj_t self_in, mp_obj_t handler) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (handler != mp_const_none && !mp_obj_is_callable(handler)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid callback"));
    }

    if (handler != mp_const_none) {
        self->io_mode = NON_BLOCKING;
        if (self->mode == RX) {
        }

    } else {
        self->io_mode = BLOCKING;
    }
    self->callback_for_non_blocking = handler;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_i2s_irq_obj, machine_i2s_irq);

static mp_obj_t machine_i2s_stop(mp_obj_t self_in) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->mode == TX) {
        cyhal_i2s_stop_tx(&self->i2s_obj);
    } else {
        cyhal_i2s_stop_rx(&self->i2s_obj);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_i2s_stop_obj, machine_i2s_stop);


// Shift() is typically used as a volume control.
// shift=1 increases volume by 6dB, shift=-1 decreases volume by 6dB
static mp_obj_t machine_i2s_shift(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buf, ARG_bits, ARG_shift};
    static const mp_arg_t allowed_args[] = {
        // { MP_QSTR_buf,    MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_bits,   MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_shift, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buf].u_obj, &bufinfo, MP_BUFFER_RW);

    int16_t *buf_16 = bufinfo.buf;
    int32_t *buf_32 = bufinfo.buf;

    uint8_t bits = args[ARG_bits].u_int;
    int8_t shift = args[ARG_shift].u_int;

    uint32_t num_audio_samples;
    switch (bits) {
        case 16:
            num_audio_samples = bufinfo.len / sizeof(uint16_t);
            break;

        case 32:
            num_audio_samples = bufinfo.len / sizeof(uint32_t);
            break;

        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid bits"));
            break;
    }

    for (uint32_t i = 0; i < num_audio_samples; i++) {
        switch (bits) {
            case 16:
                if (shift >= 0) {
                    buf_16[i] = buf_16[i] << shift;
                } else {
                    buf_16[i] = buf_16[i] >> abs(shift);
                }
                break;
            case 32:
                if (shift >= 0) {
                    buf_32[i] = buf_32[i] << shift;
                } else {
                    buf_32[i] = buf_32[i] >> abs(shift);
                }
                break;
        }
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2s_shift_fun_obj, 0, machine_i2s_shift);
static MP_DEFINE_CONST_STATICMETHOD_OBJ(machine_i2s_shift_obj, MP_ROM_PTR(&machine_i2s_shift_fun_obj));

void clock_init(void) {
    cyhal_clock_t clock_pll;
    cy_rslt_t result;

    clock_set = 1;

    /* Initialize, take ownership of PLL0/PLL */
    result = cyhal_clock_reserve(&clock_pll, &CYHAL_CLOCK_PLL[0]);
    if (CY_RSLT_SUCCESS != result) {
        mp_printf(&mp_plat_print, "Clock reserve fail");
    }

    /* Set the PLL0/PLL frequency to AUDIO_SYS_CLOCK_HZ based on AUDIO_IN_SAMPLE_FREQ */
    result = cyhal_clock_set_frequency(&clock_pll,  Audio_sys_clock_fz, NULL);
    if (CY_RSLT_SUCCESS != result) {
        mp_printf(&mp_plat_print, "set_freq fail");
    }

    /* If the PLL0/PLL clock is not already enabled, enable it */
    if (!cyhal_clock_is_enabled(&clock_pll)) {
        result = cyhal_clock_set_enabled(&clock_pll, true, true);
        if (CY_RSLT_SUCCESS != result) {
            mp_printf(&mp_plat_print, "PLL enable fail");
        }
    }

    /* Initialize, take ownership of CLK_HF1 */
    result = cyhal_clock_reserve(&audio_clock, &CYHAL_CLOCK_HF[1]);
    if (CY_RSLT_SUCCESS != result) {
        mp_printf(&mp_plat_print, "Clock Hf1 fail");
    }

    /* Source the audio subsystem clock (CLK_HF1) from PLL0/PLL */
    result = cyhal_clock_set_source(&audio_clock, &clock_pll);
    if (CY_RSLT_SUCCESS != result) {
        mp_printf(&mp_plat_print, "sourcing HF1 fail");
    }

    /* Set the divider for audio subsystem clock (CLK_HF1) */
    result = cyhal_clock_set_divider(&audio_clock, 2);
    if (CY_RSLT_SUCCESS != result) {
        mp_printf(&mp_plat_print, "dividing hf1 fail");
    }

    /* If the audio subsystem clock (CLK_HF1) is not already enabled, enable it */
    if (!cyhal_clock_is_enabled(&audio_clock)) {
        result = cyhal_clock_set_enabled(&audio_clock, true, true);
        if (CY_RSLT_SUCCESS != result) {
            mp_printf(&mp_plat_print, "enable hf1 fail");
        }
    }
}


static const mp_rom_map_elem_t machine_i2s_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_init),            MP_ROM_PTR(&machine_i2s_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto),        MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write),           MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),          MP_ROM_PTR(&machine_i2s_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),             MP_ROM_PTR(&machine_i2s_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop),           MP_ROM_PTR(&machine_i2s_stop_obj) },

    // Static method
    { MP_ROM_QSTR(MP_QSTR_shift),           MP_ROM_PTR(&machine_i2s_shift_obj) },

    // Constants
    { MP_ROM_QSTR(MP_QSTR_RX),              MP_ROM_INT(RX) },
    { MP_ROM_QSTR(MP_QSTR_TX),              MP_ROM_INT(TX) },
    { MP_ROM_QSTR(MP_QSTR_STEREO),          MP_ROM_INT(STEREO) },
    { MP_ROM_QSTR(MP_QSTR_MONO),            MP_ROM_INT(MONO) },
};
MP_DEFINE_CONST_DICT(machine_i2s_locals_dict, machine_i2s_locals_dict_table);



static mp_uint_t machine_i2s_stream_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    cy_rslt_t result;

    if (self->mode != RX) {
        *errcode = MP_EPERM;
        return MP_STREAM_ERROR;
    }

    if (size == 0) {
        return 0;
    }

    mp_buffer_info_t bufinfo;
    bufinfo.buf = (void *)buf_in;
    bufinfo.len = size;

    if (self->io_mode == NON_BLOCKING) { // just a placeholder
        // mp_buffer_info_t bufinfo;
        // bufinfo.buf = (void *)buf_in;
        // bufinfo.len = size;
        // result = cyhal_i2s_clear_rx(&self->i2s_obj);
        // result = cyhal_i2s_start_rx(&self->i2s_obj);
        // cyhal_system_delay_ms(100);
        // result = cyhal_i2s_read_async(&self->i2s_obj, bufinfo.buf, bufinfo.len);
        // if (result != CY_RSLT_SUCCESS) {
        //     mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2S read failed with return code %lx !"), result);
        // }
        return bufinfo.len;
    } else { // blocking mode
        result = cyhal_i2s_clear_rx(&self->i2s_obj);
        if (result != CY_RSLT_SUCCESS) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2S rx buffer clear failed with return code %lx !"), result);
        }
        result = cyhal_i2s_start_rx(&self->i2s_obj);
        if (result != CY_RSLT_SUCCESS) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2S reception failed with return code %lx !"), result);
        }
        cyhal_system_delay_ms(100);
        result = cyhal_i2s_read(&self->i2s_obj, bufinfo.buf, &bufinfo.len);
        if (result != CY_RSLT_SUCCESS) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2S read failed with return code %lx !"), result);
        }
        return bufinfo.len;
    }
}


static mp_uint_t machine_i2s_stream_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    cy_rslt_t result;

    if (self->mode != TX) {
        *errcode = MP_EPERM;
        return MP_STREAM_ERROR;
    }

    if (size == 0) {
        return 0;
    }

    mp_buffer_info_t bufinfo;
    bufinfo.buf = (void *)buf_in;
    bufinfo.len = size;

    if (self->io_mode == NON_BLOCKING) {  // Just a place holder
        /* Initiate a transfer */
        // cy_rslt_t result_write = cyhal_i2s_write_async(&self->i2s_obj, bufinfo.buf, bufinfo.len);
        // if (result_write != CY_RSLT_SUCCESS) {
        //     mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2S write failed with return code %lx !"), result_write);
        // }
        // /* Start the I2S TX */
        // cyhal_i2s_start_tx(&self->i2s_obj);
        return bufinfo.len;
    } else { // blocking mode
        cy_rslt_t result_write = cyhal_i2s_write(&self->i2s_obj, bufinfo.buf, &bufinfo.len);
        if (result_write != CY_RSLT_SUCCESS) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2S write failed with return code %lx !"), result_write);
        }
        result = cyhal_i2s_start_tx(&self->i2s_obj);
        if (result != CY_RSLT_SUCCESS) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2S transmission failed with return code %lx !"), result);
        }
        return bufinfo.len;
    }
}


static const mp_stream_p_t i2s_stream_p = {
    .read = machine_i2s_stream_read,
    .write = machine_i2s_stream_write,
    .ioctl = false,
    .is_text = false,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2s_type,
    MP_QSTR_I2S,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    make_new, machine_i2s_make_new,
    print, machine_i2s_print,
    protocol, &i2s_stream_p,
    locals_dict, &machine_i2s_locals_dict
    );

MP_REGISTER_ROOT_POINTER(void *machine_i2s_obj[2]);

#endif // MICROPY_PY_MACHINE_PSOC6_I2S

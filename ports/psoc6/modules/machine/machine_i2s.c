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
#include "drivers/machine/psoc6_gpio.h"
#include "modmachine.h"
#include "mplogger.h"
#include "pins.h"

#if MICROPY_PY_MACHINE_I2S

/* HFCLK1 Clock Divider */
#define HFCLK1_CLK_DIVIDER  4u

#define AUDIO_SYS_CLOCK_HZ  98000000u   /* in Hz (Ideally 98.304 MHz) */

void clock_init(void);

cyhal_clock_t audio_clock;
cyhal_clock_t pll_clock;
cyhal_clock_t fll_clock;
cyhal_clock_t system_clock;

#define I2S_RX_FRAME_SIZE_IN_BYTES (8)

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

STATIC void machine_i2s_init_helper(machine_i2s_obj_t *self, size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

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
        (i2s_bits != 32)) {
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
    self->ibuf = args[ARG_ibuf].u_int;
    self->callback_for_non_blocking = MP_OBJ_NULL;
    self->io_mode = BLOCKING;

    clock_init();
    cyhal_system_delay_ms(1);

    if (i2s_mode == TX) {
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
    } else {
        cyhal_i2s_pins_t rx_pins = { .sck = self->sck, .ws = self->ws, .data = self->sd, .mclk = NC };
        cyhal_i2s_config_t rx_config =
        {
            .is_tx_slave = false,
            .is_rx_slave = false,
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

STATIC mp_obj_t machine_i2s_deinit(mp_obj_t self_in) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    cyhal_i2s_free(&self->i2s_id);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_i2s_deinit_obj, machine_i2s_deinit);


STATIC void machine_i2s_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
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

STATIC mp_obj_t machine_i2s_make_new(const mp_obj_type_t *type, size_t n_pos_args, size_t n_kw_args, const mp_obj_t *args) {
    mp_arg_check_num(n_pos_args, n_kw_args, 1, MP_OBJ_FUN_ARGS_MAX, true);

    i2s_mode_t i2s_id = mp_obj_get_int(args[0]);
    if (i2s_id != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid id"));
    }

    machine_i2s_obj_t *self = mp_obj_malloc(machine_i2s_obj_t, &machine_i2s_type);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw_args, args + n_pos_args);
    machine_i2s_init_helper(self, n_pos_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_i2s_init(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_i2s_obj_t *self = pos_args[0];
    machine_i2s_deinit(self);
    machine_i2s_init_helper(self, n_pos_args - 1, pos_args + 1, kw_args);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2s_init_obj, 1, machine_i2s_init);

STATIC mp_obj_t machine_i2s_readinto(mp_obj_t self_in, mp_obj_t buf_in) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    cy_rslt_t result;
    if (self->mode != RX) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("I2S operation not supported"));
    }
    // get the buffer to read from
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_RW);

    if (self->io_mode == NON_BLOCKING) {
        cyhal_i2s_start_rx(&self->i2s_obj);
        result = cyhal_i2s_read_async(&self->i2s_obj, bufinfo.buf, bufinfo.len);
        if (result != CY_RSLT_SUCCESS) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2S read failed with return code %lx !"), result);
        }
    } else { // blocking or asyncio mode(asyncio - yet to implement)
        mp_printf(&mp_plat_print, "len1=%d\n", bufinfo.len);
        cyhal_i2s_start_rx(&self->i2s_obj);
        result = cyhal_i2s_read(&self->i2s_obj, bufinfo.buf, &bufinfo.len);
        mp_printf(&mp_plat_print, "len=%d\n", bufinfo.len);
        if (result != CY_RSLT_SUCCESS) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2S read failed with return code %lx !"), result);
        }
    }
    return MP_OBJ_NEW_SMALL_INT(bufinfo.len);
}
MP_DEFINE_CONST_FUN_OBJ_2(machine_i2s_readinto_obj, machine_i2s_readinto);

STATIC mp_obj_t machine_i2s_write(mp_obj_t self_in, mp_obj_t buf_in) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    cy_rslt_t result_write;

    if (self->mode != TX) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("I2S operation not supported"));
    }

    // get the buffer to write from
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    if (self->io_mode == NON_BLOCKING) {

        /* Start the I2S TX */
        cyhal_i2s_start_tx(&self->i2s_obj);
        /* Initiate a transfer */
        result_write = cyhal_i2s_write_async(&self->i2s_obj, bufinfo.buf, bufinfo.len);
        if (result_write != CY_RSLT_SUCCESS) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2S write failed with return code %lx !"), result_write);
        }


    } else { // blocking or asyncio mode

        /* Start the I2S TX */
        cyhal_i2s_start_tx(&self->i2s_obj);
        /* Initiate a transfer */
        result_write = cyhal_i2s_write(&self->i2s_obj, bufinfo.buf, &bufinfo.len);
        if (result_write != CY_RSLT_SUCCESS) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2S write failed with return code %lx !"), result_write);
        }
    }
    return MP_OBJ_NEW_SMALL_INT(bufinfo.len);
}
MP_DEFINE_CONST_FUN_OBJ_2(machine_i2s_write_obj, machine_i2s_write);

STATIC mp_obj_t machine_i2s_irq(mp_obj_t self_in, mp_obj_t handler) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (handler != mp_const_none && !mp_obj_is_callable(handler)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid callback"));
    }

    if (handler != mp_const_none) {
        self->io_mode = NON_BLOCKING;
        if (self->mode == RX) {
            /* Register a callback and set the callback argument to be a pointer to the I2S object,
            * so that we can easily reference it from the callback handler.
            */
            // cyhal_i2s_register_callback(&i2s, &i2s_event_handler_receive, &i2s);
            /* Subscribe to the async complete event so that we can queue up another transfer when this one completes */
            // cyhal_i2s_enable_event(&i2s, CYHAL_I2S_ASYNC_RX_COMPLETE, CYHAL_ISR_PRIORITY_DEFAULT, true);
            /* Configure asynchronous transfers to use DMA to free up the CPU during transfers */
            // cyhal_i2s_set_async_mode(&i2s, CYHAL_ASYNC_DMA, CYHAL_DMA_PRIORITY_DEFAULT);
        } else {
            // cyhal_i2s_register_callback(&i2s, &i2s_event_handler_transmit_streaming, &i2s);
            /* Subscribe to the async complete event so that we can queue up another transfer when this one completes */
            // cyhal_i2s_enable_event(&i2s, CYHAL_I2S_ASYNC_TX_COMPLETE, CYHAL_ISR_PRIORITY_DEFAULT, true);
            /* Configure asynchronous transfers to use DMA to free up the CPU during transfers */
            // cyhal_i2s_set_async_mode(&i2s, CYHAL_ASYNC_DMA, CYHAL_DMA_PRIORITY_DEFAULT);
        }

    } else {
        self->io_mode = BLOCKING;
    }

    self->callback_for_non_blocking = handler;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_i2s_irq_obj, machine_i2s_irq);

// Shift() is typically used as a volume control.
// shift=1 increases volume by 6dB, shift=-1 decreases volume by 6dB
STATIC mp_obj_t machine_i2s_shift(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2s_shift_fun_obj, 0, machine_i2s_shift);
STATIC MP_DEFINE_CONST_STATICMETHOD_OBJ(machine_i2s_shift_obj, MP_ROM_PTR(&machine_i2s_shift_fun_obj));

void clock_init(void) {
    /* Initialize the PLL */
    cyhal_clock_reserve(&pll_clock, &CYHAL_CLOCK_PLL[0]);
    cyhal_clock_set_frequency(&pll_clock, AUDIO_SYS_CLOCK_HZ, NULL);
    cyhal_clock_set_enabled(&pll_clock, true, true);

    /* Initialize the audio subsystem clock (HFCLK1) */
    cyhal_clock_reserve(&audio_clock, &CYHAL_CLOCK_HF[1]);
    cyhal_clock_set_source(&audio_clock, &pll_clock);

    /* Drop HFCK1 frequency for power savings */
    cyhal_clock_set_divider(&audio_clock, HFCLK1_CLK_DIVIDER);
    cyhal_clock_set_enabled(&audio_clock, true, true);

    /* Initialize the system clock (HFCLK0) */
    cyhal_clock_reserve(&system_clock, &CYHAL_CLOCK_HF[0]);
    cyhal_clock_set_source(&system_clock, &pll_clock);

    /* Disable the FLL for power savings */
    cyhal_clock_reserve(&fll_clock, &CYHAL_CLOCK_FLL);
    cyhal_clock_set_enabled(&fll_clock, false, true);
}


STATIC const mp_rom_map_elem_t machine_i2s_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_init),            MP_ROM_PTR(&machine_i2s_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto),        MP_ROM_PTR(&machine_i2s_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write),           MP_ROM_PTR(&machine_i2s_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),          MP_ROM_PTR(&machine_i2s_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),             MP_ROM_PTR(&machine_i2s_irq_obj) },

    // Static method
    { MP_ROM_QSTR(MP_QSTR_shift),           MP_ROM_PTR(&machine_i2s_shift_obj) },

    // Constants
    { MP_ROM_QSTR(MP_QSTR_RX),              MP_ROM_INT(RX) },
    { MP_ROM_QSTR(MP_QSTR_TX),              MP_ROM_INT(TX) },
    { MP_ROM_QSTR(MP_QSTR_STEREO),          MP_ROM_INT(STEREO) },
    { MP_ROM_QSTR(MP_QSTR_MONO),            MP_ROM_INT(MONO) },
};
MP_DEFINE_CONST_DICT(machine_i2s_locals_dict, machine_i2s_locals_dict_table);


MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2s_type,
    MP_QSTR_I2S,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    make_new, machine_i2s_make_new,
    print, machine_i2s_print,
    locals_dict, &machine_i2s_locals_dict
    );

MP_REGISTER_ROOT_POINTER(void *machine_i2s_obj[2]);

#endif // MICROPY_PY_MACHINE_I2S

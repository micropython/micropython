/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2021 Robert Hammelrath
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#if MICROPY_PY_MACHINE_QECNT

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/objint.h"
#include "shared/runtime/mpirq.h"
#include "modmachine.h"
#include "fsl_clock.h"
#include "fsl_enc.h"
#include "fsl_xbara.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"

typedef struct _machine_encoder_obj_t {
    mp_obj_base_t base;
    ENC_Type *instance;
    int8_t id;
    bool active;
    uint8_t input_a;
    uint8_t input_b;
    uint8_t mode;
    bool is_signed;
    uint8_t match_pin;
    uint32_t cpc;
    uint32_t filter;
    uint16_t status;
    uint16_t requested_irq;
    mp_irq_obj_t *irq;
    enc_config_t enc_config;
} machine_encoder_obj_t;

typedef struct _encoder_xbar_signal_t {
    xbar_output_signal_t enc_input_a;
    xbar_output_signal_t enc_input_b;
    xbar_output_signal_t enc_index;
    xbar_output_signal_t enc_home;
    xbar_output_signal_t enc_trigger;
    xbar_input_signal_t enc_match;
} encoder_xbar_signal_t;

#define ENCODER_TRIGGER_MATCH      (kENC_PositionCompareFlag)
#define ENCODER_TRIGGER_ROLL_OVER  (kENC_PositionRollOverFlag)
#define ENCODER_TRIGGER_ROLL_UNDER (kENC_PositionRollUnderFlag)
#define ENCODER_ALL_INTERRUPTS     (0x7f)

#define XBAR_IN                    (1)
#define XBAR_OUT                   (0)

#define COUNTER_UP                 (-2)
#define COUNTER_DOWN               (-3)
#define MODE_ENCODER               (0)
#define MODE_COUNTER               (1)

STATIC void encoder_deinit_single(machine_encoder_obj_t *self);

#if defined MIMXRT117x_SERIES

#define XBAR_ENC_DIR_OFFSET_1      (4)
#define XBAR_ENC_DIR_REGISTER_1    GPR20
#define XBAR_ENC_DIR_OFFSET_2      (32)
#define XBAR_ENC_DIR_REGISTER_2    GPR21
#define XBAR_OUT_MIN               (4)
#define XBAR_OUT_MAX               (42)
#define XBAR_STRING                "XBAR1_INOUT"
#define XBAR_STRING_LEN            strlen(XBAR_STRING)

static encoder_xbar_signal_t xbar_signal_table[FSL_FEATURE_SOC_ENC_COUNT] = {
    { kXBARA1_OutputDec1Phasea,
      kXBARA1_OutputDec1Phaseb,
      kXBARA1_OutputDec1Index,
      kXBARA1_OutputDec1Home,
      kXBARA1_OutputDec1Trigger,
      kXBARA1_InputDec1PosMatch },

    { kXBARA1_OutputDec2Phasea,
      kXBARA1_OutputDec2Phaseb,
      kXBARA1_OutputDec2Index,
      kXBARA1_OutputDec2Home,
      kXBARA1_OutputDec2Trigger,
      kXBARA1_InputDec2PosMatch },

    { kXBARA1_OutputDec3Phasea,
      kXBARA1_OutputDec3Phaseb,
      kXBARA1_OutputDec3Index,
      kXBARA1_OutputDec3Home,
      kXBARA1_OutputDec3Trigger,
      kXBARA1_InputDec3PosMatch },

    { kXBARA1_OutputDec4Phasea,
      kXBARA1_OutputDec4Phaseb,
      kXBARA1_OutputDec4Index,
      kXBARA1_OutputDec4Home,
      kXBARA1_OutputDec4Trigger,
      kXBARA1_InputDec4PosMatch },
};

#else // defined MIMXRT117x_SERIES

#if !defined(XBAR_ENC_DIR_OFFSET)
#define XBAR_ENC_DIR_OFFSET        (12)
#define XBAR_ENC_DIR_REGISTER      GPR6
#define XBAR_OUT_MIN               (4)
#define XBAR_OUT_MAX               (19)
#endif
#define XBAR_STRING                "XBAR_INOUT"
#define XBAR_STRING_LEN            strlen(XBAR_STRING)

static encoder_xbar_signal_t xbar_signal_table[FSL_FEATURE_SOC_ENC_COUNT] = {
    { kXBARA1_OutputEnc1PhaseAInput,
      kXBARA1_OutputEnc1PhaseBInput,
      kXBARA1_OutputEnc1Index,
      kXBARA1_OutputEnc1Home,
      kXBARA1_OutputEnc1Trigger,
      kXBARA1_InputEnc1PosMatch },

    #if FSL_FEATURE_SOC_ENC_COUNT > 1

    { kXBARA1_OutputEnc2PhaseAInput,
      kXBARA1_OutputEnc2PhaseBInput,
      kXBARA1_OutputEnc2Index,
      kXBARA1_OutputEnc2Home,
      kXBARA1_OutputEnc2Trigger,
      kXBARA1_InputEnc2PosMatch },

    #if FSL_FEATURE_SOC_ENC_COUNT > 2

    { kXBARA1_OutputEnc3PhaseAInput,
      kXBARA1_OutputEnc3PhaseBInput,
      kXBARA1_OutputEnc3Index,
      kXBARA1_OutputEnc3Home,
      kXBARA1_OutputEnc3Trigger,
      kXBARA1_InputEnc3PosMatch },

    { kXBARA1_OutputEnc4PhaseAInput,
      kXBARA1_OutputEnc4PhaseBInput,
      kXBARA1_OutputEnc4Index,
      kXBARA1_OutputEnc4Home,
      kXBARA1_OutputEnc4Trigger,
      kXBARA1_InputEnc4PosMatch },

    #endif
    #endif
};
#endif // defined MIMXRT117x_SERIES

static machine_encoder_obj_t *encoder_table[FSL_FEATURE_SOC_ENC_COUNT];
static ENC_Type *enc_instances[] = ENC_BASE_PTRS;
static IRQn_Type enc_irqn[] = ENC_COMPARE_IRQS;

__attribute__((section(".ram_functions"))) void irq_callback(int irq_num) {
    machine_encoder_obj_t *self = encoder_table[irq_num];
    if (self != NULL) {
        self->status = ENC_GetStatusFlags(self->instance);
        // In case of a position match event, disable that interrupt such that is is only handled
        // once until enabled again. This is needed since otherwise the match interrupt will
        // be triggered again as long as the match condition is true.
        if (self->status & kENC_PositionCompareFlag) {
            ENC_DisableInterrupts(self->instance, kENC_PositionCompareInerruptEnable);
        }
        ENC_ClearStatusFlags(self->instance, self->status);
        __DSB();
        mp_irq_handler(self->irq);
    }
}

__attribute__((section(".ram_functions"))) void ENC1_IRQHandler(void) {
    irq_callback(0);
}

#if FSL_FEATURE_SOC_ENC_COUNT > 1

__attribute__((section(".ram_functions"))) void ENC2_IRQHandler(void) {
    irq_callback(1);
}

#if FSL_FEATURE_SOC_ENC_COUNT > 2
__attribute__((section(".ram_functions"))) void ENC3_IRQHandler(void) {
    irq_callback(2);
}

__attribute__((section(".ram_functions"))) void ENC4_IRQHandler(void) {
    irq_callback(3);
}
#endif
#endif

STATIC void mp_machine_encoder_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_encoder_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print,
        self->is_signed ? "%s %d cpc=%lu match=%ld filter=%luns>\n" : "%s %d cpc=%lu match=%lu filter=%luns>\n",
        self->mode == MODE_ENCODER ? "<Encoder" : "<Counter",
        self->id, self->cpc, self->enc_config.positionCompareValue, self->filter);
}

// Utility functions
//

STATIC void encoder_set_iomux(const machine_pin_obj_t *pin, const machine_pin_af_obj_t *af) {
    IOMUXC_SetPinMux(pin->muxRegister, af->af_mode, af->input_register, af->input_daisy, pin->configRegister, 0U);
    IOMUXC_SetPinConfig(pin->muxRegister, af->af_mode, af->input_register, af->input_daisy, pin->configRegister, 0x10B0U);
}

// decode the AF objects module and Port numer. Returns NULL if it is not a XBAR object
STATIC const machine_pin_af_obj_t *af_name_decode_xbar(const machine_pin_af_obj_t *af_obj,
    xbar_input_signal_t *io_number) {
    const char *str;
    size_t len;
    size_t xlen = XBAR_STRING_LEN;
    str = (char *)qstr_data(af_obj->name, &len);
    // test for the name starting with XBAR
    if (len < (xlen + 2) || strncmp(str, XBAR_STRING, xlen) != 0) {
        return NULL;
    }
    // Get I/O number, e.g. XBAR_INOUT03
    *io_number = (str[xlen] - '0') * 10 + (str[xlen + 1] - '0');
    return af_obj;
}

STATIC const machine_pin_af_obj_t *find_xbar_af(const machine_pin_obj_t *pin, xbar_input_signal_t *io_number) {
    const machine_pin_af_obj_t *af = NULL;
    for (int i = 0; i < pin->af_list_len; ++i) {
        af = af_name_decode_xbar(&(pin->af_list[i]), io_number);
        if (af != NULL) {
            return af;
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("invalid input Pin"));
}

STATIC uint8_t connect_pin_to_encoder(mp_obj_t desc, xbar_output_signal_t encoder_signal, uint8_t direction) {
    xbar_input_signal_t xbar_pin;
    const machine_pin_obj_t *pin = pin_find(desc);
    const machine_pin_af_obj_t *af = find_xbar_af(pin,  &xbar_pin);
    encoder_set_iomux(pin, af);
    if (direction == XBAR_IN) {
        XBARA_SetSignalsConnection(XBARA1, xbar_pin, encoder_signal);
    } else {
        // No API here, so do basic Register access.
        #if defined MIMXRT117x_SERIES
        if (xbar_pin >= XBAR_OUT_MIN && xbar_pin <= XBAR_OUT_MAX) {
            if (xbar_pin < XBAR_ENC_DIR_OFFSET_2) {
                IOMUXC_GPR->XBAR_ENC_DIR_REGISTER_1 |= 1 << (xbar_pin - XBAR_ENC_DIR_OFFSET_1);
                XBARA_SetSignalsConnection(XBARA1, encoder_signal, xbar_pin);
            } else {
                IOMUXC_GPR->XBAR_ENC_DIR_REGISTER_2 |= 1 << (xbar_pin - XBAR_ENC_DIR_OFFSET_2);
                XBARA_SetSignalsConnection(XBARA1, encoder_signal, xbar_pin);
            }
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid match Pin"));
        }
        #else
        if (xbar_pin >= XBAR_OUT_MIN && xbar_pin <= XBAR_OUT_MAX) {
            IOMUXC_GPR->XBAR_ENC_DIR_REGISTER |= 1 << (xbar_pin + XBAR_ENC_DIR_OFFSET); // Compare the offset 12 with other MCU
            XBARA_SetSignalsConnection(XBARA1, encoder_signal, xbar_pin);
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid match Pin"));
        }
        #endif  // defined MIMXRT117x_SERIES
    }
    return xbar_pin;
}

STATIC void clear_encoder_registers(machine_encoder_obj_t *self) {
    // Create a High pulse on the Trigger input, clearing Position, Revolution and Hold registers.
    XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicHigh, xbar_signal_table[self->id].enc_trigger);
    XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicLow, xbar_signal_table[self->id].enc_trigger);
}

//
// Functions for configuring the ENC Device
//
// Calculate the filter parameters based on a filter_ns value, telling the shortest
// pulse that will be detected.
//
STATIC uint32_t calc_filter(uint32_t filter_ns, uint16_t *count, uint16_t *period) {

    #if defined MIMXRT117x_SERIES
    uint32_t freq_khz = CLOCK_GetRootClockFreq(kCLOCK_Root_Bus) / 1000;
    #else
    uint32_t freq_khz = CLOCK_GetIpgFreq() / 1000;
    #endif

    uint32_t cycles = (filter_ns * (freq_khz / 1000)) / 1000;
    if (cycles == 0) {
        // Set filter off
        *count = 0;
        *period = 0;
    } else {
        uint16_t pmax = cycles / 10;
        if (pmax > 255) {
            pmax = 255;
        }
        if (pmax == 0) {
            pmax = 1;
        }
        uint16_t cnt;
        cnt = cycles / pmax;
        if (cnt > 10) {
            cnt = 10;
        }
        *count = cnt >= 3 ? cnt - 3 : 0;
        *period = pmax;
    }
    return ((1000000000 / freq_khz) + 1) * (*count + 3) * *period / 1000;
}

// Micropython API functions
//
STATIC void mp_machine_encoder_init_helper_common(machine_encoder_obj_t *self,
    mp_arg_val_t args[], enc_config_t *enc_config) {

    enum { ARG_match_pin, ARG_filter_ns, ARG_cpc, ARG_signed, ARG_index };

    // Check for a Match pin for the compare match signal
    if (args[ARG_match_pin].u_obj != MP_ROM_INT(-1)) {
        if (args[ARG_match_pin].u_obj != mp_const_none) {
            self->match_pin = connect_pin_to_encoder(args[ARG_match_pin].u_obj, xbar_signal_table[self->id].enc_match, XBAR_OUT);
        } else {
            // Disconnect the XBAR from the output by switching it to an input.
            #if defined MIMXRT117x_SERIES
            if (self->match_pin < XBAR_ENC_DIR_OFFSET_2) {
                IOMUXC_GPR->XBAR_ENC_DIR_REGISTER_1 &= ~(1 << (self->match_pin - XBAR_ENC_DIR_OFFSET_1));
            } else {
                IOMUXC_GPR->XBAR_ENC_DIR_REGISTER_2 &= ~(1 << (self->match_pin - XBAR_ENC_DIR_OFFSET_2));
            }
            #else
            IOMUXC_GPR->XBAR_ENC_DIR_REGISTER &= ~(1 << (self->match_pin + XBAR_ENC_DIR_OFFSET));
            #endif
        }
    }

    if (args[ARG_filter_ns].u_int >= 0) {
        self->filter = calc_filter(args[ARG_filter_ns].u_int,
            &(enc_config->filterCount), &(enc_config->filterSamplePeriod));
    }

    if (args[ARG_cpc].u_obj != mp_const_none) {
        uint32_t cpc = mp_obj_int_get_truncated(args[ARG_cpc].u_obj);
        self->cpc = cpc;
        if (cpc == 0) {
            enc_config->enableModuloCountMode = false;
            enc_config->positionModulusValue = 0;
            enc_config->positionInitialValue = 0;
            self->is_signed = false;
        } else {
            enc_config->enableModuloCountMode = true;
            enc_config->positionModulusValue = cpc - 1;
            self->is_signed = true;
        }
    }

    if (args[ARG_signed].u_int >= 0) {
        self->is_signed = !!args[ARG_signed].u_int;
    }

    // Count cycles on RollOverModulus or index pulse
    if (args[ARG_index].u_obj != MP_ROM_INT(-1)) {
        if (args[ARG_index].u_obj != mp_const_none) {
            connect_pin_to_encoder(args[ARG_index].u_obj, xbar_signal_table[self->id].enc_index, XBAR_IN);
            enc_config->revolutionCountCondition = kENC_RevolutionCountOnINDEXPulse;
            enc_config->INDEXTriggerMode = kENC_INDEXTriggerOnRisingEdge;
        } else {
            enc_config->revolutionCountCondition = kENC_RevolutionCountOnRollOverModulus;
            XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicLow, xbar_signal_table[self->id].enc_index);
        }
    }

    // Initialize the ENC module and start
    ENC_Init(self->instance, enc_config);
    clear_encoder_registers(self);
    ENC_ClearStatusFlags(self->instance, 0xff); // Clear all status flags
    self->active = true;
}

STATIC void mp_machine_encoder_init_helper(machine_encoder_obj_t *self,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_phase_a, ARG_phase_b, ARG_home,
           ARG_match_pin, ARG_filter_ns, ARG_cpc, ARG_signed, ARG_index};

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_phase_a, MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_phase_b, MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_home, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_INT(-1)} },
        { MP_QSTR_match_pin, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_INT(-1)} },
        { MP_QSTR_filter_ns, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_cpc, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_signed, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_index, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_INT(-1)} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Process the Encoder specific keyword arguments
    // Get referred Pin object(s) and connect them to the encoder
    if (args[ARG_phase_a].u_obj != mp_const_none) {
        self->input_a = connect_pin_to_encoder(args[ARG_phase_a].u_obj, xbar_signal_table[self->id].enc_input_a, XBAR_IN);
    }
    if (args[ARG_phase_b].u_obj != mp_const_none) {
        self->input_b = connect_pin_to_encoder(args[ARG_phase_b].u_obj, xbar_signal_table[self->id].enc_input_b, XBAR_IN);
    }
    // Check for valid input pins
    if (self->input_a == 0 || self->input_b == 0 || self->input_a == self->input_b) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid or missing input pins"));
    }

    // Check for a Home pin, resetting the counters
    if (args[ARG_home].u_obj != MP_ROM_INT(-1)) {
        if (args[ARG_home].u_obj != mp_const_none) {
            connect_pin_to_encoder(args[ARG_home].u_obj, xbar_signal_table[self->id].enc_home, XBAR_IN);
            self->enc_config.HOMETriggerMode = kENC_HOMETriggerOnRisingEdge;
        } else {
            XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicLow, xbar_signal_table[self->id].enc_home);
        }
    }

    // Set the common options
    mp_machine_encoder_init_helper_common(self, args + ARG_match_pin, &self->enc_config);

    ENC_DoSoftwareLoadInitialPositionValue(self->instance); /* Update the position counter with initial value. */
}

// Qencoder(id, input_a, input_b, [args])
STATIC mp_obj_t mp_machine_encoder_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // Check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    CLOCK_EnableClock(kCLOCK_Iomuxc); // just in case it was not set yet
    XBARA_Init(XBARA1);

    uint8_t id = mp_obj_get_int(args[0]);
    if (id < 0 || id >= FSL_FEATURE_SOC_ENC_COUNT) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid encoder/counter id"));
    }
    // check, if the encoder is already in use, and if yes, dinit it
    if (encoder_table[id] != NULL) {
        encoder_deinit_single(encoder_table[id]);
    }

    // Connect the trigger input to low level
    XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicLow, xbar_signal_table[id].enc_trigger);

    // Create and populate the Qencoder object.
    machine_encoder_obj_t *self = m_new_obj(machine_encoder_obj_t);
    encoder_table[id] = self;
    self->id = id;
    self->input_a = 0;
    self->input_b = 0;
    self->base.type = &machine_encoder_type;
    self->instance = enc_instances[id + 1];
    self->cpc = 0;
    self->status = 0;
    self->irq = NULL;
    self->match_pin = 0;
    self->is_signed = true;
    self->mode = MODE_ENCODER;

    // Set defaults for ENC Config
    ENC_GetDefaultConfig(&self->enc_config);
    self->enc_config.enableTRIGGERClearPositionCounter = true;
    self->enc_config.revolutionCountCondition = kENC_RevolutionCountOnRollOverModulus;

    // Process the remaining parameters
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_encoder_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

STATIC void encoder_deinit_single(machine_encoder_obj_t *self) {
    if (self->active) {
        if (self->irq && self->irq->handler) {
            DisableIRQ(enc_irqn[self->id + 1]);
            ENC_DisableInterrupts(self->instance, ENCODER_ALL_INTERRUPTS);
        }
        if (self->match_pin != 0) {
            // Disconnect the XBAR from the output by switching it to an input.
            #if defined MIMXRT117x_SERIES
            if (self->match_pin < XBAR_ENC_DIR_OFFSET_2) {
                IOMUXC_GPR->XBAR_ENC_DIR_REGISTER_1 &= ~(1 << (self->match_pin - XBAR_ENC_DIR_OFFSET_1));
            } else {
                IOMUXC_GPR->XBAR_ENC_DIR_REGISTER_2 &= ~(1 << (self->match_pin - XBAR_ENC_DIR_OFFSET_2));
            }
            #else
            IOMUXC_GPR->XBAR_ENC_DIR_REGISTER &= ~(1 << (self->match_pin + XBAR_ENC_DIR_OFFSET));
            #endif
        }
        ENC_Deinit(self->instance);
    }
    self->active = false;
}

// encoder_deinit_all()
void machine_encoder_deinit_all(void) {
    for (int i = 0; i < ARRAY_SIZE(encoder_table); i++) {
        if (encoder_table[i] != NULL) {
            encoder_deinit_single(encoder_table[i]);
            encoder_table[i] = NULL;
        }
    }
}

// encoder.deinit()
STATIC mp_obj_t machine_encoder_deinit(mp_obj_t self_in) {
    encoder_deinit_single(MP_OBJ_TO_PTR(self_in));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_encoder_deinit_obj, machine_encoder_deinit);

// encoder.status()
mp_obj_t machine_encoder_status(mp_obj_t self_in) {
    machine_encoder_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->status & (self->requested_irq | kENC_LastCountDirectionFlag));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_encoder_status_obj, machine_encoder_status);

// encoder.value([value])
STATIC mp_obj_t machine_encoder_value(size_t n_args, const mp_obj_t *args) {
    machine_encoder_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (!self->active) {
        mp_raise_ValueError(MP_ERROR_TEXT("device stopped"));
    }
    uint32_t actual_value = ENC_GetPositionValue(self->instance);
    if (n_args > 1) {
        // Set the encoder position value and clear the rev counter.
        uint32_t value = mp_obj_int_get_truncated(args[1]);
        clear_encoder_registers(self);
        // Set the position and rev register
        ENC_SetInitialPositionValue(self->instance, value);
        ENC_DoSoftwareLoadInitialPositionValue(self->instance);
        // Reset the INIT Value
        ENC_SetInitialPositionValue(self->instance, 0);
    }
    // Get the position as signed or unsigned 32 bit value.
    if (self->is_signed) {
        return mp_obj_new_int((int32_t)actual_value);
    } else {
        return mp_obj_new_int_from_uint(actual_value);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_encoder_value_obj, 1, 2, machine_encoder_value);

// encoder.cycles([value])
STATIC mp_obj_t machine_encoder_cycles(size_t n_args, const mp_obj_t *args) {
    machine_encoder_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (!self->active) {
        mp_raise_ValueError(MP_ERROR_TEXT("device stopped"));
    }

    int16_t cycles = (int16_t)ENC_GetRevolutionValue(self->instance);
    if (n_args > 1) {
        // Set the revolution value
        self->instance->REV = mp_obj_get_int(args[1]);
    }
    return MP_OBJ_NEW_SMALL_INT(cycles);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_encoder_cycles_obj, 1, 2, machine_encoder_cycles);

// encoder.irq(trigger=ENCODER.IRQ_MATCH, value=nnn, handler=None, hard=False)
STATIC mp_obj_t machine_encoder_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_trigger, ARG_value, ARG_handler, ARG_hard };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_trigger, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_handler, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_hard, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    machine_encoder_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    if (!self->active) {
        mp_raise_ValueError(MP_ERROR_TEXT("device stopped"));
    }

    if (self->irq == NULL) {
        self->irq = m_new_obj(mp_irq_obj_t);
        self->irq->base.type = &mp_irq_type;
        self->irq->parent = MP_OBJ_FROM_PTR(self);
        self->irq->methods = NULL;
        self->irq->ishard = false;
    }

    uint16_t trigger = args[ARG_trigger].u_int &
        (ENCODER_TRIGGER_MATCH | ENCODER_TRIGGER_ROLL_UNDER | ENCODER_TRIGGER_ROLL_OVER);

    if (args[ARG_value].u_obj != mp_const_none) {
        uint32_t value = mp_obj_int_get_truncated(args[ARG_value].u_obj);
        self->enc_config.positionCompareValue = value;
        self->instance->LCOMP = (uint16_t)(value) & 0xffff;        /* Lower 16 pos bits. */
        self->instance->UCOMP = (uint16_t)(value >> 16U) & 0xffff; /* Upper 16 pos bits. */
        trigger |= ENCODER_TRIGGER_MATCH;
    }

    self->irq->handler = args[ARG_handler].u_obj;
    self->irq->ishard = args[ARG_hard].u_bool;
    self->requested_irq = trigger;
    // Clear pending interrupt flags
    ENC_ClearStatusFlags(self->instance, ENCODER_ALL_INTERRUPTS);
    if (self->irq->handler != mp_const_none) {
        ENC_DisableInterrupts(self->instance, ENCODER_ALL_INTERRUPTS);
        ENC_EnableInterrupts(self->instance, trigger);
        EnableIRQ(enc_irqn[self->id + 1]);
    } else {
        ENC_DisableInterrupts(self->instance, trigger);
        DisableIRQ(enc_irqn[self->id + 1]);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_encoder_irq_obj, 1, machine_encoder_irq);

// encoder.init([kwargs])
STATIC mp_obj_t machine_encoder_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    mp_machine_encoder_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_encoder_init_obj, 1, machine_encoder_init);

// encoder.id()
STATIC mp_obj_t machine_encoder_id(mp_obj_t self_in) {
    machine_encoder_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->id);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_encoder_id_obj, machine_encoder_id);

STATIC const mp_rom_map_elem_t machine_encoder_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_encoder_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_encoder_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&machine_encoder_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_encoder_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_cycles), MP_ROM_PTR(&machine_encoder_cycles_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&machine_encoder_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_id), MP_ROM_PTR(&machine_encoder_id_obj) },

    { MP_ROM_QSTR(MP_QSTR_IRQ_MATCH), MP_ROM_INT(ENCODER_TRIGGER_MATCH) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_ROLL_UNDER), MP_ROM_INT(ENCODER_TRIGGER_ROLL_UNDER) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_ROLL_OVER), MP_ROM_INT(ENCODER_TRIGGER_ROLL_OVER) },
};
STATIC MP_DEFINE_CONST_DICT(machine_encoder_locals_dict, machine_encoder_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_encoder_type,
    MP_QSTR_Encoder,
    MP_TYPE_FLAG_NONE,
    make_new, mp_machine_encoder_make_new,
    print, mp_machine_encoder_print,
    locals_dict, &machine_encoder_locals_dict
    );


// --- Counter class code ----------

STATIC void mp_machine_counter_init_helper(machine_encoder_obj_t *self,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_src, ARG_direction, ARG_match_pin, ARG_filter_ns, ARG_cpc, ARG_signed,  ARG_index };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_src, MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_direction, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_INT(-1)} },
        { MP_QSTR_match_pin, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_INT(-1)} },
        { MP_QSTR_filter_ns, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_cpc, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_signed, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_index, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_INT(-1)} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_src].u_obj != mp_const_none) {
        self->input_a = connect_pin_to_encoder(args[ARG_src].u_obj, xbar_signal_table[self->id].enc_input_a, XBAR_IN);
    }
    if (self->input_a == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("missing input pin"));
    }

    mp_obj_t direction = args[ARG_direction].u_obj;
    if (direction != MP_ROM_INT(-1)) {
        if (direction == MP_ROM_INT(COUNTER_UP)) {
            XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicLow, xbar_signal_table[self->id].enc_input_b);
        } else if (direction == MP_ROM_INT(COUNTER_DOWN)) {
            XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicHigh, xbar_signal_table[self->id].enc_input_b);
        } else {
            connect_pin_to_encoder(direction, xbar_signal_table[self->id].enc_input_b, XBAR_IN);
        }
    }

    // Set the common options and start
    mp_machine_encoder_init_helper_common(self, args + ARG_match_pin, &self->enc_config);
}

// Counter(id, input, [args])
STATIC mp_obj_t mp_machine_counter_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // Check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    CLOCK_EnableClock(kCLOCK_Iomuxc); // just in case it was not set yet
    XBARA_Init(XBARA1);

    uint8_t id = mp_obj_get_int(args[0]);
    if (id < 0 || id >= FSL_FEATURE_SOC_ENC_COUNT) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid encoder/counter id"));
    }
    // check, if the encoder is already in use, and if yes, dinit it
    if (encoder_table[id] != NULL) {
        encoder_deinit_single(encoder_table[id]);
    }

    // Connect input_b and the trigger input to a fixed level.
    XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicLow, xbar_signal_table[id].enc_input_b);
    XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicLow, xbar_signal_table[id].enc_trigger);

    // Create and populate the Qencoder object.
    machine_encoder_obj_t *self = m_new_obj(machine_encoder_obj_t);
    encoder_table[id] = self;
    self->id = id;
    self->input_a = 0;
    self->input_b = 0;
    self->base.type = &machine_counter_type;
    self->instance = enc_instances[id + 1];
    self->cpc = 0;
    self->status = 0;
    self->irq = NULL;
    self->match_pin = 0;
    self->is_signed = true;
    self->mode = MODE_COUNTER;

    // Set defaults for ENC Config
    ENC_GetDefaultConfig(&self->enc_config);

    // Set the mode to a 32 bit counter
    self->enc_config.decoderWorkMode = kENC_DecoderWorkAsSignalPhaseCountMode;
    self->enc_config.enableTRIGGERClearPositionCounter = true;
    self->enc_config.revolutionCountCondition = kENC_RevolutionCountOnRollOverModulus;

    // Process the remaining parameters
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_counter_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

// counter.init([kwargs])
STATIC mp_obj_t machine_counter_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    mp_machine_counter_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_counter_init_obj, 1, machine_counter_init);

STATIC const mp_rom_map_elem_t machine_counter_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_encoder_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_encoder_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_cycles), MP_ROM_PTR(&machine_encoder_cycles_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_counter_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&machine_encoder_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&machine_encoder_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_id), MP_ROM_PTR(&machine_encoder_id_obj) },

    { MP_ROM_QSTR(MP_QSTR_IRQ_MATCH), MP_ROM_INT(ENCODER_TRIGGER_MATCH) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_ROLL_UNDER), MP_ROM_INT(ENCODER_TRIGGER_ROLL_UNDER) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_ROLL_OVER), MP_ROM_INT(ENCODER_TRIGGER_ROLL_OVER) },
    { MP_ROM_QSTR(MP_QSTR_UP), MP_ROM_INT(COUNTER_UP) },
    { MP_ROM_QSTR(MP_QSTR_DOWN), MP_ROM_INT(COUNTER_DOWN) },
};
STATIC MP_DEFINE_CONST_DICT(machine_counter_locals_dict, machine_counter_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_counter_type,
    MP_QSTR_Counter,
    MP_TYPE_FLAG_NONE,
    make_new, mp_machine_counter_make_new,
    print, mp_machine_encoder_print,
    locals_dict, &machine_counter_locals_dict
    );

#endif // MICROPY_PY_MACHINE_QECNT

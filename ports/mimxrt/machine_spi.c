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

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/modmachine.h"
#include CLOCK_CONFIG_H

#include "fsl_cache.h"
#include "fsl_dmamux.h"
#include "fsl_iomuxc.h"
#include "fsl_lpspi.h"
#include "fsl_lpspi_edma.h"

#define DEFAULT_SPI_ID          (0)
#define DEFAULT_SPI_BAUDRATE    (1000000)
#define DEFAULT_SPI_POLARITY    (0)
#define DEFAULT_SPI_PHASE       (0)
#define DEFAULT_SPI_BITS        (8)
#define DEFAULT_SPI_FIRSTBIT    (kLPSPI_MsbFirst)
#define DEFAULT_SPI_DRIVE       (6)

#define CLOCK_DIVIDER           (1)

#if defined(MIMXRT117x_SERIES)
#define LPSPI_DMAMUX            DMAMUX0
#else
#define LPSPI_DMAMUX            DMAMUX
#endif

#define MICROPY_HW_SPI_NUM MP_ARRAY_SIZE(spi_index_table)

#define SCK (iomux_table[index])
#define CS0 (iomux_table[index + 1])
#define SDO (iomux_table[index + 2])
#define SDI (iomux_table[index + 3])
#define CS1 (iomux_table[index + 4])

typedef struct _machine_spi_obj_t {
    mp_obj_base_t base;
    uint8_t spi_id;
    uint8_t mode;
    uint8_t spi_hw_id;
    bool transfer_busy;
    LPSPI_Type *spi_inst;
    lpspi_master_config_t *master_config;
} machine_spi_obj_t;

typedef struct _iomux_table_t {
    uint32_t muxRegister;
    uint32_t muxMode;
    uint32_t inputRegister;
    uint32_t inputDaisy;
    uint32_t configRegister;
} iomux_table_t;

static const uint8_t spi_index_table[] = MICROPY_HW_SPI_INDEX;
static LPSPI_Type *spi_base_ptr_table[] = LPSPI_BASE_PTRS;
static const iomux_table_t iomux_table[] = {
    IOMUX_TABLE_SPI
};

bool lpspi_set_iomux(int8_t spi, uint8_t drive, int8_t cs) {
    int index = (spi - 1) * 5;

    if (SCK.muxRegister != 0) {
        IOMUXC_SetPinMux(SCK.muxRegister, SCK.muxMode, SCK.inputRegister, SCK.inputDaisy, SCK.configRegister, 0U);
        IOMUXC_SetPinConfig(SCK.muxRegister, SCK.muxMode, SCK.inputRegister, SCK.inputDaisy, SCK.configRegister,
            pin_generate_config(PIN_PULL_UP_100K, PIN_MODE_OUT, drive, SCK.configRegister));

        if (cs == 0 && CS0.muxRegister != 0) {
            IOMUXC_SetPinMux(CS0.muxRegister, CS0.muxMode, CS0.inputRegister, CS0.inputDaisy, CS0.configRegister, 0U);
            IOMUXC_SetPinConfig(CS0.muxRegister, CS0.muxMode, CS0.inputRegister, CS0.inputDaisy, CS0.configRegister,
                pin_generate_config(PIN_PULL_UP_100K, PIN_MODE_OUT, drive, CS0.configRegister));
        } else if (cs == 1 && CS1.muxRegister != 0) {
            IOMUXC_SetPinMux(CS1.muxRegister, CS1.muxMode, CS1.inputRegister, CS1.inputDaisy, CS1.configRegister, 0U);
            IOMUXC_SetPinConfig(CS1.muxRegister, CS1.muxMode, CS1.inputRegister, CS1.inputDaisy, CS1.configRegister,
                pin_generate_config(PIN_PULL_UP_100K, PIN_MODE_OUT, drive, CS1.configRegister));
        } else if (cs != -1) {
            mp_raise_ValueError(MP_ERROR_TEXT("The chosen CS is not available"));
        }

        IOMUXC_SetPinMux(SDO.muxRegister, SDO.muxMode, SDO.inputRegister, SDO.inputDaisy, SDO.configRegister, 0U);
        IOMUXC_SetPinConfig(SDO.muxRegister, SDO.muxMode, SDO.inputRegister, SDO.inputDaisy, SDO.configRegister,
            pin_generate_config(PIN_PULL_UP_100K, PIN_MODE_OUT, drive, SDO.configRegister));

        IOMUXC_SetPinMux(SDI.muxRegister, SDI.muxMode, SDI.inputRegister, SDI.inputDaisy, SDI.configRegister, 0U);
        IOMUXC_SetPinConfig(SDI.muxRegister, SDI.muxMode, SDI.inputRegister, SDI.inputDaisy, SDI.configRegister,
            pin_generate_config(PIN_PULL_UP_100K, PIN_MODE_IN, drive, SDI.configRegister));

        return true;
    } else {
        return false;
    }
}

static void machine_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    static const char *firstbit_str[] = {"MSB", "LSB"};
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPI(%u, baudrate=%u, polarity=%u, phase=%u, bits=%u, firstbit=%s, gap_ns=%d)",
        self->spi_id, self->master_config->baudRate, self->master_config->cpol,
        self->master_config->cpha, self->master_config->bitsPerFrame,
        firstbit_str[self->master_config->direction], self->master_config->betweenTransferDelayInNanoSec);
}

mp_obj_t machine_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_gap_ns, ARG_drive, ARG_cs };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_INT, {.u_int = DEFAULT_SPI_ID} },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = DEFAULT_SPI_BAUDRATE} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_POLARITY} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_PHASE} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_BITS} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_FIRSTBIT} },
        { MP_QSTR_gap_ns,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_drive,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_DRIVE} },
        { MP_QSTR_cs,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get the SPI bus id.
    int spi_id = args[ARG_id].u_int;
    if (spi_id < 0 || spi_id >= MP_ARRAY_SIZE(spi_index_table) || spi_index_table[spi_id] == 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SPI(%d) doesn't exist"), spi_id);
    }

    // Get peripheral object.
    uint8_t spi_hw_id = spi_index_table[spi_id];  // the hw spi number 1..n
    machine_spi_obj_t *self = mp_obj_malloc(machine_spi_obj_t, &machine_spi_type);
    self->spi_id = spi_id;
    self->spi_inst = spi_base_ptr_table[spi_hw_id];
    self->spi_hw_id = spi_hw_id;

    uint8_t drive = args[ARG_drive].u_int;
    if (drive < 1 || drive > 7) {
        drive = DEFAULT_SPI_DRIVE;
    }

    LPSPI_Reset(self->spi_inst);
    LPSPI_Enable(self->spi_inst, false);  // Disable first before new settings are applies

    self->master_config = m_new_obj(lpspi_master_config_t);
    LPSPI_MasterGetDefaultConfig(self->master_config);
    // Initialise the SPI peripheral.
    self->master_config->baudRate = args[ARG_baudrate].u_int;
    self->master_config->betweenTransferDelayInNanoSec = 1000000000 / self->master_config->baudRate * 2;
    self->master_config->cpol = args[ARG_polarity].u_int;
    self->master_config->cpha = args[ARG_phase].u_int;
    self->master_config->bitsPerFrame = args[ARG_bits].u_int;
    self->master_config->direction = args[ARG_firstbit].u_int;
    if (args[ARG_gap_ns].u_int != -1) {
        self->master_config->betweenTransferDelayInNanoSec = args[ARG_gap_ns].u_int;
    }
    self->master_config->lastSckToPcsDelayInNanoSec = self->master_config->betweenTransferDelayInNanoSec;
    self->master_config->pcsToSckDelayInNanoSec = self->master_config->betweenTransferDelayInNanoSec;
    int8_t cs = args[ARG_cs].u_int;
    // In the SPI master_config for automatic CS the value cs=0 is set already,
    // so only cs=1 has to be addressed here. The case cs == -1 for manual CS is handled
    // in the function spi_set_iomux() and the value in the master_config can stay at 0.
    if (cs == 1) {
        self->master_config->whichPcs = cs;
    }
    LPSPI_MasterInit(self->spi_inst, self->master_config, BOARD_BOOTCLOCKRUN_LPSPI_CLK_ROOT);
    lpspi_set_iomux(spi_index_table[spi_id], drive, cs);

    return MP_OBJ_FROM_PTR(self);
}

static void machine_spi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_gap_ns };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_gap_ns,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    // Parse the arguments.
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Reconfigure the baudrate if requested.
    if (args[ARG_baudrate].u_int != -1) {
        self->master_config->baudRate = args[ARG_baudrate].u_int;
        self->master_config->betweenTransferDelayInNanoSec = 1000000000 / self->master_config->baudRate * 2;
    }
    // Reconfigure the format if requested.
    if (args[ARG_polarity].u_int != -1) {
        self->master_config->cpol = args[ARG_polarity].u_int;
    }
    if (args[ARG_phase].u_int != -1) {
        self->master_config->cpha = args[ARG_phase].u_int;
    }
    if (args[ARG_bits].u_int != -1) {
        self->master_config->bitsPerFrame = args[ARG_bits].u_int;
    }
    if (args[ARG_firstbit].u_int != -1) {
        self->master_config->direction = args[ARG_firstbit].u_int;
    }
    if (args[ARG_gap_ns].u_int != -1) {
        self->master_config->betweenTransferDelayInNanoSec = args[ARG_gap_ns].u_int;
    }
    self->master_config->lastSckToPcsDelayInNanoSec = self->master_config->betweenTransferDelayInNanoSec;
    self->master_config->pcsToSckDelayInNanoSec = self->master_config->betweenTransferDelayInNanoSec;
    LPSPI_Enable(self->spi_inst, false);  // Disable first before new settings are applies
    LPSPI_MasterInit(self->spi_inst, self->master_config, BOARD_BOOTCLOCKRUN_LPSPI_CLK_ROOT);
}

static void machine_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;

    if (len > 0) {
        // Wait a short while for the previous transfer to finish, but not forever
        for (volatile int j = 0; (j < 5000) && ((self->spi_inst->SR & kLPSPI_ModuleBusyFlag) != 0); j++) {}

        lpspi_transfer_t masterXfer;
        masterXfer.txData = (uint8_t *)src;
        masterXfer.rxData = (uint8_t *)dest;
        masterXfer.dataSize = len;
        masterXfer.configFlags = (self->master_config->whichPcs << LPSPI_MASTER_PCS_SHIFT) | kLPSPI_MasterPcsContinuous | kLPSPI_MasterByteSwap;

        if (LPSPI_MasterTransferBlocking(self->spi_inst, &masterXfer) != kStatus_Success) {
            mp_raise_OSError(EIO);
        }
    }
}

static const mp_machine_spi_p_t machine_spi_p = {
    .init = machine_spi_init,
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

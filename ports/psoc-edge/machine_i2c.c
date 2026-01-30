/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2026 Infineon Technologies AG
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

// std includes
#include <stdio.h>
#include <string.h>


// mpy includes
#include "extmod/modmachine.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"


// MTB includes
#include "cybsp.h"
#include "cy_scb_i2c.h"
#include "cy_sysint.h"
#include "cy_sysclk.h"


// port-specific includes
#include "modmachine.h"
// #include "machine_pin_phy.h"
#include "mplogger.h"

#define DEFAULT_I2C_FREQ     (400000)

#define i2c_assert_raise_val(msg, ret)   if (ret != CY_RSLT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

typedef struct _machine_hw_i2c_obj_t {
    mp_obj_base_t base;
    int id; // This parameter is unused and added for compliance with reference API.
    uint32_t scl_pin;
    uint32_t sda_pin;
    uint32_t freq;
    uint32_t timeout;
    cy_stc_scb_i2c_config_t cfg;   // PDL I2C configuration
    cy_stc_scb_i2c_context_t ctx;  // PDL I2C runtime context
} machine_hw_i2c_obj_t;

machine_hw_i2c_obj_t *machine_hw_i2c_obj[MAX_I2C] = { NULL };

// I2C interrupt service routine
// Note: Using master-specific interrupt function to reduce flash consumption
static void machine_i2c_isr(void) {
    // Find which I2C instance triggered the interrupt
    for (uint8_t i = 0; i < MAX_I2C; i++) {
        if (machine_hw_i2c_obj[i] != NULL) {
            // Call I2C master interrupt handler (more efficient than generic Cy_SCB_I2C_Interrupt)
            Cy_SCB_I2C_MasterInterrupt(MICROPY_HW_I2C0_SCB, &machine_hw_i2c_obj[i]->ctx);
        }
    }
}

static inline machine_hw_i2c_obj_t *machine_hw_i2c_obj_alloc(void) {
    for (uint8_t i = 0; i < MAX_I2C; i++)
    {
        if (machine_hw_i2c_obj[i] == NULL) {
            machine_hw_i2c_obj[i] = mp_obj_malloc(machine_hw_i2c_obj_t, &machine_i2c_type);
            return machine_hw_i2c_obj[i];
        }
    }

    // Debug: print status of all slots
    mplogger_print("I2C alloc failed - all slots occupied:\n");
    for (uint8_t i = 0; i < MAX_I2C; i++) {
        mplogger_print("  Slot %u: %p\n", i, machine_hw_i2c_obj[i]);
    }

    return NULL;
}

static inline void machine_hw_i2c_obj_free(machine_hw_i2c_obj_t *i2c_obj_ptr) {
    for (uint8_t i = 0; i < MAX_I2C; i++)
    {
        if (machine_hw_i2c_obj[i] == i2c_obj_ptr) {
            machine_hw_i2c_obj[i] = NULL;
        }
    }
}

static void machine_hw_i2c_init(machine_hw_i2c_obj_t *self, uint32_t freq_hz) {
    cy_rslt_t result;

    self->cfg = (cy_stc_scb_i2c_config_t) {
        .i2cMode = CY_SCB_I2C_MASTER,
        .useRxFifo = false,
        .useTxFifo = true,
        .slaveAddress = 0U,
        .slaveAddressMask = 0U,
        .acceptAddrInFifo = false,
        .ackGeneralAddr = false,
        .enableWakeFromSleep = false,
        .enableDigitalFilter = false,
        .lowPhaseDutyCycle = 8U,
        .highPhaseDutyCycle = 8U,
    };

    Cy_GPIO_SetHSIOM(MICROPY_HW_I2C0_SCL_PORT, self->scl_pin, MICROPY_HW_I2C0_SCL_HSIOM);
    Cy_GPIO_SetHSIOM(MICROPY_HW_I2C0_SDA_PORT, self->sda_pin, MICROPY_HW_I2C0_SDA_HSIOM);
    Cy_GPIO_SetDrivemode(MICROPY_HW_I2C0_SCL_PORT, self->scl_pin, CY_GPIO_DM_OD_DRIVESLOW);
    Cy_GPIO_SetDrivemode(MICROPY_HW_I2C0_SDA_PORT, self->sda_pin, CY_GPIO_DM_OD_DRIVESLOW);

    result = Cy_SCB_I2C_Init(MICROPY_HW_I2C0_SCB, &self->cfg, &self->ctx);
    if (result != CY_RSLT_SUCCESS) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2C init failed: 0x%lx"), result);
    }

    // For desired data rate, clk_scb frequency must be in valid range (see TRM I2C Oversampling section)
    // For 100kHz: clk_scb range is 1.55 - 3.2 MHz (architecture reference manual 002-38331 Rev. P685 table 355)
    //   - clk_peri = 100 MHz, divider = 42 → clk_scb = 2.38 MHz ✓ (mid-range)
    // For 400kHz: clk_scb range is 7.82 - 10 MHz
    //   - clk_peri = 100 MHz, divider = 11 → clk_scb = 9.09 MHz ✓ (within range)
    // Note: Cy_SysClk_PeriphSetDivider takes (divider - 1), so divider=11 → value=10
    /* Connect assigned divider to be a clock source for I2C */
    Cy_SysClk_PeriphAssignDivider(MICROPY_HW_I2C_PCLK, CY_SYSCLK_DIV_8_BIT, 2U);
    uint32_t divider = (freq_hz <= 100000) ? 41U : 10U;
    Cy_SysClk_PeriphSetDivider(CY_SYSCLK_DIV_8_BIT, 2U, divider);
    Cy_SysClk_PeriphEnableDivider(CY_SYSCLK_DIV_8_BIT, 2U);

    uint32_t clk_scb_freq = Cy_SysClk_PeriphGetFrequency(CY_SYSCLK_DIV_8_BIT, 2U);
    mplogger_print("DEBUG: clk_scb_freq=%u Hz\n", clk_scb_freq);

    uint32_t actual_rate = Cy_SCB_I2C_SetDataRate(MICROPY_HW_I2C0_SCB, freq_hz, clk_scb_freq);
    mplogger_print("DEBUG: actual_rate=%u Hz (requested=%u Hz)\n", actual_rate, freq_hz);

    if ((actual_rate > freq_hz) || (actual_rate == 0U)) {
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("Cannot reach desired I2C data rate %u Hz (actual: %u Hz)"),
            freq_hz, actual_rate);
    }

    const cy_stc_sysint_t i2cIntrConfig = {
        .intrSrc = MICROPY_HW_I2C_IRQn,
        .intrPriority = MICROPY_HW_I2C_INTR_PRIORITY,
    };

    // Hook interrupt service routine and enable interrupt in NVIC
    result = Cy_SysInt_Init(&i2cIntrConfig, &machine_i2c_isr);
    if (result != CY_RSLT_SUCCESS) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2C interrupt init failed: 0x%lx"), result);
    }
    NVIC_EnableIRQ(MICROPY_HW_I2C_IRQn);

    Cy_SCB_I2C_Enable(MICROPY_HW_I2C0_SCB);

    mplogger_print("I2C initialized: requested=%u Hz, actual=%u Hz, clk_scb=%u Hz\n",
        freq_hz, actual_rate, clk_scb_freq);

    // Store requested frequency
    self->freq = freq_hz;
}


static int machine_hw_i2c_deinit(mp_obj_base_t *self_in) {
    machine_hw_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);

    Cy_SCB_I2C_Disable(MICROPY_HW_I2C0_SCB, &self->ctx);
    NVIC_DisableIRQ(MICROPY_HW_I2C_IRQn);
    Cy_SysClk_PeriphDisableDivider(CY_SYSCLK_DIV_8_BIT, 0U);

    machine_hw_i2c_obj_free(self);

    return 0;  // Success
}

static int machine_hw_i2c_transfer(mp_obj_base_t *self_in, uint16_t addr, size_t len, uint8_t *buf, unsigned int flags) {
    machine_hw_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    cy_rslt_t result;

    mplogger_print("I2C Transfer: addr=0x%02X, len=%u, flags=0x%02X (%s)\n",
        addr, len, flags, (flags & MP_MACHINE_I2C_FLAG_READ) ? "READ" : "WRITE");

    cy_stc_scb_i2c_master_xfer_config_t transfer;
    transfer.slaveAddress = addr;
    transfer.buffer = buf;
    transfer.bufferSize = len;
    // Generate Stop condition if MP_MACHINE_I2C_FLAG_STOP is set
    transfer.xferPending = !(flags & MP_MACHINE_I2C_FLAG_STOP);

    if (flags & MP_MACHINE_I2C_FLAG_READ) {
        result = Cy_SCB_I2C_MasterRead(MICROPY_HW_I2C0_SCB, &transfer, &self->ctx);
    } else {
        result = Cy_SCB_I2C_MasterWrite(MICROPY_HW_I2C0_SCB, &transfer, &self->ctx);
    }

    if (result != CY_RSLT_SUCCESS) {
        mplogger_print("I2C Transfer start failed: 0x%lx\n", result);
        return -MP_EIO;  // I/O error
    }

    mplogger_print("I2C Transfer started, waiting for completion...\n");

    uint32_t start_time = mp_hal_ticks_us();
    uint32_t timeout_end = start_time + self->timeout;  // Both in microseconds

    while (0UL != (CY_SCB_I2C_MASTER_BUSY & Cy_SCB_I2C_MasterGetStatus(MICROPY_HW_I2C0_SCB, &self->ctx))) {
        // Yield to allow other tasks/interrupts to run
        MICROPY_EVENT_POLL_HOOK

        // Check for timeout using actual elapsed time
        if (mp_hal_ticks_us() >= timeout_end) {
            mplogger_print("I2C Transfer timeout after %u us!\n", self->timeout);
            return -MP_ETIMEDOUT;
        }
    }

    uint32_t master_status = Cy_SCB_I2C_MasterGetStatus(MICROPY_HW_I2C0_SCB, &self->ctx);

    mplogger_print("I2C Transfer complete, status=0x%08lX\n", master_status);

    if (master_status & CY_SCB_I2C_MASTER_ERR) {
        mplogger_print("I2C Transfer error detected in status\n");
        return -MP_EIO;  // I/O error
    }

    return len;
}

/******************************************************************************/
// MicroPython bindings for machine API

static void machine_hw_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hw_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Print I2C configuration
    mp_printf(print, "I2C(scl=%u, sda=%u, freq=%u, timeout=%uus)",
        self->scl_pin,
        self->sda_pin,
        self->freq,
        self->timeout);
}

mp_obj_t machine_hw_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 0, 5, true);

    enum { ARG_id, ARG_freq, ARG_scl, ARG_sda, ARG_timeout };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,   MP_ARG_INT, {.u_int = -1}},
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_I2C_FREQ} },
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 50000} }  // Default 50000us (50ms)
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Check if an I2C instance already exists for this ID
    // If so, deinitialize it first (allows reinitialization)
    int requested_id = args[ARG_id].u_int;
    if (requested_id == -1) {
        requested_id = 0;  // Default to ID 0
    }
    // For single I2C port, reuse the existing object if present
    machine_hw_i2c_obj_t *self = machine_hw_i2c_obj[0];
    if (self != NULL) {
        // Deinitialize existing instance
        mplogger_print("Reinitializing existing I2C instance\n");
        machine_hw_i2c_deinit((mp_obj_base_t *)self);
    }

    self = machine_hw_i2c_obj_alloc();
    if (self == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("Maximum number of I2C instances reached"));
    }

    if (args[ARG_id].u_int != -1) {
        self->id = args[ARG_id].u_int;
        mplogger_print("machine.I2C: ID parameter is ignored in this port.\n");
    }
    // Parse and validate pin arguments, use defaults from mpconfigboard.h if not provided
    // Note: KIT_PSE84_AI only has one hardware I2C with fixed pins P17_0 (SCL) and P17_1 (SDA)
    bool warn_pins = false;
    if (args[ARG_scl].u_obj != MP_ROM_NONE || args[ARG_sda].u_obj != MP_ROM_NONE) {
        // Allow scl='P17_0', sda='P17_1' without warning
        bool is_valid_scl = (args[ARG_scl].u_obj == MP_ROM_NONE) ||
            (mp_obj_is_str(args[ARG_scl].u_obj) &&
                strcmp(mp_obj_str_get_str(args[ARG_scl].u_obj), "P17_0") == 0);
        bool is_valid_sda = (args[ARG_sda].u_obj == MP_ROM_NONE) ||
            (mp_obj_is_str(args[ARG_sda].u_obj) &&
                strcmp(mp_obj_str_get_str(args[ARG_sda].u_obj), "P17_1") == 0);

        if (!is_valid_scl || !is_valid_sda) {
            warn_pins = true;
        }
    }

    if (warn_pins) {
        mp_printf(&mp_plat_print, "I2C: KIT_PSE84_AI only supports fixed pins P17_0 (SCL) and P17_1 (SDA). Custom pins ignored.\n");
    }

    self->scl_pin = MICROPY_HW_I2C0_SCL;
    self->sda_pin = MICROPY_HW_I2C0_SDA;

    self->timeout = args[ARG_timeout].u_int;
    if (self->timeout == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("timeout must be > 0"));
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        machine_hw_i2c_init(self, args[ARG_freq].u_int);
        nlr_pop();
    } else {
        // Initialization failed, clean up
        machine_hw_i2c_obj_free(self);
        nlr_jump(nlr.ret_val);
    }

    return MP_OBJ_FROM_PTR(self);
}

static const mp_machine_i2c_p_t machine_hw_i2c_p = {
    .stop = machine_hw_i2c_deinit,  // Map stop() to deinit functionality
    .transfer = mp_machine_i2c_transfer_adaptor,
    .transfer_single = machine_hw_i2c_transfer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2c_type,
    MP_QSTR_I2C,
    MP_TYPE_FLAG_NONE,
    make_new, machine_hw_i2c_make_new,
    print, machine_hw_i2c_print,
    protocol, &machine_hw_i2c_p,
    locals_dict, &mp_machine_i2c_locals_dict
    );

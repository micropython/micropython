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

// MTB includes
#include "cybsp.h"
#include "cy_scb_i2c.h"
#include "cy_sysint.h"
#include "cy_sysclk.h"

// MicroPython includes
#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"

// port-specific includes
#include "mplogger.h"

// PDL event callback for slave operations
static void i2c_slave_event_callback(uint32_t events);

typedef struct _machine_i2c_target_obj_t {
    mp_obj_base_t base;
    uint8_t id;
    uint32_t scl_pin;
    uint32_t sda_pin;
    uint32_t slave_addr;
    uint8_t addrsize;
    cy_stc_scb_i2c_config_t cfg;
    cy_stc_scb_i2c_context_t ctx;
    size_t tx_index;
    size_t rx_index;
} machine_i2c_target_obj_t;

static machine_i2c_target_obj_t machine_i2c_target_obj[MAX_I2C];

/******************************************************************************/
// PSOC PDL hardware bindings

// I2C Slave interrupt service routine (calls PDL interrupt handler)
static void machine_i2c_target_isr(void) {
    for (uint8_t i = 0; i < MAX_I2C; i++) {
        machine_i2c_target_obj_t *self = &machine_i2c_target_obj[i];
        if (self->base.type != NULL) {
            Cy_SCB_I2C_SlaveInterrupt(MICROPY_HW_I2C0_SCB, &self->ctx);
        }
    }
}

// PDL event callback - called from within Cy_SCB_I2C_SlaveInterrupt
// Implements the event handling pattern from PDL Slave Operation documentation
//
// Key PDL requirements implemented:
// 1. Register callback during init: Cy_SCB_I2C_RegisterEventCallback()
// 2. Handle completion events: RD_CMPLT_EVENT, WR_CMPLT_EVENT
// 3. Reconfigure buffers after each transaction (critical!)
// 4. Clear status flags after write: Cy_SCB_I2C_SlaveClearWriteStatus()
//
// Note: Without buffer reconfiguration, next transaction continues from
// where previous stopped (e.g., if master read 8 of 10 bytes, next read
// starts at byte 9). This is PDL documented behavior.
static void i2c_slave_event_callback(uint32_t events) {
    // Find the active slave instance
    machine_i2c_target_obj_t *self = NULL;
    for (uint8_t i = 0; i < MAX_I2C; i++) {
        if (machine_i2c_target_obj[i].base.type != NULL) {
            self = &machine_i2c_target_obj[i];
            break;
        }
    }

    if (self == NULL) {
        return;
    }

    machine_i2c_target_data_t *data = &machine_i2c_target_data[self->id];

    if (events & CY_SCB_I2C_SLAVE_READ_EVENT) {
        mplogger_print("I2C Slave: Read request (address matched)\n");
        machine_i2c_target_data_addr_match(data, true);
    }


    if (events & CY_SCB_I2C_SLAVE_WRITE_EVENT) {
        mplogger_print("I2C Slave: Write request (address matched)\n");
        machine_i2c_target_data_addr_match(data, false);
    }


    if (events & CY_SCB_I2C_SLAVE_RD_BUF_EMPTY_EVENT) {
        mplogger_print("I2C Slave: Read buffer empty\n");
        if (data->mem_buf != NULL && data->mem_len > 0) {
            machine_i2c_target_data_read_request(self, data);
        }
    }

    if (events & CY_SCB_I2C_SLAVE_RD_CMPLT_EVENT) {
        if (!(events & CY_SCB_I2C_SLAVE_ERR_EVENT)) {
            mplogger_print("I2C Slave: Read complete, %u bytes sent\n",
                Cy_SCB_I2C_SlaveGetReadTransferCount(MICROPY_HW_I2C0_SCB, &self->ctx));
        }

        if (data->mem_buf != NULL && data->mem_len > 0) {
            Cy_SCB_I2C_SlaveConfigReadBuf(MICROPY_HW_I2C0_SCB, data->mem_buf, data->mem_len, &self->ctx);
        }

        Cy_SCB_I2C_SlaveClearReadStatus(MICROPY_HW_I2C0_SCB, &self->ctx);

        // Reset index for next transaction
        self->tx_index = 0;

        // Set state to READING so extmod reset_helper triggers END_READ
        data->state = STATE_READING;

        // Notify application layer - this triggers END_READ event
        machine_i2c_target_data_restart_or_stop(data);
    }

    if (events & CY_SCB_I2C_SLAVE_WR_CMPLT_EVENT) {
        if (!(events & CY_SCB_I2C_SLAVE_ERR_EVENT)) {
            uint32_t bytes_received = Cy_SCB_I2C_SlaveGetWriteTransferCount(MICROPY_HW_I2C0_SCB, &self->ctx);
            mplogger_print("I2C Slave: Write complete, %u bytes received\n", bytes_received);
            self->rx_index = 0;
            while (self->rx_index < bytes_received) {
                machine_i2c_target_data_write_request(self, data);
            }
        }

        if (data->mem_buf != NULL && data->mem_len > 0) {
            Cy_SCB_I2C_SlaveConfigWriteBuf(MICROPY_HW_I2C0_SCB, data->mem_buf, data->mem_len, &self->ctx);
        }

        Cy_SCB_I2C_SlaveClearWriteStatus(MICROPY_HW_I2C0_SCB, &self->ctx);

        // Ensure state is WRITING so extmod reset_helper triggers END_WRITE
        data->state = STATE_WRITING;

        // Notify application layer - this triggers END_WRITE event
        machine_i2c_target_data_restart_or_stop(data);
    }

    // Handle errors
    if (events & CY_SCB_I2C_SLAVE_ERR_EVENT) {
        mplogger_print("I2C Slave: Error event\n");
        machine_i2c_target_data_restart_or_stop(data);
    }
}

static void i2c_target_init(machine_i2c_target_obj_t *self, machine_i2c_target_data_t *data,
    uint32_t addr, uint32_t addrsize, bool first_init) {
    cy_rslt_t result;

    if (!first_init) {
        Cy_SCB_I2C_Disable(MICROPY_HW_I2C0_SCB, &self->ctx);
    }

    self->cfg = (cy_stc_scb_i2c_config_t) {
        .i2cMode = CY_SCB_I2C_SLAVE,
        .useRxFifo = false,  // PDL recommends false for slave to avoid side effects
        .useTxFifo = true,
        .slaveAddress = addr,
        .slaveAddressMask = 0xFEU,
        .acceptAddrInFifo = false,
        .ackGeneralAddr = false,
        .enableWakeFromSleep = false,
        .enableDigitalFilter = false,
        .lowPhaseDutyCycle = 0U,  // Not used for slave mode
        .highPhaseDutyCycle = 0U, // Not used for slave mode
    };

    self->slave_addr = addr;
    self->addrsize = addrsize;

    Cy_GPIO_SetHSIOM(MICROPY_HW_I2C0_SCL_PORT, MICROPY_HW_I2C0_SCL_PIN, MICROPY_HW_I2C0_SCL_HSIOM);
    Cy_GPIO_SetHSIOM(MICROPY_HW_I2C0_SDA_PORT, MICROPY_HW_I2C0_SDA_PIN, MICROPY_HW_I2C0_SDA_HSIOM);
    Cy_GPIO_SetDrivemode(MICROPY_HW_I2C0_SCL_PORT, MICROPY_HW_I2C0_SCL_PIN, CY_GPIO_DM_OD_DRIVESLOW);
    Cy_GPIO_SetDrivemode(MICROPY_HW_I2C0_SDA_PORT, MICROPY_HW_I2C0_SDA_PIN, CY_GPIO_DM_OD_DRIVESLOW);

    result = Cy_SCB_I2C_Init(MICROPY_HW_I2C0_SCB, &self->cfg, &self->ctx);
    if (result != CY_RSLT_SUCCESS) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2C Target init failed: 0x%lx"), result);
    }

    // Configure clock for I2C slave operation
    // For 400 khz slave, clk_scb must be 7.82 – 15.38 MHz
    // For 100 khz slave, clk_scb must be 1.55 – 12.8 MHz
    // clk_peri = 100 MHz, divider = 7, clk_scb = 100/8 = 12.5 MHz
    Cy_SysClk_PeriphAssignDivider(MICROPY_HW_I2C_PCLK, CY_SYSCLK_DIV_8_BIT, 2U);
    Cy_SysClk_PeriphSetDivider(CY_SYSCLK_DIV_8_BIT, 2U, 7U); // divider = n+1, so 7 means divide by 8
    Cy_SysClk_PeriphEnableDivider(CY_SYSCLK_DIV_8_BIT, 2U);

    const cy_stc_sysint_t i2cIntrConfig = {
        .intrSrc = MICROPY_HW_I2C_IRQn,
        .intrPriority = MICROPY_HW_I2C_INTR_PRIORITY,
    };

    result = Cy_SysInt_Init(&i2cIntrConfig, &machine_i2c_target_isr);
    if (result != CY_RSLT_SUCCESS) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2C Target interrupt init failed: 0x%lx"), result);
    }
    NVIC_EnableIRQ(MICROPY_HW_I2C_IRQn);

    Cy_SCB_I2C_RegisterEventCallback(MICROPY_HW_I2C0_SCB, i2c_slave_event_callback, &self->ctx);

    if (data->mem_buf != NULL && data->mem_len > 0) {
        Cy_SCB_I2C_SlaveConfigReadBuf(MICROPY_HW_I2C0_SCB, data->mem_buf, data->mem_len, &self->ctx);
        Cy_SCB_I2C_SlaveConfigWriteBuf(MICROPY_HW_I2C0_SCB, data->mem_buf, data->mem_len, &self->ctx);
    }

    Cy_SCB_I2C_Enable(MICROPY_HW_I2C0_SCB);

    mplogger_print("I2C Target initialized: addr=0x%02X, addrsize=%u-bit\n", addr, addrsize);
}

/******************************************************************************/
// I2CTarget port implementation

static inline size_t mp_machine_i2c_target_get_index(machine_i2c_target_obj_t *self) {
    return self->id;
}

// IRQ event callback - called from extmod to trigger Python IRQ handler
// This is called by handle_event() in extmod/machine_i2c_target.c
static void mp_machine_i2c_target_event_callback(machine_i2c_target_irq_obj_t *irq) {
    mplogger_print("[SW] mp_machine_i2c_target_event_callback: irq=%p, flags=0x%02X, handler=%p\n", irq, irq->flags, irq->base.handler);
    if (irq->base.handler != mp_const_none) {
        mp_irq_handler(&irq->base);
    }
}

static size_t mp_machine_i2c_target_read_bytes(machine_i2c_target_obj_t *self, size_t len, uint8_t *buf) {
    machine_i2c_target_data_t *data = &machine_i2c_target_data[self->id];
    size_t read_len = 0;

    NVIC_DisableIRQ(MICROPY_HW_I2C_IRQn);

    // Read from write buffer (data written by master into slave write buffer)
    uint32_t available = Cy_SCB_I2C_SlaveGetWriteTransferCount(MICROPY_HW_I2C0_SCB, &self->ctx);
    read_len = (len < available) ? len : available;

    if (data->mem_buf != NULL) {
        for (size_t i = 0; i < read_len; i++) {
            if (self->rx_index < data->mem_len) {
                buf[i] = data->mem_buf[self->rx_index++];
            }
        }
    }
    NVIC_EnableIRQ(MICROPY_HW_I2C_IRQn);

    return read_len;
}

static size_t mp_machine_i2c_target_write_bytes(machine_i2c_target_obj_t *self, size_t len, const uint8_t *buf) {
    machine_i2c_target_data_t *data = &machine_i2c_target_data[self->id];
    size_t write_len = 0;

    NVIC_DisableIRQ(MICROPY_HW_I2C_IRQn);

    if (data->mem_buf != NULL) {
        for (size_t i = 0; i < len; i++) {
            if (self->tx_index < data->mem_len) {
                data->mem_buf[self->tx_index++] = buf[i];
                write_len++;
            }
        }

        // Update slave read buffer to reflect new data (per PDL documentation)
        Cy_SCB_I2C_SlaveConfigReadBuf(MICROPY_HW_I2C0_SCB, data->mem_buf, self->tx_index, &self->ctx);
    }

    NVIC_EnableIRQ(MICROPY_HW_I2C_IRQn);

    return write_len;
}

static void mp_machine_i2c_target_irq_config(machine_i2c_target_obj_t *self, unsigned int trigger) {
    // IRQ configuration already handled in init
}

static mp_obj_t mp_machine_i2c_target_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_addr, ARG_addrsize, ARG_mem, ARG_mem_addrsize, ARG_scl, ARG_sda };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_addr, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 7} },
        { MP_QSTR_mem, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mem_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int i2c_id = args[ARG_id].u_int;

    if (i2c_id < 0 || i2c_id >= MAX_I2C) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2CTarget(%d) doesn't exist"), i2c_id);
    }

    machine_i2c_target_obj_t *self = &machine_i2c_target_obj[i2c_id];

    bool first_init = false;
    if (self->base.type == NULL) {
        // Created for the first time, set default values
        self->base.type = &machine_i2c_target_type;
        self->id = i2c_id;
        self->scl_pin = MICROPY_HW_I2C0_SCL;
        self->sda_pin = MICROPY_HW_I2C0_SDA;
        first_init = true;
    }

    if (args[ARG_addrsize].u_int != 7 && args[ARG_addrsize].u_int != 10) {
        mp_raise_ValueError(MP_ERROR_TEXT("addrsize must be 7 or 10"));
    }

    if (args[ARG_mem_addrsize].u_int != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("mem_addrsize must be 0 (EEPROM-like addressing not implemented)"));
    }

    self->tx_index = 0;
    self->rx_index = 0;

    MP_STATE_PORT(machine_i2c_target_mem_obj)[i2c_id] = args[ARG_mem].u_obj;
    machine_i2c_target_data_t *data = &machine_i2c_target_data[i2c_id];
    machine_i2c_target_data_init(data, args[ARG_mem].u_obj, args[ARG_mem_addrsize].u_int);

    // Note: KIT_PSE84_AI only has one hardware I2C with fixed pins P17_0 (SCL) and P17_1 (SDA)
    bool warn_pins = false;
    if (args[ARG_scl].u_obj != mp_const_none || args[ARG_sda].u_obj != mp_const_none) {
        // Allow scl='P17_0', sda='P17_1' without warning
        bool is_valid_scl = (args[ARG_scl].u_obj == mp_const_none) ||
            (mp_obj_is_str(args[ARG_scl].u_obj) &&
                strcmp(mp_obj_str_get_str(args[ARG_scl].u_obj), "P17_0") == 0);
        bool is_valid_sda = (args[ARG_sda].u_obj == mp_const_none) ||
            (mp_obj_is_str(args[ARG_sda].u_obj) &&
                strcmp(mp_obj_str_get_str(args[ARG_sda].u_obj), "P17_1") == 0);

        if (!is_valid_scl || !is_valid_sda) {
            warn_pins = true;
        }
    }

    if (warn_pins) {
        mp_printf(&mp_plat_print, "I2CTarget: KIT_PSE84_AI only supports fixed pins P17_0 (SCL) and P17_1 (SDA). Custom pins ignored.\n");
    }

    // Always use hardware default pins
    self->scl_pin = MICROPY_HW_I2C0_SCL;
    self->sda_pin = MICROPY_HW_I2C0_SDA;

    i2c_target_init(self, data, args[ARG_addr].u_int, args[ARG_addrsize].u_int, first_init);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_i2c_target_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2CTarget(%u, addr=0x%02X, scl=%u, sda=%u)",
        self->id, self->slave_addr, self->scl_pin, self->sda_pin);
}

static void mp_machine_i2c_target_deinit(machine_i2c_target_obj_t *self) {
    Cy_SCB_I2C_Disable(MICROPY_HW_I2C0_SCB, &self->ctx);
    NVIC_DisableIRQ(MICROPY_HW_I2C_IRQn);
    self->base.type = NULL;

    mplogger_print("I2C Target deinitialized\n");
}

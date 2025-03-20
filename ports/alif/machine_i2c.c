/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024-2025 OpenMV LLC.
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

#if MICROPY_HW_ENABLE_HW_I2C
#include "i2c.h"

#define I2C_DEFAULT_FREQ    (400000)
#define I2C_DEFAULT_TIMEOUT (50000)

#define I2C_DAT_INDEX       (0)
#define I2C_TX_FIFO_LEN     (I2C_FIFO_DEPTH / 2)
#define I2C_RX_FIFO_LEN     (I2C_FIFO_DEPTH / 2)

#define I2C_SPEED(freq) \
    ((freq) <= 100000 ? I2C_SPEED_STANDARD : \
    ((freq) <= 400000 ? I2C_SPEED_FAST : \
    I2C_SPEED_FASTPLUS))

#define I2C_IC_CON_SPEED(freq) \
    ((freq) <= 100000 ? I2C_IC_CON_SPEED_STANDARD : \
    ((freq) <= 400000 ? I2C_IC_CON_SPEED_FAST : \
    I2C_IC_CON_SPEED_HIGH))
#define I2C_IC_CON_MASTER_TX_EMPTY_CTRL (1 << 8)

#define I2C_IC_STATUS_RFNE  I2C_IC_STATUS_RECEIVE_FIFO_NOT_EMPTY
#define I2C_IC_STATUS_TFNF  I2C_IC_STATUS_TRANSMIT_FIFO_NOT_FULL
#define I2C_STAT_ERRORS     (I2C_IC_INTR_STAT_TX_ABRT | I2C_IC_INTR_STAT_TX_OVER | \
    I2C_IC_INTR_STAT_RX_OVER | I2C_IC_INTR_STAT_RX_UNDER)

#define debug_printf(...) // mp_printf(&mp_plat_print, "i2c.c: " __VA_ARGS__)
#define I2C_CHECK_ERRORS(base) \
    if (base->I2C_RAW_INTR_STAT & I2C_STAT_ERRORS) { \
        uint32_t status = base->I2C_RAW_INTR_STAT; \
        debug_printf("status: 0x%lx raw_int: 0x%lx abort: 0x%lx line: %d\n", \
    base->I2C_STATUS, status, base->I2C_TX_ABRT_SOURCE, __LINE__); \
        (void)status; \
        (void)base->I2C_CLR_TX_ABRT; \
        (void)base->I2C_CLR_ACTIVITY; \
        return -MP_EIO; \
    }

typedef struct _machine_i2c_obj_t {
    mp_obj_base_t base;
    uint32_t i2c_id;
    I2C_Type *i2c;
    mp_hal_pin_obj_t scl;
    mp_hal_pin_obj_t sda;
    uint32_t freq;
    uint32_t timeout;
} machine_i2c_obj_t;

static machine_i2c_obj_t machine_i2c_obj[] = {
    #if defined(MICROPY_HW_I2C0_SCL)
    [0] = {{&machine_i2c_type}, 0, (I2C_Type *)I2C0_BASE, MICROPY_HW_I2C0_SCL, MICROPY_HW_I2C0_SDA},
    #endif
    #if defined(MICROPY_HW_I2C1_SCL)
    [1] = {{&machine_i2c_type}, 1, (I2C_Type *)I2C1_BASE, MICROPY_HW_I2C1_SCL, MICROPY_HW_I2C1_SDA},
    #endif
    #if defined(MICROPY_HW_I2C2_SCL)
    [2] = {{&machine_i2c_type}, 2, (I2C_Type *)I2C2_BASE, MICROPY_HW_I2C2_SCL, MICROPY_HW_I2C2_SDA},
    #endif
    #if defined(MICROPY_HW_I2C3_SCL)
    [3] = {{&machine_i2c_type}, 3, (I2C_Type *)I2C3_BASE, MICROPY_HW_I2C3_SCL, MICROPY_HW_I2C3_SDA},
    #endif
};

static void machine_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2C(%u, freq=%u, scl=%q, sda=%q, timeout=%u)",
        self->i2c_id, self->freq, self->scl->name, self->sda->name, self->timeout);
}

mp_obj_t machine_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_freq, ARG_scl, ARG_sda, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = I2C_DEFAULT_FREQ} },
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = I2C_DEFAULT_TIMEOUT} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get I2C bus.
    int i2c_id = mp_obj_get_int(args[ARG_id].u_obj);
    if (i2c_id < 0 || i2c_id >= MP_ARRAY_SIZE(machine_i2c_obj) || !machine_i2c_obj[i2c_id].i2c) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2C(%d) doesn't exist"), i2c_id);
    }

    // Get static peripheral object.
    machine_i2c_obj_t *self = &machine_i2c_obj[i2c_id];

    // Set args
    self->freq = args[ARG_freq].u_int;
    self->timeout = args[ARG_timeout].u_int;

    // here we would check the scl/sda pins and configure them, but it's not implemented
    if (args[ARG_scl].u_obj != mp_const_none || args[ARG_sda].u_obj != mp_const_none) {
        mp_raise_ValueError(MP_ERROR_TEXT("explicit choice of scl/sda is not implemented"));
    }

    // Disable I2C controller.
    i2c_disable(self->i2c);

    // Configure I2C pins.
    mp_hal_pin_config(self->scl, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP,
        MP_HAL_PIN_SPEED_LOW, MP_HAL_PIN_DRIVE_8MA, MP_HAL_PIN_ALT(I2C_SCL, self->i2c_id), true);
    mp_hal_pin_config(self->sda, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP,
        MP_HAL_PIN_SPEED_LOW, MP_HAL_PIN_DRIVE_8MA, MP_HAL_PIN_ALT(I2C_SDA, self->i2c_id), true);

    // Initialize I2C controller.
    self->i2c->I2C_CON = I2C_IC_CON_ENABLE_MASTER_MODE |
        I2C_IC_CON_MASTER_RESTART_EN |
        I2C_IC_CON_MASTER_TX_EMPTY_CTRL |
        I2C_IC_CON_SPEED(self->freq);

    // Configure FIFO threshold.
    self->i2c->I2C_TX_TL = I2C_TX_FIFO_LEN;
    self->i2c->I2C_RX_TL = I2C_RX_FIFO_LEN;

    // Configure clock.
    i2c_master_set_clock(self->i2c, GetSystemAPBClock() / 1000, I2C_SPEED(self->freq));

    // Enable I2C controller.
    i2c_clear_all_interrupt(self->i2c);
    i2c_enable(self->i2c);

    return MP_OBJ_FROM_PTR(self);
}

static int machine_i2c_poll_flags(I2C_Type *base, uint32_t flags, uint32_t timeout_us) {
    mp_uint_t tick_start = mp_hal_ticks_us();
    while (!(base->I2C_STATUS & flags)) {
        I2C_CHECK_ERRORS(base);
        if ((mp_hal_ticks_us() - tick_start) >= timeout_us) {
            return -MP_ETIMEDOUT;
        }
        // Can't delay or handle pending events here otherwise we risk
        // the FIFO getting empty, which will generate a STOP condition.
    }
    return 0;
}

static int machine_i2c_write(machine_i2c_obj_t *self, uint8_t *buf, size_t tx_size) {
    mp_uint_t tick_start = mp_hal_ticks_us();
    for (size_t tx_idx = 0; tx_idx < tx_size;) {
        // Write data to FIFO
        if (self->i2c->I2C_STATUS & I2C_IC_STATUS_TFNF) {
            self->i2c->I2C_DATA_CMD = (uint16_t)buf[tx_idx++];
            I2C_CHECK_ERRORS(self->i2c);
            tick_start = mp_hal_ticks_us();
        }

        // Check for timeout
        if ((mp_hal_ticks_us() - tick_start) >= self->timeout) {
            return -MP_ETIMEDOUT;
        }
    }
    return 0;
}

static int machine_i2c_read(machine_i2c_obj_t *self, uint8_t *buf, size_t rx_size) {
    mp_uint_t tick_start = mp_hal_ticks_us();
    for (size_t tx_idx = 0, rx_idx = 0; rx_idx < rx_size;) {
        // Write command to FIFO
        if (tx_idx < rx_size && (self->i2c->I2C_STATUS & I2C_IC_STATUS_TFNF)) {
            self->i2c->I2C_DATA_CMD = I2C_IC_DATA_CMD_READ_REQ;
            I2C_CHECK_ERRORS(self->i2c);
            ++tx_idx;
        }

        // Read data from FIFO
        while (rx_idx < rx_size && (self->i2c->I2C_STATUS & I2C_IC_STATUS_RFNE)) {
            buf[rx_idx++] = self->i2c->I2C_DATA_CMD & 0xFF;
            tick_start = mp_hal_ticks_us();
        }

        // Check for timeout
        if ((mp_hal_ticks_us() - tick_start) >= self->timeout) {
            return -MP_ETIMEDOUT;
        }
    }
    return 0;
}

int machine_i2c_transfer(mp_obj_base_t *self_in, uint16_t addr, size_t n, mp_machine_i2c_buf_t *bufs, unsigned int flags) {
    int ret = 0;
    uint32_t bytes = 0;
    machine_i2c_obj_t *self = (machine_i2c_obj_t *)self_in;

    // The DesignWare I2C IP on AE3 is configured such that it auto-generates a STOP
    // condition when the TX FIFO gets empty. In other words, the code can't have any
    // control over STOP condition generation. The only fix for this would be to buffer
    // complete read/write sequences and send them out when the STOP flag is set.
    if (!(flags & MP_MACHINE_I2C_FLAG_STOP)) {
        mp_raise_ValueError(MP_ERROR_TEXT("nostop flag is not supported"));
    }

    i2c_clear_all_interrupt(self->i2c);
    i2c_set_target_addr(self->i2c, addr, I2C_7BIT_ADDRESS, 0);

    // Workaround issue with hardware I2C not accepting zero-length writes.
    if (!bufs->len) {
        mp_machine_i2c_buf_t bufs = { 0 };

        mp_machine_soft_i2c_obj_t soft_i2c = {
            .base = { &mp_machine_soft_i2c_type },
            .scl = self->scl,
            .sda = self->sda,
            .us_timeout = self->timeout,
            .us_delay = 500000 / self->freq + 1,
        };

        // Switch pins to GPIO/OD.
        mp_hal_pin_config(self->scl, MP_HAL_PIN_MODE_OPEN_DRAIN, MP_HAL_PIN_PULL_UP,
            MP_HAL_PIN_SPEED_LOW, MP_HAL_PIN_DRIVE_8MA, 0, true);
        mp_hal_pin_config(self->sda, MP_HAL_PIN_MODE_OPEN_DRAIN, MP_HAL_PIN_PULL_UP,
            MP_HAL_PIN_SPEED_LOW, MP_HAL_PIN_DRIVE_8MA, 0, true);

        // Perform the transfer.
        ret = mp_machine_soft_i2c_transfer(&soft_i2c.base, addr, 1, &bufs, flags);

        // Re-configure I2C pins.
        mp_hal_pin_config(self->scl, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP,
            MP_HAL_PIN_SPEED_LOW, MP_HAL_PIN_DRIVE_8MA, MP_HAL_PIN_ALT(I2C_SCL, self->i2c_id), true);
        mp_hal_pin_config(self->sda, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP,
            MP_HAL_PIN_SPEED_LOW, MP_HAL_PIN_DRIVE_8MA, MP_HAL_PIN_ALT(I2C_SDA, self->i2c_id), true);

        return ret;
    }

    for (size_t i = 0; i < n; i++) {
        mp_machine_i2c_buf_t *buf = &bufs[i];
        if (i == 0 && (flags & MP_MACHINE_I2C_FLAG_WRITE1)) {
            ret = machine_i2c_write(self, buf->buf, buf->len);
        } else if (flags & MP_MACHINE_I2C_FLAG_READ) {
            ret = machine_i2c_read(self, buf->buf, buf->len);
        } else if (bufs->len != 0) {
            ret = machine_i2c_write(self, buf->buf, buf->len);
        }
        if (ret < 0) {
            return ret;
        }
        bytes += bufs->len;
    }

    // Wait for TX FIFO empty
    ret = machine_i2c_poll_flags(self->i2c, I2C_IC_STATUS_TFE, self->timeout);
    if (ret < 0) {
        return ret;
    }

    return bytes;
}

static const mp_machine_i2c_p_t machine_i2c_p = {
    .transfer_supports_write1 = true,
    .transfer = machine_i2c_transfer,
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
#endif // MICROPY_HW_ENABLE_HW_I2C

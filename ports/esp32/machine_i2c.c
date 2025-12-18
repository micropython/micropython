/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
 * Copyright (c) 2025 Vincent1-python
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
#include "machine_i2c.h"

#if MICROPY_HW_ESP_NEW_I2C_DRIVER
#include "driver/i2c_master.h"
#else
#include "driver/i2c.h"
#include "esp_clk_tree.h"
#include "hal/i2c_ll.h"
#endif

#if MICROPY_PY_MACHINE_I2C || MICROPY_PY_MACHINE_SOFTI2C

#define I2C_DEFAULT_TIMEOUT_US (50000) // 50ms

// CONFIG_I2C_SKIP_LEGACY_CONFLICT_CHECK is set if the related sdkconfig
// option is set.

#if MICROPY_HW_ESP_NEW_I2C_DRIVER

typedef struct _machine_hw_i2c_obj_t {
    mp_obj_base_t base;
    i2c_master_bus_handle_t bus_handle;
    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 5, 0)
    i2c_master_dev_handle_t dev_handle;
    #endif
    uint8_t port : 8;
    gpio_num_t scl : 8;
    gpio_num_t sda : 8;
    uint32_t freq;
    uint32_t timeout_us;
} machine_hw_i2c_obj_t;

static machine_hw_i2c_obj_t machine_hw_i2c_obj[I2C_NUM_MAX];

static void machine_hw_i2c_init(machine_hw_i2c_obj_t *self, bool first_init) {

    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 5, 0)
    if (!first_init && self->dev_handle) {
        i2c_master_bus_rm_device(self->dev_handle);
        self->dev_handle = NULL;
    }
    #endif

    if (!first_init && self->bus_handle) {
        i2c_del_master_bus(self->bus_handle);
        self->bus_handle = NULL;
    }

    i2c_master_bus_config_t bus_cfg = {
        .i2c_port = self->port,
        .scl_io_num = self->scl,
        .sda_io_num = self->sda,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &self->bus_handle));
    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 5, 0)
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0,  // Will be replaced
        .scl_speed_hz = self->freq,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(self->bus_handle, &dev_cfg, &self->dev_handle));
    #endif
}

static uint8_t *create_transfer_buffer(size_t n, mp_machine_i2c_buf_t *bufs, size_t *len_ptr) {
    size_t len = 0;
    uint8_t *buf;
    if (n == 1) {
        // Use given single buffer
        len = bufs[0].len;
        buf = bufs[0].buf;
    } else {
        // Allocate a buffer that can hold the data from all buffers
        len = 0;
        for (size_t i = 0; i < n; ++i) {
            len += bufs[i].len;
        }
        buf = m_new(uint8_t, len);
    }
    *len_ptr = len;
    return buf;
}

int machine_hw_i2c_transfer(mp_obj_base_t *self_in, uint16_t addr, size_t n, mp_machine_i2c_buf_t *bufs, unsigned int flags) {
    machine_hw_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Probe the address to see if any device responds.
    // This test uses a fixed scl freq of 100_000.
    esp_err_t err = i2c_master_probe(self->bus_handle, addr, self->timeout_us / 1000);
    if (err != ESP_OK) {
        return -MP_ENODEV;   // No device at address, return immediately
    }

    #if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 5, 0)
    // Using ".device_address = I2C_DEVICE_ADDRESS_NOT_USED," below
    // allows to write the address separately using the
    // i2c_master_execute_defined_operations() API.
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = self->freq,
    };
    i2c_master_dev_handle_t dev_handle;
    err = i2c_master_bus_add_device(self->bus_handle, &dev_cfg, &dev_handle);
    #else
    #define dev_handle self->dev_handle
    err = i2c_master_device_change_address(dev_handle, addr, self->timeout_us / 1000);
    #endif
    if (err != ESP_OK) {
        return -MP_ENODEV;
    }

    size_t len = 0;
    uint8_t *buf;

    // Assume that with MP_MACHINE_I2C_FLAG_WRITE1 set the first
    // buffer has to be written and all others have to be read.
    // extmod/read_mem() uses only a single buffer for reading, but
    // the other implementation(s) support multiple buffers.
    if (flags & MP_MACHINE_I2C_FLAG_WRITE1) {
        // create a large buffer if needed
        buf = create_transfer_buffer(n - 1, bufs + 1, &len);
        // Do a write then read
        err = i2c_master_transmit_receive(dev_handle, bufs[0].buf, bufs[0].len, buf, len, self->timeout_us / 1000);
        // Copy the data back if needed starting with the second buffer.
        if (n > 2) {
            len = 0;
            for (size_t i = 1; i < n; ++i) {
                memcpy(bufs[i].buf, buf + len, bufs[i].len);
                len += bufs[i].len;
            }
            m_del(uint8_t, buf, len);
        }
        len += bufs[0].len;
    } else if (bufs->len > 0) {
        buf = create_transfer_buffer(n, bufs, &len);
        // Transfer data and copy it from/to the buffers as needed.
        if (flags & MP_MACHINE_I2C_FLAG_READ) {
            err = i2c_master_receive(dev_handle, buf, len, self->timeout_us / 1000);
            if (n > 1) {
                len = 0;
                for (size_t i = 0; i < n; ++i) {
                    memcpy(bufs[i].buf, buf + len, bufs[i].len);
                    len += bufs[i].len;
                }
            }
        } else {
            if (n > 1) {
                len = 0;
                for (size_t i = 0; i < n; ++i) {
                    memcpy(buf + len, bufs[i].buf, bufs[i].len);
                    len += bufs[i].len;
                }
            }
            err = i2c_master_transmit(dev_handle, buf, len, self->timeout_us / 1000);
            // Use i2c_master_execute_defined_operations() instead of
            // i2c_master_transmit(), allowing for len == 0.
            // That will be needed for scan() when dropping i2c_master_probe() is possible,
            // after https://github.com/espressif/esp-idf/issues/17543 backported to supported versions
            // i2c_operation_job_t i2c_ops[] = {
            //     { .command = I2C_MASTER_CMD_START },
            //     { .command = I2C_MASTER_CMD_WRITE, .write = { .ack_check = true, .data = buf, .total_bytes = len } },
            //     { .command = I2C_MASTER_CMD_STOP },  // Stop is still mandatory
            // };
            // err = i2c_master_execute_defined_operations(dev_handle, i2c_ops, 3, self->timeout_us / 1000);
        }
        if (n > 1) {
            m_del(uint8_t, buf, len);
        }
    }
    #if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 5, 0)
    // Remove the temporary handle.
    i2c_master_bus_rm_device(dev_handle);
    #endif

    // Map errors
    if (err == ESP_FAIL) {
        return -MP_ENODEV;
    }
    if (err == ESP_ERR_TIMEOUT) {
        return -MP_ETIMEDOUT;
    }
    if (err != ESP_OK) {
        return -abs(err);
    }
    return len;
}

#else

#if SOC_I2C_SUPPORT_XTAL
#if CONFIG_XTAL_FREQ > 0
#define I2C_SCLK_FREQ (CONFIG_XTAL_FREQ * 1000000)
#elif CONFIG_XTAL_FREQ == 0 && CONFIG_IDF_TARGET_ESP32C5
// The crystal is auto-detected, so the I2C sclk frequency will be computed at runtime.
#else
#error "I2C uses XTAL but no configured freq"
#endif // CONFIG_XTAL_FREQ
#elif SOC_I2C_SUPPORT_APB
#define I2C_SCLK_FREQ APB_CLK_FREQ
#else
#error "unsupported I2C for ESP32 SoC variant"
#endif

typedef struct _machine_hw_i2c_obj_t {
    mp_obj_base_t base;
    i2c_port_t port : 8;
    gpio_num_t scl : 8;
    gpio_num_t sda : 8;
    uint32_t freq;
    uint32_t timeout_us;
} machine_hw_i2c_obj_t;

static machine_hw_i2c_obj_t machine_hw_i2c_obj[I2C_NUM_MAX];

static void machine_hw_i2c_init(machine_hw_i2c_obj_t *self, bool first_init) {
    if (!first_init) {
        i2c_driver_delete(self->port);
    }
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = self->sda,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = self->scl,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = self->freq,
    };
    i2c_param_config(self->port, &conf);
    #if CONFIG_IDF_TARGET_ESP32C5
    uint32_t i2c_sclk_freq;
    check_esp_err(esp_clk_tree_src_get_freq_hz(I2C_CLK_SRC_DEFAULT, ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX, &i2c_sclk_freq));
    #else
    uint32_t i2c_sclk_freq = I2C_SCLK_FREQ;
    #endif
    int timeout = i2c_ll_calculate_timeout_us_to_reg_val(i2c_sclk_freq, self->timeout_us);
    i2c_set_timeout(self->port, (timeout > I2C_LL_MAX_TIMEOUT) ? I2C_LL_MAX_TIMEOUT : timeout);
    i2c_driver_install(self->port, I2C_MODE_MASTER, 0, 0, 0);
}

int machine_hw_i2c_transfer(mp_obj_base_t *self_in, uint16_t addr, size_t n, mp_machine_i2c_buf_t *bufs, unsigned int flags) {
    machine_hw_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    int data_len = 0;

    if (flags & MP_MACHINE_I2C_FLAG_WRITE1) {
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, addr << 1, true);
        i2c_master_write(cmd, bufs->buf, bufs->len, true);
        data_len += bufs->len;
        --n;
        ++bufs;
    }

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr << 1 | (flags & MP_MACHINE_I2C_FLAG_READ), true);

    for (; n--; ++bufs) {
        if (flags & MP_MACHINE_I2C_FLAG_READ) {
            i2c_master_read(cmd, bufs->buf, bufs->len, n == 0 ? I2C_MASTER_LAST_NACK : I2C_MASTER_ACK);
        } else {
            if (bufs->len != 0) {
                i2c_master_write(cmd, bufs->buf, bufs->len, true);
            }
        }
        data_len += bufs->len;
    }

    if (flags & MP_MACHINE_I2C_FLAG_STOP) {
        i2c_master_stop(cmd);
    }

    // TODO proper timeout
    esp_err_t err = i2c_master_cmd_begin(self->port, cmd, 100 * (1 + data_len) / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (err == ESP_FAIL) {
        return -MP_ENODEV;
    } else if (err == ESP_ERR_TIMEOUT) {
        return -MP_ETIMEDOUT;
    } else if (err != ESP_OK) {
        return -abs(err);
    }

    return data_len;
}


#endif  // MICROPY_HW_ESP_NEW_I2C_DRIVER

/******************************************************************************/
// MicroPython bindings for machine API

static void machine_hw_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hw_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2C(%u, scl=%u, sda=%u, freq=%u, timeout=%u)",
        self->port, self->scl, self->sda, self->freq, self->timeout_us);
}

mp_obj_t machine_hw_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Create a SoftI2C instance if no id is specified (or is -1) but other arguments are given
    if (n_args != 0) {
        MP_MACHINE_I2C_CHECK_FOR_LEGACY_SOFTI2C_CONSTRUCTION(n_args, n_kw, all_args);
    }

    // Parse args
    enum { ARG_id, ARG_scl, ARG_sda, ARG_freq, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_INT, {.u_int = I2C_NUM_0} },
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t i2c_id = args[ARG_id].u_int;
    if (!(I2C_NUM_0 <= i2c_id && i2c_id < I2C_NUM_MAX)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2C(%d) doesn't exist"), i2c_id);
    }

    machine_hw_i2c_obj_t *self = &machine_hw_i2c_obj[i2c_id];

    bool first_init = (self->base.type == NULL);
    if (first_init) {
        self->base.type = &machine_i2c_type;
        self->port = i2c_id;
        self->scl = (i2c_id == I2C_NUM_0) ? MICROPY_HW_I2C0_SCL : MICROPY_HW_I2C1_SCL;
        self->sda = (i2c_id == I2C_NUM_0) ? MICROPY_HW_I2C0_SDA : MICROPY_HW_I2C1_SDA;
        self->freq = 400000;
        self->timeout_us = I2C_DEFAULT_TIMEOUT_US;
    }

    if (args[ARG_scl].u_obj != MP_OBJ_NULL) {
        self->scl = machine_pin_get_id(args[ARG_scl].u_obj);
    }
    if (args[ARG_sda].u_obj != MP_OBJ_NULL) {
        self->sda = machine_pin_get_id(args[ARG_sda].u_obj);
    }
    if (args[ARG_freq].u_int != -1) {
        self->freq = args[ARG_freq].u_int;
    }
    if (args[ARG_timeout].u_int != -1) {
        self->timeout_us = args[ARG_timeout].u_int;
    }

    machine_hw_i2c_init(self, first_init);
    return MP_OBJ_FROM_PTR(self);
}

static const mp_machine_i2c_p_t machine_hw_i2c_p = {
    .transfer_supports_write1 = true,
    .transfer = machine_hw_i2c_transfer,
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

#endif // MICROPY_PY_MACHINE_I2C || MICROPY_PY_MACHINE_SOFTI2C

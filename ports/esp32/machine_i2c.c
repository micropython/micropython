/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#include "driver/i2c_master.h"

#if MICROPY_PY_MACHINE_I2C || MICROPY_PY_MACHINE_SOFTI2C

#ifndef MICROPY_HW_I2C0_SCL
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
#define MICROPY_HW_I2C0_SCL (GPIO_NUM_9)
#define MICROPY_HW_I2C0_SDA (GPIO_NUM_8)
#else
#define MICROPY_HW_I2C0_SCL (GPIO_NUM_18)
#define MICROPY_HW_I2C0_SDA (GPIO_NUM_19)
#endif
#endif

#ifndef MICROPY_HW_I2C1_SCL
#if CONFIG_IDF_TARGET_ESP32
#define MICROPY_HW_I2C1_SCL (GPIO_NUM_25)
#define MICROPY_HW_I2C1_SDA (GPIO_NUM_26)
#else
#define MICROPY_HW_I2C1_SCL (GPIO_NUM_9)
#define MICROPY_HW_I2C1_SDA (GPIO_NUM_8)
#endif
#endif

#if SOC_I2C_SUPPORT_XTAL
#define I2C_SCLK_FREQ XTAL_CLK_FREQ
#elif SOC_I2C_SUPPORT_APB
#define I2C_SCLK_FREQ APB_CLK_FREQ
#else
#error "unsupported I2C for ESP32 SoC variant"
#endif

#define I2C_DEFAULT_TIMEOUT_US (50000) // 50ms

// ---------------- 内部数据结构 ----------------
typedef struct _machine_hw_i2c_obj_t {
    mp_obj_base_t base;
    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;
    uint8_t port;
    gpio_num_t scl;
    gpio_num_t sda;
} machine_hw_i2c_obj_t;

static machine_hw_i2c_obj_t machine_hw_i2c_obj[I2C_NUM_MAX];

// ---------------- 初始化 ----------------
static void machine_hw_i2c_init(machine_hw_i2c_obj_t *self,
                                uint32_t freq,
                                uint32_t timeout_us,
                                bool first_init) {
     
    // 1. 若已初始化，先卸载旧驱动
    if (!first_init && self->bus_handle) {
        i2c_master_bus_rm_device(self->dev_handle);
        i2c_del_master_bus(self->bus_handle);
        self->bus_handle = NULL;
        self->dev_handle = NULL;
    }

    // 2. 配置总线
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port = self->port,
        .scl_io_num = self->scl,
        .sda_io_num = self->sda,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &self->bus_handle));

    // 3. 添加设备（占位地址，后面真正传输时再动态改）
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x00,               // 占位
        .scl_speed_hz = freq,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(self->bus_handle, &dev_cfg, &self->dev_handle));
}

int machine_hw_i2c_transfer(mp_obj_base_t *self_in,
                            uint16_t addr,
                            size_t n,
                            mp_machine_i2c_buf_t *bufs,
                            unsigned int flags)
{
    machine_hw_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);

    /* 0. 先探测地址是否有设备回应 */
    esp_err_t err = i2c_master_probe(self->bus_handle, addr, 1000);
    if (err != ESP_OK) {
        return -MP_ENODEV;          /* 地址无设备，直接返回 */
    }
    /* 1. 为本次事务创建临时 device 句柄 */
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = addr,
        .scl_speed_hz    = 100000,   /* 沿用总线频率即可 */
    };
    i2c_master_dev_handle_t dev_handle;
    err = i2c_master_bus_add_device(self->bus_handle, &dev_cfg, &dev_handle);
    if (err != ESP_OK) {
        return -MP_ENODEV;
    }

    int data_len = 0;

    /* 2. 若有 WRITE1 段，先写一段 */
    if (flags & MP_MACHINE_I2C_FLAG_WRITE1) {
        if (bufs->len) {
            err = i2c_master_transmit(dev_handle,
                                      bufs->buf,
                                      bufs->len,
                                      1000);                 /* 阻塞超时 1 s */
            if (err != ESP_OK) goto cleanup;
        }
        data_len += bufs->len;
        --n;
        ++bufs;
    }
    if (flags & MP_MACHINE_I2C_FLAG_READ) {
	    /* 3. 主循环：剩余段 */
	    for (; n--; ++bufs) {
		if (bufs->len == 0) continue;
                err = i2c_master_receive(dev_handle,
	                             bufs->buf,
	                             //bufs->len,
	                             1,
	                             1000);
	        if (err != ESP_OK) break;

                data_len += bufs->len;
    }
    } else {
        // 写操作逻辑
        size_t total_len = 0;
        mp_machine_i2c_buf_t *original_bufs = bufs;  // 保存原始指针
        size_t yuann=n;

        // 计算总长度
        for (; n--; ++bufs) {
            total_len += bufs->len;
        }

        // 重置指针
        bufs = original_bufs;
	// 重置n
        n = yuann;
        // 动态分配 write_buf
        uint8_t *write_buf = (uint8_t *)malloc(total_len);
        if (write_buf == NULL) return -MP_ENOMEM;

        // 复制数据到 write_buf
        size_t index = 0;
        for (; n--; ++bufs) {
            memcpy(write_buf + index, bufs->buf, bufs->len);
            index += bufs->len;
        }

        // 发送数据
        err = i2c_master_transmit(dev_handle, write_buf, total_len, 1000);
        if (err != ESP_OK) goto cleanup;

        // 释放动态分配的内存
        free(write_buf);
    }

cleanup:
    /* 4. 立即销毁临时句柄 */
    i2c_master_bus_rm_device(dev_handle);

    /* 5. 出错映射 */
    if (err == ESP_FAIL)        return -MP_ENODEV;
    if (err == ESP_ERR_TIMEOUT) return -MP_ETIMEDOUT;
    if (err != ESP_OK)          return -abs(err);

    return data_len;
}

// ---------------- 打印 ----------------
static void machine_hw_i2c_print(const mp_print_t *print,
                                 mp_obj_t self_in,
                                 mp_print_kind_t kind) {
    machine_hw_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2C(%u, scl=%u, sda=%u)",
              self->port, self->scl, self->sda);
}

// ---------------- 构造函数 ----------------
mp_obj_t machine_hw_i2c_make_new(const mp_obj_type_t *type,
                                 size_t n_args, size_t n_kw,
                                 const mp_obj_t *all_args) {
    // Create a SoftI2C instance if no id is specified (or is -1) but other arguments are given
    if (n_args != 0) {
        MP_MACHINE_I2C_CHECK_FOR_LEGACY_SOFTI2C_CONSTRUCTION(n_args, n_kw, all_args);
    }
    enum { ARG_id, ARG_scl, ARG_sda, ARG_freq, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_INT, {.u_int = I2C_NUM_0} },
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 400000} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = I2C_DEFAULT_TIMEOUT_US} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args,
                              MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t i2c_id = args[ARG_id].u_int;
    if (!(I2C_NUM_0 <= i2c_id && i2c_id < I2C_NUM_MAX)) {
        mp_raise_msg_varg(&mp_type_ValueError,
                          MP_ERROR_TEXT("I2C(%d) doesn't exist"), i2c_id);
    }

    machine_hw_i2c_obj_t *self = &machine_hw_i2c_obj[i2c_id];

    bool first_init = (self->base.type == NULL);
    if (first_init) {
        self->base.type = &machine_i2c_type;
        self->port = i2c_id;
        self->scl = (i2c_id == I2C_NUM_0) ? MICROPY_HW_I2C0_SCL : MICROPY_HW_I2C1_SCL;
        self->sda = (i2c_id == I2C_NUM_0) ? MICROPY_HW_I2C0_SDA : MICROPY_HW_I2C1_SDA;
    }

    if (args[ARG_scl].u_obj != MP_OBJ_NULL) {
        self->scl = machine_pin_get_id(args[ARG_scl].u_obj);
    }
    if (args[ARG_sda].u_obj != MP_OBJ_NULL) {
        self->sda = machine_pin_get_id(args[ARG_sda].u_obj);
    }

    machine_hw_i2c_init(self,
                        args[ARG_freq].u_int,
                        args[ARG_timeout].u_int,
                        first_init);
    return MP_OBJ_FROM_PTR(self);
}

// ---------------- 协议表 ----------------
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

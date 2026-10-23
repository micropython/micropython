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

// port-specific
#include "clk.h"
#include "genhdr/pins_af.h"
#include "modmachine.h"
#include "i2c.h"

#define DEBUG_printf(...) // printf(__VA_ARGS__)

#define MACHINE_I2C_TARGET_RX_BUF_SIZE (256)

typedef struct {
    uint8_t *buf;
    size_t len;
    size_t index;
} i2c_target_buffer_t;

static void i2c_target_buffer_init_empty(i2c_target_buffer_t *buffer) {
    buffer->buf = NULL;
    buffer->len = 0;
    buffer->index = 0;
}

static void i2c_target_buffer_alloc(i2c_target_buffer_t *buffer, size_t len) {
    if (buffer->buf == NULL) {
        buffer->buf = m_new(uint8_t, len);
    } else if (buffer->len != len) {
        buffer->buf = m_renew(uint8_t, buffer->buf, buffer->len, len);
    }
    buffer->len = len;
    buffer->index = 0;
}

static void i2c_target_buffer_free(i2c_target_buffer_t *buffer) {
    m_free(buffer->buf);
    buffer->buf = NULL;
    buffer->len = 0;
    buffer->index = 0;
}

static void i2c_target_buffer_reset(i2c_target_buffer_t *buffer) {
    buffer->index = 0;
}

typedef struct _machine_i2c_target_obj_t {
    mp_obj_base_t base;
    mp_hal_pin_obj_t scl;
    mp_hal_pin_obj_t sda;
    uint32_t addr;
    uint8_t addrsize;
    mp_obj_t mem;
    size_t mem_addrsize;
    i2c_target_buffer_t rx;
    i2c_target_buffer_t tx;
    bool in_write_event;
    i2c_obj_t i2c_obj;
} machine_i2c_target_obj_t;

MP_REGISTER_ROOT_POINTER(struct _machine_i2c_target_obj_t *machine_i2c_target_obj[MICROPY_PY_MACHINE_I2C_NUM_ENTRIES]);

static machine_i2c_target_obj_t *machine_i2c_target_obj_get(uint8_t id);
static void i2c_target_obj_event_callback(machine_i2c_target_obj_t *self, uint32_t events);

#define DEFINE_I2C_TARGET_EVENT_CALLBACK(scb) \
    void i2c_##scb##_target_event_callback(uint32_t events) { \
        machine_i2c_target_obj_t *self = machine_i2c_target_obj_get(scb); \
        i2c_target_obj_event_callback(self, events); \
    }

/**
 * The file build-<board>/genhdr/pins_af.h contains the macro
 *
 *  MICROPY_PY_FOR_ALL_SCB(DO)
 *
 *  which uses the X-macro (as argument) pattern to pass a worker
 *  macro DO(port) for the list of all user available ports.
 *
 * The available (not hidden) user SCBs are those alternate
 * functions defined in the boards/pse8x_af.csv file, for which
 * the corresponding pin are available for the user.
 * The available pins are those defined in the
 * boards/<board>/pins.csv file, which are not prefixed
 * with a hyphen(-).
 * See tools/boardgen.py and psoc-edge/boards/make-pins.py
 * for more information.
 */

MICROPY_PY_FOR_ALL_SCB(DEFINE_I2C_TARGET_EVENT_CALLBACK)

#define I2C_TARGET_EVENT_CALLBACK_ENTRY(scb) \
    [scb] = i2c_##scb##_target_event_callback,

static cy_cb_scb_i2c_handle_events_t i2c_target_event_callback[MICROPY_PY_SCB_NUM_ENTRIES] = {
    MICROPY_PY_FOR_ALL_SCB(I2C_TARGET_EVENT_CALLBACK_ENTRY)
};


static bool i2c_target_handle_read_address(machine_i2c_target_obj_t *self, machine_i2c_target_data_t *data, uint32_t events) {
    data->state = STATE_IDLE;
    machine_i2c_target_data_addr_match(data, true);
    if (events & CY_SCB_I2C_SLAVE_WR_CMPLT_EVENT) {
        return true;
    }
    machine_i2c_target_data_read_request(self, data);
    if (data->mem_buf != NULL) {
        handle_event(data, I2C_TARGET_IRQ_READ_REQ);
    }
    return false;
}

static void i2c_target_handle_write_address(machine_i2c_target_data_t *data) {
    machine_i2c_target_data_addr_match(data, false);
}

static void i2c_target_handle_read_request(machine_i2c_target_obj_t *self, machine_i2c_target_data_t *data) {
    machine_i2c_target_data_read_request(self, data);
}

static void i2c_target_handle_read_complete(machine_i2c_target_obj_t *self, machine_i2c_target_data_t *data) {
    if (data->mem_buf != NULL && data->mem_len > 0) {
        i2c_hw_target_config_read_buff(&self->i2c_obj, data->mem_buf, data->mem_len);
    }
    i2c_hw_target_clear_read_status(&self->i2c_obj);
    i2c_target_buffer_reset(&self->tx);
    if (data->mem_buf != NULL && data->mem_len > 0) {
        data->mem_addr++;
        if (data->mem_addr >= data->mem_len) {
            data->mem_addr = 0;
        }
    }
    data->state = STATE_READING;
    machine_i2c_target_data_restart_or_stop(data);
}

static void i2c_target_handle_write_complete(machine_i2c_target_obj_t *self, machine_i2c_target_data_t *data, uint32_t events) {
    uint32_t bytes_received = i2c_hw_target_get_write_count(&self->i2c_obj);
    bool memory_address_only = data->mem_buf != NULL
        && data->mem_addrsize > 0
        && bytes_received == data->mem_addrsize;
    if (!(events & CY_SCB_I2C_SLAVE_ERR_EVENT)) {
        i2c_target_buffer_reset(&self->rx);
        if (data->mem_buf == NULL) {
            self->in_write_event = true;
            for (uint32_t i = 0; i < bytes_received; i++) {
                size_t rx_index = self->rx.index;
                handle_event(data, I2C_TARGET_IRQ_WRITE_REQ);
                if (self->rx.index == rx_index) {
                    break;
                }
            }
            self->in_write_event = false;
        } else {
            while (self->rx.index < bytes_received) {
                machine_i2c_target_data_write_request(self, data);
            }
        }
    }
    if (self->rx.buf != NULL && data->mem_buf != NULL) {
        i2c_hw_target_config_write_buff(&self->i2c_obj, self->rx.buf, self->rx.len);
    }
    if (self->rx.buf != NULL && data->mem_buf == NULL && self->rx.index >= bytes_received) {
        i2c_hw_target_config_write_buff(&self->i2c_obj, self->rx.buf, self->rx.len);
        i2c_target_buffer_reset(&self->rx);
    }
    i2c_hw_target_clear_write_status(&self->i2c_obj);
    if (memory_address_only) {
        data->state = STATE_IDLE;
    } else {
        data->state = STATE_WRITING;
        machine_i2c_target_data_restart_or_stop(data);
    }
}

static void i2c_target_handle_error(machine_i2c_target_data_t *data) {
    machine_i2c_target_data_restart_or_stop(data);
}

static void i2c_target_obj_event_callback(machine_i2c_target_obj_t *self, uint32_t events) {
    if (self == NULL) {
        return;
    }

    machine_i2c_target_data_t *data = &machine_i2c_target_data[i2c_get_id(&self->i2c_obj)];
    bool read_request_pending = false;
    if (events & CY_SCB_I2C_SLAVE_READ_EVENT) {
        read_request_pending = i2c_target_handle_read_address(self, data, events);
    }

    if (events & CY_SCB_I2C_SLAVE_WRITE_EVENT) {
        i2c_target_handle_write_address(data);
    }

    if (events & CY_SCB_I2C_SLAVE_RD_BUF_EMPTY_EVENT) {
        i2c_target_handle_read_request(self, data);
    }

    if (events & CY_SCB_I2C_SLAVE_RD_IN_FIFO_EVENT) {
        if (data->mem_buf == NULL) {
            i2c_target_handle_read_request(self, data);
        }
    }

    if (events & CY_SCB_I2C_SLAVE_RD_CMPLT_EVENT) {
        i2c_target_handle_read_complete(self, data);
    }

    if (events & CY_SCB_I2C_SLAVE_WR_CMPLT_EVENT) {
        i2c_target_handle_write_complete(self, data, events);
    }

    if (read_request_pending) {
        i2c_target_handle_read_request(self, data);
        if (data->mem_buf != NULL) {
            handle_event(data, I2C_TARGET_IRQ_READ_REQ);
        }
    }

    if (events & CY_SCB_I2C_SLAVE_ERR_EVENT) {
        i2c_target_handle_error(data);
    }
}

static void machine_i2c_target_scb_isr(mp_obj_t i2c_target_obj) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(i2c_target_obj);
    i2c_hw_target_irq(&self->i2c_obj);
}

static machine_i2c_target_obj_t *machine_i2c_target_obj_get(uint8_t id) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_I2C_NUM_ENTRIES; i++) {
        if (MP_STATE_PORT(machine_i2c_target_obj[i]) != NULL) {
            if (i2c_get_id(&MP_STATE_PORT(machine_i2c_target_obj[i])->i2c_obj) == id) {
                return MP_STATE_PORT(machine_i2c_target_obj[i]);
            }
        }
    }
    return NULL;
}

static inline machine_i2c_target_obj_t *machine_i2c_target_obj_alloc(void) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_I2C_NUM_ENTRIES; i++)
    {
        if (MP_STATE_PORT(machine_i2c_target_obj[i]) == NULL) {
            MP_STATE_PORT(machine_i2c_target_obj[i]) = mp_obj_malloc(machine_i2c_target_obj_t, &machine_i2c_target_type);
            i2c_target_buffer_init_empty(&MP_STATE_PORT(machine_i2c_target_obj[i])->rx);
            MP_STATE_PORT(machine_i2c_target_obj[i])->in_write_event = false;
            i2c_target_buffer_init_empty(&MP_STATE_PORT(machine_i2c_target_obj[i])->tx);
            memset(&MP_STATE_PORT(machine_i2c_target_obj[i])->i2c_obj, 0, sizeof(i2c_obj_t));
            return MP_STATE_PORT(machine_i2c_target_obj[i]);
        }
    }
    return NULL;
}

static inline void machine_i2c_target_obj_free(machine_i2c_target_obj_t *i2c_target_obj_ptr) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_I2C_NUM_ENTRIES; i++)
    {
        if (MP_STATE_PORT(machine_i2c_target_obj[i]) == i2c_target_obj_ptr) {
            MP_STATE_PORT(machine_i2c_target_obj[i]) = NULL;
        }
    }
}

static void machine_i2c_target_obj_init_data(machine_i2c_target_obj_t *self) {
    size_t id = i2c_get_id(&self->i2c_obj);
    MP_STATE_PORT(machine_i2c_target_mem_obj)[id] = self->mem;
    machine_i2c_target_data_init(&machine_i2c_target_data[id], self->mem, self->mem_addrsize);

    machine_i2c_target_data_t *data = &machine_i2c_target_data[id];
    size_t rx_len = (data->mem_len > MACHINE_I2C_TARGET_RX_BUF_SIZE) ? data->mem_len : MACHINE_I2C_TARGET_RX_BUF_SIZE;
    i2c_target_buffer_alloc(&self->tx, MACHINE_I2C_TARGET_RX_BUF_SIZE);
    i2c_target_buffer_alloc(&self->rx, rx_len);
}

static void machine_i2c_target_hw_init(machine_i2c_target_obj_t *self) {
    i2c_hw_target_init(&self->i2c_obj, self->addr);

    i2c_hw_target_register_callback(&self->i2c_obj, i2c_target_event_callback[i2c_get_id(&self->i2c_obj)]);

    machine_i2c_target_obj_init_data(self);

    machine_i2c_target_data_t *data = &machine_i2c_target_data[i2c_get_id(&self->i2c_obj)];
    if (data->mem_buf != NULL && data->mem_len > 0) {
        i2c_hw_target_config_read_buff(&self->i2c_obj, data->mem_buf, data->mem_len);
    }
    i2c_hw_target_config_write_buff(&self->i2c_obj, self->rx.buf, self->rx.len);

    DEBUG_printf("I2C Target initialized: addr=0x%02X, addrsize=%u-bit\n", self->addr, self->addrsize);
}

/******************************************************************************/
// I2CTarget port implementation

static inline size_t mp_machine_i2c_target_get_index(machine_i2c_target_obj_t *self) {
    return i2c_get_id(&self->i2c_obj);
}

// IRQ event callback - called from extmod to trigger Python IRQ handler
// This is called by handle_event() in extmod/machine_i2c_target.c
static void mp_machine_i2c_target_event_callback(machine_i2c_target_irq_obj_t *irq) {
    if (irq->base.handler != mp_const_none) {
        mp_irq_handler(&irq->base);
    }
}

static size_t mp_machine_i2c_target_read_bytes(machine_i2c_target_obj_t *self, size_t len, uint8_t *buf) {
    machine_i2c_target_data_t *data = &machine_i2c_target_data[i2c_get_id(&self->i2c_obj)];
    size_t read_len = 0;

    i2c_hw_target_irq_disable(&self->i2c_obj);

    uint32_t available = i2c_hw_target_get_write_count(&self->i2c_obj);
    available = (available > self->rx.index) ? available - self->rx.index : 0;
    read_len = (len < available) ? len : available;

    if (self->rx.buf != NULL) {
        for (size_t i = 0; i < read_len; i++) {
            if (self->rx.index < self->rx.len) {
                buf[i] = self->rx.buf[self->rx.index++];
            }
        }
        if (data->mem_buf == NULL && !self->in_write_event) {
            i2c_hw_target_config_write_buff(&self->i2c_obj, self->rx.buf, self->rx.len);
        }
    }

    i2c_hw_target_irq_enable(&self->i2c_obj);

    return read_len;
}

static size_t mp_machine_i2c_target_write_bytes(machine_i2c_target_obj_t *self, size_t len, const uint8_t *buf) {
    size_t write_len = 0;

    i2c_hw_target_irq_disable(&self->i2c_obj);

    if (self->tx.buf != NULL) {
        size_t tx_start = self->tx.index;
        for (size_t i = 0; i < len; i++) {
            if (self->tx.index < self->tx.len) {
                self->tx.buf[self->tx.index++] = buf[i];
                write_len++;
            }
        }
        i2c_hw_target_config_read_buff(&self->i2c_obj, self->tx.buf + tx_start, write_len);
    }

    i2c_hw_target_irq_enable(&self->i2c_obj);

    return write_len;
}

static void mp_machine_i2c_target_irq_config(machine_i2c_target_obj_t *self, unsigned int trigger) {
    // IRQ configuration already handled in init
}

enum { ARG_id, ARG_addr, ARG_addrsize, ARG_mem, ARG_mem_addrsize, ARG_scl, ARG_sda };
static const mp_arg_t allowed_args[] = {
    { MP_QSTR_id, MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_addr, MP_ARG_REQUIRED | MP_ARG_INT },
    { MP_QSTR_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 7} },
    { MP_QSTR_mem, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    { MP_QSTR_mem_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
    { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
};

static void machine_i2c_target_init_impl(machine_i2c_target_obj_t *self, int i2c_id, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_hal_pin_af_config_t i2c_pins_af_config[] = {
        MP_HAL_PIN_AF_CONF_INIT_GPIO_SIGNAL(CY_GPIO_DM_OD_DRIVESLOW, 1, MACHINE_PIN_AF_SIGNAL_I2C_SCL),
        MP_HAL_PIN_AF_CONF_INIT_GPIO_SIGNAL(CY_GPIO_DM_OD_DRIVESLOW, 1, MACHINE_PIN_AF_SIGNAL_I2C_SDA),
    };

    if (args[ARG_scl].u_obj != mp_const_none) {
        mp_hal_pin_obj_t scl_pin = mp_hal_get_pin_obj(args[ARG_scl].u_obj);
        MP_HAL_PIN_AF_CONF_SET_PIN_AF(i2c_pins_af_config[0], scl_pin);
    }

    if (args[ARG_sda].u_obj != mp_const_none) {
        mp_hal_pin_obj_t sda_pin = mp_hal_get_pin_obj(args[ARG_sda].u_obj);
        MP_HAL_PIN_AF_CONF_SET_PIN_AF(i2c_pins_af_config[1], sda_pin);
    }

    /* -- Resolve ID - pin match -- */
    machine_pin_af_unit_t fn_unit = (machine_pin_af_unit_t)i2c_id;
    mp_hal_periph_pins_af_resolve_fn_unit(i2c_pins_af_config, 2, MACHINE_PIN_AF_FN_I2C, &fn_unit);

    /* -- Resolve not provided AF pins -- */
    mp_hal_periph_pins_af_resolve_pin_af(i2c_pins_af_config, 2, fn_unit);

    /** -- Target address -- **/
    if (args[ARG_addrsize].u_int != 7 && args[ARG_addrsize].u_int != 10) {
        mp_raise_ValueError(MP_ERROR_TEXT("addrsize must be 7 or 10"));
    }
    uint8_t addrsize = args[ARG_addrsize].u_int;

    /** -- Memory address size -- **/
    if (args[ARG_mem_addrsize].u_int != 0 && args[ARG_mem_addrsize].u_int != 8) {
        mp_raise_ValueError(MP_ERROR_TEXT("mem_addrsize must be 0 or 8"));
    }
    uint32_t mem_addrsize = args[ARG_mem_addrsize].u_int;

    /* -- I2C params init -- */
    self->scl = i2c_pins_af_config[0].pin;
    self->sda = i2c_pins_af_config[1].pin;
    self->mem = args[ARG_mem].u_obj;
    self->addr = args[ARG_addr].u_int;
    self->addrsize = addrsize;
    self->mem_addrsize = mem_addrsize;

    /* -- Initialise hardware -- */
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_hal_periph_pins_af_init(i2c_pins_af_config, 2);
        bool is_new;
        i2c_alloc(&self->i2c_obj, fn_unit, self, machine_i2c_target_scb_isr, &is_new);
        if (!is_new) {
            i2c_hw_deinit(&self->i2c_obj);
        }
        machine_i2c_target_hw_init(self);
        nlr_pop();
    } else {
        // Ensure partially-initialized instances are fully released on init failure.
        i2c_target_buffer_free(&self->rx);
        i2c_target_buffer_free(&self->tx);
        i2c_hw_deinit(&self->i2c_obj);
        nlr_raise(nlr.ret_val);
    }
}

static mp_obj_t mp_machine_i2c_target_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    /**
     * Only the constructor takes the id.
     * Its validation together with the rest of the arguments is
     * delegated to machine_i2c_target_init_impl() which also allocates
     * the object self = NULL.
    */
    int i2c_id = MACHINE_PIN_AF_UNIT_NONE;
    size_t init_n_args = n_args;
    const mp_obj_t *init_args = args;
    if (n_args > 0 && mp_obj_is_int(args[0])) {
        i2c_id = mp_obj_get_int(args[0]);
        if (i2c_id < 0 || i2c_id >= MICROPY_PY_SCB_NUM_ENTRIES) {
            mp_raise_ValueError(MP_ERROR_TEXT("I2C id out of range"));
        }
        init_n_args = n_args - 1;
        init_args = args + 1;
    } else if (n_args > 0) {
        mp_raise_TypeError(MP_ERROR_TEXT("I2C id must be an integer"));
    }

    machine_i2c_target_obj_t *self = machine_i2c_target_obj_get(i2c_id);
    if (self == NULL) {
        self = machine_i2c_target_obj_alloc();
        if (self == NULL) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("failed to allocate I2CTarget object"));
        }
    }

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        machine_i2c_target_init_impl(self, i2c_id, init_n_args, init_args, &kw_args);
        nlr_pop();
    } else {
        i2c_hw_deinit(&self->i2c_obj);
        i2c_free(&self->i2c_obj);
        machine_i2c_target_obj_free(self);
        nlr_raise(nlr.ret_val);
    }

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_i2c_target_deinit(machine_i2c_target_obj_t *self) {
    i2c_hw_deinit(&self->i2c_obj);
    i2c_free(&self->i2c_obj);
    i2c_target_buffer_free(&self->rx);
    i2c_target_buffer_free(&self->tx);
    machine_i2c_target_obj_free(self);

    DEBUG_printf("I2C Target deinitialized\n");
}

void machine_i2c_target_deinit_all(void) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_I2C_NUM_ENTRIES; i++) {
        if (MP_STATE_PORT(machine_i2c_target_obj[i]) != NULL) {
            mp_machine_i2c_target_deinit(MP_STATE_PORT(machine_i2c_target_obj[i]));
        }
    }
}

static void mp_machine_i2c_target_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2CTarget(%u, addr=0x%02X, scl='%q', sda='%q')",
        i2c_get_id(&self->i2c_obj), self->addr, self->scl->name, self->sda->name);
}

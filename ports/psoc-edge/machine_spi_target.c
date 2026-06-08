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

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include <string.h>
#include "cybsp.h"
#include "cy_scb_spi.h"
#include "cy_sysclk.h"
#include "genhdr/pins_af.h"
#include "machine_scb.h"
#include "modmachine.h"

#if MICROPY_PY_MACHINE_SPI_TARGET

#define DEFAULT_SPI_TARGET_POLARITY    (0)
#define DEFAULT_SPI_TARGET_PHASE       (0)
#define DEFAULT_SPI_TARGET_BITS        (8)
#define SPI_TARGET_TICKS_WRAP_MS       (15000U) // from mp_hal_ticks_ms source timer period in modtime.c
#define DEFAULT_SPI_TARGET_TIMEOUT_MS  (5000U)  // must stay below SPI_TARGET_TICKS_WRAP_MS

#define SPI_TARGET_CLK_DIV_TYPE        CY_SYSCLK_DIV_8_BIT
#define SPI_TARGET_CLK_DIV_BASE        (4U)

// mp_hal_ticks_ms() on this port wraps every SPI_TARGET_TICKS_WRAP_MS.
// This computes elapsed time across one wrap; caller keeps timeout < wrap period.
static inline uint32_t spi_target_elapsed_ms(uint32_t start_ms) {
    uint32_t now = mp_hal_ticks_ms();
    return (now >= start_ms) ? (now - start_ms) : (SPI_TARGET_TICKS_WRAP_MS - start_ms + now);
}

typedef struct _machine_spi_target_obj_t {
    mp_obj_base_t base;
    mp_hal_pin_obj_t sck;
    mp_hal_pin_obj_t mosi;
    mp_hal_pin_obj_t miso;
    mp_hal_pin_obj_t ssel;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
    uint8_t firstbit;
    uint32_t timeout;
    uint8_t div_num;  // Clock divider number for PCLK
    machine_scb_obj_t *scb_obj;
    cy_stc_scb_spi_config_t cfg;
    cy_stc_scb_spi_context_t ctx;
} machine_spi_target_obj_t;

machine_spi_target_obj_t *machine_spi_target_obj[MICROPY_PY_MACHINE_SPI_TARGET_MAX] = { NULL };

static inline machine_spi_target_obj_t *machine_spi_target_obj_alloc(void) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_SPI_TARGET_MAX; i++) {
        if (machine_spi_target_obj[i] == NULL) {
            machine_spi_target_obj[i] = mp_obj_malloc(machine_spi_target_obj_t, &machine_spi_target_type);
            machine_spi_target_obj[i]->div_num = SPI_TARGET_CLK_DIV_BASE + i;
            return machine_spi_target_obj[i];
        }
    }
    return NULL;
}

static inline void machine_spi_target_obj_free(machine_spi_target_obj_t *obj) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_SPI_TARGET_MAX; i++) {
        if (machine_spi_target_obj[i] == obj) {
            machine_spi_target_obj[i] = NULL;
        }
    }
}

static void machine_spi_target_scb_isr(mp_obj_t obj) {
    machine_spi_target_obj_t *self = MP_OBJ_TO_PTR(obj);
    Cy_SCB_SPI_Interrupt(self->scb_obj->scb, &self->ctx);
}

// Configure SPI target (slave) pins and return SCB unit
static uint8_t machine_spi_target_pins_config_and_get_scb_unit(
    mp_hal_pin_obj_t sck,
    mp_hal_pin_obj_t mosi,
    mp_hal_pin_obj_t miso,
    mp_hal_pin_obj_t ssel) {
    // Pin config: SCK, MOSI, SSEL are inputs; MISO is output
    const mp_hal_pin_af_config_t spi_pins_config[] = {
        MP_HAL_PIN_AF_CONF(sck,
            CY_GPIO_DM_HIGHZ, 0,
            MACHINE_PIN_AF_SIGNAL_SPI_CLK),
        MP_HAL_PIN_AF_CONF(mosi,
            CY_GPIO_DM_HIGHZ, 0,
            MACHINE_PIN_AF_SIGNAL_SPI_MOSI),
        MP_HAL_PIN_AF_CONF(miso,
            CY_GPIO_DM_STRONG_IN_OFF, 1,
            MACHINE_PIN_AF_SIGNAL_SPI_MISO),
        MP_HAL_PIN_AF_CONF(ssel,
            CY_GPIO_DM_HIGHZ, 0,
            MACHINE_PIN_AF_SIGNAL_SPI_SELECT0),
    };
    uint8_t scb_unit = spi_pins_config[0].af->unit;
    mp_hal_periph_pins_af_config(spi_pins_config, 4);
    return scb_unit;
}

static void machine_spi_target_hw_init(machine_spi_target_obj_t *self) {
    // Input validation
    if ((self->polarity > 1U) || (self->phase > 1U)) {
        mp_raise_ValueError(MP_ERROR_TEXT("polarity/phase must be 0 or 1"));
    }
    if (self->bits != 8U) {
        mp_raise_ValueError(MP_ERROR_TEXT("bits must be 8"));
    }
    if ((self->firstbit != MICROPY_PY_MACHINE_SPI_MSB) &&
        (self->firstbit != MICROPY_PY_MACHINE_SPI_LSB)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid firstbit"));
    }

    // Configure SPI target pins and discover SCB unit
    uint8_t scb_unit = machine_spi_target_pins_config_and_get_scb_unit(
        self->sck, self->mosi, self->miso, self->ssel);

    // Power on the peripheral group that contains this SCB.
    machine_scb_enable_group(scb_unit);

    // Allocate SCB
    self->scb_obj = machine_scb_obj_alloc(scb_unit, self,
        machine_spi_target_scb_isr);

    // Configure SPI PCLK divider (needed for slave's internal clock even though
    // the bit clock comes from master's SCK)
    MACHINE_PERI_PCLK_CONFIG_DIVIDER(self->scb_obj->clk,
        SPI_TARGET_CLK_DIV_TYPE, self->div_num, 0U);

    // Select SPI clock polarity and phase
    cy_en_scb_spi_sclk_mode_t sclk_mode;
    if (self->polarity == 0 && self->phase == 0) {
        sclk_mode = CY_SCB_SPI_CPHA0_CPOL0;
    } else if (self->polarity == 0 && self->phase == 1) {
        sclk_mode = CY_SCB_SPI_CPHA1_CPOL0;
    } else if (self->polarity == 1 && self->phase == 0) {
        sclk_mode = CY_SCB_SPI_CPHA0_CPOL1;
    } else {
        sclk_mode = CY_SCB_SPI_CPHA1_CPOL1;
    }

    // Populate PDL SPI configuration for slave/target mode
    self->cfg = (cy_stc_scb_spi_config_t) {
        .spiMode = CY_SCB_SPI_SLAVE,
        .subMode = CY_SCB_SPI_MOTOROLA,
        .sclkMode = sclk_mode,
        .parity = CY_SCB_SPI_PARITY_NONE,
        .dropOnParityError = false,
        .oversample = 0U,
        .rxDataWidth = self->bits,
        .txDataWidth = self->bits,
        .enableMsbFirst = (self->firstbit == MICROPY_PY_MACHINE_SPI_MSB),
        .enableInputFilter = false,
        .enableFreeRunSclk = false,
        .enableMisoLateSample = false,
        .enableTransferSeparation = false,
        .ssPolarity =
            ((CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT0) |
                (CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT1) |
                (CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT2) |
                (CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT3)),
        .ssSetupDelay = false,
        .ssHoldDelay = false,
        .ssInterFrameDelay = false,
        .enableWakeFromSleep = false,
        .rxFifoTriggerLevel = 0UL,
        .rxFifoIntEnableMask = 0UL,
        .txFifoTriggerLevel = 0UL,
        .txFifoIntEnableMask = 0UL,
        .masterSlaveIntEnableMask = 0UL,
    };

    // Initialize SPI hardware
    cy_en_scb_spi_status_t result =
        Cy_SCB_SPI_Init(self->scb_obj->scb, &self->cfg, &self->ctx);

    if (result != CY_SCB_SPI_SUCCESS) {
        machine_scb_obj_free(self->scb_obj);
        self->scb_obj = NULL;
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("SPITarget init failed: 0x%lx"), (uint32_t)result);
    }

    sys_int_init(&self->scb_obj->irq);
    Cy_SCB_SPI_Enable(self->scb_obj->scb);
}

static void machine_spi_target_hw_deinit(machine_spi_target_obj_t *self) {
    Cy_SCB_SPI_Disable(self->scb_obj->scb, &self->ctx);
    Cy_SCB_SPI_DeInit(self->scb_obj->scb);
    sys_int_deinit(&self->scb_obj->irq);
    Cy_SysClk_PeriPclkDisableDivider(self->scb_obj->clk, SPI_TARGET_CLK_DIV_TYPE, self->div_num);
    machine_scb_obj_free(self->scb_obj);
    self->scb_obj = NULL;
}

/******************************************************************************/
// MicroPython bindings

static void machine_spi_target_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_spi_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPITarget(sck='%q', mosi='%q', miso='%q', ssel='%q', polarity=%u, phase=%u, bits=%u)",
        self->sck->name, self->mosi->name, self->miso->name, self->ssel->name,
        self->polarity, self->phase, self->bits);
}

static mp_obj_t machine_spi_target_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 0, 8, true);

    enum { ARG_sck, ARG_mosi, ARG_miso, ARG_ssel, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_ssel,     MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_TARGET_POLARITY} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_TARGET_PHASE} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_TARGET_BITS} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = MICROPY_PY_MACHINE_SPI_MSB} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_spi_target_obj_t *self = machine_spi_target_obj_alloc();
    if (self == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("SPITarget: Maximum number of instances reached"));
    }

    self->sck = mp_hal_get_pin_obj(args[ARG_sck].u_obj);
    self->mosi = mp_hal_get_pin_obj(args[ARG_mosi].u_obj);
    self->miso = mp_hal_get_pin_obj(args[ARG_miso].u_obj);
    self->ssel = mp_hal_get_pin_obj(args[ARG_ssel].u_obj);
    self->polarity = args[ARG_polarity].u_int;
    self->phase = args[ARG_phase].u_int;
    self->bits = args[ARG_bits].u_int;
    self->firstbit = args[ARG_firstbit].u_int;
    self->timeout = DEFAULT_SPI_TARGET_TIMEOUT_MS;
    self->scb_obj = NULL;

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        machine_spi_target_hw_init(self);
        nlr_pop();
    } else {
        machine_spi_target_obj_free(self);
        nlr_jump(nlr.ret_val);
    }

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_spi_target_deinit(mp_obj_t self_in) {
    machine_spi_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->scb_obj != NULL) {
        machine_spi_target_hw_deinit(self);
    }
    machine_spi_target_obj_free(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_spi_target_deinit_obj, machine_spi_target_deinit);

static mp_obj_t machine_spi_target_read(mp_obj_t self_in, mp_obj_t buf_in) {
    machine_spi_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_WRITE);

    uint32_t start = mp_hal_ticks_ms();
    size_t received = 0;
    uint8_t *rx_buf = bufinfo.buf;

    while (received < bufinfo.len) {
        uint32_t num_available = Cy_SCB_SPI_GetNumInRxFifo(self->scb_obj->scb);
        while (num_available > 0 && received < bufinfo.len) {
            rx_buf[received++] = (uint8_t)Cy_SCB_SPI_Read(self->scb_obj->scb);
            num_available--;
        }

        if (spi_target_elapsed_ms(start) > self->timeout) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("SPITarget read timeout"));
        }
        mp_event_handle_nowait();
    }

    return mp_obj_new_int(received);
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_spi_target_read_obj, machine_spi_target_read);

static mp_obj_t machine_spi_target_write(mp_obj_t self_in, mp_obj_t buf_in) {
    machine_spi_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    Cy_SCB_SPI_ClearTxFifo(self->scb_obj->scb);

    uint32_t start = mp_hal_ticks_ms();
    size_t written = 0;
    uint8_t *tx_buf = bufinfo.buf;

    while (written < bufinfo.len) {
        if (Cy_SCB_GetNumInTxFifo(self->scb_obj->scb) < Cy_SCB_GetFifoSize(self->scb_obj->scb)) {
            Cy_SCB_SPI_Write(self->scb_obj->scb, tx_buf[written++]);
        } else {
            if (spi_target_elapsed_ms(start) > self->timeout) {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("SPITarget write timeout"));
            }
            mp_event_handle_nowait();
        }
    }

    start = mp_hal_ticks_ms();
    while (!Cy_SCB_SPI_IsTxComplete(self->scb_obj->scb)) {
        if (spi_target_elapsed_ms(start) > self->timeout) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("SPITarget write timeout"));
        }
        mp_event_handle_nowait();
    }

    return mp_obj_new_int(written);
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_spi_target_write_obj, machine_spi_target_write);

static mp_obj_t machine_spi_target_write_readinto(mp_obj_t self_in, mp_obj_t tx_buf_in, mp_obj_t rx_buf_in) {
    machine_spi_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t tx_bufinfo;
    mp_buffer_info_t rx_bufinfo;
    mp_get_buffer_raise(tx_buf_in, &tx_bufinfo, MP_BUFFER_READ);
    mp_get_buffer_raise(rx_buf_in, &rx_bufinfo, MP_BUFFER_WRITE);

    if (tx_bufinfo.len != rx_bufinfo.len) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffers must be the same length"));
    }

    Cy_SCB_SPI_ClearTxFifo(self->scb_obj->scb);
    Cy_SCB_SPI_ClearRxFifo(self->scb_obj->scb);

    uint8_t *tx_buf = tx_bufinfo.buf;
    uint8_t *rx_buf = rx_bufinfo.buf;
    size_t len = tx_bufinfo.len;
    size_t tx_idx = 0;
    size_t rx_idx = 0;
    uint32_t start = mp_hal_ticks_ms();

    while (rx_idx < len) {
        // Load TX FIFO
        while (tx_idx < len &&
               Cy_SCB_GetNumInTxFifo(self->scb_obj->scb) < Cy_SCB_GetFifoSize(self->scb_obj->scb)) {
            Cy_SCB_SPI_Write(self->scb_obj->scb, tx_buf[tx_idx++]);
        }

        // Read RX FIFO
        while (rx_idx < len && Cy_SCB_SPI_GetNumInRxFifo(self->scb_obj->scb) > 0) {
            rx_buf[rx_idx++] = (uint8_t)Cy_SCB_SPI_Read(self->scb_obj->scb);
        }

        if (spi_target_elapsed_ms(start) > self->timeout) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("SPITarget transfer timeout"));
        }
        mp_event_handle_nowait();
    }

    return mp_obj_new_int(len);
}
static MP_DEFINE_CONST_FUN_OBJ_3(machine_spi_target_write_readinto_obj, machine_spi_target_write_readinto);

void machine_spi_target_deinit_all(void) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_SPI_TARGET_MAX; i++) {
        if (machine_spi_target_obj[i] != NULL) {
            machine_spi_target_deinit(MP_OBJ_FROM_PTR(machine_spi_target_obj[i]));
        }
    }
}

static const mp_rom_map_elem_t machine_spi_target_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read),           MP_ROM_PTR(&machine_spi_target_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write),          MP_ROM_PTR(&machine_spi_target_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&machine_spi_target_write_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),         MP_ROM_PTR(&machine_spi_target_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_MSB),            MP_ROM_INT(MICROPY_PY_MACHINE_SPI_MSB) },
    { MP_ROM_QSTR(MP_QSTR_LSB),            MP_ROM_INT(MICROPY_PY_MACHINE_SPI_LSB) },
};
static MP_DEFINE_CONST_DICT(machine_spi_target_locals_dict, machine_spi_target_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_spi_target_type,
    MP_QSTR_SPITarget,
    MP_TYPE_FLAG_NONE,
    make_new, machine_spi_target_make_new,
    print, machine_spi_target_print,
    locals_dict, &machine_spi_target_locals_dict
    );

#endif // MICROPY_PY_MACHINE_SPI_TARGET

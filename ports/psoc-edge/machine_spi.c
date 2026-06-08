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
#include "extmod/modmachine.h"
#include <string.h>
#include "py/mphal.h"
#include "py/mperrno.h"
#include "cybsp.h"
#include "cy_scb_spi.h"
#include "cy_sysclk.h"
#include "genhdr/pins_af.h"
#include "machine_scb.h"
#include "modmachine.h"

#define DEFAULT_SPI_BAUDRATE    (1000000)
#define DEFAULT_SPI_POLARITY    (0)
#define DEFAULT_SPI_PHASE       (0)
#define SPI_SUPPORTED_BITS      (8U)
#define DEFAULT_SPI_BITS        (SPI_SUPPORTED_BITS)
#define DEFAULT_SPI_TIMEOUT     (50000)

#define SPI_OVERSAMPLE          (4U)
#define SPI_CLK_DIV_TYPE        CY_SYSCLK_DIV_8_BIT
#define SPI_CLK_DIV_BASE        (4U)
#define SPI_CLK_DIV_INVALID      (0xFFU)

typedef struct _machine_spi_obj_t {
    mp_obj_base_t base;
    mp_hal_pin_obj_t sck;
    mp_hal_pin_obj_t mosi;
    mp_hal_pin_obj_t miso;
    uint32_t baudrate;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
    uint8_t firstbit;
    uint32_t timeout;
    uint8_t div_num;
    machine_scb_obj_t *scb_obj;
    cy_stc_scb_spi_config_t cfg;
    cy_stc_scb_spi_context_t ctx;
} machine_spi_obj_t;

machine_spi_obj_t *machine_hw_spi_obj[MICROPY_PY_MACHINE_SPI_NUM_ENTRIES] = { NULL };

static inline uint32_t machine_spi_div8_count_for_clk(en_clk_dst_t clk_dst) {
    uint32_t grp_num = (((uint32_t)clk_dst) & PERI_PCLK_GR_NUM_Msk) >> PERI_PCLK_GR_NUM_Pos;
    uint32_t inst_num = (((uint32_t)clk_dst) & PERI_PCLK_INST_NUM_Msk) >> PERI_PCLK_INST_NUM_Pos;
    return PERI_PCLK_GR_DIV_8_NR(inst_num, grp_num);
}

static inline bool machine_spi_same_divider_group(en_clk_dst_t clk_a, en_clk_dst_t clk_b) {
    uint32_t a_grp = (((uint32_t)clk_a) & PERI_PCLK_GR_NUM_Msk) >> PERI_PCLK_GR_NUM_Pos;
    uint32_t a_inst = (((uint32_t)clk_a) & PERI_PCLK_INST_NUM_Msk) >> PERI_PCLK_INST_NUM_Pos;
    uint32_t b_grp = (((uint32_t)clk_b) & PERI_PCLK_GR_NUM_Msk) >> PERI_PCLK_GR_NUM_Pos;
    uint32_t b_inst = (((uint32_t)clk_b) & PERI_PCLK_INST_NUM_Msk) >> PERI_PCLK_INST_NUM_Pos;
    return (a_grp == b_grp) && (a_inst == b_inst);
}

static bool machine_spi_try_alloc_divider(machine_spi_obj_t *self, en_clk_dst_t clk_dst) {
    uint32_t max_div = machine_spi_div8_count_for_clk(clk_dst);
    if (SPI_CLK_DIV_BASE >= max_div) {
        return false;
    }

    for (uint32_t div = SPI_CLK_DIV_BASE; div < max_div; ++div) {
        if (div > SPI_CLK_DIV_INVALID) {
            break;
        }

        bool used = false;
        for (uint8_t i = 0; i < MICROPY_PY_MACHINE_SPI_NUM_ENTRIES; i++) {
            machine_spi_obj_t *obj = machine_hw_spi_obj[i];
            if ((obj == NULL) || (obj == self) || (obj->scb_obj == NULL)) {
                continue;
            }
            if (!machine_spi_same_divider_group(obj->scb_obj->clk, clk_dst)) {
                continue;
            }
            if (obj->div_num == (uint8_t)div) {
                used = true;
                break;
            }
        }

        if (!used) {
            self->div_num = (uint8_t)div;
            return true;
        }
    }

    return false;
}

static inline machine_spi_obj_t *machine_hw_spi_obj_alloc(void) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_SPI_NUM_ENTRIES; i++) {
        if (machine_hw_spi_obj[i] == NULL) {
            machine_hw_spi_obj[i] = mp_obj_malloc(machine_spi_obj_t, &machine_spi_type);
            machine_hw_spi_obj[i]->div_num = SPI_CLK_DIV_INVALID;
            return machine_hw_spi_obj[i];
        }
    }
    return NULL;
}

static inline void machine_hw_spi_obj_free(machine_spi_obj_t *spi_obj) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_SPI_NUM_ENTRIES; i++) {
        if (machine_hw_spi_obj[i] == spi_obj) {
            machine_hw_spi_obj[i] = NULL;
        }
    }
}

static void machine_spi_scb_isr(mp_obj_t hw_spi_obj) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(hw_spi_obj);
    Cy_SCB_SPI_Interrupt(self->scb_obj->scb, &self->ctx);
}

// Configure SPI master pins and return SCB unit
static uint8_t machine_spi_pins_config_and_get_scb_unit(
    mp_hal_pin_obj_t sck,
    mp_hal_pin_obj_t mosi,
    mp_hal_pin_obj_t miso) {
    // CS/SSEL is managed externally by the user via machine.Pin
    const mp_hal_pin_af_config_t spi_pins_config[] = {
        MP_HAL_PIN_AF_CONF(sck,
            CY_GPIO_DM_STRONG_IN_OFF, 1,
            MACHINE_PIN_AF_SIGNAL_SPI_CLK),
        MP_HAL_PIN_AF_CONF(mosi,
            CY_GPIO_DM_STRONG_IN_OFF, 1,
            MACHINE_PIN_AF_SIGNAL_SPI_MOSI),
        MP_HAL_PIN_AF_CONF(miso,
            CY_GPIO_DM_HIGHZ, 0,
            MACHINE_PIN_AF_SIGNAL_SPI_MISO),
    };
    uint8_t scb_unit = spi_pins_config[0].af->unit;
    mp_hal_periph_pins_af_config(spi_pins_config, MP_ARRAY_SIZE(spi_pins_config));
    return scb_unit;
}

static void machine_spi_validate_config(machine_spi_obj_t *self) {
    if (self->baudrate == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("baudrate must be > 0"));
    }
    if ((self->polarity > 1U) || (self->phase > 1U)) {
        mp_raise_ValueError(MP_ERROR_TEXT("polarity/phase must be 0 or 1"));
    }
    if (self->bits != SPI_SUPPORTED_BITS) {
        mp_raise_ValueError(MP_ERROR_TEXT("bits must be 8"));
    }
    if ((self->firstbit != MICROPY_PY_MACHINE_SPI_MSB) &&
        (self->firstbit != MICROPY_PY_MACHINE_SPI_LSB)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid firstbit"));
    }
    if (self->sck == NULL || self->mosi == NULL || self->miso == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("sck, mosi, and miso pins are required"));
    }
}

static void machine_spi_hw_init(machine_spi_obj_t *self) {
    // Configure SPI pins and discover SCB unit
    uint8_t scb_unit = machine_spi_pins_config_and_get_scb_unit(
        self->sck, self->mosi, self->miso);

    // Power on the peripheral group that contains this SCB.
    // Note: "Slave" in PeriGroupSlaveInit refers to the bus architecture
    // (SCB is a slave on the internal PERI interconnect), not SPI slave mode.
    machine_scb_enable_group(scb_unit);

    // Allocate SCB
    self->scb_obj = machine_scb_obj_alloc(scb_unit, self,
        machine_spi_scb_isr);

    if (!machine_spi_try_alloc_divider(self, self->scb_obj->clk)) {
        machine_scb_obj_free(self->scb_obj);
        self->scb_obj = NULL;
        mp_raise_ValueError(MP_ERROR_TEXT("SPI clock dividers exhausted"));
    }

    // Configure SPI clock divider
    MACHINE_PERI_PCLK_CONFIG_DIVIDER(self->scb_obj->clk,
        SPI_CLK_DIV_TYPE, self->div_num, 0U);

    // Get HF clock frequency and calculate divider value for desired baudrate
    uint32_t clk_hf_freq = Cy_SysClk_PeriPclkGetFrequency(
        self->scb_obj->clk, SPI_CLK_DIV_TYPE, self->div_num);

    // Compute divider value for requested baudrate
    uint32_t div_val = clk_hf_freq / (self->baudrate * SPI_OVERSAMPLE);
    if (div_val > 0U) {
        div_val--;
    }

    // Apply divider
    MACHINE_PERI_PCLK_CONFIG_DIVIDER(self->scb_obj->clk,
        SPI_CLK_DIV_TYPE, self->div_num, div_val);

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

    // Populate PDL SPI configuration
    self->cfg = (cy_stc_scb_spi_config_t) {
        .spiMode = CY_SCB_SPI_MASTER,
        .subMode = CY_SCB_SPI_MOTOROLA,
        .sclkMode = sclk_mode,
        .parity = CY_SCB_SPI_PARITY_NONE,
        .dropOnParityError = false,
        .oversample = SPI_OVERSAMPLE,
        .rxDataWidth = self->bits,
        .txDataWidth = self->bits,
        .enableMsbFirst = (self->firstbit == MICROPY_PY_MACHINE_SPI_MSB),
        .enableInputFilter = false,
        .enableFreeRunSclk = false,
        .enableMisoLateSample = true,
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
            MP_ERROR_TEXT("SPI init failed: 0x%lx"), (uint32_t)result);
    }

    sys_int_init(&self->scb_obj->irq);
    Cy_SCB_SPI_Enable(self->scb_obj->scb);
}

static void machine_spi_hw_deinit(machine_spi_obj_t *self) {
    Cy_SCB_SPI_Disable(self->scb_obj->scb, &self->ctx);
    Cy_SCB_SPI_DeInit(self->scb_obj->scb);
    sys_int_deinit(&self->scb_obj->irq);
    if (self->div_num != SPI_CLK_DIV_INVALID) {
        Cy_SysClk_PeriPclkDisableDivider(self->scb_obj->clk, SPI_CLK_DIV_TYPE, self->div_num);
        self->div_num = SPI_CLK_DIV_INVALID;
    }
    machine_scb_obj_free(self->scb_obj);
    self->scb_obj = NULL;
}

// Parse and validate user arguments shared by constructor and init().
static void machine_spi_init_helper(machine_spi_obj_t *self, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits,
           ARG_firstbit, ARG_sck, ARG_mosi, ARG_miso };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    bool reinit = false;

    if (args[ARG_baudrate].u_obj != mp_const_none) {
        mp_int_t baudrate = mp_obj_get_int(args[ARG_baudrate].u_obj);
        if (baudrate <= 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("baudrate must be > 0"));
        }
        self->baudrate = (uint32_t)baudrate;
        reinit = true;
    }
    if (args[ARG_polarity].u_int != -1) {
        self->polarity = args[ARG_polarity].u_int;
        reinit = true;
    }
    if (args[ARG_phase].u_int != -1) {
        self->phase = args[ARG_phase].u_int;
        reinit = true;
    }
    if (args[ARG_bits].u_int != -1) {
        self->bits = args[ARG_bits].u_int;
        reinit = true;
    }
    if (args[ARG_firstbit].u_int != -1) {
        self->firstbit = args[ARG_firstbit].u_int;
        reinit = true;
    }
    if (args[ARG_sck].u_obj != mp_const_none) {
        self->sck = mp_hal_get_pin_obj(args[ARG_sck].u_obj);
        reinit = true;
    }
    if (args[ARG_mosi].u_obj != mp_const_none) {
        self->mosi = mp_hal_get_pin_obj(args[ARG_mosi].u_obj);
        reinit = true;
    }
    if (args[ARG_miso].u_obj != mp_const_none) {
        self->miso = mp_hal_get_pin_obj(args[ARG_miso].u_obj);
        reinit = true;
    }

    // Reinitialize hardware if any config changed
    if (reinit) {
        machine_spi_validate_config(self);
        if (self->scb_obj != NULL) {
            machine_spi_hw_deinit(self);
        }
        machine_spi_hw_init(self);
    }
}

// Thin protocol adapter for mp_machine_spi_p.init signature.
static void machine_spi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;

    // Convert runtime pos/kw args into the kw-array form expected by init_helper.
    size_t n_kw = kw_args->used;
    size_t all_args_len = n_args + 2 * n_kw;
    mp_obj_t all_args[all_args_len == 0 ? 1 : all_args_len];

    size_t out = 0;
    for (size_t i = 0; i < n_args; i++) {
        all_args[out++] = pos_args[i];
    }
    for (size_t i = 0; i < kw_args->alloc; i++) {
        if (mp_map_slot_is_filled(kw_args, i)) {
            all_args[out++] = kw_args->table[i].key;
            all_args[out++] = kw_args->table[i].value;
        }
    }

    machine_spi_init_helper(self, n_args, n_kw, all_args);
}

static void machine_spi_deinit(mp_obj_base_t *self_in) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->scb_obj != NULL) {
        machine_spi_hw_deinit(self);
    }
    machine_hw_spi_obj_free(self);
}

static void machine_spi_transfer(mp_obj_base_t *self_in,
    size_t len,
    const uint8_t *src,
    uint8_t *dest) {
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;

    if (len == 0) {
        return;
    }

    // Prepare TX buffer: if src is NULL (read-only), send 0xFF fill bytes
    uint8_t tx_fill[len];
    const uint8_t *tx_buf = src;
    if (tx_buf == NULL) {
        memset(tx_fill, 0xFF, len);
        tx_buf = tx_fill;
    }

    // Prepare RX buffer: if dest is NULL (write-only), use a dummy buffer
    uint8_t rx_dummy[len];
    uint8_t *rx_buf = dest;
    if (rx_buf == NULL) {
        rx_buf = rx_dummy;
    }

    // Start full-duplex transfer (interrupt-driven via ISR)
    cy_en_scb_spi_status_t status = Cy_SCB_SPI_Transfer(
        self->scb_obj->scb, (void *)tx_buf, rx_buf, len, &self->ctx);

    if (status != CY_SCB_SPI_SUCCESS) {
        mp_raise_msg_varg(&mp_type_OSError,
            MP_ERROR_TEXT("SPI transfer failed: 0x%lx"), (uint32_t)status);
    }

    // Wait for transfer to complete
    uint32_t start = mp_hal_ticks_us();
    while (Cy_SCB_SPI_GetTransferStatus(self->scb_obj->scb, &self->ctx) & CY_SCB_SPI_TRANSFER_ACTIVE) {
        if (mp_hal_ticks_us() - start > self->timeout) {
            Cy_SCB_SPI_AbortTransfer(self->scb_obj->scb, &self->ctx);
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("SPI transfer timeout"));
        }
        mp_event_handle_nowait();
    }
}

static const mp_machine_spi_p_t machine_spi_p = {
    .init = machine_spi_init,
    .deinit = machine_spi_deinit,
    .transfer = machine_spi_transfer,
};

static void machine_spi_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPI(baudrate=%u, polarity=%u, phase=%u, bits=%u, firstbit=%s, sck='%q', mosi='%q', miso='%q')",
        self->baudrate, self->polarity, self->phase, self->bits,
        self->firstbit == MICROPY_PY_MACHINE_SPI_MSB ? "MSB" : "LSB",
        self->sck->name, self->mosi->name, self->miso->name);
}

// Create and initialize a new machine.SPI object from Python
mp_obj_t mp_machine_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 0, 9, true);

    // Allocate a SPI object from static pool (each slot has a unique clock divider index)
    machine_spi_obj_t *self = machine_hw_spi_obj_alloc();
    if (self == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("machine.SPI: Maximum number of SPI instances reached"));
    }

    // Set defaults
    self->baudrate = DEFAULT_SPI_BAUDRATE;
    self->polarity = DEFAULT_SPI_POLARITY;
    self->phase = DEFAULT_SPI_PHASE;
    self->bits = DEFAULT_SPI_BITS;
    self->firstbit = MICROPY_PY_MACHINE_SPI_MSB;
    self->timeout = DEFAULT_SPI_TIMEOUT;
    self->scb_obj = NULL;
    self->sck = NULL;
    self->mosi = NULL;
    self->miso = NULL;

    // Delegate argument parsing and initialization to init_helper
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        machine_spi_init_helper(self, n_args, n_kw, all_args);
        nlr_pop();
    } else {
        machine_hw_spi_obj_free(self);
        nlr_jump(nlr.ret_val);
    }

    return MP_OBJ_FROM_PTR(self);
}

MP_DEFINE_CONST_OBJ_TYPE(
    machine_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, mp_machine_spi_make_new,
    print, machine_spi_print,
    protocol, &machine_spi_p,
    locals_dict, &mp_machine_spi_locals_dict
    );

void machine_spi_deinit_all(void) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_SPI_NUM_ENTRIES; i++) {
        if (machine_hw_spi_obj[i] != NULL) {
            machine_spi_deinit((mp_obj_base_t *)machine_hw_spi_obj[i]);
        }
    }
}

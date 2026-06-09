/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Infineon Technologies AG
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

#include "mpconfigport.h"

#if MICROPY_PY_MACHINE_I2C || MICROPY_PY_MACHINE_I2C_TARGET || MICROPY_PY_MACHINE_SPI || MICROPY_PY_MACHINE_SPI_TARGET || MICROPY_PY_MACHINE_UART

#include "py/runtime.h"
#include "cy_sysclk.h"
#include "genhdr/pins_af.h"
#include "machine_scb.h"

/* Forward declaration */
static void machine_scb_irq_handler(uint8_t scb);

#define DEFINE_SCB_IRQ_HANDLER(scb) \
    void SCB##scb##_IRQ_Handler(void) { \
        machine_scb_irq_handler(scb); \
    }

/**
 * The file build-<board>/genhdr/pins_af.h contains the macro
 *
 *  MICROPY_PY_MACHINE_FOR_ALL_SCB(DO)
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

MICROPY_PY_MACHINE_FOR_ALL_SCB(DEFINE_SCB_IRQ_HANDLER)

#define MAP_SCB_IRQ_CONFIG(scb) \
    [scb] = { \
        SCB##scb, \
        { \
            scb_##scb##_interrupt_IRQn, \
            SYS_INT_IRQ_LOWEST_PRIORITY, \
            SCB##scb##_IRQ_Handler \
        }, \
        PCLK_SCB##scb##_CLOCK_SCB_EN, \
        NULL, \
        NULL, \
    },

static machine_scb_obj_t machine_scb_obj[MICROPY_PY_MACHINE_SCB_NUM_ENTRIES] = {
    MICROPY_PY_MACHINE_FOR_ALL_SCB(MAP_SCB_IRQ_CONFIG)
};

typedef struct {
    uint32_t peri_nr;
    uint32_t group_nr;
    uint32_t slave_nr;
    uint32_t clk_hf_nr;
} machine_scb_group_info_t;

#define MAP_SCB_GROUP_INFO(scb) \
    [scb] = { \
        .peri_nr = CY_MMIO_SCB##scb##_PERI_NR, \
        .group_nr = CY_MMIO_SCB##scb##_GROUP_NR, \
        .slave_nr = CY_MMIO_SCB##scb##_SLAVE_NR, \
        .clk_hf_nr = CY_MMIO_SCB##scb##_CLK_HF_NR, \
    },

static const machine_scb_group_info_t machine_scb_group_info[MICROPY_PY_MACHINE_SCB_NUM_ENTRIES] = {
    MICROPY_PY_MACHINE_FOR_ALL_SCB(MAP_SCB_GROUP_INFO)
};

typedef struct {
    const void *owner;
    en_clk_dst_t clk_dst;
    uint8_t div_num;
} machine_scb_div8_alloc_t;

static machine_scb_div8_alloc_t machine_scb_div8_alloc[MICROPY_PY_MACHINE_SCB_NUM_ENTRIES] = {0};

static inline uint32_t machine_scb_div8_count_for_clk(en_clk_dst_t clk_dst) {
    uint32_t grp_num = (((uint32_t)clk_dst) & PERI_PCLK_GR_NUM_Msk) >> PERI_PCLK_GR_NUM_Pos;
    uint32_t inst_num = (((uint32_t)clk_dst) & PERI_PCLK_INST_NUM_Msk) >> PERI_PCLK_INST_NUM_Pos;
    return PERI_PCLK_GR_DIV_8_NR(inst_num, grp_num);
}

static inline bool machine_scb_same_divider_group(en_clk_dst_t clk_a, en_clk_dst_t clk_b) {
    uint32_t a_grp = (((uint32_t)clk_a) & PERI_PCLK_GR_NUM_Msk) >> PERI_PCLK_GR_NUM_Pos;
    uint32_t a_inst = (((uint32_t)clk_a) & PERI_PCLK_INST_NUM_Msk) >> PERI_PCLK_INST_NUM_Pos;
    uint32_t b_grp = (((uint32_t)clk_b) & PERI_PCLK_GR_NUM_Msk) >> PERI_PCLK_GR_NUM_Pos;
    uint32_t b_inst = (((uint32_t)clk_b) & PERI_PCLK_INST_NUM_Msk) >> PERI_PCLK_INST_NUM_Pos;
    return (a_grp == b_grp) && (a_inst == b_inst);
}

static void machine_scb_irq_handler(uint8_t scb) {
    machine_scb_obj_t *scb_obj = &machine_scb_obj[scb];
    if (scb_obj->parent != NULL && scb_obj->parent_handler != NULL) {
        scb_obj->parent_handler(scb_obj->parent);
    }
}

machine_scb_obj_t *machine_scb_obj_alloc(uint8_t scb, mp_obj_t parent, machine_scb_parent_irq_handler_t handler) {
    machine_scb_obj_t *obj = &machine_scb_obj[scb];
    if (obj->parent != NULL) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SCB %u is already in use by another I2C, SPI or UART instance."), scb);
    }
    obj->parent = parent;
    obj->parent_handler = handler;
    return &machine_scb_obj[scb];
}

void machine_scb_obj_free(machine_scb_obj_t *scb) {
    scb->parent = NULL;
    scb->parent_handler = NULL;
}

void machine_scb_enable_group(uint8_t scb) {
    const machine_scb_group_info_t *gi = &machine_scb_group_info[scb];
    Cy_SysClk_PeriGroupSlaveInit(gi->peri_nr, gi->group_nr,
        gi->slave_nr, gi->clk_hf_nr);
}

void machine_scb_peri_pclk_config_divider(en_clk_dst_t clk_dst,
    cy_en_divider_types_t div_type, uint8_t div_num, uint32_t div_val) {
    Cy_SysClk_PeriPclkDisableDivider(clk_dst, div_type, div_num);
    Cy_SysClk_PeriPclkAssignDivider(clk_dst, div_type, div_num);
    Cy_SysClk_PeriPclkSetDivider(clk_dst, div_type, div_num, div_val);
    Cy_SysClk_PeriPclkEnableDivider(clk_dst, div_type, div_num);
}

bool machine_scb_div8_try_alloc(en_clk_dst_t clk_dst, uint8_t div_base, uint8_t div_invalid,
    const void *owner, uint8_t *div_num_out) {
    uint32_t max_div = machine_scb_div8_count_for_clk(clk_dst);
    if (div_base >= max_div || div_num_out == NULL || owner == NULL) {
        return false;
    }

    for (uint32_t div = div_base; div < max_div; ++div) {
        if (div > div_invalid) {
            break;
        }

        bool used = false;
        for (uint8_t i = 0; i < MICROPY_PY_MACHINE_SCB_NUM_ENTRIES; i++) {
            if (machine_scb_div8_alloc[i].owner == NULL) {
                continue;
            }
            if (!machine_scb_same_divider_group(machine_scb_div8_alloc[i].clk_dst, clk_dst)) {
                continue;
            }
            if (machine_scb_div8_alloc[i].div_num == (uint8_t)div) {
                used = true;
                break;
            }
        }

        if (!used) {
            for (uint8_t i = 0; i < MICROPY_PY_MACHINE_SCB_NUM_ENTRIES; i++) {
                if (machine_scb_div8_alloc[i].owner == NULL) {
                    machine_scb_div8_alloc[i].owner = owner;
                    machine_scb_div8_alloc[i].clk_dst = clk_dst;
                    machine_scb_div8_alloc[i].div_num = (uint8_t)div;
                    *div_num_out = (uint8_t)div;
                    return true;
                }
            }
            return false;
        }
    }

    return false;
}

void machine_scb_div8_free(en_clk_dst_t clk_dst, uint8_t div_num, const void *owner) {
    if (owner == NULL) {
        return;
    }

    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_SCB_NUM_ENTRIES; i++) {
        if (machine_scb_div8_alloc[i].owner != owner) {
            continue;
        }
        if (machine_scb_div8_alloc[i].div_num != div_num) {
            continue;
        }
        if (!machine_scb_same_divider_group(machine_scb_div8_alloc[i].clk_dst, clk_dst)) {
            continue;
        }

        machine_scb_div8_alloc[i].owner = NULL;
        machine_scb_div8_alloc[i].div_num = 0;
        machine_scb_div8_alloc[i].clk_dst = 0;
        return;
    }
}

#endif // MICROPY_PY_MACHINE_I2C || MICROPY_PY_MACHINE_I2C_TARGET || MICROPY_PY_MACHINE_SPI || MICROPY_PY_MACHINE_SPI_TARGET || MICROPY_PY_MACHINE_UART

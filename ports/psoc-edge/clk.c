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

#include "pse84_config.h"
#include "cy_device.h"
#include "cy_sysclk.h"
#include "py/misc.h"
#include "py/runtime.h"

#include "clk.h"

#define DEBUG_printf(...) // printf(__VA_ARGS__)
#define clk_assert_raise_val(msg, ret)   if (ret != CY_SYSCLK_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

/**
  * Value extracted from TRM.
  */
 #define PERI_PCLK_DIVIDER_MAX_NUM 26

#define CLK_DEST_PERI(clk_dest) ((clk_dest) & PERI_PCLK_PERI_NUM_Msk)
#define CLK_DEST_GROUP(clk_dest) (((clk_dest) & PERI_PCLK_GR_NUM_Msk) >> PERI_PCLK_GR_NUM_Pos)
#define CLK_DEST_INST(clk_dest) (((clk_dest) & PERI_PCLK_INST_NUM_Msk) >> PERI_PCLK_INST_NUM_Pos)

#define PCLK_DIV_NO_FREE_DIVIDER 0xFF

typedef struct _pclk_div_obj_t {
    uint32_t peri_inst;
    uint32_t group;
    cy_en_divider_types_t type;
    uint32_t number;
    uint32_t value;
    uint8_t value_frac;
    uint8_t owner_count;
} pclk_div_obj_t;

static pclk_div_obj_t *pclk_div_obj[PERI_PCLK_DIVIDER_MAX_NUM] = {NULL};

static pclk_div_obj_t *pclk_div_obj_alloc(void) {
    pclk_div_obj_t *clk = NULL;
    for (int i = 0; i < PERI_PCLK_DIVIDER_MAX_NUM; i++) {
        if (pclk_div_obj[i] == NULL) {
            clk = m_new_obj(pclk_div_obj_t);
            pclk_div_obj[i] = clk;
            break;
        }
    }
    return clk;
}

static void pclk_div_obj_free(pclk_div_obj_t *clk) {
    for (int i = 0; i < PERI_PCLK_DIVIDER_MAX_NUM; i++) {
        if (pclk_div_obj[i] == clk) {
            m_del_obj(pclk_div_obj_t, clk);
            pclk_div_obj[i] = NULL;
            break;
        }
    }
}

static pclk_div_obj_t *pclk_div_find(en_clk_dst_t clk_dst, uint32_t divider_value, uint8_t divider_value_frac) {
    pclk_div_obj_t *clk = NULL;
    for (int i = 0; i < PERI_PCLK_DIVIDER_MAX_NUM; i++) {
        if (pclk_div_obj[i] != NULL) {
            if (pclk_div_obj[i]->value == divider_value &&
                pclk_div_obj[i]->value_frac == divider_value_frac &&
                pclk_div_obj[i]->peri_inst == CLK_DEST_INST(clk_dst) &&
                pclk_div_obj[i]->group == CLK_DEST_GROUP(clk_dst)) {
                DEBUG_printf("DEBUG: pclk_div_obj found for peri_inst=%u, group=%u, number=%u, divider=%u, divider_frac=%u\n", CLK_DEST_INST(clk_dst), CLK_DEST_GROUP(clk_dst), pclk_div_obj[i]->number, divider_value, divider_value_frac);
                return pclk_div_obj[i];
            }
        }
    }
    return clk;
}

static void pclk_div_share(en_clk_dst_t clk_dst, pclk_div_obj_t *clk) {
    cy_en_sysclk_status_t ret = Cy_SysClk_PeriphAssignDivider(clk_dst, clk->type, clk->number);
    clk_assert_raise_val("failed to share divider (PDL err: %lu)", ret);
    DEBUG_printf("DEBUG: pclk_div_obj shared for dest = %u\n", clk_dst);
    clk->owner_count++;
}

static pclk_div_obj_t *pclk_div_new_assign_set_enable(en_clk_dst_t clk_dst, cy_en_divider_types_t div_type, uint32_t div_num, uint32_t divider, uint8_t divider_frac) {
    uint32_t peri_inst = CLK_DEST_INST(clk_dst);
    uint32_t group = CLK_DEST_GROUP(clk_dst);

    pclk_div_obj_t *pclk = pclk_div_obj_alloc();
    if (pclk == NULL) {
        return NULL;
    }

    pclk->peri_inst = peri_inst;
    pclk->group = group;
    pclk->type = div_type;
    pclk->number = div_num;
    pclk->value = divider;
    pclk->value_frac = divider_frac;
    pclk->owner_count = 1;

    DEBUG_printf("DEBUG: pclk_div_obj(peri_inst=%u, group=%u, div_type=%u, div_num=%u, divider=%u, divider_frac=%u)\n",
        peri_inst, group, div_type, div_num, divider, divider_frac);

    cy_en_sysclk_status_t ret = Cy_SysClk_PeriphAssignDivider(clk_dst, div_type, div_num);
    clk_assert_raise_val("failed to assign divider (PDL err: %lu)", ret);

    if (div_type == CY_SYSCLK_DIV_16_5_BIT || div_type == CY_SYSCLK_DIV_24_5_BIT) {
        ret = Cy_SysClk_PeriPclkSetFracDivider(clk_dst, div_type, div_num, divider, divider_frac);
        clk_assert_raise_val("failed to set fractional divider (PDL err: %lu)", ret);
    } else {
        ret = Cy_SysClk_PeriPclkSetDivider(clk_dst, div_type, div_num, divider);
        clk_assert_raise_val("failed to set divider (PDL err: %lu)", ret);
    }
    ret = Cy_SysClk_PeriPclkEnableDivider(clk_dst, div_type, div_num);
    clk_assert_raise_val("failed to enable divider (PDL err: %lu)", ret);

    DEBUG_printf("DEBUG: pclk_div_obj assigned, set, enabled on dest=%u.\n", clk_dst);

    return pclk;
}

static cy_en_divider_types_t pclk_div_get_type(uint32_t divider, uint8_t divider_frac) {
    /* If fractional part is needed, use fractional divider types */
    if (divider_frac != 0) {
        if (divider <= 0xFFFFU) {
            return CY_SYSCLK_DIV_16_5_BIT;
        } else {
            return CY_SYSCLK_DIV_24_5_BIT;
        }
    }

    /* No fractional part needed, use integer dividers */
    if (divider <= 0xFFU) {
        return CY_SYSCLK_DIV_8_BIT;
    } else if (divider <= 0xFFFFU) {
        return CY_SYSCLK_DIV_16_BIT;
    } else {
        return CY_SYSCLK_DIV_24_5_BIT;
    }
}

static uint8_t  pclk_div_get_max_num_for_peri_group(en_clk_dst_t clk_dst, cy_en_divider_types_t div_type) {
    uint32_t peri_inst = CLK_DEST_INST(clk_dst);
    uint32_t group_num = CLK_DEST_GROUP(clk_dst);
    uint8_t max_num = 0;

    if (div_type == CY_SYSCLK_DIV_8_BIT) {
        max_num = PERI_PCLK_GR_DIV_8_NR(peri_inst, group_num);
    } else if (div_type == CY_SYSCLK_DIV_16_BIT) {
        max_num = PERI_PCLK_GR_DIV_16_NR(peri_inst, group_num);
    } else if (div_type == CY_SYSCLK_DIV_16_5_BIT) {
        max_num = PERI_PCLK_GR_DIV_16_5_NR(peri_inst, group_num);
    } else if (div_type == CY_SYSCLK_DIV_24_5_BIT) {
        max_num = PERI_PCLK_GR_DIV_24_5_NR(peri_inst, group_num);
    }

    return max_num;
}

static uint8_t pclk_div_get_free_num(en_clk_dst_t clk_dst, cy_en_divider_types_t div_type) {
    uint8_t peri_gr_div_max_num = pclk_div_get_max_num_for_peri_group(clk_dst, div_type);

    for (uint8_t i = 0; i < peri_gr_div_max_num; i++) {
        bool used = false;
        for (uint8_t j = 0; j < PERI_PCLK_DIVIDER_MAX_NUM; j++) {
            if (pclk_div_obj[j] != NULL) {
                if (pclk_div_obj[j]->peri_inst == CLK_DEST_INST(clk_dst) &&
                    pclk_div_obj[j]->group == CLK_DEST_GROUP(clk_dst) &&
                    pclk_div_obj[j]->type == div_type &&
                    pclk_div_obj[j]->number == i) {
                    used = true;
                    break;
                }
            }
        }
        if (!used) {
            return i;
        }
    }

    return PCLK_DIV_NO_FREE_DIVIDER;
}

/******************************************************************************/
/** -- Peripheral Clock Divider API -- **/

void pclk_div_slave_init(en_clk_dst_t clk_dst, pclk_mmio_slave_num_t clk_slave_num) {
    uint32_t peri_inst = CLK_DEST_INST(clk_dst);
    uint32_t group = CLK_DEST_GROUP(clk_dst);
    if (!Cy_SysClk_IsPeriGroupSlaveEnabled(peri_inst, group, clk_slave_num)) {
        Cy_SysClk_PeriGroupSlaveInit(peri_inst, group, clk_slave_num, Cy_Sysclk_PeriPclkGetClkHfNum(clk_dst));
    }
}

void pclk_div_slave_deinit(en_clk_dst_t clk_dst, pclk_mmio_slave_num_t clk_slave_num) {
    uint32_t peri_inst = CLK_DEST_INST(clk_dst);
    uint32_t group = CLK_DEST_GROUP(clk_dst);
    if (Cy_SysClk_IsPeriGroupSlaveEnabled(peri_inst, group, clk_slave_num)) {
        Cy_SysClk_PeriGroupSlaveDeinit(peri_inst, group, clk_slave_num);
    }
}

uint32_t pclk_div_get_input_freq(en_clk_dst_t clk_dst) {
    uint32_t hf_clk_source = Cy_Sysclk_PeriPclkGetClkHfNum(clk_dst);

    if (!Cy_SysClk_ClkHfIsEnabled(hf_clk_source)) {
        /**
         * TODO:
         * Currently all this is initialized in "cycfg_clocks.c".
         * Therefore, we assume that this is initialized and enabled.
         * In the future, maybe the clock management is exposed to
         * the user.
         */
        return 0;
    }

    return Cy_SysClk_ClkHfGetFrequency(hf_clk_source);
}

pclk_div_obj_t *pclk_div_init(en_clk_dst_t clk_dst, uint32_t divider, uint8_t divider_frac) {
    pclk_div_obj_t *pclk = NULL;

    /**
     * Look for an existing peripheral clock divider
     * which could be reused.
     * If found, share it with the caller.
     */
    pclk = pclk_div_find(clk_dst, divider, divider_frac);
    if (pclk != NULL) {
        pclk_div_share(clk_dst, pclk);
        return pclk;
    }

    /* Get the required divider type base on its value + fractional value */
    cy_en_divider_types_t div_type = pclk_div_get_type(divider, divider_frac);

    /* Get the a free peri-group divider number*/
    uint8_t div_num = pclk_div_get_free_num(clk_dst, div_type);
    if (div_num == PCLK_DIV_NO_FREE_DIVIDER) {
        return NULL;
    }

    return pclk_div_new_assign_set_enable(clk_dst, div_type, div_num, divider, divider_frac);
}

void pclk_div_deinit(pclk_div_obj_t *clk) {
    if (clk == NULL) {
        return;
    }

    clk->owner_count--;

    /* Still in use by other peripherals */
    if (clk->owner_count > 0) {
        return;
    }

    uint32_t ip_block = (clk->peri_inst << PERI_PCLK_INST_NUM_Pos) | (clk->group << PERI_PCLK_GR_NUM_Pos);
    cy_en_sysclk_status_t ret = Cy_SysClk_PeriPclkDisableDivider(ip_block, clk->type, clk->number);
    clk_assert_raise_val("failed to disable divider (PDL err: %lu)", ret);
    DEBUG_printf("DEBUG: pclk_div_obj disabled for peri_inst=%u, group=%u, div_type=%u, div_num=%u\n", clk->peri_inst, clk->group, clk->type, clk->number);

    pclk_div_obj_free(clk);
}


/******************************************************************************/
/** -- Static PCLK Divider instance enablement for REPL UART-- **/


static pclk_div_obj_t pclk_div_obj_uart_repl;
void pclk_div_uart_repl_init(void) {
    pclk_div_obj_t *pclk = &pclk_div_obj_uart_repl;
    pclk->group = CLK_DEST_GROUP(PCLK_SCB2_CLOCK_SCB_EN);
    pclk->peri_inst = CLK_DEST_INST(PCLK_SCB2_CLOCK_SCB_EN);
    pclk->type = CY_SYSCLK_DIV_8_BIT;
    pclk->number = 0;
    pclk->value = 72U; // From retarget-io: floor(100000000/(115200*12)) = 72
    pclk->value_frac = 0;
    pclk->owner_count = 1;

    pclk_div_obj[0] = pclk;

    Cy_SysClk_PeriGroupSlaveInit(CY_MMIO_SCB2_PERI_NR, CY_MMIO_SCB2_GROUP_NR, CY_MMIO_SCB2_SLAVE_NR, CY_MMIO_SCB2_CLK_HF_NR);
    Cy_SysClk_PeriPclkAssignDivider(PCLK_SCB2_CLOCK_SCB_EN, CY_SYSCLK_DIV_8_BIT, 0U);

    uint32_t ip_block = (pclk->peri_inst << PERI_PCLK_INST_NUM_Pos) | (pclk->group << PERI_PCLK_GR_NUM_Pos);
    Cy_SysClk_PeriPclkSetDivider(ip_block, CY_SYSCLK_DIV_8_BIT, 0U, 72U);
    Cy_SysClk_PeriPclkEnableDivider(ip_block, CY_SYSCLK_DIV_8_BIT, 0U);
}

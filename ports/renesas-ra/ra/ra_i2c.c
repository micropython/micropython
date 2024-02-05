/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021,2022 Renesas Electronics Corporation
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

#include <stdio.h>
#include <stdint.h>
#include "hal_data.h"
#include "ra_config.h"
#include "ra_gpio.h"
#include "ra_icu.h"
#include "ra_int.h"
#include "ra_timer.h"
#include "ra_utils.h"
#include "ra_i2c.h"

#if !defined(RA_PRI_I2C)
#define RA_PRI_I2C (8)
#endif

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

extern volatile uint32_t uwTick;

static const ra_af_pin_t scl_pins[] = {
    #if defined(RA4M1)

    { AF_I2C, 0, P204 },
    { AF_I2C, 0, P400 },
    { AF_I2C, 0, P408 },
    { AF_I2C, 1, P100 },
    { AF_I2C, 1, P205 },

    #elif defined(RA4W1)

    { AF_I2C, 0, P204 },
    { AF_I2C, 1, P100 },
    { AF_I2C, 1, P205 },

    #elif defined(RA6M1)

    { AF_I2C, 0, P400 },
    { AF_I2C, 0, P408 },
    { AF_I2C, 1, P100 },
    { AF_I2C, 1, P205 },

    #elif defined(RA6M2) || defined(RA6M3)

    { AF_I2C, 0, P204 },
    { AF_I2C, 0, P400 },
    { AF_I2C, 0, P408 },
    { AF_I2C, 1, P100 },
    { AF_I2C, 1, P205 },
    { AF_I2C, 2, P512 },

    #elif defined(RA6M5)

    { AF_I2C, 0, P400 },
    { AF_I2C, 0, P408 },
    { AF_I2C, 1, P205 },
    { AF_I2C, 1, P512 },
    { AF_I2C, 2, P410 },
    { AF_I2C, 2, P415 },

    #else
    #error "CMSIS MCU Series is not specified."
    #endif
};
#define SCL_PINS_SIZE sizeof(scl_pins) / sizeof(ra_af_pin_t)

static const ra_af_pin_t sda_pins[] = {
    #if defined(RA4M1)

    { AF_I2C, 0, P401 },
    { AF_I2C, 0, P407 },
    { AF_I2C, 1, P101 },
    { AF_I2C, 1, P206 },

    #elif defined(RA4W1)

    { AF_I2C, 0, P407 },
    { AF_I2C, 1, P101 },
    { AF_I2C, 1, P206 },

    #elif defined(RA6M1)

    { AF_I2C, 0, P401 },
    { AF_I2C, 0, P407 },
    { AF_I2C, 1, P101 },
    { AF_I2C, 1, P206 },

    #elif defined(RA6M2) || defined(RA6M3)

    { AF_I2C, 0, P401 },
    { AF_I2C, 0, P407 },
    { AF_I2C, 1, P101 },
    { AF_I2C, 1, P206 },
    { AF_I2C, 2, P511 },

    #elif defined(RA6M5)

    { AF_I2C, 0, P401 },
    { AF_I2C, 0, P407 },
    { AF_I2C, 1, P206 },
    { AF_I2C, 1, P511 },
    { AF_I2C, 2, P409 },
    { AF_I2C, 2, P414 },

    #else
    #error "CMSIS MCU Series is not specified."
    #endif
};
#define SDA_PINS_SIZE sizeof(sda_pins) / sizeof(ra_af_pin_t)

static const uint8_t ra_i2c_ch_to_rxirq[] = {
    #if defined(VECTOR_NUMBER_IIC0_RXI)
    VECTOR_NUMBER_IIC0_RXI,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_IIC1_RXI)
    VECTOR_NUMBER_IIC1_RXI,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_IIC2_RXI)
    VECTOR_NUMBER_IIC2_RXI,
    #else
    VECTOR_NUMBER_NONE,
    #endif
};
static const uint8_t ra_i2c_ch_to_txirq[] = {
    #if defined(VECTOR_NUMBER_IIC0_TXI)
    VECTOR_NUMBER_IIC0_TXI,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_IIC1_TXI)
    VECTOR_NUMBER_IIC1_TXI,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_IIC2_TXI)
    VECTOR_NUMBER_IIC2_TXI,
    #else
    VECTOR_NUMBER_NONE,
    #endif
};
static const uint8_t ra_i2c_ch_to_teirq[] = {
    #if defined(VECTOR_NUMBER_IIC0_TEI)
    VECTOR_NUMBER_IIC0_TEI,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_IIC1_TEI)
    VECTOR_NUMBER_IIC1_TEI,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_IIC2_TEI)
    VECTOR_NUMBER_IIC2_TEI,
    #else
    VECTOR_NUMBER_NONE,
    #endif
};
static const uint8_t ra_i2c_ch_to_erirq[] = {
    #if defined(VECTOR_NUMBER_IIC0_ERI)
    VECTOR_NUMBER_IIC0_ERI,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_IIC1_ERI)
    VECTOR_NUMBER_IIC1_ERI,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_IIC2_ERI)
    VECTOR_NUMBER_IIC2_ERI,
    #else
    VECTOR_NUMBER_NONE,
    #endif
};

static xaction_t *current_xaction;
static xaction_unit_t *current_xaction_unit;
static bool last_stop;
static uint8_t pclk_div[8] = {
    1, 2, 4, 8, 16, 32, 64, 128
};

static uint32_t R_IIC0_Type_to_ch(R_IIC0_Type *i2c_inst) {
    if (i2c_inst == R_IIC2) {
        return 2; /* channel 2 */
    } else if (i2c_inst == R_IIC1) {
        return 1; /* channel 1 */
    } else {
        return 0; /* channel 0 */
    }
}

static R_IIC0_Type *ch_to_R_IIC0_Type(uint32_t ch) {
    if (ch == 2) {
        return R_IIC2;
    } else if (ch == 1) {
        return R_IIC1;
    } else {
        return R_IIC0;
    }
}

bool ra_i2c_find_af_ch(uint32_t scl, uint32_t sda, uint8_t *ch) {
    bool find = false;
    uint8_t scl_ch;
    uint8_t sda_ch;
    find = ra_af_find_ch((ra_af_pin_t *)&scl_pins, SCL_PINS_SIZE, scl, &scl_ch);
    if (find) {
        find = ra_af_find_ch((ra_af_pin_t *)&sda_pins, SDA_PINS_SIZE, sda, &sda_ch);
        if (find) {
            find = (scl_ch == sda_ch);
            if (find) {
                *ch = scl_ch;
            } else {
                *ch = 0;
            }
        }
    }
    return find;
}

static void ra_i2c_module_start(R_IIC0_Type *i2c_inst) {
    if (i2c_inst == R_IIC0) {
        ra_mstpcrb_start(R_MSTP_MSTPCRB_MSTPB9_Msk);
    } else if (i2c_inst == R_IIC1) {
        ra_mstpcrb_start(R_MSTP_MSTPCRB_MSTPB8_Msk);
    } else if (i2c_inst == R_IIC2) {
        ra_mstpcrb_start(R_MSTP_MSTPCRB_MSTPB7_Msk);
    }
}

static void ra_i2c_module_stop(R_IIC0_Type *i2c_inst) {
    if (i2c_inst == R_IIC0) {
        ra_mstpcrb_stop(R_MSTP_MSTPCRB_MSTPB9_Msk);
    } else if (i2c_inst == R_IIC1) {
        ra_mstpcrb_stop(R_MSTP_MSTPCRB_MSTPB8_Msk);
    } else if (i2c_inst == R_IIC2) {
        ra_mstpcrb_stop(R_MSTP_MSTPCRB_MSTPB7_Msk);
    }
}

void ra_i2c_irq_enable(R_IIC0_Type *i2c_inst) {
    uint32_t ch = R_IIC0_Type_to_ch(i2c_inst);
    R_BSP_IrqEnable((IRQn_Type const)ra_i2c_ch_to_rxirq[ch]);
    R_BSP_IrqEnable((IRQn_Type const)ra_i2c_ch_to_txirq[ch]);
    R_BSP_IrqEnable((IRQn_Type const)ra_i2c_ch_to_teirq[ch]);
    R_BSP_IrqEnable((IRQn_Type const)ra_i2c_ch_to_erirq[ch]);
}

void ra_i2c_irq_disable(R_IIC0_Type *i2c_inst) {
    uint32_t ch = R_IIC0_Type_to_ch(i2c_inst);
    R_BSP_IrqDisable((IRQn_Type const)ra_i2c_ch_to_rxirq[ch]);
    R_BSP_IrqDisable((IRQn_Type const)ra_i2c_ch_to_txirq[ch]);
    R_BSP_IrqDisable((IRQn_Type const)ra_i2c_ch_to_teirq[ch]);
    R_BSP_IrqDisable((IRQn_Type const)ra_i2c_ch_to_erirq[ch]);
}

void ra_i2c_priority(R_IIC0_Type *i2c_inst, uint32_t ipl) {
    uint32_t ch = R_IIC0_Type_to_ch(i2c_inst);
    R_BSP_IrqCfg((IRQn_Type const)ra_i2c_ch_to_rxirq[ch], ipl, (void *)NULL);
    R_BSP_IrqCfg((IRQn_Type const)ra_i2c_ch_to_txirq[ch], ipl, (void *)NULL);
    R_BSP_IrqCfg((IRQn_Type const)ra_i2c_ch_to_teirq[ch], ipl, (void *)NULL);
    R_BSP_IrqCfg((IRQn_Type const)ra_i2c_ch_to_erirq[ch], ipl, (void *)NULL);
}

void ra_i2c_clear_IR(R_IIC0_Type *i2c_inst) {
    uint32_t ch = R_IIC0_Type_to_ch(i2c_inst);
    R_BSP_IrqStatusClear((IRQn_Type const)ra_i2c_ch_to_rxirq[ch]);
    R_BSP_IrqStatusClear((IRQn_Type const)ra_i2c_ch_to_txirq[ch]);
    R_BSP_IrqStatusClear((IRQn_Type const)ra_i2c_ch_to_teirq[ch]);
    R_BSP_IrqStatusClear((IRQn_Type const)ra_i2c_ch_to_erirq[ch]);
}

// ToDo: need to properly implement
static void ra_i2c_clock_calc(uint32_t baudrate, uint8_t *cks, uint8_t *brh, uint8_t *brl);
static void ra_i2c_clock_calc(uint32_t baudrate, uint8_t *cks, uint8_t *brh, uint8_t *brl) {
    #if defined(RA4M1)
    if (baudrate >= 400000) {
        // assume clock is 400000Hz (PCLKB 32MHz)
        *cks = 1;
        *brh = 9;
        *brl = 20;
    } else {
        // assume clock is 100000Hz (PCLKB 32MHz)
        *cks = 3;
        *brh = 15;
        *brl = 18;
    }
    #elif defined(RA4W1)
    if (baudrate >= 400000) {
        // assume clock is 400000Hz (PCLKB 32MHz)
        *cks = 1;
        *brh = 9;
        *brl = 20;
    } else if (baudrate >= 100000) {
        // assume clock is 100000Hz (PCLKB 32MHz)
        *cks = 3;
        *brh = 15;
        *brl = 18;
    } else {
        // assume clock is 50000Hz (PCLKB 32MHz)
        *cks = 4;
        *brh = 15;
        *brl = 18;
    }
    #elif defined(RA6M1)
    // PCLKB 60MHz SCLE=0
    if (baudrate >= 1000000) {
        *cks = 0;
        *brh = 15;
        *brl = 29;
    } else if (baudrate >= 400000) {
        // assume clock is 400000Hz (PCLKB 32MHz)
        *cks = 2;
        *brh = 8;
        *brl = 19;
    } else {
        // assume clock is 100000Hz (PCLKB 32MHz)
        *cks = 4;
        *brh = 14;
        *brl = 17;
    }
    #elif defined(RA6M2) || defined(RA6M3)
    // PCLKB 60MHz SCLE=0
    if (baudrate >= RA_I2C_CLOCK_MAX) {
        *cks = 0;
        *brh = 15;
        *brl = 29;
    } else if (baudrate >= 400000) {
        // assume clock is 400000Hz
        *cks = 2;
        *brh = 8;
        *brl = 19;
    } else if (baudrate >= 100000) {
        // assume clock is 100000Hz
        *cks = 4;
        *brh = 14;
        *brl = 17;
    } else {
        // assume clock is 50000Hz
        *cks = 5;
        *brh = 14;
        *brl = 17;
    }
    #elif defined(RA6M5)
    // PCLKB 50MHz SCLE=0
    if (baudrate >= RA_I2C_CLOCK_MAX) {
        *cks = 0;
        *brh = 12;
        *brl = 24;
    } else if (baudrate >= 400000) {
        // assume clock is 400000Hz
        *cks = 2;
        *brh = 7;
        *brl = 15;
    } else if (baudrate >= 100000) {
        // assume clock is 100000Hz
        *cks = 3;
        *brh = 24;
        *brl = 30;
    } else {
        // assume clock is 50000Hz
        *cks = 4;
        *brh = 24;
        *brl = 30;
    }
    #else
    #error "CMSIS MCU Series is not specified."
    #endif
}

void ra_i2c_set_baudrate(R_IIC0_Type *i2c_inst, uint32_t baudrate) {
    uint8_t cks;
    uint8_t brh;
    uint8_t brl;
    ra_i2c_clock_calc(baudrate, &cks, &brh, &brl);
    i2c_inst->ICMR1_b.CKS = cks;
    i2c_inst->ICBRH_b.BRH = brh;
    i2c_inst->ICBRL_b.BRL = brl;
}

void ra_i2c_init(R_IIC0_Type *i2c_inst, uint32_t scl, uint32_t sda, uint32_t baudrate) {
    ra_i2c_module_start(i2c_inst);
    ra_gpio_config(scl, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_LOW_POWER, AF_I2C);
    ra_gpio_config(sda, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_LOW_POWER, AF_I2C);
    ra_i2c_priority(i2c_inst, RA_PRI_I2C);
    i2c_inst->ICCR1_b.ICE = 0;     // I2C disable
    i2c_inst->ICCR1_b.IICRST = 1;  // I2C internal reset
    i2c_inst->ICIER = 0x00;        // I2C disable all interrupts
    while (i2c_inst->ICIER != 0) {
        ;
    }
    ra_i2c_clear_IR(i2c_inst);     // clear IR
    i2c_inst->ICCR1_b.ICE = 1;     // I2C enable
    ra_i2c_set_baudrate(i2c_inst, baudrate);
    i2c_inst->ICSER = 0x00;         // I2C reset bus status enable register
    i2c_inst->ICMR3_b.ACKWP = 0x00; // I2C not allow to write ACKBT (transfer acknowledge bit)
    i2c_inst->ICIER = 0xFF;         // Enable all interrupts
    i2c_inst->ICCR1_b.IICRST = 0;   // I2C internal reset
    ra_i2c_irq_enable(i2c_inst);
    last_stop = true;
    return;
}

void ra_i2c_deinit(R_IIC0_Type *i2c_inst) {
    i2c_inst->ICIER = 0;        // I2C interrupt disable
    i2c_inst->ICCR1_b.ICE = 0;  // I2C disable
    ra_i2c_module_stop(i2c_inst);
    return;
}

void ra_i2c_xaction_start(R_IIC0_Type *i2c_inst, xaction_t *action, bool repeated_start) {
    uint32_t timeout;

    if (last_stop == false) {
        i2c_inst->ICSR2_b.START = 0;
        i2c_inst->ICCR2_b.RS = 1;
        return; /* We still keep I2C bus */
    }
    timeout = RA_I2C_TIMEOUT_BUS_BUSY;
    while (i2c_inst->ICCR2_b.BBSY) {
        if (timeout-- == 0) {
            action->m_status = RA_I2C_STATUS_Stopped;
            action->m_error = RA_I2C_ERROR_BUSY;
            return;
        }
    }
    i2c_inst->ICCR2_b.ST = 1;  // I2C start condition
}

void ra_i2c_xaction_stop() {
    last_stop = current_xaction->m_stop;
}

void ra_i2c_xunit_write_byte(R_IIC0_Type *i2c_inst, xaction_unit_t *unit) {
    i2c_inst->ICDRT = unit->buf[unit->m_bytes_transferred];
    ++unit->m_bytes_transferred;
    --unit->m_bytes_transfer;
}

void ra_i2c_xunit_read_byte(R_IIC0_Type *i2c_inst, xaction_unit_t *unit) {
    uint8_t data = i2c_inst->ICDRR;
    unit->buf[unit->m_bytes_transferred] = data;
    ++unit->m_bytes_transferred;
    --unit->m_bytes_transfer;
}

void ra_i2c_xunit_init(xaction_unit_t *unit, uint8_t *buf, uint32_t size, bool fread, void *next) {
    unit->m_bytes_transferred = 0;
    unit->m_bytes_transfer = size;
    unit->m_bytes_total = size;
    unit->m_fread = fread;
    unit->buf = buf;
    unit->next = (void *)next;
}

void ra_i2c_xaction_init(xaction_t *action, xaction_unit_t *units, uint32_t size, uint32_t address, bool stop) {
    action->units = units;
    action->m_num_of_units = size;
    action->m_current = 0;
    action->m_address = (((address << 1) & 0xFE) | (units->m_fread ? 0x1 : 0x0));
    action->m_status = RA_I2C_STATUS_Idle;
    action->m_error = RA_I2C_ERROR_OK;
    action->m_stop = stop;
}

static void ra_i2c_iceri_isr(R_IIC0_Type *i2c_inst) {
    xaction_t *action = current_xaction;
    if (i2c_inst->ICSR2_b.TMOF != 0) {
        action->m_error = RA_I2C_ERROR_TMOF;
        i2c_inst->ICSR2_b.TMOF = 0;
        i2c_inst->ICCR2_b.SP = 1; // request stop condition
    }
    if (i2c_inst->ICSR2_b.AL != 0) {
        action->m_error = RA_I2C_ERROR_AL;
        i2c_inst->ICSR2_b.AL = 0;
        i2c_inst->ICCR2_b.SP = 1; // request stop condition
    }
    // Check Start
    if (i2c_inst->ICSR2_b.START != 0) {
        if (action->m_status == RA_I2C_STATUS_Idle) {
            action->m_status = RA_I2C_STATUS_Started;
        }
        i2c_inst->ICSR2_b.START = 0;
    }
    // Check Stop
    if (i2c_inst->ICSR2_b.STOP != 0) {
        action->m_status = RA_I2C_STATUS_Stopped;
        i2c_inst->ICSR2_b.STOP = 0;
        i2c_inst->ICSR2_b.NACKF = 0; // clear for next transaction
    }
    // Check NACK reception
    if (i2c_inst->ICSR2_b.NACKF != 0) {
        action->m_error = RA_I2C_ERROR_NACK;
        i2c_inst->ICSR2_b.NACKF = 0;
        i2c_inst->ICCR2_b.SP = 1; // request stop condition
    }
}

static void ra_i2c_icrxi_isr(R_IIC0_Type *i2c_inst) {
    xaction_unit_t *unit = current_xaction_unit;
    xaction_t *action = current_xaction;
    // 1 byte or 2 bytes
    if (unit->m_bytes_total <= 2) {
        if (action->m_status == RA_I2C_STATUS_AddrWriteCompleted) {
            action->m_status = RA_I2C_STATUS_FirstReceiveCompleted;
            i2c_inst->ICMR3_b.WAIT = 1;
            // need dummy read processes for 1 byte and 2 bytes receive
            if (unit->m_bytes_total == 2) {
                (void)i2c_inst->ICDRR; // dummy read for 2 bytes receive
            } else {  // m_bytes_total == 1
                i2c_inst->ICMR3_b.ACKWP = 0x01; // enable write ACKBT (transfer acknowledge bit)
                i2c_inst->ICMR3_b.ACKBT = 1;
                i2c_inst->ICMR3_b.ACKWP = 0x00; // disable write ACKBT (transfer acknowledge bit)
                (void)i2c_inst->ICDRR; // dummy read for 1 byte receive
            }
            return;
        }
        if (unit->m_bytes_transfer == 2) {      // last two data
            i2c_inst->ICMR3_b.ACKWP = 0x01; // enable write ACKBT (transfer acknowledge bit)
            i2c_inst->ICMR3_b.ACKBT = 1;
            i2c_inst->ICMR3_b.ACKWP = 0x00; // disable write ACKBT (transfer acknowledge bit)
            ra_i2c_xunit_read_byte(i2c_inst, unit);
        } else { // last data
            action->m_status = RA_I2C_STATUS_LastReceiveCompleted;
            if (action->m_stop == true) {
                i2c_inst->ICCR2_b.SP = 1; // request top condition
            }
            ra_i2c_xunit_read_byte(i2c_inst, unit);
        }
        return;
    }
    // 3 bytes or more
    if (action->m_status == RA_I2C_STATUS_AddrWriteCompleted) {
        (void)i2c_inst->ICDRR; // dummy read
        action->m_status = RA_I2C_STATUS_FirstReceiveCompleted;
        return;
    }
    if (unit->m_bytes_transfer > 2) {
        if (unit->m_bytes_transfer == 3) {
            i2c_inst->ICMR3_b.WAIT = 1;
        }
        ra_i2c_xunit_read_byte(i2c_inst, unit);
    } else if (unit->m_bytes_transfer == 2) {
        i2c_inst->ICMR3_b.ACKWP = 0x01; // enable write ACKBT (transfer acknowledge bit)
        i2c_inst->ICMR3_b.ACKBT = 1;
        i2c_inst->ICMR3_b.ACKWP = 0x00; // disable write ACKBT (transfer acknowledge bit)
        ra_i2c_xunit_read_byte(i2c_inst, unit);
    } else {
        // last data
        action->m_status = RA_I2C_STATUS_LastReceiveCompleted;
        if (action->m_stop == true) {
            i2c_inst->ICCR2_b.SP = 1; // request top condition
        }
        ra_i2c_xunit_read_byte(i2c_inst, unit);
    }
}

static void ra_i2c_ictxi_isr(R_IIC0_Type *i2c_inst) {
    xaction_t *action = current_xaction;
    xaction_unit_t *unit = current_xaction_unit;
    // When STIE is already checked.        When TIE occurs before STIE
    if (action->m_status == RA_I2C_STATUS_Started || action->m_status == RA_I2C_STATUS_Idle) {
        i2c_inst->ICDRT = action->m_address;  // I2C send slave address
        action->m_status = RA_I2C_STATUS_AddrWriteCompleted;
        return;
    }
    if (action->m_status == RA_I2C_STATUS_AddrWriteCompleted &&
        unit->m_fread == false) {
        if (unit->m_bytes_transfer != 0) {
            ra_i2c_xunit_write_byte(i2c_inst, unit);
        } else {
            if (unit->next == (void *)NULL) {
                action->m_status = RA_I2C_STATUS_DataWriteCompleted;
            } else {
                current_xaction_unit = (xaction_unit_t *)unit->next;
                if (current_xaction_unit->m_bytes_transfer == 0) {
                    action->m_status = RA_I2C_STATUS_DataWriteCompleted;
                }
            }
        }
        return;
    }
}

static void ra_i2c_ictei_isr(R_IIC0_Type *i2c_inst) {
    xaction_t *action = current_xaction;
    i2c_inst->ICSR2_b.TEND = 0;
    action->m_current++;
    if (action->m_current == action->m_num_of_units) {
        // We wrote all data and received transfer end, so request stop condition
        if (action->m_stop == true) {
            action->m_status = RA_I2C_STATUS_DataSendCompleted;
            i2c_inst->ICCR2_b.SP = 1;
        } else {
            action->m_status = RA_I2C_STATUS_Stopped; // set Stopped status insted STOP condition
        }
    } else {
        ra_i2c_xaction_start(i2c_inst, action, true);
    }
}

void iic_master_rxi_isr(void) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    uint8_t ch = irq_to_ch[(uint32_t)irq];
    ra_i2c_icrxi_isr(ch_to_R_IIC0_Type(ch));
    R_BSP_IrqStatusClear(irq);
}

void iic_master_txi_isr(void) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    uint8_t ch = irq_to_ch[(uint32_t)irq];
    ra_i2c_ictxi_isr(ch_to_R_IIC0_Type(ch));
    R_BSP_IrqStatusClear(irq);
}

void iic_master_tei_isr(void) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    uint8_t ch = irq_to_ch[(uint32_t)irq];
    ra_i2c_ictei_isr(ch_to_R_IIC0_Type(ch));
    R_BSP_IrqStatusClear(irq);
}

void iic_master_eri_isr(void) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    uint8_t ch = irq_to_ch[(uint32_t)irq];
    ra_i2c_iceri_isr(ch_to_R_IIC0_Type(ch));
    R_BSP_IrqStatusClear(irq);
}

bool ra_i2c_action_execute(R_IIC0_Type *i2c_inst, xaction_t *action, bool repeated_start, uint32_t timeout_ms) {
    bool flag = false;
    uint32_t start = uwTick;

    current_xaction = action;
    current_xaction_unit = action->units;

    ra_i2c_xaction_start(i2c_inst, action, repeated_start);
    while (true) {
        if (action->m_status == RA_I2C_STATUS_Stopped) {
            if (action->m_error == RA_I2C_ERROR_OK) {
                flag = true;
            }
            break;
        }
        if (uwTick - start > timeout_ms) {
            break;
        }
    }
    ra_i2c_xaction_stop();
    if (last_stop == true) {
        mp_hal_delay_ms(3);  // avoid device busy of next access.
    }
    current_xaction = (xaction_t *)NULL;
    current_xaction_unit = (xaction_unit_t *)NULL;

    return flag;
}

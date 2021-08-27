/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"

#include "peripheral_clk_config.h"

#include "common-hal/canio/CAN.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "supervisor/port.h"

#include "component/can.h"

#include "genhdr/candata.h"

STATIC Can *const can_insts[] = CAN_INSTS;

STATIC canio_can_obj_t *can_objs[MP_ARRAY_SIZE(can_insts)];

// This must be placed in the first 64kB of RAM
STATIC COMPILER_SECTION(".canram") canio_can_state_t can_state[MP_ARRAY_SIZE(can_insts)];

void common_hal_canio_can_construct(canio_can_obj_t *self, mcu_pin_obj_t *tx, mcu_pin_obj_t *rx, int baudrate, bool loopback, bool silent) {
    mcu_pin_function_t *tx_function = mcu_find_pin_function(can_tx, tx, -1, MP_QSTR_tx);
    int instance = tx_function->instance;

    mcu_pin_function_t *rx_function = mcu_find_pin_function(can_rx, rx, instance, MP_QSTR_rx);

    const uint32_t can_frequency = CONF_CAN0_FREQUENCY;

#define DIV_ROUND(a, b) (((a) + (b) / 2) / (b))
#define DIV_ROUND_UP(a, b) (((a) + (b) - 1) / (b))

    uint32_t clocks_per_bit = DIV_ROUND(can_frequency, baudrate);
    uint32_t clocks_to_sample = DIV_ROUND(clocks_per_bit * 7, 8);
    uint32_t clocks_after_sample = clocks_per_bit - clocks_to_sample;
    uint32_t divisor = MAX(DIV_ROUND_UP(clocks_to_sample, 256), DIV_ROUND_UP(clocks_after_sample, 128));
    if (divisor > 32) {
        mp_raise_OSError(MP_EINVAL); // baudrate cannot be attained (16kHz or something is lower bound, should never happen)
    }

    gpio_set_pin_direction(tx_function->pin, GPIO_DIRECTION_OUT);
    gpio_set_pin_function(tx_function->pin, tx_function->function);
    common_hal_never_reset_pin(tx_function->obj);

    gpio_set_pin_direction(rx_function->pin, GPIO_DIRECTION_IN);
    gpio_set_pin_function(rx_function->pin, rx_function->function);
    common_hal_never_reset_pin(rx_function->obj);

    self->tx_pin_number = tx ? common_hal_mcu_pin_number(tx) : COMMON_HAL_MCU_NO_PIN;
    self->rx_pin_number = rx ? common_hal_mcu_pin_number(rx) : COMMON_HAL_MCU_NO_PIN;
    self->hw = can_insts[instance];
    self->state = &can_state[instance];

    self->loopback = loopback;
    self->silent = silent;

    // Allow configuration change
    hri_can_set_CCCR_INIT_bit(self->hw);
    while (hri_can_get_CCCR_INIT_bit(self->hw) == 0) {
    }
    hri_can_set_CCCR_CCE_bit(self->hw);

    if (instance == 0) {
        hri_mclk_set_AHBMASK_CAN0_bit(MCLK);
        hri_gclk_write_PCHCTRL_reg(GCLK, CAN0_GCLK_ID, CONF_GCLK_CAN0_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

        NVIC_DisableIRQ(CAN0_IRQn);
        NVIC_ClearPendingIRQ(CAN0_IRQn);
        NVIC_EnableIRQ(CAN0_IRQn);
        hri_can_write_ILE_reg(self->hw, CAN_ILE_EINT0);
    #ifdef CAN1_GCLK_ID
    } else if (instance == 1) {
        hri_mclk_set_AHBMASK_CAN1_bit(MCLK);
        hri_gclk_write_PCHCTRL_reg(GCLK, CAN1_GCLK_ID, CONF_GCLK_CAN1_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

        NVIC_DisableIRQ(CAN1_IRQn);
        NVIC_ClearPendingIRQ(CAN1_IRQn);
        NVIC_EnableIRQ(CAN1_IRQn);
        hri_can_write_ILE_reg(self->hw, CAN_ILE_EINT0);
    #endif
    }

    self->hw->CCCR.bit.FDOE = 0; // neither FD nor Bit Rate Switch enabled
    self->hw->CCCR.bit.BRSE = 0;

    hri_can_write_MRCFG_reg(self->hw, CAN_MRCFG_QOS(CAN_MRCFG_QOS_DISABLE_Val)); // QoS disabled (no sensitive operation)

    // A "nominal bit" is a header bit.  With dual rate CAN FD, this is a slower rate
    {
        CAN_NBTP_Type btp = {
            // 0 means "1 tq", but 2 is subtracted from NTSEG1 for the
            // fixed 1 "SYNC" tq
            .bit.NTSEG1 = DIV_ROUND(clocks_to_sample, divisor) - 2,
            .bit.NTSEG2 = DIV_ROUND(clocks_after_sample, divisor) - 1,
            .bit.NBRP = divisor - 1,
            .bit.NSJW = DIV_ROUND(clocks_after_sample, divisor * 4),
        };
        hri_can_write_NBTP_reg(self->hw, btp.reg);
    }

    // A "data bit" is a data bit :) with dula rate CAN FD, this is a higher
    // rate.  However, CAN FD is not implemented in CircuitPython, and this is
    // the same rate as the "nominal rate".
    {
        CAN_DBTP_Type btp = {
            .bit.DTSEG1 = DIV_ROUND(clocks_to_sample, divisor) - 1,
            .bit.DTSEG2 = DIV_ROUND(clocks_after_sample, divisor) - 1,
            .bit.DBRP = divisor - 1,
            .bit.DSJW = DIV_ROUND(clocks_after_sample, divisor * 4),
        };
        hri_can_write_DBTP_reg(self->hw, btp.reg);
    }

    {
        CAN_RXF0C_Type rxf = {
            .bit.F0SA = (uint32_t)self->state->rx0_fifo,
            .bit.F0S = COMMON_HAL_CANIO_RX_FIFO_SIZE,
        };
        hri_can_write_RXF0C_reg(self->hw, rxf.reg);
    }

    {
        CAN_RXF1C_Type rxf = {
            .bit.F1SA = (uint32_t)self->state->rx1_fifo,
            .bit.F1S = COMMON_HAL_CANIO_RX_FIFO_SIZE,
        };
        hri_can_write_RXF1C_reg(self->hw, rxf.reg);
    }

    // All RX data has an 8 byte payload (max)
    {
        CAN_RXESC_Type esc = {
            .bit.F0DS = CAN_RXESC_F0DS_DATA8_Val,
            .bit.F1DS = CAN_RXESC_F1DS_DATA8_Val,
            .bit.RBDS = CAN_RXESC_RBDS_DATA8_Val,
        };
        hri_can_write_RXESC_reg(self->hw, esc.reg);
    }

    // All TX data has an 8 byte payload (max)
    {
        CAN_TXESC_Type esc = {
            .bit.TBDS = CAN_TXESC_TBDS_DATA8_Val,
        };
        hri_can_write_TXESC_reg(self->hw, esc.reg);
    }

    {
        CAN_TXBC_Type bc = {
            .bit.TBSA = (uint32_t)self->state->tx_buffer,
            .bit.NDTB = COMMON_HAL_CANIO_TX_FIFO_SIZE,
            .bit.TFQM = 0, // Messages are transmitted in the order submitted
        };
        hri_can_write_TXBC_reg(self->hw, bc.reg);
    }

    {
        CAN_TXEFC_Type efc = {
            .bit.EFS = 0,
        };
        hri_can_write_TXEFC_reg(self->hw, efc.reg);
    }

    {
        CAN_GFC_Type gfc = {
            .bit.RRFE = 0,
            .bit.ANFS = CAN_GFC_ANFS_REJECT_Val,
            .bit.ANFE = CAN_GFC_ANFE_REJECT_Val,
        };
        hri_can_write_GFC_reg(self->hw, gfc.reg);
    }

    {
        CAN_SIDFC_Type dfc = {
            .bit.LSS = COMMON_HAL_CANIO_RX_FILTER_SIZE,
            .bit.FLSSA = (uint32_t)self->state->standard_rx_filter
        };
        hri_can_write_SIDFC_reg(self->hw, dfc.reg);
    }

    {
        CAN_XIDFC_Type dfc = {
            .bit.LSE = COMMON_HAL_CANIO_RX_FILTER_SIZE,
            .bit.FLESA = (uint32_t)self->state->extended_rx_filter
        };
        hri_can_write_XIDFC_reg(self->hw, dfc.reg);
    }

    {
        CAN_IE_Type ie = {
            .bit.EWE = 1,
            .bit.EPE = 1,
            .bit.BOE = 1,
        };
        hri_can_write_IE_reg(self->hw, ie.reg);
    }

    hri_can_write_XIDAM_reg(self->hw, CAN_XIDAM_RESETVALUE);

// silent: The CAN is set in Bus Monitoring Mode by programming CCCR.MON to '1'. (tx pin unused)
// external loopback: The CAN can be set in External Loop Back Mode by programming TEST.LBCK and CCCR.MON to '1'. (rx pin unused)
// internal loopback (silent loopback): Internal Loop Back Mode is entered by programming bits TEST.LBCK and CCCR.MON to '1'. (tx, rx unused)
    self->hw->CCCR.bit.MON = silent;
    self->hw->CCCR.bit.TEST = loopback;
    self->hw->TEST.bit.LBCK = loopback;

    if (instance == 0) {
        NVIC_DisableIRQ(CAN0_IRQn);
        NVIC_ClearPendingIRQ(CAN0_IRQn);
        NVIC_EnableIRQ(CAN0_IRQn);
    #ifdef CAN1_GCLK_ID
    } else if (instance == 1) {
        NVIC_DisableIRQ(CAN1_IRQn);
        NVIC_ClearPendingIRQ(CAN1_IRQn);
        NVIC_EnableIRQ(CAN1_IRQn);
    #endif
    }

    hri_can_write_ILE_reg(self->hw, CAN_ILE_EINT0);
    // Prevent configuration change
    hri_can_clear_CCCR_CCE_bit(self->hw);
    hri_can_clear_CCCR_INIT_bit(self->hw);
    while (hri_can_get_CCCR_INIT_bit(self->hw)) {
    }

    can_objs[instance] = self;
}

bool common_hal_canio_can_loopback_get(canio_can_obj_t *self) {
    return self->loopback;
}

int common_hal_canio_can_baudrate_get(canio_can_obj_t *self) {
    return self->baudrate;
}

int common_hal_canio_can_transmit_error_count_get(canio_can_obj_t *self) {
    return self->hw->ECR.bit.TEC;
}

int common_hal_canio_can_receive_error_count_get(canio_can_obj_t *self) {
    return self->hw->ECR.bit.REC;
}

canio_bus_state_t common_hal_canio_can_state_get(canio_can_obj_t *self) {
    CAN_PSR_Type psr = self->hw->PSR;
    if (psr.bit.BO) {
        return BUS_STATE_OFF;
    }
    if (psr.bit.EP) {
        return BUS_STATE_ERROR_PASSIVE;
    }
    if (psr.bit.EW) {
        return BUS_STATE_ERROR_WARNING;
    }
    return BUS_STATE_ERROR_ACTIVE;
}

void common_hal_canio_can_restart(canio_can_obj_t *self) {
    if (!self->hw->PSR.bit.BO) {
        return;
    }

    hri_can_clear_CCCR_INIT_bit(self->hw);
    while (hri_can_get_CCCR_INIT_bit(self->hw)) {
    }
}

bool common_hal_canio_can_auto_restart_get(canio_can_obj_t *self) {
    return self->auto_restart;
}

void common_hal_canio_can_auto_restart_set(canio_can_obj_t *self, bool value) {
    self->auto_restart = value;
}

static void maybe_auto_restart(canio_can_obj_t *self) {
    if (self->auto_restart) {
        common_hal_canio_can_restart(self);
    }
}

void common_hal_canio_can_send(canio_can_obj_t *self, mp_obj_t message_in) {
    maybe_auto_restart(self);

    canio_message_obj_t *message = message_in;
    ;
    // We have just one dedicated TX buffer, use it!
    canio_can_tx_buffer_t *ent = &self->state->tx_buffer[0];

    bool rtr = message->base.type == &canio_remote_transmission_request_type;
    ent->txb0.bit.ESI = false;
    ent->txb0.bit.XTD = message->extended;
    ent->txb0.bit.RTR = rtr;
    if (message->extended) {
        ent->txb0.bit.ID = message->id;
    } else {
        ent->txb0.bit.ID = message->id << 18; // short addresses are left-justified
    }

    ent->txb1.bit.MM = 0; // "message marker"
    ent->txb1.bit.EFC = 0; // don't store fifo events to event queue
    ent->txb1.bit.FDF = 0; // Classic CAN format
    ent->txb1.bit.BRS = 0; // No bit rate switching
    ent->txb1.bit.DLC = message->size;

    if (!rtr) {
        memcpy(ent->data, message->data, message->size);
    }

    // TX buffer add request
    self->hw->TXBAR.reg = 1;

    // wait 8ms (hard coded for now) for TX to occur
    uint64_t deadline = port_get_raw_ticks(NULL) + 8;
    while (port_get_raw_ticks(NULL) < deadline && !(self->hw->TXBTO.reg & 1)) {
        RUN_BACKGROUND_TASKS;
    }
}

bool common_hal_canio_can_silent_get(canio_can_obj_t *self) {
    return self->silent;
}

bool common_hal_canio_can_deinited(canio_can_obj_t *self) {
    return !self->hw;
}

void common_hal_canio_can_check_for_deinit(canio_can_obj_t *self) {
    if (common_hal_canio_can_deinited(self)) {
        raise_deinited_error();
    }
}

void common_hal_canio_can_deinit(canio_can_obj_t *self) {
    if (self->hw) {
        hri_can_set_CCCR_INIT_bit(self->hw);
        self->hw = 0;
    }
    if (self->rx_pin_number != COMMON_HAL_MCU_NO_PIN) {
        reset_pin_number(self->rx_pin_number);
        self->rx_pin_number = COMMON_HAL_MCU_NO_PIN;
    }
    if (self->tx_pin_number != COMMON_HAL_MCU_NO_PIN) {
        reset_pin_number(self->tx_pin_number);
        self->tx_pin_number = COMMON_HAL_MCU_NO_PIN;
    }
}

void common_hal_canio_reset(void) {
    memset(can_state, 0, sizeof(can_state));

    for (size_t i = 0; i < MP_ARRAY_SIZE(can_insts); i++) {
        hri_can_set_CCCR_INIT_bit(can_insts[i]);
    }

    for (size_t i = 0; i < MP_ARRAY_SIZE(can_objs); i++) {
        if (can_objs[i]) {
            common_hal_canio_can_deinit(can_objs[i]);
            can_objs[i] = NULL;
        }
    }
}


STATIC void can_handler(int i) {
    canio_can_obj_t *self = can_objs[i];
    (void)self;

    Can *hw = can_insts[i];
    uint32_t ir = hri_can_read_IR_reg(hw);

    /* Acknowledge interrupt */
    hri_can_write_IR_reg(hw, ir);
}

__attribute__((used))
void CAN0_Handler(void) {
    can_handler(0);
}

#ifdef CAN1_GCLK_ID
__attribute__((used))
void CAN1_Handler(void) {
    can_handler(1);
}
#endif

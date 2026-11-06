// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/mii/RMII.h"
#include "shared-bindings/mii/MDIO.h"
#include "shared-bindings/mii/RMII.h"

#include "py/runtime.h"


#include "soc/soc_caps.h"

void common_hal_mii_rmii_construct(mii_rmii_obj_t *self,
    mii_mdio_obj_t *mdio, uint32_t device,
    const mcu_pin_obj_t *rxd0, const mcu_pin_obj_t *rxd1, const mcu_pin_obj_t *rx_dv,
    const mcu_pin_obj_t *txd0, const mcu_pin_obj_t *txd1, const mcu_pin_obj_t *tx_en,
    const mcu_pin_obj_t *refclk, const mcu_pin_obj_t *reset) {
    // Board code constructs board-owned globals that don't go through
    // make_new, so set the type here.
    self->base.type = &mii_rmii_type;
    #if !((defined(SOC_EMAC_USE_MULTI_IO_MUX) && SOC_EMAC_USE_MULTI_IO_MUX) || (defined(SOC_EMAC_MII_USE_GPIO_MATRIX) && SOC_EMAC_MII_USE_GPIO_MATRIX))
    // The EMAC's RMII signals can't be routed to arbitrary GPIOs.
    mp_raise_NotImplementedError(NULL);
    #else
    self->mdio = mdio;
    self->device = device;
    self->rxd0 = rxd0->number;
    self->rxd1 = rxd1->number;
    self->rx_dv = rx_dv->number;
    self->txd0 = txd0->number;
    self->txd1 = txd1->number;
    self->tx_en = tx_en->number;
    self->refclk = refclk->number;
    self->reset = reset == NULL ? -1 : reset->number;

    const mcu_pin_obj_t *pins[] = { rxd0, rxd1, rx_dv, txd0, txd1, tx_en, refclk };
    for (size_t i = 0; i < MP_ARRAY_SIZE(pins); i++) {
        claim_pin(pins[i]);
    }
    if (reset != NULL) {
        claim_pin(reset);
    }
    #endif
}

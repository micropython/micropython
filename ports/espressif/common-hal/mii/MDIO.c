// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/mii/MDIO.h"
#include "shared-bindings/mii/MDIO.h"

#include "py/runtime.h"


#include "soc/soc_caps.h"

void common_hal_mii_mdio_construct(mii_mdio_obj_t *self,
    const mcu_pin_obj_t *mdc, const mcu_pin_obj_t *mdio) {
    // Board code constructs board-owned globals that don't go through
    // make_new, so set the type here.
    self->base.type = &mii_mdio_type;
    #if !((defined(SOC_EMAC_USE_MULTI_IO_MUX) && SOC_EMAC_USE_MULTI_IO_MUX) || (defined(SOC_EMAC_MII_USE_GPIO_MATRIX) && SOC_EMAC_MII_USE_GPIO_MATRIX))
    // The EMAC's management signals can't be routed to arbitrary GPIOs.
    mp_raise_NotImplementedError(NULL);
    #else
    self->mdc = mdc->number;
    self->mdio = mdio->number;

    claim_pin(mdc);
    claim_pin(mdio);
    #endif
}

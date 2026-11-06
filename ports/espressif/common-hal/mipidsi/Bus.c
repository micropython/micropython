// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/mipidsi/Bus.h"
#include "bindings/espidf/__init__.h"
#include "py/runtime.h"
#include <math.h>
#include <esp_ldo_regulator.h>
#include "hal/mipi_dsi_phy_ll.h"

// The PHY PLL and the reported pixel clock are quantized, so a lane rate a few
// hertz below the requested minimum is still the intended rate. Without this,
// a 4 Hz shortfall can push the link to the next PHY step, up to 10 Mbps
// higher, which makes DSI video burst instead of staying continuous.
#define MIPIDSI_FREQUENCY_TOLERANCE_HZ 1000

// Shared MIPI PHY LDO rail. DSI buses and CSI receivers both need it; it is
// acquired once and released only when the last user goes away.
static esp_ldo_channel_handle_t mipidsi_phy_ldo = NULL;
static uint8_t mipidsi_phy_ldo_users = 0;

esp_ldo_channel_handle_t mipidsi_phy_ldo_acquire(void) {
    if (mipidsi_phy_ldo_users == 0) {
        esp_ldo_channel_handle_t ldo = NULL;
        esp_ldo_channel_config_t ldo_config = {
            .chan_id = 3,
            .voltage_mv = 2500,
            // Fixed-voltage (non-adjustable) acquire so both users share the
            // rail. An adjustable acquire would block the other user.
            .flags.adjustable = false,
        };
        ESP_ERROR_CHECK(esp_ldo_acquire_channel(&ldo_config, &ldo));
        mipidsi_phy_ldo = ldo;
    }
    mipidsi_phy_ldo_users++;
    return mipidsi_phy_ldo;
}

void mipidsi_phy_ldo_release(void) {
    if (mipidsi_phy_ldo_users == 0) {
        return;
    }
    mipidsi_phy_ldo_users--;
    if (mipidsi_phy_ldo_users == 0 && mipidsi_phy_ldo != NULL) {
        esp_ldo_release_channel(mipidsi_phy_ldo);
        mipidsi_phy_ldo = NULL;
    }
}

void common_hal_mipidsi_bus_construct(mipidsi_bus_obj_t *self, mp_uint_t minimum_frequency, uint8_t num_lanes) {
    if (self->use_count > 0) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_mipidsi);
    }

    self->frequency = 0;
    self->num_data_lanes = num_lanes;
    self->bus_handle = NULL;

    if (minimum_frequency > MIPI_DSI_LL_MAX_PHY_MBPS * 1000000U) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q is above %u"), MP_QSTR_minimum_frequency,
            (unsigned)(MIPI_DSI_LL_MAX_PHY_MBPS * 1000000U));
    }

    self->phy_ldo = mipidsi_phy_ldo_acquire();

    // The PHY PLL snaps to the nearest rate it can generate, which can be
    // below the request. Ask for the minimum in whole Mbps, then step up
    // until the rate read back is at least the minimum.
    float requested_mbps = ceilf((float)minimum_frequency / 1000000.0f);
    if (requested_mbps < MIPI_DSI_LL_MIN_PHY_MBPS) {
        requested_mbps = MIPI_DSI_LL_MIN_PHY_MBPS;
    }
    for (uint8_t attempt = 0; attempt < 8; attempt++) {
        esp_lcd_dsi_bus_config_t bus_config = {
            .bus_id = 0,
            .num_data_lanes = num_lanes,
            .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
            .lane_bit_rate_mbps = requested_mbps,
        };

        esp_err_t ret = esp_lcd_new_dsi_bus(&bus_config, &self->bus_handle);
        if (ret != ESP_OK) {
            self->bus_handle = NULL;
            mipidsi_phy_ldo_release();
            self->phy_ldo = NULL;
            self->num_data_lanes = 0;
            CHECK_ESP_RESULT(ret);
        }

        // The PHY PLL only generates certain rates, so record the real one.
        float lane_bit_rate_mbps = requested_mbps;
        if (esp_lcd_dsi_bus_get_lane_bit_rate(self->bus_handle, &lane_bit_rate_mbps) == ESP_OK) {
            self->frequency = (mp_uint_t)lroundf(lane_bit_rate_mbps * 1000000.0f);
        } else {
            self->frequency = (mp_uint_t)lroundf(requested_mbps * 1000000.0f);
        }

        if (self->frequency + MIPIDSI_FREQUENCY_TOLERANCE_HZ >= minimum_frequency) {
            return;
        }

        // The PLL rounded down. The finest step between achievable rates is
        // 10 Mbps, so asking for the next step above what it produced always
        // targets a higher rate. Try again with that request.
        esp_lcd_del_dsi_bus(self->bus_handle);
        self->bus_handle = NULL;
        self->frequency = 0;
        requested_mbps = lane_bit_rate_mbps + 10.0f;
        if (requested_mbps > MIPI_DSI_LL_MAX_PHY_MBPS) {
            break;
        }
    }

    mipidsi_phy_ldo_release();
    self->phy_ldo = NULL;
    self->num_data_lanes = 0;
    mp_raise_ValueError_varg(MP_ERROR_TEXT("%q is not reachable by the PHY"), MP_QSTR_minimum_frequency);
}

void common_hal_mipidsi_bus_deinit(mipidsi_bus_obj_t *self) {
    if (self->use_count > 0) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_Bus);
    }
    if (common_hal_mipidsi_bus_deinited(self)) {
        return;
    }

    // Delete the DSI bus
    if (self->bus_handle != NULL) {
        esp_lcd_del_dsi_bus(self->bus_handle);
        self->bus_handle = NULL;
    }

    // Release our hold on the shared MIPI PHY LDO; it stays powered as long
    // as a CSI receiver (or other user) still holds it.
    mipidsi_phy_ldo_release();
    self->phy_ldo = NULL;

    self->frequency = 0;
    self->num_data_lanes = 0;
}

bool common_hal_mipidsi_bus_deinited(mipidsi_bus_obj_t *self) {
    return self->bus_handle == NULL;
}

mp_uint_t common_hal_mipidsi_bus_get_frequency(mipidsi_bus_obj_t *self) {
    return self->frequency;
}

uint8_t common_hal_mipidsi_bus_get_num_lanes(mipidsi_bus_obj_t *self) {
    return self->num_data_lanes;
}

void mipidsi_bus_increment_use_count(mipidsi_bus_obj_t *self) {
    self->use_count++;
}
void mipidsi_bus_decrement_use_count(mipidsi_bus_obj_t *self) {
    if (self->use_count == 0) {
        return;
    }
    self->use_count--;
    if (self->use_count == 0) {
        common_hal_mipidsi_bus_deinit(self);
    }
}

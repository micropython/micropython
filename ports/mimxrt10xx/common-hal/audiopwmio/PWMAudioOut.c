// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <string.h>

#include "mpconfigport.h"

#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "common-hal/audiobusio/__init__.h"
#include "common-hal/audiopwmio/PWMAudioOut.h"
#include "shared-bindings/audiopwmio/PWMAudioOut.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/shared/tick.h"

// Where required we use identifier names that are required by NXP's
// API, even though they do not conform to the naming standards that Adafruit
// strives to adhere to. https://www.adafruit.com/blacklivesmatter
#include "sdk/drivers/sai/fsl_sai.h"

static void config_periph_pin(const mcu_periph_obj_t *periph) {
    if (!periph) {
        return;
    }
    if (periph->pin->mux_reg) {
        IOMUXC_SetPinMux(
            periph->pin->mux_reg, periph->mux_mode,
            periph->input_reg, periph->input_idx,
            0,
            1);
    }

    IOMUXC_SetPinConfig(0, 0, 0, 0,
        periph->pin->cfg_reg,
        IOMUXC_SW_PAD_CTL_PAD_HYS(0)
        | IOMUXC_SW_PAD_CTL_PAD_PUS(0)
        | IOMUXC_SW_PAD_CTL_PAD_PUE(0)
        | IOMUXC_SW_PAD_CTL_PAD_PKE(1)
        | IOMUXC_SW_PAD_CTL_PAD_ODE(0)
        | IOMUXC_SW_PAD_CTL_PAD_SPEED(2)
        | IOMUXC_SW_PAD_CTL_PAD_DSE(4)
        | IOMUXC_SW_PAD_CTL_PAD_SRE(0));
}

static void config_mqs(void) {
    CCM->CCGR0 = (CCM->CCGR0 & (~CCM_CCGR0_CG2_MASK)) | CCM_CCGR0_CG2(3);       /* Enable MQS hmclk. */

    IOMUXC_MQSEnterSoftwareReset(IOMUXC_GPR, true);                             /* Reset MQS. */
    IOMUXC_MQSEnterSoftwareReset(IOMUXC_GPR, false);                            /* Release reset MQS. */
    IOMUXC_MQSEnable(IOMUXC_GPR, true);                                         /* Enable MQS. */
    IOMUXC_MQSConfig(IOMUXC_GPR, kIOMUXC_MqsPwmOverSampleRate64, 0u);           /* 98.304MHz/64/(0+1) = 1.536MHz
                                                                                Higher frequency PWM involves less low frequen cy harmonic.*/

}

// Caller validates that pins are free.
void common_hal_audiopwmio_pwmaudioout_construct(audiopwmio_pwmaudioout_obj_t *self,
    const mcu_pin_obj_t *left_channel, const mcu_pin_obj_t *right_channel, uint16_t default_value) {

    int instance = -1;
    const mcu_periph_obj_t *left_periph = find_pin_function(mcu_mqs_left_list, left_channel, &instance, MP_QSTR_left_channel);
    const mcu_periph_obj_t *right_periph = find_pin_function(mcu_mqs_right_list, right_channel, &instance, MP_QSTR_right_channel);

    sai_transceiver_t config;
    SAI_GetClassicI2SConfig(&config, kSAI_WordWidth16bits, kSAI_Stereo, 1U << 0u);
    config.frameSync.frameSyncEarly = false;
    config.frameSync.frameSyncPolarity = kSAI_PolarityActiveHigh;
    // config.syncMode = kSAI_ModeAsync;
    config.fifo.fifoPacking = kSAI_FifoPackingDisabled;
    // These identifier names are required by NXP's API, even though they do
    // not conform to the naming standards that Adafruit strives to adhere to.
    // https://www.adafruit.com/blacklivesmatter
    // config.masterSlave = kSAI_Master;
    port_i2s_initialize(&self->i2s, instance, &config);

    self->left_channel = left_channel;
    self->right_channel = right_channel;
    claim_pin(left_channel);
    claim_pin(right_channel);
    config_periph_pin(left_periph);
    config_periph_pin(right_periph);
    config_mqs();
}

bool common_hal_audiopwmio_pwmaudioout_deinited(audiopwmio_pwmaudioout_obj_t *self) {
    return port_i2s_deinited(&self->i2s);
}

void common_hal_audiopwmio_pwmaudioout_deinit(audiopwmio_pwmaudioout_obj_t *self) {
    if (common_hal_audiopwmio_pwmaudioout_deinited(self)) {
        return;
    }

    port_i2s_deinit(&self->i2s);

    common_hal_reset_pin(self->left_channel);
    self->left_channel = NULL;

    common_hal_reset_pin(self->right_channel);
    self->right_channel = NULL;

    IOMUXC_MQSEnterSoftwareReset(IOMUXC_GPR, true);                             /* Reset MQS. */
    CCM->CCGR0 = CCM->CCGR0 & (~CCM_CCGR0_CG2_MASK);                            /* Disable MQS hmclk. */
}

void common_hal_audiopwmio_pwmaudioout_play(audiopwmio_pwmaudioout_obj_t *self,
    mp_obj_t sample, bool loop) {
    if (common_hal_audiopwmio_pwmaudioout_get_playing(self)) {
        common_hal_audiopwmio_pwmaudioout_stop(self);
    }
    port_i2s_play(&self->i2s, sample, loop);
}

void common_hal_audiopwmio_pwmaudioout_pause(audiopwmio_pwmaudioout_obj_t *self) {
    port_i2s_pause(&self->i2s);
}

void common_hal_audiopwmio_pwmaudioout_resume(audiopwmio_pwmaudioout_obj_t *self) {
    port_i2s_resume(&self->i2s);
}

bool common_hal_audiopwmio_pwmaudioout_get_paused(audiopwmio_pwmaudioout_obj_t *self) {
    return port_i2s_get_paused(&self->i2s);
}

void common_hal_audiopwmio_pwmaudioout_stop(audiopwmio_pwmaudioout_obj_t *self) {
    port_i2s_stop(&self->i2s);
}

bool common_hal_audiopwmio_pwmaudioout_get_playing(audiopwmio_pwmaudioout_obj_t *self) {
    return port_i2s_get_playing(&self->i2s);
}

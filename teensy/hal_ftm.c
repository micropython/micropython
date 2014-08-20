/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include <stdint.h>
#include <mk20dx128.h>
#include "teensy_hal.h"

void HAL_FTM_Base_Init(FTM_HandleTypeDef *hftm) {
    /* Check the parameters */
    FTM_TypeDef *FTMx = hftm->Instance;
    assert_param(IS_FTM_INSTANCE(FTMx));
    assert_param(IS_FTM_PRESCALERSHIFT(hftm->Init.PrescalerShift));
    assert_param(IS_FTM_COUNTERMODE(hftm->Init.CounterMode));
    assert_param(IS_FTM_PERIOD(hftm->Init.Period));

    hftm->State = HAL_FTM_STATE_BUSY;

    FTMx->MODE = FTM_MODE_WPDIS;
    FTMx->SC = 0;
    FTMx->MOD = hftm->Init.Period;
    uint32_t sc = FTM_SC_PS(hftm->Init.PrescalerShift);
    if (hftm->Init.CounterMode == FTM_COUNTERMODE_CENTER) {
      sc |= FTM_SC_CPWMS;
    }
    FTMx->SC = sc;

    hftm->State = HAL_FTM_STATE_READY;
}

void HAL_FTM_Base_Start(FTM_HandleTypeDef *hftm) {
    FTM_TypeDef *FTMx = hftm->Instance;
    assert_param(IS_FTM_INSTANCE(FTMx));

    hftm->State = HAL_FTM_STATE_BUSY;

    FTMx->CNT = 0;
    FTMx->SC &= ~FTM_SC_CLKS(3);
    FTMx->SC |= FTM_SC_CLKS(1);

    hftm->State = HAL_FTM_STATE_READY;
}

void HAL_FTM_Base_Start_IT(FTM_HandleTypeDef *hftm) {
    FTM_TypeDef *FTMx = hftm->Instance;
    assert_param(IS_FTM_INSTANCE(FTMx));

    hftm->State = HAL_FTM_STATE_BUSY;

    FTMx->CNT = 0;
    FTMx->SC |= FTM_SC_CLKS(1) | FTM_SC_TOIE;

    hftm->State = HAL_FTM_STATE_READY;
}

void HAL_FTM_Base_DeInit(FTM_HandleTypeDef *hftm) {
    assert_param(IS_FTM_INSTANCE(hftm->Instance));

    hftm->State = HAL_FTM_STATE_BUSY;

    __HAL_FTM_DISABLE_TOF_IT(hftm);

    hftm->State = HAL_FTM_STATE_RESET;
}

void HAL_FTM_OC_Init(FTM_HandleTypeDef *hftm) {
    HAL_FTM_Base_Init(hftm);
}

void HAL_FTM_OC_ConfigChannel(FTM_HandleTypeDef *hftm, FTM_OC_InitTypeDef* sConfig, uint32_t channel) {
    FTM_TypeDef *FTMx = hftm->Instance;
    assert_param(IS_FTM_INSTANCE(FTMx));
    assert_param(IS_FTM_CHANNEL(channel));
    assert_param(IS_FTM_OC_MODE(sConfig->OCMode));
    assert_param(IS_FTM_OC_PULSE(sConfig->Pulse));
    assert_param(IS_FTM_OC_POLARITY(sConfig->OCPolarity));

    hftm->State = HAL_FTM_STATE_BUSY;

    FTMx->channel[channel].CSC = sConfig->OCMode;
    FTMx->channel[channel].CV  = sConfig->Pulse;
    if (sConfig->OCPolarity & 1) {
      FTMx->POL |= (1 << channel); 
    } else {
      FTMx->POL &= ~(1 << channel); 
    }

    hftm->State = HAL_FTM_STATE_READY;
}

void HAL_FTM_OC_Start(FTM_HandleTypeDef *hftm, uint32_t channel) {
    // Nothing else to do
}

void HAL_FTM_OC_Start_IT(FTM_HandleTypeDef *hftm, uint32_t channel) {
    FTM_TypeDef *FTMx = hftm->Instance;
    assert_param(IS_FTM_INSTANCE(FTMx));

    FTMx->channel[channel].CSC |= FTM_CSC_CHIE;
}

void HAL_FTM_OC_DeInit(FTM_HandleTypeDef *hftm) {
    HAL_FTM_Base_DeInit(hftm);
}

void HAL_FTM_PWM_Init(FTM_HandleTypeDef *hftm) {
    HAL_FTM_Base_Init(hftm);
}

void HAL_FTM_PWM_ConfigChannel(FTM_HandleTypeDef *hftm, FTM_OC_InitTypeDef* sConfig, uint32_t channel) {
    FTM_TypeDef *FTMx = hftm->Instance;
    assert_param(IS_FTM_INSTANCE(FTMx));
    assert_param(IS_FTM_CHANNEL(channel));
    assert_param(IS_FTM_PWM_MODE(sConfig->OCMode));
    assert_param(IS_FTM_OC_PULSE(sConfig->Pulse));
    assert_param(IS_FTM_OC_POLARITY(sConfig->OCPolarity));

    hftm->State = HAL_FTM_STATE_BUSY;

    FTMx->channel[channel].CSC = sConfig->OCMode;
    FTMx->channel[channel].CV  = sConfig->Pulse;
    if (sConfig->OCPolarity & 1) {
      FTMx->POL |= (1 << channel); 
    } else {
      FTMx->POL &= ~(1 << channel); 
    }

    hftm->State = HAL_FTM_STATE_READY;
}

void HAL_FTM_PWM_Start(FTM_HandleTypeDef *hftm, uint32_t channel) {
    // Nothing else to do
}

void HAL_FTM_PWM_Start_IT(FTM_HandleTypeDef *hftm, uint32_t channel) {
    FTM_TypeDef *FTMx = hftm->Instance;
    assert_param(IS_FTM_INSTANCE(FTMx));

    FTMx->channel[channel].CSC |= FTM_CSC_CHIE;
}

void HAL_FTM_PWM_DeInit(FTM_HandleTypeDef *hftm) {
    HAL_FTM_Base_DeInit(hftm);
}

void HAL_FTM_IC_Init(FTM_HandleTypeDef *hftm) {
    HAL_FTM_Base_Init(hftm);
}

void HAL_FTM_IC_ConfigChannel(FTM_HandleTypeDef *hftm, FTM_IC_InitTypeDef* sConfig, uint32_t channel) {
    FTM_TypeDef *FTMx = hftm->Instance;
    assert_param(IS_FTM_INSTANCE(FTMx));
    assert_param(IS_FTM_CHANNEL(channel));
    assert_param(IS_FTM_IC_POLARITY(sConfig->ICPolarity));

    hftm->State = HAL_FTM_STATE_BUSY;

    FTMx->channel[channel].CSC = sConfig->ICPolarity;

    hftm->State = HAL_FTM_STATE_READY;
}

void HAL_FTM_IC_Start(FTM_HandleTypeDef *hftm, uint32_t channel) {
    //FTM_TypeDef *FTMx = hftm->Instance;
    //assert_param(IS_FTM_INSTANCE(FTMx));

    // Nothing else to do
}

void HAL_FTM_IC_Start_IT(FTM_HandleTypeDef *hftm, uint32_t channel) {
    FTM_TypeDef *FTMx = hftm->Instance;
    assert_param(IS_FTM_INSTANCE(FTMx));

    FTMx->channel[channel].CSC |= FTM_CSC_CHIE;
}

void HAL_FTM_IC_DeInit(FTM_HandleTypeDef *hftm) {
    HAL_FTM_Base_DeInit(hftm);
}

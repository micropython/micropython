/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#ifndef MICROPY_INCLUDED_TEENSY_HAL_FTM_H
#define MICROPY_INCLUDED_TEENSY_HAL_FTM_H

#define FTM0    ((FTM_TypeDef *)&FTM0_SC)
#define FTM1    ((FTM_TypeDef *)&FTM1_SC)
#define FTM2    ((FTM_TypeDef *)&FTM2_SC)

typedef struct {
    volatile uint32_t CSC;	// Channel x Status And Control
    volatile uint32_t CV;		// Channel x Value
} FTM_ChannelTypeDef;

typedef struct {
    volatile uint32_t SC;		// Status And Control
    volatile uint32_t CNT;		// Counter
    volatile uint32_t MOD;		// Modulo
    FTM_ChannelTypeDef channel[8];
    volatile uint32_t CNTIN;	// Counter Initial Value
    volatile uint32_t STATUS;	// Capture And Compare Status
    volatile uint32_t MODE;		// Features Mode Selection
    volatile uint32_t SYNC;		// Synchronization
    volatile uint32_t OUTINIT;  // Initial State For Channels Output
    volatile uint32_t OUTMASK;	// Output Mask
    volatile uint32_t COMBINE;	// Function For Linked Channels
    volatile uint32_t DEADTIME; // Deadtime Insertion Control
    volatile uint32_t EXTTRIG;	// FTM External Trigger
    volatile uint32_t POL;		// Channels Polarity
    volatile uint32_t FMS;		// Fault Mode Status
    volatile uint32_t FILTER;	// Input Capture Filter Control
    volatile uint32_t FLTCTRL;	// Fault Control
    volatile uint32_t QDCTRL;	// Quadrature Decoder Control And Status
    volatile uint32_t CONF;		// Configuration
    volatile uint32_t FLTPOL;	// FTM Fault Input Polarity
    volatile uint32_t SYNCONF;	// Synchronization Configuration
    volatile uint32_t INVCTRL;	// FTM Inverting Control
    volatile uint32_t SWOCTRL;	// FTM Software Output Control
    volatile uint32_t PWMLOAD;  // FTM PWM Load
} FTM_TypeDef;

typedef struct {
    uint32_t    PrescalerShift; // Sets the prescaler to 1 << PrescalerShift
    uint32_t    CounterMode;    // One of FTM_COUNTERMODE_xxx
    uint32_t    Period;         // Specifies the Period for determining timer overflow
} FTM_Base_InitTypeDef;

typedef struct {
    uint32_t    OCMode;         // One of FTM_OCMODE_xxx
    uint32_t    Pulse;          // Specifies initial pulse width (0-0xffff)
    uint32_t    OCPolarity;     // One of FTM_OCPOLRITY_xxx
} FTM_OC_InitTypeDef;

typedef struct {
    uint32_t    ICPolarity;     // Specifies Rising/Falling/Both
} FTM_IC_InitTypeDef;

#define IS_FTM_INSTANCE(INSTANCE)   (((INSTANCE) == FTM0)   || \
                                     ((INSTANCE) == FTM1)   || \
                                     ((INSTANCE) == FTM2))

#define IS_FTM_PRESCALERSHIFT(PRESCALERSHIFT) (((PRESCALERSHIFT) & ~7) == 0)

#define FTM_COUNTERMODE_UP      (0)
#define FTM_COUNTERMODE_CENTER  (FTM_SC_CPWMS)

#define IS_FTM_COUNTERMODE(MODE) (((MODE) == FTM_COUNTERMODE_UP)        ||\
                                  ((MODE) == FTM_COUNTERMODE_CENTER))

#define IS_FTM_PERIOD(PERIOD)   (((PERIOD) & 0xFFFF0000) == 0)

#define FTM_CSC_CHF     0x80
#define FTM_CSC_CHIE    0x40
#define FTM_CSC_MSB     0x20
#define FTM_CSC_MSA     0x10
#define FTM_CSC_ELSB    0x08
#define FTM_CSC_ELSA    0x04
#define FTM_CSC_DMA     0x01

#define FTM_OCMODE_TIMING   (0)
#define FTM_OCMODE_ACTIVE   (FTM_CSC_MSA | FTM_CSC_ELSB | FTM_CSC_ELSA)
#define FTM_OCMODE_INACTIVE (FTM_CSC_MSA | FTM_CSC_ELSB)
#define FTM_OCMODE_TOGGLE   (FTM_CSC_MSA | FTM_CSC_ELSA)
#define FTM_OCMODE_PWM1     (FTM_CSC_MSB | FTM_CSC_ELSB)
#define FTM_OCMODE_PWM2     (FTM_CSC_MSB | FTM_CSC_ELSA)

#define IS_FTM_OC_MODE(mode)        ((mode) == FTM_OCMODE_TIMING || \
                                     (mode) == FTM_OCMODE_ACTIVE || \
                                     (mode) == FTM_OCMODE_INACTIVE || \
                                     (mode) == FTM_OCMODE_TOGGLE )

#define IS_FTM_PWM_MODE(mode)       ((mode) == FTM_OCMODE_PWM1 || \
                                     (mode) == FTM_OCMODE_PWM2)

#define IS_FTM_CHANNEL(channel)     (((channel) & ~7) == 0)

#define IS_FTM_PULSE(pulse)         (((pulse) & ~0xffff) == 0)

#define FTM_OCPOLARITY_HIGH     (0)
#define FTM_OCPOLARITY_LOW      (1)

#define IS_FTM_OC_POLARITY(polarity) ((polarity) == FTM_OCPOLARITY_HIGH    || \
                                      (polarity) == FTM_OCPOLARITY_LOW)

#define FTM_ICPOLARITY_RISING   (FTM_CSC_ELSA)
#define FTM_ICPOLARITY_FALLING  (FTM_CSC_ELSB)
#define FTM_ICPOLARITY_BOTH     (FTM_CSC_ELSA | FTM_CSC_ELSB)

#define IS_FTM_IC_POLARITY(polarity) ((polarity) == FTM_ICPOLARITY_RISING    || \
                                      (polarity) == FTM_ICPOLARITY_FALLING   || \
                                      (polarity) == FTM_ICPOLARITY_BOTH)

typedef enum {
    HAL_FTM_STATE_RESET     = 0x00,
    HAL_FTM_STATE_READY     = 0x01,
    HAL_FTM_STATE_BUSY      = 0x02,
} HAL_FTM_State;

typedef struct {
    FTM_TypeDef            *Instance;
    FTM_Base_InitTypeDef    Init;
    HAL_FTM_State           State;

} FTM_HandleTypeDef;

#define __HAL_FTM_GET_TOF_FLAG(HANDLE)     (((HANDLE)->Instance->SC & FTM_SC_TOF) != 0)
#define __HAL_FTM_CLEAR_TOF_FLAG(HANDLE)    ((HANDLE)->Instance->SC &= ~FTM_SC_TOF)

#define __HAL_FTM_GET_TOF_IT(HANDLE)       (((HANDLE)->Instance->SC & FTM_SC_TOIE) != 0)
#define __HAL_FTM_ENABLE_TOF_IT(HANDLE)     ((HANDLE)->Instance->SC |= FTM_SC_TOIE)
#define __HAL_FTM_DISABLE_TOF_IT(HANDLE)    ((HANDLE)->Instance->SC &= ~FTM_SC_TOIE)

#define __HAL_FTM_GET_CH_FLAG(HANDLE, CH)  (((HANDLE)->Instance->channel[CH].CSC & FTM_CSC_CHF) != 0)
#define __HAL_FTM_CLEAR_CH_FLAG(HANDLE, CH) ((HANDLE)->Instance->channel[CH].CSC &= ~FTM_CSC_CHF)

#define __HAL_FTM_GET_CH_IT(HANDLE, CH)    (((HANDLE)->Instance->channel[CH].CSC & FTM_CSC_CHIE) != 0)
#define __HAL_FTM_ENABLE_CH_IT(HANDLE, CH)  ((HANDLE)->Instance->channel[CH].CSC |= FTM_CSC_CHIE)
#define __HAL_FTM_DISABLE_CH_IT(HANDLE, CH) ((HANDLE)->Instance->channel[CH].CSC &= ~FTM_CSC_CHIE)

void HAL_FTM_Base_Init(FTM_HandleTypeDef *hftm);
void HAL_FTM_Base_Start(FTM_HandleTypeDef *hftm);
void HAL_FTM_Base_Start_IT(FTM_HandleTypeDef *hftm);
void HAL_FTM_Base_DeInit(FTM_HandleTypeDef *hftm);

void HAL_FTM_OC_Init(FTM_HandleTypeDef *hftm);
void HAL_FTM_OC_ConfigChannel(FTM_HandleTypeDef *hftm, FTM_OC_InitTypeDef* sConfig, uint32_t channel);
void HAL_FTM_OC_Start(FTM_HandleTypeDef *hftm, uint32_t channel);
void HAL_FTM_OC_Start_IT(FTM_HandleTypeDef *hftm, uint32_t channel);
void HAL_FTM_OC_DeInit(FTM_HandleTypeDef *hftm);

void HAL_FTM_PWM_Init(FTM_HandleTypeDef *hftm);
void HAL_FTM_PWM_ConfigChannel(FTM_HandleTypeDef *hftm, FTM_OC_InitTypeDef* sConfig, uint32_t channel);
void HAL_FTM_PWM_Start(FTM_HandleTypeDef *hftm, uint32_t channel);
void HAL_FTM_PWM_Start_IT(FTM_HandleTypeDef *hftm, uint32_t channel);
void HAL_FTM_PWM_DeInit(FTM_HandleTypeDef *hftm);

void HAL_FTM_IC_Init(FTM_HandleTypeDef *hftm);
void HAL_FTM_IC_ConfigChannel(FTM_HandleTypeDef *hftm, FTM_IC_InitTypeDef* sConfig, uint32_t channel);
void HAL_FTM_IC_Start(FTM_HandleTypeDef *hftm, uint32_t channel);
void HAL_FTM_IC_Start_IT(FTM_HandleTypeDef *hftm, uint32_t channel);
void HAL_FTM_IC_DeInit(FTM_HandleTypeDef *hftm);

#endif // MICROPY_INCLUDED_TEENSY_HAL_FTM_H

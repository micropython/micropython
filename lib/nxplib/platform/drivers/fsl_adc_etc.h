/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_ADC_ETC_H_
#define _FSL_ADC_ETC_H_

#include "fsl_common.h"

/*!
 *  @addtogroup adc_etc
 *  @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief ADC_ETC driver version */
#define FSL_ADC_ETC_DRIVER_VERSION (MAKE_VERSION(2, 0, 1)) /*!< Version 2.0.1. */
/*! @brief The mask of status flags cleared by writing 1. */
#define ADC_ETC_DMA_CTRL_TRGn_REQ_MASK 0xFF0000U

/*!
 * @brief ADC_ETC customized status flags mask.
 */
enum _adc_etc_status_flag_mask
{
    kADC_ETC_Done0StatusFlagMask = 1U,
    kADC_ETC_Done1StatusFlagMask = 2U,
    kADC_ETC_Done2StatusFlagMask = 4U,
    kADC_ETC_ErrorStatusFlagMask = 8U,
};

/*!
 * @brief External triggers sources.
 */
typedef enum _adc_etc_external_trigger_source
{
    /* External XBAR sources. Support HW or SW mode. */
    kADC_ETC_Trg0TriggerSource = 0U, /* External XBAR trigger0 source. */
    kADC_ETC_Trg1TriggerSource = 1U, /* External XBAR trigger1 source. */
    kADC_ETC_Trg2TriggerSource = 2U, /* External XBAR trigger2 source. */
    kADC_ETC_Trg3TriggerSource = 3U, /* External XBAR trigger3 source. */
    kADC_ETC_Trg4TriggerSource = 4U, /* External XBAR trigger4 source. */
    kADC_ETC_Trg5TriggerSource = 5U, /* External XBAR trigger5 source. */
    kADC_ETC_Trg6TriggerSource = 6U, /* External XBAR trigger6 source. */
    kADC_ETC_Trg7TriggerSource = 7U, /* External XBAR trigger7 source. */
    /* External TSC sources. Only support HW mode. */
    kADC_ETC_TSC0TriggerSource = 8U, /* External TSC trigger0 source. */
    kADC_ETC_TSC1TriggerSource = 9U, /* External TSC trigger1 source. */
} adc_etc_external_trigger_source_t;

/*!
 * @brief Interrupt enable/disable mask.
 */
typedef enum _adc_etc_interrupt_enable
{
    kADC_ETC_InterruptDisable     = 0U, /* Disable the ADC_ETC interrupt. */
    kADC_ETC_Done0InterruptEnable = 1U, /* Enable the DONE0 interrupt when ADC conversions complete. */
    kADC_ETC_Done1InterruptEnable = 2U, /* Enable the DONE1 interrupt when ADC conversions complete. */
    kADC_ETC_Done2InterruptEnable = 3U, /* Enable the DONE2 interrupt when ADC conversions complete. */
} adc_etc_interrupt_enable_t;

#if defined(FSL_FEATURE_ADC_ETC_HAS_CTRL_DMA_MODE_SEL) && FSL_FEATURE_ADC_ETC_HAS_CTRL_DMA_MODE_SEL
/*!
 * @brief DMA mode selection.
 */
typedef enum _adc_etc_dma_mode_selection
{
    kADC_ETC_TrigDMAWithLatchedSignal =
        0U, /* Trig DMA_REQ with latched signal, REQ will be cleared when ACK and source request cleared. */
    kADC_ETC_TrigDMAWithPulsedSignal = 1U, /* Trig DMA_REQ with pulsed signal, REQ will be cleared by ACK only. */
} adc_etc_dma_mode_selection_t;
#endif /*FSL_FEATURE_ADC_ETC_HAS_CTRL_DMA_MODE_SEL*/

/*!
 * @brief ADC_ETC configuration.
 */
typedef struct _adc_etc_config
{
    bool enableTSCBypass;   /* If bypass TSC, TSC would trigger ADC directly.
                               Otherwise TSC would trigger ADC through ADC_ETC. */
    bool enableTSC0Trigger; /* Enable external TSC0 trigger. It is valid when enableTSCBypass = false. */
    bool enableTSC1Trigger; /* Enable external TSC1 trigger. It is valid when enableTSCBypass = false.*/
#if defined(FSL_FEATURE_ADC_ETC_HAS_CTRL_DMA_MODE_SEL) && FSL_FEATURE_ADC_ETC_HAS_CTRL_DMA_MODE_SEL
    adc_etc_dma_mode_selection_t dmaMode; /* Select the ADC_ETC DMA mode. */
#endif                                    /*FSL_FEATURE_ADC_ETC_HAS_CTRL_DMA_MODE_SEL*/
    uint32_t TSC0triggerPriority;         /* External TSC0 trigger priority, 7 is highest, 0 is lowest. */
    uint32_t TSC1triggerPriority;         /* External TSC1 trigger priority, 7 is highest, 0 is lowest. */
    uint32_t clockPreDivider;             /* Pre-divider for trig delay and interval. Available range is 0-255.
                                            Clock would be divided by (clockPreDivider+1). */
    uint32_t XBARtriggerMask; /* Enable the corresponding trigger source. Available range is trigger0:0x01 to
                                 trigger7:0x80
                                 For example, XBARtriggerMask = 0x7U, which means trigger0, trigger1 and trigger2 is
                                 enabled. */
} adc_etc_config_t;

/*!
 * @brief ADC_ETC trigger chain configuration.
 */
typedef struct _adc_etc_trigger_chain_config
{
    bool enableB2BMode;           /* Enable ADC_ETC BackToBack mode. when not enabled B2B mode,
                                     wait until interval delay is reached. */
    uint32_t ADCHCRegisterSelect; /* Select relevant ADC_HCx register to trigger. 1U : HC0, 2U: HC1, 4U: HC2 ... */
    uint32_t ADCChannelSelect;    /* Select ADC sample channel. */
    adc_etc_interrupt_enable_t InterruptEnable; /* Enable/disable Interrupt. */
} adc_etc_trigger_chain_config_t;

/*!
 * @brief ADC_ETC trigger configuration.
 */
typedef struct _adc_etc_trigger_config
{
    bool enableSyncMode; /* Enable the sync Mode, In SyncMode ADC1 and ADC2 are controlled by the same trigger source.
                            In AsyncMode ADC1 and ADC2 are controlled by separate trigger source. */
    bool enableSWTriggerMode;     /* Enable the sofware trigger mode. */
    uint32_t triggerChainLength;  /* TRIG chain length to the ADC. 0: Trig length is 1. ... 7: Trig length is 8. */
    uint32_t triggerPriority;     /* External trigger priority, 7 is highest, 0 is lowest. */
    uint32_t sampleIntervalDelay; /* Set sampling interval delay. */
    uint32_t initialDelay;        /* Set trigger initial delay. */
} adc_etc_trigger_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization
 * @{
 */

/*!
 * @brief Initialize the ADC_ETC module.
 *
 * @param base ADC_ETC peripheral base address.
 * @param config Pointer to "adc_etc_config_t" structure.
 */
void ADC_ETC_Init(ADC_ETC_Type *base, const adc_etc_config_t *config);

/*!
 * @brief De-Initialize the ADC_ETC module.
 *
 * @param base ADC_ETC peripheral base address.
 */
void ADC_ETC_Deinit(ADC_ETC_Type *base);

/*!
 * @brief Gets an available pre-defined settings for the ADC_ETC's configuration.
 * This function initializes the ADC_ETC's configuration structure with available settings. The default values are:
 * @code
 *   config->enableTSCBypass = true;
 *   config->enableTSC0Trigger = false;
 *   config->enableTSC1Trigger = false;
 *   config->TSC0triggerPriority = 0U;
 *   config->TSC1triggerPriority = 0U;
 *   config->clockPreDivider = 0U;
 *   config->XBARtriggerMask = 0U;
 * @endCode
 *
 * @param config Pointer to "adc_etc_config_t" structure.
 */
void ADC_ETC_GetDefaultConfig(adc_etc_config_t *config);

/*!
 * @brief Set the external XBAR trigger configuration.
 *
 * @param base ADC_ETC peripheral base address.
 * @param triggerGroup Trigger group index.
 * @param config Pointer to "adc_etc_trigger_config_t" structure.
 */
void ADC_ETC_SetTriggerConfig(ADC_ETC_Type *base, uint32_t triggerGroup, const adc_etc_trigger_config_t *config);

/*!
 * @brief Set the external XBAR trigger chain configuration.
 * For example, if triggerGroup is set to 0U and chainGroup is set to 1U, which means Trigger0 source's chain1 would be
 * configurated.
 *
 * @param base ADC_ETC peripheral base address.
 * @param triggerGroup Trigger group index. Available number is 0~7.
 * @param chainGroup Trigger chain group index. Available number is 0~7.
 * @param config Pointer to "adc_etc_trigger_chain_config_t" structure.
 */
void ADC_ETC_SetTriggerChainConfig(ADC_ETC_Type *base,
                                   uint32_t triggerGroup,
                                   uint32_t chainGroup,
                                   const adc_etc_trigger_chain_config_t *config);

/*!
 * @brief Gets the interrupt status flags of external XBAR and TSC triggers.
 *
 * @param base ADC_ETC peripheral base address.
 * @param sourceIndex trigger source index.
 *
 * @return Status flags mask of trigger. Refer to "_adc_etc_status_flag_mask".
 */
uint32_t ADC_ETC_GetInterruptStatusFlags(ADC_ETC_Type *base, adc_etc_external_trigger_source_t sourceIndex);

/*!
 * @brief Clears the ADC_ETC's interrupt status falgs.
 *
 * @param base ADC_ETC peripheral base address.
 * @param sourceIndex trigger source index.
 * @param mask Status flags mask of trigger. Refer to "_adc_etc_status_flag_mask".
 */
void ADC_ETC_ClearInterruptStatusFlags(ADC_ETC_Type *base,
                                       adc_etc_external_trigger_source_t sourceIndex,
                                       uint32_t mask);

/*!
 * @brief Enable the DMA corresponding to each trigger source.
 *
 * @param base ADC_ETC peripheral base address.
 * @param triggerGroup Trigger group index. Available number is 0~7.
 */
static inline void ADC_ETC_EnableDMA(ADC_ETC_Type *base, uint32_t triggerGroup)
{
    /* Avoid clearing status flags at the same time. */
    base->DMA_CTRL =
        (base->DMA_CTRL | (ADC_ETC_DMA_CTRL_TRIG0_ENABLE_MASK << triggerGroup)) & ~ADC_ETC_DMA_CTRL_TRGn_REQ_MASK;
}

/*!
 * @brief Disable the DMA corresponding to each trigger sources.
 *
 * @param base ADC_ETC peripheral base address.
 * @param triggerGroup Trigger group index. Available number is 0~7.
 */
static inline void ADC_ETC_DisableDMA(ADC_ETC_Type *base, uint32_t triggerGroup)
{
    /* Avoid clearing status flags at the same time. */
    base->DMA_CTRL =
        (base->DMA_CTRL & ~(ADC_ETC_DMA_CTRL_TRIG0_ENABLE_MASK << triggerGroup)) & ~ADC_ETC_DMA_CTRL_TRGn_REQ_MASK;
}

/*!
 * @brief Get the DMA request status falgs. Only external XBAR sources support DMA request.
 *
 * @param base ADC_ETC peripheral base address.
 * @return Mask of external XBAR tirgger's DMA request asserted flags. Available range is trigger0:0x01 to
 * trigger7:0x80.
 */
static inline uint32_t ADC_ETC_GetDMAStatusFlags(ADC_ETC_Type *base)
{
    return (((base->DMA_CTRL) & ADC_ETC_DMA_CTRL_TRGn_REQ_MASK) >> ADC_ETC_DMA_CTRL_TRIG0_REQ_SHIFT);
}

/*!
 * @brief Clear the DMA request status falgs. Only external XBAR sources support DMA request.
 *
 * @param base ADC_ETC peripheral base address.
 * @param mask Mask of external XBAR tirgger's DMA request asserted flags. Available range is trigger0:0x01 to
 * trigger7:0x80.
 */
static inline void ADC_ETC_ClearDMAStatusFlags(ADC_ETC_Type *base, uint32_t mask)
{
    base->DMA_CTRL = ((base->DMA_CTRL) & ~ADC_ETC_DMA_CTRL_TRGn_REQ_MASK) | (mask << ADC_ETC_DMA_CTRL_TRIG0_REQ_SHIFT);
}

/*!
 * @brief When enable ,all logical will be reset.
 *
 * @param base ADC_ETC peripheral base address.
 * @param enable Enable/Disable the software reset.
 */
static inline void ADC_ETC_DoSoftwareReset(ADC_ETC_Type *base, bool enable)
{
    if (enable)
    {
        base->CTRL |= ADC_ETC_CTRL_SOFTRST_MASK;
    }
    else
    {
        base->CTRL &= ~ADC_ETC_CTRL_SOFTRST_MASK;
    }
}

/*!
 * @brief Do software trigger corresponding to each XBAR trigger sources.
 * Each XBAR trigger sources can be configured as HW or SW trigger mode. In hardware trigger mode,
 * trigger source is from XBAR. In software mode, trigger source is from software tigger. TSC trigger sources
 * can only work in hardware trigger mode.
 *
 * @param base ADC_ETC peripheral base address.
 * @param triggerGroup Trigger group index. Available number is 0~7.
 */
static inline void ADC_ETC_DoSoftwareTrigger(ADC_ETC_Type *base, uint32_t triggerGroup)
{
    assert(triggerGroup < ADC_ETC_TRIGn_CTRL_COUNT);

    base->TRIG[triggerGroup].TRIGn_CTRL |= ADC_ETC_TRIGn_CTRL_SW_TRIG_MASK;
}

/*!
 * @brief Get ADC conversion result from external XBAR sources.
 * For example, if triggerGroup is set to 0U and chainGroup is set to 1U, which means the API would
 * return Trigger0 source's chain1 conversion result.
 *
 * @param base ADC_ETC peripheral base address.
 * @param triggerGroup Trigger group index. Available number is 0~7.
 * @param chainGroup Trigger chain group index. Available number is 0~7.
 * @return ADC conversion result value.
 */
uint32_t ADC_ETC_GetADCConversionValue(ADC_ETC_Type *base, uint32_t triggerGroup, uint32_t chainGroup);

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_ADC_ETC_H_ */

/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_kpp.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.kpp"
#endif

#define KPP_KEYPAD_SCAN_TIMES (3U)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to SEMC clocks for each instance. */
static const clock_ip_name_t s_kppClock[FSL_FEATURE_SOC_KPP_COUNT] = KPP_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*! @brief Pointers to SEMC bases for each instance. */
static KPP_Type *const s_kppBases[] = KPP_BASE_PTRS;

/*! @brief Pointers to KPP IRQ number for each instance. */
static const IRQn_Type s_kppIrqs[] = KPP_IRQS;
/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t KPP_GetInstance(KPP_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_kppBases); instance++)
    {
        if (s_kppBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_kppBases));

    return instance;
}
static void KPP_Mdelay(uint64_t tickets)
{
    while (tickets--)
    {
        __NOP();
    }
}

/*!
 * brief KPP initialize.
 * This function ungates the KPP clock and initializes KPP.
 * This function must be called before calling any other KPP driver functions.
 *
 * param base KPP peripheral base address.
 * param configure The KPP configuration structure pointer.
 */
void KPP_Init(KPP_Type *base, kpp_config_t *configure)
{
    assert(configure);

    uint32_t instance = KPP_GetInstance(base);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Un-gate sdram controller clock. */
    CLOCK_EnableClock(s_kppClock[KPP_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* Clear all. */
    base->KPSR &= ~(KPP_KPSR_KRIE_MASK | KPP_KPSR_KDIE_MASK);

    /* Enable the keypad row and set the column strobe output to open drain. */
    base->KPCR = KPP_KPCR_KRE(configure->activeRow);
    base->KPDR = KPP_KPDR_KCD((uint8_t) ~(configure->activeColumn));
    base->KPCR |= KPP_KPCR_KCO(configure->activeColumn);

    /* Set the input direction for row and output direction for column. */
    base->KDDR = KPP_KDDR_KCDD(configure->activeColumn) | KPP_KDDR_KRDD((uint8_t) ~(configure->activeRow));

    /* Clear the status flag and enable the interrupt. */
    base->KPSR = KPP_KPSR_KPKR_MASK | KPP_KPSR_KPKD_MASK | KPP_KPSR_KDSC_MASK | configure->interrupt;

    if (configure->interrupt)
    {
        /* Enable at the Interrupt */
        EnableIRQ(s_kppIrqs[instance]);
    }
}

/*!
 * brief Deinitializes the KPP module and gates the clock.
 * This function gates the KPP clock. As a result, the KPP
 * module doesn't work after calling this function.
 *
 * param base KPP peripheral base address.
 */
void KPP_Deinit(KPP_Type *base)
{
    /* Disable interrupts and disable all rows. */
    base->KPSR &= ~(KPP_KPSR_KRIE_MASK | KPP_KPSR_KDIE_MASK);
    base->KPCR = 0;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Disable KPP clock. */
    CLOCK_DisableClock(s_kppClock[KPP_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Keypad press scanning.
 *
 * This function will scanning all columns and rows. so
 * all scanning data will be stored in the data pointer.
 *
 * param base  KPP peripheral base address.
 * param data  KPP key press scanning data. The data buffer should be prepared with
 * length at least equal to KPP_KEYPAD_COLUMNNUM_MAX * KPP_KEYPAD_ROWNUM_MAX.
 * the data pointer is recommended to be a array like uint8_t data[KPP_KEYPAD_COLUMNNUM_MAX].
 * for example the data[2] = 4, that means in column 1 row 2 has a key press event.
 */
void KPP_keyPressScanning(KPP_Type *base, uint8_t *data, uint32_t clockSrc_Hz)
{
    assert(data);

    uint16_t kppKCO      = base->KPCR & KPP_KPCR_KCO_MASK;
    uint8_t columIndex   = 0;
    uint8_t activeColumn = (base->KPCR & KPP_KPCR_KCO_MASK) >> KPP_KPCR_KCO_SHIFT;
    uint8_t times;
    uint8_t rowData[KPP_KEYPAD_SCAN_TIMES][KPP_KEYPAD_COLUMNNUM_MAX];
    bool press = false;
    uint8_t column;

    /* Initialize row data to zero. */
    memset(&rowData[0][0], 0, sizeof(rowData));

    /* Scanning. */
    /* Configure the column data to 1 according to column numbers. */
    base->KPDR = KPP_KPDR_KCD_MASK;
    /* Configure column to totem pole for quick discharge of keypad capacitance. */
    base->KPCR &= (uint16_t)(((uint16_t)~kppKCO) | KPP_KPCR_KRE_MASK);
    /* Recover. */
    base->KPCR |= kppKCO;
    /* Three times scanning. */
    for (times = 0; times < KPP_KEYPAD_SCAN_TIMES; times++)
    {
        for (columIndex = 0; columIndex < KPP_KEYPAD_COLUMNNUM_MAX; columIndex++)
        {
            column = activeColumn & (1U << columIndex);
            if (column)
            {
                /* Set the single column line to 0. */
                base->KPDR = KPP_KPDR_KCD(~(uint16_t)column);
                /* Take 100us delays. */
                KPP_Mdelay(clockSrc_Hz / 10000000);
                /* Read row data. */
                rowData[times][columIndex] = ~(base->KPDR & KPP_KPDR_KRD_MASK);
            }
            else
            {
                /* Read row data. */
                rowData[times][columIndex] = 0;
            }
        }
    }

    /* Return all columns to 0 in preparation for standby mode. */
    base->KPDR &= ~KPP_KPDR_KCD_MASK;

    /* Check if three time scan data is the same. */
    for (columIndex = 0; columIndex < KPP_KEYPAD_COLUMNNUM_MAX; columIndex++)
    {
        if ((uint8_t)(rowData[0][columIndex] & rowData[1][columIndex]) & rowData[2][columIndex])
        {
            press = true;
        }
    }

    if (press)
    {
        memcpy((void *)data, &rowData[0][0], sizeof(rowData[0]));
    }
    else
    {
        memset((void *)data, 0, sizeof(rowData[0]));
    }
}

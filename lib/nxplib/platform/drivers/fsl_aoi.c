/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "fsl_aoi.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.aoi"
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Pointers to aoi bases for each instance. */
static AOI_Type *const s_aoiBases[] = AOI_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to aoi clocks for each instance. */
static const clock_ip_name_t s_aoiClocks[] = AOI_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
       /*******************************************************************************
        * Prototypes
        ******************************************************************************/
/*!
 * @brief Get instance number for AOI module.
 *
 * @param base AOI peripheral base address
 *
 * @return The AOI instance
 */
static uint32_t AOI_GetInstance(AOI_Type *base);
/*******************************************************************************
 * Code
 ******************************************************************************/

static uint32_t AOI_GetInstance(AOI_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_aoiBases); instance++)
    {
        if (s_aoiBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_aoiBases));

    return instance;
}

/*!
 * brief Initializes an AOI instance for operation.
 *
 * This function un-gates the AOI clock.
 *
 * param base AOI peripheral address.
 */
void AOI_Init(AOI_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable the clock gate from clock manager. */
    CLOCK_EnableClock(s_aoiClocks[AOI_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Deinitializes an AOI instance for operation.
 *
 * This function shutdowns AOI module.
 *
 * param  base AOI peripheral address.
 */
void AOI_Deinit(AOI_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Disable the clock gate from clock manager */
    CLOCK_DisableClock(s_aoiClocks[AOI_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Gets the Boolean evaluation associated.
 *
 * This function returns the Boolean evaluation associated.
 *
 * Example:
  code
    aoi_event_config_t demoEventLogicStruct;

    AOI_GetEventLogicConfig(AOI, kAOI_Event0, &demoEventLogicStruct);
  endcode
 *
 * param  base AOI peripheral address.
 * param  event Index of the event which will be set of type aoi_event_t.
 * param  config Selected input configuration .
 */
void AOI_GetEventLogicConfig(AOI_Type *base, aoi_event_t event, aoi_event_config_t *config)
{
    assert(event < FSL_FEATURE_AOI_EVENT_COUNT);
    assert(config != NULL);

    uint16_t value = 0;
    /* Read BFCRT01 register at event index. */
    value = base->BFCRT[event].BFCRT01;

    config->PT0AC = (aoi_input_config_t)((value & AOI_BFCRT01_PT0_AC_MASK) >> AOI_BFCRT01_PT0_AC_SHIFT);
    config->PT0BC = (aoi_input_config_t)((value & AOI_BFCRT01_PT0_BC_MASK) >> AOI_BFCRT01_PT0_BC_SHIFT);
    config->PT0CC = (aoi_input_config_t)((value & AOI_BFCRT01_PT0_CC_MASK) >> AOI_BFCRT01_PT0_CC_SHIFT);
    config->PT0DC = (aoi_input_config_t)((value & AOI_BFCRT01_PT0_DC_MASK) >> AOI_BFCRT01_PT0_DC_SHIFT);

    config->PT1AC = (aoi_input_config_t)((value & AOI_BFCRT01_PT1_AC_MASK) >> AOI_BFCRT01_PT1_AC_SHIFT);
    config->PT1BC = (aoi_input_config_t)((value & AOI_BFCRT01_PT1_BC_MASK) >> AOI_BFCRT01_PT1_BC_SHIFT);
    config->PT1CC = (aoi_input_config_t)((value & AOI_BFCRT01_PT1_CC_MASK) >> AOI_BFCRT01_PT1_CC_SHIFT);
    config->PT1DC = (aoi_input_config_t)((value & AOI_BFCRT01_PT1_DC_MASK) >> AOI_BFCRT01_PT1_DC_SHIFT);

    /* Read BFCRT23 register at event index. */
    value = 0;
    value = base->BFCRT[event].BFCRT23;

    config->PT2AC = (aoi_input_config_t)((value & AOI_BFCRT23_PT2_AC_MASK) >> AOI_BFCRT23_PT2_AC_SHIFT);
    config->PT2BC = (aoi_input_config_t)((value & AOI_BFCRT23_PT2_BC_MASK) >> AOI_BFCRT23_PT2_BC_SHIFT);
    config->PT2CC = (aoi_input_config_t)((value & AOI_BFCRT23_PT2_CC_MASK) >> AOI_BFCRT23_PT2_CC_SHIFT);
    config->PT2DC = (aoi_input_config_t)((value & AOI_BFCRT23_PT2_DC_MASK) >> AOI_BFCRT23_PT2_DC_SHIFT);

    config->PT3AC = (aoi_input_config_t)((value & AOI_BFCRT23_PT3_AC_MASK) >> AOI_BFCRT23_PT3_AC_SHIFT);
    config->PT3BC = (aoi_input_config_t)((value & AOI_BFCRT23_PT3_BC_MASK) >> AOI_BFCRT23_PT3_BC_SHIFT);
    config->PT3CC = (aoi_input_config_t)((value & AOI_BFCRT23_PT3_CC_MASK) >> AOI_BFCRT23_PT3_CC_SHIFT);
    config->PT3DC = (aoi_input_config_t)((value & AOI_BFCRT23_PT3_DC_MASK) >> AOI_BFCRT23_PT3_DC_SHIFT);
}

/*!
 * brief Configures an AOI event.
 *
 * This function configures an AOI event according
 * to the aoiEventConfig structure. This function configures all  inputs (A, B, C, and D)
 * of all  product terms (0, 1, 2, and 3) of a desired event.
 *
 * Example:
  code
    aoi_event_config_t demoEventLogicStruct;

    demoEventLogicStruct.PT0AC = kAOI_InvInputSignal;
    demoEventLogicStruct.PT0BC = kAOI_InputSignal;
    demoEventLogicStruct.PT0CC = kAOI_LogicOne;
    demoEventLogicStruct.PT0DC = kAOI_LogicOne;

    demoEventLogicStruct.PT1AC = kAOI_LogicZero;
    demoEventLogicStruct.PT1BC = kAOI_LogicOne;
    demoEventLogicStruct.PT1CC = kAOI_LogicOne;
    demoEventLogicStruct.PT1DC = kAOI_LogicOne;

    demoEventLogicStruct.PT2AC = kAOI_LogicZero;
    demoEventLogicStruct.PT2BC = kAOI_LogicOne;
    demoEventLogicStruct.PT2CC = kAOI_LogicOne;
    demoEventLogicStruct.PT2DC = kAOI_LogicOne;

    demoEventLogicStruct.PT3AC = kAOI_LogicZero;
    demoEventLogicStruct.PT3BC = kAOI_LogicOne;
    demoEventLogicStruct.PT3CC = kAOI_LogicOne;
    demoEventLogicStruct.PT3DC = kAOI_LogicOne;

    AOI_SetEventLogicConfig(AOI, kAOI_Event0, demoEventLogicStruct);
  endcode
 *
 * param  base AOI peripheral address.
 * param  event Event which will be configured of type aoi_event_t.
 * param  eventConfig Pointer to type aoi_event_config_t structure. The user is responsible for
 * filling out the members of this structure and passing the pointer to this function.
 */
void AOI_SetEventLogicConfig(AOI_Type *base, aoi_event_t event, const aoi_event_config_t *eventConfig)
{
    assert(eventConfig != NULL);
    assert(event < FSL_FEATURE_AOI_EVENT_COUNT);

    uint16_t value = 0;
    /* Calculate value to configure product term 0, 1 */
    value = AOI_BFCRT01_PT0_AC(eventConfig->PT0AC) | AOI_BFCRT01_PT0_BC(eventConfig->PT0BC) |
            AOI_BFCRT01_PT0_CC(eventConfig->PT0CC) | AOI_BFCRT01_PT0_DC(eventConfig->PT0DC) |
            AOI_BFCRT01_PT1_AC(eventConfig->PT1AC) | AOI_BFCRT01_PT1_BC(eventConfig->PT1BC) |
            AOI_BFCRT01_PT1_CC(eventConfig->PT1CC) | AOI_BFCRT01_PT1_DC(eventConfig->PT1DC);
    /* Write value to register */
    base->BFCRT[event].BFCRT01 = value;

    /* Reset and calculate value to configure product term 2, 3 */
    value = 0;
    value = AOI_BFCRT23_PT2_AC(eventConfig->PT2AC) | AOI_BFCRT23_PT2_BC(eventConfig->PT2BC) |
            AOI_BFCRT23_PT2_CC(eventConfig->PT2CC) | AOI_BFCRT23_PT2_DC(eventConfig->PT2DC) |
            AOI_BFCRT23_PT3_AC(eventConfig->PT3AC) | AOI_BFCRT23_PT3_BC(eventConfig->PT3BC) |
            AOI_BFCRT23_PT3_CC(eventConfig->PT3CC) | AOI_BFCRT23_PT3_DC(eventConfig->PT3DC);
    /* Write value to register */
    base->BFCRT[event].BFCRT23 = value;
}

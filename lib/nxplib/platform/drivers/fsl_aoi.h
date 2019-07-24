/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_AOI_H_
#define _FSL_AOI_H_

#include "fsl_common.h"

/*!
 * @addtogroup aoi
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifndef AOI
#define AOI AOI0 /*!< AOI peripheral address */
#endif

/*! @name Driver version */
/*@{*/
#define FSL_AOI_DRIVER_VERSION (MAKE_VERSION(2, 0, 0)) /*!< Version 2.0.0. */
/*@}*/

/*!
 * @brief AOI input configurations.
 *
 * The selection item represents the Boolean evaluations.
 */
typedef enum _aoi_input_config
{
    kAOI_LogicZero      = 0x0U, /*!< Forces the input to logical zero. */
    kAOI_InputSignal    = 0x1U, /*!< Passes the input signal. */
    kAOI_InvInputSignal = 0x2U, /*!< Inverts the input signal. */
    kAOI_LogicOne       = 0x3U  /*!< Forces the input to logical one. */
} aoi_input_config_t;

/*!
 * @brief AOI event indexes, where an event is the collection of the four product
 *        terms (0, 1, 2, and 3) and the four signal inputs (A, B, C, and D).
 */
typedef enum _aoi_event
{
    kAOI_Event0 = 0x0U, /*!< Event 0 index */
    kAOI_Event1 = 0x1U, /*!< Event 1 index */
    kAOI_Event2 = 0x2U, /*!< Event 2 index */
    kAOI_Event3 = 0x3U  /*!< Event 3 index */
} aoi_event_t;

/*!
 * @brief AOI event configuration structure
 *
 * Defines structure _aoi_event_config and use the AOI_SetEventLogicConfig() function to make
 * whole event configuration.
 */
typedef struct _aoi_event_config
{
    aoi_input_config_t PT0AC; /*!< Product term 0 input A */
    aoi_input_config_t PT0BC; /*!< Product term 0 input B */
    aoi_input_config_t PT0CC; /*!< Product term 0 input C */
    aoi_input_config_t PT0DC; /*!< Product term 0 input D */
    aoi_input_config_t PT1AC; /*!< Product term 1 input A */
    aoi_input_config_t PT1BC; /*!< Product term 1 input B */
    aoi_input_config_t PT1CC; /*!< Product term 1 input C */
    aoi_input_config_t PT1DC; /*!< Product term 1 input D */
    aoi_input_config_t PT2AC; /*!< Product term 2 input A */
    aoi_input_config_t PT2BC; /*!< Product term 2 input B */
    aoi_input_config_t PT2CC; /*!< Product term 2 input C */
    aoi_input_config_t PT2DC; /*!< Product term 2 input D */
    aoi_input_config_t PT3AC; /*!< Product term 3 input A */
    aoi_input_config_t PT3BC; /*!< Product term 3 input B */
    aoi_input_config_t PT3CC; /*!< Product term 3 input C */
    aoi_input_config_t PT3DC; /*!< Product term 3 input D */
} aoi_event_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*!
 * @name  AOI Initialization
 * @{
 */

/*!
 * @brief Initializes an AOI instance for operation.
 *
 * This function un-gates the AOI clock.
 *
 * @param base AOI peripheral address.
 */
void AOI_Init(AOI_Type *base);

/*!
 * @brief Deinitializes an AOI instance for operation.
 *
 * This function shutdowns AOI module.
 *
 * @param  base AOI peripheral address.
 */
void AOI_Deinit(AOI_Type *base);

/*@}*/

/*!
 * @name  AOI Get Set Operation
 * @{
 */

/*!
 * @brief Gets the Boolean evaluation associated.
 *
 * This function returns the Boolean evaluation associated.
 *
 * Example:
  @code
    aoi_event_config_t demoEventLogicStruct;

    AOI_GetEventLogicConfig(AOI, kAOI_Event0, &demoEventLogicStruct);
  @endcode
 *
 * @param  base AOI peripheral address.
 * @param  event Index of the event which will be set of type aoi_event_t.
 * @param  config Selected input configuration .
 */
void AOI_GetEventLogicConfig(AOI_Type *base, aoi_event_t event, aoi_event_config_t *config);

/*!
 * @brief Configures an AOI event.
 *
 * This function configures an AOI event according
 * to the aoiEventConfig structure. This function configures all  inputs (A, B, C, and D)
 * of all  product terms (0, 1, 2, and 3) of a desired event.
 *
 * Example:
  @code
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
  @endcode
 *
 * @param  base AOI peripheral address.
 * @param  event Event which will be configured of type aoi_event_t.
 * @param  eventConfig Pointer to type aoi_event_config_t structure. The user is responsible for
 * filling out the members of this structure and passing the pointer to this function.
 */
void AOI_SetEventLogicConfig(AOI_Type *base, aoi_event_t event, const aoi_event_config_t *eventConfig);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

/*@}*/

/*!* @} */

#endif /* _FSL_AOI_H_*/

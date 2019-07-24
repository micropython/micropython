/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_SDMMC_EVENT_H_
#define _FSL_SDMMC_EVENT_H_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief Event type  */
typedef enum _sdmmc_event
{
    kSDMMCEVENT_TransferComplete = 0U, /*!< Transfer complete event */
    kSDMMCEVENT_CardDetect = 1U,       /*!< Card detect event */
} sdmmc_event_t;

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Event Function
 * @{
 */

/*!
 * @brief Initialize timer to implement wait event timeout.
 */
void SDMMCEVENT_InitTimer(void);

/* Callback function for SDHC */

/*!
 * @brief Create event.
 * @param eventType The event type
 * @retval true Create event successfully.
 * @retval false Create event failed.
 */
bool SDMMCEVENT_Create(sdmmc_event_t eventType);

/*!
 * @brief Wait event.
 *
 * @param eventType The event type
 * @param timeoutMilliseconds Timeout time in milliseconds.
 * @retval true Wait event successfully.
 * @retval false Wait event failed.
 */
bool SDMMCEVENT_Wait(sdmmc_event_t eventType, uint32_t timeoutMilliseconds);

/*!
 * @brief Notify event.
 * @param eventType The event type
 * @retval true Notify event successfully.
 * @retval false Notify event failed.
 */
bool SDMMCEVENT_Notify(sdmmc_event_t eventType);

/*!
 * @brief Delete event.
 * @param eventType The event type
 */
void SDMMCEVENT_Delete(sdmmc_event_t eventType);

/*!
 * @brief sdmmc delay.
 * @param milliseconds time to delay
 */
void SDMMCEVENT_Delay(uint32_t milliseconds);

/* @} */

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_SDMMC_EVENT_H_*/

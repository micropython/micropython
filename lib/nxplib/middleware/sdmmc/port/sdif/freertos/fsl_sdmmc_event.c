/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdbool.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "event_groups.h"
#include "fsl_sdmmc_event.h"
#include "semphr.h"

/*******************************************************************************
 * Definitons
 ******************************************************************************/
/*! @brief Convert the milliseconds to ticks in FreeRTOS. */
#define MSEC_TO_TICK(msec) \
    (((uint32_t)(msec) + 500uL / (uint32_t)configTICK_RATE_HZ) * (uint32_t)configTICK_RATE_HZ / 1000uL)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get event instance.
 * @param eventType The event type
 * @return The event instance's pointer.
 */
static SemaphoreHandle_t *SDMMCEVENT_GetInstance(sdmmc_event_t eventType);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Transfer complete event. */
static SemaphoreHandle_t g_eventTransferComplete;
/*! @brief Card detect event. */
static SemaphoreHandle_t g_eventCardDetect;

/*******************************************************************************
 * Code
 ******************************************************************************/
static SemaphoreHandle_t *SDMMCEVENT_GetInstance(sdmmc_event_t eventType)
{
    SemaphoreHandle_t *event;

    switch (eventType)
    {
        case kSDMMCEVENT_TransferComplete:
            event = &g_eventTransferComplete;
            break;
        case kSDMMCEVENT_CardDetect:
            event = &g_eventCardDetect;
            break;
        default:
            event = NULL;
            break;
    }

    return event;
}

bool SDMMCEVENT_Create(sdmmc_event_t eventType)
{
    SemaphoreHandle_t *event = SDMMCEVENT_GetInstance(eventType);

    if (event)
    {
        *event = xSemaphoreCreateBinary();
        if (*event == NULL)
        {
            return false;
        }

        return true;
    }
    else
    {
        return false;
    }
}

bool SDMMCEVENT_Wait(sdmmc_event_t eventType, uint32_t timeoutMilliseconds)
{
    uint32_t timeoutTicks;
    SemaphoreHandle_t *event = SDMMCEVENT_GetInstance(eventType);

    if (timeoutMilliseconds && event && (*event != 0U))
    {
        if (timeoutMilliseconds == ~0U)
        {
            timeoutTicks = portMAX_DELAY;
        }
        else
        {
            timeoutTicks = MSEC_TO_TICK(timeoutMilliseconds);
        }
        if (xSemaphoreTake(*event, timeoutTicks) == pdFALSE)
        {
            return false; /* timeout */
        }
        else
        {
            return true; /* event taken */
        }
    }
    else
    {
        return false;
    }
}

bool SDMMCEVENT_Notify(sdmmc_event_t eventType)
{
    SemaphoreHandle_t *event = SDMMCEVENT_GetInstance(eventType);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult = pdFAIL;

    if (event && (*event != 0U))
    {
        xResult = xSemaphoreGiveFromISR(*event, &xHigherPriorityTaskWoken);
        if (xResult != pdFAIL)
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

void SDMMCEVENT_Delete(sdmmc_event_t eventType)
{
    SemaphoreHandle_t *event = SDMMCEVENT_GetInstance(eventType);

    if (event && (*event != 0U))
    {
        vSemaphoreDelete(*event);
    }
}

void SDMMCEVENT_Delay(uint32_t milliseconds)
{
    vTaskDelay(MSEC_TO_TICK(milliseconds));
}

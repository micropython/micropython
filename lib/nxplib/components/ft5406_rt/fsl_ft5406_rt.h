/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_FT5406_RT_H_
#define _FSL_FT5406_RT_H_

#include "fsl_common.h"

/*!
 * @addtogroup ft5406_rt
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief FT5406_RT I2C address. */
#define FT5406_RT_I2C_ADDRESS (0x38)

/*! @brief FT5406_RT maximum number of simultaneously detected touches. */
#define FT5406_RT_MAX_TOUCHES (5U)

/*! @brief FT5406_RT register address where touch data begin. */
#define FT5406_RT_TOUCH_DATA_SUBADDR (1)

/*! @brief FT5406_RT raw touch data length. */
#define FT5406_RT_TOUCH_DATA_LEN (0x20)

typedef enum _touch_event
{
    kTouch_Down     = 0, /*!< The state changed to touched. */
    kTouch_Up       = 1, /*!< The state changed to not touched. */
    kTouch_Contact  = 2, /*!< There is a continuous touch being detected. */
    kTouch_Reserved = 3  /*!< No touch information available. */
} touch_event_t;

typedef struct _touch_point
{
    touch_event_t TOUCH_EVENT; /*!< Indicates the state or event of the touch point. */
    uint8_t TOUCH_ID; /*!< Id of the touch point. This numeric value stays constant between down and up event. */
    uint16_t TOUCH_X; /*!< X coordinate of the touch point */
    uint16_t TOUCH_Y; /*!< Y coordinate of the touch point */
} touch_point_t;

typedef struct _ft5406_rt_handle
{
    LPI2C_Type *base;
    lpi2c_master_transfer_t xfer;
    uint8_t touch_buf[FT5406_RT_TOUCH_DATA_LEN];
} ft5406_rt_handle_t;

status_t FT5406_RT_Init(ft5406_rt_handle_t *handle, LPI2C_Type *base);

status_t FT5406_RT_Denit(ft5406_rt_handle_t *handle);

status_t FT5406_RT_GetSingleTouch(ft5406_rt_handle_t *handle, touch_event_t *touch_event, int *touch_x, int *touch_y);

status_t FT5406_RT_GetMultiTouch(ft5406_rt_handle_t *handle,
                                 int *touch_count,
                                 touch_point_t touch_array[FT5406_RT_MAX_TOUCHES]);

#endif

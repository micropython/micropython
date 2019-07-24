/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_FLEXIO_CAMERA_H_
#define _FSL_FLEXIO_CAMERA_H_

#include "fsl_common.h"
#include "fsl_flexio.h"
/*!
 * @addtogroup flexio_camera
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief FlexIO Camera driver version 2.1.2. */
#define FSL_FLEXIO_CAMERA_DRIVER_VERSION (MAKE_VERSION(2, 1, 2))
/*@}*/

/*! @brief Define the Camera CPI interface is constantly 8-bit width. */
#define FLEXIO_CAMERA_PARALLEL_DATA_WIDTH (8U)

/*! @brief Error codes for the Camera driver. */
enum _flexio_camera_status
{
    kStatus_FLEXIO_CAMERA_RxBusy = MAKE_STATUS(kStatusGroup_FLEXIO_CAMERA, 0), /*!< Receiver is busy. */
    kStatus_FLEXIO_CAMERA_RxIdle = MAKE_STATUS(kStatusGroup_FLEXIO_CAMERA, 1), /*!< Camera receiver is idle. */
};

/*! @brief Define FlexIO Camera status mask. */
enum _flexio_camera_status_flags
{
    kFLEXIO_CAMERA_RxDataRegFullFlag = 0x1U, /*!< Receive buffer full flag. */
    kFLEXIO_CAMERA_RxErrorFlag       = 0x2U, /*!< Receive buffer error flag. */
};

/*!
 * @brief Define structure of configuring the FlexIO Camera device.
 */
typedef struct _flexio_camera_type
{
    FLEXIO_Type *flexioBase; /*!< FlexIO module base address. */
    uint32_t datPinStartIdx; /*!< First data pin (D0) index for flexio_camera.
                                  Then the successive following FLEXIO_CAMERA_DATA_WIDTH-1 pins
                                  are used as D1-D7.*/
    uint32_t pclkPinIdx;     /*!< Pixel clock pin (PCLK) index for flexio_camera. */
    uint32_t hrefPinIdx;     /*!< Horizontal sync pin (HREF) index for flexio_camera. */

    uint32_t shifterStartIdx; /*!< First shifter index used for flexio_camera data FIFO. */
    uint32_t shifterCount;    /*!< The count of shifters that are used as flexio_camera data FIFO. */
    uint32_t timerIdx;        /*!< Timer index used for flexio_camera in FlexIO. */
} FLEXIO_CAMERA_Type;

/*! @brief Define FlexIO Camera user configuration structure. */
typedef struct _flexio_camera_config
{
    bool enablecamera;     /*!< Enable/disable FlexIO Camera TX & RX. */
    bool enableInDoze;     /*!< Enable/disable FlexIO operation in doze mode*/
    bool enableInDebug;    /*!< Enable/disable FlexIO operation in debug mode*/
    bool enableFastAccess; /*!< Enable/disable fast access to FlexIO registers,
                            fast access requires the FlexIO clock to be at least
                            twice the frequency of the bus clock. */
} flexio_camera_config_t;

/*! @brief Define FlexIO Camera transfer structure. */
typedef struct _flexio_camera_transfer
{
    uint32_t dataAddress; /*!< Transfer buffer*/
    uint32_t dataNum;     /*!< Transfer num*/
} flexio_camera_transfer_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus*/

/*!
 * @name Initialization and configuration
 * @{
 */

/*!
 * @brief Ungates the FlexIO clock, resets the FlexIO module, and configures the FlexIO Camera.
 *
 * @param base Pointer to FLEXIO_CAMERA_Type structure
 * @param config Pointer to flexio_camera_config_t structure
 */
void FLEXIO_CAMERA_Init(FLEXIO_CAMERA_Type *base, const flexio_camera_config_t *config);

/*!
 * @brief Resets the FLEXIO_CAMERA shifer and timer config.
 *
 * @note After calling this API, call FLEXO_CAMERA_Init to use the FlexIO Camera module.
 *
 * @param base Pointer to FLEXIO_CAMERA_Type structure
 */
void FLEXIO_CAMERA_Deinit(FLEXIO_CAMERA_Type *base);

/*!
 * @brief Gets the default configuration to configure the FlexIO Camera. The configuration
 * can be used directly for calling the FLEXIO_CAMERA_Init().
 * Example:
   @code
   flexio_camera_config_t config;
   FLEXIO_CAMERA_GetDefaultConfig(&userConfig);
   @endcode
 * @param config Pointer to the flexio_camera_config_t structure
*/
void FLEXIO_CAMERA_GetDefaultConfig(flexio_camera_config_t *config);

/*!
 * @brief Enables/disables the FlexIO Camera module operation.
 *
 * @param base Pointer to the FLEXIO_CAMERA_Type
 * @param enable True to enable, false does not have any effect.
 */
static inline void FLEXIO_CAMERA_Enable(FLEXIO_CAMERA_Type *base, bool enable)
{
    if (enable)
    {
        base->flexioBase->CTRL |= FLEXIO_CTRL_FLEXEN_MASK;
    }
}

/*! @} */

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Gets the FlexIO Camera status flags.
 *
 * @param base Pointer to FLEXIO_CAMERA_Type structure
 * @return FlexIO shifter status flags
 *          @arg FLEXIO_SHIFTSTAT_SSF_MASK
 *          @arg 0
 */
uint32_t FLEXIO_CAMERA_GetStatusFlags(FLEXIO_CAMERA_Type *base);

/*!
 * @brief Clears the receive buffer full flag manually.
 *
 * @param base Pointer to the device.
 * @param mask status flag
 *      The parameter can be any combination of the following values:
 *          @arg kFLEXIO_CAMERA_RxDataRegFullFlag
 *          @arg kFLEXIO_CAMERA_RxErrorFlag
 */
void FLEXIO_CAMERA_ClearStatusFlags(FLEXIO_CAMERA_Type *base, uint32_t mask);

/* @} */

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief Switches on the interrupt for receive buffer full event.
 *
 * @param base Pointer to the device.
 */
void FLEXIO_CAMERA_EnableInterrupt(FLEXIO_CAMERA_Type *base);

/*!
 * @brief Switches off the interrupt for receive buffer full event.
 *
 * @param base Pointer to the device.
 *
 */
void FLEXIO_CAMERA_DisableInterrupt(FLEXIO_CAMERA_Type *base);

/*! @} */

/*!
 * @name DMA support
 * @{
 */

/*!
 * @brief Enables/disables the FlexIO Camera receive DMA.
 *
 * @param base Pointer to FLEXIO_CAMERA_Type structure
 * @param enable True to enable, false to disable.
 *
 *    The FlexIO Camera mode can't work without the DMA or eDMA support,
 *    Usually, it needs at least two DMA or eDMA channels, one for transferring data from
 *    Camera, such as 0V7670 to FlexIO buffer, another is for transferring data from FlexIO
 *    buffer to LCD.
 *
 */
static inline void FLEXIO_CAMERA_EnableRxDMA(FLEXIO_CAMERA_Type *base, bool enable)
{
    FLEXIO_EnableShifterStatusDMA(base->flexioBase, 1 << base->shifterStartIdx, enable);
}

/*!
 * @brief Gets the data from the receive buffer.
 *
 * @param base Pointer to the device.
 * @return data Pointer to the buffer that keeps the data with count of base->shifterCount .
 */
static inline uint32_t FLEXIO_CAMERA_GetRxBufferAddress(FLEXIO_CAMERA_Type *base)
{
    return FLEXIO_GetShifterBufferAddress(base->flexioBase, kFLEXIO_ShifterBuffer, base->shifterStartIdx);
}

/*! @} */

#if defined(__cplusplus)
}
#endif /*_cplusplus*/

/*@}*/

#endif /*_FSL_FLEXIO_CAMERA_H_*/

/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_MDIO_H_
#define _FSL_MDIO_H_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Defines the timeout macro. */
#if defined(MDIO_TIMEOUT_COUNT_NUMBER) && MDIO_TIMEOUT_COUNT_NUMBER
#define MDIO_TIMEOUT_COUNT MDIO_TIMEOUT_COUNT_NUMBER
#endif

/*! @brief Defines the PHY status. */
enum _mdio_status
{
    kStatus_PHY_SMIVisitTimeout = MAKE_STATUS(kStatusGroup_PHY, 0), /*!< ENET PHY SMI visit timeout. */
};

typedef struct _mdio_operations mdio_operations_t;

/*! @brief MDIO resource. */
typedef struct _mdio_resource
{
    void *base;           /*!< ENET Ip register base. */
    uint32_t csrClock_Hz; /*!< ENET CSR clock. */
} mdio_resource_t;

/*! @brief MDIO handle. */
typedef struct _mdio_handle
{
    mdio_resource_t resource;
    const mdio_operations_t *ops;
} mdio_handle_t;

/*! @brief Camera receiver operations. */
struct _mdio_operations
{
    void (*mdioInit)(mdio_handle_t *handle); /*!< MDIO interface init. */
    status_t (*mdioWrite)(mdio_handle_t *handle,
                          uint32_t phyAddr,
                          uint32_t devAddr,
                          uint32_t data); /*!< MDIO write data. */
    status_t (*mdioRead)(mdio_handle_t *handle,
                         uint32_t phyAddr,
                         uint32_t devAddr,
                         uint32_t *dataPtr); /*!< MDIO read data. */
    status_t (*mdioWriteExt)(mdio_handle_t *handle,
                             uint32_t phyAddr,
                             uint32_t devAddr,
                             uint32_t data); /*!< MDIO write data. */
    status_t (*mdioReadExt)(mdio_handle_t *handle,
                            uint32_t phyAddr,
                            uint32_t devAddr,
                            uint32_t *dataPtr); /*!< MDIO read data. */
};

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif
/*!
 * @name MDIO Driver
 * @{
 */

/*!
 * @brief MDIO Write function. This function write data over the SMI to
 * the specified MDIO register. This function is called by all MDIO interfaces.
 *
 * @param handle  MDIO device handle.
 * @retval kStatus_Success     MDIO write success
 * @retval kStatus_MDIO_SMIVisitTimeout  MDIO SMI visit time out
 */
static inline void MDIO_Init(mdio_handle_t *handle)
{
    handle->ops->mdioInit(handle);
}

/*!
 * @brief MDIO Write function. This function write data over the SMI to
 * the specified MDIO register. This function is called by all MDIO interfaces.
 *
 * @param handle  MDIO device handle.
 * @param phyAddr  MDIO PHY address handle.
 * @param devAddr  The PHY device register.
 * @param data    The data written to the MDIO register.
 * @retval kStatus_Success     MDIO write success
 * @retval kStatus_MDIO_SMIVisitTimeout  MDIO SMI visit time out
 */
static inline status_t MDIO_Write(mdio_handle_t *handle, uint32_t phyAddr, uint32_t devAddr, uint32_t data)
{
    return handle->ops->mdioWrite(handle, phyAddr, devAddr, data);
}

/*!
 * @brief MDIO Read function. This interface read data over the SMI from the
 * specified MDIO register. This function is called by all MDIO interfaces.
 *
 * @param handle   MDIO device handle.
 * @param phyAddr  MDIO PHY address handle.
 * @param devAddr  The PHY device register.
 * @param dataPtr  The address to store the data read from the MDIO register.
 * @retval kStatus_Success  MDIO read success
 * @retval kStatus_MDIO_SMIVisitTimeout  MDIO SMI visit time out
 */
static inline status_t MDIO_Read(mdio_handle_t *handle, uint32_t phyAddr, uint32_t devAddr, uint32_t *dataPtr)
{
    return handle->ops->mdioRead(handle, phyAddr, devAddr, dataPtr);
}

/* @} */

#if defined(__cplusplus)
}
#endif

#endif

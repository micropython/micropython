/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_PHY_H_
#define _FSL_PHY_H_

#include "fsl_mdio.h"

/*! @note The following PHY registers are the IEEE802.3 standard definition, same register and bit field may
          have different names in various PHYs, but the feature they represent should be same or very similar. */

/*! @brief Defines the IEEE802.3 standard PHY registers. */
#define PHY_BASICCONTROL_REG        0x00U /*!< The PHY basic control register. */
#define PHY_BASICSTATUS_REG         0x01U /*!< The PHY basic status register. */
#define PHY_ID1_REG                 0x02U /*!< The PHY ID one register. */
#define PHY_ID2_REG                 0x03U /*!< The PHY ID two register. */
#define PHY_AUTONEG_ADVERTISE_REG   0x04U /*!< The PHY auto-negotiate advertise register. */
#define PHY_AUTONEG_LINKPARTNER_REG 0x05U /*!< The PHY auto negotiation link partner ability register. */
#define PHY_AUTONEG_EXPANSION_REG   0x06U /*!< The PHY auto negotiation expansion register. */
#define PHY_1000BASET_CONTROL_REG   0x09U /*!< The PHY 1000BASE-T control register. */
#define PHY_MMD_ACCESS_CONTROL_REG  0x0DU /*!< The PHY MMD access control register. */
#define PHY_MMD_ACCESS_DATA_REG     0x0EU /*!< The PHY MMD access data register. */

/*! @brief Defines the mask flag in basic control register(Address 0x00). */
#define PHY_BCTL_SPEED1_MASK          0x0040U /*!< The PHY speed bit mask(MSB).*/
#define PHY_BCTL_ISOLATE_MASK         0x0400U /*!< The PHY isolate mask.*/
#define PHY_BCTL_DUPLEX_MASK          0x0100U /*!< The PHY duplex bit mask. */
#define PHY_BCTL_RESTART_AUTONEG_MASK 0x0200U /*!< The PHY restart auto negotiation mask. */
#define PHY_BCTL_AUTONEG_MASK         0x1000U /*!< The PHY auto negotiation bit mask. */
#define PHY_BCTL_SPEED0_MASK          0x2000U /*!< The PHY speed bit mask(LSB). */
#define PHY_BCTL_LOOP_MASK            0x4000U /*!< The PHY loop bit mask. */
#define PHY_BCTL_RESET_MASK           0x8000U /*!< The PHY reset bit mask. */

/*! @brief Defines the mask flag in basic status register(Address 0x01). */
#define PHY_BSTATUS_LINKSTATUS_MASK  0x0004U /*!< The PHY link status mask. */
#define PHY_BSTATUS_AUTONEGABLE_MASK 0x0008U /*!< The PHY auto-negotiation ability mask. */
#define PHY_BSTATUS_SPEEDUPLX_MASK   0x001CU /*!< The PHY speed and duplex mask. */
#define PHY_BSTATUS_AUTONEGCOMP_MASK 0x0020U /*!< The PHY auto-negotiation complete mask. */

/*! @brief Defines the mask flag in PHY auto-negotiation advertise register(Address 0x04). */
#define PHY_100BaseT4_ABILITY_MASK    0x200U /*!< The PHY have the T4 ability. */
#define PHY_100BASETX_FULLDUPLEX_MASK 0x100U /*!< The PHY has the 100M full duplex ability.*/
#define PHY_100BASETX_HALFDUPLEX_MASK 0x080U /*!< The PHY has the 100M full duplex ability.*/
#define PHY_10BASETX_FULLDUPLEX_MASK  0x040U /*!< The PHY has the 10M full duplex ability.*/
#define PHY_10BASETX_HALFDUPLEX_MASK  0x020U /*!< The PHY has the 10M full duplex ability.*/
#define PHY_IEEE802_3_SELECTOR_MASK   0x001U /*!< The message type being sent by Auto-Nego.*/

/*! @brief Defines the mask flag in the 1000BASE-T control register(Address 0x09). */
#define PHY_1000BASET_FULLDUPLEX_MASK 0x200U /*!< The PHY has the 1000M full duplex ability.*/
#define PHY_1000BASET_HALFDUPLEX_MASK 0x100U /*!< The PHY has the 1000M half duplex ability.*/

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef struct _phy_handle phy_handle_t;
/*! @brief Defines the PHY link speed. */
typedef enum _phy_speed
{
    kPHY_Speed10M = 0U, /*!< ENET PHY 10M speed. */
    kPHY_Speed100M,     /*!< ENET PHY 100M speed. */
    kPHY_Speed1000M     /*!< ENET PHY 1000M speed. */
} phy_speed_t;

/*! @brief Defines the PHY link duplex. */
typedef enum _phy_duplex
{
    kPHY_HalfDuplex = 0U, /*!< ENET PHY half duplex. */
    kPHY_FullDuplex       /*!< ENET PHY full duplex. */
} phy_duplex_t;

/*! @brief Defines the PHY loopback mode. */
typedef enum _phy_loop
{
    kPHY_LocalLoop = 0U, /*!< ENET PHY local/digital loopback. */
    kPHY_RemoteLoop,     /*!< ENET PHY remote loopback. */
    kPHY_ExternalLoop,   /*!< ENET PHY external loopback. */
} phy_loop_t;

/*! @brief Defines the PHY MMD data access mode. */
typedef enum _phy_mmd_access_mode
{
    kPHY_MMDAccessNoPostIncrement = (1U << 14), /*!< ENET PHY MMD access data with no address post increment. */
    kPHY_MMDAccessRdWrPostIncrement =
        (2U << 14),                             /*!< ENET PHY MMD access data with Read/Write address post increment. */
    kPHY_MMDAccessWrPostIncrement = (3U << 14), /*!< ENET PHY MMD access data with Write address post increment. */
} phy_mmd_access_mode_t;

/*! @brief Defines PHY configuration. */
typedef struct _phy_config
{
    uint32_t phyAddr;    /*!< PHY address. */
    phy_speed_t speed;   /*!< PHY speed configuration. */
    phy_duplex_t duplex; /*!< PHY duplex configuration. */
    bool autoNeg;        /*!< PHY auto-negotiation, true: enable, false: disable. */
    bool enableEEE;      /*!< PHY Energy Efficient Ethernet. */
} phy_config_t;

/*! @brief PHY device operations. */
typedef struct _phy_operations
{
    status_t (*phyInit)(phy_handle_t *handle, const phy_config_t *config);
    status_t (*phyWrite)(phy_handle_t *handle, uint32_t phyReg, uint32_t data);
    status_t (*phyRead)(phy_handle_t *handle, uint32_t phyReg, uint32_t *dataPtr);
    status_t (*getAutoNegoStatus)(phy_handle_t *handle, bool *status);
    status_t (*getLinkStatus)(phy_handle_t *handle, bool *status);
    status_t (*getLinkSpeedDuplex)(phy_handle_t *handle, phy_speed_t *speed, phy_duplex_t *duplex);
    status_t (*setLinkSpeedDuplex)(phy_handle_t *handle, phy_speed_t speed, phy_duplex_t duplex);
    status_t (*enableLoopback)(phy_handle_t *handle, phy_loop_t mode, phy_speed_t speed, bool enable);
} phy_operations_t;

/*! @brief PHY device handle. */

struct _phy_handle
{
    uint32_t phyAddr;            /*!< PHY address. */
    mdio_handle_t *mdioHandle;   /*!< The MDIO handle used by the phy device, it is specified by device. */
    const phy_operations_t *ops; /*!< The device related operations. */
};

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name PHY Driver
 * @{
 */

/*!
 * @brief Initializes PHY.
 *
 *  This function initialize PHY.
 *
 * @param handle       PHY device handle.
 * @param config       Pointer to structure of phy_config_t.
 * @retval kStatus_Success  PHY initialization succeeds
 * @retval kStatus_Fail  PHY initialization fails
 * @retval kStatus_PHY_SMIVisitTimeout  PHY SMI visit time out
 */
static inline status_t PHY_Init(phy_handle_t *handle, const phy_config_t *config) {
    return handle->ops->phyInit(handle, config);
}
/*!
 * @brief PHY Write function. This function write data over the SMI to
 * the specified PHY register. This function is called by all PHY interfaces.
 *
 * @param handle       PHY device handle.
 * @param phyReg  The PHY register.
 * @param data    The data written to the PHY register.
 * @retval kStatus_Success     PHY write success
 * @retval kStatus_PHY_SMIVisitTimeout  PHY SMI visit time out
 */
static inline status_t PHY_Write(phy_handle_t *handle, uint32_t phyReg, uint32_t data) {
    return handle->ops->phyWrite(handle, phyReg, data);
}

/*!
 * @brief PHY Read function. This interface read data over the SMI from the
 * specified PHY register. This function is called by all PHY interfaces.
 *
 * @param handle       PHY device handle.
 * @param phyReg   The PHY register.
 * @param dataPtr  The address to store the data read from the PHY register.
 * @retval kStatus_Success  PHY read success
 * @retval kStatus_PHY_SMIVisitTimeout  PHY SMI visit time out
 */
static inline status_t PHY_Read(phy_handle_t *handle, uint32_t phyReg, uint32_t *dataPtr) {
    return handle->ops->phyRead(handle, phyReg, dataPtr);
}

/*!
 * @brief Gets the PHY auto-negotiation status.
 *
 * @param handle   PHY device handle.
 * @param status   The auto-negotiation status of the PHY.
 *         - true the auto-negotiation is over.
 *         - false the auto-negotiation is on-going or not started.
 * @retval kStatus_Success   PHY gets status success
 * @retval kStatus_PHY_SMIVisitTimeout  PHY SMI visit time out
 */
static inline status_t PHY_GetAutoNegotiationStatus(phy_handle_t *handle, bool *status) {
    return handle->ops->getAutoNegoStatus(handle, status);
}

/*!
 * @brief Gets the PHY link status.
 *
 * @param handle       PHY device handle.
 * @param status   The link up or down status of the PHY.
 *         - true the link is up.
 *         - false the link is down.
 * @retval kStatus_Success   PHY get link status success
 * @retval kStatus_PHY_SMIVisitTimeout  PHY SMI visit time out
 */
static inline status_t PHY_GetLinkStatus(phy_handle_t *handle, bool *status) {
    return handle->ops->getLinkStatus(handle, status);
}

/*!
 * @brief Gets the PHY link speed and duplex.
 *
 * @brief This function gets the speed and duplex mode of PHY. User can give one of speed
 * and duplex address paramter and set the other as NULL if only wants to get one of them.
 *
 * @param handle       PHY device handle.
 * @param speed    The address of PHY link speed.
 * @param duplex   The link duplex of PHY.
 * @retval kStatus_Success   PHY get link speed and duplex success
 * @retval kStatus_PHY_SMIVisitTimeout  PHY SMI visit time out
 */
static inline status_t PHY_GetLinkSpeedDuplex(phy_handle_t *handle, phy_speed_t *speed, phy_duplex_t *duplex) {
    return handle->ops->getLinkSpeedDuplex(handle, speed, duplex);
}

/*!
 * @brief Sets the PHY link speed and duplex.
 *
 * @param handle   PHY device handle.
 * @param speed    Specified PHY link speed.
 * @param duplex   Specified PHY link duplex.
 * @retval kStatus_Success   PHY gets status success
 * @retval kStatus_PHY_SMIVisitTimeout  PHY SMI visit time out
 */
static inline status_t PHY_SetLinkSpeedDuplex(phy_handle_t *handle, phy_speed_t speed, phy_duplex_t duplex) {
    return handle->ops->setLinkSpeedDuplex(handle, speed, duplex);
}

/*!
 * @brief Enable PHY loopcback mode.
 *
 * @param handle   PHY device handle.
 * @param mode     The loopback mode to be enabled, please see "phy_loop_t".
 * All loopback modes should not be set together, when one loopback mode is set
 * another should be disabled.
 * @param speed    PHY speed for loopback mode.
 * @param enable   True to enable, false to disable.
 * @retval kStatus_Success   PHY get link speed and duplex success
 * @retval kStatus_PHY_SMIVisitTimeout  PHY SMI visit time out
 */
static inline status_t PHY_EnableLoopback(phy_handle_t *handle, phy_loop_t mode, phy_speed_t speed, bool enable) {
    return handle->ops->enableLoopback(handle, mode, speed, enable);
}

/* @} */

#if defined(__cplusplus)
}
#endif

/*! @}*/
#endif

/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_BEE_H_
#define _FSL_BEE_H_

#include "fsl_common.h"

/*!
 * @addtogroup bee
 * @{
 */

/*******************************************************************************
 * Definitions
 *******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief BEE driver version. Version 2.0.1.
 *
 * Current version: 2.0.1
 *
 * Change log:
 * - Version 2.0.1
 *   - Initial version
 */
#define FSL_BEE_DRIVER_VERSION (MAKE_VERSION(2, 0, 1))
/*@}*/

typedef enum _bee_aes_mode
{
    kBEE_AesEcbMode = 0U, /*!< AES ECB Mode */
    kBEE_AesCtrMode = 1U  /*!< AES CTR Mode */
} bee_aes_mode_t;

typedef enum _bee_region
{
    kBEE_Region0 = 0U, /*!< BEE region 0 */
    kBEE_Region1 = 1U  /*!< BEE region 1 */
} bee_region_t;

typedef enum _bee_ac_prot_enable
{
    kBEE_AccessProtDisabled = 0U, /*!< BEE access permission control disabled */
    kBEE_AccessProtEnabled  = 1U  /*!< BEE access permission control enabled */
} bee_ac_prot_enable;

typedef enum _bee_endian_swap_enable
{
    kBEE_EndianSwapDisabled = 1U, /*!< BEE endian swap disabled */
    kBEE_EndianSwapEnabled  = 0U  /*!< BEE endian swap enabled */
} bee_endian_swap_enable;

typedef enum _bee_security_level
{
    kBEE_SecurityLevel0 = 0U, /*!< BEE security level 0 */
    kBEE_SecurityLevel1 = 1U, /*!< BEE security level 1 */
    kBEE_SecurityLevel2 = 2U, /*!< BEE security level 2 */
    kBEE_SecurityLevel3 = 3U  /*!< BEE security level 3 */
} bee_security_level;

typedef enum _bee_status_flags
{
    kBEE_DisableAbortFlag     = 1U,                      /*!< Disable abort flag. */
    kBEE_Reg0ReadSecViolation = 2U,                      /*!< Region-0 read channel security violation */
    kBEE_ReadIllegalAccess    = 4U,                      /*!< Read channel illegal access detected */
    kBEE_Reg1ReadSecViolation = 8U,                      /*!< Region-1 read channel security violation */
    kBEE_Reg0AccessViolation  = 16U,                     /*!< Protected region-0 access violation */
    kBEE_Reg1AccessViolation  = 32U,                     /*!< Protected region-1 access violation */
    kBEE_IdleFlag             = BEE_STATUS_BEE_IDLE_MASK /*!< Idle flag */
} bee_status_flags_t;

/*! @brief BEE region configuration structure. */
typedef struct _bee_region_config
{
    bee_aes_mode_t region0Mode;          /*!< AES mode used for encryption/decryption for region 0 */
    bee_aes_mode_t region1Mode;          /*!< AES mode used for encryption/decryption for region 1 */
    uint32_t region0AddrOffset;          /*!< Region 0 address offset */
    uint32_t region1AddrOffset;          /*!< Region 1 address offset */
    bee_security_level region0SecLevel;  /*!< Region 0 security level */
    bee_security_level region1SecLevel;  /*!< Region 1 security level */
    uint32_t region1Bot;                 /*!< Region 1 bottom address */
    uint32_t region1Top;                 /*!< Region 1 top address */
    bee_ac_prot_enable accessPermission; /*!< Access permission control enable/disable */
    bee_endian_swap_enable endianSwapEn; /*!< Endian swap enable/disable */
} bee_region_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Resets BEE module to factory default values.
 *
 * This function performs hardware reset of BEE module. Attributes and keys from software for both regions are cleared.
 *
 * @param base BEE peripheral address.
 */
void BEE_Init(BEE_Type *base);

/*!
 * @brief Resets BEE module, clears keys for both regions and disables clock to the BEE.
 *
 * This function performs hardware reset of BEE module and disables clocks. Attributes and keys from software for both
 * regions are cleared.
 *
 * @param base BEE peripheral address.
 */
void BEE_Deinit(BEE_Type *base);

/*!
 * @brief Enables BEE decryption.
 *
 * This function enables decryption using BEE.
 *
 * @param base BEE peripheral address.
 */
static inline void BEE_Enable(BEE_Type *base)
{
    base->CTRL |= BEE_CTRL_BEE_ENABLE_MASK;
}

/*!
 * @brief Disables BEE decryption.
 *
 * This function disables decryption using BEE.
 *
 * @param base BEE peripheral address.
 */
static inline void BEE_Disable(BEE_Type *base)
{
    base->CTRL &= ~BEE_CTRL_BEE_ENABLE_MASK;
}

/*!
 * @brief Loads default values to the BEE region configuration structure.
 *
 * Loads default values to the BEE region configuration structure. The default values are as follows:
 * @code
 *   config->region0Mode = kBEE_AesCtrMode;
 *   config->region1Mode = kBEE_AesCtrMode;
 *   config->region0AddrOffset = 0U;
 *   config->region1AddrOffset = 0U;
 *   config->region0SecLevel = kBEE_SecurityLevel3;
 *   config->region1SecLevel = kBEE_SecurityLevel3;
 *   config->region1Bot = 0U;
 *   config->region1Top = 0U;
 *   config->accessPermission = kBEE_AccessProtDisabled;
 *   config->endianSwapEn = kBEE_EndianSwapEnabled;
 * @endcode
 *
 * @param config Configuration structure for BEE peripheral.
 */
void BEE_GetDefaultConfig(bee_region_config_t *config);

/*!
 * @brief Sets BEE configuration.
 *
 * This function sets BEE peripheral and BEE region settings accorging to given configuration structure.
 *
 * @param base BEE peripheral address.
 * @param config Configuration structure for BEE.
 */
void BEE_SetConfig(BEE_Type *base, const bee_region_config_t *config);

/*!
 * @brief Loads the AES key for selected region into BEE key registers.
 *
 * This function loads given AES key to BEE register for the given region.
 * The key must be 32-bit aligned and stored in little-endian format.
 *
 * Please note, that eFuse BEE_KEYx_SEL must be set accordingly to be able to load and use key loaded in BEE registers.
 * Otherwise, key cannot loaded and BEE will use key from OTPMK or SW_GP2.
 *
 * @param base BEE peripheral address.
 * @param region Selection of the BEE region to be configured.
 * @param key AES key (in little-endian format).
 * @param keySize Size of AES key.
 */
status_t BEE_SetRegionKey(BEE_Type *base, bee_region_t region, const uint8_t *key, size_t keySize);

/*!
 * @brief Loads the nonce for selected region into BEE nonce registers.
 *
 * This function loads given nonce(only AES CTR mode) to BEE register for the given region.
 * The nonce must be 32-bit aligned and stored in little-endian format.
 *
 * @param base BEE peripheral address.
 * @param region Selection of the BEE region to be configured.
 * @param nonce AES nonce (in little-endian format).
 * @param nonceSize Size of AES nonce.
 */
status_t BEE_SetRegionNonce(BEE_Type *base, bee_region_t region, const uint8_t *nonce, size_t nonceSize);

/*!
 * @brief Gets the BEE status flags.
 *
 * This function returns status of BEE peripheral.
 *
 * @param base BEE peripheral address.
 *
 * @return The status flags. This is the logical OR of members of the
 *         enumeration ::bee_status_flags_t
 */
uint32_t BEE_GetStatusFlags(BEE_Type *base);

/*!
 * @brief Clears the BEE status flags.
 *
 * @param base BEE peripheral base address.
 * @param mask The status flags to clear. This is a logical OR of members of the
 *             enumeration ::bee_status_flags_t
 */
void BEE_ClearStatusFlags(BEE_Type *base, uint32_t mask);

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_BEE_H_ */

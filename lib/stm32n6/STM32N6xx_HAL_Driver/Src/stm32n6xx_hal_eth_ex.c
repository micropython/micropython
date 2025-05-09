/**
  ******************************************************************************
  * @file    stm32n6xx_hal_eth_ex.c
  * @author  MCD Application Team
  * @brief   ETH HAL Extended module driver.
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

#ifdef HAL_ETH_MODULE_ENABLED

#if defined(ETH1)

/** @defgroup ETHEx ETHEx
  * @brief ETH HAL Extended module driver
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup ETHEx_Private_Constants ETHEx Private Constants
  * @{
  */
#define ETH_MACL4CR_MASK     (ETH_MACL3L4C0R_L4PEN0 | ETH_MACL3L4C0R_L4SPM0 | \
                              ETH_MACL3L4C0R_L4SPIM0 | ETH_MACL3L4C0R_L4DPM0 | \
                              ETH_MACL3L4C0R_L4DPIM0)

#define ETH_MACL3CR_MASK     (ETH_MACL3L4C0R_L3PEN0 | ETH_MACL3L4C0R_L3SAM0 | \
                              ETH_MACL3L4C0R_L3SAIM0 | ETH_MACL3L4C0R_L3DAM0 | \
                              ETH_MACL3L4C0R_L3DAIM0 | ETH_MACL3L4C0R_L3HSBM0 | \
                              ETH_MACL3L4C0R_L3HDBM0)


#define ETH_MACRXVLAN_MASK (ETH_MACVTCR_EIVLRXS | ETH_MACVTCR_EIVLS | \
                            ETH_MACVTCR_ERIVLT | ETH_MACVTCR_EDVLP | \
                            ETH_MACVTCR_VTHM | ETH_MACVTCR_EVLRXS | \
                            ETH_MACVTCR_EVLS | ETH_MACVTCR_DOVLTC | \
                            ETH_MACVTCR_ERSVLM | ETH_MACVTCR_ESVL | \
                            ETH_MACVTCR_VTIM | ETH_MACVTCR_ETV)

#define ETH_MACTXVLAN_MASK (ETH_MACVIR_VLTI | ETH_MACVIR_CSVL | \
                            ETH_MACVIR_VLP | ETH_MACVIR_VLC)

#define ETH_MACRXQC1R_MASK            0x37F77877U
#define ETH_MACRXQCR_MASK             0x00030303U
#define ETH_MTLOMR_MASK               0x00000366U
#define ETH_MTLTXQxOMR_MASK           0x000F007FU
#define ETH_MTLRXQxOMR_MASK           0x00F1C7FBU
#define ETH_MACRXQC2R_PSRQ_MASK       0x0000FFFFU
#define ETH_MAC_TMRQR_MASK            0x0017FFFFU
#define ETH_MAC_IACR_MASK             0x0000FF03U

#define ETH_OP_BUSY_TIMEOUT           0x000000FFU

#define ETH_MAC_L4_SRSP_MASK          0x0000FFFFU
#define ETH_MAC_L4_DSTP_MASK          0xFFFF0000U
#ifdef HAL_ETH_USE_TAS
#define ETH_MTLESTCR_MASK             0xFFFFF7F2U
/* Maximum Width of the time interval in Gate Control List */
#define ETH_EST_WID_MAX               (24U)
/* Maximum depth of Gate Control List */
#define ETH_EST_DEP_MAX               (64U)
#endif /* HAL_ETH_USE_TAS */
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions ---------------------------------------------------------*/
/** @defgroup ETHEx_Exported_Functions ETH Extended Exported Functions
  * @{
  */

/** @defgroup ETHEx_Exported_Functions_Group1 Extended features functions
  * @brief    Extended features functions
  *
@verbatim
 ===============================================================================
                      ##### Extended features functions #####
 ===============================================================================
    [..] This section provides functions allowing to:
      (+) Configure ARP offload module
      (+) Configure L3 and L4 filters
      (+) Configure Extended VLAN features
      (+) Configure Energy Efficient Ethernet module
      (+) Configure the Ethernet MTL after ETH peripheral initialization
      (+) HAL_ETHEx_GetMTLConfig(): Get MTL actual configuration into ETH_MTLConfigTypeDef
      (+) HAL_ETHEx_SetMTLConfig(): Set MTL configuration based on ETH_MTLConfigTypeDef
      (+) HAL_ETHEx_SetMACMTLMappingConfig(): Set MAC MTL Mapping configuration based on ETH_MACMTLMappingTypeDef
      (+) HAL_ETHEx_GetMACMTLMappingConfig(): Get MAC MTL Mapping configuration based on ETH_MACMTLMappingTypeDef
      (+) HAL_ETHEx_SetUserTagPriorityQueue(): Set User Tag Priority Queue
      (+) HAL_ETHEx_GetUserTagPriorityQueue(): Get User Tag Priority Queue
      (+) HAL_ETHEx_SetPacketTypeQueue() : Set Packet Type Queueing
      (+) HAL_ETHEx_GetPacketTypeQueue() : Get Packet Type Queueing
      (+) HAL_ETHEx_EnableCBS() : Enable Qav "Credit Based Sahper" feature
      (+) HAL_ETHEx_SetCBSConfig() : Set Credit Based Shaper parameters
      (+) HAL_ETHEx_GetCBSConfig() : Get Credit Based Shaper parameters
      (+) HAL_ETHEx_GetGCLDepth() : Get the GCL List depth
      (+) HAL_ETHEx_GetGCLWidthTimeInterval() : Get the GCL Width Time interval
      (+) HAL_ETHEx_EnableEST() : Enable Qbv feature
      (+) HAL_ETHEx_DisableEST() : Disable Qbv "Enhancement Scheduled Traffic" feature
      (+) HAL_ETHEx_SetESTConfig() : Set Qbv parameters
      (+) HAL_ETHEx_SetGCLRegisters() : Set Gate Control List registers
      (+) HAL_ETHEx_SetGCLConfig() : Set Gate Control List configuration
      (+) HAL_ETHEx_GetGCLRegisters() : Get Gate Control List configuration
      (+) HAL_ETHEx_EnableFPE() : Enable Qbu "Frame Preemption" feature
      (+) HAL_ETHEx_DisableFPE() : Disable Qbu "Frame Preemption" feature
      (+) HAL_ETHEx_GetFPEConfig() : Get Frame Preemption configuration
      (+) HAL_ETHEx_SetFPEConfig() : Set Frame Preemption configuration

@endverbatim
  * @{
  */

/**
  * @brief  Enables ARP Offload.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval None
  */

void HAL_ETHEx_EnableARPOffload(ETH_HandleTypeDef *heth)
{
  SET_BIT(heth->Instance->MACCR, ETH_MACCR_ARP);
}

/**
  * @brief  Disables ARP Offload.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval None
  */
void HAL_ETHEx_DisableARPOffload(ETH_HandleTypeDef *heth)
{
  CLEAR_BIT(heth->Instance->MACCR, ETH_MACCR_ARP);
}

/**
  * @brief  Set the ARP Match IP address
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  IpAddress: IP Address to be matched for incoming ARP requests
  * @retval None
  */
void HAL_ETHEx_SetARPAddressMatch(ETH_HandleTypeDef *heth, uint32_t IpAddress)
{
  WRITE_REG(heth->Instance->MACARPAR, IpAddress);
}

/**
  * @brief  Configures the L4 Filter, this function allow to:
  *         set the layer 4 protocol to be matched (TCP or UDP)
  *         enable/disable L4 source/destination port perfect/inverse match.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  Filter: L4 filter to configured, this parameter must be one of the following
  *           ETH_L4_FILTER_0
  *           ETH_L4_FILTER_1
  * @param  pL4FilterConfig: pointer to a ETH_L4FilterConfigTypeDef structure
  *         that contains L4 filter configuration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_SetL4FilterConfig(ETH_HandleTypeDef *heth, uint32_t Filter,
                                              const ETH_L4FilterConfigTypeDef *pL4FilterConfig)
{
  if (pL4FilterConfig == NULL)
  {
    return HAL_ERROR;
  }

  if (Filter == ETH_L4_FILTER_0)
  {
    /* Write configuration to MACL3L4C0R register */
    MODIFY_REG(heth->Instance->MACL3L4C0R, ETH_MACL4CR_MASK, (pL4FilterConfig->Protocol |
                                                              pL4FilterConfig->SrcPortFilterMatch |
                                                              pL4FilterConfig->DestPortFilterMatch));

    /* Write configuration to MACL4A0R register */
    WRITE_REG(heth->Instance->MACL4A0R, (pL4FilterConfig->SourcePort | (pL4FilterConfig->DestinationPort << 16)));

  }
  else /* Filter == ETH_L4_FILTER_1 */
  {
    /* Write configuration to MACL3L4C1R register */
    MODIFY_REG(heth->Instance->MACL3L4C1R, ETH_MACL4CR_MASK, (pL4FilterConfig->Protocol |
                                                              pL4FilterConfig->SrcPortFilterMatch |
                                                              pL4FilterConfig->DestPortFilterMatch));

    /* Write configuration to MACL4A1R register */
    WRITE_REG(heth->Instance->MACL4A1R, (pL4FilterConfig->SourcePort | (pL4FilterConfig->DestinationPort << 16)));
  }

  /* Enable L4 filter */
  SET_BIT(heth->Instance->MACPFR, ETH_MACPFR_IPFE);

  return HAL_OK;
}

/**
  * @brief  Configures the L4 Filter, this function allow to:
  *         set the layer 4 protocol to be matched (TCP or UDP)
  *         enable/disable L4 source/destination port perfect/inverse match.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  Filter: L4 filter to configured, this parameter must be one of the following
  *           ETH_L4_FILTER_0
  *           ETH_L4_FILTER_1
  * @param  pL4FilterConfig: pointer to a ETH_L4FilterConfigTypeDef structure
  *         that contains L4 filter configuration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_GetL4FilterConfig(const ETH_HandleTypeDef *heth, uint32_t Filter,
                                              ETH_L4FilterConfigTypeDef *pL4FilterConfig)
{
  if (pL4FilterConfig == NULL)
  {
    return HAL_ERROR;
  }

  if (Filter == ETH_L4_FILTER_0)
  {
    /* Get configuration from MACL3L4C0R register */
    pL4FilterConfig->Protocol = READ_BIT(heth->Instance->MACL3L4C0R, ETH_MACL3L4C0R_L4PEN0);
    pL4FilterConfig->DestPortFilterMatch = READ_BIT(heth->Instance->MACL3L4C0R,
                                                    (ETH_MACL3L4C0R_L4DPM0 | ETH_MACL3L4C0R_L4DPIM0));
    pL4FilterConfig->SrcPortFilterMatch = READ_BIT(heth->Instance->MACL3L4C0R,
                                                   (ETH_MACL3L4C0R_L4SPM0 | ETH_MACL3L4C0R_L4SPIM0));

    /* Get configuration from MACL4A0R register */
    pL4FilterConfig->DestinationPort = (READ_BIT(heth->Instance->MACL4A0R, ETH_MAC_L4_DSTP_MASK) >> 16);
    pL4FilterConfig->SourcePort = READ_BIT(heth->Instance->MACL4A0R, ETH_MAC_L4_SRSP_MASK);
  }
  else /* Filter == ETH_L4_FILTER_1 */
  {
    /* Get configuration from MACL3L4C1R register */
    pL4FilterConfig->Protocol = READ_BIT(heth->Instance->MACL3L4C1R, ETH_MACL3L4C0R_L4PEN0);
    pL4FilterConfig->DestPortFilterMatch = READ_BIT(heth->Instance->MACL3L4C1R,
                                                    (ETH_MACL3L4C0R_L4DPM0 | ETH_MACL3L4C0R_L4DPIM0));
    pL4FilterConfig->SrcPortFilterMatch = READ_BIT(heth->Instance->MACL3L4C1R,
                                                   (ETH_MACL3L4C0R_L4SPM0 | ETH_MACL3L4C0R_L4SPIM0));

    /* Get configuration from MACL4A1R register */
    pL4FilterConfig->DestinationPort = (READ_BIT(heth->Instance->MACL4A1R, ETH_MAC_L4_DSTP_MASK) >> 16);
    pL4FilterConfig->SourcePort = READ_BIT(heth->Instance->MACL4A1R, ETH_MAC_L4_SRSP_MASK);
  }

  return HAL_OK;
}

/**
  * @brief  Configures the L3 Filter, this function allow to:
  *         set the layer 3 protocol to be matched (IPv4 or IPv6)
  *         enable/disable L3 source/destination port perfect/inverse match.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  Filter: L3 filter to configured, this parameter must be one of the following
  *           ETH_L3_FILTER_0
  *           ETH_L3_FILTER_1
  * @param  pL3FilterConfig: pointer to a ETH_L3FilterConfigTypeDef structure
  *         that contains L3 filter configuration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_SetL3FilterConfig(ETH_HandleTypeDef *heth, uint32_t Filter,
                                              const ETH_L3FilterConfigTypeDef *pL3FilterConfig)
{
  if (pL3FilterConfig == NULL)
  {
    return HAL_ERROR;
  }

  if (Filter == ETH_L3_FILTER_0)
  {
    /* Write configuration to MACL3L4C0R register */
    MODIFY_REG(heth->Instance->MACL3L4C0R, ETH_MACL3CR_MASK, (pL3FilterConfig->Protocol |
                                                              pL3FilterConfig->SrcAddrFilterMatch |
                                                              pL3FilterConfig->DestAddrFilterMatch |
                                                              (pL3FilterConfig->SrcAddrHigherBitsMatch << 6) |
                                                              (pL3FilterConfig->DestAddrHigherBitsMatch << 11)));
  }
  else  /* Filter == ETH_L3_FILTER_1 */
  {
    /* Write configuration to MACL3L4C1R register */
    MODIFY_REG(heth->Instance->MACL3L4C1R, ETH_MACL3CR_MASK, (pL3FilterConfig->Protocol |
                                                              pL3FilterConfig->SrcAddrFilterMatch |
                                                              pL3FilterConfig->DestAddrFilterMatch |
                                                              (pL3FilterConfig->SrcAddrHigherBitsMatch << 6) |
                                                              (pL3FilterConfig->DestAddrHigherBitsMatch << 11)));
  }

  if (Filter == ETH_L3_FILTER_0)
  {
    /* Check if IPv6 protocol is selected */
    if (pL3FilterConfig->Protocol != ETH_L3_IPV4_MATCH)
    {
      /* Set the IPv6 address match */
      /* Set Bits[31:0] of 128-bit IP addr */
      WRITE_REG(heth->Instance->MACL3A0R0R, pL3FilterConfig->Ip6Addr[0]);
      /* Set Bits[63:32] of 128-bit IP addr */
      WRITE_REG(heth->Instance->MACL3A1R0R, pL3FilterConfig->Ip6Addr[1]);
      /* update Bits[95:64] of 128-bit IP addr */
      WRITE_REG(heth->Instance->MACL3A2R0R, pL3FilterConfig->Ip6Addr[2]);
      /* update Bits[127:96] of 128-bit IP addr */
      WRITE_REG(heth->Instance->MACL3A3R0R, pL3FilterConfig->Ip6Addr[3]);
    }
    else /* IPv4 protocol is selected */
    {
      /* Set the IPv4 source address match */
      WRITE_REG(heth->Instance->MACL3A0R0R, pL3FilterConfig->Ip4SrcAddr);
      /* Set the IPv4 destination address match */
      WRITE_REG(heth->Instance->MACL3A1R0R, pL3FilterConfig->Ip4DestAddr);
    }
  }
  else  /* Filter == ETH_L3_FILTER_1 */
  {
    /* Check if IPv6 protocol is selected */
    if (pL3FilterConfig->Protocol != ETH_L3_IPV4_MATCH)
    {
      /* Set the IPv6 address match */
      /* Set Bits[31:0] of 128-bit IP addr */
      WRITE_REG(heth->Instance->MACL3A01R, pL3FilterConfig->Ip6Addr[0]);
      /* Set Bits[63:32] of 128-bit IP addr */
      WRITE_REG(heth->Instance->MACL3A11R, pL3FilterConfig->Ip6Addr[1]);
      /* update Bits[95:64] of 128-bit IP addr */
      WRITE_REG(heth->Instance->MACL3A21R, pL3FilterConfig->Ip6Addr[2]);
      /* update Bits[127:96] of 128-bit IP addr */
      WRITE_REG(heth->Instance->MACL3A31R, pL3FilterConfig->Ip6Addr[3]);
    }
    else /* IPv4 protocol is selected */
    {
      /* Set the IPv4 source address match */
      WRITE_REG(heth->Instance->MACL3A01R, pL3FilterConfig->Ip4SrcAddr);
      /* Set the IPv4 destination address match */
      WRITE_REG(heth->Instance->MACL3A11R, pL3FilterConfig->Ip4DestAddr);

    }
  }

  /* Enable L3 filter */
  SET_BIT(heth->Instance->MACPFR, ETH_MACPFR_IPFE);

  return HAL_OK;
}

/**
  * @brief  Configures the L3 Filter, this function allow to:
  *         set the layer 3 protocol to be matched (IPv4 or IPv6)
  *         enable/disable L3 source/destination port perfect/inverse match.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  Filter: L3 filter to configured, this parameter must be one of the following
  *           ETH_L3_FILTER_0
  *           ETH_L3_FILTER_1
  * @param  pL3FilterConfig: pointer to a ETH_L3FilterConfigTypeDef structure
  *         that will contain the L3 filter configuration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_GetL3FilterConfig(const ETH_HandleTypeDef *heth, uint32_t Filter,
                                              ETH_L3FilterConfigTypeDef *pL3FilterConfig)
{
  if (pL3FilterConfig == NULL)
  {
    return HAL_ERROR;
  }
  pL3FilterConfig->Protocol = READ_BIT(*((__IO uint32_t *)(&(heth->Instance->MACL3L4C0R) + Filter)),
                                       ETH_MACL3L4C0R_L3PEN0);
  pL3FilterConfig->SrcAddrFilterMatch = READ_BIT(*((__IO uint32_t *)(&(heth->Instance->MACL3L4C0R) + Filter)),
                                                 (ETH_MACL3L4C0R_L3SAM0 | ETH_MACL3L4C0R_L3SAIM0));
  pL3FilterConfig->DestAddrFilterMatch = READ_BIT(*((__IO uint32_t *)(&(heth->Instance->MACL3L4C0R) + Filter)),
                                                  (ETH_MACL3L4C0R_L3DAM0 | ETH_MACL3L4C0R_L3DAIM0));
  pL3FilterConfig->SrcAddrHigherBitsMatch = (READ_BIT(*((__IO uint32_t *)(&(heth->Instance->MACL3L4C0R) + Filter)),
                                                      ETH_MACL3L4C0R_L3HSBM0) >> 6);
  pL3FilterConfig->DestAddrHigherBitsMatch = (READ_BIT(*((__IO uint32_t *)(&(heth->Instance->MACL3L4C0R) + Filter)),
                                                       ETH_MACL3L4C0R_L3HDBM0) >> 11);

  if (Filter == ETH_L3_FILTER_0)
  {
    if (pL3FilterConfig->Protocol != ETH_L3_IPV4_MATCH)
    {
      WRITE_REG(pL3FilterConfig->Ip6Addr[0], heth->Instance->MACL3A0R0R);
      WRITE_REG(pL3FilterConfig->Ip6Addr[1], heth->Instance->MACL3A1R0R);
      WRITE_REG(pL3FilterConfig->Ip6Addr[2], heth->Instance->MACL3A2R0R);
      WRITE_REG(pL3FilterConfig->Ip6Addr[3], heth->Instance->MACL3A3R0R);
    }
    else
    {
      WRITE_REG(pL3FilterConfig->Ip4SrcAddr, heth->Instance->MACL3A0R0R);
      WRITE_REG(pL3FilterConfig->Ip4DestAddr, heth->Instance->MACL3A1R0R);
    }
  }
  else /* ETH_L3_FILTER_1 */
  {
    if (pL3FilterConfig->Protocol != ETH_L3_IPV4_MATCH)
    {
      WRITE_REG(pL3FilterConfig->Ip6Addr[0], heth->Instance->MACL3A01R);
      WRITE_REG(pL3FilterConfig->Ip6Addr[1], heth->Instance->MACL3A11R);
      WRITE_REG(pL3FilterConfig->Ip6Addr[2], heth->Instance->MACL3A21R);
      WRITE_REG(pL3FilterConfig->Ip6Addr[3], heth->Instance->MACL3A31R);
    }
    else
    {
      WRITE_REG(pL3FilterConfig->Ip4SrcAddr, heth->Instance->MACL3A01R);
      WRITE_REG(pL3FilterConfig->Ip4DestAddr, heth->Instance->MACL3A11R);
    }
  }

  return HAL_OK;
}

/**
  * @brief  Enables L3 and L4 filtering process.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval None.
  */
void HAL_ETHEx_EnableL3L4Filtering(ETH_HandleTypeDef *heth)
{
  /* Enable L3/L4 filter */
  SET_BIT(heth->Instance->MACPFR, ETH_MACPFR_IPFE);
}

/**
  * @brief  Disables L3 and L4 filtering process.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval None.
  */
void HAL_ETHEx_DisableL3L4Filtering(ETH_HandleTypeDef *heth)
{
  /* Disable L3/L4 filter */
  CLEAR_BIT(heth->Instance->MACPFR, ETH_MACPFR_IPFE);
}

/**
  * @brief  Get the VLAN Configuration for Receive Packets.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  pVlanConfig: pointer to a ETH_RxVLANConfigTypeDef structure
  *         that will contain the VLAN filter configuration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_GetRxVLANConfig(const ETH_HandleTypeDef *heth, ETH_RxVLANConfigTypeDef *pVlanConfig)
{
  if (pVlanConfig == NULL)
  {
    return HAL_ERROR;
  }

  pVlanConfig->InnerVLANTagInStatus = ((READ_BIT(heth->Instance->MACVTCR,
                                                 ETH_MACVTCR_EIVLRXS) >> 31) == 0U) ? DISABLE : ENABLE;
  pVlanConfig->StripInnerVLANTag  = READ_BIT(heth->Instance->MACVTCR, ETH_MACVTCR_EIVLS);
  pVlanConfig->InnerVLANTag = ((READ_BIT(heth->Instance->MACVTCR, ETH_MACVTCR_ERIVLT) >> 27) == 0U) ? DISABLE : ENABLE;
  pVlanConfig->DoubleVLANProcessing = ((READ_BIT(heth->Instance->MACVTCR,
                                                 ETH_MACVTCR_EDVLP) >> 26) == 0U) ? DISABLE : ENABLE;
  pVlanConfig->VLANTagHashTableMatch = ((READ_BIT(heth->Instance->MACVTCR,
                                                  ETH_MACVTCR_VTHM) >> 25) == 0U) ? DISABLE : ENABLE;
  pVlanConfig->VLANTagInStatus = ((READ_BIT(heth->Instance->MACVTCR, ETH_MACVTCR_EVLRXS) >> 24) == 0U)
                                 ? DISABLE : ENABLE;
  pVlanConfig->StripVLANTag = READ_BIT(heth->Instance->MACVTCR, ETH_MACVTCR_EVLS);
  pVlanConfig->VLANTypeCheck = READ_BIT(heth->Instance->MACVTCR,
                                        (ETH_MACVTCR_DOVLTC | ETH_MACVTCR_ERSVLM | ETH_MACVTCR_ESVL));
  pVlanConfig->VLANTagInverseMatch = ((READ_BIT(heth->Instance->MACVTCR, ETH_MACVTCR_VTIM) >> 17) == 0U)
                                     ? DISABLE : ENABLE;
  pVlanConfig->BitVLANcomparison = ((READ_BIT(heth->Instance->MACVTCR, ETH_MACVTCR_ETV) >> 16) == 0U)
                                   ? DISABLE : ENABLE;
  pVlanConfig->VLANTagEnable = ((READ_BIT(heth->Instance->MACVTDR, ETH_MACVTDR_VEN) >> 16) == 0U)
                               ? DISABLE : ENABLE;
  pVlanConfig->VLANcomparison = READ_BIT(heth->Instance->MACVTDR, ETH_MACVTDR_ETV);

  pVlanConfig->DMAChannelNumberEnable = ((READ_BIT(heth->Instance->MACVTDR, ETH_MACVTDR_DMACHEN) >> 24) == 0U)
                                        ? DISABLE : ENABLE;
  pVlanConfig->RxDMAChannelNumber = READ_BIT(heth->Instance->MACVTDR, ETH_MACVTDR_DMACHN);

  pVlanConfig->VLANTagID = READ_BIT(heth->Instance->MACVTDR, ETH_MACVTDR_VID);

  return HAL_OK;
}

/**
  * @brief  Set the VLAN Configuration for Receive Packets.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  pVlanConfig: pointer to a ETH_RxVLANConfigTypeDef structure
  *         that contains VLAN filter configuration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_SetRxVLANConfig(ETH_HandleTypeDef *heth, ETH_RxVLANConfigTypeDef *pVlanConfig)
{
  if (pVlanConfig == NULL)
  {
    return HAL_ERROR;
  }

  /* Write config to MACVTCR */
  MODIFY_REG(heth->Instance->MACVTCR, ETH_MACRXVLAN_MASK, (((uint32_t)pVlanConfig->InnerVLANTagInStatus << 31) |
                                                           pVlanConfig->StripInnerVLANTag |
                                                           ((uint32_t)pVlanConfig->InnerVLANTag << 27) |
                                                           ((uint32_t)pVlanConfig->DoubleVLANProcessing << 26) |
                                                           ((uint32_t)pVlanConfig->VLANTagHashTableMatch << 25) |
                                                           ((uint32_t)pVlanConfig->VLANTagInStatus << 24) |
                                                           pVlanConfig->StripVLANTag |
                                                           pVlanConfig->VLANTypeCheck |
                                                           ((uint32_t)pVlanConfig->VLANTagInverseMatch << 17) |
                                                           ((uint32_t)pVlanConfig->BitVLANcomparison << 16)));
  /* Write config to MACVTDR */
  MODIFY_REG(heth->Instance->MACVTDR, ETH_MACRXVLAN_MASK, (((uint32_t)pVlanConfig->VLANTagEnable << 16) |
                                                           ((uint32_t)pVlanConfig->VLANcomparison << 17) |
                                                           ((uint32_t)pVlanConfig->DMAChannelNumberEnable << 24) |
                                                           (pVlanConfig->RxDMAChannelNumber << 25)));

  HAL_ETH_SetRxVLANIdentifier(heth, (uint32_t)pVlanConfig->VLANcomparison, pVlanConfig->VLANTagID);

  return HAL_OK;
}

/**
  * @brief  Set the VLAN Hash Table
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  VLANHashTable: VLAN hash table 16 bit value
  * @retval None
  */
void HAL_ETHEx_SetVLANHashTable(ETH_HandleTypeDef *heth, uint32_t VLANHashTable)
{
  MODIFY_REG(heth->Instance->MACVHTR, ETH_MACVHTR_VLHT, VLANHashTable);
}

/**
  * @brief  Get the VLAN Configuration for Transmit Packets.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  VLANTag: Selects the vlan tag, this parameter must be one of the following
  *                 ETH_OUTER_TX_VLANTAG
  *                 ETH_INNER_TX_VLANTAG
  * @param  pVlanConfig: pointer to a ETH_TxVLANConfigTypeDef structure
  *         that will contain the Tx VLAN filter configuration.
  * @retval HAL Status.
  */
HAL_StatusTypeDef HAL_ETHEx_GetTxVLANConfig(const ETH_HandleTypeDef *heth, uint32_t VLANTag,
                                            ETH_TxVLANConfigTypeDef *pVlanConfig)
{
  if (pVlanConfig == NULL)
  {
    return HAL_ERROR;
  }

  if (VLANTag == ETH_INNER_TX_VLANTAG)
  {
    pVlanConfig->SourceTxDesc = ((READ_BIT(heth->Instance->MACIVIR, ETH_MACVIR_VLTI) >> 20) == 0U) ? DISABLE : ENABLE;
    pVlanConfig->SVLANType = ((READ_BIT(heth->Instance->MACIVIR, ETH_MACVIR_CSVL) >> 19) == 0U) ? DISABLE : ENABLE;
    pVlanConfig->VLANTagControl = READ_BIT(heth->Instance->MACIVIR, (ETH_MACVIR_VLP | ETH_MACVIR_VLC));
  }
  else
  {
    pVlanConfig->SourceTxDesc = ((READ_BIT(heth->Instance->MACVIR, ETH_MACVIR_VLTI) >> 20) == 0U) ? DISABLE : ENABLE;
    pVlanConfig->SVLANType = ((READ_BIT(heth->Instance->MACVIR, ETH_MACVIR_CSVL) >> 19) == 0U) ? DISABLE : ENABLE;
    pVlanConfig->VLANTagControl = READ_BIT(heth->Instance->MACVIR, (ETH_MACVIR_VLP | ETH_MACVIR_VLC));
  }

  return HAL_OK;;
}

/**
  * @brief  Set the VLAN Configuration for Transmit Packets.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  VLANTag: Selects the vlan tag, this parameter must be one of the following
  *                 ETH_OUTER_TX_VLANTAG
  *                 ETH_INNER_TX_VLANTAG
  * @param  pVlanConfig: pointer to a ETH_TxVLANConfigTypeDef structure
  *         that contains Tx VLAN filter configuration.
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_ETHEx_SetTxVLANConfig(ETH_HandleTypeDef *heth, uint32_t VLANTag,
                                            const ETH_TxVLANConfigTypeDef *pVlanConfig)
{
  if (VLANTag == ETH_INNER_TX_VLANTAG)
  {
    MODIFY_REG(heth->Instance->MACIVIR, ETH_MACTXVLAN_MASK, (((uint32_t)pVlanConfig->SourceTxDesc << 20) |
                                                             ((uint32_t)pVlanConfig->SVLANType << 19) |
                                                             pVlanConfig->VLANTagControl));
    /* Enable Double VLAN processing */
    SET_BIT(heth->Instance->MACVTCR, ETH_MACVTCR_EDVLP);
  }
  else
  {
    MODIFY_REG(heth->Instance->MACVIR, ETH_MACTXVLAN_MASK, (((uint32_t)pVlanConfig->SourceTxDesc << 20) |
                                                            ((uint32_t)pVlanConfig->SVLANType << 19) |
                                                            pVlanConfig->VLANTagControl));
  }

  return HAL_OK;
}

/**
  * @brief  Set the VLAN Tag Identifier for Transmit Packets.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  VLANTag: Selects the vlan tag, this parameter must be one of the following
  *                 ETH_OUTER_TX_VLANTAG
  *                 ETH_INNER_TX_VLANTAG
  * @param  VLANIdentifier: VLAN Identifier 16 bit value
  * @retval None
  */
void HAL_ETHEx_SetTxVLANIdentifier(ETH_HandleTypeDef *heth, uint32_t VLANTag, uint32_t VLANIdentifier)
{
  if (VLANTag == ETH_INNER_TX_VLANTAG)
  {
    MODIFY_REG(heth->Instance->MACIVIR, ETH_MACVIR_VLT, VLANIdentifier);
  }
  else
  {
    MODIFY_REG(heth->Instance->MACVIR, ETH_MACVIR_VLT, VLANIdentifier);
  }
}

/**
  * @brief  Enables the VLAN Tag Filtering process.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval None.
  */
void HAL_ETHEx_EnableVLANProcessing(ETH_HandleTypeDef *heth)
{
  /* Enable VLAN processing */
  SET_BIT(heth->Instance->MACPFR, ETH_MACPFR_VTFE);
}

/**
  * @brief  Disables the VLAN Tag Filtering process.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval None.
  */
void HAL_ETHEx_DisableVLANProcessing(ETH_HandleTypeDef *heth)
{
  /* Disable VLAN processing */
  CLEAR_BIT(heth->Instance->MACPFR, ETH_MACPFR_VTFE);
}

/**
  * @brief  Enters the Low Power Idle (LPI) mode
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  TxAutomate: Enable/Disable automate enter/exit LPI mode.
  * @param  TxClockStop: Enable/Disable Tx clock stop in LPI mode.
  * @retval None
  */
void HAL_ETHEx_EnterLPIMode(ETH_HandleTypeDef *heth, FunctionalState TxAutomate, FunctionalState TxClockStop)
{
  /* Enable LPI Interrupts */
  __HAL_ETH_MAC_ENABLE_IT(heth, ETH_MACIER_LPIIE);

  /* Write to LPI Control register: Enter low power mode */
  MODIFY_REG(heth->Instance->MACLCSR, (ETH_MACLCSR_LPIEN | ETH_MACLCSR_LPITXA | ETH_MACLCSR_LPITCSE),
             (((uint32_t)TxAutomate << 19) |
              ((uint32_t)TxClockStop << 21) |
              ETH_MACLCSR_LPIEN));
}

/**
  * @brief  Exits the Low Power Idle (LPI) mode.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval None
  */
void HAL_ETHEx_ExitLPIMode(ETH_HandleTypeDef *heth)
{
  /* Clear the LPI Config and exit low power mode */
  CLEAR_BIT(heth->Instance->MACLCSR, (ETH_MACLCSR_LPIEN | ETH_MACLCSR_LPITXA | ETH_MACLCSR_LPITCSE));

  /* Enable LPI Interrupts */
  __HAL_ETH_MAC_DISABLE_IT(heth, ETH_MACIER_LPIIE);
}

/**
  * @brief  Returns the ETH MAC LPI event
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval ETH MAC WakeUp event
  */
uint32_t HAL_ETHEx_GetMACLPIEvent(const ETH_HandleTypeDef *heth)
{
  return heth->MACLPIEvent;
}

/**
  * @brief  Returns the ETH DMA Receive Channels Number Available
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval ETH DMA Rx Channels Number
  */
uint32_t HAL_ETHEx_GetRxDMAChNumber(const ETH_HandleTypeDef *heth)
{
  uint32_t nbr;
  nbr = (READ_BIT(heth->Instance->MACHWF2R, ETH_MACHWF2R_RXCHCNT) >> 12) + 1U;
  return nbr;
}

/**
  * @brief  Returns the ETH DMA Transmit Channels Number
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval ETH DMA Tx Channels Number
  */
uint32_t HAL_ETHEx_GetTxDMAChNumber(const ETH_HandleTypeDef *heth)
{
  uint32_t nbr;
  nbr = (READ_BIT(heth->Instance->MACHWF2R, ETH_MACHWF2R_TXCHCNT) >> 18) + 1U;
  return nbr;
}

/**
  * @brief  Returns the ETH MTL Receive Queues Number
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval ETH MTL Rx Queues Number
  */
uint32_t HAL_ETHEx_GetRxMTLQNumber(const ETH_HandleTypeDef *heth)
{
  uint32_t nbr;
  nbr = READ_BIT(heth->Instance->MACHWF2R, ETH_MACHWF2R_RXQCNT) + 1U;
  return nbr;
}

/**
  * @brief  Returns the ETH MTL Transmit Queues Number
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval ETH MTL Tx Queues Number
  */
uint32_t HAL_ETHEx_GetTxMTLQNumber(const ETH_HandleTypeDef *heth)
{
  uint32_t nbr;
  nbr = (READ_BIT(heth->Instance->MACHWF2R, ETH_MACHWF2R_TXQCNT) >> 6) + 1U;
  return nbr;
}

/**
  * @brief  Get the configuration of the MTL.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  mtlconf: pointer to a ETH_MACConfigTypeDef structure that will hold
  *         the configuration of the MTL.
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_ETHEx_GetMTLConfig(const ETH_HandleTypeDef *heth, ETH_MTLConfigTypeDef *mtlconf)
{
  uint32_t queue;

  if (mtlconf == NULL)
  {
    return HAL_ERROR;
  }

  mtlconf->ReceiveArbitrationAlgorithm = READ_BIT(heth->Instance->MTLOMR, ETH_MTLOMR_RAA_Msk);
  mtlconf->TxSchedulingAlgorithm = READ_BIT(heth->Instance->MTLOMR, ETH_MTLOMR_SCHALG_Msk);
  mtlconf->TransmitStatus = ((READ_BIT(heth->Instance->MTLOMR, ETH_MTLOMR_DTXSTS_Msk) >> 1) > 0U) ? ENABLE : DISABLE;

  (mtlconf->RxQ[0]).QueueOpMode = READ_BIT(heth->Instance->MACRXQC0R,
                                           ETH_MACRXQC0R_RXQ0EN_NOT | ETH_MACRXQC0R_RXQ0EN_GT |
                                           ETH_MACRXQC0R_RXQ0EN_AV);
  (mtlconf->RxQ[1]).QueueOpMode = READ_BIT(heth->Instance->MACRXQC0R,
                                           ETH_MACRXQC0R_RXQ1EN_NOT | ETH_MACRXQC0R_RXQ1EN_GT |
                                           ETH_MACRXQC0R_RXQ1EN_AV);

  (mtlconf->RxQ[0]).MappedToDMACh = READ_BIT(heth->Instance->MTLRXQDMAMR, ETH_MTLRXQDMAMR_Q0MDMACH_Msk);
  (mtlconf->RxQ[1]).MappedToDMACh = READ_BIT(heth->Instance->MTLRXQDMAMR, ETH_MTLRXQDMAMR_Q1MDMACH_Msk);

  /* Get MTL parameters */
  for (queue = 0; queue < ETH_MTL_RX_Q_CNT; queue++)
  {
    (mtlconf->RxQ[queue]).RxQueueSize = READ_BIT(heth->Instance->MTL_QUEUE[queue].MTLRXQOMR,
                                                 (ETH_MTLRXQxOMR_RQS_0 | ETH_MTLRXQxOMR_RQS_1 | ETH_MTLRXQxOMR_RQS_2 |
                                                  ETH_MTLRXQxOMR_RQS_3));
    (mtlconf->RxQ[queue]).ReceiveQueueMode = READ_BIT(heth->Instance->MTL_QUEUE[queue].MTLRXQOMR, (ETH_MTLRXQxOMR_RTC |
                                                      ETH_MTLRXQxOMR_RSF));
    (mtlconf->RxQ[queue]).ForwardRxUndersizedGoodPacket = ((READ_BIT(heth->Instance->MTL_QUEUE[queue].MTLRXQOMR,
                                                                     ETH_MTLRXQxOMR_FUP) >> 3) > 0U) ? ENABLE : DISABLE;
    (mtlconf->RxQ[queue]).ForwardRxErrorPacket = ((READ_BIT(heth->Instance->MTL_QUEUE[queue].MTLRXQOMR,
                                                            ETH_MTLRXQxOMR_FEP) >> 4) > 0U) ? ENABLE : DISABLE;
    (mtlconf->RxQ[queue]).DropTCPIPChecksumErrorPacket = ((READ_BIT(heth->Instance->MTL_QUEUE[queue].MTLRXQOMR,
                                                                    ETH_MTLRXQxOMR_DISTCPEF) >>
                                                           6) == 0U) ? ENABLE : DISABLE;
  }

  for (queue = 0; queue < ETH_MTL_TX_Q_CNT; queue++)
  {
    (mtlconf->TxQ[queue]).TxQueueSize = READ_BIT(heth->Instance->MTL_QUEUE[queue].MTLTXQOMR,
                                                 (ETH_MTLTXQxOMR_TQS_0 | ETH_MTLTXQxOMR_TQS_1 | ETH_MTLTXQxOMR_TQS_2 |
                                                  ETH_MTLTXQxOMR_TQS_3));
    (mtlconf->TxQ[queue]).TransmitQueueMode = READ_BIT(heth->Instance->MTL_QUEUE[queue].MTLTXQOMR, (ETH_MTLTXQxOMR_TTC |
                                                       ETH_MTLTXQxOMR_TSF));
    (mtlconf->TxQ[queue]).QueueOpMode = READ_BIT(heth->Instance->MTL_QUEUE[queue].MTLTXQOMR, ETH_MTLTXQxOMR_TXQEN_NOT |
                                                 ETH_MTLTXQxOMR_TXQEN_EN | ETH_MTLTXQxOMR_TXQEN_AVMODE);
  }

  (mtlconf->TxQ[1]).AVAlgorithm = READ_BIT(heth->Instance->MTL_QUEUE[1].MTLTXQ1ECR, ETH_MTLTXQ1ECR_AVALG);

  return HAL_OK;
}

/**
  * @brief  Get the configuration of the MTL.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  mtlconf: pointer to a ETH_MACConfigTypeDef structure that will hold
  *         the configuration of the MTL.
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_ETHEx_GetMACMTLMappingConfig(const ETH_HandleTypeDef *heth, ETH_MACMTLMappingTypeDef *macmtlconf)
{
  if (macmtlconf == NULL)
  {
    return HAL_ERROR;
  }

  macmtlconf->VLANTagFilterFailPacketsQueue = READ_BIT(heth->Instance->MACRXQCR, ETH_MACRXQCR_VFFQ);
  macmtlconf->VLANTagFilterFailPacketsQueuingEnable = ((READ_BIT(heth->Instance->MACRXQCR,
                                                                 ETH_MACRXQCR_VFFQE_Msk) >> 16) > 0U)
                                                      ? ENABLE : DISABLE;
  macmtlconf->MulticastAddFilterFailPacketsQueue = READ_BIT(heth->Instance->MACRXQCR, ETH_MACRXQCR_MFFQ);
  macmtlconf->MulticastAddrFilterFailPacketsQueuingEnable = ((READ_BIT(heth->Instance->MACRXQCR,
                                                                       ETH_MACRXQCR_MFFQE_Msk) >> 8) > 0U)
                                                            ? ENABLE : DISABLE;
  macmtlconf->UnicastAddrFilterFailPacketsQueue = READ_BIT(heth->Instance->MACRXQCR, ETH_MACRXQCR_UFFQ);
  macmtlconf->UnicastAddrFilterFailPacketsQueuingEnable = (READ_BIT(heth->Instance->MACRXQCR,
                                                                    ETH_MACRXQCR_UFFQE_Msk) > 0U) ? ENABLE : DISABLE;
  macmtlconf->TypeFieldBasedRxQueuingEnable = (READ_BIT(heth->Instance->MACRXQC1R,
                                                        ETH_MACRXQC1R_TBRQE_Msk) > 0U) ? ENABLE : DISABLE;
  macmtlconf->OverridingMCBCQueuePrioritySelect = READ_BIT(heth->Instance->MACRXQC1R, ETH_MACRXQC1R_OMCBCQ);
  macmtlconf->FramePreemptionResidueQueue = READ_BIT(heth->Instance->MACRXQC1R, ETH_MACRXQC1R_FPRQ);
  macmtlconf->TaggedPTPoEPacketsQueuingControl = READ_BIT(heth->Instance->MACRXQC1R, ETH_MACRXQC1R_TPQC);
  macmtlconf->TaggedAVControlPacketsQueuingEnable = (READ_BIT(heth->Instance->MACRXQC1R,
                                                              ETH_MACRXQC1R_TACPQE_Msk) > 0U) ? ENABLE : DISABLE;
  macmtlconf->MulticastBroadcastQueueEnable = (READ_BIT(heth->Instance->MACRXQC1R,
                                                        ETH_MACRXQC1R_MCBCQEN_Msk) > 0U) ? ENABLE : DISABLE;
  macmtlconf->MulticastBroadcastQueue = READ_BIT(heth->Instance->MACRXQC1R, ETH_MACRXQC1R_MCBCQ);
  macmtlconf->UntaggedPacketQueue = READ_BIT(heth->Instance->MACRXQC1R, ETH_MACRXQC1R_UPQ);
  macmtlconf->PTPPacketsQueue = READ_BIT(heth->Instance->MACRXQC1R, ETH_MACRXQC1R_PTPQ);
  macmtlconf->AVUntaggedControlPacketsQueue = READ_BIT(heth->Instance->MACRXQC1R, ETH_MACRXQC1R_AVCPQ);

  if (HAL_ETHEx_GetUserTagPriorityQueue(heth, &(macmtlconf->PrioritiesSelectedRxQ0), ETH_RX_QUEUE0) != HAL_OK)
  {
    return HAL_ERROR;
  }

  if (HAL_ETHEx_GetUserTagPriorityQueue(heth, &(macmtlconf->PrioritiesSelectedRxQ1), ETH_RX_QUEUE1) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

HAL_StatusTypeDef ETHEx_SetMACMTLMappingConfig(ETH_HandleTypeDef *heth, const ETH_MACMTLMappingTypeDef *macmtlconf)
{
  uint32_t macmtlregval;

  /*------------------------ MACRXQC1R Configuration --------------------*/
  macmtlregval = ((uint32_t)(macmtlconf->AVUntaggedControlPacketsQueue) |
                  ((uint32_t)(macmtlconf->PTPPacketsQueue) << 4)         |
                  ((uint32_t)(macmtlconf->UntaggedPacketQueue) << 12)    |
                  ((uint32_t)(macmtlconf->MulticastBroadcastQueue) << 16) |
                  ((uint32_t)((macmtlconf->MulticastBroadcastQueueEnable == DISABLE) ? 0U : 1U) << 20) |
                  ((uint32_t)((macmtlconf->TaggedAVControlPacketsQueuingEnable == DISABLE) ? 0U : 1U) << 21) |
                  (uint32_t)(macmtlconf->TaggedPTPoEPacketsQueuingControl) |
                  (uint32_t)(macmtlconf->FramePreemptionResidueQueue) |
                  (uint32_t)(macmtlconf->OverridingMCBCQueuePrioritySelect) |
                  ((uint32_t)((macmtlconf->TypeFieldBasedRxQueuingEnable == DISABLE) ? 0U : 1U) << 29));

  /* Write to MACRXQC1R */
  MODIFY_REG(heth->Instance->MACRXQC1R, ETH_MACRXQC1R_MASK, macmtlregval);

  /*------------------------ MACRXQC2R Configuration --------------------*/
  /* Write to MACRXQC2R */
  if (HAL_ETHEx_SetUserTagPriorityQueue(heth, macmtlconf->PrioritiesSelectedRxQ0, ETH_RX_QUEUE0) != HAL_OK)
  {
    return HAL_ERROR;
  }

  if (HAL_ETHEx_SetUserTagPriorityQueue(heth, macmtlconf->PrioritiesSelectedRxQ1, ETH_RX_QUEUE1) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /*------------------------ MACRXQCR Configuration --------------------*/
  macmtlregval = (((uint32_t)((macmtlconf->UnicastAddrFilterFailPacketsQueuingEnable == DISABLE) ? 0U : 1U)) |
                  ((uint32_t)(macmtlconf->UnicastAddrFilterFailPacketsQueue) << 1)         |
                  ((uint32_t)((macmtlconf->MulticastAddrFilterFailPacketsQueuingEnable == DISABLE) ? 0U : 1U) << 8) |
                  ((uint32_t)(macmtlconf->MulticastAddFilterFailPacketsQueue) << 9)         |
                  ((uint32_t)((macmtlconf->VLANTagFilterFailPacketsQueuingEnable == DISABLE) ? 0U : 1U) << 16) |
                  ((uint32_t)(macmtlconf->VLANTagFilterFailPacketsQueue) << 17));

  /* Write to MACRXQCR */
  MODIFY_REG(heth->Instance->MACRXQCR, ETH_MACRXQCR_MASK, macmtlregval);

  return HAL_OK;
}

/**
  * @brief  Set the MAC MTL Mapping configuration.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  macmtlconf: pointer to a ETH_MACMTLMappingTypeDef structure that contains
  *         the configuration of the MAC MTL mapping.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_SetMACMTLMappingConfig(ETH_HandleTypeDef *heth, const ETH_MACMTLMappingTypeDef *macmtlconf)
{
  if (macmtlconf == NULL)
  {
    return HAL_ERROR;
  }

  if (heth->gState == HAL_ETH_STATE_READY)
  {
    if (ETHEx_SetMACMTLMappingConfig(heth, macmtlconf) != HAL_OK)
    {
      return HAL_ERROR;
    }

    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;
  }
}


void ETHEx_SetMTLConfig(ETH_HandleTypeDef *heth, const ETH_MTLConfigTypeDef *mtlconf)
{
  uint32_t queue;
  uint32_t mtlregval;

  /*------------------------ MTLOMR Configuration --------------------*/
  mtlregval = (mtlconf->TxSchedulingAlgorithm) |
              (mtlconf->ReceiveArbitrationAlgorithm) |
              ((uint32_t)((mtlconf->TransmitStatus == DISABLE) ? 1U : 0U) << 1);

  /* Write to MTLOMR */
  MODIFY_REG(heth->Instance->MTLOMR, ETH_MTLOMR_MASK, mtlregval);

  /*------------------------ MTLRXQDMAMR Configuration --------------------*/
  mtlregval = (mtlconf->RxQ[0]).MappedToDMACh |
              (mtlconf->RxQ[1]).MappedToDMACh;

  /* Write to MTLRXQDMAMR */
  MODIFY_REG(heth->Instance->MTLRXQDMAMR, ETH_MTLRXQDMAMR_Q0MDMACH_Msk | ETH_MTLRXQDMAMR_Q1MDMACH_Msk, mtlregval);

  /*------------------------ MTLTXQOMR Configuration --------------------*/
  for (queue = 0; queue < ETH_MTL_TX_Q_CNT; queue++)
  {
    mtlregval = ((mtlconf->TxQ[queue]).QueueOpMode)         |
                ((mtlconf->TxQ[queue]).TransmitQueueMode) |
                ((mtlconf->TxQ[queue]).TxQueueSize);

    /* Write to MTLTXQ0OMR */
    MODIFY_REG(heth->Instance->MTL_QUEUE[queue].MTLTXQOMR, ETH_MTLTXQxOMR_MASK, mtlregval);
  }

  /*------------------------ MACRXQC0R Configuration --------------------*/
  mtlregval = (((mtlconf->RxQ[0]).QueueOpMode)  |
               ((mtlconf->RxQ[1]).QueueOpMode));

  /* Write to MACRXQC0R */
  MODIFY_REG(heth->Instance->MACRXQC0R, ETH_MACRXQC0R_RXQ1EN_Msk | ETH_MACRXQC0R_RXQ0EN_Msk, mtlregval);

  /*------------------------ MTLRXQOMR Configuration --------------------*/
  for (queue = 0; queue < ETH_MTL_RX_Q_CNT; queue++)
  {

    mtlregval = (((mtlconf->RxQ[queue]).ReceiveQueueMode)  |
                 ((mtlconf->RxQ[queue]).RxQueueSize)       |
                 ((uint32_t)(((mtlconf->RxQ[queue]).DropTCPIPChecksumErrorPacket == DISABLE) ? 1U : 0U) << 6) |
                 ((uint32_t)(mtlconf->RxQ[queue]).ForwardRxErrorPacket << 4) |
                 ((uint32_t)(mtlconf->RxQ[queue]).ForwardRxUndersizedGoodPacket << 3));

    /* Write to MTLRXQOMR */
    MODIFY_REG(heth->Instance->MTL_QUEUE[queue].MTLRXQOMR, ETH_MTLRXQxOMR_MASK, mtlregval);
  }
  /*------------------------ MTLTXQ1ECR Configuration --------------------*/
  mtlregval = ((mtlconf->TxQ[1]).AVAlgorithm);

  /* Write to MTLTXQ1ECR */
  MODIFY_REG(heth->Instance->MTL_QUEUE[1].MTLTXQ1ECR, ETH_MTLTXQ1ECR_AVALG_Msk, mtlregval);
}

/**
  * @brief  Set the MTL configuration.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  mtlconf: pointer to a ETH_MTLConfigTypeDef structure that contains
  *         the configuration of the MAC.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_SetMTLConfig(ETH_HandleTypeDef *heth,  ETH_MTLConfigTypeDef *mtlconf)
{
  if (mtlconf == NULL)
  {
    return HAL_ERROR;
  }

  if (heth->gState == HAL_ETH_STATE_READY)
  {
    ETHEx_SetMTLConfig(heth, mtlconf);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Get the User Tag Priority Queueing.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  psrq: user priority
  * @param  queue: queue index
  * @retval None
  */
HAL_StatusTypeDef HAL_ETHEx_GetUserTagPriorityQueue(const ETH_HandleTypeDef *heth, uint32_t *psrq, uint32_t queue)
{
  if (psrq == NULL)
  {
    return HAL_ERROR;
  }

  if (queue == ETH_RX_QUEUE0)
  {
    *psrq = (uint32_t)(READ_BIT(heth->Instance->MACRXQC2R, ETH_MACRXQC2R_PSRQ0) >> ETH_MACRXQC2R_PSRQ0_Pos);
    return HAL_OK;
  }

  if (queue == ETH_RX_QUEUE1)
  {
    *psrq = (uint32_t)(READ_BIT(heth->Instance->MACRXQC2R, ETH_MACRXQC2R_PSRQ1) >> ETH_MACRXQC2R_PSRQ1_Pos);
    return HAL_OK;
  }

  return HAL_ERROR;
}

/**
  * @brief  Set the User Tag Priority Queueing.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  psrq: user priority
  * @param  queue: queue index
  * @retval None
  */
HAL_StatusTypeDef HAL_ETHEx_SetUserTagPriorityQueue(ETH_HandleTypeDef *heth, uint32_t psrq, uint32_t queue)
{
  uint32_t idx;
  uint32_t tmppsrq;
  uint32_t pos;

  for (idx = 0; idx < ETH_MTL_RX_Q_CNT; idx++)
  {
    pos = idx * 8U;

    if (idx != queue)
    {
      /* Ensure that the same priority PSR is not mapped to multiple Rx queues. */
      if (HAL_ETHEx_GetUserTagPriorityQueue(heth, &tmppsrq, idx) != HAL_OK)
      {
        return HAL_ERROR;
      }

      if ((tmppsrq & psrq) != 0U)
      {
        CLEAR_BIT(heth->Instance->MACRXQC2R, (psrq << (queue * 8U)));
        /* PSR is already mapped to another queue */
        return HAL_ERROR;
      }
    }
    else
    {
      MODIFY_REG(heth->Instance->MACRXQC2R, (psrq << pos), (uint32_t)(psrq << pos));
    }
  }
  return HAL_OK;
}

/**
  * @brief  Set the Packet Type Queueing.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  typequeueconf: pointer to a ETH_PacketTypeQueueConfigTypeDef structure that contains
  *         the configuration of the MAC.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_SetPacketTypeQueue(ETH_HandleTypeDef *heth,
                                               const ETH_PacketTypeQueueConfigTypeDef *typequeueconf)
{
  uint32_t cmd;
  uint32_t config;

  if (typequeueconf == NULL)
  {
    return HAL_ERROR;
  }

  if (heth->gState != HAL_ETH_STATE_STARTED)
  {
    return HAL_ERROR;
  }

  cmd = ETH_WRITE_OPERATION | ETH_MACIACR_OB | ((uint32_t)(typequeueconf->Address) << 8);
  config = typequeueconf->Preemption | (typequeueconf->Queue << 16) | typequeueconf->Type;

  /* Enable Type field based Rx queuing */
  SET_BIT(heth->Instance->MACRXQC1R, ETH_MACRXQC1R_TBRQE);

  /* Set configuration to MACTMRQR */
  WRITE_REG(heth->Instance->MACTMRQR, config);

  /* Set command to MACIACR */
  MODIFY_REG(heth->Instance->MACIACR, ETH_MAC_IACR_MASK, cmd);

  /* Get tick */
  uint32_t tickstart = HAL_GetTick();

  /* wait until the Operation reset is done */
  while (READ_BIT(heth->Instance->MACIACR, ETH_MACIACR_OB) != (uint32_t)RESET)
  {
    if ((HAL_GetTick() - tickstart) > ETH_OP_BUSY_TIMEOUT)
    {
      /* Set Error Code */
      heth->MACErrorCode = HAL_ETH_ERROR_TIMEOUT;
      /* Set State as Error */
      heth->gState = HAL_ETH_STATE_ERROR;
      /* Return Error */
      return HAL_ERROR;
    }
  }

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Get the Packet Type Queueing.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  typequeueconf: pointer to a ETH_PacketTypeQueueConfigTypeDef structure that contains
  *         the configuration of the MAC.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_GetPacketTypeQueue(ETH_HandleTypeDef *heth, ETH_PacketTypeQueueConfigTypeDef *typequeueconf)
{
  uint32_t cmd;

  if (heth->gState != HAL_ETH_STATE_STARTED)
  {
    return HAL_ERROR;
  }

  if (typequeueconf == NULL)
  {
    return HAL_ERROR;
  }

  cmd = ETH_READ_OPERATION | ETH_MACIACR_OB | ((uint32_t)(typequeueconf->Address) << 8);

  /* Set command to MACIACR */
  MODIFY_REG(heth->Instance->MACIACR, ETH_MAC_IACR_MASK, cmd);

  /* Get tick */
  uint32_t tickstart = HAL_GetTick();

  /* wait until the Operation reset is done */
  while (READ_BIT(heth->Instance->MACIACR, ETH_MACIACR_OB) != (uint32_t)RESET)
  {
    if ((HAL_GetTick() - tickstart) > ETH_OP_BUSY_TIMEOUT)
    {
      /* Set Error Code */
      heth->MACErrorCode = HAL_ETH_ERROR_TIMEOUT;
      /* Set State as Error */
      heth->gState = HAL_ETH_STATE_ERROR;
      /* Return Error */
      return HAL_ERROR;
    }
  }

  /* Get configuration from MACTMRQR */
  typequeueconf->Preemption = READ_BIT(heth->Instance->MACTMRQR, ETH_MACTMRQR_PFEX_Msk);
  typequeueconf->Queue = (READ_BIT(heth->Instance->MACTMRQR, ETH_MACTMRQR_TMRQ_Msk) >> 16);
  typequeueconf->Type = READ_BIT(heth->Instance->MACTMRQR, ETH_MACTMRQR_TYP_Msk);

  /* Return function status */
  return HAL_OK;
}

#ifdef HAL_ETH_USE_CBS
/**
  * @brief  Enable the CBS Algorithm.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  queueIndex: contains the AV queue index which will support the CBS algorithm
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_EnableCBS(ETH_HandleTypeDef *heth, uint8_t queueIndex)
{
  /* Enable AV mode for Queue x */
  MODIFY_REG(heth->Instance->MTL_QUEUE[queueIndex].MTLTXQOMR, ETH_MTLTXQxOMR_TXQEN_Msk, ETH_TX_QUEUE_AV_ENABLED);

  /* Enable CBS Algorithm for Queue x */
  SET_BIT(heth->Instance->MTL_QUEUE[queueIndex].MTLTXQ1ECR, ETH_TX_QUEUE_AV_ALGO_CBS);

  /* Return function status */
  return HAL_OK;
}

static void ETHEx_SetCBSConfig(ETH_HandleTypeDef *heth, ETH_CBSConfigTypeDef *cbsconf)
{
  uint32_t cbsregval;
  cbsregval = (uint32_t)(cbsconf->SlotCount | cbsconf->CreditControl);
  /* Write to MTLTXQ1ECR */
  MODIFY_REG(heth->Instance->MTL_QUEUE[cbsconf->QueueIdx].MTLTXQ1ECR, ETH_MTLTXQ1ECR_SLC_Msk | ETH_MTLTXQ1ECR_CC_Msk,
             cbsregval);

  cbsregval = (uint32_t)cbsconf->IdleSlope;
  /* Write to MTLTXQ1QWR */
  MODIFY_REG(heth->Instance->MTL_QUEUE[cbsconf->QueueIdx].MTLTXQQWR, ETH_MTLTXQ1QWR_ISCQW_Msk, cbsregval);

  cbsregval = (uint32_t)cbsconf->SendSlope;
  /* Write to MTLTXQ1SSCR */
  MODIFY_REG(heth->Instance->MTL_QUEUE[cbsconf->QueueIdx].MTLTXQ1SSCR, ETH_MTLTXQ1SSCR_SSC_Msk, cbsregval);

  cbsregval = (uint32_t)cbsconf->HiCredit;
  /* Write to MTLTXQ1HCR */
  MODIFY_REG(heth->Instance->MTL_QUEUE[cbsconf->QueueIdx].MTLTXQ1HCR, ETH_MTLTXQ1HCR_HC_Msk, cbsregval);

  cbsregval = (uint32_t)cbsconf->LoCredit;
  /* Write to MTLTXQ1LCR */
  MODIFY_REG(heth->Instance->MTL_QUEUE[cbsconf->QueueIdx].MTLTXQ1LCR, ETH_MTLTXQ1LCR_LC_Msk, cbsregval);
}

/**
  * @brief  Set the CBS configuration.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  cbsconf: pointer to a ETH_CBSConfigTypeDef structure that contains
  *         the configuration of the CBS algorithm.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_SetCBSConfig(ETH_HandleTypeDef *heth,  ETH_CBSConfigTypeDef *cbsconf)
{
  if (cbsconf == NULL)
  {
    return HAL_ERROR;
  }

  if ((READ_BIT(heth->Instance->MTL_QUEUE[cbsconf->QueueIdx].MTLTXQOMR, ETH_TX_QUEUE_AV_ENABLED) != (uint32_t)RESET) &&
      (READ_BIT(heth->Instance->MTL_QUEUE[cbsconf->QueueIdx].MTLTXQ1ECR, ETH_TX_QUEUE_AV_ALGO_CBS) != (uint32_t)RESET))
  {
    ETHEx_SetCBSConfig(heth, cbsconf);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  HAL ETH Get CBS Config.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  pCBSConfig: pointer to a ETH_CBSConfigTypeDef structure that will hold
  *         the configuration of the CBS Algorithm.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_GetCBSConfig(const ETH_HandleTypeDef *heth, ETH_CBSConfigTypeDef *pCBSConfig,
                                         uint8_t queueIndex)
{
  if (pCBSConfig == NULL)
  {
    return HAL_ERROR;
  }

  pCBSConfig->QueueIdx = queueIndex;
  pCBSConfig->SlotCount = READ_BIT(heth->Instance->MTL_QUEUE[queueIndex].MTLTXQ1ECR, ETH_MTLTXQ1ECR_SLC);
  pCBSConfig->CreditControl = READ_BIT(heth->Instance->MTL_QUEUE[queueIndex].MTLTXQ1ECR, ETH_MTLTXQ1ECR_CC);
  pCBSConfig->IdleSlope = READ_BIT(heth->Instance->MTL_QUEUE[queueIndex].MTLTXQQWR, ETH_MTLTXQ1QWR_ISCQW);
  pCBSConfig->SendSlope = READ_BIT(heth->Instance->MTL_QUEUE[queueIndex].MTLTXQ1SSCR, ETH_MTLTXQ1SSCR_SSC);
  pCBSConfig->HiCredit = READ_BIT(heth->Instance->MTL_QUEUE[queueIndex].MTLTXQ1HCR, ETH_MTLTXQ1HCR_HC);
  pCBSConfig->LoCredit = READ_BIT(heth->Instance->MTL_QUEUE[queueIndex].MTLTXQ1LCR, ETH_MTLTXQ1LCR_LC);

  return HAL_OK;
}
#endif /* HAL_ETH_USE_CBS */

#ifdef HAL_ETH_USE_TAS
/**
  * @brief  HAL ETH Enable EST feature.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_EnableEST(ETH_HandleTypeDef *heth)
{
  if (heth->gState == HAL_ETH_STATE_STARTED)
  {
    /* Enable Enhancement Scheduling Transmission */
    SET_BIT(heth->Instance->MTLESTCR, ETH_MTLESTCR_EEST);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  HAL ETH Disable EST feature.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_DisableEST(ETH_HandleTypeDef *heth)
{
  if (heth->gState == HAL_ETH_STATE_STARTED)
  {
    /* Disable Enhancement Scheduling Transmission */
    CLEAR_BIT(heth->Instance->MTLESTCR, ETH_MTLESTCR_EEST);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  HAL ETH Get the HW Depth of the Gate Control List.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval Depth of the Gate Control List
  */
uint32_t HAL_ETHEx_GetGCLDepth(const ETH_HandleTypeDef *heth)
{
  uint32_t gcldepth;

  /* get HW GCL depth from HW configuration */
  gcldepth = READ_BIT(heth->Instance->MACHWF3R, ETH_MACHWF3R_ESTDEP_Msk);

  switch (gcldepth)
  {
    case ETH_MACHWF3R_ESTDEP_64 :
      return 64;
    case ETH_MACHWF3R_ESTDEP_128 :
      return 128;
    case ETH_MACHWF3R_ESTDEP_256 :
      return 256;
    case ETH_MACHWF3R_ESTDEP_512 :
      return 512;
    case ETH_MACHWF3R_ESTDEP_1024 :
      return 1024;
    default :
      return 0;
  }
}

/**
  * @brief  HAL ETH Get the HW Width of the Time Interval field in the Gate Control List
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval Width of the Time Interval field in the Gate Control List
  */
uint32_t HAL_ETHEx_GetGCLWidthTimeInterval(const ETH_HandleTypeDef *heth)
{
  uint32_t gclwidth;

  /* get HW GCL Width from HW configuration */
  gclwidth = READ_BIT(heth->Instance->MACHWF3R, ETH_MACHWF3R_ESTWID_Msk);

  switch (gclwidth)
  {
    case ETH_MACHWF3R_ESTWID_16 :
      return 16;
    case ETH_MACHWF3R_ESTWID_20 :
      return 20;
    case ETH_MACHWF3R_ESTWID_24 :
      return 24;
    default :
      return 0;
  }
}

/**
  * @brief  HAL ETH Check HW Completion Check
  */
static HAL_StatusTypeDef ETHEx_ESTHWCompletionCheck(ETH_HandleTypeDef *heth)
{
  /* Get tick */
  uint32_t tickstart = HAL_GetTick();

  /* wait until the reset is done by Hardware */
  while (READ_BIT(heth->Instance->MTLESTGCLCR, ETH_MTLESTGCLCR_SRWO) != (uint32_t)RESET)
  {
    if ((HAL_GetTick() - tickstart) > ETH_HWRESET_TIMEOUT)
    {
      /* Set State as Error */
      heth->gState = HAL_ETH_STATE_ERROR;
      /* Return Error */
      return HAL_ERROR;
    }
  }
  return HAL_OK;
}

/**
  * @brief  HAL ETH Get GCL Registers values
  */
HAL_StatusTypeDef HAL_ETHEx_GetGCLRegisters(ETH_HandleTypeDef *heth, ETH_GCLConfigTypeDef *gclconf)
{
  uint32_t mtlestgclctrl = ETH_MTLESTGCLCR_GCRR | ETH_MTLESTGCLCR_SRWO | ETH_MTLESTGCLCR_R1W0;
  uint32_t mtlestgclctrl_mask = ETH_MTLESTGCLCR_ADDR_Msk | ETH_MTLESTGCLCR_GCRR_Msk | ETH_MTLESTGCLCR_SRWO_Msk;

  /* Set BTR Low Address and start read operation */
  MODIFY_REG(heth->Instance->MTLESTGCLCR, mtlestgclctrl_mask, mtlestgclctrl | ETH_MTLESTGCLCR_ADDR_BTRLOW);

  /* wait for a HW reset Check */
  if (ETHEx_ESTHWCompletionCheck(heth) != HAL_OK)
  {
    return HAL_ERROR;
  }
  /* Get BTR Low value */
  gclconf->BaseTimeRegister = (uint32_t)READ_REG(heth->Instance->MTLESTGCLDR);

  /* Set BTR High Address and start read operation */
  MODIFY_REG(heth->Instance->MTLESTGCLCR, mtlestgclctrl_mask, mtlestgclctrl | ETH_MTLESTGCLCR_ADDR_BTRHIGH);

  /* wait for a HW reset Check */
  if (ETHEx_ESTHWCompletionCheck(heth) != HAL_OK)
  {
    return HAL_ERROR;
  }
  /* Get BTR High value */
  gclconf->BaseTimeRegister |= ((uint64_t)READ_REG(heth->Instance->MTLESTGCLDR) << 32U);

  /* Set CTR Low Address and start read operation */
  MODIFY_REG(heth->Instance->MTLESTGCLCR, mtlestgclctrl_mask, mtlestgclctrl | ETH_MTLESTGCLCR_ADDR_CTRLOW);

  /* wait for a HW reset Check */
  if (ETHEx_ESTHWCompletionCheck(heth) != HAL_OK)
  {
    return HAL_ERROR;
  }
  /* Get CTR Low value */
  gclconf->CycleTimeRegister = (uint32_t)READ_REG(heth->Instance->MTLESTGCLDR);

  /* Set CTR High Address and start read operation */
  MODIFY_REG(heth->Instance->MTLESTGCLCR, mtlestgclctrl_mask, mtlestgclctrl | ETH_MTLESTGCLCR_ADDR_CTRHIGH);

  /* wait for a HW reset Check */
  if (ETHEx_ESTHWCompletionCheck(heth) != HAL_OK)
  {
    return HAL_ERROR;
  }
  /* Get CTR High value */
  gclconf->CycleTimeRegister |= ((uint64_t)READ_REG(heth->Instance->MTLESTGCLDR) << 32U);

  /* Set TER Address and start read operation */
  MODIFY_REG(heth->Instance->MTLESTGCLCR, mtlestgclctrl_mask, mtlestgclctrl | ETH_MTLESTGCLCR_ADDR_TER);

  /* wait for a HW reset Check */
  if (ETHEx_ESTHWCompletionCheck(heth) != HAL_OK)
  {
    return HAL_ERROR;
  }
  /* Get TER value */
  gclconf->TimeExtensionRegister = (uint32_t)READ_REG(heth->Instance->MTLESTGCLDR);

  /* Set LLR Address and start read operation */
  MODIFY_REG(heth->Instance->MTLESTGCLCR, mtlestgclctrl_mask, mtlestgclctrl | ETH_MTLESTGCLCR_ADDR_LLR);

  /* wait for a HW reset Check */
  if (ETHEx_ESTHWCompletionCheck(heth) != HAL_OK)
  {
    return HAL_ERROR;
  }
  /* Get TER value */
  gclconf->ListLengthRegister = (uint32_t)READ_REG(heth->Instance->MTLESTGCLDR);

  return HAL_OK;
}

/**
  * @brief  HAL ETH Set GCL Registers values
  */
HAL_StatusTypeDef HAL_ETHEx_SetGCLRegisters(ETH_HandleTypeDef *heth, const ETH_GCLConfigTypeDef *gclconf)
{
  uint32_t mtlestgclctrl = ETH_MTLESTGCLCR_GCRR | ETH_MTLESTGCLCR_SRWO;
  uint32_t mtlestgclctrl_mask = ETH_MTLESTGCLCR_ADDR_Msk | ETH_MTLESTGCLCR_GCRR_Msk | ETH_MTLESTGCLCR_SRWO_Msk;

  /* Set BTR Low value */
  WRITE_REG(heth->Instance->MTLESTGCLDR, (uint32_t)gclconf->BaseTimeRegister);

  /* Set BTR Low Address and start write operation */
  MODIFY_REG(heth->Instance->MTLESTGCLCR, mtlestgclctrl_mask, mtlestgclctrl | ETH_MTLESTGCLCR_ADDR_BTRLOW);

  /* wait for a HW reset Check */
  if (ETHEx_ESTHWCompletionCheck(heth) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Set BTR High value */
  WRITE_REG(heth->Instance->MTLESTGCLDR, ((uint32_t)(gclconf->BaseTimeRegister >> 32)));

  /* Set BTR High Address and start write operation */
  MODIFY_REG(heth->Instance->MTLESTGCLCR, mtlestgclctrl_mask, mtlestgclctrl | ETH_MTLESTGCLCR_ADDR_BTRHIGH);

  /* wait for a HW reset Check */
  if (ETHEx_ESTHWCompletionCheck(heth) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Set CTR Low value */
  WRITE_REG(heth->Instance->MTLESTGCLDR, (uint32_t)gclconf->CycleTimeRegister);

  /* Set CTR Low Address and start write operation */
  MODIFY_REG(heth->Instance->MTLESTGCLCR, mtlestgclctrl_mask, mtlestgclctrl | ETH_MTLESTGCLCR_ADDR_CTRLOW);

  /* wait for a HW reset Check */
  if (ETHEx_ESTHWCompletionCheck(heth) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Set CTR High value */
  WRITE_REG(heth->Instance->MTLESTGCLDR, ((uint32_t)(gclconf->CycleTimeRegister >> 32)));

  /* Set CTR High Address and start write operation */
  MODIFY_REG(heth->Instance->MTLESTGCLCR, mtlestgclctrl_mask, mtlestgclctrl | ETH_MTLESTGCLCR_ADDR_CTRHIGH);

  /* wait for a HW reset Check */
  if (ETHEx_ESTHWCompletionCheck(heth) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Set TER value */
  WRITE_REG(heth->Instance->MTLESTGCLDR, (uint32_t)gclconf->TimeExtensionRegister);

  /* Set TER Address and start write operation */
  MODIFY_REG(heth->Instance->MTLESTGCLCR, mtlestgclctrl_mask, mtlestgclctrl | ETH_MTLESTGCLCR_ADDR_TER);

  /* Set LLR value */
  WRITE_REG(heth->Instance->MTLESTGCLDR, (uint32_t)gclconf->ListLengthRegister);

  /* Set LLR Address and start write operation */
  MODIFY_REG(heth->Instance->MTLESTGCLCR, mtlestgclctrl_mask, mtlestgclctrl | ETH_MTLESTGCLCR_ADDR_LLR);

  /* wait for a HW reset Check */
  if (ETHEx_ESTHWCompletionCheck(heth) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  HAL ETH Set GCL configuration
  */
HAL_StatusTypeDef HAL_ETHEx_SetGCLConfig(ETH_HandleTypeDef *heth, ETH_GCLConfigTypeDef *gclconf)
{
  uint32_t glcidx;
  uint32_t data;

  for (glcidx = 0; glcidx < gclconf->ListLengthRegister; glcidx++)
  {
    /* Set data : TC1 + TC0 + TimeInterval */
    data = gclconf->opList->Interval | (gclconf->opList->Gate << ETH_EST_WID_MAX);

    /* Set data to MTLESTGCLDR register */
    WRITE_REG(heth->Instance->MTLESTGCLDR, data);

    /* Set Address and start write operation */
    MODIFY_REG(heth->Instance->MTLESTGCLCR, ETH_MTLESTGCLCR_ADDR_Msk | ETH_MTLESTGCLCR_SRWO_Msk,
               (glcidx << ETH_MTLESTGCLCR_ADDR_Pos) | ETH_MTLESTGCLCR_SRWO);

    /* wait for a HW completion Check */
    if (ETHEx_ESTHWCompletionCheck(heth) != HAL_OK)
    {
      return HAL_ERROR;
    }

    /* If ok, Move to the next GLC in the list */
    (gclconf->opList)++;
  }

  if (HAL_ETHEx_SetGCLRegisters(heth, gclconf) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  HAL ETH Set Enhancement Scheduling Traffic Configuration
  */
static HAL_StatusTypeDef ETHEx_SetESTConfig(ETH_HandleTypeDef *heth, ETH_ESTConfigTypeDef *estconf)
{
  uint32_t estregval;

  /* Check on GCL list Length */
  if (estconf->GCLRegisters.ListLengthRegister > ETH_EST_DEP_MAX)
  {
    return HAL_ERROR;
  }
  /* Check on opList */
  if (estconf->GCLRegisters.opList == NULL)
  {
    return HAL_ERROR;
  }
  /* Check on completion of switch to the S/W owned list */
  if (READ_BIT(heth->Instance->MTLESTCR, ETH_MTLESTCR_SSWL) != (uint32_t)RESET)
  {
    return HAL_ERROR;
  }

  /* Programming the GCL and GCL-linked registers */
  if (HAL_ETHEx_SetGCLConfig(heth, &estconf->GCLRegisters) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Set EST Configuration */
  estregval = ((estconf->SwitchToSWOL << 1) |
               ((uint32_t)estconf->NotDropFramesDuringFrameSizeError << 4) |
               ((uint32_t)estconf->DropFramesCausingError << 5) |
               (estconf->LoopCountSchedulingError) |
               (estconf->TimeIntervalLeftShift << 8) |
               (estconf->CurrentTimeOffset << 12) |
               (((uint32_t)(1000000000U / estconf->PTPTimeOffset) * 6U) << 24));

  /* Write to MTLESTCR */
  MODIFY_REG(heth->Instance->MTLESTCR, ETH_MTLESTCR_MASK, estregval);

  /* Get Overhead bytes value */
  estregval = estconf->OverheadBytesValue;

  /* Write to MTLESTECR */
  MODIFY_REG(heth->Instance->MTLESTECR, ETH_MTLESTECR_OVHD_Msk, estregval);

  /* Enable Enhancement Scheduling Traffic feature */
  if (HAL_ETHEx_EnableEST(heth) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Set the EST configuration.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  estconf: pointer to a ETH_ESTConfigTypeDef structure that contains
  *         the configuration of the EST algorithm.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_SetESTConfig(ETH_HandleTypeDef *heth,  ETH_ESTConfigTypeDef *estconf)
{
  if (estconf == NULL)
  {
    return HAL_ERROR;
  }

  if (heth->gState == HAL_ETH_STATE_STARTED)
  {
    if (ETHEx_SetESTConfig(heth, estconf) != HAL_OK)
    {
      return HAL_ERROR;
    }
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}
#endif /* HAL_ETH_USE_TAS */

#ifdef HAL_ETH_USE_FPE
/**
  * @brief  Enable FPE feature.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_EnableFPE(ETH_HandleTypeDef *heth)
{
  if (heth->gState == HAL_ETH_STATE_STARTED)
  {
    /* Enable Tx Frame Preemption feature */
    SET_BIT(heth->Instance->MACFPECSR, ETH_MACFPECSR_EFPE);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Disable FPE feature.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_DisableFPE(ETH_HandleTypeDef *heth)
{
  if (heth->gState == HAL_ETH_STATE_STARTED)
  {
    /* Disable Tx Frame Preemption feature */
    CLEAR_BIT(heth->Instance->MACFPECSR, ETH_MACFPECSR_EFPE);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Get the FPE configuration.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  fpeconf: pointer to a ETH_FPEConfigTypeDef structure that contains
  *         the configuration of the FPE Feature.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_GetFPEConfig(ETH_HandleTypeDef *heth,  ETH_FPEConfigTypeDef *fpeconf)
{
  if (fpeconf == NULL)
  {
    return HAL_ERROR;
  }

  fpeconf->AdditionalFragmentSize = READ_BIT(heth->Instance->MTLFPECSR, ETH_MTLFPECSR_AFSZ);
  fpeconf->PreemptionClassification = READ_BIT(heth->Instance->MTLFPECSR, ETH_MTLFPECSR_PEC);
  fpeconf->HoldReleaseStatus = READ_BIT(heth->Instance->MTLFPECSR, ETH_MTLFPECSR_HRS);

  fpeconf->HoldAdvance = READ_BIT(heth->Instance->MTLFPEAR, ETH_MTLFPEAR_HADV);
  fpeconf->ReleaseAdvance = READ_BIT(heth->Instance->MTLFPEAR, ETH_MTLFPEAR_RADV);

  fpeconf->SendVerifymPacket = ((READ_BIT(heth->Instance->MACFPECSR,
                                          ETH_MACFPECSR_SVER) >> ETH_MACFPECSR_SVER_Pos) > 0U) ? ENABLE : DISABLE;
  fpeconf->SendRespondmPacket = ((READ_BIT(heth->Instance->MACFPECSR,
                                           ETH_MACFPECSR_SRSP) >> ETH_MACFPECSR_SRSP_Pos) > 0U) ? ENABLE : DISABLE;
  return HAL_OK;
}

static void ETHEx_SetFPEConfig(ETH_HandleTypeDef *heth, ETH_FPEConfigTypeDef *fpeconf)
{
  uint32_t fperegval;

  fperegval = fpeconf->PreemptionClassification | fpeconf->AdditionalFragmentSize;

  /* Write to MTLFPECSR */
  WRITE_REG(heth->Instance->MTLFPECSR, fperegval);

  fperegval = fpeconf->HoldAdvance | fpeconf->ReleaseAdvance;

  /* Write to MTLFPEAR */
  WRITE_REG(heth->Instance->MTLFPEAR, fperegval);

  fperegval = ((uint32_t)((fpeconf->SendVerifymPacket == ENABLE) ? 1U : 0U) << 1) |
              ((uint32_t)((fpeconf->SendRespondmPacket == ENABLE) ? 1U : 0U) << 2);

  /* Write to MACFPECSR */
  MODIFY_REG(heth->Instance->MACFPECSR, ETH_MACFPECSR_SVER_Msk | ETH_MACFPECSR_SRSP_Msk, fperegval);
}

/**
  * @brief  Set the FPE configuration.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  fpeconf: pointer to a ETH_FPEConfigTypeDef structure that contains
  *         the configuration of the FPE Feature.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_SetFPEConfig(ETH_HandleTypeDef *heth,  ETH_FPEConfigTypeDef *fpeconf)
{
  if (fpeconf == NULL)
  {
    return HAL_ERROR;
  }

  if (heth->gState == HAL_ETH_STATE_STARTED)
  {
    ETHEx_SetFPEConfig(heth, fpeconf);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}
#endif /* HAL_ETH_USE_FPE */
/**
  * @}
  */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* ETH1 */

#endif /* HAL_ETH_MODULE_ENABLED */
/**
  * @}
  */

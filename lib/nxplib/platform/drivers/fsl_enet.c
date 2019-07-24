/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_enet.h"
#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL
#include "fsl_cache.h"
#endif /* FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.enet"
#endif

/*! @brief IPv4 PTP message IP version offset. */
#define ENET_PTP1588_IPVERSION_OFFSET 0x0EU
/*! @brief IPv4 PTP message UDP protocol offset. */
#define ENET_PTP1588_IPV4_UDP_PROTOCOL_OFFSET 0x17U
/*! @brief IPv4 PTP message UDP port offset. */
#define ENET_PTP1588_IPV4_UDP_PORT_OFFSET 0x24U
/*! @brief IPv4 PTP message UDP message type offset. */
#define ENET_PTP1588_IPV4_UDP_MSGTYPE_OFFSET 0x2AU
/*! @brief IPv4 PTP message UDP version offset. */
#define ENET_PTP1588_IPV4_UDP_VERSION_OFFSET 0x2BU
/*! @brief IPv4 PTP message UDP clock id offset. */
#define ENET_PTP1588_IPV4_UDP_CLKID_OFFSET 0x3EU
/*! @brief IPv4 PTP message UDP sequence id offset. */
#define ENET_PTP1588_IPV4_UDP_SEQUENCEID_OFFSET 0x48U
/*! @brief IPv4 PTP message UDP control offset. */
#define ENET_PTP1588_IPV4_UDP_CTL_OFFSET 0x4AU
/*! @brief IPv6 PTP message UDP protocol offset. */
#define ENET_PTP1588_IPV6_UDP_PROTOCOL_OFFSET 0x14U
/*! @brief IPv6 PTP message UDP port offset. */
#define ENET_PTP1588_IPV6_UDP_PORT_OFFSET 0x38U
/*! @brief IPv6 PTP message UDP message type offset. */
#define ENET_PTP1588_IPV6_UDP_MSGTYPE_OFFSET 0x3EU
/*! @brief IPv6 PTP message UDP version offset. */
#define ENET_PTP1588_IPV6_UDP_VERSION_OFFSET 0x3FU
/*! @brief IPv6 PTP message UDP clock id offset. */
#define ENET_PTP1588_IPV6_UDP_CLKID_OFFSET 0x52U
/*! @brief IPv6 PTP message UDP sequence id offset. */
#define ENET_PTP1588_IPV6_UDP_SEQUENCEID_OFFSET 0x5CU
/*! @brief IPv6 PTP message UDP control offset. */
#define ENET_PTP1588_IPV6_UDP_CTL_OFFSET 0x5EU
/*! @brief PTPv2 message Ethernet packet type offset. */
#define ENET_PTP1588_ETHL2_PACKETTYPE_OFFSET 0x0CU
/*! @brief PTPv2 message Ethernet message type offset. */
#define ENET_PTP1588_ETHL2_MSGTYPE_OFFSET 0x0EU
/*! @brief PTPv2 message Ethernet version type offset. */
#define ENET_PTP1588_ETHL2_VERSION_OFFSET 0X0FU
/*! @brief PTPv2 message Ethernet clock id offset. */
#define ENET_PTP1588_ETHL2_CLOCKID_OFFSET 0x22
/*! @brief PTPv2 message Ethernet sequence id offset. */
#define ENET_PTP1588_ETHL2_SEQUENCEID_OFFSET 0x2c
/*! @brief Packet type Ethernet IEEE802.3 for PTPv2. */
#define ENET_ETHERNETL2 0x88F7U
/*! @brief Packet type IPv4. */
#define ENET_IPV4 0x0800U
/*! @brief Packet type IPv6. */
#define ENET_IPV6 0x86ddU
/*! @brief Packet type VLAN. */
#define ENET_8021QVLAN 0x8100U
/*! @brief UDP protocol type. */
#define ENET_UDPVERSION 0x0011U
/*! @brief Packet IP version IPv4. */
#define ENET_IPV4VERSION 0x0004U
/*! @brief Packet IP version IPv6. */
#define ENET_IPV6VERSION 0x0006U
/*! @brief Ethernet mac address length. */
#define ENET_FRAME_MACLEN 6U
/*! @brief Ethernet VLAN header length. */
#define ENET_FRAME_VLAN_TAGLEN 4U
/*! @brief MDC frequency. */
#define ENET_MDC_FREQUENCY 2500000U
/*! @brief NanoSecond in one second. */
#define ENET_NANOSECOND_ONE_SECOND 1000000000U
/*! @brief Define a common clock cycle delays used for time stamp capture. */
#ifndef ENET_1588TIME_DELAY_COUNT
#define ENET_1588TIME_DELAY_COUNT 10U
#endif

/*! @brief Defines the macro for converting constants from host byte order to network byte order. */
#define ENET_HTONS(n) __REV16(n)
#define ENET_HTONL(n) __REV(n)
#define ENET_NTOHS(n) __REV16(n)
#define ENET_NTOHL(n) __REV(n)

/*! @brief Define the ENET ring/class bumber . */
enum _enet_ring_number
{
    kENET_Ring0 = 0U, /*!< ENET ring/class 0. */
#if FSL_FEATURE_ENET_QUEUE > 1
    kENET_Ring1 = 1U, /*!< ENET ring/class 1. */
    kENET_Ring2 = 2U  /*!< ENET ring/class 2. */
#endif                /* FSL_FEATURE_ENET_QUEUE > 1 */
};

/*! @brief Define interrupt IRQ handler. */
#if FSL_FEATURE_ENET_QUEUE > 1
typedef void (*enet_isr_ring_t)(ENET_Type *base, enet_handle_t *handle, uint32_t ringId);
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */
typedef void (*enet_isr_t)(ENET_Type *base, enet_handle_t *handle);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief Get the ENET instance from peripheral base address.
 *
 * @param base ENET peripheral base address.
 * @return ENET instance.
 */
uint32_t ENET_GetInstance(ENET_Type *base);
/*!
 * @brief Set ENET MAC controller with the configuration.
 *
 * @param base ENET peripheral base address.
 * @param handle The ENET handle pointer.
 * @param config ENET Mac configuration.
 * @param bufferConfig ENET buffer configuration.
 * @param macAddr ENET six-byte mac address.
 * @param srcClock_Hz ENET module clock source, normally it's system clock.
 */
static void ENET_SetMacController(ENET_Type *base,
                                  enet_handle_t *handle,
                                  const enet_config_t *config,
                                  const enet_buffer_config_t *bufferConfig,
                                  uint8_t *macAddr,
                                  uint32_t srcClock_Hz);
/*!
 * @brief Set ENET handler.
 *
 * @param base ENET peripheral base address.
 * @param handle The ENET handle pointer.
 * @param config ENET configuration stucture pointer.
 * @param bufferConfig ENET buffer configuration.
 */
static void ENET_SetHandler(ENET_Type *base,
                            enet_handle_t *handle,
                            const enet_config_t *config,
                            const enet_buffer_config_t *bufferConfig);
/*!
 * @brief Set ENET MAC transmit buffer descriptors.
 *
 * @param handle The ENET handle pointer.
 * @param config The ENET configuration structure.
 * @param bufferConfig The ENET buffer configuration.
 */
static void ENET_SetTxBufferDescriptors(enet_handle_t *handle,
                                        const enet_config_t *config,
                                        const enet_buffer_config_t *bufferConfig);

/*!
 * @brief Set ENET MAC receive buffer descriptors.
 *
 * @param handle The ENET handle pointer.
 * @param config The ENET configuration structure.
 * @param bufferConfig The ENET buffer configuration.
 */
static void ENET_SetRxBufferDescriptors(enet_handle_t *handle,
                                        const enet_config_t *config,
                                        const enet_buffer_config_t *bufferConfig);

/*!
 * @brief Updates the ENET read buffer descriptors.
 *
 * @param base ENET peripheral base address.
 * @param handle The ENET handle pointer.
 * @param ringId The descriptor ring index, range from 0 ~ FSL_FEATURE_ENET_QUEUE - 1.
 *               0 ----- for single ring kinetis platform.
 *               0 ~ 2 for mulit-ring supported IMX8qm.
 */
static void ENET_UpdateReadBuffers(ENET_Type *base, enet_handle_t *handle, uint32_t ringId);

/*!
 * @brief Activates ENET send for multiple tx rings.
 *
 * @param base  ENET peripheral base address.
 * @param ringId The descriptor ring index, range from 0 ~ FSL_FEATURE_ENET_QUEUE - 1.
 *               0 ----- for single ring kinetis platform.
 *               0 ~ 2 for mulit-ring supported IMX8qm.
 *
 * @note This must be called after the MAC configuration and
 * state are ready. It must be called after the ENET_Init() and
 * this should be called when the ENET receive required.
 */
static void ENET_ActiveSend(ENET_Type *base, uint32_t ringId);
#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
/*!
 * @brief Parses the ENET frame for time-stamp process of PTP 1588 frame.
 *
 * @param data  The ENET read data for frame parse.
 * @param ptpTsData The ENET PTP message and time-stamp data pointer.
 * @param isFastEnabled The fast parse flag.
 *        - true , Fast processing, only check if this is a PTP message.
 *        - false, Store the PTP message data after check the PTP message.
 */
static bool ENET_Ptp1588ParseFrame(const uint8_t *data, enet_ptp_time_data_t *ptpTsData, bool isFastEnabled);

/*!
 * @brief Updates the new PTP 1588 time-stamp to the time-stamp buffer ring.
 *
 * @param ptpTsDataRing The PTP message and time-stamp data ring pointer.
 * @param ptpTimeData   The new PTP 1588 time-stamp data pointer.
 */
static status_t ENET_Ptp1588UpdateTimeRing(enet_ptp_time_data_ring_t *ptpTsDataRing, enet_ptp_time_data_t *ptpTimeData);

/*!
 * @brief Search up the right PTP 1588 time-stamp from the time-stamp buffer ring.
 *
 * @param ptpTsDataRing The PTP message and time-stamp data ring pointer.
 * @param ptpTimeData   The find out right PTP 1588 time-stamp data pointer with the specific PTP message.
 */
static status_t ENET_Ptp1588SearchTimeRing(enet_ptp_time_data_ring_t *ptpTsDataRing, enet_ptp_time_data_t *ptpTimedata);

/*!
 * @brief Store the transmit time-stamp for event PTP frame in the time-stamp buffer ring.
 *
 * @param base   ENET peripheral base address.
 * @param handle The ENET handle pointer.
 * @param ringId The descriptor ring index, range from 0 ~ FSL_FEATURE_ENET_QUEUE - 1.
 *               0 ----- for single ring kinetis platform.
 *               0 ~ 2 for mulit-ring supported IMX8qm.
 */
static status_t ENET_StoreTxFrameTime(ENET_Type *base, enet_handle_t *handle, uint32_t ringId);

/*!
 * @brief Store the receive time-stamp for event PTP frame in the time-stamp buffer ring.
 *
 * @param base   ENET peripheral base address.
 * @param handle The ENET handle pointer.
 * @param ptpTimeData The PTP 1588 time-stamp data pointer.
 */
static status_t ENET_StoreRxFrameTime(ENET_Type *base, enet_handle_t *handle, enet_ptp_time_data_t *ptpTimeData);

#if defined(FSL_FEATURE_ENET_HAS_AVB) && FSL_FEATURE_ENET_AVB
/*!
 * @brief Gets the ring index for transmission.
 *
 * @param base  ENET peripheral base address.
 * @param data The ENET transmit data.
 * @param handle The ENET handle pointer.
 *
 * @note This must be called after the MAC configuration and
 * state are ready. It must be called after the ENET_Init() and
 * this should be called when the ENET receive required.
 */
static uint8_t ENET_GetTxRingId(ENET_Type *base, uint8_t *data, enet_handle_t *handle);
#endif /* FSL_FEATURE_ENET_HAS_AVB */
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Pointers to enet handles for each instance. */
static enet_handle_t *s_ENETHandle[FSL_FEATURE_SOC_ENET_COUNT] = {NULL};

/*! @brief Pointers to enet clocks for each instance. */
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
const clock_ip_name_t s_enetClock[] = ENET_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*! @brief Pointers to enet transmit IRQ number for each instance. */
static const IRQn_Type s_enetTxIrqId[] = ENET_Transmit_IRQS;
/*! @brief Pointers to enet receive IRQ number for each instance. */
static const IRQn_Type s_enetRxIrqId[] = ENET_Receive_IRQS;
#if defined(ENET_ENHANCEDBUFFERDESCRIPTOR_MODE) && ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
/*! @brief Pointers to enet timestamp IRQ number for each instance. */
static const IRQn_Type s_enetTsIrqId[] = ENET_1588_Timer_IRQS;
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */
/*! @brief Pointers to enet error IRQ number for each instance. */
static const IRQn_Type s_enetErrIrqId[] = ENET_Error_IRQS;

/*! @brief Pointers to enet bases for each instance. */
static ENET_Type *const s_enetBases[] = ENET_BASE_PTRS;

/* ENET ISR for transactional APIs. */
#if FSL_FEATURE_ENET_QUEUE > 1
static enet_isr_ring_t s_enetTxIsr;
static enet_isr_ring_t s_enetRxIsr;
#else
static enet_isr_t s_enetTxIsr;
static enet_isr_t s_enetRxIsr;
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */
static enet_isr_t s_enetErrIsr;
static enet_isr_t s_enetTsIsr;
/*******************************************************************************
 * Code
 ******************************************************************************/

uint32_t ENET_GetInstance(ENET_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_enetBases); instance++)
    {
        if (s_enetBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_enetBases));

    return instance;
}

/*!
 * brief Gets the ENET default configuration structure.
 *
 * The purpose of this API is to get the default ENET MAC controller
 * configure structure for ENET_Init(). User may use the initialized
 * structure unchanged in ENET_Init(), or modify some fields of the
 * structure before calling ENET_Init().
 * Example:
   code
   enet_config_t config;
   ENET_GetDefaultConfig(&config);
   endcode
 * param config The ENET mac controller configuration structure pointer.
 */
void ENET_GetDefaultConfig(enet_config_t *config)
{
    /* Checks input parameter. */
    assert(config);

    /* Initializes the MAC configure structure to zero. */
    memset(config, 0, sizeof(enet_config_t));

/* Sets MII mode, full duplex, 100Mbps for MAC and PHY data interface. */
#if defined(FSL_FEATURE_ENET_HAS_AVB) && FSL_FEATURE_ENET_HAS_AVB
    config->miiMode = kENET_RgmiiMode;
#else
    config->miiMode = kENET_RmiiMode;
#endif
    config->miiSpeed  = kENET_MiiSpeed100M;
    config->miiDuplex = kENET_MiiFullDuplex;

    config->ringNum = 1;

    /* Sets the maximum receive frame length. */
    config->rxMaxFrameLen = ENET_FRAME_MAX_FRAMELEN;
}

/*!
 * brief Initializes the ENET module.
 *
 * This function ungates the module clock and initializes it with the ENET configuration.
 *
 * param base    ENET peripheral base address.
 * param handle  ENET handler pointer.
 * param config  ENET mac configuration structure pointer.
 *        The "enet_config_t" type mac configuration return from ENET_GetDefaultConfig
 *        can be used directly. It is also possible to verify the Mac configuration using other methods.
 * param bufferConfig  ENET buffer configuration structure pointer.
 *        The buffer configuration should be prepared for ENET Initialization.
 *        It is the start address of "ringNum" enet_buffer_config structures.
 *        To support added multi-ring features in some soc and compatible with the previous
 *        enet driver version. For single ring supported, this bufferConfig is a buffer
 *        configure structure pointer, for multi-ring supported and used case, this bufferConfig
 *        pointer should be a buffer configure structure array pointer.
 * param macAddr  ENET mac address of Ethernet device. This MAC address should be
 *        provided.
 * param srcClock_Hz The internal module clock source for MII clock.
 *
 * note ENET has two buffer descriptors legacy buffer descriptors and
 * enhanced IEEE 1588 buffer descriptors. The legacy descriptor is used by default. To
 * use the IEEE 1588 feature, use the enhanced IEEE 1588 buffer descriptor
 * by defining "ENET_ENHANCEDBUFFERDESCRIPTOR_MODE" and calling ENET_Ptp1588Configure()
 * to configure the 1588 feature and related buffers after calling ENET_Init().
 */
void ENET_Init(ENET_Type *base,
               enet_handle_t *handle,
               const enet_config_t *config,
               const enet_buffer_config_t *bufferConfig,
               uint8_t *macAddr,
               uint32_t srcClock_Hz)
{
    /* Checks input parameters. */
    assert(handle);
    assert(config);
    assert(bufferConfig);
    assert(macAddr);
    assert(config->ringNum <= FSL_FEATURE_ENET_QUEUE);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    uint32_t instance = ENET_GetInstance(base);

    /* Ungate ENET clock. */
    CLOCK_EnableClock(s_enetClock[instance]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
    /* Reset ENET module. */
    ENET_Reset(base);

    /* Initializes the ENET transmit buffer descriptors. */
    ENET_SetTxBufferDescriptors(handle, config, bufferConfig);

    /* Initializes the ENET receive buffer descriptors. */
    ENET_SetRxBufferDescriptors(handle, config, bufferConfig);

    /* Initializes the ENET MAC controller with basic function. */
    ENET_SetMacController(base, handle, config, bufferConfig, macAddr, srcClock_Hz);

    /* Set all buffers or data in handler for data transmit/receive process. */
    ENET_SetHandler(base, handle, config, bufferConfig);
}

/*!
 * brief Deinitializes the ENET module.

 * This function gates the module clock, clears ENET interrupts, and disables the ENET module.
 *
 * param base  ENET peripheral base address.
 */
void ENET_Deinit(ENET_Type *base)
{
    /* Disable interrupt. */
    base->EIMR = 0;

    /* Disable ENET. */
    base->ECR &= ~ENET_ECR_ETHEREN_MASK;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Disables the clock source. */
    CLOCK_DisableClock(s_enetClock[ENET_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Sets the callback function.
 * This API is provided for the application callback required case when ENET
 * interrupt is enabled. This API should be called after calling ENET_Init.
 *
 * param handle ENET handler pointer. Should be provided by application.
 * param callback The ENET callback function.
 * param userData The callback function parameter.
 */
void ENET_SetCallback(enet_handle_t *handle, enet_callback_t callback, void *userData)
{
    assert(handle);

    /* Set callback and userData. */
    handle->callback = callback;
    handle->userData = userData;
}

static void ENET_SetHandler(ENET_Type *base,
                            enet_handle_t *handle,
                            const enet_config_t *config,
                            const enet_buffer_config_t *bufferConfig)
{
    uint8_t count;
    uint32_t instance                   = ENET_GetInstance(base);
    const enet_buffer_config_t *buffCfg = bufferConfig;

    /* Store transfer parameters in handle pointer. */
    memset(handle, 0, sizeof(enet_handle_t));

    handle->ringNum = (config->ringNum > FSL_FEATURE_ENET_QUEUE) ? FSL_FEATURE_ENET_QUEUE : config->ringNum;
    for (count = 0; count < handle->ringNum; count++)
    {
        assert(buffCfg->rxBuffSizeAlign * buffCfg->rxBdNumber > config->rxMaxFrameLen);

        handle->rxBdBase[count]        = buffCfg->rxBdStartAddrAlign;
        handle->rxBdCurrent[count]     = buffCfg->rxBdStartAddrAlign;
        handle->rxBuffSizeAlign[count] = buffCfg->rxBuffSizeAlign;
        handle->txBdBase[count]        = buffCfg->txBdStartAddrAlign;
        handle->txBdCurrent[count]     = buffCfg->txBdStartAddrAlign;
        handle->txBuffSizeAlign[count] = buffCfg->txBuffSizeAlign;
        buffCfg++;
    }

    /* Save the handle pointer in the global variables. */
    s_ENETHandle[instance] = handle;

    /* Set the IRQ handler when the interrupt is enabled. */
    if (config->interrupt & ENET_TX_INTERRUPT)
    {
        s_enetTxIsr = ENET_TransmitIRQHandler;
        EnableIRQ(s_enetTxIrqId[instance]);
    }
    if (config->interrupt & ENET_RX_INTERRUPT)
    {
        s_enetRxIsr = ENET_ReceiveIRQHandler;
        EnableIRQ(s_enetRxIrqId[instance]);
    }
    if (config->interrupt & ENET_ERR_INTERRUPT)
    {
        s_enetErrIsr = ENET_ErrorIRQHandler;
        EnableIRQ(s_enetErrIrqId[instance]);
    }
}

static void ENET_SetMacController(ENET_Type *base,
                                  enet_handle_t *handle,
                                  const enet_config_t *config,
                                  const enet_buffer_config_t *bufferConfig,
                                  uint8_t *macAddr,
                                  uint32_t srcClock_Hz)
{
#if defined(FSL_FEATURE_ENET_HAS_AVB) && FSL_FEATURE_ENET_HAS_AVB
    /* Check the MII mode/speed/duplex setting. */
    if (config->miiSpeed == kENET_MiiSpeed1000M)
    {
        /* Only RGMII mode has the 1000M bit/s. The 1000M only support full duplex. */
        assert(config->miiMode == kENET_RgmiiMode);
        assert(config->miiDuplex == kENET_MiiFullDuplex);
    }
#endif /* FSL_FEATURE_ENET_HAS_AVB */

    uint32_t rcr              = 0;
    uint32_t tcr              = 0;
    uint32_t ecr              = base->ECR;
    uint32_t macSpecialConfig = config->macSpecialConfig;
    uint32_t maxFrameLen      = config->rxMaxFrameLen;

    /* Maximum frame length check. */
    if ((macSpecialConfig & kENET_ControlVLANTagEnable) && (maxFrameLen <= ENET_FRAME_MAX_FRAMELEN))
    {
        maxFrameLen = (ENET_FRAME_MAX_FRAMELEN + ENET_FRAME_VLAN_TAGLEN);
#if defined(FSL_FEATURE_ENET_HAS_AVB) && FSL_FEATURE_ENET_HAS_AVB
        if (macSpecialConfig & kENET_ControlSVLANEnable)
        {
            /* Double vlan tag (SVLAN) supported. */
            maxFrameLen += ENET_FRAME_VLAN_TAGLEN;
        }
        ecr |= ((macSpecialConfig & kENET_ControlSVLANEnable) ? (ENET_ECR_SVLANEN_MASK | ENET_ECR_SVLANDBL_MASK) : 0) |
               ((macSpecialConfig & kENET_ControlVLANUseSecondTag) ? ENET_ECR_VLANUSE2ND_MASK : 0);
#endif /* FSL_FEATURE_ENET_HAS_AVB */
    }

    /* Configures MAC receive controller with user configure structure. */
    rcr = ((macSpecialConfig & kENET_ControlRxPayloadCheckEnable) ? ENET_RCR_NLC_MASK : 0) |
          ((macSpecialConfig & kENET_ControlFlowControlEnable) ? ENET_RCR_CFEN_MASK : 0) |
          ((macSpecialConfig & kENET_ControlFlowControlEnable) ? ENET_RCR_FCE_MASK : 0) |
          ((macSpecialConfig & kENET_ControlRxPadRemoveEnable) ? ENET_RCR_PADEN_MASK : 0) |
          ((macSpecialConfig & kENET_ControlRxBroadCastRejectEnable) ? ENET_RCR_BC_REJ_MASK : 0) |
          ((macSpecialConfig & kENET_ControlPromiscuousEnable) ? ENET_RCR_PROM_MASK : 0) |
          ENET_RCR_MAX_FL(maxFrameLen) | ENET_RCR_CRCFWD_MASK;

/* Set the RGMII or RMII, MII mode and control register. */
#if defined(FSL_FEATURE_ENET_HAS_AVB) && FSL_FEATURE_ENET_HAS_AVB
    if (config->miiMode == kENET_RgmiiMode)
    {
        rcr |= ENET_RCR_RGMII_EN_MASK;
        rcr &= ~ENET_RCR_MII_MODE_MASK;
    }
    else
    {
        rcr &= ~ENET_RCR_RGMII_EN_MASK;
#endif /* FSL_FEATURE_ENET_HAS_AVB */
        rcr |= ENET_RCR_MII_MODE_MASK;
        if (config->miiMode == kENET_RmiiMode)
        {
            rcr |= ENET_RCR_RMII_MODE_MASK;
        }
#if defined(FSL_FEATURE_ENET_HAS_AVB) && FSL_FEATURE_ENET_HAS_AVB
    }
#endif /* FSL_FEATURE_ENET_HAS_AVB */
    /* Speed. */
    if (config->miiSpeed == kENET_MiiSpeed10M)
    {
        rcr |= ENET_RCR_RMII_10T_MASK;
    }
#if defined(FSL_FEATURE_ENET_HAS_AVB) && FSL_FEATURE_ENET_HAS_AVB
    if (config->miiSpeed == kENET_MiiSpeed1000M)
    {
        ecr |= ENET_ECR_SPEED_MASK;
    }
#endif /* FSL_FEATURE_ENET_HAS_AVB */

    /* Receive setting for half duplex. */
    if (config->miiDuplex == kENET_MiiHalfDuplex)
    {
        rcr |= ENET_RCR_DRT_MASK;
    }
    /* Sets internal loop only for MII mode. */
    if ((config->macSpecialConfig & kENET_ControlMIILoopEnable) && (config->miiMode != kENET_RmiiMode))
    {
        rcr |= ENET_RCR_LOOP_MASK;
        rcr &= ~ENET_RCR_DRT_MASK;
    }
    base->RCR = rcr;

    /* Configures MAC transmit controller: duplex mode, mac address insertion. */
    tcr = base->TCR & ~(ENET_TCR_FDEN_MASK | ENET_TCR_ADDINS_MASK);
    tcr |= (config->miiDuplex ? ENET_TCR_FDEN_MASK : 0) |
           ((macSpecialConfig & kENET_ControlMacAddrInsert) ? ENET_TCR_ADDINS_MASK : 0);
    base->TCR = tcr;

    /* Configures receive and transmit accelerator. */
    base->TACC = config->txAccelerConfig;
    base->RACC = config->rxAccelerConfig;

    /* Sets the pause duration and FIFO threshold for the flow control enabled case. */
    if (macSpecialConfig & kENET_ControlFlowControlEnable)
    {
        uint32_t reemReg;
        base->OPD = config->pauseDuration;
        reemReg   = ENET_RSEM_RX_SECTION_EMPTY(config->rxFifoEmptyThreshold);
#if defined(FSL_FEATURE_ENET_HAS_RECEIVE_STATUS_THRESHOLD) && FSL_FEATURE_ENET_HAS_RECEIVE_STATUS_THRESHOLD
        reemReg |= ENET_RSEM_STAT_SECTION_EMPTY(config->rxFifoStatEmptyThreshold);
#endif /* FSL_FEATURE_ENET_HAS_RECEIVE_STATUS_THRESHOLD */
        base->RSEM = reemReg;
    }

    /* FIFO threshold setting for store and forward enable/disable case. */
    if (macSpecialConfig & kENET_ControlStoreAndFwdDisable)
    {
        /* Transmit fifo watermark settings. */
        base->TFWR = config->txFifoWatermark & ENET_TFWR_TFWR_MASK;
        /* Receive fifo full threshold settings. */
        base->RSFL = config->rxFifoFullThreshold & ENET_RSFL_RX_SECTION_FULL_MASK;
    }
    else
    {
        /* Transmit fifo watermark settings. */
        base->TFWR = ENET_TFWR_STRFWD_MASK;
        base->RSFL = 0;
    }

    /* Enable store and forward when accelerator is enabled */
    if (config->txAccelerConfig & (kENET_TxAccelIpCheckEnabled | kENET_TxAccelProtoCheckEnabled))
    {
        base->TFWR = ENET_TFWR_STRFWD_MASK;
    }
    if (config->rxAccelerConfig & (kENET_RxAccelIpCheckEnabled | kENET_RxAccelProtoCheckEnabled))
    {
        base->RSFL = 0;
    }

/* Initializes the ring 0. */
#if defined(FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET) && FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET
    base->TDSR = MEMORY_ConvertMemoryMapAddress((uint32_t)bufferConfig->txBdStartAddrAlign, kMEMORY_Local2DMA);
    base->RDSR = MEMORY_ConvertMemoryMapAddress((uint32_t)bufferConfig->rxBdStartAddrAlign, kMEMORY_Local2DMA);
#else
    base->TDSR      = (uint32_t)bufferConfig->txBdStartAddrAlign;
    base->RDSR      = (uint32_t)bufferConfig->rxBdStartAddrAlign;
#endif
    base->MRBR = bufferConfig->rxBuffSizeAlign;

#if defined(FSL_FEATURE_ENET_HAS_AVB) && FSL_FEATURE_ENET_HAS_AVB
    const enet_buffer_config_t *buffCfg = bufferConfig;

    if (config->ringNum > 1)
    {
        /* Initializes the ring 1. */
        buffCfg++;
#if defined(FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET) && FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET
        base->TDSR1 = MEMORY_ConvertMemoryMapAddress((uint32_t)buffCfg->txBdStartAddrAlign, kMEMORY_Local2DMA);
        base->RDSR1 = MEMORY_ConvertMemoryMapAddress((uint32_t)buffCfg->rxBdStartAddrAlign, kMEMORY_Local2DMA);
#else
        base->TDSR1 = (uint32_t)buffCfg->txBdStartAddrAlign;
        base->RDSR1 = (uint32_t)buffCfg->rxBdStartAddrAlign;
#endif
        base->MRBR1 = buffCfg->rxBuffSizeAlign;
        /* Enable the DMAC for ring 1 and with no rx classification set. */
        base->DMACFG[0] = ENET_DMACFG_DMA_CLASS_EN_MASK;
    }
    if (config->ringNum > 2)
    {
        /* Initializes the ring 2. */
        buffCfg++;
#if defined(FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET) && FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET
        base->TDSR2 = MEMORY_ConvertMemoryMapAddress((uint32_t)buffCfg->txBdStartAddrAlign, kMEMORY_Local2DMA);
        base->RDSR2 = MEMORY_ConvertMemoryMapAddress((uint32_t)buffCfg->rxBdStartAddrAlign, kMEMORY_Local2DMA);
#else
        base->TDSR2 = (uint32_t)buffCfg->txBdStartAddrAlign;
        base->RDSR2 = (uint32_t)buffCfg->rxBdStartAddrAlign;
#endif
        base->MRBR2 = buffCfg->rxBuffSizeAlign;
        /* Enable the DMAC for ring 2 and with no rx classification set. */
        base->DMACFG[1] = ENET_DMACFG_DMA_CLASS_EN_MASK;
    }

    /* Default the class/ring 1 and 2 are not enabled and the receive classification is disabled
     * so we set the default transmit scheme with the round-robin mode. beacuse the legacy bd mode
     * only support the round-robin mode. if the avb feature is required, just call the setup avb
     * feature API. */
    base->QOS |= ENET_QOS_TX_SCHEME(1);
#endif /*  FSL_FEATURE_ENET_HAS_AVB */

    /* Configures the Mac address. */
    ENET_SetMacAddr(base, macAddr);

    /* Initialize the SMI if uninitialized. */
    if (!ENET_GetSMI(base))
    {
        ENET_SetSMI(base, srcClock_Hz, !!(config->macSpecialConfig & kENET_ControlSMIPreambleDisable));
    }

/* Enables Ethernet interrupt, enables the interrupt coalsecing if it is required. */
#if defined(FSL_FEATURE_ENET_HAS_INTERRUPT_COALESCE) && FSL_FEATURE_ENET_HAS_INTERRUPT_COALESCE
    if (config->intCoalesceCfg)
    {
        uint32_t intMask = (ENET_EIMR_TXB_MASK | ENET_EIMR_RXB_MASK);

#if FSL_FEATURE_ENET_QUEUE > 1
        uint8_t queue = 0;
        intMask |= ENET_EIMR_TXB2_MASK | ENET_EIMR_RXB2_MASK | ENET_EIMR_TXB1_MASK | ENET_EIMR_RXB1_MASK;
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */

        /* Clear all buffer interrupts. */
        base->EIMR &= ~intMask;

/* Set the interrupt coalescence. */
#if FSL_FEATURE_ENET_QUEUE > 1
        for (queue = 0; queue < FSL_FEATURE_ENET_QUEUE; queue++)
        {
            base->TXIC[queue] = ENET_TXIC_ICFT(config->intCoalesceCfg->txCoalesceFrameCount[queue]) |
                                config->intCoalesceCfg->txCoalesceTimeCount[queue] | ENET_TXIC_ICCS_MASK |
                                ENET_TXIC_ICEN_MASK;
            base->RXIC[queue] = ENET_RXIC_ICFT(config->intCoalesceCfg->rxCoalesceFrameCount[queue]) |
                                config->intCoalesceCfg->rxCoalesceTimeCount[queue] | ENET_RXIC_ICCS_MASK |
                                ENET_RXIC_ICEN_MASK;
        }
#else
        base->TXIC  = ENET_TXIC_ICFT(config->intCoalesceCfg->txCoalesceFrameCount[0]) |
                     config->intCoalesceCfg->txCoalesceTimeCount[0] | ENET_TXIC_ICCS_MASK | ENET_TXIC_ICEN_MASK;
        base->RXIC = ENET_RXIC_ICFT(config->intCoalesceCfg->rxCoalesceFrameCount[0]) |
                     config->intCoalesceCfg->rxCoalesceTimeCount[0] | ENET_RXIC_ICCS_MASK | ENET_RXIC_ICEN_MASK;
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */
    }
#endif /* FSL_FEATURE_ENET_HAS_INTERRUPT_COALESCE */
    ENET_EnableInterrupts(base, config->interrupt);

#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
    /* Sets the 1588 enhanced feature. */
    ecr |= ENET_ECR_EN1588_MASK;
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */
    /* Enables Ethernet module after all configuration except the buffer descriptor active. */
    ecr |= ENET_ECR_ETHEREN_MASK | ENET_ECR_DBSWP_MASK;
    base->ECR = ecr;
}

static void ENET_SetTxBufferDescriptors(enet_handle_t *handle,
                                        const enet_config_t *config,
                                        const enet_buffer_config_t *bufferConfig)
{
    assert(config);
    assert(bufferConfig);

    /* Default single ring is supported. */
    uint8_t ringNum;
    uint32_t count;
    uint32_t txBuffSizeAlign;
    uint8_t *txBuffer;
    const enet_buffer_config_t *buffCfg = bufferConfig;

    /* Check the input parameters. */
    for (ringNum = 0; ringNum < config->ringNum; ringNum++)
    {
        if ((buffCfg->txBdStartAddrAlign > 0) && (buffCfg->txBufferAlign > 0))
        {
            volatile enet_tx_bd_struct_t *curBuffDescrip = buffCfg->txBdStartAddrAlign;
            txBuffSizeAlign                              = buffCfg->txBuffSizeAlign;
#if defined(FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET) && FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET
            txBuffer = (uint8_t *)MEMORY_ConvertMemoryMapAddress((uint32_t)buffCfg->txBufferAlign, kMEMORY_Local2DMA);
#else
            txBuffer = buffCfg->txBufferAlign;
#endif
            for (count = 0; count < buffCfg->txBdNumber; count++)
            {
                /* Set data buffer address. */
                curBuffDescrip->buffer = (uint8_t *)((uint32_t)&txBuffer[count * txBuffSizeAlign]);
                /* Initializes data length. */
                curBuffDescrip->length = 0;
                /* Sets the crc. */
                curBuffDescrip->control = ENET_BUFFDESCRIPTOR_TX_TRANMITCRC_MASK;
                /* Sets the last buffer descriptor with the wrap flag. */
                if (count == buffCfg->txBdNumber - 1)
                {
                    curBuffDescrip->control |= ENET_BUFFDESCRIPTOR_TX_WRAP_MASK;
                }

#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
                /* Enable transmit interrupt for store the transmit timestamp. */
                curBuffDescrip->controlExtend1 |= ENET_BUFFDESCRIPTOR_TX_INTERRUPT_MASK;
#if defined(FSL_FEATURE_ENET_HAS_AVB) && FSL_FEATURE_ENET_HAS_AVB
                /* Set the type of the frame when the credit-based scheme is used. */
                curBuffDescrip->controlExtend1 |= ENET_BD_FTYPE(ringNum);
#endif /* FSL_FEATURE_ENET_HAS_AVB */
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */
                /* Increase the index. */
                curBuffDescrip++;
            }
        }
        buffCfg++;
    }
}

static void ENET_SetRxBufferDescriptors(enet_handle_t *handle,
                                        const enet_config_t *config,
                                        const enet_buffer_config_t *bufferConfig)
{
    assert(config);
    assert(bufferConfig);

    /* Default single ring is supported. */
    uint8_t ringNum;
    uint32_t count;
    uint32_t rxBuffSizeAlign;
    uint8_t *rxBuffer;
    const enet_buffer_config_t *buffCfg = bufferConfig;
#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
    uint32_t mask = (kENET_RxFrameInterrupt | kENET_RxBufferInterrupt);
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */

    /* Check the input parameters. */
    for (ringNum = 0; ringNum < config->ringNum; ringNum++)
    {
        assert(buffCfg->rxBuffSizeAlign >= ENET_RX_MIN_BUFFERSIZE);
#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
#if FSL_FEATURE_ENET_QUEUE > 1
        if (ringNum == 1)
        {
            mask = (kENET_RxFrame1Interrupt | kENET_RxBuffer1Interrupt);
        }
        else if (ringNum == 2)
        {
            mask = (kENET_RxFrame2Interrupt | kENET_RxBuffer2Interrupt);
        }
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */

        if ((buffCfg->rxBdStartAddrAlign > 0) && (buffCfg->rxBufferAlign > 0))
        {
            volatile enet_rx_bd_struct_t *curBuffDescrip = buffCfg->rxBdStartAddrAlign;
            rxBuffSizeAlign                              = buffCfg->rxBuffSizeAlign;
#if defined(FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET) && FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET
            rxBuffer = (uint8_t *)MEMORY_ConvertMemoryMapAddress((uint32_t)buffCfg->rxBufferAlign, kMEMORY_Local2DMA);
#else
            rxBuffer = buffCfg->rxBufferAlign;
#endif

#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL
            /* Invalidate rx buffers before DMA transfer data into them. */
            DCACHE_InvalidateByRange((uint32_t)rxBuffer, (buffCfg->rxBdNumber * rxBuffSizeAlign));
#endif /* FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL */

            for (count = 0; count < buffCfg->rxBdNumber; count++)
            {
                /* Set data buffer and the length. */
                curBuffDescrip->buffer = (uint8_t *)((uint32_t)&rxBuffer[count * rxBuffSizeAlign]);
                curBuffDescrip->length = 0;

                /* Initializes the buffer descriptors with empty bit. */
                curBuffDescrip->control = ENET_BUFFDESCRIPTOR_RX_EMPTY_MASK;
                /* Sets the last buffer descriptor with the wrap flag. */
                if (count == buffCfg->rxBdNumber - 1)
                {
                    curBuffDescrip->control |= ENET_BUFFDESCRIPTOR_RX_WRAP_MASK;
                }

#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
                if (config->interrupt & mask)
                {
                    /* Enable receive interrupt. */
                    curBuffDescrip->controlExtend1 |= ENET_BUFFDESCRIPTOR_RX_INTERRUPT_MASK;
                }
                else
                {
                    curBuffDescrip->controlExtend1 = 0;
                }
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */
                /* Increase the index. */
                curBuffDescrip++;
            }
        }
        buffCfg++;
    }
}

static void ENET_ActiveSend(ENET_Type *base, uint32_t ringId)
{
    assert(ringId < FSL_FEATURE_ENET_QUEUE);

    /* Ensure previous data update is completed with Data Synchronization Barrier before activing Tx BD. */
    __DSB();

    switch (ringId)
    {
        case kENET_Ring0:
            base->TDAR = ENET_TDAR_TDAR_MASK;
            break;
#if FSL_FEATURE_ENET_QUEUE > 1
        case kENET_Ring1:
            base->TDAR1 = ENET_TDAR1_TDAR_MASK;
            break;
        case kENET_Ring2:
            base->TDAR2 = ENET_TDAR2_TDAR_MASK;
            break;
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */
        default:
            base->TDAR = ENET_TDAR_TDAR_MASK;
            break;
    }
}

/*!
 * brief Sets the ENET MII speed and duplex.
 *
 * This API is provided to dynamically change the speed and dulpex for MAC.
 *
 * param base  ENET peripheral base address.
 * param speed The speed of the RMII mode.
 * param duplex The duplex of the RMII mode.
 */
void ENET_SetMII(ENET_Type *base, enet_mii_speed_t speed, enet_mii_duplex_t duplex)
{
    uint32_t rcr = base->RCR;
    uint32_t tcr = base->TCR;

#if defined(FSL_FEATURE_ENET_HAS_AVB) && FSL_FEATURE_ENET_HAS_AVB
    uint32_t ecr = base->ECR;

    if (kENET_MiiSpeed1000M == speed)
    {
        assert(duplex == kENET_MiiFullDuplex);
        ecr |= ENET_ECR_SPEED_MASK;
    }
    else
    {
        ecr &= ~ENET_ECR_SPEED_MASK;
    }

    base->ECR = ecr;
#endif /* FSL_FEATURE_ENET_HAS_AVB */

    /* Sets speed mode. */
    if (kENET_MiiSpeed10M == speed)
    {
        rcr |= ENET_RCR_RMII_10T_MASK;
    }
    else
    {
        rcr &= ~ENET_RCR_RMII_10T_MASK;
    }
    /* Set duplex mode. */
    if (duplex == kENET_MiiHalfDuplex)
    {
        rcr |= ENET_RCR_DRT_MASK;
        tcr &= ~ENET_TCR_FDEN_MASK;
    }
    else
    {
        rcr &= ~ENET_RCR_DRT_MASK;
        tcr |= ENET_TCR_FDEN_MASK;
    }

    base->RCR = rcr;
    base->TCR = tcr;
}

/*!
 * brief Sets the ENET module Mac address.
 *
 * param base  ENET peripheral base address.
 * param macAddr The six-byte Mac address pointer.
 *        The pointer is allocated by application and input into the API.
 */
void ENET_SetMacAddr(ENET_Type *base, uint8_t *macAddr)
{
    uint32_t address;

    /* Set physical address lower register. */
    address = (uint32_t)(((uint32_t)macAddr[0] << 24U) | ((uint32_t)macAddr[1] << 16U) | ((uint32_t)macAddr[2] << 8U) |
                         (uint32_t)macAddr[3]);
    base->PALR = address;
    /* Set physical address high register. */
    address    = (uint32_t)(((uint32_t)macAddr[4] << 8U) | ((uint32_t)macAddr[5]));
    base->PAUR = address << ENET_PAUR_PADDR2_SHIFT;
}

/*!
 * brief Gets the ENET module Mac address.
 *
 * param base  ENET peripheral base address.
 * param macAddr The six-byte Mac address pointer.
 *        The pointer is allocated by application and input into the API.
 */
void ENET_GetMacAddr(ENET_Type *base, uint8_t *macAddr)
{
    assert(macAddr);

    uint32_t address;

    /* Get from physical address lower register. */
    address    = base->PALR;
    macAddr[0] = 0xFFU & (address >> 24U);
    macAddr[1] = 0xFFU & (address >> 16U);
    macAddr[2] = 0xFFU & (address >> 8U);
    macAddr[3] = 0xFFU & address;

    /* Get from physical address high register. */
    address    = (base->PAUR & ENET_PAUR_PADDR2_MASK) >> ENET_PAUR_PADDR2_SHIFT;
    macAddr[4] = 0xFFU & (address >> 8U);
    macAddr[5] = 0xFFU & address;
}

/*!
 * brief Sets the ENET SMI(serial management interface)- MII management interface.
 *
 * param base  ENET peripheral base address.
 * param srcClock_Hz This is the ENET module clock frequency. Normally it's the system clock. See clock distribution.
 * param isPreambleDisabled The preamble disable flag.
 *        - true   Enables the preamble.
 *        - false  Disables the preamble.
 */
void ENET_SetSMI(ENET_Type *base, uint32_t srcClock_Hz, bool isPreambleDisabled)
{
    assert(srcClock_Hz);

    uint32_t clkCycle = 0;
    uint32_t speed    = 0;
    uint32_t mscr     = 0;

    /* Calculate the MII speed which controls the frequency of the MDC. */
    speed = srcClock_Hz / (2 * ENET_MDC_FREQUENCY);
    /* Calculate the hold time on the MDIO output. */
    clkCycle = (10 + ENET_NANOSECOND_ONE_SECOND / srcClock_Hz - 1) / (ENET_NANOSECOND_ONE_SECOND / srcClock_Hz) - 1;
    /* Build the configuration for MDC/MDIO control. */
    mscr =
        ENET_MSCR_MII_SPEED(speed) | ENET_MSCR_HOLDTIME(clkCycle) | (isPreambleDisabled ? ENET_MSCR_DIS_PRE_MASK : 0);
    base->MSCR = mscr;
}

/*!
 * brief Starts an SMI write command.
 *
 * Used for standard IEEE802.3 MDIO Clause 22 format.
 *
 * param base  ENET peripheral base address.
 * param phyAddr The PHY address.
 * param phyReg The PHY register. Range from 0 ~ 31.
 * param operation The write operation.
 * param data The data written to PHY.
 */
void ENET_StartSMIWrite(ENET_Type *base, uint32_t phyAddr, uint32_t phyReg, enet_mii_write_t operation, uint32_t data)
{
    uint32_t mmfr = 0;

    /* Build MII write command. */
    mmfr = ENET_MMFR_ST(1) | ENET_MMFR_OP(operation) | ENET_MMFR_PA(phyAddr) | ENET_MMFR_RA(phyReg) | ENET_MMFR_TA(2) |
           (data & 0xFFFF);
    base->MMFR = mmfr;
}

/*!
 * brief Starts an SMI (Serial Management Interface) read command.
 *
 * Used for standard IEEE802.3 MDIO Clause 22 format.
 *
 * param base  ENET peripheral base address.
 * param phyAddr The PHY address.
 * param phyReg The PHY register. Range from 0 ~ 31.
 * param operation The read operation.
 */
void ENET_StartSMIRead(ENET_Type *base, uint32_t phyAddr, uint32_t phyReg, enet_mii_read_t operation)
{
    uint32_t mmfr = 0;

    /* Build MII read command. */
    mmfr = ENET_MMFR_ST(1) | ENET_MMFR_OP(operation) | ENET_MMFR_PA(phyAddr) | ENET_MMFR_RA(phyReg) | ENET_MMFR_TA(2);
    base->MMFR = mmfr;
}

#if defined(FSL_FEATURE_ENET_HAS_EXTEND_MDIO) && FSL_FEATURE_ENET_HAS_EXTEND_MDIO
/*!
 * brief Starts the extended IEEE802.3 Clause 45 MDIO format SMI write command.
 *
 * param base  ENET peripheral base address.
 * param phyAddr The PHY address.
 * param phyReg The PHY register. For MDIO IEEE802.3 Clause 45,
 *        the phyReg is a 21-bits combination of the devaddr (5 bits device address)
 *        and the regAddr (16 bits phy register): phyReg = (devaddr << 16) | regAddr.
 * param data The data written to PHY.
 */
void ENET_StartExtC45SMIWrite(ENET_Type *base, uint32_t phyAddr, uint32_t phyReg, uint32_t data)
{
    uint32_t mmfr = 0;

    /* Parse the address from the input register. */
    uint16_t devAddr = (phyReg >> ENET_MMFR_TA_SHIFT) & 0x1FU;
    uint16_t regAddr = (uint16_t)(phyReg & 0xFFFFU);

    /* Address write firstly. */
    mmfr = ENET_MMFR_ST(0) | ENET_MMFR_OP(kENET_MiiAddrWrite_C45) | ENET_MMFR_PA(phyAddr) | ENET_MMFR_RA(devAddr) |
           ENET_MMFR_TA(2) | ENET_MMFR_DATA(regAddr);
    base->MMFR = mmfr;

    /* Build MII write command. */
    mmfr = ENET_MMFR_ST(0) | ENET_MMFR_OP(kENET_MiiWriteFrame_C45) | ENET_MMFR_PA(phyAddr) | ENET_MMFR_RA(devAddr) |
           ENET_MMFR_TA(2) | ENET_MMFR_DATA(data);
    base->MMFR = mmfr;
}

/*!
 * brief Starts the extended IEEE802.3 Clause 45 MDIO format SMI read command.
 *
 * param base  ENET peripheral base address.
 * param phyAddr The PHY address.
 * param phyReg The PHY register. For MDIO IEEE802.3 Clause 45,
 *        the phyReg is a 21-bits combination of the devaddr (5 bits device address)
 *        and the regAddr (16 bits phy register): phyReg = (devaddr << 16) | regAddr.
 */
void ENET_StartExtC45SMIRead(ENET_Type *base, uint32_t phyAddr, uint32_t phyReg)
{
    uint32_t mmfr = 0;

    /* Parse the address from the input register. */
    uint16_t devAddr = (phyReg >> ENET_MMFR_TA_SHIFT) & 0x1FU;
    uint16_t regAddr = (uint16_t)(phyReg & 0xFFFFU);

    /* Address write firstly. */
    mmfr = ENET_MMFR_ST(0) | ENET_MMFR_OP(kENET_MiiAddrWrite_C45) | ENET_MMFR_PA(phyAddr) | ENET_MMFR_RA(devAddr) |
           ENET_MMFR_TA(2) | ENET_MMFR_DATA(regAddr);
    base->MMFR = mmfr;

    /* Build MII read command. */
    mmfr = ENET_MMFR_ST(0) | ENET_MMFR_OP(kENET_MiiReadFrame_C45) | ENET_MMFR_PA(phyAddr) | ENET_MMFR_RA(devAddr) |
           ENET_MMFR_TA(2);
    base->MMFR = mmfr;
}
#endif /* FSL_FEATURE_ENET_HAS_EXTEND_MDIO */

/*!
 * brief Gets the error statistics of a received frame for ENET single ring.
 *
 * This API must be called after the ENET_GetRxFrameSize and before the ENET_ReadFrame().
 * If the ENET_GetRxFrameSize returns kStatus_ENET_RxFrameError,
 * the ENET_GetRxErrBeforeReadFrame can be used to get the exact error statistics.
 * This is an example.
 * code
 *       status = ENET_GetRxFrameSize(&g_handle, &length);
 *       if (status == kStatus_ENET_RxFrameError)
 *       {
 *           // Get the error information of the received frame.
 *           ENET_GetRxErrBeforeReadFrame(&g_handle, &eErrStatic);
 *           // update the receive buffer.
 *           ENET_ReadFrame(EXAMPLE_ENET, &g_handle, NULL, 0);
 *       }
 * endcode
 * param handle The ENET handler structure pointer. This is the same handler pointer used in the ENET_Init.
 * param eErrorStatic The error statistics structure pointer.
 */
void ENET_GetRxErrBeforeReadFrame(enet_handle_t *handle, enet_data_error_stats_t *eErrorStatic)
{
    assert(handle);
    assert(handle->rxBdCurrent[0]);
    assert(eErrorStatic);

    uint16_t control                             = 0;
    volatile enet_rx_bd_struct_t *curBuffDescrip = handle->rxBdCurrent[0];

    do
    {
        /* The last buffer descriptor of a frame. */
        if (curBuffDescrip->control & ENET_BUFFDESCRIPTOR_RX_LAST_MASK)
        {
            control = curBuffDescrip->control;
            if (control & ENET_BUFFDESCRIPTOR_RX_TRUNC_MASK)
            {
                /* The receive truncate error. */
                eErrorStatic->statsRxTruncateErr++;
            }
            if (control & ENET_BUFFDESCRIPTOR_RX_OVERRUN_MASK)
            {
                /* The receive over run error. */
                eErrorStatic->statsRxOverRunErr++;
            }
            if (control & ENET_BUFFDESCRIPTOR_RX_LENVLIOLATE_MASK)
            {
                /* The receive length violation error. */
                eErrorStatic->statsRxLenGreaterErr++;
            }
            if (control & ENET_BUFFDESCRIPTOR_RX_NOOCTET_MASK)
            {
                /* The receive alignment error. */
                eErrorStatic->statsRxAlignErr++;
            }
            if (control & ENET_BUFFDESCRIPTOR_RX_CRC_MASK)
            {
                /* The receive CRC error. */
                eErrorStatic->statsRxFcsErr++;
            }
#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
            uint16_t controlExt = curBuffDescrip->controlExtend1;
            if (controlExt & ENET_BUFFDESCRIPTOR_RX_MACERR_MASK)
            {
                /* The MAC error. */
                eErrorStatic->statsRxMacErr++;
            }
            if (controlExt & ENET_BUFFDESCRIPTOR_RX_PHYERR_MASK)
            {
                /* The PHY error. */
                eErrorStatic->statsRxPhyErr++;
            }
            if (controlExt & ENET_BUFFDESCRIPTOR_RX_COLLISION_MASK)
            {
                /* The receive collision error. */
                eErrorStatic->statsRxCollisionErr++;
            }
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */

            break;
        }

        /* Increase the buffer descriptor, if it is the last one, increase to first one of the ring buffer. */
        if (curBuffDescrip->control & ENET_BUFFDESCRIPTOR_RX_WRAP_MASK)
        {
            curBuffDescrip = handle->rxBdBase[0];
        }
        else
        {
            curBuffDescrip++;
        }

    } while (curBuffDescrip != handle->rxBdCurrent[0]);
}

/*!
 * brief Gets the size of the read frame for single ring.
 *
 * This function gets a received frame size from the ENET buffer descriptors.
 * note The FCS of the frame is automatically removed by MAC and the size is the length without the FCS.
 * After calling ENET_GetRxFrameSize, ENET_ReadFrame() should be called to update the
 * receive buffers If the result is not "kStatus_ENET_RxFrameEmpty".
 *
 * param handle The ENET handler structure. This is the same handler pointer used in the ENET_Init.
 * param length The length of the valid frame received.
 * retval kStatus_ENET_RxFrameEmpty No frame received. Should not call ENET_ReadFrame to read frame.
 * retval kStatus_ENET_RxFrameError Data error happens. ENET_ReadFrame should be called with NULL data
 *         and NULL length to update the receive buffers.
 * retval kStatus_Success Receive a frame Successfully then the ENET_ReadFrame
 *         should be called with the right data buffer and the captured data length input.
 */
status_t ENET_GetRxFrameSize(enet_handle_t *handle, uint32_t *length)
{
    assert(handle);
    assert(handle->rxBdCurrent[0]);
    assert(length);

    /* Reset the length to zero. */
    *length = 0;

    uint16_t validLastMask                       = ENET_BUFFDESCRIPTOR_RX_LAST_MASK | ENET_BUFFDESCRIPTOR_RX_EMPTY_MASK;
    volatile enet_rx_bd_struct_t *curBuffDescrip = handle->rxBdCurrent[0];

    /* Check the current buffer descriptor's empty flag.  if empty means there is no frame received. */
    if (curBuffDescrip->control & ENET_BUFFDESCRIPTOR_RX_EMPTY_MASK)
    {
        return kStatus_ENET_RxFrameEmpty;
    }

    do
    {
        /* Add check for abnormal case. */
        if ((!(curBuffDescrip->control & ENET_BUFFDESCRIPTOR_RX_EMPTY_MASK)) && (!curBuffDescrip->length))
        {
            return kStatus_ENET_RxFrameError;
        }

        /* Find the last buffer descriptor. */
        if ((curBuffDescrip->control & validLastMask) == ENET_BUFFDESCRIPTOR_RX_LAST_MASK)
        {
            /* The last buffer descriptor in the frame check the status of the received frame. */
            if ((curBuffDescrip->control & ENET_BUFFDESCRIPTOR_RX_ERR_MASK)
#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
                || (curBuffDescrip->controlExtend1 & ENET_BUFFDESCRIPTOR_RX_EXT_ERR_MASK)
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */
            )
            {
                return kStatus_ENET_RxFrameError;
            }
            /* FCS is removed by MAC. */
            *length = curBuffDescrip->length;
            return kStatus_Success;
        }
        /* Increase the buffer descriptor, if it is the last one, increase to first one of the ring buffer. */
        if (curBuffDescrip->control & ENET_BUFFDESCRIPTOR_RX_WRAP_MASK)
        {
            curBuffDescrip = handle->rxBdBase[0];
        }
        else
        {
            curBuffDescrip++;
        }

    } while (curBuffDescrip != handle->rxBdCurrent[0]);

    /* The frame is on processing - set to empty status to make application to receive it next time. */
    return kStatus_ENET_RxFrameEmpty;
}

/*!
 * brief Reads a frame from the ENET device for single ring.
 * This function reads a frame (both the data and the length) from the ENET buffer descriptors.
 * The ENET_GetRxFrameSize should be used to get the size of the prepared data buffer.
 * This is an example:
 * code
 *       uint32_t length;
 *       enet_handle_t g_handle;
 *       //Get the received frame size firstly.
 *       status = ENET_GetRxFrameSize(&g_handle, &length);
 *       if (length != 0)
 *       {
 *           //Allocate memory here with the size of "length"
 *           uint8_t *data = memory allocate interface;
 *           if (!data)
 *           {
 *               ENET_ReadFrame(ENET, &g_handle, NULL, 0);
 *               //Add the console warning log.
 *           }
 *           else
 *           {
 *              status = ENET_ReadFrame(ENET, &g_handle, data, length);
 *              //Call stack input API to deliver the data to stack
 *           }
 *       }
 *       else if (status == kStatus_ENET_RxFrameError)
 *       {
 *          //Update the received buffer when a error frame is received.
 *           ENET_ReadFrame(ENET, &g_handle, NULL, 0);
 *       }
 * endcode
 * param base  ENET peripheral base address.
 * param handle The ENET handler structure. This is the same handler pointer used in the ENET_Init.
 * param data The data buffer provided by user to store the frame which memory size should be at least "length".
 * param length The size of the data buffer which is still the length of the received frame.
 * return The execute status, successful or failure.
 */
status_t ENET_ReadFrame(ENET_Type *base, enet_handle_t *handle, uint8_t *data, uint32_t length)
{
    assert(handle);
    assert(handle->rxBdCurrent[0]);

    uint32_t len    = 0;
    uint32_t offset = 0;
    uint16_t control;
    bool isLastBuff                              = false;
    volatile enet_rx_bd_struct_t *curBuffDescrip = handle->rxBdCurrent[0];
    status_t result                              = kStatus_Success;
    uint32_t address;

    /* For data-NULL input, only update the buffer descriptor. */
    if (!data)
    {
        do
        {
            /* Update the control flag. */
            control = handle->rxBdCurrent[0]->control;
            /* Updates the receive buffer descriptors. */
            ENET_UpdateReadBuffers(base, handle, 0);

            /* Find the last buffer descriptor for the frame. */
            if (control & ENET_BUFFDESCRIPTOR_RX_LAST_MASK)
            {
                break;
            }

        } while (handle->rxBdCurrent[0] != curBuffDescrip);

        return result;
    }
    else
    {
/* A frame on one buffer or several receive buffers are both considered. */
#if defined(FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET) && FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET
        address = MEMORY_ConvertMemoryMapAddress((uint32_t)curBuffDescrip->buffer, kMEMORY_DMA2Local);
#else
        address = (uint32_t)curBuffDescrip->buffer;
#endif /* FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET */
#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL
        /* Add the cache invalidate maintain. */
        DCACHE_InvalidateByRange(address, handle->rxBuffSizeAlign[0]);
#endif /* FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL */
#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
        enet_ptp_time_data_t ptpTimestamp;
        bool isPtpEventMessage = false;
        /* Parse the PTP message according to the header message. */
        isPtpEventMessage = ENET_Ptp1588ParseFrame((uint8_t *)address, &ptpTimestamp, false);
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */

        while (!isLastBuff)
        {
            /* The last buffer descriptor of a frame. */
            if (curBuffDescrip->control & ENET_BUFFDESCRIPTOR_RX_LAST_MASK)
            {
                /* This is a valid frame. */
                isLastBuff = true;
                if (length == curBuffDescrip->length)
                {
                    /* Copy the frame to user's buffer without FCS. */
                    len = curBuffDescrip->length - offset;
                    memcpy(data + offset, (void *)address, len);
#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
                    /* Store the PTP 1588 timestamp for received PTP event frame. */
                    if (isPtpEventMessage)
                    {
                        /* Set the timestamp to the timestamp ring. */
                        ptpTimestamp.timeStamp.nanosecond = curBuffDescrip->timestamp;
                        result                            = ENET_StoreRxFrameTime(base, handle, &ptpTimestamp);
                    }
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */

                    /* Updates the receive buffer descriptors. */
                    ENET_UpdateReadBuffers(base, handle, 0);
                    return result;
                }
                else
                {
                    /* Updates the receive buffer descriptors. */
                    ENET_UpdateReadBuffers(base, handle, 0);
                }
            }
            else
            {
                /* Store a frame on several buffer descriptors. */
                isLastBuff = false;
                /* Length check. */
                if (offset >= length)
                {
                    break;
                }

                memcpy(data + offset, (void *)address, handle->rxBuffSizeAlign[0]);
                offset += handle->rxBuffSizeAlign[0];

                /* Updates the receive buffer descriptors. */
                ENET_UpdateReadBuffers(base, handle, 0);
            }

            /* Get the current buffer descriptor. */
            curBuffDescrip = handle->rxBdCurrent[0];
/* Add the cache invalidate maintain. */
#if defined(FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET) && FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET
            address = MEMORY_ConvertMemoryMapAddress((uint32_t)curBuffDescrip->buffer, kMEMORY_DMA2Local);
#else
            address = (uint32_t)curBuffDescrip->buffer;
#endif /* FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET */
#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL
            DCACHE_InvalidateByRange(address, handle->rxBuffSizeAlign[0]);
#endif /* FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL */
        }
    }

    return kStatus_ENET_RxFrameFail;
}

static void ENET_UpdateReadBuffers(ENET_Type *base, enet_handle_t *handle, uint32_t ringId)
{
    assert(handle);
    assert(ringId < FSL_FEATURE_ENET_QUEUE);

    /* Clears status. */
    handle->rxBdCurrent[ringId]->control &= ENET_BUFFDESCRIPTOR_RX_WRAP_MASK;
    /* Sets the receive buffer descriptor with the empty flag. */
    handle->rxBdCurrent[ringId]->control |= ENET_BUFFDESCRIPTOR_RX_EMPTY_MASK;

    /* Increase current buffer descriptor to the next one. */
    if (handle->rxBdCurrent[ringId]->control & ENET_BUFFDESCRIPTOR_RX_WRAP_MASK)
    {
        handle->rxBdCurrent[ringId] = handle->rxBdBase[ringId];
    }
    else
    {
        handle->rxBdCurrent[ringId]++;
    }

    /* Ensure previous data update is completed with Data Synchronization Barrier before activing Rx BD. */
    __DSB();

    /* Actives the receive buffer descriptor. */
    switch (ringId)
    {
        case kENET_Ring0:
            base->RDAR = ENET_RDAR_RDAR_MASK;
            break;
#if FSL_FEATURE_ENET_QUEUE > 1
        case kENET_Ring1:
            base->RDAR1 = ENET_RDAR1_RDAR_MASK;
            break;
        case kENET_Ring2:
            base->RDAR2 = ENET_RDAR2_RDAR_MASK;
            break;
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */
        default:
            base->RDAR = ENET_RDAR_RDAR_MASK;
            break;
    }
}

/*!
 * brief Transmits an ENET frame for single ring.
 * note The CRC is automatically appended to the data. Input the data
 * to send without the CRC.
 *
 *
 * param base  ENET peripheral base address.
 * param handle The ENET handler pointer. This is the same handler pointer used in the ENET_Init.
 * param data The data buffer provided by user to be send.
 * param length The length of the data to be send.
 * retval kStatus_Success  Send frame succeed.
 * retval kStatus_ENET_TxFrameBusy  Transmit buffer descriptor is busy under transmission.
 *         The transmit busy happens when the data send rate is over the MAC capacity.
 *         The waiting mechanism is recommended to be added after each call return with
 *         kStatus_ENET_TxFrameBusy.
 */
status_t ENET_SendFrame(ENET_Type *base, enet_handle_t *handle, const uint8_t *data, uint32_t length)
{
    assert(handle);
    assert(data);

    volatile enet_tx_bd_struct_t *curBuffDescrip;
    uint32_t len      = 0;
    uint32_t sizeleft = 0;
    uint32_t address;

    /* Check the frame length. */
    if (length > ENET_FRAME_MAX_FRAMELEN)
    {
        return kStatus_ENET_TxFrameOverLen;
    }

    /* Check if the transmit buffer is ready. */
    curBuffDescrip = handle->txBdCurrent[0];
    if (curBuffDescrip->control & ENET_BUFFDESCRIPTOR_TX_READY_MASK)
    {
        return kStatus_ENET_TxFrameBusy;
    }
#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
    bool isPtpEventMessage = false;
    /* Check PTP message with the PTP header. */
    isPtpEventMessage = ENET_Ptp1588ParseFrame(data, NULL, true);
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */
    /* One transmit buffer is enough for one frame. */
    if (handle->txBuffSizeAlign[0] >= length)
    {
/* Copy data to the buffer for uDMA transfer. */
#if defined(FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET) && FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET
        address = MEMORY_ConvertMemoryMapAddress((uint32_t)curBuffDescrip->buffer, kMEMORY_DMA2Local);
#else
        address = (uint32_t)curBuffDescrip->buffer;
#endif /* FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET */
        memcpy((void *)address, data, length);
#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL
        DCACHE_CleanByRange(address, length);
#endif /* FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL */
        /* Set data length. */
        curBuffDescrip->length = length;
#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
        /* For enable the timestamp. */
        if (isPtpEventMessage)
        {
            curBuffDescrip->controlExtend1 |= ENET_BUFFDESCRIPTOR_TX_TIMESTAMP_MASK;
        }
        else
        {
            curBuffDescrip->controlExtend1 &= ~ENET_BUFFDESCRIPTOR_TX_TIMESTAMP_MASK;
        }

#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */
        curBuffDescrip->control |= (ENET_BUFFDESCRIPTOR_TX_READY_MASK | ENET_BUFFDESCRIPTOR_TX_LAST_MASK);

        /* Increase the buffer descriptor address. */
        if (curBuffDescrip->control & ENET_BUFFDESCRIPTOR_TX_WRAP_MASK)
        {
            handle->txBdCurrent[0] = handle->txBdBase[0];
        }
        else
        {
            handle->txBdCurrent[0]++;
        }
        /* Active the transmit buffer descriptor. */
        ENET_ActiveSend(base, 0);

        return kStatus_Success;
    }
    else
    {
        /* One frame requires more than one transmit buffers. */
        do
        {
#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
            /* For enable the timestamp. */
            if (isPtpEventMessage)
            {
                curBuffDescrip->controlExtend1 |= ENET_BUFFDESCRIPTOR_TX_TIMESTAMP_MASK;
            }
            else
            {
                curBuffDescrip->controlExtend1 &= ~ENET_BUFFDESCRIPTOR_TX_TIMESTAMP_MASK;
            }
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */

            /* Increase the buffer descriptor address. */
            if (curBuffDescrip->control & ENET_BUFFDESCRIPTOR_TX_WRAP_MASK)
            {
                handle->txBdCurrent[0] = handle->txBdBase[0];
            }
            else
            {
                handle->txBdCurrent[0]++;
            }
            /* update the size left to be transmit. */
            sizeleft = length - len;
#if defined(FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET) && FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET
            address = MEMORY_ConvertMemoryMapAddress((uint32_t)curBuffDescrip->buffer, kMEMORY_DMA2Local);
#else
            address = (uint32_t)curBuffDescrip->buffer;
#endif /* FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET */
            if (sizeleft > handle->txBuffSizeAlign[0])
            {
                /* Data copy. */
                memcpy((void *)address, data + len, handle->txBuffSizeAlign[0]);
#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL
                /* Add the cache clean maintain. */
                DCACHE_CleanByRange(address, handle->txBuffSizeAlign[0]);
#endif /* FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL */
                /* Data length update. */
                curBuffDescrip->length = handle->txBuffSizeAlign[0];
                len += handle->txBuffSizeAlign[0];
                /* Sets the control flag. */
                curBuffDescrip->control &= ~ENET_BUFFDESCRIPTOR_TX_LAST_MASK;
                curBuffDescrip->control |= ENET_BUFFDESCRIPTOR_TX_READY_MASK;
                /* Active the transmit buffer descriptor*/
                ENET_ActiveSend(base, 0);
            }
            else
            {
                memcpy((void *)address, data + len, sizeleft);
#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL
                /* Add the cache clean maintain. */
                DCACHE_CleanByRange(address, sizeleft);
#endif /* FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL */
                curBuffDescrip->length = sizeleft;
                /* Set Last buffer wrap flag. */
                curBuffDescrip->control |= ENET_BUFFDESCRIPTOR_TX_READY_MASK | ENET_BUFFDESCRIPTOR_TX_LAST_MASK;
                /* Active the transmit buffer descriptor. */
                ENET_ActiveSend(base, 0);

                return kStatus_Success;
            }

            /* Get the current buffer descriptor address. */
            curBuffDescrip = handle->txBdCurrent[0];

        } while (!(curBuffDescrip->control & ENET_BUFFDESCRIPTOR_TX_READY_MASK));

        return kStatus_ENET_TxFrameBusy;
    }
}

#if FSL_FEATURE_ENET_QUEUE > 1
/*!
 * brief Gets the error statistics of received frame for extended multi-ring.
 *
 * This API must be called after the ENET_GetRxFrameSizeMultiRing and before the ENET_ReadFrameMultiRing().
 * If the ENET_GetRxFrameSizeMultiRing returns kStatus_ENET_RxFrameError,
 * the ENET_GetRxErrBeforeReadFrameMultiRing can be used to get the exact error statistics.
 *
 * param handle The ENET handler structure pointer. This is the same handler pointer used in the ENET_Init.
 * param eErrorStatic The error statistics structure pointer.
 * param ringId The ring index, range from 0 ~ FSL_FEATURE_ENET_QUEUE - 1.
 */
void ENET_GetRxErrBeforeReadFrameMultiRing(enet_handle_t *handle,
                                           enet_data_error_stats_t *eErrorStatic,
                                           uint32_t ringId)
{
    assert(handle);
    assert(eErrorStatic);
    assert(ringId < FSL_FEATURE_ENET_QUEUE);

    uint16_t control                             = 0;
    volatile enet_rx_bd_struct_t *curBuffDescrip = handle->rxBdCurrent[ringId];

    do
    {
        /* The last buffer descriptor of a frame. */
        if (curBuffDescrip->control & ENET_BUFFDESCRIPTOR_RX_LAST_MASK)
        {
            control = curBuffDescrip->control;
            if (control & ENET_BUFFDESCRIPTOR_RX_TRUNC_MASK)
            {
                /* The receive truncate error. */
                eErrorStatic->statsRxTruncateErr++;
            }
            if (control & ENET_BUFFDESCRIPTOR_RX_OVERRUN_MASK)
            {
                /* The receive over run error. */
                eErrorStatic->statsRxOverRunErr++;
            }
            if (control & ENET_BUFFDESCRIPTOR_RX_LENVLIOLATE_MASK)
            {
                /* The receive length violation error. */
                eErrorStatic->statsRxLenGreaterErr++;
            }
            if (control & ENET_BUFFDESCRIPTOR_RX_NOOCTET_MASK)
            {
                /* The receive alignment error. */
                eErrorStatic->statsRxAlignErr++;
            }
            if (control & ENET_BUFFDESCRIPTOR_RX_CRC_MASK)
            {
                /* The receive CRC error. */
                eErrorStatic->statsRxFcsErr++;
            }
#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
            uint16_t controlExt = curBuffDescrip->controlExtend1;
            if (controlExt & ENET_BUFFDESCRIPTOR_RX_MACERR_MASK)
            {
                /* The MAC error. */
                eErrorStatic->statsRxMacErr++;
            }
            if (controlExt & ENET_BUFFDESCRIPTOR_RX_PHYERR_MASK)
            {
                /* The PHY error. */
                eErrorStatic->statsRxPhyErr++;
            }
            if (controlExt & ENET_BUFFDESCRIPTOR_RX_COLLISION_MASK)
            {
                /* The receive collision error. */
                eErrorStatic->statsRxCollisionErr++;
            }
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */

            break;
        }

        /* Increase the buffer descriptor, if it is the last one, increase to first one of the ring buffer. */
        if (curBuffDescrip->control & ENET_BUFFDESCRIPTOR_RX_WRAP_MASK)
        {
            curBuffDescrip = handle->rxBdBase[ringId];
        }
        else
        {
            curBuffDescrip++;
        }

    } while (curBuffDescrip != handle->rxBdCurrent[ringId]);
}

/*!
 * brief Gets the size of the read frame for extended mutli-ring.
 *
 * This function gets a received frame size from the ENET buffer descriptors.
 * note The FCS of the frame is automatically removed by MAC and the size is the length without the FCS.
 * After calling ENET_GetRxFrameSizeMultiRing, ENET_ReadFrameMultiRing() should be called to update the
 * receive buffers If the result is not "kStatus_ENET_RxFrameEmpty". The usage is
 * the same to the single ring, refer to ENET_GetRxFrameSize.
 *
 * param handle The ENET handler structure. This is the same handler pointer used in the ENET_Init.
 * param length The length of the valid frame received.
 * param ringId The ring index or ring number;
 * retval kStatus_ENET_RxFrameEmpty No frame received. Should not call ENET_ReadFrameMultiRing to read frame.
 * retval kStatus_ENET_RxFrameError Data error happens. ENET_ReadFrameMultiRing should be called with NULL data
 *         and NULL length to update the receive buffers.
 * retval kStatus_Success Receive a frame Successfully then the ENET_ReadFrame
 *         should be called with the right data buffer and the captured data length input.
 */
status_t ENET_GetRxFrameSizeMultiRing(enet_handle_t *handle, uint32_t *length, uint32_t ringId)
{
    assert(handle);
    assert(length);
    assert(ringId < FSL_FEATURE_ENET_QUEUE);

    /* Reset the length to zero. */
    *length                = 0;
    uint16_t validLastMask = ENET_BUFFDESCRIPTOR_RX_LAST_MASK | ENET_BUFFDESCRIPTOR_RX_EMPTY_MASK;
    volatile enet_rx_bd_struct_t *curBuffDescrip;

    curBuffDescrip = handle->rxBdCurrent[ringId];
    /* Check the current buffer descriptor's empty flag.  if empty means there is no frame received. */
    if (curBuffDescrip->control & ENET_BUFFDESCRIPTOR_RX_EMPTY_MASK)
    {
        return kStatus_ENET_RxFrameEmpty;
    }

    do
    {
        /* Add check for abnormal case. */
        if ((!(curBuffDescrip->control & ENET_BUFFDESCRIPTOR_RX_EMPTY_MASK)) && (!curBuffDescrip->length))
        {
            return kStatus_ENET_RxFrameError;
        }
        /* Find the last buffer descriptor. */
        if ((curBuffDescrip->control & validLastMask) == ENET_BUFFDESCRIPTOR_RX_LAST_MASK)
        {
            /* The last buffer descriptor in the frame check the status of the received frame. */
            if ((curBuffDescrip->control & ENET_BUFFDESCRIPTOR_RX_ERR_MASK)
#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
                || (curBuffDescrip->controlExtend1 & ENET_BUFFDESCRIPTOR_RX_EXT_ERR_MASK)
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */
            )
            {
                return kStatus_ENET_RxFrameError;
            }
            /* FCS is removed by MAC. */
            *length = curBuffDescrip->length;
            return kStatus_Success;
        }
        /* Increase the buffer descriptor, if it is the last one, increase to first one of the ring buffer. */
        if (curBuffDescrip->control & ENET_BUFFDESCRIPTOR_RX_WRAP_MASK)
        {
            curBuffDescrip = handle->rxBdBase[ringId];
        }
        else
        {
            curBuffDescrip++;
        }
    } while (curBuffDescrip != handle->rxBdCurrent[ringId]);

    /* The frame is on processing - set to empty status to make application to receive it next time. */
    return kStatus_ENET_RxFrameEmpty;
}

/*!
 * brief Reads a frame from the ENET device for multi-ring.
 *
 * This function reads a frame (both the data and the length) from the ENET buffer descriptors.
 * The ENET_GetRxFrameSizeMultiRing should be used to get the size of the prepared data buffer.
 * This usage is the same as the single ring, refer to ENET_ReadFrame.

 * param base  ENET peripheral base address.
 * param handle The ENET handler structure. This is the same handler pointer used in the ENET_Init.
 * param data The data buffer provided by user to store the frame which memory size should be at least "length".
 * param length The size of the data buffer which is still the length of the received frame.
 * param ringId The ring index or ring number;
 * return The execute status, successful or failure.
 */
status_t ENET_ReadFrameMultiRing(
    ENET_Type *base, enet_handle_t *handle, uint8_t *data, uint32_t length, uint32_t ringId)
{
    assert(handle);
    assert(ringId < FSL_FEATURE_ENET_QUEUE);

    uint32_t len    = 0;
    uint32_t offset = 0;
    uint16_t control;
    bool isLastBuff                              = false;
    volatile enet_rx_bd_struct_t *curBuffDescrip = handle->rxBdCurrent[ringId];
    status_t result                              = kStatus_Success;
    uint32_t address;

    /* For data-NULL input, only update the buffer descriptor. */
    if (!data)
    {
        do
        {
            /* Update the control flag. */
            control = handle->rxBdCurrent[ringId]->control;
            /* Updates the receive buffer descriptors. */
            ENET_UpdateReadBuffers(base, handle, ringId);

            /* Find the last buffer descriptor for the frame. */
            if (control & ENET_BUFFDESCRIPTOR_RX_LAST_MASK)
            {
                break;
            }

        } while (handle->rxBdCurrent[ringId] != curBuffDescrip);

        return result;
    }
    else
    {
/* A frame on one buffer or several receive buffers are both considered. */
#if defined(FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET) && FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET
        address = MEMORY_ConvertMemoryMapAddress((uint32_t)curBuffDescrip->buffer, kMEMORY_DMA2Local);
#else
        address = (uint32_t)curBuffDescrip->buffer;
#endif /* FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET */
#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL
        /* Add the cache invalidate maintain. */
        DCACHE_InvalidateByRange(address, handle->rxBuffSizeAlign[ringId]);
#endif /* FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL */
#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
        enet_ptp_time_data_t ptpTimestamp;
        bool isPtpEventMessage = false;
        /* Parse the PTP message according to the header message. */
        isPtpEventMessage = ENET_Ptp1588ParseFrame((uint8_t *)address, &ptpTimestamp, false);
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */

        while (!isLastBuff)
        {
            /* The last buffer descriptor of a frame. */
            if (curBuffDescrip->control & ENET_BUFFDESCRIPTOR_RX_LAST_MASK)
            {
                /* This is a valid frame. */
                isLastBuff = true;
                if (length == curBuffDescrip->length)
                {
                    /* Copy the frame to user's buffer without FCS. */
                    len = curBuffDescrip->length - offset;
                    memcpy(data + offset, (void *)address, len);
#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
                    /* Store the PTP 1588 timestamp for received PTP event frame. */
                    if (isPtpEventMessage)
                    {
                        /* Set the timestamp to the timestamp ring. */
                        ptpTimestamp.timeStamp.nanosecond = curBuffDescrip->timestamp;
                        result                            = ENET_StoreRxFrameTime(base, handle, &ptpTimestamp);
                    }
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */

                    /* Updates the receive buffer descriptors. */
                    ENET_UpdateReadBuffers(base, handle, ringId);
                    return result;
                }
                else
                {
                    /* Updates the receive buffer descriptors. */
                    ENET_UpdateReadBuffers(base, handle, ringId);
                }
            }
            else
            {
                /* Store a frame on several buffer descriptors. */
                isLastBuff = false;
                /* Length check. */
                if (offset >= length)
                {
                    break;
                }
                memcpy(data + offset, (void *)address, handle->rxBuffSizeAlign[ringId]);
                offset += handle->rxBuffSizeAlign[ringId];

                /* Updates the receive buffer descriptors. */
                ENET_UpdateReadBuffers(base, handle, ringId);
            }

            /* Get the current buffer descriptor. */

            curBuffDescrip = handle->rxBdCurrent[ringId];
#if defined(FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET) && FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET
            address = MEMORY_ConvertMemoryMapAddress((uint32_t)curBuffDescrip->buffer, kMEMORY_DMA2Local);
#else
            address = (uint32_t)curBuffDescrip->buffer;
#endif /* FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET */
#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL
            /* Add the cache invalidate maintain. */
            DCACHE_InvalidateByRange(address, handle->rxBuffSizeAlign[ringId]);
#endif /* FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL */
        }
    }

    return kStatus_ENET_RxFrameFail;
}

/*!
 * brief Transmits an ENET frame for extended multi-ring.
 * note The CRC is automatically appended to the data. Input the data
 * to send without the CRC.
 *
 * In this API, multiple-ring are mainly used for extended avb frames are supported.
 * The transmit scheme for avb frames is the credit-based scheme, the AVB class A, AVB class B
 * and the non-AVB frame are transmitted in ring 1, ring 2 and ring 0 independently.
 * So application should care about the transmit ring index when use multiple-ring transmission.
 *
 * param base  ENET peripheral base address.
 * param handle The ENET handler pointer. This is the same handler pointer used in the ENET_Init.
 * param data The data buffer provided by user to be send.
 * param length The length of the data to be send.
 * param ringId The ring index for transmission.
 * retval kStatus_Success  Send frame succeed.
 * retval kStatus_ENET_TxFrameBusy  Transmit buffer descriptor is busy under transmission.
 *         The transmit busy happens when the data send rate is over the MAC capacity.
 *         The waiting mechanism is recommended to be added after each call return with
 *         kStatus_ENET_TxFrameBusy.
 */
status_t ENET_SendFrameMultiRing(
    ENET_Type *base, enet_handle_t *handle, uint8_t *data, uint32_t length, uint32_t ringId)
{
    assert(handle);
    assert(data);
    assert(ringId < FSL_FEATURE_ENET_QUEUE);

    volatile enet_tx_bd_struct_t *curBuffDescrip;
    uint32_t len      = 0;
    uint32_t sizeleft = 0;
    uint32_t address;

    /* Check the frame length. */
    if (length > ENET_FRAME_MAX_FRAMELEN)
    {
        return kStatus_ENET_TxFrameOverLen;
    }

    /* Check if the transmit buffer is ready. */
    curBuffDescrip = handle->txBdCurrent[ringId];
    if (curBuffDescrip->control & ENET_BUFFDESCRIPTOR_TX_READY_MASK)
    {
        return kStatus_ENET_TxFrameBusy;
    }
#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
    bool isPtpEventMessage = false;
    /* Check PTP message with the PTP header. */
    isPtpEventMessage = ENET_Ptp1588ParseFrame(data, NULL, true);
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */
    /* One transmit buffer is enough for one frame. */
    if (handle->txBuffSizeAlign[ringId] >= length)
    {
/* Copy data to the buffer for uDMA transfer. */
#if defined(FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET) && FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET
        address = MEMORY_ConvertMemoryMapAddress((uint32_t)curBuffDescrip->buffer, kMEMORY_DMA2Local);
#else
        address = (uint32_t)curBuffDescrip->buffer;
#endif /* FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET */
        memcpy((void *)address, data, length);

#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL
        /* Add the cache clean maintain. */
        DCACHE_CleanByRange(address, length);
#endif /* FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL */

        /* Set data length. */
        curBuffDescrip->length = length;
#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
        /* For enable the timestamp. */
        if (isPtpEventMessage)
        {
            curBuffDescrip->controlExtend1 |= ENET_BUFFDESCRIPTOR_TX_TIMESTAMP_MASK;
        }
        else
        {
            curBuffDescrip->controlExtend1 &= ~ENET_BUFFDESCRIPTOR_TX_TIMESTAMP_MASK;
        }

#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */
        curBuffDescrip->control |= (ENET_BUFFDESCRIPTOR_TX_READY_MASK | ENET_BUFFDESCRIPTOR_TX_LAST_MASK);

        /* Increase the buffer descriptor address. */
        if (curBuffDescrip->control & ENET_BUFFDESCRIPTOR_TX_WRAP_MASK)
        {
            handle->txBdCurrent[ringId] = handle->txBdBase[ringId];
        }
        else
        {
            handle->txBdCurrent[ringId]++;
        }

        /* Active the transmit buffer descriptor. */
        ENET_ActiveSend(base, ringId);

        return kStatus_Success;
    }
    else
    {
        /* One frame requires more than one transmit buffers. */
        do
        {
#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
            /* For enable the timestamp. */
            if (isPtpEventMessage)
            {
                curBuffDescrip->controlExtend1 |= ENET_BUFFDESCRIPTOR_TX_TIMESTAMP_MASK;
            }
            else
            {
                curBuffDescrip->controlExtend1 &= ~ENET_BUFFDESCRIPTOR_TX_TIMESTAMP_MASK;
            }
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */

            /* Increase the buffer descriptor address. */
            if (curBuffDescrip->control & ENET_BUFFDESCRIPTOR_TX_WRAP_MASK)
            {
                handle->txBdCurrent[ringId] = handle->txBdBase[ringId];
            }
            else
            {
                handle->txBdCurrent[ringId]++;
            }
            /* update the size left to be transmit. */
            sizeleft = length - len;
#if defined(FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET) && FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET
            address = MEMORY_ConvertMemoryMapAddress((uint32_t)curBuffDescrip->buffer, kMEMORY_DMA2Local);
#else
            address = (uint32_t)curBuffDescrip->buffer;
#endif /* FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET */
            if (sizeleft > handle->txBuffSizeAlign[ringId])
            {
                /* Data copy. */
                memcpy((void *)address, data + len, handle->txBuffSizeAlign[ringId]);
#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL
                /* Add the cache clean maintain. */
                DCACHE_CleanByRange(address, handle->txBuffSizeAlign[ringId]);
#endif /* FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL */
                /* Data length update. */
                curBuffDescrip->length = handle->txBuffSizeAlign[ringId];
                len += handle->txBuffSizeAlign[ringId];
                /* Sets the control flag. */
                curBuffDescrip->control &= ~ENET_BUFFDESCRIPTOR_TX_LAST_MASK;
                curBuffDescrip->control |= ENET_BUFFDESCRIPTOR_TX_READY_MASK;

                /* Active the transmit buffer descriptor*/
                ENET_ActiveSend(base, ringId);
            }
            else
            {
                memcpy((void *)address, data + len, sizeleft);
#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL
                /* Add the cache clean maintain. */
                DCACHE_CleanByRange(address, sizeleft);
#endif /* FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL */
                curBuffDescrip->length = sizeleft;
                /* Set Last buffer wrap flag. */
                curBuffDescrip->control |= ENET_BUFFDESCRIPTOR_TX_READY_MASK | ENET_BUFFDESCRIPTOR_TX_LAST_MASK;

                /* Active the transmit buffer descriptor. */
                ENET_ActiveSend(base, ringId);

                return kStatus_Success;
            }

            /* Get the current buffer descriptor address. */
            curBuffDescrip = handle->txBdCurrent[ringId];
        } while (!(curBuffDescrip->control & ENET_BUFFDESCRIPTOR_TX_READY_MASK));

        return kStatus_ENET_TxFrameBusy;
    }
}
#endif /*  FSL_FEATURE_ENET_QUEUE > 1 */

/*!
 * brief Adds the ENET device to a multicast group.
 *
 * param base    ENET peripheral base address.
 * param address The six-byte multicast group address which is provided by application.
 */
void ENET_AddMulticastGroup(ENET_Type *base, uint8_t *address)
{
    assert(address);

    uint32_t crc    = 0xFFFFFFFFU;
    uint32_t count1 = 0;
    uint32_t count2 = 0;

    /* Calculates the CRC-32 polynomial on the multicast group address. */
    for (count1 = 0; count1 < ENET_FRAME_MACLEN; count1++)
    {
        uint8_t c = address[count1];
        for (count2 = 0; count2 < 0x08U; count2++)
        {
            if ((c ^ crc) & 1U)
            {
                crc >>= 1U;
                c >>= 1U;
                crc ^= 0xEDB88320U;
            }
            else
            {
                crc >>= 1U;
                c >>= 1U;
            }
        }
    }

    /* Enable a multicast group address. */
    if (!((crc >> 0x1FU) & 1U))
    {
        base->GALR |= 1U << ((crc >> 0x1AU) & 0x1FU);
    }
    else
    {
        base->GAUR |= 1U << ((crc >> 0x1AU) & 0x1FU);
    }
}

/*!
 * brief Moves the ENET device from a multicast group.
 *
 * param base  ENET peripheral base address.
 * param address The six-byte multicast group address which is provided by application.
 */
void ENET_LeaveMulticastGroup(ENET_Type *base, uint8_t *address)
{
    assert(address);

    uint32_t crc    = 0xFFFFFFFFU;
    uint32_t count1 = 0;
    uint32_t count2 = 0;

    /* Calculates the CRC-32 polynomial on the multicast group address. */
    for (count1 = 0; count1 < ENET_FRAME_MACLEN; count1++)
    {
        uint8_t c = address[count1];
        for (count2 = 0; count2 < 0x08U; count2++)
        {
            if ((c ^ crc) & 1U)
            {
                crc >>= 1U;
                c >>= 1U;
                crc ^= 0xEDB88320U;
            }
            else
            {
                crc >>= 1U;
                c >>= 1U;
            }
        }
    }

    /* Set the hash table. */
    if (!((crc >> 0x1FU) & 1U))
    {
        base->GALR &= ~(1U << ((crc >> 0x1AU) & 0x1FU));
    }
    else
    {
        base->GAUR &= ~(1U << ((crc >> 0x1AU) & 0x1FU));
    }
}

#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
/*!
 * brief Gets the ENET transmit frame statistics after the data send for single ring.
 *
 * This interface gets the error statistics of the transmit frame.
 * Because the error information is reported by the uDMA after the data delivery, this interface
 * should be called after the data transmit API. It is recommended to call this function on
 * transmit interrupt handler. After calling the ENET_SendFrame, the
 * transmit interrupt notifies the transmit completion.
 *
 * param handle The PTP handler pointer. This is the same handler pointer used in the ENET_Init.
 * param eErrorStatic The error statistics structure pointer.
 * return The execute status.
 */
status_t ENET_GetTxErrAfterSendFrame(enet_handle_t *handle, enet_data_error_stats_t *eErrorStatic)
{
    assert(handle);
    assert(eErrorStatic);

    uint16_t control    = 0;
    uint16_t controlExt = 0;

    do
    {
        /* Get the current dirty transmit buffer descriptor. */
        control    = handle->txBdDirtyStatic[0]->control;
        controlExt = handle->txBdDirtyStatic[0]->controlExtend0;

        /* Get the control status data, If the buffer descriptor has not been processed break out. */
        if (control & ENET_BUFFDESCRIPTOR_TX_READY_MASK)
        {
            return kStatus_ENET_TxFrameBusy;
        }
        /* Increase the transmit dirty static pointer. */
        if (handle->txBdDirtyStatic[0]->control & ENET_BUFFDESCRIPTOR_TX_WRAP_MASK)
        {
            handle->txBdDirtyStatic[0] = handle->txBdBase[0];
        }
        else
        {
            handle->txBdDirtyStatic[0]++;
        }

        /* If the transmit buffer descriptor is ready and the last buffer descriptor, store packet statistic. */
        if (control & ENET_BUFFDESCRIPTOR_TX_LAST_MASK)
        {
            if (controlExt & ENET_BUFFDESCRIPTOR_TX_ERR_MASK)
            {
                /* Transmit error. */
                eErrorStatic->statsTxErr++;
            }
            if (controlExt & ENET_BUFFDESCRIPTOR_TX_EXCCOLLISIONERR_MASK)
            {
                /* Transmit excess collision error. */
                eErrorStatic->statsTxExcessCollisionErr++;
            }
            if (controlExt & ENET_BUFFDESCRIPTOR_TX_LATECOLLISIONERR_MASK)
            {
                /* Transmit late collision error. */
                eErrorStatic->statsTxLateCollisionErr++;
            }
            if (controlExt & ENET_BUFFDESCRIPTOR_TX_UNDERFLOWERR_MASK)
            {
                /* Transmit under flow error. */
                eErrorStatic->statsTxUnderFlowErr++;
            }
            if (controlExt & ENET_BUFFDESCRIPTOR_TX_OVERFLOWERR_MASK)
            {
                /* Transmit over flow error. */
                eErrorStatic->statsTxOverFlowErr++;
            }
            return kStatus_Success;
        }

    } while (handle->txBdDirtyStatic[0] != handle->txBdCurrent[0]);

    return kStatus_ENET_TxFrameFail;
}

#if FSL_FEATURE_ENET_QUEUE > 1
/*!
 * brief Gets the ENET transmit frame statistics after the data send for extended multi-ring.
 *
 * This interface gets the error statistics of the transmit frame.
 * Because the error information is reported by the uDMA after the data delivery, this interface
 * should be called after the data transmit API and shall be called by transmit interrupt handler.
 * After calling the ENET_SendFrame, the transmit interrupt notifies the transmit completion.
 *
 * param handle The PTP handler pointer. This is the same handler pointer used in the ENET_Init.
 * param eErrorStatic The error statistics structure pointer.
 * param ringId The ring index.
 * return The execute status.
 */
status_t ENET_GetTxErrAfterSendFrameMultiRing(enet_handle_t *handle,
                                              enet_data_error_stats_t *eErrorStatic,
                                              uint32_t ringId)
{
    assert(handle);
    assert(eErrorStatic);
    assert(ringId < FSL_FEATURE_ENET_QUEUE);

    uint16_t control    = 0;
    uint16_t controlExt = 0;

    do
    {
        /* Get the current dirty transmit buffer descriptor. */
        control    = handle->txBdDirtyStatic[ringId]->control;
        controlExt = handle->txBdDirtyStatic[ringId]->controlExtend0;
        /* Get the control status data, If the buffer descriptor has not been processed break out. */
        if (control & ENET_BUFFDESCRIPTOR_TX_READY_MASK)
        {
            return kStatus_ENET_TxFrameBusy;
        }
        /* Increase the transmit dirty static pointer. */
        if (handle->txBdDirtyStatic[ringId]->control & ENET_BUFFDESCRIPTOR_TX_WRAP_MASK)
        {
            handle->txBdDirtyStatic[ringId] = handle->txBdBase[ringId];
        }
        else
        {
            handle->txBdDirtyStatic[ringId]++;
        }

        /* If the transmit buffer descriptor is ready and the last buffer descriptor, store packet statistic. */
        if (control & ENET_BUFFDESCRIPTOR_TX_LAST_MASK)
        {
            if (controlExt & ENET_BUFFDESCRIPTOR_TX_ERR_MASK)
            {
                /* Transmit error. */
                eErrorStatic->statsTxErr++;
            }
            if (controlExt & ENET_BUFFDESCRIPTOR_TX_EXCCOLLISIONERR_MASK)
            {
                /* Transmit excess collision error. */
                eErrorStatic->statsTxExcessCollisionErr++;
            }
            if (controlExt & ENET_BUFFDESCRIPTOR_TX_LATECOLLISIONERR_MASK)
            {
                /* Transmit late collision error. */
                eErrorStatic->statsTxLateCollisionErr++;
            }
            if (controlExt & ENET_BUFFDESCRIPTOR_TX_UNDERFLOWERR_MASK)
            {
                /* Transmit under flow error. */
                eErrorStatic->statsTxUnderFlowErr++;
            }
            if (controlExt & ENET_BUFFDESCRIPTOR_TX_OVERFLOWERR_MASK)
            {
                /* Transmit over flow error. */
                eErrorStatic->statsTxOverFlowErr++;
            }
            return kStatus_Success;
        }

    } while (handle->txBdDirtyStatic[ringId] != handle->txBdCurrent[ringId]);

    return kStatus_ENET_TxFrameFail;
}
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */

static bool ENET_Ptp1588ParseFrame(const uint8_t *data, enet_ptp_time_data_t *ptpTsData, bool isFastEnabled)
{
    assert(data);
    if (!isFastEnabled)
    {
        assert(ptpTsData);
    }

    bool isPtpMsg         = false;
    const uint8_t *buffer = data;
    uint16_t ptpType;

    /* Check for VLAN frame.
     * Add Double vlan tag check for receiving extended QIN vlan frame. */
    if (*(uint16_t *)(buffer + ENET_PTP1588_ETHL2_PACKETTYPE_OFFSET) == (ENET_HTONS(ENET_8021QVLAN)
#if defined(FSL_FEATUR_ENET_HAS_AVB) && FSL_FEATURE_HAS_AVB
                                                                         || ENET_HTONS(ENET_8021QSVLAN)
#endif /* FSL_FEATURE_ENET_HAS_AVB */
                                                                             ))
    {
        buffer += ENET_FRAME_VLAN_TAGLEN;
#if defined(FSL_FEATUR_ENET_HAS_AVB) && FSL_FEATURE_HAS_AVB
        if (*(uint16_t *)(buffer + ENET_PTP1588_ETHL2_PACKETTYPE_OFFSET) == ENET_HTONS(ENET_8021QVLAN)
        {
            buffer += ENET_FRAME_VLAN_TAGLEN;
        }
#endif /* FSL_FEATURE_ENET_HAS_AVB */
    }

    ptpType = *(uint16_t *)(buffer + ENET_PTP1588_ETHL2_PACKETTYPE_OFFSET);
    switch (ENET_HTONS(ptpType))
    { /* Ethernet layer 2. */
        case ENET_ETHERNETL2:
            if ((*(uint8_t *)(buffer + ENET_PTP1588_ETHL2_MSGTYPE_OFFSET) & 0x0F) <= kENET_PtpEventMsgType)
            {
                isPtpMsg = true;
                if (!isFastEnabled)
                {
                    /* It's a ptpv2 message and store the ptp header information. */
                    ptpTsData->version     = (*(uint8_t *)(buffer + ENET_PTP1588_ETHL2_VERSION_OFFSET)) & 0x0F;
                    ptpTsData->messageType = (*(uint8_t *)(buffer + ENET_PTP1588_ETHL2_MSGTYPE_OFFSET)) & 0x0F;
                    ptpTsData->sequenceId  = ENET_HTONS(*(uint16_t *)(buffer + ENET_PTP1588_ETHL2_SEQUENCEID_OFFSET));
                    memcpy((void *)&ptpTsData->sourcePortId[0], (void *)(buffer + ENET_PTP1588_ETHL2_CLOCKID_OFFSET),
                           kENET_PtpSrcPortIdLen);
                }
            }
            break;
        /* IPV4. */
        case ENET_IPV4:
            if ((*(uint8_t *)(buffer + ENET_PTP1588_IPVERSION_OFFSET) >> 4) == ENET_IPV4VERSION)
            {
                if (((*(uint16_t *)(buffer + ENET_PTP1588_IPV4_UDP_PORT_OFFSET)) == ENET_HTONS(kENET_PtpEventPort)) &&
                    (*(uint8_t *)(buffer + ENET_PTP1588_IPV4_UDP_PROTOCOL_OFFSET) == ENET_UDPVERSION))
                {
                    /* Set the PTP message flag. */
                    isPtpMsg = true;
                    if (!isFastEnabled)
                    {
                        /* It's a IPV4 ptp message and store the ptp header information. */
                        ptpTsData->version     = (*(uint8_t *)(buffer + ENET_PTP1588_IPV4_UDP_VERSION_OFFSET)) & 0x0F;
                        ptpTsData->messageType = (*(uint8_t *)(buffer + ENET_PTP1588_IPV4_UDP_MSGTYPE_OFFSET)) & 0x0F;
                        ptpTsData->sequenceId =
                            ENET_HTONS(*(uint16_t *)(buffer + ENET_PTP1588_IPV4_UDP_SEQUENCEID_OFFSET));
                        memcpy((void *)&ptpTsData->sourcePortId[0],
                               (void *)(buffer + ENET_PTP1588_IPV4_UDP_CLKID_OFFSET), kENET_PtpSrcPortIdLen);
                    }
                }
            }
            break;
        /* IPV6. */
        case ENET_IPV6:
            if ((*(uint8_t *)(buffer + ENET_PTP1588_IPVERSION_OFFSET) >> 4) == ENET_IPV6VERSION)
            {
                if (((*(uint16_t *)(buffer + ENET_PTP1588_IPV6_UDP_PORT_OFFSET)) == ENET_HTONS(kENET_PtpEventPort)) &&
                    (*(uint8_t *)(buffer + ENET_PTP1588_IPV6_UDP_PROTOCOL_OFFSET) == ENET_UDPVERSION))
                {
                    /* Set the PTP message flag. */
                    isPtpMsg = true;
                    if (!isFastEnabled)
                    {
                        /* It's a IPV6 ptp message and store the ptp header information. */
                        ptpTsData->version     = (*(uint8_t *)(buffer + ENET_PTP1588_IPV6_UDP_VERSION_OFFSET)) & 0x0F;
                        ptpTsData->messageType = (*(uint8_t *)(buffer + ENET_PTP1588_IPV6_UDP_MSGTYPE_OFFSET)) & 0x0F;
                        ptpTsData->sequenceId =
                            ENET_HTONS(*(uint16_t *)(buffer + ENET_PTP1588_IPV6_UDP_SEQUENCEID_OFFSET));
                        memcpy((void *)&ptpTsData->sourcePortId[0],
                               (void *)(buffer + ENET_PTP1588_IPV6_UDP_CLKID_OFFSET), kENET_PtpSrcPortIdLen);
                    }
                }
            }
            break;
        default:
            break;
    }
    return isPtpMsg;
}

/*!
 * brief Configures the ENET PTP IEEE 1588 feature with the basic configuration.
 * The function sets the clock for PTP 1588 timer and enables
 * time stamp interrupts and transmit interrupts for PTP 1588 features.
 * This API should be called when the 1588 feature is enabled
 * or the ENET_ENHANCEDBUFFERDESCRIPTOR_MODE is defined.
 * ENET_Init should be called before calling this API.
 *
 * note The PTP 1588 time-stamp second increase though time-stamp interrupt handler
 *  and the transmit time-stamp store is done through transmit interrupt handler.
 *  As a result, the TS interrupt and TX interrupt are enabled when you call this API.
 *
 * param base    ENET peripheral base address.
 * param handle  ENET handler pointer.
 * param ptpConfig The ENET PTP1588 configuration.
 */
void ENET_Ptp1588Configure(ENET_Type *base, enet_handle_t *handle, enet_ptp_config_t *ptpConfig)
{
    assert(handle);
    assert(ptpConfig);
    uint8_t count;

    uint32_t instance = ENET_GetInstance(base);
    uint32_t mask     = kENET_TxBufferInterrupt;
#if FSL_FEATURE_ENET_QUEUE > 1
    mask |= kENET_TxBuffer1Interrupt | kENET_TxBuffer2Interrupt;
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */

    /* Start the 1588 timer. */
    ENET_Ptp1588StartTimer(base, ptpConfig->ptp1588ClockSrc_Hz);

    for (count = 0; count < handle->ringNum; count++)
    {
        handle->txBdDirtyTime[count]   = handle->txBdBase[count];
        handle->txBdDirtyStatic[count] = handle->txBdBase[count];
    }

    /* Setting the receive and transmit state for transaction. */
    handle->rxPtpTsDataRing.ptpTsData = ptpConfig->rxPtpTsData;
    handle->rxPtpTsDataRing.size      = ptpConfig->ptpTsRxBuffNum;
    handle->rxPtpTsDataRing.front     = 0;
    handle->rxPtpTsDataRing.end       = 0;
    handle->txPtpTsDataRing.ptpTsData = ptpConfig->txPtpTsData;
    handle->txPtpTsDataRing.size      = ptpConfig->ptpTsTxBuffNum;
    handle->txPtpTsDataRing.front     = 0;
    handle->txPtpTsDataRing.end       = 0;
    handle->msTimerSecond             = 0;

    /* Set the IRQ handler when the interrupt is enabled. */
    s_enetTxIsr = ENET_TransmitIRQHandler;
    s_enetTsIsr = ENET_Ptp1588TimerIRQHandler;

    /* Enables the time stamp interrupt and transmit frame interrupt to
     * handle the time-stamp . */
    ENET_EnableInterrupts(base, (ENET_TS_INTERRUPT | ENET_TX_INTERRUPT));
    ENET_DisableInterrupts(base, mask);

    EnableIRQ(s_enetTsIrqId[instance]);
    EnableIRQ(s_enetTxIrqId[instance]);
}

/*!
 * brief Starts the ENET PTP 1588 Timer.
 * This function is used to initialize the PTP timer. After the PTP starts,
 * the PTP timer starts running.
 *
 * param base  ENET peripheral base address.
 * param ptpClkSrc The clock source of the PTP timer.
 */
void ENET_Ptp1588StartTimer(ENET_Type *base, uint32_t ptpClkSrc)
{
    /* Restart PTP 1588 timer, master clock. */
    base->ATCR = ENET_ATCR_RESTART_MASK;

    /* Initializes PTP 1588 timer. */
    base->ATINC = ENET_ATINC_INC(ENET_NANOSECOND_ONE_SECOND / ptpClkSrc);
    base->ATPER = ENET_NANOSECOND_ONE_SECOND;
    /* Sets periodical event and the event signal output assertion and Actives PTP 1588 timer.  */
    base->ATCR = ENET_ATCR_PEREN_MASK | ENET_ATCR_PINPER_MASK | ENET_ATCR_EN_MASK;
}

/*!
 * brief Gets the current ENET time from the PTP 1588 timer.
 *
 * param base  ENET peripheral base address.
 * param handle The ENET state pointer. This is the same state pointer used in the ENET_Init.
 * param ptpTime The PTP timer structure.
 */
void ENET_Ptp1588GetTimer(ENET_Type *base, enet_handle_t *handle, enet_ptp_time_t *ptpTime)
{
    assert(handle);
    assert(ptpTime);
    uint16_t count = ENET_1588TIME_DELAY_COUNT;
    uint32_t primask;

    /* Disables the interrupt. */
    primask = DisableGlobalIRQ();

    /* Get the current PTP time. */
    ptpTime->second = handle->msTimerSecond;
    /* Get the nanosecond from the master timer. */
    base->ATCR |= ENET_ATCR_CAPTURE_MASK;
    /* Add at least six clock cycle delay to get accurate time.
       It's the requirement when the 1588 clock source is slower
       than the register clock.
    */
    while (count--)
    {
        __NOP();
    }
    /* Get the captured time. */
    ptpTime->nanosecond = base->ATVR;

    /* Get PTP timer wrap event. */
    if (base->EIR & kENET_TsTimerInterrupt)
    {
        ptpTime->second++;
    }

    /* Enables the interrupt. */
    EnableGlobalIRQ(primask);
}

/*!
 * brief Sets the ENET PTP 1588 timer to the assigned time.
 *
 * param base  ENET peripheral base address.
 * param handle The ENET state pointer. This is the same state pointer used in the ENET_Init.
 * param ptpTime The timer to be set to the PTP timer.
 */
void ENET_Ptp1588SetTimer(ENET_Type *base, enet_handle_t *handle, enet_ptp_time_t *ptpTime)
{
    assert(handle);
    assert(ptpTime);

    uint32_t primask;

    /* Disables the interrupt. */
    primask = DisableGlobalIRQ();

    /* Sets PTP timer. */
    handle->msTimerSecond = ptpTime->second;
    base->ATVR            = ptpTime->nanosecond;

    /* Enables the interrupt. */
    EnableGlobalIRQ(primask);
}

/*!
 * brief Adjusts the ENET PTP 1588 timer.
 *
 * param base  ENET peripheral base address.
 * param corrIncrease The correction increment value. This value is added every time the correction
 *       timer expires. A value less than the PTP timer frequency(1/ptpClkSrc) slows down the timer,
 *        a value greater than the 1/ptpClkSrc speeds up the timer.
 * param corrPeriod The PTP timer correction counter wrap-around value. This defines after how
 *       many timer clock the correction counter should be reset and trigger a correction
 *       increment on the timer. A value of 0 disables the correction counter and no correction occurs.
 */
void ENET_Ptp1588AdjustTimer(ENET_Type *base, uint32_t corrIncrease, uint32_t corrPeriod)
{
    /* Set correction for PTP timer increment. */
    base->ATINC = (base->ATINC & ~ENET_ATINC_INC_CORR_MASK) | (corrIncrease << ENET_ATINC_INC_CORR_SHIFT);
    /* Set correction for PTP timer period. */
    base->ATCOR = (base->ATCOR & ~ENET_ATCOR_COR_MASK) | (corrPeriod << ENET_ATCOR_COR_SHIFT);
}

static status_t ENET_Ptp1588UpdateTimeRing(enet_ptp_time_data_ring_t *ptpTsDataRing, enet_ptp_time_data_t *ptpTimeData)
{
    assert(ptpTsDataRing);
    assert(ptpTsDataRing->ptpTsData);
    assert(ptpTimeData);

    uint16_t usedBuffer = 0;

    /* Check if the buffers ring is full. */
    if (ptpTsDataRing->end >= ptpTsDataRing->front)
    {
        usedBuffer = ptpTsDataRing->end - ptpTsDataRing->front;
    }
    else
    {
        usedBuffer = ptpTsDataRing->size - (ptpTsDataRing->front - ptpTsDataRing->end);
    }

    if (usedBuffer == (ptpTsDataRing->size - 1))
    {
        /* Ptp timestamp ring full, drop one in the front. */
        ptpTsDataRing->front = (ptpTsDataRing->front + 1) % ptpTsDataRing->size;
    }

    /* Copy the new data into the buffer. */
    memcpy((ptpTsDataRing->ptpTsData + ptpTsDataRing->end), ptpTimeData, sizeof(enet_ptp_time_data_t));

    /* Increase the buffer pointer to the next empty one. */
    ptpTsDataRing->end = (ptpTsDataRing->end + 1) % ptpTsDataRing->size;

    return kStatus_Success;
}

static status_t ENET_Ptp1588SearchTimeRing(enet_ptp_time_data_ring_t *ptpTsDataRing, enet_ptp_time_data_t *ptpTimedata)
{
    assert(ptpTsDataRing);
    assert(ptpTsDataRing->ptpTsData);
    assert(ptpTimedata);

    uint32_t index;
    uint32_t size;
    uint16_t usedBuffer = 0;
    bool isRingBufferFull = false;

    /* Check the PTP 1588 timestamp ring. */
    if (ptpTsDataRing->front == ptpTsDataRing->end)
    {
        return kStatus_ENET_PtpTsRingEmpty;
    }

    /* Check if buffers is full. */
    if (ptpTsDataRing->end >= ptpTsDataRing->front)
    {
        usedBuffer = ptpTsDataRing->end - ptpTsDataRing->front;
    }
    else
    {
        usedBuffer = ptpTsDataRing->size - (ptpTsDataRing->front - ptpTsDataRing->end);
    }

    if (usedBuffer == (ptpTsDataRing->size-1))
    {
        isRingBufferFull = true;
    }

    /* Search the element in the ring buffer */
    index = ptpTsDataRing->front;
    size  = ptpTsDataRing->size;
    while (index != ptpTsDataRing->end)
    {
        if (((ptpTsDataRing->ptpTsData + index)->sequenceId == ptpTimedata->sequenceId) &&
            (!memcmp(((void *)&(ptpTsDataRing->ptpTsData + index)->sourcePortId[0]),
                     (void *)&ptpTimedata->sourcePortId[0], kENET_PtpSrcPortIdLen)) &&
            ((ptpTsDataRing->ptpTsData + index)->version == ptpTimedata->version) &&
            ((ptpTsDataRing->ptpTsData + index)->messageType == ptpTimedata->messageType))
        {
            break;
        }

        /* Increase the ptp ring index. */
        index = (index + 1) % size;
    }

    if (index == ptpTsDataRing->end)
    {
        if (isRingBufferFull == true)
        {
            /* PTP timestamp buffer ring full, data in index ptpTsDataRing->end is valid. */
            if (((ptpTsDataRing->ptpTsData + index)->sequenceId != ptpTimedata->sequenceId) ||
            (memcmp(((void *)&(ptpTsDataRing->ptpTsData + index)->sourcePortId[0]),
                     (void *)&ptpTimedata->sourcePortId[0], kENET_PtpSrcPortIdLen)) ||
            ((ptpTsDataRing->ptpTsData + index)->version != ptpTimedata->version)  ||
            ((ptpTsDataRing->ptpTsData + index)->messageType != ptpTimedata->messageType))
            {
                return kStatus_ENET_PtpTsRingFull;
            }
        }
        else
        {
            return kStatus_ENET_PtpTsRingFull;
        }
    }

    /* Get the right timestamp of the required ptp messag. */
    ptpTimedata->timeStamp.second     = (ptpTsDataRing->ptpTsData + index)->timeStamp.second;
    ptpTimedata->timeStamp.nanosecond = (ptpTsDataRing->ptpTsData + index)->timeStamp.nanosecond;

    if( isRingBufferFull == true)
    {
        /* If ring buffer full, move front pointer to next pointer behind end pointer, then next
        read will still read whole ring. */
        ptpTsDataRing->end = index;
    }

    /* Drop previous ptp stamp. */
    ptpTsDataRing->front = (index + 1) % size;

    return kStatus_Success;
}

static status_t ENET_StoreRxFrameTime(ENET_Type *base, enet_handle_t *handle, enet_ptp_time_data_t *ptpTimeData)
{
    assert(handle);
    assert(ptpTimeData);

    enet_ptp_time_t ptpTimer;

    /* Get current PTP timer nanosecond value. */
    ENET_Ptp1588GetTimer(base, handle, &ptpTimer);

    /* Get transmit time stamp second. */
    if (ptpTimer.nanosecond >= ptpTimeData->timeStamp.nanosecond)
    {
        ptpTimeData->timeStamp.second = ptpTimer.second;
    }
    else
    {
        ptpTimeData->timeStamp.second = ptpTimer.second - 1;
    }

    /* Store the timestamp to the receive time stamp ring. */
    /* Check if the buffers ring is full. */
    return ENET_Ptp1588UpdateTimeRing(&handle->rxPtpTsDataRing, ptpTimeData);
}

static status_t ENET_StoreTxFrameTime(ENET_Type *base, enet_handle_t *handle, uint32_t ringId)
{
    assert(handle);

    bool isPtpEventMessage = false;
    enet_ptp_time_data_t ptpTimeData;
    volatile enet_tx_bd_struct_t *curBuffDescrip;
    volatile enet_tx_bd_struct_t *endBuffDescrip;
    uint32_t address;
    bool isReadLastBd = false;
    bool readLastBd = false;

    /* Treat the txBdCurrent[ringId] as the tx bd write buffer pointer,
    txBdDirtyTime[ringId] as the tx bd read buffer pointer. Considering
    that the tx frame time read is driven by tx frame send, so the read
    pointer txBdDirtyTime[ringId] should always behind write pointer
    txBdDirtyTime[ringId]. Then if the read pointer equals write pointer,
    we could treat the tx bd buffer is full. We could read out all
    available bds. */
    if(handle->txBdDirtyTime[ringId] == handle->txBdCurrent[ringId])
    {
        isReadLastBd = true;
        if(handle->txBdDirtyTime[ringId] == handle->txBdBase[ringId])
        {
           endBuffDescrip =  handle->txBdDirtyTime[ringId] + 3;
        }
        else
        {
           endBuffDescrip = handle->txBdDirtyTime[ringId] - 1;
        }
    }
    else
    {
        endBuffDescrip = handle->txBdCurrent[ringId];
    }

    /* Read tx bd buffer until read pointer txBdDirtyTime[ringId] equals to the end pointer of
    tx bd buffer. */
    while((handle->txBdDirtyTime[ringId] != endBuffDescrip)  || (readLastBd == true))
    {
         curBuffDescrip = handle->txBdDirtyTime[ringId];

        /* Get the control status data, If the buffer descriptor has not been processed break out. */
        if (curBuffDescrip->control & ENET_BUFFDESCRIPTOR_TX_READY_MASK)
        {
            return kStatus_ENET_TxFrameBusy;
        }

    /* Parse the PTP message. */
    #if defined(FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET) && FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET
        address = MEMORY_ConvertMemoryMapAddress((uint32_t)curBuffDescrip->buffer, kMEMORY_DMA2Local);
    #else
        address = (uint32_t)curBuffDescrip->buffer;
    #endif /* FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET */
        isPtpEventMessage = ENET_Ptp1588ParseFrame((uint8_t *)address, &ptpTimeData, false);
        if (isPtpEventMessage)
        {
            /* Only store tx timestamp for ptp event message. */
            /* Increase current buffer descriptor to the next one. */
            if (handle->txBdDirtyTime[ringId]->control & ENET_BUFFDESCRIPTOR_TX_WRAP_MASK)
            {
                handle->txBdDirtyTime[ringId] = handle->txBdBase[ringId];
            }
            else
            {
                handle->txBdDirtyTime[ringId]++;
            }

            /* Do time stamp check on the last buffer descriptor of the frame. */
            if (curBuffDescrip->control & ENET_BUFFDESCRIPTOR_TX_LAST_MASK)
            {
                /* Get current PTP timer nanosecond value. */
                ENET_Ptp1588GetTimer(base, handle, &ptpTimeData.timeStamp);

                /* Get transmit time stamp second. */
                if (ptpTimeData.timeStamp.nanosecond < curBuffDescrip->timestamp)
                {
                    ptpTimeData.timeStamp.second--;
                }

                /* Save transmit time stamp nanosecond. */
                ptpTimeData.timeStamp.nanosecond = curBuffDescrip->timestamp;

                /* Store the timestamp to the transmit timestamp ring. */
                ENET_Ptp1588UpdateTimeRing(&handle->txPtpTsDataRing, &ptpTimeData);
            }
        }
        else
        {
              /* Only increase current buffer descriptor to the next one. */
              if (handle->txBdDirtyTime[ringId]->control & ENET_BUFFDESCRIPTOR_TX_WRAP_MASK)
              {
                  handle->txBdDirtyTime[ringId] = handle->txBdBase[ringId];
              }
              else
              {
                  handle->txBdDirtyTime[ringId]++;
              }
        }

        if(readLastBd == true)
        {
            break;
        }

        /* If if read pointer equals write pointer, the end pointer is set at the pointer before
        current write pointer and the buffer in the end pointer is available, so read last bd at
        the end pointer. */
        if((isReadLastBd == true) && (handle->txBdDirtyTime[ringId] == endBuffDescrip))
        {
            readLastBd = true;
        }
    }
    return kStatus_Success;
}

/*!
 * brief Gets the time stamp of the transmit frame.
 *
 * This function is used for PTP stack to get the timestamp captured by the ENET driver.
 *
 * param handle The ENET handler pointer.This is the same state pointer used in
 *        ENET_Init.
 * param ptpTimeData The special PTP timestamp data for search the receive timestamp.
 * retval kStatus_Success Get 1588 timestamp success.
 * retval kStatus_ENET_PtpTsRingEmpty 1588 timestamp ring empty.
 * retval kStatus_ENET_PtpTsRingFull 1588 timestamp ring full.
 */
status_t ENET_GetTxFrameTime(enet_handle_t *handle, enet_ptp_time_data_t *ptpTimeData)
{
    assert(handle);
    assert(ptpTimeData);

    return ENET_Ptp1588SearchTimeRing(&handle->txPtpTsDataRing, ptpTimeData);
}

/*!
 * brief Gets the time stamp of the received frame.
 *
 * This function is used for PTP stack to get the timestamp captured by the ENET driver.
 *
 * param handle The ENET handler pointer.This is the same state pointer used in
 *        ENET_Init.
 * param ptpTimeData The special PTP timestamp data for search the receive timestamp.
 * retval kStatus_Success Get 1588 timestamp success.
 * retval kStatus_ENET_PtpTsRingEmpty 1588 timestamp ring empty.
 * retval kStatus_ENET_PtpTsRingFull 1588 timestamp ring full.
 */
status_t ENET_GetRxFrameTime(enet_handle_t *handle, enet_ptp_time_data_t *ptpTimeData)
{
    assert(handle);
    assert(ptpTimeData);

    return ENET_Ptp1588SearchTimeRing(&handle->rxPtpTsDataRing, ptpTimeData);
}

#if defined(FSL_FEATURE_ENET_HAS_AVB) && FSL_FEATURE_ENET_HAS_AVB
/*!
 * brief Sets the ENET AVB feature.
 *
 * ENET AVB feature configuration, set the Receive classification match and transmit
 * bandwidth. This API is called when the AVB feature is required.
 *
 * Note: The AVB frames transmission scheme is credit-based tx scheme and it's only supported
 * with the Enhanced buffer descriptors. so the AVB configuration should only done with
 * Enhanced buffer descriptor. so when the AVB feature is required, please make sure the
 * the "ENET_ENHANCEDBUFFERDESCRIPTOR_MODE" is defined.
 *
 * param base ENET peripheral base address.
 * param handle ENET handler pointer.
 * param config The ENET AVB feature configuration structure.
 */
void ENET_AVBConfigure(ENET_Type *base, enet_handle_t *handle, const enet_avb_config_t *config)
{
    assert(config);

    uint8_t count = 0;

    for (count = 0; count < FSL_FEATURE_ENET_QUEUE - 1; count++)
    {
        /* Set the AVB receive ring classification match when the match is not 0. */
        if (config->rxClassifyMatch[count])
        {
            base->RCMR[count] = (config->rxClassifyMatch[count] & 0xFFFF) | ENET_RCMR_MATCHEN_MASK;
        }
        /* Set the dma controller for the extended ring. */
        base->DMACFG[count] |= ENET_DMACFG_IDLE_SLOPE(config->idleSlope[count]);
    }

    /* Shall use the credit-based scheme for avb. */
    base->QOS &= ~ENET_QOS_TX_SCHEME_MASK;
    base->QOS |= ENET_QOS_RX_FLUSH0_MASK;
}
#endif /* FSL_FETAURE_ENET_HAS_AVB */
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */

#if FSL_FEATURE_ENET_QUEUE > 1
/*!
 * brief The transmit IRQ handler.
 *
 * param base  ENET peripheral base address.
 * param handle The ENET handler pointer.
 */
void ENET_TransmitIRQHandler(ENET_Type *base, enet_handle_t *handle, uint32_t ringId)
#else
/*!
 * brief The transmit IRQ handler.
 *
 * param base  ENET peripheral base address.
 * param handle The ENET handler pointer.
 */
void ENET_TransmitIRQHandler(ENET_Type *base, enet_handle_t *handle)
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */
{
    assert(handle);
    uint32_t mask = kENET_TxBufferInterrupt | kENET_TxFrameInterrupt;
#if defined(ENET_ENHANCEDBUFFERDESCRIPTOR_MODE) || (FSL_FEATURE_ENET_QUEUE > 1)
    uint32_t index = 0;
#endif /* ENET_ENHANCEDBUFFERDESCRIPTORMODE  || (FSL_FEATURE_ENET_QUEUE > 1) */

/* Check if the transmit interrupt happen. */
#if FSL_FEATURE_ENET_QUEUE > 1
    switch (ringId)
    {
        case kENET_Ring1:
            mask = (kENET_TxFrame1Interrupt | kENET_TxBuffer1Interrupt);
            break;
        case kENET_Ring2:
            mask = (kENET_TxFrame2Interrupt | kENET_TxBuffer2Interrupt);
            break;
        default:
            break;
    }
    index = ringId;
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */

    while (mask & base->EIR)
    {
#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
        if (base->EIR & kENET_TxFrameInterrupt)
        {
            /* Store the transmit timestamp from the buffer descriptor should be done here. */
            ENET_StoreTxFrameTime(base, handle, index);
        }
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */

        /* Clear the transmit interrupt event. */
        base->EIR = mask;

        /* Callback function. */
        if (handle->callback)
        {
#if FSL_FEATURE_ENET_QUEUE > 1
            handle->callback(base, handle, index, kENET_TxEvent, handle->userData);
#else
            handle->callback(base, handle, kENET_TxEvent, handle->userData);
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */
        }
    }
}

#if FSL_FEATURE_ENET_QUEUE > 1
/*!
 * brief The receive IRQ handler.
 *
 * param base  ENET peripheral base address.
 * param handle The ENET handler pointer.
 */
void ENET_ReceiveIRQHandler(ENET_Type *base, enet_handle_t *handle, uint32_t ringId)
#else
/*!
 * brief The receive IRQ handler.
 *
 * param base  ENET peripheral base address.
 * param handle The ENET handler pointer.
 */
void ENET_ReceiveIRQHandler(ENET_Type *base, enet_handle_t *handle)
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */
{
    assert(handle);
    uint32_t mask = kENET_RxFrameInterrupt | kENET_RxBufferInterrupt;

/* Check if the receive interrupt happen. */
#if FSL_FEATURE_ENET_QUEUE > 1
    switch (ringId)
    {
        case kENET_Ring1:
            mask = (kENET_RxFrame1Interrupt | kENET_RxBuffer1Interrupt);
            break;
        case kENET_Ring2:
            mask = (kENET_RxFrame2Interrupt | kENET_RxBuffer2Interrupt);
            break;
        default:
            break;
    }
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */

    while (mask & base->EIR)
    {
        /* Clear the transmit interrupt event. */
        base->EIR = mask;

        /* Callback function. */
        if (handle->callback)
        {
#if FSL_FEATURE_ENET_QUEUE > 1
            handle->callback(base, handle, ringId, kENET_RxEvent, handle->userData);
#else
            handle->callback(base, handle, kENET_RxEvent, handle->userData);
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */
        }
    }
}

/*!
 * brief Some special IRQ handler including the error, mii, wakeup irq handler.
 *
 * param base  ENET peripheral base address.
 * param handle The ENET handler pointer.
 */
void ENET_ErrorIRQHandler(ENET_Type *base, enet_handle_t *handle)
{
    assert(handle);

    uint32_t errMask = kENET_BabrInterrupt | kENET_BabtInterrupt | kENET_EBusERInterrupt | kENET_PayloadRxInterrupt |
                       kENET_LateCollisionInterrupt | kENET_RetryLimitInterrupt | kENET_UnderrunInterrupt;

    /* Check if the error interrupt happen. */
    if (kENET_WakeupInterrupt & base->EIR)
    {
        /* Clear the wakeup interrupt. */
        base->EIR = kENET_WakeupInterrupt;
        /* wake up and enter the normal mode. */
        ENET_EnableSleepMode(base, false);
        /* Callback function. */
        if (handle->callback)
        {
#if FSL_FEATURE_ENET_QUEUE > 1
            handle->callback(base, handle, 0, kENET_WakeUpEvent, handle->userData);
#else
            handle->callback(base, handle, kENET_WakeUpEvent, handle->userData);
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */
        }
    }
    else
    {
        /* Clear the error interrupt event status. */
        errMask &= base->EIR;
        base->EIR = errMask;
        /* Callback function. */
        if (handle->callback)
        {
#if FSL_FEATURE_ENET_QUEUE > 1
            handle->callback(base, handle, 0, kENET_ErrEvent, handle->userData);
#else
            handle->callback(base, handle, kENET_ErrEvent, handle->userData);
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */
        }
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

#ifdef ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
/*!
 * brief The IEEE 1588 PTP time stamp interrupt handler.
 *
 * param base  ENET peripheral base address.
 * param handle The ENET state pointer. This is the same state pointer used in the ENET_Init.
 */
void ENET_Ptp1588TimerIRQHandler(ENET_Type *base, enet_handle_t *handle)
{
    assert(handle);

    /* Check if the PTP time stamp interrupt happen. */
    if (kENET_TsTimerInterrupt & base->EIR)
    {
        /* Clear the time stamp interrupt. */
        base->EIR = kENET_TsTimerInterrupt;

        /* Increase timer second counter. */
        handle->msTimerSecond++;

        /* Callback function. */
        if (handle->callback)
        {
#if FSL_FEATURE_ENET_QUEUE > 1
            handle->callback(base, handle, 0, kENET_TimeStampEvent, handle->userData);
#else
            handle->callback(base, handle, kENET_TimeStampEvent, handle->userData);
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */
        }
    }
    else
    {
        /* Clear the time stamp interrupt. */
        base->EIR = kENET_TsAvailInterrupt;
        /* Callback function. */
        if (handle->callback)
        {
#if FSL_FEATURE_ENET_QUEUE > 1
            handle->callback(base, handle, 0, kENET_TimeStampAvailEvent, handle->userData);
#else
            handle->callback(base, handle, kENET_TimeStampAvailEvent, handle->userData);
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */
        }
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */

/*!
 * brief the common IRQ handler for the tx/rx/error etc irq handler.
 *
 * This is used for the combined tx/rx/error interrupt for single/mutli-ring (frame 0).
 *
 * param base  ENET peripheral base address.
 */
void ENET_CommonFrame0IRQHandler(ENET_Type *base)
{
    uint32_t event    = base->EIR;
    uint32_t instance = ENET_GetInstance(base);

    if (event & (kENET_TxBufferInterrupt | kENET_TxFrameInterrupt))
    {
#if FSL_FEATURE_ENET_QUEUE > 1
        s_enetTxIsr(base, s_ENETHandle[instance], 0);
#else
        s_enetTxIsr(base, s_ENETHandle[instance]);
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */
    }

    if (event & (kENET_RxBufferInterrupt | kENET_RxFrameInterrupt))
    {
#if FSL_FEATURE_ENET_QUEUE > 1
        s_enetRxIsr(base, s_ENETHandle[instance], 0);
#else
        s_enetRxIsr(base, s_ENETHandle[instance]);
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */
    }

    if (event & ENET_TS_INTERRUPT)
    {
        s_enetTsIsr(base, s_ENETHandle[instance]);
    }
    if (event & ENET_ERR_INTERRUPT)
    {
        s_enetErrIsr(base, s_ENETHandle[instance]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

#if FSL_FEATURE_ENET_QUEUE > 1
/*!
 * brief the common IRQ handler for the tx/rx irq handler.
 *
 * This is used for the combined tx/rx interrupt for multi-ring (frame 1).
 *
 * param base  ENET peripheral base address.
 */
void ENET_CommonFrame1IRQHandler(ENET_Type *base)
{
    uint32_t event    = base->EIR;
    uint32_t instance = ENET_GetInstance(base);

    if (event & (kENET_TxBuffer1Interrupt | kENET_TxFrame1Interrupt))
    {
        s_enetTxIsr(base, s_ENETHandle[instance], 1);
    }

    if (event & (kENET_RxBuffer1Interrupt | kENET_RxFrame1Interrupt))
    {
        s_enetRxIsr(base, s_ENETHandle[instance], 1);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

/*!
 * brief the common IRQ handler for the tx/rx irq handler.
 *
 * This is used for the combined tx/rx interrupt for multi-ring (frame 2).
 *
 * param base  ENET peripheral base address.
 */
void ENET_CommonFrame2IRQHandler(ENET_Type *base)
{
    uint32_t event    = base->EIR;
    uint32_t instance = ENET_GetInstance(base);

    if (event & (kENET_TxBuffer2Interrupt | kENET_TxFrame2Interrupt))
    {
        s_enetTxIsr(base, s_ENETHandle[instance], 2);
    }

    if (event & (kENET_RxBuffer2Interrupt | kENET_RxFrame2Interrupt))
    {
        s_enetRxIsr(base, s_ENETHandle[instance], 2);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* FSL_FEATURE_ENET_QUEUE > 1 */

#if defined(ENET)
void ENET_Transmit_IRQHandler(void)
{
    s_enetTxIsr(ENET, s_ENETHandle[0]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void ENET_Receive_IRQHandler(void)
{
    s_enetRxIsr(ENET, s_ENETHandle[0]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void ENET_Error_IRQHandler(void)
{
    s_enetErrIsr(ENET, s_ENETHandle[0]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void ENET_1588_Timer_IRQHandler(void)
{
    s_enetTsIsr(ENET, s_ENETHandle[0]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void ENET_DriverIRQHandler(void)
{
    ENET_CommonFrame0IRQHandler(ENET);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

#endif

#if defined(ENET1)
void ENET1_DriverIRQHandler(void)
{
    ENET_CommonFrame0IRQHandler(ENET1);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(ENET2)
void ENET2_DriverIRQHandler(void)
{
    ENET_CommonFrame0IRQHandler(ENET2);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(CONNECTIVITY__ENET0)
void CONNECTIVITY_ENET0_FRAME0_EVENT_INT_DriverIRQHandler(void)
{
    ENET_CommonFrame0IRQHandler(CONNECTIVITY__ENET0);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#if FSL_FEATURE_ENET_QUEUE > 1
void CONNECTIVITY_ENET0_FRAME1_INT_DriverIRQHandler(void)
{
    ENET_CommonFrame1IRQHandler(CONNECTIVITY__ENET0);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void CONNECTIVITY_ENET0_FRAME2_INT_DriverIRQHandler(void)
{
    ENET_CommonFrame2IRQHandler(CONNECTIVITY__ENET0);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif
#if defined(CONNECTIVITY__ENET1)
void CONNECTIVITY_ENET1_FRAME0_EVENT_INT_DriverIRQHandler(void)
{
    ENET_CommonFrame0IRQHandler(CONNECTIVITY__ENET1);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#if FSL_FEATURE_ENET_QUEUE > 1
void CONNECTIVITY_ENET1_FRAME1_INT_DriverIRQHandler(void)
{
    ENET_CommonFrame1IRQHandler(CONNECTIVITY__ENET1);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void CONNECTIVITY_ENET1_FRAME2_INT_DriverIRQHandler(void)
{
    ENET_CommonFrame2IRQHandler(CONNECTIVITY__ENET1);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif

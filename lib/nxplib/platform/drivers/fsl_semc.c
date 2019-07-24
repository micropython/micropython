/*
 * Copyright 2017-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_semc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.semc"
#endif

/*! @brief Define macros for SEMC driver. */
#define SEMC_IPCOMMANDDATASIZEBYTEMAX (4U)
#define SEMC_IPCOMMANDMAGICKEY (0xA55A)
#define SEMC_IOCR_PINMUXBITWIDTH (0x3U)
#define SEMC_IOCR_NAND_CE (4U)
#define SEMC_IOCR_NOR_CE (5U)
#define SEMC_IOCR_NOR_CE_A8 (2U)
#define SEMC_IOCR_PSRAM_CE (6U)
#define SEMC_IOCR_PSRAM_CE_A8 (3U)
#define SEMC_IOCR_DBI_CSX (7U)
#define SEMC_IOCR_DBI_CSX_A8 (4U)
#define SEMC_NORFLASH_SRAM_ADDR_PORTWIDTHBASE (24U)
#define SEMC_NORFLASH_SRAM_ADDR_PORTWIDTHMAX (28U)
#define SEMC_BMCR0_TYPICAL_WQOS (5U)
#define SEMC_BMCR0_TYPICAL_WAGE (8U)
#define SEMC_BMCR0_TYPICAL_WSH (0x40U)
#define SEMC_BMCR0_TYPICAL_WRWS (0x10U)
#define SEMC_BMCR1_TYPICAL_WQOS (5U)
#define SEMC_BMCR1_TYPICAL_WAGE (8U)
#define SEMC_BMCR1_TYPICAL_WPH (0x60U)
#define SEMC_BMCR1_TYPICAL_WBR (0x40U)
#define SEMC_BMCR1_TYPICAL_WRWS (0x24U)
#define SEMC_STARTADDRESS (0x80000000U)
#define SEMC_ENDADDRESS (0xDFFFFFFFU)
#define SEMC_BR_MEMSIZE_MIN (4)
#define SEMC_BR_MEMSIZE_OFFSET (2)
#define SEMC_BR_MEMSIZE_MAX (4 * 1024 * 1024)
#define SEMC_SDRAM_MODESETCAL_OFFSET (4)
#define SEMC_BR_REG_NUM (9)
#define SEMC_BYTE_NUMBIT (8)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get instance number for SEMC module.
 *
 * @param base SEMC peripheral base address
 */
static uint32_t SEMC_GetInstance(SEMC_Type *base);

/*!
 * @brief Covert the input memory size to internal register set value.
 *
 * @param base SEMC peripheral base address
 * @param size_kbytes SEMC memory size in unit of kbytes.
 * @param sizeConverted SEMC converted memory size to 0 ~ 0x1F.
 * @return Execution status.
 */
static status_t SEMC_CovertMemorySize(SEMC_Type *base, uint32_t size_kbytes, uint8_t *sizeConverted);

/*!
 * @brief Covert the external timing nanosecond to internal clock cycle.
 *
 * @param time_ns   SEMC external time interval in unit of nanosecond.
 * @param clkSrc_Hz SEMC clock source frequency.
 * @return The changed internal clock cycle.
 */
static uint8_t SEMC_ConvertTiming(uint32_t time_ns, uint32_t clkSrc_Hz);

/*!
 * @brief Configure IP command.
 *
 * @param base SEMC peripheral base address.
 * @param size_bytes SEMC IP command data size.
 * @return Execution status.
 */
static status_t SEMC_ConfigureIPCommand(SEMC_Type *base, uint8_t size_bytes);

/*!
 * @brief Check if the IP command has finished.
 *
 * @param base SEMC peripheral base address.
 * @return Execution status.
 */
static status_t SEMC_IsIPCommandDone(SEMC_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to SEMC clocks for each instance. */
static const clock_ip_name_t s_semcClock[FSL_FEATURE_SOC_SEMC_COUNT]    = SEMC_CLOCKS;
static const clock_ip_name_t s_semcExtClock[FSL_FEATURE_SOC_SEMC_COUNT] = SEMC_EXSC_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*! @brief Pointers to SEMC bases for each instance. */
static SEMC_Type *const s_semcBases[] = SEMC_BASE_PTRS;
/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t SEMC_GetInstance(SEMC_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_semcBases); instance++)
    {
        if (s_semcBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_semcBases));

    return instance;
}

static status_t SEMC_CovertMemorySize(SEMC_Type *base, uint32_t size_kbytes, uint8_t *sizeConverted)
{
    assert(sizeConverted);
    uint32_t memsize;

    if ((size_kbytes < SEMC_BR_MEMSIZE_MIN) || (size_kbytes > SEMC_BR_MEMSIZE_MAX))
    {
        return kStatus_SEMC_InvalidMemorySize;
    }

    *sizeConverted = 0;
    memsize        = size_kbytes / 8;
    while (memsize)
    {
        memsize >>= 1;
        (*sizeConverted)++;
    }
    return kStatus_Success;
}

static uint8_t SEMC_ConvertTiming(uint32_t time_ns, uint32_t clkSrc_Hz)
{
    assert(clkSrc_Hz);

    uint8_t clockCycles = 0;
    uint32_t tClk_us;

    clkSrc_Hz /= 1000000;
    tClk_us = 1000000 / clkSrc_Hz;

    while (tClk_us * clockCycles < (time_ns * 1000))
    {
        clockCycles++;
    }

    return clockCycles;
}

static status_t SEMC_ConfigureIPCommand(SEMC_Type *base, uint8_t size_bytes)
{
    if ((size_bytes > SEMC_IPCOMMANDDATASIZEBYTEMAX) || (!size_bytes))
    {
        return kStatus_SEMC_InvalidIpcmdDataSize;
    }

    /* Set data size. */
    /* Note: It is better to set data size as the device data port width when transfering
     *    device command data. but for device memory data transfer, it can be set freely.
     * Note: If the data size is greater than data port width, for example, datsz = 4, data port = 16bit,
     *    then the 4-byte data transfer will be split into two 2-byte transfer, the slave address
     *    will be switched automatically according to connected device type*/
    base->IPCR1 = SEMC_IPCR1_DATSZ(size_bytes);
    /* Clear data size. */
    base->IPCR2 = 0;
    /* Set data size. */
    if (size_bytes < 4)
    {
        base->IPCR2 |= SEMC_IPCR2_BM3_MASK;
    }
    if (size_bytes < 3)
    {
        base->IPCR2 |= SEMC_IPCR2_BM2_MASK;
    }
    if (size_bytes < 2)
    {
        base->IPCR2 |= SEMC_IPCR2_BM1_MASK;
    }
    return kStatus_Success;
}

static status_t SEMC_IsIPCommandDone(SEMC_Type *base)
{
    /* Poll status bit till command is done*/
    while (!(base->INTR & SEMC_INTR_IPCMDDONE_MASK))
    {
    };

    /* Clear status bit */
    base->INTR |= SEMC_INTR_IPCMDDONE_MASK;

    /* Check error status */
    if (base->INTR & SEMC_INTR_IPCMDERR_MASK)
    {
        base->INTR |= SEMC_INTR_IPCMDERR_MASK;
        return kStatus_SEMC_IpCommandExecutionError;
    }

    return kStatus_Success;
}

/*!
 * brief Gets the SEMC default basic configuration structure.
 *
 * The purpose of this API is to get the default SEMC
 * configure structure for SEMC_Init(). User may use the initialized
 * structure unchanged in SEMC_Init(), or modify some fields of the
 * structure before calling SEMC_Init().
 * Example:
   code
   semc_config_t config;
   SEMC_GetDefaultConfig(&config);
   endcode
 * param config The SEMC configuration structure pointer.
 */
void SEMC_GetDefaultConfig(semc_config_t *config)
{
    assert(config);

    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    semc_queuea_weight_struct_t *queueaWeight = &(config->queueWeight.queueaWeight.queueaConfig);
    semc_queueb_weight_struct_t *queuebWeight = &(config->queueWeight.queuebWeight.queuebConfig);

    /* Get default settings. */
    config->dqsMode          = kSEMC_Loopbackinternal;
    config->cmdTimeoutCycles = 0;
    config->busTimeoutCycles = 0x1F;

    queueaWeight->qos              = SEMC_BMCR0_TYPICAL_WQOS;
    queueaWeight->aging            = SEMC_BMCR0_TYPICAL_WAGE;
    queueaWeight->slaveHitSwith    = SEMC_BMCR0_TYPICAL_WSH;
    queueaWeight->slaveHitNoswitch = SEMC_BMCR0_TYPICAL_WRWS;
    queuebWeight->qos              = SEMC_BMCR1_TYPICAL_WQOS;
    queuebWeight->aging            = SEMC_BMCR1_TYPICAL_WAGE;
    queuebWeight->slaveHitSwith    = SEMC_BMCR1_TYPICAL_WRWS;
    queuebWeight->weightPagehit    = SEMC_BMCR1_TYPICAL_WPH;
    queuebWeight->bankRotation     = SEMC_BMCR1_TYPICAL_WBR;
}

/*!
 * brief Initializes SEMC.
 * This function ungates the SEMC clock and initializes SEMC.
 * This function must be called before calling any other SEMC driver functions.
 *
 * param base SEMC peripheral base address.
 * param configure The SEMC configuration structure pointer.
 */
void SEMC_Init(SEMC_Type *base, semc_config_t *configure)
{
    assert(configure);

    uint8_t index = 0;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Un-gate sdram controller clock. */
    CLOCK_EnableClock(s_semcClock[SEMC_GetInstance(base)]);
    CLOCK_EnableClock(s_semcExtClock[SEMC_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* Initialize all BR to zero due to the default base address set. */
    for (index = 0; index < SEMC_BR_REG_NUM; index++)
    {
        base->BR[index] = 0;
    }

    /* Software reset for SEMC internal logical . */
    base->MCR = SEMC_MCR_SWRST_MASK;
    while (base->MCR & SEMC_MCR_SWRST_MASK)
    {
    }

    /* Configure, disable module first. */
    base->MCR |= SEMC_MCR_MDIS_MASK | SEMC_MCR_BTO(configure->busTimeoutCycles) |
                 SEMC_MCR_CTO(configure->cmdTimeoutCycles) | SEMC_MCR_DQSMD(configure->dqsMode);

    /* Configure Queue 0/1 for AXI bus. */
    base->BMCR0 = (uint32_t)(configure->queueWeight.queueaWeight.queueaValue);
    base->BMCR1 = (uint32_t)(configure->queueWeight.queuebWeight.queuebValue);

    /* Enable SEMC. */
    base->MCR &= ~SEMC_MCR_MDIS_MASK;
}

/*!
 * brief Deinitializes the SEMC module and gates the clock.
 * This function gates the SEMC clock. As a result, the SEMC
 * module doesn't work after calling this function.
 *
 * param base SEMC peripheral base address.
 */
void SEMC_Deinit(SEMC_Type *base)
{
    /* Disable module.  Check there is no pending command before disable module.  */
    while (!(base->STS0 & SEMC_STS0_IDLE_MASK))
    {
        ;
    }

    base->MCR |= SEMC_MCR_MDIS_MASK | SEMC_MCR_SWRST_MASK;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Disable SDRAM clock. */
    CLOCK_DisableClock(s_semcClock[SEMC_GetInstance(base)]);
    CLOCK_DisableClock(s_semcExtClock[SEMC_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Configures SDRAM controller in SEMC.
 *
 * param base SEMC peripheral base address.
 * param cs The chip selection.
 * param config The sdram configuration.
 * param clkSrc_Hz The SEMC clock frequency.
 */
status_t SEMC_ConfigureSDRAM(SEMC_Type *base, semc_sdram_cs_t cs, semc_sdram_config_t *config, uint32_t clkSrc_Hz)
{
    assert(config);
    assert(clkSrc_Hz);
    assert(config->refreshBurstLen);

    uint8_t memsize;
    status_t result   = kStatus_Success;
    uint16_t prescale = config->tPrescalePeriod_Ns / 16 / (1000000000 / clkSrc_Hz);
    uint16_t refresh;
    uint16_t urgentRef;
    uint16_t idle;
    uint16_t mode;

    if ((config->address < SEMC_STARTADDRESS) || (config->address > SEMC_ENDADDRESS))
    {
        return kStatus_SEMC_InvalidBaseAddress;
    }

    if (config->csxPinMux == kSEMC_MUXA8)
    {
        return kStatus_SEMC_InvalidSwPinmuxSelection;
    }

    if (prescale > 256)
    {
        return kStatus_SEMC_InvalidTimerSetting;
    }

    refresh   = config->refreshPeriod_nsPerRow / config->tPrescalePeriod_Ns;
    urgentRef = config->refreshUrgThreshold / config->tPrescalePeriod_Ns;
    idle      = config->tIdleTimeout_Ns / config->tPrescalePeriod_Ns;

    uint32_t iocReg = base->IOCR & ~(SEMC_IOCR_PINMUXBITWIDTH << config->csxPinMux);

    /* Base control. */
    result = SEMC_CovertMemorySize(base, config->memsize_kbytes, &memsize);
    if (result != kStatus_Success)
    {
        return result;
    }

    base->BR[cs]   = (config->address & SEMC_BR_BA_MASK) | SEMC_BR_MS(memsize) | SEMC_BR_VLD_MASK;
    base->SDRAMCR0 = SEMC_SDRAMCR0_PS(config->portSize) | SEMC_SDRAMCR0_BL(config->burstLen) |
                     SEMC_SDRAMCR0_COL(config->columnAddrBitNum) | SEMC_SDRAMCR0_CL(config->casLatency);
    /* IOMUX setting. */
    if (cs)
    {
        base->IOCR = iocReg | (cs << config->csxPinMux);
    }

    base->IOCR &= ~SEMC_IOCR_MUX_A8_MASK;

    /* Timing setting. */
    base->SDRAMCR1 = SEMC_SDRAMCR1_PRE2ACT(SEMC_ConvertTiming(config->tPrecharge2Act_Ns, clkSrc_Hz) - 1) |
                     SEMC_SDRAMCR1_ACT2RW(SEMC_ConvertTiming(config->tAct2ReadWrite_Ns, clkSrc_Hz) - 1) |
                     SEMC_SDRAMCR1_RFRC(SEMC_ConvertTiming(config->tRefreshRecovery_Ns, clkSrc_Hz) - 1) |
                     SEMC_SDRAMCR1_WRC(SEMC_ConvertTiming(config->tWriteRecovery_Ns, clkSrc_Hz) - 1) |
                     SEMC_SDRAMCR1_CKEOFF(SEMC_ConvertTiming(config->tCkeOff_Ns, clkSrc_Hz) - 1) |
                     SEMC_SDRAMCR1_ACT2PRE(SEMC_ConvertTiming(config->tAct2Prechage_Ns, clkSrc_Hz) - 1);
    base->SDRAMCR2 =
        SEMC_SDRAMCR2_SRRC(SEMC_ConvertTiming(config->tSelfRefRecovery_Ns, clkSrc_Hz) - 1) |
        SEMC_SDRAMCR2_REF2REF(
            SEMC_ConvertTiming(config->tRefresh2Refresh_Ns, clkSrc_Hz)) |           /* No Minus one to keep with RM */
        SEMC_SDRAMCR2_ACT2ACT(SEMC_ConvertTiming(config->tAct2Act_Ns, clkSrc_Hz)) | /* No Minus one to keep with RM */
        SEMC_SDRAMCR2_ITO(idle);
    base->SDRAMCR3 = SEMC_SDRAMCR3_REBL(config->refreshBurstLen - 1) |
                     /* N * 16 * 1s / clkSrc_Hz = config->tPrescalePeriod_Ns */
                     SEMC_SDRAMCR3_PRESCALE(prescale) | SEMC_SDRAMCR3_RT(refresh) | SEMC_SDRAMCR3_UT(urgentRef);

    SEMC->IPCR1 = 0x2;
    SEMC->IPCR2 = 0;

    result = SEMC_SendIPCommand(base, kSEMC_MemType_SDRAM, config->address, kSEMC_SDRAMCM_Prechargeall, 0, NULL);
    if (result != kStatus_Success)
    {
        return result;
    }
    result = SEMC_SendIPCommand(base, kSEMC_MemType_SDRAM, config->address, kSEMC_SDRAMCM_AutoRefresh, 0, NULL);
    if (result != kStatus_Success)
    {
        return result;
    }
    result = SEMC_SendIPCommand(base, kSEMC_MemType_SDRAM, config->address, kSEMC_SDRAMCM_AutoRefresh, 0, NULL);
    if (result != kStatus_Success)
    {
        return result;
    }
    /* Mode setting value. */
    mode   = (uint16_t)config->burstLen | (uint16_t)(config->casLatency << SEMC_SDRAM_MODESETCAL_OFFSET);
    result = SEMC_SendIPCommand(base, kSEMC_MemType_SDRAM, config->address, kSEMC_SDRAMCM_Modeset, mode, NULL);
    if (result != kStatus_Success)
    {
        return result;
    }
    /* Enables refresh */
    base->SDRAMCR3 |= SEMC_SDRAMCR3_REN_MASK;

    return kStatus_Success;
}

/*!
 * brief Configures NAND controller in SEMC.
 *
 * param base SEMC peripheral base address.
 * param config The nand configuration.
 * param clkSrc_Hz The SEMC clock frequency.
 */
status_t SEMC_ConfigureNAND(SEMC_Type *base, semc_nand_config_t *config, uint32_t clkSrc_Hz)
{
    assert(config);
    assert(config->timingConfig);

    uint8_t memsize;
    status_t result;

    if ((config->axiAddress < SEMC_STARTADDRESS) || (config->axiAddress > SEMC_ENDADDRESS))
    {
        return kStatus_SEMC_InvalidBaseAddress;
    }

    if (config->cePinMux == kSEMC_MUXRDY)
    {
        return kStatus_SEMC_InvalidSwPinmuxSelection;
    }

    uint32_t iocReg = base->IOCR & ~((SEMC_IOCR_PINMUXBITWIDTH << config->cePinMux) | SEMC_IOCR_MUX_RDY_MASK);

    /* Base control. */
    if (config->rdyactivePolarity == kSEMC_RdyActivehigh)
    {
        base->MCR |= SEMC_MCR_WPOL1_MASK;
    }
    else
    {
        base->MCR &= ~SEMC_MCR_WPOL1_MASK;
    }
    result = SEMC_CovertMemorySize(base, config->axiMemsize_kbytes, &memsize);
    if (result != kStatus_Success)
    {
        return result;
    }
    base->BR[4] = (config->axiAddress & SEMC_BR_BA_MASK) | SEMC_BR_MS(memsize) | SEMC_BR_VLD_MASK;

    result = SEMC_CovertMemorySize(base, config->ipgMemsize_kbytes, &memsize);
    if (result != kStatus_Success)
    {
        return result;
    }
    base->BR[8] = (config->ipgAddress & SEMC_BR_BA_MASK) | SEMC_BR_MS(memsize) | SEMC_BR_VLD_MASK;

    /* IOMUX setting. */
    if (config->cePinMux)
    {
        base->IOCR = iocReg | (SEMC_IOCR_NAND_CE << config->cePinMux);
    }
    else
    {
        base->IOCR = iocReg | (1U << config->cePinMux);
    }

    base->NANDCR0 = SEMC_NANDCR0_PS(config->portSize) | SEMC_NANDCR0_BL(config->burstLen) |
                    SEMC_NANDCR0_EDO(config->edoModeEnabled) | SEMC_NANDCR0_COL(config->columnAddrBitNum);

    /* Timing setting. */
    base->NANDCR1 = SEMC_NANDCR1_CES(SEMC_ConvertTiming(config->timingConfig->tCeSetup_Ns, clkSrc_Hz) - 1) |
                    SEMC_NANDCR1_CEH(SEMC_ConvertTiming(config->timingConfig->tCeHold_Ns, clkSrc_Hz) - 1) |
                    SEMC_NANDCR1_WEL(SEMC_ConvertTiming(config->timingConfig->tWeLow_Ns, clkSrc_Hz) - 1) |
                    SEMC_NANDCR1_WEH(SEMC_ConvertTiming(config->timingConfig->tWeHigh_Ns, clkSrc_Hz) - 1) |
                    SEMC_NANDCR1_REL(SEMC_ConvertTiming(config->timingConfig->tReLow_Ns, clkSrc_Hz) - 1) |
                    SEMC_NANDCR1_REH(SEMC_ConvertTiming(config->timingConfig->tReHigh_Ns, clkSrc_Hz) - 1) |
                    SEMC_NANDCR1_TA(SEMC_ConvertTiming(config->timingConfig->tTurnAround_Ns, clkSrc_Hz) - 1) |
                    SEMC_NANDCR1_CEITV(SEMC_ConvertTiming(config->timingConfig->tCeInterval_Ns, clkSrc_Hz) - 1);
    base->NANDCR2 = SEMC_NANDCR2_TWHR(SEMC_ConvertTiming(config->timingConfig->tWehigh2Relow_Ns, clkSrc_Hz) - 1) |
                    SEMC_NANDCR2_TRHW(SEMC_ConvertTiming(config->timingConfig->tRehigh2Welow_Ns, clkSrc_Hz) - 1) |
                    SEMC_NANDCR2_TADL(SEMC_ConvertTiming(config->timingConfig->tAle2WriteStart_Ns, clkSrc_Hz) - 1) |
                    SEMC_NANDCR2_TRR(SEMC_ConvertTiming(config->timingConfig->tReady2Relow_Ns, clkSrc_Hz) - 1) |
                    SEMC_NANDCR2_TWB(SEMC_ConvertTiming(config->timingConfig->tWehigh2Busy_Ns, clkSrc_Hz) - 1);
    base->NANDCR3 = config->arrayAddrOption;
    return kStatus_Success;
}

/*!
 * brief Configures NOR controller in SEMC.
 *
 * param base SEMC peripheral base address.
 * param config The nor configuration.
 * param clkSrc_Hz The SEMC clock frequency.
 */
status_t SEMC_ConfigureNOR(SEMC_Type *base, semc_nor_config_t *config, uint32_t clkSrc_Hz)
{
    assert(config);

    uint8_t memsize;
    status_t result;

    if ((config->address < SEMC_STARTADDRESS) || (config->address > SEMC_ENDADDRESS))
    {
        return kStatus_SEMC_InvalidBaseAddress;
    }

    uint32_t iocReg = base->IOCR & ~(SEMC_IOCR_PINMUXBITWIDTH << config->cePinMux);
    uint32_t muxCe  = (config->cePinMux == kSEMC_MUXRDY) ?
                         SEMC_IOCR_NOR_CE - 1 :
                         ((config->cePinMux == kSEMC_MUXA8) ? SEMC_IOCR_NOR_CE_A8 : SEMC_IOCR_NOR_CE);

    /* IOMUX setting. */
    base->IOCR = iocReg | (muxCe << config->cePinMux);
    /* Address bit setting. */
    if (config->addrPortWidth > SEMC_NORFLASH_SRAM_ADDR_PORTWIDTHBASE)
    {
        if (config->addrPortWidth >= SEMC_NORFLASH_SRAM_ADDR_PORTWIDTHBASE + 1)
        {
            /* Address bit 24 (A24) */
            base->IOCR &= (uint32_t)~SEMC_IOCR_MUX_CSX0_MASK;
            if (config->cePinMux == kSEMC_MUXCSX0)
            {
                return kStatus_SEMC_InvalidSwPinmuxSelection;
            }
        }
        if (config->addrPortWidth >= SEMC_NORFLASH_SRAM_ADDR_PORTWIDTHBASE + 2)
        {
            /* Address bit 25 (A25) */
            base->IOCR &= (uint32_t)~SEMC_IOCR_MUX_CSX1_MASK;
            if (config->cePinMux == kSEMC_MUXCSX1)
            {
                return kStatus_SEMC_InvalidSwPinmuxSelection;
            }
        }
        if (config->addrPortWidth >= SEMC_NORFLASH_SRAM_ADDR_PORTWIDTHBASE + 3)
        {
            /* Address bit 26 (A26) */
            base->IOCR &= (uint32_t)~SEMC_IOCR_MUX_CSX2_MASK;
            if (config->cePinMux == kSEMC_MUXCSX2)
            {
                return kStatus_SEMC_InvalidSwPinmuxSelection;
            }
        }
        if (config->addrPortWidth >= SEMC_NORFLASH_SRAM_ADDR_PORTWIDTHBASE + 4)
        {
            if (config->addr27 == kSEMC_NORA27_MUXCSX3)
            {
                /* Address bit 27 (A27) */
                base->IOCR &= (uint32_t)~SEMC_IOCR_MUX_CSX3_MASK;
            }
            else if (config->addr27 == kSEMC_NORA27_MUXRDY)
            {
                base->IOCR |= SEMC_IOCR_MUX_RDY_MASK;
            }
            else
            {
                return kStatus_SEMC_InvalidSwPinmuxSelection;
            }
            if (config->cePinMux == kSEMC_MUXCSX3)
            {
                return kStatus_SEMC_InvalidSwPinmuxSelection;
            }
        }
        if (config->addrPortWidth > SEMC_NORFLASH_SRAM_ADDR_PORTWIDTHMAX)
        {
            return kStatus_SEMC_InvalidAddressPortWidth;
        }
    }

    /* Base control. */
    if (config->rdyactivePolarity == kSEMC_RdyActivehigh)
    {
        base->MCR |= SEMC_MCR_WPOL0_MASK;
    }
    else
    {
        base->MCR &= ~SEMC_MCR_WPOL0_MASK;
    }
    result = SEMC_CovertMemorySize(base, config->memsize_kbytes, &memsize);
    if (result != kStatus_Success)
    {
        return result;
    }
    base->BR[5]  = (config->address & SEMC_BR_BA_MASK) | SEMC_BR_MS(memsize) | SEMC_BR_VLD_MASK;
    base->NORCR0 = SEMC_NORCR0_PS(config->portSize) | SEMC_NORCR0_BL(config->burstLen) |
                   SEMC_NORCR0_AM(config->addrMode) | SEMC_NORCR0_ADVP(config->advActivePolarity) |
                   SEMC_NORCR0_COL(config->columnAddrBitNum);

    /* Timing setting. */
    base->NORCR1 = SEMC_NORCR1_CES(SEMC_ConvertTiming(config->tCeSetup_Ns, clkSrc_Hz)) |
                   SEMC_NORCR1_CEH(SEMC_ConvertTiming(config->tCeHold_Ns, clkSrc_Hz)) |
                   SEMC_NORCR1_AS(SEMC_ConvertTiming(config->tAddrSetup_Ns, clkSrc_Hz)) |
                   SEMC_NORCR1_AH(SEMC_ConvertTiming(config->tAddrHold_Ns, clkSrc_Hz)) |
                   SEMC_NORCR1_WEL(SEMC_ConvertTiming(config->tWeLow_Ns, clkSrc_Hz)) |
                   SEMC_NORCR1_WEH(SEMC_ConvertTiming(config->tWeHigh_Ns, clkSrc_Hz)) |
                   SEMC_NORCR1_REL(SEMC_ConvertTiming(config->tReLow_Ns, clkSrc_Hz)) |
                   SEMC_NORCR1_REH(SEMC_ConvertTiming(config->tReHigh_Ns, clkSrc_Hz));
    base->NORCR2 =
#if defined(FSL_FEATURE_SEMC_HAS_NOR_WDS_TIME) && (FSL_FEATURE_SEMC_HAS_NOR_WDS_TIME)
        SEMC_NORCR2_WDS(SEMC_ConvertTiming(config->tWriteSetup_Ns, clkSrc_Hz)) |
#endif /* FSL_FEATURE_SEMC_HAS_NOR_WDS_TIME */
#if defined(FSL_FEATURE_SEMC_HAS_NOR_WDH_TIME) && (FSL_FEATURE_SEMC_HAS_NOR_WDH_TIME)
        SEMC_NORCR2_WDH(SEMC_ConvertTiming(config->tWriteHold_Ns, clkSrc_Hz)) |
#endif /* FSL_FEATURE_SEMC_HAS_NOR_WDH_TIME */
        SEMC_NORCR2_TA(SEMC_ConvertTiming(config->tTurnAround_Ns, clkSrc_Hz)) |
        SEMC_NORCR2_AWDH(SEMC_ConvertTiming(config->tAddr2WriteHold_Ns, clkSrc_Hz) + 1) |
        SEMC_NORCR2_LC(config->latencyCount) | SEMC_NORCR2_RD(config->readCycle) |
        SEMC_NORCR2_CEITV(SEMC_ConvertTiming(config->tCeInterval_Ns, clkSrc_Hz));

    return SEMC_ConfigureIPCommand(base, (config->portSize + 1));
}

/*!
 * brief Configures SRAM controller in SEMC.
 *
 * param base SEMC peripheral base address.
 * param config The sram configuration.
 * param clkSrc_Hz The SEMC clock frequency.
 */
status_t SEMC_ConfigureSRAM(SEMC_Type *base, semc_sram_config_t *config, uint32_t clkSrc_Hz)
{
    assert(config);

    uint8_t memsize;
    status_t result = kStatus_Success;

    if ((config->address < SEMC_STARTADDRESS) || (config->address > SEMC_ENDADDRESS))
    {
        return kStatus_SEMC_InvalidBaseAddress;
    }

    uint32_t iocReg = base->IOCR & ~(SEMC_IOCR_PINMUXBITWIDTH << config->cePinMux);
    uint32_t muxCe  = (config->cePinMux == kSEMC_MUXRDY) ?
                         SEMC_IOCR_PSRAM_CE - 1 :
                         ((config->cePinMux == kSEMC_MUXA8) ? SEMC_IOCR_PSRAM_CE_A8 : SEMC_IOCR_PSRAM_CE);

    /* IOMUX setting. */
    base->IOCR = iocReg | (muxCe << config->cePinMux);
    /* Address bit setting. */
    if (config->addrPortWidth > SEMC_NORFLASH_SRAM_ADDR_PORTWIDTHBASE)
    {
        if (config->addrPortWidth >= SEMC_NORFLASH_SRAM_ADDR_PORTWIDTHBASE + 1)
        {
            /* Address bit 24 (A24) */
            base->IOCR &= (uint32_t)~SEMC_IOCR_MUX_CSX0_MASK;
            if (config->cePinMux == kSEMC_MUXCSX0)
            {
                return kStatus_SEMC_InvalidSwPinmuxSelection;
            }
        }
        if (config->addrPortWidth >= SEMC_NORFLASH_SRAM_ADDR_PORTWIDTHBASE + 2)
        {
            /* Address bit 25 (A25) */
            base->IOCR &= (uint32_t)~SEMC_IOCR_MUX_CSX1_MASK;
            if (config->cePinMux == kSEMC_MUXCSX1)
            {
                return kStatus_SEMC_InvalidSwPinmuxSelection;
            }
        }
        if (config->addrPortWidth >= SEMC_NORFLASH_SRAM_ADDR_PORTWIDTHBASE + 3)
        {
            /* Address bit 26 (A26) */
            base->IOCR &= (uint32_t)~SEMC_IOCR_MUX_CSX2_MASK;
            if (config->cePinMux == kSEMC_MUXCSX2)
            {
                return kStatus_SEMC_InvalidSwPinmuxSelection;
            }
        }
        if (config->addrPortWidth >= SEMC_NORFLASH_SRAM_ADDR_PORTWIDTHBASE + 4)
        {
            if (config->addr27 == kSEMC_NORA27_MUXCSX3)
            {
                /* Address bit 27 (A27) */
                base->IOCR &= (uint32_t)~SEMC_IOCR_MUX_CSX3_MASK;
            }
            else if (config->addr27 == kSEMC_NORA27_MUXRDY)
            {
                base->IOCR |= SEMC_IOCR_MUX_RDY_MASK;
            }
            else
            {
                return kStatus_SEMC_InvalidSwPinmuxSelection;
            }

            if (config->cePinMux == kSEMC_MUXCSX3)
            {
                return kStatus_SEMC_InvalidSwPinmuxSelection;
            }
        }
        if (config->addrPortWidth > SEMC_NORFLASH_SRAM_ADDR_PORTWIDTHMAX)
        {
            return kStatus_SEMC_InvalidAddressPortWidth;
        }
    }
    /* Base control. */
    result = SEMC_CovertMemorySize(base, config->memsize_kbytes, &memsize);
    if (result != kStatus_Success)
    {
        return result;
    }
    base->BR[6]   = (config->address & SEMC_BR_BA_MASK) | SEMC_BR_MS(memsize) | SEMC_BR_VLD_MASK;
    base->SRAMCR0 = SEMC_SRAMCR0_PS(config->portSize) | SEMC_SRAMCR0_BL(config->burstLen) |
                    SEMC_SRAMCR0_AM(config->addrMode) | SEMC_SRAMCR0_ADVP(config->advActivePolarity) |
                    SEMC_SRAMCR0_COL_MASK;

    /* Timing setting. */
    base->SRAMCR1 = SEMC_SRAMCR1_CES(SEMC_ConvertTiming(config->tCeSetup_Ns, clkSrc_Hz)) |
                    SEMC_SRAMCR1_CEH(SEMC_ConvertTiming(config->tCeHold_Ns, clkSrc_Hz)) |
                    SEMC_SRAMCR1_AS(SEMC_ConvertTiming(config->tAddrSetup_Ns, clkSrc_Hz)) |
                    SEMC_SRAMCR1_AH(SEMC_ConvertTiming(config->tAddrHold_Ns, clkSrc_Hz)) |
                    SEMC_SRAMCR1_WEL(SEMC_ConvertTiming(config->tWeLow_Ns, clkSrc_Hz)) |
                    SEMC_SRAMCR1_WEH(SEMC_ConvertTiming(config->tWeHigh_Ns, clkSrc_Hz)) |
                    SEMC_SRAMCR1_REL(SEMC_ConvertTiming(config->tReLow_Ns, clkSrc_Hz)) |
                    SEMC_SRAMCR1_REH(SEMC_ConvertTiming(config->tReHigh_Ns, clkSrc_Hz));

    base->SRAMCR2 = SEMC_SRAMCR2_WDS(SEMC_ConvertTiming(config->tWriteSetup_Ns, clkSrc_Hz)) |
                    SEMC_SRAMCR2_WDH(SEMC_ConvertTiming(config->tWriteHold_Ns, clkSrc_Hz)) |
                    SEMC_SRAMCR2_TA(SEMC_ConvertTiming(config->tTurnAround_Ns, clkSrc_Hz)) |
                    SEMC_SRAMCR2_AWDH(SEMC_ConvertTiming(config->tAddr2WriteHold_Ns, clkSrc_Hz) + 1) |
                    SEMC_SRAMCR2_LC(config->latencyCount) | SEMC_SRAMCR2_RD(config->readCycle) |
                    SEMC_SRAMCR2_CEITV(SEMC_ConvertTiming(config->tCeInterval_Ns, clkSrc_Hz));

    return result;
}

/*!
 * brief Configures DBI controller in SEMC.
 *
 * param base SEMC peripheral base address.
 * param config The dbi configuration.
 * param clkSrc_Hz The SEMC clock frequency.
 */
status_t SEMC_ConfigureDBI(SEMC_Type *base, semc_dbi_config_t *config, uint32_t clkSrc_Hz)
{
    assert(config);

    uint8_t memsize;
    status_t result;

    if ((config->address < SEMC_STARTADDRESS) || (config->address > SEMC_ENDADDRESS))
    {
        return kStatus_SEMC_InvalidBaseAddress;
    }

    uint32_t iocReg = base->IOCR & ~(SEMC_IOCR_PINMUXBITWIDTH << config->csxPinMux);
    uint32_t muxCsx = (config->csxPinMux == kSEMC_MUXRDY) ?
                          SEMC_IOCR_DBI_CSX - 1 :
                          ((config->csxPinMux == kSEMC_MUXA8) ? SEMC_IOCR_DBI_CSX_A8 : SEMC_IOCR_DBI_CSX);

    /* IOMUX setting. */
    base->IOCR = iocReg | (muxCsx << config->csxPinMux);
    /* Base control. */
    result = SEMC_CovertMemorySize(base, config->memsize_kbytes, &memsize);
    if (result != kStatus_Success)
    {
        return result;
    }
    base->BR[7] = (config->address & SEMC_BR_BA_MASK) | SEMC_BR_MS(memsize) | SEMC_BR_VLD_MASK;
    base->DBICR0 =
        SEMC_DBICR0_PS(config->portSize) | SEMC_DBICR0_BL(config->burstLen) | SEMC_DBICR0_COL(config->columnAddrBitNum);

    /* Timing setting. */
    base->DBICR1 = SEMC_DBICR1_CES(SEMC_ConvertTiming(config->tCsxSetup_Ns, clkSrc_Hz) - 1) |
                   SEMC_DBICR1_CEH(SEMC_ConvertTiming(config->tCsxHold_Ns, clkSrc_Hz) - 1) |
                   SEMC_DBICR1_WEL(SEMC_ConvertTiming(config->tWexLow_Ns, clkSrc_Hz) - 1) |
                   SEMC_DBICR1_WEH(SEMC_ConvertTiming(config->tWexHigh_Ns, clkSrc_Hz) - 1) |
                   SEMC_DBICR1_REL(SEMC_ConvertTiming(config->tRdxLow_Ns, clkSrc_Hz) - 1) |
                   SEMC_DBICR1_REH(SEMC_ConvertTiming(config->tRdxHigh_Ns, clkSrc_Hz) - 1) |
                   SEMC_DBICR1_CEITV(SEMC_ConvertTiming(config->tCsxInterval_Ns, clkSrc_Hz) - 1);
    return SEMC_ConfigureIPCommand(base, (config->portSize + 1));
}

/*!
 * brief SEMC IP command access.
 *
 * param base  SEMC peripheral base address.
 * param type  SEMC memory type. refer to "semc_mem_type_t"
 * param address  SEMC device address.
 * param command  SEMC IP command.
 * For NAND device, we should use the SEMC_BuildNandIPCommand to get the right nand command.
 * For NOR/DBI device, take refer to "semc_ipcmd_nor_dbi_t".
 * For SRAM device, take refer to "semc_ipcmd_sram_t".
 * For SDRAM device, take refer to "semc_ipcmd_sdram_t".
 * param write  Data for write access.
 * param read   Data pointer for read data out.
 */
status_t SEMC_SendIPCommand(
    SEMC_Type *base, semc_mem_type_t type, uint32_t address, uint16_t command, uint32_t write, uint32_t *read)
{
    uint32_t cmdMode;
    bool readCmd  = 0;
    bool writeCmd = 0;
    status_t result;

    /* Clear status bit */
    base->INTR |= SEMC_INTR_IPCMDDONE_MASK;
    /* Set address. */
    base->IPCR0 = address;

    /* Check command mode. */
    cmdMode = command & 0xFU;
    switch (type)
    {
        case kSEMC_MemType_NAND:
            readCmd = (cmdMode == kSEMC_NANDCM_CommandAddressRead) || (cmdMode == kSEMC_NANDCM_CommandRead) ||
                      (cmdMode == kSEMC_NANDCM_Read);
            writeCmd = (cmdMode == kSEMC_NANDCM_CommandAddressWrite) || (cmdMode == kSEMC_NANDCM_CommandWrite) ||
                       (cmdMode == kSEMC_NANDCM_Write);
            break;
        case kSEMC_MemType_NOR:
        case kSEMC_MemType_8080:
            readCmd  = (cmdMode == kSEMC_NORDBICM_Read);
            writeCmd = (cmdMode == kSEMC_NORDBICM_Write);
            break;
        case kSEMC_MemType_SRAM:
            readCmd  = (cmdMode == kSEMC_SRAMCM_ArrayRead) || (cmdMode == kSEMC_SRAMCM_RegRead);
            writeCmd = (cmdMode == kSEMC_SRAMCM_ArrayWrite) || (cmdMode == kSEMC_SRAMCM_RegWrite);
            break;
        case kSEMC_MemType_SDRAM:
            readCmd  = (cmdMode == kSEMC_SDRAMCM_Read);
            writeCmd = (cmdMode == kSEMC_SDRAMCM_Write) || (cmdMode == kSEMC_SDRAMCM_Modeset);
            break;
        default:
            break;
    }

    if (writeCmd)
    {
        /* Set data. */
        base->IPTXDAT = write;
    }

    /* Set command code. */
    base->IPCMD = command | SEMC_IPCMD_KEY(SEMC_IPCOMMANDMAGICKEY);
    /* Wait for command done. */
    result = SEMC_IsIPCommandDone(base);
    if (result != kStatus_Success)
    {
        return result;
    }

    if (readCmd)
    {
        /* Get the read data */
        *read = base->IPRXDAT;
    }

    return kStatus_Success;
}

/*!
 * brief SEMC NAND device memory write through IP command.
 *
 * param base  SEMC peripheral base address.
 * param address  SEMC NAND device address.
 * param data  Data for write access.
 * param size_bytes   Data length.
 */
status_t SEMC_IPCommandNandWrite(SEMC_Type *base, uint32_t address, uint8_t *data, uint32_t size_bytes)
{
    assert(data);

    status_t result = kStatus_Success;
    uint16_t ipCmd;
    uint32_t tempData = 0;

    /* Write command built */
    ipCmd = SEMC_BuildNandIPCommand(0, kSEMC_NANDAM_ColumnRow, kSEMC_NANDCM_Write);
    while (size_bytes >= SEMC_IPCOMMANDDATASIZEBYTEMAX)
    {
        /* Configure IP command data size. */
        SEMC_ConfigureIPCommand(base, SEMC_IPCOMMANDDATASIZEBYTEMAX);
        result = SEMC_SendIPCommand(base, kSEMC_MemType_NAND, address, ipCmd, *(uint32_t *)data, NULL);
        if (result != kStatus_Success)
        {
            break;
        }

        data += SEMC_IPCOMMANDDATASIZEBYTEMAX;
        size_bytes -= SEMC_IPCOMMANDDATASIZEBYTEMAX;
    }

    if ((result == kStatus_Success) && size_bytes)
    {
        SEMC_ConfigureIPCommand(base, size_bytes);

        while (size_bytes)
        {
            tempData |= ((uint32_t) * (data + size_bytes - 1) << ((size_bytes - 1) * SEMC_BYTE_NUMBIT));
            size_bytes--;
        }

        result = SEMC_SendIPCommand(base, kSEMC_MemType_NAND, address, ipCmd, tempData, NULL);
    }

    return result;
}

/*!
 * brief SEMC NAND device memory read through IP command.
 *
 * param base  SEMC peripheral base address.
 * param address  SEMC NAND device address.
 * param data  Data pointer for data read out.
 * param size_bytes   Data length.
 */
status_t SEMC_IPCommandNandRead(SEMC_Type *base, uint32_t address, uint8_t *data, uint32_t size_bytes)
{
    assert(data);

    status_t result = kStatus_Success;
    uint16_t ipCmd;
    uint32_t tempData = 0;

    /* Configure IP command data size. */
    SEMC_ConfigureIPCommand(base, SEMC_IPCOMMANDDATASIZEBYTEMAX);
    /* Read command built */
    ipCmd = SEMC_BuildNandIPCommand(0, kSEMC_NANDAM_ColumnRow, kSEMC_NANDCM_Read);

    while (size_bytes >= SEMC_IPCOMMANDDATASIZEBYTEMAX)
    {
        result = SEMC_SendIPCommand(base, kSEMC_MemType_NAND, address, ipCmd, 0, (uint32_t *)data);
        if (result != kStatus_Success)
        {
            break;
        }

        data += SEMC_IPCOMMANDDATASIZEBYTEMAX;
        size_bytes -= SEMC_IPCOMMANDDATASIZEBYTEMAX;
    }

    if ((result == kStatus_Success) && size_bytes)
    {
        SEMC_ConfigureIPCommand(base, size_bytes);
        result = SEMC_SendIPCommand(base, kSEMC_MemType_NAND, address, ipCmd, 0, &tempData);

        while (size_bytes)
        {
            size_bytes--;
            *(data + size_bytes) = (tempData >> (SEMC_BYTE_NUMBIT * size_bytes)) & 0xFFU;
        }
    }

    return result;
}

/*!
 * brief SEMC NOR device memory read through IP command.
 *
 * param base  SEMC peripheral base address.
 * param address  SEMC NOR device address.
 * param data  Data pointer for data read out.
 * param size_bytes   Data length.
 */
status_t SEMC_IPCommandNorRead(SEMC_Type *base, uint32_t address, uint8_t *data, uint32_t size_bytes)
{
    assert(data);

    uint32_t tempData = 0;
    status_t result   = kStatus_Success;
    uint8_t dataSize  = base->NORCR0 & SEMC_NORCR0_PS_MASK;

    /* Configure IP command data size. */
    SEMC_ConfigureIPCommand(base, SEMC_IPCOMMANDDATASIZEBYTEMAX);

    while (size_bytes >= SEMC_IPCOMMANDDATASIZEBYTEMAX)
    {
        result = SEMC_SendIPCommand(base, kSEMC_MemType_NOR, address, kSEMC_NORDBICM_Read, 0, (uint32_t *)data);
        if (result != kStatus_Success)
        {
            break;
        }

        data += SEMC_IPCOMMANDDATASIZEBYTEMAX;
        size_bytes -= SEMC_IPCOMMANDDATASIZEBYTEMAX;
    }

    if ((result == kStatus_Success) && size_bytes)
    {
        SEMC_ConfigureIPCommand(base, size_bytes);
        result = SEMC_SendIPCommand(base, kSEMC_MemType_NOR, address, kSEMC_NORDBICM_Read, 0, &tempData);
        while (size_bytes)
        {
            size_bytes--;
            *(data + size_bytes) = (tempData >> (SEMC_BYTE_NUMBIT * size_bytes)) & 0xFFU;
        }
    }

    SEMC_ConfigureIPCommand(base, dataSize);
    return result;
}

/*!
 * brief SEMC NOR device memory write through IP command.
 *
 * param base  SEMC peripheral base address.
 * param address  SEMC NOR device address.
 * param data  Data for write access.
 * param size_bytes   Data length.
 */
status_t SEMC_IPCommandNorWrite(SEMC_Type *base, uint32_t address, uint8_t *data, uint32_t size_bytes)
{
    assert(data);

    uint32_t tempData = 0;
    status_t result   = kStatus_Success;
    uint8_t dataSize  = base->NORCR0 & SEMC_NORCR0_PS_MASK;

    /* Write command built */
    while (size_bytes >= SEMC_IPCOMMANDDATASIZEBYTEMAX)
    {
        /* Configure IP command data size. */
        SEMC_ConfigureIPCommand(base, SEMC_IPCOMMANDDATASIZEBYTEMAX);
        result = SEMC_SendIPCommand(base, kSEMC_MemType_NOR, address, kSEMC_NORDBICM_Write, *(uint32_t *)data, NULL);
        if (result != kStatus_Success)
        {
            break;
        }
        size_bytes -= SEMC_IPCOMMANDDATASIZEBYTEMAX;
        data += SEMC_IPCOMMANDDATASIZEBYTEMAX;
    }

    if ((result == kStatus_Success) && size_bytes)
    {
        SEMC_ConfigureIPCommand(base, size_bytes);

        while (size_bytes)
        {
            tempData |= ((uint32_t) * (data + size_bytes - 1) << ((size_bytes - 1) * SEMC_BYTE_NUMBIT));
            size_bytes--;
        }

        result = SEMC_SendIPCommand(base, kSEMC_MemType_NOR, address, kSEMC_NORDBICM_Write, tempData, NULL);
    }
    SEMC_ConfigureIPCommand(base, dataSize);

    return result;
}

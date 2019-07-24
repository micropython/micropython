/*
 * Copyright (c) 2015-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_COMMON_H_
#define _FSL_COMMON_H_

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#if defined(__ICCARM__)
#include <stddef.h>
#endif

#include "fsl_device_registers.h"

/*!
 * @addtogroup ksdk_common
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Construct a status code value from a group and code number. */
#define MAKE_STATUS(group, code) ((((group)*100) + (code)))

/*! @brief Construct the version number for drivers. */
#define MAKE_VERSION(major, minor, bugfix) (((major) << 16) | ((minor) << 8) | (bugfix))

/*! @name Driver version */
/*@{*/
/*! @brief common driver version 2.0.1. */
#define FSL_COMMON_DRIVER_VERSION (MAKE_VERSION(2, 1, 0))
/*@}*/

/* Debug console type definition. */
#define DEBUG_CONSOLE_DEVICE_TYPE_NONE          0U      /*!< No debug console.             */
#define DEBUG_CONSOLE_DEVICE_TYPE_UART          1U      /*!< Debug console based on UART.   */
#define DEBUG_CONSOLE_DEVICE_TYPE_LPUART        2U      /*!< Debug console based on LPUART. */
#define DEBUG_CONSOLE_DEVICE_TYPE_LPSCI         3U      /*!< Debug console based on LPSCI.  */
#define DEBUG_CONSOLE_DEVICE_TYPE_USBCDC        4U      /*!< Debug console based on USBCDC. */
#define DEBUG_CONSOLE_DEVICE_TYPE_FLEXCOMM      5U      /*!< Debug console based on FLEXCOMM. */
#define DEBUG_CONSOLE_DEVICE_TYPE_IUART         6U      /*!< Debug console based on i.MX UART. */
#define DEBUG_CONSOLE_DEVICE_TYPE_VUSART        7U      /*!< Debug console based on LPC_VUSART. */
#define DEBUG_CONSOLE_DEVICE_TYPE_MINI_USART    8U      /*!< Debug console based on LPC_USART. */
#define DEBUG_CONSOLE_DEVICE_TYPE_SWO           9U      /*!< Debug console based on SWO. */

/*! @brief Status group numbers. */
enum _status_groups
{
    kStatusGroup_Generic = 0,                 /*!< Group number for generic status codes. */
    kStatusGroup_FLASH = 1,                   /*!< Group number for FLASH status codes. */
    kStatusGroup_LPSPI = 4,                   /*!< Group number for LPSPI status codes. */
    kStatusGroup_FLEXIO_SPI = 5,              /*!< Group number for FLEXIO SPI status codes. */
    kStatusGroup_DSPI = 6,                    /*!< Group number for DSPI status codes. */
    kStatusGroup_FLEXIO_UART = 7,             /*!< Group number for FLEXIO UART status codes. */
    kStatusGroup_FLEXIO_I2C = 8,              /*!< Group number for FLEXIO I2C status codes. */
    kStatusGroup_LPI2C = 9,                   /*!< Group number for LPI2C status codes. */
    kStatusGroup_UART = 10,                   /*!< Group number for UART status codes. */
    kStatusGroup_I2C = 11,                    /*!< Group number for UART status codes. */
    kStatusGroup_LPSCI = 12,                  /*!< Group number for LPSCI status codes. */
    kStatusGroup_LPUART = 13,                 /*!< Group number for LPUART status codes. */
    kStatusGroup_SPI = 14,                    /*!< Group number for SPI status code.*/
    kStatusGroup_XRDC = 15,                   /*!< Group number for XRDC status code.*/
    kStatusGroup_SEMA42 = 16,                 /*!< Group number for SEMA42 status code.*/
    kStatusGroup_SDHC = 17,                   /*!< Group number for SDHC status code */
    kStatusGroup_SDMMC = 18,                  /*!< Group number for SDMMC status code */
    kStatusGroup_SAI = 19,                    /*!< Group number for SAI status code */
    kStatusGroup_MCG = 20,                    /*!< Group number for MCG status codes. */
    kStatusGroup_SCG = 21,                    /*!< Group number for SCG status codes. */
    kStatusGroup_SDSPI = 22,                  /*!< Group number for SDSPI status codes. */
    kStatusGroup_FLEXIO_I2S = 23,             /*!< Group number for FLEXIO I2S status codes */
    kStatusGroup_FLEXIO_MCULCD = 24,          /*!< Group number for FLEXIO LCD status codes */
    kStatusGroup_FLASHIAP = 25,               /*!< Group number for FLASHIAP status codes */
    kStatusGroup_FLEXCOMM_I2C = 26,           /*!< Group number for FLEXCOMM I2C status codes */
    kStatusGroup_I2S = 27,                    /*!< Group number for I2S status codes */
    kStatusGroup_IUART = 28,                  /*!< Group number for IUART status codes */
    kStatusGroup_CSI = 29,                    /*!< Group number for CSI status codes */
    kStatusGroup_MIPI_DSI = 30,               /*!< Group number for MIPI DSI status codes */
    kStatusGroup_SDRAMC = 35,                 /*!< Group number for SDRAMC status codes. */
    kStatusGroup_POWER = 39,                  /*!< Group number for POWER status codes. */
    kStatusGroup_ENET = 40,                   /*!< Group number for ENET status codes. */
    kStatusGroup_PHY = 41,                    /*!< Group number for PHY status codes. */
    kStatusGroup_TRGMUX = 42,                 /*!< Group number for TRGMUX status codes. */
    kStatusGroup_SMARTCARD = 43,              /*!< Group number for SMARTCARD status codes. */
    kStatusGroup_LMEM = 44,                   /*!< Group number for LMEM status codes. */
    kStatusGroup_QSPI = 45,                   /*!< Group number for QSPI status codes. */
    kStatusGroup_DMA = 50,                    /*!< Group number for DMA status codes. */
    kStatusGroup_EDMA = 51,                   /*!< Group number for EDMA status codes. */
    kStatusGroup_DMAMGR = 52,                 /*!< Group number for DMAMGR status codes. */
    kStatusGroup_FLEXCAN = 53,                /*!< Group number for FlexCAN status codes. */
    kStatusGroup_LTC = 54,                    /*!< Group number for LTC status codes. */
    kStatusGroup_FLEXIO_CAMERA = 55,          /*!< Group number for FLEXIO CAMERA status codes. */
    kStatusGroup_LPC_SPI = 56,                /*!< Group number for LPC_SPI status codes. */
    kStatusGroup_LPC_USART = 57,              /*!< Group number for LPC_USART status codes. */
    kStatusGroup_DMIC = 58,                   /*!< Group number for DMIC status codes. */
    kStatusGroup_SDIF = 59,                   /*!< Group number for SDIF status codes.*/
    kStatusGroup_SPIFI = 60,                  /*!< Group number for SPIFI status codes. */
    kStatusGroup_OTP = 61,                    /*!< Group number for OTP status codes. */
    kStatusGroup_MCAN = 62,                   /*!< Group number for MCAN status codes. */
    kStatusGroup_CAAM = 63,                   /*!< Group number for CAAM status codes. */
    kStatusGroup_ECSPI = 64,                  /*!< Group number for ECSPI status codes. */
    kStatusGroup_USDHC = 65,                  /*!< Group number for USDHC status codes.*/
    kStatusGroup_LPC_I2C = 66,                /*!< Group number for LPC_I2C status codes.*/
    kStatusGroup_DCP = 67,                    /*!< Group number for DCP status codes.*/
    kStatusGroup_MSCAN = 68,                  /*!< Group number for MSCAN status codes.*/
    kStatusGroup_ESAI = 69,                   /*!< Group number for ESAI status codes. */
    kStatusGroup_FLEXSPI = 70,                /*!< Group number for FLEXSPI status codes. */
    kStatusGroup_MMDC = 71,                   /*!< Group number for MMDC status codes. */
    kStatusGroup_PDM = 72,                    /*!< Group number for MIC status codes. */
    kStatusGroup_SDMA = 73,                   /*!< Group number for SDMA status codes. */
    kStatusGroup_ICS = 74,                    /*!< Group number for ICS status codes. */
    kStatusGroup_SPDIF = 75,                  /*!< Group number for SPDIF status codes. */
    kStatusGroup_LPC_MINISPI = 76,            /*!< Group number for LPC_MINISPI status codes. */
    kStatusGroup_HASHCRYPT = 77,              /*!< Group number for Hashcrypt status codes */
    kStatusGroup_LPC_SPI_SSP = 78,            /*!< Group number for LPC_SPI_SSP status codes. */
    kStatusGroup_I3C = 79,                    /*!< Group number for I3C status codes */
    kStatusGroup_LPC_I2C_1 = 97,              /*!< Group number for LPC_I2C_1 status codes. */
    kStatusGroup_NOTIFIER = 98,               /*!< Group number for NOTIFIER status codes. */
    kStatusGroup_DebugConsole = 99,           /*!< Group number for debug console status codes. */
    kStatusGroup_SEMC = 100,                  /*!< Group number for SEMC status codes. */
    kStatusGroup_ApplicationRangeStart = 101, /*!< Starting number for application groups. */
    kStatusGroup_IAP = 102,                   /*!< Group number for IAP status codes */

    kStatusGroup_HAL_GPIO = 121,              /*!< Group number for HAL GPIO status codes. */
    kStatusGroup_HAL_UART = 122,              /*!< Group number for HAL UART status codes. */
    kStatusGroup_HAL_TIMER = 123,             /*!< Group number for HAL TIMER status codes. */
    kStatusGroup_HAL_SPI = 124,               /*!< Group number for HAL SPI status codes. */
    kStatusGroup_HAL_I2C = 125,               /*!< Group number for HAL I2C status codes. */
    kStatusGroup_HAL_FLASH = 126,             /*!< Group number for HAL FLASH status codes. */
    kStatusGroup_HAL_PWM = 127,               /*!< Group number for HAL PWM status codes. */
    kStatusGroup_HAL_RNG = 128,               /*!< Group number for HAL RNG status codes. */
    kStatusGroup_TIMERMANAGER = 135,          /*!< Group number for TiMER MANAGER status codes. */
    kStatusGroup_SERIALMANAGER = 136,         /*!< Group number for SERIAL MANAGER status codes. */
    kStatusGroup_LED = 137,                   /*!< Group number for LED status codes. */
    kStatusGroup_BUTTON = 138,                /*!< Group number for BUTTON status codes. */
    kStatusGroup_EXTERN_EEPROM = 139,         /*!< Group number for EXTERN EEPROM status codes. */
    kStatusGroup_SHELL = 140,                 /*!< Group number for SHELL status codes. */
    kStatusGroup_MEM_MANAGER = 141,           /*!< Group number for MEM MANAGER status codes. */
    kStatusGroup_LIST = 142,                  /*!< Group number for List status codes. */
    kStatusGroup_OSA = 143,                   /*!< Group number for OSA status codes. */
    kStatusGroup_COMMON_TASK = 144,           /*!< Group number for Common task status codes. */
    kStatusGroup_MSG = 145,                   /*!< Group number for messaging status codes. */
    kStatusGroup_SDK_OCOTP = 146,             /*!< Group number for OCOTP status codes. */
    kStatusGroup_SDK_FLEXSPINOR = 147,        /*!< Group number for FLEXSPINOR status codes.*/
    kStatusGroup_CODEC = 148,                 /*!< Group number for codec status codes. */
};

/*! @brief Generic status return codes. */
enum _generic_status
{
    kStatus_Success = MAKE_STATUS(kStatusGroup_Generic, 0),
    kStatus_Fail = MAKE_STATUS(kStatusGroup_Generic, 1),
    kStatus_ReadOnly = MAKE_STATUS(kStatusGroup_Generic, 2),
    kStatus_OutOfRange = MAKE_STATUS(kStatusGroup_Generic, 3),
    kStatus_InvalidArgument = MAKE_STATUS(kStatusGroup_Generic, 4),
    kStatus_Timeout = MAKE_STATUS(kStatusGroup_Generic, 5),
    kStatus_NoTransferInProgress = MAKE_STATUS(kStatusGroup_Generic, 6),
};

/*! @brief Type used for all status and error return values. */
typedef int32_t status_t;

/*
 * Macro guard for whether to use default weak IRQ implementation in drivers
 */
#ifndef FSL_DRIVER_TRANSFER_DOUBLE_WEAK_IRQ
#define FSL_DRIVER_TRANSFER_DOUBLE_WEAK_IRQ 1
#endif

/*! @name Min/max macros */
/* @{ */
#if !defined(MIN)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#if !defined(MAX)
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
/* @} */

/*! @brief Computes the number of elements in an array. */
#if !defined(ARRAY_SIZE)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

/*! @name UINT16_MAX/UINT32_MAX value */
/* @{ */
#if !defined(UINT16_MAX)
#define UINT16_MAX ((uint16_t)-1)
#endif

#if !defined(UINT32_MAX)
#define UINT32_MAX ((uint32_t)-1)
#endif
/* @} */

/*! @name Timer utilities */
/* @{ */
/*! Macro to convert a microsecond period to raw count value */
#define USEC_TO_COUNT(us, clockFreqInHz) (uint64_t)((uint64_t)us * clockFreqInHz / 1000000U)
/*! Macro to convert a raw count value to microsecond */
#define COUNT_TO_USEC(count, clockFreqInHz) (uint64_t)((uint64_t)count * 1000000U / clockFreqInHz)

/*! Macro to convert a millisecond period to raw count value */
#define MSEC_TO_COUNT(ms, clockFreqInHz) (uint64_t)((uint64_t)ms * clockFreqInHz / 1000U)
/*! Macro to convert a raw count value to millisecond */
#define COUNT_TO_MSEC(count, clockFreqInHz) (uint64_t)((uint64_t)count * 1000U / clockFreqInHz)
/* @} */

/*! @name Alignment variable definition macros */
/* @{ */
#if (defined(__ICCARM__))
/**
 * Workaround to disable MISRA C message suppress warnings for IAR compiler.
 * http://supp.iar.com/Support/?note=24725
 */
_Pragma("diag_suppress=Pm120")
#define SDK_PRAGMA(x) _Pragma(#x)
    _Pragma("diag_error=Pm120")
/*! Macro to define a variable with alignbytes alignment */
#define SDK_ALIGN(var, alignbytes) SDK_PRAGMA(data_alignment = alignbytes) var
/*! Macro to define a variable with L1 d-cache line size alignment */
#if defined(FSL_FEATURE_L1DCACHE_LINESIZE_BYTE)
#define SDK_L1DCACHE_ALIGN(var) SDK_PRAGMA(data_alignment = FSL_FEATURE_L1DCACHE_LINESIZE_BYTE) var
#endif
/*! Macro to define a variable with L2 cache line size alignment */
#if defined(FSL_FEATURE_L2CACHE_LINESIZE_BYTE)
#define SDK_L2CACHE_ALIGN(var) SDK_PRAGMA(data_alignment = FSL_FEATURE_L2CACHE_LINESIZE_BYTE) var
#endif
#elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
/*! Macro to define a variable with alignbytes alignment */
#define SDK_ALIGN(var, alignbytes) __attribute__((aligned(alignbytes))) var
/*! Macro to define a variable with L1 d-cache line size alignment */
#if defined(FSL_FEATURE_L1DCACHE_LINESIZE_BYTE)
#define SDK_L1DCACHE_ALIGN(var) __attribute__((aligned(FSL_FEATURE_L1DCACHE_LINESIZE_BYTE))) var
#endif
/*! Macro to define a variable with L2 cache line size alignment */
#if defined(FSL_FEATURE_L2CACHE_LINESIZE_BYTE)
#define SDK_L2CACHE_ALIGN(var) __attribute__((aligned(FSL_FEATURE_L2CACHE_LINESIZE_BYTE))) var
#endif
#elif defined(__GNUC__)
/*! Macro to define a variable with alignbytes alignment */
#define SDK_ALIGN(var, alignbytes) var __attribute__((aligned(alignbytes)))
/*! Macro to define a variable with L1 d-cache line size alignment */
#if defined(FSL_FEATURE_L1DCACHE_LINESIZE_BYTE)
#define SDK_L1DCACHE_ALIGN(var) var __attribute__((aligned(FSL_FEATURE_L1DCACHE_LINESIZE_BYTE)))
#endif
/*! Macro to define a variable with L2 cache line size alignment */
#if defined(FSL_FEATURE_L2CACHE_LINESIZE_BYTE)
#define SDK_L2CACHE_ALIGN(var) var __attribute__((aligned(FSL_FEATURE_L2CACHE_LINESIZE_BYTE)))
#endif
#else
#error Toolchain not supported
#define SDK_ALIGN(var, alignbytes) var
#if defined(FSL_FEATURE_L1DCACHE_LINESIZE_BYTE)
#define SDK_L1DCACHE_ALIGN(var) var
#endif
#if defined(FSL_FEATURE_L2CACHE_LINESIZE_BYTE)
#define SDK_L2CACHE_ALIGN(var) var
#endif
#endif

/*! Macro to change a value to a given size aligned value */
#define SDK_SIZEALIGN(var, alignbytes) \
    ((unsigned int)((var) + ((alignbytes)-1)) & (unsigned int)(~(unsigned int)((alignbytes)-1)))
/* @} */

/*! @name Non-cacheable region definition macros */
/* For initialized non-zero non-cacheable variables, please using "AT_NONCACHEABLE_SECTION_INIT(var) ={xx};" or
 * "AT_NONCACHEABLE_SECTION_ALIGN_INIT(var) ={xx};" in your projects to define them, for zero-inited non-cacheable variables,
 * please using "AT_NONCACHEABLE_SECTION(var);" or "AT_NONCACHEABLE_SECTION_ALIGN(var);" to define them, these zero-inited variables
 * will be initialized to zero in system startup.
 */
/* @{ */
#if (defined(__ICCARM__))
#if ((!(defined(FSL_FEATURE_HAS_NO_NONCACHEABLE_SECTION) && FSL_FEATURE_HAS_NO_NONCACHEABLE_SECTION)) && defined(FSL_FEATURE_L1ICACHE_LINESIZE_BYTE))
#define AT_NONCACHEABLE_SECTION(var) var @"NonCacheable"
#define AT_NONCACHEABLE_SECTION_ALIGN(var, alignbytes) SDK_PRAGMA(data_alignment = alignbytes) var @"NonCacheable"
#define AT_NONCACHEABLE_SECTION_INIT(var) var @"NonCacheable.init"
#define AT_NONCACHEABLE_SECTION_ALIGN_INIT(var, alignbytes) SDK_PRAGMA(data_alignment = alignbytes) var @"NonCacheable.init"
#else
#define AT_NONCACHEABLE_SECTION(var) var
#define AT_NONCACHEABLE_SECTION_ALIGN(var, alignbytes) SDK_PRAGMA(data_alignment = alignbytes) var
#define AT_NONCACHEABLE_SECTION_INIT(var) var
#define AT_NONCACHEABLE_SECTION_ALIGN_INIT(var, alignbytes) SDK_PRAGMA(data_alignment = alignbytes) var
#endif
#elif(defined(__CC_ARM) || defined(__ARMCC_VERSION))
#if ((!(defined(FSL_FEATURE_HAS_NO_NONCACHEABLE_SECTION) && FSL_FEATURE_HAS_NO_NONCACHEABLE_SECTION)) && defined(FSL_FEATURE_L1ICACHE_LINESIZE_BYTE))
#define AT_NONCACHEABLE_SECTION(var) __attribute__((section("NonCacheable"), zero_init)) var
#define AT_NONCACHEABLE_SECTION_ALIGN(var, alignbytes) \
    __attribute__((section("NonCacheable"), zero_init)) __attribute__((aligned(alignbytes))) var
#define AT_NONCACHEABLE_SECTION_INIT(var) __attribute__((section("NonCacheable.init"))) var
#define AT_NONCACHEABLE_SECTION_ALIGN_INIT(var, alignbytes) \
    __attribute__((section("NonCacheable.init"))) __attribute__((aligned(alignbytes))) var
#else
#define AT_NONCACHEABLE_SECTION(var) var
#define AT_NONCACHEABLE_SECTION_ALIGN(var, alignbytes) __attribute__((aligned(alignbytes))) var
#define AT_NONCACHEABLE_SECTION_INIT(var) var
#define AT_NONCACHEABLE_SECTION_ALIGN_INIT(var, alignbytes) __attribute__((aligned(alignbytes))) var
#endif
#elif(defined(__GNUC__))
/* For GCC, when the non-cacheable section is required, please define "__STARTUP_INITIALIZE_NONCACHEDATA"
 * in your projects to make sure the non-cacheable section variables will be initialized in system startup.
 */
#if ((!(defined(FSL_FEATURE_HAS_NO_NONCACHEABLE_SECTION) && FSL_FEATURE_HAS_NO_NONCACHEABLE_SECTION)) && defined(FSL_FEATURE_L1ICACHE_LINESIZE_BYTE))
#define AT_NONCACHEABLE_SECTION_INIT(var) __attribute__((section("NonCacheable.init"))) var
#define AT_NONCACHEABLE_SECTION_ALIGN_INIT(var, alignbytes) \
    __attribute__((section("NonCacheable.init"))) var __attribute__((aligned(alignbytes)))
#define AT_NONCACHEABLE_SECTION(var) __attribute__((section("NonCacheable,\"aw\",%nobits @"))) var
#define AT_NONCACHEABLE_SECTION_ALIGN(var, alignbytes) \
    __attribute__((section("NonCacheable,\"aw\",%nobits @"))) var __attribute__((aligned(alignbytes)))
#else
#define AT_NONCACHEABLE_SECTION(var) var
#define AT_NONCACHEABLE_SECTION_ALIGN(var, alignbytes) var __attribute__((aligned(alignbytes)))
#define AT_NONCACHEABLE_SECTION_INIT(var) var
#define AT_NONCACHEABLE_SECTION_ALIGN_INIT(var, alignbytes) var __attribute__((aligned(alignbytes)))
#endif
#else
#error Toolchain not supported.
#define AT_NONCACHEABLE_SECTION(var) var
#define AT_NONCACHEABLE_SECTION_ALIGN(var, alignbytes) var
#define AT_NONCACHEABLE_SECTION_INIT(var) var
#define AT_NONCACHEABLE_SECTION_ALIGN_INIT(var, alignbytes) var
#endif
/* @} */

/*! @name Time sensitive region */
/* @{ */
#if defined(FSL_SDK_DRIVER_QUICK_ACCESS_ENABLE) && FSL_SDK_DRIVER_QUICK_ACCESS_ENABLE
#if (defined(__ICCARM__))
#define AT_QUICKACCESS_SECTION_CODE(func) func @"CodeQuickAccess"
#define AT_QUICKACCESS_SECTION_DATA(func) func @"DataQuickAccess"
#elif(defined(__CC_ARM) || defined(__ARMCC_VERSION))
#define AT_QUICKACCESS_SECTION_CODE(func) __attribute__((section("CodeQuickAccess"))) func
#define AT_QUICKACCESS_SECTION_DATA(func) __attribute__((section("DataQuickAccess"))) func
#elif(defined(__GNUC__))
#define AT_QUICKACCESS_SECTION_CODE(func) __attribute__((section("CodeQuickAccess"))) func
#define AT_QUICKACCESS_SECTION_DATA(func) __attribute__((section("DataQuickAccess"))) func
#else
#error Toolchain not supported.
#endif /* defined(__ICCARM__) */
#else
#if (defined(__ICCARM__))
#define AT_QUICKACCESS_SECTION_CODE(func) func
#define AT_QUICKACCESS_SECTION_DATA(func) func
#elif(defined(__CC_ARM) || defined(__ARMCC_VERSION))
#define AT_QUICKACCESS_SECTION_CODE(func) func
#define AT_QUICKACCESS_SECTION_DATA(func) func
#elif(defined(__GNUC__))
#define AT_QUICKACCESS_SECTION_CODE(func) func
#define AT_QUICKACCESS_SECTION_DATA(func) func
#else
#error Toolchain not supported.
#endif
#endif /* __FSL_SDK_DRIVER_QUICK_ACCESS_ENABLE */
/* @} */

/*! @name Ram Function */
#if (defined(__ICCARM__))
#define RAMFUNCTION_SECTION_CODE(func) func @"RamFunction"
#elif(defined(__CC_ARM) || defined(__ARMCC_VERSION))
#define RAMFUNCTION_SECTION_CODE(func) __attribute__((section("RamFunction"))) func
#elif(defined(__GNUC__))
#define RAMFUNCTION_SECTION_CODE(func) __attribute__((section("RamFunction"))) func
#else
#error Toolchain not supported.
#endif /* defined(__ICCARM__) */
/* @} */

/*
 * The fsl_clock.h is included here because it needs MAKE_VERSION/MAKE_STATUS/status_t
 * defined in previous of this file.
 */
#include "fsl_clock.h"

/*
 * Chip level peripheral reset API, for MCUs that implement peripheral reset control external to a peripheral
 */
#if ((defined(FSL_FEATURE_SOC_SYSCON_COUNT) && (FSL_FEATURE_SOC_SYSCON_COUNT > 0)) || \
     (defined(FSL_FEATURE_SOC_ASYNC_SYSCON_COUNT) && (FSL_FEATURE_SOC_ASYNC_SYSCON_COUNT > 0)))
#include "fsl_reset.h"
#endif

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
        extern "C"
{
#endif

    /*!
     * @brief Enable specific interrupt.
     *
     * Enable LEVEL1 interrupt. For some devices, there might be multiple interrupt
     * levels. For example, there are NVIC and intmux. Here the interrupts connected
     * to NVIC are the LEVEL1 interrupts, because they are routed to the core directly.
     * The interrupts connected to intmux are the LEVEL2 interrupts, they are routed
     * to NVIC first then routed to core.
     *
     * This function only enables the LEVEL1 interrupts. The number of LEVEL1 interrupts
     * is indicated by the feature macro FSL_FEATURE_NUMBER_OF_LEVEL1_INT_VECTORS.
     *
     * @param interrupt The IRQ number.
     * @retval kStatus_Success Interrupt enabled successfully
     * @retval kStatus_Fail Failed to enable the interrupt
     */
    static inline status_t EnableIRQ(IRQn_Type interrupt)
    {
        if (NotAvail_IRQn == interrupt)
        {
            return kStatus_Fail;
        }

#if defined(FSL_FEATURE_NUMBER_OF_LEVEL1_INT_VECTORS) && (FSL_FEATURE_NUMBER_OF_LEVEL1_INT_VECTORS > 0)
        if (interrupt >= FSL_FEATURE_NUMBER_OF_LEVEL1_INT_VECTORS)
        {
            return kStatus_Fail;
        }
#endif

#if defined(__GIC_PRIO_BITS)
        GIC_EnableIRQ(interrupt);
#else
        NVIC_EnableIRQ(interrupt);
#endif
        return kStatus_Success;
    }

    /*!
     * @brief Disable specific interrupt.
     *
     * Disable LEVEL1 interrupt. For some devices, there might be multiple interrupt
     * levels. For example, there are NVIC and intmux. Here the interrupts connected
     * to NVIC are the LEVEL1 interrupts, because they are routed to the core directly.
     * The interrupts connected to intmux are the LEVEL2 interrupts, they are routed
     * to NVIC first then routed to core.
     *
     * This function only disables the LEVEL1 interrupts. The number of LEVEL1 interrupts
     * is indicated by the feature macro FSL_FEATURE_NUMBER_OF_LEVEL1_INT_VECTORS.
     *
     * @param interrupt The IRQ number.
     * @retval kStatus_Success Interrupt disabled successfully
     * @retval kStatus_Fail Failed to disable the interrupt
     */
    static inline status_t DisableIRQ(IRQn_Type interrupt)
    {
        if (NotAvail_IRQn == interrupt)
        {
            return kStatus_Fail;
        }

#if defined(FSL_FEATURE_NUMBER_OF_LEVEL1_INT_VECTORS) && (FSL_FEATURE_NUMBER_OF_LEVEL1_INT_VECTORS > 0)
        if (interrupt >= FSL_FEATURE_NUMBER_OF_LEVEL1_INT_VECTORS)
        {
            return kStatus_Fail;
        }
#endif

#if defined(__GIC_PRIO_BITS)
        GIC_DisableIRQ(interrupt);
#else
    NVIC_DisableIRQ(interrupt);
#endif
        return kStatus_Success;
    }

    /*!
     * @brief Disable the global IRQ
     *
     * Disable the global interrupt and return the current primask register. User is required to provided the primask
     * register for the EnableGlobalIRQ().
     *
     * @return Current primask value.
     */
    static inline uint32_t DisableGlobalIRQ(void)
    {
#if defined (__XCC__)
        return 0;
#else
#if defined(CPSR_I_Msk)
        uint32_t cpsr = __get_CPSR() & CPSR_I_Msk;

        __disable_irq();

        return cpsr;
#else
    uint32_t regPrimask = __get_PRIMASK();

    __disable_irq();

    return regPrimask;
#endif
#endif
    }

    /*!
     * @brief Enable the global IRQ
     *
     * Set the primask register with the provided primask value but not just enable the primask. The idea is for the
     * convenience of integration of RTOS. some RTOS get its own management mechanism of primask. User is required to
     * use the EnableGlobalIRQ() and DisableGlobalIRQ() in pair.
     *
     * @param primask value of primask register to be restored. The primask value is supposed to be provided by the
     * DisableGlobalIRQ().
     */
    static inline void EnableGlobalIRQ(uint32_t primask)
    {
#if defined (__XCC__)
#else
#if defined(CPSR_I_Msk)
        __set_CPSR((__get_CPSR() & ~CPSR_I_Msk) | primask);
#else
    __set_PRIMASK(primask);
#endif
#endif
    }

#if defined(ENABLE_RAM_VECTOR_TABLE)
    /*!
     * @brief install IRQ handler
     *
     * @param irq IRQ number
     * @param irqHandler IRQ handler address
     * @return The old IRQ handler address
     */
    uint32_t InstallIRQHandler(IRQn_Type irq, uint32_t irqHandler);
#endif /* ENABLE_RAM_VECTOR_TABLE. */

#if (defined(FSL_FEATURE_SOC_SYSCON_COUNT) && (FSL_FEATURE_SOC_SYSCON_COUNT > 0))
    /*!
     * @brief Enable specific interrupt for wake-up from deep-sleep mode.
     *
     * Enable the interrupt for wake-up from deep sleep mode.
     * Some interrupts are typically used in sleep mode only and will not occur during
     * deep-sleep mode because relevant clocks are stopped. However, it is possible to enable
     * those clocks (significantly increasing power consumption in the reduced power mode),
     * making these wake-ups possible.
     *
     * @note This function also enables the interrupt in the NVIC (EnableIRQ() is called internaly).
     *
     * @param interrupt The IRQ number.
     */
    void EnableDeepSleepIRQ(IRQn_Type interrupt);

    /*!
     * @brief Disable specific interrupt for wake-up from deep-sleep mode.
     *
     * Disable the interrupt for wake-up from deep sleep mode.
     * Some interrupts are typically used in sleep mode only and will not occur during
     * deep-sleep mode because relevant clocks are stopped. However, it is possible to enable
     * those clocks (significantly increasing power consumption in the reduced power mode),
     * making these wake-ups possible.
     *
     * @note This function also disables the interrupt in the NVIC (DisableIRQ() is called internaly).
     *
     * @param interrupt The IRQ number.
     */
    void DisableDeepSleepIRQ(IRQn_Type interrupt);
#endif /* FSL_FEATURE_SOC_SYSCON_COUNT */

    /*!
     * @brief Allocate memory with given alignment and aligned size.
     *
     * This is provided to support the dynamically allocated memory
     * used in cache-able region.
     * @param size The length required to malloc.
     * @param alignbytes The alignment size.
     * @retval The allocated memory.
     */
    void *SDK_Malloc(size_t size, size_t alignbytes);

    /*!
     * @brief Free memory.
     *
     * @param ptr The memory to be release.
     */
    void SDK_Free(void *ptr);

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* _FSL_COMMON_H_ */

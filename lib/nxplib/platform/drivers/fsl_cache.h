/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_CACHE_H_
#define _FSL_CACHE_H_

#include "fsl_common.h"

/*!
 * @addtogroup cache
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief cache driver version 2.0.1. */
#define FSL_CACHE_DRIVER_VERSION (MAKE_VERSION(2, 0, 1))
/*@}*/

#if defined(FSL_FEATURE_SOC_L2CACHEC_COUNT) && FSL_FEATURE_SOC_L2CACHEC_COUNT
#ifndef FSL_SDK_DISBLE_L2CACHE_PRESENT
#define FSL_SDK_DISBLE_L2CACHE_PRESENT 0
#endif
#endif /* FSL_FEATURE_SOC_L2CACHEC_COUNT */
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if defined(FSL_FEATURE_SOC_L2CACHEC_COUNT) && FSL_FEATURE_SOC_L2CACHEC_COUNT

/*! @brief Number of level 2 cache controller ways. */
typedef enum _l2cache_way_num
{
    kL2CACHE_8ways = 0, /*!< 8 ways. */
#if defined(FSL_FEATURE_L2CACHE_SUPPORT_16_WAY_ASSOCIATIVITY) && FSL_FEATURE_L2CACHE_SUPPORT_16_WAY_ASSOCIATIVITY
    kL2CACHE_16ways /*!< 16 ways. */
#endif              /* FSL_FEATURE_L2CACHE_SUPPORT_16_WAY_ASSOCIATIVITY */
} l2cache_way_num_t;

/*! @brief Level 2 cache controller way size. */
typedef enum _l2cache_way_size
{
    kL2CACHE_16KBSize  = 1, /*!< 16 KB way size. */
    kL2CACHE_32KBSize  = 2, /*!< 32 KB way size. */
    kL2CACHE_64KBSize  = 3, /*!< 64 KB way size. */
    kL2CACHE_128KBSize = 4, /*!< 128 KB way size. */
    kL2CACHE_256KBSize = 5, /*!< 256 KB way size. */
    kL2CACHE_512KBSize = 6  /*!< 512 KB way size. */
} l2cache_way_size;

/*! @brief Level 2 cache controller replacement policy. */
typedef enum _l2cache_replacement
{
    kL2CACHE_Pseudorandom = 0U, /*!< Peseudo-random replacement policy using an lfsr. */
    kL2CACHE_Roundrobin         /*!< Round-robin replacemnt policy. */
} l2cache_replacement_t;

/*! @brief Level 2 cache controller force write allocate options. */
typedef enum _l2cache_writealloc
{
    kL2CACHE_UseAwcache = 0,    /*!< Use AWCAHE attribute for the write allocate. */
    kL2CACHE_NoWriteallocate,   /*!< Force no write allocate. */
    kL2CACHE_forceWriteallocate /*!< Force write allocate when write misses. */
} l2cache_writealloc_t;

/*! @brief Level 2 cache controller tag/data ram latency. */
typedef enum _l2cache_latency
{
    kL2CACHE_1CycleLate = 0, /*!< 1 cycle of latency. */
    kL2CACHE_2CycleLate,     /*!< 2 cycle of latency. */
    kL2CACHE_3CycleLate,     /*!< 3 cycle of latency. */
    kL2CACHE_4CycleLate,     /*!< 4 cycle of latency. */
    kL2CACHE_5CycleLate,     /*!< 5 cycle of latency. */
    kL2CACHE_6CycleLate,     /*!< 6 cycle of latency. */
    kL2CACHE_7CycleLate,     /*!< 7 cycle of latency. */
    kL2CACHE_8CycleLate      /*!< 8 cycle of latency. */
} l2cache_latency_t;

/*! @brief Level 2 cache controller tag/data ram latency configure structure. */
typedef struct _l2cache_latency_config
{
    l2cache_latency_t tagWriteLate;  /*!< Tag write latency. */
    l2cache_latency_t tagReadLate;   /*!< Tag Read latency. */
    l2cache_latency_t tagSetupLate;  /*!< Tag setup latency. */
    l2cache_latency_t dataWriteLate; /*!< Data write latency. */
    l2cache_latency_t dataReadLate;  /*!< Data Read latency. */
    l2cache_latency_t dataSetupLate; /*!< Data setup latency. */
} L2cache_latency_config_t;

/*! @brief Level 2 cache controller configure structure. */
typedef struct _l2cache_config
{
    /* ------------------------ l2 cachec basic settings ---------------------------- */
    l2cache_way_num_t wayNum;           /*!< The number of ways. */
    l2cache_way_size waySize;           /*!< The way size = Cache Ram size / wayNum. */
    l2cache_replacement_t repacePolicy; /*!< Replacemnet policy. */
    /* ------------------------ tag/data ram latency settings ----------------------- */
    L2cache_latency_config_t *lateConfig; /*!< Tag/data latency configure. Set NUll if not required. */
    /* ------------------------ Prefetch enable settings ---------------------------- */
    bool istrPrefetchEnable; /*!< Instruction prefetch enable. */
    bool dataPrefetchEnable; /*!< Data prefetch enable. */
    /* ------------------------ Non-secure access settings -------------------------- */
    bool nsLockdownEnable; /*!< None-secure lockdown enable. */
    /* ------------------------ other settings -------------------------------------- */
    l2cache_writealloc_t writeAlloc; /*!< Write allcoate force option. */
} l2cache_config_t;
#endif /* FSL_FEATURE_SOC_L2CACHEC_COUNT */
/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Control for cortex-m7 L1 cache
 *@{
 */

/*!
 * @brief Enables cortex-m7 L1 instruction cache.
 *
 */
static inline void L1CACHE_EnableICache(void)
{
    SCB_EnableICache();
}

/*!
 * @brief Disables cortex-m7 L1 instruction cache.
 *
 */
static inline void L1CACHE_DisableICache(void)
{
    SCB_DisableICache();
}

/*!
 * @brief Invalidate cortex-m7 L1 instruction cache.
 *
 */
static inline void L1CACHE_InvalidateICache(void)
{
    SCB_InvalidateICache();
}

/*!
 * @brief Invalidate cortex-m7 L1 instruction cache by range.
 *
 * @param address  The start address of the memory to be invalidated.
 * @param size_byte  The memory size.
 * @note The start address and size_byte should be 32-byte(FSL_FEATURE_L1ICACHE_LINESIZE_BYTE) aligned.
 * The startAddr here will be forced to align to L1 I-cache line size if
 * startAddr is not aligned. For the size_byte, application should make sure the
 * alignment or make sure the right operation order if the size_byte is not aligned.
 */
void L1CACHE_InvalidateICacheByRange(uint32_t address, uint32_t size_byte);

/*!
 * @brief Enables cortex-m7 L1 data cache.
 *
 */
static inline void L1CACHE_EnableDCache(void)
{
    SCB_EnableDCache();
}

/*!
 * @brief Disables cortex-m7 L1 data cache.
 *
 */
static inline void L1CACHE_DisableDCache(void)
{
    SCB_DisableDCache();
}

/*!
 * @brief Invalidates cortex-m7 L1 data cache.
 *
 */
static inline void L1CACHE_InvalidateDCache(void)
{
    SCB_InvalidateDCache();
}

/*!
 * @brief Cleans cortex-m7 L1 data cache.
 *
 */
static inline void L1CACHE_CleanDCache(void)
{
    SCB_CleanDCache();
}

/*!
 * @brief Cleans and Invalidates cortex-m7 L1 data cache.
 *
 */
static inline void L1CACHE_CleanInvalidateDCache(void)
{
    SCB_CleanInvalidateDCache();
}

/*!
 * @brief Invalidates cortex-m7 L1 data cache by range.
 *
 * @param address  The start address of the memory to be invalidated.
 * @param size_byte  The memory size.
 * @note The start address and size_byte should be 32-byte(FSL_FEATURE_L1DCACHE_LINESIZE_BYTE) aligned.
 * The startAddr here will be forced to align to L1 D-cache line size if
 * startAddr is not aligned. For the size_byte, application should make sure the
 * alignment or make sure the right operation order if the size_byte is not aligned.
 */
static inline void L1CACHE_InvalidateDCacheByRange(uint32_t address, uint32_t size_byte)
{
    uint32_t startAddr = address & (uint32_t) ~(FSL_FEATURE_L1DCACHE_LINESIZE_BYTE - 1);
    uint32_t size      = size_byte + address - startAddr;

    SCB_InvalidateDCache_by_Addr((uint32_t *)startAddr, size);
}

/*!
 * @brief Cleans cortex-m7 L1 data cache by range.
 *
 * @param address  The start address of the memory to be cleaned.
 * @param size_byte  The memory size.
 * @note The start address and size_byte should be 32-byte(FSL_FEATURE_L1DCACHE_LINESIZE_BYTE) aligned.
 * The startAddr here will be forced to align to L1 D-cache line size if
 * startAddr is not aligned. For the size_byte, application should make sure the
 * alignment or make sure the right operation order if the size_byte is not aligned.
 */
static inline void L1CACHE_CleanDCacheByRange(uint32_t address, uint32_t size_byte)
{
    uint32_t startAddr = address & (uint32_t) ~(FSL_FEATURE_L1DCACHE_LINESIZE_BYTE - 1);
    uint32_t size      = size_byte + address - startAddr;

    SCB_CleanDCache_by_Addr((uint32_t *)startAddr, size);
}

/*!
 * @brief Cleans and Invalidates cortex-m7 L1 data cache by range.
 *
 * @param address  The start address of the memory to be clean and invalidated.
 * @param size_byte  The memory size.
 * @note The start address and size_byte should be 32-byte(FSL_FEATURE_L1DCACHE_LINESIZE_BYTE) aligned.
 * The startAddr here will be forced to align to L1 D-cache line size if
 * startAddr is not aligned. For the size_byte, application should make sure the
 * alignment or make sure the right operation order if the size_byte is not aligned.
 */
static inline void L1CACHE_CleanInvalidateDCacheByRange(uint32_t address, uint32_t size_byte)
{
    uint32_t startAddr = address & (uint32_t) ~(FSL_FEATURE_L1DCACHE_LINESIZE_BYTE - 1);
    uint32_t size      = size_byte + address - startAddr;

    SCB_CleanInvalidateDCache_by_Addr((uint32_t *)startAddr, size);
}
/*@}*/

#if defined(FSL_FEATURE_SOC_L2CACHEC_COUNT) && FSL_FEATURE_SOC_L2CACHEC_COUNT
/*!
 * @name Control for L2 pl310 cache
 *@{
 */

/*!
 * @brief Initializes the level 2 cache controller module.
 *
 * @param config Pointer to configuration structure. See "l2cache_config_t".
 */
void L2CACHE_Init(l2cache_config_t *config);

/*!
 * @brief Gets an available default settings for the cache controller.
 *
 * This function initializes the cache controller configuration structure with default settings.
 * The default values are:
 * @code
 *   config->waysNum = kL2CACHE_8ways;
 *   config->waySize = kL2CACHE_32KbSize;
 *   config->repacePolicy = kL2CACHE_Roundrobin;
 *   config->lateConfig = NULL;
 *   config->istrPrefetchEnable = false;
 *   config->dataPrefetchEnable = false;
 *   config->nsLockdownEnable = false;
 *   config->writeAlloc = kL2CACHE_UseAwcache;
 * @endcode
 * @param config Pointer to the configuration structure.
 */
void L2CACHE_GetDefaultConfig(l2cache_config_t *config);

/*!
 * @brief Enables the level 2 cache controller.
 * This function enables the cache controller. Must be written using a secure access.
 * If write with a Non-secure access will cause a DECERR response.
 *
 */
void L2CACHE_Enable(void);

/*!
 * @brief Disables the level 2 cache controller.
 * This function disables the cache controller. Must be written using a secure access.
 * If write with a Non-secure access will cause a DECERR response.
 *
 */
void L2CACHE_Disable(void);

/*!
 * @brief Invalidates the Level 2 cache.
 * This function invalidates all entries in cache.
 *
 */
void L2CACHE_Invalidate(void);

/*!
 * @brief Invalidates the Level 2 cache lines in the range of two physical addresses.
 * This function invalidates all cache lines between two physical addresses.
 *
 * @param address  The start address of the memory to be invalidated.
 * @param size_byte  The memory size.
 * @note The start address and size_byte should be 32-byte(FSL_FEATURE_L2CACHE_LINESIZE_BYTE) aligned.
 * The startAddr here will be forced to align to L2 line size if startAddr
 * is not aligned. For the size_byte, application should make sure the
 * alignment or make sure the right operation order if the size_byte is not aligned.
 */
void L2CACHE_InvalidateByRange(uint32_t address, uint32_t size_byte);

/*!
 * @brief Cleans the level 2 cache controller.
 * This function cleans all entries in the level 2 cache controller.
 *
 */
void L2CACHE_Clean(void);

/*!
 * @brief Cleans the Level 2 cache lines in the range of two physical addresses.
 * This function cleans all cache lines between two physical addresses.
 *
 * @param address  The start address of the memory to be cleaned.
 * @param size_byte  The memory size.
 * @note The start address and size_byte should be 32-byte(FSL_FEATURE_L2CACHE_LINESIZE_BYTE) aligned.
 * The startAddr here will be forced to align to L2 line size if startAddr
 * is not aligned. For the size_byte, application should make sure the
 * alignment or make sure the right operation order if the size_byte is not aligned.
 */
void L2CACHE_CleanByRange(uint32_t address, uint32_t size_byte);

/*!
 * @brief Cleans and invalidates the level 2 cache controller.
 * This function cleans and invalidates all entries in the level 2 cache controller.
 *
 */
void L2CACHE_CleanInvalidate(void);

/*!
 * @brief Cleans and invalidates the Level 2 cache lines in the range of two physical addresses.
 * This function cleans and invalidates all cache lines between two physical addresses.
 *
 * @param address  The start address of the memory to be cleaned and invalidated.
 * @param size_byte  The memory size.
 * @note The start address and size_byte should be 32-byte(FSL_FEATURE_L2CACHE_LINESIZE_BYTE) aligned.
 * The startAddr here will be forced to align to L2 line size if startAddr
 * is not aligned. For the size_byte, application should make sure the
 * alignment or make sure the right operation order if the size_byte is not aligned.
 */
void L2CACHE_CleanInvalidateByRange(uint32_t address, uint32_t size_byte);

/*!
 * @brief Enables or disables to lock down the data and instruction by way.
 * This function locks down the cached instruction/data by way and prevent the adresses from
 * being allocated and prevent dara from being evicted out of the level 2 cache.
 * But the normal cache maintenance operations that invalidate, clean or clean
 * and validate cache contents affect the locked-down cache lines as normal.
 *
 * @param masterId  The master id, range from 0 ~ 7.
 * @param mask  The ways to be enabled or disabled to lockdown.
 *               each bit in value is related to each way of the cache. for example:
 *               value: bit 0  ------ way 0.
 *               value: bit 1  ------ way 1.
 *               --------------------------
 *               value: bit 15 ------ way 15.
 * Note: please make sure the value setting is align with your supported ways.
 * @param enable  True enable the lockdown, false to disable the lockdown.
 */
void L2CACHE_LockdownByWayEnable(uint32_t masterId, uint32_t mask, bool enable);

/*@}*/
#endif /* FSL_FEATURE_SOC_L2CACHEC_COUNT */

/*!
 * @name Unified Cache Control for all caches (cortex-m7 L1 cache + l2 pl310)
 * Mainly used for many drivers for easy cache operation.
 *@{
 */

/*!
 * @brief Invalidates all instruction caches by range.
 *
 * Both cortex-m7 L1 cache line and L2 PL310 cache line length is 32-byte.
 *
 * @param address The physical address.
 * @param size_byte size of the memory to be invalidated.
 * @note address and size should be aligned to cache line size
 *  32-Byte due to the cache operation unit is one cache line. The startAddr here will be forced
 * to align to the cache line size if startAddr is not aligned. For the size_byte, application should
 * make sure the alignment or make sure the right operation order if the size_byte is not aligned.
 */
void ICACHE_InvalidateByRange(uint32_t address, uint32_t size_byte);

/*!
 * @brief Invalidates all data caches by range.
 *
 * Both cortex-m7 L1 cache line and L2 PL310 cache line length is 32-byte.
 *
 * @param address The physical address.
 * @param size_byte size of the memory to be invalidated.
 * @note address and size should be aligned to cache line size
 *  32-Byte due to the cache operation unit is one cache line. The startAddr here will be forced
 * to align to the cache line size if startAddr is not aligned. For the size_byte, application should
 * make sure the alignment or make sure the right operation order if the size_byte is not aligned.
 */
void DCACHE_InvalidateByRange(uint32_t address, uint32_t size_byte);

/*!
 * @brief Cleans all data caches by range.
 *
 * Both cortex-m7 L1 cache line and L2 PL310 cache line length is 32-byte.
 *
 * @param address The physical address.
 * @param size_byte size of the memory to be cleaned.
 * @note address and size should be aligned to cache line size
 *  32-Byte due to the cache operation unit is one cache line. The startAddr here will be forced
 * to align to the cache line size if startAddr is not aligned. For the size_byte, application should
 * make sure the alignment or make sure the right operation order if the size_byte is not aligned.
 */
void DCACHE_CleanByRange(uint32_t address, uint32_t size_byte);

/*!
 * @brief Cleans and Invalidates all data caches by range.
 *
 * Both cortex-m7 L1 cache line and L2 PL310 cache line length is 32-byte.
 *
 * @param address The physical address.
 * @param size_byte size of the memory to be cleaned and invalidated.
 * @note address and size should be aligned to cache line size
 *  32-Byte due to the cache operation unit is one cache line. The startAddr here will be forced
 * to align to the cache line size if startAddr is not aligned. For the size_byte, application should
 * make sure the alignment or make sure the right operation order if the size_byte is not aligned.
 */
void DCACHE_CleanInvalidateByRange(uint32_t address, uint32_t size_byte);

/*@}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_CACHE_H_*/

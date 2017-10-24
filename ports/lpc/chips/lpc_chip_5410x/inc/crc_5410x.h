/*
 * @brief LPC5410X Cyclic Redundancy Check (CRC) Engine driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __CRC_5410X_H_
#define __CRC_5410X_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup CRC_5410X CHIP: LPC5410X Cyclic Redundancy Check Engine driver
 * @ingroup CHIP_5410X_DRIVERS
 * @{
 */

/**
 * @brief CRC register block structure
 */
typedef struct {					/*!< CRC Structure */
	__IO    uint32_t    MODE;		/*!< CRC Mode Register */
	__IO    uint32_t    SEED;		/*!< CRC SEED Register */
	union {
		__I     uint32_t    SUM;	/*!< CRC Checksum Register. */
		__O     uint32_t    WRDATA32;	/*!< CRC Data Register: write size 32-bit*/
		__O     uint16_t    WRDATA16;	/*!< CRC Data Register: write size 16-bit*/
		__O     uint8_t     WRDATA8;	/*!< CRC Data Register: write size 8-bit*/
	};

} LPC_CRC_T;

/*
 * @brief CRC MODE register description
 */
#define CRC_MODE_POLY_BITMASK   ((0x03))	/** CRC polynomial Bit mask */
#define CRC_MODE_POLY_CCITT     (0x00)		/** Select CRC-CCITT polynomial */
#define CRC_MODE_POLY_CRC16     (0x01)		/** Select CRC-16 polynomial */
#define CRC_MODE_POLY_CRC32     (0x02)		/** Select CRC-32 polynomial */
#define CRC_MODE_WRDATA_BITMASK (0x03 << 2)	/** CRC WR_Data Config Bit mask */
#define CRC_MODE_WRDATA_BIT_RVS (1 << 2)	/** Select Bit order reverse for WR_DATA (per byte) */
#define CRC_MODE_WRDATA_CMPL    (1 << 3)	/** Select One's complement for WR_DATA */
#define CRC_MODE_SUM_BITMASK    (0x03 << 4)	/** CRC Sum Config Bit mask */
#define CRC_MODE_SUM_BIT_RVS    (1 << 4)	/** Select Bit order reverse for CRC_SUM */
#define CRC_MODE_SUM_CMPL       (1 << 5)	/** Select One's complement for CRC_SUM */

#define MODE_CFG_CCITT          (0x00)	/** Pre-defined mode word for default CCITT setup */
#define MODE_CFG_CRC16          (0x15)	/** Pre-defined mode word for default CRC16 setup */
#define MODE_CFG_CRC32          (0x36)	/** Pre-defined mode word for default CRC32 setup */

#define CRC_SEED_CCITT          (0x0000FFFF)/** Initial seed value for CCITT mode */
#define CRC_SEED_CRC16          (0x00000000)/** Initial seed value for CRC16 mode */
#define CRC_SEED_CRC32          (0xFFFFFFFF)/** Initial seed value for CRC32 mode */

/**
 * @brief CRC polynomial
 */
typedef enum IP_CRC_001_POLY {
	CRC_POLY_CCITT = CRC_MODE_POLY_CCITT,	/**< CRC-CCIT polynomial */
	CRC_POLY_CRC16 = CRC_MODE_POLY_CRC16,	/**< CRC-16 polynomial */
	CRC_POLY_CRC32 = CRC_MODE_POLY_CRC32,	/**< CRC-32 polynomial */
	CRC_POLY_LAST,
} CRC_POLY_T;

/**
 * @brief	Initializes the CRC Engine
 * @return	Nothing
 */
void Chip_CRC_Init(void);

/**
 * @brief	Deinitializes the CRC Engine
 * @return	Nothing
 */
void Chip_CRC_Deinit(void);

/**
 * @brief	Set the polynomial used for the CRC calculation
 * @param	poly	: The enumerated polynomial to be used
 * @param	flags	: An Or'ed value of flags that setup the mode
 * @return	Nothing
 * @note	Flags for setting up the mode word include CRC_MODE_WRDATA_BIT_RVS,
 * CRC_MODE_WRDATA_CMPL, CRC_MODE_SUM_BIT_RVS, and CRC_MODE_SUM_CMPL.
 */
__STATIC_INLINE void Chip_CRC_SetPoly(CRC_POLY_T poly, uint32_t flags)
{
	LPC_CRC->MODE = (uint32_t) poly | flags;
}

/**
 * @brief	Sets up the CRC engine for CRC16 mode
 * @return	Nothing
 */
__STATIC_INLINE void Chip_CRC_UseCRC16(void)
{
	LPC_CRC->MODE = MODE_CFG_CRC16;
	LPC_CRC->SEED = CRC_SEED_CRC16;
}

/**
 * @brief	Sets up the CRC engine for CRC32 mode
 * @return	Nothing
 */
__STATIC_INLINE void Chip_CRC_UseCRC32(void)
{
	LPC_CRC->MODE = MODE_CFG_CRC32;
	LPC_CRC->SEED = CRC_SEED_CRC32;
}

/**
 * @brief	Sets up the CRC engine for CCITT mode
 * @return	Nothing
 */
__STATIC_INLINE void Chip_CRC_UseCCITT(void)
{
	LPC_CRC->MODE = MODE_CFG_CCITT;
	LPC_CRC->SEED = CRC_SEED_CCITT;
}

/**
 * @brief	Engage the CRC engine with defaults based on the polynomial to be used
 * @param	poly	: The enumerated polynomial to be used
 * @return	Nothing
 */
void Chip_CRC_UseDefaultConfig(CRC_POLY_T poly);

/**
 * @brief	Set the CRC Mode bits
 * @param	mode	: Mode value
 * @return	Nothing
 */
__STATIC_INLINE void Chip_CRC_SetMode(uint32_t mode)
{
	LPC_CRC->MODE = mode;
}

/**
 * @brief	Get the CRC Mode bits
 * @return	The current value of the CRC Mode bits
 */
__STATIC_INLINE uint32_t Chip_CRC_GetMode(void)
{
	return LPC_CRC->MODE;
}

/**
 * @brief	Set the seed bits used by the CRC_SUM register
 * @param	seed	: Seed value
 * @return	Nothing
 */
__STATIC_INLINE void Chip_CRC_SetSeed(uint32_t seed)
{
	LPC_CRC->SEED = seed;
}

/**
 * @brief	Get the CRC seed value
 * @return	Seed value
 */
__STATIC_INLINE uint32_t Chip_CRC_GetSeed(void)
{
	return LPC_CRC->SEED;
}

/**
 * @brief	Convenience function for writing 8-bit data to the CRC engine
 * @param	data	: 8-bit data to write
 * @return	Nothing
 */
__STATIC_INLINE void Chip_CRC_Write8(uint8_t data)
{
	LPC_CRC->WRDATA8 = data;
}

/**
 * @brief	Convenience function for writing 16-bit data to the CRC engine
 * @param	data	: 16-bit data to write
 * @return	Nothing
 */
__STATIC_INLINE void Chip_CRC_Write16(uint16_t data)
{
	LPC_CRC->WRDATA16 = data;
}

/**
 * @brief	Convenience function for writing 32-bit data to the CRC engine
 * @param	data	: 32-bit data to write
 * @return	Nothing
 */
__STATIC_INLINE void Chip_CRC_Write32(uint32_t data)
{
	LPC_CRC->WRDATA32 = data;
}

/**
 * @brief	Gets the CRC Sum based on the Mode and Seed as previously configured
 * @return	CRC Checksum value
 */
__STATIC_INLINE uint32_t Chip_CRC_Sum(void)
{
	return LPC_CRC->SUM;
}

/**
 * @brief	Convenience function for computing a standard CCITT checksum from an 8-bit data block
 * @param	data	: Pointer to the block of 8-bit data
 * @param   bytes	: The number of bytes pointed to by data
 * @return	Check sum value
 */
uint32_t Chip_CRC_CRC8(const uint8_t *data, uint32_t bytes);

/**
 * @brief	Convenience function for computing a standard CRC16 checksum from 16-bit data block
 * @param	data	: Pointer to the block of 16-bit data
 * @param   hwords	: The number of 16 byte entries pointed to by data
 * @return	Check sum value
 */
uint32_t Chip_CRC_CRC16(const uint16_t *data, uint32_t hwords);

/**
 * @brief	Convenience function for computing a standard CRC32 checksum from 32-bit data block
 * @param	data	: Pointer to the block of 32-bit data
 * @param   words	: The number of 32-bit entries pointed to by data
 * @return	Check sum value
 */
uint32_t Chip_CRC_CRC32(const uint32_t *data, uint32_t words);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __CRC_5410X_H_ */

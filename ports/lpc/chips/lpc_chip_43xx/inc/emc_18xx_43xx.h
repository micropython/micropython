/*
 * @brief LPC18xx/43xx EMC driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
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

#ifndef __EMC_18XX_43XX_H_
#define __EMC_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup EMC_18XX_43XX CHIP: LPC18xx/43xx External Memory Controller driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 * The EMC interface clocks must be enabled outside this driver prior to
 * calling any function of this driver.
 */

/**
 * @brief External Memory Controller (EMC) register block structure
 */
typedef struct {							/*!< EMC Structure          */
	__IO uint32_t  CONTROL;					/*!< Controls operation of the memory controller. */
	__I  uint32_t  STATUS;					/*!< Provides EMC status information. */
	__IO uint32_t  CONFIG;					/*!< Configures operation of the memory controller. */
	__I  uint32_t  RESERVED0[5];
	__IO uint32_t  DYNAMICCONTROL;			/*!< Controls dynamic memory operation. */
	__IO uint32_t  DYNAMICREFRESH;			/*!< Configures dynamic memory refresh operation. */
	__IO uint32_t  DYNAMICREADCONFIG;		/*!< Configures the dynamic memory read strategy. */
	__I  uint32_t  RESERVED1;
	__IO uint32_t  DYNAMICRP;				/*!< Selects the precharge command period. */
	__IO uint32_t  DYNAMICRAS;				/*!< Selects the active to precharge command period. */
	__IO uint32_t  DYNAMICSREX;				/*!< Selects the self-refresh exit time. */
	__IO uint32_t  DYNAMICAPR;				/*!< Selects the last-data-out to active command time. */
	__IO uint32_t  DYNAMICDAL;				/*!< Selects the data-in to active command time. */
	__IO uint32_t  DYNAMICWR;				/*!< Selects the write recovery time. */
	__IO uint32_t  DYNAMICRC;				/*!< Selects the active to active command period. */
	__IO uint32_t  DYNAMICRFC;				/*!< Selects the auto-refresh period. */
	__IO uint32_t  DYNAMICXSR;				/*!< Selects the exit self-refresh to active command time. */
	__IO uint32_t  DYNAMICRRD;				/*!< Selects the active bank A to active bank B latency. */
	__IO uint32_t  DYNAMICMRD;				/*!< Selects the load mode register to active command time. */
	__I  uint32_t  RESERVED2[9];
	__IO uint32_t  STATICEXTENDEDWAIT;		/*!< Selects time for long static memory read and write transfers. */
	__I  uint32_t  RESERVED3[31];
	__IO uint32_t  DYNAMICCONFIG0;			/*!< Selects the configuration information for dynamic memory chip select n. */
	__IO uint32_t  DYNAMICRASCAS0;			/*!< Selects the RAS and CAS latencies for dynamic memory chip select n. */
	__I  uint32_t  RESERVED4[6];
	__IO uint32_t  DYNAMICCONFIG1;			/*!< Selects the configuration information for dynamic memory chip select n. */
	__IO uint32_t  DYNAMICRASCAS1;			/*!< Selects the RAS and CAS latencies for dynamic memory chip select n. */
	__I  uint32_t  RESERVED5[6];
	__IO uint32_t  DYNAMICCONFIG2;			/*!< Selects the configuration information for dynamic memory chip select n. */
	__IO uint32_t  DYNAMICRASCAS2;			/*!< Selects the RAS and CAS latencies for dynamic memory chip select n. */
	__I  uint32_t  RESERVED6[6];
	__IO uint32_t  DYNAMICCONFIG3;			/*!< Selects the configuration information for dynamic memory chip select n. */
	__IO uint32_t  DYNAMICRASCAS3;			/*!< Selects the RAS and CAS latencies for dynamic memory chip select n. */
	__I  uint32_t  RESERVED7[38];
	__IO uint32_t  STATICCONFIG0;			/*!< Selects the memory configuration for static chip select n. */
	__IO uint32_t  STATICWAITWEN0;			/*!< Selects the delay from chip select n to write enable. */
	__IO uint32_t  STATICWAITOEN0;			/*!< Selects the delay from chip select n or address change, whichever is later, to output enable. */
	__IO uint32_t  STATICWAITRD0;			/*!< Selects the delay from chip select n to a read access. */
	__IO uint32_t  STATICWAITPAG0;			/*!< Selects the delay for asynchronous page mode sequential accesses for chip select n. */
	__IO uint32_t  STATICWAITWR0;			/*!< Selects the delay from chip select n to a write access. */
	__IO uint32_t  STATICWAITTURN0;			/*!< Selects bus turnaround cycles */
	__I  uint32_t  RESERVED8;
	__IO uint32_t  STATICCONFIG1;			/*!< Selects the memory configuration for static chip select n. */
	__IO uint32_t  STATICWAITWEN1;			/*!< Selects the delay from chip select n to write enable. */
	__IO uint32_t  STATICWAITOEN1;			/*!< Selects the delay from chip select n or address change, whichever is later, to output enable. */
	__IO uint32_t  STATICWAITRD1;			/*!< Selects the delay from chip select n to a read access. */
	__IO uint32_t  STATICWAITPAG1;			/*!< Selects the delay for asynchronous page mode sequential accesses for chip select n. */
	__IO uint32_t  STATICWAITWR1;			/*!< Selects the delay from chip select n to a write access. */
	__IO uint32_t  STATICWAITTURN1;			/*!< Selects bus turnaround cycles */
	__I  uint32_t  RESERVED9;
	__IO uint32_t  STATICCONFIG2;			/*!< Selects the memory configuration for static chip select n. */
	__IO uint32_t  STATICWAITWEN2;			/*!< Selects the delay from chip select n to write enable. */
	__IO uint32_t  STATICWAITOEN2;			/*!< Selects the delay from chip select n or address change, whichever is later, to output enable. */
	__IO uint32_t  STATICWAITRD2;			/*!< Selects the delay from chip select n to a read access. */
	__IO uint32_t  STATICWAITPAG2;			/*!< Selects the delay for asynchronous page mode sequential accesses for chip select n. */
	__IO uint32_t  STATICWAITWR2;			/*!< Selects the delay from chip select n to a write access. */
	__IO uint32_t  STATICWAITTURN2;			/*!< Selects bus turnaround cycles */
	__I  uint32_t  RESERVED10;
	__IO uint32_t  STATICCONFIG3;			/*!< Selects the memory configuration for static chip select n. */
	__IO uint32_t  STATICWAITWEN3;			/*!< Selects the delay from chip select n to write enable. */
	__IO uint32_t  STATICWAITOEN3;			/*!< Selects the delay from chip select n or address change, whichever is later, to output enable. */
	__IO uint32_t  STATICWAITRD3;			/*!< Selects the delay from chip select n to a read access. */
	__IO uint32_t  STATICWAITPAG3;			/*!< Selects the delay for asynchronous page mode sequential accesses for chip select n. */
	__IO uint32_t  STATICWAITWR3;			/*!< Selects the delay from chip select n to a write access. */
	__IO uint32_t  STATICWAITTURN3;			/*!< Selects bus turnaround cycles */
} LPC_EMC_T;

/**
 * Dynamic Chip Select Address
 */
#define EMC_ADDRESS_DYCS0   (0x28000000)
#define EMC_ADDRESS_DYCS1   (0x30000000)
#define EMC_ADDRESS_DYCS2   (0x60000000)
#define EMC_ADDRESS_DYCS3   (0x70000000)

/**
 * Static Chip Select Address
 */
#define EMC_ADDRESS_CS0     (0x1C000000)
#define EMC_ADDRESS_CS1     (0x1D000000)
#define EMC_ADDRESS_CS2     (0x1E000000)
#define EMC_ADDRESS_CS3     (0x1F000000)

/**
 * @brief EMC register support bitfields and mask
 */
/* Reserve for extending support to ARM9 or nextgen LPC */
#define EMC_SUPPORT_ONLY_PL172	/*!< Reserve for extending support to ARM9 or nextgen LPC */

#define EMC_CONFIG_ENDIAN_LITTLE    (0)		/*!< Value for EMC to operate in Little Endian Mode */
#define EMC_CONFIG_ENDIAN_BIG         (1)	/*!< Value for EMC to operate in Big Endian Mode */

#define EMC_CONFIG_BUFFER_ENABLE    (1 << 19)	/*!< EMC Buffer enable bit in EMC Dynamic Configuration register */
#define EMC_CONFIG_WRITE_PROTECT    (1 << 20)	/*!< EMC Write protect bit in EMC Dynamic Configuration register */

/* Dynamic Memory Configuration Register Bit Definitions */
#define EMC_DYN_CONFIG_MD_BIT             (3)								/*!< Memory device bit in EMC Dynamic Configuration register */
#define EMC_DYN_CONFIG_MD_SDRAM         (0 << EMC_DYN_CONFIG_MD_BIT)		/*!< Select device as SDRAM in EMC Dynamic Configuration register */
#define EMC_DYN_CONFIG_MD_LPSDRAM       (1 << EMC_DYN_CONFIG_MD_BIT)		/*!< Select device as LPSDRAM in EMC Dynamic Configuration register */

#define EMC_DYN_CONFIG_LPSDRAM_BIT      (12)							/*!< LPSDRAM bit in EMC Dynamic Configuration register */
#define EMC_DYN_CONFIG_LPSDRAM          (1 << EMC_DYN_CONFIG_LPSDRAM_BIT)	/*!< LPSDRAM value in EMC Dynamic Configuration register */

#define EMC_DYN_CONFIG_DEV_SIZE_BIT     (9)									/*!< Device Size starting bit in EMC Dynamic Configuration register */
#define EMC_DYN_CONFIG_DEV_SIZE_16Mb    (0x00 << EMC_DYN_CONFIG_DEV_SIZE_BIT)	/*!< 16Mb Device Size value in EMC Dynamic Configuration register */
#define EMC_DYN_CONFIG_DEV_SIZE_64Mb    (0x01 << EMC_DYN_CONFIG_DEV_SIZE_BIT)	/*!< 64Mb Device Size value in EMC Dynamic Configuration register */
#define EMC_DYN_CONFIG_DEV_SIZE_128Mb   (0x02 << EMC_DYN_CONFIG_DEV_SIZE_BIT)	/*!< 128Mb Device Size value in EMC Dynamic Configuration register */
#define EMC_DYN_CONFIG_DEV_SIZE_256Mb   (0x03 << EMC_DYN_CONFIG_DEV_SIZE_BIT)	/*!< 256Mb Device Size value in EMC Dynamic Configuration register */
#define EMC_DYN_CONFIG_DEV_SIZE_512Mb   (0x04 << EMC_DYN_CONFIG_DEV_SIZE_BIT)	/*!< 512Mb Device Size value in EMC Dynamic Configuration register */

#define EMC_DYN_CONFIG_DEV_BUS_BIT      (7)									/*!< Device bus width starting bit in EMC Dynamic Configuration register */
#define EMC_DYN_CONFIG_DEV_BUS_8        (0x00 << EMC_DYN_CONFIG_DEV_BUS_BIT)	/*!< Device 8-bit bus width value in EMC Dynamic Configuration register */
#define EMC_DYN_CONFIG_DEV_BUS_16       (0x01 << EMC_DYN_CONFIG_DEV_BUS_BIT)	/*!< Device 16-bit bus width value in EMC Dynamic Configuration register */
#define EMC_DYN_CONFIG_DEV_BUS_32       (0x02 << EMC_DYN_CONFIG_DEV_BUS_BIT)	/*!< Device 32-bit bus width value in EMC Dynamic Configuration register */

#define EMC_DYN_CONFIG_DATA_BUS_WIDTH_BIT   (14)									/*!< Device data bus width starting bit in EMC Dynamic Configuration register */
#define EMC_DYN_CONFIG_DATA_BUS_16      (0x00 << EMC_DYN_CONFIG_DATA_BUS_WIDTH_BIT)	/*!< Device 16-bit data bus width value in EMC Dynamic Configuration register */
#define EMC_DYN_CONFIG_DATA_BUS_32      (0x01 << EMC_DYN_CONFIG_DATA_BUS_WIDTH_BIT)	/*!< Device 32-bit bus width value in EMC Dynamic Configuration register */

/*!< Memory configuration values in EMC Dynamic Configuration Register */
#define EMC_DYN_CONFIG_2Mx8_2BANKS_11ROWS_9COLS     ((0x0 << 9) | (0x0 << 7))	/*!< Value for Memory configuration - 2Mx8 2 Banks 11 Rows 9 Columns */
#define EMC_DYN_CONFIG_1Mx16_2BANKS_11ROWS_8COLS    ((0x0 << 9) | (0x1 << 7))	/*!< Value for Memory configuration - 1Mx16 2 Banks 11 Rows 8 Columns */
#define EMC_DYN_CONFIG_8Mx8_4BANKS_12ROWS_9COLS     ((0x1 << 9) | (0x0 << 7))	/*!< Value for Memory configuration - 8Mx8 4 Banks 12 Rows 9 Columns */
#define EMC_DYN_CONFIG_4Mx16_4BANKS_12ROWS_8COLS    ((0x1 << 9) | (0x1 << 7))	/*!< Value for Memory configuration - 4Mx16 4 Banks 12 Rows 8 Columns */
#define EMC_DYN_CONFIG_2Mx32_4BANKS_11ROWS_8COLS    ((0x1 << 9) | (0x2 << 7))	/*!< Value for Memory configuration - 2Mx32 4 Banks 11 Rows 8 Columns */
#define EMC_DYN_CONFIG_16Mx8_4BANKS_12ROWS_10COLS   ((0x2 << 9) | (0x0 << 7))	/*!< Value for Memory configuration - 16Mx8 4 Banks 12 Rows 10 Columns */
#define EMC_DYN_CONFIG_8Mx16_4BANKS_12ROWS_9COLS    ((0x2 << 9) | (0x1 << 7))	/*!< Value for Memory configuration - 8Mx16 4 Banks 12 Rows 9 Columns */
#define EMC_DYN_CONFIG_4Mx32_4BANKS_12ROWS_8COLS    ((0x2 << 9) | (0x2 << 7))	/*!< Value for Memory configuration - 4Mx32 4 Banks 12 Rows 8 Columns */
#define EMC_DYN_CONFIG_32Mx8_4BANKS_13ROWS_10COLS   ((0x3 << 9) | (0x0 << 7))	/*!< Value for Memory configuration - 32Mx8 4 Banks 13 Rows 10 Columns */
#define EMC_DYN_CONFIG_16Mx16_4BANKS_13ROWS_9COLS   ((0x3 << 9) | (0x1 << 7))	/*!< Value for Memory configuration - 16Mx16 4 Banks 13 Rows 8 Columns */
#define EMC_DYN_CONFIG_8Mx32_4BANKS_13ROWS_8COLS    ((0x3 << 9) | (0x2 << 7))	/*!< Value for Memory configuration - 8Mx32 4 Banks 13 Rows 8 Columns */
#define EMC_DYN_CONFIG_64Mx8_4BANKS_13ROWS_11COLS   ((0x4 << 9) | (0x0 << 7))	/*!< Value for Memory configuration - 64Mx8 4 Banks 13 Rows 11 Columns */
#define EMC_DYN_CONFIG_32Mx16_4BANKS_13ROWS_10COLS  ((0x4 << 9) | (0x1 << 7))	/*!< Value for Memory configuration - 32Mx16 4 Banks 13 Rows 10 Columns */

/*!< Dynamic Memory Mode Register Bit Definition */
#define EMC_DYN_MODE_BURST_LEN_BIT      (0)	/*!< Starting bit No. of Burst Length in Dynamic Memory Mode Register */
#define EMC_DYN_MODE_BURST_LEN_1        (0)	/*!< Value to set Burst Length to 1 in Dynamic Memory Mode Register */
#define EMC_DYN_MODE_BURST_LEN_2        (1)	/*!< Value to set Burst Length to 2 in Dynamic Memory Mode Register */
#define EMC_DYN_MODE_BURST_LEN_4        (2)	/*!< Value to set Burst Length to 4 in Dynamic Memory Mode Register */
#define EMC_DYN_MODE_BURST_LEN_8        (3)	/*!< Value to set Burst Length to 8 in Dynamic Memory Mode Register */
#define EMC_DYN_MODE_BURST_LEN_FULL     (7)	/*!< Value to set Burst Length to Full in Dynamic Memory Mode Register */

#define EMC_DYN_MODE_BURST_TYPE_BIT         (3)									/*!< Burst Type bit in Dynamic Memory Mode Register */
#define EMC_DYN_MODE_BURST_TYPE_SEQUENTIAL  (0 << EMC_DYN_MODE_BURST_TYPE_BIT)	/*!< Burst Type Sequential in Dynamic Memory Mode Register */
#define EMC_DYN_MODE_BURST_TYPE_INTERLEAVE  (1 << EMC_DYN_MODE_BURST_TYPE_BIT)	/*!< Burst Type Interleaved in Dynamic Memory Mode Register */

/*!< CAS Latency in Dynamic Mode Register */
#define EMC_DYN_MODE_CAS_BIT    (4)							/*!< CAS latency starting bit in Dynamic Memory Mode register */
#define EMC_DYN_MODE_CAS_1      (1 << EMC_DYN_MODE_CAS_BIT)	/*!< value for CAS latency of 1 cycle */
#define EMC_DYN_MODE_CAS_2      (2 << EMC_DYN_MODE_CAS_BIT)	/*!< value for CAS latency of 2 cycle */
#define EMC_DYN_MODE_CAS_3      (3 << EMC_DYN_MODE_CAS_BIT)	/*!< value for CAS latency of 3 cycle */

/*!< Operation Mode in Dynamic Mode register */
#define EMC_DYN_MODE_OPMODE_BIT           (7)							/*!< Dynamic Mode Operation bit */
#define EMC_DYN_MODE_OPMODE_STANDARD    (0 << EMC_DYN_MODE_OPMODE_BIT)	/*!< Value for Dynamic standard operation Mode */

/*!< Write Burst Mode in Dynamic Mode register */
#define EMC_DYN_MODE_WBMODE_BIT             (9)							/*!< Write Burst Mode bit */
#define EMC_DYN_MODE_WBMODE_PROGRAMMED  (0 << EMC_DYN_MODE_WBMODE_BIT)	/*!< Write Burst Mode programmed */
#define EMC_DYN_MODE_WBMODE_SINGLE_LOC  (1 << EMC_DYN_MODE_WBMODE_BIT)	/*!< Write Burst Mode Single LOC */

/*!< Dynamic Memory Control Register Bit Definitions */
#define EMC_DYN_CONTROL_ENABLE          (0x03)	/*!< Control Enable value */

/*!< Static Memory Configuration Register Bit Definitions */
#define EMC_STATIC_CONFIG_MEM_WIDTH_8       (0)	/*!< Static Memory Configuration - 8-bit width */
#define EMC_STATIC_CONFIG_MEM_WIDTH_16      (1)	/*!< Static Memory Configuration - 16-bit width */
#define EMC_STATIC_CONFIG_MEM_WIDTH_32      (2)	/*!< Static Memory Configuration - 32-bit width */

#define EMC_STATIC_CONFIG_PAGE_MODE_BIT         (3)										/*!< Page Mode bit No */
#define EMC_STATIC_CONFIG_PAGE_MODE_ENABLE      (1 << EMC_STATIC_CONFIG_PAGE_MODE_BIT)	/*!< Value to enable Page Mode */

#define EMC_STATIC_CONFIG_CS_POL_BIT            (6)									/*!< Chip Select bit No */
#define EMC_STATIC_CONFIG_CS_POL_ACTIVE_HIGH    (1 << EMC_STATIC_CONFIG_CS_POL_BIT)	/*!< Chip Select polarity - Active High */
#define EMC_STATIC_CONFIG_CS_POL_ACTIVE_LOW     (0 << EMC_STATIC_CONFIG_CS_POL_BIT)	/*!< Chip Select polarity - Active Low */

#define EMC_STATIC_CONFIG_BLS_BIT           (7)								/*!< BLS Configuration bit No */
#define EMC_STATIC_CONFIG_BLS_HIGH          (1 << EMC_STATIC_CONFIG_BLS_BIT)	/*!< BLS High Configuration value */
#define EMC_STATIC_CONFIG_BLS_LOW           (0 << EMC_STATIC_CONFIG_BLS_BIT)	/*!< BLS Low Configuration value */

#define EMC_STATIC_CONFIG_EW_BIT            (8)								/*!< Ext Wait bit No */
#define EMC_STATIC_CONFIG_EW_ENABLE         (1 << EMC_STATIC_CONFIG_EW_BIT)	/*!< Ext Wait Enabled value */
#define EMC_STATIC_CONFIG_EW_DISABLE        (0 << EMC_STATIC_CONFIG_EW_BIT)	/*!< Ext Wait Diabled value */

/*!< Q24.8 Fixed Point Helper */
#define Q24_8_FP(x) ((x) * 256)
#define EMC_NANOSECOND(x)   Q24_8_FP(x)
#define EMC_CLOCK(x)        Q24_8_FP(-(x))

/**
 * @brief	EMC Dynamic Device Configuration structure used for IP drivers
 */
typedef struct {
	uint32_t    BaseAddr;		/*!< Base Address */
	uint8_t     RAS;			/*!< RAS value */
	uint32_t    ModeRegister;	/*!< Mode Register value */
	uint32_t    DynConfig;		/*!< Dynamic Configuration value */
} IP_EMC_DYN_DEVICE_CONFIG_T;

/**
 * @brief EMC Dynamic Configure Struct
 */
typedef struct {
	int32_t RefreshPeriod;							/*!< Refresh period */
	uint32_t ReadConfig;							/*!< Clock*/
	int32_t tRP;									/*!< Precharge Command Period */
	int32_t tRAS;									/*!< Active to Precharge Command Period */
	int32_t tSREX;									/*!< Self Refresh Exit Time */
	int32_t tAPR;									/*!< Last Data Out to Active Time */
	int32_t tDAL;									/*!< Data In to Active Command Time */
	int32_t tWR;									/*!< Write Recovery Time */
	int32_t tRC;									/*!< Active to Active Command Period */
	int32_t tRFC;									/*!< Auto-refresh Period */
	int32_t tXSR;									/*!< Exit Selt Refresh */
	int32_t tRRD;									/*!< Active Bank A to Active Bank B Time */
	int32_t tMRD;									/*!< Load Mode register command to Active Command */
	IP_EMC_DYN_DEVICE_CONFIG_T DevConfig[4];		/*!< Device Configuration array */
} IP_EMC_DYN_CONFIG_T;

/**
 * @brief EMC Static Configure Structure
 */
typedef struct {
	uint8_t ChipSelect;		/*!< Chip select */
	uint32_t Config;		/*!< Configuration value */
	int32_t WaitWen;		/*!< Write Enable Wait */
	int32_t WaitOen;		/*!< Output Enable Wait */
	int32_t WaitRd;			/*!< Read Wait */
	int32_t WaitPage;		/*!< Page Access Wait */
	int32_t WaitWr;			/*!< Write Wait */
	int32_t WaitTurn;		/*!< Turn around wait */
} IP_EMC_STATIC_CONFIG_T;

/**
 * @brief	Dyanmic memory setup
 * @param	Dynamic_Config	: Pointer to dynamic memory setup data
 * @return	None
 */
void Chip_EMC_Dynamic_Init(IP_EMC_DYN_CONFIG_T *Dynamic_Config);

/**
 * @brief	Static memory setup
 * @param	Static_Config	: Pointer to static memory setup data
 * @return	None
 */
void Chip_EMC_Static_Init(IP_EMC_STATIC_CONFIG_T *Static_Config);

/**
 * @brief	Enable Dynamic Memory Controller
 * @param	Enable	: 1 = Enable Dynamic Memory Controller, 0 = Disable
 * @return	None
 */
void Chip_EMC_Dynamic_Enable(uint8_t Enable);

/**
 * @brief	Mirror CS1 to CS0 and DYCS0
 * @param	Enable	: 1 = Mirror, 0 = Normal Memory Map
 * @return	None
 */
void Chip_EMC_Mirror(uint8_t Enable);

/**
 * @brief	Enable EMC
 * @param	Enable	: 1 = Enable, 0 = Disable
 * @return	None
 */
void Chip_EMC_Enable(uint8_t Enable);

/**
 * @brief	Set EMC LowPower Mode
 * @param	Enable	: 1 = Enable, 0 = Disable
 * @return	None
 * @note	This function should only be called when the memory
 * controller is not busy (bit 0 of the status register is not set).
 */
void Chip_EMC_LowPowerMode(uint8_t Enable);

/**
 * @brief	Initialize EMC
 * @param	Enable		: 1 = Enable, 0 = Disable
 * @param	ClockRatio	: clock out ratio, 0 = 1:1, 1 = 1:2
 * @param	EndianMode	: Endian Mode, 0 = Little, 1 = Big
 * @return	None
 */
void Chip_EMC_Init(uint32_t Enable, uint32_t ClockRatio, uint32_t EndianMode);

/**
 * @brief	Set Static Memory Extended Wait in Clock
 * @param	Wait16Clks	: Number of '16 clock' delay cycles
 * @return	None
 */
STATIC INLINE void Chip_EMC_SetStaticExtendedWait(uint32_t Wait16Clks)
{
	LPC_EMC->STATICEXTENDEDWAIT = Wait16Clks;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __EMC_18XX_43XX_H_ */







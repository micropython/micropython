/*
 * @brief LPC18xx/43xx SD/MMC card driver
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

#ifndef __SDIO_18XX_43XX_H_
#define __SDIO_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup SDIO_18XX_43XX CHIP: LPC18xx/43xx SDIO Card driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/** @brief	SDIO Driver events */
enum SDIO_EVENT
{
	SDIO_START_COMMAND,  /**! SDIO driver is about to start a command transfer */
	SDIO_START_DATA,     /**! SDIO driver is about to start a data transfer */
	SDIO_WAIT_DELAY,     /**! SDIO driver needs to wait for given milli seconds */
	SDIO_WAIT_COMMAND,   /**! SDIO driver is waiting for a command to complete */
	SDIO_WAIT_DATA,      /**! SDIO driver is waiting for data transfer to complete */

	SDIO_CARD_DETECT,    /**! SDIO driver has detected a card */
	SDIO_CMD_ERR,        /**! Error in command transfer */
	SDIO_CMD_DONE,       /**! Command transfer successful */
	SDIO_DATA_ERR,       /**! Data transfer error */
	SDIO_DATA_DONE,      /**! Data transfer successful */
	SDIO_CARD_INT,       /**! SDIO Card interrupt (from a function) */
};

/** @brief	SDIO Command Responses */
#define SDIO_CMD_RESP_R1     (1UL << 6)
#define SDIO_CMD_RESP_R2     (3UL << 6)
#define SDIO_CMD_RESP_R3     (1UL << 6)
#define SDIO_CMD_RESP_R4     (1UL << 6)
#define SDIO_CMD_RESP_R5     (1UL << 6)
#define SDIO_CMD_RESP_R6     (1UL << 6)

/** @brief	SDIO Command misc options */
#define SDIO_CMD_CRC         (1UL << 8)  /**! Response must have a valid CRC */
#define SDIO_CMD_DATA        (1UL << 9)  /**! Command is a data transfer command */

/** @brief	List of commands */
#define CMD0            (0 | (1 << 15))
#define CMD5            (5 | SDIO_CMD_RESP_R4)
#define CMD3            (3 | SDIO_CMD_RESP_R6)
#define CMD7            (7 | SDIO_CMD_RESP_R1)
#define CMD52           (52 | SDIO_CMD_RESP_R5 | SDIO_CMD_CRC)
#define CMD53           (53 | SDIO_CMD_RESP_R5 | SDIO_CMD_DATA | SDIO_CMD_CRC)

/** @brief SDIO Error numbers */
#define SDIO_ERROR           -1 /**! General SDIO Error */
#define SDIO_ERR_FNUM        -2 /**! Error getting Number of functions supported */
#define SDIO_ERR_READWRITE   -3 /**! Error when performing Read/write of data */
#define SDIO_ERR_VOLT        -4 /**! Error Reading or setting up the voltage to 3v3 */
#define SDIO_ERR_RCA         -5 /**! Error during RCA phase */
#define SDIO_ERR_INVFUNC     -6 /**! Invalid function argument */
#define SDIO_ERR_INVARG      -7 /**! Invalid argument supplied to function */

#define SDIO_VOLT_3_3    0x00100000UL  /* for CMD5 */

/* SDIO Data transfer modes */
/** @brief  Block mode transfer flag
 * 
 * When this flag is specified in a transfer the data will be transfered in blocks if not
 * it will be transfered in bytes. See SDIO_Card_DataRead(), SDIO_Card_DataWrite()
 * for more information.
 */
#define SDIO_MODE_BLOCK       (1UL << 27)

/** @brief Buffer mode transfer flag
 *
 * Default mode for SDIO_Card_ReadData() and SDIO_Card_WriteData() is FIFO mode
 * in FIFO mode all the given data will be written to or read from the same
 * register address in the function. This flag will set the transfers to BUFFER
 * mode; in BUFFER mode read first byte will be read from the given source address
 * and the next byte will be read from the next source address (i.e src_addr + 1),
 * and so on, in BUFFER mode write first byte will be written to dest_addr, next
 * byte will be written to dest_addr + 1 and so on.
 */
#define SDIO_MODE_BUFFER      (1UL << 26)

/* ---- SDIO Internal map ---- */
#define SDIO_AREA_CIA			0			/* function 0 */

/* ---- Card Capability(0x08) register ---- */
#define SDIO_CCCR_LSC			0x40u		/* card is low-speed cards */
#define SDIO_CCCR_4BLS			0x80u		/* 4-bit support for low-speed cards */

#define SDIO_POWER_INIT  1

#define SDIO_CLK_HISPEED            33000000UL    /* High-Speed Clock	*/
#define SDIO_CLK_FULLSPEED          16000000UL    /* Full-Speed Clock	*/
#define SDIO_CLK_LOWSPEED           400000        /* Low-Speed Clock	*/

/**
 * @brief	Initialize the SDIO card
 * @param	pSDMMC		: SDMMC peripheral selected
 * @param	freq		: Initial frequency to use during the enumeration
 * @return	0 on Success; > 0 on response error [like CRC error] < 0 on BUS error
 */
int SDIO_Card_Init(LPC_SDMMC_T *pSDMMC, uint32_t freq);

/**
 * @brief	Write 8-Bit register from SDIO register space
 * @param	pSDMMC		: SDMMC peripheral selected
 * @param	func		: function number [0 to 7] [0 = CIA function]
 * @param	addr		: Address of the register to read
 * @param	data		: 8-bit data be written
 * @return	0 on Success; > 0 on response error [like CRC error] < 0 on BUS error
 * @note SDIO_Setup_Callback() function must be called to setup the call backs before
 * calling  this API.
 */
int SDIO_Write_Direct(LPC_SDMMC_T *pSDMMC, uint32_t func, uint32_t addr, uint32_t data);

/**
 * @brief	Write 8-Bit register from SDIO register space and read the register back
 * @param	pSDMMC		: SDMMC peripheral selected
 * @param	func		: function number [0 to 7] [0 = CIA function]
 * @param	addr		: Address of the register to read
 * @param	data		: Pointer to memory where the 8-bit data be stored
 * @return	0 on Success; > 0 on response error [like CRC error] < 0 on BUS error
 * @note	@a data must have the value to be written stored in it when the function is called
 */
int SDIO_WriteRead_Direct(LPC_SDMMC_T *pSDMMC, uint32_t func, uint32_t addr, uint32_t *data);

/**
 * @brief	Read an 8-Bit register from SDIO register space
 * @param	pSDMMC		: SDMMC peripheral selected
 * @param	func		: function number [0 to 7] [0 = CIA function]
 * @param	addr		: Address of the register to read
 * @param	data		: Pointer to memory where the 8-bit data be stored
 * @return	0 on Success; > 0 on response error [like CRC error] < 0 on BUS error
 */
int SDIO_Read_Direct(LPC_SDMMC_T *pSDMMC, uint32_t func, uint32_t addr, uint32_t *data);

/**
 * @brief	Setup SDIO wait and wakeup callbacks
 * @param	pSDMMC		: SDMMC peripheral selected
 * @param	wake_evt	: Wakeup event call-back handler
 * @param	wait_evt	: Wait event call-back handler
 * @return	Nothing
 * @note	@a wake_evt and @a wait_evt should always be non-null function pointers
 * This function must be called before calling SDIO_Card_Init() function
 */
void SDIO_Setup_Callback(LPC_SDMMC_T *pSDMMC,
	void (*wake_evt)(LPC_SDMMC_T *pSDMMC, uint32_t event, void *arg),
	uint32_t (*wait_evt)(LPC_SDMMC_T *pSDMMC, uint32_t event, void *arg));

/**
 * @brief	SDIO Event handler [Should be called from SDIO interrupt handler]
 * @param	pSDMMC		: SDMMC peripheral selected
 * @return	Nothing
 */
void SDIO_Handler(LPC_SDMMC_T *pSDMMC);

/**
 * @brief	Sends a command to the SDIO Card [Example CMD52]
 * @param	pSDMMC		: SDMMC peripheral selected
 * @param	cmd			: Command to be sent along with any flags
 * @param	arg			: Argument for the command
 * @return	0 on Success; Non-Zero on failure
 */
uint32_t SDIO_Send_Command(LPC_SDMMC_T *pSDMMC, uint32_t cmd, uint32_t arg);

/**
 * @brief	Gets the block size of a given function
 * @param	pSDMMC		: SDMMC peripheral selected
 * @param	func		: function number [0 to 7] [0 = CIA function]
 * @return	Block size of the given function
 * @sa		SDIO_Card_SetBlockSize()
 * @note	If the return value is 0 then bock size is not set using
 * SDIO_Card_SetBlockSize(), or given @a func is not valid or the
 * card does not support block data transfers.
 */
uint32_t SDIO_Card_GetBlockSize(LPC_SDMMC_T *pSDMMC, uint32_t func);

/**
 * @brief	Sets the block size of a given function
 * @param	pSDMMC		: SDMMC peripheral selected
 * @param	func		: function number [0 to 7] [0 = CIA function]
 * @param	blkSize		: Block size to set
 * @return	0 on success; Non-Zero on failure
 * @sa		SDIO_Card_GetBlockSize()
 * @note	After setting block size using this API, if
 * SDIO_Card_GetBlockSize() returns 0 for a valid function then the card
 * does not support block transfers.
 */
int SDIO_Card_SetBlockSize(LPC_SDMMC_T *pSDMMC, uint32_t func, uint32_t blkSize);

/**
 * @brief	Writes stream or block of data to the SDIO card [Using CMD53]
 * @param	pSDMMC		: SDMMC peripheral selected
 * @param	func		: function number [0 to 7] [0 = CIA function]
 * @param	dest_addr	: Address where the data to be written (inside @a func register space)
 * @param	src_addr	: Buffer from which data to be taken
 * @param	size		: Number of Bytes/Blocks to be transfered [Must be in the range 1 to 512]
 * @param	flags		: Or-ed value of #SDIO_MODE_BLOCK, #SDIO_MODE_BUFFER
 * @return	0 on success; Non-Zero on failure
 * @note	When #SDIO_MODE_BLOCK is set in @a flags the size is number of blocks, so
 * the number of bytes transferd will be @a size * "block size" [See SDIO_Card_GetBlockSize() and
 * SDIO_Card_SetBlockSize() for more information]
 */
int SDIO_Card_WriteData(LPC_SDMMC_T *pSDMMC, uint32_t func,
	uint32_t dest_addr, const uint8_t *src_addr,
	uint32_t size, uint32_t flags);

/**
 * @brief	Reads stream or block of data from the SDIO card [Using CMD53]
 * @param	pSDMMC		: SDMMC peripheral selected
 * @param	func		: function number [0 to 7] [0 = CIA function]
 * @param	dest_addr	: memory where the data to be read into
 * @param	src_addr	: Register address from which data to be read  (inside @a func register space)
 * @param	size		: Number of Bytes/Blocks to be transfered [Must be in the range 1 to 512]
 * @param	flags		: Or-ed value of #SDIO_MODE_BLOCK, #SDIO_MODE_BUFFER
 * @return	0 on success; Non-Zero on failure
 * @note	When #SDIO_MODE_BLOCK is set in @a flags the size is number of blocks, so
 * the number of bytes transferd will be @a size * "block size" [See SDIO_Card_GetBlockSize() and
 * SDIO_Card_SetBlockSize() for more information]
 */
int SDIO_Card_ReadData(LPC_SDMMC_T *pSDMMC, uint32_t func,
	uint8_t *dest_addr, uint32_t src_addr,
	uint32_t size, uint32_t flags);

/**
 * @brief	Disable SDIO interrupt for a given function
 * @param	pSDMMC		: SDMMC peripheral selected
 * @param	func		: function number [0 to 7] [0 = CIA function]
 * @return	0 - on success; Non zero on failure
 */
int SDIO_Card_DisableInt(LPC_SDMMC_T *pSDMMC, uint32_t func);

/**
 * @brief	Enable SDIO interrupt for a given function
 * @param	pSDMMC		: SDMMC peripheral selected
 * @param	func		: function number [0 to 7] [0 = CIA function]
 * @return	0 - on success; Non zero on failure
 */
int SDIO_Card_EnableInt(LPC_SDMMC_T *pSDMMC, uint32_t func);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif /* __SDIO_18XX_43XX_H_ */







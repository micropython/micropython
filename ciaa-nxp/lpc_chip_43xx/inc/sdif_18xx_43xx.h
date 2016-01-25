/*
 * @brief LPC18xx/43xx SD/SDIO driver
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

#ifndef __SDIF_18XX_43XX_H_
#define __SDIF_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup SDIF_18XX_43XX CHIP: LPC18xx/43xx SD/SDIO driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief SD/MMC & SDIO register block structure
 */
typedef struct {				/*!< SDMMC Structure        */
	__IO uint32_t  CTRL;		/*!< Control Register       */
	__IO uint32_t  PWREN;		/*!< Power Enable Register  */
	__IO uint32_t  CLKDIV;		/*!< Clock Divider Register */
	__IO uint32_t  CLKSRC;		/*!< SD Clock Source Register */
	__IO uint32_t  CLKENA;		/*!< Clock Enable Register  */
	__IO uint32_t  TMOUT;		/*!< Timeout Register       */
	__IO uint32_t  CTYPE;		/*!< Card Type Register     */
	__IO uint32_t  BLKSIZ;		/*!< Block Size Register    */
	__IO uint32_t  BYTCNT;		/*!< Byte Count Register    */
	__IO uint32_t  INTMASK;		/*!< Interrupt Mask Register */
	__IO uint32_t  CMDARG;		/*!< Command Argument Register */
	__IO uint32_t  CMD;			/*!< Command Register       */
	__I  uint32_t  RESP0;		/*!< Response Register 0    */
	__I  uint32_t  RESP1;		/*!< Response Register 1    */
	__I  uint32_t  RESP2;		/*!< Response Register 2    */
	__I  uint32_t  RESP3;		/*!< Response Register 3    */
	__I  uint32_t  MINTSTS;		/*!< Masked Interrupt Status Register */
	__IO uint32_t  RINTSTS;		/*!< Raw Interrupt Status Register */
	__I  uint32_t  STATUS;		/*!< Status Register        */
	__IO uint32_t  FIFOTH;		/*!< FIFO Threshold Watermark Register */
	__I  uint32_t  CDETECT;		/*!< Card Detect Register   */
	__I  uint32_t  WRTPRT;		/*!< Write Protect Register */
	__IO uint32_t  GPIO;		/*!< General Purpose Input/Output Register */
	__I  uint32_t  TCBCNT;		/*!< Transferred CIU Card Byte Count Register */
	__I  uint32_t  TBBCNT;		/*!< Transferred Host to BIU-FIFO Byte Count Register */
	__IO uint32_t  DEBNCE;		/*!< Debounce Count Register */
	__IO uint32_t  USRID;		/*!< User ID Register       */
	__I  uint32_t  VERID;		/*!< Version ID Register    */
	__I  uint32_t  RESERVED0;
	__IO uint32_t  UHS_REG;		/*!< UHS-1 Register         */
	__IO uint32_t  RST_N;		/*!< Hardware Reset         */
	__I  uint32_t  RESERVED1;
	__IO uint32_t  BMOD;		/*!< Bus Mode Register      */
	__O  uint32_t  PLDMND;		/*!< Poll Demand Register   */
	__IO uint32_t  DBADDR;		/*!< Descriptor List Base Address Register */
	__IO uint32_t  IDSTS;		/*!< Internal DMAC Status Register */
	__IO uint32_t  IDINTEN;		/*!< Internal DMAC Interrupt Enable Register */
	__I  uint32_t  DSCADDR;		/*!< Current Host Descriptor Address Register */
	__I  uint32_t  BUFADDR;		/*!< Current Buffer Descriptor Address Register */
} LPC_SDMMC_T;

/** @brief  SDIO DMA descriptor control (des0) register defines
 */
#define MCI_DMADES0_OWN         (1UL << 31)		/*!< DMA owns descriptor bit */
#define MCI_DMADES0_CES         (1 << 30)		/*!< Card Error Summary bit */
#define MCI_DMADES0_ER          (1 << 5)		/*!< End of descriptopr ring bit */
#define MCI_DMADES0_CH          (1 << 4)		/*!< Second address chained bit */
#define MCI_DMADES0_FS          (1 << 3)		/*!< First descriptor bit */
#define MCI_DMADES0_LD          (1 << 2)		/*!< Last descriptor bit */
#define MCI_DMADES0_DIC         (1 << 1)		/*!< Disable interrupt on completion bit */

/** @brief  SDIO DMA descriptor size (des1) register defines
 */
#define MCI_DMADES1_BS1(x)      (x)				/*!< Size of buffer 1 */
#define MCI_DMADES1_BS2(x)      ((x) << 13)		/*!< Size of buffer 2 */
#define MCI_DMADES1_MAXTR       4096			/*!< Max transfer size per buffer */

/** @brief  SDIO control register defines
 */
#define MCI_CTRL_USE_INT_DMAC   (1 << 25)		/*!< Use internal DMA */
#define MCI_CTRL_CARDV_MASK     (0x7 << 16)		/*!< SD_VOLT[2:0} pins output state mask */
#define MCI_CTRL_CEATA_INT_EN   (1 << 11)		/*!< Enable CE-ATA interrupts */
#define MCI_CTRL_SEND_AS_CCSD   (1 << 10)		/*!< Send auto-stop */
#define MCI_CTRL_SEND_CCSD      (1 << 9)		/*!< Send CCSD */
#define MCI_CTRL_ABRT_READ_DATA (1 << 8)		/*!< Abort read data */
#define MCI_CTRL_SEND_IRQ_RESP  (1 << 7)		/*!< Send auto-IRQ response */
#define MCI_CTRL_READ_WAIT      (1 << 6)		/*!< Assert read-wait for SDIO */
#define MCI_CTRL_INT_ENABLE     (1 << 4)		/*!< Global interrupt enable */
#define MCI_CTRL_DMA_RESET      (1 << 2)		/*!< Reset internal DMA */
#define MCI_CTRL_FIFO_RESET     (1 << 1)		/*!< Reset data FIFO pointers */
#define MCI_CTRL_RESET          (1 << 0)		/*!< Reset controller */

/** @brief SDIO Power Enable register defines
 */
#define MCI_POWER_ENABLE        0x1				/*!< Enable slot power signal (SD_POW) */

/** @brief SDIO Clock divider register defines
 */
#define MCI_CLOCK_DIVIDER(dn, d2) ((d2) << ((dn) * 8))	/*!< Set cklock divider */

/** @brief SDIO Clock source register defines
 */
#define MCI_CLKSRC_CLKDIV0      0
#define MCI_CLKSRC_CLKDIV1      1
#define MCI_CLKSRC_CLKDIV2      2
#define MCI_CLKSRC_CLKDIV3      3
#define MCI_CLK_SOURCE(clksrc)  (clksrc)		/*!< Set cklock divider source */

/** @brief SDIO Clock Enable register defines
 */
#define MCI_CLKEN_LOW_PWR       (1 << 16)		/*!< Enable clock idle for slot */
#define MCI_CLKEN_ENABLE        (1 << 0)		/*!< Enable slot clock */

/** @brief SDIO time-out register defines
 */
#define MCI_TMOUT_DATA(clks)    ((clks) << 8)	/*!< Data timeout clocks */
#define MCI_TMOUT_DATA_MSK      0xFFFFFF00
#define MCI_TMOUT_RESP(clks)    ((clks) & 0xFF)	/*!< Response timeout clocks */
#define MCI_TMOUT_RESP_MSK      0xFF

/** @brief SDIO card-type register defines
 */
#define MCI_CTYPE_8BIT          (1 << 16)		/*!< Enable 4-bit mode */
#define MCI_CTYPE_4BIT          (1 << 0)		/*!< Enable 8-bit mode */

/** @brief SDIO Interrupt status & mask register defines
 */
#define MCI_INT_SDIO            (1 << 16)		/*!< SDIO interrupt */
#define MCI_INT_EBE             (1 << 15)		/*!< End-bit error */
#define MCI_INT_ACD             (1 << 14)		/*!< Auto command done */
#define MCI_INT_SBE             (1 << 13)		/*!< Start bit error */
#define MCI_INT_HLE             (1 << 12)		/*!< Hardware locked error */
#define MCI_INT_FRUN            (1 << 11)		/*!< FIFO overrun/underrun error */
#define MCI_INT_HTO             (1 << 10)		/*!< Host data starvation error */
#define MCI_INT_DTO             (1 << 9)		/*!< Data timeout error */
#define MCI_INT_RTO             (1 << 8)		/*!< Response timeout error */
#define MCI_INT_DCRC            (1 << 7)		/*!< Data CRC error */
#define MCI_INT_RCRC            (1 << 6)		/*!< Response CRC error */
#define MCI_INT_RXDR            (1 << 5)		/*!< RX data ready */
#define MCI_INT_TXDR            (1 << 4)		/*!< TX data needed */
#define MCI_INT_DATA_OVER       (1 << 3)		/*!< Data transfer over */
#define MCI_INT_CMD_DONE        (1 << 2)		/*!< Command done */
#define MCI_INT_RESP_ERR        (1 << 1)		/*!< Command response error */
#define MCI_INT_CD              (1 << 0)		/*!< Card detect */

/** @brief SDIO Command register defines
 */
#define MCI_CMD_START           (1UL << 31)		/*!< Start command */
#define MCI_CMD_VOLT_SWITCH     (1 << 28)		/*!< Voltage switch bit */
#define MCI_CMD_BOOT_MODE       (1 << 27)		/*!< Boot mode */
#define MCI_CMD_DISABLE_BOOT    (1 << 26)		/*!< Disable boot */
#define MCI_CMD_EXPECT_BOOT_ACK (1 << 25)		/*!< Expect boot ack */
#define MCI_CMD_ENABLE_BOOT     (1 << 24)		/*!< Enable boot */
#define MCI_CMD_CCS_EXP         (1 << 23)		/*!< CCS expected */
#define MCI_CMD_CEATA_RD        (1 << 22)		/*!< CE-ATA read in progress */
#define MCI_CMD_UPD_CLK         (1 << 21)		/*!< Update clock register only */
#define MCI_CMD_INIT            (1 << 15)		/*!< Send init sequence */
#define MCI_CMD_STOP            (1 << 14)		/*!< Stop/abort command */
#define MCI_CMD_PRV_DAT_WAIT    (1 << 13)		/*!< Wait before send */
#define MCI_CMD_SEND_STOP       (1 << 12)		/*!< Send auto-stop */
#define MCI_CMD_STRM_MODE       (1 << 11)		/*!< Stream transfer mode */
#define MCI_CMD_DAT_WR          (1 << 10)		/*!< Read(0)/Write(1) selection */
#define MCI_CMD_DAT_EXP         (1 << 9)		/*!< Data expected */
#define MCI_CMD_RESP_CRC        (1 << 8)		/*!< Check response CRC */
#define MCI_CMD_RESP_LONG       (1 << 7)		/*!< Response length */
#define MCI_CMD_RESP_EXP        (1 << 6)		/*!< Response expected */
#define MCI_CMD_INDX(n)         ((n) & 0x1F)

/** @brief SDIO status register definess
 */
#define MCI_STS_GET_FCNT(x)     (((x) >> 17) & 0x1FF)

/** @brief SDIO FIFO threshold defines
 */
#define MCI_FIFOTH_TX_WM(x)     ((x) & 0xFFF)
#define MCI_FIFOTH_RX_WM(x)     (((x) & 0xFFF) << 16)
#define MCI_FIFOTH_DMA_MTS_1    (0UL << 28)
#define MCI_FIFOTH_DMA_MTS_4    (1UL << 28)
#define MCI_FIFOTH_DMA_MTS_8    (2UL << 28)
#define MCI_FIFOTH_DMA_MTS_16   (3UL << 28)
#define MCI_FIFOTH_DMA_MTS_32   (4UL << 28)
#define MCI_FIFOTH_DMA_MTS_64   (5UL << 28)
#define MCI_FIFOTH_DMA_MTS_128  (6UL << 28)
#define MCI_FIFOTH_DMA_MTS_256  (7UL << 28)

/** @brief Bus mode register defines
 */
#define MCI_BMOD_PBL1           (0 << 8)		/*!< Burst length = 1 */
#define MCI_BMOD_PBL4           (1 << 8)		/*!< Burst length = 4 */
#define MCI_BMOD_PBL8           (2 << 8)		/*!< Burst length = 8 */
#define MCI_BMOD_PBL16          (3 << 8)		/*!< Burst length = 16 */
#define MCI_BMOD_PBL32          (4 << 8)		/*!< Burst length = 32 */
#define MCI_BMOD_PBL64          (5 << 8)		/*!< Burst length = 64 */
#define MCI_BMOD_PBL128         (6 << 8)		/*!< Burst length = 128 */
#define MCI_BMOD_PBL256         (7 << 8)		/*!< Burst length = 256 */
#define MCI_BMOD_DE             (1 << 7)		/*!< Enable internal DMAC */
#define MCI_BMOD_DSL(len)       ((len) << 2)	/*!< Descriptor skip length */
#define MCI_BMOD_FB             (1 << 1)		/*!< Fixed bursts */
#define MCI_BMOD_SWR            (1 << 0)		/*!< Software reset of internal registers */

/** @brief Commonly used definitions
 */
#define SD_FIFO_SZ              32				/*!< Size of SDIO FIFOs (32-bit wide) */

/** Function prototype for SD interface IRQ callback */
typedef uint32_t (*MCI_IRQ_CB_FUNC_T)(uint32_t);

/** Function prototype for SD detect and write protect status check */
typedef int32_t (*PSCHECK_FUNC_T)(void);

/** Function prototype for SD slot power enable or slot reset */
typedef void (*PS_POWER_FUNC_T)(int32_t enable);

/** @brief  SDIO chained DMA descriptor
 */
typedef struct {
	volatile uint32_t des0;						/*!< Control and status */
	volatile uint32_t des1;						/*!< Buffer size(s) */
	volatile uint32_t des2;						/*!< Buffer address pointer 1 */
	volatile uint32_t des3;						/*!< Buffer address pointer 2 */
} pSDMMC_DMA_T;

/** @brief  SDIO device type
 */
typedef struct _sdif_device {
	/* MCI_IRQ_CB_FUNC_T irq_cb; */
	pSDMMC_DMA_T mci_dma_dd[1 + (0x10000 / MCI_DMADES1_MAXTR)];
	/* uint32_t sdio_clk_rate; */
	/* uint32_t sdif_slot_clk_rate; */
	/* int32_t clock_enabled; */
} sdif_device;

/** @brief Setup options for the SDIO driver
 */
#define US_TIMEOUT            1000000		/*!< give 1 atleast 1 sec for the card to respond */
#define MS_ACQUIRE_DELAY      (10)			/*!< inter-command acquire oper condition delay in msec*/
#define INIT_OP_RETRIES       50			/*!< initial OP_COND retries */
#define SET_OP_RETRIES        1000			/*!< set OP_COND retries */
#define SDIO_BUS_WIDTH        4				/*!< Max bus width supported */
#define SD_MMC_ENUM_CLOCK       400000		/*!< Typical enumeration clock rate */
#define MMC_MAX_CLOCK           20000000	/*!< Max MMC clock rate */
#define MMC_LOW_BUS_MAX_CLOCK   26000000	/*!< Type 0 MMC card max clock rate */
#define MMC_HIGH_BUS_MAX_CLOCK  52000000	/*!< Type 1 MMC card max clock rate */
#define SD_MAX_CLOCK            25000000	/*!< Max SD clock rate */

/**
 * @brief	Set block size for the transfer
 * @param	pSDMMC	: SDMMC peripheral selected
 * @param	bytes	: block size in bytes
 * @return	None
 */
STATIC INLINE void Chip_SDIF_SetBlkSize(LPC_SDMMC_T *pSDMMC, uint32_t bytes)
{
	pSDMMC->BLKSIZ = bytes;
}

/**
 * @brief	Reset card in slot
 * @param	pSDMMC	: SDMMC peripheral selected
 * @param	reset	: Sets SD_RST to passed state
 * @return	None
 * @note	Reset card in slot, must manually de-assert reset after assertion
 * (Uses SD_RST pin, set per reset parameter state)
 */
STATIC INLINE void Chip_SDIF_Reset(LPC_SDMMC_T *pSDMMC, int32_t reset)
{
	if (reset) {
		pSDMMC->RST_N = 1;
	}
	else {
		pSDMMC->RST_N = 0;
	}
}

/**
 * @brief	Detect if an SD card is inserted
 * @param	pSDMMC	: SDMMC peripheral selected
 * @return	Returns 0 if a card is detected, otherwise 1
 * @note	Detect if an SD card is inserted
 * (uses SD_CD pin, returns 0 on card detect)
 */
STATIC INLINE int32_t Chip_SDIF_CardNDetect(LPC_SDMMC_T *pSDMMC)
{
	return (pSDMMC->CDETECT & 1);
}

/**
 * @brief	Detect if write protect is enabled
 * @param	pSDMMC	: SDMMC peripheral selected
 * @return	Returns 1 if card is write protected, otherwise 0
 * @note	Detect if write protect is enabled
 * (uses SD_WP pin, returns 1 if card is write protected)
 */
STATIC INLINE int32_t Chip_SDIF_CardWpOn(LPC_SDMMC_T *pSDMMC)
{
	return (pSDMMC->WRTPRT & 1);
}

/**
 * @brief	Disable slot power
 * @param	pSDMMC	: SDMMC peripheral selected
 * @return	None
 * @note	Uses SD_POW pin, set to low.
 */
STATIC INLINE void Chip_SDIF_PowerOff(LPC_SDMMC_T *pSDMMC)
{
	pSDMMC->PWREN = 0;
}

/**
 * @brief	Enable slot power
 * @param	pSDMMC	: SDMMC peripheral selected
 * @return	None
 * @note	Uses SD_POW pin, set to high.
 */
STATIC INLINE void Chip_SDIF_PowerOn(LPC_SDMMC_T *pSDMMC)
{
	pSDMMC->PWREN = 1;
}

/**
 * @brief	Function to set card type
 * @param	pSDMMC	: SDMMC peripheral selected
 * @param	ctype	: card type
 * @return	None
 */
STATIC INLINE void Chip_SDIF_SetCardType(LPC_SDMMC_T *pSDMMC, uint32_t ctype)
{
	pSDMMC->CTYPE = ctype;
}

/**
 * @brief	Returns the raw SD interface interrupt status
 * @param	pSDMMC	: SDMMC peripheral selected
 * @return	Current pending interrupt status of Or'ed values MCI_INT_*
 */
STATIC INLINE uint32_t Chip_SDIF_GetIntStatus(LPC_SDMMC_T *pSDMMC)
{
	return pSDMMC->RINTSTS;
}

/**
 * @brief	Clears the raw SD interface interrupt status
 * @param	pSDMMC	: SDMMC peripheral selected
 * @param	iVal	: Interrupts to be cleared, Or'ed values MCI_INT_*
 * @return	None
 */
STATIC INLINE void Chip_SDIF_ClrIntStatus(LPC_SDMMC_T *pSDMMC, uint32_t iVal)
{
	pSDMMC->RINTSTS = iVal;
}

/**
 * @brief	Sets the SD interface interrupt mask
 * @param	pSDMMC	: SDMMC peripheral selected
 * @param	iVal	: Interrupts to enable, Or'ed values MCI_INT_*
 * @return	None
 */
STATIC INLINE void Chip_SDIF_SetIntMask(LPC_SDMMC_T *pSDMMC, uint32_t iVal)
{
	pSDMMC->INTMASK = iVal;
}

/**
 * @brief	Set block size and byte count for transfer
 * @param	pSDMMC	: SDMMC peripheral selected
 * @param	blk_size: block size and byte count in bytes
 * @return	None
 */
STATIC INLINE void Chip_SDIF_SetBlkSizeByteCnt(LPC_SDMMC_T *pSDMMC, uint32_t blk_size)
{
	pSDMMC->BLKSIZ = blk_size;
	pSDMMC->BYTCNT = blk_size;
}

/**
 * @brief	Set byte count for transfer
 * @param	pSDMMC	: SDMMC peripheral selected
 * @param	bytes	: block size and byte count in bytes
 * @return	None
 */
STATIC INLINE void Chip_SDIF_SetByteCnt(LPC_SDMMC_T *pSDMMC, uint32_t bytes)
{
	pSDMMC->BYTCNT = bytes;
}

/**
 * @brief	Initializes the SD/MMC card controller
 * @param	pSDMMC	: SDMMC peripheral selected
 * @return	None
 */
void Chip_SDIF_Init(LPC_SDMMC_T *pSDMMC);

/**
 * @brief	Shutdown the SD/MMC card controller
 * @param	pSDMMC	: SDMMC peripheral selected
 * @return	None
 */
void Chip_SDIF_DeInit(LPC_SDMMC_T *pSDMMC);

/**
 * @brief	Function to send command to Card interface unit (CIU)
 * @param	pSDMMC	: SDMMC peripheral selected
 * @param	cmd		: Command with all flags set
 * @param	arg		: Argument for the command
 * @return	TRUE on times-out, otherwise FALSE
 */
int32_t Chip_SDIF_SendCmd(LPC_SDMMC_T *pSDMMC, uint32_t cmd, uint32_t arg);

/**
 * @brief	Read the response from the last command
 * @param	pSDMMC	: SDMMC peripheral selected
 * @param	resp	: Pointer to response array to fill
 * @return	None
 */
void Chip_SDIF_GetResponse(LPC_SDMMC_T *pSDMMC, uint32_t *resp);

/**
 * @brief	Sets the SD bus clock speed
 * @param	pSDMMC	: SDMMC peripheral selected
 * @param	clk_rate	: Input clock rate into the IP block
 * @param	speed		: Desired clock speed to the card
 * @return	None
 */
void Chip_SDIF_SetClock(LPC_SDMMC_T *pSDMMC, uint32_t clk_rate, uint32_t speed);

/**
 * @brief	Function to clear interrupt & FIFOs
 * @param	pSDMMC	: SDMMC peripheral selected
 * @return	None
 */
void Chip_SDIF_SetClearIntFifo(LPC_SDMMC_T *pSDMMC);

/**
 * @brief	Setup DMA descriptors
 * @param	pSDMMC	: SDMMC peripheral selected
 * @param	psdif_dev	: SD interface device
 * @param	addr		: Address of buffer (source or destination)
 * @param	size		: size of buffer in bytes (64K max)
 * @return	None
 */
void Chip_SDIF_DmaSetup(LPC_SDMMC_T *pSDMMC, sdif_device *psdif_dev, uint32_t addr, uint32_t size);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __SDIF_18XX_43XX_H_ */

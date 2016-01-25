/*
 * @brief LPC18xx/43xx General Purpose DMA driver
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
 * intellectual property rights. NXP Semiconductors assumes no responsibility
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

#ifndef __GPDMA_18XX_43XX_H_
#define __GPDMA_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup GPDMA_18XX_43XX CHIP: LPC18xx/43xx General Purpose DMA driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief Number of channels on GPDMA
 */
#define GPDMA_NUMBER_CHANNELS 8

/**
 * @brief GPDMA Channel register block structure
 */
typedef struct {
	__IO uint32_t  SRCADDR;				/*!< DMA Channel Source Address Register */
	__IO uint32_t  DESTADDR;			/*!< DMA Channel Destination Address Register */
	__IO uint32_t  LLI;					/*!< DMA Channel Linked List Item Register */
	__IO uint32_t  CONTROL;				/*!< DMA Channel Control Register */
	__IO uint32_t  CONFIG;				/*!< DMA Channel Configuration Register */
	__I  uint32_t  RESERVED1[3];
} GPDMA_CH_T;

/**
 * @brief GPDMA register block
 */
typedef struct {						/*!< GPDMA Structure */
	__I  uint32_t  INTSTAT;				/*!< DMA Interrupt Status Register */
	__I  uint32_t  INTTCSTAT;			/*!< DMA Interrupt Terminal Count Request Status Register */
	__O  uint32_t  INTTCCLEAR;			/*!< DMA Interrupt Terminal Count Request Clear Register */
	__I  uint32_t  INTERRSTAT;			/*!< DMA Interrupt Error Status Register */
	__O  uint32_t  INTERRCLR;			/*!< DMA Interrupt Error Clear Register */
	__I  uint32_t  RAWINTTCSTAT;		/*!< DMA Raw Interrupt Terminal Count Status Register */
	__I  uint32_t  RAWINTERRSTAT;		/*!< DMA Raw Error Interrupt Status Register */
	__I  uint32_t  ENBLDCHNS;			/*!< DMA Enabled Channel Register */
	__IO uint32_t  SOFTBREQ;			/*!< DMA Software Burst Request Register */
	__IO uint32_t  SOFTSREQ;			/*!< DMA Software Single Request Register */
	__IO uint32_t  SOFTLBREQ;			/*!< DMA Software Last Burst Request Register */
	__IO uint32_t  SOFTLSREQ;			/*!< DMA Software Last Single Request Register */
	__IO uint32_t  CONFIG;				/*!< DMA Configuration Register */
	__IO uint32_t  SYNC;				/*!< DMA Synchronization Register */
	__I  uint32_t  RESERVED0[50];
	GPDMA_CH_T     CH[GPDMA_NUMBER_CHANNELS];
} LPC_GPDMA_T;

/**
 * @brief Macro defines for DMA channel control registers
 */
#define GPDMA_DMACCxControl_TransferSize(n) (((n & 0xFFF) << 0))	/*!< Transfer size*/
#define GPDMA_DMACCxControl_SBSize(n)       (((n & 0x07) << 12))	/*!< Source burst size*/
#define GPDMA_DMACCxControl_DBSize(n)       (((n & 0x07) << 15))	/*!< Destination burst size*/
#define GPDMA_DMACCxControl_SWidth(n)       (((n & 0x07) << 18))	/*!< Source transfer width*/
#define GPDMA_DMACCxControl_DWidth(n)       (((n & 0x07) << 21))	/*!< Destination transfer width*/
#define GPDMA_DMACCxControl_SI              ((1UL << 26))			/*!< Source increment*/
#define GPDMA_DMACCxControl_DI              ((1UL << 27))			/*!< Destination increment*/
#define GPDMA_DMACCxControl_SrcTransUseAHBMaster1   ((1UL << 24))	/*!< Source AHB master select in 18xx43xx*/
#define GPDMA_DMACCxControl_DestTransUseAHBMaster1  ((1UL << 25))	/*!< Destination AHB master select in 18xx43xx*/
#define GPDMA_DMACCxControl_Prot1           ((1UL << 28))			/*!< Indicates that the access is in user mode or privileged mode*/
#define GPDMA_DMACCxControl_Prot2           ((1UL << 29))			/*!< Indicates that the access is bufferable or not bufferable*/
#define GPDMA_DMACCxControl_Prot3           ((1UL << 30))			/*!< Indicates that the access is cacheable or not cacheable*/
#define GPDMA_DMACCxControl_I               ((1UL << 31))			/*!< Terminal count interrupt enable bit */

/**
 * @brief Macro defines for DMA Configuration register
 */
#define GPDMA_DMACConfig_E              ((0x01))	/*!< DMA Controller enable*/
#define GPDMA_DMACConfig_M              ((0x02))	/*!< AHB Master endianness configuration*/
#define GPDMA_DMACConfig_BITMASK        ((0x03))

/**
 * @brief Macro defines for DMA Channel Configuration registers
 */
#define GPDMA_DMACCxConfig_E                    ((1UL << 0))			/*!< DMA control enable*/
#define GPDMA_DMACCxConfig_SrcPeripheral(n)     (((n & 0x1F) << 1))		/*!< Source peripheral*/
#define GPDMA_DMACCxConfig_DestPeripheral(n)    (((n & 0x1F) << 6))		/*!< Destination peripheral*/
#define GPDMA_DMACCxConfig_TransferType(n)      (((n & 0x7) << 11))		/*!< This value indicates the type of transfer*/
#define GPDMA_DMACCxConfig_IE                   ((1UL << 14))			/*!< Interrupt error mask*/
#define GPDMA_DMACCxConfig_ITC                  ((1UL << 15))			/*!< Terminal count interrupt mask*/
#define GPDMA_DMACCxConfig_L                    ((1UL << 16))			/*!< Lock*/
#define GPDMA_DMACCxConfig_A                    ((1UL << 17))			/*!< Active*/
#define GPDMA_DMACCxConfig_H                    ((1UL << 18))			/*!< Halt*/

/**
 * @brief GPDMA Interrupt Clear Status
 */
typedef enum {
	GPDMA_STATCLR_INTTC,	/*!< GPDMA Interrupt Terminal Count Request Clear */
	GPDMA_STATCLR_INTERR	/*!< GPDMA Interrupt Error Clear */
} GPDMA_STATECLEAR_T;

/**
 * @brief GPDMA Type of Interrupt Status
 */
typedef enum {
	GPDMA_STAT_INT,			/*!< GPDMA Interrupt Status */
	GPDMA_STAT_INTTC,		/*!< GPDMA Interrupt Terminal Count Request Status */
	GPDMA_STAT_INTERR,		/*!< GPDMA Interrupt Error Status */
	GPDMA_STAT_RAWINTTC,	/*!< GPDMA Raw Interrupt Terminal Count Status */
	GPDMA_STAT_RAWINTERR,	/*!< GPDMA Raw Error Interrupt Status */
	GPDMA_STAT_ENABLED_CH	/*!< GPDMA Enabled Channel Status */
} GPDMA_STATUS_T;

/**
 * @brief GPDMA Type of DMA controller
 */
typedef enum {
	GPDMA_TRANSFERTYPE_M2M_CONTROLLER_DMA              = ((0UL)),	/*!< Memory to memory - DMA control */
	GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA              = ((1UL)),	/*!< Memory to peripheral - DMA control */
	GPDMA_TRANSFERTYPE_P2M_CONTROLLER_DMA              = ((2UL)),	/*!< Peripheral to memory - DMA control */
	GPDMA_TRANSFERTYPE_P2P_CONTROLLER_DMA              = ((3UL)),	/*!< Source peripheral to destination peripheral - DMA control */
	GPDMA_TRANSFERTYPE_P2P_CONTROLLER_DestPERIPHERAL   = ((4UL)),	/*!< Source peripheral to destination peripheral - destination peripheral control */
	GPDMA_TRANSFERTYPE_M2P_CONTROLLER_PERIPHERAL       = ((5UL)),	/*!< Memory to peripheral - peripheral control */
	GPDMA_TRANSFERTYPE_P2M_CONTROLLER_PERIPHERAL       = ((6UL)),	/*!< Peripheral to memory - peripheral control */
	GPDMA_TRANSFERTYPE_P2P_CONTROLLER_SrcPERIPHERAL    = ((7UL))	/*!< Source peripheral to destination peripheral - source peripheral control */
} GPDMA_FLOW_CONTROL_T;

/**
 * @brief GPDMA structure using for DMA configuration
 */
typedef struct {
	uint32_t ChannelNum;	/*!< DMA channel number, should be in
							 *  range from 0 to 7.
							 *  Note: DMA channel 0 has the highest priority
							 *  and DMA channel 7 the lowest priority.
							 */
	uint32_t TransferSize;	/*!< Length/Size of transfer */
	uint32_t TransferWidth;	/*!< Transfer width - used for TransferType is GPDMA_TRANSFERTYPE_M2M only */
	uint32_t SrcAddr;		/*!< Physical Source Address, used in case TransferType is chosen as
							 *   GPDMA_TRANSFERTYPE_M2M or GPDMA_TRANSFERTYPE_M2P */
	uint32_t DstAddr;		/*!< Physical Destination Address, used in case TransferType is chosen as
							 *   GPDMA_TRANSFERTYPE_M2M or GPDMA_TRANSFERTYPE_P2M */
	uint32_t TransferType;	/*!< Transfer Type, should be one of the following:
							 * - GPDMA_TRANSFERTYPE_M2M: Memory to memory - DMA control
							 * - GPDMA_TRANSFERTYPE_M2P: Memory to peripheral - DMA control
							 * - GPDMA_TRANSFERTYPE_P2M: Peripheral to memory - DMA control
							 * - GPDMA_TRANSFERTYPE_P2P: Source peripheral to destination peripheral - DMA control
							 */
} GPDMA_CH_CFG_T;

/**
 * @brief GPDMA request connections
 */
#define GPDMA_CONN_MEMORY           ((0UL))			/**< MEMORY             */
#define GPDMA_CONN_MAT0_0           ((1UL))			/**< MAT0.0             */
#define GPDMA_CONN_UART0_Tx         ((2UL))			/**< UART0 Tx           */
#define GPDMA_CONN_MAT0_1           ((3UL))			/**< MAT0.1             */
#define GPDMA_CONN_UART0_Rx         ((4UL))			/**< UART0 Rx           */
#define GPDMA_CONN_MAT1_0           ((5UL))			/**< MAT1.0             */
#define GPDMA_CONN_UART1_Tx         ((6UL))			/**< UART1 Tx           */
#define GPDMA_CONN_MAT1_1           ((7UL))			/**< MAT1.1             */
#define GPDMA_CONN_UART1_Rx         ((8UL))			/**< UART1 Rx           */
#define GPDMA_CONN_MAT2_0           ((9UL))			/**< MAT2.0             */
#define GPDMA_CONN_UART2_Tx         ((10UL))		/**< UART2 Tx           */
#define GPDMA_CONN_MAT2_1           ((11UL))		/**< MAT2.1             */
#define GPDMA_CONN_UART2_Rx         ((12UL))		/**< UART2 Rx           */
#define GPDMA_CONN_MAT3_0           ((13UL))		/**< MAT3.0             */
#define GPDMA_CONN_UART3_Tx         ((14UL))		/**< UART3 Tx           */
#define GPDMA_CONN_SCT_0            ((15UL))		/**< SCT timer channel 0*/
#define GPDMA_CONN_MAT3_1           ((16UL))		/**< MAT3.1             */
#define GPDMA_CONN_UART3_Rx         ((17UL))		/**< UART3 Rx           */
#define GPDMA_CONN_SCT_1            ((18UL))		/**< SCT timer channel 1*/
#define GPDMA_CONN_SSP0_Rx          ((19UL))		/**< SSP0 Rx            */
#define GPDMA_CONN_I2S_Tx_Channel_0 ((20UL))		/**< I2S0 Tx on channel 0 */
#define GPDMA_CONN_SSP0_Tx          ((21UL))		/**< SSP0 Tx            */
#define GPDMA_CONN_I2S_Rx_Channel_1 ((22UL))		/**< I2S0 Rx on channel 0 */
#define GPDMA_CONN_SSP1_Rx          ((23UL))		/**< SSP1 Rx            */
#define GPDMA_CONN_SSP1_Tx          ((24UL))		/**< SSP1 Tx            */
#define GPDMA_CONN_ADC_0            ((25UL))		/**< ADC 0              */
#define GPDMA_CONN_ADC_1            ((26UL))		/**< ADC 1              */
#define GPDMA_CONN_DAC              ((27UL))		/**< DAC                */
#define GPDMA_CONN_I2S1_Tx_Channel_0 ((28UL))		/**< I2S1 Tx on channel 0 */
#define GPDMA_CONN_I2S1_Rx_Channel_1 ((29UL))		/**< I2S1 Rx on channel 0 */

/**
 * @brief GPDMA Burst size in Source and Destination definitions
 */
#define GPDMA_BSIZE_1   ((0UL))	/*!< Burst size = 1 */
#define GPDMA_BSIZE_4   ((1UL))	/*!< Burst size = 4 */
#define GPDMA_BSIZE_8   ((2UL))	/*!< Burst size = 8 */
#define GPDMA_BSIZE_16  ((3UL))	/*!< Burst size = 16 */
#define GPDMA_BSIZE_32  ((4UL))	/*!< Burst size = 32 */
#define GPDMA_BSIZE_64  ((5UL))	/*!< Burst size = 64 */
#define GPDMA_BSIZE_128 ((6UL))	/*!< Burst size = 128 */
#define GPDMA_BSIZE_256 ((7UL))	/*!< Burst size = 256 */

/**
 * @brief Width in Source transfer width and Destination transfer width definitions
 */
#define GPDMA_WIDTH_BYTE        ((0UL))	/*!< Width = 1 byte */
#define GPDMA_WIDTH_HALFWORD    ((1UL))	/*!< Width = 2 bytes */
#define GPDMA_WIDTH_WORD        ((2UL))	/*!< Width = 4 bytes */

/**
 * @brief Flow control definitions
 */
#define DMA_CONTROLLER 0		/*!< Flow control is DMA controller*/
#define SRC_PER_CONTROLLER 1	/*!< Flow control is Source peripheral controller*/
#define DST_PER_CONTROLLER 2	/*!< Flow control is Destination peripheral controller*/

/**
 * @brief DMA channel handle structure
 */
typedef struct {
	FunctionalState ChannelStatus;	/*!< DMA channel status */
} DMA_ChannelHandle_t;

/**
 * @brief Transfer Descriptor structure typedef
 */
typedef struct DMA_TransferDescriptor {
	uint32_t src;	/*!< Source address */
	uint32_t dst;	/*!< Destination address */
	uint32_t lli;	/*!< Pointer to next descriptor structure */
	uint32_t ctrl;	/*!< Control word that has transfer size, type etc. */
} DMA_TransferDescriptor_t;

/**
 * @brief	Initialize the GPDMA
 * @param	pGPDMA	: The base of GPDMA on the chip
 * @return	Nothing
 */
void Chip_GPDMA_Init(LPC_GPDMA_T *pGPDMA);

/**
 * @brief	Shutdown the GPDMA
 * @param	pGPDMA	: The base of GPDMA on the chip
 * @return	Nothing
 */
void Chip_GPDMA_DeInit(LPC_GPDMA_T *pGPDMA);

/**
 * @brief	Initialize channel configuration strucutre
 * @param	pGPDMA			: The base of GPDMA on the chip
 * @param	GPDMACfg		: Pointer to configuration structure to be initialized
 * @param	ChannelNum		: Channel used for transfer *must be obtained using Chip_GPDMA_GetFreeChannel()*
 * @param	src				: Address of Memory or one of @link #GPDMA_CONN_MEMORY
 *                              PeripheralConnection_ID @endlink, which is the source
 * @param	dst				: Address of Memory or one of @link #GPDMA_CONN_MEMORY
 *                              PeripheralConnection_ID @endlink, which is the destination
 * @param	Size			: The number of DMA transfers
 * @param	TransferType	: Select the transfer controller and the type of transfer. (See, #GPDMA_FLOW_CONTROL_T)
 * @return	ERROR on error, SUCCESS on success
 */
int Chip_GPDMA_InitChannelCfg(LPC_GPDMA_T *pGPDMA,
							  GPDMA_CH_CFG_T *GPDMACfg,
							  uint8_t  ChannelNum,
							  uint32_t src,
							  uint32_t dst,
							  uint32_t Size,
							  GPDMA_FLOW_CONTROL_T TransferType);

/**
 * @brief	Enable or Disable the GPDMA Channel
 * @param	pGPDMA		: The base of GPDMA on the chip
 * @param	channelNum	: The GPDMA channel : 0 - 7
 * @param	NewState	: ENABLE to enable GPDMA or DISABLE to disable GPDMA
 * @return	Nothing
 */
void Chip_GPDMA_ChannelCmd(LPC_GPDMA_T *pGPDMA, uint8_t channelNum, FunctionalState NewState);

/**
 * @brief	Stop a stream DMA transfer
 * @param	pGPDMA		: The base of GPDMA on the chip
 * @param	ChannelNum	: Channel Number to be closed
 * @return	Nothing
 */
void Chip_GPDMA_Stop(LPC_GPDMA_T *pGPDMA, uint8_t ChannelNum);

/**
 * @brief	The GPDMA stream interrupt status checking
 * @param	pGPDMA		: The base of GPDMA on the chip
 * @param	ChannelNum	: Channel Number to be checked on interruption
 * @return	Status:
 *              - SUCCESS	: DMA transfer success
 *              - ERROR		: DMA transfer failed
 */
Status Chip_GPDMA_Interrupt(LPC_GPDMA_T *pGPDMA, uint8_t ChannelNum);

/**
 * @brief	Read the status from different registers according to the type
 * @param	pGPDMA	: The base of GPDMA on the chip
 * @param	type	: Status mode, should be:
 *						- GPDMA_STAT_INT		: GPDMA Interrupt Status
 *						- GPDMA_STAT_INTTC		: GPDMA Interrupt Terminal Count Request Status
 *						- GPDMA_STAT_INTERR		: GPDMA Interrupt Error Status
 *						- GPDMA_STAT_RAWINTTC	: GPDMA Raw Interrupt Terminal Count Status
 *						- GPDMA_STAT_RAWINTERR	: GPDMA Raw Error Interrupt Status
 *						- GPDMA_STAT_ENABLED_CH	: GPDMA Enabled Channel Status
 * @param	channel	: The GPDMA channel : 0 - 7
 * @return	SET is interrupt is pending or RESET if not pending
 */
IntStatus Chip_GPDMA_IntGetStatus(LPC_GPDMA_T *pGPDMA, GPDMA_STATUS_T type, uint8_t channel);

/**
 * @brief	Clear the Interrupt Flag from different registers according to the type
 * @param	pGPDMA	: The base of GPDMA on the chip
 * @param	type	: Flag mode, should be:
 *						- GPDMA_STATCLR_INTTC	: GPDMA Interrupt Terminal Count Request
 *						- GPDMA_STATCLR_INTERR	: GPDMA Interrupt Error
 * @param	channel	: The GPDMA channel : 0 - 7
 * @return	Nothing
 */
void Chip_GPDMA_ClearIntPending(LPC_GPDMA_T *pGPDMA, GPDMA_STATECLEAR_T type, uint8_t channel);

/**
 * @brief	Get a free GPDMA channel for one DMA connection
 * @param	pGPDMA					: The base of GPDMA on the chip
 * @param	PeripheralConnection_ID	: Some chip fix each peripheral DMA connection on a specified channel ( have not used in 17xx/40xx )
 * @return	The channel number which is selected
 */
uint8_t Chip_GPDMA_GetFreeChannel(LPC_GPDMA_T *pGPDMA,
								  uint32_t PeripheralConnection_ID);

/**
 * @brief	Do a DMA transfer M2M, M2P,P2M or P2P
 * @param	pGPDMA		: The base of GPDMA on the chip
 * @param	ChannelNum	: Channel used for transfer
 * @param	src			: Address of Memory or PeripheralConnection_ID which is the source
 * @param	dst			: Address of Memory or PeripheralConnection_ID which is the destination
 * @param	TransferType: Select the transfer controller and the type of transfer. Should be:
 *                               - GPDMA_TRANSFERTYPE_M2M_CONTROLLER_DMA
 *                               - GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA
 *                               - GPDMA_TRANSFERTYPE_P2M_CONTROLLER_DMA
 *                               - GPDMA_TRANSFERTYPE_P2P_CONTROLLER_DMA
 *                               - GPDMA_TRANSFERTYPE_P2P_CONTROLLER_DestPERIPHERAL
 *                               - GPDMA_TRANSFERTYPE_M2P_CONTROLLER_PERIPHERAL
 *                               - GPDMA_TRANSFERTYPE_P2M_CONTROLLER_PERIPHERAL
 *                               - GPDMA_TRANSFERTYPE_P2P_CONTROLLER_SrcPERIPHERAL
 * @param	Size		: The number of DMA transfers
 * @return	ERROR on error, SUCCESS on success
 */
Status Chip_GPDMA_Transfer(LPC_GPDMA_T *pGPDMA,
						   uint8_t ChannelNum,
						   uint32_t src,
						   uint32_t dst,
						   GPDMA_FLOW_CONTROL_T TransferType,
						   uint32_t Size);

/**
 * @brief	Do a DMA transfer using linked list of descriptors
 * @param	pGPDMA			: The base of GPDMA on the chip
 * @param	ChannelNum		: Channel used for transfer *must be obtained using Chip_GPDMA_GetFreeChannel()*
 * @param	DMADescriptor	: First node in the linked list of descriptors
 * @param	TransferType	: Select the transfer controller and the type of transfer. (See, #GPDMA_FLOW_CONTROL_T)
 * @return	ERROR on error, SUCCESS on success
 */
Status Chip_GPDMA_SGTransfer(LPC_GPDMA_T *pGPDMA,
							 uint8_t ChannelNum,
							 const DMA_TransferDescriptor_t *DMADescriptor,
							 GPDMA_FLOW_CONTROL_T TransferType);

/**
 * @brief	Prepare a single DMA descriptor
 * @param	pGPDMA			: The base of GPDMA on the chip
 * @param	DMADescriptor	: DMA Descriptor to be initialized
 * @param	src				: Address of Memory or one of @link #GPDMA_CONN_MEMORY
 *                              PeripheralConnection_ID @endlink, which is the source
 * @param	dst				: Address of Memory or one of @link #GPDMA_CONN_MEMORY
 *                              PeripheralConnection_ID @endlink, which is the destination
 * @param	Size			: The number of DMA transfers
 * @param	TransferType	: Select the transfer controller and the type of transfer. (See, #GPDMA_FLOW_CONTROL_T)
 * @param	NextDescriptor	: Pointer to next descriptor (0 if no more descriptors available)
 * @return	ERROR on error, SUCCESS on success
 */
Status Chip_GPDMA_PrepareDescriptor(LPC_GPDMA_T *pGPDMA,
									DMA_TransferDescriptor_t *DMADescriptor,
									uint32_t src,
									uint32_t dst,
									uint32_t Size,
									GPDMA_FLOW_CONTROL_T TransferType,
									const DMA_TransferDescriptor_t *NextDescriptor);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __GPDMA_18XX_43XX_H_ */

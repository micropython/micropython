/*
 * @brief LPC18xx/43xx I2S driver
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

#ifndef __I2S_18XX_43XX_H_
#define __I2S_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup I2S_18XX_43XX CHIP: LPC18xx/43xx I2S driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief I2S DMA request channel define
 */
typedef enum {
	I2S_DMA_REQUEST_CHANNEL_1,	/*!< DMA request channel 1 */
	I2S_DMA_REQUEST_CHANNEL_2,	/*!< DMA request channel 2 */
	I2S_DMA_REQUEST_CHANNEL_NUM,/*!< The number of DMA request channels */
} I2S_DMA_CHANNEL_T;

/**
 * @brief I2S register block structure
 */
typedef struct {				/*!< I2S Structure */
	__IO uint32_t DAO;			/*!< I2S Digital Audio Output Register. Contains control bits for the I2S transmit channel */
	__IO uint32_t DAI;			/*!< I2S Digital Audio Input Register. Contains control bits for the I2S receive channel */
	__O uint32_t TXFIFO;		/*!< I2S Transmit FIFO. Access register for the 8 x 32-bit transmitter FIFO */
	__I uint32_t RXFIFO;		/*!< I2S Receive FIFO. Access register for the 8 x 32-bit receiver FIFO */
	__I uint32_t STATE;			/*!< I2S Status Feedback Register. Contains status information about the I2S interface */
	__IO uint32_t DMA[I2S_DMA_REQUEST_CHANNEL_NUM];	/*!< I2S DMA Configuration Registers. Contains control information for DMA request channels */
	__IO uint32_t IRQ;			/*!< I2S Interrupt Request Control Register. Contains bits that control how the I2S interrupt request is generated */
	__IO uint32_t TXRATE;		/*!< I2S Transmit MCLK divider. This register determines the I2S TX MCLK rate by specifying the value to divide PCLK by in order to produce MCLK */
	__IO uint32_t RXRATE;		/*!< I2S Receive MCLK divider. This register determines the I2S RX MCLK rate by specifying the value to divide PCLK by in order to produce MCLK */
	__IO uint32_t TXBITRATE;	/*!< I2S Transmit bit rate divider. This register determines the I2S transmit bit rate by specifying the value to divide TX_MCLK by in order to produce the transmit bit clock */
	__IO uint32_t RXBITRATE;	/*!< I2S Receive bit rate divider. This register determines the I2S receive bit rate by specifying the value to divide RX_MCLK by in order to produce the receive bit clock */
	__IO uint32_t TXMODE;		/*!< I2S Transmit mode control */
	__IO uint32_t RXMODE;		/*!< I2S Receive mode control */
} LPC_I2S_T;

/*
 * @brief I2S configuration parameter defines
 */
/* I2S Wordwidth bit */
#define I2S_WORDWIDTH_8     (0UL << 0)	/*!< 8 bit Word */
#define I2S_WORDWIDTH_16    (1UL << 0)	/*!< 16 bit word */
#define I2S_WORDWIDTH_32    (3UL << 0)	/*!< 32 bit word */

/* I2S Channel bit */
#define I2S_STEREO          (0UL << 2)	/*!< Stereo audio */
#define I2S_MONO            (1UL << 2)	/*!< Mono audio */

/* I2S Master/Slave mode bit */
#define I2S_MASTER_MODE     (0UL << 5)	/*!< I2S in master mode */
#define I2S_SLAVE_MODE      (1UL << 5)	/*!< I2S in slave mode */

/* I2S Stop bit */
#define I2S_STOP_ENABLE     (0UL << 3)	/*!< I2S stop enable mask */
#define I2S_STOP_DISABLE    (1UL << 3)	/*!< I2S stop disable mask */

/* I2S Reset bit */
#define I2S_RESET_ENABLE    (1UL << 4)	/*!< I2S reset enable mask */
#define I2S_RESET_DISABLE   (0UL << 4)	/*!< I2S reset disable mask */

/* I2S Mute bit */
#define I2S_MUTE_ENABLE     (1UL << 15)	/*!< I2S mute enable mask */
#define I2S_MUTE_DISABLE    (0UL << 15)	/*!< I2S mute disbale mask */

/*
 * @brief Macro defines for DAO-Digital Audio Output register
 */
/* I2S wordwide - the number of bytes in data*/
#define I2S_DAO_WORDWIDTH_8     ((uint32_t) (0))	/*!< DAO 8 bit	*/
#define I2S_DAO_WORDWIDTH_16    ((uint32_t) (1))	/*!< DAO 16 bit	*/
#define I2S_DAO_WORDWIDTH_32    ((uint32_t) (3))	/*!< DAO 32 bit	*/
#define I2S_DAO_WORDWIDTH_MASK  ((uint32_t) (3))

/* I2S control mono or stereo format */
#define I2S_DAO_MONO            ((uint32_t) (1 << 2))	/*!< DAO mono audio mask */

/* I2S control stop mode */
#define I2S_DAO_STOP            ((uint32_t) (1 << 3))	/*!< DAO stop mask */

/* I2S control reset mode */
#define I2S_DAO_RESET           ((uint32_t) (1 << 4))	/*!< DAO reset mask */

/* I2S control master/slave mode */
#define I2S_DAO_SLAVE           ((uint32_t) (1 << 5))	/*!< DAO slave mode mask */

/* I2S word select half period minus one */
#define I2S_DAO_WS_HALFPERIOD(n)    ((uint32_t) (((n) & 0x1FF) << 6))	/*!< DAO Word select set macro */
#define I2S_DAO_WS_HALFPERIOD_MASK  ((uint32_t) ((0x1FF) << 6))		/*!< DAO Word select mask */

/* I2S control mute mode */
#define I2S_DAO_MUTE            ((uint32_t) (1 << 15))	/*!< DAO mute mask */

/*
 * @brief Macro defines for DAI-Digital Audio Input register
 */
/* I2S wordwide - the number of bytes in data*/
#define I2S_DAI_WORDWIDTH_8     ((uint32_t) (0))	/*!< DAI 8 bit	*/
#define I2S_DAI_WORDWIDTH_16    ((uint32_t) (1))	/*!< DAI 16 bit	*/
#define I2S_DAI_WORDWIDTH_32    ((uint32_t) (3))	/*!< DAI 32 bit	*/
#define I2S_DAI_WORDWIDTH_MASK  ((uint32_t) (3))	/*!< DAI word wide mask */

/* I2S control mono or stereo format */
#define I2S_DAI_MONO            ((uint32_t) (1 << 2))	/*!< DAI mono mode mask */

/* I2S control stop mode */
#define I2S_DAI_STOP            ((uint32_t) (1 << 3))	/*!< DAI stop bit mask */

/* I2S control reset mode */
#define I2S_DAI_RESET           ((uint32_t) (1 << 4))	/*!< DAI reset bit mask */

/* I2S control master/slave mode */
#define I2S_DAI_SLAVE           ((uint32_t) (1 << 5))	/*!< DAI slave mode mask */

/* I2S word select half period minus one (9 bits)*/
#define I2S_DAI_WS_HALFPERIOD(n)    ((uint32_t) (((n) & 0x1FF) << 6))	/*!< DAI Word select set macro */
#define I2S_DAI_WS_HALFPERIOD_MASK  ((uint32_t) ((0x1FF) << 6))		/*!< DAI Word select mask */

/*
 * @brief Macro defines for STAT register (Status Feedback register)
 */
#define I2S_STATE_IRQ       ((uint32_t) (1))/*!< I2S Status Receive or Transmit Interrupt */
#define I2S_STATE_DMA1      ((uint32_t) (1 << 1))	/*!< I2S Status Receive or Transmit DMA1 */
#define I2S_STATE_DMA2      ((uint32_t) (1 << 2))	/*!< I2S Status Receive or Transmit DMA2 */
#define I2S_STATE_RX_LEVEL(n)   ((uint32_t) ((n & 1F) << 8))/*!< I2S Status Current level of the Receive FIFO (5 bits)*/
#define I2S_STATE_TX_LEVEL(n)   ((uint32_t) ((n & 1F) << 16))	/*!< I2S Status Current level of the Transmit FIFO (5 bits)*/

/*
 * @brief Macro defines for DMA1 register (DMA1 Configuration register)
 */
#define I2S_DMA1_RX_ENABLE      ((uint32_t) (1))/*!< I2S control DMA1 for I2S receive */
#define I2S_DMA1_TX_ENABLE      ((uint32_t) (1 << 1))	/*!< I2S control DMA1 for I2S transmit */
#define I2S_DMA1_RX_DEPTH(n)    ((uint32_t) ((n & 0x1F) << 8))	/*!< I2S set FIFO level that trigger a receive DMA request on DMA1 */
#define I2S_DMA1_TX_DEPTH(n)    ((uint32_t) ((n & 0x1F) << 16))	/*!< I2S set FIFO level that trigger a transmit DMA request on DMA1 */

/*
 * @brief Macro defines for DMA2 register (DMA2 Configuration register)
 */
#define I2S_DMA2_RX_ENABLE      ((uint32_t) (1))/*!< I2S control DMA2 for I2S receive */
#define I2S_DMA2_TX_ENABLE      ((uint32_t) (1 << 1))	/*!< I2S control DMA1 for I2S transmit */
#define I2S_DMA2_RX_DEPTH(n)    ((uint32_t) ((n & 0x1F) << 8))	/*!< I2S set FIFO level that trigger a receive DMA request on DMA1 */
#define I2S_DMA2_TX_DEPTH(n)    ((uint32_t) ((n & 0x1F) << 16))	/*!< I2S set FIFO level that trigger a transmit DMA request on DMA1 */

/*
 * @brief Macro defines for IRQ register (Interrupt Request Control register)
 */

#define I2S_IRQ_RX_ENABLE       ((uint32_t) (1))/*!< I2S control I2S receive interrupt */
#define I2S_IRQ_TX_ENABLE       ((uint32_t) (1 << 1))	/*!< I2S control I2S transmit interrupt */
#define I2S_IRQ_RX_DEPTH(n)     ((uint32_t) ((n & 0x0F) << 8))	/*!< I2S set the FIFO level on which to create an irq request */
#define I2S_IRQ_RX_DEPTH_MASK   ((uint32_t) ((0x0F) << 8))
#define I2S_IRQ_TX_DEPTH(n)     ((uint32_t) ((n & 0x0F) << 16))	/*!< I2S set the FIFO level on which to create an irq request */
#define I2S_IRQ_TX_DEPTH_MASK   ((uint32_t) ((0x0F) << 16))

/*
 * @brief Macro defines for TXRATE/RXRATE register (Transmit/Receive Clock Rate register)
 */
#define I2S_TXRATE_Y_DIVIDER(n) ((uint32_t) (n & 0xFF))	/*!< I2S Transmit MCLK rate denominator */
#define I2S_TXRATE_X_DIVIDER(n) ((uint32_t) ((n & 0xFF) << 8))	/*!< I2S Transmit MCLK rate denominator */
#define I2S_RXRATE_Y_DIVIDER(n) ((uint32_t) (n & 0xFF))	/*!< I2S Receive MCLK rate denominator */
#define I2S_RXRATE_X_DIVIDER(n) ((uint32_t) ((n & 0xFF) << 8))	/*!< I2S Receive MCLK rate denominator */

/*
 * @brief Macro defines for TXBITRATE & RXBITRATE register (Transmit/Receive Bit Rate register)
 */
#define I2S_TXBITRATE(n)    ((uint32_t) (n & 0x3F))
#define I2S_RXBITRATE(n)    ((uint32_t) (n & 0x3F))

/*
 * @brief Macro defines for TXMODE/RXMODE register (Transmit/Receive Mode Control register)
 */
#define I2S_TXMODE_CLKSEL(n)    ((uint32_t) (n & 0x03))	/*!< I2S Transmit select clock source (2 bits)*/
#define I2S_TXMODE_4PIN_ENABLE  ((uint32_t) (1 << 2))	/*!< I2S Transmit control 4-pin mode */
#define I2S_TXMODE_MCENA        ((uint32_t) (1 << 3))	/*!< I2S Transmit control the TX_MCLK output */
#define I2S_RXMODE_CLKSEL(n)    ((uint32_t) (n & 0x03))	/*!< I2S Receive select clock source */
#define I2S_RXMODE_4PIN_ENABLE  ((uint32_t) (1 << 2))	/*!< I2S Receive control 4-pin mode */
#define I2S_RXMODE_MCENA        ((uint32_t) (1 << 3))	/*!< I2S Receive control the TX_MCLK output */

/**
 * @brief I2S Audio Format Structure
 */
typedef struct {
	uint32_t SampleRate;	/*!< Sample Rate */
	uint8_t ChannelNumber;	/*!< Channel Number - 1 is mono, 2 is stereo */
	uint8_t WordWidth;		/*!< Word Width - 8, 16 or 32 bits */
} I2S_AUDIO_FORMAT_T;

/**
 * @brief	Initialize for I2S
 * @param	pI2S	: The base of I2S peripheral on the chip
 * @return	Nothing
 */
void Chip_I2S_Init(LPC_I2S_T *pI2S);

/**
 * @brief	Shutdown I2S
 * @param	pI2S	: The base of I2S peripheral on the chip
 * @return	Nothing
 * @note	Reset all relative registers (DMA, transmit/receive control, interrupt) to default value
 */
void Chip_I2S_DeInit(LPC_I2S_T *pI2S);

/**
 * @brief	Send a 32-bit data to TXFIFO for transmition
 * @param	pI2S	: The base of I2S peripheral on the chip
 * @param	data	: Data to be transmited
 * @return	Nothing
 * @note	The function writes to TXFIFO without checking any condition.
 */
STATIC INLINE void Chip_I2S_Send(LPC_I2S_T *pI2S, uint32_t data)
{
	pI2S->TXFIFO = data;
}

/**
 * @brief	Get received data from RXFIFO
 * @param	pI2S	: The base of I2S peripheral on the chip
 * @return	Data received in RXFIFO
 * @note	The function reads from RXFIFO without checking any condition.
 */
STATIC INLINE uint32_t Chip_I2S_Receive(LPC_I2S_T *pI2S)
{
	return pI2S->RXFIFO;
}

/**
 * @brief	Start transmit data
 * @param	pI2S	: The base of I2S peripheral on the chip
 * @return	Nothing
 */
STATIC INLINE void Chip_I2S_TxStart(LPC_I2S_T *pI2S)
{
	pI2S->DAO &= ~(I2S_DAO_RESET | I2S_DAO_STOP | I2S_DAO_MUTE);
}

/**
 * @brief	Start receive data
 * @param	pI2S	: The base of I2S peripheral on the chip
 * @return	Nothing
 */
STATIC INLINE void Chip_I2S_RxStart(LPC_I2S_T *pI2S)
{
	pI2S->DAI &= ~(I2S_DAI_RESET | I2S_DAI_STOP);
}

/**
 * @brief	Disables accesses on FIFOs, places the transmit channel in mute mode
 * @param	pI2S	: The base of I2S peripheral on the chip
 * @return	Nothing
 */
STATIC INLINE void Chip_I2S_TxPause(LPC_I2S_T *pI2S)
{
	pI2S->DAO |= I2S_DAO_STOP;
}

/**
 * @brief	Disables accesses on FIFOs, places the transmit channel in mute mode
 * @param	pI2S	: The base of I2S peripheral on the chip
 * @return	Nothing
 */
STATIC INLINE void Chip_I2S_RxPause(LPC_I2S_T *pI2S)
{
	pI2S->DAI |= I2S_DAI_STOP;
}

/**
 * @brief	Mute the Transmit channel
 * @param	pI2S		: The base of I2S peripheral on the chip
 * @return	Nothing
 * @note	The data output from I2S transmit channel is always zeroes
 */
STATIC INLINE void Chip_I2S_EnableMute(LPC_I2S_T *pI2S)
{
	pI2S->DAO |= I2S_DAO_MUTE;
}

/**
 * @brief	Un-Mute the I2S channel
 * @param	pI2S		: The base of I2S peripheral on the chip
 * @return	Nothing
 */
STATIC INLINE void Chip_I2S_DisableMute(LPC_I2S_T *pI2S)
{
	pI2S->DAO &= ~I2S_DAO_MUTE;
}

/**
 * @brief	Stop I2S asynchronously
 * @param	pI2S	: The base of I2S peripheral on the chip
 * @return	Nothing
 * @note	Pause, resets the transmit channel and FIFO asynchronously
 */
STATIC INLINE void Chip_I2S_TxStop(LPC_I2S_T *pI2S)
{
	pI2S->DAO &= ~I2S_DAO_MUTE;
	pI2S->DAO |= I2S_DAO_STOP | I2S_DAO_RESET;
}

/**
 * @brief	Stop I2S asynchronously
 * @param	pI2S	: The base of I2S peripheral on the chip
 * @return	Nothing
 * @note	Pause, resets the transmit channel and FIFO asynchronously
 */
STATIC INLINE void Chip_I2S_RxStop(LPC_I2S_T *pI2S)
{
	pI2S->DAI |= I2S_DAI_STOP | I2S_DAI_RESET;
}

/**
 * @brief	Sets the I2S receive channel in slave mode
 * @param	pI2S	: The base of I2S peripheral on the chip
 * @return	Nothing
 * @note	4 pin mode must be enabled on appropriate channel.
 * Must be called after each Chip_I2S_TxModeConfig call if
 * slave mode is needed.
 */
STATIC INLINE void Chip_I2S_RxSlave(LPC_I2S_T *pI2S)
{
	pI2S->DAI |= I2S_SLAVE_MODE;
}

/**
 * @brief	Sets the I2S transmit channel in slave mode
 * @param	pI2S	: The base of I2S peripheral on the chip
 * @return	Nothing
 * @note	4 pin mode must be enabled on appropriate channel.
 * Must be called after each Chip_I2S_TxModeConfig call if
 * slave mode is needed.
 */
STATIC INLINE void Chip_I2S_TxSlave(LPC_I2S_T *pI2S)
{
	pI2S->DAO |= I2S_SLAVE_MODE;
}

/**
 * @brief	Set the I2S transmit mode
 * @param	pI2S	: The base of I2S peripheral on the chip
 * @param	clksel	: Clock source selection for the receive bit clock divider
 * @param	fpin	: Receive 4-pin mode selection
 * @param	mcena	: Enable for the RX_MCLK output
 * @return	Nothing
 * @note	In addition to master and slave modes, which are independently configurable for
 * the transmitter and the receiver, several different clock sources are possible,
 * including variations that share the clock and/or WS between the transmitter and
 * receiver. It also allows using I2S with fewer pins, typically four.
 */
STATIC INLINE void Chip_I2S_TxModeConfig(LPC_I2S_T *pI2S,
										 uint32_t clksel,
										 uint32_t fpin,
										 uint32_t mcena)
{
	pI2S->TXMODE = clksel | fpin | mcena;
}

/**
 * @brief	Set the I2S receive mode
 * @param	pI2S	: The base of I2S peripheral on the chip
 * @param	clksel	: Clock source selection for the receive bit clock divider
 * @param	fpin	: Receive 4-pin mode selection
 * @param	mcena	: Enable for the RX_MCLK output
 * @return	Nothing
 * @note	In addition to master and slave modes, which are independently configurable for
 * the transmitter and the receiver, several different clock sources are possible,
 * including variations that share the clock and/or WS between the transmitter and
 * receiver. It also allows using I2S with fewer pins, typically four.
 */
STATIC INLINE void Chip_I2S_RxModeConfig(LPC_I2S_T *pI2S,
										 uint32_t clksel,
										 uint32_t fpin,
										 uint32_t mcena)
{
	pI2S->RXMODE = clksel | fpin | mcena;
}

/**
 * @brief	Get the current level of the Transmit FIFO
 * @param	pI2S	: The base of I2S peripheral on the chip
 * @return	Current level of the Transmit FIFO
 */
STATIC INLINE uint8_t Chip_I2S_GetTxLevel(LPC_I2S_T *pI2S)
{
	return (pI2S->STATE >> 16) & 0xF;
}

/**
 * @brief	Get the current level of the Receive FIFO
 * @param	pI2S	: The base of I2S peripheral on the chip
 * @return	Current level of the Receive FIFO
 */
STATIC INLINE uint8_t Chip_I2S_GetRxLevel(LPC_I2S_T *pI2S)
{
	return (pI2S->STATE >> 8) & 0xF;
}

/**
 * @brief	Set the clock frequency for I2S interface
 * @param	pI2S			: The base of I2S peripheral on the chip
 * @param	div	: Clock divider. This value plus one is used to divide MCLK to produce the clock frequency for I2S interface
 * @return	Nothing
 * @note	The value depends on the audio sample rate desired and the data size and format(stereo/mono) used.
 * For example, a 48 kHz sample rate for 16-bit stereo data requires a bit rate of 48 000 x 16 x 2 = 1.536 MHz. So the mclk_divider should be MCLK/1.536 MHz
 */
STATIC INLINE void Chip_I2S_SetTxBitRate(LPC_I2S_T *pI2S, uint32_t div)
{
	pI2S->TXBITRATE = div;
}

/**
 * @brief	Set the clock frequency for I2S interface
 * @param	pI2S			: The base of I2S peripheral on the chip
 * @param	div	: Clock divider. This value plus one is used to divide MCLK to produce the clock frequency for I2S interface
 * @return	Nothing
 * @note	The value depends on the audio sample rate desired and the data size and format(stereo/mono) used.
 * For example, a 48 kHz sample rate for 16-bit stereo data requires a bit rate of 48 000 x 16 x 2 = 1.536 MHz. So the mclk_divider should be MCLK/1.536 MHz
 */
STATIC INLINE void Chip_I2S_SetRxBitRate(LPC_I2S_T *pI2S, uint32_t div)
{
	pI2S->RXBITRATE = div;
}

/**
 * @brief	Set the MCLK rate by using a fractional rate generator, dividing down the frequency of PCLK
 * @param	pI2S		: The base of I2S peripheral on the chip
 * @param	xDiv	: I2S transmit MCLK rate numerator
 * @param	yDiv	: I2S transmit MCLK rate denominator
 * @return	Nothing
 * @note	Values of the numerator (X) and the denominator (Y) must be chosen to
 * produce a frequency twice that desired for the transmitter MCLK, which
 * must be an integer multiple of the transmitter bit clock rate.
 * The equation for the fractional rate generator is:
 * MCLK = PCLK * (X/Y) /2
 * Note: If the value of X or Y is 0, then no clock is generated. Also, the value of Y must be
 * greater than or equal to X.
 */
STATIC INLINE void Chip_I2S_SetTxXYDivider(LPC_I2S_T *pI2S, uint8_t xDiv, uint8_t yDiv)
{
	pI2S->TXRATE = yDiv | (xDiv << 8);
}

/**
 * @brief	Set the MCLK rate by using a fractional rate generator, dividing down the frequency of PCLK
 * @param	pI2S		: The base of I2S peripheral on the chip
 * @param	xDiv	: I2S transmit MCLK rate numerator
 * @param	yDiv	: I2S transmit MCLK rate denominator
 * @return	Nothing
 * @note	Values of the numerator (X) and the denominator (Y) must be chosen to
 * produce a frequency twice that desired for the transmitter MCLK, which
 * must be an integer multiple of the transmitter bit clock rate.
 * The equation for the fractional rate generator is:
 * MCLK = PCLK * (X/Y) /2
 * Note: If the value of X or Y is 0, then no clock is generated. Also, the value of Y must be
 * greater than or equal to X.
 */
STATIC INLINE void Chip_I2S_SetRxXYDivider(LPC_I2S_T *pI2S, uint8_t xDiv, uint8_t yDiv)
{
	pI2S->RXRATE = yDiv | (xDiv << 8);
}

/**
 * @brief   Configure I2S for Audio Format input
 * @param	pI2S			: The base I2S peripheral on the chip
 * @param	format	: Audio Format
 * @return	SUCCESS or ERROR
 */
Status Chip_I2S_TxConfig(LPC_I2S_T *pI2S, I2S_AUDIO_FORMAT_T *format);

/**
 * @brief   Configure I2S for Audio Format input
 * @param	pI2S			: The base I2S peripheral on the chip
 * @param	format	: Audio Format
 * @return	SUCCESS or ERROR
 */
Status Chip_I2S_RxConfig(LPC_I2S_T *pI2S, I2S_AUDIO_FORMAT_T *format);

/**
 * @brief   Enable/Disable Interrupt with a specific FIFO depth
 * @param	pI2S			: The base I2S peripheral on the chip
 * @param	newState		: ENABLE or DISABLE interrupt
 * @param	depth		: FIFO level creating an irq request
 * @return	Nothing
 */
void Chip_I2S_Int_TxCmd(LPC_I2S_T *pI2S, FunctionalState newState, uint8_t depth);

/**
 * @brief   Enable/Disable Interrupt with a specific FIFO depth
 * @param	pI2S			: The base I2S peripheral on the chip
 * @param	newState		: ENABLE or DISABLE interrupt
 * @param	depth		: FIFO level creating an irq request
 * @return	Nothing
 */
void Chip_I2S_Int_RxCmd(LPC_I2S_T *pI2S, FunctionalState newState, uint8_t depth);

/**
 * @brief   Enable/Disable DMA with a specific FIFO depth
 * @param	pI2S			: The base I2S peripheral on the chip
 * @param	dmaNum			: Should be
 *								- I2S_DMA_REQUEST_CHANNEL_1 : Using DMA1
 *								- I2S_DMA_REQUEST_CHANNEL_2 : Using DMA2
 * @param	newState		: ENABLE or DISABLE interrupt
 * @param	depth		: FIFO level creating an irq request
 * @return	Nothing
 */
void Chip_I2S_DMA_TxCmd(LPC_I2S_T *pI2S, I2S_DMA_CHANNEL_T dmaNum, FunctionalState newState, uint8_t depth);

/**
 * @brief   Enable/Disable DMA with a specific FIFO depth
 * @param	pI2S			: The base I2S peripheral on the chip
 * @param	dmaNum			: Should be
 *								- I2S_DMA_REQUEST_CHANNEL_1 : Using DMA1
 *								- I2S_DMA_REQUEST_CHANNEL_2 : Using DMA2
 * @param	newState		: ENABLE or DISABLE interrupt
 * @param	depth		: FIFO level creating an irq request
 * @return	Nothing
 */
void Chip_I2S_DMA_RxCmd(LPC_I2S_T *pI2S, I2S_DMA_CHANNEL_T dmaNum, FunctionalState newState, uint8_t depth);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __I2S_18XX_43XX_H_ */

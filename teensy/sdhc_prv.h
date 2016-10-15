/*
 * WMXZ Teensy uSDFS library
 * Copyright (c) 2016 Walter Zimmer.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
//sdhc_prv.h
#ifndef _SDHC_PRV_H
#define _SDHC_PRV_H
/******************************************************************************
* Includes
******************************************************************************/

/******************************************************************************
* Constants
******************************************************************************/
#define IO_SDHC_ATTRIBS (IO_DEV_ATTR_READ | IO_DEV_ATTR_REMOVE | IO_DEV_ATTR_SEEK | IO_DEV_ATTR_WRITE | IO_DEV_ATTR_BLOCK_MODE)

#define SDHC_XFERTYP_RSPTYP_NO              (0x00)
#define SDHC_XFERTYP_RSPTYP_136             (0x01)
#define SDHC_XFERTYP_RSPTYP_48              (0x02)
#define SDHC_XFERTYP_RSPTYP_48BUSY          (0x03)

#define SDHC_XFERTYP_CMDTYP_ABORT           (0x03)

#define SDHC_PROCTL_EMODE_INVARIANT         (0x02)

#define SDHC_PROCTL_DTW_1BIT                (0x00)
#define SDHC_PROCTL_DTW_4BIT                (0x01)
#define SDHC_PROCTL_DTW_8BIT                (0x10)


#define SDHC_INITIALIZATION_MAX_CNT 100000

/******************************************************************************
* Macros 
******************************************************************************/

#define SDHC_IRQSTATEN_MASK (\
               SDHC_IRQSTATEN_DMAESEN | SDHC_IRQSTATEN_AC12ESEN |	\
               SDHC_IRQSTATEN_DEBESEN | SDHC_IRQSTATEN_DCESEN |	\
               SDHC_IRQSTATEN_DTOESEN | SDHC_IRQSTATEN_CIESEN |	\
               SDHC_IRQSTATEN_CEBESEN | SDHC_IRQSTATEN_CCESEN |	\
               SDHC_IRQSTATEN_CTOESEN | SDHC_IRQSTATEN_BRRSEN |	\
               SDHC_IRQSTATEN_BWRSEN  | SDHC_IRQSTATEN_DINTSEN |	\
               SDHC_IRQSTATEN_TCSEN | SDHC_IRQSTATEN_CCSEN)

#define  SDHC_IRQSTAT_CMD_ERROR (	\
               SDHC_IRQSTAT_CIE | SDHC_IRQSTAT_CEBE |	\
               SDHC_IRQSTAT_CCE | SDHC_IRQSTAT_CTOE)

#define SDHC_IRQSTAT_DATA_ERROR ( \
               SDHC_IRQSTAT_AC12E | SDHC_IRQSTAT_DEBE | \
               SDHC_IRQSTAT_DCE | SDHC_IRQSTAT_DTOE)

#define  SDHC_IRQSTAT_ERROR ( \
               SDHC_IRQSTAT_DMAE | SDHC_IRQSTAT_CMD_ERROR |	\
               SDHC_IRQSTAT_DATA_ERROR)

#define  SDHC_IRQSIGEN_MASK ( \
               SDHC_IRQSIGEN_DMAEIEN | SDHC_IRQSIGEN_AC12EIEN | \
               SDHC_IRQSIGEN_DEBEIEN | SDHC_IRQSIGEN_DCEIEN | \
               SDHC_IRQSIGEN_DTOEIEN | SDHC_IRQSIGEN_CIEIEN | \
               SDHC_IRQSIGEN_CEBEIEN | SDHC_IRQSIGEN_CCEIEN | \
               SDHC_IRQSIGEN_CTOEIEN | SDHC_IRQSIGEN_TCIEN)

//-------------------------------------------------------------------------------------------
#define SDHC_CMD_RESP_NONE  (SDHC_XFERTYP_RSPTYP(0))
#define SDHC_CMD_RESP_R1	(SDHC_XFERTYP_CICEN | SDHC_XFERTYP_CCCEN | SDHC_XFERTYP_RSPTYP(2))
#define SDHC_CMD_RESP_R1b	(SDHC_XFERTYP_CICEN | SDHC_XFERTYP_CCCEN | SDHC_XFERTYP_RSPTYP(3))
#define SDHC_CMD_RESP_R2	(SDHC_XFERTYP_CCCEN | SDHC_XFERTYP_RSPTYP(1))
#define SDHC_CMD_RESP_R3	(SDHC_XFERTYP_RSPTYP(2))
#define SDHC_CMD_RESP_R6	(SDHC_CMD_RESP_R1)
#define SDHC_CMD_RESP_R7	(SDHC_CMD_RESP_R1)

#define SDHC_DATA_READ	(SDHC_XFERTYP_DTDSEL | SDHC_XFERTYP_DPSEL)
#define SDHC_DATA_WRITE	(SDHC_XFERTYP_DPSEL)

#define SDHC_MULTI	(SDHC_XFERTYP_MSBSEL | SDHC_XFERTYP_AC12EN | SDHC_XFERTYP_BCEN)
#define SDHC_DATA_READ_MULTI	(SDHC_DATA_READ | SDHC_MULTI)
#define SDHC_DATA_WRITE_MULTI	(SDHC_DATA_WRITE | SDHC_MULTI)
											
#define SDHC_ACMD6_XFERTYP 	(SDHC_XFERTYP_CMDINX(SDHC_ACMD6) | SDHC_CMD_RESP_R1)
#define SDHC_ACMD41_XFERTYP (SDHC_XFERTYP_CMDINX(SDHC_ACMD41) | SDHC_CMD_RESP_R3)

#define SDHC_CMD0_XFERTYP	(SDHC_XFERTYP_CMDINX(SDHC_CMD0) | SDHC_CMD_RESP_NONE)
#define SDHC_CMD2_XFERTYP	(SDHC_XFERTYP_CMDINX(SDHC_CMD2) | SDHC_CMD_RESP_R2)
#define SDHC_CMD3_XFERTYP	(SDHC_XFERTYP_CMDINX(SDHC_CMD3) | SDHC_CMD_RESP_R6)
#define SDHC_CMD7_XFERTYP	(SDHC_XFERTYP_CMDINX(SDHC_CMD7) | SDHC_CMD_RESP_R1b)
#define SDHC_CMD8_XFERTYP	(SDHC_XFERTYP_CMDINX(SDHC_CMD8) | SDHC_CMD_RESP_R7)
#define SDHC_CMD9_XFERTYP	(SDHC_XFERTYP_CMDINX(SDHC_CMD9) | SDHC_CMD_RESP_R2)
#define SDHC_CMD10_XFERTYP	(SDHC_XFERTYP_CMDINX(SDHC_CMD10) | SDHC_CMD_RESP_R2)
#define SDHC_CMD12_XFERTYP	(SDHC_XFERTYP_CMDINX(SDHC_CMD12) | SDHC_CMD_RESP_R1 | \
                             SDHC_XFERTYP_CMDTYP(SDHC_XFERTYP_CMDTYP_ABORT) )
#define SDHC_CMD13_XFERTYP	(SDHC_XFERTYP_CMDINX(SDHC_CMD13) | SDHC_CMD_RESP_R1)
#define SDHC_CMD16_XFERTYP	(SDHC_XFERTYP_CMDINX(SDHC_CMD16) | SDHC_CMD_RESP_R1)
#define SDHC_CMD32_XFERTYP	(SDHC_XFERTYP_CMDINX(SDHC_CMD32) | SDHC_CMD_RESP_R1)
#define SDHC_CMD33_XFERTYP	(SDHC_XFERTYP_CMDINX(SDHC_CMD33) | SDHC_CMD_RESP_R1)
#define SDHC_CMD38_XFERTYP	(SDHC_XFERTYP_CMDINX(SDHC_CMD38) | SDHC_CMD_RESP_R1b)
#define SDHC_CMD55_XFERTYP	(SDHC_XFERTYP_CMDINX(SDHC_CMD55) | SDHC_CMD_RESP_R1)

#define SDHC_CMD6_BASE	(SDHC_XFERTYP_CMDINX(SDHC_CMD6) | SDHC_CMD_RESP_R1 | SDHC_DATA_READ)
#define SDHC_CMD17_BASE	(SDHC_XFERTYP_CMDINX(SDHC_CMD17) | SDHC_CMD_RESP_R1 | SDHC_DATA_READ)
#define SDHC_CMD18_BASE	(SDHC_XFERTYP_CMDINX(SDHC_CMD18) | SDHC_CMD_RESP_R1 | SDHC_DATA_READ_MULTI)
#define SDHC_CMD24_BASE	(SDHC_XFERTYP_CMDINX(SDHC_CMD24) | SDHC_CMD_RESP_R1 | SDHC_DATA_WRITE)
#define SDHC_CMD25_BASE	(SDHC_XFERTYP_CMDINX(SDHC_CMD25) | SDHC_CMD_RESP_R1 | SDHC_DATA_WRITE_MULTI)

#define SDHC_CMD6_XFERTYP	(SDHC_CMD6_BASE | SDHC_XFERTYP_DMAEN)

#if SDHC_TRANSFERTYPE == SDHC_TRANSFERTYPE_DMA
	#define SDHC_CMD17_XFERTYP	(SDHC_CMD17_BASE | SDHC_XFERTYP_DMAEN)
	#define SDHC_CMD18_XFERTYP	(SDHC_CMD18_BASE | SDHC_XFERTYP_DMAEN)
	#define SDHC_CMD24_XFERTYP	(SDHC_CMD24_BASE | SDHC_XFERTYP_DMAEN)
	#define SDHC_CMD25_XFERTYP	(SDHC_CMD25_BASE | SDHC_XFERTYP_DMAEN)
#else
	#define SDHC_CMD17_XFERTYP	(SDHC_CMD17_BASE)
	#define SDHC_CMD18_XFERTYP	(SDHC_CMD18_BASE)
	#define SDHC_CMD24_XFERTYP	(SDHC_CMD24_BASE)
	#define SDHC_CMD25_XFERTYP	(SDHC_CMD25_BASE)
#endif

#define CARD_STATUS_READY_FOR_DATA	(1UL << 8)

/******************************************************************************
* Types
******************************************************************************/


/******************************************************************************
* Private variables
******************************************************************************/
uint32_t m_sdhc_baudrate;
//uint32_t m_sdhc_dma_status = 1;
uint32_t m_sdhc_irqstat;
uint8_t m_sdhc_CMD6_Status[64];
uint32_t m_sdhc_ocr;

uint32_t m_sdhc_error;
volatile uint16_t m_sdhc_dmaDone = 1;
uint16_t  m_sdhc_waitCmd13 = 0;

/******************************************************************************
* Private functions
******************************************************************************/

static DSTATUS sdhc_Init(void);
static void sdhc_InitGPIO(Word init);
static DRESULT sdhc_SetBaudrate(uint32_t kbaudrate);

#if SDHC_TRANSFERTYPE == SDHC_TRANSFERTYPE_SWPOLL 
static DRESULT sdhc_ReadBlock(LWord* pData, LWord count, LWord size);
static DRESULT sdhc_WriteBlock(const LWord* pData, LWord Count, LWord Size);
#endif

//static uint16_t sdhc_WaitStatus(LWord mask);
static uint16_t sdhc_CMD(LWord xfertype, LWord arg);

static uint32_t sdhc_waitCommandReady(void);
static uint16_t sdhc_isBusy(void) ;
static uint16_t sdhc_waitDma(void) ;
static void sdhc_enableDma(void) ;

static uint16_t sdhc_CMD6_Switch(uint32_t arg, uint8_t* status);
static DRESULT sdhc_CMD12_StopTransferWaitForBusy(void);

#endif

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
//sdhc.c
// following code is modified by Walter Zimmer from
// from version provided by
// Petr Gargulak (NXP Employee) 
//https://community.nxp.com/servlet/JiveServlet/download/339474-1-263510/SDHC_K60_Baremetal.ZIP
//see also
//https://community.nxp.com/thread/99202


#include "kinetis.h"
#include "core_pins.h" // testing only

// Missing in Teensyduino 1.29
#ifndef MPU_CESR_VLD_MASK
#define MPU_CESR_VLD_MASK         0x1u
#endif
#ifndef SDHC_PROCTL_DTW_MASK
#define SDHC_PROCTL_DTW_MASK      0x6u
#endif
#ifndef SDHC_SYSCTL_DVS_MASK
#define SDHC_SYSCTL_DVS_MASK      0xF0u
#endif
#ifndef SDHC_SYSCTL_SDCLKFS_MASK
#define SDHC_SYSCTL_SDCLKFS_MASK  0xFF00u
#endif
#ifndef SDHC_SYSCTL_DTOCV_MASK
#define SDHC_SYSCTL_DTOCV_MASK    0xF0000u
#endif

#include "sdhc.h"
#include "sdhc_prv.h"

#define USB_DEBUG
#undef USB_DEBUG
#ifdef USB_DEBUG
/* some aux functions for pure c code */
#include "usb_serial.h"
void logg(char c) {usb_serial_putchar(c); usb_serial_flush_output();}
void printb(uint32_t x)
{ char c;
  int ii;
  for(ii=31;ii>=0;ii--)
  { if(!((ii+1)%4)) usb_serial_putchar(' ');
    c=(x&1<<ii)?'1':'0'; usb_serial_putchar(c);
  }
  usb_serial_putchar('\r');
  usb_serial_putchar('\n');
  usb_serial_flush_output();
}
/* end aux functions */
#endif

SD_CARD_DESCRIPTOR sdCardDesc;

/******************************************************************************
*
*   Public functions
*
******************************************************************************/
DSTATUS SDHC_GetStatus(void) {  return sdCardDesc.status;}

uint32_t SDHC_Baudrate(void) { return m_sdhc_baudrate; }

LWord SDHC_GetBlockCnt(void) { if(sdCardDesc.status)  return 0;  return sdCardDesc.numBlocks; }

uint16_t SDHC_DMADone(void)
{ 
#if SDHC_TRANSFERTYPE == SDHC_TRANSFERTYPE_SWPOLL 
	return 1;
#elif SDHC_TRANSFERTYPE == SDHC_TRANSFERTYPE_DMA 
	return  m_sdhc_dmaDone;
#endif
}
void SDHC_DMAWait(void) 
{ 
#if SDHC_TRANSFERTYPE == SDHC_TRANSFERTYPE_DMA 
	while(!SDHC_DMADone()) { /*yield() */; }
#endif
}

//-----------------------------------------------------------------------------
// FUNCTION:    SDHC_InitCard
// RETURNS:     status of initialization(OK, nonInit, noCard, CardProtected)
//-----------------------------------------------------------------------------  
DSTATUS SDHC_InitCard(void)
{
	DSTATUS resS;
	//DRESULT resR;
	LWord ii;
	uint32_t kbaudrate;

	resS = sdhc_Init();

	sdCardDesc.status = resS;
	sdCardDesc.address = 0;
	sdCardDesc.highCapacity = 0;
	sdCardDesc.version2 = 0;
	sdCardDesc.numBlocks = 0;

	if(resS) return resS;

	if(!sdhc_CMD(SDHC_CMD0_XFERTYP, 0)) SDHC_ERROR(STA_NOINIT, SD_CARD_ERROR_CMD0);

	if(sdhc_CMD(SDHC_CMD8_XFERTYP, 0X1AA)) // 3.3V and AA check pattern
	{
		if (SDHC_CMDRSP0 != 0X1AA) SDHC_ERROR(STA_NOINIT, SD_CARD_ERROR_CMD8);
		sdCardDesc.version2 = 1;
	}

	uint32_t arg = sdCardDesc.version2 ? 0X40300000 : 0x00300000;
	ii = SDHC_INITIALIZATION_MAX_CNT;
	do {
		if(!(sdhc_CMD(SDHC_CMD55_XFERTYP,0) && sdhc_CMD(SDHC_ACMD41_XFERTYP,arg)) || !ii ) 
			SDHC_ERROR(STA_NOINIT, SD_CARD_ERROR_ACMD41);
	} while ((SDHC_CMDRSP0 & 0x80000000) == 0 && ii--);

	m_sdhc_ocr = SDHC_CMDRSP0;
	if (SDHC_CMDRSP0 & 0x40000000) 
	{	// is high capacity
		sdCardDesc.highCapacity = 1;
	}

	// Card identify
	if(!sdhc_CMD(SDHC_CMD2_XFERTYP,0)) SDHC_ERROR(STA_NOINIT, SD_CARD_ERROR_CMD2);

	// Get card address
	if(!sdhc_CMD(SDHC_CMD3_XFERTYP,0)) SDHC_ERROR(STA_NOINIT, SD_CARD_ERROR_CMD3);

	sdCardDesc.address = SDHC_CMDRSP0 & 0xFFFF0000;

	// Get card parameters 
	if(!sdhc_CMD(SDHC_CMD9_XFERTYP,sdCardDesc.address)) SDHC_ERROR(STA_NOINIT, SD_CARD_ERROR_CMD9);
	if (0 == (SDHC_CMDRSP3 & 0x00C00000))
	{
		LWord read_bl_len, c_size, c_size_mult;

		read_bl_len = (SDHC_CMDRSP2 >> 8) & 0x0F;
		c_size = SDHC_CMDRSP2 & 0x03;
		c_size = (c_size << 10) | (SDHC_CMDRSP1 >> 22);
		c_size_mult = (SDHC_CMDRSP1 >> 7) & 0x07;
		sdCardDesc.numBlocks = (c_size + 1) * (1 << (c_size_mult + 2)) * (1 << (read_bl_len - 9));
	}
	else
	{
		LWord c_size;

		sdCardDesc.version2 = 1;
		c_size = (SDHC_CMDRSP1 >> 8) & 0x003FFFFF;
		sdCardDesc.numBlocks = (c_size + 1) << 10;
	}
	
	if(!sdhc_CMD(SDHC_CMD10_XFERTYP,sdCardDesc.address)) SDHC_ERROR(STA_NOINIT, SD_CARD_ERROR_CMD10);
	{
#if 0
		uint8_t d[16];
		  d[0] = SDHC_CMDRSP3 >> 16;
		  d[1] = SDHC_CMDRSP3 >> 8;
		  d[2] = SDHC_CMDRSP3;
		  d[3] = SDHC_CMDRSP2 >> 24;
		  d[4] = SDHC_CMDRSP2 >> 16;
		  d[5] = SDHC_CMDRSP2 >> 8;
		  d[6] = SDHC_CMDRSP2;
		  d[7] = SDHC_CMDRSP1 >> 24;
		  d[8] = SDHC_CMDRSP1 >> 16;
		  d[9] = SDHC_CMDRSP1 >> 8;
		  d[10] = SDHC_CMDRSP1;
		  d[11] = SDHC_CMDRSP0 >> 24;
		  d[12] = SDHC_CMDRSP0 >> 16;
		  d[13] = SDHC_CMDRSP0 >> 8;
		  d[14] = SDHC_CMDRSP0;
		  d[15] = 0;
#endif
	} // function not used yet
	
	// Select card
	if(!sdhc_CMD(SDHC_CMD7_XFERTYP,sdCardDesc.address)) SDHC_ERROR(STA_NOINIT, SD_CARD_ERROR_CMD7);

	// Set Block Size to 512
	// Block Size in SDHC Controller is already set to 512 by SDHC_Init();
	// Set 512 Block size in SD card
	if(!sdhc_CMD(SDHC_CMD16_XFERTYP,SDHC_BLOCK_SIZE)) SDHC_ERROR(STA_NOINIT, SD_CARD_ERROR_CMD16);

	if(SDHC_DO4BITS)
	{
		// Set 4 bit data bus width
		if(!(sdhc_CMD(SDHC_CMD55_XFERTYP,sdCardDesc.address) && sdhc_CMD(SDHC_ACMD6_XFERTYP,2)))		SDHC_ERROR(STA_NOINIT, SD_CARD_ERROR_ACMD6);

		// Set Data bus width also in SDHC controller
		SDHC_PROCTL &= (~ SDHC_PROCTL_DTW_MASK);
		SDHC_PROCTL |= SDHC_PROCTL_DTW(SDHC_PROCTL_DTW_4BIT);
	}  

#if SDHC_USE_ISR == 1
	// adaptet from Bill Greiman
	if(sdhc_CMD6_Switch(0X00FFFFFF,m_sdhc_CMD6_Status) && (m_sdhc_CMD6_Status[13] & 2) &&
	   sdhc_CMD6_Switch(0X80FFFFF1,m_sdhc_CMD6_Status)	&& (m_sdhc_CMD6_Status[16] & 0XF) == 1)
		kbaudrate = 50000;
	else 
		kbaudrate = 25000;
#else
		kbaudrate = 50000;	
#endif

	// De-Init GPIO
	sdhc_InitGPIO(0);

	// Set the SDHC default baud rate
	sdhc_SetBaudrate(kbaudrate);

	// Init GPIO
	sdhc_InitGPIO(0xFFFF);

	return sdCardDesc.status;
}

//-----------------------------------------------------------------------------
// FUNCTION:    disk_read
// RETURNS:     result of operation
//-----------------------------------------------------------------------------
DRESULT SDHC_ReadBlocks(UCHAR* buff, DWORD sector, UCHAR count)
{
	DRESULT result = RES_OK;
	LWord* pData = (LWord*)buff;

	// Check if this is ready
	if(sdCardDesc.status != 0)  return RES_NOTRDY;

	// Check the valid Count of block
	if(!count) return RES_PARERR; 

	// Convert LBA to UCHAR address if needed
	if(!sdCardDesc.highCapacity) sector *= 512;

	delayMicroseconds(100); // this is workaround to avoid sdhc blocking on BREN
	m_sdhc_waitCmd13 = 1;
	uint32_t cnt = 1<<16; while ((--cnt) && sdhc_isBusy()) /* yield() */;  if(!cnt) return RES_READERROR;
	m_sdhc_waitCmd13 = 0;

	while(SDHC_PRSSTAT & (SDHC_PRSSTAT_CIHB | SDHC_PRSSTAT_CDIHB | SDHC_PRSSTAT_DLA)) /* yield() */;
	
	SDHC_IRQSTAT = 0xffff; // clear interrupt status register

	//
#if SDHC_TRANSFERTYPE == SDHC_TRANSFERTYPE_DMA
	SDHC_DSADDR  = (LWord)pData;  
	SDHC_SYSCTL |=  SDHC_SYSCTL_HCKEN;
#endif   
	SDHC_BLKATTR = SDHC_BLKATTR_BLKCNT(count) | SDHC_BLKATTR_BLKSIZE(SDHC_BLOCK_SIZE);
	sdhc_enableDma();
	
	SDHC_CMDARG = sector;
	SDHC_XFERTYP = count==1 ? SDHC_CMD17_XFERTYP: SDHC_CMD18_XFERTYP; 

#if SDHC_TRANSFERTYPE == SDHC_TRANSFERTYPE_SWPOLL
	if(sdhc_waitCommandReady())
		result = sdhc_ReadBlock(pData,count,SDHC_BLOCK_SIZE);
	else
		result=RES_READERROR;
#elif SDHC_TRANSFERTYPE == SDHC_TRANSFERTYPE_DMA
	result=RES_OK;
#endif	
	// Auto CMD12 is enabled
	if((result != RES_OK) && (count>1))
		result=sdhc_CMD12_StopTransferWaitForBusy();
  	return result; 
}

//-----------------------------------------------------------------------------
// FUNCTION:    disk_write
// RETURNS:     result of operation
//-----------------------------------------------------------------------------
DRESULT SDHC_WriteBlocks(UCHAR* buff, DWORD sector, UCHAR count)
{
	DRESULT result = RES_OK;
	LWord* pData = (LWord*)buff;

	// Check if this is ready
	if(sdCardDesc.status != 0)  return RES_NOTRDY;

	// Check the valid Count of block
	if(!count) return RES_PARERR; 

	// Convert LBA to UCHAR address if needed
	if(!sdCardDesc.highCapacity)  sector *= 512;

//	delayMicroseconds(100); // this is workaround to avoid sdhc blocking on BWEN
	m_sdhc_waitCmd13 = 1;
	uint32_t cnt = 1<<16; while ((--cnt) && sdhc_isBusy()) /* yield() */;  if(!cnt) return RES_WRITEERROR;
	m_sdhc_waitCmd13 = 0;

	while(SDHC_PRSSTAT & (SDHC_PRSSTAT_CIHB | SDHC_PRSSTAT_CDIHB | SDHC_PRSSTAT_DLA)) /* yield() */;
	
	SDHC_IRQSTAT = 0xffff; // clear interrupt status register
#if SDHC_TRANSFERTYPE == SDHC_TRANSFERTYPE_DMA
	SDHC_DSADDR  = (LWord)pData;  
#endif   
	SDHC_BLKATTR = SDHC_BLKATTR_BLKCNT(count) | SDHC_BLKATTR_BLKSIZE(SDHC_BLOCK_SIZE);
	sdhc_enableDma();
	
	SDHC_CMDARG = sector;
	SDHC_XFERTYP = count==1 ? SDHC_CMD24_XFERTYP: SDHC_CMD25_XFERTYP; 

#if SDHC_TRANSFERTYPE == SDHC_TRANSFERTYPE_SWPOLL
	if(sdhc_waitCommandReady())
		result = sdhc_WriteBlock(pData,count,SDHC_BLOCK_SIZE);
	else
		result=RES_WRITEERROR;
#elif SDHC_TRANSFERTYPE == SDHC_TRANSFERTYPE_DMA
		result=RES_OK;
#endif	
	// Auto CMD12 is enabled
	if((result != RES_OK) && (count>1))
		result=sdhc_CMD12_StopTransferWaitForBusy();
	return result;
}

static uint32_t sdhc_waitCommandReady(void)
{	do
	{	m_sdhc_irqstat = SDHC_IRQSTAT;
	} while(!( m_sdhc_irqstat & (SDHC_IRQSTAT_CC | SDHC_IRQSTAT_CMD_ERROR)));

	SDHC_IRQSTAT =m_sdhc_irqstat;
	
	return ((m_sdhc_irqstat & SDHC_IRQSTAT_CC) && !(m_sdhc_irqstat & SDHC_IRQSTAT_CMD_ERROR));
}
/******************************************************************************
*
*   Private functions
*
******************************************************************************/
//-----------------------------------------------------------------------------
// FUNCTION:    sdhc_Init
//-----------------------------------------------------------------------------  
static DSTATUS sdhc_Init(void)
{
#ifdef HAS_KINETIS_MPU
	  // Allow SDHC Bus Master access. (from Bill Greiman)
	  MPU_RGDAAC0 |= 0x0C000000;
#endif

    // Enable clock to SDHC peripheral
    SIM_SCGC3 |= SIM_SCGC3_SDHC;

    // Enable clock to PORT E peripheral (all SDHC BUS signals)
    SIM_SCGC5 |= SIM_SCGC5_PORTE;
    
    SIM_SCGC6 |= SIM_SCGC6_DMAMUX;
    SIM_SCGC7 |= SIM_SCGC7_DMA;

    // De-init GPIO - to prevent unwanted clocks on bus
    sdhc_InitGPIO(0);
    
    /* Reset SDHC */
    SDHC_SYSCTL = SDHC_SYSCTL_RSTA | SDHC_SYSCTL_SDCLKFS(0x80);
    while (SDHC_SYSCTL & SDHC_SYSCTL_RSTA) /* yield() */;
    
    /* Initial values */ // to do - Check values
    SDHC_VENDOR = 0;
    SDHC_PROCTL = SDHC_PROCTL_EMODE(SDHC_PROCTL_EMODE_INVARIANT) | SDHC_PROCTL_D3CD;
    SDHC_WML = SDHC_WML_RDWML(16) | SDHC_WML_WRWML(16); // initially for CMD6

    /* Set the SDHC initial baud rate divider and start */
    sdhc_SetBaudrate(400);

    /* Poll inhibit bits */
    while (SDHC_PRSSTAT & (SDHC_PRSSTAT_CIHB | SDHC_PRSSTAT_CDIHB)) /* yield() */;

    /* Init GPIO again */
    sdhc_InitGPIO(0xFFFF);
    
    /* Enable requests */
    SDHC_IRQSTAT = 0xFFFF;
	SDHC_IRQSTATEN = SDHC_IRQSTATEN_MASK;
    
  #if SDHC_USE_ISR == 1
	NVIC_SET_PRIORITY(IRQ_SDHC, 6*16); //4*16 is Serial
    NVIC_ENABLE_IRQ(IRQ_SDHC);
    SDHC_IRQSIGEN = SDHC_IRQSIGEN_DINTIEN;
  #endif

    /* 80 initial clocks */
    SDHC_SYSCTL |= SDHC_SYSCTL_INITA;
    while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) /* yield() */;

    // to do - check if this needed
    SDHC_IRQSTAT |= SDHC_IRQSTAT_CRM;
    
	// Check card
	if(!(SDHC_PRSSTAT & SDHC_PRSSTAT_CINS))	return STA_NODISK;
	
	return 0;
}


//-----------------------------------------------------------------------------
// FUNCTION:    sdhc_InitGPIO
// RETURNS:     none
//----------------------------------------------------------------------------- 
static void sdhc_InitGPIO(Word init)
{  
  PORTE_PCR0 = init & (PORT_PCR_MUX(4) | PORT_PCR_PS | PORT_PCR_PE | PORT_PCR_DSE);    /* SDHC.D1  */
  PORTE_PCR1 = init & (PORT_PCR_MUX(4) | PORT_PCR_PS | PORT_PCR_PE | PORT_PCR_DSE);    /* SDHC.D0  */
  PORTE_PCR2 = init & (PORT_PCR_MUX(4) | PORT_PCR_DSE);                                /* SDHC.CLK */
  PORTE_PCR3 = init & (PORT_PCR_MUX(4) | PORT_PCR_PS | PORT_PCR_PE | PORT_PCR_DSE);    /* SDHC.CMD */
  PORTE_PCR4 = init & (PORT_PCR_MUX(4) | PORT_PCR_PS | PORT_PCR_PE | PORT_PCR_DSE);    /* SDHC.D3  */
  PORTE_PCR5 = init & (PORT_PCR_MUX(4) | PORT_PCR_PS | PORT_PCR_PE | PORT_PCR_DSE);    /* SDHC.D2  */  
}

//-----------------------------------------------------------------------------
// FUNCTION:    sdhc_SetBaudrate
// RETURNS:     result
//----------------------------------------------------------------------------- 
static DRESULT sdhc_SetBaudrate(uint32_t kbaudrate) 
{
	LWord sysctl, time_out;

	// Disable SDHC clocks
	SDHC_SYSCTL &= (~ SDHC_SYSCTL_SDCLKEN);

	// get dividers from requested baud rate 
	uint32_t aux=F_CPU;
	uint32_t ii=0,jj=1;
	uint32_t baudrate=kbaudrate*1000;

	while(aux/(16*(1<<ii))>baudrate) ii++;
	while(aux/(jj*(1<<ii))>baudrate) jj++;

	uint32_t minpresc=(1<<ii)>>1;
	uint32_t mindiv=jj-1;

	m_sdhc_baudrate=F_CPU/((1<<minpresc) * (mindiv+1));

	// Change dividers
	sysctl = SDHC_SYSCTL & 
			(~ (SDHC_SYSCTL_DTOCV_MASK | SDHC_SYSCTL_SDCLKFS_MASK | SDHC_SYSCTL_DVS_MASK));
	SDHC_SYSCTL = sysctl | 
			(SDHC_SYSCTL_DTOCV(0x0E) | SDHC_SYSCTL_SDCLKFS(minpresc) | SDHC_SYSCTL_DVS(mindiv));

	/* Wait for stable clock */
	time_out = 0xfffff;
	while ((0 == (SDHC_PRSSTAT & SDHC_PRSSTAT_SDSTB)) && time_out) time_out--;

	/* Enable SDHC clocks */
	SDHC_SYSCTL |= SDHC_SYSCTL_SDCLKEN;
	SDHC_IRQSTAT |= SDHC_IRQSTAT_DTOE;

	if(time_out)
		return RES_OK;
	else
		return RES_ERROR;
  
}

//-----------------------------------------------------------------------------
// FUNCTION:    SDHC_ReadBlock
// RETURNS:     result
//----------------------------------------------------------------------------- 
#if SDHC_TRANSFERTYPE == SDHC_TRANSFERTYPE_SWPOLL 
static DRESULT sdhc_ReadBlock(LWord* pData, LWord Count, LWord Size)
{
	int ii,jj;
	int cnt;
	ii = (Count*Size)/(sizeof(unsigned long)*SDHC_FIFO_BUFFER_SIZE);    
	while (ii--) 
	{        
		if (SDHC_IRQSTAT & (SDHC_IRQSTAT_DEBE | SDHC_IRQSTAT_DCE | SDHC_IRQSTAT_DTOE)) 
		{ // check for read errors           
			SDHC_IRQSTAT |= (SDHC_IRQSTAT_DEBE | SDHC_IRQSTAT_DCE | SDHC_IRQSTAT_DTOE | SDHC_IRQSTAT_BRR);
			return 10;  // return error        
		}
		//
		cnt=1<<24; while (--cnt && !(SDHC_PRSSTAT & SDHC_PRSSTAT_BREN)) /* yield() */; if(!cnt) return 11;
		
		SDHC_IRQSTAT |= SDHC_IRQSTAT_BRR;	
		for(jj=0;jj<SDHC_FIFO_BUFFER_SIZE;jj++)	*pData++ = SDHC_DATPORT;
	}    
	
	while (!(SDHC_IRQSTAT & SDHC_IRQSTAT_TC));  // wait for transfer to complete
	SDHC_IRQSTAT = (SDHC_IRQSTAT_TC | SDHC_IRQSTAT_BRR | SDHC_IRQSTAT_AC12E);
	
   return RES_OK;
}
#endif  

//-----------------------------------------------------------------------------
// FUNCTION:    sdhc_WriteBlock
// RETURNS:     result
//----------------------------------------------------------------------------- 
#if SDHC_TRANSFERTYPE == SDHC_TRANSFERTYPE_SWPOLL 
static DRESULT sdhc_WriteBlock(const LWord* pData, LWord Count, LWord Size)
{
	int ii,jj;
	int cnt;
	ii = (Count*Size)/(sizeof(unsigned long)*SDHC_FIFO_BUFFER_SIZE);    
	while (ii--) 
	{        
		if (SDHC_IRQSTAT & (SDHC_IRQSTAT_DEBE | SDHC_IRQSTAT_DCE | SDHC_IRQSTAT_DTOE)) 
		{ // check for read errors           
			SDHC_IRQSTAT |= (SDHC_IRQSTAT_DEBE | SDHC_IRQSTAT_DCE | SDHC_IRQSTAT_DTOE | SDHC_IRQSTAT_BWR);
			return 20;  // return error        
		}
		//
		cnt=1<<24; while (--cnt && !(SDHC_PRSSTAT & SDHC_PRSSTAT_BWEN)) /* yield() */; if(!cnt) return 21;
		
		SDHC_IRQSTAT |= SDHC_IRQSTAT_BWR;	
		for(jj=0;jj<SDHC_FIFO_BUFFER_SIZE;jj++)	 SDHC_DATPORT = *pData++;
	}    
	while (!(SDHC_IRQSTAT & SDHC_IRQSTAT_TC));  // wait for transfer to complete
	SDHC_IRQSTAT = (SDHC_IRQSTAT_TC | SDHC_IRQSTAT_BWR | SDHC_IRQSTAT_AC12E);
   return RES_OK;

}
#endif	

//-------------------------------------------------------------------------------------------
static uint16_t sdhc_CMD(LWord xfertype, LWord arg)
{
    // Wait for cmd line idle // to do timeout PRSSTAT[CDIHB] and the PRSSTAT[CIHB]
    while (SDHC_PRSSTAT & (SDHC_PRSSTAT_CIHB | SDHC_PRSSTAT_CDIHB)) /* yield() */;

	SDHC_CMDARG = arg;
	SDHC_XFERTYP = xfertype;
	return sdhc_waitCommandReady();
}

static uint16_t sdhc_isBusy(void) 
{
	if (!sdhc_CMD(SDHC_CMD13_XFERTYP, sdCardDesc.address)) return 1;
	return !(SDHC_CMDRSP0 & CARD_STATUS_READY_FOR_DATA);
}

//================================================================================
void sdhc_isr(void) 
{
	while(!(SDHC_IRQSTAT & SDHC_IRQSTAT_TC))  yield() ;	// wait for transfer to complete
 	SDHC_IRQSIGEN = 0;
	SDHC_IRQSTATEN &= ~SDHC_IRQSTATEN_DINTSEN;

	// from e3978 in http://www.nxp.com/docs/pcn_attachments/15946_KINETIS_2N03G.pdf
	if(SDHC_SYSCTL & SDHC_SYSCTL_HCKEN)
		SDHC_SYSCTL &=  ~SDHC_SYSCTL_HCKEN;

	__disable_irq();
	m_sdhc_irqstat = SDHC_IRQSTAT; // we clear all interrupt status bits
	SDHC_IRQSTAT = m_sdhc_irqstat;
	__enable_irq();

	// from e3984 in http://www.nxp.com/docs/pcn_attachments/15946_KINETIS_2N03G.pdf
    SDHC_PROCTL &= ~SDHC_PROCTL_D3CD;
    SDHC_PROCTL |=  SDHC_PROCTL_D3CD;

	SDHC_IRQSTATEN |= SDHC_IRQSTATEN_DINTSEN;
  	SDHC_IRQSIGEN = SDHC_IRQSIGEN_DINTIEN; // re-enable Interrupt
	m_sdhc_dmaDone = 1;
}
//-----------------------------------------------------------------------------
static void sdhc_enableDma(void) 
{
    while (SDHC_PRSSTAT & (SDHC_PRSSTAT_CIHB | SDHC_PRSSTAT_CDIHB)) /* yield() */;
	m_sdhc_dmaDone = 0;
	m_sdhc_irqstat = 0;
	SDHC_IRQSIGEN = SDHC_IRQSIGEN_DINTIEN; // reanable Interrupt
}
//-----------------------------------------------------------------------------
static uint16_t sdhc_waitDma(void) 
{
	while(!m_sdhc_dmaDone) /* yield() */;
	return (m_sdhc_irqstat & SDHC_IRQSTAT_TC) && !(m_sdhc_irqstat & SDHC_IRQSTAT_ERROR);
}
//-----------------------------------------------------------------------------
static uint16_t sdhc_CMD6_Switch(uint32_t arg, uint8_t* status) 
{
    while (SDHC_PRSSTAT & (SDHC_PRSSTAT_CIHB | SDHC_PRSSTAT_CDIHB | SDHC_PRSSTAT_DLA)) /* yield() */;
	SDHC_IRQSTAT |= SDHC_IRQSTAT_TC;
	SDHC_DSADDR  = (LWord)status;
	SDHC_BLKATTR = SDHC_BLKATTR_BLKCNT(1) | SDHC_BLKATTR_BLKSIZE(64);

	sdhc_enableDma();
	if(!sdhc_CMD(SDHC_CMD6_XFERTYP , arg)) SDHC_ERROR(0x06, SD_CARD_ERROR_CMD6);
	if(sdhc_waitDma()) return 1; 
	
	m_sdhc_error = SD_CARD_ERROR_CMD6;
	return 0;

}

//-----------------------------------------------------------------------------
// FUNCTION:    SDHC_CMD12_StopTransferWaitForBusy
// RETURNS:     result
//----------------------------------------------------------------------------- 
static DRESULT sdhc_CMD12_StopTransferWaitForBusy(void)
{
	LWord timeOut = 100;
	uint16_t result;
	do{
		result = sdhc_CMD(SDHC_CMD12_XFERTYP,0);
		timeOut--;
	} while(timeOut && (SDHC_PRSSTAT & SDHC_PRSSTAT_DLA) && result); // DataLineActive

	if(!result) return RES_ERROR;
	if(!timeOut) return RES_NONRSPNS;
	return RES_OK;
}

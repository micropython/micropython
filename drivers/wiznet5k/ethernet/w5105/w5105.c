//*****************************************************************************
//
//! \file w5100S.c
//! \brief W5100S HAL Interface.
//! \version 1.0.0
//! \date 2018/03/29
//! \par  Revision history
//!       <2018/03/29> 1st Release
//! \author Peter
//! 
//! Copyright (c)  2013, WIZnet Co., LTD.
//! All rights reserved.
//! 
//! Redistribution and use in source and binary forms, with or without 
//! modification, are permitted provided that the following conditions 
//! are met: 
//! 
//!     * Redistributions of source code must retain the above copyright 
//! notice, this list of conditions and the following disclaimer. 
//!     * Redistributions in binary form must reproduce the above copyright
//! notice, this list of conditions and the following disclaimer in the
//! documentation and/or other materials provided with the distribution. 
//!     * Neither the name of the <ORGANIZATION> nor the names of its 
//! contributors may be used to endorse or promote products derived 
//! from this software without specific prior written permission. 
//! 
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//! CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//! SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//! INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//! CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//! ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
//! THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#include "w5105.h"

////////////////////////////////////////////////////

#define LPC_SSP0 (0)

static void Chip_SSP_ReadFrames_Blocking(int dummy, uint8_t *buf, uint32_t len) {
    WIZCHIP.IF.SPI._read_bytes(buf, len);
}

static void Chip_SSP_WriteFrames_Blocking(int dummy, const uint8_t *buf, uint32_t len) {
    WIZCHIP.IF.SPI._write_bytes(buf, len);
}


/**
@brief  This function writes the data into W5100S registers.
*/
void     WIZCHIP_WRITE(uint32_t AddrSel, uint8_t wb )
{
   uint8_t spi_data[4];

   WIZCHIP_CRITICAL_ENTER();
   WIZCHIP.CS._select();

#if( (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_))
      spi_data[0] = 0xF0;
		spi_data[1] = (AddrSel & 0xFF00) >>  8;
		spi_data[2] = (AddrSel & 0x00FF) >>  0;
		spi_data[3] = wb;
		Chip_SSP_WriteFrames_Blocking(LPC_SSP0, spi_data, 4);
#elif ( (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_5500_) )
   if(!WIZCHIP.IF.SPI._write_burst) 	// byte operation
   {
	   WIZCHIP.IF.SPI._write_byte((AddrSel & 0xFF00) >>  8);
	   WIZCHIP.IF.SPI._write_byte((AddrSel & 0x00FF) >>  0);
	   WIZCHIP.IF.SPI._write_byte(0xF0);
	   WIZCHIP.IF.SPI._write_byte(wb);    // Data write (write 1byte data)
   }
   else     // burst operation
   {
      spi_data[0] = (AddrSel & 0xFF00) >>  8;
		spi_data[1] = (AddrSel & 0x00FF) >>  0;
		spi_data[2] = 0xF0;
		spi_data[3] = wb;
		WIZCHIP.IF.SPI._write_burst(spi_data, 4);

   }
#elif ( (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_INDIR_) )

   //add indirect bus 
   //M20150601 : Rename the function for integrating with ioLibrary
   //WIZCHIP.IF.BUS._write_byte(IDM_AR0,(AddrSel & 0xFF00) >>  8);
   //WIZCHIP.IF.BUS._write_byte(IDM_AR1,(AddrSel & 0x00FF));	
   //WIZCHIP.IF.BUS._write_byte(IDM_DR,wb);
   WIZCHIP.IF.BUS._write_data(IDM_AR0,(AddrSel & 0xFF00) >>  8);
   WIZCHIP.IF.BUS._write_data(IDM_AR1,(AddrSel & 0x00FF));	
   WIZCHIP.IF.BUS._write_data(IDM_DR,wb);
#else
   #error "Unknown _WIZCHIP_IO_MODE_ in W5100. !!!"
#endif

   WIZCHIP.CS._deselect();
   WIZCHIP_CRITICAL_EXIT();
}
/**
@brief  This function reads the value from W5100S registers.
*/
uint8_t  WIZCHIP_READ(uint32_t AddrSel)
{
   uint8_t ret;
   uint8_t spi_data[3];
   WIZCHIP_CRITICAL_ENTER();
   WIZCHIP.CS._select();

#if( (_WIZCHIP_IO_MODE_ ==  _WIZCHIP_IO_MODE_SPI_))
      spi_data[0] = 0x0F;
      spi_data[1] = (AddrSel & 0xFF00) >>  8;
      spi_data[2] = (AddrSel & 0x00FF) >>  0;
      Chip_SSP_WriteFrames_Blocking(LPC_SSP0, spi_data, 3);
      Chip_SSP_ReadFrames_Blocking(LPC_SSP0, &ret, 1);
    
#elif ( (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_5500_) )
   if(!WIZCHIP.IF.SPI._read_burst || !WIZCHIP.IF.SPI._write_burst) 	// burst operation
	{
        WIZCHIP.IF.SPI._write_byte((AddrSel & 0xFF00) >>  8);
        WIZCHIP.IF.SPI._write_byte((AddrSel & 0x00FF) >>  0);
        WIZCHIP.IF.SPI._write_byte(0x0F);
   }
   else
   {
		spi_data[0] = (AddrSel & 0xFF00) >>  8;
		spi_data[1] = (AddrSel & 0x00FF) >>  0;
		spi_data[2] = 0x0F
		WIZCHIP.IF.SPI._write_burst(spi_data, 3);
   }
   ret = WIZCHIP.IF.SPI._read_byte();
#elif ( (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_INDIR_) )

   //add indirect bus
   //M20150601 : Rename the function for integrating with ioLibrary
   //WIZCHIP.IF.BUS._write_byte(IDM_AR0,(AddrSel & 0xFF00) >>  8);
   //WIZCHIP.IF.BUS._write_byte(IDM_AR1,(AddrSel & 0x00FF));	
   //ret = WIZCHIP.IF.BUS._read_byte(IDM_DR);
   WIZCHIP.IF.BUS._write_data(IDM_AR0,(AddrSel & 0xFF00) >>  8);
   WIZCHIP.IF.BUS._write_data(IDM_AR1,(AddrSel & 0x00FF));
   ret = WIZCHIP.IF.BUS._read_data(IDM_DR);

#else
   #error "Unknown _WIZCHIP_IO_MODE_ in W5100S. !!!"   
#endif

   WIZCHIP.CS._deselect();
   WIZCHIP_CRITICAL_EXIT();
   return ret;
}


/**
@brief  This function writes into W5100S memory(Buffer)
*/ 
void     WIZCHIP_WRITE_BUF(uint32_t AddrSel, uint8_t* pBuf, uint16_t len)
{
   uint8_t spi_data[3];
   uint16_t i = 0;

   WIZCHIP_CRITICAL_ENTER();
   WIZCHIP.CS._select();   //M20150601 : Moved here.

#if((_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_))
		spi_data[0] = 0xF0;
		spi_data[1] = (((uint16_t)(AddrSel+i)) & 0xFF00) >>  8;
		spi_data[2] = (((uint16_t)(AddrSel+i)) & 0x00FF) >>  0;
		Chip_SSP_WriteFrames_Blocking(LPC_SSP0, spi_data, 3);
      Chip_SSP_WriteFrames_Blocking(LPC_SSP0, pBuf, len);

#elif ( (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_5500_) )
   if(!WIZCHIP.IF.SPI._write_burst) 	// byte operation
   {
      WIZCHIP.IF.SPI._write_byte((((uint16_t)(AddrSel+i)) & 0xFF00) >>  8);
      WIZCHIP.IF.SPI._write_byte((((uint16_t)(AddrSel+i)) & 0x00FF) >>  0);
      WIZCHIP.IF.SPI._write_byte(0xF0);

      for(i = 0; i < len; i++)
      {
         WIZCHIP.IF.SPI._write_byte(pBuf[i]);    // Data write (write 1byte data)
      }
   }
   else     // burst operation
   {
		spi_data[0] = (((uint16_t)(AddrSel+i)) & 0xFF00) >>  8;
		spi_data[1] = (((uint16_t)(AddrSel+i)) & 0x00FF) >>  0;
      spi_data[2] = 0xF0;
		WIZCHIP.IF.SPI._write_burst(spi_data, 3);
		WIZCHIP.IF.SPI._write_burst(pBuf, len);
   }
#elif ( (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_INDIR_) )
   //M20150601 : Rename the function for integrating with ioLibrary  
   /*
   WIZCHIP_WRITE(MR,WIZCHIP_READ(MR) | MR_AI);     
   WIZCHIP.IF.BUS._write_byte(IDM_AR0,(AddrSel & 0xFF00) >>  8);
   WIZCHIP.IF.BUS._write_byte(IDM_AR1,(AddrSel & 0x00FF));
   for(i = 0 ; i < len; i++)
      WIZCHIP.IF.BUS._write_byte(IDM_DR,pBuf[i]);
   WIZCHIP_WRITE(MR, WIZCHIP_READ(MR) & ~MR_AI);   
   */
   setMR(getMR()|MR_AI);     
   WIZCHIP.IF.BUS._write_data(IDM_AR0,(AddrSel & 0xFF00) >>  8);
   WIZCHIP.IF.BUS._write_data(IDM_AR1,(AddrSel & 0x00FF));
   for(i = 0 ; i < len; i++)
      WIZCHIP.IF.BUS._write_data(IDM_DR,pBuf[i]);
   setMR(getMR() & ~MR_AI);   

#else
   #error "Unknown _WIZCHIP_IO_MODE_ in W5100S. !!!!"
#endif
   
   WIZCHIP.CS._deselect();  //M20150601 : Moved here.
   WIZCHIP_CRITICAL_EXIT();
}

/**
@brief  This function reads into W5100S memory(Buffer)
*/ 

void     WIZCHIP_READ_BUF (uint32_t AddrSel, uint8_t* pBuf, uint16_t len)
{
   uint8_t spi_data[3];
   uint16_t i = 0;
   WIZCHIP_CRITICAL_ENTER();
   WIZCHIP.CS._select();   //M20150601 : Moved here.
   
#if( (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_) )
		spi_data[0] = 0x0F;
		spi_data[1] = (uint16_t)((AddrSel+i) & 0xFF00) >>  8;
		spi_data[2] = (uint16_t)((AddrSel+i) & 0x00FF) >>  0;
		Chip_SSP_WriteFrames_Blocking(LPC_SSP0, spi_data, 3);
      Chip_SSP_ReadFrames_Blocking(LPC_SSP0, pBuf, len);

#elif ( (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_5500_) )
   if(!WIZCHIP.IF.SPI._read_burst || !WIZCHIP.IF.SPI._write_burst) 	// byte operation
   {
      WIZCHIP.IF.SPI._write_byte((uint16_t)((AddrSel+i) & 0xFF00) >>  8);
      WIZCHIP.IF.SPI._write_byte((uint16_t)((AddrSel+i) & 0x00FF) >>  0);
      WIZCHIP.IF.SPI._write_byte(0x0F);
   
      for(i = 0; i < len; i++)
      {
         pBuf[i] = WIZCHIP.IF.SPI._read_byte();
      }
   }
   else																// burst operation
   {
		spi_data[0] = (uint16_t)((AddrSel+i) & 0xFF00) >>  8;
		spi_data[1] = (uint16_t)((AddrSel+i) & 0x00FF) >>  0;
      spi_data[2] = 0x0F;
		WIZCHIP.IF.SPI._write_burst(spi_data, 3);
		WIZCHIP.IF.SPI._read_burst(pBuf, len);
   }


#elif ( (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_INDIR_) )
   //M20150601 : Rename the function for integrating with ioLibrary  
   /*
   WIZCHIP_WRITE(MR, WIZCHIP_READ(MR) | MR_AI);    
   WIZCHIP.IF.BUS._write_byte(IDM_AR0,(AddrSel & 0xFF00) >>  8);
   WIZCHIP.IF.BUS._write_byte(IDM_AR1,(AddrSel & 0x00FF));	
   for(i = 0 ; i < len; i++)
      pBuf[i]	= WIZCHIP.IF.BUS._read_byte(IDM_DR);
   WIZCHIP_WRITE(MR, WIZCHIP_READ(MR) & ~MR_AI); 
   */
   setMR(getMR() | MR_AI);
   WIZCHIP.IF.BUS._write_data(IDM_AR0,(AddrSel & 0xFF00) >>  8);
   WIZCHIP.IF.BUS._write_data(IDM_AR1,(AddrSel & 0x00FF));	
   for(i = 0 ; i < len; i++)
      pBuf[i]	= WIZCHIP.IF.BUS._read_data(IDM_DR);
   setMR(getMR() & ~MR_AI); 
   
#else
   #error "Unknown _WIZCHIP_IO_MODE_ in W5100S. !!!!"
#endif

   WIZCHIP.CS._deselect();    //M20150601 : Moved Here.
   WIZCHIP_CRITICAL_EXIT();
}

///////////////////////////////////
// Socket N regsiter IO function //
///////////////////////////////////

uint16_t getSn_TX_FSR(uint8_t sn)
{
   uint16_t val=0,val1=0;
   do
   {
      val1 = WIZCHIP_READ(Sn_TX_FSR(sn));
      val1 = (val1 << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_TX_FSR(sn),1));
      if (val1 != 0)
      {
        val = WIZCHIP_READ(Sn_TX_FSR(sn));
        val = (val << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_TX_FSR(sn),1));
      }
   }while (val != val1);
   return val;
}


uint16_t getSn_RX_RSR(uint8_t sn)
{
   uint16_t val=0,val1=0;
   do
   {
      val1 = WIZCHIP_READ(Sn_RX_RSR(sn));
      val1 = (val1 << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_RX_RSR(sn),1));
      if (val1 != 0)
      {
        val = WIZCHIP_READ(Sn_RX_RSR(sn));
        val = (val << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_RX_RSR(sn),1));
      }
   }while (val != val1);
   return val;
}

/////////////////////////////////////
// Sn_TXBUF & Sn_RXBUF IO function //
/////////////////////////////////////
uint32_t getSn_RxBASE(uint8_t sn)
{
   int8_t  i;
#if ( _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_DIR_)
   uint32_t rxbase = _W5100S_IO_BASE_ + _WIZCHIP_IO_RXBUF_;
#else   
   uint32_t rxbase = _WIZCHIP_IO_RXBUF_;
#endif   
   for(i = 0; i < sn; i++)
      rxbase += getSn_RxMAX(i);

   return rxbase;
}

uint32_t getSn_TxBASE(uint8_t sn)
{
   int8_t  i;
#if ( _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_DIR_)
   uint32_t txbase = _W5100S_IO_BASE_ + _WIZCHIP_IO_TXBUF_;
#else   
   uint32_t txbase = _WIZCHIP_IO_TXBUF_;
#endif   
   for(i = 0; i < sn; i++)
      txbase += getSn_TxMAX(i);
   return txbase;
}

/**
@brief  This function is being called by send() and sendto() function also. for copy the data form application buffer to Transmite buffer of the chip.

This function read the Tx write pointer register and after copy the data in buffer update the Tx write pointer
register. User should read upper byte first and lower byte later to get proper value.
And this function is being used for copy the data form application buffer to Transmite
buffer of the chip. It calculate the actual physical address where one has to write
the data in transmite buffer. Here also take care of the condition while it exceed
the Tx memory uper-bound of socket.

*/
void wiz_send_data(uint8_t sn, uint8_t *wizdata, uint16_t len)
{
  uint16_t ptr;
  uint16_t size;
  uint16_t dst_mask;
  uint16_t dst_ptr;

  ptr = getSn_TX_WR(sn);

  dst_mask = ptr & getSn_TxMASK(sn);
  dst_ptr = getSn_TxBASE(sn) + dst_mask;
  
  if (dst_mask + len > getSn_TxMAX(sn)) 
  {
    size = getSn_TxMAX(sn) - dst_mask;
    WIZCHIP_WRITE_BUF(dst_ptr, wizdata, size);
    wizdata += size;
    size = len - size;
    dst_ptr = getSn_TxBASE(sn);
    WIZCHIP_WRITE_BUF(dst_ptr, wizdata, size);
  } 
  else
  {
    WIZCHIP_WRITE_BUF(dst_ptr, wizdata, len);
  }

  ptr += len;

  setSn_TX_WR(sn, ptr);  
}


/**
@brief  This function is being called by recv() also. This function is being used for copy the data form Receive buffer of the chip to application buffer.

This function read the Rx read pointer register
and after copy the data from receive buffer update the Rx write pointer register.
User should read upper byte first and lower byte later to get proper value.
It calculate the actual physical address where one has to read
the data from Receive buffer. Here also take care of the condition while it exceed
the Rx memory uper-bound of socket.
*/
void wiz_recv_data(uint8_t sn, uint8_t *wizdata, uint16_t len)
{
  uint16_t ptr;
  uint16_t size;
  uint16_t src_mask;
  uint16_t src_ptr;

  ptr = getSn_RX_RD(sn);
  
  src_mask = (uint32_t)ptr & getSn_RxMASK(sn);
  src_ptr = (getSn_RxBASE(sn) + src_mask);

  
  if( (src_mask + len) > getSn_RxMAX(sn) ) 
  {
    size = getSn_RxMAX(sn) - src_mask;
    WIZCHIP_READ_BUF((uint32_t)src_ptr, (uint8_t*)wizdata, size);
    wizdata += size;
    size = len - size;
	src_ptr = getSn_RxBASE(sn);
    WIZCHIP_READ_BUF(src_ptr, (uint8_t*)wizdata, size);
  } 
  else
  {
    WIZCHIP_READ_BUF(src_ptr, (uint8_t*)wizdata, len);
  }
    
  ptr += len;
  
  setSn_RX_RD(sn, ptr);
}

void wiz_recv_ignore(uint8_t sn, uint16_t len)
{
  uint16_t ptr;

  ptr = getSn_RX_RD(sn);
  
  ptr += len;
  setSn_RX_RD(sn,ptr);
}

void wiz_mdio_write(uint8_t PHYMDIO_regadr, uint16_t var)
{
    WIZCHIP_WRITE(PHYRAR,PHYMDIO_regadr);
    WIZCHIP_WRITE(PHYDIR, (uint8_t)(var >> 8));
    WIZCHIP_WRITE(PHYDIR+1, (uint8_t)(var));
    WIZCHIP_WRITE(PHYACR, PHYACR_WRITE);
    while(WIZCHIP_READ(PHYACR));  //wait for command complete
}

uint16_t wiz_mdio_read(uint8_t PHYMDIO_regadr)
{
    WIZCHIP_WRITE(PHYRAR,PHYMDIO_regadr);
    WIZCHIP_WRITE(PHYACR, PHYACR_READ);
    while(WIZCHIP_READ(PHYACR));  //wait for command complete
    return ((uint16_t)WIZCHIP_READ(PHYDOR) << 8) | WIZCHIP_READ(PHYDOR+1);
}

void wiz_delay_ms(uint32_t milliseconds)
{
	uint32_t i;
	for(i = 0 ; i < milliseconds ; i++)
	{
		//Write any values to clear the TCNTCLKR register
		setTCNTCLKR(0xff);

		// Wait until counter register value reaches 10.(10 = 1ms : TCNTR is 100us tick counter register)
		while(getTCNTR() < 0x0a){}
	}
}


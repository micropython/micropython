//*****************************************************************************
// cc_pal.c
//
// simplelink abstraction file for CC3200
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

//Simplelink includes
#include <simplelink.h>
#include <cc_pal.h>

//Driverlib includes
#include <hw_ints.h>
#include <hw_types.h>
#include <pin.h>
#include <hw_memmap.h>
#include <hw_mcspi.h>
#include <hw_common_reg.h>
#include <rom.h>
#include <rom_map.h>
#include <spi.h>
#include <prcm.h>
#include <rom.h>
#include <rom_map.h>
#include <hw_ints.h>
#include <interrupt.h>
#include <utils.h>


#define REG_INT_MASK_SET                0x400F7088
#define REG_INT_MASK_CLR                0x400F708C
#define APPS_SOFT_RESET_REG             0x4402D000
#define OCP_SHARED_MAC_RESET_REG        0x4402E168

#define SPI_RATE_20M                    20000000

#define UNUSED(x) 						(x = x)

//
// GLOBAL VARIABLES -- Start
//
volatile Fd_t g_SpiFd =0;
P_EVENT_HANDLER g_pHostIntHdl  = NULL;

//
// GLOBAL VARIABLES -- End
//


//****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES
//****************************************************************************
static int spi_Read_CPU(unsigned char *pBuff, int len);
static int spi_Write_CPU(unsigned char *pBuff, int len);

//****************************************************************************
//                      LOCAL FUNCTION DEFINITIONS
//****************************************************************************

/*!
    \brief attempts to read up to len bytes from SPI channel into a buffer starting at pBuff.

	\param			pBuff		- 	points to first location to start writing the data

	\param			len			-	number of bytes to read from the SPI channel

	\return			upon successful completion, the function shall return Read Size.
					Otherwise, -1 shall be returned

    \sa             spi_Read_CPU , spi_Write_CPU
	\note
    \warning
*/
int spi_Read_CPU(unsigned char *pBuff, int len)
{
    unsigned long ulCnt;
    unsigned long ulStatusReg;
    unsigned long *ulDataIn;
    unsigned long ulTxReg;
    unsigned long ulRxReg;

    MAP_SPICSEnable(LSPI_BASE);

    //
    // Initialize local variable.
    //
    ulDataIn = (unsigned long *)pBuff;
    ulCnt = (len + 3) >> 2;
    ulStatusReg = LSPI_BASE+MCSPI_O_CH0STAT;
    ulTxReg = LSPI_BASE + MCSPI_O_TX0;
    ulRxReg = LSPI_BASE + MCSPI_O_RX0;

    //
    // Reading loop
    //
    while(ulCnt--)
    {
          while(!( HWREG(ulStatusReg)& MCSPI_CH0STAT_TXS ));
          HWREG(ulTxReg) = 0xFFFFFFFF;
          while(!( HWREG(ulStatusReg)& MCSPI_CH0STAT_RXS ));
          *ulDataIn = HWREG(ulRxReg);
          ulDataIn++;
    }

    MAP_SPICSDisable(LSPI_BASE);

    return len;
}

/*!
    \brief attempts to write up to len bytes to the SPI channel

	\param			pBuff		- 	points to first location to start getting the data from

	\param			len			-	number of bytes to write to the SPI channel

	\return			upon successful completion, the function shall return write size.
					Otherwise, -1 shall be returned

    \sa             spi_Read_CPU , spi_Write_CPU
	\note			This function could be implemented as zero copy and return only upon successful completion
					of writing the whole buffer, but in cases that memory allocation is not too tight, the
					function could copy the data to internal buffer, return back and complete the write in
					parallel to other activities as long as the other SPI activities would be blocked untill
					the entire buffer write would be completed
    \warning
*/
int spi_Write_CPU(unsigned char *pBuff, int len)
{
    unsigned long ulCnt;
    unsigned long ulStatusReg;
    unsigned long *ulDataOut;
    unsigned long ulDataIn;
    unsigned long ulTxReg;
    unsigned long ulRxReg;


    MAP_SPICSEnable(LSPI_BASE);

    //
    // Initialize local variable.
    //
    ulDataOut = (unsigned long *)pBuff;
    ulCnt = (len +3 ) >> 2;
    ulStatusReg = LSPI_BASE+MCSPI_O_CH0STAT;
    ulTxReg = LSPI_BASE + MCSPI_O_TX0;
    ulRxReg = LSPI_BASE + MCSPI_O_RX0;

    //
    // Writing Loop
    //
    while(ulCnt--)
    {
          while(!( HWREG(ulStatusReg)& MCSPI_CH0STAT_TXS ));
          HWREG(ulTxReg) = *ulDataOut;
          while(!( HWREG(ulStatusReg)& MCSPI_CH0STAT_RXS ));
          ulDataIn = HWREG(ulRxReg);
          ulDataOut++;
    }

    MAP_SPICSDisable(LSPI_BASE);

    UNUSED(ulDataIn);
    return len;
}

/*!
    \brief open spi communication port to be used for communicating with a SimpleLink device

	Given an interface name and option flags, this function opens the spi communication port
	and creates a file descriptor. This file descriptor can be used afterwards to read and
	write data from and to this specific spi channel.
	The SPI speed, clock polarity, clock phase, chip select and all other attributes are all
	set to hardcoded values in this function.

	\param	 		ifName		-	points to the interface name/path. The interface name is an
									optional attributes that the simple link driver receives
									on opening the device. in systems that the spi channel is
									not implemented as part of the os device drivers, this
									parameter could be NULL.
	\param			flags		-	option flags

	\return			upon successful completion, the function shall open the spi channel and return
					a non-negative integer representing the file descriptor.
					Otherwise, -1 shall be returned

    \sa             spi_Close , spi_Read , spi_Write
	\note
    \warning
*/

Fd_t spi_Open(char *ifName, unsigned long flags)
{
    unsigned long ulBase;
    unsigned long ulSpiBitRate = SPI_RATE_20M;

    //NWP master interface
    ulBase = LSPI_BASE;

    //Enable MCSPIA2
    MAP_PRCMPeripheralClkEnable(PRCM_LSPI,PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);

    //Disable Chip Select
    MAP_SPICSDisable(ulBase);

    //Disable SPI Channel
    MAP_SPIDisable(ulBase);

    // Reset SPI
    MAP_SPIReset(ulBase);

    //
    // Configure SPI interface
	//

    MAP_SPIConfigSetExpClk(ulBase,MAP_PRCMPeripheralClockGet(PRCM_LSPI),
		  	  	  	 ulSpiBitRate,SPI_MODE_MASTER,SPI_SUB_MODE_0,
                     (SPI_SW_CTRL_CS |
                     SPI_4PIN_MODE |
                     SPI_TURBO_OFF |
                     SPI_CS_ACTIVEHIGH |
                     SPI_WL_32));

	MAP_SPIEnable(ulBase);

    g_SpiFd = 1;
    return g_SpiFd;
}
/*!
    \brief closes an opened spi communication port

	\param	 		fd			-	file descriptor of an opened SPI channel

	\return			upon successful completion, the function shall return 0.
					Otherwise, -1 shall be returned

    \sa             spi_Open
	\note
    \warning
*/
int spi_Close(Fd_t fd)
{
    unsigned long ulBase = LSPI_BASE;

    g_SpiFd = 0;

		//Disable Chip Select
	MAP_SPICSDisable(LSPI_BASE);


		//Disable SPI Channel
	MAP_SPIDisable(ulBase);

		// Reset SPI
	MAP_SPIReset(ulBase);

		// Disable SPI Peripheral
	MAP_PRCMPeripheralClkDisable(PRCM_LSPI,PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);

    return 0;
}

/*!
    \brief closes an opened spi communication port

	\param	 		fd			-	file descriptor of an opened SPI channel

	\return			upon successful completion, the function shall return 0.
					Otherwise, -1 shall be returned

    \sa             spi_Open
	\note
    \warning
*/

int spi_Read(Fd_t fd, unsigned char *pBuff, int len)
{
    if (fd != 1 || g_SpiFd != 1) {
        return -1;
    }

    return spi_Read_CPU(pBuff, len);
}

/*!
    \brief attempts to write up to len bytes to the SPI channel

	\param	 		fd			-	file descriptor of an opened SPI channel

	\param			pBuff		- 	points to first location to start getting the data from

	\param			len			-	number of bytes to write to the SPI channel

	\return			upon successful completion, the function shall return 0.
					Otherwise, -1 shall be returned

    \sa             spi_Open , spi_Read
	\note			This function could be implemented as zero copy and return only upon successful completion
					of writing the whole buffer, but in cases that memory allocation is not too tight, the
					function could copy the data to internal buffer, return back and complete the write in
					parallel to other activities as long as the other SPI activities would be blocked untill
					the entire buffer write would be completed
    \warning
*/
int spi_Write(Fd_t fd, unsigned char *pBuff, int len)
{
    if (fd != 1 || g_SpiFd != 1) {
        return -1;
    }

    return spi_Write_CPU(pBuff,len);
}

/*!
    \brief register an interrupt handler for the host IRQ

	\param	 		InterruptHdl	-	pointer to interrupt handler function

	\param 			pValue			-	pointer to a memory strcuture that is passed to the interrupt handler.

	\return			upon successful registration, the function shall return 0.
					Otherwise, -1 shall be returned

    \sa
	\note			If there is already registered interrupt handler, the function should overwrite the old handler
					with the new one
    \warning
*/

int NwpRegisterInterruptHandler(P_EVENT_HANDLER InterruptHdl , void* pValue)
{

    if(InterruptHdl == NULL)
    {
		//De-register Interprocessor communication interrupt between App and NWP
		#ifdef SL_PLATFORM_MULTI_THREADED
		  osi_InterruptDeRegister(INT_NWPIC);
		#else
		  MAP_IntDisable(INT_NWPIC);
		  MAP_IntUnregister(INT_NWPIC);
		  MAP_IntPendClear(INT_NWPIC);
		#endif
    }
    else
    {
		  #ifdef SL_PLATFORM_MULTI_THREADED
			 MAP_IntPendClear(INT_NWPIC);
			 osi_InterruptRegister(INT_NWPIC, (P_OSI_INTR_ENTRY)InterruptHdl,INT_PRIORITY_LVL_1);
		  #else
			 MAP_IntRegister(INT_NWPIC, InterruptHdl);
			 MAP_IntPrioritySet(INT_NWPIC, INT_PRIORITY_LVL_1);
			 MAP_IntPendClear(INT_NWPIC);
			 MAP_IntEnable(INT_NWPIC);
		  #endif
    }

  return 0;
}


/*!
    \brief 				Masks host IRQ


    \sa             		NwpUnMaskInterrupt

    \warning
*/
void NwpMaskInterrupt()
{
	(*(unsigned long *)REG_INT_MASK_SET) = 0x1;
}


/*!
    \brief 				Unmasks host IRQ


    \sa             		NwpMaskInterrupt

    \warning
*/
void NwpUnMaskInterrupt()
{
	(*(unsigned long *)REG_INT_MASK_CLR) = 0x1;
}

#ifdef DEBUG
/*!
    \brief		Preamble to the enabling the Network Processor.
                        Placeholder to implement any pre-process operations
                        before enabling networking operations.

    \sa			sl_DeviceEnable

    \note       belongs to \ref ported_sec

*/
void NwpPowerOnPreamble(void)
{
    #define MAX_RETRY_COUNT         1000

    unsigned int sl_stop_ind, apps_int_sts_raw, nwp_lpds_wake_cfg;
    unsigned int retry_count;
    /* Perform the sl_stop equivalent to ensure network services
       are turned off if active */
    HWREG(0x400F70B8) = 1;   /* APPs to NWP interrupt */
    UtilsDelay(800000/5);

    retry_count = 0;
    nwp_lpds_wake_cfg = HWREG(0x4402D404);
    sl_stop_ind = HWREG(0x4402E16C);

    if((nwp_lpds_wake_cfg != 0x20) && /* Check for NWP POR condition */
            !(sl_stop_ind & 0x2))     /* Check if sl_stop was executed */
    {
        /* Loop until APPs->NWP interrupt is cleared or timeout */
        while(retry_count < MAX_RETRY_COUNT)
        {
            apps_int_sts_raw = HWREG(0x400F70C0);
            if(apps_int_sts_raw & 0x1)
            {
                UtilsDelay(800000/5);
                retry_count++;
            }
            else
            {
                break;
            }
        }
    }
    HWREG(0x400F70B0) = 1;   /* Clear APPs to NWP interrupt */
    UtilsDelay(800000/5);

    /* Stop the networking services */
    NwpPowerOff();
}
#endif

/*!
    \brief		Enable the Network Processor

    \sa			sl_DeviceDisable

    \note       belongs to \ref ported_sec

*/
void NwpPowerOn(void)
{
    //bring the 1.32 eco out of reset
    HWREG(0x4402E16C) &= 0xFFFFFFFD;

    //NWP Wakeup
    HWREG(0x44025118) = 1;
#ifdef DEBUG
    UtilsDelay(8000000);
#endif

    //UnMask Host Interrupt
    NwpUnMaskInterrupt();
}


/*!
    \brief		Disable the Network Processor

    \sa			sl_DeviceEnable

    \note       belongs to \ref ported_sec
*/
void NwpPowerOff(void)
{
	//Must delay 300 usec to enable the NWP to finish all sl_stop activities
	UtilsDelay(300*80/3);

	//Mask Host Interrupt
    NwpMaskInterrupt();

    //Switch to PFM Mode
    HWREG(0x4402F024) &= 0xF7FFFFFF;

    //sl_stop eco for PG1.32 devices
    HWREG(0x4402E16C) |= 0x2;

    UtilsDelay(800000);
}

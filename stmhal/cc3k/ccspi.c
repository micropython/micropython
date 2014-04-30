/*****************************************************************************
*
*  spi.c - CC3000 Host Driver Implementation.
*  Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
*
* Adapted for use with the Arduino/AVR by KTOWN (Kevin Townsend) 
* & Limor Fried for Adafruit Industries
* This library works with the Adafruit CC3000 breakout 
*	----> https://www.adafruit.com/products/1469
* Adafruit invests time and resources providing this open source code,
* please support Adafruit and open-source hardware by purchasing
* products from Adafruit!
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
#include <stdint.h>
#include <string.h> // for memset

#include "ccspi.h"
#include "hci.h"
#include "netapp.h"
#include "evnt_handler.h"
#include "cc3000_common.h"
#include "ccdebug.h"
#include "pybcc3k.h"

#define READ                            (3)
#define WRITE                           (1)
#define HI(value)                       (((value) & 0xFF00) >> 8)
#define LO(value)                       ((value) & 0x00FF)
#define HEADERS_SIZE_EVNT               (SPI_HEADER_SIZE + 5)
#define SPI_HEADER_SIZE                 (5)

#define eSPI_STATE_POWERUP              (0)
#define eSPI_STATE_INITIALIZED          (1)
#define eSPI_STATE_IDLE                 (2)
#define eSPI_STATE_WRITE_IRQ            (3)
#define eSPI_STATE_WRITE_FIRST_PORTION  (4)
#define eSPI_STATE_WRITE_EOT            (5)
#define eSPI_STATE_READ_IRQ             (6)
#define eSPI_STATE_READ_FIRST_PORTION   (7)
#define eSPI_STATE_READ_EOT             (8)

// CC3000 chip select
#define CC3000_ASSERT_CS()    pyb_cc3000_set_cs(0)
// CC3000 chip deselect
#define CC3000_DEASSERT_CS()  pyb_cc3000_set_cs(1)

/* smartconfig flags (defined in Adafruit_CC3000.cpp) */
// extern unsigned long ulSmartConfigFinished, ulCC3000DHCP;

typedef struct
{
  gcSpiHandleRx  SPIRxHandler;

  unsigned short usTxPacketLength;
  unsigned short usRxPacketLength;
  unsigned long  ulSpiState;
  unsigned char *pTxPacket;
  unsigned char *pRxPacket;

} tSpiInformation;

tSpiInformation sSpiInformation;

/* Static buffer for 5 bytes of SPI HEADER */
//unsigned char tSpiReadHeader[] = {READ, 0, 0, 0, 0};

void SpiWriteDataSynchronous(unsigned char *data, unsigned short size);
void SpiWriteAsync(const unsigned char *data, unsigned short size);
void SpiPauseSpi(void);
void SpiResumeSpi(void);
void SSIContReadOperation(void);
void cc3k_int_poll(void);

// The magic number that resides at the end of the TX/RX buffer (1 byte after the allocated size)
// for the purpose of detection of the overrun. The location of the memory where the magic number
// resides shall never be written. In case it is written - the overrun occured and either recevie function
// or send function will stuck forever.
#define CC3000_BUFFER_MAGIC_NUMBER (0xDE)

char spi_buffer[CC3000_RX_BUFFER_SIZE];
unsigned char wlan_tx_buffer[CC3000_TX_BUFFER_SIZE];

static volatile char ccspi_is_in_irq = 0;
static volatile char ccspi_int_enabled = 0;

/* Mandatory functions are:
    - SpiOpen
    - SpiWrite
    - SpiRead
    - SpiClose
    - SpiResumeSpi
    - ReadWlanInterruptPin
    - WlanInterruptEnable
    - WlanInterruptDisable
    - WriteWlanPin
 */

void  SpiInit(void)
{
    pyb_cc3000_spi_init();
}


/**************************************************************************/
/*!

 */
/**************************************************************************/
void SpiClose(void)
{
  DEBUGPRINT_F("\tCC3000: SpiClose");
  
  if (sSpiInformation.pRxPacket)
  {
    sSpiInformation.pRxPacket = 0;
  }

  /*  Disable Interrupt in GPIOA module... */
  tSLInformation.WlanInterruptDisable();
}

/**************************************************************************/
/*!

 */
/**************************************************************************/
void SpiOpen(gcSpiHandleRx pfRxHandler)
{
  DEBUGPRINT_F("\tCC3000: SpiOpen");
  
  sSpiInformation.ulSpiState = eSPI_STATE_POWERUP;

  memset(spi_buffer, 0, sizeof(spi_buffer));
  memset(wlan_tx_buffer, 0, sizeof(spi_buffer));

  sSpiInformation.SPIRxHandler      = pfRxHandler;
  sSpiInformation.usTxPacketLength  = 0;
  sSpiInformation.pTxPacket         = NULL;
  sSpiInformation.pRxPacket         = (unsigned char *)spi_buffer;
  sSpiInformation.usRxPacketLength  = 0;
  
  spi_buffer[CC3000_RX_BUFFER_SIZE - 1]     = CC3000_BUFFER_MAGIC_NUMBER;
  wlan_tx_buffer[CC3000_TX_BUFFER_SIZE - 1] = CC3000_BUFFER_MAGIC_NUMBER;

  /* Enable interrupt on the GPIO pin of WLAN IRQ */
  tSLInformation.WlanInterruptEnable();

  DEBUGPRINT_F("\tCC3000: Finished SpiOpen\n\r");
}

/**************************************************************************/
/*!

 */
/**************************************************************************/
#if 0
extern uint8_t g_csPin, g_irqPin, g_vbatPin, g_IRQnum, g_SPIspeed;

int init_spi(void)
{

  DEBUGPRINT_F("\tCC3000: init_spi\n\r");
  
  /* Set POWER_EN pin to output and disable the CC3000 by default */
  pinMode(g_vbatPin, OUTPUT);
  digitalWrite(g_vbatPin, 0);
  delay(500);

  /* Set CS pin to output (don't de-assert yet) */
  pinMode(g_csPin, OUTPUT);

  /* Set interrupt/gpio pin to input */
#if defined(INPUT_PULLUP)
  pinMode(g_irqPin, INPUT_PULLUP);
#else
  pinMode(g_irqPin, INPUT);
  digitalWrite(g_irqPin, HIGH); // w/weak pullup
#endif

  /* Initialise SPI (Mode 1) */
  SPI.begin();
  SPI.setDataMode(SPI_MODE1);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(g_SPIspeed);
  
  // Newly-initialized SPI is in the same state that ASSERT_CS will set it
  // to.  Invoke DEASSERT (which also restores SPI registers) so the next
  // ASSERT call won't clobber the ccspi_old* values -- we need those!
  CC3000_DEASSERT_CS();

  /* ToDo: Configure IRQ interrupt! */

  DEBUGPRINT_F("\tCC3000: Finished init_spi\n\r");
  
  return(ESUCCESS);
}
#endif

/**************************************************************************/
/*!

 */
/**************************************************************************/
long SpiFirstWrite(unsigned char *ucBuf, unsigned short usLength)
{
  DEBUGPRINT_F("\tCC3000: SpiWriteFirst\n\r");
  
  /* Workaround for the first transaction */
  CC3000_ASSERT_CS();

  /* delay (stay low) for ~50us */
  pyb_delay_us(50);

  /* SPI writes first 4 bytes of data */
  SpiWriteDataSynchronous(ucBuf, 4);

  pyb_delay_us(50);

  SpiWriteDataSynchronous(ucBuf + 4, usLength - 4);

  /* From this point on - operate in a regular manner */
  sSpiInformation.ulSpiState = eSPI_STATE_IDLE;

  CC3000_DEASSERT_CS();

  return(0);
}

/**************************************************************************/
/*!

 */
/**************************************************************************/
long SpiWrite(unsigned char *pUserBuffer, unsigned short usLength)
{
  unsigned char ucPad = 0;

  DEBUGPRINT_F("\tCC3000: SpiWrite\n\r");
  
  /* Figure out the total length of the packet in order to figure out if there is padding or not */
  if(!(usLength & 0x0001))
  {
    ucPad++;
  }

  pUserBuffer[0] = WRITE;
  pUserBuffer[1] = HI(usLength + ucPad);
  pUserBuffer[2] = LO(usLength + ucPad);
  pUserBuffer[3] = 0;
  pUserBuffer[4] = 0;

  usLength += (SPI_HEADER_SIZE + ucPad);

  /* The magic number that resides at the end of the TX/RX buffer (1 byte after the allocated size)
   * for the purpose of overrun detection. If the magic number is overwritten - buffer overrun
   * occurred - and we will be stuck here forever! */
  if (wlan_tx_buffer[CC3000_TX_BUFFER_SIZE - 1] != CC3000_BUFFER_MAGIC_NUMBER)
  {
    DEBUGPRINT_F("\tCC3000: Error - No magic number found in SpiWrite\n\r");
    while (1);
  }

  if (sSpiInformation.ulSpiState == eSPI_STATE_POWERUP)
  {
    while (sSpiInformation.ulSpiState != eSPI_STATE_INITIALIZED);
  }

  if (sSpiInformation.ulSpiState == eSPI_STATE_INITIALIZED)
  {
    /* This is time for first TX/RX transactions over SPI: the IRQ is down - so need to send read buffer size command */
    SpiFirstWrite(pUserBuffer, usLength);
  }
  else
  {
    /* We need to prevent here race that can occur in case two back to back packets are sent to the
     * device, so the state will move to IDLE and once again to not IDLE due to IRQ */
    tSLInformation.WlanInterruptDisable();

    while (sSpiInformation.ulSpiState != eSPI_STATE_IDLE);

    sSpiInformation.ulSpiState = eSPI_STATE_WRITE_IRQ;
    sSpiInformation.pTxPacket = pUserBuffer;
    sSpiInformation.usTxPacketLength = usLength;

    /* Assert the CS line and wait till SSI IRQ line is active and then initialize write operation */
    CC3000_ASSERT_CS();

    /* Re-enable IRQ - if it was not disabled - this is not a problem... */
    tSLInformation.WlanInterruptEnable();

    /* Check for a missing interrupt between the CS assertion and enabling back the interrupts */
    if (tSLInformation.ReadWlanInterruptPin() == 0)
    {
      SpiWriteDataSynchronous(sSpiInformation.pTxPacket, sSpiInformation.usTxPacketLength);

      sSpiInformation.ulSpiState = eSPI_STATE_IDLE;

      CC3000_DEASSERT_CS();
    }
  }

  /* Due to the fact that we are currently implementing a blocking situation
   * here we will wait till end of transaction */
  while (eSPI_STATE_IDLE != sSpiInformation.ulSpiState);

  return(0);
}

/**************************************************************************/
/*!

 */
/**************************************************************************/
void SpiWriteDataSynchronous(unsigned char *data, unsigned short size)
{
  int bSend = 0, bRecv = 0;
  while (bSend<size || bRecv<size) {
    int r = pyb_cc3000_spi_send((bSend<size)?data[bSend]:-1);
    bSend++;
    if (bSend>0 && r>=0) bRecv++;
  }

  pyb_delay_us(10); // because of final clock pulse

  DEBUG_printf("SpiWriteDataSynchronous: data=%p size=%u bSend=%d bRecv=%d [%x %x %x %x]\n", data, size, bSend, bRecv, data[0], data[1], data[2], data[3]);
}

/**************************************************************************/
/*!

 */
/**************************************************************************/
void SpiReadDataSynchronous(unsigned char *data, unsigned short size)
{
  int bSend = 0, bRecv = 0;
  while (bSend<size || bRecv<size) {
    int r = pyb_cc3000_spi_send((bSend<size)?READ:-1);
    bSend++;
    if (bSend>0 && r>=0) data[bRecv++] = r;
  }

  pyb_delay_us(10); // because of final clock pulse

  DEBUG_printf("SpiReadDataSynchronous: data=%p size=%u bSend=%d bRecv=%d [%x %x %x %x]\n", data, size, bSend, bRecv, data[0], data[1], data[2], data[3]);
}

/**************************************************************************/
/*!

 */
/**************************************************************************/
void SpiReadHeader(void)
{
  DEBUGPRINT_F("\tCC3000: SpiReadHeader\n\r");

  SpiReadDataSynchronous(sSpiInformation.pRxPacket, HEADERS_SIZE_EVNT);
}

/**************************************************************************/
/*!

 */
/**************************************************************************/
long SpiReadDataCont(void)
{
  long data_to_recv;
  unsigned char *evnt_buff, type;

  DEBUGPRINT_F("\tCC3000: SpiReadDataCont\n\r");

  /* Determine what type of packet we have */
  evnt_buff =  sSpiInformation.pRxPacket;
  data_to_recv = 0;
  STREAM_TO_UINT8((uint8_t *)(evnt_buff + SPI_HEADER_SIZE), HCI_PACKET_TYPE_OFFSET, type);

  switch(type)
  {
    case HCI_TYPE_DATA:
      {
        /* We need to read the rest of data.. */
        STREAM_TO_UINT16((char *)(evnt_buff + SPI_HEADER_SIZE), HCI_DATA_LENGTH_OFFSET, data_to_recv);
        if (!((HEADERS_SIZE_EVNT + data_to_recv) & 1))
        {
          data_to_recv++;
        }

        if (data_to_recv)
        {
          SpiReadDataSynchronous(evnt_buff + HEADERS_SIZE_EVNT, data_to_recv);
        }
        break;
      }
    case HCI_TYPE_EVNT:
      {
        /* Calculate the rest length of the data */
        STREAM_TO_UINT8((char *)(evnt_buff + SPI_HEADER_SIZE), HCI_EVENT_LENGTH_OFFSET, data_to_recv);
        data_to_recv -= 1;

        /* Add padding byte if needed */
        if ((HEADERS_SIZE_EVNT + data_to_recv) & 1)
        {
          data_to_recv++;
        }

        if (data_to_recv)
        {
          SpiReadDataSynchronous(evnt_buff + HEADERS_SIZE_EVNT, data_to_recv);
        }

        sSpiInformation.ulSpiState = eSPI_STATE_READ_EOT;
        break;
      }
  }

  return (0);
}

/**************************************************************************/
/*!

 */
/**************************************************************************/
void SpiPauseSpi(void)
{
  DEBUGPRINT_F("\tCC3000: SpiPauseSpi\n\r");

  ccspi_int_enabled = 0;
    pyb_cc3000_pause_spi();
}

/**************************************************************************/
/*!

 */
/**************************************************************************/
void SpiResumeSpi(void)
{
  DEBUGPRINT_F("\tCC3000: SpiResumeSpi\n\r");

  ccspi_int_enabled = 1;
    pyb_cc3000_resume_spi();
}

/**************************************************************************/
/*!

 */
/**************************************************************************/
void SpiTriggerRxProcessing(void)
{
  DEBUGPRINT_F("\tCC3000: SpiTriggerRxProcessing\n\r");

  /* Trigger Rx processing */
  SpiPauseSpi();
  CC3000_DEASSERT_CS();

  //DEBUGPRINT_F("Magic?\n\r");
  /* The magic number that resides at the end of the TX/RX buffer (1 byte after the allocated size)
   * for the purpose of detection of the overrun. If the magic number is overriten - buffer overrun
   * occurred - and we will stuck here forever! */
  if (sSpiInformation.pRxPacket[CC3000_RX_BUFFER_SIZE - 1] != CC3000_BUFFER_MAGIC_NUMBER)
  {
    /* You've got problems if you're here! */
    DEBUGPRINT_F("\tCC3000: ERROR - magic number missing!\n\r");
    while (1);
  }

  //DEBUGPRINT_F("OK!\n\r");
  sSpiInformation.ulSpiState = eSPI_STATE_IDLE;
  sSpiInformation.SPIRxHandler(sSpiInformation.pRxPacket + SPI_HEADER_SIZE);
}

/**************************************************************************/
/*!

 */
/**************************************************************************/
void SSIContReadOperation(void)
{
  DEBUGPRINT_F("\tCC3000: SpiContReadOperation\n\r");
  
  /* The header was read - continue with  the payload read */
  if (!SpiReadDataCont())
  {
    /* All the data was read - finalize handling by switching to teh task
     *  and calling from task Event Handler */
    //DEBUGPRINT_F("SPItrig\n\r");
    SpiTriggerRxProcessing();
  }
}

/**************************************************************************/
/*!

 */
/**************************************************************************/
void WriteWlanPin( unsigned char val )
{  
#if 0
  if (DEBUG_MODE)
  {
    DEBUGPRINT_F("\tCC3000: WriteWlanPin - ");
    DEBUGPRINT_DEC(val);
    DEBUGPRINT_F("\n\r");
    delay(1);
  }
  if (val)
  {
    digitalWrite(g_vbatPin, HIGH);
  }
  else
  {
    digitalWrite(g_vbatPin, LOW);
  }
#endif
    pyb_cc3000_set_en(val == WLAN_ENABLE);
}

/**************************************************************************/
/*!

 */
/**************************************************************************/
long ReadWlanInterruptPin(void)
{
  DEBUGPRINT_F("\tCC3000: ReadWlanInterruptPin - ");
  DEBUGPRINT_DEC(digitalRead(g_irqPin));
  DEBUGPRINT_F("\n\r");

    return pyb_cc3000_get_irq();
}

/**************************************************************************/
/*!

 */
/**************************************************************************/
void WlanInterruptEnable()
{
  DEBUGPRINT_F("\tCC3000: WlanInterruptEnable.\n\r");
  // delay(100);
  ccspi_int_enabled = 1;
    pyb_cc3000_enable_irq();
}

/**************************************************************************/
/*!

 */
/**************************************************************************/
void WlanInterruptDisable()
{
  DEBUGPRINT_F("\tCC3000: WlanInterruptDisable\n\r");
  ccspi_int_enabled = 0;
    pyb_cc3000_disable_irq();
}

//*****************************************************************************
//
//! sendDriverPatch
//!
//! @param  pointer to the length
//!
//! @return none
//!
//! @brief  The function returns a pointer to the driver patch:
//!         since there is no patch in the host - it returns 0
//
//*****************************************************************************
char *sendDriverPatch(unsigned long *Length) {
  *Length = 0;
  return NULL;
}

//*****************************************************************************
//
//! sendBootLoaderPatch
//!
//! @param  pointer to the length
//!
//! @return none
//!
//! @brief  The function returns a pointer to the boot loader patch:
//!         since there is no patch in the host - it returns 0
//
//*****************************************************************************
char *sendBootLoaderPatch(unsigned long *Length) {
  *Length = 0;
  return NULL;
}

//*****************************************************************************
//
//! sendWLFWPatch
//!
//! @param  pointer to the length
//!
//! @return none
//!
//! @brief  The function returns a pointer to the FW patch:
//!         since there is no patch in the host - it returns 0
//
//*****************************************************************************
char *sendWLFWPatch(unsigned long *Length) {
  *Length = 0;
  return NULL;
}


/**************************************************************************/
/*!

 */
/**************************************************************************/

void SpiIntGPIOHandler(void)
{
    DEBUG_printf("SpiIntGPIOHandler\n");

  ccspi_is_in_irq = 1;

        if (sSpiInformation.ulSpiState == eSPI_STATE_POWERUP)
        {
            //This means IRQ line was low call a callback of HCI Layer to inform
            //on event
            sSpiInformation.ulSpiState = eSPI_STATE_INITIALIZED;
        }
        else if (sSpiInformation.ulSpiState == eSPI_STATE_IDLE)
        {
            sSpiInformation.ulSpiState = eSPI_STATE_READ_IRQ;

            /* IRQ line goes down - we are start reception */
            CC3000_ASSERT_CS();

            // Wait for TX/RX Compete which will come as DMA interrupt
            SpiReadHeader();

            sSpiInformation.ulSpiState = eSPI_STATE_READ_EOT;

            SSIContReadOperation();
        }
        else if (sSpiInformation.ulSpiState == eSPI_STATE_WRITE_IRQ)
        {
            SpiWriteDataSynchronous(sSpiInformation.pTxPacket, sSpiInformation.usTxPacketLength);

            sSpiInformation.ulSpiState = eSPI_STATE_IDLE;

            CC3000_DEASSERT_CS();
        }
  ccspi_is_in_irq = 0;
}

#if 0
void SPI_IRQ(void)
{
  ccspi_is_in_irq = 1;

  DEBUGPRINT_F("\tCC3000: Entering SPI_IRQ\n\r");
    
  if (sSpiInformation.ulSpiState == eSPI_STATE_POWERUP)
  {
    /* IRQ line was low ... perform a callback on the HCI Layer */
    sSpiInformation.ulSpiState = eSPI_STATE_INITIALIZED;
  }
  else if (sSpiInformation.ulSpiState == eSPI_STATE_IDLE)
  {
    //DEBUGPRINT_F("IDLE\n\r");
    sSpiInformation.ulSpiState = eSPI_STATE_READ_IRQ;    
    /* IRQ line goes down - start reception */

    CC3000_ASSERT_CS();

    // Wait for TX/RX Compete which will come as DMA interrupt
    SpiReadHeader();
    sSpiInformation.ulSpiState = eSPI_STATE_READ_EOT;
    //DEBUGPRINT_F("SSICont\n\r");
    SSIContReadOperation();
  }
  else if (sSpiInformation.ulSpiState == eSPI_STATE_WRITE_IRQ)
  {
    SpiWriteDataSynchronous(sSpiInformation.pTxPacket, sSpiInformation.usTxPacketLength);
    sSpiInformation.ulSpiState = eSPI_STATE_IDLE;
    CC3000_DEASSERT_CS();
  }

  DEBUGPRINT_F("\tCC3000: Leaving SPI_IRQ\n\r");

  ccspi_is_in_irq = 0;
  return;
}
#endif

//*****************************************************************************
//
//!  cc3k_int_poll
//!
//!  \brief               checks if the interrupt pin is low
//!                       just in case the hardware missed a falling edge
//!                       function is in ccspi.cpp
//
//*****************************************************************************

void cc3k_int_poll()
{
  if (pyb_cc3000_get_irq() == 0 && ccspi_is_in_irq == 0 && ccspi_int_enabled != 0) {
    SpiIntGPIOHandler();
  }
}

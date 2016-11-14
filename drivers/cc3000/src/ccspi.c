/*****************************************************************************
 *
 *  spi.c - CC3000 Host Driver Implementation.
 *  Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
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
 *    documentation and/or other materials provided with the distribution.
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

#include <string.h>

#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "pin.h"
#include "led.h"
#include "extint.h"
#include "spi.h"
#include "ccspi.h"
#include "evnt_handler.h"

#if 0 // print debugging info
#include <stdio.h>
#define DEBUG_printf(args...) printf(args)
#else // don't print debugging info
#define DEBUG_printf(args...) (void)0
#endif

// these need to be set to valid values before anything in this file will work
STATIC SPI_HandleTypeDef *SPI_HANDLE = NULL;
STATIC const pin_obj_t *PIN_CS = NULL;
STATIC const pin_obj_t *PIN_EN = NULL;
STATIC const pin_obj_t *PIN_IRQ = NULL;

#define CS_LOW()            HAL_GPIO_WritePin(PIN_CS->gpio, PIN_CS->pin_mask, GPIO_PIN_RESET)
#define CS_HIGH()           HAL_GPIO_WritePin(PIN_CS->gpio, PIN_CS->pin_mask, GPIO_PIN_SET)

#define READ                3
#define WRITE               1

#define HI(value)           (((value) & 0xFF00) >> 8)
#define LO(value)           ((value) & 0x00FF)

#define SPI_TIMEOUT         (1000)
#define HEADERS_SIZE_EVNT   (SPI_HEADER_SIZE + 5)

/* SPI bus states */
#define eSPI_STATE_POWERUP                  (0)
#define eSPI_STATE_INITIALIZED              (1)
#define eSPI_STATE_IDLE                     (2)
#define eSPI_STATE_WRITE_IRQ                (3)
#define eSPI_STATE_WRITE_FIRST_PORTION      (4)
#define eSPI_STATE_WRITE_EOT                (5)
#define eSPI_STATE_READ_IRQ                 (6)
#define eSPI_STATE_READ_FIRST_PORTION       (7)
#define eSPI_STATE_READ_EOT                 (8)

// The magic number that resides at the end of the TX/RX buffer (1 byte after the allocated size)
// for the purpose of detection of the overrun. The location of the memory where the magic number
// resides shall never be written. In case it is written - the overrun occured and either recevie function
// or send function will stuck forever.
#define CC3000_BUFFER_MAGIC_NUMBER (0xDE)

typedef struct {
    gcSpiHandleRx  SPIRxHandler;
    unsigned short usTxPacketLength;
    unsigned short usRxPacketLength;
    unsigned long  ulSpiState;
    unsigned char *pTxPacket;
    unsigned char *pRxPacket;
} tSpiInformation;
STATIC tSpiInformation sSpiInformation;

STATIC char spi_buffer[CC3000_RX_BUFFER_SIZE];
unsigned char wlan_tx_buffer[CC3000_TX_BUFFER_SIZE];

STATIC const mp_obj_fun_builtin_fixed_t irq_callback_obj;

// set the pins to use to communicate with the CC3000
// the arguments must be of type pin_obj_t* and SPI_HandleTypeDef*
void SpiInit(void *spi, const void *pin_cs, const void *pin_en, const void *pin_irq) {
    SPI_HANDLE = spi;
    PIN_CS = pin_cs;
    PIN_EN = pin_en;
    PIN_IRQ = pin_irq;
}

void SpiClose(void)
{
    if (sSpiInformation.pRxPacket) {
        sSpiInformation.pRxPacket = 0;
    }

    tSLInformation.WlanInterruptDisable();

    //HAL_SPI_DeInit(SPI_HANDLE);
}

void SpiOpen(gcSpiHandleRx pfRxHandler)
{
    DEBUG_printf("SpiOpen\n");

    /* initialize SPI state */
    sSpiInformation.ulSpiState = eSPI_STATE_POWERUP;
    sSpiInformation.SPIRxHandler = pfRxHandler;
    sSpiInformation.usTxPacketLength = 0;
    sSpiInformation.pTxPacket = NULL;
    sSpiInformation.pRxPacket = (unsigned char *)spi_buffer;
    sSpiInformation.usRxPacketLength = 0;
    spi_buffer[CC3000_RX_BUFFER_SIZE - 1] = CC3000_BUFFER_MAGIC_NUMBER;
    wlan_tx_buffer[CC3000_TX_BUFFER_SIZE - 1] = CC3000_BUFFER_MAGIC_NUMBER;

    /* SPI configuration */
    SPI_HANDLE->Init.Mode              = SPI_MODE_MASTER;
    SPI_HANDLE->Init.Direction         = SPI_DIRECTION_2LINES;
    SPI_HANDLE->Init.DataSize          = SPI_DATASIZE_8BIT;
    SPI_HANDLE->Init.CLKPolarity       = SPI_POLARITY_LOW;
    SPI_HANDLE->Init.CLKPhase          = SPI_PHASE_2EDGE;
    SPI_HANDLE->Init.NSS               = SPI_NSS_SOFT;
    SPI_HANDLE->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    SPI_HANDLE->Init.FirstBit          = SPI_FIRSTBIT_MSB;
    SPI_HANDLE->Init.TIMode            = SPI_TIMODE_DISABLED;
    SPI_HANDLE->Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLED;
    SPI_HANDLE->Init.CRCPolynomial     = 7;
    spi_init(SPI_HANDLE, false);

    // configure wlan CS and EN pins
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Alternate = 0;

    GPIO_InitStructure.Pin = PIN_CS->pin_mask;
    HAL_GPIO_Init(PIN_CS->gpio, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = PIN_EN->pin_mask;
    HAL_GPIO_Init(PIN_EN->gpio, &GPIO_InitStructure);

    HAL_GPIO_WritePin(PIN_CS->gpio, PIN_CS->pin_mask, GPIO_PIN_SET);
    HAL_GPIO_WritePin(PIN_EN->gpio, PIN_EN->pin_mask, GPIO_PIN_RESET);

    /* do a dummy read, this ensures SCLK is low before
       actual communications start, it might be required */
    CS_LOW();
    uint8_t buf[1];
    HAL_SPI_Receive(SPI_HANDLE, buf, sizeof(buf), SPI_TIMEOUT);
    CS_HIGH();

    // register EXTI
    extint_register((mp_obj_t)PIN_IRQ, GPIO_MODE_IT_FALLING, GPIO_PULLUP, (mp_obj_t)&irq_callback_obj, true);
    extint_enable(PIN_IRQ->pin);

    DEBUG_printf("SpiOpen finished; IRQ.pin=%d IRQ_LINE=%d\n", PIN_IRQ->pin, PIN_IRQ->pin);
}

long ReadWlanInterruptPin(void)
{
    return HAL_GPIO_ReadPin(PIN_IRQ->gpio, PIN_IRQ->pin_mask);
}

void WriteWlanPin(unsigned char val)
{
    HAL_GPIO_WritePin(PIN_EN->gpio, PIN_EN->pin_mask,
            (WLAN_ENABLE)? GPIO_PIN_SET:GPIO_PIN_RESET);
}

STATIC void SpiWriteDataSynchronous(unsigned char *data, unsigned short size)
{
    DEBUG_printf("SpiWriteDataSynchronous(data=%p [%x %x %x %x], size=%u)\n", data, data[0], data[1], data[2], data[3], size);
    __disable_irq();
    if (HAL_SPI_TransmitReceive(SPI_HANDLE, data, data, size, SPI_TIMEOUT) != HAL_OK) {
        //BREAK();
    }
    __enable_irq();
    DEBUG_printf(" - rx data = [%x %x %x %x]\n", data[0], data[1], data[2], data[3]);
}

STATIC void SpiReadDataSynchronous(unsigned char *data, unsigned short size)
{
    memset(data, READ, size);
    __disable_irq();
    if (HAL_SPI_TransmitReceive(SPI_HANDLE, data, data, size, SPI_TIMEOUT) != HAL_OK) {
       //BREAK();
    }
    __enable_irq();
}

STATIC void __delay_cycles(volatile int x)
{
    x *= 6; // for 168 MHz CPU
    while (x--);
}

STATIC long SpiFirstWrite(unsigned char *ucBuf, unsigned short usLength)
{
    DEBUG_printf("SpiFirstWrite %lu\n", sSpiInformation.ulSpiState);

    CS_LOW();

    // Assuming we are running on 24 MHz ~50 micro delay is 1200 cycles;
    __delay_cycles(1200);

    // SPI writes first 4 bytes of data
    SpiWriteDataSynchronous(ucBuf, 4);

    __delay_cycles(1200);

    SpiWriteDataSynchronous(ucBuf + 4, usLength - 4);

    // From this point on - operate in a regular way
    sSpiInformation.ulSpiState = eSPI_STATE_IDLE;

    CS_HIGH();

    return(0);
}

long SpiWrite(unsigned char *pUserBuffer, unsigned short usLength)
{
    DEBUG_printf("SpiWrite %lu\n", sSpiInformation.ulSpiState);

    unsigned char ucPad = 0;

    // Figure out the total length of the packet in order to figure out if there 
    // is padding or not
    if(!(usLength & 0x0001)) {
        ucPad++;
    }

    pUserBuffer[0] = WRITE;
    pUserBuffer[1] = HI(usLength + ucPad);
    pUserBuffer[2] = LO(usLength + ucPad);
    pUserBuffer[3] = 0;
    pUserBuffer[4] = 0;

    usLength += (SPI_HEADER_SIZE + ucPad);

    // The magic number that resides at the end of the TX/RX buffer (1 byte after the allocated size)
    // for the purpose of detection of the overrun. If the magic number is overriten - buffer overrun
    // occurred - and we will stuck here forever!
    if (wlan_tx_buffer[CC3000_TX_BUFFER_SIZE - 1] != CC3000_BUFFER_MAGIC_NUMBER) {
        while (1);
    }

    if (sSpiInformation.ulSpiState == eSPI_STATE_POWERUP) {
        while (sSpiInformation.ulSpiState != eSPI_STATE_INITIALIZED);
    }

    if (sSpiInformation.ulSpiState == eSPI_STATE_INITIALIZED) {
        // This is time for first TX/RX transactions over SPI:
        // the IRQ is down - so need to send read buffer size command
        SpiFirstWrite(pUserBuffer, usLength);
    } else {
        //
        // We need to prevent here race that can occur in case 2 back to back packets are sent to the
        // device, so the state will move to IDLE and once again to not IDLE due to IRQ
        //
        tSLInformation.WlanInterruptDisable();

        while (sSpiInformation.ulSpiState != eSPI_STATE_IDLE);

        sSpiInformation.ulSpiState = eSPI_STATE_WRITE_IRQ;
        sSpiInformation.pTxPacket = pUserBuffer;
        sSpiInformation.usTxPacketLength = usLength;

        // Assert the CS line and wait till SSI IRQ line is active and then initialize write operation
        CS_LOW();

        // Re-enable IRQ - if it was not disabled - this is not a problem...
        tSLInformation.WlanInterruptEnable();

        // check for a missing interrupt between the CS assertion and enabling back the interrupts
        if (tSLInformation.ReadWlanInterruptPin() == 0) {
            SpiWriteDataSynchronous(sSpiInformation.pTxPacket, sSpiInformation.usTxPacketLength);

            sSpiInformation.ulSpiState = eSPI_STATE_IDLE;

            CS_HIGH();
        }
    }

    // Due to the fact that we are currently implementing a blocking situation
    // here we will wait till end of transaction
    while (eSPI_STATE_IDLE != sSpiInformation.ulSpiState);

    return(0);
}

#if 0
unused
STATIC void SpiReadPacket(void)
{
    int length;

    /* read SPI header */
    SpiReadDataSynchronous(sSpiInformation.pRxPacket, SPI_HEADER_SIZE);

    /* parse data length  */
    STREAM_TO_UINT8(sSpiInformation.pRxPacket, SPI_HEADER_SIZE-1, length);

    /* read the remainder of the packet */
    SpiReadDataSynchronous(sSpiInformation.pRxPacket + SPI_HEADER_SIZE, length);

    sSpiInformation.ulSpiState = eSPI_STATE_READ_EOT;
}
#endif

STATIC void SpiReadHeader(void)
{
    SpiReadDataSynchronous(sSpiInformation.pRxPacket, 10);
}

STATIC void SpiTriggerRxProcessing(void)
{
    SpiPauseSpi();
    CS_HIGH();

    // The magic number that resides at the end of the TX/RX buffer (1 byte after the allocated size)
    // for the purpose of detection of the overrun. If the magic number is overriten - buffer overrun
    // occurred - and we will stuck here forever!
    if (sSpiInformation.pRxPacket[CC3000_RX_BUFFER_SIZE - 1] != CC3000_BUFFER_MAGIC_NUMBER) {
        while (1);
    }

    sSpiInformation.ulSpiState = eSPI_STATE_IDLE;
    sSpiInformation.SPIRxHandler(sSpiInformation.pRxPacket + SPI_HEADER_SIZE);
}


STATIC long SpiReadDataCont(void)
{
    long data_to_recv=0;
    unsigned char *evnt_buff, type;

    //determine what type of packet we have
    evnt_buff =  sSpiInformation.pRxPacket;
    STREAM_TO_UINT8((char *)(evnt_buff + SPI_HEADER_SIZE), HCI_PACKET_TYPE_OFFSET, type);

    switch (type) {
        case HCI_TYPE_DATA:{
                // We need to read the rest of data..
                STREAM_TO_UINT16((char *)(evnt_buff + SPI_HEADER_SIZE),
                        HCI_DATA_LENGTH_OFFSET, data_to_recv);
                if (!((HEADERS_SIZE_EVNT + data_to_recv) & 1)) {
                    data_to_recv++;
                }

                if (data_to_recv) {
                    SpiReadDataSynchronous(evnt_buff + 10, data_to_recv);
                }
                break;
            }
        case HCI_TYPE_EVNT: {
                // Calculate the rest length of the data
                STREAM_TO_UINT8((char *)(evnt_buff + SPI_HEADER_SIZE),
                        HCI_EVENT_LENGTH_OFFSET, data_to_recv);
                data_to_recv -= 1;

                // Add padding byte if needed
                if ((HEADERS_SIZE_EVNT + data_to_recv) & 1) {
                    data_to_recv++;
                }

                if (data_to_recv) {
                    SpiReadDataSynchronous(evnt_buff + 10, data_to_recv);
                }

                sSpiInformation.ulSpiState = eSPI_STATE_READ_EOT;
                break;
            }
    }

    return 0;
}

STATIC void SSIContReadOperation(void)
{
    // The header was read - continue with  the payload read
    if (!SpiReadDataCont()) {
        /* All the data was read - finalize handling by switching
           to the task and calling from task Event Handler */
        SpiTriggerRxProcessing();
    }
}

STATIC mp_obj_t irq_callback(mp_obj_t line) {
    DEBUG_printf("<< IRQ; state=%lu >>\n", sSpiInformation.ulSpiState);
    switch (sSpiInformation.ulSpiState) {
        case eSPI_STATE_POWERUP:
            /* This means IRQ line was low call a callback of HCI Layer to inform on event */
            DEBUG_printf(" - POWERUP\n");
            sSpiInformation.ulSpiState = eSPI_STATE_INITIALIZED;
            break;
        case eSPI_STATE_IDLE:
            DEBUG_printf(" - IDLE\n");
            sSpiInformation.ulSpiState = eSPI_STATE_READ_IRQ;

            /* IRQ line goes down - we are start reception */
            CS_LOW();

            // Wait for TX/RX Compete which will come as DMA interrupt
            SpiReadHeader();

            sSpiInformation.ulSpiState = eSPI_STATE_READ_EOT;

            SSIContReadOperation();
            break;
        case eSPI_STATE_WRITE_IRQ:
            DEBUG_printf(" - WRITE IRQ\n");
            SpiWriteDataSynchronous(sSpiInformation.pTxPacket, sSpiInformation.usTxPacketLength);

            sSpiInformation.ulSpiState = eSPI_STATE_IDLE;

            CS_HIGH();
            break;
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(irq_callback_obj, irq_callback);

void SpiPauseSpi(void) {
    DEBUG_printf("SpiPauseSpi\n");
    extint_disable(PIN_IRQ->pin);
}

void SpiResumeSpi(void) {
    DEBUG_printf("SpiResumeSpi\n");
    extint_enable(PIN_IRQ->pin);
}

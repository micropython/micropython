/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "device.h"
#include "main.h"
#include "spi_api.h"
#include "spi_ex_api.h"

#define SPI_IS_AS_MASTER    1
#define TEST_BUF_SIZE       2048
#define SCLK_FREQ           1000000
#define SPI_DMA_DEMO        0
#define TEST_LOOP           100

// SPI0
#define SPI0_MOSI  PC_2
#define SPI0_MISO  PC_3
#define SPI0_SCLK  PC_1
#define SPI0_CS    PC_0

_LONG_CALL_ extern
void __rtl_memDump_v1_00(const u8 *start, u32 size, char * strHeader);
extern void wait_ms(u32);

char TestBuf[TEST_BUF_SIZE];
volatile int TrDone;

void master_tr_done_callback(void *pdata, SpiIrq event)
{
    TrDone = 1;
}

void slave_tr_done_callback(void *pdata, SpiIrq event)
{
    TrDone = 1;
}

#if SPI_IS_AS_MASTER
spi_t spi_master;
#else
spi_t spi_slave;
#endif
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
    int Counter = 0;
    int i;

#if SPI_IS_AS_MASTER
    spi_init(&spi_master, SPI0_MOSI, SPI0_MISO, SPI0_SCLK, SPI0_CS);
    spi_frequency(&spi_master, SCLK_FREQ);
    spi_format(&spi_master, 16, (SPI_SCLK_IDLE_LOW|SPI_SCLK_TOGGLE_MIDDLE) , 0);
    // wait Slave ready
    wait_ms(1000);

    while (Counter < TEST_LOOP) {
        DBG_8195A("======= Test Loop %d =======\r\n", Counter);

        for (i=0;i<TEST_BUF_SIZE;i++) {
            TestBuf[i] = i;
        }

        spi_irq_hook(&spi_master, master_tr_done_callback, (uint32_t)&spi_master);
        DBG_8195A("SPI Master Write Test==>\r\n");
        TrDone = 0;
#if SPI_DMA_DEMO
        spi_master_write_stream_dma(&spi_master, TestBuf, TEST_BUF_SIZE);
#else
        spi_master_write_stream(&spi_master, TestBuf, TEST_BUF_SIZE);
#endif
        i=0;
        DBG_8195A("SPI Master Wait Write Done...\r\n");
        while(TrDone == 0) {
            wait_ms(10);
            i++;
        }
        DBG_8195A("SPI Master Write Done!!\r\n");

        DBG_8195A("SPI Master Read Test==>\r\n");
        DBG_8195A("Wait 5 sec for SPI Slave get ready...\r\n");
        for (i=0;i<5;i++) {
            wait_ms(1000);
        }

        _memset(TestBuf, 0, TEST_BUF_SIZE);
        spi_flush_rx_fifo(&spi_master);

        TrDone = 0;
#if SPI_DMA_DEMO
        spi_master_read_stream_dma(&spi_master, TestBuf, TEST_BUF_SIZE);
#else
        spi_master_read_stream(&spi_master, TestBuf, TEST_BUF_SIZE);
#endif
        i=0;
        DBG_8195A("SPI Master Wait Read Done...\r\n");
        while(TrDone == 0) {
            wait_ms(10);
            i++;
        }
        DBG_8195A("SPI Master Read Done!!\r\n");
        __rtl_memDump_v1_00(TestBuf, TEST_BUF_SIZE, "SPI Master Read Data:");
        Counter++;
    }
    spi_free(&spi_master);
    DBG_8195A("SPI Master Test <==\r\n");

#else
    spi_init(&spi_slave, SPI0_MOSI, SPI0_MISO, SPI0_SCLK, SPI0_CS);
    spi_format(&spi_slave, 16, (SPI_SCLK_IDLE_LOW|SPI_SCLK_TOGGLE_MIDDLE) , 1);

    while (spi_busy(&spi_slave)) {
        DBG_8195A("Wait SPI Bus Ready...\r\n");
        wait_ms(1000);
    }

    while (Counter < TEST_LOOP) {
        DBG_8195A("======= Test Loop %d =======\r\n", Counter);
        _memset(TestBuf, 0, TEST_BUF_SIZE);
        DBG_8195A("SPI Slave Read Test ==>\r\n");
        spi_irq_hook(&spi_slave, slave_tr_done_callback, (uint32_t)&spi_slave);
        TrDone = 0;
        spi_flush_rx_fifo(&spi_slave);
#if SPI_DMA_DEMO
        spi_slave_read_stream_dma(&spi_slave, TestBuf, TEST_BUF_SIZE);
#else
        spi_slave_read_stream(&spi_slave, TestBuf, TEST_BUF_SIZE);
#endif
        i=0;
        DBG_8195A("SPI Slave Wait Read Done...\r\n");
        while(TrDone == 0) {
            wait_ms(100);
            i++;
            if (i>150) {
                DBG_8195A("SPI Slave Wait Timeout\r\n");
                break;
            }
        }
        __rtl_memDump_v1_00(TestBuf, TEST_BUF_SIZE, "SPI Slave Read Data:");

        // Slave Write Test
        DBG_8195A("SPI Slave Write Test ==>\r\n");
        TrDone = 0;
#if SPI_DMA_DEMO
        spi_slave_write_stream_dma(&spi_slave, TestBuf, TEST_BUF_SIZE);
#else
        spi_slave_write_stream(&spi_slave, TestBuf, TEST_BUF_SIZE);
#endif
        i=0;
        DBG_8195A("SPI Slave Wait Write Done...\r\n");
        while(TrDone == 0) {
            wait_ms(100);
            i++;
            if (i> 200) {
                DBG_8195A("SPI Slave Write Timeout...\r\n");
                break;
            }
        }
        DBG_8195A("SPI Slave Write Done!!\r\n");
        Counter++;
    }
    spi_free(&spi_slave);
#endif

    DBG_8195A("SPI Demo finished.\n");
    for(;;);
}

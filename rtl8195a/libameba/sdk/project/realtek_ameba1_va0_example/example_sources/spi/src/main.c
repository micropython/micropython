/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "device.h"
#include "diag.h"
#include "main.h"
#include "spi_api.h"

#define FakeMbedAPI  1

// SPI0 (S0)
#define SPI0_MOSI  PC_2
#define SPI0_MISO  PC_3
#define SPI0_SCLK  PC_1
#define SPI0_CS    PC_0

// SPI1 (S1)
#define SPI1_MOSI  PB_6
#define SPI1_MISO  PB_7
#define SPI1_SCLK  PB_5
#define SPI1_CS    PB_4

#if 1
// SPI2 (S2) for DEV 3V0
// Please note that PA_7 need pull high before using GPIOA group
#define SPI2_MOSI  PA_1
#define SPI2_MISO  PA_0
#define SPI2_SCLK  PA_2
#define SPI2_CS    PA_4
#else
// SPI2 (S2)
#define SPI2_MOSI  PD_2
#define SPI2_MISO  PD_3
#define SPI2_SCLK  PD_1
#define SPI2_CS    PD_0
#endif

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
spi_t spi_master;
spi_t spi_slave;

void main(void)
{
#if FakeMbedAPI

    /* SPI0 is as Slave */
    //SPI0_IS_AS_SLAVE = 1;

    spi_init(&spi_master, SPI2_MOSI, SPI2_MISO, SPI2_SCLK, SPI2_CS);
	spi_format(&spi_master, 8, 0, 0);
	spi_frequency(&spi_master, 200000);
    spi_init(&spi_slave,  SPI0_MOSI, SPI0_MISO, SPI0_SCLK, SPI0_CS);
	spi_format(&spi_slave, 8, 0, 1);

    int TestingTimes = 10;
    int Counter      = 0;
    int TestData     = 0;
    int ReadData     = 0;

    int result = 1;

    /**
     * Master read/write, Slave read/write
     */
    DBG_SSI_INFO("--------------------------------------------------------\n");
    for(Counter = 0, TestData=0x01; Counter < TestingTimes; Counter++)
    {
        ReadData = spi_master_write(&spi_master, TestData);
        DBG_SSI_INFO("Master write: %02X, read: %02X\n", TestData, ReadData);
        if (TestData - 1 != ReadData) {
            result = 0;
        }

        TestData++;

        spi_slave_write(&spi_slave, TestData);
        ReadData = spi_slave_read(&spi_slave);
        DBG_SSI_INFO(ANSI_COLOR_CYAN"Slave  write: %02X, read: %02X\n"ANSI_COLOR_RESET, TestData, ReadData);
        if (TestData - 1 != ReadData) {
            result = 0;
        }

        TestData++;
    }

    /**
     * Master write, Slave read
     */
    DBG_SSI_INFO("--------------------------------------------------------\n");
    for(Counter = 0, TestData=0xFF; Counter < TestingTimes; Counter++)
    {
        spi_master_write(&spi_master, TestData);
        ReadData = spi_slave_read(&spi_slave);
        DBG_SSI_INFO("Master write: %02X\n", TestData);
        DBG_SSI_INFO(ANSI_COLOR_CYAN"Slave  read : %02X\n"ANSI_COLOR_RESET, ReadData);
        if (TestData != ReadData) {
            result = 0;
        }

        TestData--;
    }

    spi_free(&spi_master);
    spi_free(&spi_slave);

    DBG_SSI_INFO("SPI Demo finished.\n");

    printf("\r\nResult is %s\r\n", (result) ? "success" : "fail");

    for(;;);

#else  // mbed SPI API emulation

#endif

}


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

#define SPI_IS_AS_MASTER 1

// SPI0
#define SPI0_MOSI  PC_2
#define SPI0_MISO  PC_3
#define SPI0_SCLK  PC_1
#define SPI0_CS    PC_0

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
    int TestingTimes = 10;
    int Counter      = 0;
    int TestData     = 0;

#if SPI_IS_AS_MASTER
    spi_t spi_master;

    SPI0_IS_AS_SLAVE = 0;
    spi_init(&spi_master, SPI0_MOSI, SPI0_MISO, SPI0_SCLK, SPI0_CS);

    DBG_SSI_INFO("--------------------------------------------------------\n");
    for(Counter = 0, TestData=0xFF; Counter < TestingTimes; Counter++) {
        spi_master_write(&spi_master, TestData);
        DBG_SSI_INFO("Master write: %02X\n", TestData);
        TestData--;
    }
    spi_free(&spi_master);

#else
    spi_t spi_slave;

    SPI0_IS_AS_SLAVE = 1;
    spi_init(&spi_slave,  SPI0_MOSI, SPI0_MISO, SPI0_SCLK, SPI0_CS);

    DBG_SSI_INFO("--------------------------------------------------------\n");
    for(Counter = 0, TestData=0xFF; Counter < TestingTimes; Counter++) {
        DBG_SSI_INFO(ANSI_COLOR_CYAN"Slave  read : %02X\n"ANSI_COLOR_RESET,
                spi_slave_read(&spi_slave));
        TestData--;
    }
    spi_free(&spi_slave);
#endif

    DBG_SSI_INFO("SPI Demo finished.\n");
    for(;;);
}

/*
	File defining the QSPI configuration to be used by the QSPI Flash in our microcontroller.
	
	TODO: Check this file once again together with main.c in order to simulate an even closer behavior to Tiger 2 Plus
	
	Author: GabSi
	Year: 2024
*/

#include "storage.h"
#include "qspi.h"
#include "spi.h"

#if MICROPY_HW_SPIFLASH_ENABLE_CACHE
// Shared cache for first and second SPI block devices
static mp_spiflash_cache_t spi_bdev_cache;
#endif

const mp_spiflash_config_t spiflash_config = //defined in /drivers/memory/spiflash
{
    .bus_kind = MP_SPIFLASH_BUS_QSPI,
    .bus.u_qspi.data = NULL, //maybe initial commands to be sent?
    .bus.u_qspi.proto = &qspi_proto, //defined in qspi.h
    #if MICROPY_HW_SPIFLASH_ENABLE_CACHE
    .cache = &spi_bdev_cache,
    #endif
};

spi_bdev_t spi_bdev; //defined in storage.h

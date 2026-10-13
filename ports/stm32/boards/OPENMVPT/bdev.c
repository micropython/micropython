#include "qspi.h"
#include "storage.h"

// Shared cache for first and second SPI block devices
static mp_spiflash_cache_t spi_bdev_cache;

const mp_spiflash_config_t spiflash_config = {
    .bus_kind = MP_SPIFLASH_BUS_QSPI,
    .bus.u_qspi.data = NULL,
    .bus.u_qspi.proto = &qspi_proto,
    .cache = &spi_bdev_cache,
};

spi_bdev_t spi_bdev;

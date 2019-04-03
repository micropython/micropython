#include "storage.h"
#include "qspi.h"

// Shared cache for first and second SPI block devices
STATIC mp_spiflash_cache_t spi_bdev_cache;

#if 0

// First external SPI flash uses software SPI interface

STATIC const mp_soft_spi_obj_t soft_spi_bus = {
    .delay_half = MICROPY_HW_SOFTSPI_MIN_DELAY,
    .polarity = 0,
    .phase = 0,
    .sck = MICROPY_HW_SPIFLASH_SCK,
    .mosi = MICROPY_HW_SPIFLASH_IO0,
    .miso = MICROPY_HW_SPIFLASH_IO1,
};

const mp_spiflash_config_t spiflash_config = {
    .bus_kind = MP_SPIFLASH_BUS_SPI,
    .bus.u_spi.cs = MICROPY_HW_SPIFLASH_CS,
    .bus.u_spi.data = (void*)&soft_spi_bus,
    .bus.u_spi.proto = &mp_soft_spi_proto,
    .cache = &spi_bdev_cache,
};

spi_bdev_t spi_bdev;

#else

// First external SPI flash uses software QSPI interface

STATIC const mp_soft_qspi_obj_t soft_qspi_bus = {
    .cs = MICROPY_HW_SPIFLASH_CS,
    .clk = MICROPY_HW_SPIFLASH_SCK,
    .io0 = MICROPY_HW_SPIFLASH_IO0,
    .io1 = MICROPY_HW_SPIFLASH_IO1,
    .io2 = MICROPY_HW_SPIFLASH_IO2,
    .io3 = MICROPY_HW_SPIFLASH_IO3,
};

const mp_spiflash_config_t spiflash_config = {
    .bus_kind = MP_SPIFLASH_BUS_QSPI,
    .bus.u_qspi.data = (void*)&soft_qspi_bus,
    .bus.u_qspi.proto = &mp_soft_qspi_proto,
    .cache = &spi_bdev_cache,
};

spi_bdev_t spi_bdev;

#endif

// Second external SPI flash uses hardware QSPI interface

const mp_spiflash_config_t spiflash2_config = {
    .bus_kind = MP_SPIFLASH_BUS_QSPI,
    .bus.u_qspi.data = NULL,
    .bus.u_qspi.proto = &qspi_proto,
    .cache = &spi_bdev_cache,
};

spi_bdev_t spi_bdev2;

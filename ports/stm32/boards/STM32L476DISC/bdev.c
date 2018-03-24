#include "storage.h"
#include "genhdr/pins.h"

// External SPI flash uses standard SPI interface

const mp_soft_spi_obj_t soft_spi_bus = {
    .delay_half = MICROPY_HW_SOFTSPI_MIN_DELAY,
    .polarity = 0,
    .phase = 0,
    .sck = &MICROPY_HW_SPIFLASH_SCK,
    .mosi = &MICROPY_HW_SPIFLASH_MOSI,
    .miso = &MICROPY_HW_SPIFLASH_MISO,
};

const mp_spiflash_config_t spiflash_config = {
    .bus_kind = MP_SPIFLASH_BUS_SPI,
    .bus.u_spi.cs = &MICROPY_HW_SPIFLASH_CS,
    .bus.u_spi.data = (void*)&soft_spi_bus,
    .bus.u_spi.proto = &mp_soft_spi_proto,
};

spi_bdev_t spi_bdev;

#include "drivers/memory/spiflash.h"
#include "qspi.h"

// This configuration is needed for mboot to be able to write to the external QSPI flash

const mp_spiflash_config_t spiflash_config = {
    .bus_kind = MP_SPIFLASH_BUS_QSPI,
    .bus.u_qspi.data = NULL,
    .bus.u_qspi.proto = &qspi_proto,
    .cache = NULL,
};

mp_spiflash_t spiflash_instance;

// This init function is needed to memory map the QSPI flash early in the boot process

void board_early_init(void) {
    qspi_init();
    qspi_memory_map();
}

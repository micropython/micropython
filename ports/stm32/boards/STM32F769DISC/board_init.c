#include "drivers/memory/spiflash.h"
#include "drivers/bus/qspi.h"

// This configuration is needed for mboot to be able to write to the external QSPI flash
extern const mp_qspi_proto_t qspi_proto;

mp_spiflash_t spiflash_instance = {
    .qspi_proto = &qspi_proto
};

// This init function is needed to memory map the QSPI flash early in the boot process

void board_early_init(void) {
    qspi_init();
    qspi_memory_map();
}

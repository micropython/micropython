// flash_ws.c - Flash wait-state configuration for the GD32F4xx port.
//
// The GD32 standard peripheral library's system_gd32f4xx.c switches the core
// clock to 168/240 MHz without ever programming FMC_WS, leaving WSCNT at its
// reset value of 0.  At those frequencies the Flash cannot respond in time,
// so the wait states must be programmed before the PLL becomes the clock
// source.
//
// This file runs from board_early_init(), which main() calls before the
// first use of the peripheral library.  Because SystemInit() has already
// raised the clock by then, the sequence below raises the wait states to the
// safe maximum for the running frequency as early as possible.

#include "py/mpconfig.h"
#include "gd32f4xx.h"
#include "gd32f4xx_fmc.h"
#include "board.h"

#ifndef MICROPY_HW_FLASH_WS
// Fall back to the value for the slowest supported clock so that an
// unconfigured board still runs (slower, but correct).
#define MICROPY_HW_FLASH_WS (5)
#endif

void board_flash_ws_init(void) {
    // Sector erase / programming is not needed here, so no unlock is required:
    // FMC_WS is writable once the FMC is unlocked for a modify operation.
    // GD32 requires the FMC to be unlocked to change WSCNT.
    fmc_unlock();

    // Set the wait states for the target core frequency.
    // WS_WSCNT_0 .. WS_WSCNT_15 map to 0..15 wait states.
    fmc_wscnt_set(MICROPY_HW_FLASH_WS);

    fmc_lock();
}

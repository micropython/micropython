// board.h - board-level definitions for GD32 port
#ifndef MICROPY_INCLUDED_GD32_BOARD_H
#define MICROPY_INCLUDED_GD32_BOARD_H

#include "py/mpconfig.h"

// board config provides MICROPY_HW_BOARD_NAME / MCU_NAME / clocks
#include "mpconfigboard.h"

void board_early_init(void);
void board_init(void);

#endif

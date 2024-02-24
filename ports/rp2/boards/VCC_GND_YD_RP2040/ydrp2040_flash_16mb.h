// A pico-sdk board definition is required since the boards are
// not officially supported.
//
// Officially supported boards:
//     https://github.com/raspberrypi/pico-sdk/tree/master/src/boards/include/boards

#ifndef _BOARDS_YDRP2040_FLASH_16MB_H
#define _BOARDS_YDRP2040_FLASH_16MB_H

#include "ydrp2040_common.h"

#define VCC_GND_YD_RP2040_16MB

#ifndef PICO_FLASH_SIZE_BYTES
#define PICO_FLASH_SIZE_BYTES (16 * 1024 * 1024)
#endif

#endif

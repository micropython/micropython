// A pico-sdk board definition is required since the WeAct Studio boards are
// not officially supported.
//
// Officially supported boards:
//     https://github.com/raspberrypi/pico-sdk/tree/master/src/boards/include/boards

#ifndef _BOARDS_WEACTSTUDIO_8MB_H
#define _BOARDS_WEACTSTUDIO_8MB_H

#include "weactstudio_common.h"

#define WEACTSTUDIO_8MB

#ifndef PICO_FLASH_SIZE_BYTES
#define PICO_FLASH_SIZE_BYTES (8 * 1024 * 1024)
#endif

#endif

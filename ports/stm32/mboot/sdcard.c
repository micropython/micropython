// Include relevant source files for SD card only when it's needed and
// configured at the C level (in mpconfigboard.h).

#include "py/mpconfig.h"

#if defined(MBOOT_SDCARD_ADDR)

#define MICROPY_HW_DMA_ENABLE_AUTO_TURN_OFF (0)

#include "ports/stm32/dma.c"
#include "ports/stm32/sdcard.c"

#endif

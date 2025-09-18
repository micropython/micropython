// -----------------------------------------------------
// NOTE: THIS HEADER IS ALSO INCLUDED BY ASSEMBLER SO
//       SHOULD ONLY CONSIST OF PREPROCESSOR DIRECTIVES
// -----------------------------------------------------

// pico_cmake_set PICO_PLATFORM=rp2350

#ifndef _BOARDS_WIZNET_W5100S_EVB_PICO2_H
#define _BOARDS_WIZNET_W5100S_EVB_PICO2_H

// For board detection
#define WIZNET_W5100S_EVB_PICO2

#include "boards/pico2.h"

// --- BOARD SPECIFIC ---
#ifndef W5100S_EVB_PICO_INTN_PIN
#define W5100S_EVB_PICO_INTN_PIN 21
#endif

#ifndef W5100S_EVB_PICO_RSTN_PIN
#define W5100S_EVB_PICO_RSTN_PIN 20
#endif

#ifndef W5100S_EVB_PICO_A0_PIN
#define W5100S_EVB_PICO_A0_PIN 26
#endif
#ifndef W5100S_EVB_PICO_A1_PIN
#define W5100S_EVB_PICO_A1_PIN 27
#endif
#ifndef W5100S_EVB_PICO_A2_PIN
#define W5100S_EVB_PICO_A2_PIN 28
#endif


// pico_cmake_set_default PICO_FLASH_SIZE_BYTES = (2 * 1024 * 1024)
#ifndef PICO_FLASH_SIZE_BYTES
#define PICO_FLASH_SIZE_BYTES (2 * 1024 * 1024)
#endif

#endif

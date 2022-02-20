#define MICROPY_HW_BOARD_NAME "Wio Terminal D51R"
#define MICROPY_HW_MCU_NAME   "SAMD51P19A"

#define MICROPY_FLOAT_IMPL              (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_PY_BUILTINS_COMPLEX     (0)
#define MICROPY_PY_MATH                 (0)
#define MICROPY_PY_CMATH                (0)

// MicroPython configs
// samd_flash.c flash parameters
// Build a 128k Flash storage at top. 512k-128k=384k=0x60000
// 512*1024= 0x80000 minus 128*1024= 0x20000 =  0x60000
#define MICROPY_HW_FLASH_STORAGE_BASE       (0x60000)
#define MICROPY_HW_FLASH_STORAGE_BYTES      (0x1FFFF)
#define VFS_BLOCK_SIZE_BYTES                (1536) // 24x 64B flash pages;

// ASF4 MCU package specific Pin definitions
#include "samd51p19a.h"

// Please consult the SAM_D51 Datasheet, I/O Multiplexing and Considerations.
// WIO_Terminal USART pin assignments: Tx=BCM14=PB27=SERCOM2/PAD[0], Rx=BCM15=PB26=SERCOM2/PAD[1]
#define CPU_FREQ (48000000) // For selecting Baud from clock.
#define MP_PIN_GRP 1 // A-D=0-3
#define MP_TX_PIN 27
#define MP_RX_PIN 26 // 'n'
#define MP_PERIPHERAL_MUX 13 // 'n'th group of 2 pins
#define USARTx SERCOM2 //
#define MP_PORT_FUNC 0x22 // Sets PMUXE & PMUXO to the Alternative Function.A-N=0-13
#define MP_RXPO_PAD 1 // RXPO- Receive Data Pinout
#define MP_TXPO_PAD 0 // TXPO- Tranmit Data Pinout
#define MP_SERCOMx SERCOM2_ // APBCMASK
#define MP_SERCOM_GCLK_ID_x_CORE SERCOM2_GCLK_ID_CORE

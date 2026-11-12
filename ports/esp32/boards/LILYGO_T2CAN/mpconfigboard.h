#define MICROPY_HW_BOARD_NAME               "LILYGO T-2CAN"
#define MICROPY_HW_MCU_NAME                 "ESP32-S3"
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-t2can"

// Native TWAI (machine.CAN) — "CAN-B" channel.
// The ESP32-S3 TWAI peripheral drives an on-board isolated transceiver
// (TD501MCAN) wired to the channel-B screw terminal, so machine.CAN works with
// no external hardware.
// Verified against the LilyGo T-2Can repo (libraries/private_library/pin_config.h
// and examples/esp32_can/esp32_can.ino): TX = GPIO7, RX = GPIO6.
#define MICROPY_HW_ENABLE_CAN               (1)
#define MICROPY_HW_CAN1_TX                  (7)
#define MICROPY_HW_CAN1_RX                  (6)

// SPI bus to the "CAN-A" controller (MCP2515 on the classic SKU, MCP2518FD on
// the -Fd SKU). Provided for convenience; driving CAN-A needs a separate
// MCP25xx SPI driver and is out of scope for the native-TWAI machine.CAN.
#define MICROPY_HW_SPI1_SCK                 (12)
#define MICROPY_HW_SPI1_MOSI                (11)
#define MICROPY_HW_SPI1_MISO                (13)

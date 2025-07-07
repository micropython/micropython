// RAK11310 specific configurations

#define MICROPY_HW_BOARD_NAME          "RAKwireless RAK11310" // Changed
#define MICROPY_HW_MCU_NAME            "rp2040"

// Flash size: Assuming 2MB. Please verify from official datasheet if possible.
// Assumed to be same as RAK11300.
#define MICROPY_HW_FLASH_STORAGE_BYTES (2 * 1024 * 1024)

// USB VID/PID: Using Raspberry Pi Pico defaults.
// RAKwireless may have its own VID, but it's not readily available.
// Assumed to be same as RAK11300.
#define MICROPY_HW_USB_VID (0x2E8A)
#define MICROPY_HW_USB_PID (0x0005) // Pico PID

// On-board SX1262 LoRa chip connections (User Updated Pinout, identical to RAK11300)
#define MICROPY_HW_SX1262_SPI_ID    1    // SPI1
#define MICROPY_HW_SX1262_SPI_SCK   10   // GPIO10 (LORA_CLK)
#define MICROPY_HW_SX1262_SPI_MOSI  11   // GPIO11 (LORA_MOSI)
#define MICROPY_HW_SX1262_SPI_MISO  12   // GPIO12 (LORA_MISO)
#define MICROPY_HW_SX1262_NSS       13   // GPIO13 (LORA_CS)
#define MICROPY_HW_SX1262_NRESET    14   // GPIO14 (LORA_RST)
#define MICROPY_HW_SX1262_DIO1      29   // GPIO29 (LORA_IRQ) - Note: ADC capable pin
#define MICROPY_HW_SX1262_BUSY      15   // GPIO15 (LORA_GPIO, assumed BUSY)

// Power Enable for SX1262 module (Identical to RAK11300)
#define MICROPY_HW_SX1262_PWR_EN    25   // GPIO25 (LORA_POWER_EN)

// Default UART / I2C assignments (Identical to RAK11300)
// UART0
#define MICROPY_HW_UART0_TX  (0)
#define MICROPY_HW_UART0_RX  (1)
// I2C0
#define MICROPY_HW_I2C0_SCL (5)
#define MICROPY_HW_I2C0_SDA (4)

// No specific user LED defined from available info. Add if known:
// #define MICROPY_HW_LED_STATUS      (pin_GPx)

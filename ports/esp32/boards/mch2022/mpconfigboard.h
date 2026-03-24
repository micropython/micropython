// MCH2022 MicroPython board configuration
// Hardware: ESP32-WROVER-E-N16R8 (16MB flash, 8MB PSRAM)

#define MICROPY_HW_BOARD_NAME               "MCH2022"
#define MICROPY_HW_MCU_NAME                 "ESP32-WROVER-E"

// Enable PSRAM (8MB)
#define MICROPY_HW_ENABLE_PSRAM             (1)

// UART0 is the only REPL path (USB is handled by RP2040)
#define MICROPY_HW_UART_REPL                (0)
#define MICROPY_HW_UART_REPL_BAUD           (115200)
#define MICROPY_HW_UART_REPL_TX             (1)
#define MICROPY_HW_UART_REPL_RX             (3)

// Status LED (if desired; MCH2022 has RGB via FPGA, so disable)
#define MICROPY_HW_LED_STATUS               (-1)

// SD-card slot available
#define MICROPY_HW_ENABLE_SDCARD            (1)

// WiFi + BLE supported by ESP32-WROVER-E
#define MICROPY_PY_NETWORK                  (0)
#define MICROPY_PY_BLUETOOTH                (0)

// Default I2C bus (for sensors: BNO055, BME680)
#define MICROPY_HW_I2C0_SCL                 (GPIO_NUM_22)
#define MICROPY_HW_I2C0_SDA                 (GPIO_NUM_21)

// SPI bus (for SD-card or peripherals)
#define MICROPY_HW_SPI1_SCK                 (GPIO_NUM_18)
#define MICROPY_HW_SPI1_MOSI                (GPIO_NUM_23)
#define MICROPY_HW_SPI1_MISO                (GPIO_NUM_19)

#define MICROPY_BOARD_STARTUP                mch2022_board_startup
void mch2022_board_startup(void);
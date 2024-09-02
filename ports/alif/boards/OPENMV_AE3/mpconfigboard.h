#define MICROPY_HW_BOARD_NAME       "OpenMV-AE3"
#define MICROPY_HW_MCU_NAME         "AE302F80F55D5AE"

#define MICROPY_OBJ_REPR            (MICROPY_OBJ_REPR_C)
typedef intptr_t mp_int_t; // must be pointer size
typedef uintptr_t mp_uint_t; // must be pointer size
typedef intptr_t mp_off_t;

#define MICROPY_HW_USB_MSC          (CORE_M55_HP)
#define MICROPY_HW_ENABLE_HW_I2C    (1)
#define MICROPY_HW_ENABLE_OSPI      (CORE_M55_HP)

// ROMFS partitions
#define MICROPY_HW_ROMFS_ENABLE_PART0 (1)
#define MICROPY_HW_ROMFS_ENABLE_PART1 (CORE_M55_HP)

// I2C buses
#define MICROPY_HW_I2C1_SCL         (pin_P0_5)
#define MICROPY_HW_I2C1_SDA         (pin_P0_4)

#define MICROPY_HW_I2C2_SCL         (pin_P5_1)
#define MICROPY_HW_I2C2_SDA         (pin_P5_0)

#define MICROPY_HW_I2C3_SCL         (pin_P1_1)
#define MICROPY_HW_I2C3_SDA         (pin_P1_0)

// SPI buses
#define MICROPY_HW_SPI0_MISO        (pin_P5_0)
#define MICROPY_HW_SPI0_MOSI        (pin_P5_1)
// #define MICROPY_HW_SPI0_NSS         (pin_P5_2)
#define MICROPY_HW_SPI0_SCK         (pin_P5_3)

// UART buses
#define MICROPY_HW_UART1_TX         (pin_P0_5)
#define MICROPY_HW_UART1_RX         (pin_P0_4)
#define MICROPY_HW_UART3_TX         (pin_P1_3)
#define MICROPY_HW_UART3_RX         (pin_P1_2)
#define MICROPY_HW_UART3_RTS        (pin_P7_3)
#define MICROPY_HW_UART3_CTS        (pin_P7_2)
#define MICROPY_HW_UART4_TX         (pin_P5_1)
#define MICROPY_HW_UART4_RX         (pin_P5_0)
#define MICROPY_HW_UART5_TX         (pin_P5_3)
#define MICROPY_HW_UART5_RX         (pin_P5_2)

#define MICROPY_HW_USB_VID          0x37C5
#define MICROPY_HW_USB_PID          0x16E3

#define MICROPY_HW_USB_MANUFACTURER_STRING  "OpenMV"
#define MICROPY_HW_USB_PRODUCT_FS_STRING    "OpenMV Camera"
#define MICROPY_HW_USB_MSC_INQUIRY_VENDOR_STRING "OpenMV"

extern void board_startup(void);
#define MICROPY_BOARD_STARTUP       board_startup

extern void board_early_init(void);
#define MICROPY_BOARD_EARLY_INIT board_early_init

extern void board_enter_bootloader(void);
#define MICROPY_BOARD_ENTER_BOOTLOADER(nargs, args) board_enter_bootloader()

extern void board_enter_stop(void);
#define MICROPY_BOARD_ENTER_STOP board_enter_stop

extern void board_enter_standby(void);
#define MICROPY_BOARD_ENTER_STANDBY board_enter_standby

extern void board_exit_standby(void);
#define MICROPY_BOARD_EXIT_STANDBY board_exit_standby

// This is used for alif.Flash() and USB MSC.
#define MICROPY_HW_FLASH_STORAGE_BASE_ADDR      (0)
#define MICROPY_HW_FLASH_STORAGE_BYTES          (32 * 1024 * 1024)
#define MICROPY_HW_FLASH_STORAGE_FS_BYTES       (16 * 1024 * 1024)
#define MICROPY_HW_FLASH_STORAGE_ROMFS_BYTES    (16 * 1024 * 1024)

// Murata 1YN configuration
#define CYW43_CHIPSET_FIRMWARE_INCLUDE_FILE     "lib/cyw43-driver/firmware/w43439_sdio_1yn_7_95_59_combined.h"
#define CYW43_WIFI_NVRAM_INCLUDE_FILE           "lib/cyw43-driver/firmware/wifi_nvram_1yn.h"
#define CYW43_BT_FIRMWARE_INCLUDE_FILE          "lib/cyw43-driver/firmware/cyw43_btfw_1yn.h"
#define CYW43_BT_UART_BAUDRATE_DOWNLOAD_FIRMWARE (2000000)
#define CYW43_BT_UART_BAUDRATE_ACTIVE_USE       (2000000)

// Bluetooth config
#define MICROPY_HW_BLE_UART_ID                  (0)
#define MICROPY_HW_BLE_UART_BAUDRATE            (115200)

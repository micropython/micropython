#define MICROPY_HW_BOARD_NAME       "OpenMV N6"
#define MICROPY_HW_MCU_NAME         "STM32N657X0"

#define MICROPY_GC_STACK_ENTRY_TYPE uint32_t
#define MICROPY_ALLOC_GC_STACK_SIZE (128)

#define MICROPY_OBJ_REPR            (MICROPY_OBJ_REPR_C)

#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)
#define MICROPY_HW_HAS_SWITCH       (0)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_SDCARD_MOUNT_AT_BOOT (0)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_DAC       (0)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_SDCARD    (1)
#define MICROPY_PY_PYB_LEGACY       (0)
#define MICROPY_FATFS_EXFAT         (1)

#define MICROPY_BOARD_ENTER_BOOTLOADER board_enter_bootloader
#define MICROPY_BOARD_EARLY_INIT    board_early_init
#define MICROPY_BOARD_LEAVE_STANDBY board_leave_standby()

// HSE is 48MHz, this gives a CPU frequency of 800MHz.
#define MICROPY_HW_CLK_PLLM         (6)
#define MICROPY_HW_CLK_PLLN         (100)
#define MICROPY_HW_CLK_PLLP1        (1)
#define MICROPY_HW_CLK_PLLP2        (1)
#define MICROPY_HW_CLK_PLLFRAC      (0)

// The LSE is a 32kHz crystal.
#define MICROPY_HW_RTC_USE_LSE      (1)
#define MICROPY_HW_RTC_USE_US       (1)

// External SPI flash.
#define MICROPY_HW_XSPIFLASH_SIZE_BITS_LOG2 (28) // 256Mbit

// ROMFS config
#define MICROPY_HW_ROMFS_ENABLE_EXTERNAL_XSPI (1)
#define MICROPY_HW_ROMFS_XSPI_SPIBDEV_OBJ (&spi_bdev)
#define MICROPY_HW_ROMFS_ENABLE_PART0 (1)

// SPI flash, block device config.
#define MICROPY_HW_BDEV_SPIFLASH                (&spi_bdev)
#define MICROPY_HW_BDEV_SPIFLASH_EXTENDED       (&spi_bdev)
#define MICROPY_HW_BDEV_SPIFLASH_CONFIG         (&spiflash_config)
#define MICROPY_HW_BDEV_SPIFLASH_OFFSET_BYTES   (4 * 1024 * 1024)
#define MICROPY_HW_BDEV_SPIFLASH_SIZE_BYTES     (4 * 1024 * 1024)

// UART buses
#define MICROPY_HW_UART2_TX         (pyb_pin_BT_TXD)
#define MICROPY_HW_UART2_RX         (pyb_pin_BT_RXD)
#define MICROPY_HW_UART2_RTS        (pyb_pin_BT_RTS)
#define MICROPY_HW_UART2_CTS        (pyb_pin_BT_CTS)
#define MICROPY_HW_UART3_TX         (pyb_pin_UART3_TX)
#define MICROPY_HW_UART3_RX         (pyb_pin_UART3_RX)
#define MICROPY_HW_UART3_RTS        (pyb_pin_UART3_RTS)
#define MICROPY_HW_UART4_TX         (pyb_pin_UART4_TX)
#define MICROPY_HW_UART4_RX         (pyb_pin_UART4_RX)
#define MICROPY_HW_UART7_TX         (pyb_pin_UART7_TX)
#define MICROPY_HW_UART7_RX         (pyb_pin_UART7_RX)

// I2C buses
#define MICROPY_HW_I2C2_SCL         (pyb_pin_I2C2_SCL)
#define MICROPY_HW_I2C2_SDA         (pyb_pin_I2C2_SDA)
#define MICROPY_HW_I2C4_SCL         (pyb_pin_I2C4_SCL)
#define MICROPY_HW_I2C4_SDA         (pyb_pin_I2C4_SDA)

// SPI buses
#define MICROPY_HW_SPI2_NSS         (pyb_pin_SPI2_CS)
#define MICROPY_HW_SPI2_SCK         (pyb_pin_SPI2_SCK)
#define MICROPY_HW_SPI2_MISO        (pyb_pin_SPI2_MISO)
#define MICROPY_HW_SPI2_MOSI        (pyb_pin_SPI2_MOSI)
#define MICROPY_HW_SPI4_NSS         (pyb_pin_SPI4_CS)
#define MICROPY_HW_SPI4_SCK         (pyb_pin_SPI4_SCK)
#define MICROPY_HW_SPI4_MISO        (pyb_pin_SPI4_MISO)
#define MICROPY_HW_SPI4_MOSI        (pyb_pin_SPI4_MOSI)

// USER is pulled high, and pressing the button makes the input go low.
#define MICROPY_HW_USRSW_PIN        (pyb_pin_BUTTON)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// LEDs
#define MICROPY_HW_LED1             (pyb_pin_LED_RED)
#define MICROPY_HW_LED2             (pyb_pin_LED_GREEN)
#define MICROPY_HW_LED3             (pyb_pin_LED_BLUE)
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))

// SD Card SDMMC
// SD_VSELECT: low(default)=3.3V IO, high=1.8V IO
// SD_RESET: drive low to turn off SD VCC (pulled high by default)
// SD_DETECT: pulled high in hardware, goes low when SD inserted
#define MICROPY_HW_SDCARD_SDMMC             (1)
#define MICROPY_HW_SDCARD_CK                (pyb_pin_SD_SDIO_CK)
#define MICROPY_HW_SDCARD_CMD               (pyb_pin_SD_SDIO_CMD)
#define MICROPY_HW_SDCARD_D0                (pyb_pin_SD_SDIO_D0)
#define MICROPY_HW_SDCARD_D1                (pyb_pin_SD_SDIO_D1)
#define MICROPY_HW_SDCARD_D2                (pyb_pin_SD_SDIO_D2)
#define MICROPY_HW_SDCARD_D3                (pyb_pin_SD_SDIO_D3)
#define MICROPY_HW_SDCARD_DETECT_PIN        (pyb_pin_SD_DETECT)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_NOPULL)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)

// WiFi SDMMC
#define MICROPY_HW_SDIO_SDMMC       (2)
#define MICROPY_HW_SDIO_CK          (pyb_pin_WL_SDIO_CK)
#define MICROPY_HW_SDIO_CMD         (pyb_pin_WL_SDIO_CMD)
#define MICROPY_HW_SDIO_D0          (pyb_pin_WL_SDIO_D0)
#define MICROPY_HW_SDIO_D1          (pyb_pin_WL_SDIO_D1)
#define MICROPY_HW_SDIO_D2          (pyb_pin_WL_SDIO_D2)
#define MICROPY_HW_SDIO_D3          (pyb_pin_WL_SDIO_D3)

// USB config
#define MICROPY_HW_USB_HS           (1)
#define MICROPY_HW_USB_HS_IN_FS     (1)
#define MICROPY_HW_USB_MAIN_DEV     (USB_PHY_HS_ID)
#define MICROPY_HW_USB_VID          0x37C5
#define MICROPY_HW_USB_PID          0x1206
#define MICROPY_HW_USB_PID_CDC      (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_MSC      (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC_MSC  (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC_HID  (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC_MSC_HID  (MICROPY_HW_USB_PID)

// Murata 1YN configuration
#define CYW43_CHIPSET_FIRMWARE_INCLUDE_FILE     "lib/cyw43-driver/firmware/w43439_sdio_1yn_7_95_59_combined.h"
#define CYW43_WIFI_NVRAM_INCLUDE_FILE           "lib/cyw43-driver/firmware/wifi_nvram_1yn.h"
#define CYW43_BT_FIRMWARE_INCLUDE_FILE          "lib/cyw43-driver/firmware/cyw43_btfw_1yn.h"

// Bluetooth config
#define MICROPY_HW_BLE_UART_ID                  (PYB_UART_2)
#define MICROPY_HW_BLE_UART_BAUDRATE            (115200)
#define MICROPY_HW_BLE_UART_BAUDRATE_SECONDARY  (3000000)
#define MICROPY_HW_BLE_UART_BAUDRATE_DOWNLOAD_FIRMWARE (3000000)

/******************************************************************************/
// Bootloader configuration

#define MBOOT_BOARD_EARLY_INIT(initial_r0)      mboot_board_early_init()

#define MBOOT_FSLOAD                            (1)
#define MBOOT_VFS_FAT                           (1)

#define MBOOT_SPIFLASH_CS                       (pyb_pin_XSPIM_P2_CS)
#define MBOOT_SPIFLASH_SCK                      (pyb_pin_XSPIM_P2_SCK)
#define MBOOT_SPIFLASH_MOSI                     (pyb_pin_XSPIM_P2_IO0)
#define MBOOT_SPIFLASH_MISO                     (pyb_pin_XSPIM_P2_IO1)
#define MBOOT_SPIFLASH_ADDR                     (0x70000000)
#define MBOOT_SPIFLASH_BYTE_SIZE                (32 * 1024 * 1024)
#define MBOOT_SPIFLASH_LAYOUT                   "/0x70000000/8192*4Kg"
#define MBOOT_SPIFLASH_ERASE_BLOCKS_PER_PAGE    (1)
#define MBOOT_SPIFLASH_SPIFLASH                 (&spi_bdev.spiflash)
#define MBOOT_SPIFLASH_CONFIG                   (&spiflash_config)

/******************************************************************************/
// Function and variable declarations

extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;

void mboot_board_early_init(void);
void mboot_board_entry_init(void);

void board_enter_bootloader(unsigned int n_args, const void *args);
void board_early_init(void);
void board_leave_standby(void);

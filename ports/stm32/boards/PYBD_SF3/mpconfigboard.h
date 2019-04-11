#define MICROPY_HW_BOARD_NAME       "PYBD_SF3W"
#define MICROPY_HW_MCU_NAME         "STM32F733IEK"

#define MICROPY_PY_PYB_LEGACY       (1)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_SDCARD    (1)
#define MICROPY_HW_ENABLE_MMCARD    (1)
#define MICROPY_HW_ENABLE_CYW43     (1)

#define MICROPY_BOARD_EARLY_INIT    board_early_init
void board_early_init(void);

// HSE is 25MHz, run SYS at 120MHz
#define MICROPY_HW_CLK_PLLM (20)
#define MICROPY_HW_CLK_PLLN (192)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (5)
#define MICROPY_HW_FLASH_LATENCY (FLASH_LATENCY_3)

// There is an external 32kHz oscillator
#define RTC_ASYNCH_PREDIV (0)
#define RTC_SYNCH_PREDIV (0x7fff)
#define MICROPY_HW_RTC_USE_BYPASS   (1)
#define MICROPY_HW_RTC_USE_US       (1)
#define MICROPY_HW_RTC_USE_CALOUT   (1)

// use external SPI flash for storage
#define MICROPY_HW_SOFTQSPI_SCK_LOW(self) (GPIOE->BSRR = (0x10000 << 11))
#define MICROPY_HW_SOFTQSPI_SCK_HIGH(self) (GPIOE->BSRR = (1 << 11))
#define MICROPY_HW_SOFTQSPI_NIBBLE_READ(self) ((GPIOE->IDR >> 7) & 0xf)
#define MICROPY_HW_SPIFLASH_SIZE_BITS (16 * 1024 * 1024)
#define MICROPY_HW_SPIFLASH_CS      (pyb_pin_QSPI2_CS)
#define MICROPY_HW_SPIFLASH_SCK     (pyb_pin_QSPI2_CLK)
#define MICROPY_HW_SPIFLASH_IO0     (pyb_pin_QSPI2_D0)
#define MICROPY_HW_SPIFLASH_IO1     (pyb_pin_QSPI2_D1)
#define MICROPY_HW_SPIFLASH_IO2     (pyb_pin_QSPI2_D2)
#define MICROPY_HW_SPIFLASH_IO3     (pyb_pin_QSPI2_D3)

// block device config for first SPI flash
extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;
#define MICROPY_HW_BDEV_IOCTL(op, arg) ( \
    (op) == BDEV_IOCTL_NUM_BLOCKS ? (MICROPY_HW_SPIFLASH_SIZE_BITS / 8 / FLASH_BLOCK_SIZE) : \
    (op) == BDEV_IOCTL_INIT ? spi_bdev_ioctl(&spi_bdev, (op), (uint32_t)&spiflash_config) : \
    spi_bdev_ioctl(&spi_bdev, (op), (arg)) \
)
#define MICROPY_HW_BDEV_READBLOCKS(dest, bl, n) spi_bdev_readblocks(&spi_bdev, (dest), (bl), (n))
#define MICROPY_HW_BDEV_WRITEBLOCKS(src, bl, n) spi_bdev_writeblocks(&spi_bdev, (src), (bl), (n))

// additional SPI flash, to be memory mapped
#define MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 (24)
#define MICROPY_HW_QSPIFLASH_CS     (pyb_pin_QSPI_CS)
#define MICROPY_HW_QSPIFLASH_SCK    (pyb_pin_QSPI_CLK)
#define MICROPY_HW_QSPIFLASH_IO0    (pyb_pin_QSPI_D0)
#define MICROPY_HW_QSPIFLASH_IO1    (pyb_pin_QSPI_D1)
#define MICROPY_HW_QSPIFLASH_IO2    (pyb_pin_QSPI_D2)
#define MICROPY_HW_QSPIFLASH_IO3    (pyb_pin_QSPI_D3)

// block device config for second SPI flash
extern const struct _mp_spiflash_config_t spiflash2_config;
extern struct _spi_bdev_t spi_bdev2;
#if 0
#define MICROPY_HW_BDEV2_IOCTL(op, arg) ( \
    (op) == BDEV_IOCTL_NUM_BLOCKS ? ((1 << (MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 - 3)) / FLASH_BLOCK_SIZE) : \
    (op) == BDEV_IOCTL_INIT ? spi_bdev_ioctl(&spi_bdev2, (op), (uint32_t)&spiflash2_config) : \
    spi_bdev_ioctl(&spi_bdev2, (op), (arg)) \
)
#define MICROPY_HW_BDEV2_READBLOCKS(dest, bl, n) spi_bdev_readblocks(&spi_bdev2, (dest), (bl), (n))
#define MICROPY_HW_BDEV2_WRITEBLOCKS(src, bl, n) spi_bdev_writeblocks(&spi_bdev2, (src), (bl), (n))
#endif

// UART config
#define MICROPY_HW_UART1_NAME       "YA"
#define MICROPY_HW_UART1_TX         (pyb_pin_Y1)
#define MICROPY_HW_UART1_RX         (pyb_pin_Y2)
#define MICROPY_HW_UART2_TX         (pyb_pin_X3)
#define MICROPY_HW_UART2_RX         (pyb_pin_X4)
#define MICROPY_HW_UART2_RTS        (pyb_pin_X2)
#define MICROPY_HW_UART2_CTS        (pyb_pin_X1)
#define MICROPY_HW_UART3_NAME       "YB"
#define MICROPY_HW_UART3_TX         (pyb_pin_Y9)
#define MICROPY_HW_UART3_RX         (pyb_pin_Y10)
#define MICROPY_HW_UART4_NAME       "XA"
#define MICROPY_HW_UART4_TX         (pyb_pin_X1)
#define MICROPY_HW_UART4_RX         (pyb_pin_X2)
#define MICROPY_HW_UART6_TX         (pyb_pin_BT_TXD)
#define MICROPY_HW_UART6_RX         (pyb_pin_BT_RXD)
#define MICROPY_HW_UART6_RTS        (pyb_pin_BT_RTS)
#define MICROPY_HW_UART6_CTS        (pyb_pin_BT_CTS)
//#define MICROPY_HW_UART_REPL        PYB_UART_3
//#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C busses
#define MICROPY_HW_I2C1_NAME "X"
#define MICROPY_HW_I2C1_SCL  (pyb_pin_X9)
#define MICROPY_HW_I2C1_SDA  (pyb_pin_X10)
#define MICROPY_HW_I2C2_NAME "Y"
#define MICROPY_HW_I2C2_SCL  (pyb_pin_Y9)
#define MICROPY_HW_I2C2_SDA  (pyb_pin_Y10)

// SPI busses
#define MICROPY_HW_SPI1_NAME "X"
#define MICROPY_HW_SPI1_NSS  (pyb_pin_X5)
#define MICROPY_HW_SPI1_SCK  (pyb_pin_X6)
#define MICROPY_HW_SPI1_MISO (pyb_pin_X7)
#define MICROPY_HW_SPI1_MOSI (pyb_pin_X8)
#define MICROPY_HW_SPI2_NAME "Y"
#define MICROPY_HW_SPI2_NSS  (pyb_pin_Y5)
#define MICROPY_HW_SPI2_SCK  (pyb_pin_Y6)
#define MICROPY_HW_SPI2_MISO (pyb_pin_Y7)
#define MICROPY_HW_SPI2_MOSI (pyb_pin_Y8)
#define MICROPY_HW_SPI3_NSS  (pyb_pin_W16)
#define MICROPY_HW_SPI3_SCK  (pyb_pin_W29)
#define MICROPY_HW_SPI3_MISO (pyb_pin_W50)
#define MICROPY_HW_SPI3_MOSI (pyb_pin_W46)

// CAN busses
#define MICROPY_HW_CAN1_NAME "X"
#define MICROPY_HW_CAN1_TX   (pyb_pin_X10)
#define MICROPY_HW_CAN1_RX   (pyb_pin_X9)

// USRSW is not pulled, and pressing the button makes the input go low.
#define MICROPY_HW_USRSW_PIN        (pyb_pin_USR)
#define MICROPY_HW_USRSW_PULL       (GPIO_PULLUP)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// LEDs
#define MICROPY_HW_LED1             (pyb_pin_LED_RED)
#define MICROPY_HW_LED2             (pyb_pin_LED_GREEN)
#define MICROPY_HW_LED3             (pyb_pin_LED_BLUE)
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))

// SD card
#define MICROPY_HW_SDMMC2_CK                (pyb_pin_SD_CK)
#define MICROPY_HW_SDMMC2_CMD               (pyb_pin_SD_CMD)
#define MICROPY_HW_SDMMC2_D0                (pyb_pin_SD_D0)
#define MICROPY_HW_SDMMC2_D1                (pyb_pin_SD_D1)
#define MICROPY_HW_SDMMC2_D2                (pyb_pin_SD_D2)
#define MICROPY_HW_SDMMC2_D3                (pyb_pin_SD_D3)
#define MICROPY_HW_SDCARD_DETECT_PIN        (pyb_pin_SD_SW)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)
#define MICROPY_HW_SDCARD_MOUNT_AT_BOOT     (0)

// USB config
#define MICROPY_HW_USB_FS (1)
#define MICROPY_HW_USB_HS (1)
#define MICROPY_HW_USB_HS_IN_FS (1)
#define MICROPY_HW_USB_MAIN_DEV (USB_PHY_HS_ID)
#define MICROPY_HW_USB_ENABLE_CDC2 (0)
//#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_A9)
//#define MICROPY_HW_USB_OTG_ID_PIN      (pin_A10)

/******************************************************************************/
// Bootloader configuration

#define MBOOT_USB_AUTODETECT_PORT (1)
#define MBOOT_FSLOAD (1)

#define MBOOT_I2C_PERIPH_ID 1
#define MBOOT_I2C_SCL (pin_B8)
#define MBOOT_I2C_SDA (pin_B9)
#define MBOOT_I2C_ALTFUNC (4)

#define MBOOT_SPIFLASH_ADDR (0x80000000)
#define MBOOT_SPIFLASH_BYTE_SIZE (64 * 32 * 1024)
#define MBOOT_SPIFLASH_LAYOUT "/0x80000000/64*32Kg"
#define MBOOT_SPIFLASH_ERASE_BLOCKS_PER_PAGE (32 / 4)
#define MBOOT_SPIFLASH_SPIFLASH (&spi_bdev.spiflash)
#define MBOOT_SPIFLASH_CONFIG (&spiflash_config)

#define MBOOT_SPIFLASH2_ADDR (0x90000000)
#define MBOOT_SPIFLASH2_BYTE_SIZE (64 * 32 * 1024)
#define MBOOT_SPIFLASH2_LAYOUT "/0x90000000/64*32Kg"
#define MBOOT_SPIFLASH2_ERASE_BLOCKS_PER_PAGE (32 / 4)
#define MBOOT_SPIFLASH2_SPIFLASH (&spi_bdev2.spiflash)
#define MBOOT_SPIFLASH2_CONFIG (&spiflash2_config)

#define MBOOT_BOARD_EARLY_INIT mboot_board_early_init
void mboot_board_early_init(void);

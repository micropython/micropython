#define MICROPY_HW_BOARD_NAME       "F469I-DISCO"
#define MICROPY_HW_MCU_NAME         "STM32F469"

#define MICROPY_HW_HAS_FLASH        (1)
#define USE_QSPI_DATA_FALSH         (1)

#define MICROPY_PY_THREAD           (1)
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_SDCARD    (1)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)

// HSE is 8MHz, HCLK=168MHz
#define MICROPY_HW_CLK_PLLM (8)
#define MICROPY_HW_CLK_PLLN (336)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (7)

#define MICROPY_HW_UART_REPL        PYB_UART_3
#define MICROPY_HW_UART_REPL_BAUD   115200

// UART config
#define MICROPY_HW_UART3_TX     (pin_B10)
#define MICROPY_HW_UART3_RX     (pin_B11)
#define MICROPY_HW_UART6_TX     (pin_G9)
#define MICROPY_HW_UART6_RX     (pin_G14)

// I2C busses
#define MICROPY_HW_I2C1_SCL (pin_B8)
#define MICROPY_HW_I2C1_SDA (pin_B9)
#define MICROPY_HW_I2C2_SCL (pin_H4)
#define MICROPY_HW_I2C2_SDA (pin_H5)

// CAN config
#define MICROPY_HW_CAN2_TX (pin_B13)
#define MICROPY_HW_CAN2_RX (pin_B12)

// SD card detect switch
#if MICROPY_HW_ENABLE_SDCARD
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_G2)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (0)
#endif

// 128MBit external QSPI flash, used for either the filesystem
// MICRON N25Q128A13EF840F
#define MICROPY_HW_QSPIFLASH_TYPE      (MP_SPI_FLASH_N25Qxx)
#define MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 (27)    /* 128MBit */
#define MICROPY_HW_QSPIFLASH_SIZE_BITS      (1<<MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2)
#define MICROPY_HW_QSPI_PRESCALER      (2)          /* QSPI Freq= 168 MHz / (1+1) = 84 MHz */
#define MICROPY_HW_QSPI_SAMPLE_SHIFT   (1)    
#define MICROPY_HW_QSPI_CS_HIGH_CYCLES (5)

#define MICROPY_HW_QSPIFLASH_CS     (pin_B6)
#define MICROPY_HW_QSPIFLASH_SCK    (pin_F10)
#define MICROPY_HW_QSPIFLASH_IO0    (pin_F8)
#define MICROPY_HW_QSPIFLASH_IO1    (pin_F9)
#define MICROPY_HW_QSPIFLASH_IO2    (pin_F7)
#define MICROPY_HW_QSPIFLASH_IO3    (pin_F6)

// SPI flash, block device config (when used as the filesystem)
#if USE_QSPI_DATA_FALSH
extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)
#define MICROPY_HW_BDEV_IOCTL(op, arg) ( \
    (op) == BDEV_IOCTL_NUM_BLOCKS ? (MICROPY_HW_QSPIFLASH_SIZE_BITS / 8 / FLASH_BLOCK_SIZE) : \
    (op) == BDEV_IOCTL_INIT ? spi_bdev_ioctl(&spi_bdev, (op), (uint32_t)&spiflash_config) : \
    spi_bdev_ioctl(&spi_bdev, (op), (arg)) \
)
#define MICROPY_HW_BDEV_READBLOCKS(dest, bl, n) spi_bdev_readblocks(&spi_bdev, (dest), (bl), (n))
#define MICROPY_HW_BDEV_WRITEBLOCKS(src, bl, n) spi_bdev_writeblocks(&spi_bdev, (src), (bl), (n))
#define MICROPY_HW_BDEV_SPIFLASH_EXTENDED (&spi_bdev) // for extended block protocol
#endif

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_A0)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_G6) // green
#define MICROPY_HW_LED2             (pin_D4) // orange
#define MICROPY_HW_LED3             (pin_D5) // red
#define MICROPY_HW_LED4             (pin_K3) // blue
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))

// USB config
#define MICROPY_HW_USB_FS              (1)
#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_A9)
#define MICROPY_HW_USB_OTG_ID_PIN      (pin_A10)

// SDRAM MT48LC4M32B2B5
#define MICROPY_HW_SDRAM_SIZE  (128 / 8 * 1024 * 1024)  // 128 Mbit
#define MICROPY_HW_SDRAM_STARTUP_TEST              (1)
#define MICROPY_HEAP_START               sdram_start()
#define MICROPY_HEAP_END                   sdram_end()

// Timing configuration for 90 Mhz (11.90ns) of SD clock frequency (180Mhz/2)
#define MICROPY_HW_SDRAM_TIMING_TMRD        (2)
#define MICROPY_HW_SDRAM_TIMING_TXSR        (7)
#define MICROPY_HW_SDRAM_TIMING_TRAS        (4)
#define MICROPY_HW_SDRAM_TIMING_TRC         (7)
#define MICROPY_HW_SDRAM_TIMING_TWR         (2)
#define MICROPY_HW_SDRAM_TIMING_TRP         (2)
#define MICROPY_HW_SDRAM_TIMING_TRCD        (2)
#define MICROPY_HW_SDRAM_REFRESH_RATE       (64) // ms
#define MICROPY_HW_SDRAM_BURST_LENGTH       2
#define MICROPY_HW_SDRAM_CAS_LATENCY        3
#define MICROPY_HW_SDRAM_COLUMN_BITS_NUM    8
#define MICROPY_HW_SDRAM_ROW_BITS_NUM       12
#define MICROPY_HW_SDRAM_MEM_BUS_WIDTH      32
#define MICROPY_HW_SDRAM_INTERN_BANKS_NUM   4
#define MICROPY_HW_SDRAM_CLOCK_PERIOD       2
#define MICROPY_HW_SDRAM_RPIPE_DELAY        1
#define MICROPY_HW_SDRAM_RBURST             (0)
#define MICROPY_HW_SDRAM_WRITE_PROTECTION   (0)
#define MICROPY_HW_SDRAM_AUTOREFRESH_NUM    (4)
#define MICROPY_HW_FMC_SDCKE0   (pin_H2)
#define MICROPY_HW_FMC_SDNE0    (pin_H3)
#define MICROPY_HW_FMC_SDCLK    (pin_G8)
#define MICROPY_HW_FMC_SDNCAS   (pin_G15)
#define MICROPY_HW_FMC_SDNRAS   (pin_F11)
#define MICROPY_HW_FMC_SDNWE    (pin_C0)
#define MICROPY_HW_FMC_BA0      (pin_G4)
#define MICROPY_HW_FMC_BA1      (pin_G5)
#define MICROPY_HW_FMC_NBL0     (pin_E0)
#define MICROPY_HW_FMC_NBL1     (pin_E1)
#define MICROPY_HW_FMC_NBL2     (pin_I4)
#define MICROPY_HW_FMC_NBL3     (pin_I5)
#define MICROPY_HW_FMC_A0       (pin_F0)
#define MICROPY_HW_FMC_A1       (pin_F1)
#define MICROPY_HW_FMC_A2       (pin_F2)
#define MICROPY_HW_FMC_A3       (pin_F3)
#define MICROPY_HW_FMC_A4       (pin_F4)
#define MICROPY_HW_FMC_A5       (pin_F5)
#define MICROPY_HW_FMC_A6       (pin_F12)
#define MICROPY_HW_FMC_A7       (pin_F13)
#define MICROPY_HW_FMC_A8       (pin_F14)
#define MICROPY_HW_FMC_A9       (pin_F15)
#define MICROPY_HW_FMC_A10      (pin_G0)
#define MICROPY_HW_FMC_A11      (pin_G1)
#define MICROPY_HW_FMC_D0       (pin_D14)
#define MICROPY_HW_FMC_D1       (pin_D15)
#define MICROPY_HW_FMC_D2       (pin_D0)
#define MICROPY_HW_FMC_D3       (pin_D1)
#define MICROPY_HW_FMC_D4       (pin_E7)
#define MICROPY_HW_FMC_D5       (pin_E8)
#define MICROPY_HW_FMC_D6       (pin_E9)
#define MICROPY_HW_FMC_D7       (pin_E10)
#define MICROPY_HW_FMC_D8       (pin_E11)
#define MICROPY_HW_FMC_D9       (pin_E12)
#define MICROPY_HW_FMC_D10      (pin_E13)
#define MICROPY_HW_FMC_D11      (pin_E14)
#define MICROPY_HW_FMC_D12      (pin_E15)
#define MICROPY_HW_FMC_D13      (pin_D8)
#define MICROPY_HW_FMC_D14      (pin_D9)
#define MICROPY_HW_FMC_D15      (pin_D10)
#define MICROPY_HW_FMC_D16      (pin_H8)
#define MICROPY_HW_FMC_D17      (pin_H9)
#define MICROPY_HW_FMC_D18      (pin_H10)
#define MICROPY_HW_FMC_D19      (pin_H11)
#define MICROPY_HW_FMC_D20      (pin_H12)
#define MICROPY_HW_FMC_D21      (pin_H13)
#define MICROPY_HW_FMC_D22      (pin_H14)
#define MICROPY_HW_FMC_D23      (pin_H15)
#define MICROPY_HW_FMC_D24      (pin_I0)
#define MICROPY_HW_FMC_D25      (pin_I1)
#define MICROPY_HW_FMC_D26      (pin_I2)
#define MICROPY_HW_FMC_D27      (pin_I3)
#define MICROPY_HW_FMC_D28      (pin_I6)
#define MICROPY_HW_FMC_D29      (pin_I7)
#define MICROPY_HW_FMC_D30      (pin_I9)
#define MICROPY_HW_FMC_D31      (pin_I10)

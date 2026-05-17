#define MICROPY_HW_BOARD_NAME       "STM32H747I-DISCO"
#define MICROPY_HW_MCU_NAME         "STM32H747"

#define MICROPY_FATFS_EXFAT         (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_SDCARD    (1)
#define MICROPY_GC_SPLIT_HEAP       (1)

// Flash storage config (external QSPI flash)
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE            (1)
#define MICROPY_HW_SPIFLASH_SOFT_RESET              (1)
#define MICROPY_HW_SPIFLASH_CHIP_PARAMS             (1)
#define MICROPY_HW_QSPIFLASH_DUMMY_CYCLES           (4)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE    (0)

#define MICROPY_BOARD_EARLY_INIT    DISCO_board_early_init
void DISCO_board_early_init(void);

void DISCO_board_low_power(int mode);
#define MICROPY_BOARD_LEAVE_STOP    DISCO_board_low_power(0);
#define MICROPY_BOARD_ENTER_STOP    DISCO_board_low_power(1);
#define MICROPY_BOARD_ENTER_STANDBY DISCO_board_low_power(2);

// PLL1 400MHz/50MHz for system and SDMMC
// HSE is 25MHz
#define MICROPY_HW_CLK_PLLM             (5)
#define MICROPY_HW_CLK_PLLN             (160)
#define MICROPY_HW_CLK_PLLP             (2)
#define MICROPY_HW_CLK_PLLQ             (8)
#define MICROPY_HW_CLK_PLLR             (2)
#define MICROPY_HW_CLK_PLLVCI           (RCC_PLL1VCIRANGE_2)
#define MICROPY_HW_CLK_PLLVCO           (RCC_PLL1VCOWIDE)
#define MICROPY_HW_CLK_PLLFRAC          (0)

// PLL2 240MHz for FMC and QSPI
#define MICROPY_HW_CLK_PLL2M            (5)
#define MICROPY_HW_CLK_PLL2N            (96)
#define MICROPY_HW_CLK_PLL2P            (2)
#define MICROPY_HW_CLK_PLL2Q            (2)
#define MICROPY_HW_CLK_PLL2R            (10)
#define MICROPY_HW_CLK_PLL2VCI          (RCC_PLL2VCIRANGE_2)
#define MICROPY_HW_CLK_PLL2VCO          (RCC_PLL2VCOWIDE)
#define MICROPY_HW_CLK_PLL2FRAC         (0)

// PLL3 for USB and other peripherals
#define MICROPY_HW_CLK_PLL3M            (5)
#define MICROPY_HW_CLK_PLL3N            (96)
#define MICROPY_HW_CLK_PLL3P            (8)
#define MICROPY_HW_CLK_PLL3Q            (8)
#define MICROPY_HW_CLK_PLL3R            (4)
#define MICROPY_HW_CLK_PLL3VCI          (RCC_PLL3VCIRANGE_2)
#define MICROPY_HW_CLK_PLL3VCO          (RCC_PLL3VCOWIDE)
#define MICROPY_HW_CLK_PLL3FRAC         (0)

// HSE in bypass mode (25MHz oscillator)
#define MICROPY_HW_CLK_USE_BYPASS       (1)

// Bus clock divider values
#define MICROPY_HW_CLK_AHB_DIV          (RCC_HCLK_DIV2)
#define MICROPY_HW_CLK_APB1_DIV         (RCC_APB1_DIV2)
#define MICROPY_HW_CLK_APB2_DIV         (RCC_APB2_DIV2)
#define MICROPY_HW_CLK_APB3_DIV         (RCC_APB3_DIV2)
#define MICROPY_HW_CLK_APB4_DIV         (RCC_APB4_DIV2)

// Peripheral clock sources
#define MICROPY_HW_RCC_HSI48_STATE      (RCC_HSI48_ON)
#define MICROPY_HW_RCC_USB_CLKSOURCE    (RCC_USBCLKSOURCE_PLL3)
#define MICROPY_HW_RCC_FMC_CLKSOURCE    (RCC_FMCCLKSOURCE_PLL2)
#define MICROPY_HW_RCC_RNG_CLKSOURCE    (RCC_RNGCLKSOURCE_HSI48)
#define MICROPY_HW_RCC_ADC_CLKSOURCE    (RCC_ADCCLKSOURCE_PLL3)
#define MICROPY_HW_RCC_SDMMC_CLKSOURCE  (RCC_SDMMCCLKSOURCE_PLL)
#define MICROPY_HW_RCC_FDCAN_CLKSOURCE  (RCC_FDCANCLKSOURCE_PLL)
#define MICROPY_HW_RCC_QSPI_CLKSOURCE   (RCC_QSPICLKSOURCE_PLL2)

// SMPS configuration
#define MICROPY_HW_PWR_SMPS_CONFIG      (PWR_DIRECT_SMPS_SUPPLY)

// Configure the analog switches for dual-pad pins
#define MICROPY_HW_ANALOG_SWITCH_PA0    (SYSCFG_SWITCH_PA0_OPEN)
#define MICROPY_HW_ANALOG_SWITCH_PA1    (SYSCFG_SWITCH_PA1_OPEN)
#define MICROPY_HW_ANALOG_SWITCH_PC2    (SYSCFG_SWITCH_PC2_OPEN)
#define MICROPY_HW_ANALOG_SWITCH_PC3    (SYSCFG_SWITCH_PC3_OPEN)

// Using LSE for RTC (32.768kHz crystal on PC14/PC15)
#define MICROPY_HW_RTC_USE_LSE      (1)
#define MICROPY_HW_RTC_USE_US       (1)
#define MICROPY_HW_RTC_USE_CALOUT   (0)

// QSPI flash for storage (single bank, dual-flash mode disabled in driver)
#define MICROPY_HW_QSPI_PRESCALER           (2) // 120MHz
#define MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 (27)  // 128Mbit = 16MB (single MT25QL bank)
#define MICROPY_HW_SPIFLASH_SIZE_BITS       (128 * 1024 * 1024)
#define MICROPY_HW_QSPIFLASH_CS             (pyb_pin_QSPI_BK1_NCS)
#define MICROPY_HW_QSPIFLASH_SCK            (pyb_pin_QSPI_CLK)
#define MICROPY_HW_QSPIFLASH_IO0            (pyb_pin_QSPI_BK1_IO0)
#define MICROPY_HW_QSPIFLASH_IO1            (pyb_pin_QSPI_BK1_IO1)
#define MICROPY_HW_QSPIFLASH_IO2            (pyb_pin_QSPI_BK1_IO2)
#define MICROPY_HW_QSPIFLASH_IO3            (pyb_pin_QSPI_BK1_IO3)

// SPI flash block device config
extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;
#define MICROPY_HW_BDEV_SPIFLASH    (&spi_bdev)
#define MICROPY_HW_BDEV_SPIFLASH_CONFIG (&spiflash_config)
#define MICROPY_HW_BDEV_SPIFLASH_SIZE_BYTES (MICROPY_HW_SPIFLASH_SIZE_BITS / 8)
#define MICROPY_HW_BDEV_SPIFLASH_EXTENDED (&spi_bdev)

// Flash latency
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_2

// UART config (directly accessible on Arduino header pins)
#define MICROPY_HW_UART1_TX         (pyb_pin_UART1_TX)
#define MICROPY_HW_UART1_RX         (pyb_pin_UART1_RX)
// UART REPL disabled to avoid slowing USB HS REPL throughput
// #define MICROPY_HW_UART_REPL        PYB_UART_1
// #define MICROPY_HW_UART_REPL_BAUD   115200

#define MICROPY_HW_UART8_TX         (pin_J8)
#define MICROPY_HW_UART8_RX         (pin_J9)

// I2C buses
#define MICROPY_HW_I2C4_SCL         (pin_D12)
#define MICROPY_HW_I2C4_SDA         (pin_D13)

// SPI config
#define MICROPY_HW_SPI5_NSS         (pin_K1)
#define MICROPY_HW_SPI5_SCK         (pin_K0)
#define MICROPY_HW_SPI5_MISO        (pin_J11)
#define MICROPY_HW_SPI5_MOSI        (pin_J10)

// USRSW is pulled high. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_C13)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_I12) // green
#define MICROPY_HW_LED2             (pin_I13) // orange
#define MICROPY_HW_LED3             (pin_I14) // red
#define MICROPY_HW_LED4             (pin_I15) // blue
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))

// USB config - Use High-Speed via ULPI PHY
#define MICROPY_HW_USB_HS           (1)
#define MICROPY_HW_USB_HS_ULPI      (1)
#define MICROPY_HW_USB_HS_ULPI_NXT  (pyb_pin_USB_HS_NXT)
#define MICROPY_HW_USB_HS_ULPI_STP  (pyb_pin_USB_HS_STP)
#define MICROPY_HW_USB_HS_ULPI_DIR  (pyb_pin_USB_HS_DIR)
#define MICROPY_HW_USB_HS_ULPI3320  (1)
#define MICROPY_HW_USB_MAIN_DEV     (USB_PHY_HS_ID)
#define GPIO_AF10_OTG_HS            (GPIO_AF10_OTG1_HS)

#define MICROPY_HW_USB_CDC_RX_DATA_SIZE     (1024)
#define MICROPY_HW_USB_CDC_TX_DATA_SIZE     (1024)

// SD card (SDMMC1, 4-bit)
#define MICROPY_HW_SDCARD_SDMMC     (1)
#define MICROPY_HW_SDCARD_CK        (pyb_pin_SDMMC1_CK)
#define MICROPY_HW_SDCARD_CMD       (pyb_pin_SDMMC1_CMD)
#define MICROPY_HW_SDCARD_D0        (pyb_pin_SDMMC1_D0)
#define MICROPY_HW_SDCARD_D1        (pyb_pin_SDMMC1_D1)
#define MICROPY_HW_SDCARD_D2        (pyb_pin_SDMMC1_D2)
#define MICROPY_HW_SDCARD_D3        (pyb_pin_SDMMC1_D3)
#define MICROPY_HW_SDCARD_DETECT_PIN        (pyb_pin_SD_DETECT)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)

// Ethernet via RMII
#define MICROPY_HW_ETH_MDC          (pyb_pin_ETH_MDC)
#define MICROPY_HW_ETH_MDIO         (pyb_pin_ETH_MDIO)
#define MICROPY_HW_ETH_RMII_REF_CLK (pyb_pin_ETH_RMII_REF_CLK)
#define MICROPY_HW_ETH_RMII_CRS_DV  (pyb_pin_ETH_RMII_CRS_DV)
#define MICROPY_HW_ETH_RMII_RXD0    (pyb_pin_ETH_RMII_RXD0)
#define MICROPY_HW_ETH_RMII_RXD1    (pyb_pin_ETH_RMII_RXD1)
#define MICROPY_HW_ETH_RMII_TX_EN   (pyb_pin_ETH_RMII_TX_EN)
#define MICROPY_HW_ETH_RMII_TXD0    (pyb_pin_ETH_RMII_TXD0)
#define MICROPY_HW_ETH_RMII_TXD1    (pyb_pin_ETH_RMII_TXD1)

// SDRAM configuration - 32MB (256Mbit)
#define MICROPY_HW_SDRAM_SIZE               (256 / 8 * 1024 * 1024)  // 256 Mbit = 32 MB
#define MICROPY_HW_SDRAM_STARTUP_TEST       (1)
#define MICROPY_HW_SDRAM_TEST_FAIL_ON_ERROR (1)

// Timing configuration for 240MHz/2=120MHz (8.33ns)
#define MICROPY_HW_SDRAM_CLOCK_PERIOD       2
#define MICROPY_HW_SDRAM_CAS_LATENCY        2
#define MICROPY_HW_SDRAM_FREQUENCY_KHZ      (120000) // 120 MHz
#define MICROPY_HW_SDRAM_TIMING_TMRD        (2)
#define MICROPY_HW_SDRAM_TIMING_TXSR        (6)
#define MICROPY_HW_SDRAM_TIMING_TRAS        (4)
#define MICROPY_HW_SDRAM_TIMING_TRC         (6)
#define MICROPY_HW_SDRAM_TIMING_TWR         (2)
#define MICROPY_HW_SDRAM_TIMING_TRP         (2)
#define MICROPY_HW_SDRAM_TIMING_TRCD        (2)

#define MICROPY_HW_SDRAM_ROW_BITS_NUM       12
#define MICROPY_HW_SDRAM_MEM_BUS_WIDTH      32
#define MICROPY_HW_SDRAM_REFRESH_CYCLES     4096

#define MICROPY_HW_SDRAM_COLUMN_BITS_NUM    9
#define MICROPY_HW_SDRAM_INTERN_BANKS_NUM   4
#define MICROPY_HW_SDRAM_RPIPE_DELAY        0
#define MICROPY_HW_SDRAM_RBURST             (1)
#define MICROPY_HW_SDRAM_WRITE_PROTECTION   (0)

#define MICROPY_HW_SDRAM_AUTOREFRESH_NUM    (8)
#define MICROPY_HW_SDRAM_BURST_LENGTH       1
#define MICROPY_HW_SDRAM_REFRESH_RATE       (64) // ms

// SDRAM FMC pin configuration (see pins.csv for CPU pin mapping)
#define MICROPY_HW_FMC_SDCKE1       (pyb_pin_FMC_SDCKE1)
#define MICROPY_HW_FMC_SDNE1        (pyb_pin_FMC_SDNE1)
#define MICROPY_HW_FMC_SDCLK        (pyb_pin_FMC_SDCLK)
#define MICROPY_HW_FMC_SDNCAS       (pyb_pin_FMC_SDNCAS)
#define MICROPY_HW_FMC_SDNRAS       (pyb_pin_FMC_SDNRAS)
#define MICROPY_HW_FMC_SDNWE        (pyb_pin_FMC_SDNWE)
#define MICROPY_HW_FMC_BA0          (pyb_pin_FMC_BA0)
#define MICROPY_HW_FMC_BA1          (pyb_pin_FMC_BA1)
#define MICROPY_HW_FMC_NBL0         (pyb_pin_FMC_NBL0)
#define MICROPY_HW_FMC_NBL1         (pyb_pin_FMC_NBL1)
#define MICROPY_HW_FMC_NBL2         (pyb_pin_FMC_NBL2)
#define MICROPY_HW_FMC_NBL3         (pyb_pin_FMC_NBL3)
#define MICROPY_HW_FMC_A0           (pyb_pin_FMC_A0)
#define MICROPY_HW_FMC_A1           (pyb_pin_FMC_A1)
#define MICROPY_HW_FMC_A2           (pyb_pin_FMC_A2)
#define MICROPY_HW_FMC_A3           (pyb_pin_FMC_A3)
#define MICROPY_HW_FMC_A4           (pyb_pin_FMC_A4)
#define MICROPY_HW_FMC_A5           (pyb_pin_FMC_A5)
#define MICROPY_HW_FMC_A6           (pyb_pin_FMC_A6)
#define MICROPY_HW_FMC_A7           (pyb_pin_FMC_A7)
#define MICROPY_HW_FMC_A8           (pyb_pin_FMC_A8)
#define MICROPY_HW_FMC_A9           (pyb_pin_FMC_A9)
#define MICROPY_HW_FMC_A10          (pyb_pin_FMC_A10)
#define MICROPY_HW_FMC_A11          (pyb_pin_FMC_A11)
#define MICROPY_HW_FMC_A12          (pyb_pin_FMC_A12)
#define MICROPY_HW_FMC_D0           (pyb_pin_FMC_D0)
#define MICROPY_HW_FMC_D1           (pyb_pin_FMC_D1)
#define MICROPY_HW_FMC_D2           (pyb_pin_FMC_D2)
#define MICROPY_HW_FMC_D3           (pyb_pin_FMC_D3)
#define MICROPY_HW_FMC_D4           (pyb_pin_FMC_D4)
#define MICROPY_HW_FMC_D5           (pyb_pin_FMC_D5)
#define MICROPY_HW_FMC_D6           (pyb_pin_FMC_D6)
#define MICROPY_HW_FMC_D7           (pyb_pin_FMC_D7)
#define MICROPY_HW_FMC_D8           (pyb_pin_FMC_D8)
#define MICROPY_HW_FMC_D9           (pyb_pin_FMC_D9)
#define MICROPY_HW_FMC_D10          (pyb_pin_FMC_D10)
#define MICROPY_HW_FMC_D11          (pyb_pin_FMC_D11)
#define MICROPY_HW_FMC_D12          (pyb_pin_FMC_D12)
#define MICROPY_HW_FMC_D13          (pyb_pin_FMC_D13)
#define MICROPY_HW_FMC_D14          (pyb_pin_FMC_D14)
#define MICROPY_HW_FMC_D15          (pyb_pin_FMC_D15)
#define MICROPY_HW_FMC_D16          (pyb_pin_FMC_D16)
#define MICROPY_HW_FMC_D17          (pyb_pin_FMC_D17)
#define MICROPY_HW_FMC_D18          (pyb_pin_FMC_D18)
#define MICROPY_HW_FMC_D19          (pyb_pin_FMC_D19)
#define MICROPY_HW_FMC_D20          (pyb_pin_FMC_D20)
#define MICROPY_HW_FMC_D21          (pyb_pin_FMC_D21)
#define MICROPY_HW_FMC_D22          (pyb_pin_FMC_D22)
#define MICROPY_HW_FMC_D23          (pyb_pin_FMC_D23)
#define MICROPY_HW_FMC_D24          (pyb_pin_FMC_D24)
#define MICROPY_HW_FMC_D25          (pyb_pin_FMC_D25)
#define MICROPY_HW_FMC_D26          (pyb_pin_FMC_D26)
#define MICROPY_HW_FMC_D27          (pyb_pin_FMC_D27)
#define MICROPY_HW_FMC_D28          (pyb_pin_FMC_D28)
#define MICROPY_HW_FMC_D29          (pyb_pin_FMC_D29)
#define MICROPY_HW_FMC_D30          (pyb_pin_FMC_D30)
#define MICROPY_HW_FMC_D31          (pyb_pin_FMC_D31)

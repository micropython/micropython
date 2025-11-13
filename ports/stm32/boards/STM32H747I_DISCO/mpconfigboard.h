/*
 * This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2025
 */

#define MICROPY_HW_BOARD_NAME       "STM32H747I-DISCO"
#define MICROPY_HW_MCU_NAME         "STM32H747"
#define MICROPY_HW_FLASH_FS_LABEL   "STM32H747I-DISCO"

// Network config
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-stm32h747i-disco"

#define MICROPY_OBJ_REPR            (MICROPY_OBJ_REPR_C)
typedef int mp_int_t;               // must be pointer size
typedef unsigned int mp_uint_t;     // must be pointer size

#define MICROPY_FATFS_EXFAT         (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SDCARD    (1)
#define MICROPY_HW_ENABLE_MMCARD    (0)
#define MICROPY_HW_ENTER_BOOTLOADER_VIA_RESET   (0)
#define MICROPY_HW_TIM_IS_RESERVED(id) (id == 1)
#define MICROPY_GC_SPLIT_HEAP       (1)

// Flash storage config (use external QSPI flash)
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE            (1)
#define MICROPY_HW_SPIFLASH_SOFT_RESET              (1)
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
#define MICROPY_HW_RCC_RTC_CLKSOURCE    (RCC_RTCCLKSOURCE_LSE)
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
#define MICROPY_HW_RTC_USE_CALOUT   (1)

#if !MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
// Dual QSPI flash for storage
#define MICROPY_HW_QSPI_PRESCALER           (2) // 120MHz
#define MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 (28)  // 256Mbit total (2x 128Mbit)
#define MICROPY_HW_SPIFLASH_SIZE_BITS       (256 * 1024 * 1024)
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
#endif

// Flash latency
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_2

// UART config
#define MICROPY_HW_UART1_TX         (pin_A9)
#define MICROPY_HW_UART1_RX         (pin_A10)
#define MICROPY_HW_UART_REPL        PYB_UART_1
#define MICROPY_HW_UART_REPL_BAUD   115200

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
#define MICROPY_HW_USB_HS_ULPI_NXT  (pin_H4)
#define MICROPY_HW_USB_HS_ULPI_STP  (pin_C0)
#define MICROPY_HW_USB_HS_ULPI_DIR  (pin_I11)
#define MICROPY_HW_USB_HS_ULPI3320  (1)
#define MICROPY_HW_USB_MAIN_DEV     (USB_PHY_HS_ID)
#define GPIO_AF10_OTG_HS            (GPIO_AF10_OTG1_HS)

#define MICROPY_HW_USB_CDC_RX_DATA_SIZE     (1024)
#define MICROPY_HW_USB_CDC_TX_DATA_SIZE     (1024)

// SD card
#define MICROPY_HW_SDCARD_SDMMC     (1)
#define MICROPY_HW_SDCARD_CK        (pin_C12)
#define MICROPY_HW_SDCARD_CMD       (pin_D2)
#define MICROPY_HW_SDCARD_D0        (pin_C8)
#define MICROPY_HW_SDCARD_D1        (pin_C9)
#define MICROPY_HW_SDCARD_D2        (pin_C10)
#define MICROPY_HW_SDCARD_D3        (pin_C11)
#define MICROPY_HW_SDCARD_D4        (pin_B8)
#define MICROPY_HW_SDCARD_D5        (pin_B9)
#define MICROPY_HW_SDCARD_D6        (pin_C6)
#define MICROPY_HW_SDCARD_D7        (pin_C7)
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_I8)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)

// Ethernet via RMII
#define MICROPY_HW_ETH_MDC          (pin_C1)
#define MICROPY_HW_ETH_MDIO         (pin_A2)
#define MICROPY_HW_ETH_RMII_REF_CLK (pin_A1)
#define MICROPY_HW_ETH_RMII_CRS_DV  (pin_A7)
#define MICROPY_HW_ETH_RMII_RXD0    (pin_C4)
#define MICROPY_HW_ETH_RMII_RXD1    (pin_C5)
#define MICROPY_HW_ETH_RMII_TX_EN   (pin_G11)
#define MICROPY_HW_ETH_RMII_TXD0    (pin_G13)
#define MICROPY_HW_ETH_RMII_TXD1    (pin_G12)

// SDRAM configuration - 32MB (256Mbit) on FMC Bank 1
#define MICROPY_HW_SDRAM_SIZE               (256 / 8 * 1024 * 1024)  // 256 Mbit = 32 MB
#define MICROPY_HW_SDRAM_STARTUP_TEST       (1)
#define MICROPY_HW_SDRAM_TEST_FAIL_ON_ERROR (true)

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

// SDRAM FMC configuration (bank 1)
#define MICROPY_HW_FMC_SDCKE1       (pin_H7)
#define MICROPY_HW_FMC_SDNE1        (pin_H6)
#define MICROPY_HW_FMC_SDNBL0       (pin_E0)
#define MICROPY_HW_FMC_SDNBL1       (pin_E1)
#define MICROPY_HW_FMC_SDNBL2       (pin_I4)
#define MICROPY_HW_FMC_SDNBL3       (pin_I5)
#define MICROPY_HW_FMC_SDCLK        (pin_G8)
#define MICROPY_HW_FMC_SDNCAS       (pin_G15)
#define MICROPY_HW_FMC_SDNRAS       (pin_F11)
#define MICROPY_HW_FMC_SDNWE        (pin_H5)
#define MICROPY_HW_FMC_BA0          (pin_G4)
#define MICROPY_HW_FMC_BA1          (pin_G5)
#define MICROPY_HW_FMC_NBL0         (pin_E0)
#define MICROPY_HW_FMC_NBL1         (pin_E1)
#define MICROPY_HW_FMC_NBL2         (pin_I4)
#define MICROPY_HW_FMC_NBL3         (pin_I5)
#define MICROPY_HW_FMC_A0           (pin_F0)
#define MICROPY_HW_FMC_A1           (pin_F1)
#define MICROPY_HW_FMC_A2           (pin_F2)
#define MICROPY_HW_FMC_A3           (pin_F3)
#define MICROPY_HW_FMC_A4           (pin_F4)
#define MICROPY_HW_FMC_A5           (pin_F5)
#define MICROPY_HW_FMC_A6           (pin_F12)
#define MICROPY_HW_FMC_A7           (pin_F13)
#define MICROPY_HW_FMC_A8           (pin_F14)
#define MICROPY_HW_FMC_A9           (pin_F15)
#define MICROPY_HW_FMC_A10          (pin_G0)
#define MICROPY_HW_FMC_A11          (pin_G1)
#define MICROPY_HW_FMC_A12          (pin_G2)
#define MICROPY_HW_FMC_A14          (pin_G4)
#define MICROPY_HW_FMC_A15          (pin_G5)
#define MICROPY_HW_FMC_D0           (pin_D14)
#define MICROPY_HW_FMC_D1           (pin_D15)
#define MICROPY_HW_FMC_D2           (pin_D0)
#define MICROPY_HW_FMC_D3           (pin_D1)
#define MICROPY_HW_FMC_D4           (pin_E7)
#define MICROPY_HW_FMC_D5           (pin_E8)
#define MICROPY_HW_FMC_D6           (pin_E9)
#define MICROPY_HW_FMC_D7           (pin_E10)
#define MICROPY_HW_FMC_D8           (pin_E11)
#define MICROPY_HW_FMC_D9           (pin_E12)
#define MICROPY_HW_FMC_D10          (pin_E13)
#define MICROPY_HW_FMC_D11          (pin_E14)
#define MICROPY_HW_FMC_D12          (pin_E15)
#define MICROPY_HW_FMC_D13          (pin_D8)
#define MICROPY_HW_FMC_D14          (pin_D9)
#define MICROPY_HW_FMC_D15          (pin_D10)
#define MICROPY_HW_FMC_D16          (pin_H8)
#define MICROPY_HW_FMC_D17          (pin_H9)
#define MICROPY_HW_FMC_D18          (pin_H10)
#define MICROPY_HW_FMC_D19          (pin_H11)
#define MICROPY_HW_FMC_D20          (pin_H12)
#define MICROPY_HW_FMC_D21          (pin_H13)
#define MICROPY_HW_FMC_D22          (pin_H14)
#define MICROPY_HW_FMC_D23          (pin_H15)
#define MICROPY_HW_FMC_D24          (pin_I0)
#define MICROPY_HW_FMC_D25          (pin_I1)
#define MICROPY_HW_FMC_D26          (pin_I2)
#define MICROPY_HW_FMC_D27          (pin_I3)
#define MICROPY_HW_FMC_D28          (pin_I6)
#define MICROPY_HW_FMC_D29          (pin_I7)
#define MICROPY_HW_FMC_D30          (pin_I9)
#define MICROPY_HW_FMC_D31          (pin_I10)

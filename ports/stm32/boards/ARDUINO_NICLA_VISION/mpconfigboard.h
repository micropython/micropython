/*
 * This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2023 Arduino SA
 */

#define MICROPY_HW_BOARD_NAME       "Arduino Nicla Vision"
#define MICROPY_HW_MCU_NAME         "STM32H747"
#define MICROPY_HW_FLASH_FS_LABEL   "Nicla Vision"

// Network config
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-nicla-vision"

#define MICROPY_OBJ_REPR            (MICROPY_OBJ_REPR_C)

#define MICROPY_FATFS_EXFAT         (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_HAS_SWITCH       (0)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SDCARD    (0)
#define MICROPY_HW_ENABLE_MMCARD    (0)
#define MICROPY_HW_ENTER_BOOTLOADER_VIA_RESET   (0)
#define MICROPY_HW_TIM_IS_RESERVED(id) (id == 3)

// ROMFS config
#define MICROPY_HW_ROMFS_ENABLE_EXTERNAL_QSPI       (1)
#define MICROPY_HW_ROMFS_QSPI_SPIFLASH_OBJ          (&spi_bdev.spiflash)
#define MICROPY_HW_ROMFS_ENABLE_PART0               (1)

// Flash storage config
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE            (1)
#define MICROPY_HW_SPIFLASH_SOFT_RESET              (1)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE    (0)

#define MICROPY_BOARD_STARTUP       NICLAV_board_startup
void NICLAV_board_startup(void);

#define MICROPY_BOARD_EARLY_INIT NICLAV_board_early_init
void NICLAV_board_early_init(void);

#define MICROPY_BOARD_ENTER_BOOTLOADER(nargs, args) NICLAV_board_enter_bootloader()
void NICLAV_board_enter_bootloader(void);

void NICLAV_board_low_power(int mode);
#define MICROPY_BOARD_LEAVE_STOP    NICLAV_board_low_power(0);
#define MICROPY_BOARD_ENTER_STOP    NICLAV_board_low_power(1);
#define MICROPY_BOARD_ENTER_STANDBY NICLAV_board_low_power(2);

void NICLAV_board_osc_enable(int enable);
#define MICROPY_BOARD_PRE_STOP      NICLAV_board_osc_enable(0);
#define MICROPY_BOARD_POST_STOP     NICLAV_board_osc_enable(1);

// PLL1 400MHz/50MHz for SDMMC and FDCAN
// USB and RNG are clocked from the HSI48
#define MICROPY_HW_CLK_PLLM             (5)
#define MICROPY_HW_CLK_PLLN             (160)
#define MICROPY_HW_CLK_PLLP             (2)
#define MICROPY_HW_CLK_PLLQ             (16)
#define MICROPY_HW_CLK_PLLR             (2)
#define MICROPY_HW_CLK_PLLVCI           (RCC_PLL1VCIRANGE_2)
#define MICROPY_HW_CLK_PLLVCO           (RCC_PLL1VCOWIDE)
#define MICROPY_HW_CLK_PLLFRAC          (0)

// PLL2 200MHz for FMC and QSPI.
#define MICROPY_HW_CLK_PLL2M            (5)
#define MICROPY_HW_CLK_PLL2N            (80)
#define MICROPY_HW_CLK_PLL2P            (2)
#define MICROPY_HW_CLK_PLL2Q            (2)
#define MICROPY_HW_CLK_PLL2R            (2)
#define MICROPY_HW_CLK_PLL2VCI          (RCC_PLL2VCIRANGE_2)
#define MICROPY_HW_CLK_PLL2VCO          (RCC_PLL2VCOWIDE)
#define MICROPY_HW_CLK_PLL2FRAC         (0)

// PLL3 160MHz for ADC and SPI123
#define MICROPY_HW_CLK_PLL3M            (5)
#define MICROPY_HW_CLK_PLL3N            (160)
#define MICROPY_HW_CLK_PLL3P            (2)
#define MICROPY_HW_CLK_PLL3Q            (5)
#define MICROPY_HW_CLK_PLL3R            (2)
#define MICROPY_HW_CLK_PLL3VCI          (RCC_PLL3VCIRANGE_2)
#define MICROPY_HW_CLK_PLL3VCO          (RCC_PLL3VCOWIDE)
#define MICROPY_HW_CLK_PLL3FRAC         (0)

// HSE in BYPASS mode.
#define MICROPY_HW_CLK_USE_BYPASS       (1)

// Bus clock divider values
#define MICROPY_HW_CLK_AHB_DIV          (RCC_HCLK_DIV2)
#define MICROPY_HW_CLK_APB1_DIV         (RCC_APB1_DIV2)
#define MICROPY_HW_CLK_APB2_DIV         (RCC_APB2_DIV2)
#define MICROPY_HW_CLK_APB3_DIV         (RCC_APB3_DIV2)
#define MICROPY_HW_CLK_APB4_DIV         (RCC_APB4_DIV2)

// Peripheral clock sources
#define MICROPY_HW_RCC_HSI48_STATE      (RCC_HSI48_ON)
#define MICROPY_HW_RCC_USB_CLKSOURCE    (RCC_USBCLKSOURCE_HSI48)
#define MICROPY_HW_RCC_RTC_CLKSOURCE    (RCC_RTCCLKSOURCE_LSI)
#define MICROPY_HW_RCC_FMC_CLKSOURCE    (RCC_FMCCLKSOURCE_PLL2)
#define MICROPY_HW_RCC_RNG_CLKSOURCE    (RCC_RNGCLKSOURCE_HSI48)
#define MICROPY_HW_RCC_ADC_CLKSOURCE    (RCC_ADCCLKSOURCE_PLL3)
#define MICROPY_HW_RCC_SDMMC_CLKSOURCE  (RCC_SDMMCCLKSOURCE_PLL)
#define MICROPY_HW_RCC_FDCAN_CLKSOURCE  (RCC_FDCANCLKSOURCE_PLL)
#define MICROPY_HW_RCC_SPI123_CLKSOURCE (RCC_SPI123CLKSOURCE_PLL3)
#define MICROPY_HW_RCC_I2C123_CLKSOURCE (RCC_I2C123CLKSOURCE_D2PCLK1)
#define MICROPY_HW_RCC_QSPI_CLKSOURCE   (RCC_QSPICLKSOURCE_PLL2)

// SMPS configuration
#define MICROPY_HW_PWR_SMPS_CONFIG      (PWR_LDO_SUPPLY)

// Configure the analog switches for dual-pad pins.
#define MICROPY_HW_ANALOG_SWITCH_PA0    (SYSCFG_SWITCH_PA0_OPEN)
#define MICROPY_HW_ANALOG_SWITCH_PA1    (SYSCFG_SWITCH_PA1_OPEN)
// PC2_C and PC3_C, which are connected to ULPI NXT and DIR pins.
#define MICROPY_HW_ANALOG_SWITCH_PC2    (SYSCFG_SWITCH_PC2_CLOSE)
#define MICROPY_HW_ANALOG_SWITCH_PC3    (SYSCFG_SWITCH_PC3_CLOSE)

// There is an external 32kHz oscillator
#define RTC_ASYNCH_PREDIV           (0)
#define RTC_SYNCH_PREDIV            (0x7fff)
#define MICROPY_HW_RTC_USE_BYPASS   (1)
#define MICROPY_HW_RTC_USE_US       (1)
#define MICROPY_HW_RTC_USE_CALOUT   (1)

#if !MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
// QSPI flash for storage
#define MICROPY_HW_QSPI_PRESCALER           (2) // 100MHz
#define MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 (27)
// Reserve 4MiB for romfs and 1MiB for WiFi/BT firmware.
#define MICROPY_HW_SPIFLASH_SIZE_BITS       (88 * 1024 * 1024)
#define MICROPY_HW_QSPIFLASH_CS             (pyb_pin_QSPI2_CS)
#define MICROPY_HW_QSPIFLASH_SCK            (pyb_pin_QSPI2_CLK)
#define MICROPY_HW_QSPIFLASH_IO0            (pyb_pin_QSPI2_D0)
#define MICROPY_HW_QSPIFLASH_IO1            (pyb_pin_QSPI2_D1)
#define MICROPY_HW_QSPIFLASH_IO2            (pyb_pin_QSPI2_D2)
#define MICROPY_HW_QSPIFLASH_IO3            (pyb_pin_QSPI2_D3)

// SPI flash #1, block device config
extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;
#define MICROPY_HW_BDEV_SPIFLASH            (&spi_bdev)
#define MICROPY_HW_BDEV_SPIFLASH_CONFIG     (&spiflash_config)
#define MICROPY_HW_BDEV_SPIFLASH_SIZE_BYTES (MICROPY_HW_SPIFLASH_SIZE_BITS / 8)
#define MICROPY_HW_BDEV_SPIFLASH_EXTENDED   (&spi_bdev)
#endif

// 4 wait states
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_2

// UART config
#define MICROPY_HW_UART1_TX         (pyb_pin_BT_TXD)
#define MICROPY_HW_UART1_RX         (pyb_pin_BT_RXD)
#define MICROPY_HW_UART1_RTS        (pyb_pin_BT_RTS)
#define MICROPY_HW_UART1_CTS        (pyb_pin_BT_CTS)

#define MICROPY_HW_UART4_TX         (pin_B9)
#define MICROPY_HW_UART4_RX         (pin_B8)

// LPUART1 config
#define MICROPY_HW_LPUART1_TX       (pin_A9)
#define MICROPY_HW_LPUART1_RX       (pin_A10)

// I2C buses
#define MICROPY_HW_I2C1_SCL         (pin_B8)
#define MICROPY_HW_I2C1_SDA         (pin_B9)

#define MICROPY_HW_I2C2_SCL         (pin_F1)
#define MICROPY_HW_I2C2_SDA         (pin_F0)

#define MICROPY_HW_I2C3_SCL         (pin_A8)
#define MICROPY_HW_I2C3_SDA         (pin_C9)

// SPI buses
#define MICROPY_HW_SPI4_NSS         (pin_E11)
#define MICROPY_HW_SPI4_SCK         (pin_E12)
#define MICROPY_HW_SPI4_MISO        (pin_E13)
#define MICROPY_HW_SPI4_MOSI        (pin_E14)

#define MICROPY_HW_SPI5_NSS         (pin_F6)
#define MICROPY_HW_SPI5_SCK         (pin_F7)
#define MICROPY_HW_SPI5_MISO        (pin_F8)
#define MICROPY_HW_SPI5_MOSI        (pin_F11)

// FDCAN bus
#define MICROPY_HW_CAN1_NAME        "FDCAN1"
#define MICROPY_HW_CAN1_TX          (pin_B9)
#define MICROPY_HW_CAN1_RX          (pin_B8)
#define MICROPY_HW_CAN_IS_RESERVED(id) (id != PYB_CAN_1)

// LEDs
#define MICROPY_HW_LED1             (pyb_pin_LED_RED) // red
#define MICROPY_HW_LED2             (pyb_pin_LED_GREEN) // green
#define MICROPY_HW_LED3             (pyb_pin_LED_BLUE) // yellow
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))

// WiFi SDMMC
#define MICROPY_HW_SDIO_SDMMC       (2)
#define MICROPY_HW_SDIO_CK          (pin_D6)
#define MICROPY_HW_SDIO_CMD         (pin_D7)
#define MICROPY_HW_SDIO_D0          (pin_B14)
#define MICROPY_HW_SDIO_D1          (pin_B15)
#define MICROPY_HW_SDIO_D2          (pin_G11)
#define MICROPY_HW_SDIO_D3          (pin_B4)

// USB config
#define MICROPY_HW_USB_HS           (1)
#define MICROPY_HW_USB_HS_ULPI_NXT  (pin_C3)
#define MICROPY_HW_USB_HS_ULPI_STP  (pin_C0)
#define MICROPY_HW_USB_HS_ULPI_DIR  (pin_C2)
#define MICROPY_HW_USB_HS_ULPI3320  (1)

#define MICROPY_HW_USB_CDC_RX_DATA_SIZE     (1024)
#define MICROPY_HW_USB_CDC_TX_DATA_SIZE     (1024)
#define MICROPY_HW_USB_CDC_1200BPS_TOUCH    (1)
#define GPIO_AF10_OTG_HS                    (GPIO_AF10_OTG2_HS)

// Bluetooth config
#define MICROPY_HW_BLE_UART_ID              (PYB_UART_1)
#define MICROPY_HW_BLE_UART_BAUDRATE        (115200)
#define MICROPY_HW_BLE_UART_BAUDRATE_SECONDARY (3000000)
#define MICROPY_HW_BLE_UART_BAUDRATE_DOWNLOAD_FIRMWARE (3000000)

#define MICROPY_HW_USB_VID                      0x2341
#define MICROPY_HW_USB_PID                      0x055F
#define MICROPY_HW_USB_PID_CDC_MSC              (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC_HID              (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC                  (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_MSC                  (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC2_MSC             (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC2                 (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC3                 (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC3_MSC             (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC_MSC_HID          (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC2_MSC_HID         (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC3_MSC_HID         (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_LANGID_STRING            0x409
#define MICROPY_HW_USB_MANUFACTURER_STRING      "Arduino"
#define MICROPY_HW_USB_PRODUCT_FS_STRING        "Nicla Vision Virtual Comm Port in FS Mode"
#define MICROPY_HW_USB_PRODUCT_HS_STRING        "Nicla Vision Virtual Comm Port in HS Mode"
#define MICROPY_HW_USB_INTERFACE_FS_STRING      "Nicla Vision Interface"
#define MICROPY_HW_USB_INTERFACE_HS_STRING      "Nicla Vision Interface"
#define MICROPY_HW_USB_CONFIGURATION_FS_STRING  "Nicla Vision Config"
#define MICROPY_HW_USB_CONFIGURATION_HS_STRING  "Nicla Vision Config"

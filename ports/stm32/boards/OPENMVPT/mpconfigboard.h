/*
 * This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (C) 2013-2025 OpenMV, LLC.
 */

#define MICROPY_HW_BOARD_NAME       "OPENMVPT"
#define MICROPY_HW_MCU_NAME         "STM32H743"
#define MICROPY_PY_SYS_PLATFORM     "OpenMVPureThermal"
#define MICROPY_HW_FLASH_FS_LABEL   "OPENMV"

// Network config
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-openmv-4p"

#define MICROPY_OBJ_REPR            (MICROPY_OBJ_REPR_C)

#define MICROPY_FATFS_EXFAT         (1)
#define MICROPY_GC_SPLIT_HEAP       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_SPI2      (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SDCARD    (1)
#define MICROPY_HW_ENTER_BOOTLOADER_VIA_RESET   (0)
#define MICROPY_HW_TIM_IS_RESERVED(id) (id == 1 || id == 6)

// ROMFS config
#define MICROPY_HW_ROMFS_ENABLE_EXTERNAL_QSPI   (1)
#define MICROPY_HW_ROMFS_QSPI_SPIFLASH_OBJ      (&spi_bdev.spiflash)
#define MICROPY_HW_ROMFS_ENABLE_PART0           (1)

// Flash storage config
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE    (1)
#define MICROPY_HW_SPIFLASH_SOFT_RESET      (1)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)

#define MICROPY_BOARD_EARLY_INIT board_early_init
void board_early_init(void);

extern void board_enter_bootloader(void);
#define MICROPY_BOARD_ENTER_BOOTLOADER(nargs, args) board_enter_bootloader()

// The board has a 12MHz HSE.
// PLL1 480MHz, 48MHz for SDMMC and FDCAN.
// USB and RNG are clocked from the HSI48.
#define MICROPY_HW_CLK_PLLM         (3)
#define MICROPY_HW_CLK_PLLN         (240)
#define MICROPY_HW_CLK_PLLP         (2)
#define MICROPY_HW_CLK_PLLQ         (20)
#define MICROPY_HW_CLK_PLLR         (2)
#define MICROPY_HW_CLK_PLLVCI       (RCC_PLL1VCIRANGE_2)
#define MICROPY_HW_CLK_PLLVCO       (RCC_PLL1VCOWIDE)
#define MICROPY_HW_CLK_PLLFRAC      (0)

// PLL2 200MHz for FMC and QSPI.
#define MICROPY_HW_CLK_PLL2M        (3)
#define MICROPY_HW_CLK_PLL2N        (100)
#define MICROPY_HW_CLK_PLL2P        (2)
#define MICROPY_HW_CLK_PLL2Q        (2)
#define MICROPY_HW_CLK_PLL2R        (2)
#define MICROPY_HW_CLK_PLL2VCI      (RCC_PLL2VCIRANGE_2)
#define MICROPY_HW_CLK_PLL2VCO      (RCC_PLL2VCOWIDE)
#define MICROPY_HW_CLK_PLL2FRAC     (0)

// PLL3 160MHz for ADC and SPI123.
#define MICROPY_HW_CLK_PLL3M        (3)
#define MICROPY_HW_CLK_PLL3N        (80)
#define MICROPY_HW_CLK_PLL3P        (2)
#define MICROPY_HW_CLK_PLL3Q        (2)
#define MICROPY_HW_CLK_PLL3R        (2)
#define MICROPY_HW_CLK_PLL3VCI      (RCC_PLL3VCIRANGE_2)
#define MICROPY_HW_CLK_PLL3VCO      (RCC_PLL3VCOWIDE)
#define MICROPY_HW_CLK_PLL3FRAC     (0)

// Bus clock divider values
#define MICROPY_HW_CLK_AHB_DIV      (RCC_HCLK_DIV2)
#define MICROPY_HW_CLK_APB1_DIV     (RCC_APB1_DIV2)
#define MICROPY_HW_CLK_APB2_DIV     (RCC_APB2_DIV2)
#define MICROPY_HW_CLK_APB3_DIV     (RCC_APB3_DIV2)
#define MICROPY_HW_CLK_APB4_DIV     (RCC_APB4_DIV2)

// Peripheral clock sources
#define MICROPY_HW_RCC_HSI48_STATE      (RCC_HSI48_ON)
#define MICROPY_HW_RCC_USB_CLKSOURCE    (RCC_USBCLKSOURCE_HSI48)
#define MICROPY_HW_RCC_RNG_CLKSOURCE    (RCC_RNGCLKSOURCE_HSI48)
#define MICROPY_HW_RCC_ADC_CLKSOURCE    (RCC_ADCCLKSOURCE_PLL3)
#define MICROPY_HW_RCC_SDMMC_CLKSOURCE  (RCC_SDMMCCLKSOURCE_PLL)
#define MICROPY_HW_RCC_FDCAN_CLKSOURCE  (RCC_FDCANCLKSOURCE_PLL)
#define MICROPY_HW_RCC_SPI123_CLKSOURCE (RCC_SPI123CLKSOURCE_PLL3)
#define MICROPY_HW_RCC_FMC_CLKSOURCE    (RCC_FMCCLKSOURCE_PLL2)
#define MICROPY_HW_RCC_QSPI_CLKSOURCE   (RCC_QSPICLKSOURCE_PLL2)

#define MICROPY_HW_FLASH_LATENCY    (FLASH_LATENCY_2)

// Use external 32kHz crystal for the RTC.
#define MICROPY_HW_RTC_USE_LSE      (1)
#define MICROPY_HW_RTC_USE_BYPASS   (0)
#define MICROPY_HW_RTC_USE_US       (1)
#define MICROPY_HW_RTC_USE_CALOUT   (0)

void board_low_power(int mode);
#define MICROPY_BOARD_LEAVE_STOP    board_low_power(0);
#define MICROPY_BOARD_ENTER_STOP    board_low_power(1);
#define MICROPY_BOARD_ENTER_STANDBY board_low_power(2);

// UART1 config
#define MICROPY_HW_UART1_TX  (pin_B14)
#define MICROPY_HW_UART1_RX  (pin_B15)

// UART3 config
#define MICROPY_HW_UART3_TX  (pin_B10)
#define MICROPY_HW_UART3_RX  (pin_B11)
#define MICROPY_HW_UART3_RTS (pin_B14)
#define MICROPY_HW_UART3_CTS (pin_B13)

// I2C buses
#define MICROPY_HW_I2C2_SCL (pin_B10)
#define MICROPY_HW_I2C2_SDA (pin_B11)

#define MICROPY_HW_I2C4_SCL (pin_D12)
#define MICROPY_HW_I2C4_SDA (pin_D13)

// SPI buses
#define MICROPY_HW_SPI2_NSS  (pin_B12)
#define MICROPY_HW_SPI2_SCK  (pin_B13)
#define MICROPY_HW_SPI2_MISO (pin_B14)
#define MICROPY_HW_SPI2_MOSI (pin_B15)

// SPI buses
#define MICROPY_HW_SPI3_NSS  (pin_A15)
#define MICROPY_HW_SPI3_SCK  (pin_B3)
#define MICROPY_HW_SPI3_MISO (pin_B4)
#define MICROPY_HW_SPI3_MOSI (pin_B5)

// FDCAN bus
#define MICROPY_HW_CAN2_NAME "FDCAN2"
#define MICROPY_HW_CAN2_TX   (pin_B13)
#define MICROPY_HW_CAN2_RX   (pin_B12)
#define MICROPY_HW_CAN_IS_RESERVED(id) (id != PYB_CAN_2)

// SD card detect switch
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_G7)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)

// USB config
#define MICROPY_HW_USB_FS                   (1)
#define MICROPY_HW_USB_CDC_RX_DATA_SIZE     (512)
#define MICROPY_HW_USB_CDC_TX_DATA_SIZE     (512)
#define MICROPY_HW_USB_VBUS_DETECT_PIN      (pin_A9)

// LEDs
#define MICROPY_HW_LED1             (pin_C0) // red
#define MICROPY_HW_LED2             (pin_C1) // green
#define MICROPY_HW_LED3             (pin_C2) // blue
#define MICROPY_HW_LED4             (pin_G3) // white
#define MICROPY_HW_LED_OTYPE        (GPIO_MODE_OUTPUT_PP)
// NOTE: The RGB LED is active low.  The white illuminator is active high, PG3
// drives the gate of an N-MOSFET.
#define MICROPY_HW_LED_ON(pin)      ((pin)->gpio->BSRR = ((pin) == MICROPY_HW_LED4) ? (pin)->pin_mask : ((pin)->pin_mask << 16))
#define MICROPY_HW_LED_OFF(pin)     ((pin)->gpio->BSRR = ((pin) == MICROPY_HW_LED4) ? ((pin)->pin_mask << 16) : (pin)->pin_mask)

// Servos, on TIM4
#define MICROPY_HW_SERVO_TIM        (4)
#define MICROPY_HW_SERVO1           (pin_D12) // P7
#define MICROPY_HW_SERVO2           (pin_D13) // P8
#define PYB_SERVO_NUM (2)

// Board network interfaces config.
#if MICROPY_PY_WINC1500
extern const struct _mp_obj_type_t mod_network_nic_type_winc;
#define MICROPY_PY_USOCKET_EXTENDED_STATE   (1)
#define MICROPY_BOARD_NETWORK_INTERFACES \
    { MP_ROM_QSTR(MP_QSTR_WINC), MP_ROM_PTR(&mod_network_nic_type_winc) }, \
    { MP_ROM_QSTR(MP_QSTR_WLAN), MP_ROM_PTR(&mod_network_nic_type_winc) },
#else
#define MICROPY_BOARD_NETWORK_INTERFACES
#endif

// QSPI Flash 256MBits
#define MICROPY_HW_QSPI_PRESCALER           (2) // F_CLK = F_AHB/2 (100MHz)
#define MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 (28)
// Reserve 8MiB for romfs
#define MICROPY_HW_SPIFLASH_SIZE_BITS       (192 * 1024 * 1024)
#define MICROPY_HW_QSPIFLASH_CS             (pin_G6)
#define MICROPY_HW_QSPIFLASH_SCK            (pin_F10)
#define MICROPY_HW_QSPIFLASH_IO0            (pin_F8)
#define MICROPY_HW_QSPIFLASH_IO1            (pin_F9)
#define MICROPY_HW_QSPIFLASH_IO2            (pin_F7)
#define MICROPY_HW_QSPIFLASH_IO3            (pin_F6)

// block device config for SPI flash
extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;
#define MICROPY_HW_BDEV_SPIFLASH            (&spi_bdev)
#define MICROPY_HW_BDEV_SPIFLASH_CONFIG     (&spiflash_config)
#define MICROPY_HW_BDEV_SPIFLASH_EXTENDED   (&spi_bdev)
#define MICROPY_HW_BDEV_SPIFLASH_SIZE_BYTES (MICROPY_HW_SPIFLASH_SIZE_BITS / 8)

// SDRAM
#define MICROPY_HW_SDRAM_SIZE               (64 * 1024 * 1024)
#define MICROPY_HW_SDRAM_STARTUP_TEST       (1)
#define MICROPY_HW_SDRAM_TEST_FAIL_ON_ERROR (true)

// Timing configuration for 200MHz/2=100MHz (10ns)
#define MICROPY_HW_SDRAM_CLOCK_PERIOD       2
#define MICROPY_HW_SDRAM_CAS_LATENCY        2
#define MICROPY_HW_SDRAM_FREQUENCY_KHZ      (100000) // 100 MHz
#define MICROPY_HW_SDRAM_TIMING_TMRD        (2)
#define MICROPY_HW_SDRAM_TIMING_TXSR        (7)
#define MICROPY_HW_SDRAM_TIMING_TRAS        (5)
#define MICROPY_HW_SDRAM_TIMING_TRC         (7)
#define MICROPY_HW_SDRAM_TIMING_TWR         (3)
#define MICROPY_HW_SDRAM_TIMING_TRP         (2)
#define MICROPY_HW_SDRAM_TIMING_TRCD        (2)

// 32-bit SDRAM
#define MICROPY_HW_SDRAM_ROW_BITS_NUM       13
#define MICROPY_HW_SDRAM_MEM_BUS_WIDTH      32
#define MICROPY_HW_SDRAM_REFRESH_CYCLES     8192

// IS42S32160F-7TLI - Industrial - 143MHz - 7ns - CAS Latency 2 - 10 ns - 100 MHz - 6 ns
#define MICROPY_HW_SDRAM_COLUMN_BITS_NUM    9
#define MICROPY_HW_SDRAM_INTERN_BANKS_NUM   4
#define MICROPY_HW_SDRAM_RPIPE_DELAY        0
#define MICROPY_HW_SDRAM_RBURST             (1)
#define MICROPY_HW_SDRAM_WRITE_PROTECTION   (0)

#define MICROPY_HW_SDRAM_AUTOREFRESH_NUM    (8)
#define MICROPY_HW_SDRAM_BURST_LENGTH       1
#define MICROPY_HW_SDRAM_REFRESH_RATE       (64) // ms

#define MICROPY_HW_FMC_SDCKE0   (pin_C5)
#define MICROPY_HW_FMC_SDNE0    (pin_C4)
#define MICROPY_HW_FMC_SDCLK    (pin_G8)
#define MICROPY_HW_FMC_SDNCAS   (pin_G15)
#define MICROPY_HW_FMC_SDNRAS   (pin_F11)
#define MICROPY_HW_FMC_SDNWE    (pin_A7)
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
#define MICROPY_HW_FMC_A12      (pin_G2)
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

#define MICROPY_HW_USB_VID                      0x37C5
#ifndef MICROPY_HW_USB_PID
#define MICROPY_HW_USB_PID                      0x1205
#endif
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
#define MICROPY_HW_USB_MANUFACTURER_STRING      "OpenMV"
#define MICROPY_HW_USB_PRODUCT_FS_STRING        "OpenMV Virtual Comm Port in FS Mode"
#define MICROPY_HW_USB_PRODUCT_HS_STRING        "OpenMV Virtual Comm Port in HS Mode"
#define MICROPY_HW_USB_INTERFACE_FS_STRING      "VCP Interface"
#define MICROPY_HW_USB_INTERFACE_HS_STRING      "VCP Interface"
#define MICROPY_HW_USB_CONFIGURATION_FS_STRING  "VCP Config"
#define MICROPY_HW_USB_CONFIGURATION_HS_STRING  "VCP Config"

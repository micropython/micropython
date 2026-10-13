/*
 * This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (C) 2013-2025 OpenMV, LLC.
 */

#define MICROPY_HW_BOARD_NAME       "OPENMV4"
#define MICROPY_HW_MCU_NAME         "STM32H743"
#define MICROPY_PY_SYS_PLATFORM     "OpenMV4-H7"
#define MICROPY_HW_FLASH_FS_LABEL   "OPENMV"

// Network config
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-openmv-4"

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
#define MICROPY_HW_ROMFS_ENABLE_INTERNAL_FLASH  (1)
#define MICROPY_HW_ROMFS_ENABLE_PART0           (1)

#define MICROPY_BOARD_ENTER_STANDBY

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

#define MICROPY_HW_FLASH_LATENCY    (FLASH_LATENCY_2)

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

// FDCAN bus
#define MICROPY_HW_CAN2_NAME "FDCAN2"
#define MICROPY_HW_CAN2_TX   (pin_B13)
#define MICROPY_HW_CAN2_RX   (pin_B12)
#define MICROPY_HW_CAN_IS_RESERVED(id) (id != PYB_CAN_2)

// SD card detect switch
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_D0)
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
#define MICROPY_HW_LED4             (pin_E2) // IR
#define MICROPY_HW_LED_OTYPE        (GPIO_MODE_OUTPUT_PP)
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_write((pin), ((pin) == MICROPY_HW_LED4)))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_write((pin), ((pin) != MICROPY_HW_LED4)))

// Servos, on TIM4
#define MICROPY_HW_SERVO_TIM        (4)
#define MICROPY_HW_SERVO1           (pin_D12) // P7
#define MICROPY_HW_SERVO2           (pin_D13) // P8
#define MICROPY_HW_SERVO3           (pin_D14) // P9
#define MICROPY_HW_SERVO4           (pin_D15) // P10
#define PYB_SERVO_NUM (4)

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

#define MICROPY_HW_USB_VID                      0x37C5
#ifndef MICROPY_HW_USB_PID
#define MICROPY_HW_USB_PID                      0x1204
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

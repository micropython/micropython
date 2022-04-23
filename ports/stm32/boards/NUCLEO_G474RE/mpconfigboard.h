#define MICROPY_HW_BOARD_NAME       "NUCLEO_G474RE"
#define MICROPY_HW_MCU_NAME         "STM32G474"

#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_ENABLE_DAC       (1) // A4, A5
#define MICROPY_HW_ENABLE_USB       (0) // A12 (dp), A11 (dm)
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (0) // QSPI extflash not mounted

#define MICROPY_PY_UASYNCIO         (0)
#define MICROPY_PY_UZLIB            (0)
#define MICROPY_PY_UBINASCII        (0)
#define MICROPY_PY_UHASHLIB         (0)
#define MICROPY_PY_UJSON            (0)
#define MICROPY_PY_URE              (0)
#define MICROPY_PY_FRAMEBUF         (0)
#define MICROPY_PY_USOCKET          (0)
#define MICROPY_PY_NETWORK          (0)

// The board has an 24MHz HSE, the following gives 170MHz CPU speed
#define MICROPY_HW_CLK_PLLM         (6)
#define MICROPY_HW_CLK_PLLN         (85)
#define MICROPY_HW_CLK_PLLP         (2)
#define MICROPY_HW_CLK_PLLQ         (8)
#define MICROPY_HW_CLK_PLLR         (2)

#define MICROPY_HW_CLK_USE_HSI48    (1) // for RNG

// 4 wait states
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_8

// UART config
#define MICROPY_HW_LPUART1_TX       (pin_A2)  // A2 (to STLINK), B11, C1
#define MICROPY_HW_LPUART1_RX       (pin_A3)  // A3 (to STLINK), B10, C0
#define MICROPY_HW_UART1_TX         (pin_C4)  // A9, B6, C4, E0
#define MICROPY_HW_UART1_RX         (pin_C5)  // A10, B7, C5, E1
// #define MICROPY_HW_UART2_TX       (pin_A2)  // A14, B3, D5
// #define MICROPY_HW_UART2_RX       (pin_A3)  // A15, B4, D6
// #define MICROPY_HW_UART2_RTS      (pin_A1)  // D4
// #define MICROPY_HW_UART2_CTS      (pin_A0)  // D3
#define MICROPY_HW_UART3_TX         (pin_B10) // B9, B10, C10, D8
#define MICROPY_HW_UART3_RX         (pin_B11) // B8 = boot0, B11, C11, D9, E15
// #define MICROPY_HW_UART3_RTS      (pin_B14) // D12, F6
// #define MICROPY_HW_UART3_CTS      (pin_B13) // A13, D11
// #define MICROPY_HW_UART4_TX       (pin_)    // C10
// #define MICROPY_HW_UART4_RX       (pin_)    // C11

#define MICROPY_HW_UART_REPL        (PYB_LPUART_1) // default on Nucleo G474
#define MICROPY_HW_UART_REPL_BAUD   (115200)

// I2C buses
#define MICROPY_HW_I2C1_SCL         (pin_B8) // A13, A15, B8 (=boot0)
#define MICROPY_HW_I2C1_SDA         (pin_B9) // A14, B7, B9
// #define MICROPY_HW_I2C2_SCL       (pin_)   // A9, C4
// #define MICROPY_HW_I2C2_SDA       (pin_)   // A8, F0
// #define MICROPY_HW_I2C3_SCL       (pin_)   // A8, C8
// #define MICROPY_HW_I2C3_SDA       (pin_)   // B5, C9, C11

// SPI
#define MICROPY_HW_SPI1_NSS         (pin_A4) // A4, A15 (Nucleo64 specifies B6 as pin CS, must be done as GPIO, not as AF)
#define MICROPY_HW_SPI1_SCK         (pin_A5) // A5 (LED), B3 (SWO)
#define MICROPY_HW_SPI1_MISO        (pin_A6) // A6, B4
#define MICROPY_HW_SPI1_MOSI        (pin_A7) // A7, B5
// #define MICROPY_HW_SPI2_NSS       (pin_)   // B12, D15, F0
// #define MICROPY_HW_SPI2_SCK       (pin_)   // B13, F1, F9, F10
// #define MICROPY_HW_SPI2_MISO      (pin_)   // A10, B14
// #define MICROPY_HW_SPI2_MOSI      (pin_)   // A11, B15
// #define MICROPY_HW_SPI3_NSS       (pin_)   // A4, A15
// #define MICROPY_HW_SPI3_SCK       (pin_)   // B3, C10
// #define MICROPY_HW_SPI3_MISO      (pin_)   // B4, C11
// #define MICROPY_HW_SPI3_MOSI      (pin_)   // B5, C12

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_C13)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_A5)    // green
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB config pin A12 (dp), A11 (dm) not mounted on Nucleo
// #define MICROPY_HW_USB_FS              (1)
// #define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_A9)
// #define MICROPY_HW_USB_OTG_ID_PIN      (pin_A10)

// FDCAN bus
// User TODO: fit transceiver chip
#define MICROPY_HW_CAN1_NAME        "FDCAN1"
#define MICROPY_HW_CAN1_TX          (pin_A12) // A12, B9, D1
#define MICROPY_HW_CAN1_RX          (pin_A11) // A11, B8, D0

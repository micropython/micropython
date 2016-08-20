// The following would be from a board specific file, if one existed

#define MICROPY_HW_BOARD_NAME       "Teensy-3.1"
#define MICROPY_HW_MCU_NAME         "MK20DX256"

#define MICROPY_HW_HAS_SWITCH       (0)
#define MICROPY_HW_HAS_SDCARD       (0)
#define MICROPY_HW_HAS_MMA7660      (0)
#define MICROPY_HW_HAS_LIS3DSH      (0)
#define MICROPY_HW_HAS_LCD          (0)
#define MICROPY_HW_ENABLE_RNG       (0)
#define MICROPY_HW_ENABLE_RTC       (0)
#define MICROPY_HW_ENABLE_TIMER     (0)
#define MICROPY_HW_ENABLE_SERVO     (0)
#define MICROPY_HW_ENABLE_DAC       (0)
#define MICROPY_HW_ENABLE_I2C1      (0)
#define MICROPY_HW_ENABLE_SPI1      (0)
#define MICROPY_HW_ENABLE_SPI3      (0)
#define MICROPY_HW_ENABLE_CC3K      (0)

#define MICROPY_HW_LED1             (pin_C5)
#define MICROPY_HW_LED_OTYPE        (GPIO_MODE_OUTPUT_PP)
#define MICROPY_HW_LED_ON(pin)      (pin->gpio->PSOR = pin->pin_mask)
#define MICROPY_HW_LED_OFF(pin)     (pin->gpio->PCOR = pin->pin_mask)

#define MICROPY_HW_NUM_UARTS        3

#define MICROPY_HW_UART0_NUM        1
#define MICROPY_HW_UART0_TX         (pin_B17)   // also D7
#define MICROPY_HW_UART0_RX         (pin_B16)   // also D6
#define MICROPY_HW_UART0_RTS        (pin_B2)    // also D4
#define MICROPY_HW_UART0_CTS        (pin_B3)    // also D5

#define MICROPY_HW_UART1_NUM        2
#define MICROPY_HW_UART1_TX         (pin_C4)    // also E0
#define MICROPY_HW_UART1_RX         (pin_C3)    // also E1
#define MICROPY_HW_UART1_RTS        (pin_C1)
#define MICROPY_HW_UART1_CTS        (pin_C2)

#define MICROPY_HW_UART2_NUM        3
#define MICROPY_HW_UART2_TX         (pin_D3)
#define MICROPY_HW_UART2_RX         (pin_D2)
#define MICROPY_HW_UART2_RTS        (pin_D0)
#define MICROPY_HW_UART2_CTS        (pin_D1)


#if 0
// SD card detect switch
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_A8)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)
#endif

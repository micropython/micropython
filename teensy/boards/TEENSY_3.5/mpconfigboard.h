#define MICROPY_HW_BOARD_NAME       "Teensy-3.5"
#define MICROPY_HW_MCU_NAME         "MK64FX512"

// MICROPY_HW_HAS_SDCARD comes from mpconfigboard.mk

#define MICROPY_HW_HAS_SWITCH       (0)
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

#define MICROPY_HW_NUM_UARTS        6

// Numbers in parenthesis are the pin numbers (0-57) which have a gray
// background on the reference card.

#define MICROPY_HW_UART0_NUM        1
#define MICROPY_HW_UART0_TX         (pin_B17)   // B17(1)  also A14(26), D7(5)
#define MICROPY_HW_UART0_RX         (pin_B16)   // B16(0)  also A15(27), D6(21)
#define MICROPY_HW_UART0_RTS        (pin_B2)    // B2(19)  also A17(39), D4(6)
#define MICROPY_HW_UART0_CTS        (pin_B3)    // B3(18)  also A16(28), D5(20)

#define MICROPY_HW_UART1_NUM        2
#define MICROPY_HW_UART1_TX         (pin_C4)    // C4(10)  also E0(58), 
#define MICROPY_HW_UART1_RX         (pin_C3)    // C3(9)   also E1(59), 
#define MICROPY_HW_UART1_RTS        (pin_C1)    // C1(22)
#define MICROPY_HW_UART1_CTS        (pin_C2)    // C2(23)

#define MICROPY_HW_UART2_NUM        3
#define MICROPY_HW_UART2_TX         (pin_D3)    // D3(8)
#define MICROPY_HW_UART2_RX         (pin_D2)    // D2(7)
#define MICROPY_HW_UART2_RTS        (pin_D0)    // D0(2)
#define MICROPY_HW_UART2_CTS        (pin_D1)    // D1(14)

#define MICROPY_HW_UART3_NUM        4
#define MICROPY_HW_UART3_TX         (pin_B11)   // B11(32)
#define MICROPY_HW_UART3_RX         (pin_B10)   // B10(31)

#define MICROPY_HW_UART4_NUM        5
#define MICROPY_HW_UART4_TX         (pin_E24)   // E24(33)
#define MICROPY_HW_UART4_RX         (pin_E25)   // E25(34)
//      MICROPY_HW_UART4_RTS        not available on any accessible pins
#define MICROPY_HW_UART4_CTS        (pin_E26)   // E26(24)

#define MICROPY_HW_UART5_NUM        6
#define MICROPY_HW_UART5_TX         (pin_D9)    // D9(48)
#define MICROPY_HW_UART5_RX         (pin_D8)    // D8(47)
#define MICROPY_HW_UART5_RTS        (pin_E11)   // E11(56)
#define MICROPY_HW_UART5_CTS        (pin_E10)   // E10(57) also D11(55)

#if 0
// SD card detect switch
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_A8)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)
#endif

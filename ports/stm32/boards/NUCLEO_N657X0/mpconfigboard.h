#define MICROPY_HW_BOARD_NAME       "NUCLEO-N657X0"
#define MICROPY_HW_MCU_NAME         "STM32N657X0"

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (0)
#define MICROPY_HW_ENABLE_RNG       (0)
#define MICROPY_HW_ENABLE_RTC       (0)
#define MICROPY_HW_ENABLE_ADC       (0)
#define MICROPY_HW_ENABLE_DAC       (0)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_PY_PYB_LEGACY       (0)

#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)

// HSE is 48MHz, this gives a CPU frequency of 800MHz.
#define MICROPY_HW_CLK_PLLM         (6)
#define MICROPY_HW_CLK_PLLN         (100)
#define MICROPY_HW_CLK_PLLP1        (1)
#define MICROPY_HW_CLK_PLLP2        (1)
#define MICROPY_HW_CLK_PLLFRAC      (0)

// USART1 config
#define MICROPY_HW_UART1_TX         (pyb_pin_UART1_TX)
#define MICROPY_HW_UART1_RX         (pyb_pin_UART1_RX)
// USART1 is connected to the virtual com port on the ST-Link
#define MICROPY_HW_UART_REPL        PYB_UART_1
#define MICROPY_HW_UART_REPL_BAUD   115200

// USER2 is floating, and pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pyb_pin_BUTTON)
#define MICROPY_HW_USRSW_PULL       (GPIO_PULLDOWN)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pyb_pin_LED_BLUE)
#define MICROPY_HW_LED2             (pyb_pin_LED_RED)
#define MICROPY_HW_LED3             (pyb_pin_LED_GREEN)
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))

// USB config
#define MICROPY_HW_USB_HS           (1)
#define MICROPY_HW_USB_HS_IN_FS     (1)
#define MICROPY_HW_USB_MAIN_DEV     (USB_PHY_HS_ID)
#define MICROPY_HW_USB_MSC          (0)
#define MICROPY_HW_USB_HID          (0)

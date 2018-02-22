8#define MICROPY_HW_BOARD_NAME       "NUCLEO_H743ZI"
#define MICROPY_HW_MCU_NAME         "STM32H743"

#define MICROPY_HW_HAS_SWITCH       (0)
#define MICROPY_HW_HAS_FLASH        (0)
#define MICROPY_HW_HAS_SDCARD       (0)
#define MICROPY_HW_ENABLE_RNG       (0)
#define MICROPY_HW_ENABLE_RTC       (0)
#define MICROPY_HW_ENABLE_CAN       (0)
#define MICROPY_HW_ENABLE_USB       (0)
#define MICROPY_HW_ENABLE_ADC       (0)

/**
* System Clock Configuration
* The system Clock is configured as follows:
*    System Clock source            = PLL (HSE BYPASS)
*    SYSCLK(Hz)                     = 400000000 (CPU Clock)
*    HCLK(Hz)                       = 200000000 (AXI and AHBs Clock)
*    AHB Prescaler                  = 2
*    D1 APB3 Prescaler              = 2 (APB3 Clock  100MHz)
*    D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
*    D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
*    D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
*    HSE Frequency(Hz)              = 8000000
*    PLL_M                          = 4
*    PLL_N                          = 400
*    PLL_P                          = 2
*    PLL_Q                          = 4
*    PLL_R                          = 2
*    VDD(V)                         = 3.3v
*    Flash Latency(WS)              = 4
*/
#define MICROPY_HW_CLK_PLLM (4)
#define MICROPY_HW_CLK_PLLN (400)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (4)
#define MICROPY_HW_CLK_PLLR (2)

// 4 wait states
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_4

// UART config
//#define MICROPY_HW_UART1_TX         (pin_A9)
//#define MICROPY_HW_UART1_RX         (pin_B7)
//#define MICROPY_HW_UART6_TX         (pin_C6)
//#define MICROPY_HW_UART6_RX         (pin_C7)
//#define MICROPY_HW_UART7_TX         (pin_F6)
//#define MICROPY_HW_UART7_RX         (pin_F7)
//#define MICROPY_HW_UART_REPL        PYB_UART_1
//#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C busses
//#define MICROPY_HW_I2C1_SCL         (pin_B8)
//#define MICROPY_HW_I2C1_SDA         (pin_B9)
//
//#define MICROPY_HW_I2C3_SCL         (pin_H7)
//#define MICROPY_HW_I2C3_SDA         (pin_H8)

// SPI
//#define MICROPY_HW_SPI2_NSS         (pin_I0)
//#define MICROPY_HW_SPI2_SCK         (pin_I1)
//#define MICROPY_HW_SPI2_MISO        (pin_B14)
//#define MICROPY_HW_SPI2_MOSI        (pin_B15)

// USRSW is pulled low. Pressing the button makes the input go high.
//#define MICROPY_HW_USRSW_PIN        (pin_I11)
//#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
//#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
//#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
//#define MICROPY_HW_LED1             (pin_I1) // green
//#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
//#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// SD card detect switch
//#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_C13)
//#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
//#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)

// USB config (CN13 - USB OTG FS)
// The Hardware VBUS detect only works on pin PA9. The STM32F7 Discovery uses
// PA9 for VCP_TX functionality and connects the VBUS to pin J12 (so software
// only detect). So we don't define the VBUS detect pin since that requires PA9.
#define MICROPY_HW_USB_FS              (1)
//#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_J12)
//#define MICROPY_HW_USB_OTG_ID_PIN      (pin_A10)

/* This file is part of the MicroPython project, http://micropython.org/
 * MIT License; Copyright (c) 2021 Damien P. George
 */

// STM32F407VET6 Robot controller by WHEELTEC
// Used in their ROS robots, see:
// https://www.aliexpress.com/item/1005002541064402.html

#define MICROPY_HW_BOARD_NAME       "WHEELTEC STM32F407VET6 1.0"
#define MICROPY_HW_MCU_NAME         "STM32F407VE"

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)
// No USB  : Not populated.
// No Servo: PA[3:0] used for Motor drive and QEI.

// HSE is 8MHz
#define MICROPY_HW_CLK_PLLM (8)             // divide external clock by 8 for 1MHz
#define MICROPY_HW_CLK_PLLN (336)           // PLL clock in MHz
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2) // divide PLL clock by this to get core clock
#define MICROPY_HW_CLK_PLLQ (7)             // divide core clock by this to get 48MHz

// The board does not have a RTC crystal.
#define MICROPY_HW_RTC_USE_LSE      (0)

// USARTs
// USART1: Connected to CP2102 serving USB1.
#define MICROPY_HW_UART1_TX     (pin_A9)
#define MICROPY_HW_UART1_RX     (pin_A10)

// USART2: Connected to port for Bluetooth module.
#define MICROPY_HW_UART2_TX     (pin_D5)
#define MICROPY_HW_UART2_RX     (pin_D6)

// USART3: Connected to CP2102 serving USB2.
//         Flow control signals not connected.
#define MICROPY_HW_UART3_TX     (pin_C10)
#define MICROPY_HW_UART3_RX     (pin_C11)

// UART4: Conflicts with QEI.

// UART5: Available on U11.
#define MICROPY_HW_UART5_TX     (pin_C12)
#define MICROPY_HW_UART5_RX     (pin_D2)

// USART6: Conflicts with motor drive.

// REPL mapping
// USB1's CP2102 has modem control signals wired to RST & BOOT0, use USB2's
// CP2102 instead.
//
// Since the UART signals do not go off-board, the baud rate can potentially
// be set higher.
#define MICROPY_HW_UART_REPL PYB_UART_3
#define MICROPY_HW_UART_REPL_BAUD 115200

// I2Cs
// I2C1: Connected to ICM20948.
#define MICROPY_HW_I2C1_SCL     (pin_B8)
#define MICROPY_HW_I2C1_SDA     (pin_B9)
// I2C2: Conflicts with motor drive.
// I2C3: Conflicts with motor drive.

// SPIs
// SPI1: Conflicts with motor drive and QEI.
// SPI2: MISO not exposed.
// SPI3: Conflicts with QEI.

// CAN buses
// CAN1: Connected to VP230.
#define MICROPY_HW_CAN1_TX      (pin_D1)
#define MICROPY_HW_CAN1_RX      (pin_D0)
// CAN2: Conflicts with PS2 controller input.

// USRSW (SW1 on schematic) is pulled high.
// Pressing the button makes the input go low.
#define MICROPY_HW_USRSW_PIN        (pin_D8)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// LEDs
// LED1 (LED3 on schematic) - MCU sinks current.
#define MICROPY_HW_LED1         (pin_E10)
#define MICROPY_HW_LED_ON(pin)  (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin) (mp_hal_pin_high(pin))

// USB config
#define MICROPY_HW_USB_FS (MICROPY_HW_ENABLE_USB)
// #define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_A9)
// #define MICROPY_HW_USB_OTG_ID_PIN      (pin_A10)

#define MICROPY_HW_BOARD_NAME       "CLICKER2"
#define MICROPY_HW_MCU_NAME         "STM32F407"


#define MICROPY_HW_FLASH_FS_LABEL	"CLICKER2"


#define MICROPY_HW_FLASH_EXTRA	(1)	// use the extra 64K sector


#define MICROPY_BOARD_EARLY_INIT    CLICKER2_board_early_init
void CLICKER2_board_early_init(void);


#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (1)


// HSE is 25MHz
#define MICROPY_HW_CLK_PLLM 		(25)
#define MICROPY_HW_CLK_PLLN 		(336)
#define MICROPY_HW_CLK_PLLP 		(RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ 		(7)

// RTC has external 32kHz crystal
#define MICROPY_HW_RTC_USE_LSE      (1)

// USB config
#define MICROPY_HW_USB_FS           (1)
#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_A9)


// T2-PE0 - USRSW is pulled high. Pressing the button makes the input go low.
#define MICROPY_HW_USRSW_PIN        (pin_E0)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// T3-PA10 - Second Switch
#define MICROPY_HW_USRSW2_PIN        (pin_A10)


// LEDs
#define MICROPY_HW_LED1             (pin_E12) // LED 1 (red)
#define MICROPY_HW_LED2             (pin_E15) // LED 2

#define MICROPY_HW_LED_OTYPE        (GPIO_MODE_OUTPUT_PP)
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))


// REPL on second port
//#define MICROPY_HW_UART_REPL 		(1)				
//#define MICROPY_HW_UART_REPL_BAUD	(115200)


//#define MICROPY_HW_UART1_TX     (pin_B6)
//#define MICROPY_HW_UART1_RX     (pin_B7)

// on MB1 connector
#define MICROPY_HW_UART2_TX     (pin_D5)
#define MICROPY_HW_UART2_RX     (pin_D6)
// on HDR2 connector
//#define MICROPY_HW_UART2_RTS    (pin_A1)
//#define MICROPY_HW_UART2_CTS    (pin_A0)

// on MB2 connector
#define MICROPY_HW_UART3_TX     (pin_D8)
#define MICROPY_HW_UART3_RX     (pin_D9)
// on MB2 and HDR1
//#define MICROPY_HW_UART3_RTS    (pin_B14)
//#define MICROPY_HW_UART3_CTS    (pin_B13)

// on HDR2 connector
#define MICROPY_HW_UART4_TX     (pin_A0)
#define MICROPY_HW_UART4_RX     (pin_A1)

// CAN busses have some conflicting pins

// I2C busses
//#define MICROPY_HW_I2C1_SCL (pin_B6)	// on HDR2
//#define MICROPY_HW_I2C1_SDA (pin_B7)	// on HDR1
#define MICROPY_HW_I2C2_SCL (pin_B10)	// on MB2
#define MICROPY_HW_I2C2_SDA (pin_B11)
#define MICROPY_HW_I2C3_SCL (pin_A8)	// on MB1
#define MICROPY_HW_I2C3_SDA (pin_C9)

// SPI busses
#define MICROPY_HW_SPI2_SCK  (pin_B13)	// on MB 2
#define MICROPY_HW_SPI2_MISO (pin_B14)
#define MICROPY_HW_SPI2_MOSI (pin_B15)
#define MICROPY_HW_SPI2_NSS  (pin_E11)

#define MICROPY_HW_SPI3_SCK  (pin_C10)	// on MB 1
#define MICROPY_HW_SPI3_MISO (pin_C11)
#define MICROPY_HW_SPI3_MOSI (pin_C12)
#define MICROPY_HW_SPI3_NSS  (pin_E8)

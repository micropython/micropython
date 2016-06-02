#include STM32_HAL_H

#define MICROPY_HW_BOARD_NAME       "L475_EMFBADGE"
#define MICROPY_HW_MCU_NAME         "STM32L475"
#define MICROPY_PY_SYS_PLATFORM     "emfbadge"

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_HAS_SDCARD       (0)
#define MICROPY_HW_HAS_LCD          (0)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_HAS_UGFX         (1)
#define MICROPY_HW_HAS_CC3100       (1)

#define MICROPY_BOARD_EARLY_INIT    STM32L475_EMFBADGE_board_early_init
void STM32L475_EMFBADGE_board_early_init(void);


//cc3100 stuff
// defined as the const pin_objs
#define MICROPY_HW_CC3100_HIB       pin_C6
#define MICROPY_HW_CC3100_IRQ       pin_E4
#define MICROPY_HW_CC3100_CS        pin_A4
#define MICROPY_HW_CC3100_SPI       SPIHandle1


// uGFX PinDefs
#define UGFX_DRIVER_SPI 1
#define UGFX_DRIVER_PARALLEL 2
#define MICROPY_HW_UGFX_INTERFACE UGFX_DRIVER_PARALLEL

//used for selecting SPI/parallel mode
#define MICROPY_HW_UGFX_PORT_MODE GPIOE
#define MICROPY_HW_UGFX_PIN_MODE GPIO_PIN_6
#define MICROPY_HW_UGFX_PORT_BL GPIOE
#define MICROPY_HW_UGFX_PIN_BL GPIO_PIN_1
#define MICROPY_HW_UGFX_PORT_RST GPIOD
#define MICROPY_HW_UGFX_PIN_RST GPIO_PIN_13

#if MICROPY_HW_UGFX_INTERFACE == UGFX_DRIVER_SPI

	#define MICROPY_HW_UGFX_PORT_CS GPIOD
	#define MICROPY_HW_UGFX_PIN_CS GPIO_PIN_7
	#define MICROPY_HW_UGFX_PORT_A0 GPIOD
	#define MICROPY_HW_UGFX_PIN_A0 GPIO_PIN_5	
	
	#define MICROPY_HW_UGFX_SPI SPIHandle3
	
	#define MICROPY_HW_UGFX_SET_MODE GPIO_set_pin(MICROPY_HW_UGFX_PORT_MODE, MICROPY_HW_UGFX_PIN_MODE)

#elif MICROPY_HW_UGFX_INTERFACE == UGFX_DRIVER_PARALLEL

	#define FMC_BASE_ADDR (0x60000000)
	#define FMC_ADDR_PIN (23)
	
	#define MICROPY_HW_UGFX_PORT_A0 GPIOE
	#define MICROPY_HW_UGFX_PIN_A0 GPIO_PIN_2

	#define MICROPY_HW_UGFX_SET_MODE GPIO_clear_pin(MICROPY_HW_UGFX_PORT_MODE, MICROPY_HW_UGFX_PIN_MODE)

#else
	#error "Select SPI or PARALLEL for UGFX driver type"
#endif

// LSE is 32kHz, MSI is 4MHz, CPU freq set to 80MHz
#define MICROPY_HW_CLK_PLLM (1)
#define MICROPY_HW_CLK_PLLN (40)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV7)
#define MICROPY_HW_CLK_PLLR (RCC_PLLR_DIV2)
#define MICROPY_HW_CLK_PLLQ (RCC_PLLQ_DIV2)

#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_4

// UART config
#define MICROPY_HW_UART3_PORT (GPIOC)
#define MICROPY_HW_UART3_PINS (GPIO_PIN_4 | GPIO_PIN_5)

// CC3100 UART
//#define MICROPY_HW_UART1_PORT (GPIOB)
//#define MICROPY_HW_UART1_PINS (GPIO_PIN_6 | GPIO_PIN_7)

/*
// UART 2 connects to the external UART headers
#define MICROPY_HW_UART_REPL        PYB_UART_3
#define MICROPY_HW_UART_REPL_BAUD   115200
*/

// I2C busses
#define MICROPY_HW_I2C1_SCL (pin_B8)    // edge connector
#define MICROPY_HW_I2C1_SDA (pin_B9)    // 
#define MICROPY_HW_I2C3_SCL (pin_C0)    // imu
#define MICROPY_HW_I2C3_SDA (pin_C1)    // 

#define MICROPY_HW_I2C_BAUDRATE_TIMING  {{100000, 0x90112626}}
#define MICROPY_HW_I2C_BAUDRATE_DEFAULT 100000
#define MICROPY_HW_I2C_BAUDRATE_MAX     100000

// SPI busses
#define MICROPY_HW_SPI1_NAME "X"       //CC3100
#define MICROPY_HW_SPI1_NSS  (pin_A4)
#define MICROPY_HW_SPI1_SCK  (pin_E13)
#define MICROPY_HW_SPI1_MISO (pin_A6)
#define MICROPY_HW_SPI1_MOSI (pin_A7)
#define MICROPY_HW_SPI2_NAME "Y"       //edge connector
#define MICROPY_HW_SPI2_NSS  (pin_B12)
#define MICROPY_HW_SPI2_SCK  (pin_B10)
#define MICROPY_HW_SPI2_MISO (pin_B14)
#define MICROPY_HW_SPI2_MOSI (pin_B15)
#define MICROPY_HW_SPI3_NAME "Z"       //screen
#define MICROPY_HW_SPI3_NSS  (pin_A15) //not used for SPI
#define MICROPY_HW_SPI3_SCK  (pin_B3) 
#define MICROPY_HW_SPI3_MISO (pin_B4) 
#define MICROPY_HW_SPI3_MOSI (pin_B5) 

// USRSW is pulled high. Pressing the button makes the input go low.
#define MICROPY_HW_USRSW_PIN        (pin_D3)
#define MICROPY_HW_USRSW_PULL       (GPIO_PULLUP)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)



// LEDs (x3)

#define MICROPY_HW_LED1             (pin_E15) //LED A
#define MICROPY_HW_LED2             (pin_E12) //LED B
#define MICROPY_HW_LED3             (pin_C7) //torch
#define MICROPY_HW_LED_OTYPE        (GPIO_MODE_OUTPUT_PP)
#define MICROPY_HW_LED_ON(pin)      (GPIO_set_pin(pin->gpio,pin->pin_mask))
#define MICROPY_HW_LED_OFF(pin)     (GPIO_clear_pin(pin->gpio,pin->pin_mask))


// SD card detect switch
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_A8)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)

// USB config
#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_A9)
#define MICROPY_HW_USB_OTG_ID_PIN      (pin_A10)
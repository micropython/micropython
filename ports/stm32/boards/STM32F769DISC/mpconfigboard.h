// This board is confirmed to operate using stlink and openocd.
// REPL is on UART(1) and is available through the stlink USB-UART device.
// To use openocd run "OPENOCD_CONFIG=boards/openocd_stm32f7.cfg" in
// the make command.
#define MICROPY_HW_BOARD_NAME       "F769DISC"
#define MICROPY_HW_MCU_NAME         "STM32F769"

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_HAS_SDCARD       (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_USB       (1)

// HSE is 25MHz
// VCOClock = HSE * PLLN / PLLM = 25 MHz * 432 / 25 = 432 MHz
// SYSCLK = VCOClock / PLLP = 432 MHz / 2 = 216 MHz
// USB/SDMMC/RNG Clock = VCOClock / PLLQ = 432 MHz / 9 = 48 MHz
#define MICROPY_HW_CLK_PLLM (25)
#define MICROPY_HW_CLK_PLLN (432)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (9)

#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_7 // 210-216 MHz needs 7 wait states

// UART config
#define MICROPY_HW_UART1_TX         (pin_A9)
#define MICROPY_HW_UART1_RX         (pin_A10)
#define MICROPY_HW_UART5_TX         (pin_C12)
#define MICROPY_HW_UART5_RX         (pin_D2)
#define MICROPY_HW_UART_REPL        PYB_UART_1
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C busses
#define MICROPY_HW_I2C1_SCL         (pin_B8)
#define MICROPY_HW_I2C1_SDA         (pin_B9)
#define MICROPY_HW_I2C3_SCL         (pin_H7)
#define MICROPY_HW_I2C3_SDA         (pin_H8)

// SPI
#define MICROPY_HW_SPI2_NSS         (pin_A11)
#define MICROPY_HW_SPI2_SCK         (pin_A12)
#define MICROPY_HW_SPI2_MISO        (pin_B14)
#define MICROPY_HW_SPI2_MOSI        (pin_B15)

// CAN busses
#define MICROPY_HW_CAN1_TX          (pin_B9)
#define MICROPY_HW_CAN1_RX          (pin_B8)
#define MICROPY_HW_CAN2_TX          (pin_B13)
#define MICROPY_HW_CAN2_RX          (pin_B12)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_A0)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_J13) // red
#define MICROPY_HW_LED2             (pin_J5) // green 
#define MICROPY_HW_LED3             (pin_A12) // green
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// SD card detect switch
#define MICROPY_HW_SDMMC2_CK                (pin_D6)
#define MICROPY_HW_SDMMC2_CMD               (pin_D7)
#define MICROPY_HW_SDMMC2_D0                (pin_G9)
#define MICROPY_HW_SDMMC2_D1                (pin_G10)
#define MICROPY_HW_SDMMC2_D2                (pin_B3)
#define MICROPY_HW_SDMMC2_D3                (pin_B4)
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_I15)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)

// USB config (CN13 - USB OTG FS)
#define MICROPY_HW_USB_HS              (1)
#define MICROPY_HW_USB_HS_IN_FS        (1)
/*#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_J12)*/
#define MICROPY_HW_USB_OTG_ID_PIN      (pin_J12)

#if 0
// Optional SDRAM configuration; requires SYSCLK <= 200MHz
#define MICROPY_HW_SDRAM_SIZE (128 * 1024 * 1024 / 8) // 128 Mbit
#define MICROPY_HW_SDRAM_STARTUP_TEST (0)

// Timing configuration for 90 Mhz (11.90ns) of SD clock frequency (180Mhz/2)
#define MICROPY_HW_SDRAM_TIMING_TMRD        (2)
#define MICROPY_HW_SDRAM_TIMING_TXSR        (7)
#define MICROPY_HW_SDRAM_TIMING_TRAS        (4)
#define MICROPY_HW_SDRAM_TIMING_TRC         (7)
#define MICROPY_HW_SDRAM_TIMING_TWR         (2)
#define MICROPY_HW_SDRAM_TIMING_TRP         (2)
#define MICROPY_HW_SDRAM_TIMING_TRCD        (2)
#define MICROPY_HW_SDRAM_REFRESH_RATE       (64) // ms

#define MICROPY_HW_SDRAM_BURST_LENGTH       1
#define MICROPY_HW_SDRAM_CAS_LATENCY        2
#define MICROPY_HW_SDRAM_COLUMN_BITS_NUM    8
#define MICROPY_HW_SDRAM_ROW_BITS_NUM       13
#define MICROPY_HW_SDRAM_MEM_BUS_WIDTH      32
#define MICROPY_HW_SDRAM_INTERN_BANKS_NUM   4
#define MICROPY_HW_SDRAM_CLOCK_PERIOD       2
#define MICROPY_HW_SDRAM_RPIPE_DELAY        0
#define MICROPY_HW_SDRAM_RBURST             (1)
#define MICROPY_HW_SDRAM_WRITE_PROTECTION   (0)
#define MICROPY_HW_SDRAM_AUTOREFRESH_NUM    (8)

// See pins.csv for CPU pin mapping
#define MICROPY_HW_FMC_SDCKE0  (pyb_pin_FMC_SDCKE0)
#define MICROPY_HW_FMC_SDNE0   (pyb_pin_FMC_SDNE0)
#define MICROPY_HW_FMC_SDCLK   (pyb_pin_FMC_SDCLK)
#define MICROPY_HW_FMC_SDNCAS  (pyb_pin_FMC_SDNCAS)
#define MICROPY_HW_FMC_SDNRAS  (pyb_pin_FMC_SDNRAS)
#define MICROPY_HW_FMC_SDNWE   (pyb_pin_FMC_SDNWE)
#define MICROPY_HW_FMC_BA0     (pyb_pin_FMC_BA0)
#define MICROPY_HW_FMC_BA1     (pyb_pin_FMC_BA1)
#define MICROPY_HW_FMC_NBL0    (pyb_pin_FMC_NBL0)
#define MICROPY_HW_FMC_NBL1    (pyb_pin_FMC_NBL1)
#define MICROPY_HW_FMC_NBL2    (pyb_pin_FMC_NBL2)
#define MICROPY_HW_FMC_NBL3    (pyb_pin_FMC_NBL3)
#define MICROPY_HW_FMC_A0      (pyb_pin_FMC_A0)
#define MICROPY_HW_FMC_A1      (pyb_pin_FMC_A1)
#define MICROPY_HW_FMC_A2      (pyb_pin_FMC_A2)
#define MICROPY_HW_FMC_A3      (pyb_pin_FMC_A3)
#define MICROPY_HW_FMC_A4      (pyb_pin_FMC_A4)
#define MICROPY_HW_FMC_A5      (pyb_pin_FMC_A5)
#define MICROPY_HW_FMC_A6      (pyb_pin_FMC_A6)
#define MICROPY_HW_FMC_A7      (pyb_pin_FMC_A7)
#define MICROPY_HW_FMC_A8      (pyb_pin_FMC_A8)
#define MICROPY_HW_FMC_A9      (pyb_pin_FMC_A9)
#define MICROPY_HW_FMC_A10     (pyb_pin_FMC_A10)
#define MICROPY_HW_FMC_A11     (pyb_pin_FMC_A11)
#define MICROPY_HW_FMC_A12     (pyb_pin_FMC_A12)
#define MICROPY_HW_FMC_D0      (pyb_pin_FMC_D0)
#define MICROPY_HW_FMC_D1      (pyb_pin_FMC_D1)
#define MICROPY_HW_FMC_D2      (pyb_pin_FMC_D2)
#define MICROPY_HW_FMC_D3      (pyb_pin_FMC_D3)
#define MICROPY_HW_FMC_D4      (pyb_pin_FMC_D4)
#define MICROPY_HW_FMC_D5      (pyb_pin_FMC_D5)
#define MICROPY_HW_FMC_D6      (pyb_pin_FMC_D6)
#define MICROPY_HW_FMC_D7      (pyb_pin_FMC_D7)
#define MICROPY_HW_FMC_D8      (pyb_pin_FMC_D8)
#define MICROPY_HW_FMC_D9      (pyb_pin_FMC_D9)
#define MICROPY_HW_FMC_D10     (pyb_pin_FMC_D10)
#define MICROPY_HW_FMC_D11     (pyb_pin_FMC_D11)
#define MICROPY_HW_FMC_D12     (pyb_pin_FMC_D12)
#define MICROPY_HW_FMC_D13     (pyb_pin_FMC_D13)
#define MICROPY_HW_FMC_D14     (pyb_pin_FMC_D14)
#define MICROPY_HW_FMC_D15     (pyb_pin_FMC_D15)
#define MICROPY_HW_FMC_D16     (pyb_pin_FMC_D16)
#define MICROPY_HW_FMC_D17     (pyb_pin_FMC_D17)
#define MICROPY_HW_FMC_D18     (pyb_pin_FMC_D18)
#define MICROPY_HW_FMC_D19     (pyb_pin_FMC_D19)
#define MICROPY_HW_FMC_D20     (pyb_pin_FMC_D20)
#define MICROPY_HW_FMC_D21     (pyb_pin_FMC_D21)
#define MICROPY_HW_FMC_D22     (pyb_pin_FMC_D22)
#define MICROPY_HW_FMC_D23     (pyb_pin_FMC_D23)
#define MICROPY_HW_FMC_D24     (pyb_pin_FMC_D24)
#define MICROPY_HW_FMC_D25     (pyb_pin_FMC_D25)
#define MICROPY_HW_FMC_D26     (pyb_pin_FMC_D26)
#define MICROPY_HW_FMC_D27     (pyb_pin_FMC_D27)
#define MICROPY_HW_FMC_D28     (pyb_pin_FMC_D28)
#define MICROPY_HW_FMC_D29     (pyb_pin_FMC_D29)
#define MICROPY_HW_FMC_D30     (pyb_pin_FMC_D30)
#define MICROPY_HW_FMC_D31     (pyb_pin_FMC_D31)
#endif

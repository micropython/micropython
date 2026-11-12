#define MICROPY_HW_BOARD_NAME                   "STM32N6570-DK"
#define MICROPY_HW_MCU_NAME                     "STM32N657X0"

#define MICROPY_GC_STACK_ENTRY_TYPE             uint32_t
#define MICROPY_ALLOC_GC_STACK_SIZE             (128)
#define MICROPY_GC_SPLIT_HEAP                   (1)
#define MICROPY_FATFS_EXFAT                     (1)

#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)
#define MICROPY_HW_HAS_SWITCH                   (1)
#define MICROPY_HW_HAS_FLASH                    (1)
#define MICROPY_HW_ENABLE_RNG                   (1)
#define MICROPY_HW_ENABLE_RTC                   (1)
#define MICROPY_HW_ENABLE_DAC                   (0)
#define MICROPY_HW_ENABLE_USB                   (1)
#define MICROPY_PY_PYB_LEGACY                   (0)

#define MICROPY_BOARD_EARLY_INIT                board_early_init
#define MICROPY_BOARD_LEAVE_STANDBY             board_leave_standby()

// HSE is 48MHz, this gives a CPU frequency of 800MHz.
#define MICROPY_HW_CLK_PLLM                     (6)
#define MICROPY_HW_CLK_PLLN                     (100)
#define MICROPY_HW_CLK_PLLP1                    (1)
#define MICROPY_HW_CLK_PLLP2                    (1)
#define MICROPY_HW_CLK_PLLFRAC                  (0)

// The LSE is a 32kHz crystal.
#define MICROPY_HW_RTC_USE_LSE                  (1)
#define MICROPY_HW_RTC_USE_US                   (1)

// External SPI flash, MX66UW1G45GXDI00.
#define MICROPY_HW_XSPIFLASH_SIZE_BITS_LOG2     (30)

// SPI flash, block device config.
#define MICROPY_HW_BDEV_SPIFLASH                (&spi_bdev)
#define MICROPY_HW_BDEV_SPIFLASH_EXTENDED       (&spi_bdev)
#define MICROPY_HW_BDEV_SPIFLASH_CONFIG         (&spiflash_config)
#define MICROPY_HW_BDEV_SPIFLASH_OFFSET_BYTES   (4 * 1024 * 1024)
#define MICROPY_HW_BDEV_SPIFLASH_SIZE_BYTES     (124 * 1024 * 1024)

// UART buses
#define MICROPY_HW_UART1_TX                     (pyb_pin_UART1_TX)
#define MICROPY_HW_UART1_RX                     (pyb_pin_UART1_RX)
#define MICROPY_HW_UART_REPL                    (PYB_UART_1)
#define MICROPY_HW_UART_REPL_BAUD               (115200)

// I2C buses
#define MICROPY_HW_I2C1_SCL                     (pyb_pin_I2C1_SCL)
#define MICROPY_HW_I2C1_SDA                     (pyb_pin_I2C1_SDA)
#define MICROPY_HW_I2C2_SCL                     (pyb_pin_I2C2_SCL)
#define MICROPY_HW_I2C2_SDA                     (pyb_pin_I2C2_SDA)

// Pressing the USER1 button makes the input go high.
#define MICROPY_HW_USRSW_PIN                    (pyb_pin_BUTTON)
#define MICROPY_HW_USRSW_PULL                   (GPIO_PULLDOWN)
#define MICROPY_HW_USRSW_EXTI_MODE              (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED                (1)

// LEDs: the red LED2 is active low and the green LED1 is active high.
#define MICROPY_HW_LED1                         (pyb_pin_LED_RED)
#define MICROPY_HW_LED2                         (pyb_pin_LED_GREEN)
#define MICROPY_HW_LED_ON(pin)                  ((pin) == pyb_pin_LED_GREEN ? mp_hal_pin_high(pin) : mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)                 ((pin) == pyb_pin_LED_GREEN ? mp_hal_pin_low(pin) : mp_hal_pin_high(pin))

// USB config
#define MICROPY_HW_USB_HS                       (1)
#define MICROPY_HW_USB_HS_IN_FS                 (1)
#define MICROPY_HW_USB_MAIN_DEV                 (USB_PHY_HS_ID)

// LCD: 800x480 RK050HR18 panel on the MB1860 board, with RGB888 to the LTDC.
#define MICROPY_HW_ENABLE_LTDC                  (1)
#define MICROPY_HW_LTDC_WIDTH                   (800)
#define MICROPY_HW_LTDC_HEIGHT                  (480)
#define MICROPY_HW_LTDC_HSW                     (4)
#define MICROPY_HW_LTDC_HBP                     (4)
#define MICROPY_HW_LTDC_HFP                     (4)
#define MICROPY_HW_LTDC_VSW                     (4)
#define MICROPY_HW_LTDC_VBP                     (4)
#define MICROPY_HW_LTDC_VFP                     (4)
#define MICROPY_HW_LTDC_PCLK_HZ                 (25000000)
#define MICROPY_HW_LTDC_CLK                     (pyb_pin_LCD_CLK)
#define MICROPY_HW_LTDC_HSYNC                   (pyb_pin_LCD_HSYNC)
#define MICROPY_HW_LTDC_VSYNC                   (pyb_pin_LCD_VSYNC)
#define MICROPY_HW_LTDC_R0                      (pyb_pin_LCD_R0)
#define MICROPY_HW_LTDC_R1                      (pyb_pin_LCD_R1)
#define MICROPY_HW_LTDC_R2                      (pyb_pin_LCD_R2)
#define MICROPY_HW_LTDC_R3                      (pyb_pin_LCD_R3)
#define MICROPY_HW_LTDC_R4                      (pyb_pin_LCD_R4)
#define MICROPY_HW_LTDC_R5                      (pyb_pin_LCD_R5)
#define MICROPY_HW_LTDC_R6                      (pyb_pin_LCD_R6)
#define MICROPY_HW_LTDC_R7                      (pyb_pin_LCD_R7)
#define MICROPY_HW_LTDC_G0                      (pyb_pin_LCD_G0)
#define MICROPY_HW_LTDC_G1                      (pyb_pin_LCD_G1)
#define MICROPY_HW_LTDC_G2                      (pyb_pin_LCD_G2)
#define MICROPY_HW_LTDC_G3                      (pyb_pin_LCD_G3)
#define MICROPY_HW_LTDC_G4                      (pyb_pin_LCD_G4)
#define MICROPY_HW_LTDC_G5                      (pyb_pin_LCD_G5)
#define MICROPY_HW_LTDC_G6                      (pyb_pin_LCD_G6)
#define MICROPY_HW_LTDC_G7                      (pyb_pin_LCD_G7)
#define MICROPY_HW_LTDC_B0                      (pyb_pin_LCD_B0)
#define MICROPY_HW_LTDC_B1                      (pyb_pin_LCD_B1)
#define MICROPY_HW_LTDC_B2                      (pyb_pin_LCD_B2)
#define MICROPY_HW_LTDC_B3                      (pyb_pin_LCD_B3)
#define MICROPY_HW_LTDC_B4                      (pyb_pin_LCD_B4)
#define MICROPY_HW_LTDC_B5                      (pyb_pin_LCD_B5)
#define MICROPY_HW_LTDC_B6                      (pyb_pin_LCD_B6)
#define MICROPY_HW_LTDC_B7                      (pyb_pin_LCD_B7)
#define MICROPY_HW_LTDC_PWR_PIN                 (pyb_pin_LCD_ON)
#define MICROPY_HW_LTDC_DISP_PIN                (pyb_pin_LCD_DISP)
#define MICROPY_HW_LTDC_BL_PIN                  (pyb_pin_LCD_BL)

/******************************************************************************/
// Bootloader configuration

#define MBOOT_BOARD_EARLY_INIT(initial_r0)      mboot_board_early_init()

#define MBOOT_SPIFLASH_CS                       (pyb_pin_XSPIM_P2_CS)
#define MBOOT_SPIFLASH_SCK                      (pyb_pin_XSPIM_P2_SCK)
#define MBOOT_SPIFLASH_MOSI                     (pyb_pin_XSPIM_P2_IO0)
#define MBOOT_SPIFLASH_MISO                     (pyb_pin_XSPIM_P2_IO1)
#define MBOOT_SPIFLASH_ADDR                     (0x70000000)
#define MBOOT_SPIFLASH_BYTE_SIZE                (128 * 1024 * 1024)
#define MBOOT_SPIFLASH_LAYOUT                   "/0x70000000/32768*4Kg"
#define MBOOT_SPIFLASH_ERASE_BLOCKS_PER_PAGE    (1)
#define MBOOT_SPIFLASH_SPIFLASH                 (&spi_bdev.spiflash)
#define MBOOT_SPIFLASH_CONFIG                   (&spiflash_config)

/******************************************************************************/
// Function and variable declarations

extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;

void mboot_board_early_init(void);
void mboot_board_entry_init(void);

void board_early_init(void);
void board_leave_standby(void);

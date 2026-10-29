#define MICROPY_HW_BOARD_NAME               "Seeed Studio XIAO STM32C5"
#define MICROPY_HW_MCU_NAME                 "STM32C5A3CG"

#define MICROPY_PY_PYB_LEGACY               (0)
#define MICROPY_PY_MACHINE_MEM_BACKUP       (0)
#define MICROPY_HW_HAS_FLASH                (0)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)
#define MICROPY_HW_HAS_SWITCH               (0)
#define MICROPY_HW_ENABLE_RTC               (0)
#define MICROPY_HW_ENABLE_RNG               (0)
#define MICROPY_HW_ENABLE_ADC               (0)
#define MICROPY_HW_ENABLE_DAC               (0)
// 48 MHz HSE crystal, with PSI generating a 144 MHz system clock.
#define MICROPY_HW_CLK_USE_HSE              (1)
#define MICROPY_HW_CLK_PSI_REF              (HAL_RCC_PSI_REF_48MHZ)
#define MICROPY_HW_CLK_PSI_OUT              (HAL_RCC_PSI_OUT_144MHZ)
#define MICROPY_HW_FLASH_LATENCY            FLASH_LATENCY_4

// Native USB CDC REPL without USB MSC.
#define MICROPY_HW_ENABLE_USB               (1)
#define MICROPY_HW_TINYUSB_STACK            (1)
#define MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE (0)
#define MICROPY_HW_USB_FS                   (1)
#define MICROPY_HW_USB_MSC                  (0)
#define MICROPY_HW_USB_PRODUCT_FS_STRING    "XIAO STM32C5"

// Keep D6/D7 UART REPL available alongside USB for diagnostics.
#define MICROPY_HW_UART1_TX                 (pin_A9)
#define MICROPY_HW_UART1_RX                 (pin_A10)
#define MICROPY_HW_UART_REPL                PYB_UART_1
#define MICROPY_HW_UART_REPL_BAUD           115200


// Green user LED, active low.
#define MICROPY_HW_LED1                     (pin_B12)
#define MICROPY_HW_LED_ON(pin)              (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)             (mp_hal_pin_high(pin))

#define MICROPY_BOARD_EARLY_INIT            board_early_init
void board_early_init(void);

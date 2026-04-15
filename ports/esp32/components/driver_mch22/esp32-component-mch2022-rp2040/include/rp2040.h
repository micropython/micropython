#pragma once

#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <stdint.h>

enum {
    RP2040_REG_FW_VER = 0,
    RP2040_REG_GPIO_DIR,
    RP2040_REG_GPIO_IN,
    RP2040_REG_GPIO_OUT,
    RP2040_REG_LCD_BACKLIGHT,
    RP2040_REG_FPGA,
    RP2040_REG_INPUT1,
    RP2040_REG_INPUT2,
    RP2040_REG_INTERRUPT1,
    RP2040_REG_INTERRUPT2,
    RP2040_REG_ADC_TRIGGER,
    RP2040_REG_ADC_VALUE_VUSB_LO,
    RP2040_REG_ADC_VALUE_VUSB_HI,
    RP2040_REG_ADC_VALUE_VBAT_LO,
    RP2040_REG_ADC_VALUE_VBAT_HI,
    RP2040_REG_USB,
    RP2040_REG_BL_TRIGGER,
    RP2040_REG_WEBUSB_MODE,
    RP2040_REG_CRASH_DEBUG,
    RP2040_REG_RESET_LOCK,
    RP2040_REG_RESET_ATTEMPTED,
    RP2040_REG_CHARGING_STATE,
    RP2040_REG_ADC_VALUE_TEMP_LO,
    RP2040_REG_ADC_VALUE_TEMP_HI,
    RP2040_REG_UID0,  // Unique board identifier of the RP2040
    RP2040_REG_UID1,
    RP2040_REG_UID2,
    RP2040_REG_UID3,
    RP2040_REG_UID4,
    RP2040_REG_UID5,
    RP2040_REG_UID6,
    RP2040_REG_UID7,
    RP2040_REG_SCRATCH0,  // Used by the ESP32 to store boot parameters, can also be read and written to from WebUSB
    RP2040_REG_SCRATCH1,
    RP2040_REG_SCRATCH2,
    RP2040_REG_SCRATCH3,
    RP2040_REG_SCRATCH4,
    RP2040_REG_SCRATCH5,
    RP2040_REG_SCRATCH6,
    RP2040_REG_SCRATCH7,
    RP2040_REG_SCRATCH8,
    RP2040_REG_SCRATCH9,
    RP2040_REG_SCRATCH10,
    RP2040_REG_SCRATCH11,
    RP2040_REG_SCRATCH12,
    RP2040_REG_SCRATCH13,
    RP2040_REG_SCRATCH14,
    RP2040_REG_SCRATCH15,
    RP2040_REG_SCRATCH16,
    RP2040_REG_SCRATCH17,
    RP2040_REG_SCRATCH18,
    RP2040_REG_SCRATCH19,
    RP2040_REG_SCRATCH20,
    RP2040_REG_SCRATCH21,
    RP2040_REG_SCRATCH22,
    RP2040_REG_SCRATCH23,
    RP2040_REG_SCRATCH24,
    RP2040_REG_SCRATCH25,
    RP2040_REG_SCRATCH26,
    RP2040_REG_SCRATCH27,
    RP2040_REG_SCRATCH28,
    RP2040_REG_SCRATCH29,
    RP2040_REG_SCRATCH30,
    RP2040_REG_SCRATCH31,
    RP2040_REG_SCRATCH32,
    RP2040_REG_SCRATCH33,
    RP2040_REG_SCRATCH34,
    RP2040_REG_SCRATCH35,
    RP2040_REG_SCRATCH36,
    RP2040_REG_SCRATCH37,
    RP2040_REG_SCRATCH38,
    RP2040_REG_SCRATCH39,
    RP2040_REG_SCRATCH40,
    RP2040_REG_SCRATCH41,
    RP2040_REG_SCRATCH42,
    RP2040_REG_SCRATCH43,
    RP2040_REG_SCRATCH44,
    RP2040_REG_SCRATCH45,
    RP2040_REG_SCRATCH46,
    RP2040_REG_SCRATCH47,
    RP2040_REG_SCRATCH48,
    RP2040_REG_SCRATCH49,
    RP2040_REG_SCRATCH50,
    RP2040_REG_SCRATCH51,
    RP2040_REG_SCRATCH52,
    RP2040_REG_SCRATCH53,
    RP2040_REG_SCRATCH54,
    RP2040_REG_SCRATCH55,
    RP2040_REG_SCRATCH56,
    RP2040_REG_SCRATCH57,
    RP2040_REG_SCRATCH58,
    RP2040_REG_SCRATCH59,
    RP2040_REG_SCRATCH60,
    RP2040_REG_SCRATCH61,
    RP2040_REG_SCRATCH62,
    RP2040_REG_SCRATCH63,
    RP2040_REG_IR_ADDRESS_LO,
    RP2040_REG_IR_ADDRESS_HI,
    RP2040_REG_IR_COMMAND,
    RP2040_REG_IR_TRIGGER,
    RP2040_REG_RESERVED11,
    RP2040_REG_RESERVED12,
    RP2040_REG_RESERVED13,
    RP2040_REG_RESERVED14,
    RP2040_REG_WS2812_MODE,
    RP2040_REG_WS2812_TRIGGER,
    RP2040_REG_WS2812_LENGTH,
    RP2040_REG_WS2812_SPEED,
    RP2040_REG_WS2812_LED0_DATA0,
    RP2040_REG_WS2812_LED0_DATA1,
    RP2040_REG_WS2812_LED0_DATA2,
    RP2040_REG_WS2812_LED0_DATA3,
    RP2040_REG_WS2812_LED1_DATA0,
    RP2040_REG_WS2812_LED1_DATA1,
    RP2040_REG_WS2812_LED1_DATA2,
    RP2040_REG_WS2812_LED1_DATA3,
    RP2040_REG_WS2812_LED2_DATA0,
    RP2040_REG_WS2812_LED2_DATA1,
    RP2040_REG_WS2812_LED2_DATA2,
    RP2040_REG_WS2812_LED2_DATA3,
    RP2040_REG_WS2812_LED3_DATA0,
    RP2040_REG_WS2812_LED3_DATA1,
    RP2040_REG_WS2812_LED3_DATA2,
    RP2040_REG_WS2812_LED3_DATA3,
    RP2040_REG_WS2812_LED4_DATA0,
    RP2040_REG_WS2812_LED4_DATA1,
    RP2040_REG_WS2812_LED4_DATA2,
    RP2040_REG_WS2812_LED4_DATA3,
    RP2040_REG_WS2812_LED5_DATA0,
    RP2040_REG_WS2812_LED5_DATA1,
    RP2040_REG_WS2812_LED5_DATA2,
    RP2040_REG_WS2812_LED5_DATA3,
    RP2040_REG_WS2812_LED6_DATA0,
    RP2040_REG_WS2812_LED6_DATA1,
    RP2040_REG_WS2812_LED6_DATA2,
    RP2040_REG_WS2812_LED6_DATA3,
    RP2040_REG_WS2812_LED7_DATA0,
    RP2040_REG_WS2812_LED7_DATA1,
    RP2040_REG_WS2812_LED7_DATA2,
    RP2040_REG_WS2812_LED7_DATA3,
    RP2040_REG_WS2812_LED8_DATA0,
    RP2040_REG_WS2812_LED8_DATA1,
    RP2040_REG_WS2812_LED8_DATA2,
    RP2040_REG_WS2812_LED8_DATA3,
    RP2040_REG_WS2812_LED9_DATA0,
    RP2040_REG_WS2812_LED9_DATA1,
    RP2040_REG_WS2812_LED9_DATA2,
    RP2040_REG_WS2812_LED9_DATA3,
    RP2040_REG_MSC_CONTROL,
    RP2040_REG_MSC_STATE,
    RP2040_REG_MSC0_BLOCK_COUNT_LO_A,
    RP2040_REG_MSC0_BLOCK_COUNT_LO_B,
    RP2040_REG_MSC0_BLOCK_COUNT_LO_C,
    RP2040_REG_MSC0_BLOCK_COUNT_HI,
    RP2040_REG_MSC0_BLOCK_SIZE_LO,
    RP2040_REG_MSC0_BLOCK_SIZE_HI,
    RP2040_REG_MSC1_BLOCK_COUNT_LO_A,
    RP2040_REG_MSC1_BLOCK_COUNT_LO_B,
    RP2040_REG_MSC1_BLOCK_COUNT_LO_C,
    RP2040_REG_MSC1_BLOCK_COUNT_HI,
    RP2040_REG_MSC1_BLOCK_SIZE_LO,
    RP2040_REG_MSC1_BLOCK_SIZE_HI,
};

enum { RP2040_BL_REG_FW_VER, RP2040_BL_REG_BL_VER, RP2040_BL_REG_BL_STATE, RP2040_BL_REG_BL_CTRL };

enum {
    RP2040_INPUT_BUTTON_HOME = 0,
    RP2040_INPUT_BUTTON_MENU,
    RP2040_INPUT_BUTTON_START,
    RP2040_INPUT_BUTTON_ACCEPT,
    RP2040_INPUT_BUTTON_BACK,
    RP2040_INPUT_FPGA_CDONE,
    RP2040_INPUT_BATTERY_CHARGING,
    RP2040_INPUT_BUTTON_SELECT,
    RP2040_INPUT_JOYSTICK_LEFT,
    RP2040_INPUT_JOYSTICK_PRESS,
    RP2040_INPUT_JOYSTICK_DOWN,
    RP2040_INPUT_JOYSTICK_UP,
    RP2040_INPUT_JOYSTICK_RIGHT
};

typedef void (*rp2040_intr_t)();

typedef struct {
    int               i2c_bus;
    int               i2c_address;
    int               pin_interrupt;
    QueueHandle_t     queue;
    SemaphoreHandle_t i2c_semaphore;
    rp2040_intr_t     _intr_handler;
    TaskHandle_t      _intr_task_handle;
    SemaphoreHandle_t _intr_trigger;
    uint8_t           _gpio_direction;
    uint8_t           _gpio_value;
    uint8_t           _fw_version;
} RP2040;

typedef struct _rp2040_input_message {
    uint8_t input;
    bool    state;
} rp2040_input_message_t;

esp_err_t rp2040_init(RP2040* device);

esp_err_t rp2040_get_firmware_version(RP2040* device, uint8_t* version);

esp_err_t rp2040_get_bootloader_version(RP2040* device, uint8_t* version);
esp_err_t rp2040_get_bootloader_state(RP2040* device, uint8_t* state);
esp_err_t rp2040_set_bootloader_ctrl(RP2040* device, uint8_t action);
esp_err_t rp2040_reboot_to_bootloader(RP2040* device);

esp_err_t rp2040_get_gpio_dir(RP2040* device, uint8_t gpio, bool* direction);
esp_err_t rp2040_set_gpio_dir(RP2040* device, uint8_t gpio, bool direction);

esp_err_t rp2040_get_gpio_value(RP2040* device, uint8_t gpio, bool* value);
esp_err_t rp2040_set_gpio_value(RP2040* device, uint8_t gpio, bool value);

esp_err_t rp2040_get_lcd_backlight(RP2040* device, uint8_t* brightness);
esp_err_t rp2040_set_lcd_backlight(RP2040* device, uint8_t brightness);

esp_err_t rp2040_set_fpga(RP2040* device, bool enabled);
esp_err_t rp2040_set_fpga_loopback(RP2040* device, bool enabled, bool loopback);

esp_err_t rp2040_read_buttons(RP2040* device, uint16_t* value);

esp_err_t rp2040_get_uid(RP2040* device, uint8_t* uid);

esp_err_t rp2040_read_vusb_raw(RP2040* device, uint16_t* value);
esp_err_t rp2040_read_vusb(RP2040* device, float* value);

esp_err_t rp2040_read_vbat_raw(RP2040* device, uint16_t* value);
esp_err_t rp2040_read_vbat(RP2040* device, float* value);

esp_err_t rp2040_read_temp(RP2040* device, uint16_t* value);
esp_err_t rp2040_get_charging(RP2040* device, uint8_t* charging);

esp_err_t rp2040_get_usb(RP2040* device, uint8_t* usb);

esp_err_t rp2040_get_webusb_mode(RP2040* device, uint8_t* mode);
esp_err_t rp2040_exit_webusb_mode(RP2040* device);

esp_err_t rp2040_get_crash_state(RP2040* device, uint8_t* crash_debug);

esp_err_t rp2040_ir_send(RP2040* device, uint16_t address, uint8_t command);

esp_err_t rp2040_get_reset_attempted(RP2040* device, uint8_t* reset_attempted);
esp_err_t rp2040_set_reset_attempted(RP2040* device, uint8_t reset_attempted);
esp_err_t rp2040_set_reset_lock(RP2040* device, uint8_t lock);

esp_err_t rp2040_set_ws2812_mode(RP2040* device, uint8_t mode);
esp_err_t rp2040_set_ws2812_length(RP2040* device, uint8_t length);
esp_err_t rp2040_set_ws2812_data(RP2040* device, uint8_t position, uint32_t value);
esp_err_t rp2040_ws2812_trigger(RP2040* device);

esp_err_t rp2040_set_msc_control(RP2040* device, uint8_t value);
esp_err_t rp2040_get_msc_state(RP2040* device, uint8_t* value);
esp_err_t rp2040_set_msc_block_count(RP2040* device, uint8_t lun, uint32_t value);
esp_err_t rp2040_set_msc_block_size(RP2040* device, uint8_t lun, uint16_t value);

#define MICROPY_HW_BOARD_NAME "uGame10"
#define MICROPY_HW_MCU_NAME "samd21e18"

#define SPI_FLASH_MOSI_PIN          &pin_PA16
#define SPI_FLASH_MISO_PIN          &pin_PA19
#define SPI_FLASH_SCK_PIN           &pin_PA17
#define SPI_FLASH_CS_PIN            &pin_PA18

// These are pins not to reset.
#define MICROPY_PORT_A        (0)
#define MICROPY_PORT_B        (0)
#define MICROPY_PORT_C        (0)

#define CALIBRATE_CRYSTALLESS 1

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code.
#define CIRCUITPY_INTERNAL_NVM_SIZE 256

#define BOARD_FLASH_SIZE (0x00040000 - 0x2000 - CIRCUITPY_INTERNAL_NVM_SIZE)

#define EXTRA_BUILTIN_MODULES \
    { MP_OBJ_NEW_QSTR(MP_QSTR_audioio), (mp_obj_t)&audioio_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_gamepad),(mp_obj_t)&gamepad_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR__stage), (mp_obj_t)&stage_module }

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1

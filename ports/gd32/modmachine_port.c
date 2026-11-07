// modmachine_port.c - included by extmod/modmachine.c via MICROPY_PY_MACHINE_INCLUDEFILE
// All functions here are static and become part of modmachine.c's translation unit.

#include "gd32f4xx.h"

// Types defined by the port itself (no extmod layer for these).
extern const mp_obj_type_t machine_dac_type;
extern const mp_obj_type_t machine_rtc_type;
extern const mp_obj_type_t machine_sdcard_type;
extern const mp_obj_type_t machine_flash_type;

#define MP_UNUSED __attribute__((unused))

// ---- reset ----
MP_NORETURN static void mp_machine_reset(void) {
    NVIC_SystemReset();
    for (;;) {
        ;
    }
}

// ---- reset cause ----
MP_UNUSED static mp_int_t mp_machine_reset_cause(void) {
    // GD32F4 has no sticky reset-cause register across all variants
    return 0;
}

// ---- unique id: 96-bit UID ----
MP_UNUSED static mp_obj_t mp_machine_unique_id(void) {
    const uint32_t *uid = (const uint32_t *)0x1FFF7A10;
    return mp_obj_new_bytes((const byte *)uid, 12);
}

// ---- CPU frequency ----
MP_UNUSED static mp_obj_t mp_machine_get_freq(void) {
    return mp_obj_new_int(SystemCoreClock);
}

MP_UNUSED static void mp_machine_set_freq(size_t n_args, const mp_obj_t *args) {
    mp_raise_NotImplementedError(MP_ERROR_TEXT("set_freq not supported"));
}

// ---- sleep ----
static void mp_machine_idle(void) {
    __WFI();
}

MP_UNUSED static void mp_machine_lightsleep(size_t n_args, const mp_obj_t *args) {
    __WFI();
}

MP_NORETURN MP_UNUSED static void mp_machine_deepsleep(size_t n_args, const mp_obj_t *args) {
    pmu_to_standbymode();
    for (;;) {
        ;
    }
}

// ---- machine.Pin registration ----
// modmachine.c expands MICROPY_PY_MACHINE_EXTRA_GLOBALS into its globals table.
#define MICROPY_PY_MACHINE_EXTRA_GLOBALS \
    { MP_ROM_QSTR(MP_QSTR_Pin),   MP_ROM_PTR(&machine_pin_type) }, \
    { MP_ROM_QSTR(MP_QSTR_Timer), MP_ROM_PTR(&machine_timer_type) }, \
    { MP_ROM_QSTR(MP_QSTR_WDT),   MP_ROM_PTR(&machine_wdt_type) }, \
    { MP_ROM_QSTR(MP_QSTR_RTC),   MP_ROM_PTR(&machine_rtc_type) }, \
    { MP_ROM_QSTR(MP_QSTR_SDCard), MP_ROM_PTR(&machine_sdcard_type) }, \
    { MP_ROM_QSTR(MP_QSTR_Flash),  MP_ROM_PTR(&machine_flash_type) }, \
    { MP_ROM_QSTR(MP_QSTR_UART),  MP_ROM_PTR(&machine_uart_type) }, \
    { MP_ROM_QSTR(MP_QSTR_SPI),   MP_ROM_PTR(&machine_spi_type) }, \
    { MP_ROM_QSTR(MP_QSTR_I2C),   MP_ROM_PTR(&machine_i2c_type) }, \
    { MP_ROM_QSTR(MP_QSTR_ADC),   MP_ROM_PTR(&machine_adc_type) }, \
    { MP_ROM_QSTR(MP_QSTR_PWM),   MP_ROM_PTR(&machine_pwm_type) }, \
    { MP_ROM_QSTR(MP_QSTR_SoftSPI), MP_ROM_PTR(&mp_machine_soft_spi_type) }, \
    { MP_ROM_QSTR(MP_QSTR_SoftI2C), MP_ROM_PTR(&mp_machine_soft_i2c_type) },

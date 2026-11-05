// modmachine_port.c - included by extmod/modmachine.c via MICROPY_PY_MACHINE_INCLUDEFILE
// All functions here are static and become part of modmachine.c's translation unit.

#include "gd32f4xx.h"

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
    { MP_ROM_QSTR(MP_QSTR_Pin), MP_ROM_PTR(&machine_pin_type) },

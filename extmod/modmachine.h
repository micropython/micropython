/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MICROPY_INCLUDED_EXTMOD_MODMACHINE_H
#define MICROPY_INCLUDED_EXTMOD_MODMACHINE_H

#include "py/mphal.h"
#include "py/obj.h"

#if MICROPY_PY_MACHINE_SPI || MICROPY_PY_MACHINE_SOFTSPI
#include "drivers/bus/spi.h"
#endif

// Whether to enable the ADC.init() method.
// Requires a port to implement mp_machine_adc_init_helper().
#ifndef MICROPY_PY_MACHINE_ADC_INIT
#define MICROPY_PY_MACHINE_ADC_INIT (0)
#endif

// Whether to enable the ADC.deinit() method.
// Requires a port to implement mp_machine_adc_deinit().
#ifndef MICROPY_PY_MACHINE_ADC_DEINIT
#define MICROPY_PY_MACHINE_ADC_DEINIT (0)
#endif

// Whether to enable the ADC.block() method.
// Requires a port to implement mp_machine_adc_block().
#ifndef MICROPY_PY_MACHINE_ADC_BLOCK
#define MICROPY_PY_MACHINE_ADC_BLOCK (0)
#endif

// Whether to enable the ADC.read_uv() method.
// Requires a port to implement mp_machine_adc_read_uv().
#ifndef MICROPY_PY_MACHINE_ADC_READ_UV
#define MICROPY_PY_MACHINE_ADC_READ_UV (0)
#endif

// Whether to enable the ADC.atten() and ADC.width() methods.
// Note: these are legacy and should not be used on new ports.
#ifndef MICROPY_PY_MACHINE_ADC_ATTEN_WIDTH
#define MICROPY_PY_MACHINE_ADC_ATTEN_WIDTH (0)
#endif

// Whether to enable the ADC.read() method.
// Note: this is legacy and should not be used on new ports.
#ifndef MICROPY_PY_MACHINE_ADC_READ
#define MICROPY_PY_MACHINE_ADC_READ (0)
#endif

// Whether to enable the UART.sendbreak() method.
// Requires a port to implement mp_machine_uart_sendbreak().
#ifndef MICROPY_PY_MACHINE_UART_SENDBREAK
#define MICROPY_PY_MACHINE_UART_SENDBREAK (0)
#endif

// Whether to enable the UART.readchar() and UART.writechar() methods.
// Requires a port to implement mp_machine_uart_readchar() and mp_machine_uart_writechar().
#ifndef MICROPY_PY_MACHINE_UART_READCHAR_WRITECHAR
#define MICROPY_PY_MACHINE_UART_READCHAR_WRITECHAR (0)
#endif

// Whether to enable the UART.irq() method.
// Requires a port to implement mp_machine_uart_irq().
#ifndef MICROPY_PY_MACHINE_UART_IRQ
#define MICROPY_PY_MACHINE_UART_IRQ (0)
#endif

// Temporary support for legacy construction of SoftI2C via I2C type.
#define MP_MACHINE_I2C_CHECK_FOR_LEGACY_SOFTI2C_CONSTRUCTION(n_args, n_kw, all_args) \
    do { \
        if (n_args == 0 || all_args[0] == MP_OBJ_NEW_SMALL_INT(-1)) { \
            mp_print_str(MICROPY_ERROR_PRINTER, "Warning: I2C(-1, ...) is deprecated, use SoftI2C(...) instead\n"); \
            if (n_args != 0) { \
                --n_args; \
                ++all_args; \
            } \
            return MP_OBJ_TYPE_GET_SLOT(&mp_machine_soft_i2c_type, make_new)(&mp_machine_soft_i2c_type, n_args, n_kw, all_args); \
        } \
    } while (0)

// Temporary support for legacy construction of SoftSPI via SPI type.
#define MP_MACHINE_SPI_CHECK_FOR_LEGACY_SOFTSPI_CONSTRUCTION(n_args, n_kw, all_args) \
    do { \
        if (n_args == 0 || all_args[0] == MP_OBJ_NEW_SMALL_INT(-1)) { \
            mp_print_str(MICROPY_ERROR_PRINTER, "Warning: SPI(-1, ...) is deprecated, use SoftSPI(...) instead\n"); \
            if (n_args != 0) { \
                --n_args; \
                ++all_args; \
            } \
            return MP_OBJ_TYPE_GET_SLOT(&mp_machine_soft_spi_type, make_new)(&mp_machine_soft_spi_type, n_args, n_kw, all_args); \
        } \
    } while (0)

#if MICROPY_PY_MACHINE_I2C || MICROPY_PY_MACHINE_SOFTI2C

#define MP_MACHINE_I2C_FLAG_READ (0x01) // if not set then it's a write
#define MP_MACHINE_I2C_FLAG_STOP (0x02)

#if MICROPY_PY_MACHINE_I2C_TRANSFER_WRITE1
// If set, the first mp_machine_i2c_buf_t in a transfer is a write.
#define MP_MACHINE_I2C_FLAG_WRITE1 (0x04)
#endif

#endif

// A port must provide these types, but they are otherwise opaque.
typedef struct _machine_adc_obj_t machine_adc_obj_t;
typedef struct _machine_adc_block_obj_t machine_adc_block_obj_t;
typedef struct _machine_i2s_obj_t machine_i2s_obj_t;
typedef struct _machine_pwm_obj_t machine_pwm_obj_t;
typedef struct _machine_uart_obj_t machine_uart_obj_t;
typedef struct _machine_wdt_obj_t machine_wdt_obj_t;

typedef struct _machine_mem_obj_t {
    mp_obj_base_t base;
    unsigned elem_size; // in bytes
} machine_mem_obj_t;

#if MICROPY_PY_MACHINE_I2C || MICROPY_PY_MACHINE_SOFTI2C

typedef struct _mp_machine_i2c_buf_t {
    size_t len;
    uint8_t *buf;
} mp_machine_i2c_buf_t;

// I2C protocol:
// - init must be non-NULL
// - start/stop/read/write can be NULL, meaning operation is not supported
// - transfer must be non-NULL
// - transfer_single only needs to be set if transfer=mp_machine_i2c_transfer_adaptor
typedef struct _mp_machine_i2c_p_t {
    #if MICROPY_PY_MACHINE_I2C_TRANSFER_WRITE1
    bool transfer_supports_write1;
    #endif
    void (*init)(mp_obj_base_t *obj, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
    int (*start)(mp_obj_base_t *obj);
    int (*stop)(mp_obj_base_t *obj);
    int (*read)(mp_obj_base_t *obj, uint8_t *dest, size_t len, bool nack);
    int (*write)(mp_obj_base_t *obj, const uint8_t *src, size_t len);
    int (*transfer)(mp_obj_base_t *obj, uint16_t addr, size_t n, mp_machine_i2c_buf_t *bufs, unsigned int flags);
    int (*transfer_single)(mp_obj_base_t *obj, uint16_t addr, size_t len, uint8_t *buf, unsigned int flags);
} mp_machine_i2c_p_t;

// SoftI2C object.
typedef struct _mp_machine_soft_i2c_obj_t {
    mp_obj_base_t base;
    uint32_t us_delay;
    uint32_t us_timeout;
    mp_hal_pin_obj_t scl;
    mp_hal_pin_obj_t sda;
} mp_machine_soft_i2c_obj_t;

#endif

#if MICROPY_PY_MACHINE_SPI || MICROPY_PY_MACHINE_SOFTSPI

// SPI protocol.
typedef struct _mp_machine_spi_p_t {
    void (*init)(mp_obj_base_t *obj, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
    void (*deinit)(mp_obj_base_t *obj); // can be NULL
    void (*transfer)(mp_obj_base_t *obj, size_t len, const uint8_t *src, uint8_t *dest);
} mp_machine_spi_p_t;

// SoftSPI object.
typedef struct _mp_machine_soft_spi_obj_t {
    mp_obj_base_t base;
    mp_soft_spi_obj_t spi;
} mp_machine_soft_spi_obj_t;

#endif

// Objects for machine.mem8, machine.mem16 and machine.mem32.
extern const machine_mem_obj_t machine_mem8_obj;
extern const machine_mem_obj_t machine_mem16_obj;
extern const machine_mem_obj_t machine_mem32_obj;

// These classes correspond to machine.Type entries in the machine module.
// Their Python bindings are implemented in extmod, and their implementation
// is provided by a port.
extern const mp_obj_type_t machine_adc_type;
extern const mp_obj_type_t machine_adc_block_type;
extern const mp_obj_type_t machine_i2c_type;
extern const mp_obj_type_t machine_i2s_type;
extern const mp_obj_type_t machine_mem_type;
extern const mp_obj_type_t machine_pin_type;
extern const mp_obj_type_t machine_pinbase_type;
extern const mp_obj_type_t machine_pwm_type;
extern const mp_obj_type_t machine_rtc_type;
extern const mp_obj_type_t machine_signal_type;
extern const mp_obj_type_t machine_spi_type;
extern const mp_obj_type_t machine_timer_type;
extern const mp_obj_type_t machine_uart_type;
extern const mp_obj_type_t machine_wdt_type;

#if MICROPY_PY_MACHINE_SOFTI2C
extern const mp_obj_type_t mp_machine_soft_i2c_type;
#endif
#if MICROPY_PY_MACHINE_I2C || MICROPY_PY_MACHINE_SOFTI2C
extern const mp_obj_dict_t mp_machine_i2c_locals_dict;
#endif

#if MICROPY_PY_MACHINE_SOFTSPI
extern const mp_obj_type_t mp_machine_soft_spi_type;
extern const mp_machine_spi_p_t mp_machine_soft_spi_p;
#endif
#if MICROPY_PY_MACHINE_SPI || MICROPY_PY_MACHINE_SOFTSPI
extern const mp_obj_dict_t mp_machine_spi_locals_dict;
#endif

#if defined(MICROPY_MACHINE_MEM_GET_READ_ADDR)
uintptr_t MICROPY_MACHINE_MEM_GET_READ_ADDR(mp_obj_t addr_o, uint align);
#endif
#if defined(MICROPY_MACHINE_MEM_GET_WRITE_ADDR)
uintptr_t MICROPY_MACHINE_MEM_GET_WRITE_ADDR(mp_obj_t addr_o, uint align);
#endif

NORETURN mp_obj_t machine_bootloader(size_t n_args, const mp_obj_t *args);
void machine_bitstream_high_low(mp_hal_pin_obj_t pin, uint32_t *timing_ns, const uint8_t *buf, size_t len);
mp_uint_t machine_time_pulse_us(mp_hal_pin_obj_t pin, int pulse_level, mp_uint_t timeout_us);

MP_DECLARE_CONST_FUN_OBJ_0(machine_unique_id_obj);
MP_DECLARE_CONST_FUN_OBJ_0(machine_reset_obj);
MP_DECLARE_CONST_FUN_OBJ_0(machine_reset_cause_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(machine_freq_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(machine_lightsleep_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(machine_deepsleep_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(machine_bootloader_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(machine_bitstream_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(machine_time_pulse_us_obj);

#if MICROPY_PY_MACHINE_I2C
int mp_machine_i2c_transfer_adaptor(mp_obj_base_t *self, uint16_t addr, size_t n, mp_machine_i2c_buf_t *bufs, unsigned int flags);
int mp_machine_soft_i2c_transfer(mp_obj_base_t *self, uint16_t addr, size_t n, mp_machine_i2c_buf_t *bufs, unsigned int flags);
#endif

#if MICROPY_PY_MACHINE_SPI
mp_obj_t mp_machine_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_spi_read_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_spi_readinto_obj);
MP_DECLARE_CONST_FUN_OBJ_2(mp_machine_spi_write_obj);
MP_DECLARE_CONST_FUN_OBJ_3(mp_machine_spi_write_readinto_obj);
#endif

#endif // MICROPY_INCLUDED_EXTMOD_MODMACHINE_H

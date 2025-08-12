/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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

#include "py/ringbuf.h"
#include "shared/runtime/interrupt_char.h"
#include "irq.h"
#include "system_tick.h"
#include ALIF_CMSIS_H

#define MICROPY_BEGIN_ATOMIC_SECTION()     disable_irq()
#define MICROPY_END_ATOMIC_SECTION(state)  enable_irq(state)

// For regular code that wants to prevent "background tasks" from running.
// These background tasks (LWIP, Bluetooth) run in PENDSV context.
#define MICROPY_PY_PENDSV_ENTER   uint32_t atomic_state = raise_irq_pri(IRQ_PRI_PENDSV);
#define MICROPY_PY_PENDSV_REENTER atomic_state = raise_irq_pri(IRQ_PRI_PENDSV);
#define MICROPY_PY_PENDSV_EXIT    restore_irq_pri(atomic_state);

// Prevent the "lwIP task" from running.
#define MICROPY_PY_LWIP_ENTER   MICROPY_PY_PENDSV_ENTER
#define MICROPY_PY_LWIP_REENTER MICROPY_PY_PENDSV_REENTER
#define MICROPY_PY_LWIP_EXIT    MICROPY_PY_PENDSV_EXIT

// Port level Wait-for-Event macro
//
// Do not use this macro directly, include py/runtime.h and
// call mp_event_wait_indefinite() or mp_event_wait_ms(timeout)
#define MICROPY_INTERNAL_WFE(TIMEOUT_MS) \
    do {                                 \
        if ((TIMEOUT_MS) < 0) { \
            __WFE(); \
        } else { \
            system_tick_wfe_with_timeout_us(TIMEOUT_MS * 1000); \
        } \
    } while (0)

// TODO requires mods to py/emitglue.c for this to be picked up
#define MP_HAL_CLEAN_DCACHE(addr, size) \
    (SCB_CleanDCache_by_Addr((uint32_t *)((uint32_t)addr & ~0x1f), \
    ((uint32_t)((uint8_t *)addr + size + 0x1f) & ~0x1f) - ((uint32_t)addr & ~0x1f)))

#define mp_hal_quiet_timing_enter() raise_irq_pri(IRQ_PRI_QUIET_TIMING)
#define mp_hal_quiet_timing_exit(irq_state) restore_irq_pri(irq_state)
#define mp_hal_delay_us_fast mp_hal_delay_us

extern ringbuf_t stdin_ringbuf;

/******************************************************************************/
// C-level pin HAL

#include "py/obj.h"
#include "gpio.h"
#include "pinconf.h"

#define MP_HAL_PIN_FMT                          "%q"
#define MP_HAL_PIN_MODE_INPUT                   (0)
#define MP_HAL_PIN_MODE_OUTPUT                  (1)
#define MP_HAL_PIN_MODE_OPEN_DRAIN              (2)
#define MP_HAL_PIN_MODE_ALT                     (3)
#define MP_HAL_PIN_PULL_NONE                    (0)
#define MP_HAL_PIN_PULL_UP                      (1)
#define MP_HAL_PIN_PULL_DOWN                    (2)
#define MP_HAL_PIN_DRIVE_2MA                    (PADCTRL_OUTPUT_DRIVE_STRENGTH_2MA)
#define MP_HAL_PIN_DRIVE_4MA                    (PADCTRL_OUTPUT_DRIVE_STRENGTH_4MA)
#define MP_HAL_PIN_DRIVE_8MA                    (PADCTRL_OUTPUT_DRIVE_STRENGTH_8MA)
#define MP_HAL_PIN_DRIVE_12MA                   (PADCTRL_OUTPUT_DRIVE_STRENGTH_12MA)
#define MP_HAL_PIN_SPEED_LOW                    (0)
#define MP_HAL_PIN_SPEED_HIGH                   (PADCTRL_SLEW_RATE_FAST)

#define MP_HAL_PIN_TRIGGER_FALL                 (1)
#define MP_HAL_PIN_TRIGGER_RISE                 (2)

#define mp_hal_pin_obj_t const machine_pin_obj_t *

#define MP_HAL_PIN_ALT(function, unit)          (MP_HAL_PIN_ALT_MAKE((MP_HAL_PIN_ALT_##function), (unit)))
#define MP_HAL_PIN_ALT_MAKE(function, unit)     ((function) | ((unit) << 8))
#define MP_HAL_PIN_ALT_GET_FUNC(alt)            ((alt) & 0xff)
#define MP_HAL_PIN_ALT_GET_UNIT(alt)            ((alt) >> 8)

enum {
    MP_HAL_PIN_ALT_NONE = 0,
    MP_HAL_PIN_ALT_GPIO,
    MP_HAL_PIN_ALT_ANA,
    MP_HAL_PIN_ALT_AUDIO_CLK,
    MP_HAL_PIN_ALT_CAM_D0,
    MP_HAL_PIN_ALT_CAM_D1,
    MP_HAL_PIN_ALT_CAM_D2,
    MP_HAL_PIN_ALT_CAM_D3,
    MP_HAL_PIN_ALT_CAM_D4,
    MP_HAL_PIN_ALT_CAM_D5,
    MP_HAL_PIN_ALT_CAM_D6,
    MP_HAL_PIN_ALT_CAM_D7,
    MP_HAL_PIN_ALT_CAM_D8,
    MP_HAL_PIN_ALT_CAM_D9,
    MP_HAL_PIN_ALT_CAM_D10,
    MP_HAL_PIN_ALT_CAM_D11,
    MP_HAL_PIN_ALT_CAM_D12,
    MP_HAL_PIN_ALT_CAM_D13,
    MP_HAL_PIN_ALT_CAM_D14,
    MP_HAL_PIN_ALT_CAM_D15,
    MP_HAL_PIN_ALT_CAM_HSYNC,
    MP_HAL_PIN_ALT_CAM_PCLK,
    MP_HAL_PIN_ALT_CAM_VSYNC,
    MP_HAL_PIN_ALT_CAM_XVCLK,
    MP_HAL_PIN_ALT_CDC_D0,
    MP_HAL_PIN_ALT_CDC_D1,
    MP_HAL_PIN_ALT_CDC_D2,
    MP_HAL_PIN_ALT_CDC_D3,
    MP_HAL_PIN_ALT_CDC_D4,
    MP_HAL_PIN_ALT_CDC_D5,
    MP_HAL_PIN_ALT_CDC_D6,
    MP_HAL_PIN_ALT_CDC_D7,
    MP_HAL_PIN_ALT_CDC_D8,
    MP_HAL_PIN_ALT_CDC_D9,
    MP_HAL_PIN_ALT_CDC_D10,
    MP_HAL_PIN_ALT_CDC_D11,
    MP_HAL_PIN_ALT_CDC_D12,
    MP_HAL_PIN_ALT_CDC_D13,
    MP_HAL_PIN_ALT_CDC_D14,
    MP_HAL_PIN_ALT_CDC_D15,
    MP_HAL_PIN_ALT_CDC_D16,
    MP_HAL_PIN_ALT_CDC_D17,
    MP_HAL_PIN_ALT_CDC_D18,
    MP_HAL_PIN_ALT_CDC_D19,
    MP_HAL_PIN_ALT_CDC_D20,
    MP_HAL_PIN_ALT_CDC_D21,
    MP_HAL_PIN_ALT_CDC_D22,
    MP_HAL_PIN_ALT_CDC_D23,
    MP_HAL_PIN_ALT_CDC_DE,
    MP_HAL_PIN_ALT_CDC_HSYNC,
    MP_HAL_PIN_ALT_CDC_PCLK,
    MP_HAL_PIN_ALT_CDC_VSYNC,
    MP_HAL_PIN_ALT_CMP_OUT,
    MP_HAL_PIN_ALT_ETH_CRS_DV_A,
    MP_HAL_PIN_ALT_ETH_CRS_DV_B,
    MP_HAL_PIN_ALT_ETH_CRS_DV_C,
    MP_HAL_PIN_ALT_ETH_IRQ,
    MP_HAL_PIN_ALT_ETH_MDC,
    MP_HAL_PIN_ALT_ETH_MDIO,
    MP_HAL_PIN_ALT_ETH_REFCLK,
    MP_HAL_PIN_ALT_ETH_RST,
    MP_HAL_PIN_ALT_ETH_RXD0,
    MP_HAL_PIN_ALT_ETH_RXD1,
    MP_HAL_PIN_ALT_ETH_TXD0,
    MP_HAL_PIN_ALT_ETH_TXD1,
    MP_HAL_PIN_ALT_ETH_TXEN,
    MP_HAL_PIN_ALT_FAULT_A,
    MP_HAL_PIN_ALT_FAULT_B,
    MP_HAL_PIN_ALT_FAULT_C,
    MP_HAL_PIN_ALT_HFXO_OUT,
    MP_HAL_PIN_ALT_I2C_SCL,
    MP_HAL_PIN_ALT_I2C_SDA,
    MP_HAL_PIN_ALT_I2S_SCLK,
    MP_HAL_PIN_ALT_I2S_SDI,
    MP_HAL_PIN_ALT_I2S_SDO,
    MP_HAL_PIN_ALT_I2S_WS,
    MP_HAL_PIN_ALT_I3C_SCL,
    MP_HAL_PIN_ALT_I3C_SDA,
    MP_HAL_PIN_ALT_JTAG_TCK,
    MP_HAL_PIN_ALT_JTAG_TDATA0,
    MP_HAL_PIN_ALT_JTAG_TDATA1,
    MP_HAL_PIN_ALT_JTAG_TDATA2,
    MP_HAL_PIN_ALT_JTAG_TDATA3,
    MP_HAL_PIN_ALT_JTAG_TDI,
    MP_HAL_PIN_ALT_JTAG_TDO,
    MP_HAL_PIN_ALT_JTAG_TMS,
    MP_HAL_PIN_ALT_JTAG_TRACECLK,
    MP_HAL_PIN_ALT_LPCAM_D0,
    MP_HAL_PIN_ALT_LPCAM_D1,
    MP_HAL_PIN_ALT_LPCAM_D2,
    MP_HAL_PIN_ALT_LPCAM_D3,
    MP_HAL_PIN_ALT_LPCAM_D4,
    MP_HAL_PIN_ALT_LPCAM_D5,
    MP_HAL_PIN_ALT_LPCAM_D6,
    MP_HAL_PIN_ALT_LPCAM_D7,
    MP_HAL_PIN_ALT_LPCAM_HSYNC,
    MP_HAL_PIN_ALT_LPCAM_PCLK,
    MP_HAL_PIN_ALT_LPCAM_VSYNC,
    MP_HAL_PIN_ALT_LPCAM_XVCLK,
    MP_HAL_PIN_ALT_LPI2C_SCL,
    MP_HAL_PIN_ALT_LPI2C_SDA,
    MP_HAL_PIN_ALT_LPI2S_SCLK,
    MP_HAL_PIN_ALT_LPI2S_SDI,
    MP_HAL_PIN_ALT_LPI2S_SDO,
    MP_HAL_PIN_ALT_LPI2S_WS,
    MP_HAL_PIN_ALT_LPPDM_C0,
    MP_HAL_PIN_ALT_LPPDM_C1,
    MP_HAL_PIN_ALT_LPPDM_C2,
    MP_HAL_PIN_ALT_LPPDM_C3,
    MP_HAL_PIN_ALT_LPPDM_D0,
    MP_HAL_PIN_ALT_LPPDM_D1,
    MP_HAL_PIN_ALT_LPPDM_D2,
    MP_HAL_PIN_ALT_LPPDM_D3,
    MP_HAL_PIN_ALT_LPSPI_MISO,
    MP_HAL_PIN_ALT_LPSPI_MOSI,
    MP_HAL_PIN_ALT_LPSPI_SCLK,
    MP_HAL_PIN_ALT_LPSPI_SS,
    MP_HAL_PIN_ALT_LPTMR_CLK,
    MP_HAL_PIN_ALT_LPUART_CTS,
    MP_HAL_PIN_ALT_LPUART_RTS,
    MP_HAL_PIN_ALT_LPUART_RX,
    MP_HAL_PIN_ALT_LPUART_TX,
    MP_HAL_PIN_ALT_OSPI_D0,
    MP_HAL_PIN_ALT_OSPI_D1,
    MP_HAL_PIN_ALT_OSPI_D2,
    MP_HAL_PIN_ALT_OSPI_D3,
    MP_HAL_PIN_ALT_OSPI_D4,
    MP_HAL_PIN_ALT_OSPI_D5,
    MP_HAL_PIN_ALT_OSPI_D6,
    MP_HAL_PIN_ALT_OSPI_D7,
    MP_HAL_PIN_ALT_OSPI_RXDS,
    MP_HAL_PIN_ALT_OSPI_SCLK,
    MP_HAL_PIN_ALT_OSPI_SCLKN,
    MP_HAL_PIN_ALT_OSPI_SS0,
    MP_HAL_PIN_ALT_OSPI_SS1,
    MP_HAL_PIN_ALT_PDM_C0,
    MP_HAL_PIN_ALT_PDM_C1,
    MP_HAL_PIN_ALT_PDM_C2,
    MP_HAL_PIN_ALT_PDM_C3,
    MP_HAL_PIN_ALT_PDM_D0,
    MP_HAL_PIN_ALT_PDM_D1,
    MP_HAL_PIN_ALT_PDM_D2,
    MP_HAL_PIN_ALT_PDM_D3,
    MP_HAL_PIN_ALT_QEC_X,
    MP_HAL_PIN_ALT_QEC_Y,
    MP_HAL_PIN_ALT_QEC_Z,
    MP_HAL_PIN_ALT_SD_CLK,
    MP_HAL_PIN_ALT_SD_CMD,
    MP_HAL_PIN_ALT_SD_D0,
    MP_HAL_PIN_ALT_SD_D1,
    MP_HAL_PIN_ALT_SD_D2,
    MP_HAL_PIN_ALT_SD_D3,
    MP_HAL_PIN_ALT_SD_D4,
    MP_HAL_PIN_ALT_SD_D5,
    MP_HAL_PIN_ALT_SD_D6,
    MP_HAL_PIN_ALT_SD_D7,
    MP_HAL_PIN_ALT_SD_RST,
    MP_HAL_PIN_ALT_SPI_MISO,
    MP_HAL_PIN_ALT_SPI_MOSI,
    MP_HAL_PIN_ALT_SPI_SCLK,
    MP_HAL_PIN_ALT_SPI_SS0,
    MP_HAL_PIN_ALT_SPI_SS1,
    MP_HAL_PIN_ALT_SPI_SS2,
    MP_HAL_PIN_ALT_SPI_SS3,
    MP_HAL_PIN_ALT_UART_CTS,
    MP_HAL_PIN_ALT_UART_DE,
    MP_HAL_PIN_ALT_UART_RTS,
    MP_HAL_PIN_ALT_UART_RX,
    MP_HAL_PIN_ALT_UART_TX,
    MP_HAL_PIN_ALT_UT_T0,
    MP_HAL_PIN_ALT_UT_T1,
};

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    GPIO_Type *gpio;
    uint8_t port;
    uint8_t pin;
    uint8_t adc12_periph : 2;
    uint8_t adc12_channel : 3;
    qstr name;
    const uint16_t alt[8]; // holds result of MP_HAL_PIN_ALT_MAKE(function, unit)
} machine_pin_obj_t;

mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t pin_in);

static inline qstr mp_hal_pin_name(mp_hal_pin_obj_t pin) {
    return pin->name;
}

static inline void mp_hal_pin_input(mp_hal_pin_obj_t pin) {
    uint8_t alt_func = PINMUX_ALTERNATE_FUNCTION_0;
    uint8_t pad_ctrl = PADCTRL_READ_ENABLE;
    pinconf_set(pin->port, pin->pin, alt_func, pad_ctrl);
    gpio_set_direction_input(pin->gpio, pin->pin);
}

static inline void mp_hal_pin_output(mp_hal_pin_obj_t pin) {
    uint8_t alt_func = PINMUX_ALTERNATE_FUNCTION_0;
    uint8_t pad_ctrl = PADCTRL_READ_ENABLE;
    pinconf_set(pin->port, pin->pin, alt_func, pad_ctrl);
    gpio_set_direction_output(pin->gpio, pin->pin);
}

static inline void mp_hal_pin_open_drain(mp_hal_pin_obj_t pin) {
    uint8_t alt_func = PINMUX_ALTERNATE_FUNCTION_0;
    uint8_t pad_ctrl = PADCTRL_DRIVER_OPEN_DRAIN | PADCTRL_READ_ENABLE;
    pinconf_set(pin->port, pin->pin, alt_func, pad_ctrl);
    gpio_set_direction_output(pin->gpio, pin->pin);
}

static inline void mp_hal_pin_config_irq_falling(mp_hal_pin_obj_t pin, bool enable) {
    if (enable) {
        gpio_enable_interrupt(pin->gpio, pin->pin);
        gpio_interrupt_set_edge_trigger(pin->gpio, pin->pin);
        gpio_interrupt_set_polarity_low(pin->gpio, pin->pin);
    } else {
        gpio_disable_interrupt(pin->gpio, pin->pin);
    }
}

static inline void mp_hal_pin_low(mp_hal_pin_obj_t pin) {
    gpio_set_value_low(pin->gpio, pin->pin);
}

static inline void mp_hal_pin_high(mp_hal_pin_obj_t pin) {
    gpio_set_value_high(pin->gpio, pin->pin);
}

static inline int mp_hal_pin_read(mp_hal_pin_obj_t pin) {
    return gpio_get_value(pin->gpio, pin->pin);
}

static inline void mp_hal_pin_write(mp_hal_pin_obj_t pin, int v) {
    if (v) {
        mp_hal_pin_high(pin);
    } else {
        mp_hal_pin_low(pin);
    }
}

static inline void mp_hal_pin_od_low(mp_hal_pin_obj_t pin) {
    mp_hal_pin_low(pin);
}

static inline void mp_hal_pin_od_high(mp_hal_pin_obj_t pin) {
    mp_hal_pin_high(pin);
}

static inline void mp_hal_wake_main_task_from_isr(void) {
    // Defined for tinyusb support, nothing needs to be done here.
}

void mp_hal_pin_config(const machine_pin_obj_t *pin, uint32_t mode,
    uint32_t pull, uint32_t speed, uint32_t drive, uint32_t alt, bool ren);

// Include all the pin definitions.
#include "genhdr/pins_board.h"

/******************************************************************************/
// Other HAL functions.

enum {
    MP_HAL_MAC_WLAN0 = 0,
    MP_HAL_MAC_WLAN1,
    MP_HAL_MAC_BDADDR,
    MP_HAL_MAC_ETH0,
};

void mp_hal_generate_laa_mac(int idx, uint8_t buf[6]);
void mp_hal_get_mac(int idx, uint8_t buf[6]);
void mp_hal_get_mac_ascii(int idx, size_t chr_off, size_t chr_len, char *dest);

uint32_t mp_hal_time_get(uint32_t *microseconds);

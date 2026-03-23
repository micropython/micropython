/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 "Eric Poulsen" <eric@zyxod.com>
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"

#include "driver/spi_master.h"
#include "soc/gpio_sig_map.h"
#include "soc/spi_pins.h"

// Bare-metal SPI support for ESP32 rev 3.1+ (spi_master driver bug workaround)
#if CONFIG_IDF_TARGET_ESP32
#include "esp_chip_info.h"
#include "soc/dport_reg.h"
#include "soc/spi_struct.h"
#include "soc/spi_reg.h"
#include "esp_rom_gpio.h"
#include "driver/gpio.h"
#endif

// SPI mappings by device, naming used by IDF old/new
// upython   | ESP32     | ESP32S2   | ESP32S3 | ESP32C3 | ESP32C6
// ----------+-----------+-----------+---------+---------+---------
// SPI(id=1) | HSPI/SPI2 | FSPI/SPI2 | SPI2    | SPI2    | SPI2
// SPI(id=2) | VSPI/SPI3 | HSPI/SPI3 | SPI3    | err     | err

// Number of available hardware SPI peripherals.
#if SOC_SPI_PERIPH_NUM > 2
#define MICROPY_HW_SPI_MAX (2)
#else
#define MICROPY_HW_SPI_MAX (1)
#endif

// Default pins for SPI(id=1) aka IDF SPI2, can be overridden by a board
#ifndef MICROPY_HW_SPI1_SCK
// Use IO_MUX pins by default.
// If SPI lines are routed to other pins through GPIO matrix
// routing adds some delay and lower limit applies to SPI clk freq
#define MICROPY_HW_SPI1_SCK SPI2_IOMUX_PIN_NUM_CLK
#define MICROPY_HW_SPI1_MOSI SPI2_IOMUX_PIN_NUM_MOSI
#define MICROPY_HW_SPI1_MISO SPI2_IOMUX_PIN_NUM_MISO
#endif

// Default pins for SPI(id=2) aka IDF SPI3, can be overridden by a board
#ifndef MICROPY_HW_SPI2_SCK
#if CONFIG_IDF_TARGET_ESP32
// ESP32 has IO_MUX pins for VSPI/SPI3 lines, use them as defaults
#define MICROPY_HW_SPI2_SCK SPI3_IOMUX_PIN_NUM_CLK      // pin 18
#define MICROPY_HW_SPI2_MOSI SPI3_IOMUX_PIN_NUM_MOSI    // pin 23
#define MICROPY_HW_SPI2_MISO SPI3_IOMUX_PIN_NUM_MISO    // pin 19
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
// ESP32S2 and S3 uses GPIO matrix for SPI3 pins, no IO_MUX possible
// Set defaults to the pins used by SPI2 in Octal mode
#define MICROPY_HW_SPI2_SCK (36)
#define MICROPY_HW_SPI2_MOSI (35)
#define MICROPY_HW_SPI2_MISO (37)
#endif
#endif

#define MP_HW_SPI_MAX_XFER_BYTES (4092)
#define MP_HW_SPI_MAX_XFER_BITS (MP_HW_SPI_MAX_XFER_BYTES * 8) // Has to be an even multiple of 8

typedef struct _machine_hw_spi_default_pins_t {
    union {
        int8_t array[3];
        struct {
            // Must be in enum's ARG_sck, ARG_mosi, ARG_miso, etc. order
            int8_t sck;
            int8_t mosi;
            int8_t miso;
        } pins;
    };
} machine_hw_spi_default_pins_t;

typedef struct _machine_hw_spi_obj_t {
    mp_obj_base_t base;
    spi_host_device_t host;
    uint32_t baudrate;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
    uint8_t firstbit;
    int8_t sck;
    int8_t mosi;
    int8_t miso;
    spi_device_handle_t spi;
    #if CONFIG_IDF_TARGET_ESP32
    bool use_bare_metal;             // true on ESP32 rev >= 3.1
    volatile spi_dev_t *spi_hw;     // direct register pointer for bare-metal mode
    #endif
    enum {
        MACHINE_HW_SPI_STATE_NONE,
        MACHINE_HW_SPI_STATE_INIT,
        MACHINE_HW_SPI_STATE_DEINIT
    } state;
} machine_hw_spi_obj_t;

// Default pin mappings for the hardware SPI instances
static const machine_hw_spi_default_pins_t machine_hw_spi_default_pins[MICROPY_HW_SPI_MAX] = {
    { .pins = { .sck = MICROPY_HW_SPI1_SCK, .mosi = MICROPY_HW_SPI1_MOSI, .miso = MICROPY_HW_SPI1_MISO }},
    #ifdef MICROPY_HW_SPI2_SCK
    { .pins = { .sck = MICROPY_HW_SPI2_SCK, .mosi = MICROPY_HW_SPI2_MOSI, .miso = MICROPY_HW_SPI2_MISO }},
    #endif
};

// Common arguments for init() and make new
enum { ARG_id, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_sck, ARG_mosi, ARG_miso };
static const mp_arg_t spi_allowed_args[] = {
    { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = -1} },
    { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = -1} },
    { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
};

// Static objects mapping to SPI2 (and SPI3 if available) hardware peripherals.
static machine_hw_spi_obj_t machine_hw_spi_obj[MICROPY_HW_SPI_MAX];

// ============================================================================
// Bare-metal SPI for ESP32 rev 3.1+
// Workaround for ESP-IDF spi_master driver bug that breaks GPIO matrix output
// ============================================================================
#if CONFIG_IDF_TARGET_ESP32

static bool esp32_needs_bare_metal_spi(void) {
    esp_chip_info_t info;
    esp_chip_info(&info);
    // revision >= 3.1 (encoded as 301)
    return info.revision >= 301;
}

// Calculate SPI clock divider registers from target frequency
// Returns actual frequency achieved
static uint32_t bare_metal_spi_calc_clock(uint32_t target_hz, volatile spi_dev_t *hw) {
    uint32_t apb = APB_CLK_FREQ;  // 80 MHz

    if (target_hz >= apb) {
        // Use APB clock directly
        hw->clock.clk_equ_sysclk = 1;
        return apb;
    }

    hw->clock.clk_equ_sysclk = 0;

    // Find best divider: freq = APB / (pre+1) / (n+1)
    // For simplicity, use pre=0 and vary n
    uint32_t best_n = 1;
    uint32_t best_freq = apb / 2;

    for (uint32_t n = 1; n <= 63; n++) {
        uint32_t freq = apb / (n + 1);
        if (freq <= target_hz && freq > best_freq) {
            best_freq = freq;
            best_n = n;
            if (freq == target_hz) break;
        }
        // Also try with best_freq tracking closest <= target
        if (freq <= target_hz) {
            best_freq = freq;
            best_n = n;
            break;
        }
    }

    hw->clock.clkdiv_pre = 0;
    hw->clock.clkcnt_n = best_n;
    hw->clock.clkcnt_h = (best_n + 1) / 2 - 1;
    hw->clock.clkcnt_l = best_n;

    return apb / (best_n + 1);
}

static void bare_metal_spi_init(machine_hw_spi_obj_t *self) {
    // Get the right SPI peripheral
    volatile spi_dev_t *hw;
    uint32_t clk_en_bit, rst_bit;
    uint8_t clk_out_sig, mosi_out_sig, miso_in_sig;

    if (self->host == SPI2_HOST) {
        hw = (volatile spi_dev_t *)0x3FF64000;  // SPI2 (HSPI)
        clk_en_bit = DPORT_SPI2_CLK_EN;
        rst_bit = DPORT_SPI2_RST;
        clk_out_sig = HSPICLK_OUT_IDX;
        mosi_out_sig = HSPID_OUT_IDX;
        miso_in_sig = HSPIQ_IN_IDX;
    } else {
        hw = (volatile spi_dev_t *)0x3FF65000;  // SPI3 (VSPI)
        clk_en_bit = DPORT_SPI3_CLK_EN;
        rst_bit = DPORT_SPI3_RST;
        clk_out_sig = VSPICLK_OUT_IDX;
        mosi_out_sig = VSPID_OUT_IDX;
        miso_in_sig = VSPIQ_IN_IDX;
    }

    self->spi_hw = hw;

    // Enable peripheral clock, clear reset
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, clk_en_bit);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, rst_bit);

    // Clear all registers (Arduino-style init)
    hw->slave.trans_done = 0;
    hw->slave.slave_mode = 0;
    hw->pin.val = 0;
    hw->user.val = 0;
    hw->user1.val = 0;
    hw->ctrl.val = 0;
    hw->ctrl1.val = 0;
    hw->ctrl2.val = 0;
    hw->clock.val = 0;

    // SPI mode (CPOL/CPHA)
    hw->pin.ck_idle_edge = self->polarity;
    hw->user.ck_out_edge = (self->polarity ^ self->phase) ? 1 : 0;

    // Clock configuration
    bare_metal_spi_calc_clock(self->baudrate, hw);

    // Enable MOSI, full-duplex
    hw->user.usr_mosi = 1;
    hw->user.doutdin = 1;

    // LSB first if requested
    if (self->firstbit == MICROPY_PY_MACHINE_SPI_LSB) {
        hw->ctrl.wr_bit_order = 1;
        hw->ctrl.rd_bit_order = 1;
    }

    // Attach SCK pin
    if (self->sck != -1) {
        gpio_set_direction(self->sck, GPIO_MODE_OUTPUT);
        esp_rom_gpio_connect_out_signal(self->sck, clk_out_sig, false, false);
    }

    // Attach MOSI pin
    if (self->mosi != -1) {
        gpio_set_direction(self->mosi, GPIO_MODE_OUTPUT);
        esp_rom_gpio_connect_out_signal(self->mosi, mosi_out_sig, false, false);
    }

    // Attach MISO pin (input)
    if (self->miso != -1) {
        gpio_set_direction(self->miso, GPIO_MODE_INPUT);
        esp_rom_gpio_connect_in_signal(self->miso, miso_in_sig, false);
    }
}

static void bare_metal_spi_deinit(machine_hw_spi_obj_t *self) {
    // Disable peripheral clock
    uint32_t clk_en_bit;
    if (self->host == SPI2_HOST) {
        clk_en_bit = DPORT_SPI2_CLK_EN;
    } else {
        clk_en_bit = DPORT_SPI3_CLK_EN;
    }
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, clk_en_bit);

    // Reset GPIO pins
    int8_t pins[3] = {self->miso, self->mosi, self->sck};
    for (int i = 0; i < 3; i++) {
        if (pins[i] != -1) {
            esp_rom_gpio_pad_select_gpio(pins[i]);
            esp_rom_gpio_connect_out_signal(pins[i], SIG_GPIO_OUT_IDX, false, false);
            gpio_set_direction(pins[i], GPIO_MODE_INPUT);
        }
    }

    self->spi_hw = NULL;
}

static void bare_metal_spi_transfer(machine_hw_spi_obj_t *self, size_t len, const uint8_t *src, uint8_t *dest) {
    volatile spi_dev_t *hw = self->spi_hw;

    while (len > 0) {
        int chunk = (len > 64) ? 64 : len;
        int bits = chunk * 8;
        int words = (chunk + 3) / 4;

        hw->mosi_dlen.usr_mosi_dbitlen = bits - 1;
        hw->miso_dlen.usr_miso_dbitlen = bits - 1;

        // Fill TX FIFO
        if (src != NULL) {
            volatile uint32_t *fifo = hw->data_buf;
            for (int i = 0; i < words; i++) {
                uint32_t word = 0;
                for (int j = 0; j < 4 && (i * 4 + j) < chunk; j++) {
                    word |= ((uint32_t)src[i * 4 + j]) << (j * 8);
                }
                fifo[i] = word;
            }
        } else {
            // Send zeros
            volatile uint32_t *fifo = hw->data_buf;
            for (int i = 0; i < words; i++) {
                fifo[i] = 0;
            }
        }

        // Start transaction
        hw->cmd.usr = 1;
        while (hw->cmd.usr) {
            // Busy wait — transactions are very short at MHz speeds
        }

        // Read RX FIFO
        if (dest != NULL) {
            volatile uint32_t *fifo = hw->data_buf;
            for (int i = 0; i < words; i++) {
                uint32_t word = fifo[i];
                for (int j = 0; j < 4 && (i * 4 + j) < chunk; j++) {
                    dest[i * 4 + j] = (word >> (j * 8)) & 0xFF;
                }
            }
        }

        if (src != NULL) src += chunk;
        if (dest != NULL) dest += chunk;
        len -= chunk;
    }
}

#endif // CONFIG_IDF_TARGET_ESP32

// ============================================================================
// Standard ESP-IDF SPI driver (original MicroPython code)
// ============================================================================

static void machine_hw_spi_deinit_internal(machine_hw_spi_obj_t *self) {
    #if CONFIG_IDF_TARGET_ESP32
    if (self->use_bare_metal) {
        bare_metal_spi_deinit(self);
        return;
    }
    #endif

    switch (spi_bus_remove_device(self->spi)) {
        case ESP_ERR_INVALID_ARG:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("invalid configuration"));
            return;

        case ESP_ERR_INVALID_STATE:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("SPI device already freed"));
            return;
    }

    switch (spi_bus_free(self->host)) {
        case ESP_ERR_INVALID_ARG:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("invalid configuration"));
            return;

        case ESP_ERR_INVALID_STATE:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("SPI bus already freed"));
            return;
    }

    int8_t pins[3] = {self->miso, self->mosi, self->sck};

    for (int i = 0; i < 3; i++) {
        if (pins[i] != -1) {
            esp_rom_gpio_pad_select_gpio(pins[i]);
            esp_rom_gpio_connect_out_signal(pins[i], SIG_GPIO_OUT_IDX, false, false);
            gpio_set_direction(pins[i], GPIO_MODE_INPUT);
        }
    }
}

static void machine_hw_spi_init_internal(machine_hw_spi_obj_t *self, mp_arg_val_t args[]) {

    // if we're not initialized, then we're
    // implicitly 'changed', since this is the init routine
    bool changed = self->state != MACHINE_HW_SPI_STATE_INIT;

    esp_err_t ret;

    machine_hw_spi_obj_t old_self = *self;

    if (args[ARG_baudrate].u_int != -1) {
        // calculate the actual clock frequency that the SPI peripheral can produce
        uint32_t baudrate = spi_get_actual_clock(APB_CLK_FREQ, args[ARG_baudrate].u_int, 0);
        if (baudrate != self->baudrate) {
            self->baudrate = baudrate;
            changed = true;
        }
    }

    if (args[ARG_polarity].u_int != -1 && args[ARG_polarity].u_int != self->polarity) {
        self->polarity = args[ARG_polarity].u_int;
        changed = true;
    }

    if (args[ARG_phase].u_int != -1 && args[ARG_phase].u_int != self->phase) {
        self->phase = args[ARG_phase].u_int;
        changed = true;
    }

    if (args[ARG_bits].u_int != -1 && args[ARG_bits].u_int <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid bits"));
    }

    if (args[ARG_bits].u_int != -1 && args[ARG_bits].u_int != self->bits) {
        self->bits = args[ARG_bits].u_int;
        changed = true;
    }

    if (args[ARG_firstbit].u_int != -1 && args[ARG_firstbit].u_int != self->firstbit) {
        self->firstbit = args[ARG_firstbit].u_int;
        changed = true;
    }

    if (args[ARG_sck].u_int != -2 && args[ARG_sck].u_int != self->sck) {
        self->sck = args[ARG_sck].u_int;
        changed = true;
    }

    if (args[ARG_mosi].u_int != -2 && args[ARG_mosi].u_int != self->mosi) {
        self->mosi = args[ARG_mosi].u_int;
        changed = true;
    }

    if (args[ARG_miso].u_int != -2 && args[ARG_miso].u_int != self->miso) {
        self->miso = args[ARG_miso].u_int;
        changed = true;
    }

    if (changed) {
        if (self->state == MACHINE_HW_SPI_STATE_INIT) {
            self->state = MACHINE_HW_SPI_STATE_DEINIT;
            machine_hw_spi_deinit_internal(&old_self);
        }
    } else {
        return; // no changes
    }

    #if CONFIG_IDF_TARGET_ESP32
    // Check if we need bare-metal SPI (ESP32 rev 3.1+ workaround)
    self->use_bare_metal = esp32_needs_bare_metal_spi();

    if (self->use_bare_metal) {
        bare_metal_spi_init(self);
        self->state = MACHINE_HW_SPI_STATE_INIT;
        return;
    }
    #endif

    spi_bus_config_t buscfg = {
        .miso_io_num = self->miso,
        .mosi_io_num = self->mosi,
        .sclk_io_num = self->sck,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = self->baudrate,
        .mode = self->phase | (self->polarity << 1),
        .spics_io_num = -1, // No CS pin
        .queue_size = 2,
        .flags = self->firstbit == MICROPY_PY_MACHINE_SPI_LSB ? SPI_DEVICE_TXBIT_LSBFIRST | SPI_DEVICE_RXBIT_LSBFIRST : 0,
        .pre_cb = NULL
    };

    // Initialize the SPI bus

    // Select DMA channel based on the hardware SPI host
    int dma_chan = 0;
    #if CONFIG_IDF_TARGET_ESP32
    if (self->host == SPI2_HOST) {
        dma_chan = 1;
    } else {
        dma_chan = 2;
    }
    #else
    dma_chan = SPI_DMA_CH_AUTO;
    #endif

    ret = spi_bus_initialize(self->host, &buscfg, dma_chan);
    switch (ret) {
        case ESP_ERR_INVALID_ARG:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("invalid configuration"));
            return;

        case ESP_ERR_INVALID_STATE:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("SPI host already in use"));
            return;
    }

    ret = spi_bus_add_device(self->host, &devcfg, &self->spi);
    switch (ret) {
        case ESP_ERR_INVALID_ARG:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("invalid configuration"));
            spi_bus_free(self->host);
            return;

        case ESP_ERR_NO_MEM:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("out of memory"));
            spi_bus_free(self->host);
            return;

        case ESP_ERR_NOT_FOUND:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("no free slots"));
            spi_bus_free(self->host);
            return;
    }
    self->state = MACHINE_HW_SPI_STATE_INIT;
}

static void machine_hw_spi_deinit(mp_obj_base_t *self_in) {
    machine_hw_spi_obj_t *self = (machine_hw_spi_obj_t *)self_in;
    if (self->state == MACHINE_HW_SPI_STATE_INIT) {
        self->state = MACHINE_HW_SPI_STATE_DEINIT;
        machine_hw_spi_deinit_internal(self);
    }
}

static mp_uint_t gcd(mp_uint_t x, mp_uint_t y) {
    while (x != y) {
        if (x > y) {
            x -= y;
        } else {
            y -= x;
        }
    }
    return x;
}

static void machine_hw_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    machine_hw_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->state == MACHINE_HW_SPI_STATE_DEINIT) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("transfer on deinitialized SPI"));
        return;
    }

    // Round to nearest whole set of bits
    int bits_to_send = len * 8 / self->bits * self->bits;

    if (!bits_to_send) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffer too short"));
    }

    #if CONFIG_IDF_TARGET_ESP32
    if (self->use_bare_metal) {
        bare_metal_spi_transfer(self, len, src, dest);
        return;
    }
    #endif

    if (len <= 4) {
        spi_transaction_t transaction = { 0 };

        if (src != NULL) {
            memcpy(&transaction.tx_data, src, len);
        }

        transaction.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
        transaction.length = bits_to_send;
        spi_device_transmit(self->spi, &transaction);

        if (dest != NULL) {
            memcpy(dest, &transaction.rx_data, len);
        }
    } else {
        int offset = 0;
        int bits_remaining = bits_to_send;
        int optimum_word_size = 8 * self->bits / gcd(8, self->bits);
        int max_transaction_bits = MP_HW_SPI_MAX_XFER_BITS / optimum_word_size * optimum_word_size;
        spi_transaction_t *transaction, *result, transactions[2];
        int i = 0;

        spi_device_acquire_bus(self->spi, portMAX_DELAY);

        while (bits_remaining) {
            transaction = transactions + i++ % 2;
            memset(transaction, 0, sizeof(spi_transaction_t));

            transaction->length =
                bits_remaining > max_transaction_bits ? max_transaction_bits : bits_remaining;

            if (src != NULL) {
                transaction->tx_buffer = src + offset;
            }
            if (dest != NULL) {
                transaction->rx_buffer = dest + offset;
            }

            spi_device_queue_trans(self->spi, transaction, portMAX_DELAY);
            bits_remaining -= transaction->length;

            if (offset > 0) {
                // wait for previously queued transaction
                MP_THREAD_GIL_EXIT();
                spi_device_get_trans_result(self->spi, &result, portMAX_DELAY);
                MP_THREAD_GIL_ENTER();
            }

            // doesn't need ceil(); loop ends when bits_remaining is 0
            offset += transaction->length / 8;
        }

        // wait for last transaction
        MP_THREAD_GIL_EXIT();
        spi_device_get_trans_result(self->spi, &result, portMAX_DELAY);
        MP_THREAD_GIL_ENTER();
        spi_device_release_bus(self->spi);
    }
}

/******************************************************************************/
// MicroPython bindings for hw_spi

static void machine_hw_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hw_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPI(id=%u, baudrate=%u, polarity=%u, phase=%u, bits=%u, firstbit=%u, sck=%d, mosi=%d, miso=%d)",
        self->host, self->baudrate, self->polarity,
        self->phase, self->bits, self->firstbit,
        self->sck, self->mosi, self->miso);
}

// Take an arg list made from spi_allowed_args, and put in default or "keep same" values
// into all the u_int fields.
// The behavior is slightly different for a new call vs an init method on an existing object.
// Unspecified arguments for new will use defaults, for init they keep the existing value.
static void machine_hw_spi_argcheck(mp_arg_val_t args[], const machine_hw_spi_default_pins_t *default_pins) {
// A non-NULL default_pins argument will trigger the "use default" behavior.
    // Replace pin args with default/current values for new vs init call, respectively
    for (int i = ARG_sck; i <= ARG_miso; i++) {
        if (args[i].u_obj == MP_OBJ_NULL) {
            args[i].u_int = default_pins ? default_pins->array[i - ARG_sck] : -2;
        } else if (args[i].u_obj == mp_const_none) {
            args[i].u_int = -1;
        } else {
            args[i].u_int = machine_pin_get_id(args[i].u_obj);
        }
    }
}

static void machine_hw_spi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_hw_spi_obj_t *self = (machine_hw_spi_obj_t *)self_in;

    mp_arg_val_t args[MP_ARRAY_SIZE(spi_allowed_args)];
    // offset arg lists by 1 to skip first arg, id, which is not valid for init()
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(spi_allowed_args) - 1,
        spi_allowed_args + 1, args + 1);

    machine_hw_spi_argcheck(args, NULL);
    machine_hw_spi_init_internal(self, args);
}

mp_obj_t machine_hw_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    MP_MACHINE_SPI_CHECK_FOR_LEGACY_SOFTSPI_CONSTRUCTION(n_args, n_kw, all_args);

    mp_arg_val_t args[MP_ARRAY_SIZE(spi_allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(spi_allowed_args), spi_allowed_args, args);

    const mp_int_t spi_id = args[ARG_id].u_int;
    if (1 <= spi_id && spi_id <= MICROPY_HW_SPI_MAX) {
        machine_hw_spi_argcheck(args, &machine_hw_spi_default_pins[spi_id - 1]);
    } else {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SPI(%d) doesn't exist"), spi_id);
    }
    // Replace -1 non-pin args with default values
    static const mp_int_t defaults[] = { 500000, 0, 0, 8, MICROPY_PY_MACHINE_SPI_MSB };
    for (int i = ARG_baudrate; i <= ARG_firstbit; i++) {
        if (args[i].u_int == -1) {
            args[i].u_int = defaults[i - ARG_baudrate];
        }
    }

    machine_hw_spi_obj_t *self = &machine_hw_spi_obj[spi_id - 1];
    self->host = spi_id;

    self->base.type = &machine_spi_type;

    machine_hw_spi_init_internal(self, args);

    return MP_OBJ_FROM_PTR(self);
}

spi_host_device_t machine_hw_spi_get_host(mp_obj_t in) {
    if (mp_obj_get_type(in) != &machine_spi_type) {
        mp_raise_ValueError(MP_ERROR_TEXT("expecting a SPI object"));
    }
    machine_hw_spi_obj_t *self = (machine_hw_spi_obj_t *)in;
    return self->host;
}

static const mp_machine_spi_p_t machine_hw_spi_p = {
    .init = machine_hw_spi_init,
    .deinit = machine_hw_spi_deinit,
    .transfer = machine_hw_spi_transfer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, machine_hw_spi_make_new,
    print, machine_hw_spi_print,
    protocol, &machine_hw_spi_p,
    locals_dict, &mp_machine_spi_locals_dict
    );

/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024-2025 OpenMV LLC.
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

#if MICROPY_PY_NETWORK_CYW43

#include "lib/cyw43-driver/src/cyw43.h"
#include "lib/cyw43-driver/src/cyw43_internal.h"
#include "lib/cyw43-driver/src/cyw43_spi.h"

#include "spi.h"
#include "sys_ctrl_spi.h"

// CYW43 is connected to SPI3.
#define HW_SPI_UNIT (3)
#define HW_SPI ((SPI_Type *)SPI3_BASE)
#define SPI_BAUDRATE (16000000)
#define SPI_RX_FIFO_SIZE (16)

// WL_IRQ is on P9_6.
#define WL_IRQ_IRQN (GPIO9_IRQ6_IRQn)
#define WL_IRQ_HANDLER GPIO9_IRQ6Handler

// Must run at IRQ priority above PendSV so it can wake cyw43-driver when PendSV is disabled.
void WL_IRQ_HANDLER(void) {
    if (gpio_read_int_rawstatus(pin_WL_IRQ->gpio, pin_WL_IRQ->pin)) {
        gpio_interrupt_eoi(pin_WL_IRQ->gpio, pin_WL_IRQ->pin);
        pendsv_schedule_dispatch(PENDSV_DISPATCH_CYW43, cyw43_poll);
        __SEV();
    }
}

static void spi_bus_init(void) {
    // Configure pins.
    mp_hal_pin_output(pin_WL_CS);
    mp_hal_pin_high(pin_WL_CS);
    // NOTE: Alif recommends enabled input read for all SPI pins.
    mp_hal_pin_config(pin_WL_SCLK, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE,
        MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_8MA, MP_HAL_PIN_ALT(SPI_SCLK, HW_SPI_UNIT), true);
    mp_hal_pin_config(pin_WL_MOSI, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE,
        MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_8MA, MP_HAL_PIN_ALT(SPI_MOSI, HW_SPI_UNIT), true);
    mp_hal_pin_config(pin_WL_MISO, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE,
        MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_8MA, MP_HAL_PIN_ALT(SPI_MISO, HW_SPI_UNIT), true);

    // Starts out clock_polarity=1, clock_phase=0.
    spi_mode_master(HW_SPI);
    spi_set_bus_speed(HW_SPI, SPI_BAUDRATE, GetSystemAHBClock());
    spi_set_mode(HW_SPI, SPI_MODE_2);
    spi_set_protocol(HW_SPI, SPI_PROTO_SPI);
    spi_set_dfs(HW_SPI, 8);
    spi_set_tmod(HW_SPI, SPI_TMOD_TX_AND_RX);
    spi_control_ss(HW_SPI, 0, SPI_SS_STATE_ENABLE);
}

int cyw43_spi_init(cyw43_int_t *self) {
    spi_bus_init();

    // Configure IRQ for WL_IRQ (active low input).
    NVIC_SetPriority(WL_IRQ_IRQN, IRQ_PRI_CYW43);
    NVIC_ClearPendingIRQ(WL_IRQ_IRQN);
    NVIC_EnableIRQ(WL_IRQ_IRQN);

    return 0;
}

void cyw43_spi_deinit(cyw43_int_t *self) {
    // Disable clock, SS and SPI.
    spi_mask_interrupts(HW_SPI);
    spi_control_ss(HW_SPI, 0, 0);
    spi_disable(HW_SPI);

    // Disable SPI IRQ.
    NVIC_DisableIRQ(WL_IRQ_IRQN);
    NVIC_ClearPendingIRQ(WL_IRQ_IRQN);
}

void cyw43_spi_gpio_setup(void) {
}

void cyw43_spi_reset(void) {
}

void cyw43_spi_set_polarity(cyw43_int_t *self, int pol) {
    (void)self;

    if (pol == 0) {
        spi_set_mode(HW_SPI, SPI_MODE_0);
    } else {
        spi_set_mode(HW_SPI, SPI_MODE_2);
    }
}

// tx must not be NULL.
// rx_len must be 0, or the same as tx_len.
int cyw43_spi_transfer(cyw43_int_t *self, const uint8_t *tx, size_t tx_len, uint8_t *rx, size_t rx_len) {
    (void)self;

    if (tx_len == 0 && rx_len == 0) {
        return 0;
    }

    mp_hal_pin_low(pin_WL_CS);

    // Must read the same amount of data that is written out to SPI.
    rx_len = tx_len;

    while (tx_len || rx_len) {
        // Only add data to the TX FIFO if:
        // - there's data to add
        // - and TX is not ahead of RX by more than the RX FIFO size
        // - and there's space in the TX FIFO
        if (tx_len && tx_len + SPI_RX_FIFO_SIZE > rx_len && (HW_SPI->SPI_SR & SPI_SR_TFNF) != 0) {
            HW_SPI->SPI_DR[0] = *tx++;
            --tx_len;
        }

        // Take data from the RX FIFO and store it into the output buffer (if given).
        if (rx_len && (HW_SPI->SPI_SR & SPI_SR_RFNE) != 0) {
            uint8_t data = HW_SPI->SPI_DR[0];
            if (rx != NULL) {
                *rx++ = data;
            }
            --rx_len;
        }
    }

    mp_hal_pin_high(pin_WL_CS);

    return 0;
}

#endif

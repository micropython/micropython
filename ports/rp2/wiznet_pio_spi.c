/*
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/error.h"

#include "hardware/dma.h"
#include "hardware/clocks.h"

#include "wiznet_pio_spi.h"
#include "wiznet_pio_spi.pio.h"

#ifndef PIO_SPI_PREFERRED_PIO
#define PIO_SPI_PREFERRED_PIO 1
#endif

#define PADS_DRIVE_STRENGTH PADS_BANK0_GPIO0_DRIVE_VALUE_12MA
#define IRQ_SAMPLE_DELAY_NS 100

#define WIZNET_PIO_SPI_PROGRAM_NAME wiznet_pio_spi_write_read
#define WIZNET_PIO_SPI_PROGRAM_FUNC __CONCAT(WIZNET_PIO_SPI_PROGRAM_NAME, _program)
#define WIZNET_PIO_SPI_PROGRAM_GET_DEFAULT_CONFIG_FUNC __CONCAT(WIZNET_PIO_SPI_PROGRAM_NAME, _program_get_default_config)
#define WIZNET_PIO_SPI_OFFSET_WRITE_BITS __CONCAT(WIZNET_PIO_SPI_PROGRAM_NAME, _offset_write_bits)
#define WIZNET_PIO_SPI_OFFSET_WRITE_END __CONCAT(WIZNET_PIO_SPI_PROGRAM_NAME, _offset_write_end)
#define WIZNET_PIO_SPI_OFFSET_READ_END __CONCAT(WIZNET_PIO_SPI_PROGRAM_NAME, _offset_read_end)

// All wiznet spi operations must start with writing a 3 byte header
#define WIZNET_PIO_SPI_HEADER_LEN 3

#ifndef WIZNET_PICO_PIO_SPI_INSTANCE_COUNT
#define WIZNET_PICO_PIO_SPI_INSTANCE_COUNT 1
#endif

typedef struct wiznet_pio_spi_state {
    wiznet_pio_spi_funcs_t *funcs;
    const wiznet_pio_spi_config_t *spi_config;
    pio_hw_t *pio;
    uint8_t pio_func_sel;
    int8_t pio_offset;
    int8_t pio_sm;
    int8_t dma_out;
    int8_t dma_in;
    uint8_t spi_header[WIZNET_PIO_SPI_HEADER_LEN];
    uint8_t spi_header_count;
} wiznet_pio_spi_state_t;
static wiznet_pio_spi_state_t wiznet_pio_spi_state[WIZNET_PICO_PIO_SPI_INSTANCE_COUNT];
static wiznet_pio_spi_state_t *active_state;

static wiznet_pio_spi_funcs_t *get_wiznet_pio_spi_impl(void);

// Initialise our gpios
static void wiznet_pio_spi_gpio_setup(wiznet_pio_spi_state_t *state) {

    // Setup MOSI, MISO and IRQ
    gpio_init(state->spi_config->data_out_pin);
    gpio_set_dir(state->spi_config->data_out_pin, GPIO_OUT);
    gpio_put(state->spi_config->data_out_pin, false);

    // Setup CS
    gpio_init(state->spi_config->cs_pin);
    gpio_set_dir(state->spi_config->cs_pin, GPIO_OUT);
    gpio_put(state->spi_config->cs_pin, true);

    // Setup IRQ
    gpio_init(state->spi_config->irq_pin);
    gpio_set_dir(state->spi_config->irq_pin, GPIO_IN);
    gpio_set_pulls(state->spi_config->irq_pin, false, false);
}

wiznet_pio_spi_handle_t wiznet_pio_spi_open(const wiznet_pio_spi_config_t *wiznet_pio_spi_config) {
    wiznet_pio_spi_state_t *state;
    for (int i = 0; i < count_of(wiznet_pio_spi_state); i++) {
        if (!wiznet_pio_spi_state[i].funcs) {
            state = &wiznet_pio_spi_state[i];
            break;
        }
    }
    assert(state);
    // if (!state) return NULL;
    state->spi_config = wiznet_pio_spi_config;
    state->funcs = get_wiznet_pio_spi_impl();

    wiznet_pio_spi_gpio_setup(state);

    pio_hw_t *pios[2] = {pio0, pio1};
    uint pio_index = PIO_SPI_PREFERRED_PIO;

    if (!pio_can_add_program(pios[pio_index], &WIZNET_PIO_SPI_PROGRAM_FUNC)) {
        pio_index ^= 1;
        if (!pio_can_add_program(pios[pio_index], &WIZNET_PIO_SPI_PROGRAM_FUNC)) {
            return NULL;
        }
    }

    state->pio = pios[pio_index];
    state->dma_in = -1;
    state->dma_out = -1;

    static_assert(GPIO_FUNC_PIO1 == GPIO_FUNC_PIO0 + 1, "");
    state->pio_func_sel = GPIO_FUNC_PIO0 + pio_index;
    state->pio_sm = (int8_t)pio_claim_unused_sm(state->pio, false);
    if (state->pio_sm < 0) {
        wiznet_pio_spi_close(&state->funcs);
        return NULL;
    }

    state->pio_offset = pio_add_program(state->pio, &WIZNET_PIO_SPI_PROGRAM_FUNC);

    pio_sm_config sm_config = WIZNET_PIO_SPI_PROGRAM_GET_DEFAULT_CONFIG_FUNC(state->pio_offset);

    sm_config_set_clkdiv_int_frac(&sm_config, state->spi_config->clock_div_major, state->spi_config->clock_div_minor);
    hw_write_masked(&pads_bank0_hw->io[state->spi_config->clock_pin],
        (uint)PADS_DRIVE_STRENGTH << PADS_BANK0_GPIO0_DRIVE_LSB,
            PADS_BANK0_GPIO0_DRIVE_BITS
        );
    hw_write_masked(&pads_bank0_hw->io[state->spi_config->clock_pin],
        (uint)1 << PADS_BANK0_GPIO0_SLEWFAST_LSB,
            PADS_BANK0_GPIO0_SLEWFAST_BITS
        );

    sm_config_set_out_pins(&sm_config, state->spi_config->data_out_pin, 1);
    sm_config_set_in_pins(&sm_config, state->spi_config->data_in_pin);
    sm_config_set_set_pins(&sm_config, state->spi_config->data_out_pin, 1);
    sm_config_set_sideset(&sm_config, 1, false, false);
    sm_config_set_sideset_pins(&sm_config, state->spi_config->clock_pin);

    sm_config_set_in_shift(&sm_config, false, true, 8);
    sm_config_set_out_shift(&sm_config, false, true, 8);
    hw_set_bits(&state->pio->input_sync_bypass, 1u << state->spi_config->data_in_pin);
    pio_sm_set_config(state->pio, state->pio_sm, &sm_config);
    pio_sm_set_consecutive_pindirs(state->pio, state->pio_sm, state->spi_config->clock_pin, 1, true);
    gpio_set_function(state->spi_config->data_out_pin, state->pio_func_sel);
    gpio_set_function(state->spi_config->clock_pin, state->pio_func_sel);

    // Set data pin to pull down and schmitt
    gpio_set_pulls(state->spi_config->data_in_pin, false, true);
    gpio_set_input_hysteresis_enabled(state->spi_config->data_in_pin, true);

    pio_sm_exec(state->pio, state->pio_sm, pio_encode_set(pio_pins, 1));

    state->dma_out = (int8_t)dma_claim_unused_channel(false); // todo: Should be able to use one dma channel?
    state->dma_in = (int8_t)dma_claim_unused_channel(false);
    if (state->dma_out < 0 || state->dma_in < 0) {
        wiznet_pio_spi_close(&state->funcs);
        return NULL;
    }
    return &state->funcs;
}

void wiznet_pio_spi_close(wiznet_pio_spi_handle_t handle) {
    wiznet_pio_spi_state_t *state = (wiznet_pio_spi_state_t *)handle;
    if (state) {
        if (state->pio_sm >= 0) {
            if (state->pio_offset != -1) {
                pio_remove_program(state->pio, &WIZNET_PIO_SPI_PROGRAM_FUNC, state->pio_offset);
            }

            pio_sm_unclaim(state->pio, state->pio_sm);
        }
        if (state->dma_out >= 0) {
            dma_channel_unclaim(state->dma_out);
            state->dma_out = -1;
        }
        if (state->dma_in >= 0) {
            dma_channel_unclaim(state->dma_in);
            state->dma_in = -1;
        }
        state->funcs = NULL;
    }
}

static void cs_set(wiznet_pio_spi_state_t *state, bool value) {
    gpio_put(state->spi_config->cs_pin, value);
}

static __noinline void ns_delay(uint32_t ns) {
    // cycles = ns * clk_sys_hz / 1,000,000,000
    uint32_t cycles = ns * (clock_get_hz(clk_sys) >> 16u) / (1000000000u >> 16u);
    busy_wait_at_least_cycles(cycles);
}

static void wiznet_pio_spi_frame_start(void) {
    assert(active_state);

    gpio_set_function(active_state->spi_config->data_out_pin, active_state->pio_func_sel);
    gpio_set_function(active_state->spi_config->clock_pin, active_state->pio_func_sel);
    gpio_pull_down(active_state->spi_config->clock_pin);

    // Pull CS low
    cs_set(active_state, false);
}

static void wiznet_pio_spi_frame_end(void) {
    assert(active_state);

    // from this point a positive edge will cause an IRQ to be pending
    cs_set(active_state, true);

    // we need to wait a bit in case the irq line is incorrectly high
    #ifdef IRQ_SAMPLE_DELAY_NS
    ns_delay(IRQ_SAMPLE_DELAY_NS);
    #endif
}

// send tx then receive rx
// rx can be null if you just want to send, but tx and tx_length must be valid
bool wiznet_pio_spi_transfer(const uint8_t *tx, size_t tx_length, uint8_t *rx, size_t rx_length) {
    const wiznet_pio_spi_state_t *state = active_state;
    if (!state || (tx == NULL)) {
        return false;
    }

    if (rx != NULL && tx != NULL) {
        assert(tx && tx_length && rx_length);

        pio_sm_set_enabled(state->pio, state->pio_sm, false); // disable sm
        pio_sm_set_wrap(state->pio, state->pio_sm, state->pio_offset + WIZNET_PIO_SPI_OFFSET_WRITE_BITS, state->pio_offset + WIZNET_PIO_SPI_OFFSET_READ_END - 1);
        pio_sm_clear_fifos(state->pio, state->pio_sm); // clear fifos from previous run
        pio_sm_set_pindirs_with_mask(state->pio, state->pio_sm, 1u << state->spi_config->data_out_pin, 1u << state->spi_config->data_out_pin);
        pio_sm_restart(state->pio, state->pio_sm);
        pio_sm_clkdiv_restart(state->pio, state->pio_sm);
        pio_sm_put(state->pio, state->pio_sm, tx_length * 8 - 1); // set x
        pio_sm_exec(state->pio, state->pio_sm, pio_encode_out(pio_x, 32));
        pio_sm_put(state->pio, state->pio_sm, rx_length - 1); // set y
        pio_sm_exec(state->pio, state->pio_sm, pio_encode_out(pio_y, 32));
        pio_sm_exec(state->pio, state->pio_sm, pio_encode_jmp(state->pio_offset)); // setup pc
        dma_channel_abort(state->dma_out);
        dma_channel_abort(state->dma_in);

        dma_channel_config out_config = dma_channel_get_default_config(state->dma_out);
        channel_config_set_dreq(&out_config, pio_get_dreq(state->pio, state->pio_sm, true));
        channel_config_set_transfer_data_size(&out_config, DMA_SIZE_8);
        dma_channel_configure(state->dma_out, &out_config, &state->pio->txf[state->pio_sm], tx, tx_length, true);

        dma_channel_config in_config = dma_channel_get_default_config(state->dma_in);
        channel_config_set_dreq(&in_config, pio_get_dreq(state->pio, state->pio_sm, false));
        channel_config_set_write_increment(&in_config, true);
        channel_config_set_read_increment(&in_config, false);
        channel_config_set_transfer_data_size(&in_config, DMA_SIZE_8);
        dma_channel_configure(state->dma_in, &in_config, rx, &state->pio->rxf[state->pio_sm], rx_length, true);

        pio_sm_set_enabled(state->pio, state->pio_sm, true);
        __compiler_memory_barrier();

        dma_channel_wait_for_finish_blocking(state->dma_out);
        dma_channel_wait_for_finish_blocking(state->dma_in);

        __compiler_memory_barrier();
    } else if (tx != NULL) {
        assert(tx_length);

        pio_sm_set_enabled(state->pio, state->pio_sm, false);
        pio_sm_set_wrap(state->pio, state->pio_sm, state->pio_offset + WIZNET_PIO_SPI_OFFSET_WRITE_BITS, state->pio_offset + WIZNET_PIO_SPI_OFFSET_WRITE_END - 1);
        pio_sm_clear_fifos(state->pio, state->pio_sm);
        pio_sm_restart(state->pio, state->pio_sm);
        pio_sm_clkdiv_restart(state->pio, state->pio_sm);
        pio_sm_put(state->pio, state->pio_sm, tx_length * 8 - 1);
        pio_sm_exec(state->pio, state->pio_sm, pio_encode_out(pio_x, 32));
        pio_sm_put(state->pio, state->pio_sm, tx_length - 1);
        pio_sm_exec(state->pio, state->pio_sm, pio_encode_out(pio_y, 32));
        pio_sm_exec(state->pio, state->pio_sm, pio_encode_set(pio_pins, 0));
        pio_sm_set_consecutive_pindirs(state->pio, state->pio_sm, state->spi_config->data_out_pin, 1, true);
        pio_sm_exec(state->pio, state->pio_sm, pio_encode_jmp(state->pio_offset + WIZNET_PIO_SPI_OFFSET_WRITE_BITS));
        dma_channel_abort(state->dma_out);

        dma_channel_config out_config = dma_channel_get_default_config(state->dma_out);
        channel_config_set_dreq(&out_config, pio_get_dreq(state->pio, state->pio_sm, true));

        channel_config_set_transfer_data_size(&out_config, DMA_SIZE_8);
        dma_channel_configure(state->dma_out, &out_config, &state->pio->txf[state->pio_sm], tx, tx_length, true);

        const uint32_t fDebugTxStall = 1u << (PIO_FDEBUG_TXSTALL_LSB + state->pio_sm);
        state->pio->fdebug = fDebugTxStall;
        pio_sm_set_enabled(state->pio, state->pio_sm, true);
        while (!(state->pio->fdebug & fDebugTxStall)) {
            tight_loop_contents(); // todo timeout
        }
        __compiler_memory_barrier();
        pio_sm_set_enabled(state->pio, state->pio_sm, false);
        pio_sm_set_consecutive_pindirs(state->pio, state->pio_sm, state->spi_config->data_in_pin, 1, false);
    } else if (rx != NULL) {
        panic_unsupported(); // shouldn't be used
    }
    pio_sm_exec(state->pio, state->pio_sm, pio_encode_mov(pio_pins, pio_null)); // for next time we turn output on

    return true;
}

// To read a byte we must first have been asked to write a 3 byte spi header
static uint8_t wiznet_pio_spi_read_byte(void) {
    assert(active_state);
    assert(active_state->spi_header_count == WIZNET_PIO_SPI_HEADER_LEN);
    uint8_t ret;
    if (!wiznet_pio_spi_transfer(active_state->spi_header, active_state->spi_header_count, &ret, 1)) {
        panic("spi failed read");
    }
    active_state->spi_header_count = 0;
    return ret;
}

// This is not used when the burst functions are provided
static void wiznet_pio_spi_write_byte(uint8_t wb) {
    panic_unsupported(); // shouldn't be used
}

// To read a buffer we must first have been asked to write a 3 byte spi header
void wiznet_pio_spi_read_buffer(uint8_t *pBuf, uint16_t len) {
    assert(active_state);
    assert(active_state->spi_header_count == WIZNET_PIO_SPI_HEADER_LEN);
    if (!wiznet_pio_spi_transfer(active_state->spi_header, active_state->spi_header_count, pBuf, len)) {
        panic("spi failed reading buffer");
    }
    active_state->spi_header_count = 0;
}

// If we have been asked to write a spi header already, then write it and the rest of the buffer
// or else if we've been given enough data for just the spi header, save it until the next call
// or we're writing a byte in which case we're given a buffer including the spi header
void wiznet_pio_spi_write_buffer(const uint8_t *pBuf, uint16_t len) {
    assert(active_state);
    if (len == WIZNET_PIO_SPI_HEADER_LEN && active_state->spi_header_count == 0) {
        memcpy(active_state->spi_header, pBuf, WIZNET_PIO_SPI_HEADER_LEN); // expect another call
        active_state->spi_header_count = WIZNET_PIO_SPI_HEADER_LEN;
    } else {
        if (active_state->spi_header_count == WIZNET_PIO_SPI_HEADER_LEN) {
            if (!wiznet_pio_spi_transfer(active_state->spi_header, WIZNET_PIO_SPI_HEADER_LEN, NULL, 0)) {
                panic("spi failed writing header");
            }
            active_state->spi_header_count = 0;
        }
        assert(active_state->spi_header_count == 0);
        if (!wiznet_pio_spi_transfer(pBuf, len, NULL, 0)) {
            panic("spi failed writing buffer");
        }
    }
}

static void wiznet_pio_spi_set_active(wiznet_pio_spi_handle_t handle) {
    active_state = (wiznet_pio_spi_state_t *)handle;
}

static void wiznet_pio_spi_set_inactive(void) {
    active_state = NULL;
}

static void wiznet_pio_spi_reset(wiznet_pio_spi_handle_t handle) {
    wiznet_pio_spi_state_t *state = (wiznet_pio_spi_state_t *)handle;
    gpio_set_dir(state->spi_config->reset_pin, GPIO_OUT);
    gpio_put(state->spi_config->reset_pin, 0);
    sleep_ms(100);
    gpio_put(state->spi_config->reset_pin, 1);
    sleep_ms(100);
}

static wiznet_pio_spi_funcs_t *get_wiznet_pio_spi_impl(void) {
    static wiznet_pio_spi_funcs_t funcs = {
        .close = wiznet_pio_spi_close,
        .set_active = wiznet_pio_spi_set_active,
        .set_inactive = wiznet_pio_spi_set_inactive,
        .frame_start = wiznet_pio_spi_frame_start,
        .frame_end = wiznet_pio_spi_frame_end,
        .read_byte = wiznet_pio_spi_read_byte,
        .write_byte = wiznet_pio_spi_write_byte,
        .read_buffer = wiznet_pio_spi_read_buffer,
        .write_buffer = wiznet_pio_spi_write_buffer,
        .reset = wiznet_pio_spi_reset,
    };
    return &funcs;
}

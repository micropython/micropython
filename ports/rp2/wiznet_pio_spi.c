/*
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if MICROPY_WIZNET_PIO

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/error.h"

#include "hardware/dma.h"
#include "hardware/clocks.h"

#include "lib/wiznet6k/Ethernet/wizchip_conf.h"
#include "wiznet_pio_spi.h"

#include "wiznet_pio_spi.pio.h"

#include "py/runtime.h"
#include "py/mphal.h"

#ifndef PIO_SPI_PREFERRED_PIO
#define PIO_SPI_PREFERRED_PIO 1
#endif

#define PADS_DRIVE_STRENGTH PADS_BANK0_GPIO0_DRIVE_VALUE_12MA
#define IRQ_SAMPLE_DELAY_NS 100

#if   (_WIZCHIP_ == W6300)
    #if (_WIZCHIP_QSPI_MODE_ == QSPI_SINGLE_MODE)
    #define PIO_PROGRAM_NAME wizchip_pio_spi_single_write_read
    #elif (_WIZCHIP_QSPI_MODE_ == QSPI_DUAL_MODE)
    #define PIO_PROGRAM_NAME wizchip_pio_spi_dual_write_read
    #elif (_WIZCHIP_QSPI_MODE_ == QSPI_QUAD_MODE)
    #define PIO_PROGRAM_NAME wizchip_pio_spi_quad_write_read
    #endif

    #define PIO_PROGRAM_FUNC __CONCAT(PIO_PROGRAM_NAME, _program)
    #define PIO_PROGRAM_GET_DEFAULT_CONFIG_FUNC __CONCAT(PIO_PROGRAM_NAME, _program_get_default_config)
    #define PIO_OFFSET_WRITE_BITS __CONCAT(PIO_PROGRAM_NAME, _offset_write_bits)
    #define PIO_OFFSET_WRITE_BITS_END __CONCAT(PIO_PROGRAM_NAME, _offset_write_bits_end)
    #define PIO_OFFSET_READ_BITS_END __CONCAT(PIO_PROGRAM_NAME, _offset_read_bits_end)
#else
    #define PIO_PROGRAM_NAME wiznet_spi_write_read
    #define PIO_PROGRAM_FUNC __CONCAT(PIO_PROGRAM_NAME, _program)
    #define PIO_PROGRAM_GET_DEFAULT_CONFIG_FUNC __CONCAT(PIO_PROGRAM_NAME, _program_get_default_config)
    #define PIO_OFFSET_WRITE_BITS __CONCAT(PIO_PROGRAM_NAME, _offset_write_bits)
    #define PIO_OFFSET_WRITE_BITS_END __CONCAT(PIO_PROGRAM_NAME, _offset_write_end)
    #define PIO_OFFSET_READ_BITS_END __CONCAT(PIO_PROGRAM_NAME, _offset_read_end)
// All wiznet spi operations must start with writing a 3 byte header
#endif

#ifndef PICO_WIZNET_SPI_PIO_INSTANCE_COUNT
#define PICO_WIZNET_SPI_PIO_INSTANCE_COUNT 1
#endif

#define SPI_HEADER_LEN 3

typedef struct spi_pio_state {
    wiznet_pio_spi_funcs_t *funcs;
    const wiznet_pio_spi_config_t *spi_config;
    pio_hw_t *pio;
    uint8_t pio_func_sel;
    int8_t pio_offset;
    int8_t pio_sm;
    int8_t dma_out;
    int8_t dma_in;
    uint8_t spi_header[SPI_HEADER_LEN];
    uint8_t spi_header_count;
} spi_pio_state_t;

static spi_pio_state_t spi_pio_state[PICO_WIZNET_SPI_PIO_INSTANCE_COUNT];
static spi_pio_state_t *active_state;

// static void wiznet_pio_spi_close(wiznet_pio_spi_handle_t funcs);
static wiznet_pio_spi_funcs_t *get_wiznet_pio_spi_impl(void);


static uint16_t mk_cmd_buf(uint8_t *pdst, uint8_t opcode, uint16_t addr) {
    #if (_WIZCHIP_QSPI_MODE_ == QSPI_SINGLE_MODE)

    pdst[0] = opcode;
    pdst[1] = (uint8_t)((addr >> 8) & 0xFF);
    pdst[2] = (uint8_t)((addr >> 0) & 0xFF);
    pdst[3] = 0;

    return 3 + 1;
    #elif (_WIZCHIP_QSPI_MODE_ == QSPI_DUAL_MODE)
    pdst[0] = ((opcode >> 7 & 0x01) << 6) | ((opcode >> 6 & 0x01) << 4) | ((opcode >> 5 & 0x01) << 2) | ((opcode >> 4 & 0x01) << 0);
    pdst[1] = ((opcode >> 3 & 0x01) << 6) | ((opcode >> 2 & 0x01) << 4) | ((opcode >> 1 & 0x01) << 2) | ((opcode >> 0 & 0x01) << 0);
    pdst[2] = (uint8_t)((addr >> 8) & 0xFF);
    pdst[3] = (uint8_t)((addr >> 0) & 0xFF);

    pdst[4] = 0;

    return 4 + 1;
    #elif (_WIZCHIP_QSPI_MODE_ == QSPI_QUAD_MODE)
    pdst[0] = ((opcode >> 7 & 0x01) << 4) | ((opcode >> 6 & 0x01) << 0);
    pdst[1] = ((opcode >> 5 & 0x01) << 4) | ((opcode >> 4 & 0x01) << 0);
    pdst[2] = ((opcode >> 3 & 0x01) << 4) | ((opcode >> 2 & 0x01) << 0);
    pdst[3] = ((opcode >> 1 & 0x01) << 4) | ((opcode >> 0 & 0x01) << 0);

    pdst[4] = ((uint8_t)(addr >> 8) & 0xFF);
    pdst[5] = ((uint8_t)(addr >> 0) & 0xFF);

    pdst[6] = 0;

    return 6 + 1;
    #endif
    return 0;
}

// Initialise our gpios
static void pio_spi_gpio_setup(spi_pio_state_t *state) {

    #if   (_WIZCHIP_ == W6300)
    // Setup CS
    gpio_init(state->spi_config->cs_pin);
    gpio_set_dir(state->spi_config->cs_pin, GPIO_OUT);
    gpio_put(state->spi_config->cs_pin, true);

    if (state->spi_config->irq_pin != 0xFF) {
        gpio_init(state->spi_config->irq_pin);
        gpio_set_dir(state->spi_config->irq_pin, GPIO_IN);
        gpio_set_pulls(state->spi_config->irq_pin, false, false);
    }

    #if (_WIZCHIP_QSPI_MODE_ == QSPI_SINGLE_MODE)
    // Setup DO and DI
    gpio_init(state->spi_config->data_io0_pin);
    gpio_init(state->spi_config->data_io1_pin);
    gpio_set_dir(state->spi_config->data_io0_pin, GPIO_OUT);
    gpio_set_dir(state->spi_config->data_io1_pin, GPIO_OUT);
    gpio_put(state->spi_config->data_io0_pin, false);
    gpio_put(state->spi_config->data_io1_pin, false);
    #elif (_WIZCHIP_QSPI_MODE_ == QSPI_DUAL_MODE)
    // Setup DO and DI
    gpio_init(state->spi_config->data_io0_pin);
    gpio_init(state->spi_config->data_io1_pin);
    gpio_set_dir(state->spi_config->data_io0_pin, GPIO_OUT);
    gpio_set_dir(state->spi_config->data_io1_pin, GPIO_OUT);
    gpio_put(state->spi_config->data_io0_pin, false);
    gpio_put(state->spi_config->data_io1_pin, false);
    #elif (_WIZCHIP_QSPI_MODE_ == QSPI_QUAD_MODE)
    // Setup DO and DI
    gpio_init(state->spi_config->data_io0_pin);
    gpio_init(state->spi_config->data_io1_pin);
    gpio_init(state->spi_config->data_io2_pin);
    gpio_init(state->spi_config->data_io3_pin);
    gpio_set_dir(state->spi_config->data_io0_pin, GPIO_OUT);
    gpio_set_dir(state->spi_config->data_io1_pin, GPIO_OUT);
    gpio_set_dir(state->spi_config->data_io2_pin, GPIO_OUT);
    gpio_set_dir(state->spi_config->data_io3_pin, GPIO_OUT);
    gpio_put(state->spi_config->data_io0_pin, false);
    gpio_put(state->spi_config->data_io1_pin, false);
    gpio_put(state->spi_config->data_io2_pin, false);
    gpio_put(state->spi_config->data_io3_pin, false);
    #endif
    #else // W55RP20
    // Setup CS pin as regular GPIO (not PIO controlled)
    gpio_init(state->spi_config->cs_pin);
    gpio_set_dir(state->spi_config->cs_pin, GPIO_OUT);
    gpio_put(state->spi_config->cs_pin, true);

    // Setup MOSI, MISO pins as regular GPIO first, then will set to PIO later
    gpio_init(state->spi_config->data_out_pin);
    gpio_set_dir(state->spi_config->data_out_pin, GPIO_OUT);
    gpio_put(state->spi_config->data_out_pin, false);

    gpio_init(state->spi_config->data_in_pin);
    gpio_set_dir(state->spi_config->data_in_pin, GPIO_IN);
    // Add pull-down on MISO to prevent floating
    gpio_set_pulls(state->spi_config->data_in_pin, false, true);

    // Setup IRQ
    if (state->spi_config->irq_pin != 0xFF) {
        gpio_init(state->spi_config->irq_pin);
        gpio_set_dir(state->spi_config->irq_pin, GPIO_IN);
        gpio_set_pulls(state->spi_config->irq_pin, false, false);
    }
    #endif
}

wiznet_pio_spi_handle_t wiznet_pio_spi_open(const wiznet_pio_spi_config_t *spi_config) {
    spi_pio_state_t *state = NULL;
    for (int i = 0; i < count_of(spi_pio_state); i++) {
        if (!spi_pio_state[i].funcs) {
            state = &spi_pio_state[i];
            break;
        }
    }
    if (!state) {
        return NULL;
    }
    memset(state, 0, sizeof(*state));
    state->spi_config = spi_config;
    state->funcs = get_wiznet_pio_spi_impl();

    pio_spi_gpio_setup(state);

    pio_hw_t *pios[2] = {pio0, pio1};
    uint pio_index = PIO_SPI_PREFERRED_PIO;

    if (!pio_can_add_program(pios[pio_index], &PIO_PROGRAM_FUNC)) {
        pio_index ^= 1;
        if (!pio_can_add_program(pios[pio_index], &PIO_PROGRAM_FUNC)) {
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

    state->pio_offset = pio_add_program(state->pio, &PIO_PROGRAM_FUNC);
    // SPI_DEBUG_PRINT("[SPI CLOCK SPEED : %.2lf MHz]\r\n\r\n", 66.5 / (state->spi_config->clock_div_major + ((double)state->spi_config->clock_div_minor / 256)));
    pio_sm_config sm_config = PIO_PROGRAM_GET_DEFAULT_CONFIG_FUNC(state->pio_offset);

    sm_config_set_clkdiv_int_frac(&sm_config, state->spi_config->clock_div_major, state->spi_config->clock_div_minor);
    hw_write_masked(&pads_bank0_hw->io[state->spi_config->clock_pin],
        (uint)PADS_DRIVE_STRENGTH << PADS_BANK0_GPIO0_DRIVE_LSB,
            PADS_BANK0_GPIO0_DRIVE_BITS
        );
    hw_write_masked(&pads_bank0_hw->io[state->spi_config->clock_pin],
        (uint)1 << PADS_BANK0_GPIO0_SLEWFAST_LSB,
            PADS_BANK0_GPIO0_SLEWFAST_BITS
        );

    #if   (_WIZCHIP_ == W6300)
    #if (_WIZCHIP_QSPI_MODE_ == QSPI_SINGLE_MODE)
    printf("\r\n[QSPI SINGLE MODE]\r\n");
    sm_config_set_out_pins(&sm_config, state->spi_config->data_io0_pin, 1);
    sm_config_set_in_pins(&sm_config, state->spi_config->data_io1_pin);
    sm_config_set_set_pins(&sm_config, state->spi_config->data_io0_pin, 2);
    sm_config_set_sideset(&sm_config, 1, false, false);
    sm_config_set_sideset_pins(&sm_config, state->spi_config->clock_pin);

    sm_config_set_in_shift(&sm_config, false, true, 8);
    sm_config_set_out_shift(&sm_config, false, true, 8);

    hw_set_bits(&state->pio->input_sync_bypass,
        (1u << state->spi_config->data_io0_pin) | (1u << state->spi_config->data_io1_pin));
    pio_sm_set_config(state->pio, state->pio_sm, &sm_config);
    pio_sm_set_consecutive_pindirs(state->pio, state->pio_sm, state->spi_config->clock_pin, 1, true);

    gpio_set_function(state->spi_config->data_io0_pin, state->pio_func_sel);

    // Set data pin to pull down and schmitt
    gpio_set_pulls(state->spi_config->data_io0_pin, false, true);
    gpio_set_pulls(state->spi_config->data_io1_pin, false, true);
    gpio_set_input_hysteresis_enabled(state->spi_config->data_io0_pin, true);
    gpio_set_input_hysteresis_enabled(state->spi_config->data_io1_pin, true);
    #elif (_WIZCHIP_QSPI_MODE_ == QSPI_DUAL_MODE)
    printf("[QSPI DUAL MODE]\r\n\r\n");
    sm_config_set_out_pins(&sm_config, state->spi_config->data_io0_pin, 2);
    sm_config_set_in_pins(&sm_config, state->spi_config->data_io0_pin);
    sm_config_set_set_pins(&sm_config, state->spi_config->data_io0_pin, 2);
    sm_config_set_sideset(&sm_config, 1, false, false);
    sm_config_set_sideset_pins(&sm_config, state->spi_config->clock_pin);

    sm_config_set_in_shift(&sm_config, false, true, 8);
    sm_config_set_out_shift(&sm_config, false, true, 8);

    hw_set_bits(&state->pio->input_sync_bypass,
        (1u << state->spi_config->data_io0_pin) | (1u << state->spi_config->data_io1_pin));
    pio_sm_set_config(state->pio, state->pio_sm, &sm_config);
    pio_sm_set_consecutive_pindirs(state->pio, state->pio_sm, state->spi_config->clock_pin, 1, true);

    gpio_set_function(state->spi_config->data_io0_pin, state->pio_func_sel);
    gpio_set_function(state->spi_config->data_io1_pin, state->pio_func_sel);

    // Set data pin to pull down and schmitt
    gpio_set_pulls(state->spi_config->data_io0_pin, false, true);
    gpio_set_pulls(state->spi_config->data_io1_pin, false, true);
    gpio_set_input_hysteresis_enabled(state->spi_config->data_io0_pin, true);
    gpio_set_input_hysteresis_enabled(state->spi_config->data_io1_pin, true);
    #elif (_WIZCHIP_QSPI_MODE_ == QSPI_QUAD_MODE)
    printf("\r\n[QSPI QUAD MODE]\r\n");
    sm_config_set_out_pins(&sm_config, state->spi_config->data_io0_pin, 4);
    sm_config_set_in_pins(&sm_config, state->spi_config->data_io0_pin);
    sm_config_set_set_pins(&sm_config, state->spi_config->data_io0_pin, 4);
    sm_config_set_sideset(&sm_config, 1, false, false);
    sm_config_set_sideset_pins(&sm_config, state->spi_config->clock_pin);

    sm_config_set_in_shift(&sm_config, false, true, 8);
    sm_config_set_out_shift(&sm_config, false, true, 8);

    hw_set_bits(&state->pio->input_sync_bypass,
        (1u << state->spi_config->data_io0_pin) | (1u << state->spi_config->data_io1_pin) | (1u << state->spi_config->data_io2_pin) | (1u << state->spi_config->data_io3_pin));
    pio_sm_set_config(state->pio, state->pio_sm, &sm_config);
    pio_sm_set_consecutive_pindirs(state->pio, state->pio_sm, state->spi_config->clock_pin, 1, true);

    gpio_set_function(state->spi_config->data_io0_pin, state->pio_func_sel);
    gpio_set_function(state->spi_config->data_io1_pin, state->pio_func_sel);
    gpio_set_function(state->spi_config->data_io2_pin, state->pio_func_sel);
    gpio_set_function(state->spi_config->data_io3_pin, state->pio_func_sel);

    // Set data pin to pull down and schmitt
    gpio_set_pulls(state->spi_config->data_io0_pin, false, true);
    gpio_set_pulls(state->spi_config->data_io1_pin, false, true);
    gpio_set_pulls(state->spi_config->data_io2_pin, false, true);
    gpio_set_pulls(state->spi_config->data_io3_pin, false, true);
    gpio_set_input_hysteresis_enabled(state->spi_config->data_io0_pin, true);
    gpio_set_input_hysteresis_enabled(state->spi_config->data_io1_pin, true);
    gpio_set_input_hysteresis_enabled(state->spi_config->data_io2_pin, true);
    gpio_set_input_hysteresis_enabled(state->spi_config->data_io3_pin, true);
    /* @todo: Implement to use. */
    #endif
    #else // W55RP20
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
    gpio_set_function(state->spi_config->data_in_pin, state->pio_func_sel);
    gpio_set_function(state->spi_config->clock_pin, state->pio_func_sel);

// Set data pin to pull down and schmitt
    gpio_set_pulls(state->spi_config->data_in_pin, false, true);
    gpio_set_input_hysteresis_enabled(state->spi_config->data_in_pin, true);
    #endif

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

    spi_pio_state_t *state = (spi_pio_state_t *)handle;
    if (state) {
        if (state->pio_sm >= 0) {
            if (state->pio_offset != -1) {
                pio_remove_program(state->pio, &PIO_PROGRAM_FUNC, state->pio_offset);
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

static void cs_set(spi_pio_state_t *state, bool value) {
    gpio_put(state->spi_config->cs_pin, value);
}

static __noinline void ns_delay(uint32_t ns) {
    // cycles = ns * clk_sys_hz / 1,000,000,000
    uint32_t cycles = ns * (clock_get_hz(clk_sys) >> 16u) / (1000000000u >> 16u);
    busy_wait_at_least_cycles(cycles);
}

static void wiznet_pio_spi_frame_start(void) {
    assert(active_state);
    // Pull CS low
    cs_set(active_state, false);
    #if   (_WIZCHIP_ == W6300)
    #if (_WIZCHIP_QSPI_MODE_ == QSPI_SINGLE_MODE)
    gpio_set_function(active_state->spi_config->data_io0_pin, active_state->pio_func_sel);
    gpio_set_function(active_state->spi_config->data_io1_pin, active_state->pio_func_sel);
    #elif (_WIZCHIP_QSPI_MODE_ == QSPI_DUAL_MODE)
    gpio_set_function(active_state->spi_config->data_io0_pin, active_state->pio_func_sel);
    gpio_set_function(active_state->spi_config->data_io1_pin, active_state->pio_func_sel);
    #elif (_WIZCHIP_QSPI_MODE_ == QSPI_QUAD_MODE)
    gpio_set_function(active_state->spi_config->data_io0_pin, active_state->pio_func_sel);
    gpio_set_function(active_state->spi_config->data_io1_pin, active_state->pio_func_sel);
    gpio_set_function(active_state->spi_config->data_io2_pin, active_state->pio_func_sel);
    gpio_set_function(active_state->spi_config->data_io3_pin, active_state->pio_func_sel);
    /* @todo: Implement to use. */
    #endif
    gpio_set_function(active_state->spi_config->clock_pin, active_state->pio_func_sel);
    gpio_pull_down(active_state->spi_config->clock_pin);
    #else
    gpio_set_function(active_state->spi_config->data_out_pin, active_state->pio_func_sel);
    gpio_set_function(active_state->spi_config->clock_pin, active_state->pio_func_sel);
    gpio_pull_down(active_state->spi_config->clock_pin);
    #endif
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

#if   (_WIZCHIP_ == W6300)
// To read a byte we must first have been asked to write a 3 byte spi header
void wiznet_pio_spi_read_byte(uint8_t op_code, uint16_t AddrSel, uint8_t *rx, uint16_t rx_length) {
    uint8_t command_buf[8] = {0, };
    uint16_t command_len = mk_cmd_buf(command_buf, op_code, AddrSel);
    uint32_t loop_cnt = 0;

    pio_sm_set_enabled(active_state->pio, active_state->pio_sm, false);
    pio_sm_set_wrap(active_state->pio, active_state->pio_sm, active_state->pio_offset, active_state->pio_offset + PIO_OFFSET_READ_BITS_END - 1);
    // pio_sm_set_wrap(active_state->pio, active_state->pio_sm, active_state->pio_offset + PIO_SPI_OFFSET_WRITE_BITS, active_state->pio_offset + PIO_SPI_OFFSET_READ_BITS_END - 1);
    pio_sm_clear_fifos(active_state->pio, active_state->pio_sm);

    #if (_WIZCHIP_QSPI_MODE_ == QSPI_SINGLE_MODE)
    loop_cnt = 8;
    pio_sm_set_pindirs_with_mask(active_state->pio,
        active_state->pio_sm,
        (1u << active_state->spi_config->data_io0_pin), (1u << active_state->spi_config->data_io0_pin));                          // | (1u << active_state->spi_config->data_io1_pin));
    #elif (_WIZCHIP_QSPI_MODE_ == QSPI_DUAL_MODE)
    loop_cnt = 4;
    pio_sm_set_pindirs_with_mask(active_state->pio,
        active_state->pio_sm,
        (1u << active_state->spi_config->data_io0_pin) | (1u << active_state->spi_config->data_io1_pin),
        (1u << active_state->spi_config->data_io0_pin) | (1u << active_state->spi_config->data_io1_pin));
    #elif (_WIZCHIP_QSPI_MODE_ == QSPI_QUAD_MODE)
    loop_cnt = 2;
    pio_sm_set_pindirs_with_mask(active_state->pio,
        active_state->pio_sm,
        (1u << active_state->spi_config->data_io0_pin) | (1u << active_state->spi_config->data_io1_pin) | (1u << active_state->spi_config->data_io2_pin) | (1u << active_state->spi_config->data_io3_pin),
        (1u << active_state->spi_config->data_io0_pin) | (1u << active_state->spi_config->data_io1_pin) | (1u << active_state->spi_config->data_io2_pin) | (1u << active_state->spi_config->data_io3_pin));

    /* @todo: Implement to use. */
    #endif

    pio_sm_restart(active_state->pio, active_state->pio_sm);
    pio_sm_clkdiv_restart(active_state->pio, active_state->pio_sm);

    pio_sm_put(active_state->pio, active_state->pio_sm, command_len * loop_cnt - 1);
    pio_sm_exec(active_state->pio, active_state->pio_sm, pio_encode_out(pio_x, 32));

    pio_sm_put(active_state->pio, active_state->pio_sm, rx_length - 1);
    pio_sm_exec(active_state->pio, active_state->pio_sm, pio_encode_out(pio_y, 32));

    pio_sm_exec(active_state->pio, active_state->pio_sm, pio_encode_jmp(active_state->pio_offset));

    dma_channel_abort(active_state->dma_out);
    dma_channel_abort(active_state->dma_in);

    dma_channel_config out_config = dma_channel_get_default_config(active_state->dma_out);
    channel_config_set_transfer_data_size(&out_config, DMA_SIZE_8);
    channel_config_set_bswap(&out_config, true);
    channel_config_set_dreq(&out_config, pio_get_dreq(active_state->pio, active_state->pio_sm, true));
    dma_channel_configure(active_state->dma_out, &out_config, &active_state->pio->txf[active_state->pio_sm], command_buf, command_len, true);

    dma_channel_config in_config = dma_channel_get_default_config(active_state->dma_in);
    channel_config_set_transfer_data_size(&in_config, DMA_SIZE_8);
    channel_config_set_bswap(&in_config, true);
    channel_config_set_dreq(&in_config, pio_get_dreq(active_state->pio, active_state->pio_sm, false));
    channel_config_set_write_increment(&in_config, true);
    channel_config_set_read_increment(&in_config, false);
    dma_channel_configure(active_state->dma_in, &in_config, rx, &active_state->pio->rxf[active_state->pio_sm], rx_length, true);

    #if 1
    pio_sm_set_enabled(active_state->pio, active_state->pio_sm, true);

    __compiler_memory_barrier();

    dma_channel_wait_for_finish_blocking(active_state->dma_out);
    dma_channel_wait_for_finish_blocking(active_state->dma_in);

    __compiler_memory_barrier();

    pio_sm_set_enabled(active_state->pio, active_state->pio_sm, false);
    pio_sm_exec(active_state->pio, active_state->pio_sm, pio_encode_mov(pio_pins, pio_null));

    #endif
}

void wiznet_pio_spi_write_byte(uint8_t op_code, uint16_t AddrSel, uint8_t *tx, uint16_t tx_length) {
    uint8_t command_buf[8] = {0, };
    uint16_t command_len = mk_cmd_buf(command_buf, op_code, AddrSel);
    uint32_t loop_cnt = 0;
    tx_length = tx_length + command_len;

    pio_sm_set_enabled(active_state->pio, active_state->pio_sm, false);
    pio_sm_set_wrap(active_state->pio, active_state->pio_sm, active_state->pio_offset, active_state->pio_offset + PIO_OFFSET_WRITE_BITS_END - 1);
    pio_sm_clear_fifos(active_state->pio, active_state->pio_sm);

    #if (_WIZCHIP_QSPI_MODE_ == QSPI_SINGLE_MODE)
    loop_cnt = 8;
    pio_sm_set_pindirs_with_mask(active_state->pio,
        active_state->pio_sm,
        (1u << active_state->spi_config->data_io0_pin), (1u << active_state->spi_config->data_io0_pin));
    #elif (_WIZCHIP_QSPI_MODE_ == QSPI_DUAL_MODE)
    loop_cnt = 4;
    pio_sm_set_pindirs_with_mask(active_state->pio,
        active_state->pio_sm,
        (1u << active_state->spi_config->data_io0_pin) | (1u << active_state->spi_config->data_io1_pin),
        (1u << active_state->spi_config->data_io0_pin) | (1u << active_state->spi_config->data_io1_pin));
    #elif (_WIZCHIP_QSPI_MODE_ == QSPI_QUAD_MODE)
    loop_cnt = 2;
    pio_sm_set_pindirs_with_mask(active_state->pio,
        active_state->pio_sm,
        (1u << active_state->spi_config->data_io0_pin) | (1u << active_state->spi_config->data_io1_pin) | (1u << active_state->spi_config->data_io2_pin) | (1u << active_state->spi_config->data_io3_pin),
        (1u << active_state->spi_config->data_io0_pin) | (1u << active_state->spi_config->data_io1_pin) | (1u << active_state->spi_config->data_io2_pin) | (1u << active_state->spi_config->data_io3_pin));

    #endif

    pio_sm_restart(active_state->pio, active_state->pio_sm);
    pio_sm_clkdiv_restart(active_state->pio, active_state->pio_sm);
    pio_sm_put(active_state->pio, active_state->pio_sm, tx_length * loop_cnt - 1);
    pio_sm_exec(active_state->pio, active_state->pio_sm, pio_encode_out(pio_x, 32));
    pio_sm_put(active_state->pio, active_state->pio_sm, 0);
    pio_sm_exec(active_state->pio, active_state->pio_sm, pio_encode_out(pio_y, 32));
    pio_sm_exec(active_state->pio, active_state->pio_sm, pio_encode_jmp(active_state->pio_offset));
    dma_channel_abort(active_state->dma_out);


    dma_channel_config out_config = dma_channel_get_default_config(active_state->dma_out);
    channel_config_set_transfer_data_size(&out_config, DMA_SIZE_8);
    channel_config_set_bswap(&out_config, true);
    channel_config_set_dreq(&out_config, pio_get_dreq(active_state->pio, active_state->pio_sm, true));

    pio_sm_set_enabled(active_state->pio, active_state->pio_sm, true);

    dma_channel_configure(active_state->dma_out, &out_config, &active_state->pio->txf[active_state->pio_sm], command_buf, command_len, true);
    dma_channel_wait_for_finish_blocking(active_state->dma_out);
    dma_channel_configure(active_state->dma_out, &out_config, &active_state->pio->txf[active_state->pio_sm], tx, tx_length - command_len, true);
    dma_channel_wait_for_finish_blocking(active_state->dma_out);

    const uint32_t fdebug_tx_stall = 1u << (PIO_FDEBUG_TXSTALL_LSB + active_state->pio_sm);
    active_state->pio->fdebug = fdebug_tx_stall;
    // pio_sm_set_enabled(active_state->pio, active_state->pio_sm, true);
    while (!(active_state->pio->fdebug & fdebug_tx_stall)) {
        tight_loop_contents(); // todo timeout
    }
    #if 1

    __compiler_memory_barrier();
    // pio_sm_set_enabled(active_state->pio, active_state->pio_sm, false);
    #if (_WIZCHIP_QSPI_MODE_ == QSPI_SINGLE_MODE)
    pio_sm_set_consecutive_pindirs(active_state->pio, active_state->pio_sm, active_state->spi_config->data_io0_pin, 1, false);
    #elif (_WIZCHIP_QSPI_MODE_ == QSPI_DUAL_MODE)
    pio_sm_set_consecutive_pindirs(active_state->pio, active_state->pio_sm, active_state->spi_config->data_io0_pin, 2, false);
    #elif (_WIZCHIP_QSPI_MODE_ == QSPI_QUAD_MODE)
    pio_sm_set_consecutive_pindirs(active_state->pio, active_state->pio_sm, active_state->spi_config->data_io0_pin, 4, false);
    #endif

    pio_sm_exec(active_state->pio, active_state->pio_sm, pio_encode_mov(pio_pins, pio_null));
    pio_sm_set_enabled(active_state->pio, active_state->pio_sm, false);
    #endif
}
#else
// send tx then receive rx
// rx can be null if you just want to send, but tx and tx_length must be valid
static bool wiznet_pio_spi_transfer_impl(spi_pio_state_t *state, const uint8_t *tx, size_t tx_length, uint8_t *rx, size_t rx_length) {
    assert(state);
    if (!state || (tx == NULL)) {
        return false;
    }

    if (rx != NULL && tx != NULL) {
        assert(tx && tx_length && rx_length);

        pio_sm_set_enabled(state->pio, state->pio_sm, false); // disable sm
        pio_sm_set_wrap(state->pio, state->pio_sm, state->pio_offset + PIO_OFFSET_WRITE_BITS, state->pio_offset + PIO_OFFSET_READ_BITS_END - 1);
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
        pio_sm_set_wrap(state->pio, state->pio_sm, state->pio_offset + PIO_OFFSET_WRITE_BITS, state->pio_offset + PIO_OFFSET_WRITE_BITS_END - 1);
        pio_sm_clear_fifos(state->pio, state->pio_sm);
        pio_sm_restart(state->pio, state->pio_sm);
        pio_sm_clkdiv_restart(state->pio, state->pio_sm);
        pio_sm_put(state->pio, state->pio_sm, tx_length * 8 - 1);
        pio_sm_exec(state->pio, state->pio_sm, pio_encode_out(pio_x, 32));
        pio_sm_put(state->pio, state->pio_sm, tx_length - 1);
        pio_sm_exec(state->pio, state->pio_sm, pio_encode_out(pio_y, 32));
        pio_sm_exec(state->pio, state->pio_sm, pio_encode_set(pio_pins, 0));
        pio_sm_set_consecutive_pindirs(state->pio, state->pio_sm, state->spi_config->data_out_pin, 1, true);
        pio_sm_exec(state->pio, state->pio_sm, pio_encode_jmp(state->pio_offset + PIO_OFFSET_WRITE_BITS));
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


static uint8_t wiznet_pio_spi_read_byte(void) {
    assert(active_state);
    assert(active_state->spi_header_count == SPI_HEADER_LEN);
    uint8_t ret;
    if (!wiznet_pio_spi_transfer_impl(active_state, active_state->spi_header, active_state->spi_header_count, &ret, 1)) {
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
    assert(active_state->spi_header_count == SPI_HEADER_LEN);
    if (!wiznet_pio_spi_transfer_impl(active_state, active_state->spi_header, active_state->spi_header_count, pBuf, len)) {
        panic("spi failed reading buffer");
    }
    active_state->spi_header_count = 0;
}

// If we have been asked to write a spi header already, then write it and the rest of the buffer
// or else if we've been given enough data for just the spi header, save it until the next call
// or we're writing a byte in which case we're given a buffer including the spi header
void wiznet_pio_spi_write_buffer(const uint8_t *pBuf, uint16_t len) {
    assert(active_state);
    if (len == SPI_HEADER_LEN && active_state->spi_header_count == 0) {
        memcpy(active_state->spi_header, pBuf, SPI_HEADER_LEN); // expect another call
        active_state->spi_header_count = SPI_HEADER_LEN;
    } else if (len == 4 && active_state->spi_header_count == 0) {
        // W5500 WIZCHIP_WRITE case: 4 bytes = 3-byte header + 1-byte data
        // Send header first, then data
        if (!wiznet_pio_spi_transfer_impl(active_state, pBuf, SPI_HEADER_LEN, NULL, 0)) {
            panic("spi failed writing W5500 header");
        }
        if (!wiznet_pio_spi_transfer_impl(active_state, pBuf + SPI_HEADER_LEN, 1, NULL, 0)) {
            panic("spi failed writing W5500 data");
        }
    } else {
        if (active_state->spi_header_count == SPI_HEADER_LEN) {
            if (!wiznet_pio_spi_transfer_impl(active_state, active_state->spi_header, SPI_HEADER_LEN, NULL, 0)) {
                panic("spi failed writing header");
            }
            active_state->spi_header_count = 0;
        }
        assert(active_state->spi_header_count == 0);
        if (!wiznet_pio_spi_transfer_impl(active_state, pBuf, len, NULL, 0)) {
            panic("spi failed writing buffer");
        }
    }
}
#endif


static void wiznet_pio_spi_set_active(wiznet_pio_spi_handle_t handle) {
    active_state = (spi_pio_state_t *)handle;
}

static void wiznet_pio_spi_set_inactive(void) {
    active_state = NULL;
}

static void wizchip_pio_spi_reset(wiznet_pio_spi_handle_t handle) {

    spi_pio_state_t *state = (spi_pio_state_t *)handle;
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
        #if   (_WIZCHIP_ == W6300)
        .read_byte = wiznet_pio_spi_read_byte,
        .write_byte = wiznet_pio_spi_write_byte,
        .read_buffer = NULL,
        .write_buffer = NULL,
        #else // W55RP20
        .read_byte = wiznet_pio_spi_read_byte,
        .write_byte = wiznet_pio_spi_write_byte,
        .read_buffer = wiznet_pio_spi_read_buffer,
        .write_buffer = wiznet_pio_spi_write_buffer,
        #endif
        .reset = wizchip_pio_spi_reset,
    };
    return &funcs;
}

#endif // MICROPY_WIZNET_PIO

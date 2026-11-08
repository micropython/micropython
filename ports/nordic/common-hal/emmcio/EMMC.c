// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// ============================================================================
//  eMMC flash driver (1-bit MMC protocol over the nRF52840)
// ============================================================================
//    Two layers:
//
//    * COMMAND / control phases (init, CMD17/18 headers, busy polling) are
//      bit-banged on GPIO. They are short and timing-insensitive.
//
//    * The 512-byte DATA payloads ride SPIM3 + EasyDMA at 16 MHz. eMMC DAT0 at
//      default speed is SPI-mode-0 compatible: the host launches data while
//      CLK is low, the card samples (and launches) on the rising edge, MSB
//      first. The start-bit hunt is bit-banged, then the payload + CRC16 is
//      exactly byte-aligned for one RX DMA; on ENABLE=0 the pins fall back to
//      their GPIO latches.
//
//  INTEGRITY: every block read is verified against the card's CRC16 and the
//  caller retries on a mismatch.
//
// ============================================================================

#include "common-hal/emmcio/EMMC.h"
#include "common-hal/emmcio/emmc_hw.h"

#include <string.h>

#include "extmod/vfs.h"          // MP_BLOCKDEV_IOCTL_*
#include "lib/sdmmc/include/sdmmc_defs.h" // MMC_* command numbers
#include "py/mphal.h"
#include "py/runtime.h"          // RUN_BACKGROUND_TASKS
#include "shared-bindings/microcontroller/__init__.h"
#include "common-hal/microcontroller/Pin.h"
#include "peripherals/nrf/nrf52840/pins.h"
#include "shared-module/emmcio/__init__.h"

#define CMD_SAFE_HALF_US 1u

// Command-phase half-period: starts slow, switched to 0 (full-speed) after init.
static uint32_t s_cmd_half_us = CMD_SAFE_HALF_US;

static volatile uint32_t g_emmc_clk_half_us = CMD_SAFE_HALF_US;


static bool s_ready;
static uint32_t s_rca;
static uint32_t s_block_count;        // from EXT_CSD SEC_COUNT; 0 = not read yet
static uint8_t s_device_type;         // from EXT_CSD[196]; 0 = not read yet

// One 512-byte block + its CRC16, byte-aligned, for the RX DMA.
static uint8_t s_dma_rx[EMMC_BLOCK_SIZE + 2];

// ---- bounded-wait helpers --------------------------------------------------
// The 32768 Hz counter is 24-bit, so every elapsed calculation masks.
#define US_TO_TICKS(us) ((uint32_t)(((uint64_t)(us) * EMMC_TICKS_HZ + 999999u) / 1000000u))

static inline uint32_t ticks_since(uint32_t t0) {
    return ticks_since_raw(t0);
}

static inline void half_delay(uint32_t us) {
    if (us) {
        common_hal_mcu_delay_us(us);
    }
}

static bool s_deadline_armed;
static uint32_t s_deadline_t0;
static uint32_t s_deadline_lim;

void common_hal_emmcio_emmc_set_deadline(uint32_t timeout_us) {
    s_deadline_t0 = EMMC_TICKS();
    s_deadline_lim = US_TO_TICKS(timeout_us);
    s_deadline_armed = true;
}

void common_hal_emmcio_emmc_clear_deadline(void) {
    s_deadline_armed = false;
}

static bool emmc_deadline_expired(void) {
    return s_deadline_armed && ticks_since(s_deadline_t0) >= s_deadline_lim;
}

static inline void emmc_yield(void) {
    RUN_BACKGROUND_TASKS;
}

// Safe clock pulse for command/CRC phases.
static inline void clk_pulse(void) {
    CLK_HIGH();
    half_delay(s_cmd_half_us);
    CLK_LOW();
    half_delay(s_cmd_half_us);
}

static void cmd_send_bit(uint8_t bit) {
    // caller (send_command) sets CMD_OUT() once.
    if (bit) {
        CMD_HIGH();
    } else {
        CMD_LOW();
    }
    clk_pulse();
}

// SAMPLE POINT: read the line at the END of the low phase, i.e. before this
// bit's clock pulse, not in the middle of it. That is the one point in the
// cycle where both of the card's timing modes hold valid data.
//
//   * backward-compatible timing: the card launches on the FALLING edge and
//     holds the bit until the next one, so the whole low phase is valid.
//     tOSU(min) = tWL(min) - tODLY, data good from ~8 ns after the edge.
//     We read a full low phase later.
//   * high-speed timing: the card launches on the RISING edge (tODLY, 13.7 ns
//     max, referenced to it) and holds until the next rising edge.
//
static uint8_t cmd_recv_bit(void) {
    // caller sets CMD_IN() once before the response read
    uint8_t b = (uint8_t)READ_CMD();
    clk_pulse();
    return b;
}

static uint8_t crc7(const uint8_t *data, uint8_t len) {
    uint8_t crc = 0;
    for (uint8_t i = 0; i < len; i++) {
        uint8_t v = data[i];
        for (int b = 7; b >= 0; b--) {
            crc <<= 1;
            if (((v >> b) & 1) ^ ((crc >> 7) & 1)) {
                crc ^= 0x09;
            }
            crc &= 0x7F;
        }
    }
    return (crc << 1) | 1;
}

// Table-driven CRC16-CCITT
static uint16_t s_crc16_tab[256];
static void crc16_tab_init(void) {
    for (uint32_t i = 0; i < 256; i++) {
        uint16_t crc = (uint16_t)(i << 8);
        for (int b = 0; b < 8; b++) {
            crc = (crc & 0x8000) ? (uint16_t)((crc << 1) ^ 0x1021) : (uint16_t)(crc << 1);
        }
        s_crc16_tab[i] = crc;
    }
}

// The port builds at -Os, so this is an opt-up. crc16 is pure computation,
// the level can only change its speed, never its value.
__attribute__((optimize("O2")))
static uint16_t crc16(const uint8_t *data, uint32_t len) {
    uint16_t crc = 0;
    for (uint32_t i = 0; i < len; i++) {
        crc = (uint16_t)((crc << 8) ^ s_crc16_tab[(crc >> 8) ^ data[i]]);
    }
    return crc;
}

static bool send_command(uint8_t cmd_index, uint32_t arg, uint8_t *r1_out) {
    uint8_t frame[6];
    frame[0] = 0x40 | (cmd_index & 0x3F);
    frame[1] = (uint8_t)(arg >> 24);
    frame[2] = (uint8_t)(arg >> 16);
    frame[3] = (uint8_t)(arg >> 8);
    frame[4] = (uint8_t)(arg);
    frame[5] = crc7(frame, 5);

    // PRE-COMMAND GAP on an UNDRIVEN line
    CMD_IN();
    for (int i = 0; i < 24; i++) {
        clk_pulse();
    }
    CMD_OUT();
    cmd_send_bit(0);
    cmd_send_bit(1);
    for (int b = 5; b >= 0; b--) {
        cmd_send_bit((frame[0] >> b) & 1);
    }
    for (int i = 1; i <= 4; i++) {
        for (int b = 7; b >= 0; b--) {
            cmd_send_bit((frame[i] >> b) & 1);
        }
    }
    for (int b = 7; b >= 1; b--) {
        cmd_send_bit((frame[5] >> b) & 1);
    }
    cmd_send_bit(1);

    CMD_IN();
    bool responded = false;
    for (int t = 0; t < 200; t++) {
        clk_pulse();
        if (!READ_CMD()) {
            responded = true;
            break;
        }
    }
    if (!responded) {
        return false;
    }

    if (!r1_out) {
        return true;
    }

    uint8_t resp[6] = {0};
    for (int i = 0; i < 38; i++) {
        uint8_t bit = cmd_recv_bit();
        resp[i / 8] |= (bit << (7 - (i % 8)));
    }
    memcpy(r1_out, resp, 6);

    // Leave CMD as an INPUT (pulled up)
    return true;
}

// Bit-banged MMC commands intermittently miss the response on the first try
// (settling after the previous command); retry until the card answers.
static bool send_command_retry(uint8_t cmd, uint32_t arg, uint8_t *r1_out, int tries) {
    for (int t = 0; t < tries; t++) {
        if (emmc_deadline_expired()) {
            return false;
        }
        if (send_command(cmd, arg, r1_out)) {
            return true;
        }
        if (t == 0) {
            // First miss = the card still settling after the previous burst: a
            // handful of idle clocks is all it needs.
            for (int c = 0; c < 16; c++) {
                clk_pulse();
            }
        } else {
            mp_hal_delay_ms(2);
        }
    }
    return false;
}

// DATA read: per-bit CLK toggle uses the configurable (possibly 0) half-period.
// -O2 opts up from the port's -Os default and is needed: at -Os the GPIO
// and delay helpers stop being inlined and become calls inside the per-bit
// loop, and throw off the timing
__attribute__((optimize("O2")))
static bool read_data_block(uint8_t *buf) {
    const uint32_t hd = g_emmc_clk_half_us;
    const uint32_t clk_bit = emmc_pinout.clk_bit;
    const uint32_t dat_bit = emmc_pinout.dat_bit;

    DAT0_IN();
    // START-BIT HUNT
    {
        uint32_t t0 = EMMC_TICKS();
        const uint32_t lim = US_TO_TICKS(80000u);      // 80 ms bound
        const uint32_t yield_at = US_TO_TICKS(500u);
        bool got_start = false;
        for (;;) {
            // This hunt samples in the HIGH phase and stays there in both
            // timing modes
            for (int burst = 0; burst < 64 && !got_start; burst++) {
                RCLK_HIGH(clk_bit);
                half_delay(hd);
                EDGE_SETTLE();
                if (!RDAT_GET(dat_bit)) {
                    got_start = true;    // leave with RCLK HIGH (as before)
                    break;
                }
                RCLK_LOW(clk_bit);
                half_delay(hd);
            }
            uint32_t el = ticks_since(t0);
            if (got_start) {
                break;
            }
            if (el >= lim || emmc_deadline_expired()) {
                return false;
            }
            if (el >= yield_at) {
                emmc_yield();
            }
        }
    }
    RCLK_LOW(clk_bit);
    half_delay(hd);

    // The start bit was just consumed by the bit-bang hunt above, so the
    // remaining 512 data bytes + CRC16 are exactly byte-aligned.
    emmc_spim_xfer(NULL, 0, s_dma_rx, sizeof(s_dma_rx));
    memcpy(buf, s_dma_rx, EMMC_BLOCK_SIZE);
    uint16_t card_crc = (uint16_t)(((uint16_t)s_dma_rx[EMMC_BLOCK_SIZE] << 8) |
        s_dma_rx[EMMC_BLOCK_SIZE + 1]);
    RCLK_HIGH(clk_bit);
    half_delay(hd);
    RCLK_LOW(clk_bit);
    half_delay(hd);                                          // end bit
    bool crc_ok = crc16(buf, EMMC_BLOCK_SIZE) == card_crc;
    DAT0_OUT();
    RDAT_HIGH(dat_bit);
    return crc_ok;                                     // a mismatch: caller retries
}

bool common_hal_emmcio_emmc_read_status(uint8_t *r1_out) {
    return send_command_retry(13, s_rca, r1_out, 8);
}

// Clock out an R2 response and reassemble the CID. R2 framing: start(0) +
// transmission(0) + 6 reserved ones + CID[127:1] + end(1) = 136 bits.
static void drain_r2_cid(uint8_t *cid_out) {
    uint8_t bits[136];
    for (int i = 0; i < 136; i++) {
        bits[i] = cmd_recv_bit();
    }
    memset(cid_out, 0, 16);
    for (int i = 0; i < 128; i++) {
        // bits[0] start, bits[1] transmission, bits[2..7] six reserved ones,
        // bits[8..134] CID[127:1], bits[135] end bit
        cid_out[i / 8] |= (uint8_t)(bits[8 + i] << (7 - (i % 8)));
    }
}

#define EMMC_POWER_OFF_MS 50u

static void emmc_power_cycle(void) {
    emmc_spim_deinit();              // SPIM3 must not drive DAT0 either
    emmc_pins_init();

    RST_ASSERT();
    CLK_LOW();
    CMD_LOW();
    DAT0_OUT();
    DAT0_LOW();
    VCCQ_OFF();
    mp_hal_delay_ms(EMMC_POWER_OFF_MS);
}

static bool emmc_init(emmcio_emmc_obj_t *self) {
    s_ready = false;
    s_block_count = 0;
    s_device_type = 0;
    g_emmc_clk_half_us = CMD_SAFE_HALF_US;
    s_cmd_half_us = CMD_SAFE_HALF_US;

    self->cmd0_sent = false;
    self->cmd1_retries = -1;
    self->cmd2_resp = false;
    self->cmd3_resp = false;
    self->cmd7_resp = false;
    self->cmd16_resp = false;
    memset(self->cid, 0, sizeof(self->cid));
    self->hs_switch_error = false;
    self->hs_active = false;
    self->hs_stage = 0;

    emmc_power_cycle();

    emmc_spim_init();                // hardware-clocked data path, at M16
    crc16_tab_init();

    CLK_LOW();
    CMD_HIGH();
    DAT0_HIGH();

    VCCQ_ON();
    mp_hal_delay_ms(10);

    RST_ASSERT();
    mp_hal_delay_ms(1);
    RST_RELEASE();
    mp_hal_delay_ms(2);

    CMD_HIGH();
    for (int i = 0; i < 80; i++) {       // 74+ clocks before the first command
        clk_pulse();
    }

    send_command(0, 0x00000000, NULL);   // CMD0 GO_IDLE (no response expected)
    self->cmd0_sent = true;
    mp_hal_delay_ms(1);

    // CMD1 SEND_OP_COND, arg 0x40FF8000: HCS=1
    uint8_t r3[6] = {0};
    for (int retry = 0; retry < 1000; retry++) {
        bool ok = send_command(1, 0x40FF8000, r3);
        emmc_yield();
        mp_hal_delay_ms(1);
        if (ok && (r3[1] & 0x80)) {      // response seen AND busy bit set = ready
            self->cmd1_retries = retry;
            break;
        }
        if (emmc_deadline_expired()) {
            break;
        }
    }
    if (self->cmd1_retries < 0) {  // card never responded ready -> stop
        return false;
    }

    for (int t = 0; t < 8; t++) {
        self->cmd2_resp = send_command(2, 0, NULL);
        if (self->cmd2_resp) {
            drain_r2_cid(self->cid);
            break;
        }
        mp_hal_delay_ms(2);
    }
    mp_hal_delay_ms(1);

    uint8_t r6[6] = {0};
    s_rca = 0x0001u << 16;
    self->cmd3_resp = send_command_retry(3, s_rca, r6, 8);   // SET_RELATIVE_ADDR
    mp_hal_delay_ms(1);

    uint8_t r1[6] = {0};
    self->cmd7_resp = send_command_retry(7, s_rca, r1, 8);   // SELECT_CARD
    mp_hal_delay_ms(1);
    self->cmd16_resp = send_command_retry(16, EMMC_BLOCK_SIZE, r1, 8); // SET_BLOCKLEN
    mp_hal_delay_ms(1);

    // strict: ready only if the card actually selected AND accepted block length
    s_ready = self->cmd7_resp && self->cmd16_resp;
    if (s_ready) {
        s_cmd_half_us = 0u;              // identification done: full-speed commands
        g_emmc_clk_half_us = 0u;
    }
    return s_ready;
}

uint32_t common_hal_emmcio_emmc_get_block_count(emmcio_emmc_obj_t *self) {
    (void)self;
    return s_block_count;
}

// The block-device ioctl
bool common_hal_emmcio_emmc_ioctl(uint32_t op, uint32_t arg, uint32_t *out_value) {
    (void)arg;
    *out_value = 0;
    switch (op) {
        case MP_BLOCKDEV_IOCTL_INIT:
            // The constructor already did the whole CMD0..CMD16 + EXT_CSD
            // walk, or raised. 0 means "initialised"; a card that has since
            // been deinited answers with the error the callers check for
            // (s_ready), so a mount over a dead object fails at INIT rather
            // than at the first read.
            *out_value = s_ready ? 0u : 1u;
            break;
        case MP_BLOCKDEV_IOCTL_DEINIT:
        case MP_BLOCKDEV_IOCTL_SYNC:
        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE:
            break;
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            *out_value = s_block_count;
            break;
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            *out_value = EMMC_BLOCK_SIZE;
            break;
        default:
            return false;
    }
    return true;
}

// Power-off: release the bus pins and cut the VCCQ I/O rail.
// The card is gone until the next emmc_init().
static void emmc_power_down(void) {
    s_ready = false;
    s_block_count = 0;
    emmc_spim_deinit();
    RST_ASSERT();
    emmc_pins_release();
    VCCQ_OFF();                          // rail off (pin stays an output)
}

// CMD8 SEND_EXT_CSD: an ADTC (read) command -- the card responds R1, then
// sends a single 512-byte EXT_CSD data block on DAT0 exactly like CMD17.
// Read-only and safe. buf must be >= EMMC_BLOCK_SIZE.
bool common_hal_emmcio_emmc_read_ext_csd(uint8_t *buf) {
    if (!s_ready) {
        return false;
    }
    uint8_t r1[6];
    if (!send_command_retry(8, 0, r1, 8)) {
        return false;
    }
    if (!read_data_block(buf)) {
        return false;
    }
    // SEC_COUNT[215:212], little-endian. 0x00760000 on this part = 7,733,248
    // blocks; the value is the software LBA bound for every later read.
    s_block_count = (uint32_t)buf[212] | ((uint32_t)buf[213] << 8) |
        ((uint32_t)buf[214] << 16) | ((uint32_t)buf[215] << 24);
    // DEVICE_TYPE[196] gates the HS_TIMING switch (bit 1 = 52 MHz supported;
    // this part reads 0x57).
    s_device_type = buf[196];
    return true;
}

// ---- R1b / program busy on DAT0 --------------------------------------------
// Shared by the CMD6 switch (below) and the write path (further down): the
// card pulls DAT0 low while it programs and releases it high when done, and it
// only advances on OUR clock, so the host must keep clocking for the card to
// get anywhere.

#define EMMC_BUSY_LEADIN_CLOCKS 16

static bool dat0_busy_wait(uint32_t timeout_us, bool run_bg) {
    DAT0_IN();                                   // never drive against a busy card
    for (int i = 0; i < EMMC_BUSY_LEADIN_CLOCKS; i++) {
        clk_pulse();
    }
    uint32_t t0 = EMMC_TICKS();
    const uint32_t lim = US_TO_TICKS(timeout_us);
    for (;;) {
        bool released = false;
        for (int i = 0; i < 64 && !released; i++) {
            CLK_HIGH();
            half_delay(s_cmd_half_us);
            released = READ_DAT0() != 0;
            CLK_LOW();
            half_delay(s_cmd_half_us);
        }
        uint32_t el = ticks_since(t0);
        if (released) {
            DAT0_OUT();                          // back to the read path's resting state
            DAT0_HIGH();
            return true;
        }
        if (el >= lim || emmc_deadline_expired()) {
            // DAT0 STAYS AN INPUT on a timeout
            return false;
        }
        if (run_bg) {
            emmc_yield();
        }
    }
}

// CMD6 SWITCH argument: access 0b11 (WRITE_BYTE) | index 185 | value 1 |
// cmd_set 0  ->  0x03 B9 01 00.
#define EMMC_SWITCH_HS_TIMING_ARG  0x03B90100u
#define EMMC_EXT_CSD_HS_TIMING     185u
#define EMMC_EXT_CSD_DEVICE_TYPE   196u
#define EMMC_DEVICE_TYPE_HS52      0x02u

// GENERIC_CMD6_TIME on this part is 0x05 = 50 ms. Ten times that is the bound.
#define EMMC_CMD6_BUSY_US  500000u

// Poll CMD13 until the card is back in tran and ready for data. This is the
// authoritative "the switch finished" test, and it is also where SWITCH_ERROR
// (status bit 7) shows up if the card rejected the write.
static bool wait_tran_after_switch(emmcio_emmc_obj_t *self, uint32_t timeout_us) {
    uint32_t t0 = EMMC_TICKS();
    const uint32_t lim = US_TO_TICKS(timeout_us);
    for (;;) {
        uint8_t r1[6];
        if (common_hal_emmcio_emmc_read_status(r1)) {
            uint32_t status = ((uint32_t)r1[1] << 24) | ((uint32_t)r1[2] << 16) |
                ((uint32_t)r1[3] << 8) | (uint32_t)r1[4];
            if (status & (1u << 7)) {            // SWITCH_ERROR: the card said no
                self->hs_switch_error = true;
                return false;
            }
            if (((status >> 9) & 0xFu) == 4u && ((status >> 8) & 1u)) {
                return true;                     // tran + ready_for_data
            }
        }
        if (ticks_since(t0) >= lim || emmc_deadline_expired()) {
            return false;
        }
        emmc_yield();
        mp_hal_delay_ms(1);
    }
}

static bool emmc_set_high_speed(emmcio_emmc_obj_t *self) {
    if (!s_ready) {
        return false;
    }
    // Gate on the card's own capability byte.
    if (!(s_device_type & EMMC_DEVICE_TYPE_HS52)) {
        return false;
    }
    self->hs_stage = 1;

    uint8_t r1[6];
    if (!send_command_retry(6, EMMC_SWITCH_HS_TIMING_ARG, r1, 8)) {
        return false;
    }
    self->hs_stage = 2;
    // run_bg = true: a CMD6 on a volatile byte has no in-flight card state a
    // power-off gesture could damage, so this wait services them as the read
    // path does.
    if (!dat0_busy_wait(EMMC_CMD6_BUSY_US, true)) {
        return false;
    }
    self->hs_stage = 3;
    if (!wait_tran_after_switch(self, EMMC_CMD6_BUSY_US)) {
        return false;
    }
    self->hs_stage = 4;


    NRF_SPIM3->CONFIG = SPIM_CONFIG_MODE1;

    // Read the byte back AT THE OLD CLOCK.
    uint8_t ext_csd[EMMC_BLOCK_SIZE];
    if (!common_hal_emmcio_emmc_read_ext_csd(ext_csd) ||
        ext_csd[EMMC_EXT_CSD_HS_TIMING] != 1u) {
        NRF_SPIM3->CONFIG = SPIM_CONFIG_MODE0;
        return false;                            // still at M16, card still readable
    }
    self->hs_stage = 5;

    // Now host clock moves. The re-read is a smoke test of the
    // faster bus with the integrity layer watching.
    NRF_SPIM3->FREQUENCY = SPIM_FREQ_M32;
    if (!common_hal_emmcio_emmc_read_ext_csd(ext_csd) || ext_csd[EMMC_EXT_CSD_HS_TIMING] != 1u) {
        // Back to the old CLOCK but NOT to the old phase: the card is in
        // high-speed timing and stays there until the rail drops, and
        // high-speed timing is specified from 0 Hz up. Mode 1 is how we talk
        // to it at M16 now.
        NRF_SPIM3->FREQUENCY = SPIM_FREQ_M16;
        return false;
    }
    self->hs_active = true;
    self->hs_stage = 6;
    return true;
}

bool common_hal_emmcio_emmc_readblocks(uint32_t block_addr, uint8_t *buf, uint32_t count) {
    if (!s_ready || count == 0) {
        return false;
    }
    // Reject an out-of-range LBA before any command reaches the card
    if (s_block_count != 0 &&
        (block_addr >= s_block_count || count > s_block_count - block_addr)) {
        return false;
    }
    uint8_t r1[6];
    if (count == 1) {
        if (!send_command_retry(17, block_addr, r1, 8)) {
            return false;
        }
        return read_data_block(buf);
    }
    // RETRY like CMD17 above: at high bus duty the card intermittently misses
    // the first command after the previous burst's CMD12
    if (!send_command_retry(18, block_addr, r1, 4)) {
        return false;
    }

    uint32_t bt0 = EMMC_TICKS();
    const uint32_t blim = US_TO_TICKS(150000u);
    for (uint32_t i = 0; i < count; i++) {
        if (i && (ticks_since(bt0) >= blim || emmc_deadline_expired())) {
            (void)send_command_retry(12, 0, r1, 3);
            return false;
        }
        if (!read_data_block(buf + i * EMMC_BLOCK_SIZE)) {
            (void)send_command_retry(12, 0, r1, 3);
            return false;
        }
    }
    (void)send_command_retry(12, 0, r1, 3);
    return true;
}

// The card declares MIN_PERF_W_* = 0x00: no minimum write performance
#define EMMC_WR_BUSY_US    500000u
// Same shape as the read side
#define EMMC_WR_BURST_US   250000u

// -Os states the intent for this bit-bang, but matches the port default and so
// changes nothing today.
__attribute__((optimize("Os")))
static bool write_data_block(const uint8_t *buf) {
    const uint32_t hd = g_emmc_clk_half_us;
    const uint32_t clk_bit = emmc_pinout.clk_bit;
    const uint32_t dat_bit = emmc_pinout.dat_bit;

    // Write convention: change DAT0 while CLK is LOW, then a full half-period
    // of setup before the rising edge where the card latches it. DAT0 is a
    // HIGH-DRIVE (H0H1) output.
    //
    // The frame opens with DAT0 idle-HIGH for a whole byte (the Nwr gap) so
    // the card cannot mistake a stray low for an early start bit and misframe
    // the token.
    DAT0_OUT();
    RDAT_HIGH(dat_bit);

    uint8_t *tx = EMMC_TX_FRAME;   // the reserved low-RAM SPIM3 buffer
    uint16_t crc = crc16(buf, EMMC_BLOCK_SIZE);
    tx[0] = 0xFF;                                  // Nwr idle gap
    tx[1] = 0xFE;                                  // 7 idle bits + START 0
    memcpy(&tx[2], buf, EMMC_BLOCK_SIZE);
    tx[2 + EMMC_BLOCK_SIZE] = (uint8_t)(crc >> 8);
    tx[2 + EMMC_BLOCK_SIZE + 1] = (uint8_t)crc;
    RCLK_LOW(clk_bit);

    const uint32_t saved_cfg = NRF_SPIM3->CONFIG;
    if (saved_cfg != SPIM_CONFIG_MODE0) {
        NRF_SPIM3->CONFIG = SPIM_CONFIG_MODE0;
    }
    // The TX frame ends exactly at the crc's last bit, no trailing idle
    // byte. The card emits its CRC-status token a couple of clocks after the
    // end bit.
    emmc_spim_xfer(tx, 2u + EMMC_BLOCK_SIZE + 2u, NULL, 0);
    if (saved_cfg != SPIM_CONFIG_MODE0) {
        NRF_SPIM3->CONFIG = saved_cfg;
    }
    // END bit: DAT0 is back at its GPIO latch (output HIGH) -- clock it.
    half_delay(hd);
    EDGE_SETTLE();
    RCLK_HIGH(clk_bit);
    half_delay(hd);
    RCLK_LOW(clk_bit);

    // CRC-status token: the card drives DAT0 low (start bit), then 3 status
    // bits -- 010 accepted, 101 CRC error, 110 write error -- then releases.
    DAT0_IN();
    int wr_status = -1;
    for (int i = 0; i < 16; i++) {
        RCLK_HIGH(clk_bit);
        half_delay(hd);
        EDGE_SETTLE();
        int start = (int)RDAT_GET(dat_bit);
        RCLK_LOW(clk_bit);
        half_delay(hd);
        if (!start) {
            int st = 0;
            for (int k = 0; k < 3; k++) {
                RCLK_HIGH(clk_bit);
                half_delay(hd);
                EDGE_SETTLE();
                st = (st << 1) | (int)RDAT_GET(dat_bit);
                RCLK_LOW(clk_bit);
                half_delay(hd);
            }
            wr_status = st;
            break;
        }
    }

    // Programming busy on DAT0
    if (!dat0_busy_wait(EMMC_WR_BUSY_US, false)) {
        return false;                    // DAT0 left an INPUT -- see the wait
    }

    // ENFORCE the token: 0b010 = accepted. Anything else means the card did
    // not take the block.
    if (wr_status != 0x2) {
        return false;
    }
    return true;
}

bool common_hal_emmcio_emmc_writeblocks(uint32_t block_addr, const uint8_t *buf, uint32_t count) {
    if (!s_ready || count == 0) {
        return false;
    }
    if (s_block_count != 0 &&
        (block_addr >= s_block_count || count > s_block_count - block_addr)) {
        return false;
    }
    uint8_t r1[6];
    if (count == 1) {
        if (!send_command_retry(24, block_addr, r1, 8)) {
            return false;
        }
        return write_data_block(buf);
    }
    // Settle-miss retry, exactly as CMD18: at high bus duty the card
    // intermittently misses the first command after the previous burst.
    if (!send_command_retry(25, block_addr, r1, 4)) {
        return false;
    }
    uint32_t bt0 = EMMC_TICKS();
    const uint32_t blim = US_TO_TICKS(EMMC_WR_BURST_US);
    for (uint32_t i = 0; i < count; i++) {
        if (i && (ticks_since(bt0) >= blim || emmc_deadline_expired())) {
            (void)send_command_retry(12, 0, r1, 3);
            return false;
        }
        if (!write_data_block(buf + i * EMMC_BLOCK_SIZE)) {
            (void)send_command_retry(12, 0, r1, 3);
            return false;
        }
    }
    (void)send_command_retry(12, 0, r1, 3);

    (void)dat0_busy_wait(EMMC_WR_BUSY_US, false);
    return true;
}

uint32_t common_hal_emmcio_emmc_get_frequency(emmcio_emmc_obj_t *self) {
    (void)self;
    return NRF_SPIM3->FREQUENCY == SPIM_FREQ_M32 ? 32000000u : 16000000u;
}

bool common_hal_emmcio_emmc_get_high_speed(emmcio_emmc_obj_t *self) {
    return self->hs_active;
}

const uint8_t *common_hal_emmcio_emmc_get_cid(emmcio_emmc_obj_t *self) {
    return self->cid;
}

static bool s_constructed;

emmc_pinout_t emmc_pinout;

static const mcu_pin_obj_t *s_claimed_pins[5];
static size_t s_claimed_pin_count;

bool emmcio_spim3_in_use(void) {
    return s_constructed;
}

void emmcio_emmc_release_hardware(void) {
    emmc_power_down();
    for (size_t i = 0; i < s_claimed_pin_count; i++) {
        reset_pin_number(s_claimed_pins[i]->number);
    }
    s_claimed_pin_count = 0;
    s_constructed = false;
}

static void emmc_claim_pins(const mcu_pin_obj_t *clock, const mcu_pin_obj_t *command,
    const mcu_pin_obj_t *data, const mcu_pin_obj_t *reset, const mcu_pin_obj_t *vccq) {
    emmc_pinout.clk = clock->number;
    emmc_pinout.cmd = command->number;
    emmc_pinout.dat0 = data->number;
    emmc_pinout.rst = reset != NULL ? reset->number : EMMC_NO_PIN;
    emmc_pinout.vccq = vccq != NULL ? vccq->number : EMMC_NO_PIN;
    emmc_pinout.clk_bit = 1u << clock->number;
    emmc_pinout.dat_bit = 1u << data->number;
    emmc_pinout.dat0_cnf = &NRF_P0->PIN_CNF[data->number];
    emmc_pinout.dat0_cnf_in = EMMC_CNF_IN;
    emmc_pinout.dat0_cnf_out = EMMC_CNF_OUT_H0H1;

    s_claimed_pin_count = 0;
    const mcu_pin_obj_t *pins[] = { clock, command, data, reset, vccq };
    for (size_t i = 0; i < MP_ARRAY_SIZE(pins); i++) {
        if (pins[i] == NULL) {
            continue;
        }
        claim_pin(pins[i]);
        s_claimed_pins[s_claimed_pin_count++] = pins[i];
    }
}

static emmcio_construct_result_t emmc_check_pins(const mcu_pin_obj_t *clock,
    const mcu_pin_obj_t *command, const mcu_pin_obj_t *data,
    const mcu_pin_obj_t *reset, const mcu_pin_obj_t *vccq, int *detail) {
    // The data path drives CLK and DAT0 through NRF_P0 directly, so both have
    // to be on port 0. CMD, RESET and VCCQ go through the HAL and may be
    // anywhere.
    if (clock->number >= P0_PIN_NUM) {
        *detail = MP_QSTR_clock;
        return EMMCIO_ERR_PIN_PORT;
    }
    if (data->number >= P0_PIN_NUM) {
        *detail = MP_QSTR_data;
        return EMMCIO_ERR_PIN_PORT;
    }
    if ((NRF_SPIM3->ENABLE & SPIM_ENABLE_ENABLE_Msk) != 0) {
        return EMMCIO_ERR_SPI_IN_USE;
    }
    const mcu_pin_obj_t *pins[] = { clock, command, data, reset, vccq };
    for (size_t i = 0; i < MP_ARRAY_SIZE(pins); i++) {
        if (pins[i] != NULL && !pin_number_is_free(pins[i]->number)) {
            return EMMCIO_ERR_PIN_IN_USE;
        }
    }
    return EMMCIO_OK;
}

#define RETURN_CMD_UNLESS(cmd, done) do { if (!(done)) { return cmd; } } while (0)

// The MMC command that never answered, so a failure names the step it stopped at.
static int init_failure_command(emmcio_emmc_obj_t *self) {
    RETURN_CMD_UNLESS(MMC_GO_IDLE_STATE, self->cmd0_sent);
    RETURN_CMD_UNLESS(MMC_SEND_OP_COND, self->cmd1_retries >= 0);  // never left busy
    RETURN_CMD_UNLESS(MMC_ALL_SEND_CID, self->cmd2_resp);
    RETURN_CMD_UNLESS(MMC_SET_RELATIVE_ADDR, self->cmd3_resp);
    RETURN_CMD_UNLESS(MMC_SELECT_CARD, self->cmd7_resp);
    RETURN_CMD_UNLESS(MMC_SET_BLOCKLEN, self->cmd16_resp);
    return MMC_SEND_EXT_CSD;
}

#undef RETURN_CMD_UNLESS

// How far the high-speed switch got: hs_stage as documented on the struct,
// except that a CMD13 SWITCH_ERROR (the card rejecting HS_TIMING) reports 7 to
// tell it apart from the card simply never coming back to tran.
static int hs_failure_stage(emmcio_emmc_obj_t *self) {
    if (self->hs_stage == 3 && self->hs_switch_error) {
        return 7;
    }
    return self->hs_stage;
}

static emmcio_construct_result_t emmc_power_up(emmcio_emmc_obj_t *self, bool high_speed,
    int *detail) {
    s_constructed = true;

    if (!emmc_init(self)) {
        *detail = init_failure_command(self);
        emmcio_emmc_release_hardware();
        return EMMCIO_ERR_INIT;
    }

    uint8_t ext_csd[EMMC_BLOCK_SIZE];
    if (!common_hal_emmcio_emmc_read_ext_csd(ext_csd)) {
        emmcio_emmc_release_hardware();
        *detail = 8;
        return EMMCIO_ERR_INIT;
    }
    if (high_speed && !emmc_set_high_speed(self)) {
        *detail = hs_failure_stage(self);
        emmcio_emmc_release_hardware();
        return EMMCIO_ERR_HIGH_SPEED;
    }
    return EMMCIO_OK;
}

emmcio_construct_result_t common_hal_emmcio_emmc_construct(emmcio_emmc_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *command, const mcu_pin_obj_t *data,
    const mcu_pin_obj_t *reset, const mcu_pin_obj_t *vccq,
    bool high_speed, bool write_enabled, int *detail) {
    // Ensure the object starts in its deinit state, so its finaliser does
    // nothing if construct fails.
    self->deinited = true;
    *detail = 0;
    if (emmcio_is_automounted()) {
        return EMMCIO_ERR_USB_OWNED;
    }
    if (s_constructed) {
        return EMMCIO_ERR_IN_USE;
    }
    emmcio_construct_result_t pin_err = emmc_check_pins(clock, command, data, reset, vccq, detail);
    if (pin_err != EMMCIO_OK) {
        return pin_err;
    }
    emmc_claim_pins(clock, command, data, reset, vccq);

    emmcio_construct_result_t err = emmc_power_up(self, high_speed, detail);
    if (err != EMMCIO_OK) {
        return err;
    }

    self->deinited = false;
    self->write_enabled = write_enabled;
    return EMMCIO_OK;
}

void common_hal_emmcio_emmc_deinit(emmcio_emmc_obj_t *self) {
    if (self->deinited) {
        return;
    }
    emmcio_emmc_release_hardware();
    self->deinited = true;
}

bool common_hal_emmcio_emmc_deinited(emmcio_emmc_obj_t *self) {
    return self->deinited;
}

#if CIRCUITPY_EMMC_USB

static emmcio_emmc_obj_t s_automount_obj;
static bool s_automounted;

mp_obj_t emmcio_automount_construct(const mcu_pin_obj_t *clock, const mcu_pin_obj_t *command,
    const mcu_pin_obj_t *data, const mcu_pin_obj_t *reset, const mcu_pin_obj_t *vccq,
    bool high_speed, bool write_enabled) {
    if (s_constructed) {
        return MP_OBJ_NULL;
    }
    int detail;
    if (emmc_check_pins(clock, command, data, reset, vccq, &detail) != EMMCIO_OK) {
        return MP_OBJ_NULL;
    }
    emmc_claim_pins(clock, command, data, reset, vccq);
    s_automount_obj.base.type = &emmcio_emmc_type;
    if (emmc_power_up(&s_automount_obj, high_speed, &detail) != EMMCIO_OK) {
        return MP_OBJ_NULL;
    }
    s_automount_obj.deinited = false;
    s_automount_obj.write_enabled = write_enabled;
    s_automounted = true;
    return MP_OBJ_FROM_PTR(&s_automount_obj);
}

bool emmcio_is_automounted(void) {
    return s_automounted;
}

void emmcio_automount_abandon(void) {
    s_automounted = false;
    s_automount_obj.deinited = true;
    if (s_constructed) {
        emmcio_emmc_release_hardware();
    }
}
#endif

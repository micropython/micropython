// sdcard.c - SD card driver over the GD32F4 SDIO controller.
//
// Implements: card detection, CMD/ACMD handling, block read/write,
// hot-plug re-init and a block-device protocol for the VFS layer.

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#include "gd32f4xx.h"

// ---- board pin configuration (SDIO as on GD32F4 default AF12) ----
#define SD_GPIO_RCU     RCU_GPIOC
#define SD_GPIO_PORT    GPIOC
// Card detect is PD3 on the LCKFB "Sky Star" board.
//
// This used to say GPIO_PIN_13.  The schematic
// (doc/SCH_立创梁山派·天空星开发板原理图_2024-01-17.pdf) ties the TF socket's
// CD(SD_CARD_DET) contact to the net SD_CARD_DET, which lands on PD3; PD13 is
// not connected to the card socket at all.  With the wrong pin the driver
// always read "no card" (or, because the PR was enabled, "card present" from a
// floating pin), and never got as far as real initialisation on a board whose
// detect line was actually working.
//
// Polarity is unchanged and confirmed by the schematic: the contact is pulled
// up to +3V3 through R20 (47k) and the socket shorts it to GND when a card is
// inserted, so a LOW level means "card present".
#define SD_CD_PORT      GPIOD
#define SD_CD_RCU       RCU_GPIOD
#define SD_CD_PIN       GPIO_PIN_3       // card detect, active low

// SDIO CMD is on PD2 (GPIOD), while CLK/D0-D3 are on GPIOC.
#define SD_CMD_PORT     GPIOD
#define SD_CMD_PIN      GPIO_PIN_2

#define SD_AF           GPIO_AF_12

// SDIO clock divider: 48 MHz / (div+2) -> 400 kHz for init
#define SD_INIT_CLK_DIV     (118)
#define SD_MAX_CLK_DIV      (0)          // 24 MHz after switching to high speed

// ---- SD protocol constants ----
#define CMD_GO_IDLE_STATE       0
#define CMD_SEND_OP_COND        1
#define CMD_ALL_SEND_CID        2
#define CMD_SEND_REL_ADDR       3
#define CMD_SWITCH_FUNC         6
#define CMD_SELECT_CARD         7
#define CMD_SEND_IF_COND        8
#define CMD_SEND_CSD            9
#define CMD_SEND_CID           10
#define CMD_STOP_TRANSMISSION  12
#define CMD_SEND_STATUS        13
#define CMD_SET_BLOCKLEN       16
#define CMD_READ_SINGLE        17
#define CMD_READ_MULTIPLE      18
#define CMD_WRITE_SINGLE       24
#define CMD_WRITE_MULTIPLE     25
#define CMD_APP_CMD            55
#define CMD_READ_OCR           58

#define ACMD_SET_BUS_WIDTH      6
#define ACMD_SD_STATUS         13
#define ACMD_SEND_OP_COND      41

#define RESP_SHORT              CMDCTL_CMDRESP(1)
#define RESP_LONG               CMDCTL_CMDRESP(3)

#define BLOCK_SIZE              512
#define SD_TIMEOUT_MS           1000

// ---- card state ----
typedef enum {
    SD_STATE_NO_CARD = 0,
    SD_STATE_IDLE,
    SD_STATE_READY,
    SD_STATE_ERROR,
} sd_state_t;

static struct {
    sd_state_t state;
    uint32_t rca;            // relative card address
    uint32_t card_size_blocks;
    bool     high_capacity;  // SDHC/SDXC (block addressing)
    bool     initialized;
} sd_ctx;

static const char *sd_err_str;


// ---- GPIO / peripheral init ----
static void sd_gpio_init(void) {
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_SDIO);

    // SDIO pinout per the GD32F4xx datasheet (all AF12):
    //   CLK = PC12, D0 = PC8, D1 = PC9, D2 = PC10, D3 = PC11  -> GPIOC
    //   CMD = PD2                                              -> GPIOD
    uint32_t pins_c = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
    gpio_af_set(SD_GPIO_PORT, SD_AF, pins_c);
    gpio_mode_set(SD_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, pins_c);
    gpio_output_options_set(SD_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pins_c);

    // CMD lives on GPIOD, not GPIOC.
    gpio_af_set(SD_CMD_PORT, SD_AF, SD_CMD_PIN);
    gpio_mode_set(SD_CMD_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SD_CMD_PIN);
    gpio_output_options_set(SD_CMD_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SD_CMD_PIN);

    // card detect input: PD3, pull-up enabled, low when a card is inserted
    // (R20 pulls the line to +3V3; the socket grounds it when a card is in).
    gpio_mode_set(SD_CD_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, SD_CD_PIN);
}

bool sdcard_is_present(void) {
    // active-low CD pin
    return gpio_input_bit_get(SD_CD_PORT, SD_CD_PIN) == RESET;
}

// ---- low-level command helpers ----
static bool sd_wait_flag(uint32_t flag, FlagStatus status, uint32_t timeout_ms) {
    mp_uint_t start = mp_hal_ticks_ms();
    while (sdio_flag_get(flag) != status) {
        if ((mp_hal_ticks_ms() - start) > timeout_ms) {
            return false;
        }
    }
    return true;
}

// Clear every latched status bit before starting a new command or transfer.
//
// The completion flags CMDSENT/CMDREND/DTEND latch until explicitly cleared by
// writing SDIO_INTC.  Leaving them set makes the next wait return immediately
// with whatever value is still in the response registers: after CMD8 every
// following command "succeeded" instantly and reported the CMD8 echo (0x1AA)
// instead of the real response, so ACMD41's ready bit never appeared and the
// driver blamed the card ("card init failed (ACMD41)").
static void sd_clear_flags(void) {
    sdio_flag_clear(SDIO_FLAG_CCRCERR | SDIO_FLAG_CMDTMOUT | SDIO_FLAG_DTCRCERR |
                    SDIO_FLAG_DTTMOUT | SDIO_FLAG_RXORE | SDIO_FLAG_TXURE |
                    SDIO_FLAG_STBITE | SDIO_FLAG_CMDRECV | SDIO_FLAG_CMDSEND |
                    SDIO_FLAG_DTEND | SDIO_FLAG_DTBLKEND);
}

// Send a command and wait for it to complete.
//
// The two completion flags are NOT interchangeable:
//   CMDSENT (SDIO_FLAG_CMDSEND) - set only for commands that expect NO response
//   CMDREND (SDIO_FLAG_CMDRECV) - set when a response was received and its CRC
//                                 check passed
//
// This function used to wait for CMDSEND unconditionally, so every command that
// expects a response (CMD8, CMD55, ACMD41, CMD2, CMD3, CMD9, CMD7, CMD16 and
// all the data commands) sat there until the timeout.  Only CMD0 - the single
// no-response command in the init sequence - ever succeeded, which is why the
// whole init reported the unhelpful "card init failed (ACMD41)".
//
// `check_crc` exists for R3 responses (ACMD41): a R3 reply carries no CRC
// field, so the controller's CRC check fails on a perfectly good response and
// must be ignored.
static int sd_cmd_internal(uint8_t cmd, uint32_t arg, uint32_t resp_type, bool check_crc) {
    sd_clear_flags();
    sdio_command_response_config(cmd, arg, resp_type);
    sdio_wait_type_set(SDIO_WAITTYPE_NO);
    sdio_csm_enable();

    if (resp_type == CMDCTL_CMDRESP(0)) {
        if (!sd_wait_flag(SDIO_FLAG_CMDSEND, SET, SD_TIMEOUT_MS)) {
            sd_err_str = "CMDSEND timeout";
            return -1;
        }
        return 0;
    }

    mp_uint_t start = mp_hal_ticks_ms();
    for (;;) {
        if (sdio_flag_get(SDIO_FLAG_CMDRECV) != RESET) {
            return 0;
        }
        if (sdio_flag_get(SDIO_FLAG_CCRCERR) != RESET) {
            // The response arrived; only its CRC is considered bad.
            if (!check_crc) {
                return 0;
            }
            sd_err_str = "CRC error";
            return -2;
        }
        if (sdio_flag_get(SDIO_FLAG_CMDTMOUT) != RESET) {
            sd_err_str = "no response";
            return -1;
        }
        if ((mp_hal_ticks_ms() - start) > SD_TIMEOUT_MS) {
            sd_err_str = "CMDRECV timeout";
            return -1;
        }
    }
}

// Start a command without waiting for its response.
//
// Needed by the write path: the data path state machine has to be enabled
// before the command (that is how the reference STM32 SDIO driver does it), and
// it starts transmitting as soon as the card answers.  Waiting for that answer
// before pushing the first word loses the race - the transmitter underruns
// immediately (STAT.TXURE, DATACNT frozen a few bytes in).
static void sd_cmd_start(uint8_t cmd, uint32_t arg, uint32_t resp_type) {
    sd_clear_flags();
    sdio_command_response_config(cmd, arg, resp_type);
    sdio_wait_type_set(SDIO_WAITTYPE_NO);
    sdio_csm_enable();
}

// Wait for the response of a command started with sd_cmd_start().
static int sd_cmd_finish(uint32_t resp_type) {
    mp_uint_t start = mp_hal_ticks_ms();
    for (;;) {
        if (sdio_flag_get(SDIO_FLAG_CMDRECV) != RESET) {
            return 0;
        }
        if (sdio_flag_get(SDIO_FLAG_CCRCERR) != RESET) {
            sd_err_str = "CRC error";
            return -2;
        }
        if (sdio_flag_get(SDIO_FLAG_CMDTMOUT) != RESET) {
            sd_err_str = "no response";
            return -1;
        }
        if ((mp_hal_ticks_ms() - start) > SD_TIMEOUT_MS) {
            sd_err_str = "CMDRECV timeout";
            return -1;
        }
    }
    (void)resp_type;
}

static int sd_cmd(uint8_t cmd, uint32_t arg, uint32_t resp_type) {
    return sd_cmd_internal(cmd, arg, resp_type, true);
}

// R3 response (ACMD41): no CRC field, do not fail on a CRC mismatch.
static int sd_cmd_r3(uint8_t cmd, uint32_t arg) {
    return sd_cmd_internal(cmd, arg, RESP_SHORT, false);
}

static uint32_t sd_resp(uint32_t index) {
    return sdio_response_get(index);
}

// Waits until the card reports READY_FOR_DATA (R1 bit 8).
//
// After a write the card keeps DAT0 low while it programs internally; polling
// CMD13 is the defined way to wait for that, and it also catches write errors
// instead of silently ignoring them.
static void sd_wait_ready(void) {
    for (int i = 0; i < 500; i++) {
        if (sd_cmd(CMD_SEND_STATUS, sd_ctx.rca, RESP_SHORT) != 0) {
            return;
        }
        if (sd_resp(SDIO_RESPONSE0) & (1u << 8)) {
            return;
        }
        mp_hal_delay_ms(1);
    }
}

// ---- data transfer ----
//
// Two things here were wrong and between them guaranteed failure:
//
//  * sdio_data_config()'s first argument is the DATA TIMEOUT in card clock
//    cycles, not a block count.  The old code passed 0x03, i.e. three clock
//    cycles, so DTIMEOUT fired before any byte could arrive.
//  * The data path state machine must be armed BEFORE the command is sent.  The
//    card starts pushing data as soon as it has answered the R1 response, and
//    anything that arrives while DPSM is disabled is lost.  The old code
//    enabled it only after sd_cmd() had already returned.
#define SD_DATA_TIMEOUT      (0x00FFFFFFu)

static int sd_read_blocks(uint32_t addr, uint8_t *buf, uint32_t blocks) {
    uint32_t start_addr = sd_ctx.high_capacity ? addr : addr * BLOCK_SIZE;
    uint32_t total = blocks * BLOCK_SIZE;

    sd_clear_flags();
    sdio_data_config(SD_DATA_TIMEOUT, total, SDIO_DATABLOCKSIZE_512BYTES);
    sdio_data_transfer_config(SDIO_TRANSMODE_BLOCK, SDIO_TRANSDIRECTION_TOSDIO);
    sdio_dsm_enable();

    if (sd_cmd(blocks == 1 ? CMD_READ_SINGLE : CMD_READ_MULTIPLE,
               start_addr, RESP_SHORT) != 0) {
        sdio_dsm_disable();
        return -1;
    }

    for (uint32_t i = 0; i < total; i += 4) {
        if (!sd_wait_flag(SDIO_FLAG_RXDTVAL, SET, SD_TIMEOUT_MS)) {
            sd_err_str = "RX data timeout";
            sdio_dsm_disable();
            return -1;
        }
        uint32_t word = sdio_data_read();
        memcpy(&buf[i], &word, 4);
    }

    if (!sd_wait_flag(SDIO_FLAG_DTEND, SET, SD_TIMEOUT_MS)) {
        sd_err_str = "DTEND timeout";
        sdio_dsm_disable();
        return -1;
    }
    sdio_dsm_disable();

    // A multiple-block read only stops when told to; without CMD12 the card
    // keeps streaming.  A single-block read terminates on its own.
    if (blocks > 1) {
        sd_cmd(CMD_STOP_TRANSMISSION, 0, RESP_SHORT);
        sd_wait_ready();                                      // card releases DAT0 when done
    }
    return 0;
}

static int sd_write_blocks(uint32_t addr, const uint8_t *buf, uint32_t blocks) {
    uint32_t start_addr = sd_ctx.high_capacity ? addr : addr * BLOCK_SIZE;
    uint32_t total = blocks * BLOCK_SIZE;

    // Reset the data path first: disabling it flushes the transmit FIFO, so a
    // write never starts with leftovers from an earlier transfer.  Without this
    // the preload below silently drops words and the transfer stalls with
    // "TX data timeout".
    sdio_dsm_disable();
    sd_clear_flags();
    sdio_data_config(SD_DATA_TIMEOUT, total, SDIO_DATABLOCKSIZE_512BYTES);
    sdio_data_transfer_config(SDIO_TRANSMODE_BLOCK, SDIO_TRANSDIRECTION_TOCARD);

    // Queue the first words while the data path is still disabled: the TX FIFO
    // accepts data with DPSM off.  Enabling DPSM with an empty FIFO makes the
    // transmitter start and underrun immediately, and pushing only after
    // enabling loses the same race.  So preload, then arm.
    //
    // The preload must stop as soon as the FIFO reports full (TFF): a write to
    // a full FIFO is silently dropped, which shifts every following byte by one
    // word and makes the card reject the block with a CRC error.
    uint32_t i = 0;
    while (i < total && sdio_flag_get(SDIO_FLAG_TFF) == RESET) {
        uint32_t word;
        memcpy(&word, &buf[i], 4);
        sdio_data_write(word);
        i += 4;
    }

    sd_cmd_start(blocks == 1 ? CMD_WRITE_SINGLE : CMD_WRITE_MULTIPLE,
                 start_addr, RESP_SHORT);
    sdio_dsm_enable();

    for (; i < total; i += 4) {
        // Mirror the reference driver: wait until the transmit FIFO reports at
        // least eight free words (TFH) before handing it the next one.  If that
        // times out the words may already have been consumed, so fall through to
        // the DATACNT check below instead of failing outright.
        if (!sd_wait_flag(SDIO_FLAG_TFH, SET, SD_TIMEOUT_MS)) {
            break;
        }
        uint32_t word;
        memcpy(&word, &buf[i], 4);
        sdio_data_write(word);
    }

    // The R1 response has been waiting since the data phase began; collect it
    // and act on the card's own verdict instead of ignoring a rejected write.
    if (sd_cmd_finish(RESP_SHORT) != 0) {
        sdio_dsm_disable();
        return -1;
    }
    uint32_t r1 = sd_resp(SDIO_RESPONSE0);
    if (r1 & 0x0FFE0000u) {      // OUT_OF_RANGE .. UNDERRUN error bits
        sd_err_str = "write rejected";
        sdio_dsm_disable();
        return -1;
    }

    // Completion is judged from DATACNT (bytes still to move) plus the error
    // flags.  Waiting for DTEND alone is unreliable here: the transfer can
    // finish with DATACNT already at zero while DTEND never latches, which used
    // to be reported as "DTEND timeout" even though every byte had been sent.
    uint32_t left = SDIO_DATACNT & 0x01FFFFFFu;
    mp_uint_t t0 = mp_hal_ticks_ms();
    while (left != 0 && (mp_hal_ticks_ms() - t0) <= SD_TIMEOUT_MS) {
        left = SDIO_DATACNT & 0x01FFFFFFu;
    }
    uint32_t errf = SDIO_STAT & (SDIO_FLAG_TXURE | SDIO_FLAG_DTCRCERR | SDIO_FLAG_DTTMOUT);
    sdio_dsm_disable();

    if (errf != 0) {
        sd_err_str = "data transfer error";
        return -1;
    }
    if (left != 0) {
        sd_err_str = "DTEND timeout";
        return -1;
    }

    if (blocks > 1) {
        sd_cmd(CMD_STOP_TRANSMISSION, 0, RESP_SHORT);
    }

    // Wait for the card to finish its internal programming before returning.
    sd_wait_ready();
    return 0;
}

// ---- card init ----
static int sdcard_do_init(void) {
    sd_err_str = NULL;
    sd_ctx.state = SD_STATE_IDLE;
    sd_ctx.initialized = false;

    sd_gpio_init();

    // power up at 400 kHz
    sdio_deinit();
    sdio_power_state_set(SDIO_POWER_ON);
    mp_hal_delay_ms(2);

    sdio_clock_config(SDIO_SDIOCLKEDGE_RISING, SDIO_CLOCKBYPASS_DISABLE,
                      SDIO_CLOCKPWRSAVE_DISABLE, SD_INIT_CLK_DIV);
    sdio_bus_mode_set(SDIO_BUSMODE_1BIT);
    sdio_clock_enable();
    mp_hal_delay_ms(2);

    // CMD0: reset card
    if (sd_cmd(CMD_GO_IDLE_STATE, 0, CMDCTL_CMDRESP(0)) != 0) {
        sd_ctx.state = SD_STATE_ERROR;
        return -1;
    }
    mp_hal_delay_ms(2);

    // CMD8: interface condition (SD v2)
    int r = sd_cmd(CMD_SEND_IF_COND, 0x1AA, RESP_SHORT);
    bool sdhc = false;
    if (r == 0 && (sd_resp(SDIO_RESPONSE0) & 0xFFF) == 0x1AA) {
        sdhc = true;
    }

    // ACMD41: wait for card ready
    for (int i = 0; i < 100; i++) {
        int rc55 = sd_cmd(CMD_APP_CMD, 0, RESP_SHORT);
        if (rc55 != 0) {
            break;
        }
        uint32_t arg = 0x40FF8000;   // HCS | voltage window
        // R3 response -> no CRC field in the reply, hence sd_cmd_r3().
        int rc41 = sd_cmd_r3(ACMD_SEND_OP_COND, arg);
        if (rc41 != 0) {
            break;
        }
        uint32_t ocr = sd_resp(SDIO_RESPONSE0);
        if (ocr & 0x80000000) {
            sd_ctx.high_capacity = (ocr & 0x40000000) != 0;
            sd_ctx.state = SD_STATE_READY;
            break;
        }
        mp_hal_delay_ms(10);
    }

    if (sd_ctx.state != SD_STATE_READY) {
        // Keep whatever sd_cmd_internal() recorded; replacing it with a generic
        // message is what hid the real cause (a bogus CMDSEND wait) behind
        // "card init failed (ACMD41)" for so long.
        if (sd_err_str == NULL) {
            sd_err_str = "card init failed (ACMD41)";
        }
        sd_ctx.state = SD_STATE_ERROR;
        return -1;
    }
    (void)sdhc;

    // CMD2: get CID
    if (sd_cmd(CMD_ALL_SEND_CID, 0, RESP_LONG) != 0) {
        sd_ctx.state = SD_STATE_ERROR;
        return -1;
    }

    // CMD3: get RCA
    if (sd_cmd(CMD_SEND_REL_ADDR, 0, RESP_SHORT) != 0) {
        sd_ctx.state = SD_STATE_ERROR;
        return -1;
    }
    sd_ctx.rca = sd_resp(SDIO_RESPONSE0) & 0xFFFF0000;

    // CMD9: read CSD to get capacity
    if (sd_cmd(CMD_SEND_CSD, sd_ctx.rca, RESP_LONG) != 0) {
        sd_ctx.state = SD_STATE_ERROR;
        return -1;
    }
    // A long response is split across all four response registers, MSB first:
    //   RESP0 = CSD[127:96], RESP1 = CSD[95:64],
    //   RESP2 = CSD[ 63:32], RESP3 = CSD[31:0]
    // The capacity fields straddle RESP1/RESP2, so reading RESP2 is essential.
    // The previous code used RESP0 (the top 32 bits of the CID-ish header) for
    // the low half of both C_SIZE and READ_BL_LEN, which produced a nonsense
    // block count - and a wrong block count breaks every filesystem.
    uint32_t csd1 = sd_resp(SDIO_RESPONSE1);
    uint32_t csd2 = sd_resp(SDIO_RESPONSE2);
    if (sd_ctx.high_capacity) {
        // CSD v2: C_SIZE = CSD[69:48]
        uint32_t c_size = ((csd1 & 0x3F) << 16) | ((csd2 >> 16) & 0xFFFF);
        sd_ctx.card_size_blocks = (c_size + 1) * 1024;
    } else {
        // CSD v1: READ_BL_LEN = CSD[83:80], C_SIZE = CSD[73:62]
        uint32_t read_bl_len = (csd1 >> 16) & 0xF;
        uint32_t c_size = ((csd1 & 0x3FF) << 2) | ((csd2 >> 30) & 0x3);
        if (read_bl_len < 9) {
            // Block size below 512 bytes: the driver always moves 512-byte
            // blocks, so report at least one block instead of shifting by a
            // negative amount.
            sd_ctx.card_size_blocks = (c_size + 1);
        } else {
            sd_ctx.card_size_blocks = (c_size + 1) << (read_bl_len - 9);
        }
    }

    // CMD7: select card
    if (sd_cmd(CMD_SELECT_CARD, sd_ctx.rca, RESP_SHORT) != 0) {
        sd_ctx.state = SD_STATE_ERROR;
        return -1;
    }

    // ACMD6 + CMD16: 4-bit bus, 512-byte blocks
    if (sd_cmd(CMD_APP_CMD, sd_ctx.rca, RESP_SHORT) == 0) {
        sd_cmd(ACMD_SET_BUS_WIDTH, 0x2, RESP_SHORT);   // 4-bit
    }
    sd_cmd(CMD_SET_BLOCKLEN, BLOCK_SIZE, RESP_SHORT);

    // switch to high speed clock
    sdio_clock_config(SDIO_SDIOCLKEDGE_RISING, SDIO_CLOCKBYPASS_DISABLE,
                      SDIO_CLOCKPWRSAVE_DISABLE, SD_MAX_CLK_DIV);
    sdio_bus_mode_set(SDIO_BUSMODE_4BIT);

    sd_ctx.initialized = true;
    return 0;
}

// ---- public API ----
int sdcard_init(void) {
    if (!sdcard_is_present()) {
        sd_ctx.state = SD_STATE_NO_CARD;
        return -1;
    }
    return sdcard_do_init();
}

bool sdcard_ready(void) {
    if (!sdcard_is_present()) {
        sd_ctx.state = SD_STATE_NO_CARD;
        sd_ctx.initialized = false;
        return false;
    }
    return sd_ctx.initialized;
}

const char *sdcard_error(void) {
    return sd_err_str ? sd_err_str : "";
}

uint32_t sdcard_block_count(void) {
    return sd_ctx.card_size_blocks;
}

int sdcard_read(uint32_t block, uint8_t *buf, uint32_t count) {
    if (!sd_ctx.initialized) {
        return -1;
    }
    return sd_read_blocks(block, buf, count);
}

int sdcard_write(uint32_t block, const uint8_t *buf, uint32_t count) {
    if (!sd_ctx.initialized) {
        return -1;
    }
    return sd_write_blocks(block, buf, count);
}

bool sdcard_is_high_capacity(void) {
    return sd_ctx.high_capacity;
}

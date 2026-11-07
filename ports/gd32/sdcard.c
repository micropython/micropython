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
#define SD_CD_PORT      GPIOD
#define SD_CD_RCU       RCU_GPIOD
#define SD_CD_PIN       GPIO_PIN_13      // card detect, active low

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

    // card detect input
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

static void sd_clear_flags(void) {
    sdio_flag_clear(SDIO_FLAG_CCRCERR | SDIO_FLAG_CMDTMOUT | SDIO_FLAG_DTCRCERR |
                    SDIO_FLAG_DTTMOUT | SDIO_FLAG_RXORE | SDIO_FLAG_TXURE |
                    SDIO_FLAG_STBITE);
}

// send a command, optionally waiting for a response
static int sd_cmd(uint8_t cmd, uint32_t arg, uint32_t resp_type) {
    sd_clear_flags();
    sdio_command_response_config(cmd, arg, resp_type);
    sdio_wait_type_set(SDIO_WAITTYPE_NO);
    sdio_csm_enable();

    if (!sd_wait_flag(SDIO_FLAG_CMDSEND, SET, SD_TIMEOUT_MS)) {
        sd_err_str = "CMDSEND timeout";
        return -1;
    }

    if (resp_type != CMDCTL_CMDRESP(0)) {
        if (!sd_wait_flag(SDIO_FLAG_CMDRECV, SET, SD_TIMEOUT_MS)) {
            sd_err_str = "CMDRECV timeout";
            return -1;
        }
        if (sdio_flag_get(SDIO_FLAG_CCRCERR) != RESET) {
            sd_err_str = "CRC error";
            return -2;
        }
    }
    return 0;
}

static uint32_t sd_resp(uint32_t index) {
    return sdio_response_get(index);
}

// ---- data transfer ----
static int sd_read_blocks(uint32_t addr, uint8_t *buf, uint32_t blocks) {
    uint32_t start_addr = sd_ctx.high_capacity ? addr : addr * BLOCK_SIZE;

    sd_clear_flags();
    if (blocks == 1) {
        if (sd_cmd(CMD_READ_SINGLE, start_addr, RESP_SHORT) != 0) {
            return -1;
        }
    } else {
        if (sd_cmd(CMD_READ_MULTIPLE, start_addr, RESP_SHORT) != 0) {
            return -1;
        }
    }

    sdio_data_config(0x03, blocks * BLOCK_SIZE, SDIO_DATABLOCKSIZE_512BYTES);
    sdio_data_transfer_config(SDIO_TRANSMODE_BLOCK, SDIO_TRANSDIRECTION_TOSDIO);
    sdio_dsm_enable();

    uint32_t total = blocks * BLOCK_SIZE;
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
    return 0;
}

static int sd_write_blocks(uint32_t addr, const uint8_t *buf, uint32_t blocks) {
    uint32_t start_addr = sd_ctx.high_capacity ? addr : addr * BLOCK_SIZE;

    sd_clear_flags();
    if (blocks == 1) {
        if (sd_cmd(CMD_WRITE_SINGLE, start_addr, RESP_SHORT) != 0) {
            return -1;
        }
    } else {
        if (sd_cmd(CMD_WRITE_MULTIPLE, start_addr, RESP_SHORT) != 0) {
            return -1;
        }
    }

    sdio_data_config(0x03, blocks * BLOCK_SIZE, SDIO_DATABLOCKSIZE_512BYTES);
    sdio_data_transfer_config(SDIO_TRANSMODE_BLOCK, SDIO_TRANSDIRECTION_TOCARD);
    sdio_dsm_enable();

    uint32_t total = blocks * BLOCK_SIZE;
    for (uint32_t i = 0; i < total; i += 4) {
        if (!sd_wait_flag(SDIO_FLAG_TXDTVAL, SET, SD_TIMEOUT_MS)) {
            sd_err_str = "TX data timeout";
            sdio_dsm_disable();
            return -1;
        }
        uint32_t word;
        memcpy(&word, &buf[i], 4);
        sdio_data_write(word);
    }

    if (!sd_wait_flag(SDIO_FLAG_DTEND, SET, SD_TIMEOUT_MS)) {
        sd_err_str = "DTEND timeout";
        sdio_dsm_disable();
        return -1;
    }
    sdio_dsm_disable();
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
        if (sd_cmd(CMD_APP_CMD, 0, RESP_SHORT) != 0) {
            break;
        }
        uint32_t arg = 0x40FF8000;   // HCS | voltage window
        if (sd_cmd(ACMD_SEND_OP_COND, arg, RESP_SHORT) != 0) {
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
        sd_err_str = "card init failed (ACMD41)";
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
    uint32_t csd0 = sd_resp(SDIO_RESPONSE0);
    uint32_t csd1 = sd_resp(SDIO_RESPONSE1);
    (void)csd0;
    if (sd_ctx.high_capacity) {
        // CSD v2: C_SIZE in bits 69:48
        uint32_t c_size = ((csd1 & 0x3FFFFF00) >> 8) | ((csd0 >> 24) & 0xFF) << 16;
        sd_ctx.card_size_blocks = (c_size + 1) * 1024;
    } else {
        // CSD v1: approximate using READ_BL_LEN and C_SIZE
        uint32_t read_bl_len = (csd1 >> 16) & 0xF;
        uint32_t c_size = ((csd1 & 0x3FF) << 2) | ((csd0 >> 30) & 0x3);
        sd_ctx.card_size_blocks = (c_size + 1) << (read_bl_len + 2 - 9);
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

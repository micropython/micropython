// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "soc/soc_caps.h"
#if SOC_SDMMC_HOST_SUPPORTED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "unity.h"

/* Second ESP32 board attached as follows:
 *   Master     Slave
 *   IO18        EN
 *   IO19        IO0
 *   IO14        SD_CLK
 *   IO15        SD_CMD
 *   IO2         SD_D0
 *   IO4         SD_D1
 *   IO12        SD_D2
 *   IO13        SD_D3
 */


/* TODO: add SDIO slave header files, remove these definitions */

#define DR_REG_SLC_MASK 0xfffffc00

#define SLCCONF1 (DR_REG_SLC_BASE + 0x60)
#define SLC_SLC0_RX_STITCH_EN (BIT(6))
#define SLC_SLC0_TX_STITCH_EN (BIT(5))

#define SLC0TX_LINK (DR_REG_SLC_BASE + 0x40)
#define SLC_SLC0_TXLINK_PARK (BIT(31))
#define SLC_SLC0_TXLINK_RESTART (BIT(30))
#define SLC_SLC0_TXLINK_START (BIT(29))

#define DR_REG_SLCHOST_MASK 0xfffffc00
#define SLCHOST_STATE_W0 (DR_REG_SLCHOST_BASE + 0x64)
#define SLCHOST_CONF_W0 (DR_REG_SLCHOST_BASE + 0x6C)
#define SLCHOST_CONF_W5 (DR_REG_SLCHOST_BASE + 0x80)
#define SLCHOST_WIN_CMD (DR_REG_SLCHOST_BASE + 0x84)

#define SLC_WIN_CMD_READ    0x80
#define SLC_WIN_CMD_WRITE   0xC0
#define SLC_WIN_CMD_S       8

#define SLC_THRESHOLD_ADDR 0x1f800

static const char* TAG = "sdio_test";

static esp_err_t slave_slchost_reg_read(sdmmc_card_t* card, uint32_t addr, uint32_t* out_val)
{
    if ((addr & DR_REG_SLCHOST_MASK) != DR_REG_SLCHOST_BASE) {
        ESP_LOGW(TAG, "%s: invalid addr 0x%08x\n", __func__, addr);
        return ESP_ERR_INVALID_ARG;
    }
    return sdmmc_io_read_bytes(card, 1, addr & (~DR_REG_SLCHOST_MASK), out_val, sizeof(*out_val));
}

static esp_err_t slave_slchost_reg_write(sdmmc_card_t* card, uint32_t addr, uint32_t val)
{
    if ((addr & DR_REG_SLCHOST_MASK) != DR_REG_SLCHOST_BASE) {
        ESP_LOGW(TAG, "%s: invalid addr 0x%08x\n", __func__, addr);
        return ESP_ERR_INVALID_ARG;
    }
    return sdmmc_io_write_bytes(card, 1, addr & (~DR_REG_SLCHOST_MASK), &val, sizeof(val));
}

static esp_err_t slave_slc_reg_read(sdmmc_card_t* card, uint32_t addr, uint32_t* val)
{
    if ((addr & DR_REG_SLC_MASK) != DR_REG_SLC_BASE) {
        ESP_LOGW(TAG, "%s: invalid addr 0x%08x\n", __func__, addr);
        return ESP_ERR_INVALID_ARG;
    }
    uint32_t word = (addr - DR_REG_SLC_BASE) / 4;
    if (word > INT8_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t window_command = word | (SLC_WIN_CMD_READ << SLC_WIN_CMD_S);
    esp_err_t err = slave_slchost_reg_write(card, SLCHOST_WIN_CMD, window_command);
    if (err != ESP_OK) {
        return err;
    }

    return slave_slchost_reg_read(card, SLCHOST_STATE_W0, val);
}

static esp_err_t slave_slc_reg_write(sdmmc_card_t* card, uint32_t addr, uint32_t val)
{
    if ((addr & DR_REG_SLC_MASK) != DR_REG_SLC_BASE) {
        ESP_LOGW(TAG, "%s: invalid addr 0x%08x\n", __func__, addr);
        return ESP_ERR_INVALID_ARG;
    }
    uint32_t word = (addr - DR_REG_SLC_BASE) / 4;
    if (word > INT8_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = slave_slchost_reg_write(card, SLCHOST_CONF_W5, val);
    if (err != ESP_OK) {
        return err;
    }

    uint32_t window_command = word | (SLC_WIN_CMD_WRITE << SLC_WIN_CMD_S);
    return slave_slchost_reg_write(card, SLCHOST_WIN_CMD, window_command);
}

/** Reset and put slave into download mode */
static void reset_slave(void)
{
    const int pin_en = 18;
    const int pin_io0 = 19;
    gpio_config_t gpio_cfg = {
            .pin_bit_mask = BIT64(pin_en) | BIT64(pin_io0),
            .mode = GPIO_MODE_OUTPUT_OD,
    };
    TEST_ESP_OK(gpio_config(&gpio_cfg));
    gpio_set_level(pin_en, 0);
    gpio_set_level(pin_io0, 0);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    gpio_set_level(pin_en, 1);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    gpio_set_level(pin_io0, 1);
}

static void sdio_slave_common_init(sdmmc_card_t* card)
{
    uint8_t card_cap;
    esp_err_t err = sdmmc_io_read_byte(card, 0, SD_IO_CCCR_CARD_CAP, &card_cap);
    TEST_ESP_OK(err);
    printf("CAP: 0x%02x\n", card_cap);

    uint8_t hs;
    err = sdmmc_io_read_byte(card, 0, SD_IO_CCCR_HIGHSPEED, &hs);
    TEST_ESP_OK(err);
    printf("HS: 0x%02x\n", hs);


#define FUNC1_EN_MASK   (BIT(1))

    uint8_t ioe;
    err = sdmmc_io_read_byte(card, 0, SD_IO_CCCR_FN_ENABLE, &ioe);
    TEST_ESP_OK(err);
    printf("IOE: 0x%02x\n", ioe);

    uint8_t ior = 0;
    err = sdmmc_io_read_byte(card, 0, SD_IO_CCCR_FN_READY, &ior);
    TEST_ESP_OK(err);
    printf("IOR: 0x%02x\n", ior);

    // enable function 1
    ioe |= FUNC1_EN_MASK;
    err = sdmmc_io_write_byte(card, 0, SD_IO_CCCR_FN_ENABLE, ioe, NULL);
    TEST_ESP_OK(err);

    err = sdmmc_io_read_byte(card, 0, SD_IO_CCCR_FN_ENABLE, &ioe);
    TEST_ESP_OK(err);
    printf("IOE: 0x%02x\n", ioe);

    // wait for the card to become ready
    while ( (ior & FUNC1_EN_MASK) == 0 ) {
        err = sdmmc_io_read_byte(card, 0, SD_IO_CCCR_FN_READY, &ior);
        TEST_ESP_OK(err);
        printf("IOR: 0x%02x\n", ior);
    }

    // get interrupt status
    uint8_t ie;
    err = sdmmc_io_read_byte(card, 0, SD_IO_CCCR_INT_ENABLE, &ie);
    TEST_ESP_OK(err);
    printf("IE: 0x%02x\n", ie);

    // enable interrupts for function 1&2 and master enable
    ie |= BIT(0) | FUNC1_EN_MASK;
    err = sdmmc_io_write_byte(card, 0, SD_IO_CCCR_INT_ENABLE, ie, NULL);
    TEST_ESP_OK(err);

    err = sdmmc_io_read_byte(card, 0, SD_IO_CCCR_INT_ENABLE, &ie);
    TEST_ESP_OK(err);
    printf("IE: 0x%02x\n", ie);
}

/** Common for all SDIO devices, set block size for specific function */
static void sdio_slave_set_blocksize(sdmmc_card_t* card, int function, uint16_t bs)
{
    const uint8_t* bs_u8 = (const uint8_t*) &bs;
    uint16_t bs_read = 0;
    uint8_t* bs_read_u8 = (uint8_t*) &bs_read;
    uint32_t offset = SD_IO_FBR_START * function;
    TEST_ESP_OK( sdmmc_io_write_byte(card, 0, offset + SD_IO_CCCR_BLKSIZEL, bs_u8[0], NULL));
    TEST_ESP_OK( sdmmc_io_write_byte(card, 0, offset + SD_IO_CCCR_BLKSIZEH, bs_u8[1], NULL));
    TEST_ESP_OK( sdmmc_io_read_byte(card, 0, offset + SD_IO_CCCR_BLKSIZEL, &bs_read_u8[0]));
    TEST_ESP_OK( sdmmc_io_read_byte(card, 0, offset + SD_IO_CCCR_BLKSIZEH, &bs_read_u8[1]));
    TEST_ASSERT_EQUAL_HEX16(bs, bs_read);
}

/**
 * ESP32 ROM code does not set some SDIO slave registers to the defaults
 * we need, this function clears/sets some bits.
 */
static void esp32_slave_init_extra(sdmmc_card_t* card)
{
    printf("Initialize some ESP32 SDIO slave registers\n");

    uint32_t reg_val;
    TEST_ESP_OK( slave_slc_reg_read(card, SLCCONF1, &reg_val) );
    reg_val &= ~(SLC_SLC0_RX_STITCH_EN | SLC_SLC0_TX_STITCH_EN);
    TEST_ESP_OK( slave_slc_reg_write(card, SLCCONF1, reg_val) );

    TEST_ESP_OK( slave_slc_reg_read(card, SLC0TX_LINK, &reg_val) );
    reg_val |= SLC_SLC0_TXLINK_START;
    TEST_ESP_OK( slave_slc_reg_write(card, SLC0TX_LINK, reg_val) );
}

/**
 * ESP32 bootloader implements "SIP" protocol which can be used to exchange
 * some commands, events, and data packets between the host and the slave.
 * This function sends a SIP command, testing CMD53 block writes along the way.
 */
static void esp32_send_sip_command(sdmmc_card_t* card)
{
    printf("Test block write using CMD53\n");
    const size_t block_size = 512;
    uint8_t* data = heap_caps_calloc(1, block_size, MALLOC_CAP_DMA);
    struct sip_cmd_bootup {
            uint32_t boot_addr;
            uint32_t discard_link;
    };
    struct sip_cmd_write_reg {
            uint32_t addr;
            uint32_t val;
    };
    struct sip_hdr {
            uint8_t fc[2];
            uint16_t len;
            uint32_t cmdid;
            uint32_t seq;
    };

    struct sip_hdr* hdr = (struct sip_hdr*) data;
    size_t len;

#define SEND_WRITE_REG_CMD

#ifdef SEND_WRITE_REG_CMD
    struct sip_cmd_write_reg *write_reg = (struct sip_cmd_write_reg*) (data + sizeof(*hdr));
    len = sizeof(*hdr) + sizeof(*write_reg);
    hdr->cmdid = 3; /* SIP_CMD_WRITE_REG */
    write_reg->addr = GPIO_ENABLE_W1TS_REG;
    write_reg->val = BIT(0) | BIT(2) | BIT(4); /* Turn of RGB LEDs on WROVER-KIT */
#else
    struct sip_cmd_bootup *bootup = (struct sip_cmd_bootup*) (data + sizeof(*hdr));
    len = sizeof(*hdr) + sizeof(*bootup);
    hdr->cmdid = 5; /* SIP_CMD_BOOTUP */
    bootup->boot_addr = 0x4005a980; /* start_tb_console function in ROM */
    bootup->discard_link = 1;
#endif
    hdr->len = len;

    TEST_ESP_OK( sdmmc_io_write_blocks(card, 1, SLC_THRESHOLD_ADDR - len, data, block_size) );
    free(data);
}

static void test_cmd52_read_write_single_byte(sdmmc_card_t* card)
{
    esp_err_t err;
    printf("Write bytes to slave's W0_REG using CMD52\n");
    const size_t scratch_area_reg = SLCHOST_CONF_W0 - DR_REG_SLCHOST_BASE;

    const uint8_t test_byte_1 = 0xa5;
    const uint8_t test_byte_2 = 0xb6;
    // used to check Read-After-Write
    uint8_t test_byte_1_raw;
    uint8_t test_byte_2_raw;
    uint8_t val = 0;
    err = sdmmc_io_write_byte(card, 1, scratch_area_reg, test_byte_1, &test_byte_1_raw);
    TEST_ESP_OK(err);
    TEST_ASSERT_EQUAL_UINT8(test_byte_1, test_byte_1_raw);
    err = sdmmc_io_write_byte(card, 1, scratch_area_reg + 1, test_byte_2, &test_byte_2_raw);
    TEST_ESP_OK(err);
    TEST_ASSERT_EQUAL_UINT8(test_byte_2, test_byte_2_raw);

    printf("Read back bytes using CMD52\n");
    TEST_ESP_OK(sdmmc_io_read_byte(card, 1, scratch_area_reg, &val));
    TEST_ASSERT_EQUAL_UINT8(test_byte_1, val);

    TEST_ESP_OK(sdmmc_io_read_byte(card, 1, scratch_area_reg + 1, &val));
    TEST_ASSERT_EQUAL_UINT8(test_byte_2, val);
}

static void test_cmd53_read_write_multiple_bytes(sdmmc_card_t* card, size_t n_bytes)
{
    printf("Write multiple bytes using CMD53\n");
    const size_t scratch_area_reg = SLCHOST_CONF_W0 - DR_REG_SLCHOST_BASE;

    uint8_t* src = heap_caps_malloc(512, MALLOC_CAP_DMA);
    uint32_t* src_32 = (uint32_t*) src;

    for (size_t i = 0; i < (n_bytes + 3) / 4; ++i) {
        src_32[i] = rand();
    }

    TEST_ESP_OK(sdmmc_io_write_bytes(card, 1, scratch_area_reg, src, n_bytes));
    ESP_LOG_BUFFER_HEX(TAG, src, n_bytes);

    printf("Read back using CMD52\n");
    uint8_t* dst = heap_caps_malloc(512, MALLOC_CAP_DMA);
    for (size_t i = 0; i < n_bytes; ++i) {
        TEST_ESP_OK(sdmmc_io_read_byte(card, 1, scratch_area_reg + i, &dst[i]));
    }
    ESP_LOG_BUFFER_HEX(TAG, dst, n_bytes);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(src, dst, n_bytes);

    printf("Read back using CMD53\n");
    TEST_ESP_OK(sdmmc_io_read_bytes(card, 1, scratch_area_reg, dst, n_bytes));
    ESP_LOG_BUFFER_HEX(TAG, dst, n_bytes);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(src, dst, n_bytes);

    free(src);
    free(dst);
}


TEST_CASE("can probe and talk to ESP32 SDIO slave", "[sdio][ignore]")
{
    reset_slave();

    /* Probe */
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    config.flags = SDMMC_HOST_FLAG_1BIT;
    config.max_freq_khz = SDMMC_FREQ_PROBING;

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
     (sdmmc_host_init());
     (sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config));
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);
    TEST_ESP_OK(sdmmc_card_init(&config, card));
    sdmmc_card_print_info(stdout, card);

    /* Set up standard SDIO registers */
    sdio_slave_common_init(card);

    srand(0);
    for (int repeat = 0; repeat < 4; ++repeat) {
        test_cmd52_read_write_single_byte(card);
        test_cmd53_read_write_multiple_bytes(card, 1);
        test_cmd53_read_write_multiple_bytes(card, 2);
        test_cmd53_read_write_multiple_bytes(card, 3);
        test_cmd53_read_write_multiple_bytes(card, 4);
        test_cmd53_read_write_multiple_bytes(card, 5);
        test_cmd53_read_write_multiple_bytes(card, 23);
        test_cmd53_read_write_multiple_bytes(card, 24);
    }

    sdio_slave_set_blocksize(card, 0, 512);
    sdio_slave_set_blocksize(card, 1, 512);

    esp32_slave_init_extra(card);

    esp32_send_sip_command(card);

    TEST_ESP_OK(sdmmc_host_deinit());
    free(card);
}

#endif //SOC_SDMMC_HOST_SUPPORTED

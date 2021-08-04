// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include "unity.h"
#include "driver/gpio.h"
#include "soc/soc_caps.h"
#if SOC_SDMMC_HOST_SUPPORTED
#include "driver/sdmmc_host.h"
#endif
#include "driver/sdspi_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_rom_gpio.h"

// Can't test eMMC (slot 0) and PSRAM together
#ifndef CONFIG_SPIRAM
#define WITH_EMMC_TEST
#endif

/* power supply enable pin */
#define SD_TEST_BOARD_VSEL_EN_GPIO  27

/* power supply voltage select pin */
#define SD_TEST_BOARD_VSEL_GPIO     26
#define SD_TEST_BOARD_VSEL_3V3      1
#define SD_TEST_BOARD_VSEL_1V8      0

#define TEST_SDSPI_DMACHAN 1

/* time to wait for reset / power-on */
#define SD_TEST_BOARD_PWR_RST_DELAY_MS  5
#define SD_TEST_BOARD_PWR_ON_DELAY_MS   50

/* gpio which is not connected to actual CD pin, used to simulate CD behavior */
#define CD_WP_TEST_GPIO 18


__attribute__((unused)) static void sd_test_board_power_on(void)
{
    gpio_set_direction(SD_TEST_BOARD_VSEL_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(SD_TEST_BOARD_VSEL_GPIO, SD_TEST_BOARD_VSEL_3V3);
    gpio_set_direction(SD_TEST_BOARD_VSEL_EN_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(SD_TEST_BOARD_VSEL_EN_GPIO, 0);
    usleep(SD_TEST_BOARD_PWR_RST_DELAY_MS * 1000);
    gpio_set_level(SD_TEST_BOARD_VSEL_EN_GPIO, 1);
    usleep(SD_TEST_BOARD_PWR_ON_DELAY_MS * 1000);
}

__attribute__((unused)) static void sd_test_board_power_off(void)
{
    gpio_set_level(SD_TEST_BOARD_VSEL_EN_GPIO, 0);
    gpio_set_direction(SD_TEST_BOARD_VSEL_GPIO, GPIO_MODE_INPUT);
    gpio_set_level(SD_TEST_BOARD_VSEL_GPIO, 0);
    gpio_set_direction(SD_TEST_BOARD_VSEL_EN_GPIO, GPIO_MODE_INPUT);
}

TEST_CASE("MMC_RSP_BITS", "[sd]")
{
    uint32_t data[2] = { 0x01234567, 0x89abcdef };
    TEST_ASSERT_EQUAL_HEX32(0x7,   MMC_RSP_BITS(data, 0, 4));
    TEST_ASSERT_EQUAL_HEX32(0x567, MMC_RSP_BITS(data, 0, 12));
    TEST_ASSERT_EQUAL_HEX32(0xf0,  MMC_RSP_BITS(data, 28, 8));
    TEST_ASSERT_EQUAL_HEX32(0x3,   MMC_RSP_BITS(data, 1, 3));
    TEST_ASSERT_EQUAL_HEX32(0x11,  MMC_RSP_BITS(data, 59, 5));
}

#if SOC_SDMMC_HOST_SUPPORTED

static void probe_sd(int slot, int width, int freq_khz, int ddr)
{
    sd_test_board_power_on();
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    config.slot = slot;
    config.max_freq_khz = freq_khz;
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    if (width == 1) {
        config.flags = SDMMC_HOST_FLAG_1BIT;
        slot_config.width = 1;
    } else if (width == 4) {
        config.flags &= ~SDMMC_HOST_FLAG_8BIT;
        slot_config.width = 4;
    } else {
        assert(!ddr && "host driver does not support 8-line DDR mode yet");
    }
    if (!ddr) {
        config.flags &= ~SDMMC_HOST_FLAG_DDR;
    }
    TEST_ESP_OK(sdmmc_host_init());
    TEST_ESP_OK(sdmmc_host_init_slot(slot, &slot_config));
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);
    TEST_ESP_OK(sdmmc_card_init(&config, card));
    sdmmc_card_print_info(stdout, card);
    uint8_t* buffer = heap_caps_malloc(512, MALLOC_CAP_DMA);
    TEST_ESP_OK(sdmmc_read_sectors(card, buffer, 0, 1));
    free(buffer);
    TEST_ESP_OK(sdmmc_host_deinit());
    free(card);
    sd_test_board_power_off();
}

TEST_CASE("probe SD, slot 1, 4-bit", "[sd][test_env=UT_T1_SDMODE]")
{
    probe_sd(SDMMC_HOST_SLOT_1, 4, SDMMC_FREQ_PROBING, 0);
    probe_sd(SDMMC_HOST_SLOT_1, 4, SDMMC_FREQ_DEFAULT, 0);
    probe_sd(SDMMC_HOST_SLOT_1, 4, SDMMC_FREQ_HIGHSPEED, 0);
}

TEST_CASE("probe SD, slot 1, 1-bit", "[sd][test_env=UT_T1_SDMODE]")
{
    probe_sd(SDMMC_HOST_SLOT_1, 1, SDMMC_FREQ_PROBING, 0);
    probe_sd(SDMMC_HOST_SLOT_1, 1, SDMMC_FREQ_DEFAULT, 0);
    probe_sd(SDMMC_HOST_SLOT_1, 1, SDMMC_FREQ_HIGHSPEED, 0);
}

#ifdef WITH_EMMC_TEST
TEST_CASE("probe eMMC, slot 0, 4-bit, DDR", "[sd][test_env=EMMC]")
{
    probe_sd(SDMMC_HOST_SLOT_0, 4, SDMMC_FREQ_HIGHSPEED, 1);
}

TEST_CASE("probe eMMC, slot 0, 8-bit", "[sd][test_env=EMMC]")
{
    probe_sd(SDMMC_HOST_SLOT_0, 8, SDMMC_FREQ_PROBING, 0);
    probe_sd(SDMMC_HOST_SLOT_0, 8, SDMMC_FREQ_DEFAULT, 0);
    probe_sd(SDMMC_HOST_SLOT_0, 8, SDMMC_FREQ_HIGHSPEED, 0);
}
#endif // WITH_EMMC_TEST

TEST_CASE("probe SD, slot 0, 4-bit", "[sd][test_env=UT_T1_SDCARD][ignore]")
{
    probe_sd(SDMMC_HOST_SLOT_0, 4, SDMMC_FREQ_PROBING, 0);
    probe_sd(SDMMC_HOST_SLOT_0, 4, SDMMC_FREQ_DEFAULT, 0);
    probe_sd(SDMMC_HOST_SLOT_0, 4, SDMMC_FREQ_HIGHSPEED, 0);
}

TEST_CASE("probe SD, slot 0, 1-bit", "[sd][test_env=UT_T1_SDCARD][ignore]")
{
    probe_sd(SDMMC_HOST_SLOT_0, 1, SDMMC_FREQ_PROBING, 0);
    probe_sd(SDMMC_HOST_SLOT_0, 1, SDMMC_FREQ_DEFAULT, 0);
    probe_sd(SDMMC_HOST_SLOT_0, 1, SDMMC_FREQ_HIGHSPEED, 0);
}

#endif

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32C3)
//No runners
static void test_sdspi_init_bus(spi_host_device_t host, int mosi_pin, int miso_pin, int clk_pin, int dma_chan)
{
    spi_bus_config_t bus_config = {
        .mosi_io_num = mosi_pin,
        .miso_io_num = miso_pin,
        .sclk_io_num = clk_pin,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    esp_err_t err = spi_bus_initialize(host, &bus_config, dma_chan);
    TEST_ESP_OK(err);
}

static void test_sdspi_deinit_bus(spi_host_device_t host)
{
    esp_err_t err = spi_bus_free(host);
    TEST_ESP_OK(err);
}

static void probe_core(int slot)
{
    sdmmc_host_t config = SDSPI_HOST_DEFAULT();
    config.slot = slot;

    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);

    TEST_ESP_OK(sdmmc_card_init(&config, card));
    sdmmc_card_print_info(stdout, card);
    free(card);
}

static void probe_spi(int freq_khz, int pin_miso, int pin_mosi, int pin_sck, int pin_cs)
{
    sd_test_board_power_on();

    sdspi_dev_handle_t handle;
    sdspi_device_config_t dev_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    dev_config.gpio_cs = pin_cs;
    test_sdspi_init_bus(dev_config.host_id, pin_mosi, pin_miso, pin_sck, TEST_SDSPI_DMACHAN);
    TEST_ESP_OK(sdspi_host_init());
    TEST_ESP_OK(sdspi_host_init_device(&dev_config, &handle));

    probe_core(handle);

    TEST_ESP_OK(sdspi_host_deinit());
    test_sdspi_deinit_bus(dev_config.host_id);
    sd_test_board_power_off();
}

static void probe_spi_legacy(int freq_khz, int pin_miso, int pin_mosi, int pin_sck, int pin_cs)
{
    sd_test_board_power_on();
    sdmmc_host_t config = SDSPI_HOST_DEFAULT();
    sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
    slot_config.gpio_miso = pin_miso;
    slot_config.gpio_mosi = pin_mosi;
    slot_config.gpio_sck = pin_sck;
    slot_config.gpio_cs = pin_cs;

    TEST_ESP_OK(sdspi_host_init());
    TEST_ESP_OK(sdspi_host_init_slot(config.slot, &slot_config));

    probe_core(config.slot);

    TEST_ESP_OK(sdspi_host_deinit());
    sd_test_board_power_off();
}

TEST_CASE("probe SD in SPI mode, slot 1", "[sd][test_env=UT_T1_SPIMODE]")
{
    probe_spi(SDMMC_FREQ_DEFAULT, 2, 15, 14, 13);
    probe_spi_legacy(SDMMC_FREQ_DEFAULT, 2, 15, 14, 13);
}

TEST_CASE("probe SD in SPI mode, slot 0", "[sd][test_env=UT_T1_SDCARD][ignore]")
{
    probe_spi(SDMMC_FREQ_DEFAULT, 7, 11, 6, 10);
    probe_spi_legacy(SDMMC_FREQ_DEFAULT, 7, 11, 6, 10);
}

#endif //DISABLED(ESP32S2)

// Fill buffer pointed to by 'dst' with 'count' 32-bit ints generated
// from 'rand' with the starting value of 'seed'
__attribute__((unused)) static void fill_buffer(uint32_t seed, uint8_t* dst, size_t count) {
    srand(seed);
    for (size_t i = 0; i < count; ++i) {
        uint32_t val = rand();
        memcpy(dst + i * sizeof(uint32_t), &val, sizeof(val));
    }
}

// Check if the buffer pointed to by 'dst' contains 'count' 32-bit
// ints generated from 'rand' with the starting value of 'seed'
__attribute__((unused)) static void check_buffer(uint32_t seed, const uint8_t* src, size_t count) {
    srand(seed);
    for (size_t i = 0; i < count; ++i) {
        uint32_t val;
        memcpy(&val, src + i * sizeof(uint32_t), sizeof(val));
        TEST_ASSERT_EQUAL_HEX32(rand(), val);
    }
}

__attribute__((unused)) static void do_single_write_read_test(sdmmc_card_t* card,
        size_t start_block, size_t block_count, size_t alignment)
{
    size_t block_size = card->csd.sector_size;
    size_t total_size = block_size * block_count;
    printf(" %8d |  %3d  |   %d   |    %4.1f  ", start_block, block_count, alignment, total_size / 1024.0f);

    uint32_t* buffer = heap_caps_malloc(total_size + 4, MALLOC_CAP_DMA);
    size_t offset = alignment % 4;
    uint8_t* c_buffer = (uint8_t*) buffer + offset;
    fill_buffer(start_block, c_buffer, total_size / sizeof(buffer[0]));

    struct timeval t_start_wr;
    gettimeofday(&t_start_wr, NULL);
    TEST_ESP_OK(sdmmc_write_sectors(card, c_buffer, start_block, block_count));
    struct timeval t_stop_wr;
    gettimeofday(&t_stop_wr, NULL);
    float time_wr = 1e3f * (t_stop_wr.tv_sec - t_start_wr.tv_sec) + 1e-3f * (t_stop_wr.tv_usec - t_start_wr.tv_usec);

    memset(buffer, 0xbb, total_size + 4);

    struct timeval t_start_rd;
    gettimeofday(&t_start_rd, NULL);
    TEST_ESP_OK(sdmmc_read_sectors(card, c_buffer, start_block, block_count));
    struct timeval t_stop_rd;
    gettimeofday(&t_stop_rd, NULL);
    float time_rd = 1e3f * (t_stop_rd.tv_sec - t_start_rd.tv_sec) + 1e-3f * (t_stop_rd.tv_usec - t_start_rd.tv_usec);

    printf(" |   %6.2f    |      %5.2f      |    %6.2f     |     %5.2f\n",
            time_wr, total_size / (time_wr / 1000) / (1024 * 1024),
            time_rd, total_size / (time_rd / 1000) / (1024 * 1024));
    check_buffer(start_block, c_buffer, total_size / sizeof(buffer[0]));
    free(buffer);
}

__attribute__((unused)) static void read_write_test(sdmmc_card_t* card)
{
    sdmmc_card_print_info(stdout, card);
    printf("  sector  | count | align | size(kB)  | wr_time(ms) | wr_speed(MB/s)  |  rd_time(ms)  | rd_speed(MB/s)\n");
    do_single_write_read_test(card, 0, 1, 4);
    do_single_write_read_test(card, 0, 4, 4);
    do_single_write_read_test(card, 1, 16, 4);
    do_single_write_read_test(card, 16, 32, 4);
    do_single_write_read_test(card, 48, 64, 4);
    do_single_write_read_test(card, 128, 128, 4);
    do_single_write_read_test(card, card->csd.capacity - 64, 32, 4);
    do_single_write_read_test(card, card->csd.capacity - 64, 64, 4);
    do_single_write_read_test(card, card->csd.capacity - 8, 1, 4);
    do_single_write_read_test(card, card->csd.capacity/2, 1, 4);
    do_single_write_read_test(card, card->csd.capacity/2, 4, 4);
    do_single_write_read_test(card, card->csd.capacity/2, 8, 4);
    do_single_write_read_test(card, card->csd.capacity/2, 16, 4);
    do_single_write_read_test(card, card->csd.capacity/2, 32, 4);
    do_single_write_read_test(card, card->csd.capacity/2, 64, 4);
    do_single_write_read_test(card, card->csd.capacity/2, 128, 4);
    do_single_write_read_test(card, card->csd.capacity/2, 1, 1);
    do_single_write_read_test(card, card->csd.capacity/2, 8, 1);
    do_single_write_read_test(card, card->csd.capacity/2, 128, 1);
}

#if SOC_SDMMC_HOST_SUPPORTED
void test_sd_rw_blocks(int slot, int width)
{
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    config.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    config.slot = slot;
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    if (width != 0) {
        slot_config.width = width;
    }
    if (slot_config.width == 8) {
        config.flags &= ~SDMMC_HOST_FLAG_DDR;
    }
    TEST_ESP_OK(sdmmc_host_init());
    TEST_ESP_OK(sdmmc_host_init_slot(slot, &slot_config));
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);
    TEST_ESP_OK(sdmmc_card_init(&config, card));
    read_write_test(card);
    free(card);
    TEST_ESP_OK(sdmmc_host_deinit());
}

TEST_CASE("SDMMC read/write test (SD slot 1)", "[sd][test_env=UT_T1_SDMODE]")
{
    sd_test_board_power_on();
    test_sd_rw_blocks(1, 4);
    sd_test_board_power_off();
}

#ifdef WITH_EMMC_TEST
TEST_CASE("SDMMC read/write test (eMMC slot 0, 4 line DDR)", "[sd][test_env=EMMC]")
{
    sd_test_board_power_on();
    test_sd_rw_blocks(0, 4);
    sd_test_board_power_off();
}

TEST_CASE("SDMMC read/write test (eMMC slot 0, 8 line)", "[sd][test_env=EMMC]")
{
    sd_test_board_power_on();
    test_sd_rw_blocks(0, 8);
    sd_test_board_power_off();
}
#endif // WITH_EMMC_TEST
#endif // SDMMC_HOST_SUPPORTED

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32C3)
//No runners
TEST_CASE("SDMMC read/write test (SD slot 1, in SPI mode)", "[sdspi][test_env=UT_T1_SPIMODE]")
{
    sd_test_board_power_on();

    sdspi_dev_handle_t handle;
    sdspi_device_config_t dev_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    test_sdspi_init_bus(dev_config.host_id, GPIO_NUM_15, GPIO_NUM_2, GPIO_NUM_14, TEST_SDSPI_DMACHAN);
    TEST_ESP_OK(sdspi_host_init());
    TEST_ESP_OK(sdspi_host_init_device(&dev_config, &handle));

    sdmmc_host_t config = SDSPI_HOST_DEFAULT();
    config.slot = handle;
    // This test can only run under 20MHz on ESP32, because the runner connects the card to
    // non-IOMUX pins of HSPI.

    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);
    TEST_ESP_OK(sdmmc_card_init(&config, card));
    read_write_test(card);
    TEST_ESP_OK(sdspi_host_deinit());
    free(card);
    test_sdspi_deinit_bus(dev_config.host_id);
    sd_test_board_power_off();
}
#endif //DISABLED_FOR_TARGETS(ESP32S2, ESP32C3)

#if SOC_SDMMC_HOST_SUPPORTED
TEST_CASE("reads and writes with an unaligned buffer", "[sd][test_env=UT_T1_SDMODE]")
{
    sd_test_board_power_on();
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    TEST_ESP_OK(sdmmc_host_init());

    TEST_ESP_OK(sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config));
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);
    TEST_ESP_OK(sdmmc_card_init(&config, card));

    const size_t buffer_size = 4096;
    const size_t block_count = buffer_size / 512;
    const size_t extra = 4;
    uint8_t* buffer = heap_caps_malloc(buffer_size + extra, MALLOC_CAP_DMA);

    // Check read behavior: do aligned write, then unaligned read
    const uint32_t seed = 0x89abcdef;
    fill_buffer(seed, buffer, buffer_size / sizeof(uint32_t));
    TEST_ESP_OK(sdmmc_write_sectors(card, buffer, 0, block_count));
    memset(buffer, 0xcc, buffer_size + extra);
    TEST_ESP_OK(sdmmc_read_sectors(card, buffer + 1, 0, block_count));
    check_buffer(seed, buffer + 1, buffer_size / sizeof(uint32_t));

    // Check write behavior: do unaligned write, then aligned read
    fill_buffer(seed, buffer + 1, buffer_size / sizeof(uint32_t));
    TEST_ESP_OK(sdmmc_write_sectors(card, buffer + 1, 8, block_count));
    memset(buffer, 0xcc, buffer_size + extra);
    TEST_ESP_OK(sdmmc_read_sectors(card, buffer, 8, block_count));
    check_buffer(seed, buffer, buffer_size / sizeof(uint32_t));

    free(buffer);
    free(card);
    TEST_ESP_OK(sdmmc_host_deinit());
    sd_test_board_power_off();
}
#endif

__attribute__((unused)) static void test_cd_input(int gpio_cd_num, const sdmmc_host_t* config)
{
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);

    // SDMMC host should have configured CD as input.
    // Enable output as well (not using the driver, to avoid touching input
    // enable bits).
    esp_rom_gpio_connect_out_signal(gpio_cd_num, SIG_GPIO_OUT_IDX, false, false);
    REG_WRITE(GPIO_ENABLE_W1TS_REG, BIT(gpio_cd_num));

    // Check that card initialization fails if CD is high
    REG_WRITE(GPIO_OUT_W1TS_REG, BIT(gpio_cd_num));
    usleep(1000);
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, sdmmc_card_init(config, card));

    // Check that card initialization succeeds if CD is low
    REG_WRITE(GPIO_OUT_W1TC_REG, BIT(gpio_cd_num));
    usleep(1000);
    TEST_ESP_OK(sdmmc_card_init(config, card));

    free(card);
}

#if SOC_SDMMC_HOST_SUPPORTED
TEST_CASE("CD input works in SD mode", "[sd][test_env=UT_T1_SDMODE]")
{
    sd_test_board_power_on();
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.gpio_cd = CD_WP_TEST_GPIO;
    TEST_ESP_OK(sdmmc_host_init());
    TEST_ESP_OK(sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config));

    test_cd_input(CD_WP_TEST_GPIO, &config);

    TEST_ESP_OK(sdmmc_host_deinit());
    sd_test_board_power_off();
}
#endif

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32C3)
//No runners
TEST_CASE("CD input works in SPI mode", "[sd][test_env=UT_T1_SPIMODE]")
{
    sd_test_board_power_on();

    sdspi_dev_handle_t handle;
    sdspi_device_config_t dev_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    dev_config.gpio_cd = CD_WP_TEST_GPIO;
    test_sdspi_init_bus(dev_config.host_id, GPIO_NUM_15, GPIO_NUM_2, GPIO_NUM_14, TEST_SDSPI_DMACHAN);
    TEST_ESP_OK(sdspi_host_init());
    TEST_ESP_OK(sdspi_host_init_device(&dev_config, &handle));

    sdmmc_host_t config = SDSPI_HOST_DEFAULT();
    config.slot = handle;

    test_cd_input(CD_WP_TEST_GPIO, &config);

    TEST_ESP_OK(sdspi_host_deinit());
    test_sdspi_deinit_bus(dev_config.host_id);
    sd_test_board_power_off();
}
#endif //DISABLED_FOR_TARGETS(ESP32S2)

__attribute__((unused)) static void test_wp_input(int gpio_wp_num, const sdmmc_host_t* config)
{
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);

    // SDMMC host should have configured WP as input.
    // Enable output as well (not using the driver, to avoid touching input
    // enable bits).
    esp_rom_gpio_connect_out_signal(gpio_wp_num, SIG_GPIO_OUT_IDX, false, false);
    REG_WRITE(GPIO_ENABLE_W1TS_REG, BIT(gpio_wp_num));

    // Check that the card can be initialized with WP low
    REG_WRITE(GPIO_OUT_W1TC_REG, BIT(gpio_wp_num));
    TEST_ESP_OK(sdmmc_card_init(config, card));

    uint32_t* data = heap_caps_calloc(1, 512, MALLOC_CAP_DMA);

    // Check that card write succeeds if WP is high
    REG_WRITE(GPIO_OUT_W1TS_REG, BIT(gpio_wp_num));
    usleep(1000);
    TEST_ESP_OK(sdmmc_write_sectors(card, &data, 0, 1));

    // Check that write fails if WP is low
    REG_WRITE(GPIO_OUT_W1TC_REG, BIT(gpio_wp_num));
    usleep(1000);
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, sdmmc_write_sectors(card, &data, 0, 1));
    // ...but reads still work
    TEST_ESP_OK(sdmmc_read_sectors(card, &data, 0, 1));

    free(data);
    free(card);
}

#if SOC_SDMMC_HOST_SUPPORTED
TEST_CASE("WP input works in SD mode", "[sd][test_env=UT_T1_SDMODE]")
{
    sd_test_board_power_on();
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.gpio_wp = CD_WP_TEST_GPIO;
    TEST_ESP_OK(sdmmc_host_init());
    TEST_ESP_OK(sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config));

    test_wp_input(CD_WP_TEST_GPIO, &config);

    TEST_ESP_OK(sdmmc_host_deinit());
    sd_test_board_power_off();
}
#endif

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32C3)
//No runners
TEST_CASE("WP input works in SPI mode", "[sd][test_env=UT_T1_SPIMODE]")
{
    sd_test_board_power_on();

    sdspi_dev_handle_t handle;
    sdspi_device_config_t dev_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    dev_config.gpio_wp = CD_WP_TEST_GPIO;
    test_sdspi_init_bus(dev_config.host_id, GPIO_NUM_15, GPIO_NUM_2, GPIO_NUM_14, TEST_SDSPI_DMACHAN);

    TEST_ESP_OK(sdspi_host_init());
    TEST_ESP_OK(sdspi_host_init_device(&dev_config, &handle));

    sdmmc_host_t config = SDSPI_HOST_DEFAULT();
    config.slot = handle;

    test_wp_input(CD_WP_TEST_GPIO, &config);

    TEST_ESP_OK(sdspi_host_deinit());
    test_sdspi_deinit_bus(dev_config.host_id);
    sd_test_board_power_off();
}
#endif //DISABLED_FOR_TARGETS(ESP32S2)

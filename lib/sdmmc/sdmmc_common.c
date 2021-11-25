/*
 * Copyright (c) 2006 Uwe Stuehler <uwe@openbsd.org>
 * Adaptations to ESP-IDF Copyright (c) 2016-2018 Espressif Systems (Shanghai) PTE LTD
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <assert.h>

#include "sdmmc_common.h"

sdmmc_err_t sdmmc_init_ocr(sdmmc_card_t* card)
{
    sdmmc_err_t err;
    /* In SPI mode, READ_OCR (CMD58) command is used to figure out which voltage
     * ranges the card can support. This step is skipped since 1.8V isn't
     * supported on the ESP32.
     */

    uint32_t host_ocr = get_host_ocr(card->host.io_voltage);
    if ((card->ocr & SD_OCR_SDHC_CAP) != 0) {
        host_ocr |= SD_OCR_SDHC_CAP;
    }
    /* Send SEND_OP_COND (ACMD41) command to the card until it becomes ready. */
    err = sdmmc_send_cmd_send_op_cond(card, host_ocr, &card->ocr);

    /* If time-out, re-try send_op_cond as MMC */
    if (err == SDMMC_ERR_TIMEOUT && !host_is_spi(card)) {
        ESP_LOGD(TAG, "send_op_cond timeout, trying MMC");
        card->is_mmc = 1;
        err = sdmmc_send_cmd_send_op_cond(card, host_ocr, &card->ocr);
    }

    if (err != SDMMC_OK) {
        ESP_LOGE(TAG, "%s: send_op_cond (1) returned 0x%x", __func__, err);
        return err;
    }
    if (host_is_spi(card)) {
        err = sdmmc_send_cmd_read_ocr(card, &card->ocr);
        if (err != SDMMC_OK) {
            ESP_LOGE(TAG, "%s: read_ocr returned 0x%x", __func__, err);
            return err;
        }
    }
    ESP_LOGD(TAG, "host_ocr=0x%x card_ocr=0x%x", host_ocr, card->ocr);

    /* Clear all voltage bits in host's OCR which the card doesn't support.
     * Don't touch CCS bit because in SPI mode cards don't report CCS in ACMD41
     * response.
     */
    host_ocr &= (card->ocr | (~SD_OCR_VOL_MASK));
    ESP_LOGD(TAG, "sdmmc_card_init: host_ocr=%08x, card_ocr=%08x", host_ocr, card->ocr);
    return SDMMC_OK;
}

sdmmc_err_t sdmmc_init_cid(sdmmc_card_t* card)
{
    sdmmc_err_t err;
    sdmmc_response_t raw_cid;
    if (!host_is_spi(card)) {
        err = sdmmc_send_cmd_all_send_cid(card, &raw_cid);
        if (err != SDMMC_OK) {
            ESP_LOGE(TAG, "%s: all_send_cid returned 0x%x", __func__, err);
            return err;
        }
        if (!card->is_mmc) {
            err = sdmmc_decode_cid(raw_cid, &card->cid);
            if (err != SDMMC_OK) {
                ESP_LOGE(TAG, "%s: decoding CID failed (0x%x)", __func__, err);
                return err;
            }
        } else {
            /* For MMC, need to know CSD to decode CID. But CSD can only be read
             * in data transfer mode, and it is not possible to read CID in data
             * transfer mode. We temporiliy store the raw cid and do the
             * decoding after the RCA is set and the card is in data transfer
             * mode.
             */
            memcpy(card->raw_cid, raw_cid, sizeof(sdmmc_response_t));
        }
    } else {
        err = sdmmc_send_cmd_send_cid(card, &card->cid);
        if (err != SDMMC_OK) {
            ESP_LOGE(TAG, "%s: send_cid returned 0x%x", __func__, err);
            return err;
        }
    }
    return SDMMC_OK;
}

sdmmc_err_t sdmmc_init_rca(sdmmc_card_t* card)
{
    sdmmc_err_t err;
    err = sdmmc_send_cmd_set_relative_addr(card, &card->rca);
    if (err != SDMMC_OK) {
        ESP_LOGE(TAG, "%s: set_relative_addr returned 0x%x", __func__, err);
        return err;
    }
    return SDMMC_OK;
}

sdmmc_err_t sdmmc_init_mmc_decode_cid(sdmmc_card_t* card)
{
    sdmmc_err_t err;
    sdmmc_response_t raw_cid;
    memcpy(raw_cid, card->raw_cid, sizeof(raw_cid));
    err = sdmmc_mmc_decode_cid(card->csd.mmc_ver, raw_cid, &card->cid);
    if (err != SDMMC_OK) {
        ESP_LOGE(TAG, "%s: decoding CID failed (0x%x)", __func__, err);
        return err;
    }
    return SDMMC_OK;
}

sdmmc_err_t sdmmc_init_csd(sdmmc_card_t* card)
{
    assert(card->is_mem == 1);
    /* Get and decode the contents of CSD register. Determine card capacity. */
    sdmmc_err_t err = sdmmc_send_cmd_send_csd(card, &card->csd);
    if (err != SDMMC_OK) {
        ESP_LOGE(TAG, "%s: send_csd returned 0x%x", __func__, err);
        return err;
    }
    const size_t max_sdsc_capacity = UINT32_MAX / card->csd.sector_size + 1;
    if (!(card->ocr & SD_OCR_SDHC_CAP) &&
         (size_t) card->csd.capacity > max_sdsc_capacity) {
        ESP_LOGW(TAG, "%s: SDSC card reports capacity=%u. Limiting to %u.",
                __func__, card->csd.capacity, max_sdsc_capacity);
        card->csd.capacity = max_sdsc_capacity;
    }
    return SDMMC_OK;
}

sdmmc_err_t sdmmc_init_select_card(sdmmc_card_t* card)
{
    assert(!host_is_spi(card));
    sdmmc_err_t err = sdmmc_send_cmd_select_card(card, card->rca);
    if (err != SDMMC_OK) {
        ESP_LOGE(TAG, "%s: select_card returned 0x%x", __func__, err);
        return err;
    }
    return SDMMC_OK;
}

sdmmc_err_t sdmmc_init_card_hs_mode(sdmmc_card_t* card)
{
    sdmmc_err_t err = SDMMC_ERR_NOT_SUPPORTED;
    if (card->is_mem && !card->is_mmc) {
        err = sdmmc_enable_hs_mode_and_check(card);
    } else if (card->is_sdio) {
        err = sdmmc_io_enable_hs_mode(card);
    } else if (card->is_mmc){
        err = sdmmc_mmc_enable_hs_mode(card);
    }
    if (err == SDMMC_ERR_NOT_SUPPORTED) {
        ESP_LOGD(TAG, "%s: host supports HS mode, but card doesn't", __func__);
        card->max_freq_khz = SDMMC_FREQ_DEFAULT;
    } else if (err != SDMMC_OK) {
        return err;
    }
    return SDMMC_OK;
}

sdmmc_err_t sdmmc_init_host_bus_width(sdmmc_card_t* card)
{
    int bus_width = 1;

    if ((card->host.flags & SDMMC_HOST_FLAG_4BIT) &&
        (card->log_bus_width == 2)) {
        bus_width = 4;
    } else if ((card->host.flags & SDMMC_HOST_FLAG_8BIT) &&
        (card->log_bus_width == 3)) {
        bus_width = 8;
    }
    ESP_LOGD(TAG, "%s: using %d-bit bus", __func__, bus_width);
    if (bus_width > 1) {
        sdmmc_err_t err = (*card->host.set_bus_width)(card->host.slot, bus_width);
        if (err != SDMMC_OK) {
            ESP_LOGE(TAG, "host.set_bus_width failed (0x%x)", err);
            return err;
        }
    }
    return SDMMC_OK;
}

sdmmc_err_t sdmmc_init_host_frequency(sdmmc_card_t* card)
{
    assert(card->max_freq_khz <= card->host.max_freq_khz);

    /* Find highest frequency in the following list,
     * which is below card->max_freq_khz.
     */
    const uint32_t freq_values[] = {
            SDMMC_FREQ_52M,
            SDMMC_FREQ_HIGHSPEED,
            SDMMC_FREQ_26M,
            SDMMC_FREQ_DEFAULT
            //NOTE: in sdspi mode, 20MHz may not work. in that case, add 10MHz here.
    };
    const int n_freq_values = sizeof(freq_values) / sizeof(freq_values[0]);

    uint32_t selected_freq = SDMMC_FREQ_PROBING;
    for (int i = 0; i < n_freq_values; ++i) {
        uint32_t freq = freq_values[i];
        if (card->max_freq_khz >= freq) {
            selected_freq = freq;
            break;
        }
    }

    ESP_LOGD(TAG, "%s: using %d kHz bus frequency", __func__, selected_freq);
    if (selected_freq > SDMMC_FREQ_PROBING) {
        sdmmc_err_t err = (*card->host.set_card_clk)(card->host.slot, selected_freq);
        if (err != SDMMC_OK) {
            ESP_LOGE(TAG, "failed to switch bus frequency (0x%x)", err);
            return err;
        }
    }

    if (card->is_ddr) {
        if (card->host.set_bus_ddr_mode == NULL) {
            ESP_LOGE(TAG, "host doesn't support DDR mode or voltage switching");
            return SDMMC_ERR_NOT_SUPPORTED;
        }
        sdmmc_err_t err = (*card->host.set_bus_ddr_mode)(card->host.slot, true);
        if (err != SDMMC_OK) {
            ESP_LOGE(TAG, "failed to switch bus to DDR mode (0x%x)", err);
            return err;
        }
    }
    return SDMMC_OK;
}

void sdmmc_flip_byte_order(uint32_t* response, size_t size)
{
    assert(size % (2 * sizeof(uint32_t)) == 0);
    const size_t n_words = size / sizeof(uint32_t);
    for (size_t i = 0; i < n_words / 2; ++i) {
        uint32_t left = __builtin_bswap32(response[i]);
        uint32_t right = __builtin_bswap32(response[n_words - i - 1]);
        response[i] = right;
        response[n_words - i - 1] = left;
    }
}

void sdmmc_card_print_info(FILE* stream, const sdmmc_card_t* card)
{
    // bool print_scr = false;
    // bool print_csd = false;
    // const char* type;
    // fprintf(stream, "Name: %s\n", card->cid.name);
    // if (card->is_sdio) {
    //     type = "SDIO";
    //     print_scr = true;
    //     print_csd = true;
    // } else if (card->is_mmc) {
    //     type = "MMC";
    //     print_csd = true;
    // } else {
    //     type = (card->ocr & SD_OCR_SDHC_CAP) ? "SDHC/SDXC" : "SDSC";
    // }
    // fprintf(stream, "Type: %s\n", type);
    // if (card->max_freq_khz < 1000) {
    //     fprintf(stream, "Speed: %d kHz\n", card->max_freq_khz);
    // } else {
    //     fprintf(stream, "Speed: %d MHz%s\n", card->max_freq_khz / 1000,
    //             card->is_ddr ? ", DDR" : "");
    // }
    // fprintf(stream, "Size: %luMB\n", ((uint64_t) card->csd.capacity) * card->csd.sector_size / (1024 * 1024));

    // if (print_csd) {
    //     fprintf(stream, "CSD: ver=%d, sector_size=%d, capacity=%d read_bl_len=%d\n",
    //             card->csd.csd_ver,
    //             card->csd.sector_size, card->csd.capacity, card->csd.read_block_len);
    // }
    // if (print_scr) {
    //     fprintf(stream, "SCR: sd_spec=%d, bus_width=%d\n", card->scr.sd_spec, card->scr.bus_width);
    // }
}

sdmmc_err_t sdmmc_fix_host_flags(sdmmc_card_t* card)
{
    const uint32_t width_1bit = SDMMC_HOST_FLAG_1BIT;
    const uint32_t width_4bit = SDMMC_HOST_FLAG_4BIT;
    const uint32_t width_8bit = SDMMC_HOST_FLAG_8BIT;
    const uint32_t width_mask = width_1bit | width_4bit | width_8bit;

    int slot_bit_width = card->host.get_bus_width(card->host.slot);
    if (slot_bit_width == 1 &&
            (card->host.flags & (width_4bit | width_8bit))) {
        card->host.flags &= ~width_mask;
        card->host.flags |= width_1bit;
    } else if (slot_bit_width == 4 && (card->host.flags & width_8bit)) {
        if ((card->host.flags & width_4bit) == 0) {
            ESP_LOGW(TAG, "slot width set to 4, but host flags don't have 4 line mode enabled; using 1 line mode");
            card->host.flags &= ~width_mask;
            card->host.flags |= width_1bit;
        } else {
            card->host.flags &= ~width_mask;
            card->host.flags |= width_4bit;
        }
    }
    return SDMMC_OK;
}

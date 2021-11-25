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

sdmmc_err_t sdmmc_send_cmd(sdmmc_card_t* card, sdmmc_command_t* cmd)
{
    if (card->host.command_timeout_ms != 0) {
        cmd->timeout_ms = card->host.command_timeout_ms;
    } else if (cmd->timeout_ms == 0) {
        cmd->timeout_ms = SDMMC_DEFAULT_CMD_TIMEOUT_MS;
    }

    int slot = card->host.slot;
    ESP_LOGV(TAG, "sending cmd slot=%d op=%d arg=%x flags=%x data=%p blklen=%d datalen=%d timeout=%d",
            slot, cmd->opcode, cmd->arg, cmd->flags, cmd->data, cmd->blklen, cmd->datalen, cmd->timeout_ms);
    sdmmc_err_t err = (*card->host.do_transaction)(slot, cmd);
    if (err != 0) {
        ESP_LOGD(TAG, "cmd=%d, sdmmc_req_run returned 0x%x", cmd->opcode, err);
        return err;
    }
    ESP_LOGV(TAG, "cmd response %08x %08x %08x %08x err=0x%x state=%d",
               cmd->response[0],
               cmd->response[1],
               cmd->response[2],
               cmd->response[3],
               cmd->error,
               MMC_R1_CURRENT_STATE(cmd->response));
    return cmd->error;
}

sdmmc_err_t sdmmc_send_app_cmd(sdmmc_card_t* card, sdmmc_command_t* cmd)
{
    sdmmc_command_t app_cmd = {
        .opcode = MMC_APP_CMD,
        .flags = SCF_CMD_AC | SCF_RSP_R1,
        .arg = MMC_ARG_RCA(card->rca),
    };
    sdmmc_err_t err = sdmmc_send_cmd(card, &app_cmd);
    if (err != SDMMC_OK) {
        return err;
    }
    // Check APP_CMD status bit (only in SD mode)
    if (!host_is_spi(card) && !(MMC_R1(app_cmd.response) & MMC_R1_APP_CMD)) {
        ESP_LOGW(TAG, "card doesn't support APP_CMD");
        return SDMMC_ERR_NOT_SUPPORTED;
    }
    return sdmmc_send_cmd(card, cmd);
}


sdmmc_err_t sdmmc_send_cmd_go_idle_state(sdmmc_card_t* card)
{
    sdmmc_command_t cmd = {
        .opcode = MMC_GO_IDLE_STATE,
        .flags = SCF_CMD_BC | SCF_RSP_R0,
    };
    sdmmc_err_t err = sdmmc_send_cmd(card, &cmd);
    if (host_is_spi(card)) {
        /* To enter SPI mode, CMD0 needs to be sent twice (see figure 4-1 in
         * SD Simplified spec v4.10). Some cards enter SD mode on first CMD0,
         * so don't expect the above command to succeed.
         * SCF_RSP_R1 flag below tells the lower layer to expect correct R1
         * response (in SPI mode).
         */
        (void) err;
        osal_task_delay(SDMMC_GO_IDLE_DELAY_MS);

        cmd.flags |= SCF_RSP_R1;
        err = sdmmc_send_cmd(card, &cmd);
    }
    if (err == SDMMC_OK) {
        osal_task_delay(SDMMC_GO_IDLE_DELAY_MS);
    }
    return err;
}


sdmmc_err_t sdmmc_send_cmd_send_if_cond(sdmmc_card_t* card, uint32_t ocr)
{
    const uint8_t pattern = 0xaa; /* any pattern will do here */
    sdmmc_command_t cmd = {
        .opcode = SD_SEND_IF_COND,
        .arg = (((ocr & SD_OCR_VOL_MASK) != 0) << 8) | pattern,
        .flags = SCF_CMD_BCR | SCF_RSP_R7,
    };
    sdmmc_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != SDMMC_OK) {
        return err;
    }
    uint8_t response = cmd.response[0] & 0xff;
    if (response != pattern) {
        ESP_LOGD(TAG, "%s: received=0x%x expected=0x%x", __func__, response, pattern);
        return SDMMC_ERR_INVALID_RESPONSE;
    }
    return SDMMC_OK;
}

sdmmc_err_t sdmmc_send_cmd_send_op_cond(sdmmc_card_t* card, uint32_t ocr, uint32_t *ocrp)
{
    sdmmc_err_t err;

    sdmmc_command_t cmd = {
            .arg = ocr,
            .flags = SCF_CMD_BCR | SCF_RSP_R3,
            .opcode = SD_APP_OP_COND
    };
    int nretries = SDMMC_SEND_OP_COND_MAX_RETRIES;
    int err_cnt = SDMMC_SEND_OP_COND_MAX_ERRORS;
    for (; nretries != 0; --nretries)  {
        bzero(&cmd, sizeof cmd);
        cmd.arg = ocr;
        cmd.flags = SCF_CMD_BCR | SCF_RSP_R3;
        if (!card->is_mmc) { /* SD mode */
            cmd.opcode = SD_APP_OP_COND;
            err = sdmmc_send_app_cmd(card, &cmd);
        } else { /* MMC mode */
            cmd.arg &= ~MMC_OCR_ACCESS_MODE_MASK;
            cmd.arg |= MMC_OCR_SECTOR_MODE;
            cmd.opcode = MMC_SEND_OP_COND;
            err = sdmmc_send_cmd(card, &cmd);
        }

        if (err != SDMMC_OK) {
            if (--err_cnt == 0) {
                ESP_LOGD(TAG, "%s: sdmmc_send_app_cmd err=0x%x", __func__, err);
                return err;
            } else {
                ESP_LOGV(TAG, "%s: ignoring err=0x%x", __func__, err);
                continue;
            }
        }
        // In SD protocol, card sets MEM_READY bit in OCR when it is ready.
        // In SPI protocol, card clears IDLE_STATE bit in R1 response.
        if (!host_is_spi(card)) {
            if ((MMC_R3(cmd.response) & MMC_OCR_MEM_READY) ||
                ocr == 0) {
                break;
            }
        } else {
            if ((SD_SPI_R1(cmd.response) & SD_SPI_R1_IDLE_STATE) == 0) {
                break;
            }
        }
        osal_task_delay(10);
    }
    if (nretries == 0) {
        return SDMMC_ERR_TIMEOUT;
    }
    if (ocrp) {
        *ocrp = MMC_R3(cmd.response);
    }
    return SDMMC_OK;
}

sdmmc_err_t sdmmc_send_cmd_read_ocr(sdmmc_card_t *card, uint32_t *ocrp)
{
    assert(ocrp);
    sdmmc_command_t cmd = {
        .opcode = SD_READ_OCR,
        .flags = SCF_CMD_BCR | SCF_RSP_R2
    };
    sdmmc_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != SDMMC_OK) {
        return err;
    }
    *ocrp = SD_SPI_R3(cmd.response);
    return SDMMC_OK;
}


sdmmc_err_t sdmmc_send_cmd_all_send_cid(sdmmc_card_t* card, sdmmc_response_t* out_raw_cid)
{
    assert(out_raw_cid);
    sdmmc_command_t cmd = {
            .opcode = MMC_ALL_SEND_CID,
            .flags = SCF_CMD_BCR | SCF_RSP_R2
    };
    sdmmc_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != SDMMC_OK) {
        return err;
    }
    memcpy(out_raw_cid, &cmd.response, sizeof(sdmmc_response_t));
    return SDMMC_OK;
}

sdmmc_err_t sdmmc_send_cmd_send_cid(sdmmc_card_t *card, sdmmc_cid_t *out_cid)
{
    assert(out_cid);
    assert(host_is_spi(card) && "SEND_CID should only be used in SPI mode");
    assert(!card->is_mmc && "MMC cards are not supported in SPI mode");
    sdmmc_response_t buf;
    sdmmc_command_t cmd = {
        .opcode = MMC_SEND_CID,
        .flags = SCF_CMD_READ | SCF_CMD_ADTC,
        .arg = 0,
        .data = &buf[0],
        .datalen = sizeof(buf)
    };
    sdmmc_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != SDMMC_OK) {
        return err;
    }
    sdmmc_flip_byte_order(buf, sizeof(buf));
    return sdmmc_decode_cid(buf, out_cid);
}


sdmmc_err_t sdmmc_send_cmd_set_relative_addr(sdmmc_card_t* card, uint16_t* out_rca)
{
    assert(out_rca);
    sdmmc_command_t cmd = {
            .opcode = SD_SEND_RELATIVE_ADDR,
            .flags = SCF_CMD_BCR | SCF_RSP_R6
    };

    /* MMC cards expect us to set the RCA.
     * Set RCA to 1 since we don't support multiple cards on the same bus, for now.
     */
    uint16_t mmc_rca = 1;
    if (card->is_mmc) {
        cmd.arg = MMC_ARG_RCA(mmc_rca);
    }

    sdmmc_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != SDMMC_OK) {
        return err;
    }
    *out_rca = (card->is_mmc) ? mmc_rca : SD_R6_RCA(cmd.response);
    return SDMMC_OK;
}

sdmmc_err_t sdmmc_send_cmd_set_blocklen(sdmmc_card_t* card, sdmmc_csd_t* csd)
{
    sdmmc_command_t cmd = {
            .opcode = MMC_SET_BLOCKLEN,
            .arg = csd->sector_size,
            .flags = SCF_CMD_AC | SCF_RSP_R1
    };
    return sdmmc_send_cmd(card, &cmd);
}

sdmmc_err_t sdmmc_send_cmd_send_csd(sdmmc_card_t* card, sdmmc_csd_t* out_csd)
{
    /* The trick with SEND_CSD is that in SPI mode, it acts as a data read
     * command, while in SD mode it is an AC command with R2 response.
     */
    sdmmc_response_t spi_buf;
    const bool is_spi = host_is_spi(card);
    sdmmc_command_t cmd = {
            .opcode = MMC_SEND_CSD,
            .arg = is_spi ? 0 : MMC_ARG_RCA(card->rca),
            .flags = is_spi ? (SCF_CMD_READ | SCF_CMD_ADTC | SCF_RSP_R1) :
                    (SCF_CMD_AC | SCF_RSP_R2),
            .data = is_spi ? &spi_buf[0] : 0,
            .datalen = is_spi ? sizeof(spi_buf) : 0,
    };
    sdmmc_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != SDMMC_OK) {
        return err;
    }
    uint32_t* ptr = cmd.response;
    if (is_spi) {
        sdmmc_flip_byte_order(spi_buf,  sizeof(spi_buf));
        ptr = spi_buf;
    }
    if (card->is_mmc) {
        err = sdmmc_mmc_decode_csd(cmd.response, out_csd);
    } else {
        err = sdmmc_decode_csd(ptr, out_csd);
    }
    return err;
}

sdmmc_err_t sdmmc_send_cmd_select_card(sdmmc_card_t* card, uint32_t rca)
{
    /* Don't expect to see a response when de-selecting a card */
    uint32_t response = (rca == 0) ? 0 : SCF_RSP_R1;
    sdmmc_command_t cmd = {
            .opcode = MMC_SELECT_CARD,
            .arg = MMC_ARG_RCA(rca),
            .flags = SCF_CMD_AC | response
    };
    return sdmmc_send_cmd(card, &cmd);
}

sdmmc_err_t sdmmc_send_cmd_send_scr(sdmmc_card_t* card, sdmmc_scr_t *out_scr)
{
    size_t datalen = 8;
    uint32_t buf[datalen];
    sdmmc_command_t cmd = {
            .data = buf,
            .datalen = datalen,
            .blklen = datalen,
            .flags = SCF_CMD_ADTC | SCF_CMD_READ | SCF_RSP_R1,
            .opcode = SD_APP_SEND_SCR
    };
    sdmmc_err_t err = sdmmc_send_app_cmd(card, &cmd);
    if (err == SDMMC_OK) {
        err = sdmmc_decode_scr(buf, out_scr);
    }
    return err;
}

sdmmc_err_t sdmmc_send_cmd_set_bus_width(sdmmc_card_t* card, int width)
{
    sdmmc_command_t cmd = {
            .opcode = SD_APP_SET_BUS_WIDTH,
            .flags = SCF_RSP_R1 | SCF_CMD_AC,
            .arg = (width == 4) ? SD_ARG_BUS_WIDTH_4 : SD_ARG_BUS_WIDTH_1,
    };

    return sdmmc_send_app_cmd(card, &cmd);
}

sdmmc_err_t sdmmc_send_cmd_crc_on_off(sdmmc_card_t* card, bool crc_enable)
{
    assert(host_is_spi(card) && "CRC_ON_OFF can only be used in SPI mode");
    sdmmc_command_t cmd = {
            .opcode = SD_CRC_ON_OFF,
            .arg = crc_enable ? 1 : 0,
            .flags = SCF_CMD_AC | SCF_RSP_R1
    };
    return sdmmc_send_cmd(card, &cmd);
}

sdmmc_err_t sdmmc_send_cmd_send_status(sdmmc_card_t* card, uint32_t* out_status)
{
    sdmmc_command_t cmd = {
            .opcode = MMC_SEND_STATUS,
            .arg = MMC_ARG_RCA(card->rca),
            .flags = SCF_CMD_AC | SCF_RSP_R1
    };
    sdmmc_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != SDMMC_OK) {
        return err;
    }
    if (out_status) {
        *out_status = MMC_R1(cmd.response);
    }
    return SDMMC_OK;
}

sdmmc_err_t sdmmc_write_sectors(sdmmc_card_t* card, const void* src,
        size_t start_block, size_t block_count)
{
    sdmmc_err_t err = SDMMC_OK;
    size_t block_size = card->csd.sector_size;
    if ((intptr_t)src % 4 == 0) {
        err = sdmmc_write_sectors_dma(card, src, start_block, block_count);
    } else {
        // SDMMC peripheral needs DMA-capable buffers. Split the write into
        // separate single block writes, if needed, and allocate a temporary
        // DMA-capable buffer.
        uint32_t tmp_buf[block_size / sizeof(uint32_t)];
        const uint8_t* cur_src = (const uint8_t*) src;
        for (size_t i = 0; i < block_count; ++i) {
            memcpy(tmp_buf, cur_src, block_size);
            cur_src += block_size;
            err = sdmmc_write_sectors_dma(card, tmp_buf, start_block + i, 1);
            if (err != SDMMC_OK) {
                ESP_LOGD(TAG, "%s: error 0x%x writing block %d+%d",
                        __func__, err, start_block, i);
                break;
            }
        }
    }
    return err;
}

sdmmc_err_t sdmmc_write_sectors_dma(sdmmc_card_t* card, const void* src,
        size_t start_block, size_t block_count)
{
    if (start_block + block_count > (size_t) card->csd.capacity) {
        return SDMMC_ERR_INVALID_SIZE;
    }
    size_t block_size = card->csd.sector_size;
    sdmmc_command_t cmd = {
            .flags = SCF_CMD_ADTC | SCF_RSP_R1,
            .blklen = block_size,
            .data = (void*) src,
            .datalen = block_count * block_size,
            .timeout_ms = SDMMC_WRITE_CMD_TIMEOUT_MS
    };
    if (block_count == 1) {
        cmd.opcode = MMC_WRITE_BLOCK_SINGLE;
    } else {
        cmd.opcode = MMC_WRITE_BLOCK_MULTIPLE;
        cmd.flags |= SCF_AUTO_STOP;
    }
    if (card->ocr & SD_OCR_SDHC_CAP) {
        cmd.arg = start_block;
    } else {
        cmd.arg = start_block * block_size;
    }
    sdmmc_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != SDMMC_OK) {
        ESP_LOGE(TAG, "%s: sdmmc_send_cmd returned 0x%x", __func__, err);
        return err;
    }
    uint32_t status = 0;
    size_t count = 0;
    while (!host_is_spi(card) && !(status & MMC_R1_READY_FOR_DATA)) {
        // TODO: add some timeout here
        err = sdmmc_send_cmd_send_status(card, &status);
        if (err != SDMMC_OK) {
            return err;
        }
        if (++count % 10 == 0) {
            ESP_LOGV(TAG, "waiting for card to become ready (%d)", count);
        }
    }
    return SDMMC_OK;
}

sdmmc_err_t sdmmc_read_sectors(sdmmc_card_t* card, void* dst,
        size_t start_block, size_t block_count)
{
    sdmmc_err_t err = SDMMC_OK;
    size_t block_size = card->csd.sector_size;
    if ((intptr_t)dst % 4 == 0) {
        err = sdmmc_read_sectors_dma(card, dst, start_block, block_count);
    } else {
        // SDMMC peripheral needs DMA-capable buffers. Split the read into
        // separate single block reads, if needed, and allocate a temporary
        // DMA-capable buffer.
        uint32_t tmp_buf[block_size / sizeof(uint32_t)];
        uint8_t* cur_dst = (uint8_t*) dst;
        for (size_t i = 0; i < block_count; ++i) {
            err = sdmmc_read_sectors_dma(card, tmp_buf, start_block + i, 1);
            if (err != SDMMC_OK) {
                ESP_LOGD(TAG, "%s: error 0x%x writing block %d+%d",
                        __func__, err, start_block, i);
                break;
            }
            memcpy(cur_dst, tmp_buf, block_size);
            cur_dst += block_size;
        }
    }
    return err;
}

sdmmc_err_t sdmmc_read_sectors_dma(sdmmc_card_t* card, void* dst,
        size_t start_block, size_t block_count)
{
    if (start_block + block_count > (size_t) card->csd.capacity) {
        return SDMMC_ERR_INVALID_SIZE;
    }
    size_t block_size = card->csd.sector_size;
    sdmmc_command_t cmd = {
            .flags = SCF_CMD_ADTC | SCF_CMD_READ | SCF_RSP_R1,
            .blklen = block_size,
            .data = (void*) dst,
            .datalen = block_count * block_size
    };
    if (block_count == 1) {
        cmd.opcode = MMC_READ_BLOCK_SINGLE;
    } else {
        cmd.opcode = MMC_READ_BLOCK_MULTIPLE;
        cmd.flags |= SCF_AUTO_STOP;
    }
    if (card->ocr & SD_OCR_SDHC_CAP) {
        cmd.arg = start_block;
    } else {
        cmd.arg = start_block * block_size;
    }
    sdmmc_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != SDMMC_OK) {
        ESP_LOGE(TAG, "%s: sdmmc_send_cmd returned 0x%x", __func__, err);
        return err;
    }
    uint32_t status = 0;
    size_t count = 0;
    while (!host_is_spi(card) && !(status & MMC_R1_READY_FOR_DATA)) {
        // TODO: add some timeout here
        err = sdmmc_send_cmd_send_status(card, &status);
        if (err != SDMMC_OK) {
            return err;
        }
        if (++count % 10 == 0) {
            ESP_LOGV(TAG, "waiting for card to become ready (%d)", count);
        }
    }
    return SDMMC_OK;
}

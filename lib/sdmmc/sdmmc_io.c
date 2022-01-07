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


#define CIS_TUPLE(NAME)  (cis_tuple_t) {.code=CISTPL_CODE_##NAME, .name=#NAME, .func=&cis_tuple_func_default, }
#define CIS_TUPLE_WITH_FUNC(NAME, FUNC)  (cis_tuple_t) {.code=CISTPL_CODE_##NAME, .name=#NAME, .func=&(FUNC), }

#define CIS_CHECK_SIZE(SIZE, MINIMAL) do {int store_size = (SIZE); if((store_size) < (MINIMAL)) return SDMMC_ERR_INVALID_SIZE;} while(0)
#define CIS_CHECK_UNSUPPORTED(COND) do {if(!(COND)) return SDMMC_ERR_NOT_SUPPORTED;} while(0)
#define CIS_GET_MINIMAL_SIZE    32

typedef sdmmc_err_t (*cis_tuple_info_func_t)(const void* tuple_info, uint8_t* data, FILE* fp);

typedef struct {
    int code;
    const char *name;
    cis_tuple_info_func_t   func;
} cis_tuple_t;

static sdmmc_err_t cis_tuple_func_default(const void* p, uint8_t* data, FILE* fp);
static sdmmc_err_t cis_tuple_func_manfid(const void* p, uint8_t* data, FILE* fp);
static sdmmc_err_t cis_tuple_func_cftable_entry(const void* p, uint8_t* data, FILE* fp);
static sdmmc_err_t cis_tuple_func_end(const void* p, uint8_t* data, FILE* fp);

static const cis_tuple_t cis_table[] = {
    CIS_TUPLE(NULL),
    CIS_TUPLE(DEVICE),
    CIS_TUPLE(CHKSUM),
    CIS_TUPLE(VERS1),
    CIS_TUPLE(ALTSTR),
    CIS_TUPLE(CONFIG),
    CIS_TUPLE_WITH_FUNC(CFTABLE_ENTRY, cis_tuple_func_cftable_entry),
    CIS_TUPLE_WITH_FUNC(MANFID, cis_tuple_func_manfid),
    CIS_TUPLE(FUNCID),
    CIS_TUPLE(FUNCE),
    CIS_TUPLE(VENDER_BEGIN),
    CIS_TUPLE(VENDER_END),
    CIS_TUPLE(SDIO_STD),
    CIS_TUPLE(SDIO_EXT),
    CIS_TUPLE_WITH_FUNC(END, cis_tuple_func_end),
};


sdmmc_err_t sdmmc_io_reset(sdmmc_card_t* card)
{
    uint8_t sdio_reset = CCCR_CTL_RES;
    sdmmc_err_t err = sdmmc_io_rw_direct(card, 0, SD_IO_CCCR_CTL, SD_ARG_CMD52_WRITE, &sdio_reset);
    if (err == SDMMC_ERR_TIMEOUT || (host_is_spi(card) && err == SDMMC_ERR_NOT_SUPPORTED)) {
        /* Non-IO cards are allowed to time out (in SD mode) or
         * return "invalid command" error (in SPI mode).
         */
    } else if (err == SDMMC_ERR_NO_CARD) {
        ESP_LOGD(TAG, "%s: card not present", __func__);
        return err;
    } else if (err != SDMMC_OK) {
        ESP_LOGE(TAG, "%s: unexpected return: 0x%x", __func__, err );
        return err;
    }
    return SDMMC_OK;
}

sdmmc_err_t sdmmc_init_io(sdmmc_card_t* card)
{
    /* IO_SEND_OP_COND(CMD5), Determine if the card is an IO card.
     * Non-IO cards will not respond to this command.
     */
    sdmmc_err_t err = sdmmc_io_send_op_cond(card, 0, &card->ocr);
    if (err != SDMMC_OK) {
        ESP_LOGD(TAG, "%s: io_send_op_cond (1) returned 0x%x; not IO card", __func__, err);
        card->is_sdio = 0;
        card->is_mem = 1;
    } else {
        card->is_sdio = 1;

        if (card->ocr & SD_IO_OCR_MEM_PRESENT) {
            ESP_LOGD(TAG, "%s: IO-only card", __func__);
            card->is_mem = 0;
        }
        card->num_io_functions = SD_IO_OCR_NUM_FUNCTIONS(card->ocr);
        ESP_LOGD(TAG, "%s: number of IO functions: %d", __func__, card->num_io_functions);
        if (card->num_io_functions == 0) {
            card->is_sdio = 0;
        }
        uint32_t host_ocr = get_host_ocr(card->host.io_voltage);
        host_ocr &= card->ocr;
        err = sdmmc_io_send_op_cond(card, host_ocr, &card->ocr);
        if (err != SDMMC_OK) {
            ESP_LOGE(TAG, "%s: sdmmc_io_send_op_cond (1) returned 0x%x", __func__, err);
            return err;
        }
        err = sdmmc_io_enable_int(card);
        if (err != SDMMC_OK) {
            ESP_LOGD(TAG, "%s: sdmmc_enable_int failed (0x%x)", __func__, err);
        }
    }
    return SDMMC_OK;
}

sdmmc_err_t sdmmc_init_io_bus_width(sdmmc_card_t* card)
{
    sdmmc_err_t err;
    card->log_bus_width = 0;
    if (card->host.flags & SDMMC_HOST_FLAG_4BIT) {
        uint8_t card_cap = 0;
        err = sdmmc_io_rw_direct(card, 0, SD_IO_CCCR_CARD_CAP,
                SD_ARG_CMD52_READ, &card_cap);
        if (err != SDMMC_OK) {
            ESP_LOGE(TAG, "%s: sdmmc_io_rw_direct (read SD_IO_CCCR_CARD_CAP) returned 0x%0x", __func__, err);
            return err;
        }
        ESP_LOGD(TAG, "IO card capabilities byte: %02x", card_cap);
        if (!(card_cap & CCCR_CARD_CAP_LSC) ||
                (card_cap & CCCR_CARD_CAP_4BLS)) {
            // This card supports 4-bit bus mode
            uint8_t bus_width = CCCR_BUS_WIDTH_4;
            err = sdmmc_io_rw_direct(card, 0, SD_IO_CCCR_BUS_WIDTH,
                                SD_ARG_CMD52_WRITE, &bus_width);
            if (err != SDMMC_OK) {
                ESP_LOGE(TAG, "%s: sdmmc_io_rw_direct (write SD_IO_CCCR_BUS_WIDTH) returned 0x%0x", __func__, err);
                return err;
            }
            card->log_bus_width = 2;
        }
    }
    return SDMMC_OK;
}


sdmmc_err_t sdmmc_io_enable_hs_mode(sdmmc_card_t* card)
{
    /* If the host is configured to use low frequency, don't attempt to switch */
    if (card->host.max_freq_khz < SDMMC_FREQ_DEFAULT) {
        card->max_freq_khz = card->host.max_freq_khz;
        return SDMMC_OK;
    } else if (card->host.max_freq_khz < SDMMC_FREQ_HIGHSPEED) {
        card->max_freq_khz = SDMMC_FREQ_DEFAULT;
        return SDMMC_OK;
    }

    /* For IO cards, do write + read operation on "High Speed" register,
     * setting EHS bit. If both EHS and SHS read back as set, then HS mode
     * has been enabled.
     */
    uint8_t val = CCCR_HIGHSPEED_ENABLE;
    sdmmc_err_t err = sdmmc_io_rw_direct(card, 0, SD_IO_CCCR_HIGHSPEED,
            SD_ARG_CMD52_WRITE | SD_ARG_CMD52_EXCHANGE, &val);
    if (err != SDMMC_OK) {
        ESP_LOGD(TAG, "%s: sdmmc_io_rw_direct returned 0x%x", __func__, err);
        return err;
    }

    ESP_LOGD(TAG, "%s: CCCR_HIGHSPEED=0x%02x", __func__, val);
    const uint8_t hs_mask = CCCR_HIGHSPEED_ENABLE | CCCR_HIGHSPEED_SUPPORT;
    if ((val & hs_mask) != hs_mask) {
        return SDMMC_ERR_NOT_SUPPORTED;
    }
    card->max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    return SDMMC_OK;
}


sdmmc_err_t sdmmc_io_send_op_cond(sdmmc_card_t* card, uint32_t ocr, uint32_t *ocrp)
{
    sdmmc_err_t err = SDMMC_OK;
    sdmmc_command_t cmd = {
        .flags = SCF_CMD_BCR | SCF_RSP_R4,
        .arg = ocr,
        .opcode = SD_IO_SEND_OP_COND
    };
    for (size_t i = 0; i < 100; i++) {
        err = sdmmc_send_cmd(card, &cmd);
        if (err != SDMMC_OK) {
            break;
        }
        if ((MMC_R4(cmd.response) & SD_IO_OCR_MEM_READY) ||
            ocr == 0) {
            break;
        }
        err = SDMMC_ERR_TIMEOUT;
        osal_task_delay(SDMMC_IO_SEND_OP_COND_DELAY_MS);
    }
    if (err == SDMMC_OK && ocrp != NULL)
        *ocrp = MMC_R4(cmd.response);

    return err;
}

sdmmc_err_t sdmmc_io_rw_direct(sdmmc_card_t* card, int func,
        uint32_t reg, uint32_t arg, uint8_t *data)
{
    sdmmc_err_t err;
    sdmmc_command_t cmd = {
        .flags = SCF_CMD_AC | SCF_RSP_R5,
        .arg = 0,
        .opcode = SD_IO_RW_DIRECT
    };

    arg |= (func & SD_ARG_CMD52_FUNC_MASK) << SD_ARG_CMD52_FUNC_SHIFT;
    arg |= (reg & SD_ARG_CMD52_REG_MASK) << SD_ARG_CMD52_REG_SHIFT;
    arg |= (*data & SD_ARG_CMD52_DATA_MASK) << SD_ARG_CMD52_DATA_SHIFT;
    cmd.arg = arg;

    err = sdmmc_send_cmd(card, &cmd);
    if (err != SDMMC_OK) {
        ESP_LOGV(TAG, "%s: sdmmc_send_cmd returned 0x%x", __func__, err);
        return err;
    }

    *data = SD_R5_DATA(cmd.response);

    return SDMMC_OK;
}


sdmmc_err_t sdmmc_io_read_byte(sdmmc_card_t* card, uint32_t function,
        uint32_t addr, uint8_t *out_byte)
{
    sdmmc_err_t ret = sdmmc_io_rw_direct(card, function, addr, SD_ARG_CMD52_READ, out_byte);
    if (ret != SDMMC_OK) {
        ESP_LOGE(TAG, "%s: sdmmc_io_rw_direct (read 0x%x) returned 0x%x", __func__, addr, ret);
    }
    return ret;
}

sdmmc_err_t sdmmc_io_write_byte(sdmmc_card_t* card, uint32_t function,
        uint32_t addr, uint8_t in_byte, uint8_t* out_byte)
{
    uint8_t tmp_byte = in_byte;
    sdmmc_err_t ret = sdmmc_io_rw_direct(card, function, addr,
            SD_ARG_CMD52_WRITE | SD_ARG_CMD52_EXCHANGE, &tmp_byte);
    if (ret != SDMMC_OK) {
        ESP_LOGE(TAG, "%s: sdmmc_io_rw_direct (write 0x%x) returned 0x%x", __func__, addr, ret);
        return ret;
    }
    if (out_byte != NULL) {
        *out_byte = tmp_byte;
    }
    return SDMMC_OK;
}

sdmmc_err_t sdmmc_io_rw_extended(sdmmc_card_t* card, int func,
    uint32_t reg, int arg, void *datap, size_t datalen)
{
    sdmmc_err_t err;
    const size_t max_byte_transfer_size = 512;
    sdmmc_command_t cmd = {
        .flags = SCF_CMD_AC | SCF_RSP_R5,
        .arg = 0,
        .opcode = SD_IO_RW_EXTENDED,
        .data = datap,
        .datalen = datalen,
        .blklen = max_byte_transfer_size /* TODO: read max block size from CIS */
    };

    uint32_t count; /* number of bytes or blocks, depending on transfer mode */
    if (arg & SD_ARG_CMD53_BLOCK_MODE) {
        if (cmd.datalen % cmd.blklen != 0) {
            return SDMMC_ERR_INVALID_SIZE;
        }
        count = cmd.datalen / cmd.blklen;
    } else {
        if (datalen > max_byte_transfer_size) {
            /* TODO: split into multiple operations? */
            return SDMMC_ERR_INVALID_SIZE;
        }
        if (datalen == max_byte_transfer_size) {
            count = 0;  // See 5.3.1 SDIO simplifed spec
        } else {
            count = datalen;
        }
        cmd.blklen = datalen;
    }

    arg |= (func & SD_ARG_CMD53_FUNC_MASK) << SD_ARG_CMD53_FUNC_SHIFT;
    arg |= (reg & SD_ARG_CMD53_REG_MASK) << SD_ARG_CMD53_REG_SHIFT;
    arg |= (count & SD_ARG_CMD53_LENGTH_MASK) << SD_ARG_CMD53_LENGTH_SHIFT;
    cmd.arg = arg;

    if ((arg & SD_ARG_CMD53_WRITE) == 0) {
        cmd.flags |= SCF_CMD_READ;
    }

    err = sdmmc_send_cmd(card, &cmd);
    if (err != SDMMC_OK) {
        ESP_LOGE(TAG, "%s: sdmmc_send_cmd returned 0x%x", __func__, err);
        return err;
    }

    return SDMMC_OK;
}

sdmmc_err_t sdmmc_io_read_bytes(sdmmc_card_t* card, uint32_t function,
        uint32_t addr, void* dst, size_t size)
{
    /* host quirk: SDIO transfer with length not divisible by 4 bytes
     * has to be split into two transfers: one with aligned length,
     * the other one for the remaining 1-3 bytes.
     */
    uint8_t *pc_dst = dst;
    while (size > 0) {
        size_t size_aligned = size & (~3);
        size_t will_transfer = size_aligned > 0 ? size_aligned : size;

        sdmmc_err_t err = sdmmc_io_rw_extended(card, function, addr,
                SD_ARG_CMD53_READ | SD_ARG_CMD53_INCREMENT,
                pc_dst, will_transfer);
        if (err != SDMMC_OK) {
            return err;
        }
        pc_dst += will_transfer;
        size -= will_transfer;
        addr += will_transfer;
    }
    return SDMMC_OK;
}

sdmmc_err_t sdmmc_io_write_bytes(sdmmc_card_t* card, uint32_t function,
        uint32_t addr, const void* src, size_t size)
{
    /* same host quirk as in sdmmc_io_read_bytes */
    const uint8_t *pc_src = (const uint8_t*) src;

    while (size > 0) {
        size_t size_aligned = size & (~3);
        size_t will_transfer = size_aligned > 0 ? size_aligned : size;

        sdmmc_err_t err = sdmmc_io_rw_extended(card, function, addr,
                SD_ARG_CMD53_WRITE | SD_ARG_CMD53_INCREMENT,
                (void*) pc_src, will_transfer);
        if (err != SDMMC_OK) {
            return err;
        }
        pc_src += will_transfer;
        size -= will_transfer;
        addr += will_transfer;
    }
    return SDMMC_OK;
}

sdmmc_err_t sdmmc_io_read_blocks(sdmmc_card_t* card, uint32_t function,
        uint32_t addr, void* dst, size_t size)
{
    if (size % 4 != 0) {
        return SDMMC_ERR_INVALID_SIZE;
    }
    return sdmmc_io_rw_extended(card, function, addr,
            SD_ARG_CMD53_READ | SD_ARG_CMD53_INCREMENT | SD_ARG_CMD53_BLOCK_MODE,
            dst, size);
}

sdmmc_err_t sdmmc_io_write_blocks(sdmmc_card_t* card, uint32_t function,
        uint32_t addr, const void* src, size_t size)
{
    if (size % 4 != 0) {
        return SDMMC_ERR_INVALID_SIZE;
    }
    return sdmmc_io_rw_extended(card, function, addr,
            SD_ARG_CMD53_WRITE | SD_ARG_CMD53_INCREMENT | SD_ARG_CMD53_BLOCK_MODE,
            (void*) src, size);
}

sdmmc_err_t sdmmc_io_enable_int(sdmmc_card_t* card)
{
    if (card->host.io_int_enable == NULL) {
        return SDMMC_ERR_NOT_SUPPORTED;
    }
    return (*card->host.io_int_enable)(card->host.slot);
}

sdmmc_err_t sdmmc_io_wait_int(sdmmc_card_t* card, int timeout_ms)
{
    if (card->host.io_int_wait == NULL) {
        return SDMMC_ERR_NOT_SUPPORTED;
    }
    return (*card->host.io_int_wait)(card->host.slot, timeout_ms);
}


/*
 * Print the CIS information of a CIS card, currently only ESP slave supported.
 */

static sdmmc_err_t cis_tuple_func_default(const void* p, uint8_t* data, FILE* fp)
{
    const cis_tuple_t* tuple = (const cis_tuple_t*)p;
    uint8_t code = *(data++);
    int size = *(data++);
    if (tuple) {
        fprintf(fp, "TUPLE: %s, size: %d: ", tuple->name, size);
    } else {
        fprintf(fp, "TUPLE: unknown(%02X), size: %d: ", code, size);
    }
    for (int i = 0; i < size; i++) fprintf(fp, "%02X ", *(data++));
    fprintf(fp, "\n");
    return SDMMC_OK;
}

static sdmmc_err_t cis_tuple_func_manfid(const void* p, uint8_t* data, FILE* fp)
{
    const cis_tuple_t* tuple = (const cis_tuple_t*)p;
    data++;
    int size = *(data++);
    fprintf(fp, "TUPLE: %s, size: %d\n", tuple->name, size);
    CIS_CHECK_SIZE(size, 4);
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-align"
    fprintf(fp, "  MANF: %04X, CARD: %04X\n", *(uint16_t*)(data), *(uint16_t*)(data+2));
    #pragma GCC diagnostic pop
    return SDMMC_OK;
}

static sdmmc_err_t cis_tuple_func_end(const void* p, uint8_t* data, FILE* fp)
{
    const cis_tuple_t* tuple = (const cis_tuple_t*)p;
    data++;
    fprintf(fp, "TUPLE: %s\n", tuple->name);
    return SDMMC_OK;
}

static sdmmc_err_t cis_tuple_func_cftable_entry(const void* p, uint8_t* data, FILE* fp)
{
    const cis_tuple_t* tuple = (const cis_tuple_t*)p;
    data++;
    int size = *(data++);
    fprintf(fp, "TUPLE: %s, size: %d\n", tuple->name, size);
    CIS_CHECK_SIZE(size, 2);

    CIS_CHECK_SIZE(size--, 1);
    bool interface = data[0] & BIT(7);
    bool def = data[0] & BIT(6);
    int conf_ent_num = data[0] & 0x3F;
    fprintf(fp, "  INDX: %02X, Intface: %d, Default: %d, Conf-Entry-Num: %d\n", *(data++), interface, def, conf_ent_num);

    if (interface) {
        CIS_CHECK_SIZE(size--, 1);
        fprintf(fp, "  IF: %02X\n", *(data++));
    }

    CIS_CHECK_SIZE(size--, 1);
    bool misc = data[0] & BIT(7);
    int mem_space = (data[0] >> 5 )&(0x3);
    bool irq = data[0] & BIT(4);
    bool io_sp = data[0] & BIT(3);
    bool timing = data[0] & BIT(2);
    int power = data[0] & 3;
    fprintf(fp, "  FS: %02X, misc: %d, mem_space: %d, irq: %d, io_space: %d, timing: %d, power: %d\n", *(data++), misc, mem_space, irq, io_sp, timing, power);

    CIS_CHECK_UNSUPPORTED(power == 0);  //power descriptor is not handled yet
    CIS_CHECK_UNSUPPORTED(!timing);     //timing descriptor is not handled yet
    CIS_CHECK_UNSUPPORTED(!io_sp);      //io space descriptor is not handled yet

    if (irq) {
        CIS_CHECK_SIZE(size--, 1);
        bool mask = data[0] & BIT(4);
        fprintf(fp, "  IR: %02X, mask: %d, ",*(data++), mask);
        if (mask) {
            CIS_CHECK_SIZE(size, 2);
            size-=2;
            fprintf(fp, "  IRQ: %02X %02X\n", data[0], data[1]);
            data+=2;
        }
    }

    if (mem_space) {
        CIS_CHECK_SIZE(size, 2);
        size-=2;
        CIS_CHECK_UNSUPPORTED(mem_space==1); //other cases not handled yet
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wcast-align"
        int len = *(uint16_t*)data;
        #pragma GCC diagnostic pop
        fprintf(fp, "  LEN: %04X\n", len);
        data+=2;
    }

    CIS_CHECK_UNSUPPORTED(misc==0);    //misc descriptor is not handled yet
    return SDMMC_OK;
}

static const cis_tuple_t* get_tuple(uint8_t code)
{
    for (size_t i = 0; i < sizeof(cis_table)/sizeof(cis_tuple_t); i++) {
        if (code == cis_table[i].code) return &cis_table[i];
    }
    return NULL;
}

sdmmc_err_t sdmmc_io_print_cis_info(uint8_t* buffer, size_t buffer_size, FILE* fp)
{
    if (!fp) fp = stdout;

    uint8_t* cis = buffer;
    do {
        const cis_tuple_t* tuple = get_tuple(cis[0]);
        int size = cis[1];
        sdmmc_err_t ret = SDMMC_OK;
        if (tuple) {
            ret = tuple->func(tuple, cis, fp);
        } else {
            ret = cis_tuple_func_default(NULL, cis, fp);
        }
        if (ret != SDMMC_OK) return ret;
        cis += 2 + size;
        if (tuple && tuple->code == CISTPL_CODE_END) break;
    } while (cis < buffer + buffer_size) ;
    return SDMMC_OK;
}

/**
 * Check tuples in the buffer.
 *
 * @param buf Buffer to check
 * @param buffer_size Size of the buffer
 * @param inout_cis_offset
 *          - input: the last cis_offset, relative to the beginning of the buf. -1 if
 *                      this buffer begin with the tuple length, otherwise should be no smaller than
 *                      zero.
 *          - output: when the end tuple found, output offset of the CISTPL_CODE_END
 *                      byte + 1 (relative to the beginning of the buffer; when not found, output
 *                      the address of next tuple code.
 *
 * @return true if found, false if haven't.
 */
static bool check_tuples_in_buffer(uint8_t* buf, int buffer_size, int* inout_cis_offset)
{
    int cis_offset = *inout_cis_offset;
    if (cis_offset == -1) {
        //the CIS code is checked in the last buffer, skip to next tuple
        cis_offset += buf[0] + 2;
    }
    assert(cis_offset >= 0);
    while (1) {
        if (cis_offset < buffer_size) {
            //A CIS code in the buffer, check it
            if (buf[cis_offset] == CISTPL_CODE_END) {
                *inout_cis_offset = cis_offset + 1;
                return true;
            }
        }
        if (cis_offset + 1 < buffer_size) {
            cis_offset += buf[cis_offset+1] + 2;
        } else {
            break;
        }
    }
    *inout_cis_offset = cis_offset;
    return false;
}

sdmmc_err_t sdmmc_io_get_cis_data(sdmmc_card_t* card, uint8_t* out_buffer, size_t buffer_size, size_t* inout_cis_size)
{
    sdmmc_err_t ret = SDMMC_OK;
    uint32_t buf[CIS_GET_MINIMAL_SIZE / sizeof(uint32_t)];

    /* Pointer to size is a mandatory parameter */
    assert(inout_cis_size);

    /*
     * CIS region exist in 0x1000~0x17FFF of FUNC 0, get the start address of it
     * from CCCR register.
     */
    uint32_t addr;
    ret = sdmmc_io_read_bytes(card, 0, 9, &addr, 3);
    if (ret != SDMMC_OK) return ret;
    //the sdmmc_io driver reads 4 bytes, the most significant byte is not the address.
    addr &= 0xffffff;
    if (addr < 0x1000 || addr > 0x17FFF) {
        return SDMMC_ERR_INVALID_RESPONSE;
    }

    /*
     * To avoid reading too long, take the input value as limitation if
     * existing.
     */
    size_t max_reading = UINT32_MAX;
    if (*inout_cis_size != 0) {
        max_reading = *inout_cis_size;
    }

    /*
     * Parse the length while reading. If find the end tuple, or reaches the
     * limitation, read no more and return both the data and the size already
     * read.
     */
    size_t buffer_offset = 0;
    size_t cur_cis_offset = 0;
    bool end_tuple_found = false;
    do {
        ret = sdmmc_io_read_bytes(card, 0, addr + buffer_offset, &buf, CIS_GET_MINIMAL_SIZE);
        if (ret != SDMMC_OK) return ret;

        //calculate relative to the beginning of the buffer
        int offset = cur_cis_offset - buffer_offset;
        bool finish = check_tuples_in_buffer((uint8_t*) buf, CIS_GET_MINIMAL_SIZE, &offset);

        int remain_size = buffer_size - buffer_offset;
        int copy_len;
        if (finish) {
            copy_len = TSD_MIN(offset, remain_size);
            end_tuple_found = true;
        } else {
            copy_len = TSD_MIN(CIS_GET_MINIMAL_SIZE, remain_size);
        }
        if (copy_len > 0) {
            memcpy(out_buffer + buffer_offset, buf, copy_len);
        }
        cur_cis_offset = buffer_offset + offset;
        buffer_offset += CIS_GET_MINIMAL_SIZE;
    } while (!end_tuple_found && buffer_offset < max_reading);

    if (end_tuple_found) {
        *inout_cis_size = cur_cis_offset;
        if (cur_cis_offset > buffer_size) {
            return SDMMC_ERR_INVALID_SIZE;
        } else {
            return SDMMC_OK;
        }
    } else {
        return SDMMC_ERR_NO_CARD;
    }
}

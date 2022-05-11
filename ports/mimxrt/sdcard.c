/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Philipp Ebensberger
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#if MICROPY_PY_MACHINE_SDCARD

#include "sdcard.h"
#include "ticks.h"
#include "fsl_iomuxc.h"

#define SDCARD_VOLTAGE_WINDOW_SD                (0x80100000U)
#define SDCARD_HIGH_CAPACITY                    (0x40000000U)
#define SDCARD_SWITCH_1_8V_CAPACITY             ((uint32_t)0x01000000U)
#define SDCARD_MAX_VOLT_TRIAL                   ((uint32_t)0x000000FFU)

// Error
#define SDCARD_STATUS_OUT_OF_RANGE_SHIFT        (31U)
#define SDCARD_STATUS_ADDRESS_ERROR_SHIFT       (30U)
#define SDCARD_STATUS_BLOCK_LEN_ERROR_SHIFT     (29U)
#define SDCARD_STATUS_ERASE_SEQ_ERROR_SHIFT     (28U)
#define SDCARD_STATUS_ERASE_PARAM_SHIFT         (27U)
#define SDCARD_STATUS_WP_VIOLATION_SHIFT        (26U)
#define SDCARD_STATUS_LOCK_UNLOCK_FAILED_SHIFT  (24U)
#define SDCARD_STATUS_COM_CRC_ERROR_SHIFT       (23U)
#define SDCARD_STATUS_ILLEGAL_COMMAND_SHIFT     (22U)
#define SDCARD_STATUS_CARD_ECC_FAILED_SHIFT     (21U)
#define SDCARD_STATUS_CC_ERROR_SHIFT            (20U)
#define SDCARD_STATUS_ERROR_SHIFT               (19U)
#define SDCARD_STATUS_CSD_OVERWRITE_SHIFT       (16U)
#define SDCARD_STATUS_WP_ERASE_SKIP_SHIFT       (15U)
#define SDCARD_STATUS_AUTH_SEQ_ERR_SHIFT        (3U)

// Status Flags
#define SDCARD_STATUS_CARD_IS_LOCKED_SHIFT      (25U)
#define SDCARD_STATUS_CARD_ECC_DISABLED_SHIFT   (14U)
#define SDCARD_STATUS_ERASE_RESET_SHIFT         (13U)
#define SDCARD_STATUS_READY_FOR_DATA_SHIFT      (8U)
#define SDCARD_STATUS_FX_EVENT_SHIFT            (6U)
#define SDCARD_STATUS_APP_CMD_SHIFT             (5U)

#define SDMMC_MASK(bit) (1U << (bit))
#define SDMMC_R1_ALL_ERROR_FLAG \
    (SDMMC_MASK(SDCARD_STATUS_OUT_OF_RANGE_SHIFT)) | \
    (SDMMC_MASK(SDCARD_STATUS_ADDRESS_ERROR_SHIFT)) | \
    (SDMMC_MASK(SDCARD_STATUS_BLOCK_LEN_ERROR_SHIFT)) | \
    (SDMMC_MASK(SDCARD_STATUS_ERASE_SEQ_ERROR_SHIFT)) | \
    (SDMMC_MASK(SDCARD_STATUS_ERASE_PARAM_SHIFT)) | \
    (SDMMC_MASK(SDCARD_STATUS_WP_VIOLATION_SHIFT)) | \
    (SDMMC_MASK(SDCARD_STATUS_LOCK_UNLOCK_FAILED_SHIFT)) | \
    (SDMMC_MASK(SDCARD_STATUS_COM_CRC_ERROR_SHIFT)) | \
    (SDMMC_MASK(SDCARD_STATUS_ILLEGAL_COMMAND_SHIFT)) | \
    (SDMMC_MASK(SDCARD_STATUS_CARD_ECC_FAILED_SHIFT)) | \
    (SDMMC_MASK(SDCARD_STATUS_CC_ERROR_SHIFT)) | \
    (SDMMC_MASK(SDCARD_STATUS_ERROR_SHIFT)) | \
    (SDMMC_MASK(SDCARD_STATUS_CSD_OVERWRITE_SHIFT)) | \
    (SDMMC_MASK(SDCARD_STATUS_WP_ERASE_SKIP_SHIFT)) | \
    (SDMMC_MASK(SDCARD_STATUS_AUTH_SEQ_ERR_SHIFT))

#define SDMMC_R1_CURRENT_STATE(x) (((x) & 0x00001E00U) >> 9U)

// ---
// SD Card command identifiers
// ---
enum
{
    SDCARD_CMD_GO_IDLE_STATE        = 0U,
    SDCARD_CMD_ALL_SEND_CID         = 2U,
    SDCARD_CMD_SEND_REL_ADDR        = 3U,
    SDCARD_CMD_SET_DSR              = 4U,
    SDCARD_CMD_SELECT_CARD          = 7U,
    SDCARD_CMD_SEND_IF_COND         = 8U,
    SDCARD_CMD_SEND_CSD             = 9U,
    SDCARD_CMD_SEND_CID             = 10U,
    SDCARD_CMD_STOP_TRANSMISSION    = 12U,
    SDCARD_CMD_SEND_STATUS          = 13U,
    SDCARD_CMD_GO_INACTIVE_STATE    = 15U,
    SDCARD_CMD_SET_BLOCKLENGTH      = 16U,
    SDCARD_CMD_READ_SINGLE_BLOCK    = 17U,
    SDCARD_CMD_READ_MULTIPLE_BLOCK  = 18U,
    SDCARD_CMD_SET_BLOCK_COUNT      = 23U,
    SDCARD_CMD_WRITE_SINGLE_BLOCK   = 24U,
    SDCARD_CMD_WRITE_MULTIPLE_BLOCK = 25U,
    SDCARD_CMD_PROGRAM_CSD          = 27U,
    SDCARD_CMD_SET_WRITE_PROTECT    = 28U,
    SDCARD_CMD_CLEAR_WRITE_PROTECT  = 29U,
    SDCARD_CMD_SEND_WRITE_PROTECT   = 30U,
    SDCARD_CMD_ERASE                = 38U,
    SDCARD_CMD_LOCK_UNLOCK          = 42U,
    SDCARD_CMD_APP_CMD              = 55U,
    SDCARD_CMD_GEN_CMD              = 56U,
    SDCARD_CMD_READ_OCR             = 58U,
};

// ---
// SD Card application command identifiers
// ---
enum
{
    SDCARD_ACMD_SET_BUS_WIDTH       =  6U,
    SDCARD_ACMD_SD_SEND_OP_COND     = 41U,
};

// ---
// SD Card state identifiers
// ---
enum
{
    SDCARD_STATE_IDLE        = 0U,
    SDCARD_STATE_READY       = 1U,
    SDCARD_STATE_IDENTIFY    = 2U,
    SDCARD_STATE_STANDBY     = 3U,
    SDCARD_STATE_TRANSFER    = 4U,
    SDCARD_STATE_SENDDATA    = 5U,
    SDCARD_STATE_RECEIVEDATA = 6U,
    SDCARD_STATE_PROGRAM     = 7U,
    SDCARD_STATE_DISCONNECT  = 8U,
};

// ---
// SD Card transfer status
// ---
typedef enum
{
    SDCARD_TRANSFER_SUCCESS = 0,
    SDCARD_TRANSFER_ERROR,
    SDCARD_TRANSFER_PENDING
} sdcard_transfer_status_t;

// ---
// SD Card type definitions
// ---
typedef struct _cid_t {
    uint8_t reserved_0;
    uint16_t mdt : 12;
    uint16_t reserved_1 : 4;
    uint32_t psn;
    uint8_t prv;
    char pnm[6];
    uint16_t oid;
    uint8_t mid;
} __attribute__((packed)) cid_t;

typedef struct _csd_t {
    uint32_t data[4];
} __attribute__((packed)) csd_t;

typedef struct _csr_t {
    uint32_t data[2];
} __attribute__((packed)) csr_t;

#define DMA_DESCRIPTOR_BUFFER_SIZE (32U)
#define DMA_DESCRIPTOR_BUFFER_ALIGN_SIZE  (4U)
AT_NONCACHEABLE_SECTION_ALIGN(static uint32_t sdcard_adma_descriptor_table[DMA_DESCRIPTOR_BUFFER_SIZE], DMA_DESCRIPTOR_BUFFER_ALIGN_SIZE);

#if defined MICROPY_USDHC1 && USDHC1_AVAIL
const mimxrt_sdcard_obj_pins_t mimxrt_sdcard_1_obj_pins = MICROPY_USDHC1;
#endif

#if defined MICROPY_USDHC2 && USDHC2_AVAIL
const mimxrt_sdcard_obj_pins_t mimxrt_sdcard_2_obj_pins = MICROPY_USDHC2;
#endif

#if defined MICROPY_USDHC1 && USDHC1_AVAIL
mimxrt_sdcard_status_obj_t sdcard_usdhc1_state = {.initialized = false, .inserted = false};
#endif
#if defined MICROPY_USDHC2 && USDHC2_AVAIL
mimxrt_sdcard_status_obj_t sdcard_usdhc2_state = {.initialized = false, .inserted = false};
#endif

mimxrt_sdcard_obj_t mimxrt_sdcard_objs[] =
{
    #if defined MICROPY_USDHC1 && USDHC1_AVAIL
    {
        .base.type = &machine_sdcard_type,
        .usdhc_inst = USDHC1,
        .state = &sdcard_usdhc1_state,
        .rca = 0x0UL,
        .block_len = SDCARD_DEFAULT_BLOCK_SIZE,
        .block_count = 0UL,
        .pins = &mimxrt_sdcard_1_obj_pins,
    },
    #endif
    #if defined MICROPY_USDHC2 && USDHC2_AVAIL
    {
        .base.type = &machine_sdcard_type,
        .usdhc_inst = USDHC2,
        .state = &sdcard_usdhc2_state,
        .rca = 0x0UL,
        .block_len = SDCARD_DEFAULT_BLOCK_SIZE,
        .block_count = 0UL,
        .pins = &mimxrt_sdcard_2_obj_pins,
    };
    #endif
};

volatile status_t sdcard_transfer_status;
volatile bool sdcard_transfer_done;

// ---
// Local function declarations
// ---
static status_t sdcard_transfer_blocking(USDHC_Type *base,
    usdhc_handle_t *handle,
    usdhc_transfer_t *transfer,
    uint32_t timeout_ms);
static void sdcard_decode_csd(mimxrt_sdcard_obj_t *sdcard, csd_t *csd);
static bool sdcard_reset(mimxrt_sdcard_obj_t *card);
static inline void sdcard_init_pin(const machine_pin_obj_t *pin, uint8_t af_idx, uint32_t config_value);

// SD Card interrupt callbacks
void sdcard_card_inserted_callback(USDHC_Type *base, void *userData);
void sdcard_card_removed_callback(USDHC_Type *base, void *userData);
void sdcard_transfer_complete_callback(USDHC_Type *base, usdhc_handle_t *handle, status_t status, void *userData);
void sdcard_dummy_callback(USDHC_Type *base, void *userData);

// SD Card commmands
static bool sdcard_cmd_go_idle_state(mimxrt_sdcard_obj_t *card);
static bool sdcard_cmd_oper_cond(mimxrt_sdcard_obj_t *card);
static bool sdcard_cmd_app_cmd(mimxrt_sdcard_obj_t *card);
static bool sdcard_cmd_sd_app_op_cond(mimxrt_sdcard_obj_t *card, uint32_t argument);
static bool sdcard_cmd_all_send_cid(mimxrt_sdcard_obj_t *card, cid_t *cid);
static bool sdcard_cmd_send_cid(mimxrt_sdcard_obj_t *card, cid_t *cid);
static bool sdcard_cmd_set_rel_add(mimxrt_sdcard_obj_t *card);
static bool sdcard_cmd_send_csd(mimxrt_sdcard_obj_t *card, csd_t *csd);
static bool sdcard_cmd_select_card(mimxrt_sdcard_obj_t *sdcard);
static bool sdcard_cmd_set_blocklen(mimxrt_sdcard_obj_t *sdcard);
static bool sdcard_cmd_set_bus_width(mimxrt_sdcard_obj_t *sdcard, uint8_t bus_width);

void sdcard_card_inserted_callback(USDHC_Type *base, void *userData) {
    #if defined MICROPY_USDHC1 && USDHC1_AVAIL
    if (base == USDHC1) {
        sdcard_usdhc1_state.inserted = true;
    }
    #endif
    #if defined MICROPY_USDHC2 && USDHC2_AVAIL
    if (base == USDHC2) {
        sdcard_usdhc2_state.inserted = true;

    }
    #endif

    USDHC_ClearInterruptStatusFlags(base, kUSDHC_CardInsertionFlag);
}

void sdcard_card_removed_callback(USDHC_Type *base, void *userData) {
    #if defined MICROPY_USDHC1 && USDHC1_AVAIL
    if (base == USDHC1) {
        sdcard_usdhc1_state.inserted = false;
        sdcard_usdhc1_state.initialized = false;
    }
    #endif
    #if defined MICROPY_USDHC2 && USDHC2_AVAIL
    if (base == USDHC2) {
        sdcard_usdhc2_state.inserted = false;
        sdcard_usdhc2_state.initialized = false;
    }
    #endif

    USDHC_ClearInterruptStatusFlags(base, kUSDHC_CardRemovalFlag);
}

void sdcard_transfer_complete_callback(USDHC_Type *base, usdhc_handle_t *handle, status_t status, void *userData) {
    sdcard_transfer_status = status;
    sdcard_transfer_done = true;
    USDHC_ClearInterruptStatusFlags(base, kUSDHC_CommandCompleteFlag | kUSDHC_DataCompleteFlag);
}

void sdcard_dummy_callback(USDHC_Type *base, void *userData) {
    return;
}

static status_t sdcard_transfer_blocking(USDHC_Type *base, usdhc_handle_t *handle, usdhc_transfer_t *transfer, uint32_t timeout_ms) {
    uint32_t retry_ctr = 0UL;
    status_t status;

    usdhc_adma_config_t dma_config;

    (void)memset(&dma_config, 0, sizeof(usdhc_adma_config_t));
    dma_config.dmaMode = kUSDHC_DmaModeAdma2;
    dma_config.burstLen = kUSDHC_EnBurstLenForINCR;
    dma_config.admaTable = sdcard_adma_descriptor_table;
    dma_config.admaTableWords = DMA_DESCRIPTOR_BUFFER_SIZE;

    do {
        status = USDHC_TransferNonBlocking(base, handle, &dma_config, transfer);
        retry_ctr++;
    } while (!(status == kStatus_Success) && (retry_ctr < 1000000UL));

    if (status == kStatus_Success) {
        for (int i = 0; i < timeout_ms * 100; i++) {
            if ((sdcard_transfer_done == true) && (sdcard_transfer_status == kStatus_Success)) {
                sdcard_transfer_done = false;
                return kStatus_Success;
            }
            ticks_delay_us64(10);
        }
        return kStatus_Timeout;
    } else {
        return status;
    }
}

static void sdcard_decode_csd(mimxrt_sdcard_obj_t *card, csd_t *csd) {
    uint8_t csd_structure = 0x3 & (csd->data[3] >> 30);
    uint8_t read_bl_len;
    uint32_t c_size;
    uint8_t c_size_mult;

    switch (csd_structure)
    {
        case 0: {
            read_bl_len = 0xF & (csd->data[2] >> 16);
            c_size = ((0x3FF & csd->data[2]) << 30) | (0x3 & (csd->data[1] >> 30));
            c_size_mult = 0x7 & (csd->data[1] >> 15);

            card->block_len = (1U << (read_bl_len));
            card->block_count = ((c_size + 1U) << (c_size_mult + 2U));

            if (card->block_len != SDCARD_DEFAULT_BLOCK_SIZE) {
                card->block_count = (card->block_count * card->block_len);
                card->block_len = SDCARD_DEFAULT_BLOCK_SIZE;
                card->block_count = (card->block_count / card->block_len);
            }
            break;
        }
        case 1: {
            c_size = ((0x3F & csd->data[2]) << 16) | (0xFFFF & (csd->data[1] >> 16));

            card->block_len = 512UL;
            card->block_count = (uint32_t)(((uint64_t)(c_size + 1U) * (uint64_t)1024UL));
            break;
        }
        case 2: {
            c_size = ((0xFF & csd->data[2]) << 16) | (0xFFFF & (csd->data[1] >> 16));

            card->block_len = 512UL;
            card->block_count = (uint32_t)(((uint64_t)(c_size + 1U) * (uint64_t)1024UL));
            break;
        }
        default: {
            break;
        }
    }
}

static bool sdcard_cmd_go_idle_state(mimxrt_sdcard_obj_t *card) {
    status_t status;
    usdhc_command_t command = {
        .index = SDCARD_CMD_GO_IDLE_STATE,
        .argument = 0UL,
        .type = kCARD_CommandTypeNormal,
        .responseType = kCARD_ResponseTypeNone,
    };
    usdhc_transfer_t transfer = {
        .data = NULL,
        .command = &command,
    };

    status = sdcard_transfer_blocking(card->usdhc_inst, &card->handle, &transfer, 250);

    if (status == kStatus_Success) {
        return true;
    } else {
        return false;
    }
}

static bool sdcard_cmd_oper_cond(mimxrt_sdcard_obj_t *card) {
    status_t status;
    usdhc_command_t command = {
        .index = SDCARD_CMD_SEND_IF_COND,
        .argument = 0x000001AAU,       // 2.7-3.3V range and 0xAA check pattern
        .type = kCARD_CommandTypeNormal,
        .responseType = kCARD_ResponseTypeR7,
    };
    usdhc_transfer_t transfer = {
        .data = NULL,
        .command = &command,
    };

    status = sdcard_transfer_blocking(card->usdhc_inst, &card->handle, &transfer, 250);

    if (status == kStatus_Success) {
        card->oper_cond = command.response[0];
        return true;
    } else {
        return false;
    }
}

static bool sdcard_cmd_app_cmd(mimxrt_sdcard_obj_t *card) {
    status_t status;
    usdhc_command_t command = {
        .index = SDCARD_CMD_APP_CMD,
        .argument = (card->rca << 16),
        .type = kCARD_CommandTypeNormal,
        .responseType = kCARD_ResponseTypeR1,
    };
    usdhc_transfer_t transfer = {
        .data = NULL,
        .command = &command,
    };

    status = sdcard_transfer_blocking(card->usdhc_inst, &card->handle, &transfer, 250);

    if (status == kStatus_Success) {
        card->status = command.response[0];
        return true;
    } else {
        return false;
    }
}

static bool sdcard_cmd_sd_app_op_cond(mimxrt_sdcard_obj_t *card, uint32_t argument) {
    if (!sdcard_cmd_app_cmd(card)) {
        return false;
    }

    status_t status;
    usdhc_command_t command = {
        .index = SDCARD_ACMD_SD_SEND_OP_COND,
        .argument = argument,
        .type = kCARD_CommandTypeNormal,
        .responseType = kCARD_ResponseTypeR3,
    };

    usdhc_transfer_t transfer = {
        .data = NULL,
        .command = &command,
    };

    status = sdcard_transfer_blocking(card->usdhc_inst, &card->handle, &transfer, 250);

    if (status == kStatus_Success) {
        card->oper_cond = command.response[0];
        return true;
    } else {
        return false;
    }
}

static bool sdcard_cmd_all_send_cid(mimxrt_sdcard_obj_t *card, cid_t *cid) {
    status_t status;
    usdhc_command_t command = {
        .index = SDCARD_CMD_ALL_SEND_CID,
        .argument = 0UL,
        .type = kCARD_CommandTypeNormal,
        .responseType = kCARD_ResponseTypeR2,
    };
    usdhc_transfer_t transfer = {
        .data = NULL,
        .command = &command,
    };

    status = sdcard_transfer_blocking(card->usdhc_inst, &card->handle, &transfer, 250);

    if (status == kStatus_Success) {
        cid->mdt = (uint16_t)((command.response[0] & 0xFFF00U) >> 8U);
        cid->psn = (uint32_t)(((command.response[1] & 0xFFFFFFU) << 8U) | ((command.response[0] & 0xFF000000U) >> 24U));
        cid->prv = (uint8_t)((command.response[1] & 0xFF000000U) >> 24U);
        cid->pnm[0] = (char)(command.response[2] & 0xFFU);
        cid->pnm[1] = (char)((command.response[2] & 0xFF00U) >> 8U);
        cid->pnm[2] = (char)((command.response[2] & 0xFF0000U) >> 16U);
        cid->pnm[3] = (char)((command.response[2] & 0xFF000000U) >> 24U);
        cid->pnm[4] = (char)(command.response[3] & 0xFFU);
        cid->pnm[5] = '\0';
        cid->oid = (uint16_t)((command.response[3] & 0xFFFF00U) >> 8U);
        cid->mid = (uint8_t)((command.response[3] & 0xFF000000U) >> 24U);
        return true;
    } else {
        return false;
    }
}

static bool sdcard_cmd_send_cid(mimxrt_sdcard_obj_t *card, cid_t *cid) {
    status_t status;
    usdhc_command_t command = {
        .index = SDCARD_CMD_SEND_CID,
        .argument = (card->rca << 16),
        .type = kCARD_CommandTypeNormal,
        .responseType = kCARD_ResponseTypeR2,
    };
    usdhc_transfer_t transfer = {
        .data = NULL,
        .command = &command,
    };

    status = sdcard_transfer_blocking(card->usdhc_inst, &card->handle, &transfer, 250);

    if (status == kStatus_Success) {
        cid->mdt = (uint16_t)((command.response[0] & 0xFFF00U) >> 8U);
        cid->psn = (uint32_t)(((command.response[1] & 0xFFFFFFU) << 8U) | ((command.response[0] & 0xFF000000U) >> 24U));
        cid->prv = (uint8_t)((command.response[1] & 0xFF000000U) >> 24U);
        cid->pnm[0] = (char)(command.response[2] & 0xFFU);
        cid->pnm[1] = (char)((command.response[2] & 0xFF00U) >> 8U);
        cid->pnm[2] = (char)((command.response[2] & 0xFF0000U) >> 16U);
        cid->pnm[3] = (char)((command.response[2] & 0xFF000000U) >> 24U);
        cid->pnm[4] = (char)(command.response[3] & 0xFFU);
        cid->pnm[5] = '\0';
        cid->oid = (uint16_t)((command.response[3] & 0xFFFF00U) >> 8U);
        cid->mid = (uint8_t)((command.response[3] & 0xFF000000U) >> 24U);
        return true;
    } else {
        return false;
    }
}

static bool sdcard_cmd_set_rel_add(mimxrt_sdcard_obj_t *card) {
    status_t status;
    usdhc_command_t command = {
        .index = SDCARD_CMD_SEND_REL_ADDR,
        .argument = 0UL,
        .type = kCARD_CommandTypeNormal,
        .responseType = kCARD_ResponseTypeR6,
    };
    usdhc_transfer_t transfer = {
        .data = NULL,
        .command = &command,
    };

    status = sdcard_transfer_blocking(card->usdhc_inst, &card->handle, &transfer, 250);

    if (status == kStatus_Success) {
        card->rca = 0xFFFFFFFF & (command.response[0] >> 16);
        return true;
    } else {
        return false;
    }
}

static bool sdcard_cmd_send_csd(mimxrt_sdcard_obj_t *card, csd_t *csd) {
    status_t status;
    usdhc_command_t command = {
        .index = SDCARD_CMD_SEND_CSD,
        .argument = (card->rca << 16),
        .type = kCARD_CommandTypeNormal,
        .responseType = kCARD_ResponseTypeR2,
    };
    usdhc_transfer_t transfer = {
        .data = NULL,
        .command = &command,
    };

    status = sdcard_transfer_blocking(card->usdhc_inst, &card->handle, &transfer, 250);

    if (status == kStatus_Success) {
        csd->data[0] = command.response[0];
        csd->data[1] = command.response[1];
        csd->data[2] = command.response[2];
        csd->data[3] = command.response[3];
        return true;
    } else {
        return false;
    }
}

static bool sdcard_cmd_select_card(mimxrt_sdcard_obj_t *card) {
    status_t status;
    usdhc_command_t command = {
        .index = SDCARD_CMD_SELECT_CARD,
        .argument = (card->rca << 16),
        .type = kCARD_CommandTypeNormal,
        .responseType = kCARD_ResponseTypeR1b,
        .responseErrorFlags = SDMMC_R1_ALL_ERROR_FLAG,
    };
    usdhc_transfer_t transfer = {
        .data = NULL,
        .command = &command,
    };

    status = sdcard_transfer_blocking(card->usdhc_inst, &card->handle, &transfer, 250);

    if (status == kStatus_Success) {
        card->status = command.response[0];
        return true;
    } else {
        return false;
    }
}

static bool sdcard_cmd_set_blocklen(mimxrt_sdcard_obj_t *card) {
    status_t status;
    usdhc_command_t command = {
        .index = SDCARD_CMD_SET_BLOCKLENGTH,
        .argument = card->block_len,
        .type = kCARD_CommandTypeNormal,
        .responseType = kCARD_ResponseTypeR1,
        .responseErrorFlags = SDMMC_R1_ALL_ERROR_FLAG,
    };
    usdhc_transfer_t transfer = {
        .data = NULL,
        .command = &command,
    };

    status = sdcard_transfer_blocking(card->usdhc_inst, &card->handle, &transfer, 250);

    if (status == kStatus_Success) {
        card->status = command.response[0];
        return true;
    } else {
        return false;
    }
}

static bool sdcard_cmd_set_bus_width(mimxrt_sdcard_obj_t *card, usdhc_data_bus_width_t bus_width) {
    if (!sdcard_cmd_app_cmd(card)) {
        return false;
    }

    status_t status;
    usdhc_command_t command = {
        .index = SDCARD_ACMD_SET_BUS_WIDTH,
        .type = kCARD_CommandTypeNormal,
        .responseType = kCARD_ResponseTypeR1,
    };

    if (bus_width == kUSDHC_DataBusWidth1Bit) {
        command.argument = 0U;
    } else if (bus_width == kUSDHC_DataBusWidth4Bit) {
        command.argument = 2U;
    } else {
        return false;  // Invalid argument
    }

    usdhc_transfer_t transfer = {
        .data = NULL,
        .command = &command,
    };

    status = sdcard_transfer_blocking(card->usdhc_inst, &card->handle, &transfer, 250);

    if (status == kStatus_Success) {
        card->status = command.response[0];
        return true;
    } else {
        return false;
    }
}

static bool sdcard_reset(mimxrt_sdcard_obj_t *card) {
    card->block_len = SDCARD_DEFAULT_BLOCK_SIZE;
    card->rca = 0UL;
    card->block_count = 0UL;
    card->status = 0UL;
    card->oper_cond = 0UL;
    card->state->initialized = false;
    return USDHC_Reset(card->usdhc_inst, (USDHC_SYS_CTRL_RSTA_MASK | USDHC_SYS_CTRL_RSTC_MASK | USDHC_SYS_CTRL_RSTD_MASK), 2048);
}

void sdcard_init(mimxrt_sdcard_obj_t *card, uint32_t base_clk) {
    // Configure PFD0 of PLL2 (system PLL) fractional divider to 24 resulting in:
    //  with PFD0_clk = PLL2_clk * 18 / N
    //       PFD0_clk = 528MHz   * 18 / 24 = 396MHz
    CLOCK_InitSysPfd(kCLOCK_Pfd0, 24U);

    #if defined MICROPY_USDHC1 && USDHC1_AVAIL
    // Configure USDHC clock source and divider
    CLOCK_SetDiv(kCLOCK_Usdhc1Div, 1U);  // USDHC_input_clk = PFD0_clk / 2 = 198MHZ
    CLOCK_SetMux(kCLOCK_Usdhc1Mux, 1U);  // Select PFD0 as clock input for USDHC
    #endif

    #if defined MICROPY_USDHC2 && USDHC2_AVAIL
    // Configure USDHC clock source and divider
    CLOCK_SetDiv(kCLOCK_Usdhc2Div, 1U);  // USDHC_input_clk = PFD0_clk / 2 = 198MHZ
    CLOCK_SetMux(kCLOCK_Usdhc2Mux, 1U);  // Select PFD0 as clock input for USDHC
    #endif

    // Initialize USDHC
    const usdhc_config_t config = {
        .endianMode = kUSDHC_EndianModeLittle,
        .dataTimeout = 0xFU,
        .readWatermarkLevel = 128U,
        .writeWatermarkLevel = 128U,
    };
    USDHC_Init(card->usdhc_inst, &config);

    (void)sdcard_reset(card);
    card->base_clk = base_clk;

    usdhc_transfer_callback_t callbacks = {
        .CardInserted = sdcard_card_inserted_callback,
        .CardRemoved = sdcard_card_removed_callback,
        .SdioInterrupt = sdcard_dummy_callback,
        .BlockGap = sdcard_dummy_callback,
        .TransferComplete = sdcard_transfer_complete_callback,
        .ReTuning = sdcard_dummy_callback,
    };

    USDHC_TransferCreateHandle(card->usdhc_inst, &card->handle, &callbacks, NULL);
}

void sdcard_deinit(mimxrt_sdcard_obj_t *card) {
    sdcard_power_off(card);
    USDHC_Deinit(card->usdhc_inst);
}

static inline void sdcard_init_pin(const machine_pin_obj_t *pin, uint8_t af_idx, uint32_t config_value) {
    machine_pin_af_obj_t af = pin->af_list[af_idx];

    IOMUXC_SetPinMux(pin->muxRegister, af.af_mode, af.input_register, af.input_daisy, pin->configRegister, 0U);
    IOMUXC_SetPinConfig(pin->muxRegister, af.af_mode, af.input_register, af.input_daisy, pin->configRegister, config_value);
}

void sdcard_init_pins(mimxrt_sdcard_obj_t *card) {
    // speed and strength optimized for clock frequency < 100MHz
    const mimxrt_sdcard_obj_pins_t *pins = card->pins;

    uint32_t default_config = pin_generate_config(
        PIN_PULL_UP_47K, PIN_MODE_SKIP, PIN_DRIVE_6, card->pins->clk.pin->configRegister);
    #if USDHC_DATA3_PULL_DOWN_ON_BOARD
    // Pull down on the board -> must not enable internal PD.
    uint32_t no_cd_config = pin_generate_config(
        PIN_PULL_DISABLED, PIN_MODE_SKIP, PIN_DRIVE_6, card->pins->data3.pin->configRegister);
    #else
    uint32_t no_cd_config = pin_generate_config(
        PIN_PULL_DOWN_100K, PIN_MODE_SKIP, PIN_DRIVE_6, card->pins->data3.pin->configRegister);
    #endif // USDHC_DATA3_PULL_DOWN_ON_BOARD

    sdcard_init_pin(card->pins->clk.pin, card->pins->clk.af_idx, default_config);  // USDHC1_CLK
    sdcard_init_pin(card->pins->cmd.pin, card->pins->cmd.af_idx, default_config);  // USDHC1_CMD
    sdcard_init_pin(card->pins->data0.pin, card->pins->data0.af_idx, default_config);  // USDHC1_DATA0
    sdcard_init_pin(card->pins->data1.pin, card->pins->data1.af_idx, default_config);  // USDHC1_DATA1
    sdcard_init_pin(card->pins->data2.pin, card->pins->data2.af_idx, default_config);  // USDHC1_DATA2

    if (pins->cd_b.pin) {
        sdcard_init_pin(card->pins->data3.pin, card->pins->data3.af_idx, default_config);  // USDHC1_DATA3
        sdcard_init_pin(card->pins->cd_b.pin, card->pins->cd_b.af_idx, default_config);  // USDHC1_CD_B
        USDHC_CardDetectByData3(card->usdhc_inst, false);
    } else {
        sdcard_init_pin(card->pins->data3.pin, card->pins->data3.af_idx, no_cd_config);  // USDHC1_DATA3
        USDHC_CardDetectByData3(card->usdhc_inst, true);
    }
}

bool sdcard_read(mimxrt_sdcard_obj_t *card, uint8_t *buffer, uint32_t block_num, uint32_t block_count) {
    if (!card->state->initialized) {
        return false;
    }

    usdhc_data_t data = {
        .enableAutoCommand12 = true,
        .enableAutoCommand23 = false,
        .enableIgnoreError = false,
        .dataType = kUSDHC_TransferDataNormal,
        .blockSize = card->block_len,
        .blockCount = block_count,
        .rxData = (uint32_t *)buffer,
        .txData = NULL,
    };

    usdhc_command_t command = {
        .index = (block_count == 1U) ? (uint32_t)SDCARD_CMD_READ_SINGLE_BLOCK : (uint32_t)SDCARD_CMD_READ_MULTIPLE_BLOCK,
        .argument = block_num,
        .type = kCARD_CommandTypeNormal,
        .responseType = kCARD_ResponseTypeR1,
        .responseErrorFlags = SDMMC_R1_ALL_ERROR_FLAG,
    };

    usdhc_transfer_t transfer = {
        .data = &data,
        .command = &command,
    };

    status_t status = sdcard_transfer_blocking(card->usdhc_inst, &card->handle, &transfer, 500);

    if (status == kStatus_Success) {
        card->status = command.response[0];
        return true;
    } else {
        return false;
    }
}

bool sdcard_write(mimxrt_sdcard_obj_t *card, uint8_t *buffer, uint32_t block_num, uint32_t block_count) {
    if (!card->state->initialized) {
        return false;
    }

    usdhc_data_t data = {
        .enableAutoCommand12 = true,
        .enableAutoCommand23 = false,
        .enableIgnoreError = false,
        .dataType = kUSDHC_TransferDataNormal,
        .blockSize = card->block_len,
        .blockCount = block_count,
        .rxData = NULL,
        .txData = (uint32_t *)buffer,
    };

    usdhc_command_t command = {
        .index = (block_count == 1U) ? (uint32_t)SDCARD_CMD_WRITE_SINGLE_BLOCK : (uint32_t)SDCARD_CMD_WRITE_MULTIPLE_BLOCK,
        .argument = block_num,
        .type = kCARD_CommandTypeNormal,
        .responseType = kCARD_ResponseTypeR1,
        .responseErrorFlags = SDMMC_R1_ALL_ERROR_FLAG,
    };

    usdhc_transfer_t transfer = {
        .data = &data,
        .command = &command,
    };

    status_t status = sdcard_transfer_blocking(card->usdhc_inst, &card->handle, &transfer, 3000);

    if (status == kStatus_Success) {
        card->status = command.response[0];
        return true;
    } else {
        return false;
    }
}

bool sdcard_set_active(mimxrt_sdcard_obj_t *card) {
    return USDHC_SetCardActive(card->usdhc_inst, 8192);
}

bool sdcard_probe_bus_voltage(mimxrt_sdcard_obj_t *card) {
    bool valid_voltage = false;
    uint32_t count = 0UL;
    bool status = false;

    // Perform voltage validation
    while ((count < SDCARD_MAX_VOLT_TRIAL) && (valid_voltage == false)) {
        status = sdcard_cmd_sd_app_op_cond(card, (uint32_t)(SDCARD_VOLTAGE_WINDOW_SD |
            SDCARD_HIGH_CAPACITY |
            SDCARD_SWITCH_1_8V_CAPACITY));
        if (status == false) {
            return false;
        }

        /* Get operating voltage*/
        valid_voltage = (((card->oper_cond >> 31U) == 1U) ? true : false);
        count++;
        ticks_delay_us64(1000);
    }

    if (count >= SDCARD_MAX_VOLT_TRIAL) {
        return false;
    } else {
        return true;
    }
}

bool sdcard_power_on(mimxrt_sdcard_obj_t *card) {
    bool status = false;

    // Check if card is already initialized and powered on
    if (card->state->initialized) {
        return true;
    }

    USDHC_SetDataBusWidth(card->usdhc_inst, kUSDHC_DataBusWidth1Bit);
    card->bus_clk = USDHC_SetSdClock(card->usdhc_inst, card->base_clk, SDCARD_CLOCK_400KHZ);

    // Start initialization process
    status = sdcard_reset(card);
    if (!status) {
        return false;
    }

    status = sdcard_set_active(card);
    if (!status) {
        return false;
    }

    status = sdcard_cmd_go_idle_state(card);
    if (!status) {
        return false;
    }

    status = sdcard_cmd_oper_cond(card);
    if (!status) {
        return false;
    }

    status = sdcard_probe_bus_voltage(card);
    if (!status) {
        return false;
    }

    // ===
    // Ready State
    // ===
    cid_t cid_all;
    status = sdcard_cmd_all_send_cid(card, &cid_all);
    if (!status) {
        return false;
    }

    // ===
    // Identification State
    // ===
    status = sdcard_cmd_set_rel_add(card);
    if (!status) {
        return false;
    }

    // ===
    // Standby State
    // ===
    card->bus_clk = USDHC_SetSdClock(card->usdhc_inst, card->base_clk, SDCARD_CLOCK_50MHZ);

    csd_t csd;
    status = sdcard_cmd_send_csd(card, &csd);
    if (!status) {
        return false;
    }
    sdcard_decode_csd(card, &csd);

    cid_t cid;
    status = sdcard_cmd_send_cid(card, &cid);
    if (!status) {
        return false;
    }

    // ===
    // Transfer State
    // ===
    status = sdcard_cmd_select_card(card);
    if (!status) {
        return false;
    }

    status = sdcard_cmd_set_blocklen(card);
    if (!status) {
        return false;
    }

    status = sdcard_cmd_set_bus_width(card, kUSDHC_DataBusWidth4Bit);
    if (!status) {
        return false;
    }
    USDHC_SetDataBusWidth(card->usdhc_inst, kUSDHC_DataBusWidth4Bit);


    status = sdcard_cmd_set_blocklen(card);
    if (!status) {
        return false;
    }

    // Finialize initialization
    card->state->initialized = true;
    return true;
}

bool sdcard_power_off(mimxrt_sdcard_obj_t *card) {
    (void)sdcard_cmd_go_idle_state(card);

    // Reset card bus clock
    USDHC_SetDataBusWidth(card->usdhc_inst, kUSDHC_DataBusWidth1Bit);
    card->bus_clk = USDHC_SetSdClock(card->usdhc_inst, card->base_clk, SDCARD_CLOCK_400KHZ);

    (void)sdcard_reset(card);
    return true;
}

bool sdcard_detect(mimxrt_sdcard_obj_t *card) {
    bool detect = false;

    #if defined MICROPY_USDHC1 && USDHC1_AVAIL
    if ((card->usdhc_inst == USDHC1) && (sdcard_usdhc1_state.inserted == true)) {
        return true;
    }
    #endif
    #if defined MICROPY_USDHC2 && USDHC2_AVAIL
    if ((card->usdhc_inst == USDHC2) && (sdcard_usdhc2_state.inserted == true)) {
        return true;
    }
    #endif

    if (card->pins->cd_b.pin) {
        detect = USDHC_DetectCardInsert(card->usdhc_inst);
    } else {
        USDHC_CardDetectByData3(card->usdhc_inst, true);
        detect = (USDHC_GetPresentStatusFlags(card->usdhc_inst) & USDHC_PRES_STATE_DLSL(8)) != 0;
    }

    // Update card state when detected via pin state
    #if defined MICROPY_USDHC1 && USDHC1_AVAIL
    if (card->usdhc_inst == USDHC1) {
        sdcard_usdhc1_state.inserted = detect;
        sdcard_usdhc1_state.initialized = detect ? sdcard_usdhc1_state.initialized : false;
    }
    #endif
    #if defined MICROPY_USDHC2 && USDHC2_AVAIL
    if (card->usdhc_inst == USDHC2) {
        sdcard_usdhc2_state.inserted = detect;
        sdcard_usdhc2_state.initialized = detect ? sdcard_usdhc2_state.initialized : false;
    }
    #endif

    return detect;
}

#endif // MICROPY_PY_MACHINE_SDCARD

/**
 * \file
 *
 * \brief Common SD/MMC stack
 *
 * Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <string.h>

#include "conf_sd_mmc.h"

#include "sd_mmc_protocol.h"
#include "sd_mmc.h"

#if CONF_OS_SUPPORT
#include <hal_rtos.h>
#endif
#include <hal_gpio.h>
#include <utils.h>
#include <utils_assert.h>
#include <utils_repeat_macro.h>

#define TPASTE2(a, b) a##b
#define ATPASTE2(a, b) TPASTE2(a, b)

/**
 * \ingroup sd_mmc_stack
 * \defgroup sd_mmc_stack_internal Implementation of SD/MMC/SDIO Stack
 * @{
 */

#ifndef CONF_SD_MMC_MEM_CNT
#define CONF_SD_MMC_MEM_CNT 0
#endif

/* Macros to switch SD MMC stack to the correct driver (MCI or SPI) */
#if (CONF_SD_MMC_MEM_CNT != 0)
#if CONF_MCI_OS_SUPPORT
#include "hal_mci_os.h"
#else
#include "hal_mci_sync.h"
#endif
#define driver mci
#else
#error No MCI or HSMCI interfaces are defined for SD MMC stack. \
    CONF_SD_MMC_MEM_CNT must be added in board.h file.
#endif

#if CONF_MCI_OS_SUPPORT
#define driver_select_device ATPASTE2(driver, _os_select_device)
#define driver_deselect_device ATPASTE2(driver, _os_deselect_device)
#define driver_get_bus_width ATPASTE2(driver, _os_get_bus_width)
#define driver_is_high_speed_capable ATPASTE2(driver, _os_is_high_speed_capable)
#define driver_send_clock ATPASTE2(driver, _os_send_init_sequence)
#define driver_send_cmd ATPASTE2(driver, _os_send_cmd)
#define driver_get_response ATPASTE2(driver, _os_get_response)
#define driver_get_response_128 ATPASTE2(driver, _os_get_response_128)
#define driver_adtc_start ATPASTE2(driver, _os_adtc_start)
#define driver_adtc_stop ATPASTE2(driver, _os_send_cmd)
#define driver_read_word ATPASTE2(driver, _os_read_bytes)
#define driver_write_word ATPASTE2(driver, _os_write_bytes)
#define driver_start_read_blocks ATPASTE2(driver, _os_start_read_blocks)
#define driver_wait_end_of_read_blocks ATPASTE2(driver, _os_wait_end_of_read_blocks)
#define driver_start_write_blocks ATPASTE2(driver, _os_start_write_blocks)
#define driver_wait_end_of_write_blocks ATPASTE2(driver, _os_wait_end_of_write_blocks)
#else
#define driver_select_device ATPASTE2(driver, _sync_select_device)
#define driver_deselect_device ATPASTE2(driver, _sync_deselect_device)
#define driver_get_bus_width ATPASTE2(driver, _sync_get_bus_width)
#define driver_is_high_speed_capable ATPASTE2(driver, _sync_is_high_speed_capable)
#define driver_send_clock ATPASTE2(driver, _sync_send_clock)
#define driver_send_cmd ATPASTE2(driver, _sync_send_cmd)
#define driver_get_response ATPASTE2(driver, _sync_get_response)
#define driver_get_response_128 ATPASTE2(driver, _sync_get_response_128)
#define driver_adtc_start ATPASTE2(driver, _sync_adtc_start)
#define driver_adtc_stop ATPASTE2(driver, _sync_send_cmd)
#define driver_read_word ATPASTE2(driver, _sync_read_word)
#define driver_write_word ATPASTE2(driver, _sync_write_word)
#define driver_start_read_blocks ATPASTE2(driver, _sync_start_read_blocks)
#define driver_wait_end_of_read_blocks ATPASTE2(driver, _sync_wait_end_of_read_blocks)
#define driver_start_write_blocks ATPASTE2(driver, _sync_start_write_blocks)
#define driver_wait_end_of_write_blocks ATPASTE2(driver, _sync_wait_end_of_write_blocks)
#endif

#if (CONF_SDIO_SUPPORT == 1)
#define IS_SDIO() (sd_mmc_card->type & CARD_TYPE_SDIO)
#else
#define IS_SDIO() false
#endif

/** This SD MMC stack supports only the high voltage */
#define SD_MMC_VOLTAGE_SUPPORT                                                                                         \
    (OCR_VDD_27_28 | OCR_VDD_28_29 | OCR_VDD_29_30 | OCR_VDD_30_31 | OCR_VDD_31_32 | OCR_VDD_32_33)

/** SD/MMC card states */
enum card_state {
    SD_MMC_CARD_STATE_READY    = 0,     /**< Ready to use */
    SD_MMC_CARD_STATE_DEBOUNCE = 1,     /**< Debounce on going */
    SD_MMC_CARD_STATE_INIT     = 2,     /**< Initialization on going */
    SD_MMC_CARD_STATE_UNUSABLE = 3,     /**< Unusable card */
    SD_MMC_CARD_STATE_NO_CARD  = 4      /**< No SD/MMC card inserted */
};

/** SD/MMC card information structure */
struct sd_mmc_card {
    uint32_t clock;                         /**< Card access clock */
    uint32_t capacity;                      /**< Card capacity in KBytes */
    uint16_t rca;                           /**< Relative card address */
    enum card_state state;                  /**< Card state */
    card_type_t type;                       /**< Card type */
    card_version_t version;                 /**< Card version */
    uint8_t bus_width;                      /**< Number of DATA lin on bus (MCI only) */
    uint8_t csd[CSD_REG_BSIZE];             /**< CSD register */
    uint8_t high_speed;                     /**< High speed card (1) */
};

/** Card detect pin */
static sd_mmc_detect_t *_cd;
/** Write protect pin */
static sd_mmc_detect_t *_wp;

/** SD/MMC card list */
/** Note: Initialize card detect pin fields if present */
static struct sd_mmc_card sd_mmc_cards[CONF_SD_MMC_MEM_CNT];

/** HAL driver instance */
static void *sd_mmc_hal;
/** Index of current slot configurated */
static uint8_t sd_mmc_slot_sel;
/** Pointer on current slot configurated */
static struct sd_mmc_card *sd_mmc_card;
/** Number of block to read or write on the current transfer */
static uint16_t sd_mmc_nb_block_to_tranfer = 0;
/** Number of block remaining to read or write on the current transfer */
static uint16_t sd_mmc_nb_block_remaining = 0;

/** SD/MMC transfer rate unit codes (10K) list */
const uint32_t sd_mmc_trans_units[7] = {10, 100, 1000, 10000, 0, 0, 0};
/** SD transfer multiplier factor codes (1/10) list */
const uint32_t sd_trans_multipliers[16] = {0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80};
/** MMC transfer multiplier factor codes (1/10) list */
const uint32_t mmc_trans_multipliers[16] = {0, 10, 12, 13, 15, 20, 26, 30, 35, 40, 45, 52, 55, 60, 70, 80};

/** \name MMC, SD and SDIO commands process */
/** @{ */
#if CONF_MMC_SUPPORT
static bool mmc_mci_op_cond(void);
static bool mmc_cmd6_set_bus_width(uint8_t bus_width);
static bool mmc_cmd6_set_high_speed(void);
static bool mmc_cmd8(uint8_t *b_authorize_high_speed);
static void mmc_decode_csd(void);
#endif
static bool sd_mci_op_cond(uint8_t v2);
static bool sdio_op_cond(void);
static bool sdio_get_max_speed(void);
static bool sdio_cmd52_set_bus_width(void);
static bool sdio_cmd52_set_high_speed(void);
static bool sd_cm6_set_high_speed(void);
static bool sd_cmd8(uint8_t *v2);
static bool sd_mmc_cmd9_mci(void);
static void sd_decode_csd(void);
static bool sd_mmc_cmd13(void);
#if (CONF_SDIO_SUPPORT == 1)
static bool sdio_cmd52(uint8_t rw_flag, uint8_t func_nb, uint32_t reg_addr, uint8_t rd_after_wr, uint8_t *io_data);
static bool sdio_cmd53(uint8_t rw_flag, uint8_t func_nb, uint32_t reg_addr, uint8_t inc_addr, uint32_t size,
    bool access_block);
#endif
static bool sd_acmd6(void);
static bool sd_acmd51(void);
/** @} */

/** \name Internal function to process the initialization and install */
/** @{ */
static sd_mmc_err_t sd_mmc_select_slot(uint8_t slot);
static void         sd_mmc_configure_slot(void);
static void         sd_mmc_deselect_slot(void);
static bool         sd_mmc_mci_card_init(void);
#if CONF_MMC_SUPPORT
static bool sd_mmc_mci_install_mmc(void);
#endif
/** @} */

/** \name Internal functions to manage a large timeout after a card insertion */
/** @{ */
#if CONF_OS_SUPPORT
#define SD_MMC_START_TIMEOUT() os_sleep(CONF_SD_MMC_DEBOUNCE)
#else
#define SD_MMC_START_TIMEOUT()
#endif
#define SD_MMC_IS_TIMEOUT() true
#define SD_MMC_STOP_TIMEOUT()
/** @} */

#if CONF_MMC_SUPPORT
/**
 * \brief Sends operation condition command and read OCR (MCI only)
 * - CMD1 sends operation condition command
 * - CMD1 reads OCR
 *
 * \return true if success, otherwise false
 */
static bool mmc_mci_op_cond(void) {
    uint32_t retry, resp;

    /*
     * Timeout 1s = 400KHz / ((6+6)*8) cylces = 4200 retry
     * 6 = cmd byte size
     * 6 = response byte size
     */
    retry = 4200;
    do {
        if (!driver_send_cmd(sd_mmc_hal, MMC_MCI_CMD1_SEND_OP_COND, SD_MMC_VOLTAGE_SUPPORT | OCR_ACCESS_MODE_SECTOR)) {
            return false;
        }
        /* Check busy flag */
        resp = driver_get_response(sd_mmc_hal);
        if (resp & OCR_POWER_UP_BUSY) {
            /* Check OCR value */
            if ((resp & OCR_ACCESS_MODE_MASK) == OCR_ACCESS_MODE_SECTOR) {
                sd_mmc_card->type |= CARD_TYPE_HC;
            }
            break;
        }
        if (retry-- == 0) {
            return false;
        }
    } while (1);
    return true;
}
#endif

/**
 * \brief Ask to all cards to send their operations conditions (MCI only).
 * - ACMD41 sends operation condition command.
 * - ACMD41 reads OCR
 *
 * \param v2   Shall be 1 if it is a SD card V2
 *
 * \return true if success, otherwise false
 */
static bool sd_mci_op_cond(uint8_t v2) {
    uint32_t arg, retry, resp;

    /*
     * Timeout 1s = 400KHz / ((6+6+6+6)*8) cylces = 2100 retry
     * 6 = cmd byte size
     * 6 = response byte size
     * 6 = cmd byte size
     * 6 = response byte size
     */
    retry = 2100;
    do {
        /* CMD55 - Indicate to the card that the next command is an
         * application specific command rather than a standard command.*/
        if (!driver_send_cmd(sd_mmc_hal, SDMMC_CMD55_APP_CMD, 0)) {
            return false;
        }

        /* (ACMD41) Sends host OCR register */
        arg = SD_MMC_VOLTAGE_SUPPORT;
        if (v2) {
            arg |= SD_ACMD41_HCS;
        }
        /* Check response */
        if (!driver_send_cmd(sd_mmc_hal, SD_MCI_ACMD41_SD_SEND_OP_COND, arg)) {
            return false;
        }
        resp = driver_get_response(sd_mmc_hal);
        if (resp & OCR_POWER_UP_BUSY) {
            /* Card is ready */
            if ((resp & OCR_CCS) != 0) {
                sd_mmc_card->type |= CARD_TYPE_HC;
            }
            break;
        }
        if (retry-- == 0) {
            return false;
        }
    } while (1);
    return true;
}

#if (CONF_SDIO_SUPPORT == 1)
/**
 * \brief Try to get the SDIO card's operating condition
 * - CMD5 to read OCR NF field
 * - CMD5 to wait OCR power up busy
 * - CMD5 to read OCR MP field
 *   sd_mmc_card->type is updated
 *
 * \return true if success, otherwise false
 */
static bool sdio_op_cond(void) {
    uint32_t resp;

    /* CMD5 - SDIO send operation condition (OCR) command. */
    if (!driver_send_cmd(sd_mmc_hal, SDIO_CMD5_SEND_OP_COND, 0)) {
        return true;         /* No error but card type not updated */
    }
    resp = driver_get_response(sd_mmc_hal);
    if ((resp & OCR_SDIO_NF) == 0) {
        return true;         /* No error but card type not updated */
    }

    /*
     * Wait card ready
     * Timeout 1s = 400KHz / ((6+4)*8) cylces = 5000 retry
     * 6 = cmd byte size
     * 4(SPI) 6(MCI) = response byte size
     */
    uint32_t cmd5_retry = 5000;
    while (1) {
        /* CMD5 - SDIO send operation condition (OCR) command.*/
        if (!driver_send_cmd(sd_mmc_hal, SDIO_CMD5_SEND_OP_COND, resp & SD_MMC_VOLTAGE_SUPPORT)) {
            return false;
        }
        resp = driver_get_response(sd_mmc_hal);
        if ((resp & OCR_POWER_UP_BUSY) == OCR_POWER_UP_BUSY) {
            break;
        }
        if (cmd5_retry-- == 0) {
            return false;
        }
    }
    /* Update card type at the end of busy */
    if ((resp & OCR_SDIO_MP) > 0) {
        sd_mmc_card->type = CARD_TYPE_SD_COMBO;
    } else {
        sd_mmc_card->type = CARD_TYPE_SDIO;
    }
    return true;     /* No error and card type updated with SDIO type */
}

/**
 * \brief Get SDIO max transfer speed in Hz.
 * - CMD53 reads CIS area address in CCCR area.
 * - Nx CMD53 search Fun0 tuple in CIS area
 * - CMD53 reads TPLFE_MAX_TRAN_SPEED in Fun0 tuple
 * - Compute maximum speed of SDIO
 *   and update sd_mmc_card->clock
 *
 * \return true if success, otherwise false
 */
static bool sdio_get_max_speed(void) {
    uint32_t addr_new, addr_old;
    uint8_t buf[6];
    uint32_t unit;
    uint32_t mul;
    uint8_t tplfe_max_tran_speed, i;
    uint8_t addr_cis[4];

    /* Read CIS area address in CCCR area */
    addr_old = SDIO_CCCR_CIS_PTR;
    for (i = 0; i < 4; i++) {
        sdio_cmd52(SDIO_CMD52_READ_FLAG, SDIO_CIA, addr_old, 0, &addr_cis[i]);
        addr_old++;
    }
    addr_old = addr_cis[0] + (addr_cis[1] << 8) + (addr_cis[2] << 16) + (addr_cis[3] << 24);
    addr_new = addr_old;

    while (1) {
        /* Read a sample of CIA area */
        for (i = 0; i < 3; i++) {
            sdio_cmd52(SDIO_CMD52_READ_FLAG, SDIO_CIA, addr_new, 0, &buf[i]);
            addr_new++;
        }
        if (buf[0] == SDIO_CISTPL_END) {
            return false;             /* Tuple error */
        }
        if (buf[0] == SDIO_CISTPL_FUNCE && buf[2] == 0x00) {
            break;             /* Fun0 tuple found */
        }
        if (buf[1] == 0) {
            return false;             /* Tuple error */
        }
        /* Next address */
        addr_new += buf[1] - 1;
        if (addr_new > (addr_old + 256)) {
            return false;             /* Outoff CIS area */
        }
    }

    /* Read all Fun0 tuple fields: fn0_blk_siz & max_tran_speed */
    addr_new -= 3;
    for (i = 0; i < 6; i++) {
        sdio_cmd52(SDIO_CMD52_READ_FLAG, SDIO_CIA, addr_new, 0, &buf[i]);
        addr_new++;
    }

    tplfe_max_tran_speed = buf[5];
    if (tplfe_max_tran_speed > 0x32) {
        /* Error on SDIO register, the high speed is not activated
         * and the clock can not be more than 25MHz.
         * This error is present on specific SDIO card
         * (H&D wireless card - HDG104 WiFi SIP).
         */
        tplfe_max_tran_speed = 0x32;         /* 25Mhz */
    }

    /* Decode transfer speed in Hz.*/
    unit = sd_mmc_trans_units[tplfe_max_tran_speed & 0x7];
    mul = sd_trans_multipliers[(tplfe_max_tran_speed >> 3) & 0xF];
    sd_mmc_card->clock = unit * mul * 1000;
    /**
     * Note: A combo card shall be a Full-Speed SDIO card
     * which supports upto 25MHz.
     * A SDIO card alone can be:
     * - a Low-Speed SDIO card which supports 400Khz minimum
     * - a Full-Speed SDIO card which supports upto 25MHz
     */
    return true;
}

/**
 * \brief CMD52 for SDIO - Switches the bus width mode to 4
 *
 * \note sd_mmc_card->bus_width is updated.
 *
 * \return true if success, otherwise false
 */
static bool sdio_cmd52_set_bus_width(void) {
    /**
     * A SD memory card always supports bus 4bit
     * A SD COMBO card always supports bus 4bit
     * A SDIO Full-Speed alone always supports 4bit
     * A SDIO Low-Speed alone can supports 4bit (Optional)
     */
    uint8_t u8_value;

    /* Check 4bit support in 4BLS of "Card Capability" register */
    if (!sdio_cmd52(SDIO_CMD52_READ_FLAG, SDIO_CIA, SDIO_CCCR_CAP, 0, &u8_value)) {
        return false;
    }
    if ((u8_value & SDIO_CAP_4BLS) != SDIO_CAP_4BLS) {
        /* No supported, it is not a protocol error */
        return true;
    }
    /* HS mode possible, then enable */
    u8_value = SDIO_BUSWIDTH_4B;
    if (!sdio_cmd52(SDIO_CMD52_WRITE_FLAG, SDIO_CIA, SDIO_CCCR_BUS_CTRL, 1, &u8_value)) {
        return false;
    }
    sd_mmc_card->bus_width = 4;
    return true;
}

/**
 * \brief CMD52 for SDIO - Enable the high speed mode
 *
 * \note sd_mmc_card->high_speed is updated.
 * \note sd_mmc_card->clock is updated.
 *
 * \return true if success, otherwise false
 */
static bool sdio_cmd52_set_high_speed(void) {
    uint8_t u8_value;

    /* Check CIA.HS */
    if (!sdio_cmd52(SDIO_CMD52_READ_FLAG, SDIO_CIA, SDIO_CCCR_HS, 0, &u8_value)) {
        return false;
    }
    if ((u8_value & SDIO_SHS) != SDIO_SHS) {
        /* No supported, it is not a protocol error */
        return true;
    }
    /* HS mode possible, then enable */
    u8_value = SDIO_EHS;
    if (!sdio_cmd52(SDIO_CMD52_WRITE_FLAG, SDIO_CIA, SDIO_CCCR_HS, 1, &u8_value)) {
        return false;
    }
    sd_mmc_card->high_speed = 1;
    sd_mmc_card->clock *= 2;
    return true;
}

#else
static bool sdio_op_cond(void) {
    return true;     /* No error but card type not updated */
}
static bool sdio_get_max_speed(void) {
    return false;
}
static bool sdio_cmd52_set_bus_width(void) {
    return false;
}
static bool sdio_cmd52_set_high_speed(void) {
    return false;
}
#endif

/**
 * \brief CMD6 for SD - Switch card in high speed mode
 *
 * \note CMD6 for SD is valid under the "trans" state.
 * \note sd_mmc_card->high_speed is updated.
 * \note sd_mmc_card->clock is updated.
 *
 * \return true if success, otherwise false
 */
static bool sd_cm6_set_high_speed(void) {
    uint8_t switch_status[SD_SW_STATUS_BSIZE] = {0};

    if (!driver_adtc_start(sd_mmc_hal,
        SD_CMD6_SWITCH_FUNC,
        SD_CMD6_MODE_SWITCH | SD_CMD6_GRP6_NO_INFLUENCE | SD_CMD6_GRP5_NO_INFLUENCE
        | SD_CMD6_GRP4_NO_INFLUENCE | SD_CMD6_GRP3_NO_INFLUENCE | SD_CMD6_GRP2_DEFAULT
        | SD_CMD6_GRP1_HIGH_SPEED,
        SD_SW_STATUS_BSIZE,
        1,
        true)) {
        return false;
    }
    if (!driver_start_read_blocks(sd_mmc_hal, switch_status, 1)) {
        return false;
    }
    if (!driver_wait_end_of_read_blocks(sd_mmc_hal)) {
        return false;
    }

    if (driver_get_response(sd_mmc_hal) & CARD_STATUS_SWITCH_ERROR) {
        return false;
    }
    if (SD_SW_STATUS_FUN_GRP1_RC(switch_status) == SD_SW_STATUS_FUN_GRP_RC_ERROR) {
        /* No supported, it is not a protocol error */
        return true;
    }
    if (SD_SW_STATUS_FUN_GRP1_BUSY(switch_status)) {
        return false;
    }
    /* CMD6 function switching period is within 8 clocks
     * after the end bit of status data.*/
    driver_send_clock(sd_mmc_hal);
    sd_mmc_card->high_speed = 1;
    sd_mmc_card->clock *= 2;
    return true;
}

#if CONF_MMC_SUPPORT

/**
 * \brief CMD6 for MMC - Switches the bus width mode
 *
 * \note CMD6 is valid under the "trans" state.
 * \note sd_mmc_card->bus_width is updated.
 *
 * \param bus_width   Bus width to set
 *
 * \return true if success, otherwise false
 */
static bool mmc_cmd6_set_bus_width(uint8_t bus_width) {
    uint32_t arg;

    switch (bus_width) {
        case 8:
            arg = MMC_CMD6_ACCESS_SET_BITS | MMC_CMD6_INDEX_BUS_WIDTH | MMC_CMD6_VALUE_BUS_WIDTH_8BIT;
            break;
        case 4:
            arg = MMC_CMD6_ACCESS_SET_BITS | MMC_CMD6_INDEX_BUS_WIDTH | MMC_CMD6_VALUE_BUS_WIDTH_4BIT;
            break;
        default:
            arg = MMC_CMD6_ACCESS_SET_BITS | MMC_CMD6_INDEX_BUS_WIDTH | MMC_CMD6_VALUE_BUS_WIDTH_1BIT;
            break;
    }
    if (!driver_send_cmd(sd_mmc_hal, MMC_CMD6_SWITCH, arg)) {
        return false;
    }
    if (driver_get_response(sd_mmc_hal) & CARD_STATUS_SWITCH_ERROR) {
        /* No supported, it is not a protocol error */
        return false;
    }
    sd_mmc_card->bus_width = bus_width;
    return true;
}

/**
 * \brief CMD6 for MMC - Switches in high speed mode
 *
 * \note CMD6 is valid under the "trans" state.
 * \note sd_mmc_card->high_speed is updated.
 * \note sd_mmc_card->clock is updated.
 *
 * \return true if success, otherwise false
 */
static bool mmc_cmd6_set_high_speed(void) {
    if (!driver_send_cmd(sd_mmc_hal,
        MMC_CMD6_SWITCH,
        MMC_CMD6_ACCESS_WRITE_BYTE | MMC_CMD6_INDEX_HS_TIMING | MMC_CMD6_VALUE_HS_TIMING_ENABLE)) {
        return false;
    }
    if (driver_get_response(sd_mmc_hal) & CARD_STATUS_SWITCH_ERROR) {
        /* No supported, it is not a protocol error */
        return false;
    }
    sd_mmc_card->high_speed = 1;
    sd_mmc_card->clock = 52000000lu;
    return true;
}
#endif

/**
 * \brief CMD8 for SD card - Send Interface Condition Command.
 *
 * \note
 * Send SD Memory Card interface condition, which includes host supply
 * voltage information and asks the card whether card supports voltage.
 * Should be performed at initialization time to detect the card type.
 *
 * \param v2 Pointer to v2 flag to update
 *
 * \return true if success, otherwise false
 *         with a update of \ref sd_mmc_err.
 */
static bool sd_cmd8(uint8_t *v2) {
    uint32_t resp;

    *v2 = 0;
    /* Test for SD version 2 */
    if (!driver_send_cmd(sd_mmc_hal, SD_CMD8_SEND_IF_COND, SD_CMD8_PATTERN | SD_CMD8_HIGH_VOLTAGE)) {
        return true;         /* It is not a V2 */
    }
    /* Check R7 response */
    resp = driver_get_response(sd_mmc_hal);
    if (resp == 0xFFFFFFFF) {
        /* No compliance R7 value */
        return true;         /* It is not a V2 */
    }
    if ((resp & (SD_CMD8_MASK_PATTERN | SD_CMD8_MASK_VOLTAGE)) != (SD_CMD8_PATTERN | SD_CMD8_HIGH_VOLTAGE)) {
        return false;
    }
    *v2 = 1;
    return true;
}

#if CONF_MMC_SUPPORT
/**
 * \brief CMD8 - The card sends its EXT_CSD register as a block of data.
 *
 * \param b_authorize_high_speed Pointer to update with the high speed
 * support information
 *
 * \return true if success, otherwise false
 */
static bool mmc_cmd8(uint8_t *b_authorize_high_speed) {
    uint16_t i;
    uint32_t ext_csd;
    uint32_t sec_count;

    if (!driver_adtc_start(sd_mmc_hal, MMC_CMD8_SEND_EXT_CSD, 0, EXT_CSD_BSIZE, 1, false)) {
        return false;
    }
    /* Read and decode Extended Extended CSD
     * Note: The read access is done in byte to avoid a buffer
     * of EXT_CSD_BSIZE Byte in stack.*/

    /* Read card type */
    for (i = 0; i < (EXT_CSD_CARD_TYPE_INDEX + 4) / 4; i++) {
        if (!driver_read_word(sd_mmc_hal, &ext_csd)) {
            return false;
        }
    }
    *b_authorize_high_speed = (ext_csd >> ((EXT_CSD_CARD_TYPE_INDEX % 4) * 8)) & MMC_CTYPE_52MHZ;

    if (MMC_CSD_C_SIZE(sd_mmc_card->csd) == 0xFFF) {
        /* For high capacity SD/MMC card,
         * memory capacity = SEC_COUNT * 512 byte */
        for (; i < (EXT_CSD_SEC_COUNT_INDEX + 4) / 4; i++) {
            if (!driver_read_word(sd_mmc_hal, &sec_count)) {
                return false;
            }
        }
        sd_mmc_card->capacity = sec_count / 2;
    }
    for (; i < EXT_CSD_BSIZE / 4; i++) {
        if (!driver_read_word(sd_mmc_hal, &sec_count)) {
            return false;
        }
    }
    return true;
}

#endif

/**
 * \brief CMD9: Addressed card sends its card-specific
 * data (CSD) on the CMD line mci.
 *
 * \return true if success, otherwise false
 */
static bool sd_mmc_cmd9_mci(void) {
    if (!driver_send_cmd(sd_mmc_hal, SDMMC_MCI_CMD9_SEND_CSD, (uint32_t)sd_mmc_card->rca << 16)) {
        return false;
    }
    driver_get_response_128(sd_mmc_hal, sd_mmc_card->csd);
    return true;
}

#if CONF_MMC_SUPPORT
/**
 * \brief Decodes MMC CSD register
 */
static void mmc_decode_csd(void) {
    uint32_t unit;
    uint32_t mul;
    uint32_t tran_speed;

    /* Get MMC System Specification version supported by the card */
    switch (MMC_CSD_SPEC_VERS(sd_mmc_card->csd)) {
        default:
        case 0:
            sd_mmc_card->version = CARD_VER_MMC_1_2;
            break;

        case 1:
            sd_mmc_card->version = CARD_VER_MMC_1_4;
            break;

        case 2:
            sd_mmc_card->version = CARD_VER_MMC_2_2;
            break;

        case 3:
            sd_mmc_card->version = CARD_VER_MMC_3;
            break;

        case 4:
            sd_mmc_card->version = CARD_VER_MMC_4;
            break;
    }

    /* Get MMC memory max transfer speed in Hz.*/
    tran_speed = CSD_TRAN_SPEED(sd_mmc_card->csd);
    unit = sd_mmc_trans_units[tran_speed & 0x7];
    mul = mmc_trans_multipliers[(tran_speed >> 3) & 0xF];
    sd_mmc_card->clock = unit * mul * 1000;

    /*
     * Get card capacity.
     * ----------------------------------------------------
     * For normal SD/MMC card:
     * memory capacity = BLOCKNR * BLOCK_LEN
     * Where
     * BLOCKNR = (C_SIZE+1) * MULT
     * MULT = 2 ^ (C_SIZE_MULT+2)       (C_SIZE_MULT < 8)
     * BLOCK_LEN = 2 ^ READ_BL_LEN      (READ_BL_LEN < 12)
     * ----------------------------------------------------
     * For high capacity SD/MMC card:
     * memory capacity = SEC_COUNT * 512 byte
     */
    if (MMC_CSD_C_SIZE(sd_mmc_card->csd) != 0xFFF) {
        uint32_t blocknr
            = ((MMC_CSD_C_SIZE(sd_mmc_card->csd) + 1) * (1 << (MMC_CSD_C_SIZE_MULT(sd_mmc_card->csd) + 2)));
        sd_mmc_card->capacity = blocknr * (1 << MMC_CSD_READ_BL_LEN(sd_mmc_card->csd)) / 1024;
    }
}
#endif

/**
 * \brief Decodes SD CSD register
 */
static void sd_decode_csd(void) {
    uint32_t unit;
    uint32_t mul;
    uint32_t tran_speed;

    /* Get SD memory maximum transfer speed in Hz. */
    tran_speed = CSD_TRAN_SPEED(sd_mmc_card->csd);
    unit = sd_mmc_trans_units[tran_speed & 0x7];
    mul = sd_trans_multipliers[(tran_speed >> 3) & 0xF];
    sd_mmc_card->clock = unit * mul * 1000;

    /*
     * Get card capacity.
     * ----------------------------------------------------
     * For normal SD/MMC card:
     * memory capacity = BLOCKNR * BLOCK_LEN
     * Where
     * BLOCKNR = (C_SIZE+1) * MULT
     * MULT = 2 ^ (C_SIZE_MULT+2)       (C_SIZE_MULT < 8)
     * BLOCK_LEN = 2 ^ READ_BL_LEN      (READ_BL_LEN < 12)
     * ----------------------------------------------------
     * For high capacity SD card:
     * memory capacity = (C_SIZE+1) * 512K byte
     */
    if (CSD_STRUCTURE_VERSION(sd_mmc_card->csd) >= SD_CSD_VER_2_0) {
        sd_mmc_card->capacity = (SD_CSD_2_0_C_SIZE(sd_mmc_card->csd) + 1) * 512;
    } else {
        uint32_t blocknr
            = ((SD_CSD_1_0_C_SIZE(sd_mmc_card->csd) + 1) * (1 << (SD_CSD_1_0_C_SIZE_MULT(sd_mmc_card->csd) + 2)));
        sd_mmc_card->capacity = blocknr * (1 << SD_CSD_1_0_READ_BL_LEN(sd_mmc_card->csd)) / 1024;
    }
}

/**
 * \brief CMD13 - Addressed card sends its status register.
 * This function waits the clear of the busy flag
 *
 * \return true if success, otherwise false
 */
static bool sd_mmc_cmd13(void) {
    uint32_t nec_timeout;

    /* Wait for data ready status.
     * Nec timing: 0 to unlimited
     * However a timeout is used.
     * 200 000 * 8 cycles
     */
    nec_timeout = 200000;
    do {
        if (!driver_send_cmd(sd_mmc_hal, SDMMC_MCI_CMD13_SEND_STATUS, (uint32_t)sd_mmc_card->rca << 16)) {
            return false;
        }
        /* Check busy flag */
        if (driver_get_response(sd_mmc_hal) & CARD_STATUS_READY_FOR_DATA) {
            break;
        }
        if (nec_timeout-- == 0) {
            return false;
        }
    } while (1);

    return true;
}

#if (CONF_SDIO_SUPPORT == 1)
/**
 * \brief CMD52 - SDIO IO_RW_DIRECT command
 *
 * \param rw_flag   Direction, 1:write, 0:read.
 * \param func_nb   Number of the function.
 * \param rd_after_wr Read after Write flag.
 * \param reg_addr  register address.
 * \param io_data   Pointer to input argument and response buffer.
 *
 * \return true if success, otherwise false
 */
static bool sdio_cmd52(uint8_t rw_flag, uint8_t func_nb, uint32_t reg_addr, uint8_t rd_after_wr, uint8_t *io_data) {
    ASSERT(io_data != NULL);
    if (!driver_send_cmd(sd_mmc_hal,
        SDIO_CMD52_IO_RW_DIRECT,
        ((uint32_t)*io_data << SDIO_CMD52_WR_DATA) | ((uint32_t)rw_flag << SDIO_CMD52_RW_FLAG)
        | ((uint32_t)func_nb << SDIO_CMD52_FUNCTION_NUM)
        | ((uint32_t)rd_after_wr << SDIO_CMD52_RAW_FLAG)
        | ((uint32_t)reg_addr << SDIO_CMD52_REG_ADRR))) {
        return false;
    }
    *io_data = driver_get_response(sd_mmc_hal) & 0xFF;
    return true;
}

/**
 * \brief CMD53 - SDIO IO_RW_EXTENDED command
 * This implementation support only the SDIO multi-byte transfer mode which is
 * similar to the single block transfer on memory.
 * Note: The SDIO block transfer mode is optional for SDIO card.
 *
 * \param rw_flag   Direction, 1:write, 0:read.
 * \param func_nb   Number of the function.
 * \param reg_addr  Register address.
 * \param inc_addr  1:Incrementing address, 0: fixed.
 * \param size      Transfer data size.
 * \param access_block  true, if the block access (DMA) is used
 *
 * \return true if success, otherwise false
 */
static bool sdio_cmd53(uint8_t rw_flag, uint8_t func_nb, uint32_t reg_addr, uint8_t inc_addr, uint32_t size,
    bool access_block) {
    ASSERT(size != 0);
    ASSERT(size <= 512);

    return driver_adtc_start(
        sd_mmc_hal,
        (rw_flag == SDIO_CMD53_READ_FLAG) ? SDIO_CMD53_IO_R_BYTE_EXTENDED : SDIO_CMD53_IO_W_BYTE_EXTENDED,
        ((size % 512) << SDIO_CMD53_COUNT) | ((uint32_t)reg_addr << SDIO_CMD53_REG_ADDR)
        | ((uint32_t)inc_addr << SDIO_CMD53_OP_CODE) | ((uint32_t)0 << SDIO_CMD53_BLOCK_MODE)
        | ((uint32_t)func_nb << SDIO_CMD53_FUNCTION_NUM) | ((uint32_t)rw_flag << SDIO_CMD53_RW_FLAG),
        size,
        1,
        access_block);
}
#endif

/**
 * \brief ACMD6 - Define the data bus width to 4 bits bus
 *
 * \return true if success, otherwise false
 */
static bool sd_acmd6(void) {
    /* CMD55 - Indicate to the card that the next command is an
     * application specific command rather than a standard command.*/
    if (!driver_send_cmd(sd_mmc_hal, SDMMC_CMD55_APP_CMD, (uint32_t)sd_mmc_card->rca << 16)) {
        return false;
    }
    /* 10b = 4 bits bus */
    if (!driver_send_cmd(sd_mmc_hal, SD_ACMD6_SET_BUS_WIDTH, 0x2)) {
        return false;
    }
    sd_mmc_card->bus_width = 4;
    return true;
}

/**
 * \brief ACMD51 - Read the SD Configuration Register.
 *
 * \note
 * SD Card Configuration Register (SCR) provides information on the SD Memory
 * Card's special features that were configured into the given card. The size
 * of SCR register is 64 bits.
 *
 *
 * \return true if success, otherwise false
 */
static bool sd_acmd51(void) {
    uint8_t scr[SD_SCR_REG_BSIZE];

    /* CMD55 - Indicate to the card that the next command is an
     * application specific command rather than a standard command.*/
    if (!driver_send_cmd(sd_mmc_hal, SDMMC_CMD55_APP_CMD, (uint32_t)sd_mmc_card->rca << 16)) {
        return false;
    }
    if (!driver_adtc_start(sd_mmc_hal, SD_ACMD51_SEND_SCR, 0, SD_SCR_REG_BSIZE, 1, true)) {
        return false;
    }
    if (!driver_start_read_blocks(sd_mmc_hal, scr, 1)) {
        return false;
    }
    if (!driver_wait_end_of_read_blocks(sd_mmc_hal)) {
        return false;
    }

    /* Get SD Memory Card - Spec. Version */
    switch (SD_SCR_SD_SPEC(scr)) {
        case SD_SCR_SD_SPEC_1_0_01:
            sd_mmc_card->version = CARD_VER_SD_1_0;
            break;

        case SD_SCR_SD_SPEC_1_10:
            sd_mmc_card->version = CARD_VER_SD_1_10;
            break;

        case SD_SCR_SD_SPEC_2_00:
            if (SD_SCR_SD_SPEC3(scr) == SD_SCR_SD_SPEC_3_00) {
                sd_mmc_card->version = CARD_VER_SD_3_0;
            } else {
                sd_mmc_card->version = CARD_VER_SD_2_0;
            }
            break;

        default:
            sd_mmc_card->version = CARD_VER_SD_1_0;
            break;
    }
    return true;
}

/**
 * \brief Select a card slot and initialize the associated driver
 *
 * \param slot  Card slot number
 *
 * \retval SD_MMC_ERR_SLOT     Wrong slot number
 * \retval SD_MMC_ERR_NO_CARD  No card present on slot
 * \retval SD_MMC_ERR_UNUSABLE Unusable card
 * \retval SD_MMC_INIT_ONGOING Card initialization requested
 * \retval SD_MMC_OK           Card present
 */
static sd_mmc_err_t sd_mmc_select_slot(uint8_t slot) {
    if (slot >= CONF_SD_MMC_MEM_CNT) {
        return SD_MMC_ERR_SLOT;
    }

    if (_cd && _cd[slot].pin != -1) {
        /** Card Detect pins */
        if (gpio_get_pin_level(_cd[slot].pin) != _cd[slot].val) {
            if (sd_mmc_cards[slot].state == SD_MMC_CARD_STATE_DEBOUNCE) {
                SD_MMC_STOP_TIMEOUT();
            }
            sd_mmc_cards[slot].state = SD_MMC_CARD_STATE_NO_CARD;
            return SD_MMC_ERR_NO_CARD;
        }
        if (sd_mmc_cards[slot].state == SD_MMC_CARD_STATE_NO_CARD) {
            /* A card plug on going, but this is not initialized */
            sd_mmc_cards[slot].state = SD_MMC_CARD_STATE_DEBOUNCE;
            /* Debounce + Power On Setup */
            SD_MMC_START_TIMEOUT();
            return SD_MMC_ERR_NO_CARD;
        }
        if (sd_mmc_cards[slot].state == SD_MMC_CARD_STATE_DEBOUNCE) {
            if (!SD_MMC_IS_TIMEOUT()) {
                /* Debounce on going */
                return SD_MMC_ERR_NO_CARD;
            }
            /* Card is not initialized */
            sd_mmc_cards[slot].state = SD_MMC_CARD_STATE_INIT;
            /* Set 1-bit bus width and low clock for initialization */
            sd_mmc_cards[slot].clock = SDMMC_CLOCK_INIT;
            sd_mmc_cards[slot].bus_width = 1;
            sd_mmc_cards[slot].high_speed = 0;
        }
        if (sd_mmc_cards[slot].state == SD_MMC_CARD_STATE_UNUSABLE) {
            return SD_MMC_ERR_UNUSABLE;
        }
    } else {
        /* No pin card detection, then always try to install it */
        if ((sd_mmc_cards[slot].state == SD_MMC_CARD_STATE_NO_CARD)
            || (sd_mmc_cards[slot].state == SD_MMC_CARD_STATE_UNUSABLE)) {
            /* Card is not initialized */
            sd_mmc_cards[slot].state = SD_MMC_CARD_STATE_INIT;
            /* Set 1-bit bus width and low clock for initialization */
            sd_mmc_cards[slot].clock = SDMMC_CLOCK_INIT;
            sd_mmc_cards[slot].bus_width = 1;
            sd_mmc_cards[slot].high_speed = 0;
        }
    }

    ASSERT(!(sd_mmc_slot_sel != slot && sd_mmc_nb_block_remaining != 0));

    /* Initialize interface */
    sd_mmc_slot_sel = slot;
    sd_mmc_card = &sd_mmc_cards[slot];
    sd_mmc_configure_slot();
    return (sd_mmc_cards[slot].state == SD_MMC_CARD_STATE_INIT) ? SD_MMC_INIT_ONGOING : SD_MMC_OK;
}

/**
 * \brief Configures the driver with the selected card configuration
 */
static void sd_mmc_configure_slot(void) {
    driver_select_device(
        sd_mmc_hal, sd_mmc_slot_sel, sd_mmc_card->clock, sd_mmc_card->bus_width, sd_mmc_card->high_speed);
}

/**
 * \brief Deselect the current card slot
 */
static void sd_mmc_deselect_slot(void) {
    if (sd_mmc_slot_sel < CONF_SD_MMC_MEM_CNT) {
        driver_deselect_device(sd_mmc_hal, sd_mmc_slot_sel);
    }
}

/**
 * \brief Initialize the SD card in MCI mode.
 *
 * \note
 * This function runs the initialization procedure and the identification
 * process, then it sets the SD/MMC card in transfer state.
 * At last, it will automaticly enable maximum bus width and transfer speed.
 *
 * \return true if success, otherwise false
 */
static bool sd_mmc_mci_card_init(void) {
    uint8_t v2 = 0;
    #if (CONF_SDIO_SUPPORT == 1)
    uint8_t data = 0x08;
    #endif

    /* In first, try to install SD/SDIO card */
    sd_mmc_card->type = CARD_TYPE_SD;
    sd_mmc_card->version = CARD_VER_UNKNOWN;
    sd_mmc_card->rca = 0;

    /* Card need of 74 cycles clock minimum to start */
    driver_send_clock(sd_mmc_hal);

    #if (CONF_SDIO_SUPPORT == 1)
    /* CMD52 Reset SDIO */
    sdio_cmd52(SDIO_CMD52_WRITE_FLAG, SDIO_CIA, SDIO_CCCR_IOA, 0, &data);
    #endif

    /* CMD0 - Reset all cards to idle state.*/
    if (!driver_send_cmd(sd_mmc_hal, SDMMC_MCI_CMD0_GO_IDLE_STATE, 0)) {
        return false;
    }
    if (!sd_cmd8(&v2)) {
        return false;
    }
    /* Try to get the SDIO card's operating condition */
    if (!sdio_op_cond()) {
        return false;
    }

    if (sd_mmc_card->type & CARD_TYPE_SD) {
        /* Try to get the SD card's operating condition */
        if (!sd_mci_op_cond(v2)) {
            /* It is not a SD card */
            #if CONF_MMC_SUPPORT
            sd_mmc_card->type = CARD_TYPE_MMC;
            return sd_mmc_mci_install_mmc();
            #else
            sd_mmc_card->type = CARD_TYPE_UNKNOWN;
            return false;
            #endif
        }
    }

    if (sd_mmc_card->type & CARD_TYPE_SD) {
        /* SD MEMORY, Put the Card in Identify Mode
         * Note: The CID is not used in this stack */
        if (!driver_send_cmd(sd_mmc_hal, SDMMC_CMD2_ALL_SEND_CID, 0)) {
            return false;
        }
    }
    /* Ask the card to publish a new relative address (RCA).*/
    if (!driver_send_cmd(sd_mmc_hal, SD_CMD3_SEND_RELATIVE_ADDR, 0)) {
        return false;
    }
    sd_mmc_card->rca = (driver_get_response(sd_mmc_hal) >> 16) & 0xFFFF;

    /* SD MEMORY, Get the Card-Specific Data */
    if (sd_mmc_card->type & CARD_TYPE_SD) {
        if (!sd_mmc_cmd9_mci()) {
            return false;
        }
        sd_decode_csd();
    }
    /* Select the and put it into Transfer Mode */
    if (!driver_send_cmd(sd_mmc_hal, SDMMC_CMD7_SELECT_CARD_CMD, (uint32_t)sd_mmc_card->rca << 16)) {
        return false;
    }
    /* SD MEMORY, Read the SCR to get card version */
    if (sd_mmc_card->type & CARD_TYPE_SD) {
        if (!sd_acmd51()) {
            return false;
        }
    }
    if (IS_SDIO()) {
        if (!sdio_get_max_speed()) {
            return false;
        }
    }
    if ((4 <= driver_get_bus_width(sd_mmc_hal, sd_mmc_slot_sel))) {
        /* TRY to enable 4-bit mode */
        if (IS_SDIO()) {
            if (!sdio_cmd52_set_bus_width()) {
                return false;
            }
        }
        if (sd_mmc_card->type & CARD_TYPE_SD) {
            if (!sd_acmd6()) {
                return false;
            }
        }
        /* Switch to selected bus mode */
        sd_mmc_configure_slot();
    }
    if (driver_is_high_speed_capable(sd_mmc_hal)) {
        /* TRY to enable High-Speed Mode */
        if (IS_SDIO()) {
            if (!sdio_cmd52_set_high_speed()) {
                return false;
            }
        }
        if (sd_mmc_card->type & CARD_TYPE_SD) {
            if (sd_mmc_card->version > CARD_VER_SD_1_0) {
                if (!sd_cm6_set_high_speed()) {
                    return false;
                }
            }
        }
        /* Valid new configuration */
        sd_mmc_configure_slot();
    }
    /* SD MEMORY, Set default block size */
    if (sd_mmc_card->type & CARD_TYPE_SD) {
        if (!driver_send_cmd(sd_mmc_hal, SDMMC_CMD16_SET_BLOCKLEN, SD_MMC_BLOCK_SIZE)) {
            return false;
        }
    }
    return true;
}

#if CONF_MMC_SUPPORT
/**
 * \brief Initialize the MMC card in MCI mode.
 *
 * \note
 * This function runs the initialization procedure and the identification
 * process, then it sets the SD/MMC card in transfer state.
 * At last, it will automaticly enable maximum bus width and transfer speed.
 *
 * \return true if success, otherwise false
 */
static bool sd_mmc_mci_install_mmc(void) {
    uint8_t b_authorize_high_speed;

    /* CMD0 - Reset all cards to idle state. */
    if (!driver_send_cmd(sd_mmc_hal, SDMMC_MCI_CMD0_GO_IDLE_STATE, 0)) {
        return false;
    }

    if (!mmc_mci_op_cond()) {
        return false;
    }

    /* Put the Card in Identify Mode
     * Note: The CID is not used in this stack*/
    if (!driver_send_cmd(sd_mmc_hal, SDMMC_CMD2_ALL_SEND_CID, 0)) {
        return false;
    }
    /* Assign relative address to the card.*/
    sd_mmc_card->rca = 1;
    if (!driver_send_cmd(sd_mmc_hal, MMC_CMD3_SET_RELATIVE_ADDR, (uint32_t)sd_mmc_card->rca << 16)) {
        return false;
    }
    /* Get the Card-Specific Data */
    if (!sd_mmc_cmd9_mci()) {
        return false;
    }
    mmc_decode_csd();
    /* Select the and put it into Transfer Mode */
    if (!driver_send_cmd(sd_mmc_hal, SDMMC_CMD7_SELECT_CARD_CMD, (uint32_t)sd_mmc_card->rca << 16)) {
        return false;
    }
    if (sd_mmc_card->version >= CARD_VER_MMC_4) {
        /* For MMC 4.0 Higher version
         * Get EXT_CSD */
        if (!mmc_cmd8(&b_authorize_high_speed)) {
            return false;
        }
        if (4 <= driver_get_bus_width(sd_mmc_hal, sd_mmc_slot_sel)) {
            /* Enable more bus width */
            if (!mmc_cmd6_set_bus_width(driver_get_bus_width(sd_mmc_hal, sd_mmc_slot_sel))) {
                return false;
            }
            /* Reinitialize the slot with the bus width */
            sd_mmc_configure_slot();
        }
        if (driver_is_high_speed_capable(sd_mmc_hal) && b_authorize_high_speed) {
            /* Enable HS */
            if (!mmc_cmd6_set_high_speed()) {
                return false;
            }
            /* Reinitialize the slot with the new speed */
            sd_mmc_configure_slot();
        }
    } else {
        /* Reinitialize the slot with the new speed */
        sd_mmc_configure_slot();
    }

    uint8_t retry = 10;
    while (retry--) {
        /* Retry is a WORKAROUND for no compliance card (Atmel Internal ref. MMC19):
         * These cards seem not ready immediatly
         * after the end of busy of mmc_cmd6_set_high_speed()*/

        /* Set default block size */
        if (driver_send_cmd(sd_mmc_hal, SDMMC_CMD16_SET_BLOCKLEN, SD_MMC_BLOCK_SIZE)) {
            return true;
        }
    }
    return false;
}
#endif

/*--------------------- PUBLIC FUNCTIONS ----------------------------*/

void sd_mmc_init(void *hal, sd_mmc_detect_t *card_detects, sd_mmc_detect_t *wp_detects) {
    /* GPIO will be used to detect card and write protect.
     * The related clocks and pinmux must be configurated in good
     * condition. */

    for (uint8_t slot = 0; slot < CONF_SD_MMC_MEM_CNT; slot++) {
        sd_mmc_cards[slot].state = SD_MMC_CARD_STATE_NO_CARD;
    }
    sd_mmc_slot_sel = 0xFF;     /* No slot configurated */
    sd_mmc_hal = hal;
    _cd = card_detects;
    _wp = wp_detects;
}

uint8_t sd_mmc_nb_slot(void) {
    return CONF_SD_MMC_MEM_CNT;
}

sd_mmc_err_t sd_mmc_check(uint8_t slot) {
    sd_mmc_err_t sd_mmc_err;

    sd_mmc_err = sd_mmc_select_slot(slot);
    if (sd_mmc_err != SD_MMC_INIT_ONGOING) {
        sd_mmc_deselect_slot();
        return sd_mmc_err;
    }

    /* Initialization of the card requested */
    if (sd_mmc_mci_card_init()) {
        sd_mmc_card->state = SD_MMC_CARD_STATE_READY;
        sd_mmc_deselect_slot();
        /* To notify that the card has been just initialized
         * It is necessary for USB Device MSC */
        return SD_MMC_INIT_ONGOING;
    }
    sd_mmc_card->state = SD_MMC_CARD_STATE_UNUSABLE;
    sd_mmc_deselect_slot();
    return SD_MMC_ERR_UNUSABLE;
}

card_type_t sd_mmc_get_type(uint8_t slot) {
    if (SD_MMC_OK != sd_mmc_select_slot(slot)) {
        return CARD_TYPE_UNKNOWN;
    }
    sd_mmc_deselect_slot();
    return sd_mmc_card->type;
}

card_version_t sd_mmc_get_version(uint8_t slot) {
    if (SD_MMC_OK != sd_mmc_select_slot(slot)) {
        return CARD_VER_UNKNOWN;
    }
    sd_mmc_deselect_slot();
    return sd_mmc_card->version;
}

uint32_t sd_mmc_get_capacity(uint8_t slot) {
    if (SD_MMC_OK != sd_mmc_select_slot(slot)) {
        return 0;
    }
    sd_mmc_deselect_slot();
    return sd_mmc_card->capacity;
}

bool sd_mmc_is_write_protected(uint8_t slot) {
    /* No detection, always writable */
    if (!_wp || _wp[slot].pin == -1) {
        return false;
    }
    /* Write Protect Detect */
    if (gpio_get_pin_level(_wp[slot].pin) == _wp[slot].val) {
        return true;
    }
    return false;
}

sd_mmc_err_t sd_mmc_init_read_blocks(uint8_t slot, uint32_t start, uint16_t nb_block) {
    sd_mmc_err_t sd_mmc_err;
    uint32_t cmd, arg, resp;

    sd_mmc_err = sd_mmc_select_slot(slot);
    if (sd_mmc_err != SD_MMC_OK) {
        return sd_mmc_err;
    }

    /* Wait for data ready status */
    if (!sd_mmc_cmd13()) {
        sd_mmc_deselect_slot();
        return SD_MMC_ERR_COMM;
    }

    if (nb_block > 1) {
        cmd = SDMMC_CMD18_READ_MULTIPLE_BLOCK;
    } else {
        cmd = SDMMC_CMD17_READ_SINGLE_BLOCK;
    }
    /*
     * SDSC Card (CCS=0) uses byte unit address,
     * SDHC and SDXC Cards (CCS=1) use block unit address (512 Bytes unit).
     */
    if (sd_mmc_card->type & CARD_TYPE_HC) {
        arg = start;
    } else {
        arg = (start * SD_MMC_BLOCK_SIZE);
    }

    if (!driver_adtc_start(sd_mmc_hal, cmd, arg, SD_MMC_BLOCK_SIZE, nb_block, true)) {
        sd_mmc_deselect_slot();
        return SD_MMC_ERR_COMM;
    }
    /* Check response */
    resp = driver_get_response(sd_mmc_hal);
    if (resp & CARD_STATUS_ERR_RD_WR) {
        sd_mmc_deselect_slot();
        return SD_MMC_ERR_COMM;
    }

    sd_mmc_nb_block_remaining = nb_block;
    sd_mmc_nb_block_to_tranfer = nb_block;
    return SD_MMC_OK;
}

sd_mmc_err_t sd_mmc_start_read_blocks(void *dest, uint16_t nb_block) {
    ASSERT(sd_mmc_nb_block_remaining >= nb_block);

    if (!driver_start_read_blocks(sd_mmc_hal, dest, nb_block)) {
        sd_mmc_nb_block_remaining = 0;
        return SD_MMC_ERR_COMM;
    }
    sd_mmc_nb_block_remaining -= nb_block;
    return SD_MMC_OK;
}

sd_mmc_err_t sd_mmc_wait_end_of_read_blocks(bool abort) {
    if (!driver_wait_end_of_read_blocks(sd_mmc_hal)) {
        return SD_MMC_ERR_COMM;
    }
    if (abort) {
        sd_mmc_nb_block_remaining = 0;
    } else if (sd_mmc_nb_block_remaining) {
        return SD_MMC_OK;
    }

    /* All blocks are transfered then stop read operation */
    if (sd_mmc_nb_block_to_tranfer == 1) {
        /* Single block transfer, then nothing to do */
        sd_mmc_deselect_slot();
        return SD_MMC_OK;
    }
    /* WORKAROUND for no compliance card (Atmel Internal ref. !MMC7 !SD19):
     * The errors on this command must be ignored
     * and one retry can be necessary in SPI mode for no compliance card.*/
    if (!driver_adtc_stop(sd_mmc_hal, SDMMC_CMD12_STOP_TRANSMISSION, 0)) {
        driver_adtc_stop(sd_mmc_hal, SDMMC_CMD12_STOP_TRANSMISSION, 0);
    }
    sd_mmc_deselect_slot();
    return SD_MMC_OK;
}

sd_mmc_err_t sd_mmc_init_write_blocks(uint8_t slot, uint32_t start, uint16_t nb_block) {
    sd_mmc_err_t sd_mmc_err;
    uint32_t cmd, arg, resp;

    sd_mmc_err = sd_mmc_select_slot(slot);
    if (sd_mmc_err != SD_MMC_OK) {
        return sd_mmc_err;
    }
    if (sd_mmc_is_write_protected(slot)) {
        sd_mmc_deselect_slot();
        return SD_MMC_ERR_WP;
    }

    if (nb_block > 1) {
        cmd = SDMMC_CMD25_WRITE_MULTIPLE_BLOCK;
    } else {
        cmd = SDMMC_CMD24_WRITE_BLOCK;
    }
    /*
     * SDSC Card (CCS=0) uses byte unit address,
     * SDHC and SDXC Cards (CCS=1) use block unit address (512 Bytes unit).
     */
    if (sd_mmc_card->type & CARD_TYPE_HC) {
        arg = start;
    } else {
        arg = (start * SD_MMC_BLOCK_SIZE);
    }
    if (!driver_adtc_start(sd_mmc_hal, cmd, arg, SD_MMC_BLOCK_SIZE, nb_block, true)) {
        sd_mmc_deselect_slot();
        return SD_MMC_ERR_COMM;
    }
    /* Check response */
    resp = driver_get_response(sd_mmc_hal);
    if (resp & CARD_STATUS_ERR_RD_WR) {
        sd_mmc_deselect_slot();
        return SD_MMC_ERR_COMM;
    }

    sd_mmc_nb_block_remaining = nb_block;
    sd_mmc_nb_block_to_tranfer = nb_block;
    return SD_MMC_OK;
}

sd_mmc_err_t sd_mmc_start_write_blocks(const void *src, uint16_t nb_block) {
    ASSERT(sd_mmc_nb_block_remaining >= nb_block);
    if (!driver_start_write_blocks(sd_mmc_hal, src, nb_block)) {
        sd_mmc_nb_block_remaining = 0;
        return SD_MMC_ERR_COMM;
    }
    sd_mmc_nb_block_remaining -= nb_block;
    return SD_MMC_OK;
}

sd_mmc_err_t sd_mmc_wait_end_of_write_blocks(bool abort) {
    if (!driver_wait_end_of_write_blocks(sd_mmc_hal)) {
        return SD_MMC_ERR_COMM;
    }
    if (abort) {
        sd_mmc_nb_block_remaining = 0;
    } else if (sd_mmc_nb_block_remaining) {
        return SD_MMC_OK;
    }

    /* All blocks are transfered then stop write operation */
    if (sd_mmc_nb_block_to_tranfer == 1) {
        /* Single block transfer, then nothing to do */
        sd_mmc_deselect_slot();
        return SD_MMC_OK;
    }

    /* Note: SPI multiblock writes terminate using a special
     * token, not a STOP_TRANSMISSION request.*/
    if (!driver_adtc_stop(sd_mmc_hal, SDMMC_CMD12_STOP_TRANSMISSION, 0)) {
        sd_mmc_deselect_slot();
        return SD_MMC_ERR_COMM;
    }

    sd_mmc_deselect_slot();
    return SD_MMC_OK;
}

#if (CONF_SDIO_SUPPORT == 1)
sd_mmc_err_t sdio_read_direct(uint8_t slot, uint8_t func_num, uint32_t addr, uint8_t *dest) {
    sd_mmc_err_t sd_mmc_err;

    if (dest == NULL) {
        return SD_MMC_ERR_PARAM;
    }

    sd_mmc_err = sd_mmc_select_slot(slot);
    if (sd_mmc_err != SD_MMC_OK) {
        return sd_mmc_err;
    }

    if (!sdio_cmd52(SDIO_CMD52_READ_FLAG, func_num, addr, 0, dest)) {
        sd_mmc_deselect_slot();
        return SD_MMC_ERR_COMM;
    }
    sd_mmc_deselect_slot();
    return SD_MMC_OK;
}

sd_mmc_err_t sdio_write_direct(uint8_t slot, uint8_t func_num, uint32_t addr, uint8_t data) {
    sd_mmc_err_t sd_mmc_err;

    sd_mmc_err = sd_mmc_select_slot(slot);
    if (sd_mmc_err != SD_MMC_OK) {
        return sd_mmc_err;
    }

    if (!sdio_cmd52(SDIO_CMD52_WRITE_FLAG, func_num, addr, 0, &data)) {
        sd_mmc_deselect_slot();
        return SD_MMC_ERR_COMM;
    }

    sd_mmc_deselect_slot();
    return SD_MMC_OK;
}

sd_mmc_err_t sdio_read_extended(uint8_t slot, uint8_t func_num, uint32_t addr, uint8_t inc_addr, uint8_t *dest,
    uint16_t size) {
    sd_mmc_err_t sd_mmc_err;

    if ((size == 0) || (size > 512)) {
        return SD_MMC_ERR_PARAM;
    }

    sd_mmc_err = sd_mmc_select_slot(slot);
    if (sd_mmc_err != SD_MMC_OK) {
        return sd_mmc_err;
    }

    if (!sdio_cmd53(SDIO_CMD53_READ_FLAG, func_num, addr, inc_addr, size, true)) {
        sd_mmc_deselect_slot();
        return SD_MMC_ERR_COMM;
    }
    if (!driver_start_read_blocks(sd_mmc_hal, dest, 1)) {
        sd_mmc_deselect_slot();
        return SD_MMC_ERR_COMM;
    }
    if (!driver_wait_end_of_read_blocks(sd_mmc_hal)) {
        sd_mmc_deselect_slot();
        return SD_MMC_ERR_COMM;
    }

    sd_mmc_deselect_slot();
    return SD_MMC_OK;
}

sd_mmc_err_t sdio_write_extended(uint8_t slot, uint8_t func_num, uint32_t addr, uint8_t inc_addr, uint8_t *src,
    uint16_t size) {
    sd_mmc_err_t sd_mmc_err;

    if ((size == 0) || (size > 512)) {
        return SD_MMC_ERR_PARAM;
    }

    sd_mmc_err = sd_mmc_select_slot(slot);
    if (sd_mmc_err != SD_MMC_OK) {
        return sd_mmc_err;
    }

    if (!sdio_cmd53(SDIO_CMD53_WRITE_FLAG, func_num, addr, inc_addr, size, true)) {
        sd_mmc_deselect_slot();
        return SD_MMC_ERR_COMM;
    }
    if (!driver_start_write_blocks(sd_mmc_hal, src, 1)) {
        sd_mmc_deselect_slot();
        return SD_MMC_ERR_COMM;
    }
    if (!driver_wait_end_of_write_blocks(sd_mmc_hal)) {
        sd_mmc_deselect_slot();
        return SD_MMC_ERR_COMM;
    }

    sd_mmc_deselect_slot();
    return SD_MMC_OK;
}
#endif

/** @} */

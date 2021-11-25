/*
 * Copyright (c) 2006 Uwe Stuehler <uwe@openbsd.org>
 * Adaptations to ESP-IDF Copyright (c) 2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _SDMMC_TYPES_H_
#define _SDMMC_TYPES_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/**
 * Decoded values from SD card Card Specific Data register
 */
typedef struct {
    int csd_ver;                /*!< CSD structure format */
    int mmc_ver;                /*!< MMC version (for CID format) */
    int capacity;               /*!< total number of sectors */
    int sector_size;            /*!< sector size in bytes */
    int read_block_len;         /*!< block length for reads */
    int card_command_class;     /*!< Card Command Class for SD */
    int tr_speed;               /*!< Max transfer speed */
} sdmmc_csd_t;

/**
 * Decoded values from SD card Card IDentification register
 */
typedef struct {
    int mfg_id;     /*!< manufacturer identification number */
    int oem_id;     /*!< OEM/product identification number */
    char name[8];   /*!< product name (MMC v1 has the longest) */
    int revision;   /*!< product revision */
    int serial;     /*!< product serial number */
    int date;       /*!< manufacturing date */
} sdmmc_cid_t;

/**
 * Decoded values from SD Configuration Register
 */
typedef struct {
    int sd_spec;    /*!< SD Physical layer specification version, reported by card */
    int bus_width;  /*!< bus widths supported by card: BIT(0) — 1-bit bus, BIT(2) — 4-bit bus */
} sdmmc_scr_t;

/**
 * Decoded values of Extended Card Specific Data
 */
typedef struct {
    uint8_t power_class;    /*!< Power class used by the card */
} sdmmc_ext_csd_t;

/**
 * SD/MMC command response buffer
 */
typedef uint32_t sdmmc_response_t[4];

/**
 * SD SWITCH_FUNC response buffer
 */
typedef struct {
    uint32_t data[512 / 8 / sizeof(uint32_t)];  /*!< response data */
} sdmmc_switch_func_rsp_t;

typedef enum {
    SDMMC_OK = 0,
    SDMMC_ERR_NOT_SUPPORTED = 1,
    SDMMC_ERR_INVALID_RESPONSE = 2,
    SDMMC_ERR_TIMEOUT = 3,
    SDMMC_ERR_NO_MEM = 4,
    SDMMC_ERR_INVALID_SIZE = 5,
    SDMMC_ERR_NO_CARD = 6,
    SDMMC_ERR_INVALID_ARG = 7,
    SDMMC_ERR_BUSY = 8,
} sdmmc_err_t;

/**
 * SD/MMC command information
 */
typedef struct {
        uint32_t opcode;            /*!< SD or MMC command index */
        uint32_t arg;               /*!< SD/MMC command argument */
        sdmmc_response_t response;  /*!< response buffer */
        void* data;                 /*!< buffer to send or read into */
        size_t datalen;             /*!< length of data buffer */
        size_t blklen;              /*!< block length */
        int flags;                  /*!< see below */
/** @cond */
#define SCF_ITSDONE      0x0001     /*!< command is complete */
#define SCF_CMD(flags)   ((flags) & 0x00f0)
#define SCF_CMD_AC       0x0000
#define SCF_CMD_ADTC     0x0010
#define SCF_CMD_BC       0x0020
#define SCF_CMD_BCR      0x0030
#define SCF_CMD_READ     0x0040     /*!< read command (data expected) */
#define SCF_RSP_BSY      0x0100
#define SCF_RSP_136      0x0200
#define SCF_RSP_CRC      0x0400
#define SCF_RSP_IDX      0x0800
#define SCF_RSP_PRESENT  0x1000
/* response types */
#define SCF_RSP_R0       0 /*!< none */
#define SCF_RSP_R1       (SCF_RSP_PRESENT|SCF_RSP_CRC|SCF_RSP_IDX)
#define SCF_RSP_R1B      (SCF_RSP_PRESENT|SCF_RSP_CRC|SCF_RSP_IDX|SCF_RSP_BSY)
#define SCF_RSP_R2       (SCF_RSP_PRESENT|SCF_RSP_CRC|SCF_RSP_136)
#define SCF_RSP_R3       (SCF_RSP_PRESENT)
#define SCF_RSP_R4       (SCF_RSP_PRESENT)
#define SCF_RSP_R5       (SCF_RSP_PRESENT|SCF_RSP_CRC|SCF_RSP_IDX)
#define SCF_RSP_R5B      (SCF_RSP_PRESENT|SCF_RSP_CRC|SCF_RSP_IDX|SCF_RSP_BSY)
#define SCF_RSP_R6       (SCF_RSP_PRESENT|SCF_RSP_CRC|SCF_RSP_IDX)
#define SCF_RSP_R7       (SCF_RSP_PRESENT|SCF_RSP_CRC|SCF_RSP_IDX)
/* special flags */
#define SCF_WAIT_BUSY    0x2000     /*!< Wait for completion of card busy signal before returning */
#define SCF_AUTO_STOP    0x4000     /*!< Auto stop with command 12 */
/** @endcond */
        sdmmc_err_t error;            /*!< error returned from transfer */
        int timeout_ms;             /*!< response timeout, in milliseconds */
} sdmmc_command_t;

/**
 * SD/MMC Host description
 *
 * This structure defines properties of SD/MMC host and functions
 * of SD/MMC host which can be used by upper layers.
 */
#define BIT(x) (1 << x)

typedef struct {
    uint32_t flags;             /*!< flags defining host properties */
#define SDMMC_HOST_FLAG_1BIT    BIT(0)      /*!< host supports 1-line SD and MMC protocol */
#define SDMMC_HOST_FLAG_4BIT    BIT(1)      /*!< host supports 4-line SD and MMC protocol */
#define SDMMC_HOST_FLAG_8BIT    BIT(2)      /*!< host supports 8-line MMC protocol */
#define SDMMC_HOST_FLAG_SPI     BIT(3)      /*!< host supports SPI protocol */
#define SDMMC_HOST_FLAG_DDR     BIT(4)      /*!< host supports DDR mode for SD/MMC */
#define SDMMC_HOST_FLAG_DEINIT_ARG BIT(5)      /*!< host `deinit` function called with the slot argument */
    int slot;                   /*!< slot number, to be passed to host functions */
    int max_freq_khz;           /*!< max frequency supported by the host */
#define SDMMC_FREQ_DEFAULT      20000       /*!< SD/MMC Default speed (limited by clock divider) */
#define SDMMC_FREQ_HIGHSPEED    40000       /*!< SD High speed (limited by clock divider) */
#define SDMMC_FREQ_PROBING      400         /*!< SD/MMC probing speed */
#define SDMMC_FREQ_52M          52000       /*!< MMC 52MHz speed */
#define SDMMC_FREQ_26M          26000       /*!< MMC 26MHz speed */
    float io_voltage;           /*!< I/O voltage used by the controller (voltage switching is not supported) */
    sdmmc_err_t (*init)(void);    /*!< Host function to initialize the driver */
    sdmmc_err_t (*set_bus_width)(int slot, size_t width);    /*!< host function to set bus width */
    size_t (*get_bus_width)(int slot); /*!< host function to get bus width */
    sdmmc_err_t (*set_bus_ddr_mode)(int slot, bool ddr_enable); /*!< host function to set DDR mode */
    sdmmc_err_t (*set_card_clk)(int slot, uint32_t freq_khz); /*!< host function to set card clock frequency */
    sdmmc_err_t (*do_transaction)(int slot, sdmmc_command_t* cmdinfo);    /*!< host function to do a transaction */
    union {
        sdmmc_err_t (*deinit)(void);  /*!< host function to deinitialize the driver */
        sdmmc_err_t (*deinit_p)(int slot);  /*!< host function to deinitialize the driver, called with the `slot` */
    };
    sdmmc_err_t (*io_int_enable)(int slot); /*!< Host function to enable SDIO interrupt line */
    sdmmc_err_t (*io_int_wait)(int slot, int timeout_ms); /*!< Host function to wait for SDIO interrupt line to be active */
    int command_timeout_ms;     /*!< timeout, in milliseconds, of a single command. Set to 0 to use the default value. */
} sdmmc_host_t;

/**
 * SD/MMC card information structure
 */
typedef struct {
    sdmmc_host_t host;          /*!< Host with which the card is associated */
    uint32_t ocr;               /*!< OCR (Operation Conditions Register) value */
    union {
        sdmmc_cid_t cid;            /*!< decoded CID (Card IDentification) register value */
        sdmmc_response_t raw_cid;   /*!< raw CID of MMC card to be decoded
                                         after the CSD is fetched in the data transfer mode*/
    };
    sdmmc_csd_t csd;            /*!< decoded CSD (Card-Specific Data) register value */
    sdmmc_scr_t scr;            /*!< decoded SCR (SD card Configuration Register) value */
    sdmmc_ext_csd_t ext_csd;    /*!< decoded EXT_CSD (Extended Card Specific Data) register value */
    uint16_t rca;               /*!< RCA (Relative Card Address) */
    uint16_t max_freq_khz;      /*!< Maximum frequency, in kHz, supported by the card */
    uint32_t is_mem : 1;        /*!< Bit indicates if the card is a memory card */
    uint32_t is_sdio : 1;       /*!< Bit indicates if the card is an IO card */
    uint32_t is_mmc : 1;        /*!< Bit indicates if the card is MMC */
    uint32_t num_io_functions : 3;  /*!< If is_sdio is 1, contains the number of IO functions on the card */
    uint32_t log_bus_width : 2; /*!< log2(bus width supported by card) */
    uint32_t is_ddr : 1;        /*!< Card supports DDR mode */
    uint32_t reserved : 23;     /*!< Reserved for future expansion */
} sdmmc_card_t;

// OS abstraction layer (copied from TinyUSB)
void osal_task_delay(uint32_t msec);

#endif // _SDMMC_TYPES_H_

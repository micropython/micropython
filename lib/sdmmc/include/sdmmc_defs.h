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

#ifndef _SDMMC_DEFS_H_
#define _SDMMC_DEFS_H_

#include <stdint.h>
#include <limits.h>

/* MMC commands */                              /* response type */
#define MMC_GO_IDLE_STATE               0       /* R0 */
#define MMC_SEND_OP_COND                1       /* R3 */
#define MMC_ALL_SEND_CID                2       /* R2 */
#define MMC_SET_RELATIVE_ADDR           3       /* R1 */
#define MMC_SWITCH                      6       /* R1B */
#define MMC_SELECT_CARD                 7       /* R1 */
#define MMC_SEND_EXT_CSD                8       /* R1 */
#define MMC_SEND_CSD                    9       /* R2 */
#define MMC_SEND_CID                    10      /* R1 */
#define MMC_READ_DAT_UNTIL_STOP         11      /* R1 */
#define MMC_STOP_TRANSMISSION           12      /* R1B */
#define MMC_SEND_STATUS                 13      /* R1 */
#define MMC_SET_BLOCKLEN                16      /* R1 */
#define MMC_READ_BLOCK_SINGLE           17      /* R1 */
#define MMC_READ_BLOCK_MULTIPLE         18      /* R1 */
#define MMC_WRITE_DAT_UNTIL_STOP        20      /* R1 */
#define MMC_SET_BLOCK_COUNT             23      /* R1 */
#define MMC_WRITE_BLOCK_SINGLE          24      /* R1 */
#define MMC_WRITE_BLOCK_MULTIPLE        25      /* R1 */
#define MMC_APP_CMD                     55      /* R1 */

/* SD commands */                               /* response type */
#define SD_SEND_RELATIVE_ADDR           3       /* R6 */
#define SD_SEND_SWITCH_FUNC             6       /* R1 */
#define SD_SEND_IF_COND                 8       /* R7 */
#define SD_READ_OCR                     58      /* R3 */
#define SD_CRC_ON_OFF                   59      /* R1 */

/* SD application commands */                   /* response type */
#define SD_APP_SET_BUS_WIDTH            6       /* R1 */
#define SD_APP_SD_STATUS                13      /* R2 */
#define SD_APP_OP_COND                  41      /* R3 */
#define SD_APP_SEND_SCR                 51      /* R1 */

/* SD IO commands */
#define SD_IO_SEND_OP_COND              5       /* R4 */
#define SD_IO_RW_DIRECT                 52      /* R5 */
#define SD_IO_RW_EXTENDED               53      /* R5 */


/* OCR bits */
#define MMC_OCR_MEM_READY               (1<<31) /* memory power-up status bit */
#define MMC_OCR_ACCESS_MODE_MASK        0x60000000 /* bits 30:29 */
#define MMC_OCR_SECTOR_MODE             (1<<30)
#define MMC_OCR_BYTE_MODE               (1<<29)
#define MMC_OCR_3_5V_3_6V               (1<<23)
#define MMC_OCR_3_4V_3_5V               (1<<22)
#define MMC_OCR_3_3V_3_4V               (1<<21)
#define MMC_OCR_3_2V_3_3V               (1<<20)
#define MMC_OCR_3_1V_3_2V               (1<<19)
#define MMC_OCR_3_0V_3_1V               (1<<18)
#define MMC_OCR_2_9V_3_0V               (1<<17)
#define MMC_OCR_2_8V_2_9V               (1<<16)
#define MMC_OCR_2_7V_2_8V               (1<<15)
#define MMC_OCR_2_6V_2_7V               (1<<14)
#define MMC_OCR_2_5V_2_6V               (1<<13)
#define MMC_OCR_2_4V_2_5V               (1<<12)
#define MMC_OCR_2_3V_2_4V               (1<<11)
#define MMC_OCR_2_2V_2_3V               (1<<10)
#define MMC_OCR_2_1V_2_2V               (1<<9)
#define MMC_OCR_2_0V_2_1V               (1<<8)
#define MMC_OCR_1_65V_1_95V             (1<<7)

#define SD_OCR_SDHC_CAP                 (1<<30)
#define SD_OCR_VOL_MASK                 0xFF8000 /* bits 23:15 */

/* SD mode R1 response type bits */
#define MMC_R1_READY_FOR_DATA           (1<<8)  /* ready for next transfer */
#define MMC_R1_APP_CMD                  (1<<5)  /* app. commands supported */
#define MMC_R1_SWITCH_ERROR             (1<<7)  /* switch command did not succeed */

/* SPI mode R1 response type bits */
#define SD_SPI_R1_IDLE_STATE            (1<<0)
#define SD_SPI_R1_ERASE_RST             (1<<1)
#define SD_SPI_R1_ILLEGAL_CMD           (1<<2)
#define SD_SPI_R1_CMD_CRC_ERR           (1<<3)
#define SD_SPI_R1_ERASE_SEQ_ERR         (1<<4)
#define SD_SPI_R1_ADDR_ERR              (1<<5)
#define SD_SPI_R1_PARAM_ERR             (1<<6)
#define SD_SPI_R1_NO_RESPONSE           (1<<7)

#define SDIO_R1_FUNC_NUM_ERR            (1<<4)

/* 48-bit response decoding (32 bits w/o CRC) */
#define MMC_R1(resp)                    ((resp)[0])
#define MMC_R3(resp)                    ((resp)[0])
#define MMC_R4(resp)                    ((resp)[0])
#define MMC_R5(resp)                    ((resp)[0])
#define SD_R6(resp)                     ((resp)[0])
#define MMC_R1_CURRENT_STATE(resp)      (((resp)[0] >> 9) & 0xf)

/* SPI mode response decoding */
#define SD_SPI_R1(resp)                 ((resp)[0] & 0xff)
#define SD_SPI_R2(resp)                 ((resp)[0] & 0xffff)
#define SD_SPI_R3(resp)                 ((resp)[0])
#define SD_SPI_R7(resp)                 ((resp)[0])

/* SPI mode data response decoding */
#define SD_SPI_DATA_RSP_VALID(resp_byte)        (((resp_byte)&0x11)==0x1)
#define SD_SPI_DATA_RSP(resp_byte)              (((resp_byte)>>1)&0x7)
#define  SD_SPI_DATA_ACCEPTED                   0x2
#define  SD_SPI_DATA_CRC_ERROR                  0x5
#define  SD_SPI_DATA_WR_ERROR                   0x6

/* RCA argument and response */
#define MMC_ARG_RCA(rca)                ((rca) << 16)
#define SD_R6_RCA(resp)                 (SD_R6((resp)) >> 16)

/* bus width argument */
#define SD_ARG_BUS_WIDTH_1              0
#define SD_ARG_BUS_WIDTH_4              2

/* EXT_CSD fields */
#define EXT_CSD_BUS_WIDTH               183     /* WO */
#define EXT_CSD_HS_TIMING               185     /* R/W */
#define EXT_CSD_REV                     192     /* RO */
#define EXT_CSD_STRUCTURE               194     /* RO */
#define EXT_CSD_CARD_TYPE               196     /* RO */
#define EXT_CSD_SEC_COUNT               212     /* RO */
#define EXT_CSD_PWR_CL_26_360           203     /* RO */
#define EXT_CSD_PWR_CL_52_360           202     /* RO */
#define EXT_CSD_PWR_CL_26_195           201     /* RO */
#define EXT_CSD_PWR_CL_52_195           200     /* RO */
#define EXT_CSD_POWER_CLASS             187     /* R/W */
#define EXT_CSD_CMD_SET                 191     /* R/W */
#define EXT_CSD_S_CMD_SET               504     /* RO */

/* EXT_CSD field definitions */
#define EXT_CSD_CMD_SET_NORMAL          (1U << 0)
#define EXT_CSD_CMD_SET_SECURE          (1U << 1)
#define EXT_CSD_CMD_SET_CPSECURE        (1U << 2)

/* EXT_CSD_HS_TIMING */
#define EXT_CSD_HS_TIMING_BC            0
#define EXT_CSD_HS_TIMING_HS            1
#define EXT_CSD_HS_TIMING_HS200         2
#define EXT_CSD_HS_TIMING_HS400         3

/* EXT_CSD_BUS_WIDTH  */
#define EXT_CSD_BUS_WIDTH_1             0
#define EXT_CSD_BUS_WIDTH_4             1
#define EXT_CSD_BUS_WIDTH_8             2
#define EXT_CSD_BUS_WIDTH_4_DDR         5
#define EXT_CSD_BUS_WIDTH_8_DDR         6

/* EXT_CSD_CARD_TYPE */
/* The only currently valid values for this field are 0x01, 0x03, 0x07,
 * 0x0B and 0x0F. */
#define EXT_CSD_CARD_TYPE_F_26M         (1 << 0)        /* SDR at "rated voltages */
#define EXT_CSD_CARD_TYPE_F_52M         (1 << 1)        /* SDR at "rated voltages */
#define EXT_CSD_CARD_TYPE_F_52M_1_8V    (1 << 2)        /* DDR, 1.8V or 3.3V I/O */
#define EXT_CSD_CARD_TYPE_F_52M_1_2V    (1 << 3)        /* DDR, 1.2V I/O */
#define EXT_CSD_CARD_TYPE_26M           0x01
#define EXT_CSD_CARD_TYPE_52M           0x03
#define EXT_CSD_CARD_TYPE_52M_V18       0x07
#define EXT_CSD_CARD_TYPE_52M_V12       0x0b
#define EXT_CSD_CARD_TYPE_52M_V12_18    0x0f

/* EXT_CSD MMC */
#define EXT_CSD_MMC_SIZE 512

/* MMC_SWITCH access mode */
#define MMC_SWITCH_MODE_CMD_SET         0x00    /* Change the command set */
#define MMC_SWITCH_MODE_SET_BITS        0x01    /* Set bits in value */
#define MMC_SWITCH_MODE_CLEAR_BITS      0x02    /* Clear bits in value */
#define MMC_SWITCH_MODE_WRITE_BYTE      0x03    /* Set target to value */

/* MMC R2 response (CSD) */
#define MMC_CSD_CSDVER(resp)            MMC_RSP_BITS((resp), 126, 2)
#define  MMC_CSD_CSDVER_1_0             1
#define  MMC_CSD_CSDVER_2_0             2
#define  MMC_CSD_CSDVER_EXT_CSD         3
#define MMC_CSD_MMCVER(resp)            MMC_RSP_BITS((resp), 122, 4)
#define  MMC_CSD_MMCVER_1_0             0 /* MMC 1.0 - 1.2 */
#define  MMC_CSD_MMCVER_1_4             1 /* MMC 1.4 */
#define  MMC_CSD_MMCVER_2_0             2 /* MMC 2.0 - 2.2 */
#define  MMC_CSD_MMCVER_3_1             3 /* MMC 3.1 - 3.3 */
#define  MMC_CSD_MMCVER_4_0             4 /* MMC 4 */
#define MMC_CSD_READ_BL_LEN(resp)       MMC_RSP_BITS((resp), 80, 4)
#define MMC_CSD_C_SIZE(resp)            MMC_RSP_BITS((resp), 62, 12)
#define MMC_CSD_CAPACITY(resp)          ((MMC_CSD_C_SIZE((resp))+1) << \
                                         (MMC_CSD_C_SIZE_MULT((resp))+2))
#define MMC_CSD_C_SIZE_MULT(resp)       MMC_RSP_BITS((resp), 47, 3)

/* MMC v1 R2 response (CID) */
#define MMC_CID_MID_V1(resp)            MMC_RSP_BITS((resp), 104, 24)
#define MMC_CID_PNM_V1_CPY(resp, pnm)                                   \
        do {                                                            \
                (pnm)[0] = MMC_RSP_BITS((resp), 96, 8);                 \
                (pnm)[1] = MMC_RSP_BITS((resp), 88, 8);                 \
                (pnm)[2] = MMC_RSP_BITS((resp), 80, 8);                 \
                (pnm)[3] = MMC_RSP_BITS((resp), 72, 8);                 \
                (pnm)[4] = MMC_RSP_BITS((resp), 64, 8);                 \
                (pnm)[5] = MMC_RSP_BITS((resp), 56, 8);                 \
                (pnm)[6] = MMC_RSP_BITS((resp), 48, 8);                 \
                (pnm)[7] = '\0';                                        \
        } while (0)
#define MMC_CID_REV_V1(resp)            MMC_RSP_BITS((resp), 40, 8)
#define MMC_CID_PSN_V1(resp)            MMC_RSP_BITS((resp), 16, 24)
#define MMC_CID_MDT_V1(resp)            MMC_RSP_BITS((resp), 8, 8)

/* MMC v2 R2 response (CID) */
#define MMC_CID_MID_V2(resp)            MMC_RSP_BITS((resp), 120, 8)
#define MMC_CID_OID_V2(resp)            MMC_RSP_BITS((resp), 104, 16)
#define MMC_CID_PNM_V2_CPY(resp, pnm)                                   \
        do {                                                            \
                (pnm)[0] = MMC_RSP_BITS((resp), 96, 8);                 \
                (pnm)[1] = MMC_RSP_BITS((resp), 88, 8);                 \
                (pnm)[2] = MMC_RSP_BITS((resp), 80, 8);                 \
                (pnm)[3] = MMC_RSP_BITS((resp), 72, 8);                 \
                (pnm)[4] = MMC_RSP_BITS((resp), 64, 8);                 \
                (pnm)[5] = MMC_RSP_BITS((resp), 56, 8);                 \
                (pnm)[6] = '\0';                                        \
        } while (0)
#define MMC_CID_PSN_V2(resp)            MMC_RSP_BITS((resp), 16, 32)

/* SD R2 response (CSD) */
#define SD_CSD_CSDVER(resp)             MMC_RSP_BITS((resp), 126, 2)
#define  SD_CSD_CSDVER_1_0              0
#define  SD_CSD_CSDVER_2_0              1
#define SD_CSD_TAAC(resp)               MMC_RSP_BITS((resp), 112, 8)
#define  SD_CSD_TAAC_1_5_MSEC           0x26
#define SD_CSD_NSAC(resp)               MMC_RSP_BITS((resp), 104, 8)
#define SD_CSD_SPEED(resp)              MMC_RSP_BITS((resp), 96, 8)
#define  SD_CSD_SPEED_25_MHZ            0x32
#define  SD_CSD_SPEED_50_MHZ            0x5a
#define SD_CSD_CCC(resp)                MMC_RSP_BITS((resp), 84, 12)
#define  SD_CSD_CCC_BASIC               (1 << 0)        /* basic */
#define  SD_CSD_CCC_BR                  (1 << 2)        /* block read */
#define  SD_CSD_CCC_BW                  (1 << 4)        /* block write */
#define  SD_CSD_CCC_ERASE               (1 << 5)        /* erase */
#define  SD_CSD_CCC_WP                  (1 << 6)        /* write protection */
#define  SD_CSD_CCC_LC                  (1 << 7)        /* lock card */
#define  SD_CSD_CCC_AS                  (1 << 8)        /*application specific*/
#define  SD_CSD_CCC_IOM                 (1 << 9)        /* I/O mode */
#define  SD_CSD_CCC_SWITCH              (1 << 10)       /* switch */
#define SD_CSD_READ_BL_LEN(resp)        MMC_RSP_BITS((resp), 80, 4)
#define SD_CSD_READ_BL_PARTIAL(resp)    MMC_RSP_BITS((resp), 79, 1)
#define SD_CSD_WRITE_BLK_MISALIGN(resp) MMC_RSP_BITS((resp), 78, 1)
#define SD_CSD_READ_BLK_MISALIGN(resp)  MMC_RSP_BITS((resp), 77, 1)
#define SD_CSD_DSR_IMP(resp)            MMC_RSP_BITS((resp), 76, 1)
#define SD_CSD_C_SIZE(resp)             MMC_RSP_BITS((resp), 62, 12)
#define SD_CSD_CAPACITY(resp)           ((SD_CSD_C_SIZE((resp))+1) << \
                                         (SD_CSD_C_SIZE_MULT((resp))+2))
#define SD_CSD_V2_C_SIZE(resp)          MMC_RSP_BITS((resp), 48, 22)
#define SD_CSD_V2_CAPACITY(resp)        ((SD_CSD_V2_C_SIZE((resp))+1) << 10)
#define SD_CSD_V2_BL_LEN                0x9     /* 512 */
#define SD_CSD_VDD_R_CURR_MIN(resp)     MMC_RSP_BITS((resp), 59, 3)
#define SD_CSD_VDD_R_CURR_MAX(resp)     MMC_RSP_BITS((resp), 56, 3)
#define SD_CSD_VDD_W_CURR_MIN(resp)     MMC_RSP_BITS((resp), 53, 3)
#define SD_CSD_VDD_W_CURR_MAX(resp)     MMC_RSP_BITS((resp), 50, 3)
#define  SD_CSD_VDD_RW_CURR_100mA       0x7
#define  SD_CSD_VDD_RW_CURR_80mA        0x6
#define SD_CSD_C_SIZE_MULT(resp)        MMC_RSP_BITS((resp), 47, 3)
#define SD_CSD_ERASE_BLK_EN(resp)       MMC_RSP_BITS((resp), 46, 1)
#define SD_CSD_SECTOR_SIZE(resp)        MMC_RSP_BITS((resp), 39, 7) /* +1 */
#define SD_CSD_WP_GRP_SIZE(resp)        MMC_RSP_BITS((resp), 32, 7) /* +1 */
#define SD_CSD_WP_GRP_ENABLE(resp)      MMC_RSP_BITS((resp), 31, 1)
#define SD_CSD_R2W_FACTOR(resp)         MMC_RSP_BITS((resp), 26, 3)
#define SD_CSD_WRITE_BL_LEN(resp)       MMC_RSP_BITS((resp), 22, 4)
#define  SD_CSD_RW_BL_LEN_2G            0xa
#define  SD_CSD_RW_BL_LEN_1G            0x9
#define SD_CSD_WRITE_BL_PARTIAL(resp)   MMC_RSP_BITS((resp), 21, 1)
#define SD_CSD_FILE_FORMAT_GRP(resp)    MMC_RSP_BITS((resp), 15, 1)
#define SD_CSD_COPY(resp)               MMC_RSP_BITS((resp), 14, 1)
#define SD_CSD_PERM_WRITE_PROTECT(resp) MMC_RSP_BITS((resp), 13, 1)
#define SD_CSD_TMP_WRITE_PROTECT(resp)  MMC_RSP_BITS((resp), 12, 1)
#define SD_CSD_FILE_FORMAT(resp)        MMC_RSP_BITS((resp), 10, 2)

/* SD R2 response (CID) */
#define SD_CID_MID(resp)                MMC_RSP_BITS((resp), 120, 8)
#define SD_CID_OID(resp)                MMC_RSP_BITS((resp), 104, 16)
#define SD_CID_PNM_CPY(resp, pnm)                                       \
        do {                                                            \
                (pnm)[0] = MMC_RSP_BITS((resp), 96, 8);                 \
                (pnm)[1] = MMC_RSP_BITS((resp), 88, 8);                 \
                (pnm)[2] = MMC_RSP_BITS((resp), 80, 8);                 \
                (pnm)[3] = MMC_RSP_BITS((resp), 72, 8);                 \
                (pnm)[4] = MMC_RSP_BITS((resp), 64, 8);                 \
                (pnm)[5] = '\0';                                        \
        } while (0)
#define SD_CID_REV(resp)                MMC_RSP_BITS((resp), 56, 8)
#define SD_CID_PSN(resp)                MMC_RSP_BITS((resp), 24, 32)
#define SD_CID_MDT(resp)                MMC_RSP_BITS((resp), 8, 12)

/* SCR (SD Configuration Register) */
#define SCR_STRUCTURE(scr)              MMC_RSP_BITS((scr), 60, 4)
#define  SCR_STRUCTURE_VER_1_0          0 /* Version 1.0 */
#define SCR_SD_SPEC(scr)                MMC_RSP_BITS((scr), 56, 4)
#define  SCR_SD_SPEC_VER_1_0            0 /* Version 1.0 and 1.01 */
#define  SCR_SD_SPEC_VER_1_10           1 /* Version 1.10 */
#define  SCR_SD_SPEC_VER_2              2 /* Version 2.00 or Version 3.0X */
#define SCR_DATA_STAT_AFTER_ERASE(scr)  MMC_RSP_BITS((scr), 55, 1)
#define SCR_SD_SECURITY(scr)            MMC_RSP_BITS((scr), 52, 3)
#define  SCR_SD_SECURITY_NONE           0 /* no security */
#define  SCR_SD_SECURITY_1_0            1 /* security protocol 1.0 */
#define  SCR_SD_SECURITY_1_0_2          2 /* security protocol 1.0 */
#define SCR_SD_BUS_WIDTHS(scr)          MMC_RSP_BITS((scr), 48, 4)
#define  SCR_SD_BUS_WIDTHS_1BIT         (1 << 0) /* 1bit (DAT0) */
#define  SCR_SD_BUS_WIDTHS_4BIT         (1 << 2) /* 4bit (DAT0-3) */
#define SCR_SD_SPEC3(scr)               MMC_RSP_BITS((scr), 47, 1)
#define SCR_EX_SECURITY(scr)            MMC_RSP_BITS((scr), 43, 4)
#define SCR_SD_SPEC4(scr)               MMC_RSP_BITS((scr), 42, 1)
#define SCR_RESERVED(scr)               MMC_RSP_BITS((scr), 34, 8)
#define SCR_CMD_SUPPORT_CMD23(scr)      MMC_RSP_BITS((scr), 33, 1)
#define SCR_CMD_SUPPORT_CMD20(scr)      MMC_RSP_BITS((scr), 32, 1)
#define SCR_RESERVED2(scr)              MMC_RSP_BITS((scr), 0, 32)

/* Max supply current in SWITCH_FUNC response (in mA) */
#define SD_SFUNC_I_MAX(status) (MMC_RSP_BITS((uint32_t *)(status), 496, 16))

/* Supported flags in SWITCH_FUNC response */
#define SD_SFUNC_SUPPORTED(status, group) \
        (MMC_RSP_BITS((uint32_t *)(status), 400 + (group - 1) * 16, 16))

/* Selected function in SWITCH_FUNC response */
#define SD_SFUNC_SELECTED(status, group) \
        (MMC_RSP_BITS((uint32_t *)(status), 376 + (group - 1) * 4, 4))

/* Busy flags in SWITCH_FUNC response */
#define SD_SFUNC_BUSY(status, group) \
        (MMC_RSP_BITS((uint32_t *)(status), 272 + (group - 1) * 16, 16))

/* Version of SWITCH_FUNC response */
#define SD_SFUNC_VER(status)    (MMC_RSP_BITS((uint32_t *)(status), 368, 8))

#define SD_SFUNC_GROUP_MAX      6
#define SD_SFUNC_FUNC_MAX       15

#define SD_ACCESS_MODE          1       /* Function group 1, Access Mode */

#define SD_ACCESS_MODE_SDR12    0       /* 25 MHz clock */
#define SD_ACCESS_MODE_SDR25    1       /* 50 MHz clock */
#define SD_ACCESS_MODE_SDR50    2       /* UHS-I, 100 MHz clock */
#define SD_ACCESS_MODE_SDR104   3       /* UHS-I, 208 MHz clock */
#define SD_ACCESS_MODE_DDR50    4       /* UHS-I, 50 MHz clock, DDR */

/**
 * @brief Extract up to 32 sequential bits from an array of 32-bit words
 *
 * Bits within the word are numbered in the increasing order from LSB to MSB.
 *
 * As an example, consider 2 32-bit words:
 *
 * 0x01234567 0x89abcdef
 *
 * On a little-endian system, the bytes are stored in memory as follows:
 *
 * 67 45 23 01 ef cd ab 89
 *
 * MMC_RSP_BITS will extact bits as follows:
 *
 * start=0  len=4   -> result=0x00000007
 * start=0  len=12  -> result=0x00000567
 * start=28 len=8   -> result=0x000000f0
 * start=59 len=5   -> result=0x00000011
 *
 * @param src array of words to extract bits from
 * @param start index of the first bit to extract
 * @param len number of bits to extract, 1 to 32
 * @return 32-bit word where requested bits start from LSB
 */
static inline uint32_t MMC_RSP_BITS(uint32_t *src, int start, int len)
{
    uint32_t mask = (len % 32 == 0) ? UINT_MAX : UINT_MAX >> (32 - (len % 32));
    size_t word = start / 32;
    size_t shift = start % 32;
    uint32_t right = src[word] >> shift;
    uint32_t left = (len + shift <= 32) ? 0 : src[word + 1] << ((32 - shift) % 32);
    return (left | right) & mask;
}

/* SD R4 response (IO OCR) */
#define SD_IO_OCR_MEM_READY             (1<<31)
#define SD_IO_OCR_NUM_FUNCTIONS(ocr)    (((ocr) >> 28) & 0x7)
#define SD_IO_OCR_MEM_PRESENT       (1<<27)
#define SD_IO_OCR_MASK              0x00fffff0

/* CMD52 arguments */
#define SD_ARG_CMD52_READ           (0<<31)
#define SD_ARG_CMD52_WRITE          (1<<31)
#define SD_ARG_CMD52_FUNC_SHIFT     28
#define SD_ARG_CMD52_FUNC_MASK      0x7
#define SD_ARG_CMD52_EXCHANGE       (1<<27)
#define SD_ARG_CMD52_REG_SHIFT      9
#define SD_ARG_CMD52_REG_MASK       0x1ffff
#define SD_ARG_CMD52_DATA_SHIFT     0
#define SD_ARG_CMD52_DATA_MASK      0xff
#define SD_R5_DATA(resp)            ((resp)[0] & 0xff)

/* CMD53 arguments */
#define SD_ARG_CMD53_READ           (0<<31)
#define SD_ARG_CMD53_WRITE          (1<<31)
#define SD_ARG_CMD53_FUNC_SHIFT     28
#define SD_ARG_CMD53_FUNC_MASK      0x7
#define SD_ARG_CMD53_BLOCK_MODE     (1<<27)
#define SD_ARG_CMD53_INCREMENT      (1<<26)
#define SD_ARG_CMD53_REG_SHIFT      9
#define SD_ARG_CMD53_REG_MASK       0x1ffff
#define SD_ARG_CMD53_LENGTH_SHIFT   0
#define SD_ARG_CMD53_LENGTH_MASK    0x1ff
#define SD_ARG_CMD53_LENGTH_MAX     512

/* Card Common Control Registers (CCCR) */
#define SD_IO_CCCR_START            0x00000
#define SD_IO_CCCR_SIZE             0x100
#define SD_IO_CCCR_FN_ENABLE        0x02
#define SD_IO_CCCR_FN_READY         0x03
#define SD_IO_CCCR_INT_ENABLE       0x04
#define SD_IO_CCCR_INT_PENDING      0x05
#define SD_IO_CCCR_CTL              0x06
#define  CCCR_CTL_RES               (1<<3)
#define SD_IO_CCCR_BUS_WIDTH        0x07
#define  CCCR_BUS_WIDTH_1           (0<<0)
#define  CCCR_BUS_WIDTH_4           (2<<0)
#define  CCCR_BUS_WIDTH_8           (3<<0)
#define  CCCR_BUS_WIDTH_ECSI        (1<<5)
#define SD_IO_CCCR_CARD_CAP         0x08
#define  CCCR_CARD_CAP_LSC          BIT(6)
#define  CCCR_CARD_CAP_4BLS         BIT(7)
#define SD_IO_CCCR_CISPTR           0x09
#define SD_IO_CCCR_BLKSIZEL         0x10
#define SD_IO_CCCR_BLKSIZEH         0x11
#define SD_IO_CCCR_HIGHSPEED        0x13
#define  CCCR_HIGHSPEED_SUPPORT     BIT(0)
#define  CCCR_HIGHSPEED_ENABLE      BIT(1)

/* Function Basic Registers (FBR) */
#define SD_IO_FBR_START         0x00100
#define SD_IO_FBR_SIZE          0x00700

/* Card Information Structure (CIS) */
#define SD_IO_CIS_START         0x01000
#define SD_IO_CIS_SIZE          0x17000

/* CIS tuple codes (based on PC Card 16) */
#define CISTPL_CODE_NULL            0x00
#define CISTPL_CODE_DEVICE          0x01
#define CISTPL_CODE_CHKSUM          0x10
#define CISTPL_CODE_VERS1           0x15
#define CISTPL_CODE_ALTSTR          0x16
#define CISTPL_CODE_CONFIG          0x1A
#define CISTPL_CODE_CFTABLE_ENTRY   0x1B
#define CISTPL_CODE_MANFID          0x20
#define CISTPL_CODE_FUNCID          0x21
#define   TPLFID_FUNCTION_SDIO        0x0c
#define CISTPL_CODE_FUNCE           0x22
#define CISTPL_CODE_VENDER_BEGIN    0x80
#define CISTPL_CODE_VENDER_END      0x8F
#define CISTPL_CODE_SDIO_STD        0x91
#define CISTPL_CODE_SDIO_EXT        0x92
#define CISTPL_CODE_END             0xFF


/* Timing */
#define SDMMC_TIMING_LEGACY 0
#define SDMMC_TIMING_HIGHSPEED 1
#define SDMMC_TIMING_MMC_DDR52 2

#include "py/runtime.h"

// Logging macros
// #define ESP_LOGD(tag, string, ...) mp_printf(&mp_plat_print, string "\n" __VA_OPT__(,) __VA_ARGS__)
// #define ESP_LOGV(tag, string, ...) mp_printf(&mp_plat_print, string "\n" __VA_OPT__(,) __VA_ARGS__)
// #define ESP_LOGW(tag, string, ...) mp_printf(&mp_plat_print, string "\n" __VA_OPT__(,) __VA_ARGS__)
// #define ESP_LOGE(tag, string, ...) mp_printf(&mp_plat_print, string "\n" __VA_OPT__(,) __VA_ARGS__)

#define ESP_LOGD(tag, string, ...)
#define ESP_LOGV(tag, string, ...)
#define ESP_LOGW(tag, string, ...)
#define ESP_LOGE(tag, string, ...)
#define TSD_MIN(_x, _y)        ( ( (_x) < (_y) ) ? (_x) : (_y) )

#endif //_SDMMC_DEFS_H_

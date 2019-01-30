/*
 * (C) Copyright 2015-2016 - Analog Devices, Inc.  All rights reserved.
 *
 * FILE:     sd_card_registers.h
 *
 * CHANGES:  1.00.0  - initial release
 */

#ifndef __SD_CARD_REGISTERS_H__
#define __SD_CARD_REGISTERS_H__

#include <stdbool.h>

/****************************************************/
/*  SD Card Status Register                         */
/****************************************************/
/****************************************************/
/*  SD Card Status Register Enumerations            */
/****************************************************/
typedef enum {  SD_CSR_CURRENT_STATE_IDLE            = (0<<9),
                SD_CSR_CURRENT_STATE_READY           = (1<<9),
                SD_CSR_CURRENT_STATE_IDENTIFICATION  = (2<<9),
                SD_CSR_CURRENT_STATE_STANDBY         = (3<<9),
                SD_CSR_CURRENT_STATE_TRANSFER        = (4<<9),
                SD_CSR_CURRENT_STATE_DATA            = (5<<9),
                SD_CSR_CURRENT_STATE_RECEIVE         = (6<<9),
                SD_CSR_CURRENT_STATE_PROGRAM         = (7<<9),
                SD_CSR_CURRENT_STATE_DISCONNECT      = (8<<9),
                SD_CSR_CURRENT_STATE_RESERVED0       = (9<<9),
                SD_CSR_CURRENT_STATE_RESERVED1       = (10<<9),
                SD_CSR_CURRENT_STATE_RESERVED2       = (11<<9),
                SD_CSR_CURRENT_STATE_RESERVED3       = (12<<9),
                SD_CSR_CURRENT_STATE_RESERVED4       = (13<<9),
                SD_CSR_CURRENT_STATE_RESERVED5       = (14<<9),
                SD_CSR_CURRENT_STATE_IO_MODE         = (15<<9)
} SD_CSR_CURRENT_CARD_STATE;

/****************************************************/
/*  SD Card Status Register Bit Masks               */
/****************************************************/
#define SD_CSR_AKE_SEQ_ERROR        (1<<3)
#define SD_CSR_APP_CMD              (1<<5)
#define SD_CSR_READY_FOR_DATA       (1<<8)
#define SD_CSR_CURRENT_STATE        (15<<9)
#define SD_CSR_ERASE_RESET          (1<<13)
#define SD_CSR_CARD_ECC_DISABLED    (1<<14)
#define SD_CSR_WP_ERASE_SKIP        (1<<15)
#define SD_CSR_CSD_OVERWRITE        (1<<16)
#define SD_CSR_ERROR                (1<<19)
#define SD_CSR_CC_ERROR             (1<<20)
#define SD_CSR_CARD_ECC_FAILED      (1<<21)
#define SD_CSR_ILLEGAL_COMMAND      (1<<22)
#define SD_CSR_COM_CRC_ERROR        (1<<23)
#define SD_CSR_LOCK_UNLOCK_FAILED   (1<<24)
#define SD_CSR_CARD_IS_LOCKED       (1<<25)
#define SD_CSR_WP_VIOLATION         (1<<26)
#define SD_CSR_ERASE_PARAM          (1<<27)
#define SD_CSR_ERASE_SEQ_ERROR      (1<<28)
#define SD_CSR_BLOCK_LEN_ERROR      (1<<29)
#define SD_CSR_ADDRESS_ERROR        (1<<30)
#define SD_CSR_OUT_OF_RANGE         (1<<31)

typedef unsigned long RSI_SD_CSR_REGISTER;
/****************************************************/
/*  SD Status Register                              */
/****************************************************/

/****************************************************/
/*  SD Status Register Enumerations                 */
/****************************************************/
typedef enum {  SD_SSR_DAT_BUS_WIDTH_1           = 0,
                SD_SSR_DAT_BUS_WIDTH_RESERVED0   = 1,
                SD_SSR_DAT_BUS_WIDTH_4           = 2,
                SD_SSR_DAT_BUS_WIDTH_RESERVED1   = 3
} SD_SSR_DAT_BUS_WIDTH;

typedef enum {  SD_SSR_SECURED_MODE_DISABLED           = 0,
                SD_SSR_SECURED_MODE_ENABLED            = 1
} SD_SSR_SECURED_MODE;

typedef enum {  SD_SSR_SD_CARD_TYPE_REGULAR_RD_WR_CARD   = 0,
                SD_SSR_SD_CARD_TYPE_ROM                  = 1
} SD_SSR_SD_CARD_TYPE;

typedef enum {  SD_SSR_SPEED_CLASS_0 = 0,
                SD_SSR_SPEED_CLASS_2 = 1,
                SD_SSR_SPEED_CLASS_3 = 2,
                SD_SSR_SPEED_CLASS_6 = 3
} SD_SSR_SPEED_CLASS;

typedef enum {  SD_SSR_AU_SIZE_NOT_DEFINED   = 0,
                SD_SSR_AU_SIZE_16KB          = 1,
                SD_SSR_AU_SIZE_32KB          = 2,
                SD_SSR_AU_SIZE_64KB          = 3,
                SD_SSR_AU_SIZE_128KB         = 4,
                SD_SSR_AU_SIZE_256KB         = 5,
                SD_SSR_AU_SIZE_512KB         = 6,
                SD_SSR_AU_SIZE_1MB           = 7,
                SD_SSR_AU_SIZE_2MB           = 8,
                SD_SSR_AU_SIZE_4MB           = 9,
                SD_SSR_AU_SIZE_RESERVED0     = 10,
                SD_SSR_AU_SIZE_RESERVED1     = 11,
                SD_SSR_AU_SIZE_RESERVED2     = 12,
                SD_SSR_AU_SIZE_RESERVED3     = 13,
                SD_SSR_AU_SIZE_RESERVED4     = 14,
                SD_SSR_AU_SIZE_RESERVED5     = 15
}SD_SSR_AU_SIZE;

/****************************************************/
/*  SD Status Register Structure                    */
/****************************************************/
typedef struct _RSI_SD_SSR_REGISTER{
    SD_SSR_DAT_BUS_WIDTH dat_bus_width;
    SD_SSR_SECURED_MODE  secured_mode;
    SD_SSR_SD_CARD_TYPE  sd_card_type;
    unsigned long           size_of_protected_area;
    SD_SSR_SPEED_CLASS   speed_class;
    unsigned char           performance_move;
    SD_SSR_AU_SIZE       au_size;
    unsigned short          erase_size;
    unsigned char           erase_timeout;
    unsigned char           erase_offset;
}RSI_SD_SSR_REGISTER;

/****************************************************/
/*  SD OCR Register                                 */
/****************************************************/
/****************************************************/
/*  SD OCR Register Bit Masks                       */
/****************************************************/
#define SD_OCR_LOW_VOLTAGE          (1<<7)
#define SD_OCR_VOLTAGE_27_28        (1<<15)
#define SD_OCR_VOLTAGE_28_29        (1<<16)
#define SD_OCR_VOLTAGE_29_30        (1<<17)
#define SD_OCR_VOLTAGE_30_31        (1<<18)
#define SD_OCR_VOLTAGE_31_32        (1<<19)
#define SD_OCR_VOLTAGE_32_33        (1<<20)
#define SD_OCR_VOLTAGE_33_34        (1<<21)
#define SD_OCR_VOLTAGE_34_35        (1<<22)
#define SD_OCR_VOLTAGE_35_36        (1<<23)
#define SD_OCR_CARD_CAPACITY_STATUS (1<<30)
#define SD_OCR_CARD_POWER_UP_STATUS (1<<31)

typedef  unsigned long RSI_SD_OCR_REGISTER;

/****************************************************/
/*  SD CID Register                                 */
/****************************************************/
/****************************************************/
/*  SD CID Register Structure                       */
/****************************************************/
typedef struct _RSI_SD_CID_REGISTER{
    unsigned char   mid;
    char            oid[2];
    char            pnm[5];
    unsigned char   prv;
    unsigned long   psn;
    unsigned short  mdt;
    unsigned char   crc;
} RSI_SD_CID_REGISTER;

/****************************************************/
/*  SD CSD Register Version 1.0 & 2.0               */
/****************************************************/
/****************************************************/
/*  SD CSD Register Version 1.0 & 2.0 Enumerations  */
/****************************************************/
typedef enum{
    SD_CSD_STRUCTURE_VERSION_1_0          = 0,
    SD_CSD_STRUCTURE_VERSION_2_0          = 1,
    SD_CSD_STRUCTURE_VERSION_RESERVED0    = 2,
    SD_CSD_STRUCTURE_VERSION_RESERVED1    = 3
}SD_CSD_STRUCTURE;

typedef enum{
    SD_CSD_VDD_CURR_MIN_0_5mA   = 0,
    SD_CSD_VDD_CURR_MIN_1_0mA   = 1,
    SD_CSD_VDD_CURR_MIN_5_0mA   = 2,
    SD_CSD_VDD_CURR_MIN_10_0mA  = 3,
    SD_CSD_VDD_CURR_MIN_25_0mA  = 4,
    SD_CSD_VDD_CURR_MIN_35_0mA  = 5,
    SD_CSD_VDD_CURR_MIN_60_0mA  = 6,
    SD_CSD_VDD_CURR_MIN_100_0mA = 7,
    SD_CSD_VDD_CURR_MIN_NOT_APPLICABLE = 8
}SD_CSD_VDD_CURR_MIN;

typedef enum{
    SD_CSD_VDD_CURR_MAX_1_0mA   = 0,
    SD_CSD_VDD_CURR_MAX_5_0mA   = 1,
    SD_CSD_VDD_CURR_MAX_10_0mA  = 2,
    SD_CSD_VDD_CURR_MAX_25_0mA  = 3,
    SD_CSD_VDD_CURR_MAX_35_0mA  = 4,
    SD_CSD_VDD_CURR_MAX_45_0mA  = 5,
    SD_CSD_VDD_CURR_MAX_80_0mA  = 6,
    SD_CSD_VDD_CURR_MAX_200_0mA = 7,
    SD_CSD_VDD_CURR_MAX_NOT_APPLICABLE = 8
}SD_CSD_VDD_CURR_MAX;

typedef enum{
    SD_CSD_FILE_FORMAT_HD_WITH_PARTITION_TABLE          = 0,
    SD_CSD_FILE_FORMAT_DOS_FAT_WITH_BOOT_SECTOR_ONLY    = 1,
    SD_CSD_FILE_FORMAT_UNIVERSAL_FILE_FORMAT            = 2,
    SD_CSD_FILE_FORMAT_OTHER_UNKNOWN                    = 3
}SD_CSD_FILE_FORMAT;

/********************************************************/
/*  SD CSD Register Version 1.0 and 2.0 Structure       */
/********************************************************/
/********************************************************/
/* For Version 2.0 support C_size has been extended     */
/* Vdd_curr_min and Vdd_curr_max values not applicable  */
/* C_SIZE_MULT not applicable                           */
/********************************************************/
typedef struct _RSI_SD_CSD_REGISTER{
    SD_CSD_STRUCTURE    csd_structure;
    unsigned char       taac;
    unsigned char       nsac;
    unsigned char       tran_speed;
    unsigned short      ccc;
    unsigned char       read_bl_len;
    bool                read_bl_partial;
    bool                write_blk_misalign;
    bool                read_blk_misalign;
    bool                dsr_imp;
    unsigned long       c_size;
    SD_CSD_VDD_CURR_MIN vdd_r_curr_min;
    SD_CSD_VDD_CURR_MAX vdd_r_curr_max;
    SD_CSD_VDD_CURR_MIN vdd_w_curr_min;
    SD_CSD_VDD_CURR_MAX vdd_w_curr_max;
    unsigned char       c_size_mult;
    bool                erase_blk_en;
    unsigned char       sector_size;
    unsigned char       wp_grp_size;
    bool                wp_grp_enable;
    unsigned char       r2w_factor;
    unsigned char       write_bl_length;
    bool                write_bl_partial;
    bool                file_format_grp;
    bool                copy;
    bool                perm_write_protect;
    bool                tmp_write_protect;
    SD_CSD_FILE_FORMAT  file_format;
    unsigned char       crc;
}RSI_SD_CSD_REGISTER;

/********************************************************/
/*  SD RCA Register                                     */
/********************************************************/    
typedef unsigned short RSI_SD_RCA_REGISTER;

/********************************************************/
/*  SD DSR Register                                     */
/********************************************************/    
typedef unsigned short RSI_SD_DSR_REGISTER;

/****************************************************/
/*  SD SCR Register                                 */
/****************************************************/
/****************************************************/
/*  SD SCR Register Enumerations                    */
/****************************************************/
typedef enum{
    SD_SCR_STRUCTURE_VERSION_NO_1_0    = 0,
    SD_SCR_STRUCTURE_VERSION_RESERVED0  = 1,
    SD_SCR_STRUCTURE_VERSION_RESERVED1  = 2,
    SD_SCR_STRUCTURE_VERSION_RESERVED2  = 3,
    SD_SCR_STRUCTURE_VERSION_RESERVED3  = 4,
    SD_SCR_STRUCTURE_VERSION_RESERVED4  = 5,
    SD_SCR_STRUCTURE_VERSION_RESERVED5  = 6,
    SD_SCR_STRUCTURE_VERSION_RESERVED6  = 7,
    SD_SCR_STRUCTURE_VERSION_RESERVED7  = 8,
    SD_SCR_STRUCTURE_VERSION_RESERVED8  = 9,
    SD_SCR_STRUCTURE_VERSION_RESERVED9  = 10,
    SD_SCR_STRUCTURE_VERSION_RESERVED10 = 11,
    SD_SCR_STRUCTURE_VERSION_RESERVED11 = 12,
    SD_SCR_STRUCTURE_VERSION_RESERVED12 = 13,
    SD_SCR_STRUCTURE_VERSION_RESERVED13 = 14,
    SD_SCR_STRUCTURE_VERSION_RESERVED14 = 15
}SD_SCR_STRUCTURE;

typedef enum{
    SD_SCR_SD_SPEC_VERSION_1_0_TO_1_01   = 0,
    SD_SCR_SD_SPEC_VERSION_1_10      = 1,
    SD_SCR_SD_SPEC_VERSION_2_00      = 2,
    SD_SCR_SD_SPEC_RESERVED0        = 3,
    SD_SCR_SD_SPEC_RESERVED1        = 4,
    SD_SCR_SD_SPEC_RESERVED2        = 5,
    SD_SCR_SD_SPEC_RESERVED3        = 6,
    SD_SCR_SD_SPEC_RESERVED4        = 7,
    SD_SCR_SD_SPEC_RESERVED5        = 8,
    SD_SCR_SD_SPEC_RESERVED6        = 9,
    SD_SCR_SD_SPEC_RESERVED7        = 10,
    SD_SCR_SD_SPEC_RESERVED8        = 11,
    SD_SCR_SD_SPEC_RESERVED9        = 12,
    SD_SCR_SD_SPEC_RESERVED10       = 13,
    SD_SCR_SD_SPEC_RESERVED11       = 14,
    SD_SCR_SD_SPEC_RESERVED12       = 15
}SD_SCR_SD_SPEC;

typedef enum{
    SD_SCR_SD_SECURITY_NO_SECURITY  = 0,
    SD_SCR_SD_SECURITY_NOT_USED     = 1,
    SD_SCR_SD_SECURITY_VERSION_1_01  = 2,
    SD_SCR_SD_SECURITY_VERSION_2_00  = 3,
    SD_SCR_SD_SECURITY_RESERVED0    = 4,
    SD_SCR_SD_SECURITY_RESERVED1    = 5,
    SD_SCR_SD_SECURITY_RESERVED2    = 6,
    SD_SCR_SD_SECURITY_RESERVED3    = 7
}SD_SCR_SD_SECURITY;

typedef enum{
    SD_SCR_SD_BUS_WIDTHS_1_BIT      = 0,
    SD_SCR_SD_BUS_WIDTHS_RESERVED0  = 1,
    SD_SCR_SD_BUS_WIDTHS_4_BIT      = 2,
    SD_SCR_SD_BUS_WIDTHS_RESERVED1  = 3,
    SD_SCR_SD_BUS_WIDTHS_1_BIT_AND_4_BIT = 5
}SD_SCR_SD_BUS_WIDTHS;

/********************************************************/
/*  SD SCR Register Structure                           */
/********************************************************/

typedef struct _RSI_SD_SCR_REGISTER{
    SD_SCR_STRUCTURE        scr_structure;
    SD_SCR_SD_SPEC          sd_spec;
    bool                    data_stat_after_erase;
    SD_SCR_SD_SECURITY      sd_security;
    SD_SCR_SD_BUS_WIDTHS    sd_bus_widths;
}RSI_SD_SCR_REGISTER;

/********************************************************/
/*  SD Card Register Structure                          */
/********************************************************/
typedef struct _RSI_SD_CARD_REGISTERS{
    RSI_SD_CSR_REGISTER csr;
    RSI_SD_SSR_REGISTER ssr;
    RSI_SD_OCR_REGISTER ocr;
    RSI_SD_CID_REGISTER cid;
    RSI_SD_CSD_REGISTER csd;
    RSI_SD_RCA_REGISTER rca;
    RSI_SD_DSR_REGISTER dsr;
    RSI_SD_SCR_REGISTER scr;
} RSI_SD_CARD_REGISTERS;

#endif /* __SD_CARD_REGISTERS_H__ */

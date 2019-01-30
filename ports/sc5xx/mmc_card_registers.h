/*
 * (C) Copyright 2015-2016 - Analog Devices, Inc.  All rights reserved.
 *
 * FILE:     mmc_card_registers.h
 *
 * CHANGES:  1.00.0  - initial release
 */

#ifndef __MMC_CARD_REGISTERS_H__
#define __MMC_CARD_REGISTERS_H__

#include <sys/platform.h>
#include <stdbool.h>

/****************************************************/
/*  MMC Card Status Register                        */
/****************************************************/
/****************************************************/
/*  MMC Card Status Register Enumerations           */
/****************************************************/
typedef enum
{	MMC_CSR_CURRENT_STATE_IDLE            = (0),
	MMC_CSR_CURRENT_STATE_READY           = (1),
	MMC_CSR_CURRENT_STATE_IDENTIFICATION  = (2),
	MMC_CSR_CURRENT_STATE_STANDBY         = (3),
	MMC_CSR_CURRENT_STATE_TRANSFER        = (4),
	MMC_CSR_CURRENT_STATE_DATA            = (5),
	MMC_CSR_CURRENT_STATE_RECEIVE         = (6),
	MMC_CSR_CURRENT_STATE_PROGRAM         = (7),
	MMC_CSR_CURRENT_STATE_DISCONNECT      = (8),
	MMC_CSR_CURRENT_STATE_BUS_TEST        = (9),
	MMC_CSR_CURRENT_STATE_SLEEP           = (10),
	MMC_CSR_CURRENT_STATE_RESERVED2       = (11),
	MMC_CSR_CURRENT_STATE_RESERVED3       = (12),
	MMC_CSR_CURRENT_STATE_RESERVED4       = (13),
	MMC_CSR_CURRENT_STATE_RESERVED5       = (14),
	MMC_CSR_CURRENT_STATE_IO_MODE         = (15)
} MMC_CSR_CURRENT_CARD_STATE;

typedef unsigned long RSI_MMC_CSR_REGISTER;

/****************************************************/
/*  MMC OCR Register                                */
/****************************************************/
/****************************************************/
/*  MMC OCR Register Bit Masks                      */
/****************************************************/
#define MMC_OCR_LOW_VOLTAGE_MASK          (1<<7)
#define MMC_OCR_VOLTAGE_20_21_MASK        (1<<8)
#define MMC_OCR_VOLTAGE_21_22_MASK        (1<<9)
#define MMC_OCR_VOLTAGE_22_23_MASK        (1<<10)
#define MMC_OCR_VOLTAGE_23_24_MASK        (1<<11)
#define MMC_OCR_VOLTAGE_24_25_MASK        (1<<12)
#define MMC_OCR_VOLTAGE_25_26_MASK        (1<<13)
#define MMC_OCR_VOLTAGE_26_27_MASK        (1<<14)
#define MMC_OCR_VOLTAGE_27_28_MASK        (1<<15)
#define MMC_OCR_VOLTAGE_28_29_MASK        (1<<16)
#define MMC_OCR_VOLTAGE_29_30_MASK        (1<<17)
#define MMC_OCR_VOLTAGE_30_31_MASK        (1<<18)
#define MMC_OCR_VOLTAGE_31_32_MASK        (1<<19)
#define MMC_OCR_VOLTAGE_32_33_MASK        (1<<20)
#define MMC_OCR_VOLTAGE_33_34_MASK        (1<<21)
#define MMC_OCR_VOLTAGE_34_35_MASK        (1<<22)
#define MMC_OCR_VOLTAGE_35_36_MASK        (1<<23)
#define MMC_OCR_CARD_CAPACITY_STATUS_MASK (1<<30)
#define MMC_OCR_CARD_POWER_UP_STATUS_MASK (1<<31)

typedef  unsigned long RSI_MMC_OCR_REGISTER;

/****************************************************/
/*  MMC CID Register                                */
/****************************************************/
typedef enum
{	MMC_CID_CBX_DEVICE_TYPE_CARD = 0,
	MMC_CID_CBX_DEVICE_TYPE_BGA = 1,
	MMC_CID_CBX_DEVICE_TYPE_RESERVED_0 = 2,
	MMC_CID_CBX_DEVICE_TYPE_RESERVED_1 = 3
} MMC_CID_CBX;
/****************************************************/
/*  MMC CID Register Structure                      */
/****************************************************/
typedef struct _RSI_MMC_CID_REGISTER
{
    unsigned char   mid;
    unsigned char   cbx;
    unsigned char   oid;
    char            pnm[6];
    unsigned char   prv;
    unsigned long   psn;
    unsigned short  mdt;
    unsigned char   crc;
} RSI_MMC_CID_REGISTER;

/****************************************************/
/*  MMC CID Register Access Macros                  */
/****************************************************/
#define ADI_MMC_CID_MID(x)       (((x) & 0xFF000000)>>24)
#define ADI_MMC_CID_CBX(x)       (((x) & 0x00030000)>>16)
#define ADI_MMC_CID_OID(x)       (((x) & 0x0000FF00)>>8)
#define ADI_MMC_CID_PNM0(x)      (((x) & 0x000000FF))
#define ADI_MMC_CID_PNM1(x)      (((x) & 0xFF000000)>>24)
#define ADI_MMC_CID_PNM2(x)      (((x) & 0x00FF0000)>>16)
#define ADI_MMC_CID_PNM3(x)      (((x) & 0x0000FF00)>>8)
#define ADI_MMC_CID_PNM4(x)      (((x) & 0x000000FF))
#define ADI_MMC_CID_PNM5(x)      (((x) & 0xFF000000)>>24)
#define ADI_MMC_CID_PRV(x)       (((x) & 0x00FF0000)>>16)
#define ADI_MMC_CID_PSN(x, y)    ((((x) & 0x0000FFFF)<<16) | (((y) & 0x7FFF8000)>>15))
#define ADI_MMC_CID_MDT(x)       (((x) & 0x00007F80)>>7)
#define ADI_MMC_CID_CRC(x)       (((x) & 0x0000007F))

/****************************************************/
/*  MMC CSD Register                                */
/****************************************************/
/****************************************************/
/*  MMC Card Status Register Enumerations           */
/****************************************************/
typedef enum
{
    MMC_CSD_STRUCTURE_VERSION_1_0           = 0,
    MMC_CSD_STRUCTURE_VERSION_1_1           = 1,
    MMC_CSD_STRUCTURE_VERSION_1_2           = 2,
    MMC_CSD_STRUCTURE_VERSION_IN_EXT_CSD    = 3
} MMC_CSD_STRUCTURE;

typedef enum
{	MMC_CSD_VERSION_ALLOCATED_BY_MMCA_0 = 0,
	MMC_CSD_VERSION_ALLOCATED_BY_MMCA_1 = 1,
	MMC_CSD_VERSION_ALLOCATED_BY_MMCA_2 = 2,
	MMC_CSD_VERSION_ALLOCATED_BY_MMCA_3 = 3,
	MMC_CSD_VERSION_4_1_TO_4_3 = 4,
	MMC_CSD_VERSION_RESERVED_0 = 5,
	MMC_CSD_VERSION_RESERVED_1 = 6,
	MMC_CSD_VERSION_RESERVED_2 = 7,
	MMC_CSD_VERSION_RESERVED_3 = 8,
	MMC_CSD_VERSION_RESERVED_4 = 9,
	MMC_CSD_VERSION_RESERVED_5 = 10,
	MMC_CSD_VERSION_RESERVED_6 = 11,
	MMC_CSD_VERSION_RESERVED_7 = 12,
	MMC_CSD_VERSION_RESERVED_8 = 13,
	MMC_CSD_VERSION_RESERVED_9 = 14,
	MMC_CSD_VERSION_RESERVED_10 = 15                
} MMC_CSD_SPEC_VERS;

typedef enum
{
    MMC_CSD_VDD_CURR_MIN_0_5mA   = 0,
    MMC_CSD_VDD_CURR_MIN_1_0mA   = 1,
    MMC_CSD_VDD_CURR_MIN_5_0mA   = 2,
    MMC_CSD_VDD_CURR_MIN_10_0mA  = 3,
    MMC_CSD_VDD_CURR_MIN_25_0mA  = 4,
    MMC_CSD_VDD_CURR_MIN_35_0mA  = 5,
    MMC_CSD_VDD_CURR_MIN_60_0mA  = 6,
    MMC_CSD_VDD_CURR_MIN_100_0mA = 7,
    MMC_CSD_VDD_CURR_MIN_NOT_APPLICABLE = 8
} MMC_CSD_VDD_CURR_MIN;

typedef enum
{
    MMC_CSD_VDD_CURR_MAX_1_0mA   = 0,
    MMC_CSD_VDD_CURR_MAX_5_0mA   = 1,
    MMC_CSD_VDD_CURR_MAX_10_0mA  = 2,
    MMC_CSD_VDD_CURR_MAX_25_0mA  = 3,
    MMC_CSD_VDD_CURR_MAX_35_0mA  = 4,
    MMC_CSD_VDD_CURR_MAX_45_0mA  = 5,
    MMC_CSD_VDD_CURR_MAX_80_0mA  = 6,
    MMC_CSD_VDD_CURR_MAX_200_0mA = 7,
    MMC_CSD_VDD_CURR_MAX_NOT_APPLICABLE = 8
} MMC_CSD_VDD_CURR_MAX;

typedef enum
{
    MMC_CSD_FILE_FORMAT_HD_WITH_PARTITION_TABLE          = 0,
    MMC_CSD_FILE_FORMAT_DOS_FAT_WITH_BOOT_SECTOR_ONLY    = 1,
    MMC_CSD_FILE_FORMAT_UNIVERSAL_FILE_FORMAT            = 2,
    MMC_CSD_FILE_FORMAT_OTHER_UNKNOWN                    = 3
} MMC_CSD_FILE_FORMAT;

typedef enum
{	MMC_CSD_ECC_NONE = 0,
	MMC_CSD_ECC_BCH_542_512 = 1,
	MMC_CSD_ECC_RESERVED_0 = 2,
	MMC_CSD_ECC_RESERVED_1 = 3
} MMC_CSD_ECC;

/********************************************************/
/*  MMC CSD Register Structure                          */
/********************************************************/
typedef struct _RSI_MMC_CSD_REGISTER
{
    MMC_CSD_STRUCTURE   csd_structure;
    MMC_CSD_SPEC_VERS   spec_vers;
    unsigned char       taac;
    unsigned char       nsac;
    unsigned char       tran_speed;
    unsigned short      ccc;
    unsigned char       read_bl_len;
    bool                read_bl_partial;
    bool                write_blk_misalign;
    bool                read_blk_misalign;
    bool                dsr_imp;
    unsigned short       c_size;
    MMC_CSD_VDD_CURR_MIN vdd_r_curr_min;
    MMC_CSD_VDD_CURR_MAX vdd_r_curr_max;
    MMC_CSD_VDD_CURR_MIN vdd_w_curr_min;
    MMC_CSD_VDD_CURR_MAX vdd_w_curr_max;
    unsigned short       c_size_mult;
    unsigned char        erase_grp_size;
    unsigned char        erase_grp_mult;
    unsigned char       wp_grp_size;
    bool                wp_grp_enable;
    MMC_CSD_ECC         default_ecc;
    unsigned char       r2w_factor;
    unsigned char       write_bl_length;
    bool                write_bl_partial;
    bool                content_prot_app;
    bool                file_format_grp;
    bool                copy;
    bool                perm_write_protect;
    bool                tmp_write_protect;
    MMC_CSD_FILE_FORMAT  file_format;
    MMC_CSD_ECC         ecc;
    unsigned char       crc;
} RSI_MMC_CSD_REGISTER;

/****************************************************/
/*  MMC Extended CSD Register                       */
/****************************************************/
/****************************************************/
/*  MMC Extended CSD Register Enumerations          */
/****************************************************/

/********************************************************/
/*  MMC EXTENDED CSD Register Structure                 */
/********************************************************/
typedef struct _RSI_MMC_EXTCSD_REGISTER
{
    unsigned char        s_cmd_set;
    unsigned char        boot_info;
    unsigned char        boot_size_mult;
    unsigned char        acc_size;
    unsigned char        hc_erase_grp_size;
    unsigned char        erase_timeout_mult;
    unsigned char        rel_wr_sec_c;
    unsigned char        hc_wp_grp_size;
    unsigned char        s_c_vcc;
    unsigned char        s_c_vccq;
    unsigned char        s_a_timeout;
    unsigned long        sec_count;
    unsigned char        min_perf_w_8_52;
    unsigned char        min_perf_r_8_52;
    unsigned char        min_perf_w_8_26_4_52;
    unsigned char        min_perf_r_8_26_4_52;
    unsigned char        min_perf_w_4_26;
    unsigned char        min_perf_r_4_26;
    unsigned char        pwr_cl_26_360;
    unsigned char        pwr_cl_52_360;
    unsigned char        pwr_cl_26_195;
    unsigned char        pwr_cl_52_195;
    unsigned char        card_type;
    unsigned char        csd_structure;
    unsigned char        ext_csd_rev;
    unsigned char        cmd_set;
    unsigned char        cmd_set_rev;
    unsigned char        power_class;
    unsigned char        hs_timing;
    unsigned char        bus_width;
    unsigned char        erased_mem_cont;
    unsigned char        boot_config;
    unsigned char        boot_bus_width;
    unsigned char        erase_group_def;
} RSI_MMC_EXTCSD_REGISTER;

/********************************************************/
/*  MMC RCA Register                                    */
/********************************************************/    
typedef unsigned short RSI_MMC_RCA_REGISTER;

/********************************************************/
/*  MMC DSR Register                                    */
/********************************************************/    
typedef unsigned short RSI_MMC_DSR_REGISTER;

/********************************************************/
/*  MMC Card Register Structure                         */
/********************************************************/
typedef struct _RSI_MMC_CARD_REGISTERS
{
    RSI_MMC_CSR_REGISTER csr;
    RSI_MMC_OCR_REGISTER ocr;
    RSI_MMC_CID_REGISTER cid;
    RSI_MMC_CSD_REGISTER csd;
    RSI_MMC_EXTCSD_REGISTER extcsd;
    RSI_MMC_RCA_REGISTER rca;
    RSI_MMC_DSR_REGISTER dsr;
} RSI_MMC_CARD_REGISTERS;

#endif /* __MMC_CARD_REGISTERS_H__ */

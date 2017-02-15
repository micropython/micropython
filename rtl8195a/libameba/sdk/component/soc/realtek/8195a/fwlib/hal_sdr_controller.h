/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _HAL_SDR_CONTROLLER_H_
#define _HAL_SDR_CONTROLLER_H_

typedef enum _DRAM_TYPE_ {
  DRAM_DDR_1 = 1,
  DRAM_DDR_2 = 2,
  DRAM_DDR_3 = 3,
  DRAM_DDR_4 = 4,
  DRAM_SDR   = 8 
}DRAM_TYPE;

typedef enum _DRAM_COLADDR_WTH_ {
  DRAM_COLADDR_8B   = 0,
  DRAM_COLADDR_9B   = 1,
  DRAM_COLADDR_10B  = 2,
  DRAM_COLADDR_11B  = 3,
  DRAM_COLADDR_12B  = 4,
  DRAM_COLADDR_13B  = 5,
  DRAM_COLADDR_14B  = 6,
  DRAM_COLADDR_15B  = 7,
  DRAM_COLADDR_16B  = 8 
}DRAM_COLADDR_WTH;

typedef enum _DRAM_BANK_SIZE_ {
  DRAM_BANK_2       = 0,
  DRAM_BANK_4       = 1,
  DRAM_BANK_8       = 2 
}DRAM_BANK_SIZE;

typedef enum _DRAM_DQ_WIDTH_ {
  DRAM_DQ_16        = 0,
  DRAM_DQ_32        = 1, 
  DRAM_HALF_DQ32    = 2 
}DRAM_DQ_WIDTH;

typedef enum _MODE0_BST_LEN_ {
  BST_LEN_4   = 0,
  BST_LEN_FLY = 1,
  BST_LEN_8   = 2 
}MODE0_BST_LEN;

typedef enum _MODE0_BST_TYPE_ {
  SENQUENTIAL = 0,
  INTERLEAVE  = 1 
}MODE0_BST_TYPE;   

typedef enum _DFI_RATIO_TYPE_ {
  DFI_RATIO_1 = 0,  // DFI= 1:1, or SDR
  DFI_RATIO_2 = 1, 
  DFI_RATIO_4 = 2  
}DFI_RATIO_TYPE;   

typedef struct _DRAM_INFO_ {
  DRAM_TYPE         DeviceType;   
  DRAM_COLADDR_WTH  ColAddrWth;   
  DRAM_BANK_SIZE    Bank;   
  DRAM_DQ_WIDTH     DqWidth;
}DRAM_INFO;

typedef struct _DRAM_MODE_REG_INFO_ {
  MODE0_BST_LEN     BstLen;
  MODE0_BST_TYPE    BstType;
  //enum mode0_cas         rd_cas;
  u32 Mode0Cas;
  u32 Mode0Wr;
  u32 Mode1DllEnN;
  u32 Mode1AllLat;
  u32 Mode2Cwl;
}DRAM_MODE_REG_INFO;

typedef struct _DRAM_TIMING_INFO_ {
  u32  TrfcPs;
  u32  TrefiPs;
  u32  WrMaxTck;
  u32  TrcdPs; 
  u32  TrpPs;
  u32  TrasPs;
  u32  TrrdTck;
  u32  TwrPs;
  u32  TwtrTck; 
  //u32  TrtpPs;
  u32  TmrdTck;
  u32  TrtpTck;
  u32  TccdTck;
  u32  TrcPs;
}DRAM_TIMING_INFO;


typedef struct _DRAM_DEVICE_INFO_ {
  DRAM_INFO          *Dev;
  DRAM_MODE_REG_INFO *ModeReg;
  DRAM_TIMING_INFO   *Timing;
  u32                DdrPeriodPs;
  DFI_RATIO_TYPE     *DfiRate; 
}DRAM_DEVICE_INFO;


//======================================================
//DRAM Info
#ifdef CONFIG_FPGA
    #define DRAM_INFO_TYPE          DRAM_SDR
    #define DRAM_INFO_COL_ADDR_WTH  DRAM_COLADDR_9B
    #define DRAM_INFO_BANK_SZ       DRAM_BANK_4
    #define DRAM_INFO_DQ_WTH        DRAM_DQ_16
#else
    #define DRAM_INFO_TYPE          DRAM_SDR
    #define DRAM_INFO_COL_ADDR_WTH  DRAM_COLADDR_8B
    #define DRAM_INFO_BANK_SZ       DRAM_BANK_2
    #define DRAM_INFO_DQ_WTH        DRAM_DQ_16
#endif

//======================================================
//DRAM Timing 
#ifdef CONFIG_SDR_100MHZ
#define DRAM_TIMING_TCK         10000       //ps
#endif
#ifdef CONFIG_SDR_50MHZ
#define DRAM_TIMING_TCK         20000       //ps
#endif
#ifdef CONFIG_SDR_25MHZ
#define DRAM_TIMING_TCK         40000       //ps
#endif
#ifdef CONFIG_SDR_12_5MHZ
#define DRAM_TIMING_TCK         80000       //ps
#endif

#if 1
#define DRAM_TIMING_TREF        64000       //us
#define DRAM_ROW_NUM            8192        //depends on row bit number

#define DRAM_TIMING_TRFC        60000       //ps
#define DRAM_TIMING_TREFI       ((u32)((DRAM_TIMING_TREF*1000)/DRAM_ROW_NUM)*1000)  //ps
#define DRAM_TIMING_TWRMAXTCK   2           //tck
#define DRAM_TIMING_TRCD        15000       //ps
#define DRAM_TIMING_TRP         15000       //ps
#define DRAM_TIMING_TRAS        42000       //ps
#define DRAM_TIMING_TRRD        2           //tck
#define DRAM_TIMING_TWR         ((u32)(DRAM_TIMING_TCK*2))
#define DRAM_TIMING_TWTR        0           //tck
#define DRAM_TIMING_TMRD        2           //tck
#define DRAM_TIMING_TRTP        0           //tck
#define DRAM_TIMING_TCCD        1           //tck
#define DRAM_TIMING_TRC         60000       //ps    
#else

#define DRAM_TIMING_TREF        66000       //us
#define DRAM_ROW_NUM            8192        //depends on row bit number

#define DRAM_TIMING_TRFC        66000       //ps
#define DRAM_TIMING_TREFI       63999800
#define DRAM_TIMING_TWRMAXTCK   2           //tck
#define DRAM_TIMING_TRCD        15000       //ps
#define DRAM_TIMING_TRP         15000       //ps
#define DRAM_TIMING_TRAS        37000       //ps
#define DRAM_TIMING_TRRD        2           //tck
#define DRAM_TIMING_TWR         7000
#define DRAM_TIMING_TWTR        0           //tck
#define DRAM_TIMING_TMRD        2           //tck
#define DRAM_TIMING_TRTP        0           //tck
#define DRAM_TIMING_TCCD        1           //tck
#define DRAM_TIMING_TRC         60000       //ps
#endif

#define HAL_SDR_WRITE32(addr, value32)  HAL_WRITE32(SDR_CTRL_BASE, addr, value32)
#define HAL_SDR_WRITE16(addr, value16)  HAL_WRITE16(SDR_CTRL_BASE, addr, value16)
#define HAL_SDR_WRITE8(addr, value8)    HAL_WRITE8(SDR_CTRL_BASE, addr, value8)
#define HAL_SDR_READ32(addr)            HAL_READ32(SDR_CTRL_BASE, addr)
#define HAL_SDR_READ16(addr)            HAL_READ16(SDR_CTRL_BASE, addr)
#define HAL_SDR_READ8(addr)             HAL_READ8(SDR_CTRL_BASE, addr)

#define HAL_SDRAM_WRITE32(addr, value32)  HAL_WRITE32(SDR_SDRAM_BASE, addr, value32)
#define HAL_SDRAM_WRITE16(addr, value16)  HAL_WRITE16(SDR_SDRAM_BASE, addr, value16)
#define HAL_SDRAM_WRITE8(addr, value8)    HAL_WRITE8(SDR_SDRAM_BASE, addr, value8)
#define HAL_SDRAM_READ32(addr)            HAL_READ32(SDR_SDRAM_BASE, addr)
#define HAL_SDRAM_READ16(addr)            HAL_READ16(SDR_SDRAM_BASE, addr)
#define HAL_SDRAM_READ8(addr)             HAL_READ8(SDR_SDRAM_BASE, addr)


#endif  // end of "#ifndef _HAL_SDR_CONTROLLER_H_"

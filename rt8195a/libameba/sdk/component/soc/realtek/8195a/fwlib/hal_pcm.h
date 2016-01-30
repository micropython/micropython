/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _HAL_PCM_H_
#define _HAL_PCM_H_

#include "rtl8195a_pcm.h"
/*
typedef struct _GDMA_CH_LLI_ELE_ {
    u32                   Sarx;
    u32                   Darx;
    u32                   Llpx;
    u32                   CtlxLow;
    u32                   CtlxUp;
    u32                   Temp;
}GDMA_CH_LLI_ELE, *PGDMA_CH_LLI_ELE;
#if 1
#if 0
typedef struct _GDMA_CH_LLI_ {
    PGDMA_CH_LLI_ELE      pLliEle;
    PGDMA_CH_LLI          pNextLli;
}GDMA_CH_LLI, *PGDMA_CH_LLI;

typedef struct _BLOCK_SIZE_LIST_ {
    u32                  BlockSize;
    PBLOCK_SIZE_LIST     pNextBlockSiz;
}BLOCK_SIZE_LIST, *PBLOCK_SIZE_LIST;
#else
struct GDMA_CH_LLI {
    PGDMA_CH_LLI_ELE                pLliEle;
    struct GDMA_CH_LLI             *pNextLli;
};

struct BLOCK_SIZE_LIST {
    u32                             BlockSize;
    struct BLOCK_SIZE_LIST          *pNextBlockSiz;
};

#endif

#endif
typedef struct _HAL_GDMA_ADAPTER_ {
    u32                   ChSar;
    u32                   ChDar;
    GDMA_CHANNEL_NUM      ChEn;
    GDMA_CTL_REG          GdmaCtl;
    GDMA_CFG_REG          GdmaCfg;
    u32                   PacketLen;
    u32                   BlockLen;
    u32                   MuliBlockCunt;
    u32                   MaxMuliBlock;
    struct GDMA_CH_LLI          *pLlix;
    struct BLOCK_SIZE_LIST      *pBlockSizeList;

    PGDMA_CH_LLI_ELE            pLli;
    u32                         NextPlli;
    u8                    TestItem;
    u8                          ChNum;
    u8                          GdmaIndex;
    u8                          IsrCtrl:1;
    u8                          GdmaOnOff:1;
    u8                          Llpctrl:1;
    u8                          Lli0:1;
    u8                          Rsvd4to7:4;
    u8                          GdmaIsrType;
}HAL_GDMA_ADAPTER, *PHAL_GDMA_ADAPTER;

*/

typedef struct _HAL_PCM_ADAPTER_ {
    u32                       Enable:1;
    PCM_CTL_REG               PcmCtl;
    PCM_CHCNR03_REG           PcmChCNR03;
    PCM_TSR03_REG             PcmTSR03;
    PCM_BSIZE03_REG           PcmBSize03;
    u32                       abc;
    u8                        PcmIndex;
    u8                        PcmCh;
}HAL_PCM_ADAPTER, *PHAL_PCM_ADAPTER;


typedef struct _HAL_PCM_OP_ {
    VOID (*HalPcmOnOff)(VOID *Data);
    BOOL (*HalPcmInit)(VOID *Data);
    BOOL (*HalPcmSetting)(VOID *Data);
    BOOL (*HalPcmEn)(VOID *Data);
    BOOL (*HalPcmIsrEnAndDis) (VOID *Data);
    BOOL (*HalPcmDumpReg)(VOID *Data);
    BOOL (*HalPcm)(VOID *Data);
}HAL_PCM_OP, *PHAL_PCM_OP;


VOID HalPcmOpInit(
    IN  VOID *Data
);


#endif

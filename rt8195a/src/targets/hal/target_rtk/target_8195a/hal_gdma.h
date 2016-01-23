/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _HAL_GDMA_H_
#define _HAL_GDMA_H_

#include "rtl8195a_gdma.h"

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


typedef struct _HAL_GDMA_OP_ {
    VOID (*HalGdmaOnOff)(VOID *Data);
    BOOL (*HalGdamChInit)(VOID *Data);
    BOOL (*HalGdmaChSeting)(VOID *Data);
    BOOL (*HalGdmaChBlockSeting)(VOID *Data);
    VOID (*HalGdmaChDis)(VOID *Data);
    VOID (*HalGdmaChEn)(VOID *Data);
    VOID (*HalGdmaChIsrEnAndDis) (VOID *Data);
    u8   (*HalGdmaChIsrClean)(VOID *Data);
    VOID (*HalGdmaChCleanAutoSrc)(VOID *Data);
    VOID (*HalGdmaChCleanAutoDst)(VOID *Data);
}HAL_GDMA_OP, *PHAL_GDMA_OP;


VOID HalGdmaOpInit(
    IN  VOID *Data
);

extern const HAL_GDMA_OP _HalGdmaOp;

#endif

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

typedef struct _HAL_GDMA_CHNL_ {
    u8 GdmaIndx;
    u8 GdmaChnl;
    u8 IrqNum;
    u8 Reserved;
}HAL_GDMA_CHNL, *PHAL_GDMA_CHNL;

typedef struct _HAL_GDMA_BLOCK_ {
    u32 SrcAddr;
    u32 DstAddr;
    u32 BlockLength;
    u32 SrcOffset;
    u32 DstOffset;
}HAL_GDMA_BLOCK, *PHAL_GDMA_BLOCK;

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

typedef struct _HAL_GDMA_OBJ_ {
    HAL_GDMA_ADAPTER HalGdmaAdapter;
    IRQ_HANDLE GdmaIrqHandle;
    volatile GDMA_CH_LLI_ELE GdmaChLli[16];
    struct GDMA_CH_LLI Lli[16];
    struct BLOCK_SIZE_LIST BlockSizeList[16];
    u8 Busy;      // is transfering
    u8 BlockNum;
} HAL_GDMA_OBJ, *PHAL_GDMA_OBJ;

VOID HalGdmaOpInit(IN  VOID *Data);
VOID HalGdmaOn(PHAL_GDMA_ADAPTER pHalGdmaAdapter);
VOID HalGdmaOff(PHAL_GDMA_ADAPTER pHalGdmaAdapter);
BOOL HalGdmaChInit(PHAL_GDMA_ADAPTER pHalGdmaAdapter);
VOID HalGdmaChDis(PHAL_GDMA_ADAPTER pHalGdmaAdapter);
VOID HalGdmaChEn(PHAL_GDMA_ADAPTER pHalGdmaAdapter);
BOOL HalGdmaChSeting(PHAL_GDMA_ADAPTER pHalGdmaAdapter);
BOOL HalGdmaChBlockSeting(PHAL_GDMA_ADAPTER pHalGdmaAdapter);
VOID HalGdmaChIsrEn(PHAL_GDMA_ADAPTER pHalGdmaAdapter);
VOID HalGdmaChIsrDis(PHAL_GDMA_ADAPTER pHalGdmaAdapter);
u8 HalGdmaChIsrClean(PHAL_GDMA_ADAPTER pHalGdmaAdapter);
VOID HalGdmaChCleanAutoSrc(PHAL_GDMA_ADAPTER pHalGdmaAdapter);
VOID HalGdmaChCleanAutoDst(PHAL_GDMA_ADAPTER pHalGdmaAdapter);

extern HAL_Status HalGdmaChnlRegister (u8 GdmaIdx, u8 ChnlNum);
extern VOID HalGdmaChnlUnRegister (u8 GdmaIdx, u8 ChnlNum);
extern PHAL_GDMA_CHNL HalGdmaChnlAlloc (HAL_GDMA_CHNL *pChnlOption);
extern VOID HalGdmaChnlFree (HAL_GDMA_CHNL *pChnl);
extern BOOL HalGdmaMemCpyInit(PHAL_GDMA_OBJ pHalGdmaObj);
extern VOID HalGdmaMemCpyDeInit(PHAL_GDMA_OBJ pHalGdmaObj);
extern VOID* HalGdmaMemCpy(PHAL_GDMA_OBJ pHalGdmaObj, void* pDest, void* pSrc, u32 len);
extern VOID HalGdmaMemAggr(PHAL_GDMA_OBJ pHalGdmaObj, PHAL_GDMA_BLOCK pHalGdmaBlock);
extern BOOL HalGdmaMemCpyAggrInit(PHAL_GDMA_OBJ pHalGdmaObj);

extern const HAL_GDMA_OP _HalGdmaOp;
extern const HAL_GDMA_CHNL GDMA_Chnl_Option[];
extern const HAL_GDMA_CHNL GDMA_Multi_Block_Chnl_Option[];
extern const u16 HalGdmaChnlEn[6];

#endif

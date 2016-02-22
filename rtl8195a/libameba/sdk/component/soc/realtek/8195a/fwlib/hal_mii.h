/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _HAL_MII_H_
#define _HAL_MII_H_

#include "rtl8195a_mii.h"


/**
 * LOG Configurations
 */

#define NOLOG

#define LOG_TAG           "NoTag"
#define LOG_INFO_HEADER   "I"
#define LOG_DEBUG_HEADER  "D"
#define LOG_ERROR_HEADER  "E"
#define LOG_TEST_HEADER   "T"

#define IDENT_TWO_SPACE    "  "
#define IDENT_FOUR_SPACE   "    "

#define LOG_INFO(...)  do {\
            DiagPrintf("\r"LOG_INFO_HEADER"/"LOG_TAG": " __VA_ARGS__);\
}while(0)

#define LOG_DEBUG(...)  do {\
            DiagPrintf("\r"LOG_DEBUG_HEADER"/"LOG_TAG": " __VA_ARGS__);\
}while(0)

#define LOG_ERROR(...)  do {\
            DiagPrintf("\r"LOG_ERROR_HEADER"/"LOG_TAG": " __VA_ARGS__);\
}while(0)

#ifdef NOLOG
    #define LOGI
    #define LOGD
    #define LOGE
    #define LOGI2
    #define LOGD2
    #define LOGE2
    #define LOGI4
    #define LOGD4
    #define LOGE4
#else
    #define LOGI  LOG_INFO
    #define LOGD  LOG_DEBUG
    #define LOGE  LOG_ERROR
    #define LOGI2(...) LOG_INFO(IDENT_TWO_SPACE __VA_ARGS__)
    #define LOGD2(...) LOG_DEBUG(IDENT_TWO_SPACE __VA_ARGS__)
    #define LOGE2(...) LOG_ERROR(IDENT_TWO_SPACE __VA_ARGS__)
    #define LOGI4(...) LOG_INFO(IDENT_FOUR_SPACE __VA_ARGS__)
    #define LOGD4(...) LOG_DEBUG(IDENT_FOUR_SPACE __VA_ARGS__)
    #define LOGE4(...) LOG_ERROR(IDENT_FOUR_SPACE __VA_ARGS__)
#endif

#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define DBG_ENTRANCE LOGI(ANSI_COLOR_GREEN "=> %s() <%s>\n" ANSI_COLOR_RESET, \
        __func__, __FILE__)


// GMAC MII Configurations
#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG  "MII"
#endif


typedef struct _HAL_MII_ADAPTER_ {
    u32 InterruptMask;
    PPHY_MODE_INFO pPhyModeInfo;
}HAL_MII_ADAPTER, *PHAL_MII_ADAPTER;

typedef struct _HAL_MII_OP_ {
    BOOL (*HalMiiGmacInit)(VOID *Data);
    BOOL (*HalMiiInit)(VOID *Data);
    BOOL (*HalMiiGmacReset)(VOID *Data);
    BOOL (*HalMiiGmacEnablePhyMode)(VOID *Data);
    u32  (*HalMiiGmacXmit)(VOID *Data);
    VOID (*HalMiiGmacCleanTxRing)(VOID *Data);
    VOID (*HalMiiGmacFillTxInfo)(VOID *Data);
    VOID (*HalMiiGmacFillRxInfo)(VOID *Data);
    VOID (*HalMiiGmacTx)(VOID *Data);
    VOID (*HalMiiGmacRx)(VOID *Data);
    VOID (*HalMiiGmacSetDefaultEthIoCmd)(VOID *Data);
    VOID (*HalMiiGmacInitIrq)(VOID *Data);
    u32  (*HalMiiGmacGetInterruptStatus)(VOID);
    VOID (*HalMiiGmacClearInterruptStatus)(u32 IsrStatus);
}HAL_MII_OP, *PHAL_MII_OP;

VOID HalMiiOpInit(IN VOID *Data);

typedef struct _MII_ADAPTER_ {
    PHAL_MII_OP      pHalMiiOp;
    PHAL_MII_ADAPTER pHalMiiAdapter;
    PTX_INFO         pTx_Info;
    PRX_INFO         pRx_Info;
    VOID*            TxBuffer;
    VOID*            RxBuffer;
}MII_ADAPTER, *PMII_ADAPTER;

#endif


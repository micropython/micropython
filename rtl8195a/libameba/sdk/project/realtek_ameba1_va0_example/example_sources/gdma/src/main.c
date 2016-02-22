/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "device.h"
#include "diag.h"
#include "main.h"

#include "dma_api.h"

#if 1
//Multi-Block Example Demo
#define DMA_CPY_LEN     176
#define DMA_BLOCK_LENGTH 22
#define DMA_SRC_OFFSET  0
#define DMA_DST_OFFSET  0
#define BLOCK_NUM 8

gdma_t gdma;
uint8_t TestBuf1[DMA_CPY_LEN];
uint8_t TestBuf2[DMA_CPY_LEN];
volatile uint8_t dma_done;

struct BlockInfo{
    u32 SrcAddr;
    u32 DstAddr;
    u32 BlockLength;
    u32 SrcOffset;
    u32 DstOffset;
};

void dma_done_handler(uint32_t id) {
    DiagPrintf("DMA Copy Done!!\r\n");
    dma_done = 1;
}


int main(void) {
    int i = 0,err = 0;
    struct BlockInfo block_info[BLOCK_NUM];
    //Set how many blocks we want to transfer (16 at most)
    gdma.gdma_obj.BlockNum = BLOCK_NUM;
    //Initialize DMA multi-block mode setting
    dma_memcpy_aggr_init(&gdma, dma_done_handler, (uint32_t) &gdma);

    _memset(TestBuf1, 0,DMA_CPY_LEN);

    for(i = 0; i < DMA_CPY_LEN; i++){
        TestBuf1[i] = DMA_CPY_LEN - 1 - i;
    }

    _memset(TestBuf2, 0,DMA_CPY_LEN);
    dma_done = 0;

    for(i = 0; i < BLOCK_NUM; i++){
        //User can decide the relation between SrcOffset/DstOffset,SrcAddr/DstAddr and Block length
        // For example :
        //block_info[i].SrcOffset = 0;
        //block_info[i].DstOffset = 4;   
        //block_info[i].SrcAddr = &TestBuf1[ i * DMA_BLOCK_LENGTH] ;//SRC
        //block_info[i].DstAddr = &TestBuf2[0]  + (DMA_BLOCK_LENGTH + block_info[i].DstOffset )*i;//Dest
        //block_info[i].BlockLength = DMA_BLOCK_LENGTH;
          
        block_info[i].SrcOffset = 0;
        block_info[i].DstOffset = 0;   
        block_info[i].SrcAddr = (uint32_t) &TestBuf1[ i * DMA_BLOCK_LENGTH] ;//SRC
        block_info[i].DstAddr = (uint32_t) &TestBuf2[ i * DMA_BLOCK_LENGTH] ;//Dest
        block_info[i].BlockLength = DMA_BLOCK_LENGTH;
        //DiagPrintf("block_info[%d].SrcAddr = %x\r\n",i, block_info[i].SrcAddr);
        //DiagPrintf("block_info[%d].DstAddr = %x\r\n",i, block_info[i].DstAddr);
        //DiagPrintf("block_info[%d].BlockLength = %x\r\n",i, block_info[i].BlockLength);
        //DiagPrintf("block_info[%d].SrcOffset = %x\r\n",i, block_info[i].SrcOffset);
        //DiagPrintf("block_info[%d].DstOffset = %x\r\n",i, block_info[i].DstOffset);
    }


    dma_memcpy_aggr(&gdma, (PHAL_GDMA_BLOCK) &block_info);

    while (dma_done == 0);

    err = 0;
    for (i=0;i<DMA_CPY_LEN;i++) {
        //DiagPrintf("dma_done = %x\r\n", dma_done);
        //DiagPrintf("TestBuf2[%d] = %x\r\n",i, TestBuf2[i]);
        if (TestBuf2[i+DMA_DST_OFFSET] != TestBuf1[i+DMA_SRC_OFFSET]) {
            DiagPrintf("DMA Copy Memory Compare Err, %d %x %x\r\n", i, TestBuf1[i+DMA_SRC_OFFSET], TestBuf2[i+DMA_DST_OFFSET]);
            DiagPrintf("DMA done =  %x\r\n", dma_done);
            err = 1;
            break;
        }
    }

    if (!err) {
        DiagPrintf("DMA Copy Memory Compare OK!! %x\r\n", TestBuf2[DMA_DST_OFFSET+DMA_CPY_LEN - 1]);
    }
    
    HalGdmaMemCpyDeInit(&(gdma.gdma_obj));

    while(1);
    return 0;
}
#else
//Single-Block Example Demo

#define DMA_CPY_LEN     256
#define DMA_SRC_OFFSET  0
#define DMA_DST_OFFSET  0

gdma_t gdma;
uint8_t TestBuf1[512];
uint8_t TestBuf2[512];
volatile uint8_t dma_done;


void dma_done_handler(uint32_t id) {
    DiagPrintf("DMA Copy Done!!\r\n");
    dma_done = 1;
}


int main(void) {
    int i;
    int err;

    dma_memcpy_init(&gdma, dma_done_handler, (uint32_t)&gdma);
    for (i=0;i< 512;i++) {
        TestBuf1[i] = i;
    }
    _memset(TestBuf2, 0xff, 512);

    dma_done = 0;
    dma_memcpy(&gdma, TestBuf2+DMA_DST_OFFSET, TestBuf1+DMA_SRC_OFFSET, DMA_CPY_LEN);

    while (dma_done == 0);

    err = 0;
    for (i=0;i<DMA_CPY_LEN;i++) {
        if (TestBuf2[i+DMA_DST_OFFSET] != TestBuf1[i+DMA_SRC_OFFSET]) {
            DiagPrintf("DMA Copy Memory Compare Err, %d %x %x\r\n", i, TestBuf1[i+DMA_SRC_OFFSET], TestBuf2[i+DMA_DST_OFFSET]);
            err = 1;
            break;
        }
    }
    
    if (!err) {
        DiagPrintf("DMA Copy Memory Compare OK!! %x\r\n", TestBuf2[DMA_DST_OFFSET+DMA_CPY_LEN]);
    }
    HalGdmaMemCpyDeInit(&(gdma.gdma_obj));
   
    while(1);
    return 0;
}
#endif

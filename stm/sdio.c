// TODO
// make it work with DMA

#include <stdio.h>
//#include "stm32f4xx_sdio.h"
#include "stm324x7i_eval_sdio_sd.h"
#include "misc.h"
#include "systick.h"

#define BLOCK_SIZE            512 /* Block Size in Bytes */

uint8_t aBuffer_Block_Rx[BLOCK_SIZE];

void sdio_init(void) {
    SD_Error error = SD_Init();
    printf("Init: %x\n", error);
    uint8_t det = SD_Detect();
    printf("Detc: %x\n", det);

    if (!det) {
        printf("no card detected\n");
        SD_PowerOFF();
        SD_DeInit();
        return;
    }

    // read a block!
    error = SD_ReadBlock(aBuffer_Block_Rx, 512, BLOCK_SIZE);
    printf("ReadBlock: %d\n", error);

    /*
    // Check if the Transfer is finished
    error = SD_WaitReadOperation();
    printf("WaitReadOp: %d\n", error);
    */

    uint32_t stc = sys_tick_counter;
    while (SD_GetStatus() != SD_TRANSFER_OK) {
        if (sys_tick_has_passed(stc, 2000)) {
            printf("timeout waiting for read to finish\n");
            break;
        }
    }
    printf("done!!\n");

    printf("%.16s", aBuffer_Block_Rx);

    /*
    snprintf((char*)aBuffer_Block_Rx, BLOCK_SIZE, "Here is some data back for you!\nBLOCK_SIZE=%d\n", BLOCK_SIZE);
    error = SD_WriteBlock(aBuffer_Block_Rx, 512, BLOCK_SIZE);
    printf("WriteBlock: %d\n", error);

    while (SD_GetStatus() != SD_TRANSFER_OK) {
    }
    printf("done writing!\n");
    */

    SD_PowerOFF();
    SD_DeInit();
}

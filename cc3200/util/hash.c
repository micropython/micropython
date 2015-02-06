#include "std.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "inc/hw_shamd5.h"
#include "inc/hw_dthe.h"
#include "hw_memmap.h"
#include "rom_map.h"
#include "prcm.h"
#include "shamd5.h"
#include "hash.h"

#ifdef USE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#endif


#ifdef USE_FREERTOS
static SemaphoreHandle_t xShamd5Semaphore = NULL;
#endif

void HASH_Init (void) {
    // Enable the Data Hashing and Transform Engine
    MAP_PRCMPeripheralClkEnable(PRCM_DTHE, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
#ifdef USE_FREERTOS
    vSemaphoreCreateBinary(xShamd5Semaphore);
#endif
}



void HASH_SHAMD5Start (uint32_t algo, uint32_t blocklen) {

#ifdef USE_FREERTOS
    xSemaphoreTake (xShamd5Semaphore, portMAX_DELAY);
#endif

    MAP_PRCMPeripheralReset(PRCM_DTHE);

    // wait until the context is ready
    while ((HWREG(SHAMD5_BASE + SHAMD5_O_IRQSTATUS) & SHAMD5_INT_CONTEXT_READY) == 0);

    // Configure the SHA/MD5 module algorithm
    MAP_SHAMD5ConfigSet(SHAMD5_BASE, algo);

    // if not a multiple of 64 bytes, close the hash
    if (blocklen % 64) {
        HWREG(SHAMD5_BASE + SHAMD5_O_MODE) |= SHAMD5_MODE_CLOSE_HASH;
    }

    // set the lenght
    HWREG(SHAMD5_BASE + SHAMD5_O_LENGTH) = blocklen;
}

void HASH_SHAMD5Update (uint8_t *data, uint32_t datalen) {
    // write the data
    SHAMD5DataWriteMultiple(data, datalen);
}

void HASH_SHAMD5Read (uint8_t *hash) {
    // wait for the output to be ready.
    while((HWREG(SHAMD5_BASE + SHAMD5_O_IRQSTATUS) & SHAMD5_INT_OUTPUT_READY) == 0);
    // read the result.
    MAP_SHAMD5ResultRead(SHAMD5_BASE, hash);
#ifdef USE_FREERTOS
    xSemaphoreGive (xShamd5Semaphore);
#endif
}

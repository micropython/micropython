/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "cmsis_os.h"

#include "diag.h"
#include "main.h"

#include "nfc_api.h"
#include "flash_api.h"

#define NFC_RESTORE_DEFAULT (0)

#define NFC_MAX_PAGE_NUM 36
nfctag_t nfctag;
unsigned int  nfc_tag_content[NFC_MAX_PAGE_NUM];
unsigned char nfc_tag_dirty[NFC_MAX_PAGE_NUM];

#define RTK_NFC_UID 0x58
unsigned char nfc_default_uid[7] = {
    RTK_NFC_UID, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06
};

osThreadId nfc_tid = 0;

#define FLASH_APP_NFC_BASE   0x85000
flash_t flash_nfc;

void nfc_event_listener(void *arg, unsigned int event) {
    switch(event) {
        case NFC_EV_READER_PRESENT:
            DiagPrintf("NFC_EV_READER_PRESENT\r\n");
            break;
        case NFC_EV_READ:
            DiagPrintf("NFC_EV_READ\r\n");
            break;
        case NFC_EV_WRITE:
            DiagPrintf("NFC_EV_WRITE\r\n");
            break;
        case NFC_EV_ERR:
            DiagPrintf("NFC_EV_ERR\r\n");
            break;
        case NFC_EV_CACHE_READ:
            DiagPrintf("NFC_EV_CACHE_READ\r\n");
            break;
    }
}

/**
 *  This callback function is called several times if tag is being written multiple pages.
 *  DO NOT put heavy task here otherwise it will block tag write and cause timeout failure.
 **/
void nfc_write_listener(void *arg, unsigned int page, unsigned int pgdat) {
    nfc_tag_content[page] = pgdat;
    nfc_tag_dirty[page] = 1;
    if (nfc_tid) {
        osSignalSet(nfc_tid, NFC_EV_WRITE);
    }
}

int is_valid_nfc_uid() {
    int valid_content = 1;

    unsigned char uid[7];
    unsigned char bcc[2];

    uid[0] = (unsigned char)((nfc_tag_content[0] & 0x000000FF) >>  0);
    uid[1] = (unsigned char)((nfc_tag_content[0] & 0x0000FF00) >>  8);
    uid[2] = (unsigned char)((nfc_tag_content[0] & 0x00FF0000) >> 16);
    bcc[0] = (unsigned char)((nfc_tag_content[0] & 0xFF000000) >> 24);
    uid[3] = (unsigned char)((nfc_tag_content[1] & 0x000000FF) >>  0);
    uid[4] = (unsigned char)((nfc_tag_content[1] & 0x0000FF00) >>  8);
    uid[5] = (unsigned char)((nfc_tag_content[1] & 0x00FF0000) >> 16);
    uid[6] = (unsigned char)((nfc_tag_content[1] & 0xFF000000) >> 24);
    bcc[1] = (unsigned char)((nfc_tag_content[2] & 0x000000FF) >>  0);

    // verify Block Check Character
    if (bcc[0] != (0x88 ^ uid[0] ^ uid[1] ^ uid[2])) {
        valid_content = 0;
    }
    if (bcc[1] != (uid[3] ^ uid[4] ^ uid[5] ^ uid[6])) {
        valid_content = 0;
    }

    return valid_content;
}

unsigned int generate_default_tag_content() {
    unsigned int page_size = 0;

    memset(nfc_tag_content, 0, NFC_MAX_PAGE_NUM * sizeof(unsigned int));

    // calculate Block Check Character
    unsigned char bcc[2];
    bcc[0] = 0x88 ^ nfc_default_uid[0] ^ nfc_default_uid[1] ^ nfc_default_uid[2];
    bcc[1] = nfc_default_uid[3] ^ nfc_default_uid[4] ^ nfc_default_uid[5] ^ nfc_default_uid[6];

    // generate header
    nfc_tag_content[page_size++] = ((unsigned int)nfc_default_uid[0]) <<  0 |
                                   ((unsigned int)nfc_default_uid[1]) <<  8 |
                                   ((unsigned int)nfc_default_uid[2]) << 16 |
                                   ((unsigned int)            bcc[0]) << 24;
    nfc_tag_content[page_size++] = ((unsigned int)nfc_default_uid[3]) <<  0 |
                                   ((unsigned int)nfc_default_uid[4]) <<  8 |
                                   ((unsigned int)nfc_default_uid[5]) << 16 |
                                   ((unsigned int)nfc_default_uid[6]) << 24;
    nfc_tag_content[page_size++] = ((unsigned int)            bcc[1]) <<  0;
    nfc_tag_content[page_size++] = 0x001211E1;

    // Init tag content as NDEF will-known text message "HELLO WORLD!" in little endian
    nfc_tag_content[page_size++] = 0x01d11303;
    nfc_tag_content[page_size++] = 0x6502540f;
    nfc_tag_content[page_size++] = 0x4c45486e;
    nfc_tag_content[page_size++] = 0x57204f4c;
    nfc_tag_content[page_size++] = 0x444c524f;
    nfc_tag_content[page_size++] = 0x0000fe21;

    return page_size;
}

void nfc_load_tag_content_from_flash() {
    int i, address, page_size;

    memset(nfc_tag_content, 0, NFC_MAX_PAGE_NUM * sizeof(unsigned int));
    memset(nfc_tag_dirty, 0, NFC_MAX_PAGE_NUM);

    for (i = 0, address = FLASH_APP_NFC_BASE; i < NFC_MAX_PAGE_NUM; i++, address+=4) {
        flash_read_word(&flash_nfc, address, &nfc_tag_content[i]);
    }

    if (!is_valid_nfc_uid() || NFC_RESTORE_DEFAULT) {
        DiagPrintf("Invalid tag content, restore to default value\r\n");
        page_size = generate_default_tag_content();

        // update to flash
        flash_erase_sector(&flash_nfc, FLASH_APP_NFC_BASE);
        for (i = 0, address = FLASH_APP_NFC_BASE; i < page_size; i++, address += 4) {
            flash_write_word(&flash_nfc, address, nfc_tag_content[i]);
        }
    }
}

void nfc_store_tag_content_to_flash() {
    int i, address;
    int modified_page_count;

    // dump the modified tag content
    modified_page_count = 4; // 4 for tag header
    for (i = 4; i < NFC_MAX_PAGE_NUM && nfc_tag_dirty[i]; i++) {
        modified_page_count++;
        DiagPrintf("page:%02d data:%08x\r\n", i, nfc_tag_content[i]);
    }

    flash_erase_sector(&flash_nfc, FLASH_APP_NFC_BASE);
    for (i = 0, address = FLASH_APP_NFC_BASE; i < modified_page_count; i++, address += 4) {
        flash_write_word(&flash_nfc, address, nfc_tag_content[i]);
    }
}

void nfc_task(void const *arg) {
    int i;
    osEvent evt;

    nfc_load_tag_content_from_flash();

    nfc_init(&nfctag, nfc_tag_content);
    nfc_event(&nfctag, nfc_event_listener, NULL, 0xFF);
    nfc_write(&nfctag, nfc_write_listener, NULL);

    osSignalClear(nfc_tid, NFC_EV_WRITE);

    while(1) {
        evt = osSignalWait (0, 0xFFFFFFFF); // wait for any signal with max timeout
        if (evt.status == osEventSignal && (evt.value.signals & NFC_EV_WRITE)) {
            osDelay(300);

            nfc_store_tag_content_to_flash();

            memset(nfc_tag_dirty, 0, NFC_MAX_PAGE_NUM);
            osSignalClear(nfc_tid, NFC_EV_WRITE);
        }
    }
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
    osThreadDef(nfc_task, osPriorityRealtime, 1, 1024);
    nfc_tid = osThreadCreate (osThread (nfc_task), NULL);

    DBG_INFO_MSG_OFF(_DBG_SPI_FLASH_);

    //3 3)Enable Schedule, Start Kernel
#if defined(CONFIG_KERNEL) && !TASK_SCHEDULER_DISABLED
    #ifdef PLATFORM_FREERTOS
    vTaskStartScheduler();
    #endif
#else
    RtlConsolTaskRom(NULL);
#endif

    while(1);
}


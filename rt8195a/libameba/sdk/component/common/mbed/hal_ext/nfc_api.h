/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_NFC_API_H
#define MBED_NFC_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NFCTAGLENGTH 36 // maximum 36*4=144 bytes
#define NFC_MAX_CACHE_PAGE_NUM      36  // maximum 36*4=144 bytes

typedef enum _NFC_STATUS_ {
    NFC_OK = 0,
    NFC_ERROR = -1
}NFC_STATUS, *PNFC_STATUS;

typedef enum _NFC_PWR_STATUS_ {
    NFC_PWR_DISABLE = 0,
    NFC_PWR_RUNNING = 1,
    NFC_PWR_SLEEP0 = 2,
    NFC_PWR_SLEEP1 = 3,
    NFC_PWR_DOWN = 4,
    NFC_PWR_ERROR = -1
}NFC_PWR_STATUS, *PNFC_PWR_STATUS;

typedef enum _NFC_EVENT_ {
    NFC_EV_READER_PRESENT = (1<<0),
    NFC_EV_READ = (1<<1),
    NFC_EV_WRITE = (1<<2),
    NFC_EV_ERR = (1<<3),
    NFC_EV_CACHE_READ = (1<<4)
}NFC_EVENT, *PNFC_EVENT;

typedef struct nfctag_s nfctag_t;

typedef void (*nfc_read_cb)(void *arg, void *buf, unsigned int page);
typedef void(*nfc_write_cb)(void *arg, unsigned int page, uint32_t pgdat);
typedef void(*nfc_event_cb)(void *arg, unsigned int event);
typedef void(*nfc_cache_read_cb)(void *arg, void *buf, unsigned int page);

int nfc_init(nfctag_t *obj, uint32_t *pg_init_val);
void nfc_read(nfctag_t *obj, nfc_read_cb handler, void *arg);
void nfc_write(nfctag_t *obj, nfc_write_cb handler, void *arg);
void nfc_event(nfctag_t *obj, nfc_event_cb handler, void *arg, unsigned int event_mask);
int nfc_power(nfctag_t *obj, int pwr_mode, int wake_event);
int nfc_cache_write(nfctag_t *obj, uint32_t *tbuf, unsigned int spage, unsigned int pg_num);
int nfc_cache_raed(nfctag_t *obj, nfc_cache_read_cb handler, void *arg, unsigned int start_pg);
int nfc_status(nfctag_t *obj);

#ifdef __cplusplus
}
#endif

#endif

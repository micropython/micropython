/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>

#include "py/mperrno.h"
#include "py/mphal.h"
#include "rtc.h"
#include "rfcore.h"

#if defined(STM32WB)

// Define to 1 to print traces of HCI packets
#define HCI_TRACE (0)

#define IPCC_CH_BLE         (0x01) // BLE HCI command and response
#define IPCC_CH_SYS         (0x02) // system HCI command and response
#define IPCC_CH_MM          (0x08) // release buffer
#define IPCC_CH_HCI_ACL     (0x20) // HCI ACL outgoing data

#define OGF_VENDOR          (0x3f)
#define OCF_WRITE_CONFIG    (0x0c)
#define OCF_SET_TX_POWER    (0x0f)
#define OCF_BLE_INIT        (0x66)

#define HCI_OPCODE(ogf, ocf) ((ogf) << 10 | (ocf))

typedef struct _tl_list_node_t {
    volatile struct _tl_list_node_t *next;
    volatile struct _tl_list_node_t *prev;
    uint8_t body[0];
} tl_list_node_t;

typedef struct _parse_hci_info_t {
    int (*cb_fun)(void*, uint8_t);
    void *cb_env;
    bool was_hci_reset_evt;
} parse_hci_info_t;

static volatile uint32_t ipcc_mem_dev_info_tab[8];
static volatile uint32_t ipcc_mem_ble_tab[4];
static volatile uint32_t ipcc_mem_sys_tab[2];
static volatile uint32_t ipcc_mem_memmgr_tab[7];

static volatile uint32_t ipcc_mem_sys_cmd_buf[272 / 4];
static volatile tl_list_node_t ipcc_mem_sys_queue;

static volatile tl_list_node_t ipcc_mem_memmgr_free_buf_queue;
static volatile uint32_t ipcc_mem_memmgr_ble_spare_evt_buf[272 / 4];
static volatile uint32_t ipcc_mem_memmgr_sys_spare_evt_buf[272 / 4];
static volatile uint32_t ipcc_mem_memmgr_evt_pool[6 * 272 / 4];

static volatile uint32_t ipcc_mem_ble_cmd_buf[272 / 4];
static volatile uint32_t ipcc_mem_ble_cs_buf[272 / 4];
static volatile tl_list_node_t ipcc_mem_ble_evt_queue;
static volatile uint32_t ipcc_mem_ble_hci_acl_data_buf[272 / 4];

/******************************************************************************/
// Transport layer linked list

STATIC void tl_list_init(volatile tl_list_node_t *n) {
    n->next = n;
    n->prev = n;
}

STATIC volatile tl_list_node_t *tl_list_unlink(volatile tl_list_node_t *n) {
    volatile tl_list_node_t *next = n->next;
    volatile tl_list_node_t *prev = n->prev;
    prev->next = next;
    next->prev = prev;
    return next;
}

STATIC void tl_list_append(volatile tl_list_node_t *head, volatile tl_list_node_t *n) {
    n->next = head;
    n->prev = head->prev;
    head->prev->next = n;
    head->prev = n;
}

/******************************************************************************/
// IPCC interface

STATIC uint32_t get_ipccdba(void) {
    return *(uint32_t*)(OPTION_BYTE_BASE + 0x68) & 0x3fff;
}

STATIC volatile void **get_buffer_table(void) {
    return (volatile void**)(SRAM2A_BASE + get_ipccdba());
}

void ipcc_init(uint32_t irq_pri) {
    // Setup buffer table pointers
    volatile void **tab = get_buffer_table();
    tab[0] = &ipcc_mem_dev_info_tab[0];
    tab[1] = &ipcc_mem_ble_tab[0];
    tab[3] = &ipcc_mem_sys_tab[0];
    tab[4] = &ipcc_mem_memmgr_tab[0];

    // Start IPCC peripheral
    __HAL_RCC_IPCC_CLK_ENABLE();

    // Enable wanted IRQs
    IPCC->C1CR = 0;//IPCC_C1CR_RXOIE;
    IPCC->C1MR = 0xffffffff;
    NVIC_SetPriority(IPCC_C1_RX_IRQn, irq_pri);
    HAL_NVIC_EnableIRQ(IPCC_C1_RX_IRQn);

    // Device info table will be populated by FUS/WS

    // Populate system table
    tl_list_init(&ipcc_mem_sys_queue);
    ipcc_mem_sys_tab[0] = (uint32_t)&ipcc_mem_sys_cmd_buf[0];
    ipcc_mem_sys_tab[1] = (uint32_t)&ipcc_mem_sys_queue;

    // Populate memory manager table
    tl_list_init(&ipcc_mem_memmgr_free_buf_queue);
    ipcc_mem_memmgr_tab[0] = (uint32_t)&ipcc_mem_memmgr_ble_spare_evt_buf[0];
    ipcc_mem_memmgr_tab[1] = (uint32_t)&ipcc_mem_memmgr_sys_spare_evt_buf[0];
    ipcc_mem_memmgr_tab[2] = (uint32_t)&ipcc_mem_memmgr_evt_pool[0];
    ipcc_mem_memmgr_tab[3] = sizeof(ipcc_mem_memmgr_evt_pool);
    ipcc_mem_memmgr_tab[4] = (uint32_t)&ipcc_mem_memmgr_free_buf_queue;
    ipcc_mem_memmgr_tab[5] = 0;
    ipcc_mem_memmgr_tab[6] = 0;

    // Populate BLE table
    tl_list_init(&ipcc_mem_ble_evt_queue);
    ipcc_mem_ble_tab[0] = (uint32_t)&ipcc_mem_ble_cmd_buf[0];
    ipcc_mem_ble_tab[1] = (uint32_t)&ipcc_mem_ble_cs_buf[0];
    ipcc_mem_ble_tab[2] = (uint32_t)&ipcc_mem_ble_evt_queue;
    ipcc_mem_ble_tab[3] = (uint32_t)&ipcc_mem_ble_hci_acl_data_buf[0];
}

STATIC int ipcc_wait_ack(unsigned int ch, uint32_t timeout_ms) {
    uint32_t t0 = mp_hal_ticks_ms();
    while (IPCC->C1TOC2SR & ch) {
        if (mp_hal_ticks_ms() - t0 > timeout_ms) {
            printf("ipcc_wait_ack: timeout\n");
            return -MP_ETIMEDOUT;
        }
    }
    // C2 cleared IPCC flag
    return 0;
}

STATIC int ipcc_wait_msg(unsigned int ch, uint32_t timeout_ms) {
    uint32_t t0 = mp_hal_ticks_ms();
    while (!(IPCC->C2TOC1SR & ch)) {
        if (mp_hal_ticks_ms() - t0 > timeout_ms) {
            printf("ipcc_wait_msg: timeout\n");
            return -MP_ETIMEDOUT;
        }
    }
    // C2 set IPCC flag
    return 0;
}

/******************************************************************************/
// Transport layer HCI interface

STATIC void tl_parse_hci_msg(const uint8_t *buf, parse_hci_info_t *parse) {
    const char *kind;
    size_t len = 3 + buf[2];
    switch (buf[0]) {
        case 0x02: {
            // Standard BT HCI ACL packet
            kind = "HCI_ACL";
            if (parse != NULL) {
                for (size_t i = 0; i < len; ++i) {
                    parse->cb_fun(parse->cb_env, buf[i]);
                }
            }
            break;
        }
        case 0x04: {
            // Standard BT HCI event packet
            kind = "HCI_EVT";
            if (parse != NULL) {
                bool fix = false;
                if (buf[1] == 0x0e && len == 7 && buf[3] == 0x01 && buf[4] == 0x63 && buf[5] == 0x0c && buf[6] == 0x01) {
                    len -= 1;
                    fix = true;
                }
                for (size_t i = 0; i < len; ++i) {
                    parse->cb_fun(parse->cb_env, buf[i]);
                }
                if (fix) {
                    len += 1;
                    parse->cb_fun(parse->cb_env, 0x00); // success
                }
                // Check for successful HCI_Reset event
                parse->was_hci_reset_evt = buf[1] == 0x0e && buf[2] == 0x04 && buf[3] == 0x01
                    && buf[4] == 0x03 && buf[5] == 0x0c && buf[6] == 0x00;
            }
            break;
        }
        case 0x11: {
            // Response packet
            // assert(buf[1] == 0x0e);
            kind = "VEND_RESP";
            //uint16_t cmd = buf[4] | buf[5] << 8;
            //uint8_t status = buf[6];
            break;
        }
        case 0x12: {
            // Event packet
            // assert(buf[1] == 0xff);
            kind = "VEND_EVT";
            //uint16_t evt = buf[3] | buf[4] << 8;
            break;
        }
        default:
            kind = "HCI_UNKNOWN";
            break;
    }

    #if HCI_TRACE
    printf("[% 8d] %s(%02x", mp_hal_ticks_ms(), kind, buf[0]);
    for (int i = 1; i < len; ++i) {
        printf(":%02x", buf[i]);
    }
    printf(")\n");
    #else
    (void)kind;
    #endif
}

STATIC void tl_check_msg(volatile tl_list_node_t *head, unsigned int ch, parse_hci_info_t *parse) {
    if (IPCC->C2TOC1SR & ch) {
        // Message available on CH2
        volatile tl_list_node_t *cur = head->next;
        bool free = false;
        while (cur != head) {
            tl_parse_hci_msg((uint8_t*)cur->body, parse);
            volatile tl_list_node_t *next = tl_list_unlink(cur);
            if ((void*)&ipcc_mem_memmgr_evt_pool[0] <= (void*)cur
                && (void*)cur < (void*)&ipcc_mem_memmgr_evt_pool[MP_ARRAY_SIZE(ipcc_mem_memmgr_evt_pool)]) {
                // Place memory back in free pool
                tl_list_append(&ipcc_mem_memmgr_free_buf_queue, cur);
                free = true;
            }
            cur = next;
        }
        if (free) {
            // Notify change in free pool
            IPCC->C1SCR = IPCC_CH_MM << 16;
        }
        // Clear receive channel
        IPCC->C1SCR = ch;
    }
}

STATIC void tl_hci_cmd(uint8_t *cmd, unsigned int ch, uint8_t hdr, uint16_t opcode, size_t len, const uint8_t *buf) {
    tl_list_node_t *n = (tl_list_node_t*)cmd;
    n->next = n;
    n->prev = n;
    cmd[8] = hdr;
    cmd[9] = opcode;
    cmd[10] = opcode >> 8;
    cmd[11] = len;
    memcpy(&cmd[12], buf, len);
    // IPCC indicate
    IPCC->C1SCR = ch << 16;
}

STATIC void tl_sys_wait_resp(const uint8_t *buf, unsigned int ch) {
    if (ipcc_wait_ack(ch, 250) == 0) {
        tl_parse_hci_msg(buf, NULL);
    }
}

STATIC void tl_sys_hci_cmd_resp(uint16_t opcode, size_t len, const uint8_t *buf) {
    tl_hci_cmd((uint8_t*)&ipcc_mem_sys_cmd_buf, IPCC_CH_SYS, 0x10, opcode, len, buf);
    tl_sys_wait_resp((uint8_t*)&ipcc_mem_sys_cmd_buf, IPCC_CH_SYS);
}

STATIC void tl_ble_hci_cmd_resp(uint16_t opcode, size_t len, const uint8_t *buf) {
    tl_hci_cmd((uint8_t*)&ipcc_mem_ble_cmd_buf[0], IPCC_CH_BLE, 0x01, opcode, len, buf);
    ipcc_wait_msg(IPCC_CH_BLE, 250);
    tl_check_msg(&ipcc_mem_ble_evt_queue, IPCC_CH_BLE, NULL);
}

/******************************************************************************/
// RF core interface

void rfcore_init(void) {
    // Ensure LSE is running
    rtc_init_finalise();

    // Select LSE as RF wakeup source
    RCC->CSR = (RCC->CSR & ~RCC_CSR_RFWKPSEL) | 1 << RCC_CSR_RFWKPSEL_Pos;

    // Initialise IPCC and shared memory structures
    ipcc_init(IRQ_PRI_SDIO);

    // Boot the second core
    __SEV();
    __WFE();
    PWR->CR4 |= PWR_CR4_C2BOOT;
}

static const struct {
    uint8_t* pBleBufferAddress;     // unused
    uint32_t BleBufferSize;         // unused
    uint16_t NumAttrRecord;
    uint16_t NumAttrServ;
    uint16_t AttrValueArrSize;
    uint8_t NumOfLinks;
    uint8_t ExtendedPacketLengthEnable;
    uint8_t PrWriteListSize;
    uint8_t MblockCount;
    uint16_t AttMtu;
    uint16_t SlaveSca;
    uint8_t MasterSca;
    uint8_t LsSource;               // 0=LSE 1=internal RO
    uint32_t MaxConnEventLength;
    uint16_t HsStartupTime;
    uint8_t ViterbiEnable;
    uint8_t LlOnly;                 // 0=LL+Host, 1=LL only
    uint8_t HwVersion;
} ble_init_params = {
    0,
    0,
    0, // NumAttrRecord
    0, // NumAttrServ
    0, // AttrValueArrSize
    1, // NumOfLinks
    1, // ExtendedPacketLengthEnable
    0, // PrWriteListSize
    0x79, // MblockCount
    0, // AttMtu
    0, // SlaveSca
    0, // MasterSca
    1, // LsSource
    0xffffffff, // MaxConnEventLength
    0x148, // HsStartupTime
    0, // ViterbiEnable
    1, // LlOnly
    0, // HwVersion
};

void rfcore_ble_init(void) {
    // Clear any outstanding messages from ipcc_init
    tl_check_msg(&ipcc_mem_sys_queue, IPCC_CH_SYS, NULL);
    tl_check_msg(&ipcc_mem_ble_evt_queue, IPCC_CH_BLE, NULL);

    // Configure and reset the BLE controller
    tl_sys_hci_cmd_resp(HCI_OPCODE(OGF_VENDOR, OCF_BLE_INIT), sizeof(ble_init_params), (const uint8_t*)&ble_init_params);
    tl_ble_hci_cmd_resp(HCI_OPCODE(0x03, 0x0003), 0, NULL);
}

void rfcore_ble_hci_cmd(size_t len, const uint8_t *src) {
    #if HCI_TRACE
    printf("[% 8d] HCI_CMD(%02x", mp_hal_ticks_ms(), src[0]);
    for (int i = 1; i < len; ++i) {
        printf(":%02x", src[i]);
    }
    printf(")\n");
    #endif

    tl_list_node_t *n;
    uint32_t ch;
    if (src[0] == 0x01) {
        n = (tl_list_node_t*)&ipcc_mem_ble_cmd_buf[0];
        ch = IPCC_CH_BLE;
    } else if (src[0] == 0x02) {
        n = (tl_list_node_t*)&ipcc_mem_ble_hci_acl_data_buf[0];
        ch = IPCC_CH_HCI_ACL;
    } else {
        printf("** UNEXPECTED HCI HDR: 0x%02x **\n", src[0]);
        return;
    }

    n->next = n;
    n->prev = n;
    memcpy(n->body, src, len);

    // IPCC indicate
    IPCC->C1SCR = ch << 16;
}

void rfcore_ble_check_msg(int (*cb)(void*, uint8_t), void *env) {
    parse_hci_info_t parse = { cb, env, false };
    tl_check_msg(&ipcc_mem_ble_evt_queue, IPCC_CH_BLE, &parse);

    // Intercept HCI_Reset events and reconfigure the controller following the reset
    if (parse.was_hci_reset_evt) {
        uint8_t buf[8];
        buf[0] = 0; // config offset
        buf[1] = 6; // config length
        mp_hal_get_mac(MP_HAL_MAC_BDADDR, &buf[2]);
        #define SWAP_UINT8(a, b) { uint8_t temp = a; a = b; b = temp; }
        SWAP_UINT8(buf[2], buf[7]);
        SWAP_UINT8(buf[3], buf[6]);
        SWAP_UINT8(buf[4], buf[5]);
        tl_ble_hci_cmd_resp(HCI_OPCODE(OGF_VENDOR, OCF_WRITE_CONFIG), 8, buf); // set BDADDR
        tl_ble_hci_cmd_resp(HCI_OPCODE(OGF_VENDOR, OCF_SET_TX_POWER), 2, (const uint8_t*)"\x00\x06"); // 0 dBm
    }
}

#endif // defined(STM32WB)

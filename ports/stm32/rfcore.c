/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
 * Copyright (c) 2020 Jim Mussared
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

#include "stm32wbxx_ll_ipcc.h"

#define DEBUG_printf(...) // printf("rfcore: " __VA_ARGS__)

// Define to 1 to print traces of HCI packets
#define HCI_TRACE (0)

#define IPCC_CH_BLE         (LL_IPCC_CHANNEL_1) // BLE HCI command and response
#define IPCC_CH_SYS         (LL_IPCC_CHANNEL_2) // system HCI command and response
#define IPCC_CH_MM          (LL_IPCC_CHANNEL_4) // release buffer
#define IPCC_CH_HCI_ACL     (LL_IPCC_CHANNEL_6) // HCI ACL outgoing data

#define OGF_CTLR_BASEBAND        (0x03)
#define OCF_CB_RESET             (0x03)
#define OCF_CB_SET_EVENT_MASK2   (0x63)

#define OGF_VENDOR               (0x3f)
#define OCF_WRITE_CONFIG         (0x0c)
#define OCF_SET_TX_POWER         (0x0f)
#define OCF_BLE_INIT             (0x66)

#define HCI_OPCODE(ogf, ocf) ((ogf) << 10 | (ocf))

#define HCI_KIND_BT_CMD (0x01) // <kind=1>...?
#define HCI_KIND_BT_ACL (0x02) // <kind=2><?><?><len LSB><len MSB>
#define HCI_KIND_BT_EVENT (0x04) // <kind=4><op><len><data...>
#define HCI_KIND_VENDOR_RESPONSE (0x11)
#define HCI_KIND_VENDOR_EVENT (0x12)

#define HCI_EVENT_COMMAND_COMPLETE     (0x0E) // <num packets><opcode 16><status><data...>

#define SYS_ACK_TIMEOUT_MS (250)
#define BLE_ACK_TIMEOUT_MS (250)

typedef struct _tl_list_node_t {
    volatile struct _tl_list_node_t *next;
    volatile struct _tl_list_node_t *prev;
    uint8_t body[0];
} tl_list_node_t;

typedef struct _parse_hci_info_t {
    int (*cb_fun)(void *, const uint8_t *, size_t);
    void *cb_env;
    bool was_hci_reset_evt;
} parse_hci_info_t;

// Version
// [0:3]   = Build - 0: Untracked - 15:Released - x: Tracked version
// [4:7]   = branch - 0: Mass Market - x: ...
// [8:15]  = Subversion
// [16:23] = Version minor
// [24:31] = Version major

// Memory Size
// [0:7]   = Flash (Number of 4k sectors)
// [8:15]  = Reserved (Shall be set to 0 - may be used as flash extension)
// [16:23] = SRAM2b (Number of 1k sectors)
// [24:31] = SRAM2a (Number of 1k sectors)

typedef struct __attribute__((packed)) _ipcc_device_info_table_t {
    uint32_t safeboot_version;
    uint32_t fus_version;
    uint32_t fus_memorysize;
    uint32_t fus_info;
    uint32_t fw_version;
    uint32_t fw_memorysize;
    uint32_t fw_infostack;
    uint32_t fw_reserved;
} ipcc_device_info_table_t;

typedef struct __attribute__((packed)) _ipcc_ble_table_t {
    uint8_t *pcmd_buffer;
    uint8_t *pcs_buffer;
    tl_list_node_t *pevt_queue;
    uint8_t *phci_acl_data_buffer;
} ipcc_ble_table_t;

// msg
// [0:7]  = cmd/evt
// [8:31] = Reserved
typedef struct __attribute__((packed)) _ipcc_sys_table_t {
    uint8_t *pcmd_buffer;
    tl_list_node_t *sys_queue;
} ipcc_sys_table_t;

typedef struct __attribute__((packed)) _ipcc_mem_manager_table_t {
    uint8_t *spare_ble_buffer;
    uint8_t *spare_sys_buffer;
    uint8_t *blepool;
    uint32_t blepoolsize;
    tl_list_node_t *pevt_free_buffer_queue;
    uint8_t *traces_evt_pool;
    uint32_t tracespoolsize;
} ipcc_mem_manager_table_t;

typedef struct __attribute__((packed)) _ipcc_ref_table_t {
    ipcc_device_info_table_t *p_device_info_table;
    ipcc_ble_table_t *p_ble_table;
    void *p_thread_table;
    ipcc_sys_table_t *p_sys_table;
    ipcc_mem_manager_table_t *p_mem_manager_table;
    void *p_traces_table;
    void *p_mac_802_15_4_table;
    void *p_zigbee_table;
    void *p_lld_tests_table;
    void *p_lld_ble_table;
} ipcc_ref_table_t;

// The stm32wb55xg.ld script puts .bss.ipcc_mem_* into SRAM2A and .bss_ipcc_membuf_* into SRAM2B.
// It also leaves 64 bytes at the start of SRAM2A for the ref table.

STATIC ipcc_device_info_table_t ipcc_mem_dev_info_tab; // mem1
STATIC ipcc_ble_table_t ipcc_mem_ble_tab; // mem1
STATIC ipcc_sys_table_t ipcc_mem_sys_tab; // mem1
STATIC ipcc_mem_manager_table_t ipcc_mem_memmgr_tab; // mem1

STATIC uint8_t ipcc_membuf_sys_cmd_buf[272];  // mem2
STATIC tl_list_node_t ipcc_mem_sys_queue; // mem1

STATIC tl_list_node_t ipcc_mem_memmgr_free_buf_queue; // mem1
STATIC uint8_t ipcc_membuf_memmgr_ble_spare_evt_buf[272]; // mem2
STATIC uint8_t ipcc_membuf_memmgr_sys_spare_evt_buf[272]; // mem2
STATIC uint8_t ipcc_membuf_memmgr_evt_pool[6 * 272];  // mem2

STATIC uint8_t ipcc_membuf_ble_cmd_buf[272]; // mem2
STATIC uint8_t ipcc_membuf_ble_cs_buf[272]; // mem2
STATIC tl_list_node_t ipcc_mem_ble_evt_queue; // mem1
STATIC uint8_t ipcc_membuf_ble_hci_acl_data_buf[272]; // mem2

// Set by the RX IRQ handler on incoming HCI payload.
STATIC volatile bool had_ble_irq = false;

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

STATIC volatile ipcc_ref_table_t *get_buffer_table(void) {
    // The IPCCDBA option bytes must not be changed without
    // making a corresponding change to the linker script.
    return (volatile ipcc_ref_table_t *)(SRAM2A_BASE + LL_FLASH_GetIPCCBufferAddr() * 4);
}

void ipcc_init(uint32_t irq_pri) {
    DEBUG_printf("ipcc_init\n");

    // Setup buffer table pointers
    volatile ipcc_ref_table_t *tab = get_buffer_table();
    tab->p_device_info_table = &ipcc_mem_dev_info_tab;
    tab->p_ble_table = &ipcc_mem_ble_tab;
    tab->p_sys_table = &ipcc_mem_sys_tab;
    tab->p_mem_manager_table = &ipcc_mem_memmgr_tab;

    // Start IPCC peripheral
    __HAL_RCC_IPCC_CLK_ENABLE();

    // Enable receive IRQ on the BLE channel.
    LL_C1_IPCC_EnableIT_RXO(IPCC);
    LL_C1_IPCC_DisableReceiveChannel(IPCC, LL_IPCC_CHANNEL_1 | LL_IPCC_CHANNEL_2 | LL_IPCC_CHANNEL_3 | LL_IPCC_CHANNEL_4 | LL_IPCC_CHANNEL_5 | LL_IPCC_CHANNEL_6);
    LL_C1_IPCC_EnableReceiveChannel(IPCC, IPCC_CH_BLE);
    NVIC_SetPriority(IPCC_C1_RX_IRQn, irq_pri);
    HAL_NVIC_EnableIRQ(IPCC_C1_RX_IRQn);

    // Device info table will be populated by FUS/WS on CPU2 boot.

    // Populate system table
    tl_list_init(&ipcc_mem_sys_queue);
    ipcc_mem_sys_tab.pcmd_buffer = ipcc_membuf_sys_cmd_buf;
    ipcc_mem_sys_tab.sys_queue = &ipcc_mem_sys_queue;

    // Populate memory manager table
    tl_list_init(&ipcc_mem_memmgr_free_buf_queue);
    ipcc_mem_memmgr_tab.spare_ble_buffer = ipcc_membuf_memmgr_ble_spare_evt_buf;
    ipcc_mem_memmgr_tab.spare_sys_buffer = ipcc_membuf_memmgr_sys_spare_evt_buf;
    ipcc_mem_memmgr_tab.blepool = ipcc_membuf_memmgr_evt_pool;
    ipcc_mem_memmgr_tab.blepoolsize = sizeof(ipcc_membuf_memmgr_evt_pool);
    ipcc_mem_memmgr_tab.pevt_free_buffer_queue = &ipcc_mem_memmgr_free_buf_queue;
    ipcc_mem_memmgr_tab.traces_evt_pool = NULL;
    ipcc_mem_memmgr_tab.tracespoolsize = 0;

    // Populate BLE table
    tl_list_init(&ipcc_mem_ble_evt_queue);
    ipcc_mem_ble_tab.pcmd_buffer = ipcc_membuf_ble_cmd_buf;
    ipcc_mem_ble_tab.pcs_buffer = ipcc_membuf_ble_cs_buf;
    ipcc_mem_ble_tab.pevt_queue = &ipcc_mem_ble_evt_queue;
    ipcc_mem_ble_tab.phci_acl_data_buffer = ipcc_membuf_ble_hci_acl_data_buf;
}

/******************************************************************************/
// Transport layer HCI interface

STATIC void tl_parse_hci_msg(const uint8_t *buf, parse_hci_info_t *parse) {
    const char *info;
    size_t len = 0;
    bool applied_set_event_event_mask2_fix = false;
    switch (buf[0]) {
        case HCI_KIND_BT_ACL: {
            info = "HCI_ACL";

            len = 5 + buf[3] + (buf[4] << 8);
            if (parse != NULL) {
                parse->cb_fun(parse->cb_env, buf, len);
            }
            break;
        }
        case HCI_KIND_BT_EVENT: {
            info = "HCI_EVT";

            len = 3 + buf[2];
            if (parse != NULL) {

                if (buf[1] == HCI_EVENT_COMMAND_COMPLETE && len == 7) {
                    uint16_t opcode = (buf[5] << 8) | buf[4];
                    uint8_t status = buf[6];

                    if (opcode == HCI_OPCODE(OGF_CTLR_BASEBAND, OCF_CB_SET_EVENT_MASK2) && status != 0) {
                        // The WB doesn't support this command (despite being in CS 4.1), so pretend like
                        // it succeeded by replacing the final byte (status) with a zero.
                        applied_set_event_event_mask2_fix = true;
                        len -= 1;
                    }

                    if (opcode == HCI_OPCODE(OGF_CTLR_BASEBAND, OCF_CB_RESET) && status == 0) {
                        // Controller acknowledged reset command.
                        // This will trigger setting the MAC address.
                        parse->was_hci_reset_evt = true;
                    }
                }

                parse->cb_fun(parse->cb_env, buf, len);

                if (applied_set_event_event_mask2_fix) {
                    // Inject the zero status.
                    uint8_t data = 0;
                    parse->cb_fun(parse->cb_env, &data, 1);
                    // Restore the length for the HCI tracing below.
                    len += 1;
                }
            }
            break;
        }
        case HCI_KIND_VENDOR_RESPONSE: {
            // assert(buf[1] == 0x0e);
            info = "VEND_RESP";
            len = 3 + buf[2]; // ???
            // uint16_t cmd = buf[4] | buf[5] << 8;
            // uint8_t status = buf[6];
            break;
        }
        case HCI_KIND_VENDOR_EVENT: {
            // assert(buf[1] == 0xff);
            info = "VEND_EVT";
            len = 3 + buf[2]; // ???
            // uint16_t evt = buf[3] | buf[4] << 8;
            break;
        }
        default:
            info = "HCI_UNKNOWN";
            break;
    }

    #if HCI_TRACE
    printf("[% 8d] <%s(%02x", mp_hal_ticks_ms(), info, buf[0]);
    for (int i = 1; i < len; ++i) {
        printf(":%02x", buf[i]);
    }
    printf(")");
    if (parse && parse->was_hci_reset_evt) {
        printf(" (reset)");
    }
    if (applied_set_event_event_mask2_fix) {
        printf(" (mask2 fix)");
    }
    printf("\n");

    #else
    (void)info;
    #endif
}

STATIC void tl_process_msg(volatile tl_list_node_t *head, unsigned int ch, parse_hci_info_t *parse) {
    volatile tl_list_node_t *cur = head->next;
    bool added_to_free_queue = false;
    while (cur != head) {
        tl_parse_hci_msg((uint8_t *)cur->body, parse);

        volatile tl_list_node_t *next = tl_list_unlink(cur);

        // If this node is allocated from the memmgr event pool, then place it into the free buffer.
        if ((uint8_t *)cur >= ipcc_membuf_memmgr_evt_pool && (uint8_t *)cur < ipcc_membuf_memmgr_evt_pool + sizeof(ipcc_membuf_memmgr_evt_pool)) {
            // Place memory back in free pool.
            tl_list_append(&ipcc_mem_memmgr_free_buf_queue, cur);
            added_to_free_queue = true;
        }

        cur = next;
    }

    if (added_to_free_queue) {
        // Notify change in free pool.
        LL_C1_IPCC_SetFlag_CHx(IPCC, IPCC_CH_MM);
    }
}

STATIC void tl_check_msg(volatile tl_list_node_t *head, unsigned int ch, parse_hci_info_t *parse) {
    if (LL_C2_IPCC_IsActiveFlag_CHx(IPCC, ch)) {
        tl_process_msg(head, ch, parse);

        // Clear receive channel.
        LL_C1_IPCC_ClearFlag_CHx(IPCC, ch);
    }
}

STATIC void tl_check_msg_ble(volatile tl_list_node_t *head, parse_hci_info_t *parse) {
    if (had_ble_irq) {
        tl_process_msg(head, IPCC_CH_BLE, parse);

        had_ble_irq = false;
    }
}

STATIC void tl_hci_cmd(uint8_t *cmd, unsigned int ch, uint8_t hdr, uint16_t opcode, size_t len, const uint8_t *buf) {
    tl_list_node_t *n = (tl_list_node_t *)cmd;
    n->next = n;
    n->prev = n;
    cmd[8] = hdr;
    cmd[9] = opcode;
    cmd[10] = opcode >> 8;
    cmd[11] = len;
    memcpy(&cmd[12], buf, len);

    // Indicate that this channel is ready.
    LL_C1_IPCC_SetFlag_CHx(IPCC, ch);
}

STATIC int tl_sys_wait_ack(const uint8_t *buf) {
    uint32_t t0 = mp_hal_ticks_ms();

    // C2 will clear this bit to acknowledge the request.
    while (LL_C1_IPCC_IsActiveFlag_CHx(IPCC, IPCC_CH_SYS)) {
        if (mp_hal_ticks_ms() - t0 > SYS_ACK_TIMEOUT_MS) {
            printf("tl_sys_wait_ack: timeout\n");
            return -MP_ETIMEDOUT;
        }
    }

    // C1-to-C2 bit cleared, so process (but ignore) the response.
    tl_parse_hci_msg(buf, NULL);
    return 0;
}

STATIC void tl_sys_hci_cmd_resp(uint16_t opcode, size_t len, const uint8_t *buf) {
    tl_hci_cmd(ipcc_membuf_sys_cmd_buf, IPCC_CH_SYS, 0x10, opcode, len, buf);
    tl_sys_wait_ack(ipcc_membuf_sys_cmd_buf);
}

STATIC int tl_ble_wait_resp(void) {
    uint32_t t0 = mp_hal_ticks_ms();
    while (!had_ble_irq) {
        if (mp_hal_ticks_ms() - t0 > BLE_ACK_TIMEOUT_MS) {
            printf("tl_ble_wait_resp: timeout\n");
            return -MP_ETIMEDOUT;
        }
    }

    // C2 set IPCC flag.
    tl_check_msg_ble(&ipcc_mem_ble_evt_queue, NULL);
    return 0;
}

// Synchronously send a BLE command.
STATIC void tl_ble_hci_cmd_resp(uint16_t opcode, size_t len, const uint8_t *buf) {
    tl_hci_cmd(ipcc_membuf_ble_cmd_buf, IPCC_CH_BLE, HCI_KIND_BT_CMD, opcode, len, buf);
    tl_ble_wait_resp();

}

/******************************************************************************/
// RF core interface

void rfcore_init(void) {
    DEBUG_printf("rfcore_init\n");

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
    uint8_t *pBleBufferAddress;     // unused
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
    DEBUG_printf("rfcore_ble_init\n");

    // Clear any outstanding messages from ipcc_init
    tl_check_msg(&ipcc_mem_sys_queue, IPCC_CH_SYS, NULL);
    tl_check_msg_ble(&ipcc_mem_ble_evt_queue, NULL);

    // Configure and reset the BLE controller
    tl_sys_hci_cmd_resp(HCI_OPCODE(OGF_VENDOR, OCF_BLE_INIT), sizeof(ble_init_params), (const uint8_t *)&ble_init_params);
    tl_ble_hci_cmd_resp(HCI_OPCODE(0x03, 0x0003), 0, NULL);
}

void rfcore_ble_hci_cmd(size_t len, const uint8_t *src) {
    DEBUG_printf("rfcore_ble_hci_cmd\n");

    #if HCI_TRACE
    printf("[% 8d] >HCI_CMD(%02x", mp_hal_ticks_ms(), src[0]);
    for (int i = 1; i < len; ++i) {
        printf(":%02x", src[i]);
    }
    printf(")\n");
    #endif

    tl_list_node_t *n;
    uint32_t ch;
    if (src[0] == HCI_KIND_BT_CMD) {
        n = (tl_list_node_t *)&ipcc_membuf_ble_cmd_buf[0];
        ch = IPCC_CH_BLE;
    } else if (src[0] == HCI_KIND_BT_ACL) {
        n = (tl_list_node_t *)&ipcc_membuf_ble_hci_acl_data_buf[0];
        ch = IPCC_CH_HCI_ACL;
    } else {
        printf("** UNEXPECTED HCI HDR: 0x%02x **\n", src[0]);
        return;
    }

    n->next = n;
    n->prev = n;
    memcpy(n->body, src, len);

    // IPCC indicate.
    LL_C1_IPCC_SetFlag_CHx(IPCC, ch);
}

void rfcore_ble_check_msg(int (*cb)(void *, const uint8_t *, size_t), void *env) {
    parse_hci_info_t parse = { cb, env, false };
    tl_check_msg_ble(&ipcc_mem_ble_evt_queue, &parse);

    // Intercept HCI_Reset events and reconfigure the controller following the reset
    if (parse.was_hci_reset_evt) {
        uint8_t buf[8];
        buf[0] = 0; // config offset
        buf[1] = 6; // config length
        mp_hal_get_mac(MP_HAL_MAC_BDADDR, &buf[2]);
        #define SWAP_UINT8(a, b) { uint8_t temp = a; a = b; b = temp; \
}
        SWAP_UINT8(buf[2], buf[7]);
        SWAP_UINT8(buf[3], buf[6]);
        SWAP_UINT8(buf[4], buf[5]);
        tl_ble_hci_cmd_resp(HCI_OPCODE(OGF_VENDOR, OCF_WRITE_CONFIG), 8, buf); // set BDADDR
    }
}

// "level" is 0x00-0x1f, ranging from -40 dBm to +6 dBm (not linear).
void rfcore_ble_set_txpower(uint8_t level) {
    uint8_t buf[2] = { 0x00, level };
    tl_ble_hci_cmd_resp(HCI_OPCODE(OGF_VENDOR, OCF_SET_TX_POWER), 2, buf);
}

// IPCC IRQ Handlers
void IPCC_C1_TX_IRQHandler(void) {
    IRQ_ENTER(IPCC_C1_TX_IRQn);
    IRQ_EXIT(IPCC_C1_TX_IRQn);
}

void IPCC_C1_RX_IRQHandler(void) {
    IRQ_ENTER(IPCC_C1_RX_IRQn);

    if (LL_C2_IPCC_IsActiveFlag_CHx(IPCC, IPCC_CH_BLE)) {
        had_ble_irq = true;

        LL_C1_IPCC_ClearFlag_CHx(IPCC, IPCC_CH_BLE);

        // Schedule PENDSV to process incoming HCI payload.
        extern void mp_bluetooth_hci_poll_wrapper(uint32_t ticks_ms);
        mp_bluetooth_hci_poll_wrapper(0);
    }

    IRQ_EXIT(IPCC_C1_RX_IRQn);
}

#endif // defined(STM32WB)

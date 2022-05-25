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
#include "py/runtime.h"
#include "extmod/modbluetooth.h"
#include "mpbthciport.h"
#include "rtc.h"
#include "rfcore.h"

#if defined(STM32WB)

#include "stm32wbxx_ll_ipcc.h"

#if MICROPY_PY_BLUETOOTH

#if MICROPY_BLUETOOTH_NIMBLE
// For mp_bluetooth_nimble_hci_uart_wfi
#include "nimble/nimble_npl.h"
#else
#error "STM32WB must use NimBLE."
#endif

#if !MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS
#error "STM32WB must use synchronous BLE events."
#endif

#endif

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

#define OGF_VENDOR                        (0x3f)
#define OCF_WRITE_CONFIG                  (0x0c)
#define OCF_SET_TX_POWER                  (0x0f)
#define OCF_BLE_INIT                      (0x66)
#define OCF_C2_FLASH_ERASE_ACTIVITY       (0x69)
#define OCF_C2_SET_FLASH_ACTIVITY_CONTROL (0x73)

#define HCI_OPCODE(ogf, ocf) ((ogf) << 10 | (ocf))

#define HCI_KIND_BT_CMD (0x01) // <kind=1>...?
#define HCI_KIND_BT_ACL (0x02) // <kind=2><?><?><len LSB><len MSB>
#define HCI_KIND_BT_EVENT (0x04) // <kind=4><op><len><data...>
#define HCI_KIND_VENDOR_RESPONSE (0x11)
#define HCI_KIND_VENDOR_EVENT (0x12)

#define HCI_EVENT_COMMAND_COMPLETE            (0x0E) // <num packets><opcode 16><status><data...>
#define HCI_EVENT_COMMAND_STATUS              (0x0F) // <status><num_packets><opcode 16>
#define HCI_EVENT_NUMBER_OF_COMPLETED_PACKETS (0x13) // <num>(<handle 16><completed 16>)*

#define SYS_ACK_TIMEOUT_MS (250)
#define BLE_ACK_TIMEOUT_MS (250)

// AN5185
#define MAGIC_FUS_ACTIVE 0xA94656B9
// AN5289
#define MAGIC_IPCC_MEM_INCORRECT 0x3DE96F61

volatile bool hci_acl_cmd_pending = false;

typedef struct _tl_list_node_t {
    volatile struct _tl_list_node_t *next;
    volatile struct _tl_list_node_t *prev;
    uint8_t body[0];
} tl_list_node_t;

typedef struct _parse_hci_info_t {
    rfcore_ble_msg_callback_t cb_fun;
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

typedef union __attribute__((packed)) _ipcc_device_info_table_t {
    struct {
        uint32_t table_state;
        uint8_t reserved0;
        uint8_t last_fus_state;
        uint8_t last_ws_state;
        uint8_t ws_type;
        uint32_t safeboot_version;
        uint32_t fus_version;
        uint32_t fus_memorysize;
        uint32_t ws_version;
        uint32_t ws_memorysize;
        uint32_t ws_ble_info;
        uint32_t ws_thread_info;
        uint32_t reserved1;
        uint64_t uid64;
        uint16_t device_id;
        uint16_t pad;
    } fus;
    struct {
        uint32_t safeboot_version;
        uint32_t fus_version;
        uint32_t fus_memorysize;
        uint32_t fus_info;
        uint32_t fw_version;
        uint32_t fw_memorysize;
        uint32_t fw_infostack;
        uint32_t fw_reserved;
    } ws;
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

// The WS firmware doesn't support OCF_CB_SET_EVENT_MASK2, and fails with:
//  v1.8.0.0.4 (and below): HCI_EVENT_COMMAND_COMPLETE with a non-zero status
//  v1.9.0.0.4 (and above): HCI_EVENT_COMMAND_STATUS with a non-zero status
// In either case we detect the failure response and inject this response
// instead (which is HCI_EVENT_COMMAND_COMPLETE for OCF_CB_SET_EVENT_MASK2
// with status=0).
STATIC const uint8_t set_event_event_mask2_fix_payload[] = { 0x04, 0x0e, 0x04, 0x01, 0x63, 0x0c, 0x00 };

STATIC size_t tl_parse_hci_msg(const uint8_t *buf, parse_hci_info_t *parse) {
    const char *info;
    #if HCI_TRACE
    int applied_set_event_event_mask2_fix = 0;
    #endif
    size_t len;
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

            // Acknowledgment of a pending ACL request, allow another one to be sent.
            if (buf[1] == HCI_EVENT_NUMBER_OF_COMPLETED_PACKETS) {
                hci_acl_cmd_pending = false;
            }

            len = 3 + buf[2];
            if (parse != NULL) {

                if (buf[1] == HCI_EVENT_COMMAND_COMPLETE && len == 7) {
                    uint16_t opcode = (buf[5] << 8) | buf[4];
                    uint8_t status = buf[6];

                    if (opcode == HCI_OPCODE(OGF_CTLR_BASEBAND, OCF_CB_SET_EVENT_MASK2) && status != 0) {
                        // For WS firmware v1.8.0.0.4 and below. Reply with the "everything OK" payload.
                        parse->cb_fun(parse->cb_env, set_event_event_mask2_fix_payload, sizeof(set_event_event_mask2_fix_payload));
                        #if HCI_TRACE
                        applied_set_event_event_mask2_fix = 18;
                        #endif
                        break; // Don't send the original payload.
                    }

                    if (opcode == HCI_OPCODE(OGF_CTLR_BASEBAND, OCF_CB_RESET) && status == 0) {
                        // Controller acknowledged reset command.
                        // This will trigger setting the MAC address.
                        parse->was_hci_reset_evt = true;
                    }
                }

                if (buf[1] == HCI_EVENT_COMMAND_STATUS && len == 7) {
                    uint16_t opcode = (buf[6] << 8) | buf[5];
                    uint8_t status = buf[3];

                    if (opcode == HCI_OPCODE(OGF_CTLR_BASEBAND, OCF_CB_SET_EVENT_MASK2) && status != 0) {
                        // For WS firmware v1.9.0.0.4 and higher. Reply with the "everything OK" payload.
                        parse->cb_fun(parse->cb_env, set_event_event_mask2_fix_payload, sizeof(set_event_event_mask2_fix_payload));
                        #if HCI_TRACE
                        applied_set_event_event_mask2_fix = 19;
                        #endif
                        break;  // Don't send the original payload.
                    }
                }

                parse->cb_fun(parse->cb_env, buf, len);
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
            len = 0;
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
        printf(" (mask2 fix %d)", applied_set_event_event_mask2_fix);
    }
    printf("\n");

    #else
    (void)info;
    #endif

    return len;
}

STATIC size_t tl_process_msg(volatile tl_list_node_t *head, unsigned int ch, parse_hci_info_t *parse) {
    volatile tl_list_node_t *cur = head->next;
    bool added_to_free_queue = false;
    size_t len = 0;
    while (cur != head) {
        len += tl_parse_hci_msg((uint8_t *)cur->body, parse);

        volatile tl_list_node_t *next = tl_list_unlink(cur);

        // If this node is allocated from the memmgr event pool, then place it into the free buffer.
        if ((uint8_t *)cur >= ipcc_membuf_memmgr_evt_pool && (uint8_t *)cur < ipcc_membuf_memmgr_evt_pool + sizeof(ipcc_membuf_memmgr_evt_pool)) {
            // Wait for C2 to indicate that it has finished using the free buffer,
            // so that we can link the newly-freed memory in to this buffer.
            // If waiting is needed then it is typically between 5 and 20 microseconds.
            while (LL_C1_IPCC_IsActiveFlag_CHx(IPCC, IPCC_CH_MM)) {
            }

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
    return len;
}

// Only call this when IRQs are disabled on this channel.
STATIC size_t tl_check_msg(volatile tl_list_node_t *head, unsigned int ch, parse_hci_info_t *parse) {
    size_t len = 0;
    if (LL_C2_IPCC_IsActiveFlag_CHx(IPCC, ch)) {
        // Process new data.
        len = tl_process_msg(head, ch, parse);

        // Clear receive channel (allows RF core to send more data to us).
        LL_C1_IPCC_ClearFlag_CHx(IPCC, ch);

        if (ch == IPCC_CH_BLE) {
            // Renable IRQs for BLE now that we've cleared the flag.
            LL_C1_IPCC_EnableReceiveChannel(IPCC, IPCC_CH_BLE);
        }
    }
    return len;
}

STATIC void tl_hci_cmd(uint8_t *cmd, unsigned int ch, uint8_t hdr, uint16_t opcode, const uint8_t *buf, size_t len) {
    tl_list_node_t *n = (tl_list_node_t *)cmd;
    n->next = NULL;
    n->prev = NULL;
    cmd[8] = hdr;
    cmd[9] = opcode;
    cmd[10] = opcode >> 8;
    cmd[11] = len;
    memcpy(&cmd[12], buf, len);

    #if HCI_TRACE
    printf("[% 8d] >HCI(", mp_hal_ticks_ms());
    for (int i = 0; i < len + 4; ++i) {
        printf(":%02x", cmd[i + 8]);
    }
    printf(")\n");
    #endif

    // Indicate that this channel is ready.
    LL_C1_IPCC_SetFlag_CHx(IPCC, ch);
}

STATIC ssize_t tl_sys_wait_ack(const uint8_t *buf, mp_int_t timeout_ms) {
    uint32_t t0 = mp_hal_ticks_ms();

    timeout_ms = MAX(SYS_ACK_TIMEOUT_MS, timeout_ms);

    // C2 will clear this bit to acknowledge the request.
    while (LL_C1_IPCC_IsActiveFlag_CHx(IPCC, IPCC_CH_SYS)) {
        if (mp_hal_ticks_ms() - t0 > timeout_ms) {
            printf("tl_sys_wait_ack: timeout\n");
            return -MP_ETIMEDOUT;
        }
    }

    // C1-to-C2 bit cleared, so process the response (just get the length, do
    // not parse any further).
    return (ssize_t)tl_parse_hci_msg(buf, NULL);
}

STATIC ssize_t tl_sys_hci_cmd_resp(uint16_t opcode, const uint8_t *buf, size_t len, mp_int_t timeout_ms) {
    tl_hci_cmd(ipcc_membuf_sys_cmd_buf, IPCC_CH_SYS, 0x10, opcode, buf, len);
    return tl_sys_wait_ack(ipcc_membuf_sys_cmd_buf, timeout_ms);
}

STATIC int tl_ble_wait_resp(void) {
    uint32_t t0 = mp_hal_ticks_ms();
    while (!LL_C2_IPCC_IsActiveFlag_CHx(IPCC, IPCC_CH_BLE)) {
        if (mp_hal_ticks_ms() - t0 > BLE_ACK_TIMEOUT_MS) {
            printf("tl_ble_wait_resp: timeout\n");
            return -MP_ETIMEDOUT;
        }
    }

    // C2 set IPCC flag -- process the data, clear the flag, and re-enable IRQs.
    tl_check_msg(&ipcc_mem_ble_evt_queue, IPCC_CH_BLE, NULL);
    return 0;
}

// Synchronously send a BLE command.
STATIC void tl_ble_hci_cmd_resp(uint16_t opcode, const uint8_t *buf, size_t len) {
    // Poll for completion rather than wait for IRQ->scheduler.
    LL_C1_IPCC_DisableReceiveChannel(IPCC, IPCC_CH_BLE);
    tl_hci_cmd(ipcc_membuf_ble_cmd_buf, IPCC_CH_BLE, HCI_KIND_BT_CMD, opcode, buf, len);
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
    uint8_t LsSource;
    uint32_t MaxConnEventLength;
    uint16_t HsStartupTime;
    uint8_t ViterbiEnable;
    uint8_t LlOnly;
    uint8_t HwVersion;
} ble_init_params = {
    0, // pBleBufferAddress
    0, // BleBufferSize
    MICROPY_HW_RFCORE_BLE_NUM_GATT_ATTRIBUTES,
    MICROPY_HW_RFCORE_BLE_NUM_GATT_SERVICES,
    MICROPY_HW_RFCORE_BLE_ATT_VALUE_ARRAY_SIZE,
    MICROPY_HW_RFCORE_BLE_NUM_LINK,
    MICROPY_HW_RFCORE_BLE_DATA_LENGTH_EXTENSION,
    MICROPY_HW_RFCORE_BLE_PREPARE_WRITE_LIST_SIZE,
    MICROPY_HW_RFCORE_BLE_MBLOCK_COUNT,
    MICROPY_HW_RFCORE_BLE_MAX_ATT_MTU,
    MICROPY_HW_RFCORE_BLE_SLAVE_SCA,
    MICROPY_HW_RFCORE_BLE_MASTER_SCA,
    MICROPY_HW_RFCORE_BLE_LSE_SOURCE,
    MICROPY_HW_RFCORE_BLE_MAX_CONN_EVENT_LENGTH,
    MICROPY_HW_RFCORE_BLE_HSE_STARTUP_TIME,
    MICROPY_HW_RFCORE_BLE_VITERBI_MODE,
    MICROPY_HW_RFCORE_BLE_LL_ONLY,
    0, // HwVersion
};

void rfcore_ble_init(void) {
    DEBUG_printf("rfcore_ble_init\n");

    // Clear any outstanding messages from ipcc_init.
    tl_check_msg(&ipcc_mem_sys_queue, IPCC_CH_SYS, NULL);

    // Configure and reset the BLE controller.
    tl_sys_hci_cmd_resp(HCI_OPCODE(OGF_VENDOR, OCF_BLE_INIT), (const uint8_t *)&ble_init_params, sizeof(ble_init_params), 0);
    tl_ble_hci_cmd_resp(HCI_OPCODE(0x03, 0x0003), NULL, 0);

    // Enable PES rather than SEM7 to moderate flash access between the cores.
    uint8_t buf = 0; // FLASH_ACTIVITY_CONTROL_PES
    tl_sys_hci_cmd_resp(HCI_OPCODE(OGF_VENDOR, OCF_C2_SET_FLASH_ACTIVITY_CONTROL), &buf, 1, 0);
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

        // Give the previous ACL command up to 100ms to complete.
        mp_uint_t timeout_start_ticks_ms = mp_hal_ticks_ms();
        while (hci_acl_cmd_pending) {
            if (mp_hal_ticks_ms() - timeout_start_ticks_ms > 100) {
                break;
            }
            #if MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_NIMBLE
            mp_bluetooth_nimble_hci_uart_wfi();
            #endif
        }

        // Prevent sending another command until this one returns with HCI_EVENT_COMMAND_{COMPLETE,STATUS}.
        hci_acl_cmd_pending = true;
    } else {
        printf("** UNEXPECTED HCI HDR: 0x%02x **\n", src[0]);
        return;
    }

    n->next = NULL;
    n->prev = NULL;
    memcpy(n->body, src, len);

    // IPCC indicate.
    LL_C1_IPCC_SetFlag_CHx(IPCC, ch);
}

size_t rfcore_ble_check_msg(rfcore_ble_msg_callback_t cb, void *env) {
    parse_hci_info_t parse = { cb, env, false };
    size_t len = tl_check_msg(&ipcc_mem_ble_evt_queue, IPCC_CH_BLE, &parse);

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
        tl_ble_hci_cmd_resp(HCI_OPCODE(OGF_VENDOR, OCF_WRITE_CONFIG), buf, 8); // set BDADDR
    }
    return len;
}

// "level" is 0x00-0x1f, ranging from -40 dBm to +6 dBm (not linear).
void rfcore_ble_set_txpower(uint8_t level) {
    uint8_t buf[2] = { 0x00, level };
    tl_ble_hci_cmd_resp(HCI_OPCODE(OGF_VENDOR, OCF_SET_TX_POWER), buf, 2);
}

void rfcore_start_flash_erase(void) {
    uint8_t buf = 1; // ERASE_ACTIVITY_ON
    tl_sys_hci_cmd_resp(HCI_OPCODE(OGF_VENDOR, OCF_C2_FLASH_ERASE_ACTIVITY), &buf, 1, 0);
}

void rfcore_end_flash_erase(void) {
    uint8_t buf = 0; // ERASE_ACTIVITY_OFF
    tl_sys_hci_cmd_resp(HCI_OPCODE(OGF_VENDOR, OCF_C2_FLASH_ERASE_ACTIVITY), &buf, 1, 0);
}

/******************************************************************************/
// IPCC IRQ Handlers

void IPCC_C1_TX_IRQHandler(void) {
    IRQ_ENTER(IPCC_C1_TX_IRQn);
    IRQ_EXIT(IPCC_C1_TX_IRQn);
}

void IPCC_C1_RX_IRQHandler(void) {
    IRQ_ENTER(IPCC_C1_RX_IRQn);

    DEBUG_printf("IPCC_C1_RX_IRQHandler\n");

    if (LL_C2_IPCC_IsActiveFlag_CHx(IPCC, IPCC_CH_BLE)) {
        // Disable this IRQ until the incoming data is processed (in tl_check_msg).
        LL_C1_IPCC_DisableReceiveChannel(IPCC, IPCC_CH_BLE);

        #if MICROPY_PY_BLUETOOTH
        // Queue up the scheduler to process UART data and run events.
        mp_bluetooth_hci_poll_now();
        #endif
    }

    IRQ_EXIT(IPCC_C1_RX_IRQn);
}

/******************************************************************************/
// MicroPython bindings

STATIC mp_obj_t rfcore_status(void) {
    return mp_obj_new_int_from_uint(ipcc_mem_dev_info_tab.fus.table_state);
}
MP_DEFINE_CONST_FUN_OBJ_0(rfcore_status_obj, rfcore_status);

STATIC mp_obj_t get_version_tuple(uint32_t data) {
    mp_obj_t items[] = {
        MP_OBJ_NEW_SMALL_INT(data >> 24), MP_OBJ_NEW_SMALL_INT(data >> 16 & 0xFF), MP_OBJ_NEW_SMALL_INT(data >> 8 & 0xFF), MP_OBJ_NEW_SMALL_INT(data >> 4 & 0xF), MP_OBJ_NEW_SMALL_INT(data & 0xF)
    };
    return mp_obj_new_tuple(5, items);
}

STATIC mp_obj_t rfcore_fw_version(mp_obj_t fw_id_in) {
    if (ipcc_mem_dev_info_tab.fus.table_state == MAGIC_IPCC_MEM_INCORRECT) {
        mp_raise_OSError(MP_EINVAL);
    }
    mp_int_t fw_id = mp_obj_get_int(fw_id_in);
    bool fus_active = ipcc_mem_dev_info_tab.fus.table_state == MAGIC_FUS_ACTIVE;
    uint32_t v;
    if (fw_id == 0) {
        // FUS
        v = fus_active ? ipcc_mem_dev_info_tab.fus.fus_version : ipcc_mem_dev_info_tab.ws.fus_version;
    } else {
        // WS
        v = fus_active ? ipcc_mem_dev_info_tab.fus.ws_version : ipcc_mem_dev_info_tab.ws.fw_version;
    }
    return get_version_tuple(v);
}
MP_DEFINE_CONST_FUN_OBJ_1(rfcore_fw_version_obj, rfcore_fw_version);

STATIC mp_obj_t rfcore_sys_hci(size_t n_args, const mp_obj_t *args) {
    if (ipcc_mem_dev_info_tab.fus.table_state == MAGIC_IPCC_MEM_INCORRECT) {
        mp_raise_OSError(MP_EINVAL);
    }
    mp_int_t ogf = mp_obj_get_int(args[0]);
    mp_int_t ocf = mp_obj_get_int(args[1]);
    mp_buffer_info_t bufinfo = {0};
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);
    mp_int_t timeout_ms = 0;
    if (n_args >= 4) {
        timeout_ms = mp_obj_get_int(args[3]);
    }
    ssize_t len = tl_sys_hci_cmd_resp(HCI_OPCODE(ogf, ocf), bufinfo.buf, bufinfo.len, timeout_ms);
    if (len < 0) {
        mp_raise_OSError(-len);
    }
    return mp_obj_new_bytes(ipcc_membuf_sys_cmd_buf, len);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rfcore_sys_hci_obj, 3, 4, rfcore_sys_hci);

#endif // defined(STM32WB)

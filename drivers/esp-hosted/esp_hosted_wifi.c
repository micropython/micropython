/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Arduino SA
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
 *
 * ESP-Hosted WiFi driver.
 */

#include "py/mphal.h"
#include "py/mperrno.h"

#if MICROPY_PY_NETWORK_ESP_HOSTED

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "lwip/err.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "netif/etharp.h"

#include "shared/netutils/netutils.h"
#include "shared/netutils/dhcpserver.h"

#include "esp_hosted.pb-c.h"

#include "esp_hosted_hal.h"
#include "esp_hosted_stack.h"
#include "esp_hosted_netif.h"
#include "esp_hosted_wifi.h"
#include "esp_hosted_internal.h"

static esp_hosted_state_t esp_state;

static ProtobufCAllocator protobuf_alloc = {
    .alloc = &esp_hosted_hal_alloc,
    .free = &esp_hosted_hal_free,
    .allocator_data = NULL,
};

static void esp_hosted_macstr_to_bytes(const uint8_t *mac_str, size_t mac_len, uint8_t *mac_out) {
    uint8_t byte = 0;
    for (int i = 0; i < mac_len; i++) {
        char c = mac_str[i];
        if (c >= '0' && c <= '9') {
            byte = (byte << 4) | (c - '0');
        } else if (c >= 'a' && c <= 'f') {
            byte = (byte << 4) | (c - 'a' + 10);
        } else if (c >= 'A' && c <= 'F') {
            byte = (byte << 4) | (c - 'A' + 10);
        }
        if (c == ':' || (i + 1) == mac_len) {
            *mac_out++ = byte;
            byte = 0;
        }
    }
}

// to avoid bleeding the protocol buffer API into the public interface, convert esp_hosted_security_t
// to/from CtrlWifiSecProt here.

static esp_hosted_security_t sec_prot_to_hosted_security(CtrlWifiSecProt sec_prot)
{
    switch (sec_prot) {
    case CTRL__WIFI_SEC_PROT__Open:
        return ESP_HOSTED_SEC_OPEN;
    case CTRL__WIFI_SEC_PROT__WEP:
        return ESP_HOSTED_SEC_WEP;
    case CTRL__WIFI_SEC_PROT__WPA_PSK:
        return ESP_HOSTED_SEC_WPA_PSK;
    case CTRL__WIFI_SEC_PROT__WPA2_PSK:
        return ESP_HOSTED_SEC_WPA2_PSK;
    case CTRL__WIFI_SEC_PROT__WPA_WPA2_PSK:
        return ESP_HOSTED_SEC_WPA_WPA2_PSK;
    case CTRL__WIFI_SEC_PROT__WPA2_ENTERPRISE:
        return ESP_HOSTED_SEC_WPA2_ENTERPRISE;
    case CTRL__WIFI_SEC_PROT__WPA3_PSK:
        return ESP_HOSTED_SEC_WPA3_PSK;
    case CTRL__WIFI_SEC_PROT__WPA2_WPA3_PSK:
        return ESP_HOSTED_SEC_WPA2_WPA3_PSK;
    default:
        return ESP_HOSTED_SEC_INVALID;
    }
}

static CtrlWifiSecProt hosted_security_to_sec_prot(esp_hosted_security_t hosted_security)
{
    switch (hosted_security) {
    case ESP_HOSTED_SEC_OPEN:
        return CTRL__WIFI_SEC_PROT__Open;
    case ESP_HOSTED_SEC_WEP:
        return CTRL__WIFI_SEC_PROT__WEP;
    case ESP_HOSTED_SEC_WPA_PSK:
        return CTRL__WIFI_SEC_PROT__WPA_PSK;
    case ESP_HOSTED_SEC_WPA2_PSK:
        return CTRL__WIFI_SEC_PROT__WPA2_PSK;
    case ESP_HOSTED_SEC_WPA_WPA2_PSK:
        return CTRL__WIFI_SEC_PROT__WPA_WPA2_PSK;
    case ESP_HOSTED_SEC_WPA2_ENTERPRISE:
        return CTRL__WIFI_SEC_PROT__WPA2_ENTERPRISE;
    case ESP_HOSTED_SEC_WPA3_PSK:
        return CTRL__WIFI_SEC_PROT__WPA3_PSK;
    case ESP_HOSTED_SEC_WPA2_WPA3_PSK:
        return CTRL__WIFI_SEC_PROT__WPA2_WPA3_PSK;
    default:
        abort(); // Range should be checked by the caller, making this unreachable
    }
}

uint16_t esp_hosted_checksum(esp_header_t *esp_header) {
    uint16_t checksum = 0;
    esp_header->checksum = 0;
    uint8_t *buf = (uint8_t *)esp_header;
    for (size_t i = 0; i < (esp_header->len + sizeof(esp_header_t)); i++) {
        checksum += buf[i];
    }
    return checksum;
}

#if ESP_HOSTED_DEBUG
static void esp_hosted_dump_header(esp_header_t *esp_header) {
    static const char *if_strs[] = { "STA", "AP", "SERIAL", "HCI", "PRIV", "TEST" };
    if (esp_header->if_type > ESP_HOSTED_MAX_IF) {
        return;
    }
    debug_printf("esp header: if %s_IF length %d offset %d checksum %d seq %d flags %x\n",
        if_strs[esp_header->if_type], esp_header->len, esp_header->offset,
        esp_header->checksum, esp_header->seq_num, esp_header->flags);

    if (esp_header->if_type == ESP_HOSTED_SERIAL_IF) {
        tlv_header_t *tlv_header = (tlv_header_t *)(esp_header->payload);
        debug_printf("tlv header: ep_type %d ep_length %d ep_value %.8s data_type %d data_length %d\n",
            tlv_header->ep_type, tlv_header->ep_length,
            tlv_header->ep_value, tlv_header->data_type, tlv_header->data_length);
    }
}
#endif

static int32_t esp_hosted_resp_value(CtrlMsg *ctrl_msg) {
    // Each response struct return value is located at a different offset,
    // the following array maps response CtrlMsgs to return values (resp)
    // offsets within each response struct.
    const static size_t ctrl_msg_resp_offset[] = {
        offsetof(CtrlMsgRespGetMacAddress, resp),
        offsetof(CtrlMsgRespSetMacAddress, resp),
        offsetof(CtrlMsgRespGetMode, resp),
        offsetof(CtrlMsgRespSetMode, resp),
        offsetof(CtrlMsgRespScanResult, resp),
        offsetof(CtrlMsgRespGetAPConfig, resp),
        offsetof(CtrlMsgRespConnectAP, resp),
        offsetof(CtrlMsgRespGetStatus, resp),
        offsetof(CtrlMsgRespGetSoftAPConfig, resp),
        offsetof(CtrlMsgRespSetSoftAPVendorSpecificIE, resp),
        offsetof(CtrlMsgRespStartSoftAP, resp),
        offsetof(CtrlMsgRespSoftAPConnectedSTA, resp),
        offsetof(CtrlMsgRespGetStatus, resp),
        offsetof(CtrlMsgRespSetMode, resp),
        offsetof(CtrlMsgRespGetMode, resp),
        offsetof(CtrlMsgRespOTABegin, resp),
        offsetof(CtrlMsgRespOTAWrite, resp),
        offsetof(CtrlMsgRespOTAEnd, resp),
        offsetof(CtrlMsgRespSetWifiMaxTxPower, resp),
        offsetof(CtrlMsgRespGetWifiCurrTxPower, resp),
        offsetof(CtrlMsgRespConfigHeartbeat, resp),
    };

    int32_t resp = -1;
    size_t index = ctrl_msg->msg_id - CTRL_MSG_ID__Resp_Base;

    // All types of messages share the same payload base address.
    if (ctrl_msg->resp_get_mac_address != NULL &&
        ctrl_msg->msg_type == CTRL_MSG_TYPE__Resp &&
        index > 0 && index <= MP_ARRAY_SIZE(ctrl_msg_resp_offset)) {
        // Return the response struct's return value.
        size_t offset = ctrl_msg_resp_offset[index - 1];
        resp = *((int32_t *)((char *)ctrl_msg->resp_get_mac_address + offset));
    }
    return resp;
}

static int esp_hosted_request(CtrlMsgId msg_id, void *ctrl_payload) {
    CtrlMsg ctrl_msg = {0};
    ctrl_msg__init(&ctrl_msg);
    ctrl_msg.msg_id = msg_id;
    ctrl_msg.payload_case = msg_id;

    // All types of messages share the same payload base address.
    ctrl_msg.req_get_mac_address = ctrl_payload;

    // Pack protobuf
    size_t payload_size = ctrl_msg__get_packed_size(&ctrl_msg);
    if ((payload_size + sizeof(tlv_header_t)) > ESP_FRAME_MAX_PAYLOAD) {
        error_printf("esp_hosted_request() payload size > max payload %d\n", msg_id);
        return -1;
    }

    esp_header_t *esp_header = (esp_header_t *)(esp_state.buf);
    tlv_header_t *tlv_header = (tlv_header_t *)(esp_header->payload);

    esp_header->if_type = ESP_HOSTED_SERIAL_IF;
    esp_header->if_num = 0;
    esp_header->flags = 0;
    esp_header->len = payload_size + sizeof(tlv_header_t);
    esp_header->offset = sizeof(esp_header_t);
    esp_header->seq_num = esp_state.seq_num++;

    tlv_header->ep_type = TLV_HEADER_TYPE_EP;
    tlv_header->ep_length = 8;
    memcpy(tlv_header->ep_value, TLV_HEADER_EP_RESP, 8);
    tlv_header->data_type = TLV_HEADER_TYPE_DATA;
    tlv_header->data_length = payload_size;
    ctrl_msg__pack(&ctrl_msg, tlv_header->data);
    esp_header->checksum = esp_hosted_checksum(esp_header);

    size_t frame_size = (sizeof(esp_header_t) + esp_header->len + 3) & ~3U;
    if (esp_hosted_hal_spi_transfer(esp_state.buf, NULL, frame_size) != 0) {
        error_printf("esp_hosted_request() request %d failed\n", msg_id);
        return -1;
    }
    return 0;
}

static CtrlMsg *esp_hosted_response(CtrlMsgId msg_id, uint32_t timeout) {
    CtrlMsg *ctrl_msg = NULL;
    for (mp_uint_t start = mp_hal_ticks_ms(); ; mp_event_wait_ms(10)) {
        if (!esp_hosted_stack_empty(&esp_state.stack)) {
            ctrl_msg = esp_hosted_stack_pop(&esp_state.stack, true);
            if (ctrl_msg->msg_id == msg_id) {
                ctrl_msg = esp_hosted_stack_pop(&esp_state.stack, false);
                break;
            }

            debug_printf("esp_hosted_response() waiting for id %lu last id %lu\n", msg_id, ctrl_msg->msg_id);
            ctrl_msg = NULL;
        }

        if (timeout == 0) {
            // Request expected a sync response.
            return NULL;
        }

        // Check timeout.
        if ((mp_hal_ticks_ms() - start) >= timeout) {
            return NULL;
        }
    }

    // If message type is a response, check the response struct's return value.
    if (ctrl_msg->msg_type == CTRL_MSG_TYPE__Resp && esp_hosted_resp_value(ctrl_msg) != 0) {
        error_printf("esp_hosted_response() response %d failed %d\n", msg_id, esp_hosted_resp_value(ctrl_msg));
        ctrl_msg__free_unpacked(ctrl_msg, &protobuf_alloc);
        return NULL;
    }

    return ctrl_msg;
}

static int esp_hosted_ctrl(CtrlMsgId req_id, void *req_payload, CtrlMsg **resp_msg) {
    if (esp_hosted_request(req_id, req_payload) != 0) {
        return -1;
    }
    uint32_t resp_id = (req_id - CTRL_MSG_ID__Req_Base) + CTRL_MSG_ID__Resp_Base;
    if ((*resp_msg = esp_hosted_response(resp_id, ESP_SYNC_REQ_TIMEOUT)) == NULL) {
        return -1;
    }
    return 0;
}

int esp_hosted_wifi_poll(void) {
    size_t offset = 0;
    esp_header_t *esp_header = (esp_header_t *)(esp_state.buf);
    tlv_header_t *tlv_header = (tlv_header_t *)(esp_header->payload);

    if (!(esp_state.flags & ESP_HOSTED_FLAGS_INIT) || !esp_hosted_hal_data_ready()) {
        return 0;
    }

    do {
        esp_header_t *frag_header = (esp_header_t *)(esp_state.buf + offset);
        if ((ESP_STATE_BUF_SIZE - offset) < ESP_FRAME_MAX_SIZE) {
            // This shouldn't happen, but if it did stop polling.
            error_printf("esp_hosted_poll() spi buffer overflow offs %d\n", offset);
            return -1;
        }

        if (esp_hosted_hal_spi_transfer(NULL, esp_state.buf + offset, ESP_FRAME_MAX_SIZE) != 0) {
            error_printf("esp_hosted_poll() spi transfer failed\n");
            return 0;
        }

        if (frag_header->len == 0 ||
            frag_header->len > ESP_FRAME_MAX_PAYLOAD ||
            frag_header->offset != sizeof(esp_header_t)) {
            // Invalid or empty packet, just ignore it silently.
            warn_printf("esp_hosted_poll() invalid frame size %d offset %d\n",
                esp_header->len, esp_header->offset);
            return 0;
        }

        uint16_t checksum = frag_header->checksum;
        frag_header->checksum = esp_hosted_checksum(frag_header);
        if (frag_header->checksum != checksum) {
            warn_printf("esp_hosted_poll() invalid checksum, expected %d\n", checksum);
            return 0;
        }

        if (offset) {
            // Combine fragmented packet
            if ((esp_header->seq_num + 1) != frag_header->seq_num) {
                error_printf("esp_hosted_poll() fragmented frame sequence mismatch\n");
                return 0;
            }
            esp_header->len += frag_header->len;
            esp_header->seq_num = frag_header->seq_num;
            esp_header->flags = frag_header->flags;
            info_printf("esp_hosted_poll() received fragmented packet %d\n", frag_header->len);
            // Append the current fragment's payload to the previous one.
            memcpy(esp_state.buf + offset, frag_header->payload, frag_header->len);
        }

        offset = sizeof(esp_header_t) + esp_header->len;
    } while ((esp_header->flags & ESP_FRAME_FLAGS_FRAGMENT));

    #if ESP_HOSTED_DEBUG
    esp_hosted_dump_header(esp_header);
    #endif

    switch (esp_header->if_type) {
        case ESP_HOSTED_STA_IF:
        case ESP_HOSTED_AP_IF: {
            // Networking traffic
            uint32_t itf = esp_header->if_type;
            if (netif_is_link_up(&esp_state.netif[itf])) {
                if (esp_hosted_netif_input(&esp_state, itf, esp_header->payload, esp_header->len) != 0) {
                    error_printf("esp_hosted_poll() netif input failed\n");
                    return -1;
                }
                debug_printf("esp_hosted_poll() eth frame input %d\n", esp_header->len);
            }
            return 0;
        }
        case ESP_HOSTED_PRIV_IF: {
            esp_event_t *priv_event = (esp_event_t *)(esp_header->payload);
            if (esp_header->priv_pkt_type == ESP_PACKET_TYPE_EVENT &&
                priv_event->event_type == ESP_PRIV_EVENT_INIT) {
                esp_state.chip_id = priv_event->event_data[2];
                esp_state.spi_clk = priv_event->event_data[5];
                esp_state.chip_flags = priv_event->event_data[8];
                info_printf("esp_hosted_poll() chip id %d spi_mhz %d caps 0x%x\n",
                    esp_state.chip_id, esp_state.spi_clk, esp_state.chip_flags);
            }
            return 0;
        }
        case ESP_HOSTED_HCI_IF:
        case ESP_HOSTED_TEST_IF:
        case ESP_HOSTED_MAX_IF:
            error_printf("esp_hosted_poll() unexpected interface type %d\n", esp_header->if_type);
            return 0;
        case ESP_HOSTED_SERIAL_IF:
            // Requires further processing
            break;
    }

    CtrlMsg *ctrl_msg = ctrl_msg__unpack(&protobuf_alloc, tlv_header->data_length, tlv_header->data);
    if (ctrl_msg == NULL) {
        error_printf("esp_hosted_poll() failed to unpack protobuf\n");
        return 0;
    }

    if (ctrl_msg->msg_type == CTRL_MSG_TYPE__Event) {
        switch (ctrl_msg->msg_id) {
            case CTRL_MSG_ID__Event_ESPInit:
                esp_state.flags |= ESP_HOSTED_FLAGS_ACTIVE;
                break;
            case CTRL_MSG_ID__Event_Heartbeat:
                esp_state.last_hb_ms = mp_hal_ticks_ms();
                info_printf("esp_hosted_poll() heartbeat %lu\n", esp_state.last_hb_ms);
                return 0;
            case CTRL_MSG_ID__Event_StationDisconnectFromAP:
                esp_state.flags &= ~ESP_HOSTED_FLAGS_STA_CONNECTED;
                return 0;
            case CTRL_MSG_ID__Event_StationDisconnectFromESPSoftAP:
                return 0;
            default:
                error_printf("esp_hosted_poll() unexpected event %d\n", ctrl_msg->msg_id);
                return 0;
        }
    }

    // Responses that should be handled here.
    if (ctrl_msg->msg_type == CTRL_MSG_TYPE__Resp) {
        switch (ctrl_msg->msg_id) {
            case CTRL_MSG_ID__Resp_ConnectAP: {
                if (esp_hosted_resp_value(ctrl_msg) == 0) {
                    esp_state.flags |= ESP_HOSTED_FLAGS_STA_CONNECTED;
                }
                ctrl_msg__free_unpacked(ctrl_msg, &protobuf_alloc);
                debug_printf("esp_hosted_poll() state %d\n", esp_state.flags);
                return 0;
            }
            default:
                break;
        }
    }

    // A control message resp/event will be pushed on the stack for further processing.
    if (!esp_hosted_stack_push(&esp_state.stack, ctrl_msg)) {
        error_printf("esp_hosted_poll() message stack full\n");
        return -1;
    }

    debug_printf("esp_hosted_poll() pushed msg_type %lu msg_id %lu\n", ctrl_msg->msg_type, ctrl_msg->msg_id);
    return 0;
}

int esp_hosted_wifi_init(uint32_t itf) {
    if (esp_state.flags == ESP_HOSTED_FLAGS_RESET) {
        // Init state
        memset(&esp_state, 0, sizeof(esp_hosted_state_t));
        esp_hosted_stack_init(&esp_state.stack);

        // Low-level pins and SPI init, memory pool allocation etc...
        if (esp_hosted_hal_init(ESP_HOSTED_MODE_WIFI) != 0) {
            return -1;
        }

        // Allow polling the bus.
        esp_state.flags |= ESP_HOSTED_FLAGS_INIT;

        CtrlMsg *ctrl_msg = NULL;

        // Wait for an ESPInit control event.
        ctrl_msg = esp_hosted_response(CTRL_MSG_ID__Event_ESPInit, ESP_SYNC_REQ_TIMEOUT);
        if (ctrl_msg == NULL) {
            return -1;
        }
        ctrl_msg__free_unpacked(ctrl_msg, &protobuf_alloc);

        // Set WiFi mode to STA/AP.
        CtrlMsgReqSetMode ctrl_payload;
        ctrl_msg__req__set_mode__init(&ctrl_payload);
        ctrl_payload.mode = CTRL__WIFI_MODE__APSTA;
        if (esp_hosted_ctrl(CTRL_MSG_ID__Req_SetWifiMode, &ctrl_payload, &ctrl_msg) != 0) {
            return -1;
        }
        ctrl_msg__free_unpacked(ctrl_msg, &protobuf_alloc);

        info_printf("esp_hosted_init() device initialized\n");
    }

    if (!netif_is_link_up(&esp_state.netif[itf])) {
        // Init lwip netif, and start DHCP client/server.
        esp_hosted_netif_init(&esp_state, itf);
        info_printf("esp_hosted_init() initialized itf %lu\n", itf);
    }

    // Re/enable IRQ pin.
    esp_hosted_hal_irq_enable(true);

    return 0;
}

int esp_hosted_wifi_disable(uint32_t itf) {
    // Remove netif
    esp_hosted_netif_deinit(&esp_state, itf);

    if (itf == ESP_HOSTED_STA_IF) {
        esp_state.flags &= ~ESP_HOSTED_FLAGS_STA_CONNECTED;
    } else {
        esp_state.flags &= ~ESP_HOSTED_FLAGS_AP_STARTED;
    }

    info_printf("esp_hosted_deinit() deinitialized itf %lu\n", itf);
    return 0;
}

int esp_hosted_wifi_deinit(void) {
    if (esp_state.flags & ESP_HOSTED_FLAGS_INIT) {
        // Remove network interfaces
        esp_hosted_wifi_disable(ESP_HOSTED_STA_IF);
        esp_hosted_wifi_disable(ESP_HOSTED_AP_IF);

        // Reset state
        memset(&esp_state, 0, sizeof(esp_hosted_state_t));
        esp_hosted_stack_init(&esp_state.stack);

        info_printf("esp_hosted_deinit() deinitialized\n");
    }
    return 0;
}

void *esp_hosted_wifi_get_netif(uint32_t itf) {
    return &esp_state.netif[itf];
}

int esp_hosted_wifi_get_mac(int itf, uint8_t *mac) {
    CtrlMsgReqGetMacAddress ctrl_payload;
    ctrl_msg__req__get_mac_address__init(&ctrl_payload);
    ctrl_payload.mode = (itf == ESP_HOSTED_STA_IF) ? CTRL__WIFI_MODE__STA : CTRL__WIFI_MODE__AP;

    CtrlMsg *ctrl_msg = NULL;
    if (esp_hosted_ctrl(CTRL_MSG_ID__Req_GetMACAddress, &ctrl_payload, &ctrl_msg) != 0) {
        error_printf("esp_hosted_get_mac() request failed\n");
        return -1;
    }

    ProtobufCBinaryData macstr = ctrl_msg->resp_get_mac_address->mac;
    if (macstr.data) {
        esp_hosted_macstr_to_bytes(macstr.data, macstr.len, mac);
    }
    ctrl_msg__free_unpacked(ctrl_msg, &protobuf_alloc);
    return 0;
}

int esp_hosted_wifi_connect(const char *ssid, const char *bssid, esp_hosted_security_t security, const char *key, uint16_t channel) {
    CtrlMsgReqConnectAP ctrl_payload;
    ctrl_msg__req__connect_ap__init(&ctrl_payload);

    if (security >= ESP_HOSTED_SEC_MAX) {
        // Note: this argument is otherwise unused(!)
        return -1;
    }

    ctrl_payload.ssid = (char *)ssid;
    ctrl_payload.bssid = (char *)bssid;
    ctrl_payload.pwd = (char *)key;
    ctrl_payload.is_wpa3_supported = false;
    ctrl_payload.listen_interval = 0;

    if (esp_hosted_request(CTRL_MSG_ID__Req_ConnectAP, &ctrl_payload) != 0) {
        return -1;
    }
    return 0;
}

int esp_hosted_wifi_start_ap(const char *ssid, esp_hosted_security_t security, const char *key, uint16_t channel) {
    CtrlMsgReqStartSoftAP ctrl_payload;
    ctrl_msg__req__start_soft_ap__init(&ctrl_payload);

    if (security >= ESP_HOSTED_SEC_MAX) {
        return -1;
    }

    ctrl_payload.ssid = (char *)ssid;
    ctrl_payload.pwd = (char *)key;
    ctrl_payload.chnl = channel;
    ctrl_payload.sec_prot = hosted_security_to_sec_prot(security);
    ctrl_payload.max_conn = ESP_HOSTED_MAX_AP_CLIENTS;
    ctrl_payload.ssid_hidden = false;
    ctrl_payload.bw = CTRL__WIFI_BW__HT40;

    CtrlMsg *ctrl_msg = NULL;
    if (esp_hosted_ctrl(CTRL_MSG_ID__Req_StartSoftAP, &ctrl_payload, &ctrl_msg) != 0) {
        return -1;
    }
    ctrl_msg__free_unpacked(ctrl_msg, &protobuf_alloc);
    esp_state.flags |= ESP_HOSTED_FLAGS_AP_STARTED;
    return 0;
}

int esp_hosted_wifi_disconnect(uint32_t itf) {
    CtrlMsg *ctrl_msg = NULL;
    CtrlMsgReqGetStatus ctrl_payload;
    ctrl_msg__req__get_status__init(&ctrl_payload);

    if (itf == ESP_HOSTED_STA_IF) {
        esp_state.flags &= ~ESP_HOSTED_FLAGS_STA_CONNECTED;
        if (esp_hosted_ctrl(CTRL_MSG_ID__Req_DisconnectAP, &ctrl_payload, &ctrl_msg) != 0) {
            return -1;
        }
    } else {
        esp_state.flags &= ~ESP_HOSTED_FLAGS_AP_STARTED;
        if (esp_hosted_ctrl(CTRL_MSG_ID__Req_StopSoftAP, &ctrl_payload, &ctrl_msg) != 0) {
            return -1;
        }
    }
    ctrl_msg__free_unpacked(ctrl_msg, &protobuf_alloc);
    return 0;
}

int esp_hosted_wifi_link_status(uint32_t itf) {
    return netif_is_link_up(&esp_state.netif[itf]);
}

int esp_hosted_wifi_is_connected(uint32_t itf) {
    if (!esp_hosted_wifi_link_status(itf)) {
        return false;
    }
    if (itf == ESP_HOSTED_AP_IF) {
        return esp_state.flags & ESP_HOSTED_FLAGS_AP_STARTED;
    }
    if ((esp_state.flags & ESP_HOSTED_FLAGS_STA_CONNECTED) &&
        ((esp_state.flags & ESP_HOSTED_FLAGS_STATIC_IP) ||
         dhcp_supplied_address(&esp_state.netif[itf]))) {
        return true;
    }
    return false;
}

int esp_hosted_wifi_get_stations(uint8_t *sta_list, size_t *sta_count) {
    CtrlMsgReqSoftAPConnectedSTA ctrl_payload;
    ctrl_msg__req__soft_apconnected_sta__init(&ctrl_payload);

    CtrlMsg *ctrl_msg = NULL;
    if (esp_hosted_ctrl(CTRL_MSG_ID__Req_GetSoftAPConnectedSTAList, &ctrl_payload, &ctrl_msg) != 0) {
        return -1;
    }

    CtrlMsgRespSoftAPConnectedSTA *resp = ctrl_msg->resp_softap_connected_stas_list;
    *sta_count = resp->n_stations;
    for (size_t i = 0; i < resp->n_stations; i++) {
        ProtobufCBinaryData mac = resp->stations[i]->mac;
        esp_hosted_macstr_to_bytes(mac.data, mac.len, &sta_list[i * 6]);
    }
    ctrl_msg__free_unpacked(ctrl_msg, &protobuf_alloc);
    return 0;
}

int esp_hosted_wifi_netinfo(esp_hosted_netinfo_t *netinfo) {
    CtrlMsgReqGetAPConfig ctrl_payload;
    ctrl_msg__req__get_apconfig__init(&ctrl_payload);

    CtrlMsg *ctrl_msg = NULL;
    if (esp_hosted_ctrl(CTRL_MSG_ID__Req_GetAPConfig, &ctrl_payload, &ctrl_msg) != 0) {
        return -1;
    }

    netinfo->rssi = ctrl_msg->resp_get_ap_config->rssi;
    netinfo->security = sec_prot_to_hosted_security(ctrl_msg->resp_get_ap_config->sec_prot);
    netinfo->channel = ctrl_msg->resp_get_ap_config->chnl;

    ProtobufCBinaryData ssid = ctrl_msg->resp_get_ap_config->ssid;
    if (ssid.data) {
        size_t ssid_len = MIN(ssid.len, (ESP_HOSTED_MAX_SSID_LEN - 1));
        memcpy(netinfo->ssid, ssid.data, ssid_len);
        netinfo->ssid[ssid_len] = 0;
    }

    ProtobufCBinaryData bssid = ctrl_msg->resp_get_ap_config->bssid;
    if (bssid.data) {
        esp_hosted_macstr_to_bytes(bssid.data, bssid.len, netinfo->bssid);
    }

    ctrl_msg__free_unpacked(ctrl_msg, &protobuf_alloc);
    return 0;
}

int esp_hosted_wifi_scan(esp_hosted_scan_callback_t scan_callback, void *arg, uint32_t timeout) {
    CtrlMsgReqScanResult ctrl_payload;
    ctrl_msg__req__scan_result__init(&ctrl_payload);

    CtrlMsg *ctrl_msg = NULL;
    if (esp_hosted_ctrl(CTRL_MSG_ID__Req_GetAPScanList, &ctrl_payload, &ctrl_msg) != 0) {
        return -MP_ETIMEDOUT;
    }

    CtrlMsgRespScanResult *rp = ctrl_msg->resp_scan_ap_list;
    for (int i = 0; i < rp->count; i++) {
        esp_hosted_scan_result_t result = {0};
        result.rssi = rp->entries[i]->rssi;
        result.security = sec_prot_to_hosted_security(rp->entries[i]->sec_prot);
        result.channel = rp->entries[i]->chnl;
        if (rp->entries[i]->bssid.data) {
            esp_hosted_macstr_to_bytes(rp->entries[i]->bssid.data, rp->entries[i]->bssid.len, result.bssid);
        }

        if (rp->entries[i]->ssid.len) {
            size_t ssid_len = MIN(rp->entries[i]->ssid.len, (ESP_HOSTED_MAX_SSID_LEN - 1));
            memcpy(result.ssid, rp->entries[i]->ssid.data, ssid_len);
            result.ssid[ssid_len] = 0;
        }
        scan_callback(&result, arg);
    }

    ctrl_msg__free_unpacked(ctrl_msg, &protobuf_alloc);
    return 0;
}
#endif // MICROPY_PY_NETWORK_ESP_HOSTED

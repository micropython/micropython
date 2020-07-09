/*
  This file is part of the ArduinoBLE library.
  Copyright (c) 2018 Arduino SA. All rights reserved.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "hci_api.h"

#include "py/obj.h"

// Zephyr include files to define HCI communication values and structs.
#include "hci_include/hci.h"
#include "hci_include/hci_err.h"

#include <string.h>

#include "supervisor/shared/tick.h"
#include "shared-bindings/_bleio/__init__.h"
#include "common-hal/_bleio/Adapter.h"

// HCI H4 protocol packet types: first byte in the packet.
#define H4_CMD 0x01
#define H4_ACL 0x02
#define H4_SCO 0x03
#define H4_EVT 0x04

//FIX replace
#define ATT_CID       0x0004

#define sizeof_field(TYPE, MEMBER) sizeof((((TYPE *)0)->MEMBER))

#define RX_BUFFER_SIZE (3 + 255)
#define ACL_PKT_BUFFER_SIZE (255)

#define CTS_TIMEOUT_MSECS (1000)
#define RESPONSE_TIMEOUT_MSECS (1000)

#define adapter (&common_hal_bleio_adapter_obj)

STATIC uint8_t rx_buffer[RX_BUFFER_SIZE];
STATIC size_t rx_idx;

STATIC size_t num_command_packets_allowed;
STATIC size_t max_pkt;
STATIC size_t pending_pkt;

// Results from parsing a command response packet.
STATIC bool cmd_response_received;
STATIC uint16_t cmd_response_opcode;
STATIC uint8_t cmd_response_status;
STATIC size_t cmd_response_len;
STATIC uint8_t* cmd_response_data;

//FIX STATIC uint8_t acl_pkt_buffer[ACL_PKT_BUFFER_SIZE];

STATIC bool debug = true;

// These are the headers of the full packets that are sent over the serial interface.
// They all have a one-byte type-field at the front, one of the H4_xxx packet types.

typedef struct __attribute__ ((packed)) {
        uint8_t pkt_type;
        uint16_t opcode;
        uint8_t param_len;
} h4_hci_cmd_hdr_t;

typedef struct __attribute__ ((packed)) {
    uint8_t pkt_type;
    uint16_t handle;
    uint16_t total_data_len;
    uint16_t acl_data_len;
    uint16_t cid;
}  h4_hci_acl_hdr_t;

typedef struct __attribute__ ((packed)) {
    uint8_t pkt_type;
    uint8_t evt;
    uint8_t param_len;
} h4_hci_evt_hdr_t;


STATIC void dump_cmd_pkt(bool tx, uint8_t pkt_len, uint8_t pkt_data[]) {
    if (debug) {
        h4_hci_cmd_hdr_t *pkt = (h4_hci_cmd_hdr_t *) pkt_data;
        mp_printf(&mp_plat_print,
                  "%s HCI COMMAND (%x) opcode: %04x, len: %d, data: ",
                  tx ? "TX->" : "RX<-",
                  pkt->pkt_type, pkt->opcode, pkt->param_len);
        uint8_t i;
        for (i = sizeof(h4_hci_cmd_hdr_t); i < pkt_len; i++) {
            mp_printf(&mp_plat_print, "%02x ", pkt_data[i]);
        }
        if (i != pkt->param_len + sizeof(h4_hci_cmd_hdr_t)) {
            mp_printf(&mp_plat_print, "  LENGTH MISMATCH");
        }
        mp_printf(&mp_plat_print, "\n");
    }
}

STATIC void dump_acl_pkt(bool tx, uint8_t pkt_len, uint8_t pkt_data[]) {
    if (debug) {
        h4_hci_acl_hdr_t *pkt = (h4_hci_acl_hdr_t *) pkt_data;
        mp_printf(&mp_plat_print,
                  "%s HCI ACLDATA (%x) handle: %04x, total_data_len: %d, acl_data_len: %d,  cid: %04x,  data: ",
                  tx ? "TX->" : "RX<-",
                  pkt->pkt_type, pkt->handle, pkt->total_data_len, pkt->acl_data_len, pkt->cid);
        uint8_t i;
        for (i = sizeof(h4_hci_acl_hdr_t); i < pkt_len; i++) {
            mp_printf(&mp_plat_print, "%02x ", pkt_data[i]);
        }
        if (i != pkt->acl_data_len + sizeof(h4_hci_acl_hdr_t)) {
            mp_printf(&mp_plat_print, "  LENGTH MISMATCH");
        }
        mp_printf(&mp_plat_print, "\n");
    }
}

STATIC void dump_evt_pkt(bool tx, uint8_t pkt_len, uint8_t pkt_data[]) {
    if (debug) {
        h4_hci_evt_hdr_t *pkt = (h4_hci_evt_hdr_t *) pkt_data;
        mp_printf(&mp_plat_print,
                  "%s HCI EVENT   (%x) evt: %02x,  param_len: %d,  data: ",
                  tx ? "TX->" : "RX<-",
                  pkt->pkt_type, pkt->evt, pkt->param_len);
        uint8_t i;
        for (i = sizeof(h4_hci_evt_hdr_t); i < pkt_len; i++) {
            mp_printf(&mp_plat_print, "%02x ", pkt_data[i]);
        }
        if (i != pkt->param_len + sizeof(h4_hci_evt_hdr_t)) {
            mp_printf(&mp_plat_print, "  LENGTH MISMATCH");
        }
        mp_printf(&mp_plat_print, "\n");
    }
}

STATIC void process_acl_data_pkt(uint8_t pkt_len, uint8_t pkt_data[]) {
    //FIX pkt_len is +1 than before, because it includes the pkt_type.
    // h4_hci_acl_hdr_t *aclHdr = (h4_hci_acl_hdr_t*)pkt_data;

    // uint16_t aclFlags = (aclHdr->handle & 0xf000) >> 12;

    // if ((aclHdr->data_len - 4) != aclHdr->len) {
    //     // packet is fragmented
    //     if (aclFlags != 0x01) {
    //         // copy into ACL buffer
    //         memcpy(acl_pkt_buffer, &rx_buffer[1], sizeof(HCIACLHdr) + aclHdr->data_len - 4);
    //     } else {
    //         // copy next chunk into the buffer
    //         HCIACLHdr* aclBufferHeader = (HCIACLHdr*)acl_pkt_buffer;

    //         memcpy(&acl_pkt_buffer[sizeof(HCIACLHdr) + aclBufferHeader->data_len - 4], &rx_buffer[1 + sizeof(aclHdr->handle) + sizeof(aclHdr->data_len)], aclHdr->data_len);

    //         aclBufferHeader->data_len += aclHdr->data_len;
    //         aclHdr = aclBufferHeader;
    //     }
    // }

    // if ((aclHdr->data_len - 4) != aclHdr->len) {
    //     // don't have the full packet yet
    //     return;
    // }

    // if (aclHdr->cid == ATT_CID) {
    //     if (aclFlags == 0x01) {
    //         // use buffered packet
    //         ATT.handleData(aclHdr->handle & 0x0fff, aclHdr->len, &acl_pkt_buffer[sizeof(HCIACLHdr)]);
    //     } else {
    //         // use the rx buffer
    //         ATT.handleData(aclHdr->handle & 0x0fff, aclHdr->len, &rx_buffer[1 + sizeof(HCIACLHdr)]);
    //     }
    // } else if (aclHdr->cid == SIGNALING_CID) {
    //     L2CAPSignaling.handleData(aclHdr->handle & 0x0fff, aclHdr->len, &rx_buffer[1 + sizeof(HCIACLHdr)]);
    // } else {
    //     struct __attribute__ ((packed)) {
    //         uint8_t op;
    //         uint8_t id;
    //         uint16_t length;
    //         uint16_t reason;
    //         uint16_t localCid;
    //         uint16_t remoteCid;
    //     } l2capRejectCid= { 0x01, 0x00, 0x006, 0x0002, aclHdr->cid, 0x0000 };

    //     sendAclPkt(aclHdr->handle & 0x0fff, 0x0005, sizeof(l2capRejectCid), &l2capRejectCid);
    // }
}

// Process number of completed packets. Reduce number of pending packets by reported
// number of completed.
STATIC void process_num_comp_pkts(uint16_t handle, uint16_t num_pkts) {
    if (num_pkts && pending_pkt > num_pkts) {
        pending_pkt -= num_pkts;
    } else {
        pending_pkt = 0;
    }
}

STATIC void process_evt_pkt(size_t pkt_len, uint8_t pkt[])
{
    h4_hci_evt_hdr_t *evt_hdr = (h4_hci_evt_hdr_t*) pkt;
    // The data itself, after the header.
    uint8_t *evt_data = pkt + sizeof(h4_hci_evt_hdr_t);

    switch (evt_hdr->evt) {
        case BT_HCI_EVT_DISCONN_COMPLETE: {
            struct bt_hci_evt_disconn_complete *disconn_complete = (struct bt_hci_evt_disconn_complete*) evt_data;
            (void) disconn_complete;
            //FIX
            // ATT.removeConnection(disconn_complete->handle, disconn_complete->reason);
            // L2CAPSignaling.removeConnection(disconn_complete->handle, disconn_complete->reason);
            hci_le_set_advertise_enable(0x01);
            break;
        }

        case BT_HCI_EVT_CMD_COMPLETE: {

            struct cmd_complete_with_status {
                struct bt_hci_evt_cmd_complete cmd_complete;
                struct bt_hci_evt_cc_status cc_status;
            } __packed;

            struct cmd_complete_with_status *evt = (struct cmd_complete_with_status *) evt_data;

            num_command_packets_allowed = evt->cmd_complete.ncmd;

            cmd_response_received = true;
            cmd_response_opcode = evt->cmd_complete.opcode;
            cmd_response_status = evt->cc_status.status;
            // All the bytes following cmd_complete, -including- the status byte, which is
            // included in all the _bt_hci_rp_* structs.
            cmd_response_data = &evt_data[sizeof_field(struct cmd_complete_with_status, cmd_complete)];
            // Includes status byte.
            cmd_response_len = evt_hdr->param_len - sizeof_field(struct cmd_complete_with_status, cmd_complete);

            break;
        }

        case BT_HCI_EVT_CMD_STATUS: {
            struct bt_hci_evt_cmd_status *evt = (struct bt_hci_evt_cmd_status *) evt_data;

            num_command_packets_allowed = evt->ncmd;

            cmd_response_received = true;
            cmd_response_opcode = evt->opcode;
            cmd_response_status = evt->status;
            cmd_response_data = NULL;
            cmd_response_len = 0;

            break;
        }

        case BT_HCI_EVT_NUM_COMPLETED_PACKETS: {
            struct bt_hci_evt_num_completed_packets *evt = (struct bt_hci_evt_num_completed_packets *) evt_data;

            // Start at zero-th pair: (conn handle, num completed packets).
            struct bt_hci_handle_count *handle_and_count = &(evt->h[0]);
            for (uint8_t i = 0; i < evt->num_handles; i++) {
                process_num_comp_pkts(handle_and_count->handle, handle_and_count->count);
                handle_and_count++;
            }
            break;
        }

        case BT_HCI_EVT_LE_META_EVENT: {
            struct bt_hci_evt_le_meta_event *meta_evt = (struct bt_hci_evt_le_meta_event *) evt_data;
            // Start of the encapsulated LE event.
            uint8_t *le_evt = evt_data + sizeof (struct bt_hci_evt_le_meta_event);

            if (meta_evt->subevent == BT_HCI_EVT_LE_CONN_COMPLETE) {
                struct bt_hci_evt_le_conn_complete *le_conn_complete =
                    (struct bt_hci_evt_le_conn_complete *) le_evt;

                if (le_conn_complete->status == 0x00) {
                    // ATT.addConnection(le_conn_complete->handle,
                    //                   le_conn_complete->role,
                    //                   le_conn_complete->peer_addr  //FIX struct
                    //                   le_conn_complete->interval,
                    //                   le_conn_complete->latency,
                    //                   le_conn_complete->supv_timeout
                    //                   le_conn_complete->clock_accuracy);

                    // L2CAPSignaling.addConnection(le_conn_complete->handle,
                    //                              le_conn_complete->role,
                    //                              le_conn_complete->peer_addr, //FIX struct
                    //                              le_conn_complete->interval,
                    //                              le_conn_complete->latency,
                    //                              le_conn_complete->supv_timeout,
                    //                              le_conn_complete->clock_accuracy);
                }
            } else if (meta_evt->subevent == BT_HCI_EVT_LE_ADVERTISING_REPORT) {
                struct bt_hci_evt_le_advertising_info *le_advertising_info =
                    (struct bt_hci_evt_le_advertising_info *) le_evt;
                if (le_advertising_info->evt_type == BT_HCI_ADV_DIRECT_IND) {  //FIX handle kind of advertising
                    // last byte is RSSI
                    // GAP.handleLeAdvertisingReport(leAdvertisingReport->type,
                    //                               leAdvertisingReport->peerBdaddrType,
                    //                               leAdvertisingReport->peerBdaddr,
                    //                               leAdvertisingReport->eirLength,
                    //                               leAdvertisingReport->eirData,
                    //                               rssi); //FIX, don't separate

                }
            }
            break;
        }

        default:
            break;
    }
}

void hci_init(void) {
    rx_idx = 0;
    pending_pkt = 0;
}

hci_result_t hci_poll_for_incoming_pkt(void) {
    // Assert RTS low to say we're ready to read data.
    common_hal_digitalio_digitalinout_set_value(adapter->rts_digitalinout, false);

    int errcode = 0;
    bool packet_is_complete = false;

    // Read bytes until we run out, or accumulate a complete packet.
    while (common_hal_busio_uart_rx_characters_available(adapter->hci_uart)) {
        common_hal_busio_uart_read(adapter->hci_uart, rx_buffer + rx_idx, 1, &errcode);
        if (errcode) {
            return HCI_READ_ERROR;
        }
        rx_idx++;

        switch (rx_buffer[0]) {
            case H4_ACL:
                if (rx_idx > sizeof(h4_hci_acl_hdr_t) &&
                    rx_idx >= sizeof(h4_hci_acl_hdr_t) + ((h4_hci_acl_hdr_t *) rx_buffer)->total_data_len) {
                    packet_is_complete = true;
                }
                break;

            case H4_EVT:
                if (rx_idx > sizeof(h4_hci_evt_hdr_t) &&
                    rx_idx >= sizeof(h4_hci_evt_hdr_t) + ((h4_hci_evt_hdr_t *) rx_buffer)->param_len) {
                    packet_is_complete = true;
                }
                break;

            default:
                // Unknown or bad packet type. Start over.
                rx_idx = 0;
                break;
        }
    }

    if (!packet_is_complete) {
        return HCI_OK;
    }

    // Stop incoming data while processing packet.
    common_hal_digitalio_digitalinout_set_value(adapter->rts_digitalinout, true);
    size_t pkt_len = rx_idx;
    // Reset for next pack
    rx_idx = 0;

    switch (rx_buffer[0]) {
        case H4_ACL:
            if (debug) {
                dump_acl_pkt(false, pkt_len, rx_buffer);
            }

            process_acl_data_pkt(pkt_len, rx_buffer);
            break;

        case H4_EVT:
            if (debug) {
                dump_evt_pkt(false, pkt_len, rx_buffer);
            }

            process_evt_pkt(pkt_len, rx_buffer);
            break;

        default:
            break;
    }

    common_hal_digitalio_digitalinout_set_value(adapter->rts_digitalinout, true);

    return HCI_OK;
}


// Returns
STATIC hci_result_t write_pkt(uint8_t *buffer, size_t len) {
    // Wait for CTS to go low before writing to HCI adapter.
    uint64_t start = supervisor_ticks_ms64();

    while (common_hal_digitalio_digitalinout_get_value(adapter->cts_digitalinout)) {
        RUN_BACKGROUND_TASKS;
        if (supervisor_ticks_ms64() - start > CTS_TIMEOUT_MSECS) {
            return HCI_WRITE_TIMEOUT;
        }
    }

    int errcode = 0;
    common_hal_busio_uart_write(adapter->hci_uart, buffer, len, &errcode);
    if (errcode) {
        return HCI_WRITE_ERROR;
    }

    return HCI_OK;
}

STATIC hci_result_t send_command(uint16_t opcode, uint8_t params_len, void* params) {
    uint8_t tx_buffer[sizeof(h4_hci_cmd_hdr_t) + params_len];

    // cmd header is at the beginning of tx_buffer
    h4_hci_cmd_hdr_t *cmd_hdr = (h4_hci_cmd_hdr_t *) tx_buffer;
    cmd_hdr->pkt_type = H4_CMD;
    cmd_hdr->opcode = opcode;
    cmd_hdr->param_len = params_len;

    // Copy the params data into the space after the header.
    memcpy(&tx_buffer[sizeof(h4_hci_cmd_hdr_t)], params, params_len);

    if (debug) {
        dump_cmd_pkt(true, sizeof(tx_buffer), tx_buffer);
    }

    int result = write_pkt(tx_buffer, sizeof(h4_hci_cmd_hdr_t) + params_len);
    if (result != HCI_OK) {
        return result;
    }

    cmd_response_received = false;

    // Wait for a response. Note that other packets may be received that are not
    // command responses.
    uint64_t start = supervisor_ticks_ms64();
    while (1) {
        result = hci_poll_for_incoming_pkt();
        if (result != HCI_OK) {
            // I/O error.
            return result;
        }

        if (cmd_response_received && cmd_response_opcode == opcode) {
            // If this is definitely a response to the command that was sent,
            // return the status value, which will will be
            // BT_HCI_ERR_SUCCESS (0x00) if the command succeeded,
            // or a BT_HCI_ERR_x value (> 0x00) if there ws a problem.
            return cmd_response_status;
        }

        if (supervisor_ticks_ms64() - start > RESPONSE_TIMEOUT_MSECS) {
            return HCI_READ_TIMEOUT;
        }
        RUN_BACKGROUND_TASKS;
    }

    // No I/O error, but no response sent back in time.
    return HCI_NO_RESPONSE;
}

//FIX remove unused
STATIC int __attribute__((unused)) send_acl_pkt(uint16_t handle, uint8_t cid, void* data, uint8_t data_len) {
    int result;
    while (pending_pkt >= max_pkt) {
        result = hci_poll_for_incoming_pkt();
        if (result != HCI_OK) {
            return result;
        }
    }

    // data_len does not include cid.
    const size_t cid_len = sizeof_field(h4_hci_acl_hdr_t, cid);
    // buf_len is size of entire packet including header.
    const size_t buf_len = sizeof(h4_hci_acl_hdr_t) + cid_len + data_len;
    uint8_t tx_buffer[buf_len];

    h4_hci_acl_hdr_t *acl_hdr = (h4_hci_acl_hdr_t *) tx_buffer;
    acl_hdr->pkt_type = H4_ACL;
    acl_hdr->handle = handle;
    acl_hdr->total_data_len = (uint8_t)(cid_len + data_len);
    acl_hdr->acl_data_len = (uint8_t) data_len;
    acl_hdr->cid = cid;

    memcpy(&tx_buffer[sizeof(h4_hci_acl_hdr_t)], data, data_len);

    if (debug) {
        dump_acl_pkt(true, buf_len, tx_buffer);
    }

    pending_pkt++;

    int errcode = 0;
    common_hal_busio_uart_write(adapter->hci_uart, tx_buffer, buf_len, &errcode);
    if (errcode) {
        return HCI_WRITE_ERROR;
    }

    return HCI_OK;
}

hci_result_t hci_reset(void) {
    return send_command(BT_HCI_OP_RESET, 0, NULL);
}

hci_result_t hci_read_local_version(uint8_t *hci_version, uint16_t *hci_revision, uint8_t *lmp_version, uint16_t *manufacturer, uint16_t *lmp_subversion) {
    hci_result_t result = send_command(BT_HCI_OP_READ_LOCAL_VERSION_INFO, 0, NULL);
    if (result == HCI_OK) {
        struct bt_hci_rp_read_local_version_info *response =
            (struct bt_hci_rp_read_local_version_info *) cmd_response_data;
        *hci_version = response->hci_version;
        *hci_revision = response->hci_revision;
        *lmp_version = response->lmp_version;
        *manufacturer = response->manufacturer;
        *lmp_subversion = response->lmp_subversion;
    }

    return result;
}

hci_result_t hci_read_bd_addr(bt_addr_t *addr) {
    int result = send_command(BT_HCI_OP_READ_BD_ADDR, 0, NULL);
    if (result == HCI_OK) {
        struct bt_hci_rp_read_bd_addr *response = (struct bt_hci_rp_read_bd_addr *) cmd_response_data;
        memcpy(addr->val, response->bdaddr.val, sizeof_field(bt_addr_t, val));
    }

    return result;
}

hci_result_t hci_read_rssi(uint16_t handle, int *rssi) {
    int result = send_command(BT_HCI_OP_READ_RSSI, sizeof(handle), &handle);
    if (result == HCI_OK) {
        struct bt_hci_rp_read_rssi *response = (struct bt_hci_rp_read_rssi *) cmd_response_data;
        if (response->handle != handle) {
            // Handle doesn't match.
            return HCI_NO_RESPONSE;
        }

        *rssi = response->rssi;
    }

    return result;
}

hci_result_t hci_set_evt_mask(uint64_t event_mask) {
    return send_command(BT_HCI_OP_SET_EVENT_MASK, sizeof(event_mask), &event_mask);
}

hci_result_t hci_read_le_buffer_size(uint16_t *le_max_len, uint8_t *le_max_num) {
    int result = send_command(BT_HCI_OP_LE_READ_BUFFER_SIZE, 0, NULL);
    if (result == HCI_OK) {
        struct bt_hci_rp_le_read_buffer_size *response =
            (struct bt_hci_rp_le_read_buffer_size *) cmd_response_data;
        *le_max_len = response->le_max_len;
        *le_max_num = response->le_max_num;
    }

    return result;
}

hci_result_t hci_le_set_random_address(uint8_t addr[6]) {
    return send_command(BT_HCI_OP_LE_SET_RANDOM_ADDRESS, 6, addr);
}

hci_result_t hci_le_set_advertising_parameters(uint16_t min_interval, uint16_t max_interval, uint8_t type, uint8_t own_addr_type, bt_addr_le_t *direct_addr, uint8_t channel_map, uint8_t filter_policy) {
    struct bt_hci_cp_le_set_adv_param params = {
        .min_interval = min_interval,
        .max_interval = max_interval,
        .type = type,
        .own_addr_type = own_addr_type,
        // .direct_addr set below.
        .channel_map = channel_map,
        .filter_policy = filter_policy,
    };
    params.direct_addr.type = direct_addr->type;
    memcpy(params.direct_addr.a.val, direct_addr->a.val, sizeof(params.direct_addr.a.val));

    return send_command(BT_HCI_OP_LE_SET_ADV_PARAM, sizeof(params), &params);
}

hci_result_t hci_le_read_maximum_advertising_data_length(int *max_adv_data_len) {
    int result = send_command(BT_HCI_OP_LE_READ_MAX_ADV_DATA_LEN, 0, NULL);
    if (result == HCI_OK) {
        struct bt_hci_rp_le_read_max_adv_data_len *response =
            (struct bt_hci_rp_le_read_max_adv_data_len *) cmd_response_data;
        if (response->status == BT_HCI_ERR_SUCCESS) {
            *max_adv_data_len = response->max_adv_data_len;
        }
    }

    return result;
}

hci_result_t hci_le_set_advertising_data(uint8_t len, uint8_t data[]) {
    struct bt_hci_cp_le_set_adv_data params = {
        // Zero out unused data bytes.
        .data = { 0 },
    };

    params.len = len;
    memcpy(params.data, data, len);

    // All data bytes are sent even if some are unused.
    return send_command(BT_HCI_OP_LE_SET_ADV_DATA, sizeof(params), &params);
}

hci_result_t hci_le_set_scan_response_data(uint8_t len, uint8_t data[]) {
    struct bt_hci_cp_le_set_scan_rsp_data params = {
        // Zero out unused data bytes.
        .data = { 0 },
    };
    params.len = len;
    memcpy(params.data, data, len);

    // All data bytes are sent even if some are unused.
    return send_command(BT_HCI_OP_LE_SET_SCAN_RSP_DATA, sizeof(params), &params);
}

hci_result_t hci_le_set_advertise_enable(uint8_t enable) {
    return send_command(BT_HCI_OP_LE_SET_ADV_ENABLE, sizeof(enable), &enable);
}

hci_result_t hci_le_set_scan_parameters(uint8_t scan_type, uint16_t interval, uint16_t window, uint8_t addr_type, uint8_t filter_policy) {
    struct bt_hci_cp_le_set_scan_param params = {
        .scan_type = scan_type,
        .interval = interval,
        .window = window,
        .addr_type = addr_type,
        .filter_policy = filter_policy,
    };

    return send_command(BT_HCI_OP_LE_SET_SCAN_PARAM, sizeof(params), &params);
}

hci_result_t hci_le_set_scan_enable(uint8_t enable, uint8_t filter_dup) {
    struct bt_hci_cp_le_set_scan_enable params = {
        .enable = enable,
        .filter_dup = filter_dup,
    };

    return send_command(BT_HCI_OP_LE_SET_SCAN_ENABLE, sizeof(params), &params);
}

hci_result_t hci_le_create_conn(uint16_t scan_interval, uint16_t scan_window, uint8_t filter_policy, bt_addr_le_t *peer_addr, uint8_t own_addr_type, uint16_t conn_interval_min, uint16_t conn_interval_max, uint16_t conn_latency, uint16_t supervision_timeout, uint16_t min_ce_len, uint16_t max_ce_len) {
    struct bt_hci_cp_le_create_conn params = {
        .scan_interval = scan_interval,
        .scan_window = scan_window,
        .filter_policy = filter_policy,
        // .peer_addr is set below
        .own_addr_type = own_addr_type,
        .conn_interval_min = conn_interval_min,
        .conn_interval_max = conn_interval_max,
        .conn_latency = conn_latency,
        .supervision_timeout = supervision_timeout,
        .min_ce_len = min_ce_len,
        .max_ce_len = max_ce_len,
    };
    params.peer_addr.type = peer_addr->type;
    memcpy(params.peer_addr.a.val, peer_addr->a.val, sizeof(params.peer_addr.a.val));

    return send_command(BT_HCI_OP_LE_CREATE_CONN, sizeof(params), &params);
}

hci_result_t hci_le_cancel_conn(void) {
    return send_command(BT_HCI_OP_CONNECT_CANCEL, 0, NULL);
}

hci_result_t hci_le_conn_update(uint16_t handle, uint16_t conn_interval_min, uint16_t conn_interval_max, uint16_t conn_latency, uint16_t supervision_timeout) {
    struct hci_cp_le_conn_update params = {
        .handle = handle,
        .conn_interval_min = conn_interval_min,
        .conn_interval_max = conn_interval_max,
        .conn_latency = conn_latency,
        .supervision_timeout = supervision_timeout,
        .min_ce_len = 4,
        .max_ce_len = 6,
    };

    return send_command(BT_HCI_OP_LE_CONN_UPDATE, sizeof(params), &params);
}

hci_result_t hci_disconnect(uint16_t handle) {
    struct bt_hci_cp_disconnect params = {
        .handle = handle,
        .reason = BT_HCI_ERR_REMOTE_USER_TERM_CONN,
    };

    return send_command(BT_HCI_OP_DISCONNECT, sizeof(params), &params);
}

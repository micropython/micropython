//  This file is derived from the ArduinoBLE library. Its header is below.
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

#include "att.h"
#include "hci.h"

#include "py/obj.h"

// Zephyr include files to define HCI communication values and structs.
#include "hci_include/hci.h"
#include "hci_include/hci_err.h"
#include "hci_include/l2cap_internal.h"

#include <string.h>

#include "supervisor/shared/tick.h"
#include "shared-bindings/_bleio/__init__.h"
#include "common-hal/_bleio/Adapter.h"
#include "shared-bindings/microcontroller/__init__.h"


// HCI H4 protocol packet types: first byte in the packet.
#define H4_CMD 0x01
#define H4_ACL 0x02
#define H4_SCO 0x03
#define H4_EVT 0x04

#define CTS_TIMEOUT_MSECS (1000)
#define RESPONSE_TIMEOUT_MSECS (1000)

#define adapter (&common_hal_bleio_adapter_obj)

// These are the headers of the full packets that are sent over the serial interface.
// They all have a one-byte type-field at the front, one of the H4_xxx packet types.

typedef struct __attribute__ ((packed)) {
    uint8_t pkt_type;
    uint16_t opcode;
    uint8_t param_len;
    uint8_t params[];
} h4_hci_cmd_pkt_t;

#define ACL_DATA_PB_FIRST_NON_FLUSH  0
#define ACL_DATA_PB_MIDDLE           1
#define ACL_DATA_PB_FIRST_FLUSH      2
#define ACL_DATA_PB_FULL             3

typedef struct __attribute__ ((packed)) {
    uint8_t pkt_type;
    uint16_t handle : 12;
    uint8_t pb: 2;            // Packet boundary flag: ACL_DATA_PB values.
    uint8_t bc: 2;            // Broadcast flag: always 0b00 for BLE.
    uint16_t data_len;        // length of data[] in this packet.
    uint8_t data[];
}  h4_hci_acl_pkt_t;

// The ACL data in an h4_hci_acl_pkt_t may be fragmented across
// multiple ACL_DATA packets, and need to be recombined.  This is the
// structure of the combined packet or the first fragment.
typedef struct __attribute__ ((packed)) {
    uint16_t acl_data_len;  // Length of acl_data. Does not include this header.
    uint16_t cid;           // Channel ID.
    uint8_t acl_data[];     // Length is acl_data_len of full packet.
} acl_data_t;

typedef struct __attribute__ ((packed)) {
    uint8_t pkt_type;
    uint8_t evt;
    uint8_t param_len;
    uint8_t params[];
} h4_hci_evt_pkt_t;


//////////////////////////////////////////////////////////////////////
// Static storage:

//FIX size
#define RX_BUFFER_SIZE (3 + 255)
#define ACL_DATA_BUFFER_SIZE (255)

STATIC uint8_t rx_buffer[RX_BUFFER_SIZE];
STATIC size_t rx_idx;

STATIC uint8_t acl_data_buffer[ACL_DATA_BUFFER_SIZE];
STATIC size_t acl_data_len;

STATIC size_t num_command_packets_allowed;
STATIC size_t max_pkt;
STATIC size_t pending_pkt;

// Results from parsing a command response packet.
STATIC bool cmd_response_received;
STATIC uint16_t cmd_response_opcode;
STATIC uint8_t cmd_response_status;
STATIC size_t cmd_response_len;
STATIC uint8_t* cmd_response_data;

STATIC volatile bool hci_poll_in_progress = false;

STATIC bool debug = true;

//////////////////////////////////////////////////////////////////////

STATIC void dump_cmd_pkt(bool tx, uint8_t pkt_len, uint8_t pkt_data[]) {
    if (debug) {
        h4_hci_cmd_pkt_t *pkt = (h4_hci_cmd_pkt_t *) pkt_data;
        mp_printf(&mp_plat_print,
                  "%s HCI COMMAND (%x) opcode: %04x, len: %d, data: ",
                  tx ? "TX->" : "RX<-",
                  pkt->pkt_type, pkt->opcode, pkt->param_len);
        for (size_t i = 0; i < pkt->param_len; i++) {
            mp_printf(&mp_plat_print, "%02x ", pkt->params[i]);
        }
        if (pkt_len != sizeof(h4_hci_cmd_pkt_t) + pkt->param_len) {
            mp_printf(&mp_plat_print, "  LENGTH MISMATCH");
        }
        mp_printf(&mp_plat_print, "\n");
    }
}

STATIC void dump_acl_pkt(bool tx, uint8_t pkt_len, uint8_t pkt_data[]) {
    if (debug) {
        // mp_printf(&mp_plat_print, "\\ PKT_DATA:  ");
        // for (size_t i = 0; i < pkt_len; i++) {
        //     mp_printf(&mp_plat_print, "%02x ", pkt_data[i]);
        // }
        // mp_printf(&mp_plat_print, "\n");
        h4_hci_acl_pkt_t *pkt = (h4_hci_acl_pkt_t *) pkt_data;
        mp_printf(&mp_plat_print,
                  "%s HCI ACLDATA (%x) handle: %04x, pb: %d, bc: %d, data_len: %d, ",
                  tx ? "TX->" : "RX<-", pkt->pkt_type, pkt->handle, pkt->pb, pkt->bc, pkt->data_len);

        if (pkt->pb != ACL_DATA_PB_MIDDLE) {
            // This is the start of a fragmented acl_data packet or is a full packet.
            acl_data_t *acl = (acl_data_t *) pkt->data;
            mp_printf(&mp_plat_print,
                      "acl data_len: %d, cid: %04x, data: ",
                      acl->acl_data_len, acl->cid);
            for (size_t i = 0; i < acl->acl_data_len; i++) {
                mp_printf(&mp_plat_print, "%02x ", acl->acl_data[i]);
            }
        } else {
            for (size_t i = 0; i < pkt->data_len; i++) {
                mp_printf(&mp_plat_print, "more data: %02x ", pkt->data[i]);
            }
        }

        if (pkt_len != sizeof(h4_hci_acl_pkt_t) + pkt->data_len) {
            mp_printf(&mp_plat_print, "  LENGTH MISMATCH");
        }
        mp_printf(&mp_plat_print, "\n");
    }
}

STATIC void dump_evt_pkt(bool tx, uint8_t pkt_len, uint8_t pkt_data[]) {
    if (debug) {
        h4_hci_evt_pkt_t *pkt = (h4_hci_evt_pkt_t *) pkt_data;
        mp_printf(&mp_plat_print,
                  "%s HCI EVENT   (%x) evt: %02x,  param_len: %d,  data: ",
                  tx ? "TX->" : "RX<-",
                  pkt->pkt_type, pkt->evt, pkt->param_len);
        for (size_t i = 0; i < pkt->param_len; i++) {
            mp_printf(&mp_plat_print, "%02x ", pkt->params[i]);
        }
        if (pkt_len != sizeof(h4_hci_evt_pkt_t) + pkt->param_len) {
            mp_printf(&mp_plat_print, "  LENGTH MISMATCH");
        }
        mp_printf(&mp_plat_print, "\n");
    }
}

STATIC void process_acl_data_pkt(uint8_t pkt_len, uint8_t pkt_data[]) {
    h4_hci_acl_pkt_t *pkt = (h4_hci_acl_pkt_t*) pkt_data;

    if (pkt->pb != ACL_DATA_PB_MIDDLE) {
        // This is the start of a fragmented acl_data packet or is a full packet.
        memcpy(acl_data_buffer, pkt->data, pkt->data_len);
        acl_data_len = pkt->data_len;
    } else {
        // This is a middle or end fragment of acl data.
        // Append to the accumulated data so far.
        memcpy(&acl_data_buffer[acl_data_len], pkt->data, pkt->data_len);
        acl_data_len += pkt->data_len;
    }

    acl_data_t *acl = (acl_data_t *) &acl_data_buffer;
    if (acl_data_len != acl->acl_data_len) {
        // We don't have the full packet yet.
        return;
    }

    if (acl->cid == BT_L2CAP_CID_ATT) {
        att_process_data(pkt->handle, acl->acl_data_len, acl->acl_data);
    }
    // } else if (aclHdr->cid == BT_L2CAP_CID_LE_SIG) {
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

STATIC void process_evt_pkt(size_t pkt_len, uint8_t pkt_data[])
{
    h4_hci_evt_pkt_t *pkt = (h4_hci_evt_pkt_t*) pkt_data;

    switch (pkt->evt) {
        case BT_HCI_EVT_DISCONN_COMPLETE: {
            struct bt_hci_evt_disconn_complete *disconn_complete =
                (struct bt_hci_evt_disconn_complete*) pkt->params;
            (void) disconn_complete;
            //FIX
            // ATT.removeConnection(disconn_complete->handle, disconn_complete->reason);
            // L2CAPSignaling.removeConnection(disconn_complete->handle, disconn_complete->reason);
            hci_le_set_advertising_enable(0x01);
            break;
        }

        case BT_HCI_EVT_CMD_COMPLETE: {

            struct cmd_complete_with_status {
                struct bt_hci_evt_cmd_complete cmd_complete;
                struct bt_hci_evt_cc_status cc_status;
            } __packed;

            struct cmd_complete_with_status *evt = (struct cmd_complete_with_status *) pkt->params;

            num_command_packets_allowed = evt->cmd_complete.ncmd;

            cmd_response_received = true;
            cmd_response_opcode = evt->cmd_complete.opcode;
            cmd_response_status = evt->cc_status.status;
            // All the bytes following cmd_complete, -including- the status byte, which is
            // included in all the _bt_hci_rp_* structs.
            cmd_response_data = (uint8_t *) &evt->cc_status;
            // Includes status byte.
            cmd_response_len = pkt->param_len - sizeof_field(struct cmd_complete_with_status, cmd_complete);

            break;
        }

        case BT_HCI_EVT_CMD_STATUS: {
            struct bt_hci_evt_cmd_status *evt = (struct bt_hci_evt_cmd_status *) pkt->params;

            num_command_packets_allowed = evt->ncmd;

            cmd_response_received = true;
            cmd_response_opcode = evt->opcode;
            cmd_response_status = evt->status;
            cmd_response_data = NULL;
            cmd_response_len = 0;

            break;
        }

        case BT_HCI_EVT_NUM_COMPLETED_PACKETS: {
            struct bt_hci_evt_num_completed_packets *evt =
                (struct bt_hci_evt_num_completed_packets *) pkt->params;

            // Start at zero-th pair: (conn handle, num completed packets).
            struct bt_hci_handle_count *handle_and_count = &(evt->h[0]);
            for (uint8_t i = 0; i < evt->num_handles; i++) {
                process_num_comp_pkts(handle_and_count->handle, handle_and_count->count);
                handle_and_count++;
            }
            break;
        }

        case BT_HCI_EVT_LE_META_EVENT: {
            struct bt_hci_evt_le_meta_event *meta_evt = (struct bt_hci_evt_le_meta_event *) pkt->params;
            uint8_t *le_evt =  pkt->params + sizeof (struct bt_hci_evt_le_meta_event);

            if (meta_evt->subevent == BT_HCI_EVT_LE_CONN_COMPLETE) {
                struct bt_hci_evt_le_conn_complete *le_conn_complete =
                    (struct bt_hci_evt_le_conn_complete *) le_evt;

                if (le_conn_complete->status == BT_HCI_ERR_SUCCESS) {
                    // ATT.addConnection(le_conn_complete->handle,
                    //                   le_conn_complete->role,
                    //                   le_conn_complete->peer_addr  //FIX struct
                    //                   le_conn_complete->interval,
                    //                   le_conn_complete->latency,
                    //                   le_conn_complete->supv_timeout
                    //                   le_conn_complete->clock_accuracy);

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
            if (debug) {
                mp_printf(&mp_plat_print, "process_evt_pkt: Unknown event: %02x\n");
            }
            break;
    }
}

void hci_init(void) {
    rx_idx = 0;
    pending_pkt = 0;
    hci_poll_in_progress = false;
}

hci_result_t hci_poll_for_incoming_pkt_timeout(uint32_t timeout_msecs) {
    uint64_t start = supervisor_ticks_ms64();

    hci_result_t result;

    while (supervisor_ticks_ms64() -start < timeout_msecs) {
        result = hci_poll_for_incoming_pkt();
        RUN_BACKGROUND_TASKS;
    }

    return result;
}


hci_result_t hci_poll_for_incoming_pkt(void) {
    if (hci_poll_in_progress) {
        return HCI_OK;
    }
    common_hal_mcu_disable_interrupts();
    if (!hci_poll_in_progress) {
        hci_poll_in_progress = true;
    }
    common_hal_mcu_enable_interrupts();

    // Assert RTS low to say we're ready to read data.
    common_hal_digitalio_digitalinout_set_value(adapter->rts_digitalinout, false);

    int errcode = 0;
    bool packet_is_complete = false;

    // Read bytes until we run out, or accumulate a complete packet.
    while (common_hal_busio_uart_rx_characters_available(adapter->hci_uart)) {
        common_hal_busio_uart_read(adapter->hci_uart, rx_buffer + rx_idx, 1, &errcode);
        if (errcode) {
            hci_poll_in_progress = false;
            return HCI_READ_ERROR;
        }
        rx_idx++;

        switch (rx_buffer[0]) {
            case H4_ACL:
                if (rx_idx > sizeof(h4_hci_acl_pkt_t) &&
                    rx_idx >= sizeof(h4_hci_acl_pkt_t) + ((h4_hci_acl_pkt_t *) rx_buffer)->data_len) {
                    packet_is_complete = true;
                }
                break;

            case H4_EVT:
                if (rx_idx > sizeof(h4_hci_evt_pkt_t) &&
                    rx_idx >= sizeof(h4_hci_evt_pkt_t) + ((h4_hci_evt_pkt_t *) rx_buffer)->param_len) {
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
        hci_poll_in_progress = false;
        return HCI_OK;
    }

    // Stop incoming data while processing packet.
    common_hal_digitalio_digitalinout_set_value(adapter->rts_digitalinout, true);
    size_t pkt_len = rx_idx;
    // Reset for next packet.
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
            if (debug) {
                mp_printf(&mp_plat_print, "Unknown HCI packet type: %d\n", rx_buffer[0]);
            }
            break;
    }

    common_hal_digitalio_digitalinout_set_value(adapter->rts_digitalinout, true);

    hci_poll_in_progress = false;
    return HCI_OK;
}


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
    uint8_t cmd_pkt_len = sizeof(h4_hci_cmd_pkt_t) + params_len;
    uint8_t tx_buffer[cmd_pkt_len];

    // cmd header is at the beginning of tx_buffer
    h4_hci_cmd_pkt_t *cmd_pkt = (h4_hci_cmd_pkt_t *) tx_buffer;
    cmd_pkt->pkt_type = H4_CMD;
    cmd_pkt->opcode = opcode;
    cmd_pkt->param_len = params_len;

    memcpy(cmd_pkt->params, params, params_len);

    if (debug) {
        dump_cmd_pkt(true, sizeof(tx_buffer), tx_buffer);
    }

    int result = write_pkt(tx_buffer, cmd_pkt_len);
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

hci_result_t hci_send_acl_pkt(uint16_t handle, uint8_t cid, uint8_t data_len, uint8_t *data) {
    int result;
    while (pending_pkt >= max_pkt) {
        result = hci_poll_for_incoming_pkt();
        if (result != HCI_OK) {
            return result;
        }
    }

    // data_len does not include cid
    const size_t cid_len = sizeof_field(acl_data_t, cid);
    // buf_len is size of entire packet including header.
    const size_t buf_len = sizeof(h4_hci_acl_pkt_t) + cid_len + data_len;
    uint8_t tx_buffer[buf_len];

    h4_hci_acl_pkt_t *acl_pkt = (h4_hci_acl_pkt_t *) tx_buffer;
    acl_data_t *acl_data = (acl_data_t *) acl_pkt->data;
    acl_pkt->pkt_type = H4_ACL;
    acl_pkt->handle = handle;
    acl_pkt->data_len = (uint8_t)(cid_len + data_len);
    acl_data->acl_data_len = (uint8_t) data_len;
    acl_data->cid = cid;

    memcpy(&tx_buffer[sizeof(h4_hci_acl_pkt_t)], data, data_len);

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

hci_result_t hci_le_read_buffer_size(uint16_t *le_max_len, uint8_t *le_max_num) {
    int result = send_command(BT_HCI_OP_LE_READ_BUFFER_SIZE, 0, NULL);
    if (result == HCI_OK) {
        struct bt_hci_rp_le_read_buffer_size *response =
            (struct bt_hci_rp_le_read_buffer_size *) cmd_response_data;
        *le_max_len = response->le_max_len;
        *le_max_num = response->le_max_num;
    }

    return result;
}

hci_result_t hci_read_buffer_size(uint16_t *acl_max_len, uint8_t *sco_max_len, uint16_t *acl_max_num, uint16_t *sco_max_num) {
    int result = send_command(BT_HCI_OP_READ_BUFFER_SIZE, 0, NULL);
    if (result == HCI_OK) {
        struct bt_hci_rp_read_buffer_size *response =
            (struct bt_hci_rp_read_buffer_size *) cmd_response_data;
        *acl_max_len = response->acl_max_len;
        *sco_max_len = response->sco_max_len;
        *acl_max_num = response->acl_max_num;
        *sco_max_num = response->sco_max_num;
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

hci_result_t hci_le_set_extended_advertising_parameters(uint8_t handle, uint16_t props, uint32_t prim_min_interval, uint32_t prim_max_interval, uint8_t prim_channel_map, uint8_t own_addr_type, bt_addr_le_t *peer_addr, uint8_t filter_policy, int8_t tx_power, uint8_t prim_adv_phy, uint8_t sec_adv_max_skip, uint8_t sec_adv_phy, uint8_t sid, uint8_t scan_req_notify_enable) {
    struct bt_hci_cp_le_set_ext_adv_param params = {
        .handle = handle,
        .props = props,
        // .prim_min_interval and .prim_max_interval set below
        .prim_channel_map = prim_channel_map,
        .own_addr_type = own_addr_type,
        // .peer_addr set below.
        .tx_power = tx_power,
        .sec_adv_max_skip = sec_adv_max_skip,
        .sec_adv_phy = sec_adv_phy,
        .sid = sid,
        .scan_req_notify_enable = scan_req_notify_enable,
    };
    // Assumes little-endian.
    memcpy(params.prim_min_interval, (void *) &prim_min_interval,
           sizeof_field(struct bt_hci_cp_le_set_ext_adv_param, prim_min_interval));
    memcpy(params.prim_max_interval, (void *) &prim_max_interval,
           sizeof_field(struct bt_hci_cp_le_set_ext_adv_param, prim_max_interval));
    memcpy(params.peer_addr.a.val, peer_addr->a.val, sizeof_field(bt_addr_le_t, a.val));
    return send_command(BT_HCI_OP_LE_SET_EXT_ADV_PARAM, sizeof(params), &params);
}

hci_result_t hci_le_read_maximum_advertising_data_length(uint16_t *max_adv_data_len) {
    int result = send_command(BT_HCI_OP_LE_READ_MAX_ADV_DATA_LEN, 0, NULL);
    if (result == HCI_OK) {
        struct bt_hci_rp_le_read_max_adv_data_len *response =
            (struct bt_hci_rp_le_read_max_adv_data_len *) cmd_response_data;
        *max_adv_data_len = response->max_adv_data_len;
    }

    return result;
}

hci_result_t hci_le_read_local_supported_features(uint8_t features[8]) {
    int result = send_command(BT_HCI_OP_LE_READ_LOCAL_FEATURES, 0, NULL);
    if (result == HCI_OK) {
        struct bt_hci_rp_le_read_local_features *response =
            (struct bt_hci_rp_le_read_local_features *) cmd_response_data;
        memcpy(features, response->features,
               sizeof_field(struct bt_hci_rp_le_read_local_features, features));
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

hci_result_t hci_le_set_extended_advertising_data(uint8_t handle, uint8_t op, uint8_t frag_pref, uint8_t len, uint8_t data[]) {
    const uint8_t max_len = sizeof_field(struct bt_hci_cp_le_set_ext_adv_data, data);
    uint8_t valid_len = MIN(len, max_len);
    struct bt_hci_cp_le_set_ext_adv_data params = {
        .handle = handle,
        .op = op,
        .frag_pref = frag_pref,
        .len = valid_len,
    };
    memcpy(params.data, data, valid_len);
    return send_command(BT_HCI_OP_LE_SET_EXT_ADV_DATA, sizeof(params) - (max_len - valid_len), &params);
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

hci_result_t hci_le_set_advertising_enable(uint8_t enable) {
    return send_command(BT_HCI_OP_LE_SET_ADV_ENABLE, sizeof(enable), &enable);
}

hci_result_t hci_le_set_extended_advertising_enable(uint8_t enable, uint8_t set_num, uint8_t handle[], uint16_t duration[], uint8_t max_ext_adv_evts[]) {
    uint8_t params[sizeof(struct bt_hci_cp_le_set_ext_adv_enable) +
                   set_num * (sizeof(struct bt_hci_ext_adv_set))];
    struct bt_hci_cp_le_set_ext_adv_enable *params_p = (struct bt_hci_cp_le_set_ext_adv_enable *) &params;
    params_p->enable = enable;
    params_p->set_num = set_num;
    for (size_t i = 0; i < set_num; i++) {
        params_p->s[i].handle = handle[i];
        params_p->s[i].duration = duration[i];
        params_p->s[i].max_ext_adv_evts = max_ext_adv_evts[i];
    }

    return send_command(BT_HCI_OP_LE_SET_EXT_ADV_ENABLE, sizeof(params), &params);
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

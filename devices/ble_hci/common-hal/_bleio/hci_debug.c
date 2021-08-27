/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Dan Halbert for Adafruit Industries
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

// This file is #include'd in hci.c when HCI_DEBUG is non-zero.

STATIC const char *att_opcode_name(uint16_t opcode) {
    switch (opcode) {
        case BT_ATT_OP_ERROR_RSP:
            return "ERROR_RSP";
        case BT_ATT_OP_MTU_REQ:
            return "MTU_REQ";
        case BT_ATT_OP_MTU_RSP:
            return "MTU_RSP";
        case BT_ATT_OP_FIND_INFO_REQ:
            return "FIND_INFO_REQ";
        case BT_ATT_OP_FIND_INFO_RSP:
            return "FIND_INFO_RSP";
        case BT_ATT_OP_FIND_TYPE_REQ:
            return "FIND_TYPE_REQ";
        case BT_ATT_OP_FIND_TYPE_RSP:
            return "FIND_TYPE_RSP";
        case BT_ATT_OP_READ_TYPE_REQ:
            return "READ_TYPE_REQ";
        case BT_ATT_OP_READ_TYPE_RSP:
            return "READ_TYPE_RSP";
        case BT_ATT_OP_READ_REQ:
            return "READ_REQ";
        case BT_ATT_OP_READ_RSP:
            return "READ_RSP";
        case BT_ATT_OP_READ_BLOB_REQ:
            return "READ_BLOB_REQ";
        case BT_ATT_OP_READ_BLOB_RSP:
            return "READ_BLOB_RSP";
        case BT_ATT_OP_READ_MULT_REQ:
            return "READ_MULT_REQ";
        case BT_ATT_OP_READ_MULT_RSP:
            return "READ_MULT_RSP";
        case BT_ATT_OP_READ_GROUP_REQ:
            return "READ_GROUP_REQ";
        case BT_ATT_OP_READ_GROUP_RSP:
            return "READ_GROUP_RSP";
        case BT_ATT_OP_WRITE_REQ:
            return "WRITE_REQ";
        case BT_ATT_OP_WRITE_RSP:
            return "WRITE_RSP";
        case BT_ATT_OP_PREPARE_WRITE_REQ:
            return "PREPARE_WRITE_REQ";
        case BT_ATT_OP_PREPARE_WRITE_RSP:
            return "PREPARE_WRITE_RSP";
        case BT_ATT_OP_EXEC_WRITE_REQ:
            return "EXEC_WRITE_REQ";
        case BT_ATT_OP_EXEC_WRITE_RSP:
            return "EXEC_WRITE_RSP";
        case BT_ATT_OP_NOTIFY:
            return "NOTIFY";
        case BT_ATT_OP_INDICATE:
            return "INDICATE";
        case BT_ATT_OP_CONFIRM:
            return "CONFIRM";
        case BT_ATT_OP_READ_MULT_VL_REQ:
            return "READ_MULT_VL_REQ";
        case BT_ATT_OP_READ_MULT_VL_RSP:
            return "READ_MULT_VL_RSP";
        case BT_ATT_OP_NOTIFY_MULT:
            return "NOTIFY_MULT";
        case BT_ATT_OP_WRITE_CMD:
            return "WRITE_CMD";
        case BT_ATT_OP_SIGNED_WRITE_CMD:
            return "SIGNED_WRITE_CMD";
        default:
            return "";
    }
}

STATIC const char *hci_evt_name(uint8_t evt) {
    switch (evt) {
        case BT_HCI_EVT_UNKNOWN:
            return "UNKNOWN";
        case BT_HCI_EVT_VENDOR:
            return "VENDOR";
        case BT_HCI_EVT_INQUIRY_COMPLETE:
            return "INQUIRY_COMPLETE";
        case BT_HCI_EVT_CONN_COMPLETE:
            return "CONN_COMPLETE";
        case BT_HCI_EVT_CONN_REQUEST:
            return "CONN_REQUEST";
        case BT_HCI_EVT_DISCONN_COMPLETE:
            return "DISCONN_COMPLETE";
        case BT_HCI_EVT_AUTH_COMPLETE:
            return "AUTH_COMPLETE";
        case BT_HCI_EVT_REMOTE_NAME_REQ_COMPLETE:
            return "REMOTE_NAME_REQ_COMPLETE";
        case BT_HCI_EVT_ENCRYPT_CHANGE:
            return "ENCRYPT_CHANGE";
        case BT_HCI_EVT_REMOTE_FEATURES:
            return "REMOTE_FEATURES";
        case BT_HCI_EVT_REMOTE_VERSION_INFO:
            return "REMOTE_VERSION_INFO";
        case BT_HCI_EVT_CMD_COMPLETE:
            return "CMD_COMPLETE";
        case BT_HCI_EVT_CMD_STATUS:
            return "CMD_STATUS";
        case BT_HCI_EVT_ROLE_CHANGE:
            return "ROLE_CHANGE";
        case BT_HCI_EVT_NUM_COMPLETED_PACKETS:
            return "NUM_COMPLETED_PACKETS";
        case BT_HCI_EVT_PIN_CODE_REQ:
            return "PIN_CODE_REQ";
        case BT_HCI_EVT_LINK_KEY_REQ:
            return "LINK_KEY_REQ";
        case BT_HCI_EVT_LINK_KEY_NOTIFY:
            return "LINK_KEY_NOTIFY";
        case BT_HCI_EVT_DATA_BUF_OVERFLOW:
            return "DATA_BUF_OVERFLOW";
        case BT_HCI_EVT_INQUIRY_RESULT_WITH_RSSI:
            return "INQUIRY_RESULT_WITH_RSSI";
        case BT_HCI_EVT_REMOTE_EXT_FEATURES:
            return "REMOTE_EXT_FEATURES";
        case BT_HCI_EVT_SYNC_CONN_COMPLETE:
            return "SYNC_CONN_COMPLETE";
        case BT_HCI_EVT_EXTENDED_INQUIRY_RESULT:
            return "EXTENDED_INQUIRY_RESULT";
        case BT_HCI_EVT_ENCRYPT_KEY_REFRESH_COMPLETE:
            return "ENCRYPT_KEY_REFRESH_COMPLETE";
        case BT_HCI_EVT_IO_CAPA_REQ:
            return "IO_CAPA_REQ";
        case BT_HCI_EVT_IO_CAPA_RESP:
            return "IO_CAPA_RESP";
        case BT_HCI_EVT_USER_CONFIRM_REQ:
            return "USER_CONFIRM_REQ";
        case BT_HCI_EVT_USER_PASSKEY_REQ:
            return "USER_PASSKEY_REQ";
        case BT_HCI_EVT_SSP_COMPLETE:
            return "SSP_COMPLETE";
        case BT_HCI_EVT_USER_PASSKEY_NOTIFY:
            return "USER_PASSKEY_NOTIFY";
        case BT_HCI_EVT_LE_META_EVENT:
            return "LE_META_EVENT";
        case BT_HCI_EVT_AUTH_PAYLOAD_TIMEOUT_EXP:
            return "AUTH_PAYLOAD_TIMEOUT_EXP";
        default:
            return "";
    }
}

STATIC const char *hci_evt_le_name(uint8_t evt_le) {
    switch (evt_le) {
        case BT_HCI_EVT_LE_CONN_COMPLETE:
            return "LE_CONN_COMPLETE";
        case BT_HCI_EVT_LE_ADVERTISING_REPORT:
            return "LE_ADVERTISING_REPORT";
        case BT_HCI_EVT_LE_CONN_UPDATE_COMPLETE:
            return "LE_CONN_UPDATE_COMPLETE";
        case BT_HCI_EVT_LE_LTK_REQUEST:
            return "LE_LTK_REQUEST";
        case BT_HCI_EVT_LE_CONN_PARAM_REQ:
            return "LE_CONN_PARAM_REQ";
        case BT_HCI_EVT_LE_DATA_LEN_CHANGE:
            return "LE_DATA_LEN_CHANGE";
        case BT_HCI_EVT_LE_P256_PUBLIC_KEY_COMPLETE:
            return "LE_P256_PUBLIC_KEY_COMPLETE";
        case BT_HCI_EVT_LE_GENERATE_DHKEY_COMPLETE:
            return "LE_GENERATE_DHKEY_COMPLETE";
        case BT_HCI_EVT_LE_ENH_CONN_COMPLETE:
            return "LE_ENH_CONN_COMPLETE";
        case BT_HCI_EVT_LE_DIRECT_ADV_REPORT:
            return "LE_DIRECT_ADV_REPORT";
        case BT_HCI_EVT_LE_PHY_UPDATE_COMPLETE:
            return "LE_PHY_UPDATE_COMPLETE";
        case BT_HCI_EVT_LE_EXT_ADVERTISING_REPORT:
            return "LE_EXT_ADVERTISING_REPORT";
        case BT_HCI_EVT_LE_PER_ADV_SYNC_ESTABLISHED:
            return "LE_PER_ADV_SYNC_ESTABLISHED";
        case BT_HCI_EVT_LE_PER_ADVERTISING_REPORT:
            return "LE_PER_ADVERTISING_REPORT";
        case BT_HCI_EVT_LE_PER_ADV_SYNC_LOST:
            return "LE_PER_ADV_SYNC_LOST";
        case BT_HCI_EVT_LE_SCAN_TIMEOUT:
            return "LE_SCAN_TIMEOUT";
        case BT_HCI_EVT_LE_ADV_SET_TERMINATED:
            return "LE_ADV_SET_TERMINATED";
        case BT_HCI_EVT_LE_SCAN_REQ_RECEIVED:
            return "LE_SCAN_REQ_RECEIVED";
        case BT_HCI_EVT_LE_CHAN_SEL_ALGO:
            return "LE_CHAN_SEL_ALGO";
        default:
            return "";
    }
}

STATIC const char *hci_opcode_name(uint16_t opcode) {
    switch (opcode) {
        case BT_OP_NOP:
            return "NOP";
        case BT_HCI_OP_INQUIRY:
            return "INQUIRY";
        case BT_HCI_OP_INQUIRY_CANCEL:
            return "INQUIRY_CANCEL";
        case BT_HCI_OP_CONNECT:
            return "CONNECT";
        case BT_HCI_OP_DISCONNECT:
            return "DISCONNECT";
        case BT_HCI_OP_CONNECT_CANCEL:
            return "CONNECT_CANCEL";
        case BT_HCI_OP_ACCEPT_CONN_REQ:
            return "ACCEPT_CONN_REQ";
        case BT_HCI_OP_SETUP_SYNC_CONN:
            return "SETUP_SYNC_CONN";
        case BT_HCI_OP_ACCEPT_SYNC_CONN_REQ:
            return "ACCEPT_SYNC_CONN_REQ";
        case BT_HCI_OP_REJECT_CONN_REQ:
            return "REJECT_CONN_REQ";
        case BT_HCI_OP_LINK_KEY_REPLY:
            return "LINK_KEY_REPLY";
        case BT_HCI_OP_LINK_KEY_NEG_REPLY:
            return "LINK_KEY_NEG_REPLY";
        case BT_HCI_OP_PIN_CODE_REPLY:
            return "PIN_CODE_REPLY";
        case BT_HCI_OP_PIN_CODE_NEG_REPLY:
            return "PIN_CODE_NEG_REPLY";
        case BT_HCI_OP_AUTH_REQUESTED:
            return "AUTH_REQUESTED";
        case BT_HCI_OP_SET_CONN_ENCRYPT:
            return "SET_CONN_ENCRYPT";
        case BT_HCI_OP_REMOTE_NAME_REQUEST:
            return "REMOTE_NAME_REQUEST";
        case BT_HCI_OP_REMOTE_NAME_CANCEL:
            return "REMOTE_NAME_CANCEL";
        case BT_HCI_OP_READ_REMOTE_FEATURES:
            return "READ_REMOTE_FEATURES";
        case BT_HCI_OP_READ_REMOTE_EXT_FEATURES:
            return "READ_REMOTE_EXT_FEATURES";
        case BT_HCI_OP_READ_REMOTE_VERSION_INFO:
            return "READ_REMOTE_VERSION_INFO";
        case BT_HCI_OP_IO_CAPABILITY_REPLY:
            return "IO_CAPABILITY_REPLY";
        case BT_HCI_OP_USER_CONFIRM_REPLY:
            return "USER_CONFIRM_REPLY";
        case BT_HCI_OP_USER_CONFIRM_NEG_REPLY:
            return "USER_CONFIRM_NEG_REPLY";
        case BT_HCI_OP_USER_PASSKEY_REPLY:
            return "USER_PASSKEY_REPLY";
        case BT_HCI_OP_USER_PASSKEY_NEG_REPLY:
            return "USER_PASSKEY_NEG_REPLY";
        case BT_HCI_OP_IO_CAPABILITY_NEG_REPLY:
            return "IO_CAPABILITY_NEG_REPLY";
        case BT_HCI_OP_SET_EVENT_MASK:
            return "SET_EVENT_MASK";
        case BT_HCI_OP_RESET:
            return "RESET";
        case BT_HCI_OP_WRITE_LOCAL_NAME:
            return "WRITE_LOCAL_NAME";
        case BT_HCI_OP_WRITE_PAGE_TIMEOUT:
            return "WRITE_PAGE_TIMEOUT";
        case BT_HCI_OP_WRITE_SCAN_ENABLE:
            return "WRITE_SCAN_ENABLE";
        case BT_HCI_OP_READ_TX_POWER_LEVEL:
            return "READ_TX_POWER_LEVEL";
        case BT_HCI_OP_SET_CTL_TO_HOST_FLOW:
            return "SET_CTL_TO_HOST_FLOW";
        case BT_HCI_OP_HOST_BUFFER_SIZE:
            return "HOST_BUFFER_SIZE";
        case BT_HCI_OP_HOST_NUM_COMPLETED_PACKETS:
            return "HOST_NUM_COMPLETED_PACKETS";
        case BT_HCI_OP_WRITE_INQUIRY_MODE:
            return "WRITE_INQUIRY_MODE";
        case BT_HCI_OP_WRITE_SSP_MODE:
            return "WRITE_SSP_MODE";
        case BT_HCI_OP_SET_EVENT_MASK_PAGE_2:
            return "SET_EVENT_MASK_PAGE_2";
        case BT_HCI_OP_LE_WRITE_LE_HOST_SUPP:
            return "LE_WRITE_LE_HOST_SUPP";
        case BT_HCI_OP_WRITE_SC_HOST_SUPP:
            return "WRITE_SC_HOST_SUPP";
        case BT_HCI_OP_READ_AUTH_PAYLOAD_TIMEOUT:
            return "READ_AUTH_PAYLOAD_TIMEOUT";
        case BT_HCI_OP_WRITE_AUTH_PAYLOAD_TIMEOUT:
            return "WRITE_AUTH_PAYLOAD_TIMEOUT";
        case BT_HCI_OP_READ_LOCAL_VERSION_INFO:
            return "READ_LOCAL_VERSION_INFO";
        case BT_HCI_OP_READ_SUPPORTED_COMMANDS:
            return "READ_SUPPORTED_COMMANDS";
        case BT_HCI_OP_READ_LOCAL_EXT_FEATURES:
            return "READ_LOCAL_EXT_FEATURES";
        case BT_HCI_OP_READ_LOCAL_FEATURES:
            return "READ_LOCAL_FEATURES";
        case BT_HCI_OP_READ_BUFFER_SIZE:
            return "READ_BUFFER_SIZE";
        case BT_HCI_OP_READ_BD_ADDR:
            return "READ_BD_ADDR";
        case BT_HCI_OP_READ_RSSI:
            return "READ_RSSI";
        case BT_HCI_OP_READ_ENCRYPTION_KEY_SIZE:
            return "READ_ENCRYPTION_KEY_SIZE";
        case BT_HCI_OP_LE_SET_EVENT_MASK:
            return "LE_SET_EVENT_MASK";
        case BT_HCI_OP_LE_READ_BUFFER_SIZE:
            return "LE_READ_BUFFER_SIZE";
        case BT_HCI_OP_LE_READ_LOCAL_FEATURES:
            return "LE_READ_LOCAL_FEATURES";
        case BT_HCI_OP_LE_SET_RANDOM_ADDRESS:
            return "LE_SET_RANDOM_ADDRESS";
        case BT_HCI_OP_LE_SET_ADV_PARAM:
            return "LE_SET_ADV_PARAM";
        case BT_HCI_OP_LE_READ_ADV_CHAN_TX_POWER:
            return "LE_READ_ADV_CHAN_TX_POWER";
        case BT_HCI_OP_LE_SET_ADV_DATA:
            return "LE_SET_ADV_DATA";
        case BT_HCI_OP_LE_SET_SCAN_RSP_DATA:
            return "LE_SET_SCAN_RSP_DATA";
        case BT_HCI_OP_LE_SET_ADV_ENABLE:
            return "LE_SET_ADV_ENABLE";
        case BT_HCI_OP_LE_SET_SCAN_PARAM:
            return "LE_SET_SCAN_PARAM";
        case BT_HCI_OP_LE_SET_SCAN_ENABLE:
            return "LE_SET_SCAN_ENABLE";
        case BT_HCI_OP_LE_CREATE_CONN:
            return "LE_CREATE_CONN";
        case BT_HCI_OP_LE_CREATE_CONN_CANCEL:
            return "LE_CREATE_CONN_CANCEL";
        case BT_HCI_OP_LE_READ_WL_SIZE:
            return "LE_READ_WL_SIZE";
        case BT_HCI_OP_LE_CLEAR_WL:
            return "LE_CLEAR_WL";
        case BT_HCI_OP_LE_ADD_DEV_TO_WL:
            return "LE_ADD_DEV_TO_WL";
        case BT_HCI_OP_LE_REM_DEV_FROM_WL:
            return "LE_REM_DEV_FROM_WL";
        case BT_HCI_OP_LE_CONN_UPDATE:
            return "LE_CONN_UPDATE";
        case BT_HCI_OP_LE_SET_HOST_CHAN_CLASSIF:
            return "LE_SET_HOST_CHAN_CLASSIF";
        case BT_HCI_OP_LE_READ_CHAN_MAP:
            return "LE_READ_CHAN_MAP";
        case BT_HCI_OP_LE_READ_REMOTE_FEATURES:
            return "LE_READ_REMOTE_FEATURES";
        case BT_HCI_OP_LE_ENCRYPT:
            return "LE_ENCRYPT";
        case BT_HCI_OP_LE_RAND:
            return "LE_RAND";
        case BT_HCI_OP_LE_START_ENCRYPTION:
            return "LE_START_ENCRYPTION";
        case BT_HCI_OP_LE_LTK_REQ_REPLY:
            return "LE_LTK_REQ_REPLY";
        case BT_HCI_OP_LE_LTK_REQ_NEG_REPLY:
            return "LE_LTK_REQ_NEG_REPLY";
        case BT_HCI_OP_LE_READ_SUPP_STATES:
            return "LE_READ_SUPP_STATES";
        case BT_HCI_OP_LE_RX_TEST:
            return "LE_RX_TEST";
        case BT_HCI_OP_LE_TX_TEST:
            return "LE_TX_TEST";
        case BT_HCI_OP_LE_TEST_END:
            return "LE_TEST_END";
        case BT_HCI_OP_LE_CONN_PARAM_REQ_REPLY:
            return "LE_CONN_PARAM_REQ_REPLY";
        case BT_HCI_OP_LE_CONN_PARAM_REQ_NEG_REPLY:
            return "LE_CONN_PARAM_REQ_NEG_REPLY";
        case BT_HCI_OP_LE_SET_DATA_LEN:
            return "LE_SET_DATA_LEN";
        case BT_HCI_OP_LE_READ_DEFAULT_DATA_LEN:
            return "LE_READ_DEFAULT_DATA_LEN";
        case BT_HCI_OP_LE_WRITE_DEFAULT_DATA_LEN:
            return "LE_WRITE_DEFAULT_DATA_LEN";
        case BT_HCI_OP_LE_P256_PUBLIC_KEY:
            return "LE_P256_PUBLIC_KEY";
        case BT_HCI_OP_LE_GENERATE_DHKEY:
            return "LE_GENERATE_DHKEY";
        case BT_HCI_OP_LE_ADD_DEV_TO_RL:
            return "LE_ADD_DEV_TO_RL";
        case BT_HCI_OP_LE_REM_DEV_FROM_RL:
            return "LE_REM_DEV_FROM_RL";
        case BT_HCI_OP_LE_CLEAR_RL:
            return "LE_CLEAR_RL";
        case BT_HCI_OP_LE_READ_RL_SIZE:
            return "LE_READ_RL_SIZE";
        case BT_HCI_OP_LE_READ_PEER_RPA:
            return "LE_READ_PEER_RPA";
        case BT_HCI_OP_LE_READ_LOCAL_RPA:
            return "LE_READ_LOCAL_RPA";
        case BT_HCI_OP_LE_SET_ADDR_RES_ENABLE:
            return "LE_SET_ADDR_RES_ENABLE";
        case BT_HCI_OP_LE_SET_RPA_TIMEOUT:
            return "LE_SET_RPA_TIMEOUT";
        case BT_HCI_OP_LE_READ_MAX_DATA_LEN:
            return "LE_READ_MAX_DATA_LEN";
        case BT_HCI_OP_LE_READ_PHY:
            return "LE_READ_PHY";
        case BT_HCI_OP_LE_SET_DEFAULT_PHY:
            return "LE_SET_DEFAULT_PHY";
        case BT_HCI_OP_LE_SET_PHY:
            return "LE_SET_PHY";
        case BT_HCI_OP_LE_ENH_RX_TEST:
            return "LE_ENH_RX_TEST";
        case BT_HCI_OP_LE_ENH_TX_TEST:
            return "LE_ENH_TX_TEST";
        case BT_HCI_OP_LE_SET_ADV_SET_RANDOM_ADDR:
            return "LE_SET_ADV_SET_RANDOM_ADDR";
        case BT_HCI_OP_LE_SET_EXT_ADV_PARAM:
            return "LE_SET_EXT_ADV_PARAM";
        case BT_HCI_OP_LE_SET_EXT_ADV_DATA:
            return "LE_SET_EXT_ADV_DATA";
        case BT_HCI_OP_LE_SET_EXT_SCAN_RSP_DATA:
            return "LE_SET_EXT_SCAN_RSP_DATA";
        case BT_HCI_OP_LE_SET_EXT_ADV_ENABLE:
            return "LE_SET_EXT_ADV_ENABLE";
        case BT_HCI_OP_LE_READ_MAX_ADV_DATA_LEN:
            return "LE_READ_MAX_ADV_DATA_LEN";
        case BT_HCI_OP_LE_READ_NUM_ADV_SETS:
            return "LE_READ_NUM_ADV_SETS";
        case BT_HCI_OP_LE_REMOVE_ADV_SET:
            return "LE_REMOVE_ADV_SET";
        case BT_HCI_OP_CLEAR_ADV_SETS:
            return "CLEAR_ADV_SETS";
        case BT_HCI_OP_LE_SET_PER_ADV_PARAM:
            return "LE_SET_PER_ADV_PARAM";
        case BT_HCI_OP_LE_SET_PER_ADV_DATA:
            return "LE_SET_PER_ADV_DATA";
        case BT_HCI_OP_LE_SET_PER_ADV_ENABLE:
            return "LE_SET_PER_ADV_ENABLE";
        case BT_HCI_OP_LE_SET_EXT_SCAN_PARAM:
            return "LE_SET_EXT_SCAN_PARAM";
        case BT_HCI_OP_LE_SET_EXT_SCAN_ENABLE:
            return "LE_SET_EXT_SCAN_ENABLE";
        case BT_HCI_OP_LE_EXT_CREATE_CONN:
            return "LE_EXT_CREATE_CONN";
        case BT_HCI_OP_LE_PER_ADV_CREATE_SYNC:
            return "LE_PER_ADV_CREATE_SYNC";
        case BT_HCI_OP_LE_PER_ADV_CREATE_SYNC_CANCEL:
            return "LE_PER_ADV_CREATE_SYNC_CANCEL";
        case BT_HCI_OP_LE_PER_ADV_TERMINATE_SYNC:
            return "LE_PER_ADV_TERMINATE_SYNC";
        case BT_HCI_OP_LE_ADD_DEV_TO_PER_ADV_LIST:
            return "LE_ADD_DEV_TO_PER_ADV_LIST";
        case BT_HCI_OP_LE_REM_DEV_FROM_PER_ADV_LIST:
            return "LE_REM_DEV_FROM_PER_ADV_LIST";
        case BT_HCI_OP_LE_CLEAR_PER_ADV_LIST:
            return "LE_CLEAR_PER_ADV_LIST";
        case BT_HCI_OP_LE_READ_PER_ADV_LIST_SIZE:
            return "LE_READ_PER_ADV_LIST_SIZE";
        case BT_HCI_OP_LE_READ_TX_POWER:
            return "LE_READ_TX_POWER";
        case BT_HCI_OP_LE_READ_RF_PATH_COMP:
            return "LE_READ_RF_PATH_COMP";
        case BT_HCI_OP_LE_WRITE_RF_PATH_COMP:
            return "LE_WRITE_RF_PATH_COMP";
        case BT_HCI_OP_LE_SET_PRIVACY_MODE:
            return "LE_SET_PRIVACY_MODE";
        default:
            return "";
    }
}


STATIC void dump_cmd_pkt(bool tx, uint8_t pkt_len, uint8_t pkt_data[]) {
    h4_hci_cmd_pkt_t *pkt = (h4_hci_cmd_pkt_t *)pkt_data;
    mp_printf(&mp_plat_print,
        "%s HCI COMMAND (%x) op:  %s (%04x), len: %d, data: ",
        tx ? "TX->" : "RX<-",
        pkt->pkt_type,
        hci_opcode_name(pkt->opcode), pkt->opcode, pkt->param_len);
    for (size_t i = 0; i < pkt->param_len; i++) {
        mp_printf(&mp_plat_print, "%02x ", pkt->params[i]);
    }
    if (pkt_len != sizeof(h4_hci_cmd_pkt_t) + pkt->param_len) {
        mp_printf(&mp_plat_print, "  LENGTH MISMATCH, pkt_len: %d", pkt_len);
    }
    mp_printf(&mp_plat_print, "\n");
}

STATIC void dump_acl_pkt(bool tx, uint8_t pkt_len, uint8_t pkt_data[]) {
    h4_hci_acl_pkt_t *pkt = (h4_hci_acl_pkt_t *)pkt_data;
    acl_data_t *acl = (acl_data_t *)pkt->data;

    mp_printf(&mp_plat_print,
        "%s HCI ACLDATA (%x) ",
        tx ? "TX->" : "RX<-", pkt->pkt_type);

    if (pkt->pb != ACL_DATA_PB_MIDDLE && acl->cid == BT_L2CAP_CID_ATT) {
        // This is the start of a fragmented acl_data packet or is a full packet,
        // and is an ATT protocol packet.
        mp_printf(&mp_plat_print, "att: %s (%02x), ", att_opcode_name(acl->acl_data[0]), acl->acl_data[0]);
    }

    mp_printf(&mp_plat_print,
        "handle: %04x, pb: %d, bc: %d, data_len: %d, ",
        pkt->handle, pkt->pb, pkt->bc, pkt->data_len);

    if (pkt->pb != ACL_DATA_PB_MIDDLE) {
        // This is the start of a fragmented acl_data packet or is a full packet.
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
        mp_printf(&mp_plat_print, "  LENGTH MISMATCH, pkt_len: %d", pkt_len);
    }
    mp_printf(&mp_plat_print, "\n");
}

STATIC void dump_evt_pkt(bool tx, uint8_t pkt_len, uint8_t pkt_data[]) {
    h4_hci_evt_pkt_t *pkt = (h4_hci_evt_pkt_t *)pkt_data;
    mp_printf(&mp_plat_print,
        "%s HCI EVENT   (%x) evt: %s (%02x),  param_len: %d,  data: ",
        tx ? "TX->" : "RX<-",
        pkt->pkt_type,
        pkt->evt == BT_HCI_EVT_LE_META_EVENT
                ? hci_evt_le_name(pkt->params[0])
                : hci_evt_name(pkt->evt),
        pkt->evt, pkt->param_len);
    for (size_t i = 0; i < pkt->param_len; i++) {
        mp_printf(&mp_plat_print, "%02x ", pkt->params[i]);
    }
    if (pkt_len != sizeof(h4_hci_evt_pkt_t) + pkt->param_len) {
        mp_printf(&mp_plat_print, "  LENGTH MISMATCH, pkt_len: %d", pkt_len);
    }
    mp_printf(&mp_plat_print, "\n");
}

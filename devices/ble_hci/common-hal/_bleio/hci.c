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

#include "hci.h"
#include <string.h>

#include "supervisor/shared/tick.h"

#define ATT_CID       0x0004

#define HCI_COMMAND_PKT 0x01
#define HCI_ACLDATA_PKT 0x02
#define HCI_EVENT_PKT   0x04

#define EVT_DISCONN_COMPLETE 0x05
#define EVT_CMD_COMPLETE     0xe
#define EVT_CMD_STATUS       0x0f
#define EVT_NUM_COMP_PKTS    0x13
#define EVT_LE_META_EVENT    0x3e

#define EVT_LE_CONN_COMPLETE      0x01
#define EVT_LE_ADVERTISING_REPORT 0x02

#define OGF_LINK_CTL           0x01
#define OGF_HOST_CTL           0x03
#define OGF_INFO_PARAM         0x04
#define OGF_STATUS_PARAM       0x05
#define OGF_LE_CTL             0x08

// OGF_LINK_CTL
#define OCF_DISCONNECT         0x0006

// OGF_HOST_CTL
#define OCF_SET_EVENT_MASK     0x0001
#define OCF_RESET              0x0003

// OGF_INFO_PARAM
#define OCF_READ_LOCAL_VERSION 0x0001
#define OCF_READ_BD_ADDR       0x0009

// OGF_STATUS_PARAM
#define OCF_READ_RSSI          0x0005

// OGF_LE_CTL
#define OCF_LE_READ_BUFFER_SIZE           0x0002
#define OCF_LE_SET_RANDOM_ADDRESS         0x0005
#define OCF_LE_SET_ADVERTISING_PARAMETERS 0x0006
#define OCF_LE_SET_ADVERTISING_DATA       0x0008
#define OCF_LE_SET_SCAN_RESPONSE_DATA     0x0009
#define OCF_LE_SET_ADVERTISE_ENABLE       0x000a
#define OCF_LE_SET_SCAN_PARAMETERS        0x000b
#define OCF_LE_SET_SCAN_ENABLE            0x000c
#define OCF_LE_CREATE_CONN                0x000d
#define OCF_LE_CANCEL_CONN                0x000e
#define OCF_LE_CONN_UPDATE                0x0013

#define HCI_OE_USER_ENDED_CONNECTION 0x13


#define RECV_BUFFER_SIZE (3 + 255)
#define ACL_PKT_BUFFER_SIZE (255)

STATIC bleio_adapter_obj_t *adapter;

STATIC int recv_idx;
STATIC uint8_t recv_buffer[RECV_BUFFER_SIZE];
STATIC uint16_t cmd_complete_opcode;
STATIC int cmd_complete_status;
STATIC uint8_t cmd_response_len;
STATIC uint8_t* cmd_response;

STATIC uint8_t max_pkt;
STATIC uint8_t pending_pkt;

//FIX STATIC uint8_t acl_pkt_buffer[255];

STATIC bool debug = true;

typedef struct __attribute__ ((packed)) {
    uint8_t evt;
    uint8_t plen;
} HCIEventHdr;

STATIC void dumpPkt(const char* prefix, uint8_t plen, uint8_t pdata[])
{
    if (debug) {
        mp_printf(&mp_plat_print, "%s", prefix);

        for (uint8_t i = 0; i < plen; i++) {
            mp_printf(&mp_plat_print, "%02x", pdata[i]);
        }
        mp_printf(&mp_plat_print, "\n");
    }
}


STATIC void handleAclDataPkt(uint8_t plen, uint8_t pdata[])
{
    // typedef struct __attribute__ ((packed)) {
    //     uint16_t handle;
    //     uint16_t dlen;
    //     uint16_t len;
    //     uint16_t cid;
    // } HCIACLHdr;

    // HCIACLHdr *aclHdr = (HCIACLHdr*)pdata;

    // uint16_t aclFlags = (aclHdr->handle & 0xf000) >> 12;

    // if ((aclHdr->dlen - 4) != aclHdr->len) {
    //     // packet is fragmented
    //     if (aclFlags != 0x01) {
    //         // copy into ACL buffer
    //         memcpy(acl_pkt_buffer, &recv_buffer[1], sizeof(HCIACLHdr) + aclHdr->dlen - 4);
    //     } else {
    //         // copy next chunk into the buffer
    //         HCIACLHdr* aclBufferHeader = (HCIACLHdr*)acl_pkt_buffer;

    //         memcpy(&acl_pkt_buffer[sizeof(HCIACLHdr) + aclBufferHeader->dlen - 4], &recv_buffer[1 + sizeof(aclHdr->handle) + sizeof(aclHdr->dlen)], aclHdr->dlen);

    //         aclBufferHeader->dlen += aclHdr->dlen;
    //         aclHdr = aclBufferHeader;
    //     }
    // }

    // if ((aclHdr->dlen - 4) != aclHdr->len) {
    //     // don't have the full packet yet
    //     return;
    // }

    // if (aclHdr->cid == ATT_CID) {
    //     if (aclFlags == 0x01) {
    //         // use buffered packet
    //         ATT.handleData(aclHdr->handle & 0x0fff, aclHdr->len, &acl_pkt_buffer[sizeof(HCIACLHdr)]);
    //     } else {
    //         // use the recv buffer
    //         ATT.handleData(aclHdr->handle & 0x0fff, aclHdr->len, &recv_buffer[1 + sizeof(HCIACLHdr)]);
    //     }
    // } else if (aclHdr->cid == SIGNALING_CID) {
    //     L2CAPSignaling.handleData(aclHdr->handle & 0x0fff, aclHdr->len, &recv_buffer[1 + sizeof(HCIACLHdr)]);
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

STATIC void handleNumCompPkts(uint16_t handle, uint16_t numPkts)
{
    if (numPkts && pending_pkt > numPkts) {
        pending_pkt -= numPkts;
    } else {
        pending_pkt = 0;
    }
}

STATIC void handleEventPkt(uint8_t plen, uint8_t pdata[])
{
    HCIEventHdr *eventHdr = (HCIEventHdr*)pdata;

    if (eventHdr->evt == EVT_DISCONN_COMPLETE) {
        typedef struct __attribute__ ((packed)) {
            uint8_t status;
            uint16_t handle;
            uint8_t reason;
        } DisconnComplete;

        DisconnComplete *disconnComplete = (DisconnComplete*)&pdata[sizeof(HCIEventHdr)];
        (void) disconnComplete;
        //FIX
        // ATT.removeConnection(disconnComplete->handle, disconnComplete->reason);
        // L2CAPSignaling.removeConnection(disconnComplete->handle, disconnComplete->reason);

        hci_leSetAdvertiseEnable(0x01);
    } else if (eventHdr->evt == EVT_CMD_COMPLETE) {
        typedef struct __attribute__ ((packed)) {
            uint8_t ncmd;
            uint16_t opcode;
            uint8_t status;
        } CmdComplete;

        CmdComplete *cmdCompleteHeader = (CmdComplete*)&pdata[sizeof(HCIEventHdr)];
        cmd_complete_opcode = cmdCompleteHeader->opcode;
        cmd_complete_status = cmdCompleteHeader->status;
        cmd_response_len = pdata[1] - sizeof(CmdComplete);
        cmd_response = &pdata[sizeof(HCIEventHdr) + sizeof(CmdComplete)];

    } else if (eventHdr->evt == EVT_CMD_STATUS) {
        typedef struct __attribute__ ((packed)) {
            uint8_t status;
            uint8_t ncmd;
            uint16_t opcode;
        } CmdStatus;

        CmdStatus *cmdStatusHeader = (CmdStatus*)&pdata[sizeof(HCIEventHdr)];
        cmd_complete_opcode = cmdStatusHeader->opcode;
        cmd_complete_status = cmdStatusHeader->status;
        cmd_response_len = 0;
    } else if (eventHdr->evt == EVT_NUM_COMP_PKTS) {
        uint8_t numHandles = pdata[sizeof(HCIEventHdr)];
        uint8_t* data = &pdata[sizeof(HCIEventHdr) + sizeof(numHandles)];

        for (uint8_t i = 0; i < numHandles; i++) {
            handleNumCompPkts(data[0], data[1]);

            data += 2;
        }
    } else if (eventHdr->evt == EVT_LE_META_EVENT) {
        typedef struct __attribute__ ((packed)) {
            uint8_t subevent;
        } LeMetaEventHeader;

        LeMetaEventHeader *leMetaHeader = (LeMetaEventHeader*)&pdata[sizeof(HCIEventHdr)];
        if (leMetaHeader->subevent == EVT_LE_CONN_COMPLETE) {
            typedef struct __attribute__ ((packed)) {
                uint8_t status;
                uint16_t handle;
                uint8_t role;
                uint8_t peerBdaddrType;
                uint8_t peerBdaddr[6];
                uint16_t interval;
                uint16_t latency;
                uint16_t supervisionTimeout;
                uint8_t masterClockAccuracy;
            } EvtLeConnectionComplete;

            EvtLeConnectionComplete *leConnectionComplete = (EvtLeConnectionComplete*)&pdata[sizeof(HCIEventHdr) + sizeof(LeMetaEventHeader)];

            if (leConnectionComplete->status == 0x00) {
                // ATT.addConnection(leConnectionComplete->handle,
                //                   leConnectionComplete->role,
                //                   leConnectionComplete->peerBdaddrType,
                //                   leConnectionComplete->peerBdaddr,
                //                   leConnectionComplete->interval,
                //                   leConnectionComplete->latency,
                //                   leConnectionComplete->supervisionTimeout,
                //                   leConnectionComplete->masterClockAccuracy);

                // L2CAPSignaling.addConnection(leConnectionComplete->handle,
                //                              leConnectionComplete->role,
                //                              leConnectionComplete->peerBdaddrType,
                //                              leConnectionComplete->peerBdaddr,
                //                              leConnectionComplete->interval,
                //                              leConnectionComplete->latency,
                //                              leConnectionComplete->supervisionTimeout,
                //                              leConnectionComplete->masterClockAccuracy);
            }
        } else if (leMetaHeader->subevent == EVT_LE_ADVERTISING_REPORT) {
            typedef struct __attribute__ ((packed)) {
                uint8_t status;
                uint8_t type;
                uint8_t peerBdaddrType;
                uint8_t peerBdaddr[6];
                uint8_t eirLength;
                uint8_t eirData[31];
            } EvtLeAdvertisingReport;

            EvtLeAdvertisingReport*leAdvertisingReport = (EvtLeAdvertisingReport*)&pdata[sizeof(HCIEventHdr) + sizeof(LeMetaEventHeader)];

            if (leAdvertisingReport->status == 0x01) {
                // last byte is RSSI
                //FIX int8_t rssi = leAdvertisingReport->eirData[leAdvertisingReport->eirLength];

                // GAP.handleLeAdvertisingReport(leAdvertisingReport->type,
                //                               leAdvertisingReport->peerBdaddrType,
                //                               leAdvertisingReport->peerBdaddr,
                //                               leAdvertisingReport->eirLength,
                //                               leAdvertisingReport->eirData,
                //                               rssi);

            }
        }
    }
}

void hci_init(bleio_adapter_obj_t *adapter_in) {
    adapter = adapter_in;
    recv_idx = 0;
    pending_pkt = 0;
}

void hci_poll(void) {
    // Assert RTS low to say we're ready to read data.
    common_hal_digitalio_digitalinout_set_value(&adapter->rts_digitalinout, false);

    int errcode = 0;
    while (common_hal_busio_uart_rx_characters_available(&adapter->hci_uart)) {
        // Read just one character.
        common_hal_busio_uart_read(&adapter->hci_uart, recv_buffer + recv_idx, 1, &errcode);
        recv_idx++;

        if (recv_buffer[0] == HCI_ACLDATA_PKT) {
            if (recv_idx > 5 && recv_idx >= (5 + (recv_buffer[3] + (recv_buffer[4] << 8)))) {
                if (debug) {
                    dumpPkt("HCI ACLDATA RX <- ", recv_idx, recv_buffer);
                }
                // Hold data while processing packet.
                common_hal_digitalio_digitalinout_set_value(&adapter->rts_digitalinout, true);
                size_t pktLen = recv_idx - 1;
                recv_idx = 0;

                handleAclDataPkt(pktLen, &recv_buffer[1]);

                common_hal_digitalio_digitalinout_set_value(&adapter->rts_digitalinout, false);
            }
        } else if (recv_buffer[0] == HCI_EVENT_PKT) {
            if (recv_idx > 3 && recv_idx >= (3 + recv_buffer[2])) {
                if (debug) {
                    dumpPkt("HCI EVENT RX <- ", recv_idx, recv_buffer);
                }
                // Hold data while processing packet.
                common_hal_digitalio_digitalinout_set_value(&adapter->rts_digitalinout, true);
                // Received full event. Reset buffer and handle packet.
                size_t pktLen = recv_idx - 1;
                recv_idx = 0;

                handleEventPkt(pktLen, &recv_buffer[1]);

                common_hal_digitalio_digitalinout_set_value(&adapter->rts_digitalinout, false);
            }
        } else {
            recv_idx = 0;
        }
    }

    common_hal_digitalio_digitalinout_set_value(&adapter->rts_digitalinout, true);
}


int hci_sendCommand(uint8_t ogf, uint16_t ocf, uint8_t plen, void* parameters)
{
    uint16_t opcode = ogf << 10 | ocf;

    struct __attribute__ ((packed)) {
        uint8_t pktType;
        uint16_t opcode;
        uint8_t plen;
    } pktHdr = {HCI_COMMAND_PKT, opcode, plen};

    uint8_t txBuffer[sizeof(pktHdr) + plen];
    memcpy(txBuffer, &pktHdr, sizeof(pktHdr));
    memcpy(&txBuffer[sizeof(pktHdr)], parameters, plen);

    if (debug) {
        dumpPkt("HCI COMMAND TX -> ", sizeof(pktHdr) + plen, txBuffer);
    }

    int errcode = 0;
    common_hal_busio_uart_write(&adapter->hci_uart, txBuffer, sizeof(pktHdr) + plen, &errcode);
    if (errcode) {
        return -1;
    }

    cmd_complete_opcode = 0xffff;
    cmd_complete_status = -1;

    // Wait up to one second for a response.
    for (uint64_t start = supervisor_ticks_ms64();
         cmd_complete_opcode != opcode && supervisor_ticks_ms64() < (start + 5000);
        ) {
        hci_poll();
    }

    return cmd_complete_status;
}

int hci_reset(void) {
    return hci_sendCommand(OGF_HOST_CTL, OCF_RESET, 0, NULL);
}

int hci_readLocalVersion(uint8_t *hciVer, uint16_t *hciRev, uint8_t *lmpVer, uint16_t *manufacturer, uint16_t *lmpSubVer) {
    int result = hci_sendCommand(OGF_INFO_PARAM, OCF_READ_LOCAL_VERSION, 0, NULL);

    if (result == 0) {
        typedef struct __attribute__ ((packed)) {
            uint8_t hciVer;
            uint16_t hciRev;
            uint8_t lmpVer;
            uint16_t manufacturer;
            uint16_t lmpSubVer;
        } HCILocalVersion;

        HCILocalVersion *localVersion = (HCILocalVersion*)cmd_response;
        *hciVer = localVersion->hciVer;
        *hciRev = localVersion->hciRev;
        *lmpVer = localVersion->lmpVer;
        *manufacturer = localVersion->manufacturer;
        *lmpSubVer = localVersion->lmpSubVer;
    }

    return result;
}

int hci_readBdAddr(uint8_t addr[6]) {
    int result = hci_sendCommand(OGF_INFO_PARAM, OCF_READ_BD_ADDR, 0, NULL);

    if (result == 0) {
        memcpy(addr, cmd_response, 6);
    }

    return result;
}

int hci_readRssi(uint16_t handle) {
    int result = hci_sendCommand(OGF_STATUS_PARAM, OCF_READ_RSSI, sizeof(handle), &handle);
    int rssi = 127;

    if (result == 0) {
        typedef struct __attribute__ ((packed)) {
            uint16_t handle;
            int8_t rssi;
        } HCIReadRssi;

        HCIReadRssi *readRssi = (HCIReadRssi*)cmd_response;
        if (readRssi->handle == handle) {
            rssi = readRssi->rssi;
        }
    }

    return rssi;
}

int hci_setEventMask(uint64_t eventMask) {
    return hci_sendCommand(OGF_HOST_CTL, OCF_SET_EVENT_MASK, sizeof(eventMask), &eventMask);
}

int hci_readLeBufferSize(uint16_t *pktLen, uint8_t *maxPkt)
{
    int result = hci_sendCommand(OGF_LE_CTL, OCF_LE_READ_BUFFER_SIZE, 0, NULL);

    if (result == 0) {
        typedef struct __attribute__ ((packed)) {
            uint16_t pktLen;
            uint8_t maxPkt;
        } HCILeBufferSize;

        HCILeBufferSize *leBufferSize = (HCILeBufferSize*)cmd_response;
        *pktLen = leBufferSize->pktLen;
        *maxPkt = leBufferSize->maxPkt;

#ifndef __AVR__
        // FIX (needed?) ATT.setMaxMtu(pktLen - 9); // max pkt len - ACL header size
#endif
    }

    return result;
}

int hci_leSetRandomAddress(uint8_t addr[6])
{
    return hci_sendCommand(OGF_LE_CTL, OCF_LE_SET_RANDOM_ADDRESS, 6, addr);
}

int hci_leSetAdvertisingParameters(uint16_t minInterval, uint16_t maxInterval,
                                         uint8_t advType, uint8_t ownBdaddrType,
                                         uint8_t directBdaddrType, uint8_t directBdaddr[6],
                                         uint8_t chanMap,
                                         uint8_t filter)
{
    struct __attribute__ ((packed)) HCILeAdvertisingParameters {
        uint16_t minInterval;
        uint16_t maxInterval;
        uint8_t advType;
        uint8_t ownBdaddrType;
        uint8_t directBdaddrType;
        uint8_t directBdaddr[6];
        uint8_t chanMap;
        uint8_t filter;
    } leAdvertisingParamters;

    leAdvertisingParamters.minInterval = minInterval;
    leAdvertisingParamters.maxInterval = maxInterval;
    leAdvertisingParamters.advType = advType;
    leAdvertisingParamters.ownBdaddrType = ownBdaddrType;
    leAdvertisingParamters.directBdaddrType = directBdaddrType;
    memcpy(leAdvertisingParamters.directBdaddr, directBdaddr, 6);
    leAdvertisingParamters.chanMap = chanMap;
    leAdvertisingParamters.filter = filter;

    return hci_sendCommand(OGF_LE_CTL, OCF_LE_SET_ADVERTISING_PARAMETERS, sizeof(leAdvertisingParamters), &leAdvertisingParamters);
}

int hci_leSetAdvertisingData(uint8_t length, uint8_t data[])
{
    struct __attribute__ ((packed)) HCILeAdvertisingData {
        uint8_t length;
        uint8_t data[31];
    } leAdvertisingData;

    memset(&leAdvertisingData, 0, sizeof(leAdvertisingData));
    leAdvertisingData.length = length;
    memcpy(leAdvertisingData.data, data, length);

    return hci_sendCommand(OGF_LE_CTL, OCF_LE_SET_ADVERTISING_DATA, sizeof(leAdvertisingData), &leAdvertisingData);
}

int hci_leSetScanResponseData(uint8_t length, uint8_t data[])
{
    struct __attribute__ ((packed)) HCILeScanResponseData {
        uint8_t length;
        uint8_t data[31];
    } leScanResponseData;

    memset(&leScanResponseData, 0, sizeof(leScanResponseData));
    leScanResponseData.length = length;
    memcpy(leScanResponseData.data, data, length);

    return hci_sendCommand(OGF_LE_CTL, OCF_LE_SET_SCAN_RESPONSE_DATA, sizeof(leScanResponseData), &leScanResponseData);
}

int hci_leSetAdvertiseEnable(uint8_t enable)
{
    return hci_sendCommand(OGF_LE_CTL, OCF_LE_SET_ADVERTISE_ENABLE, sizeof(enable), &enable);
}

int hci_leSetScanParameters(uint8_t type, uint16_t interval, uint16_t window,
                                  uint8_t ownBdaddrType, uint8_t filter)
{
    struct __attribute__ ((packed)) HCILeSetScanParameters {
        uint8_t type;
        uint16_t interval;
        uint16_t window;
        uint8_t ownBdaddrType;
        uint8_t filter;
    } leScanParameters;

    leScanParameters.type = type;
    leScanParameters.interval = interval;
    leScanParameters.window = window;
    leScanParameters.ownBdaddrType = ownBdaddrType;
    leScanParameters.filter = filter;

    return hci_sendCommand(OGF_LE_CTL, OCF_LE_SET_SCAN_PARAMETERS, sizeof(leScanParameters), &leScanParameters);
}

int hci_leSetScanEnable(uint8_t enabled, uint8_t duplicates)
{
    struct __attribute__ ((packed)) HCILeSetScanEnableData {
        uint8_t enabled;
        uint8_t duplicates;
    } leScanEnableData;

    leScanEnableData.enabled = enabled;
    leScanEnableData.duplicates = duplicates;

    return hci_sendCommand(OGF_LE_CTL, OCF_LE_SET_SCAN_ENABLE, sizeof(leScanEnableData), &leScanEnableData);
}

int hci_leCreateConn(uint16_t interval, uint16_t window, uint8_t initiatorFilter,
                           uint8_t peerBdaddrType, uint8_t peerBdaddr[6], uint8_t ownBdaddrType,
                           uint16_t minInterval, uint16_t maxInterval, uint16_t latency,
                           uint16_t supervisionTimeout, uint16_t minCeLength, uint16_t maxCeLength)
{
    struct __attribute__ ((packed)) HCILeCreateConnData {
        uint16_t interval;
        uint16_t window;
        uint8_t initiatorFilter;
        uint8_t peerBdaddrType;
        uint8_t peerBdaddr[6];
        uint8_t ownBdaddrType;
        uint16_t minInterval;
        uint16_t maxInterval;
        uint16_t latency;
        uint16_t supervisionTimeout;
        uint16_t minCeLength;
        uint16_t maxCeLength;
    } leCreateConnData;

    leCreateConnData.interval = interval;
    leCreateConnData.window = window;
    leCreateConnData.initiatorFilter = initiatorFilter;
    leCreateConnData.peerBdaddrType = peerBdaddrType;
    memcpy(leCreateConnData.peerBdaddr, peerBdaddr, sizeof(leCreateConnData.peerBdaddr));
    leCreateConnData.ownBdaddrType = ownBdaddrType;
    leCreateConnData.minInterval = minInterval;
    leCreateConnData.maxInterval = maxInterval;
    leCreateConnData.latency = latency;
    leCreateConnData.supervisionTimeout = supervisionTimeout;
    leCreateConnData.minCeLength = minCeLength;
    leCreateConnData.maxCeLength = maxCeLength;

    return hci_sendCommand(OGF_LE_CTL, OCF_LE_CREATE_CONN, sizeof(leCreateConnData), &leCreateConnData);
}

int hci_leCancelConn()
{
    return hci_sendCommand(OGF_LE_CTL, OCF_LE_CANCEL_CONN, 0, NULL);
}

int hci_leConnUpdate(uint16_t handle, uint16_t minInterval, uint16_t maxInterval,
                           uint16_t latency, uint16_t supervisionTimeout)
{
    struct __attribute__ ((packed)) HCILeConnUpdateData {
        uint16_t handle;
        uint16_t minInterval;
        uint16_t maxInterval;
        uint16_t latency;
        uint16_t supervisionTimeout;
        uint16_t minCeLength;
        uint16_t maxCeLength;
    } leConnUpdateData;

    leConnUpdateData.handle = handle;
    leConnUpdateData.minInterval = minInterval;
    leConnUpdateData.maxInterval = maxInterval;
    leConnUpdateData.latency = latency;
    leConnUpdateData.supervisionTimeout = supervisionTimeout;
    leConnUpdateData.minCeLength = 0x0004;
    leConnUpdateData.maxCeLength = 0x0006;

    return hci_sendCommand(OGF_LE_CTL, OCF_LE_CONN_UPDATE, sizeof(leConnUpdateData), &leConnUpdateData);
}

int hci_sendAclPkt(uint16_t handle, uint8_t cid, uint8_t plen, void* data) {
    while (pending_pkt >= max_pkt) {
        hci_poll();
    }

    typedef struct __attribute__ ((packed)) {
        uint8_t pktType;
        uint16_t handle;
        uint16_t dlen;
        uint16_t plen;
        uint16_t cid;
    }  HCIACLHdr;

    HCIACLHdr aclHdr = { HCI_ACLDATA_PKT, handle, (uint8_t)(plen + 4), plen, cid };

    uint8_t txBuffer[sizeof(aclHdr) + plen];
    memcpy(txBuffer, &aclHdr, sizeof(aclHdr));
    memcpy(&txBuffer[sizeof(aclHdr)], data, plen);

    if (debug) {
        dumpPkt("HCI ACLDATA TX -> ", sizeof(aclHdr) + plen, txBuffer);
    }

    pending_pkt++;

    int errcode = 0;
    common_hal_busio_uart_write(&adapter->hci_uart, txBuffer, sizeof(aclHdr) + plen, &errcode);
    if (errcode) {
        return -1;
    }

    return 0;
}

int hci_disconnect(uint16_t handle)
{
    struct __attribute__ ((packed)) HCIDisconnectData {
        uint16_t handle;
        uint8_t reason;
    } disconnectData = { handle, HCI_OE_USER_ENDED_CONNECTION };

    return hci_sendCommand(OGF_LINK_CTL, OCF_DISCONNECT, sizeof(disconnectData), &disconnectData);
}

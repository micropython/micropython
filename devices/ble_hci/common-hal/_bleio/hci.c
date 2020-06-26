
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "HCI.h"

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

HCIClass::HCIClass() :
  _debug(NULL),
  _recvIndex(0),
  _pendingPkt(0)
{
}

HCIClass::~HCIClass()
{
}

int HCIClass::begin()
{
  _recvIndex = 0;

  return HCITransport.begin();
}

void HCIClass::end()
{
  HCITransport.end();
}

void HCIClass::poll()
{
  poll(0);
}

void HCIClass::poll(unsigned long timeout)
{
#if defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_METRO_M4_AIRLIFT_LITE)
  digitalWrite(NINA_RTS, LOW);
#endif

  if (timeout) {
    HCITransport.wait(timeout);
  }

  while (HCITransport.available()) {
    byte b = HCITransport.read();

    _recvBuffer[_recvIndex++] = b;

    if (_recvBuffer[0] == HCI_ACLDATA_PKT) {
      if (_recvIndex > 5 && _recvIndex >= (5 + (_recvBuffer[3] + (_recvBuffer[4] << 8)))) {
        if (_debug) {
          dumpPkt("HCI ACLDATA RX <- ", _recvIndex, _recvBuffer);
        }
#if defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_METRO_M4_AIRLIFT_LITE)
        digitalWrite(NINA_RTS, HIGH);
#endif
        int pktLen = _recvIndex - 1;
        _recvIndex = 0;

        handleAclDataPkt(pktLen, &_recvBuffer[1]);

#if defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_METRO_M4_AIRLIFT_LITE)
        digitalWrite(NINA_RTS, LOW);
#endif
      }
    } else if (_recvBuffer[0] == HCI_EVENT_PKT) {
      if (_recvIndex > 3 && _recvIndex >= (3 + _recvBuffer[2])) {
        if (_debug) {
          dumpPkt("HCI EVENT RX <- ", _recvIndex, _recvBuffer);
        }
#if defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_METRO_M4_AIRLIFT_LITE)
        digitalWrite(NINA_RTS, HIGH);
#endif
        // received full event
        int pktLen = _recvIndex - 1;
        _recvIndex = 0;

        handleEventPkt(pktLen, &_recvBuffer[1]);

#if defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_METRO_M4_AIRLIFT_LITE)
        digitalWrite(NINA_RTS, LOW);
#endif
      }
    } else {
      _recvIndex = 0;

      if (_debug) {
        _debug->println(b, HEX);
      }
    }
  }

#if defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_METRO_M4_AIRLIFT_LITE)
  digitalWrite(NINA_RTS, HIGH);
#endif
}

int HCIClass::reset()
{
  return sendCommand(OGF_HOST_CTL << 10 | OCF_RESET);
}

int HCIClass::readLocalVersion(uint8_t& hciVer, uint16_t& hciRev, uint8_t& lmpVer, uint16_t& manufacturer, uint16_t& lmpSubVer)
{
  int result = sendCommand(OGF_INFO_PARAM << 10 | OCF_READ_LOCAL_VERSION);

  if (result == 0) {
    struct __attribute__ ((packed)) HCILocalVersion {
      uint8_t hciVer;
      uint16_t hciRev;
      uint8_t lmpVer;
      uint16_t manufacturer;
      uint16_t lmpSubVer;
    } *localVersion = (HCILocalVersion*)_cmdResponse;

    hciVer = localVersion->hciVer;
    hciRev = localVersion->hciRev;
    lmpVer = localVersion->lmpVer;
    manufacturer = localVersion->manufacturer;
    lmpSubVer = localVersion->lmpSubVer;
  }

  return result;
}

int HCIClass::readBdAddr(uint8_t addr[6])
{
  int result = sendCommand(OGF_INFO_PARAM << 10 | OCF_READ_BD_ADDR);

  if (result == 0) {
    memcpy(addr, _cmdResponse, 6);
  }

  return result;
}

int HCIClass::readRssi(uint16_t handle)
{
  int result = sendCommand(OGF_STATUS_PARAM << 10 | OCF_READ_RSSI, sizeof(handle), &handle);
  int rssi = 127;

  if (result == 0) {
    struct __attribute__ ((packed)) HCIReadRssi {
      uint16_t handle;
        int8_t rssi;
    } *readRssi = (HCIReadRssi*)_cmdResponse;

    if (readRssi->handle == handle) {
      rssi = readRssi->rssi;
    }
  }

  return rssi;
}

int HCIClass::setEventMask(uint64_t eventMask)
{
  return sendCommand(OGF_HOST_CTL << 10 | OCF_SET_EVENT_MASK, sizeof(eventMask), &eventMask);
}

int HCIClass::readLeBufferSize(uint16_t& pktLen, uint8_t& maxPkt)
{
  int result = sendCommand(OGF_LE_CTL << 10 | OCF_LE_READ_BUFFER_SIZE);

  if (result == 0) {
    struct __attribute__ ((packed)) HCILeBufferSize {
      uint16_t pktLen;
      uint8_t maxPkt;
    } *leBufferSize = (HCILeBufferSize*)_cmdResponse;

    pktLen = leBufferSize->pktLen;
    _maxPkt = maxPkt = leBufferSize->maxPkt;

#ifndef __AVR__
    ATT.setMaxMtu(pktLen - 9); // max pkt len - ACL header size
#endif
  }

  return result;
}

int HCIClass::leSetRandomAddress(uint8_t addr[6])
{
  return sendCommand(OGF_LE_CTL << 10 | OCF_LE_SET_RANDOM_ADDRESS, 6, addr);
}

int HCIClass::leSetAdvertisingParameters(uint16_t minInterval, uint16_t maxInterval,
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

  return sendCommand(OGF_LE_CTL << 10 | OCF_LE_SET_ADVERTISING_PARAMETERS, sizeof(leAdvertisingParamters), &leAdvertisingParamters);
}

int HCIClass::leSetAdvertisingData(uint8_t length, uint8_t data[])
{
  struct __attribute__ ((packed)) HCILeAdvertisingData {
    uint8_t length;
    uint8_t data[31];
  } leAdvertisingData;

  memset(&leAdvertisingData, 0, sizeof(leAdvertisingData));
  leAdvertisingData.length = length;
  memcpy(leAdvertisingData.data, data, length);

  return sendCommand(OGF_LE_CTL << 10 | OCF_LE_SET_ADVERTISING_DATA, sizeof(leAdvertisingData), &leAdvertisingData);
}

int HCIClass::leSetScanResponseData(uint8_t length, uint8_t data[])
{
  struct __attribute__ ((packed)) HCILeScanResponseData {
    uint8_t length;
    uint8_t data[31];
  } leScanResponseData;

  memset(&leScanResponseData, 0, sizeof(leScanResponseData));
  leScanResponseData.length = length;
  memcpy(leScanResponseData.data, data, length);

  return sendCommand(OGF_LE_CTL << 10 | OCF_LE_SET_SCAN_RESPONSE_DATA, sizeof(leScanResponseData), &leScanResponseData);
}

int HCIClass::leSetAdvertiseEnable(uint8_t enable)
{
  return sendCommand(OGF_LE_CTL << 10 | OCF_LE_SET_ADVERTISE_ENABLE, sizeof(enable), &enable);
}

int HCIClass::leSetScanParameters(uint8_t type, uint16_t interval, uint16_t window,
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

  return sendCommand(OGF_LE_CTL << 10 | OCF_LE_SET_SCAN_PARAMETERS, sizeof(leScanParameters), &leScanParameters);
}

int HCIClass::leSetScanEnable(uint8_t enabled, uint8_t duplicates)
{
  struct __attribute__ ((packed)) HCILeSetScanEnableData {
    uint8_t enabled;
    uint8_t duplicates;
  } leScanEnableData;

  leScanEnableData.enabled = enabled;
  leScanEnableData.duplicates = duplicates;

  return sendCommand(OGF_LE_CTL << 10 | OCF_LE_SET_SCAN_ENABLE, sizeof(leScanEnableData), &leScanEnableData);
}

int HCIClass::leCreateConn(uint16_t interval, uint16_t window, uint8_t initiatorFilter,
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

  return sendCommand(OGF_LE_CTL << 10 | OCF_LE_CREATE_CONN, sizeof(leCreateConnData), &leCreateConnData);
}

int HCIClass::leCancelConn()
{
  return sendCommand(OGF_LE_CTL << 10 | OCF_LE_CANCEL_CONN, 0, NULL);
}

int HCIClass::leConnUpdate(uint16_t handle, uint16_t minInterval, uint16_t maxInterval,
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

  return sendCommand(OGF_LE_CTL << 10 | OCF_LE_CONN_UPDATE, sizeof(leConnUpdateData), &leConnUpdateData);
}

int HCIClass::sendAclPkt(uint16_t handle, uint8_t cid, uint8_t plen, void* data)
{
  while (_pendingPkt >= _maxPkt) {
    poll();
  }

  struct __attribute__ ((packed)) HCIACLHdr {
    uint8_t pktType;
    uint16_t handle;
    uint16_t dlen;
    uint16_t plen;
    uint16_t cid;
  } aclHdr = { HCI_ACLDATA_PKT, handle, uint8_t(plen + 4), plen, cid };

  uint8_t txBuffer[sizeof(aclHdr) + plen];
  memcpy(txBuffer, &aclHdr, sizeof(aclHdr));
  memcpy(&txBuffer[sizeof(aclHdr)], data, plen);

  if (_debug) {
    dumpPkt("HCI ACLDATA TX -> ", sizeof(aclHdr) + plen, txBuffer);
  }

  _pendingPkt++;
  HCITransport.write(txBuffer, sizeof(aclHdr) + plen);

  return 0;
}

int HCIClass::disconnect(uint16_t handle)
{
    struct __attribute__ ((packed)) HCIDisconnectData {
    uint16_t handle;
    uint8_t reason;
  } disconnectData = { handle, HCI_OE_USER_ENDED_CONNECTION };

  return sendCommand(OGF_LINK_CTL << 10 | OCF_DISCONNECT, sizeof(disconnectData), &disconnectData);
}

void HCIClass::debug(Stream& stream)
{
  _debug = &stream;
}

void HCIClass::noDebug()
{
  _debug = NULL;
}

int HCIClass::sendCommand(uint16_t opcode, uint8_t plen, void* parameters)
{
  struct __attribute__ ((packed)) {
    uint8_t pktType;
    uint16_t opcode;
    uint8_t plen;
  } pktHdr = {HCI_COMMAND_PKT, opcode, plen};

  uint8_t txBuffer[sizeof(pktHdr) + plen];
  memcpy(txBuffer, &pktHdr, sizeof(pktHdr));
  memcpy(&txBuffer[sizeof(pktHdr)], parameters, plen);

  if (_debug) {
    dumpPkt("HCI COMMAND TX -> ", sizeof(pktHdr) + plen, txBuffer);
  }

  HCITransport.write(txBuffer, sizeof(pktHdr) + plen);

  _cmdCompleteOpcode = 0xffff;
  _cmdCompleteStatus = -1;

  for (unsigned long start = millis(); _cmdCompleteOpcode != opcode && millis() < (start + 1000);) {
    poll();
  }

  return _cmdCompleteStatus;
}

void HCIClass::handleAclDataPkt(uint8_t /*plen*/, uint8_t pdata[])
{
  struct __attribute__ ((packed)) HCIACLHdr {
    uint16_t handle;
    uint16_t dlen;
    uint16_t len;
    uint16_t cid;
  } *aclHdr = (HCIACLHdr*)pdata;

  uint16_t aclFlags = (aclHdr->handle & 0xf000) >> 12;

  if ((aclHdr->dlen - 4) != aclHdr->len) {
    // packet is fragmented
    if (aclFlags != 0x01) {
      // copy into ACL buffer
      memcpy(_aclPktBuffer, &_recvBuffer[1], sizeof(HCIACLHdr) + aclHdr->dlen - 4);
    } else {
      // copy next chunk into the buffer
      HCIACLHdr* aclBufferHeader = (HCIACLHdr*)_aclPktBuffer;

      memcpy(&_aclPktBuffer[sizeof(HCIACLHdr) + aclBufferHeader->dlen - 4], &_recvBuffer[1 + sizeof(aclHdr->handle) + sizeof(aclHdr->dlen)], aclHdr->dlen);

      aclBufferHeader->dlen += aclHdr->dlen;
      aclHdr = aclBufferHeader;
    }
  }

  if ((aclHdr->dlen - 4) != aclHdr->len) {
    // don't have the full packet yet
    return;
  }

  if (aclHdr->cid == ATT_CID) {
    if (aclFlags == 0x01) {
      // use buffered packet
      ATT.handleData(aclHdr->handle & 0x0fff, aclHdr->len, &_aclPktBuffer[sizeof(HCIACLHdr)]);
    } else {
      // use the recv buffer
      ATT.handleData(aclHdr->handle & 0x0fff, aclHdr->len, &_recvBuffer[1 + sizeof(HCIACLHdr)]);
    }
  } else if (aclHdr->cid == SIGNALING_CID) {
    L2CAPSignaling.handleData(aclHdr->handle & 0x0fff, aclHdr->len, &_recvBuffer[1 + sizeof(HCIACLHdr)]);
  } else {
    struct __attribute__ ((packed)) {
      uint8_t op;
      uint8_t id;
      uint16_t length;
      uint16_t reason;
      uint16_t localCid;
      uint16_t remoteCid;
    } l2capRejectCid= { 0x01, 0x00, 0x006, 0x0002, aclHdr->cid, 0x0000 };

    sendAclPkt(aclHdr->handle & 0x0fff, 0x0005, sizeof(l2capRejectCid), &l2capRejectCid);
  }
}

void HCIClass::handleNumCompPkts(uint16_t /*handle*/, uint16_t numPkts)
{
  if (numPkts && _pendingPkt > numPkts) {
    _pendingPkt -= numPkts;
  } else {
    _pendingPkt = 0;
  }
}

void HCIClass::handleEventPkt(uint8_t /*plen*/, uint8_t pdata[])
{
  struct __attribute__ ((packed)) HCIEventHdr {
    uint8_t evt;
    uint8_t plen;
  } *eventHdr = (HCIEventHdr*)pdata;

  if (eventHdr->evt == EVT_DISCONN_COMPLETE) {
    struct __attribute__ ((packed)) DisconnComplete {
      uint8_t status;
      uint16_t handle;
      uint8_t reason;
    } *disconnComplete = (DisconnComplete*)&pdata[sizeof(HCIEventHdr)];

    ATT.removeConnection(disconnComplete->handle, disconnComplete->reason);
    L2CAPSignaling.removeConnection(disconnComplete->handle, disconnComplete->reason);

    HCI.leSetAdvertiseEnable(0x01);
  } else if (eventHdr->evt == EVT_CMD_COMPLETE) {
    struct __attribute__ ((packed)) CmdComplete {
      uint8_t ncmd;
      uint16_t opcode;
      uint8_t status;
    } *cmdCompleteHeader = (CmdComplete*)&pdata[sizeof(HCIEventHdr)];

    _cmdCompleteOpcode = cmdCompleteHeader->opcode;
    _cmdCompleteStatus = cmdCompleteHeader->status;
    _cmdResponseLen = pdata[1] - sizeof(CmdComplete);
    _cmdResponse = &pdata[sizeof(HCIEventHdr) + sizeof(CmdComplete)];

  } else if (eventHdr->evt == EVT_CMD_STATUS) {
    struct __attribute__ ((packed)) CmdStatus {
      uint8_t status;
      uint8_t ncmd;
      uint16_t opcode;
    } *cmdStatusHeader = (CmdStatus*)&pdata[sizeof(HCIEventHdr)];

    _cmdCompleteOpcode = cmdStatusHeader->opcode;
    _cmdCompleteStatus = cmdStatusHeader->status;
    _cmdResponseLen = 0;
  } else if (eventHdr->evt == EVT_NUM_COMP_PKTS) {
    uint8_t numHandles = pdata[sizeof(HCIEventHdr)];
    uint16_t* data = (uint16_t*)&pdata[sizeof(HCIEventHdr) + sizeof(numHandles)];

    for (uint8_t i = 0; i < numHandles; i++) {
      handleNumCompPkts(data[0], data[1]);

      data += 2;
    }
  } else if (eventHdr->evt == EVT_LE_META_EVENT) {
    struct __attribute__ ((packed)) LeMetaEventHeader {
      uint8_t subevent;
    } *leMetaHeader = (LeMetaEventHeader*)&pdata[sizeof(HCIEventHdr)];

    if (leMetaHeader->subevent == EVT_LE_CONN_COMPLETE) {
      struct __attribute__ ((packed)) EvtLeConnectionComplete {
        uint8_t status;
        uint16_t handle;
        uint8_t role;
        uint8_t peerBdaddrType;
        uint8_t peerBdaddr[6];
        uint16_t interval;
        uint16_t latency;
        uint16_t supervisionTimeout;
        uint8_t masterClockAccuracy;
      } *leConnectionComplete = (EvtLeConnectionComplete*)&pdata[sizeof(HCIEventHdr) + sizeof(LeMetaEventHeader)];

      if (leConnectionComplete->status == 0x00) {
        ATT.addConnection(leConnectionComplete->handle,
                          leConnectionComplete->role,
                          leConnectionComplete->peerBdaddrType,
                          leConnectionComplete->peerBdaddr,
                          leConnectionComplete->interval,
                          leConnectionComplete->latency,
                          leConnectionComplete->supervisionTimeout,
                          leConnectionComplete->masterClockAccuracy);

        L2CAPSignaling.addConnection(leConnectionComplete->handle,
                              leConnectionComplete->role,
                              leConnectionComplete->peerBdaddrType,
                              leConnectionComplete->peerBdaddr,
                              leConnectionComplete->interval,
                              leConnectionComplete->latency,
                              leConnectionComplete->supervisionTimeout,
                              leConnectionComplete->masterClockAccuracy);
      }
    } else if (leMetaHeader->subevent == EVT_LE_ADVERTISING_REPORT) {
      struct __attribute__ ((packed)) EvtLeAdvertisingReport {
        uint8_t status;
        uint8_t type;
        uint8_t peerBdaddrType;
        uint8_t peerBdaddr[6];
        uint8_t eirLength;
        uint8_t eirData[31];
      } *leAdvertisingReport = (EvtLeAdvertisingReport*)&pdata[sizeof(HCIEventHdr) + sizeof(LeMetaEventHeader)];

      if (leAdvertisingReport->status == 0x01) {
        // last byte is RSSI
        int8_t rssi = leAdvertisingReport->eirData[leAdvertisingReport->eirLength];

        GAP.handleLeAdvertisingReport(leAdvertisingReport->type,
                                      leAdvertisingReport->peerBdaddrType,
                                      leAdvertisingReport->peerBdaddr,
                                      leAdvertisingReport->eirLength,
                                      leAdvertisingReport->eirData,
                                      rssi);

      }
    }
  }
}

void HCIClass::dumpPkt(const char* prefix, uint8_t plen, uint8_t pdata[])
{
  if (_debug) {
    _debug->print(prefix);

    for (uint8_t i = 0; i < plen; i++) {
      byte b = pdata[i];

      if (b < 16) {
        _debug->print("0");
      }

      _debug->print(b, HEX);
    }

    _debug->println();
    _debug->flush();
  }
}
`

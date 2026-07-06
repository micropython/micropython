/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Nicko van Someren
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

/* Typed EFI networking protocols for the MicroPython UEFI `network`/`socket` port.
 *
 * Hand-rolled subset, same style as efi.h: unused vtable slots are padded with
 * void* so the typed members land at the correct offsets. Compiled with the UEFI/MS
 * calling convention as the default (EFIAPI empty). Expand as later network phases
 * (DNS/UDP/TCP/DHCP/WiFi) need more protocols. See README.md.
 */
#ifndef MPY_UEFI_EFI_NET_H
#define MPY_UEFI_EFI_NET_H

#include "efi.h"

/* ---- Address types (UEFI 2.x) ---- */
typedef struct { UINT8 Addr[4];
} EFI_IPv4_ADDRESS;
typedef struct { UINT8 Addr[16];
} EFI_IPv6_ADDRESS;
typedef struct { UINT8 Addr[32];
} EFI_MAC_ADDRESS;
typedef union { UINT32 Addr[4];
                EFI_IPv4_ADDRESS v4;
                EFI_IPv6_ADDRESS v6;
} EFI_IP_ADDRESS;

/* ---- EFI_SERVICE_BINDING_PROTOCOL (spawns per-instance child handles) ---- */
typedef struct _EFI_SERVICE_BINDING_PROTOCOL EFI_SERVICE_BINDING_PROTOCOL;
struct _EFI_SERVICE_BINDING_PROTOCOL {
    EFI_STATUS(EFIAPI * CreateChild)(EFI_SERVICE_BINDING_PROTOCOL * This, EFI_HANDLE *ChildHandle);
    EFI_STATUS(EFIAPI * DestroyChild)(EFI_SERVICE_BINDING_PROTOCOL * This, EFI_HANDLE ChildHandle);
};

/* ---- EFI_SIMPLE_NETWORK_PROTOCOL (NIC: start/stop, MAC, link) ---- */
/* SNP State values. */
#define EFI_SIMPLE_NETWORK_STOPPED     0
#define EFI_SIMPLE_NETWORK_STARTED     1
#define EFI_SIMPLE_NETWORK_INITIALIZED 2

typedef struct {
    UINT32 State;
    UINT32 HwAddressSize;
    UINT32 MediaHeaderSize;
    UINT32 MaxPacketSize;
    UINT32 NvRamSize;
    UINT32 NvRamAccessSize;
    UINT32 ReceiveFilterMask;
    UINT32 ReceiveFilterSetting;
    UINT32 MaxMCastFilterCount;
    UINT32 MCastFilterCount;
    EFI_MAC_ADDRESS MCastFilter[16];      /* MAX_MCAST_FILTER_CNT */
    EFI_MAC_ADDRESS CurrentAddress;
    EFI_MAC_ADDRESS BroadcastAddress;
    EFI_MAC_ADDRESS PermanentAddress;
    UINT8 IfType;
    BOOLEAN MacAddressChangeable;
    BOOLEAN MultipleTxSupported;
    BOOLEAN MediaPresentSupported;
    BOOLEAN MediaPresent;
} EFI_SIMPLE_NETWORK_MODE;

typedef struct _EFI_SIMPLE_NETWORK_PROTOCOL EFI_SIMPLE_NETWORK_PROTOCOL;
struct _EFI_SIMPLE_NETWORK_PROTOCOL {
    UINT64 Revision;
    EFI_STATUS(EFIAPI * Start)(EFI_SIMPLE_NETWORK_PROTOCOL * This);
    EFI_STATUS(EFIAPI * Stop)(EFI_SIMPLE_NETWORK_PROTOCOL * This);
    EFI_STATUS(EFIAPI * Initialize)(EFI_SIMPLE_NETWORK_PROTOCOL * This, UINTN ExtraRx, UINTN ExtraTx);
    EFI_STATUS(EFIAPI * Reset)(EFI_SIMPLE_NETWORK_PROTOCOL * This, BOOLEAN ExtendedVerification);
    EFI_STATUS(EFIAPI * Shutdown)(EFI_SIMPLE_NETWORK_PROTOCOL * This);
    void *ReceiveFilters;
    void *StationAddress;
    void *Statistics;
    void *MCastIpToMac;
    void *NvData;
    void *GetStatus;
    void *Transmit;
    void *Receive;
    EFI_EVENT WaitForPacket;
    EFI_SIMPLE_NETWORK_MODE *Mode;
};

/* ---- EFI_IP4_CONFIG2_PROTOCOL (per-interface IPv4 configuration) ---- */
typedef enum {
    Ip4Config2DataTypeInterfaceInfo,
    Ip4Config2DataTypePolicy,
    Ip4Config2DataTypeManualAddress,
    Ip4Config2DataTypeGateway,
    Ip4Config2DataTypeDnsServer,
    Ip4Config2DataTypeMaximum
} EFI_IP4_CONFIG2_DATA_TYPE;

typedef enum {
    Ip4Config2PolicyStatic,
    Ip4Config2PolicyDhcp,
    Ip4Config2PolicyMax
} EFI_IP4_CONFIG2_POLICY;

typedef struct {
    EFI_IPv4_ADDRESS SubnetAddress;
    EFI_IPv4_ADDRESS SubnetMask;
    EFI_IPv4_ADDRESS GatewayAddress;
} EFI_IP4_ROUTE_TABLE;

typedef struct {
    CHAR16 Name[32];
    UINT8 IfType;
    UINT32 HwAddressSize;
    EFI_MAC_ADDRESS HwAddress;
    EFI_IPv4_ADDRESS StationAddress;
    EFI_IPv4_ADDRESS SubnetMask;
    UINT32 RouteTableSize;
    EFI_IP4_ROUTE_TABLE *RouteTable;
} EFI_IP4_CONFIG2_INTERFACE_INFO;

typedef struct {
    EFI_IPv4_ADDRESS Address;
    EFI_IPv4_ADDRESS SubnetMask;
} EFI_IP4_CONFIG2_MANUAL_ADDRESS;

typedef struct _EFI_IP4_CONFIG2_PROTOCOL EFI_IP4_CONFIG2_PROTOCOL;
struct _EFI_IP4_CONFIG2_PROTOCOL {
    EFI_STATUS(EFIAPI * SetData)(EFI_IP4_CONFIG2_PROTOCOL * This, EFI_IP4_CONFIG2_DATA_TYPE DataType,
        UINTN DataSize, void *Data);
    EFI_STATUS(EFIAPI * GetData)(EFI_IP4_CONFIG2_PROTOCOL * This, EFI_IP4_CONFIG2_DATA_TYPE DataType,
        UINTN *DataSize, void *Data);
    EFI_STATUS(EFIAPI * RegisterDataNotify)(EFI_IP4_CONFIG2_PROTOCOL * This,
        EFI_IP4_CONFIG2_DATA_TYPE DataType, EFI_EVENT Event);
    EFI_STATUS(EFIAPI * UnregisterDataNotify)(EFI_IP4_CONFIG2_PROTOCOL * This,
        EFI_IP4_CONFIG2_DATA_TYPE DataType, EFI_EVENT Event);
};

/* ---- EFI_DNS4_PROTOCOL (name resolution; token/event based) ---- */
typedef struct {
    UINT32 IpCount;
    EFI_IPv4_ADDRESS *IpList;
} EFI_DNS4_HOST_TO_ADDR_DATA;

typedef struct {
    EFI_EVENT Event;
    EFI_STATUS Status;
    UINT32 RetryCount;
    UINT32 RetryInterval;
    /* EFI_DNS4_RSP_DATA is a union of pointers; H2AData is the HostNameToIp result. */
    EFI_DNS4_HOST_TO_ADDR_DATA *RspData;
} EFI_DNS4_COMPLETION_TOKEN;

typedef struct {
    UINTN DnsServerListCount;
    EFI_IPv4_ADDRESS *DnsServerList;
    BOOLEAN UseDefaultSetting;   /* take station IP + DNS servers from the IP4 config (DHCP) */
    BOOLEAN EnableDnsCache;
    UINT8 Protocol;              /* EFI_IP_PROTO_UDP = 17 */
    EFI_IPv4_ADDRESS StationIp;
    EFI_IPv4_ADDRESS SubnetMask;
    UINT16 LocalPort;
    UINT32 RetryCount;
    UINT32 RetryInterval;
} EFI_DNS4_CONFIG_DATA;

typedef struct _EFI_DNS4_PROTOCOL EFI_DNS4_PROTOCOL;
struct _EFI_DNS4_PROTOCOL {
    void *GetModeData;
    EFI_STATUS(EFIAPI * Configure)(EFI_DNS4_PROTOCOL * This, EFI_DNS4_CONFIG_DATA *DnsConfigData);
    EFI_STATUS(EFIAPI * HostNameToIp)(EFI_DNS4_PROTOCOL * This, CHAR16 *HostName,
        EFI_DNS4_COMPLETION_TOKEN *Token);
    void *IpToHostName;
    void *GeneralLookUp;
    void *UpdateDnsCache;
    EFI_STATUS(EFIAPI * Poll)(EFI_DNS4_PROTOCOL * This);
    EFI_STATUS(EFIAPI * Cancel)(EFI_DNS4_PROTOCOL * This, EFI_DNS4_COMPLETION_TOKEN *Token);
};

/* ---- EFI_IP4_PROTOCOL (raw IP; used for ICMP echo / ping) ---- */
typedef struct {
    UINT32 FragmentLength;
    void *FragmentBuffer;
} EFI_IP4_FRAGMENT_DATA;

typedef struct {
    EFI_IPv4_ADDRESS DestinationAddress;
    void *OverrideData;              /* EFI_IP4_OVERRIDE_DATA*, OPTIONAL */
    UINT32 OptionsLength;
    void *OptionsBuffer;
    UINT32 TotalDataLength;
    UINT32 FragmentCount;
    EFI_IP4_FRAGMENT_DATA FragmentTable[1];
} EFI_IP4_TRANSMIT_DATA;

typedef struct {
    EFI_TIME TimeStamp;
    EFI_EVENT RecycleSignal;
    UINT32 HeaderLength;
    void *Header;                    /* EFI_IP4_HEADER* */
    UINT32 OptionsLength;
    void *OptionsBuffer;
    UINT32 DataLength;
    UINT32 FragmentCount;
    EFI_IP4_FRAGMENT_DATA FragmentTable[1];
} EFI_IP4_RECEIVE_DATA;

typedef struct {
    EFI_EVENT Event;
    EFI_STATUS Status;
    union {
        EFI_IP4_RECEIVE_DATA *RxData;
        EFI_IP4_TRANSMIT_DATA *TxData;
    } Packet;
} EFI_IP4_COMPLETION_TOKEN;

typedef struct {
    UINT8 DefaultProtocol;           /* 1 = ICMP */
    BOOLEAN AcceptAnyProtocol;
    BOOLEAN AcceptIcmpErrors;
    BOOLEAN AcceptBroadcast;
    BOOLEAN AcceptPromiscuous;
    BOOLEAN UseDefaultAddress;
    EFI_IPv4_ADDRESS StationAddress;
    EFI_IPv4_ADDRESS SubnetMask;
    UINT8 TypeOfService;
    UINT8 TimeToLive;
    BOOLEAN DoNotFragment;
    BOOLEAN RawData;
    UINT32 ReceiveTimeout;
    UINT32 TransmitTimeout;
} EFI_IP4_CONFIG_DATA;

typedef struct _EFI_IP4_PROTOCOL EFI_IP4_PROTOCOL;
struct _EFI_IP4_PROTOCOL {
    void *GetModeData;
    EFI_STATUS(EFIAPI * Configure)(EFI_IP4_PROTOCOL * This, EFI_IP4_CONFIG_DATA *IpConfigData);
    void *Groups;
    void *Routes;
    EFI_STATUS(EFIAPI * Transmit)(EFI_IP4_PROTOCOL * This, EFI_IP4_COMPLETION_TOKEN *Token);
    EFI_STATUS(EFIAPI * Receive)(EFI_IP4_PROTOCOL * This, EFI_IP4_COMPLETION_TOKEN *Token);
    EFI_STATUS(EFIAPI * Cancel)(EFI_IP4_PROTOCOL * This, EFI_IP4_COMPLETION_TOKEN *Token);
    EFI_STATUS(EFIAPI * Poll)(EFI_IP4_PROTOCOL * This);
};

/* ---- EFI_UDP4_PROTOCOL (datagram transport; token/event based) ---- */
typedef struct {
    EFI_IPv4_ADDRESS SourceAddress;
    UINT16 SourcePort;
    EFI_IPv4_ADDRESS DestinationAddress;
    UINT16 DestinationPort;
} EFI_UDP4_SESSION_DATA;

typedef struct {
    UINT32 FragmentLength;
    void *FragmentBuffer;
} EFI_UDP4_FRAGMENT_DATA;

typedef struct {
    EFI_UDP4_SESSION_DATA *UdpSessionData;   /* OPTIONAL per-packet destination override */
    EFI_IPv4_ADDRESS *GatewayAddress;        /* OPTIONAL */
    UINT32 DataLength;
    UINT32 FragmentCount;
    EFI_UDP4_FRAGMENT_DATA FragmentTable[1];
} EFI_UDP4_TRANSMIT_DATA;

typedef struct {
    EFI_TIME TimeStamp;
    EFI_EVENT RecycleSignal;                 /* signal to release the buffer after use */
    EFI_UDP4_SESSION_DATA UdpSession;        /* the datagram's source/dest */
    UINT32 DataLength;
    UINT32 FragmentCount;
    EFI_UDP4_FRAGMENT_DATA FragmentTable[1];
} EFI_UDP4_RECEIVE_DATA;

typedef struct {
    EFI_EVENT Event;
    EFI_STATUS Status;
    union {
        EFI_UDP4_RECEIVE_DATA *RxData;
        EFI_UDP4_TRANSMIT_DATA *TxData;
    } Packet;
} EFI_UDP4_COMPLETION_TOKEN;

typedef struct {
    BOOLEAN AcceptBroadcast;
    BOOLEAN AcceptPromiscuous;
    BOOLEAN AcceptAnyPort;
    BOOLEAN AllowDuplicatePort;
    UINT8 TypeOfService;
    UINT8 TimeToLive;
    BOOLEAN DoNotFragment;
    UINT32 ReceiveTimeout;
    UINT32 TransmitTimeout;
    BOOLEAN UseDefaultAddress;
    EFI_IPv4_ADDRESS StationAddress;
    EFI_IPv4_ADDRESS SubnetMask;
    UINT16 StationPort;
    EFI_IPv4_ADDRESS RemoteAddress;
    UINT16 RemotePort;
} EFI_UDP4_CONFIG_DATA;

typedef struct _EFI_UDP4_PROTOCOL EFI_UDP4_PROTOCOL;
struct _EFI_UDP4_PROTOCOL {
    void *GetModeData;
    EFI_STATUS(EFIAPI * Configure)(EFI_UDP4_PROTOCOL * This, EFI_UDP4_CONFIG_DATA *UdpConfigData);
    void *Groups;
    void *Routes;
    EFI_STATUS(EFIAPI * Transmit)(EFI_UDP4_PROTOCOL * This, EFI_UDP4_COMPLETION_TOKEN *Token);
    EFI_STATUS(EFIAPI * Receive)(EFI_UDP4_PROTOCOL * This, EFI_UDP4_COMPLETION_TOKEN *Token);
    EFI_STATUS(EFIAPI * Cancel)(EFI_UDP4_PROTOCOL * This, EFI_UDP4_COMPLETION_TOKEN *Token);
    EFI_STATUS(EFIAPI * Poll)(EFI_UDP4_PROTOCOL * This);
};

/* ---- EFI_TCP4_PROTOCOL (stream transport; token/event based) ---- */
/* TCP connection status codes (error range, but FIN/RESET are normal end-of-stream). */
#ifndef EFI_CONNECTION_FIN
#define EFI_CONNECTION_FIN     EFIERR(104)
#define EFI_CONNECTION_RESET   EFIERR(105)
#define EFI_CONNECTION_REFUSED EFIERR(106)
#endif

typedef struct {
    BOOLEAN UseDefaultAddress;
    EFI_IPv4_ADDRESS StationAddress;
    EFI_IPv4_ADDRESS SubnetMask;
    UINT16 StationPort;
    EFI_IPv4_ADDRESS RemoteAddress;
    UINT16 RemotePort;
    BOOLEAN ActiveFlag;              /* TRUE = active (client) open */
} EFI_TCP4_ACCESS_POINT;

typedef struct {
    UINT8 TypeOfService;
    UINT8 TimeToLive;
    EFI_TCP4_ACCESS_POINT AccessPoint;
    void *ControlOption;             /* EFI_TCP4_OPTION*, OPTIONAL */
} EFI_TCP4_CONFIG_DATA;

typedef struct {
    EFI_EVENT Event;
    EFI_STATUS Status;
} EFI_TCP4_COMPLETION_TOKEN;

typedef struct {
    EFI_TCP4_COMPLETION_TOKEN CompletionToken;
} EFI_TCP4_CONNECTION_TOKEN;

typedef struct {
    EFI_TCP4_COMPLETION_TOKEN CompletionToken;
    EFI_HANDLE NewChildHandle;       /* the accepted connection's TCP4 child */
} EFI_TCP4_LISTEN_TOKEN;

typedef struct {
    EFI_TCP4_COMPLETION_TOKEN CompletionToken;
    BOOLEAN AbortOnClose;
} EFI_TCP4_CLOSE_TOKEN;

typedef struct {
    UINT32 FragmentLength;
    void *FragmentBuffer;
} EFI_TCP4_FRAGMENT_DATA;

typedef struct {
    BOOLEAN Push;
    BOOLEAN Urgent;
    UINT32 DataLength;
    UINT32 FragmentCount;
    EFI_TCP4_FRAGMENT_DATA FragmentTable[1];
} EFI_TCP4_TRANSMIT_DATA;

typedef struct {
    BOOLEAN UrgentFlag;
    UINT32 DataLength;               /* in: buffer size; out: bytes received */
    UINT32 FragmentCount;
    EFI_TCP4_FRAGMENT_DATA FragmentTable[1];
} EFI_TCP4_RECEIVE_DATA;

typedef struct {
    EFI_TCP4_COMPLETION_TOKEN CompletionToken;
    union {
        EFI_TCP4_RECEIVE_DATA *RxData;
        EFI_TCP4_TRANSMIT_DATA *TxData;
    } Packet;
} EFI_TCP4_IO_TOKEN;

typedef struct _EFI_TCP4_PROTOCOL EFI_TCP4_PROTOCOL;
struct _EFI_TCP4_PROTOCOL {
    EFI_STATUS(EFIAPI * GetModeData)(EFI_TCP4_PROTOCOL * This, void *Tcp4State,
        EFI_TCP4_CONFIG_DATA *Tcp4ConfigData, void *Ip4ModeData, void *MnpConfigData, void *SnpModeData);
    EFI_STATUS(EFIAPI * Configure)(EFI_TCP4_PROTOCOL * This, EFI_TCP4_CONFIG_DATA *TcpConfigData);
    void *Routes;
    EFI_STATUS(EFIAPI * Connect)(EFI_TCP4_PROTOCOL * This, EFI_TCP4_CONNECTION_TOKEN *ConnectionToken);
    EFI_STATUS(EFIAPI * Accept)(EFI_TCP4_PROTOCOL * This, EFI_TCP4_LISTEN_TOKEN *ListenToken);
    EFI_STATUS(EFIAPI * Transmit)(EFI_TCP4_PROTOCOL * This, EFI_TCP4_IO_TOKEN *Token);
    EFI_STATUS(EFIAPI * Receive)(EFI_TCP4_PROTOCOL * This, EFI_TCP4_IO_TOKEN *Token);
    EFI_STATUS(EFIAPI * Close)(EFI_TCP4_PROTOCOL * This, EFI_TCP4_CLOSE_TOKEN *CloseToken);
    EFI_STATUS(EFIAPI * Cancel)(EFI_TCP4_PROTOCOL * This, EFI_TCP4_COMPLETION_TOKEN *Token);
    EFI_STATUS(EFIAPI * Poll)(EFI_TCP4_PROTOCOL * This);
};

/* ---- EFI WiFi2: EFI_WIRELESS_MAC_CONNECTION_II_PROTOCOL (802.11 station MLME) ----
 * Scan / associate / disconnect. Async, token/event based like the transports above.
 * The WPA credentials live in the companion EFI_SUPPLICANT_PROTOCOL (below); this
 * protocol drives the RSNA 4-way handshake through that supplicant internally. */
#define EFI_MAX_SSID_LEN 32

typedef enum {
    IeeeInfrastructureBSS,
    IeeeIndependentBSS,
    IeeeMeshBSS,
    IeeeAnyBss
} EFI_80211_BSS_TYPE;

typedef enum {
    ConnectSuccess,
    ConnectRefused,                  /* auth/assoc rejected — typically a bad key */
    ConnectFailed,
    ConnectFailureTimeout,
    ConnectFailedReasonUnspecified
} EFI_80211_CONNECT_NETWORK_RESULT_CODE;

typedef struct { UINT8 Addr[6];
} EFI_80211_MAC_ADDRESS;

typedef struct {
    UINT8 SSIdLen;
    UINT8 SSId[EFI_MAX_SSID_LEN];
} EFI_80211_SSID;

/* One AKM or cipher selector: IEEE 802.11 OUI (00-0F-AC for RSN) + a type byte. */
typedef struct {
    UINT8 Oui[3];
    UINT8 SuiteType;
} EFI_80211_SUITE_SELECTOR;

typedef struct {
    UINT16 AKMSuiteCount;
    EFI_80211_SUITE_SELECTOR AKMSuiteList[1];
} EFI_80211_AKM_SUITE_SELECTOR;

typedef struct {
    UINT16 CipherSuiteCount;
    EFI_80211_SUITE_SELECTOR CipherSuiteList[1];
} EFI_80211_CIPHER_SUITE_SELECTOR;

typedef struct {
    EFI_80211_BSS_TYPE BSSType;
    EFI_80211_SSID SSId;
    EFI_80211_AKM_SUITE_SELECTOR *AKMSuite;      /* NULL for an open network */
    EFI_80211_CIPHER_SUITE_SELECTOR *CipherSuite;
} EFI_80211_NETWORK;

typedef struct {
    EFI_80211_NETWORK Network;
    UINT8 NetworkQuality;            /* 0..100 (no dBm RSSI is exposed by WiFi2) */
} EFI_80211_NETWORK_DESCRIPTION;

typedef struct {
    UINT32 NumOfSSID;                /* 0 = broadcast probe (scan all) */
    EFI_80211_SSID SSIDList[1];
} EFI_80211_GET_NETWORKS_DATA;

typedef struct {
    UINT8 NumOfNetworkDesc;
    EFI_80211_NETWORK_DESCRIPTION NetworkDesc[1];
} EFI_80211_GET_NETWORKS_RESULT;

typedef struct {
    EFI_EVENT Event;
    EFI_STATUS Status;
    EFI_80211_GET_NETWORKS_DATA *Data;
    EFI_80211_GET_NETWORKS_RESULT *Result;       /* driver-allocated; caller frees */
} EFI_80211_GET_NETWORKS_TOKEN;

typedef struct {
    EFI_80211_NETWORK *Network;
    UINT32 FailureTimeout;           /* seconds before ConnectFailureTimeout */
} EFI_80211_CONNECT_NETWORK_DATA;

typedef struct {
    EFI_EVENT Event;
    EFI_STATUS Status;
    EFI_80211_CONNECT_NETWORK_DATA *Data;
    EFI_80211_CONNECT_NETWORK_RESULT_CODE ResultCode;
} EFI_80211_CONNECT_NETWORK_TOKEN;

typedef struct {
    EFI_EVENT Event;
    EFI_STATUS Status;
} EFI_80211_DISCONNECT_NETWORK_TOKEN;

typedef struct _EFI_WIRELESS_MAC_CONNECTION_II_PROTOCOL EFI_WIRELESS_MAC_CONNECTION_II_PROTOCOL;
struct _EFI_WIRELESS_MAC_CONNECTION_II_PROTOCOL {
    EFI_STATUS(EFIAPI * GetNetworks)(EFI_WIRELESS_MAC_CONNECTION_II_PROTOCOL * This,
        EFI_80211_GET_NETWORKS_TOKEN *Token);
    EFI_STATUS(EFIAPI * ConnectNetwork)(EFI_WIRELESS_MAC_CONNECTION_II_PROTOCOL * This,
        EFI_80211_CONNECT_NETWORK_TOKEN *Token);
    EFI_STATUS(EFIAPI * DisconnectNetwork)(EFI_WIRELESS_MAC_CONNECTION_II_PROTOCOL * This,
        EFI_80211_DISCONNECT_NETWORK_TOKEN *Token);
};

/* RSN (802.11i) suite constants — OUI 00-0F-AC. AKM 2 = PSK; cipher 4 = CCMP (AES). */
#define EFI_80211_SUITE_OUI_0 0x00
#define EFI_80211_SUITE_OUI_1 0x0F
#define EFI_80211_SUITE_OUI_2 0xAC
#define EFI_80211_AKM_SUITE_PSK    2
#define EFI_80211_CIPHER_SUITE_CCMP 4

/* ---- EFI_SUPPLICANT_PROTOCOL (holds WPA credentials; service-binding) ---- */
typedef enum {
    EfiSupplicant80211AKMSuite,
    EfiSupplicant80211GroupDataCipherSuite,
    EfiSupplicant80211PairwiseCipherSuite,
    EfiSupplicant80211PskPassword,
    EfiSupplicant80211TargetSSIDName,
    EfiSupplicant80211StationMac,
    EfiSupplicant80211TargetSSIDMac,
    EfiSupplicant80211PTK,
    EfiSupplicant80211GTK,
    EfiSupplicantState,
    EfiSupplicant80211LinkState,
    EfiSupplicantKeyRefresh,
    EfiSupplicant80211SupportedAKMSuites,
    EfiSupplicant80211SupportedSoftwareCipherSuites,
    EfiSupplicant80211SupportedHardwareCipherSuites,
    EfiSupplicant80211IGTK,
    EfiSupplicant80211PMK,
    EfiSupplicantDataTypeMaximum
} EFI_SUPPLICANT_DATA_TYPE;

/* EFI_80211_LINK_STATE (queried via GetData(EfiSupplicant80211LinkState)). */
typedef enum {
    Ieee80211UnauthenticatedUnassociated,
    Ieee80211AuthenticatedUnassociated,
    Ieee80211PendingRSNAuthentication,
    Ieee80211AuthenticatedAssociated
} EFI_80211_LINK_STATE;

typedef struct _EFI_SUPPLICANT_PROTOCOL EFI_SUPPLICANT_PROTOCOL;
struct _EFI_SUPPLICANT_PROTOCOL {
    void *BuildResponsePacket;
    void *ProcessPacket;
    EFI_STATUS(EFIAPI * SetData)(EFI_SUPPLICANT_PROTOCOL * This, EFI_SUPPLICANT_DATA_TYPE DataType,
        void *Data, UINTN DataSize);
    EFI_STATUS(EFIAPI * GetData)(EFI_SUPPLICANT_PROTOCOL * This, EFI_SUPPLICANT_DATA_TYPE DataType,
        UINT8 *Data, UINTN *DataSize);
};

/* ---- Well-known GUIDs (mirror of uefi.guid, for the C side) ---- */
#define EFI_SIMPLE_NETWORK_PROTOCOL_GUID \
    { 0xa19832b9, 0xac25, 0x11d3, { 0x9a, 0x2d, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d } }
#define EFI_MANAGED_NETWORK_SERVICE_BINDING_PROTOCOL_GUID \
    { 0xf36ff770, 0xa7e1, 0x42cf, { 0x9e, 0xd2, 0x56, 0xf0, 0xf2, 0x71, 0xf4, 0x4c } }
#define EFI_IP4_CONFIG2_PROTOCOL_GUID \
    { 0x5b446ed1, 0xe30b, 0x4faa, { 0x87, 0x1a, 0x36, 0x54, 0xec, 0xa3, 0x60, 0x80 } }
#define EFI_DNS4_PROTOCOL_GUID \
    { 0xae3d28cc, 0xe05b, 0x4fa1, { 0xa0, 0x11, 0x7e, 0xb5, 0x5a, 0x3f, 0x14, 0x01 } }
#define EFI_DNS4_SERVICE_BINDING_PROTOCOL_GUID \
    { 0xb625b186, 0xe063, 0x44f7, { 0x89, 0x05, 0x6a, 0x74, 0xdc, 0x6f, 0x52, 0xb4 } }
#define EFI_UDP4_PROTOCOL_GUID \
    { 0x3ad9df29, 0x4501, 0x478d, { 0xb1, 0xf8, 0x7f, 0x7f, 0xe7, 0x0e, 0x50, 0xf3 } }
#define EFI_UDP4_SERVICE_BINDING_PROTOCOL_GUID \
    { 0x83f01464, 0x99bd, 0x45e5, { 0xb3, 0x83, 0xaf, 0x63, 0x05, 0xd8, 0xe9, 0xe6 } }
#define EFI_TCP4_PROTOCOL_GUID \
    { 0x65530bc7, 0xa359, 0x410f, { 0xb0, 0x10, 0x5a, 0xad, 0xc7, 0xec, 0x2b, 0x62 } }
#define EFI_TCP4_SERVICE_BINDING_PROTOCOL_GUID \
    { 0x00720665, 0x67eb, 0x4a99, { 0xba, 0xf7, 0xd3, 0xc3, 0x3a, 0x1c, 0x7c, 0xc9 } }
#define EFI_IP4_PROTOCOL_GUID \
    { 0x41d94cd2, 0x35b6, 0x455a, { 0x82, 0x58, 0xd4, 0xe5, 0x13, 0x34, 0xaa, 0xdd } }
#define EFI_IP4_SERVICE_BINDING_PROTOCOL_GUID \
    { 0xc51711e7, 0xb4bf, 0x404a, { 0xbf, 0xb8, 0x0a, 0x04, 0x8e, 0xf1, 0xff, 0xe4 } }
#define EFI_WIRELESS_MAC_CONNECTION_II_PROTOCOL_GUID \
    { 0x1b0fb9bf, 0x699d, 0x4fdd, { 0xa7, 0xc3, 0x25, 0x46, 0x68, 0x1b, 0xf6, 0x3b } }
#define EFI_SUPPLICANT_PROTOCOL_GUID \
    { 0x54fcc43e, 0xaa89, 0x4333, { 0x9a, 0x85, 0xcd, 0xea, 0x24, 0x05, 0x1e, 0x9e } }
#define EFI_SUPPLICANT_SERVICE_BINDING_PROTOCOL_GUID \
    { 0x45bcd98e, 0x59ad, 0x4174, { 0x95, 0x46, 0x34, 0x4a, 0x07, 0x48, 0x58, 0x98 } }

/* ── Shared C helpers (uefi_net.c): async completion tokens + service binding ── */
/* A posted UEFI async op: an EVT_NOTIFY_SIGNAL event whose notify sets `done` and
 * wakes the idle loop. Wait rides the port's WFE so timers/asyncio/Ctrl-C stay live. */
typedef struct {
    volatile int done;
    EFI_EVENT event;
} net_op_t;

EFI_STATUS net_op_init(net_op_t *op);
/* Block until the op completes or `timeout_ms` elapses (negative = forever). Returns
 * 0 on completion, -1 on timeout. May raise KeyboardInterrupt (Ctrl-C) via the VM. */
int net_op_wait(net_op_t *op, int timeout_ms);
void net_op_close(net_op_t *op);

/* Service-binding child lifecycle: CreateChild/DestroyChild on `nic` for `sb_guid`. */
EFI_STATUS net_sb_create_child(EFI_HANDLE nic, const EFI_GUID *sb_guid, EFI_HANDLE *child_out);
EFI_STATUS net_sb_destroy_child(EFI_HANDLE nic, const EFI_GUID *sb_guid, EFI_HANDLE child);

/* Global DNS-server override (network.ipconfig(dns=...)); getaddrinfo prefers it. */
void net_set_dns_override(const UINT8 ip[4]);
int net_get_dns_override(UINT8 out[4]);   /* 1 if set (fills out), 0 otherwise */

#endif /* MPY_UEFI_EFI_NET_H */

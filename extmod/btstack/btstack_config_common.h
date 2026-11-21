#ifndef MICROPY_INCLUDED_EXTMOD_BTSTACK_BTSTACK_CONFIG_COMMON_H
#define MICROPY_INCLUDED_EXTMOD_BTSTACK_BTSTACK_CONFIG_COMMON_H

// BTstack features that can be enabled
#define ENABLE_BLE
#define ENABLE_LE_PERIPHERAL
#define ENABLE_LE_CENTRAL
// #define ENABLE_CLASSIC
#define ENABLE_L2CAP_LE_CREDIT_BASED_FLOW_CONTROL_MODE
#define ENABLE_PRINTF_HEXDUMP

/* BTstack HCI Logging
 * - see also "lib/btstack/src/hci_dump.h"
 */
#define ENABLE_LOG_INFO
#define ENABLE_LOG_DEBUG
#define ENABLE_LOG_ERROR
// Dump internal BTstack events
// #define ENABLE_LOG_BTSTACK_EVENTS


// The GATT Client starts pairing on insufficient-security errors
// and retries the request.
#define ENABLE_GATT_CLIENT_PAIRING

// Enable LE Secure connections
//
// If not defined the 'LE Secure connections' flag will not be set.
// and instead it will be reset by
// 'void sm_set_authentication_requirements(uint8_t auth_req)'.
/*
 * 261120
 * sm_ec_generate_new_key() will stick in state "EC_KEY_GENERATION_ACTIVE"
 */
// #define ENABLE_LE_SECURE_CONNECTIONS

// needed for ENABLE_LE_SECURE_CONNECTIONS
// #define HAVE_MBEDTLS_ECC_P256

// BTstack configuration. buffers, sizes, ...
#define HCI_ACL_PAYLOAD_SIZE 1021
#define MAX_NR_GATT_CLIENTS 1
#define MAX_NR_HCI_CONNECTIONS 1
#define MAX_NR_L2CAP_SERVICES  3
#define MAX_NR_L2CAP_CHANNELS  3
#define MAX_NR_RFCOMM_MULTIPLEXERS 1
#define MAX_NR_RFCOMM_SERVICES 1
#define MAX_NR_RFCOMM_CHANNELS 1
#define MAX_NR_BTSTACK_LINK_KEY_DB_MEMORY_ENTRIES  2
#define MAX_NR_BNEP_SERVICES 1
#define MAX_NR_BNEP_CHANNELS 1
#define MAX_NR_HFP_CONNECTIONS 1
#define MAX_NR_WHITELIST_ENTRIES 1
#define MAX_NR_SM_LOOKUP_ENTRIES 3
#define MAX_NR_SERVICE_RECORD_ITEMS 1
#define MAX_NR_AVDTP_STREAM_ENDPOINTS 1
#define MAX_NR_AVDTP_CONNECTIONS 1
#define MAX_NR_AVRCP_CONNECTIONS 1

#define MAX_NR_LE_DEVICE_DB_ENTRIES 4

// Link Key DB and LE Device DB using TLV on top of Flash Sector interface
// #define NVM_NUM_DEVICE_DB_ENTRIES 16

// We don't give btstack a malloc, so use a fixed-size ATT DB.
#define MAX_ATT_DB_SIZE 512

// BTstack HAL configuration
#define HAVE_EMBEDDED_TIME_MS

// Some USB dongles take longer to respond to HCI reset (e.g. BCM20702A).
#define HCI_RESET_RESEND_TIMEOUT_MS 1000

#endif // MICROPY_INCLUDED_EXTMOD_BTSTACK_BTSTACK_CONFIG_COMMON_H

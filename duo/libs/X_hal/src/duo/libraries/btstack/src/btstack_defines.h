/*
 * Copyright (C) 2015 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at 
 * contact@bluekitchen-gmbh.com
 *
 */

/*
 * btstack-defines.h
 *
 * BTstack definitions, events, and error codes */

#ifndef __BTSTACK_DEFINES_H
#define __BTSTACK_DEFINES_H

#include <stdint.h>
#include "btstack_linked_list.h" 

// TYPES

// packet handler
typedef void (*btstack_packet_handler_t) (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);

// packet callback supporting multiple registrations
typedef struct {
    btstack_linked_item_t    item;
    btstack_packet_handler_t callback;
} btstack_packet_callback_registration_t;

/**
 * @brief 128 bit key used with AES128 in Security Manager
 */
typedef uint8_t sm_key_t[16];

// DEFINES

#define DAEMON_EVENT_PACKET     0x05
    
// L2CAP data
#define L2CAP_DATA_PACKET       0x06

// RFCOMM data
#define RFCOMM_DATA_PACKET      0x07

// Attribute protocol data
#define ATT_DATA_PACKET         0x08

// Security Manager protocol data
#define SM_DATA_PACKET          0x09

// SDP query result
// format: type (8), record_id (16), attribute_id (16), attribute_length (16), attribute_value (max 1k)
#define SDP_CLIENT_PACKET       0x0a

// BNEP data
#define BNEP_DATA_PACKET        0x0b

// Unicast Connectionless Data
#define UCD_DATA_PACKET         0x0c
 
// debug log messages
#define LOG_MESSAGE_PACKET      0xfc


// ERRORS

// last error code in 2.1 is 0x38 - we start with 0x50 for BTstack errors
#define BTSTACK_CONNECTION_TO_BTDAEMON_FAILED              0x50
#define BTSTACK_ACTIVATION_FAILED_SYSTEM_BLUETOOTH         0x51
#define BTSTACK_ACTIVATION_POWERON_FAILED                  0x52
#define BTSTACK_ACTIVATION_FAILED_UNKNOWN                  0x53
#define BTSTACK_NOT_ACTIVATED                              0x54
#define BTSTACK_BUSY                                       0x55
#define BTSTACK_MEMORY_ALLOC_FAILED                        0x56
#define BTSTACK_ACL_BUFFERS_FULL                           0x57

// l2cap errors - enumeration by the command that created them
#define L2CAP_COMMAND_REJECT_REASON_COMMAND_NOT_UNDERSTOOD 0x60
#define L2CAP_COMMAND_REJECT_REASON_SIGNALING_MTU_EXCEEDED 0x61
#define L2CAP_COMMAND_REJECT_REASON_INVALID_CID_IN_REQUEST 0x62

#define L2CAP_CONNECTION_RESPONSE_RESULT_SUCCESSFUL        0x63
#define L2CAP_CONNECTION_RESPONSE_RESULT_PENDING           0x64
#define L2CAP_CONNECTION_RESPONSE_RESULT_REFUSED_PSM       0x65
#define L2CAP_CONNECTION_RESPONSE_RESULT_REFUSED_SECURITY  0x66
#define L2CAP_CONNECTION_RESPONSE_RESULT_REFUSED_RESOURCES 0x67
#define L2CAP_CONNECTION_RESPONSE_RESULT_RTX_TIMEOUT       0x68

#define L2CAP_SERVICE_ALREADY_REGISTERED                   0x69
#define L2CAP_DATA_LEN_EXCEEDS_REMOTE_MTU                  0x6A
    
#define RFCOMM_MULTIPLEXER_STOPPED                         0x70
#define RFCOMM_CHANNEL_ALREADY_REGISTERED                  0x71
#define RFCOMM_NO_OUTGOING_CREDITS                         0x72
#define RFCOMM_AGGREGATE_FLOW_OFF                          0x73
#define RFCOMM_DATA_LEN_EXCEEDS_MTU                        0x74

#define SDP_HANDLE_ALREADY_REGISTERED                      0x80
#define SDP_QUERY_INCOMPLETE                               0x81
#define SDP_SERVICE_NOT_FOUND                              0x82
#define SDP_HANDLE_INVALID                                 0x83
#define SDP_QUERY_BUSY                                     0x84

#define ATT_HANDLE_VALUE_INDICATION_IN_PORGRESS            0x90 
#define ATT_HANDLE_VALUE_INDICATION_TIMEOUT                0x91

#define GATT_CLIENT_NOT_CONNECTED                          0x93
#define GATT_CLIENT_BUSY                                   0x94
#define GATT_CLIENT_IN_WRONG_STATE                         0x95
#define GATT_CLIENT_DIFFERENT_CONTEXT_FOR_ADDRESS_ALREADY_EXISTS 0x96
#define GATT_CLIENT_VALUE_TOO_LONG                         0x97
#define GATT_CLIENT_CHARACTERISTIC_NOTIFICATION_NOT_SUPPORTED 0x98
#define GATT_CLIENT_CHARACTERISTIC_INDICATION_NOT_SUPPORTED   0x99 

#define BNEP_SERVICE_ALREADY_REGISTERED                    0xA0
#define BNEP_CHANNEL_NOT_CONNECTED                         0xA1
#define BNEP_DATA_LEN_EXCEEDS_MTU                          0xA2

// COMMANDS

#define OGF_BTSTACK 0x3d

// cmds for BTstack 
// get state: @returns HCI_STATE
#define BTSTACK_GET_STATE                                  0x01

// set power mode: @param HCI_POWER_MODE
#define BTSTACK_SET_POWER_MODE                             0x02

// set capture mode: @param on
#define BTSTACK_SET_ACL_CAPTURE_MODE                       0x03

// get BTstack version
#define BTSTACK_GET_VERSION                                0x04

// get system Bluetooth state
#define BTSTACK_GET_SYSTEM_BLUETOOTH_ENABLED               0x05

// set system Bluetooth state
#define BTSTACK_SET_SYSTEM_BLUETOOTH_ENABLED               0x06

// enable inquiry scan for this client
#define BTSTACK_SET_DISCOVERABLE                           0x07

// set global Bluetooth state
#define BTSTACK_SET_BLUETOOTH_ENABLED                      0x08

// create l2cap channel: @param bd_addr(48), psm (16)
#define L2CAP_CREATE_CHANNEL                               0x20

// disconnect l2cap disconnect, @param channel(16), reason(8)
#define L2CAP_DISCONNECT                                   0x21

// register l2cap service: @param psm(16), mtu (16)
#define L2CAP_REGISTER_SERVICE                             0x22

// unregister l2cap disconnect, @param psm(16)
#define L2CAP_UNREGISTER_SERVICE                           0x23

// accept connection @param bd_addr(48), dest cid (16)
#define L2CAP_ACCEPT_CONNECTION                            0x24

// decline l2cap disconnect,@param bd_addr(48), dest cid (16), reason(8)
#define L2CAP_DECLINE_CONNECTION                           0x25

// create l2cap channel: @param bd_addr(48), psm (16), mtu (16)
#define L2CAP_CREATE_CHANNEL_MTU                           0x26

// register SDP Service Record: service record (size)
#define SDP_REGISTER_SERVICE_RECORD                        0x30

// unregister SDP Service Record
#define SDP_UNREGISTER_SERVICE_RECORD                      0x31

// Get remote RFCOMM services
#define SDP_CLIENT_QUERY_RFCOMM_SERVICES                   0x32

// Get remote SDP services
#define SDP_CLIENT_QUERY_SERVICES                          0x33

// RFCOMM "HCI" Commands
#define RFCOMM_CREATE_CHANNEL       0x40
#define RFCOMM_DISCONNECT     0x41
#define RFCOMM_REGISTER_SERVICE     0x42
#define RFCOMM_UNREGISTER_SERVICE   0x43
#define RFCOMM_ACCEPT_CONNECTION    0x44
#define RFCOMM_DECLINE_CONNECTION   0x45
#define RFCOMM_PERSISTENT_CHANNEL   0x46
#define RFCOMM_CREATE_CHANNEL_WITH_CREDITS   0x47
#define RFCOMM_REGISTER_SERVICE_WITH_CREDITS 0x48
#define RFCOMM_GRANT_CREDITS                 0x49
    
// GAP Classic 0x50
#define GAP_DISCONNECT              0x50

// GAP LE      0x60  
#define GAP_LE_SCAN_START           0x60
#define GAP_LE_SCAN_STOP            0x61
#define GAP_LE_CONNECT              0x62
#define GAP_LE_CONNECT_CANCEL       0x63
#define GAP_LE_SET_SCAN_PARAMETERS  0x64

// GATT (Client) 0x70
#define GATT_DISCOVER_ALL_PRIMARY_SERVICES                       0x70
#define GATT_DISCOVER_PRIMARY_SERVICES_BY_UUID16                 0x71
#define GATT_DISCOVER_PRIMARY_SERVICES_BY_UUID128                0x72
#define GATT_FIND_INCLUDED_SERVICES_FOR_SERVICE                  0x73
#define GATT_DISCOVER_CHARACTERISTICS_FOR_SERVICE                0x74
#define GATT_DISCOVER_CHARACTERISTICS_FOR_SERVICE_BY_UUID128     0x75
#define GATT_DISCOVER_CHARACTERISTIC_DESCRIPTORS                 0x76
#define GATT_READ_VALUE_OF_CHARACTERISTIC                        0x77
#define GATT_READ_LONG_VALUE_OF_CHARACTERISTIC                   0x78
#define GATT_WRITE_VALUE_OF_CHARACTERISTIC_WITHOUT_RESPONSE      0x79
#define GATT_WRITE_VALUE_OF_CHARACTERISTIC                       0x7A
#define GATT_WRITE_LONG_VALUE_OF_CHARACTERISTIC                  0x7B
#define GATT_RELIABLE_WRITE_LONG_VALUE_OF_CHARACTERISTIC         0x7C
#define GATT_READ_CHARACTERISTIC_DESCRIPTOR                      0X7D
#define GATT_READ_LONG_CHARACTERISTIC_DESCRIPTOR                 0X7E
#define GATT_WRITE_CHARACTERISTIC_DESCRIPTOR                     0X7F
#define GATT_WRITE_LONG_CHARACTERISTIC_DESCRIPTOR                0X80
#define GATT_WRITE_CLIENT_CHARACTERISTIC_CONFIGURATION           0X81
#define GATT_GET_MTU                                             0x82


// EVENTS

/**
 * @format 1
 * @param state
 */
#define BTSTACK_EVENT_STATE                                0x60

// data: event(8), len(8), nr hci connections
#define BTSTACK_EVENT_NR_CONNECTIONS_CHANGED               0x61

/**
 * @format 
 */
#define BTSTACK_EVENT_POWERON_FAILED                       0x62

/**
 * @format 1
 * @param discoverable
 */
#define BTSTACK_EVENT_DISCOVERABLE_ENABLED                 0x66

// Daemon Events

/**
 * @format 112
 * @param major
 * @param minor
 @ @param revision
 */
#define DAEMON_EVENT_VERSION                               0x63

// data: system bluetooth on/off (bool)
/**
 * @format 1
 * param system_bluetooth_enabled
 */
#define DAEMON_EVENT_SYSTEM_BLUETOOTH_ENABLED              0x64

// data: event (8), len(8), status (8) == 0, address (48), name (1984 bits = 248 bytes)

/* 
 * @format 1BT
 * @param status == 0 to match read_remote_name_request
 * @param address
 * @param name
 */
#define DAEMON_EVENT_REMOTE_NAME_CACHED                    0x65

// internal daemon events

// data: event(8)
#define DAEMON_EVENT_CONNECTION_OPENED                     0x67

// data: event(8)
#define DAEMON_EVENT_CONNECTION_CLOSED                     0x68


// additional HCI events

/**
 * @brief Outgoing packet 
 */
#define HCI_EVENT_TRANSPORT_PACKET_SENT                    0x6E

/**
 * @format B
 * @param handle
 */
#define HCI_EVENT_SCO_CAN_SEND_NOW                         0x6F

// L2CAP EVENTS
    
/**
 * @format 1BH222222
 * @param status
 * @param address
 * @param handle
 * @param psm
 * @param local_cid
 * @param remote_cid
 * @param local_mtu
 * @param remote_mtu
 * @param flush_timeout
 */
#define L2CAP_EVENT_CHANNEL_OPENED                         0x70

/*
 * @format 2
 * @param local_cid
 */
#define L2CAP_EVENT_CHANNEL_CLOSED                         0x71

/**
 * @format 1BH222
 * @param status
 * @param address
 * @param handle
 * @param psm
 * @param local_cid
 * @param remote_cid
 */
#define L2CAP_EVENT_INCOMING_CONNECTION                    0x72

// ??
// data: event(8), len(8), handle(16)
#define L2CAP_EVENT_TIMEOUT_CHECK                          0x73

// ??
// data: event(8), len(8), local_cid(16), credits(8)
#define L2CAP_EVENT_CREDITS                                0x74

/**
 * @format 12
 * @param status
 * @param psm
 */
#define L2CAP_EVENT_SERVICE_REGISTERED                     0x75

/**
 * @format H2222
 * @param handle
 * @param interval_min
 * @param interval_max
 * @param latencey
 * @param timeout_multiplier
 */
#define L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST    0x76

// data: event(8), len(8), handle(16), result (16) (0 == ok, 1 == fail)
 /** 
  * @format H2
  * @param handle
  * @result
  */
#define L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_RESPONSE   0x77

/**
 * @format 2
 * @param local_cid
 */
#define L2CAP_EVENT_CAN_SEND_NOW                           0x78

// RFCOMM EVENTS

/**
 * @format 1B2122
 * @param status
 * @param bd_addr
 * @param con_handle
 * @param server_channel
 * @param rfcomm_cid
 * @param max_frame_size
 */
#define RFCOMM_EVENT_OPEN_CHANNEL_COMPLETE                 0x80

/**
 * @format 2
 * @param rfcomm_cid
 */
#define RFCOMM_EVENT_CHANNEL_CLOSED                        0x81

/**
 * @format B12
 * @param bd_addr
 * @param server_channel
 * @param rfcomm_cid
 */
#define RFCOMM_EVENT_INCOMING_CONNECTION                   0x82

/**
 * @format 21
 * @param rfcomm_cid
 * @param line_status
 */
#define RFCOMM_EVENT_REMOTE_LINE_STATUS                    0x83
    
/**
 * @format 21
 * @param rfcomm_cid
 * @param credits
 */
#define RFCOMM_EVENT_CREDITS                               0x84

/**
 * @format 11
 * @param status
 * @param channel_id
 */
#define RFCOMM_EVENT_SERVICE_REGISTERED                    0x85
    
/**
 * @format 11
 * @param status
 * @param server_channel_id
 */
#define RFCOMM_EVENT_PERSISTENT_CHANNEL                    0x86
    
/**
 * @format 21
 * @param rfcomm_cid
 * @param modem_status
 */
#define RFCOMM_EVENT_REMOTE_MODEM_STATUS                   0x87

 /**
  * TODO: format for variable data 2?
  * @param rfcomm_cid
  * @param rpn_data
  */
#define RFCOMM_EVENT_PORT_CONFIGURATION                    0x88

/**
 * @format 2
 * @param local_cid
 */
#define RFCOMM_EVENT_CAN_SEND_NOW                          0x89


/**
  * @format 14
  * @param status
  * @param service_record_handle
  */
#define SDP_EVENT_SERVICE_REGISTERED                             0x90

/**
 * @format 1
 * @param status
 */
#define SDP_EVENT_QUERY_COMPLETE                                 0x91 

/**
 * @format 1T
 * @param rfcomm_channel
 * @param name
 */
#define SDP_EVENT_QUERY_RFCOMM_SERVICE                           0x92

/**
 * @format 22221
 * @param record_id
 * @param attribute_id
 * @param attribute_length
 * @param data_offset
 * @param data
 */
#define SDP_EVENT_QUERY_ATTRIBUTE_BYTE                           0x93

/**
 * @format 22LV
 * @param record_id
 * @param attribute_id
 * @param attribute_length
 * @param attribute_value
 */
#define SDP_EVENT_QUERY_ATTRIBUTE_VALUE                          0x94

/**
 * @format 224
 * @param total_count
 * @param record_index
 * @param record_handle
 * @note Not provided by daemon, only used for internal testing
 */
#define SDP_EVENT_QUERY_SERVICE_RECORD_HANDLE                    0x95

/**
 * @format H1
 * @param handle
 * @param status
 */
#define GATT_EVENT_QUERY_COMPLETE                                0xA0

/**
 * @format HX
 * @param handle
 * @param service
 */
#define GATT_EVENT_SERVICE_QUERY_RESULT                          0xA1

/**
 * @format HY
 * @param handle
 * @param characteristic
 */
#define GATT_EVENT_CHARACTERISTIC_QUERY_RESULT                   0xA2

/**
 * @format H2X
 * @param handle
 * @param include_handle
 * @param service
 */
#define GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT                 0xA3

/**
 * @format HZ
 * @param handle
 * @param characteristic_descriptor
 */
#define GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT   0xA4

/**
 * @format H2LV
 * @param handle
 * @param value_handle
 * @param value_length
 * @param value
 */
#define GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT             0xA5

/**
 * @format H22LV
 * @param handle
 * @param value_handle
 * @param value_offset
 * @param value_length
 * @param value
 */
#define GATT_EVENT_LONG_CHARACTERISTIC_VALUE_QUERY_RESULT        0xA6

/**
 * @format H2LV
 * @param handle
 * @param value_handle
 * @param value_length
 * @param value
 */
#define GATT_EVENT_NOTIFICATION                                  0xA7

/**
 * @format H2LV
 * @param handle
 * @param value_handle
 * @param value_length
 * @param value
 */
#define GATT_EVENT_INDICATION                                    0xA8

/**
 * @format H2LV
 * @param descriptor_handle
 * @param descriptor_length
 * @param descriptor
 */
#define GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT        0xA9

/**
 * @format H2LV
 * @param handle
 * @param descriptor_offset
 * @param descriptor_length
 * @param descriptor
 */
#define GATT_EVENT_LONG_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT   0xAA

/** 
 * @format H2
 * @param handle
 * @param MTU
 */    
#define GATT_EVENT_MTU                                           0xAB

/** 
 * @format H2
 * @param handle
 * @param MTU
 */    
#define ATT_EVENT_MTU_EXCHANGE_COMPLETE                          0xB5

// data: event(8), len(8), status (8), hci_handle (16), attribute_handle (16)
#define ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE               0xB6


// data: event(8), len(8), status (8), bnep service uuid (16) 
#define BNEP_EVENT_SERVICE_REGISTERED                      0xC0

// data: event(8), len(8), status (8), bnep source uuid (16), bnep destination uuid (16), mtu (16), remote_address (48) 
#define BNEP_EVENT_OPEN_CHANNEL_COMPLETE                   0xC1

// data: event(8), len(8), bnep source uuid (16), bnep destination uuid (16), remote_address (48) 
#define BNEP_EVENT_CHANNEL_CLOSED                          0xC2

// data: event(8), len(8), bnep source uuid (16), bnep destination uuid (16), remote_address (48), channel state (8)
#define BNEP_EVENT_CHANNEL_TIMEOUT                         0xC3    
    
// data: event(8), len(8)
#define BNEP_EVENT_READY_TO_SEND                           0xC4

 /**
  * @format H1B
  * @param handle
  * @param addr_type
  * @param address
  */
#define SM_EVENT_JUST_WORKS_REQUEST                              0xD0

 /**
  * @format H1B
  * @param handle
  * @param addr_type
  * @param address
  */
#define SM_EVENT_JUST_WORKS_CANCEL                               0xD1 

 /**
  * @format H1B4
  * @param handle
  * @param addr_type
  * @param address
  * @param passkey
  */
#define SM_EVENT_PASSKEY_DISPLAY_NUMBER                          0xD2

 /**
  * @format H1B
  * @param handle
  * @param addr_type
  * @param address
  */
#define SM_EVENT_PASSKEY_DISPLAY_CANCEL                          0xD3

 /**
  * @format H1B421
  * @param handle
  * @param addr_type
  * @param address
  */
#define SM_EVENT_PASSKEY_INPUT_NUMBER                            0xD4

 /**
  * @format H1B
  * @param handle
  * @param addr_type
  * @param address
  */
#define SM_EVENT_PASSKEY_INPUT_CANCEL                            0xD5

 /**
  * @format H1B
  * @param handle
  * @param addr_type
  * @param address
  */
#define SM_EVENT_IDENTITY_RESOLVING_STARTED                      0xD6

 /**
  * @format H1B
  * @param handle
  * @param addr_type
  * @param address
  */
#define SM_EVENT_IDENTITY_RESOLVING_FAILED                       0xD7

 /**
  * @format H1B2
  * @param handle
  * @param addr_type
  * @param address
  * @param le_device_db_index
  */
#define SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED                    0xD8

 /**
  * @format H1B
  * @param handle
  * @param addr_type
  * @param address
  */
#define SM_EVENT_AUTHORIZATION_REQUEST                           0xD9

 /**
  * @format H1B1
  * @param handle
  * @param addr_type
  * @param address
  * @param authorization_result
  */
#define SM_EVENT_AUTHORIZATION_RESULT                            0xDA

// GAP

// data: event(8), len(8), hci_handle (16), security_level (8)
#define GAP_EVENT_SECURITY_LEVEL                                 0xE0

// data: event(8), len(8), status (8), bd_addr(48)
#define GAP_EVENT_DEDICATED_BONDING_COMPLETED                    0xE1

/**
 * @format 11B1JV
 * @param advertising_event_type
 * @param address_type
 * @param address
 * @param rssi
 * @param data_length
 * @param data
 */
#define GAP_LE_EVENT_ADVERTISING_REPORT                          0xE2


// Meta Events, see below for sub events
#define HCI_EVENT_HSP_META                                 0xE8
#define HCI_EVENT_HFP_META                                 0xE9
#define HCI_EVENT_ANCS_META                                0xEA

// Potential other meta groups
 // #define HCI_EVENT_BNEP_META                                0xxx
// #define HCI_EVENT_GAP_META                                 0xxx
// #define HCI_EVENT_GATT_META                                0xxx
// #define HCI_EVENT_PAN_META                                 0xxx
// #define HCI_EVENT_SDP_META                                 0xxx
// #define HCI_EVENT_SM_META                                  0xxx

/**
 * @format 11H
 * @param subevent_code
 * @param status 0 == OK
 * @param handle
 */
#define HSP_SUBEVENT_AUDIO_CONNECTION_COMPLETE             0x01

/**
 * @format 11
 * @param subevent_code
 * @param status 0 == OK
 */
#define HSP_SUBEVENT_AUDIO_DISCONNECTION_COMPLETE          0x02

/**
 * @format 1
 * @param subevent_code
 */
#define HSP_SUBEVENT_RING                                  0x03

/**
 * @format 11
 * @param subevent_code
 * @param gain Valid range: [0,15]
 */
#define HSP_SUBEVENT_MICROPHONE_GAIN_CHANGED               0x04

/**
 * @format 11
 * @param subevent_code
 * @param gain Valid range: [0,15]
 */
#define HSP_SUBEVENT_SPEAKER_GAIN_CHANGED                  0x05

/**
 * @format 1JV
 * @param subevent_code
 * @param value_length
 * @param value
 */
#define HSP_SUBEVENT_HS_COMMAND                            0x06

/**
 * @format 1JV
 * @param subevent_code
 * @param value_length
 * @param value
 */
#define HSP_SUBEVENT_AG_INDICATION                         0x07


// HFP Subevents

#define HFP_SUBEVENT_SERVICE_LEVEL_CONNECTION_ESTABLISHED  0x01
#define HFP_SUBEVENT_SERVICE_LEVEL_CONNECTION_RELEASED     0x02
#define HFP_SUBEVENT_AUDIO_CONNECTION_ESTABLISHED          0x03
#define HFP_SUBEVENT_AUDIO_CONNECTION_RELEASED             0x04
#define HFP_SUBEVENT_COMPLETE                              0x05
#define HFP_SUBEVENT_AG_INDICATOR_STATUS_CHANGED           0x06
#define HFP_SUBEVENT_NETWORK_OPERATOR_CHANGED              0x07
#define HFP_SUBEVENT_EXTENDED_AUDIO_GATEWAY_ERROR          0x08
#define HFP_SUBEVENT_CODECS_CONNECTION_COMPLETE            0x09
#define HFP_SUBEVENT_START_RINGINIG                        0x0A
#define HFP_SUBEVENT_STOP_RINGINIG                         0x0B
#define HFP_SUBEVENT_CALL_TERMINATED                       0x0C
#define HFP_SUBEVENT_PLACE_CALL_WITH_NUMBER                0x0D
#define HFP_SUBEVENT_REDIAL_LAST_NUMBER                    0x0E
#define HFP_SUBEVENT_ATTACH_NUMBER_TO_VOICE_TAG            0x0F
#define HFP_SUBEVENT_NUMBER_FOR_VOICE_TAG                  0x10
#define HFP_SUBEVENT_TRANSMIT_DTMF_CODES                   0x11
#define HFP_SUBEVENT_TRANSMIT_STATUS_OF_CURRENT_CALL       0x12
#define HFP_SUBEVENT_CALL_ANSWERED                         0x13
#define HFP_SUBEVENT_CONFERENCE_CALL                       0x14
#define HFP_SUBEVENT_RING                                  0x15
#define HFP_SUBEVENT_SPEAKER_VOLUME                        0x16
#define HFP_SUBEVENT_MICROPHONE_VOLUME                     0x17

// ANCS Client

/**
 * @format 1H
 * @param subevent_code
 * @param handle
 */ 
#define ANCS_SUBEVENT_CLIENT_CONNECTED                              0xF0

/**
 * @format 1H2T
 * @param subevent_code
 * @param handle
 * @param attribute_id
 * @param text
 */ 
#define ANCS_SUBEVENT_CLIENT_NOTIFICATION                           0xF1

/**
 * @format 1H
 * @param subevent_code
 * @param handle
 */ 
#define ANCS_SUBEVENT_CLIENT_DISCONNECTED                           0xF2

#endif

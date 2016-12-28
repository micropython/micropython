/*
 * Copyright (C) 2014 BlueKitchen GmbH
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


#ifndef btstack_gatt_client_h
#define btstack_gatt_client_h

#include "hci.h"

#if defined __cplusplus
extern "C" {
#endif

typedef enum {
    P_READY,
    P_W2_SEND_SERVICE_QUERY,
    P_W4_SERVICE_QUERY_RESULT,
    P_W2_SEND_SERVICE_WITH_UUID_QUERY,
    P_W4_SERVICE_WITH_UUID_RESULT,
    
    P_W2_SEND_ALL_CHARACTERISTICS_OF_SERVICE_QUERY,
    P_W4_ALL_CHARACTERISTICS_OF_SERVICE_QUERY_RESULT,
    P_W2_SEND_CHARACTERISTIC_WITH_UUID_QUERY,
    P_W4_CHARACTERISTIC_WITH_UUID_QUERY_RESULT,
    
    P_W2_SEND_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY,
    P_W4_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT,
    
    P_W2_SEND_INCLUDED_SERVICE_QUERY,
    P_W4_INCLUDED_SERVICE_QUERY_RESULT,
    P_W2_SEND_INCLUDED_SERVICE_WITH_UUID_QUERY,
    P_W4_INCLUDED_SERVICE_UUID_WITH_QUERY_RESULT,
    
    P_W2_SEND_READ_CHARACTERISTIC_VALUE_QUERY,
    P_W4_READ_CHARACTERISTIC_VALUE_RESULT,
    
    P_W2_SEND_READ_BLOB_QUERY,
    P_W4_READ_BLOB_RESULT,

    P_W2_SEND_READ_BY_TYPE_REQUEST,
    P_W4_READ_BY_TYPE_RESPONSE,
    
    P_W2_SEND_READ_MULTIPLE_REQUEST,
    P_W4_READ_MULTIPLE_RESPONSE,

    P_W2_SEND_WRITE_CHARACTERISTIC_VALUE,
    P_W4_WRITE_CHARACTERISTIC_VALUE_RESULT,
    
    P_W2_PREPARE_WRITE,
    P_W4_PREPARE_WRITE_RESULT,
    P_W2_PREPARE_RELIABLE_WRITE,
    P_W4_PREPARE_RELIABLE_WRITE_RESULT,
    
    P_W2_EXECUTE_PREPARED_WRITE,
    P_W4_EXECUTE_PREPARED_WRITE_RESULT,
    P_W2_CANCEL_PREPARED_WRITE,
    P_W4_CANCEL_PREPARED_WRITE_RESULT,
    P_W2_CANCEL_PREPARED_WRITE_DATA_MISMATCH,
    P_W4_CANCEL_PREPARED_WRITE_DATA_MISMATCH_RESULT,
    
    P_W2_SEND_READ_CLIENT_CHARACTERISTIC_CONFIGURATION_QUERY,
    P_W4_READ_CLIENT_CHARACTERISTIC_CONFIGURATION_QUERY_RESULT,
    P_W2_WRITE_CLIENT_CHARACTERISTIC_CONFIGURATION,
    P_W4_CLIENT_CHARACTERISTIC_CONFIGURATION_RESULT,
    
    P_W2_SEND_READ_CHARACTERISTIC_DESCRIPTOR_QUERY,
    P_W4_READ_CHARACTERISTIC_DESCRIPTOR_RESULT,
    
    P_W2_SEND_READ_BLOB_CHARACTERISTIC_DESCRIPTOR_QUERY,
    P_W4_READ_BLOB_CHARACTERISTIC_DESCRIPTOR_RESULT,
    
    P_W2_SEND_WRITE_CHARACTERISTIC_DESCRIPTOR,
    P_W4_WRITE_CHARACTERISTIC_DESCRIPTOR_RESULT,

    // all long writes use this    
    P_W2_PREPARE_WRITE_CHARACTERISTIC_DESCRIPTOR,
    P_W4_PREPARE_WRITE_CHARACTERISTIC_DESCRIPTOR_RESULT,
    P_W2_EXECUTE_PREPARED_WRITE_CHARACTERISTIC_DESCRIPTOR,
    P_W4_EXECUTE_PREPARED_WRITE_CHARACTERISTIC_DESCRIPTOR_RESULT,

    // gatt reliable write API use this (manual version of the above)
    P_W2_PREPARE_WRITE_SINGLE,
    P_W4_PREPARE_WRITE_SINGLE_RESULT,

    P_W4_CMAC_READY,
    P_W4_CMAC_RESULT,
    P_W2_SEND_SIGNED_WRITE,
    P_W4_SEND_SINGED_WRITE_DONE,
} gatt_client_state_t;
    
    
typedef enum{
    SEND_MTU_EXCHANGE,
    SENT_MTU_EXCHANGE,
    MTU_EXCHANGED
} gatt_client_mtu_t;

typedef struct gatt_client{
    btstack_linked_item_t    item;
    // TODO: rename gatt_client_state -> state
    gatt_client_state_t gatt_client_state;

    // user callback 
    btstack_packet_handler_t callback;
    
    hci_con_handle_t con_handle;
    
    uint8_t   address_type;
    bd_addr_t address;
    uint16_t mtu;
    gatt_client_mtu_t mtu_state;
    
    uint16_t uuid16;
    uint8_t  uuid128[16];
    
    uint16_t start_group_handle;
    uint16_t end_group_handle;
    
    uint16_t query_start_handle;
    uint16_t query_end_handle;
    
    uint8_t  characteristic_properties;
    uint16_t characteristic_start_handle;
    
    uint16_t attribute_handle;
    uint16_t attribute_offset;
    uint16_t attribute_length;
    uint8_t* attribute_value;
    
    // read multiple characteristic values
    uint16_t    read_multiple_handle_count;
    uint16_t  * read_multiple_handles;

    uint16_t client_characteristic_configuration_handle;
    uint8_t  client_characteristic_configuration_value[2];
    
    uint8_t  filter_with_uuid;
    uint8_t  send_confirmation;
   
    int      le_device_index;
    uint8_t  cmac[8];

    btstack_timer_source_t gc_timeout;
} gatt_client_t;

typedef struct gatt_client_notification {
    btstack_linked_item_t    item;
    btstack_packet_handler_t callback;
    hci_con_handle_t con_handle;
    uint16_t attribute_handle;
} gatt_client_notification_t;

/* API_START */

typedef struct {
    uint16_t start_group_handle;
    uint16_t end_group_handle;
    uint16_t uuid16;
    uint8_t  uuid128[16];
} gatt_client_service_t;

typedef struct {
    uint16_t start_handle;
    uint16_t value_handle;
    uint16_t end_handle;
    uint16_t properties;
    uint16_t uuid16;
    uint8_t  uuid128[16];
} gatt_client_characteristic_t;

typedef struct {
    uint16_t handle;
    uint16_t uuid16;
    uint8_t  uuid128[16];
} gatt_client_characteristic_descriptor_t;

/** 
 * @brief Set up GATT client.
 */
void gatt_client_init(void);

/** 
 * @brief MTU is available after the first query has completed. If status is equal to 0, it returns the real value, otherwise the default value of 23. 
 */
uint8_t gatt_client_get_mtu(hci_con_handle_t con_handle, uint16_t * mtu);

/** 
 * @brief Returns if the GATT client is ready to receive a query. It is used with daemon. 
 */
int gatt_client_is_ready(hci_con_handle_t con_handle);

/** 
 * @brief Discovers all primary services. For each found service, an le_service_event_t with type set to GATT_EVENT_SERVICE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t, with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery. 
 */
uint8_t gatt_client_discover_primary_services(btstack_packet_handler_t callback, hci_con_handle_t con_handle);

/** 
 * @brief Discovers a specific primary service given its UUID. This service may exist multiple times. For each found service, an le_service_event_t with type set to GATT_EVENT_SERVICE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t, with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery. 
 */
uint8_t gatt_client_discover_primary_services_by_uuid16(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t uuid16);
uint8_t gatt_client_discover_primary_services_by_uuid128(btstack_packet_handler_t callback, hci_con_handle_t con_handle, const uint8_t  * uuid);

/** 
 * @brief Finds included services within the specified service. For each found included service, an le_service_event_t with type set to GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery. Information about included service type (primary/secondary) can be retrieved either by sending an ATT find information request for the returned start group handle (returning the handle and the UUID for primary or secondary service) or by comparing the service to the list of all primary services. 
 */
uint8_t gatt_client_find_included_services_for_service(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t  *service);

/** 
 * @brief Discovers all characteristics within the specified service. For each found characteristic, an le_characteristics_event_t with type set to GATT_EVENT_CHARACTERISTIC_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery.
 */
uint8_t gatt_client_discover_characteristics_for_service(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t  *service);

/** 
 * @brief The following four functions are used to discover all characteristics within the specified service or handle range, and return those that match the given UUID. For each found characteristic, an le_characteristic_event_t with type set to GATT_EVENT_CHARACTERISTIC_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery.
 */
uint8_t gatt_client_discover_characteristics_for_handle_range_by_uuid16(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, uint16_t uuid16);
uint8_t gatt_client_discover_characteristics_for_handle_range_by_uuid128(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, uint8_t  * uuid);
uint8_t gatt_client_discover_characteristics_for_service_by_uuid16 (btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t  *service, uint16_t  uuid16);
uint8_t gatt_client_discover_characteristics_for_service_by_uuid128(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t  *service, uint8_t  * uuid128);

/** 
 * @brief Discovers attribute handle and UUID of a characteristic descriptor within the specified characteristic. For each found descriptor, an le_characteristic_descriptor_event_t with type set to GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery.
 */
uint8_t gatt_client_discover_characteristic_descriptors(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t  *characteristic);

/** 
 * @brief Reads the characteristic value using the characteristic's value handle. If the characteristic value is found, an le_characteristic_value_event_t with type set to GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
 */
uint8_t gatt_client_read_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t  *characteristic);
uint8_t gatt_client_read_value_of_characteristic_using_value_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle);

/**
 * @brief Reads the characteric value of all characteristics with the uuid. For each found, an le_characteristic_value_event_t with type set to GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
 */
uint8_t gatt_client_read_value_of_characteristics_by_uuid16(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, uint16_t uuid16);
uint8_t gatt_client_read_value_of_characteristics_by_uuid128(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, uint8_t * uuid128);

/** 
 * @brief Reads the long characteristic value using the characteristic's value handle. The value will be returned in several blobs. For each blob, an le_characteristic_value_event_t with type set to GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT and updated value offset will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, mark the end of read.
 */
uint8_t gatt_client_read_long_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t  *characteristic);
uint8_t gatt_client_read_long_value_of_characteristic_using_value_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle);
uint8_t gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t offset);

/*
 * @brief Read multiple characteristic values
 * @param number handles
 * @param list_of_handles list of handles 
 */
uint8_t gatt_client_read_multiple_characteristic_values(btstack_packet_handler_t callback, hci_con_handle_t con_handle, int num_value_handles, uint16_t * value_handles);

/** 
 * @brief Writes the characteristic value using the characteristic's value handle without an acknowledgment that the write was successfully performed.
 */
uint8_t gatt_client_write_value_of_characteristic_without_response(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t  * data);

/** 
 * @brief Writes the authenticated characteristic value using the characteristic's value handle without an acknowledgment that the write was successfully performed.
 */
uint8_t gatt_client_signed_write_without_response(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t handle, uint16_t message_len, uint8_t  * message);

/** 
 * @brief Writes the characteristic value using the characteristic's value handle. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of write. The write is successfully performed, if the event's status field is set to 0.
 */
uint8_t gatt_client_write_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t  * data);
uint8_t gatt_client_write_long_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t  * data);
uint8_t gatt_client_write_long_value_of_characteristic_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t offset, uint16_t length, uint8_t  * data);

/** 
 * @brief Writes of the long characteristic value using the characteristic's value handle. It uses server response to validate that the write was correctly received. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE marks the end of write. The write is successfully performed, if the event's status field is set to 0.
 */
uint8_t gatt_client_reliable_write_long_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t  * data);

/** 
 * @brief Reads the characteristic descriptor using its handle. If the characteristic descriptor is found, an le_characteristic_descriptor_event_t with type set to GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
 */
uint8_t gatt_client_read_characteristic_descriptor(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t  * descriptor);
uint8_t gatt_client_read_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle);

/** 
 * @brief Reads the long characteristic descriptor using its handle. It will be returned in several blobs. For each blob, an le_characteristic_descriptor_event_t with type set to GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
 */
uint8_t gatt_client_read_long_characteristic_descriptor(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t  * descriptor);
uint8_t gatt_client_read_long_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle);
uint8_t gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t offset);

/** 
 * @brief Writes the characteristic descriptor using its handle. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of write. The write is successfully performed, if the event's status field is set to 0.
 */
uint8_t gatt_client_write_characteristic_descriptor(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t  * descriptor, uint16_t length, uint8_t  * data);
uint8_t gatt_client_write_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t length, uint8_t  * data);
uint8_t gatt_client_write_long_characteristic_descriptor(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t  * descriptor, uint16_t length, uint8_t  * data);
uint8_t gatt_client_write_long_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t length, uint8_t  * data);
uint8_t gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t offset, uint16_t length, uint8_t  * data);

/** 
 * @brief Writes the client characteristic configuration of the specified characteristic. It is used to subscribe for notifications or indications of the characteristic value. For notifications or indications specify: GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION resp. GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION as configuration value.
 */
uint8_t gatt_client_write_client_characteristic_configuration(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t * characteristic, uint16_t configuration);

/**
 * @brief Register for notifications and indications of a characteristic enabled by gatt_client_write_client_characteristic_configuration
 * @param notification struct used to store registration
 * @param con_handle
 * @param characteristic
 */
void gatt_client_listen_for_characteristic_value_updates(gatt_client_notification_t * notification, hci_con_handle_t con_handle, gatt_client_characteristic_t * characteristic);

/**
 * @brief -> gatt complete event
 */
uint8_t gatt_client_prepare_write(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t offset, uint16_t length, uint8_t * data);

/**
 * @brief -> gatt complete event
 */
uint8_t gatt_client_execute_write(btstack_packet_handler_t callback, hci_con_handle_t con_handle);

/**
 * @brief -> gatt complete event
 */
uint8_t gatt_client_cancel_write(btstack_packet_handler_t callback, hci_con_handle_t con_handle);

/* API_END */

// used by generated btstack_event.c

void gatt_client_deserialize_service(const uint8_t *packet, int offset, gatt_client_service_t *service);
void gatt_client_deserialize_characteristic(const uint8_t * packet, int offset, gatt_client_characteristic_t * characteristic);
void gatt_client_deserialize_characteristic_descriptor(const uint8_t * packet, int offset, gatt_client_characteristic_descriptor_t * descriptor);

// only used for testing
void gatt_client_pts_suppress_mtu_exchange(void);

#if defined __cplusplus
}
#endif

#endif

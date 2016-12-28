
#ifndef BTSTACK_HAL_H_
#define BTSTACK_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "btstack.h"
#include "btstack_chipset_bcm.h"
#include "btstack_config.h"
#include "hci_dump.h"

/**@brief BLE status */
typedef enum BLEStatus {
    BLE_STATUS_OK,
    BLE_STATUS_DONE,
    BLE_STATUS_CONNECTION_TIMEOUT,
    BLE_STATUS_CONNECTION_ERROR,
    BLE_STATUS_OTHER_ERROR
} BLEStatus_t;

/**@brief BLE advertising report data. */
typedef struct{
    uint8_t peerAddrType;
    bd_addr_t  peerAddr;
    int     rssi;
    uint8_t advEventType;
    uint8_t advDataLen;
    uint8_t advData[31];
}advertisementReport_t;

typedef struct{
    uint16_t adv_int_min;
    uint16_t adv_int_max;
    uint8_t adv_type;
    uint8_t dir_addr_type;
    uint8_t dir_addr[BD_ADDR_LEN];
    uint8_t channel_map;
    uint8_t filter_policy;
}advParams_t;


/**@brief Device API */
void hal_btstack_init(void);
void hal_btstack_deInit(void);
void hal_btstack_loop_execute(void);

void     hal_btstack_setTimer(btstack_timer_source_t *ts, uint32_t timeout_in_ms);
void     hal_btstack_setTimerHandler(btstack_timer_source_t *ts, void (*process)(btstack_timer_source_t *_ts));
void     hal_btstack_addTimer(btstack_timer_source_t *timer);
int      hal_btstack_removeTimer(btstack_timer_source_t *timer);
uint32_t hal_btstack_getTimeMs(void);

void hal_btstack_Log_info(uint8_t flag);
void hal_btstack_error_info(uint8_t flag);
void hal_btstack_enable_packet_info(void);

/**@brief Gap API */
void hal_btstack_setRandomAddressMode(gap_random_address_type_t random_address_type);
void hal_btstack_setRandomAddr(bd_addr_t addr);
void hal_btstack_setPublicBdAddr(bd_addr_t addr);
void hal_btstack_setLocalName(const char *local_name);
void hal_btstack_setAdvParams(uint16_t adv_int_min, uint16_t adv_int_max, uint8_t adv_type, uint8_t dir_addr_type, bd_addr_t dir_addr, uint8_t channel_map, uint8_t filter_policy);
void hal_btstack_setAdvData(uint16_t size, uint8_t *data);

void hal_btstack_startAdvertising(void);
void hal_btstack_stopAdvertising(void);

void hal_btstack_setConnectedCallback(void (*callback)(BLEStatus_t status, uint16_t handle));
void hal_btstack_setDisconnectedCallback(void (*callback)(uint16_t handle));

void hal_btstack_disconnect(uint16_t handle);
uint8_t hal_btstack_connect(bd_addr_t addr, bd_addr_type_t type);

void hal_btstack_setConnParamsRange(le_connection_parameter_range_t range);

void hal_btstack_startScanning(void);
void hal_btstack_stopScanning(void);

void hal_btstack_setScanParams(uint8_t scan_type, uint16_t scan_interval, uint16_t scan_window);
void hal_btstack_setBLEAdvertisementCallback(void (*cb)(advertisementReport_t *advertisement_report));

/**@brief Gatt server API */
int  hal_btstack_attServerCanSend(void);
int  hal_btstack_attServerSendNotify(uint16_t value_handle, uint8_t *value, uint16_t length);
int  hal_btstack_attServerSendIndicate(uint16_t value_handle, uint8_t *value, uint16_t length);

void hal_btstack_setGattCharsRead(uint16_t (*cb)(uint16_t handle, uint8_t *buffer, uint16_t buffer_size));
void hal_btstack_setGattCharsWrite(int (*cb)(uint16_t handle, uint8_t *buffer, uint16_t buffer_size));

void hal_btstack_addServiceUUID16bits(uint16_t uuid);
void hal_btstack_addServiceUUID128bits(uint8_t *uuid);

uint16_t hal_btstack_addCharsUUID16bits(uint16_t uuid, uint16_t flags, uint8_t *data, uint16_t data_len);
uint16_t hal_btstack_addCharsDynamicUUID16bits(uint16_t uuid, uint16_t flags, uint8_t *data, uint16_t data_len);
uint16_t hal_btstack_addCharsUUID128bits(uint8_t *uuid, uint16_t flags, uint8_t *data, uint16_t data_len);
uint16_t hal_btstack_addCharsDynamicUUID128bits(uint8_t *uuid, uint16_t flags, uint8_t *data, uint16_t data_len);


/**@brief Gatt client API */
void hal_btstack_setGattServiceDiscoveredCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, gatt_client_service_t *service));
void hal_btstack_setGattCharsDiscoveredCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, gatt_client_characteristic_t *characteristic));
void hal_btstack_setGattCharsDescriptorsDiscoveredCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, gatt_client_characteristic_descriptor_t *characteristic));
void hal_btstack_setGattCharacteristicReadCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, uint16_t value_handle, uint8_t * value, uint16_t length));
void hal_btstack_setGattCharacteristicWrittenCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle));

void hal_btstack_setGattCharsDescriptorReadCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, uint16_t value_handle, uint8_t * value, uint16_t length));
void hal_btstack_setGattCharsDescriptorWrittenCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle));

void hal_btstack_setGattWriteClientCharacteristicConfigCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle));
void hal_btstack_setGattNotifyUpdateCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, uint16_t value_handle, uint8_t * value, uint16_t length));
void hal_btstack_setGattIndicateUpdateCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, uint16_t value_handle, uint8_t * value, uint16_t length));


uint8_t hal_btstack_discoverPrimaryServices(uint16_t con_handle);
uint8_t hal_btstack_discoverPrimaryServicesByUUID16(uint16_t con_handle, uint16_t uuid16);
uint8_t hal_btstack_discoverPrimaryServicesByUUID128(uint16_t con_handle, const uint8_t *uuid);

uint8_t hal_btstack_discoverCharsForService(uint16_t con_handle, gatt_client_service_t  *service);
uint8_t hal_btstack_discoverCharsForHandleRangeByUUID16(uint16_t con_handle, uint16_t start_handle, uint16_t end_handle, uint16_t uuid16);
uint8_t hal_btstack_discoverCharsForHandleRangeByUUID128(uint16_t con_handle, uint16_t start_handle, uint16_t end_handle, uint8_t *uuid);
uint8_t hal_btstack_discoverCharsForServiceByUUID16(uint16_t con_handle, gatt_client_service_t *service, uint16_t uuid16);
uint8_t hal_btstack_discoverCharsForServiceByUUID128(uint16_t con_handle, gatt_client_service_t *service, uint8_t *uuid128);

uint8_t hal_btstack_discoverCharsDescriptors(uint16_t con_handle, gatt_client_characteristic_t *characteristic);

uint8_t hal_btstack_readValueOfCharacteristic(uint16_t con_handle, gatt_client_characteristic_t  *characteristic);
uint8_t hal_btstack_readValueOfCharacteristicUsingValueHandle(uint16_t con_handle, uint16_t characteristic_value_handle);
uint8_t hal_btstack_readValueOfCharacteristicByUUID16(uint16_t con_handle, uint16_t start_handle, uint16_t end_handle, uint16_t uuid16);
uint8_t hal_btstack_readValueOfCharacteristicByUUID128(uint16_t con_handle, uint16_t start_handle, uint16_t end_handle, uint8_t *uuid128);
uint8_t hal_btstack_readLongValueOfCharacteristic(uint16_t con_handle, gatt_client_characteristic_t *characteristic);
uint8_t hal_btstack_readLongValueOfCharacteristicUsingValueHandle(uint16_t con_handle, uint16_t characteristic_value_handle);
uint8_t hal_btstack_readLongValueOfCharacteristicUsingValueHandleWithOffset(uint16_t con_handle, uint16_t characteristic_value_handle, uint16_t offset);

uint8_t hal_btstack_writeValueOfChracteristicWithoutResponse(uint16_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t *data);
uint8_t hal_btstack_writeValueOfCharacteristic(uint16_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t *data);
uint8_t hal_btstack_writeLongValueOfCharacteristic(uint16_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t *data);
uint8_t hal_btstack_writeLongValueOfCharacteristicWithOffset(uint16_t con_handle, uint16_t characteristic_value_handle, uint16_t offset, uint16_t length, uint8_t *data);

uint8_t hal_btstack_readCharacteristicDescriptor(uint16_t con_handle, gatt_client_characteristic_descriptor_t *descriptor);
uint8_t hal_btstack_readCharacteristicDescriptorUsingDescriptorHandle(uint16_t con_handle, uint16_t descriptor_handle);
uint8_t hal_btstack_readLongCharacteristicDescriptor(uint16_t con_handle, gatt_client_characteristic_descriptor_t *descriptor);
uint8_t hal_btstack_readLongCharacteristicDescriptorUsingDescriptorHandle(uint16_t con_handle, uint16_t descriptor_handle);
uint8_t hal_btstack_readLongCharacteristicDescriptorUsingDescriptorHandleWithOffset(uint16_t con_handle, uint16_t descriptor_handle, uint16_t offset);

uint8_t hal_btstack_writeCharacteristicDescriptor(uint16_t con_handle, gatt_client_characteristic_descriptor_t *descriptor, uint16_t length, uint8_t *data);
uint8_t hal_btstack_writeCharacteristicDescriptorUsingDescriptorHandle(uint16_t con_handle, uint16_t descriptor_handle, uint16_t length, uint8_t *data);
uint8_t hal_btstack_writeLongCharacteristicDescriptor(uint16_t con_handle, gatt_client_characteristic_descriptor_t *descriptor, uint16_t length, uint8_t *data);
uint8_t hal_btstack_writeLongCharacteristicDescriptorUsingDescriptorHandle(uint16_t con_handle, uint16_t descriptor_handle, uint16_t length, uint8_t *data);
uint8_t hal_btstack_writeLongCharacteristicDescriptorUsingDescriptorHandleWithOffset(uint16_t con_handle, uint16_t descriptor_handle, uint16_t offset, uint16_t length, uint8_t *data);


uint8_t hal_btstack_WriteClientCharacteristicConfiguration(uint16_t con_handle, gatt_client_characteristic_t *characteristic, uint16_t configuration);
void hal_btstack_listenForCharacteristicValueUpdates(gatt_client_notification_t *notification, uint16_t con_handle, gatt_client_characteristic_t *characteristic);



#ifdef __cplusplus
}
#endif

#endif

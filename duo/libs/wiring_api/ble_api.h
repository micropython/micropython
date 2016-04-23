#ifndef _BLE_API_H
#define _BLE_API_H

#include "btstack_hal.h"

void ble_init(void);
void ble_deInit(void);

void ble_setTimer(btstack_timer_source_t *ts, uint32_t timeout_in_ms);
void ble_setTimerHandler(btstack_timer_source_t *ts, void (*process)(btstack_timer_source_t *_ts));
void ble_addTimer(btstack_timer_source_t *timer);
int ble_removeTimer(btstack_timer_source_t *timer);
uint32_t ble_getTimeMs(void);

void ble_debugLogger(bool flag);
void ble_debugError(bool flag);
void ble_enablePacketLogger(void);

void ble_setRandomAddrMode(gap_random_address_type_t random_addr_type);
void ble_setRandomAddr(bd_addr_t addr);
void ble_setPublicBDAddr(bd_addr_t addr);
void ble_setLocalName(const char *local_name);
void ble_setAdvParams(advParams_t *adv_params);
void ble_setAdvData(uint16_t size, uint8_t *data);

void ble_onConnectedCallback(void (*callback)(BLEStatus_t status, uint16_t handle));
void ble_onDisconnectedCallback(void (*callback)(uint16_t handle));

void ble_startAdvertising(void);
void ble_stopAdvertising(void);

void ble_disconnect(uint16_t conn_handle);
uint8_t ble_connect(bd_addr_t addr, bd_addr_type_t type);

void ble_setConnParams(le_connection_parameter_range_t range);
void ble_setScanParams(uint8_t scan_type, uint16_t scan_interval, uint16_t scan_window);

void ble_startScanning(void);
void ble_stopScanning(void);

void ble_onScanReportCallback(void (*cb)(advertisementReport_t *advertisement_report));

void ble_addServiceUUID16(uint16_t uuid);
void ble_addServiceUUID128(uint8_t *uuid);

uint16_t ble_addCharacteristicUUID16(uint16_t uuid, uint16_t flags, uint8_t *data, uint16_t data_len);
uint16_t ble_addCharacteristicUUID128(uint8_t *uuid, uint16_t flags, uint8_t *data, uint16_t data_len);
uint16_t ble_addCharacteristicDynamicUUID16(uint16_t uuid, uint16_t flags, uint8_t *data, uint16_t data_len);
uint16_t ble_addCharacteristicDynamicUUID128(uint8_t *uuid, uint16_t flags, uint8_t *data, uint16_t data_len);

void ble_onDataReadCallback(uint16_t (*cb)(uint16_t handle, uint8_t *buffer, uint16_t buffer_size));
void ble_onDataWriteCallback(int (*cb)(uint16_t handle, uint8_t *buffer, uint16_t buffer_size));

int ble_attServerCanSendPacket(void);

int ble_sendNotify(uint16_t value_handle, uint8_t *value, uint16_t length);
int ble_sendIndicate(uint16_t value_handle, uint8_t *value, uint16_t length);

void ble_onServiceDiscoveredCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, gatt_client_service_t *service));
void ble_onCharacteristicDiscoveredCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, gatt_client_characteristic_t *characteristic));
void ble_onDescriptorDiscoveredCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, gatt_client_characteristic_descriptor_t *characteristic));
void ble_onGattCharacteristicReadCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, uint16_t value_handle, uint8_t * value, uint16_t length));
void ble_onGattCharacteristicWrittenCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle));
void ble_onGattDescriptorReadCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, uint16_t value_handle, uint8_t * value, uint16_t length));
void ble_onGattDescriptorWrittenCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle));
void ble_onGattWriteClientCharacteristicConfigCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle));
void ble_onGattNotifyUpdateCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, uint16_t value_handle, uint8_t * value, uint16_t length));
void ble_onGattIndicateUpdateCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, uint16_t value_handle, uint8_t * value, uint16_t length));

uint8_t ble_discoverPrimaryServices(uint16_t con_handle);
uint8_t ble_discoverPrimaryServicesByUUID16(uint16_t con_handle, uint16_t uuid16);
uint8_t ble_discoverPrimaryServicesByUUID128(uint16_t con_handle, const uint8_t *uuid);
uint8_t ble_discoverCharacteristics(uint16_t con_handle, gatt_client_service_t *service);
uint8_t ble_discoverCharacteristicsForHandleByUUID16(uint16_t con_handle, uint16_t start_handle, uint16_t end_handle, uint16_t uuid16);
uint8_t ble_discoverCharacteristicsForHandleByUUID128(uint16_t con_handle, uint16_t start_handle, uint16_t end_handle, uint8_t *uuid);
uint8_t ble_discoverCharacteristicsForServiceByUUID16(uint16_t con_handle, gatt_client_service_t *service, uint16_t uuid16);
uint8_t ble_discoverCharacteristicsForServiceByUUID128(uint16_t con_handle, gatt_client_service_t *service, uint8_t *uuid128);
uint8_t ble_discoverCharacteristicDescriptors(uint16_t con_handle, gatt_client_characteristic_t *characteristic);

uint8_t ble_readValue(uint16_t con_handle, gatt_client_characteristic_t  *characteristic);
uint8_t ble_readValueUsingHandle(uint16_t con_handle, uint16_t characteristic_value_handle);
uint8_t ble_readValueByUUID16(uint16_t con_handle, uint16_t start_handle, uint16_t end_handle, uint16_t uuid16);
uint8_t ble_readValueByUUID128(uint16_t con_handle, uint16_t start_handle, uint16_t end_handle, uint8_t *uuid128);
uint8_t ble_readLongValue(uint16_t con_handle, gatt_client_characteristic_t *characteristic);
uint8_t ble_readLongValueUsingHandle(uint16_t con_handle, uint16_t characteristic_value_handle);
uint8_t ble_readLongValueWithOffset(uint16_t con_handle, uint16_t characteristic_value_handle, uint16_t offset);

uint8_t ble_writeValueWithoutResponse(uint16_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t *data);
uint8_t ble_writeValue(uint16_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t *data);
uint8_t ble_writeLongValue(uint16_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t *data);
uint8_t ble_writeLongValueWithOffset(uint16_t con_handle, uint16_t characteristic_value_handle, uint16_t offset, uint16_t length, uint8_t *data);

uint8_t ble_readDescriptorValue(uint16_t con_handle, gatt_client_characteristic_descriptor_t *descriptor);
uint8_t ble_readDescriptorValueUsingHandle(uint16_t con_handle, uint16_t descriptor_handle);
uint8_t ble_readLongDescriptorValue(uint16_t con_handle, gatt_client_characteristic_descriptor_t *descriptor);
uint8_t ble_readLongDescriptorValueUsingHandle(uint16_t con_handle, uint16_t descriptor_handle);
uint8_t ble_readLongDescriptorValueWithOffset(uint16_t con_handle, uint16_t descriptor_handle, uint16_t offset);

uint8_t ble_writeDescriptorValue(uint16_t con_handle, gatt_client_characteristic_descriptor_t *descriptor, uint16_t length, uint8_t *data);
uint8_t ble_writeDescriptorValueUsingHandle(uint16_t con_handle, uint16_t descriptor_handle, uint16_t length, uint8_t *data);
uint8_t ble_writeLongDescriptorValue(uint16_t con_handle, gatt_client_characteristic_descriptor_t *descriptor, uint16_t length, uint8_t *data);;
uint8_t ble_writeLongDescriptorValueWithOffset(uint16_t con_handle, uint16_t descriptor_handle, uint16_t offset, uint16_t length, uint8_t *data);
uint8_t ble_writeClientCharsConfigDescritpor(uint16_t con_handle, gatt_client_characteristic_t *characteristic, uint16_t configuration);


#endif

#include "spark_wiring_btstack.h"

#ifdef __cplusplus
extern "C" {
#endif


void ble_init(void)
{
    ble.init();
}

void ble_deInit(void)
{
    ble.deInit();
}

void ble_setTimer(btstack_timer_source_t *ts, uint32_t timeout_in_ms)
{
    ble.setTimer(ts, timeout_in_ms);
}

void ble_setTimerHandler(btstack_timer_source_t *ts, void (*process)(btstack_timer_source_t *_ts))
{
    ble.setTimerHandler(ts, process);
}

void ble_addTimer(btstack_timer_source_t *timer)
{
    ble.addTimer(timer);
}
int ble_removeTimer(btstack_timer_source_t *timer)
{
    return ble.removeTimer(timer);
}

uint32_t ble_getTimeMs(void)
{
    return ble.getTimeMs();
}

void ble_debugLogger(bool flag)
{
	ble.debugLogger(flag);
}

void ble_debugError(bool flag)
{
	ble.debugError(flag);
}

void ble_enablePacketLogger(void)
{
	ble.enablePacketLogger();
}

/***************************************************************
 *
 * Gap API
 *
***************************************************************/

void ble_setRandomAddrMode(gap_random_address_type_t random_addr_type)
{
    ble.setRandomAddrMode(random_addr_type);
}

void ble_setRandomAddr(bd_addr_t addr)
{
    ble.setRandomAddr(addr);
}

void ble_setPublicBDAddr(bd_addr_t addr)
{
    ble.setPublicBDAddr(addr);
}

void ble_setLocalName(const char *local_name)
{
    ble.setLocalName(local_name);
}

void ble_setAdvParams(advParams_t *adv_params)
{
    ble.setAdvParams(adv_params);
}

void ble_setAdvData(uint16_t size, uint8_t *data)
{
    ble.setAdvData(size, data);
}

void ble_onConnectedCallback(void (*callback)(BLEStatus_t status, uint16_t handle))
{
    ble.onConnectedCallback(callback);
}

void ble_onDisconnectedCallback(void (*callback)(uint16_t handle))
{
    ble.onDisconnectedCallback(callback);
}

void ble_startAdvertising(void)
{
    ble.startAdvertising();
}

void ble_stopAdvertising(void)
{
    ble.stopAdvertising();
}

void ble_disconnect(uint16_t conn_handle)
{
    ble.disconnect(conn_handle);
}

uint8_t ble_connect(bd_addr_t addr, bd_addr_type_t type)
{
    return ble.connect(addr, type);
}

void ble_setConnParams(le_connection_parameter_range_t range)
{
    ble.setConnParams(range);
}

void ble_startScanning(void)
{
    ble.startScanning();
}

void ble_stopScanning(void)
{
    ble.stopScanning();
}

void ble_setScanParams(uint8_t scan_type, uint16_t scan_interval, uint16_t scan_window)
{
    ble.setScanParams(scan_type, scan_interval, scan_window);
}

void ble_onScanReportCallback(void (*cb)(advertisementReport_t *advertisement_report))
{
    ble.onScanReportCallback(cb);
}

/***************************************************************
 *
 * Gatt server API
 *
***************************************************************/
void ble_addServiceUUID16(uint16_t uuid)
{
    ble.addService(uuid);
}

void ble_addServiceUUID128(uint8_t *uuid)
{
    ble.addService(uuid);
}

/**
 * @brief Add characteristic.
 *
 * @Note Just has read permit, if need to write,use "addCharacteristicDynamic".
 */
uint16_t ble_addCharacteristicUUID16(uint16_t uuid, uint16_t flags, uint8_t *data, uint16_t data_len)
{
    return ble.addCharacteristic(uuid, flags, data, data_len);
}

uint16_t ble_addCharacteristicUUID128(uint8_t *uuid, uint16_t flags, uint8_t *data, uint16_t data_len)
{
    return ble.addCharacteristic(uuid, flags, data, data_len);
}

uint16_t ble_addCharacteristicDynamicUUID16(uint16_t uuid, uint16_t flags, uint8_t *data, uint16_t data_len)
{
    return ble.addCharacteristicDynamic(uuid, flags, data, data_len);
}

uint16_t ble_addCharacteristicDynamicUUID128(uint8_t *uuid, uint16_t flags, uint8_t *data, uint16_t data_len)
{
    return ble.addCharacteristicDynamic(uuid, flags, data, data_len);
}

void ble_onDataReadCallback(uint16_t (*cb)(uint16_t handle, uint8_t *buffer, uint16_t buffer_size))
{
    ble.onDataReadCallback(cb);
}

void ble_onDataWriteCallback(int (*cb)(uint16_t handle, uint8_t *buffer, uint16_t buffer_size))
{
    ble.onDataWriteCallback(cb);
}

int ble_attServerCanSendPacket(void)
{
    return ble.attServerCanSendPacket();
}

int ble_sendNotify(uint16_t value_handle, uint8_t *value, uint16_t length)
{
    return ble.sendNotify(value_handle, value, length);
}

int ble_sendIndicate(uint16_t value_handle, uint8_t *value, uint16_t length)
{
    return ble.sendIndicate(value_handle, value, length);
}

/***************************************************************
 *
 * Gatt client API
 *
***************************************************************/
void ble_onServiceDiscoveredCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, gatt_client_service_t *service))
{
	ble.onServiceDiscoveredCallback(cb);
}

void ble_onCharacteristicDiscoveredCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, gatt_client_characteristic_t *characteristic))
{
	ble.onCharacteristicDiscoveredCallback(cb);
}

void ble_onDescriptorDiscoveredCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, gatt_client_characteristic_descriptor_t *characteristic))
{
	ble.onDescriptorDiscoveredCallback(cb);
}

void ble_onGattCharacteristicReadCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, uint16_t value_handle, uint8_t * value, uint16_t length))
{
	ble.onGattCharacteristicReadCallback(cb);
}

void ble_onGattCharacteristicWrittenCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle))
{
	ble.onGattCharacteristicWrittenCallback(cb);
}

void ble_onGattDescriptorReadCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, uint16_t value_handle, uint8_t * value, uint16_t length))
{
	ble.onGattDescriptorReadCallback(cb);
}

void ble_onGattDescriptorWrittenCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle))
{
	ble.onGattDescriptorWrittenCallback(cb);
}

void ble_onGattWriteClientCharacteristicConfigCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle))
{
	ble.onGattWriteClientCharacteristicConfigCallback(cb);
}

void ble_onGattNotifyUpdateCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, uint16_t value_handle, uint8_t * value, uint16_t length))
{
	ble.onGattNotifyUpdateCallback(cb);
}

void ble_onGattIndicateUpdateCallback(void (*cb)(BLEStatus_t status, uint16_t con_handle, uint16_t value_handle, uint8_t * value, uint16_t length))
{
	ble.onGattIndicateUpdateCallback(cb);
}

uint8_t ble_discoverPrimaryServices(uint16_t con_handle)
{
	return ble.discoverPrimaryServices(con_handle);
}

uint8_t ble_discoverPrimaryServicesByUUID16(uint16_t con_handle, uint16_t uuid16)
{
	return ble.discoverPrimaryServices(con_handle, uuid16);
}

uint8_t ble_discoverPrimaryServicesByUUID128(uint16_t con_handle, const uint8_t *uuid)
{
	return ble.discoverPrimaryServices(con_handle, uuid);
}

uint8_t ble_discoverCharacteristics(uint16_t con_handle, gatt_client_service_t *service)
{
	return ble.discoverCharacteristics(con_handle, service);
}

uint8_t ble_discoverCharacteristicsForHandleByUUID16(uint16_t con_handle, uint16_t start_handle, uint16_t end_handle, uint16_t uuid16)
{
	return ble.discoverCharacteristics(con_handle, start_handle, end_handle, uuid16);
}

uint8_t ble_discoverCharacteristicsForHandleByUUID128(uint16_t con_handle, uint16_t start_handle, uint16_t end_handle, uint8_t *uuid)
{
	return ble.discoverCharacteristics(con_handle, start_handle, end_handle, uuid);
}

uint8_t ble_discoverCharacteristicsForServiceByUUID16(uint16_t con_handle, gatt_client_service_t *service, uint16_t uuid16)
{
	return ble.discoverCharacteristics(con_handle, service, uuid16);
}

uint8_t ble_discoverCharacteristicsForServiceByUUID128(uint16_t con_handle, gatt_client_service_t *service, uint8_t *uuid128)
{
	return ble.discoverCharacteristics(con_handle, service, uuid128);
}

uint8_t ble_discoverCharacteristicDescriptors(uint16_t con_handle, gatt_client_characteristic_t *characteristic)
{
	return ble.discoverCharacteristicDescriptors(con_handle, characteristic);
}

uint8_t ble_readValue(uint16_t con_handle, gatt_client_characteristic_t  *characteristic)
{
	return ble.readValue(con_handle, characteristic);
}

uint8_t ble_readValueUsingHandle(uint16_t con_handle, uint16_t characteristic_value_handle)
{
	return ble.readValue(con_handle, characteristic_value_handle);
}

uint8_t ble_readValueByUUID16(uint16_t con_handle, uint16_t start_handle, uint16_t end_handle, uint16_t uuid16)
{
	return ble.readValue(con_handle, start_handle, end_handle, uuid16);
}

uint8_t ble_readValueByUUID128(uint16_t con_handle, uint16_t start_handle, uint16_t end_handle, uint8_t *uuid128)
{
	return ble.readValue(con_handle, start_handle, end_handle, uuid128);
}

uint8_t ble_readLongValue(uint16_t con_handle, gatt_client_characteristic_t *characteristic)
{
	return ble.readLongValue(con_handle, characteristic);
}

uint8_t ble_readLongValueUsingHandle(uint16_t con_handle, uint16_t characteristic_value_handle)
{
	return ble.readLongValue(con_handle, characteristic_value_handle);
}

uint8_t ble_readLongValueWithOffset(uint16_t con_handle, uint16_t characteristic_value_handle, uint16_t offset)
{
	return ble.readLongValueWithOffset(con_handle, characteristic_value_handle, offset);
}

uint8_t ble_writeValueWithoutResponse(uint16_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t *data)
{
	return ble.writeValueWithoutResponse(con_handle, characteristic_value_handle, length, data);
}

uint8_t ble_writeValue(uint16_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t *data)
{
	return ble.writeValue(con_handle, characteristic_value_handle, length, data);
}

uint8_t ble_writeLongValue(uint16_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t *data)
{
	return ble.writeLongValue(con_handle, characteristic_value_handle, length, data);
}

uint8_t ble_writeLongValueWithOffset(uint16_t con_handle, uint16_t characteristic_value_handle, uint16_t offset, uint16_t length, uint8_t *data)
{
	return ble.writeLongValueWithOffset(con_handle, characteristic_value_handle, offset, length, data);
}

uint8_t ble_readDescriptorValue(uint16_t con_handle, gatt_client_characteristic_descriptor_t *descriptor)
{
	return ble.readDescriptorValue(con_handle, descriptor);
}

uint8_t ble_readDescriptorValueUsingHandle(uint16_t con_handle, uint16_t descriptor_handle)
{
	return ble.readDescriptorValue(con_handle, descriptor_handle);
}

uint8_t ble_readLongDescriptorValue(uint16_t con_handle, gatt_client_characteristic_descriptor_t *descriptor)
{
	return ble.readLongDescriptorValue(con_handle, descriptor);
}

uint8_t ble_readLongDescriptorValueUsingHandle(uint16_t con_handle, uint16_t descriptor_handle)
{
	return ble.readLongDescriptorValue(con_handle, descriptor_handle);
}

uint8_t ble_readLongDescriptorValueWithOffset(uint16_t con_handle, uint16_t descriptor_handle, uint16_t offset)
{
	return ble.readLongDescriptorValueWithOffset(con_handle, descriptor_handle, offset);
}

uint8_t ble_writeDescriptorValue(uint16_t con_handle, gatt_client_characteristic_descriptor_t *descriptor, uint16_t length, uint8_t *data)
{
	return ble.writeDescriptorValue(con_handle, descriptor, length, data);
}

uint8_t ble_writeDescriptorValueUsingHandle(uint16_t con_handle, uint16_t descriptor_handle, uint16_t length, uint8_t *data)
{
	return ble.writeDescriptorValue(con_handle, descriptor_handle, length, data);
}

uint8_t ble_writeLongDescriptorValue(uint16_t con_handle, gatt_client_characteristic_descriptor_t *descriptor, uint16_t length, uint8_t *data)
{
	return ble.writeLongDescriptorValue(con_handle, descriptor, length, data);
}

uint8_t ble_writeLongDescriptorValueUsingHandle(uint16_t con_handle, uint16_t descriptor_handle, uint16_t length, uint8_t *data)
{
	return ble.writeLongDescriptorValue(con_handle, descriptor_handle, length, data);
}

uint8_t ble_writeLongDescriptorValueWithOffset(uint16_t con_handle, uint16_t descriptor_handle, uint16_t offset, uint16_t length, uint8_t *data)
{
	return ble.writeLongDescriptorValueWithOffset(con_handle, descriptor_handle, offset, length, data);
}

uint8_t ble_writeClientCharsConfigDescritpor(uint16_t con_handle, gatt_client_characteristic_t *characteristic, uint16_t configuration)
{
	return ble.writeClientCharsConfigDescritpor(con_handle, characteristic, configuration);
}


#ifdef __cplusplus
}
#endif

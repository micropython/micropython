
#ifndef SYSTEM_WIRELESS_UPDATE_H_
#define SYSTEM_WIRELESS_UPDATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#if (PLATFORM_ID==88)

void Wireless_Update_Begin(uint32_t file_length, uint16_t chunk_size, uint32_t chunk_address, uint8_t file_store);
uint8_t Wireless_Update_Save_Chunk(uint8_t *data, uint16_t length);
void Wireless_Update_Finish(void);

#endif

#ifdef __cplusplus
}
#endif

void HAL_WLAN_notify_OTA_update_completed(void);

#endif /* SYSTEM_WIRELESS_UPDATE_H_ */


#include "hw_config.h"
#include "flash_device_hal.h"
#include "module_info.h"
#include "module_info_hal.h"

#ifdef __cplusplus
extern "C" {
#endif


/* MAL access layer for Internal/Serial Flash Routines */
//New routines specific for BM09/BM14 flash usage
uint16_t FLASH_SectorToWriteProtect(uint8_t flashDeviceID, uint32_t startAddress);
uint16_t FLASH_SectorToErase(flash_device_t flashDeviceID, uint32_t startAddress);
bool FLASH_CheckValidAddressRange(flash_device_t flashDeviceID, uint32_t startAddress, uint32_t length);
bool FLASH_WriteProtectMemory(flash_device_t flashDeviceID, uint32_t startAddress, uint32_t length, bool protect);
bool FLASH_EraseMemory(flash_device_t flashDeviceID, uint32_t startAddress, uint32_t length);

typedef bool (*copymem_fn_t)(flash_device_t sourceDeviceID, uint32_t sourceAddress,
                      flash_device_t destinationDeviceID, uint32_t destinationAddress,
                      uint32_t length, uint8_t module_function, uint8_t flags);


/**
 * Determines if the memory copy can be performed.
 */
bool FLASH_CheckCopyMemory(flash_device_t sourceDeviceID, uint32_t sourceAddress,
                      flash_device_t destinationDeviceID, uint32_t destinationAddress,
                      uint32_t length, uint8_t module_function, uint8_t flags);

/**
 * @param validateDestinationAddress checks if the destination address corresponds with the start address in the module
 */
bool FLASH_CopyMemory(flash_device_t sourceDeviceID, uint32_t sourceAddress,
                      flash_device_t destinationDeviceID, uint32_t destinationAddress,
                      uint32_t length, uint8_t module_function, uint8_t flags);

bool FLASH_CompareMemory(flash_device_t sourceDeviceID, uint32_t sourceAddress,
                         flash_device_t destinationDeviceID, uint32_t destinationAddress,
                         uint32_t length);

bool FLASH_AddToNextAvailableModulesSlot(flash_device_t sourceDeviceID, uint32_t sourceAddress,
                                         flash_device_t destinationDeviceID, uint32_t destinationAddress,
                                         uint32_t length, uint8_t module_function, uint8_t flags);

bool FLASH_AddToFactoryResetModuleSlot(flash_device_t sourceDeviceID, uint32_t sourceAddress,
                                       flash_device_t destinationDeviceID, uint32_t destinationAddress,
                                       uint32_t length, uint8_t module_function, uint8_t flags);

bool FLASH_IsFactoryResetAvailable(void);
bool FLASH_ClearFactoryResetModuleSlot(void);
bool FLASH_RestoreFromFactoryResetModuleSlot(void);
void FLASH_UpdateModules(void (*flashModulesCallback)(bool isUpdating));

const module_info_t* FLASH_ModuleInfo(uint8_t flashDeviceID, uint32_t startAddress);
uint32_t FLASH_ModuleAddress(flash_device_t flashDeviceID, uint32_t startAddress);
uint32_t FLASH_ModuleLength(flash_device_t flashDeviceID, uint32_t startAddress);
uint16_t FLASH_ModuleVersion(uint8_t flashDeviceID, uint32_t startAddress);
bool FLASH_isUserModuleInfoValid(uint8_t flashDeviceID, uint32_t startAddress, uint32_t expectedAddress);
bool FLASH_VerifyCRC32(flash_device_t flashDeviceID, uint32_t startAddress, uint32_t length);
bool FLASH_InvalidCRC32(flash_device_t flashDeviceID, uint32_t startAddress, uint32_t length);

//Old routines with same signature both for Core and Photon
void FLASH_ClearFlags(void);
void FLASH_Erase(void);
void FLASH_Backup(uint32_t FLASH_Address);
void FLASH_Restore(uint32_t FLASH_Address);
void FLASH_Begin(uint32_t FLASH_Address, uint32_t imageSize);
int FLASH_Update(const uint8_t *pBuffer, uint32_t address, uint32_t bufferSize);
void FLASH_End(void);


#ifdef __cplusplus
}
#endif

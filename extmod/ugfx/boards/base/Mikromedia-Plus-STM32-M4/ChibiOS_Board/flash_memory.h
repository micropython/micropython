void flash_sector_erase(uint32_t sector);
void flash_read(uint32_t address, size_t bytes, uint8_t *out);
void flash_write(uint32_t address, size_t bytes, const uint8_t *data);
bool flash_tp_calibrated(void);
void flash_tp_calibration_save(uint16_t instance, const uint8_t *calbuf, size_t sz);
const char *flash_tp_calibration_load(uint16_t instance);

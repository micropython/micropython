uint32_t flash_get_sector_info(uint32_t addr, uint32_t *start_addr, uint32_t *size);
void flash_write(uint32_t flash_dest, const uint32_t *src, uint32_t num_word32);

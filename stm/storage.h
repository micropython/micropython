void storage_init();
void storage_flush();
bool storage_read_block(uint8_t *dest, uint32_t block);
bool storage_write_block(const uint8_t *src, uint32_t block);

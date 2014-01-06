void storage_init(void);
uint32_t storage_get_block_size(void);
uint32_t storage_get_block_count(void);
MP_BOOL storage_needs_flush(void);
void storage_flush(void);
MP_BOOL storage_read_block(uint8_t *dest, uint32_t block);
MP_BOOL storage_write_block(const uint8_t *src, uint32_t block);

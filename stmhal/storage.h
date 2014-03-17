#define FLASH_BLOCK_SIZE (512)

void storage_init(void);
uint32_t storage_get_block_size(void);
uint32_t storage_get_block_count(void);
bool storage_needs_flush(void);
void storage_flush(void);
bool storage_read_block(uint8_t *dest, uint32_t block);
bool storage_write_block(const uint8_t *src, uint32_t block);

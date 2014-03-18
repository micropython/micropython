// this is a fixed size and should not be changed
#define SDCARD_BLOCK_SIZE (512)

void sdcard_init(void);
bool sdcard_is_present(void);
bool sdcard_power_on(void);
void sdcard_power_off(void);
uint64_t sdcard_get_capacity_in_bytes(void);
bool sdcard_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks);
bool sdcard_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks);

extern const struct _mp_obj_base_t pyb_sdcard_obj;

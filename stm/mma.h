void mma_init(void);
void mma_restart(uint8_t addr, int write);
void mma_start(uint8_t addr, int write);
void mma_send_byte(uint8_t data);
uint8_t mma_read_ack(void);
uint8_t mma_read_nack(void);
void mma_stop(void);

MP_DECLARE_CONST_FUN_OBJ(pyb_mma_read_obj);
MP_DECLARE_CONST_FUN_OBJ(pyb_mma_read_all_obj);
MP_DECLARE_CONST_FUN_OBJ(pyb_mma_write_mode_obj);

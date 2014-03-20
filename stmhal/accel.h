void accel_init(void);
void accel_restart(uint8_t addr, int write);
void accel_start(uint8_t addr, int write);
void accel_send_byte(uint8_t data);
uint8_t accel_read_ack(void);
uint8_t accel_read_nack(void);
void accel_stop(void);

MP_DECLARE_CONST_FUN_OBJ(pyb_accel_read_obj);
MP_DECLARE_CONST_FUN_OBJ(pyb_accel_read_all_obj);
MP_DECLARE_CONST_FUN_OBJ(pyb_accel_write_mode_obj);

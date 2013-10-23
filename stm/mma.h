void mma_init(void);
void mma_restart(uint8_t addr, int write);
void mma_start(uint8_t addr, int write);
void mma_send_byte(uint8_t data);
uint8_t mma_read_ack(void);
uint8_t mma_read_nack(void);
void mma_stop(void);

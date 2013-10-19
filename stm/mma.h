void mma_init();
void mma_restart(uint8_t addr, int write);
void mma_start(uint8_t addr, int write);
void mma_send_byte(uint8_t data);
uint8_t mma_read_ack();
uint8_t mma_read_nack();
void mma_stop();

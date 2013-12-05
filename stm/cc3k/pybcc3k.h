void pyb_delay_us(uint32_t us);

void pyb_cc3000_set_en(int val);
void pyb_cc3000_set_cs(int val);
int pyb_cc3000_get_irq(void);
void pyb_cc3000_enable_irq(void);
void pyb_cc3000_disable_irq(void);
void pyb_cc3000_pause_spi(void);
void pyb_cc3000_resume_spi(void);

void pyb_cc3000_spi_init(void);
uint8_t pyb_cc3000_spi_send(uint8_t val);

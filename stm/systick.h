extern volatile uint32_t sys_tick_counter;

void sys_tick_init(void);
void SysTick_Handler(void);
void sys_tick_delay_ms(uint32_t delay_ms);
void sys_tick_wait_at_least(uint32_t stc, uint32_t delay_ms);
MP_BOOL sys_tick_has_passed(uint32_t stc, uint32_t delay_ms);

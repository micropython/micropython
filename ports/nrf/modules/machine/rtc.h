extern uint64_t ticks_ms_64(void);

// This is the time in seconds since 2000 that the RTC was started.
__attribute__((section(".uninitialized"))) static uint32_t rtc_offset[3];

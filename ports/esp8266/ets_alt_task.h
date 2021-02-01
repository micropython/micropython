#ifndef MICROPY_INCLUDED_ESP8266_ETS_ALT_TASK_H
#define MICROPY_INCLUDED_ESP8266_ETS_ALT_TASK_H

extern int ets_loop_iter_disable;
extern int ets_loop_dont_feed_sw_wdt;
extern uint32_t system_time_low_word;
extern uint32_t system_time_high_word;

void system_time_update(void);
bool ets_loop_iter(void);

#endif // MICROPY_INCLUDED_ESP8266_ETS_ALT_TASK_H

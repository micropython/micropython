#ifndef MICROPY_INCLUDED_ESP8266_ETS_ALT_TASK_H
#define MICROPY_INCLUDED_ESP8266_ETS_ALT_TASK_H

extern int ets_loop_iter_disable;
extern uint32_t system_time_high_word;

bool ets_loop_iter(void);

#endif // MICROPY_INCLUDED_ESP8266_ETS_ALT_TASK_H

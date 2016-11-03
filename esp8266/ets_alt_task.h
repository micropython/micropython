#ifndef _INCLUDED_ETS_ALT_TASK_H_
#define _INCLUDED_ETS_ALT_TASK_H_

#include <stdbool.h>
#include <stdint.h>

extern int ets_loop_iter_disable;
extern uint32_t system_time_high_word;

bool ets_loop_iter(void);

#endif  // _INCLUDED_ETS_ALT_TASK_H_

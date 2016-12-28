#ifndef _TIMER_API_H
#define _TIMER_API_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*timer_callback_t)(void);

void* OSTimer_newTimer(unsigned int period, timer_callback_t callback);
void OSTimer_deleteTimer(void *timer);
void OSTimer_start(void *timer);
void OSTimer_startFromISR(void *timer);
void OSTimer_stop(void *timer);
void OSTimer_stopFromISR(void *timer);
void OSTimer_reset(void *timer);
void OSTimer_resetFromISR(void *timer);
void OSTimer_changePeriod(void *timer, unsigned int period);
void OSTimer_changePeriodFromISR(void *timer, unsigned int period);
bool OSTimer_isValid(void *timer);

#ifdef __cplusplus
}
#endif


#endif

#include "ticks_api.h"
#include "spark_wiring_timer.h"
#include "timer_api.h"

#ifdef __cplusplus
extern "C" {
#endif


void* OSTimer_newTimer(unsigned int period, timer_callback_t callback)
{
    Timer *_timer = new Timer(period, callback);
    return ((void *)_timer);
}

void OSTimer_deleteTimer(void *timer)
{
    delete ((Timer *)timer);
}

void OSTimer_start(void *timer)
{
	((Timer *)timer)->start();
}

void OSTimer_startFromISR(void *timer)
{
	((Timer *)timer)->startFromISR();
}

void OSTimer_stop(void *timer)
{
	((Timer *)timer)->stop();
}

void OSTimer_stopFromISR(void *timer)
{
	((Timer *)timer)->stopFromISR();
}

void OSTimer_reset(void *timer)
{
	((Timer *)timer)->reset();
}

void OSTimer_resetFromISR(void *timer)
{
	((Timer *)timer)->resetFromISR();
}

void OSTimer_changePeriod(void *timer, unsigned int period)
{
	((Timer *)timer)->changePeriod(period);
}

void OSTimer_changePeriodFromISR(void *timer, unsigned int period)
{
	((Timer *)timer)->changePeriodFromISR(period);
}

bool OSTimer_isValid(void *timer)
{
    return ((Timer *)timer)->isValid();
}


#ifdef __cplusplus
}
#endif

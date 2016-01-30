#ifndef __FREERTOS_PMU_H_
#define __FREERTOS_PMU_H_

#include "sleep_ex_api.h"

#define BIT(n)                   (1<<n)
// wakelock for system usage
#define WAKELOCK_OS              BIT(0)
#define WAKELOCK_WLAN            BIT(1)
#define WAKELOCK_LOGUART         BIT(2)
#define WAKELOCK_SDIO_DEVICE     BIT(3)

// wakelock for user defined
#define WAKELOCK_USER_BASE       BIT(16)

#if 0
#define DEFAULT_WAKELOCK         (0)
#else
// default locked by OS and not to sleep until OS release wakelock in somewhere
#define DEFAULT_WAKELOCK         (WAKELOCK_OS)
#endif

#define DEFAULT_WAKEUP_EVENT (SLEEP_WAKEUP_BY_STIMER | SLEEP_WAKEUP_BY_GTIMER | SLEEP_WAKEUP_BY_GPIO_INT | SLEEP_WAKEUP_BY_WLAN)

void acquire_wakelock(uint32_t lock_id);
void release_wakelock(uint32_t lock_id);
uint32_t get_wakelock_status();
#if (configGENERATE_RUN_TIME_STATS == 1)
void get_wakelock_hold_stats( char *pcWriteBuffer );
void clean_wakelock_stat();
#endif

void add_wakeup_event(uint32_t event);
void del_wakeup_event(uint32_t event);

void register_pre_sleep_callback( void (*pre_sleep_cb)(unsigned int) );
void register_post_sleep_callback( void (*post_sleep_cb)(unsigned int) );

#endif

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

typedef void (*freertos_sleep_callback)( unsigned int );

/** Acquire wakelock
 *
 *  A wakelock is a 32-bit map. Each module own 1 bit in this bit map.
 *  FreeRTOS tickless reference the wakelock and decide that if it can or cannot enter sleep state.
 *  If any module acquire and hold a bit in wakelock, then the whole system won't enter sleep state.
 *
 *  If wakelock is not equals to 0, then the system won't enter sleep.
 *
 *  @param lock_id        : The bit which is attempt to add into wakelock
 */
void acquire_wakelock(uint32_t lock_id);

/** Release wakelock
 *
 *  If wakelock equals to 0, then the system may enter sleep state if it is in idle state.
 *
 *  @param lock_id        : The bit which is attempt to remove from wakelock
 */
void release_wakelock(uint32_t lock_id);

/** Get current wakelock bit map value
 *
 *  @return               : the current wakelock bit map value
 */
uint32_t get_wakelock_status();

#if (configGENERATE_RUN_TIME_STATS == 1)

/** Get text report that contain the statics of wakelock holding time
 *
 *  Each time a module acquries or releases wakelock, a holding time is calculated and sum up to a table.
 *  It is for debug that which module is power saving killer.
 *
 *  @param pcWriteBuffer  : The char buffer that contain the report
 */
void get_wakelock_hold_stats( char *pcWriteBuffer );

/** Recalculate the wakelock statics
 *
 *  By default the wakelock statics is calculated from system boot up.
 *  If we want to debug power saving killer from a specified timestamp, we can reset the statics.
 */
void clean_wakelock_stat();

#endif

void add_wakeup_event(uint32_t event);
void del_wakeup_event(uint32_t event);

/** Register sleep callback
 *
 *  Pre-sleep callbacks are called before entering sleep.
 *  Post-sleep callbacks are called after resume.
 *
 *  @param is_pre_sleep   : Indicate the sleep_cb is for pre-sleep or post-sleep
 *  @param sleep_cb       : The callback function which is called before/after sleep
 *  @param module         : The callback is assigned according to the bit specify in bit field of param module
 *                          The bit 15 (0x00008000) is used for unspecified callback.
 */
void register_sleep_callback_by_module( unsigned char is_pre_sleep, freertos_sleep_callback sleep_cb, uint32_t module );

/** Register unspecified pre sleep callback
 *
 *  Pre-sleep callbacks are called before entering sleep.
 *
 *  @param pre_sleep_cb   : The callback function which is called before sleep
 *                          It is registed in bit 15 (0x00008000) of module list
 */
void register_pre_sleep_callback( freertos_sleep_callback pre_sleep_cb );

/** Register unspecified post sleep callback
 *
 *  Post-sleep callbacks are called before entering sleep.
 *
 *  @param post_sleep_cb  : The callback function which is called after sleep
 *                          It is registed in bit 15 (0x00008000) of module list
 */
void register_post_sleep_callback( freertos_sleep_callback post_sleep_cb );

/** Set PLL reserved or not when sleep is called
 *
 *  @param reserve: true for sleep with PLL reserve
 */
void set_pll_reserved(unsigned char reserve);

#endif

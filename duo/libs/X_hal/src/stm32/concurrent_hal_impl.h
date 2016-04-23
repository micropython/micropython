
#ifndef CONCURRENT_HAL_IMPL_H
#define	CONCURRENT_HAL_IMPL_H

#ifdef	__cplusplus
extern "C" {
#endif

// This code is used by HAL-clients which don't have access to the FreeRTOS sources
// so we cannot directly define __gthread_t as TaskHandle_t, however, we define it
// here and statically assert that it is the same size.

typedef void* __gthread_t;

typedef void* os_thread_t;
typedef int32_t os_result_t;
typedef uint8_t os_thread_prio_t;
/* Default priority is the same as the application thread */
const os_thread_prio_t OS_THREAD_PRIORITY_DEFAULT = 2;
const os_thread_prio_t OS_THREAD_PRIORITY_CRITICAL = 9;
const size_t OS_THREAD_STACK_SIZE_DEFAULT = 3*1024;

typedef void* os_mutex_t;
typedef void* os_mutex_recursive_t;
typedef void* condition_variable_t;
typedef void* os_timer_t;

typedef os_mutex_t gthread_mutex_t;
typedef os_mutex_recursive_t gthread_recursive_mutex_t;


/**
 * Alias for a queue handle in FreeRTOS - all handles are pointers.
 */
typedef void* os_queue_t;
typedef void* os_semaphore_t;

typedef struct timespec __gthread_time_t;

bool __gthread_equal(__gthread_t t1, __gthread_t t2);
__gthread_t __gthread_self();

typedef condition_variable_t __gthread_cond_t;

int __gthread_cond_timedwait (__gthread_cond_t *cond,
                                   gthread_mutex_t *mutex,
                                   const __gthread_time_t *abs_timeout);


int __gthread_mutex_timedlock (gthread_mutex_t* mutex, const __gthread_time_t* timeout);

#ifdef	__cplusplus
}
#endif

#endif	/* CONCURRENT_HAL_IMPL_H */


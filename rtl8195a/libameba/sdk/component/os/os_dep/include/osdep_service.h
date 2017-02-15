/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *                                        
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/
#ifndef __OSDEP_SERVICE_H_
#define __OSDEP_SERVICE_H_

/* OS dep feature enable */

#define CONFIG_LITTLE_ENDIAN

#if defined(CONFIG_PLATFORM_8195A) || defined(CONFIG_PLATFORM_8711B)
#define CONFIG_PLATFORM_AMEBA_X
#endif

#if defined(CONFIG_PLATFORM_8195A)
	#define CONFIG_USE_TCM_HEAP 1					/* USE TCM HEAP */
#endif

#if defined(CONFIG_PLATFORM_AMEBA_X)
	#define CONFIG_MEM_MONITOR	MEM_MONITOR_SIMPLE
#else
	#define CONFIG_MEM_MONITOR	MEM_MONITOR_LEAK
#endif

/* Define compilor specific symbol */
//
// inline function
//

#if defined ( __ICCARM__ )
#define __inline__                      inline
#define __inline                        inline
#define __inline_definition			//In dialect C99, inline means that a function's definition is provided 
								//only for inlining, and that there is another definition 
								//(without inline) somewhere else in the program. 
								//That means that this program is incomplete, because if 
								//add isn't inlined (for example, when compiling without optimization), 
								//then main will have an unresolved reference to that other definition.

								// Do not inline function is the function body is defined .c file and this 
								// function will be called somewhere else, otherwise there is compile error
#elif defined ( __CC_ARM   )
#define __inline__			__inline	//__linine__ is not supported in keil compilor, use __inline instead
#define inline				__inline
#define __inline_definition			// for dialect C99
#elif defined   (  __GNUC__  )
#define __inline__                      inline
#define __inline                        inline
#define __inline_definition	inline
#endif

#include <stdio.h>
#include "platform_autoconf.h"
#if defined( PLATFORM_FREERTOS)
#include "freertos_service.h"
#elif defined( PLATFORM_ECOS)
#include "ecos/ecos_service.h"
#endif

#define RTW_MAX_DELAY			0xFFFFFFFF
#define RTW_WAIT_FOREVER		0xFFFFFFFF

struct timer_list {
	_timerHandle 	timer_hdl;
	unsigned long	data;
	void (*function)(void *);
};

typedef thread_return (*thread_func_t)(thread_context context);
typedef void (*TIMER_FUN)(void *context);
typedef int (*event_handler_t)(char *buf, int buf_len, int flags, void *user_data);

#define CONFIG_THREAD_COMM_SEMA
struct task_struct {
	const char *task_name;
	_thread_hdl_ task; /* I: workqueue thread */

#ifdef CONFIG_THREAD_COMM_SIGNAL
	const char *name;		/* I: workqueue thread name */
	u32 queue_num; /* total signal num */
	u32 cur_queue_num; /* cur signal num should < queue_num */
#elif defined(CONFIG_THREAD_COMM_SEMA)
	_sema wakeup_sema;
	_sema terminate_sema;
//	_queue work_queue;  //TODO
#endif
	u32 blocked; 
	u32 callback_running;
};

typedef struct {
	_xqueue event_queue;
	struct task_struct thread;
}rtw_worker_thread_t;

typedef struct
{
	event_handler_t function;
	char *buf;
	int buf_len;
	int flags;
	void *user_data;
} rtw_event_message_t;

struct worker_timer_entry {
	struct list_head 	list;
	_timerHandle 		timer_hdl;
	rtw_event_message_t	message;
	rtw_worker_thread_t	*worker_thread;
	u32 				timeout; 
};
#ifdef CONFIG_THREAD_COMM_SIGNAL
struct work_struct;
typedef void (*work_func_t)(void *context);
struct work_struct {
	_list list;
	u32 data;
	work_func_t func;
	void *context;
	struct task_struct *used_wq; 
};

struct delayed_work {
	struct work_struct work;
	struct timer_list timer;
};
#endif

#ifdef CONFIG_MEM_MONITOR
//----- ------------------------------------------------------------------
// Memory Monitor
//----- ------------------------------------------------------------------
#define MEM_MONITOR_SIMPLE		0x1
#define MEM_MONITOR_LEAK		0x2

#define MEM_MONITOR_FLAG_WIFI_DRV	0x1
#define MEM_MONITOR_FLAG_WPAS		0x2
#if CONFIG_MEM_MONITOR & MEM_MONITOR_LEAK
struct mem_entry {
	struct list_head	list;
	int			size;
	void			*ptr;
};
#endif

void init_mem_monitor(_list *pmem_table, int *used_num);
void deinit_mem_monitor(_list *pmem_table, int *used_num);
void add_mem_usage(_list *pmem_table, void *ptr, int size, int *used_num, int flag);
void del_mem_usage(_list *pmem_table, void *ptr, int *used_num, int flag);
int get_mem_usage(_list *pmem_table);
#endif

/*********************************** OSDEP API *****************************************/
u8*	_rtw_vmalloc(u32 sz);
u8*	_rtw_zvmalloc(u32 sz);
void	_rtw_vmfree(u8 *pbuf, u32 sz);
u8*	_rtw_zmalloc(u32 sz);
u8*	_rtw_malloc(u32 sz);
void	_rtw_mfree(u8 *pbuf, u32 sz);
#ifdef CONFIG_MEM_MONITOR
u8*	rtw_vmalloc(u32 sz);
u8*	rtw_zvmalloc(u32 sz);
void	rtw_vmfree(u8 *pbuf, u32 sz);
u8*	rtw_zmalloc(u32 sz);
u8*	rtw_malloc(u32 sz);
void	rtw_mfree(u8 *pbuf, u32 sz);
#else
#define	rtw_vmalloc		_rtw_vmalloc
#define	rtw_zvmalloc		_rtw_zvmalloc
#define	rtw_vmfree  		_rtw_vmfree
#define	rtw_zmalloc 		_rtw_zmalloc
#define	rtw_malloc  		_rtw_malloc
#define	rtw_mfree   		_rtw_mfree
#endif
#define rtw_free(buf)		rtw_mfree((u8 *)buf, 0)
void*	rtw_malloc2d(int h, int w, int size);
void	rtw_mfree2d(void *pbuf, int h, int w, int size);
void	rtw_memcpy(void* dst, void* src, u32 sz);
int	rtw_memcmp(void *dst, void *src, u32 sz);
void	rtw_memset(void *pbuf, int c, u32 sz);

void	rtw_init_listhead(_list *list);
u32	rtw_is_list_empty(_list *phead);
void	rtw_list_insert_head(_list *plist, _list *phead);
void	rtw_list_insert_tail(_list *plist, _list *phead);
void	rtw_list_delete(_list *plist);

void	rtw_init_sema(_sema *sema, int init_val);
void	rtw_free_sema(_sema	*sema);
void	rtw_up_sema(_sema	*sema);
void	rtw_up_sema_from_isr(_sema	*sema);
u32	rtw_down_sema(_sema *sema);
u32	rtw_down_timeout_sema(_sema *sema, u32 timeout);
void	rtw_mutex_init(_mutex *pmutex);
void	rtw_mutex_free(_mutex *pmutex);
void	rtw_mutex_put(_mutex *pmutex);
void	rtw_mutex_get(_mutex *pmutex);
void	rtw_enter_critical(_lock *plock, _irqL *pirqL);
void	rtw_exit_critical(_lock *plock, _irqL *pirqL);
void	rtw_enter_critical_bh(_lock *plock, _irqL *pirqL);
void	rtw_exit_critical_bh(_lock *plock, _irqL *pirqL);
int	rtw_enter_critical_mutex(_mutex *pmutex, _irqL *pirqL);
void	rtw_exit_critical_mutex(_mutex *pmutex, _irqL *pirqL);
void	rtw_spinlock_init(_lock *plock);
void	rtw_spinlock_free(_lock *plock);
void	rtw_spinlock_init(_lock *plock);
void	rtw_spinlock_free(_lock *plock);
void	rtw_spin_lock(_lock *plock);
void	rtw_spin_unlock(_lock *plock);
void	rtw_spinlock_irqsave(_lock *plock, _irqL *irqL);
void	rtw_spinunlock_irqsave(_lock *plock, _irqL *irqL);

int rtw_init_xqueue( _xqueue* queue, const char* name, u32 message_size, u32 number_of_messages );
int rtw_push_to_xqueue( _xqueue* queue, void* message, u32 timeout_ms );
int rtw_pop_from_xqueue( _xqueue* queue, void* message, u32 timeout_ms );
int rtw_deinit_xqueue( _xqueue* queue );

void	rtw_init_queue(_queue	*pqueue);
void	rtw_deinit_queue(_queue	*pqueue);
u32	rtw_is_queue_empty(_queue *pqueue);
u32	rtw_queue_empty(_queue	*pqueue);
u32	rtw_end_of_queue_search(_list *queue, _list *pelement);
_list* rtw_get_queue_head(_queue	*queue);

u32	rtw_get_current_time(void);
u32	rtw_systime_to_ms(u32 systime);
u32 rtw_systime_to_sec(u32 systime);
u32	rtw_ms_to_systime(u32 ms);
u32	rtw_sec_to_systime(u32 sec);
s32	rtw_get_passing_time_ms(u32 start);
s32	rtw_get_time_interval_ms(u32 start, u32 end);

void	rtw_msleep_os(int ms);
void	rtw_usleep_os(int us);
u32 	rtw_atoi(u8* s);
void	rtw_mdelay_os(int ms);
void	rtw_udelay_os(int us);
void	rtw_yield_os(void);

void	rtw_init_timer(_timer *ptimer, void *adapter, TIMER_FUN pfunc,void* cntx, const char *name);
void	rtw_set_timer(_timer *ptimer,u32 delay_time);
u8		rtw_cancel_timer(_timer *ptimer);
void	rtw_del_timer(_timer *ptimer);

//Atomic integer operations
void 	ATOMIC_SET(ATOMIC_T *v, int i);
int		ATOMIC_READ(ATOMIC_T *v);
void 	ATOMIC_ADD(ATOMIC_T *v, int i);
void 	ATOMIC_SUB(ATOMIC_T *v, int i);
void 	ATOMIC_INC(ATOMIC_T *v);
void 	ATOMIC_DEC(ATOMIC_T *v);
int 	ATOMIC_ADD_RETURN(ATOMIC_T *v, int i);
int 	ATOMIC_SUB_RETURN(ATOMIC_T *v, int i);
int 	ATOMIC_INC_RETURN(ATOMIC_T *v);
int 	ATOMIC_DEC_RETURN(ATOMIC_T *v);
int ATOMIC_DEC_AND_TEST(ATOMIC_T *v);

u64	rtw_modular64(u64 x, u64 y);
int	rtw_get_random_bytes(void* dst, u32 size);
u32	rtw_getFreeHeapSize(void);
void	flush_signals_thread(void);

void	rtw_acquire_wakelock(void);
void	rtw_release_wakelock(void);

/*********************************** Thread related *****************************************/
int	rtw_create_task(struct task_struct *task, const char *name, u32  stack_size, u32 priority, thread_func_t func, void *thctx);
void rtw_delete_task(struct task_struct * task);
void	rtw_wakeup_task(struct task_struct *task);
int rtw_create_worker_thread( rtw_worker_thread_t* worker_thread, u8 priority, u32 stack_size, u32 event_queue_size );
int rtw_delete_worker_thread( rtw_worker_thread_t* worker_thread );

#if 0 //TODO
void	rtw_init_delayed_work(struct delayed_work *dwork, work_func_t func, const char *name);
void	rtw_deinit_delayed_work(struct delayed_work *dwork);
int		rtw_queue_delayed_work(struct workqueue_struct *wq, struct delayed_work *dwork, u32 delay, void* context);
BOOLEAN rtw_cancel_delayed_work(struct delayed_work *dwork);
#endif

void	rtw_thread_enter(char *name);
void	rtw_thread_exit(void);
#ifdef PLATFORM_LINUX
#define rtw_warn_on(condition) WARN_ON(condition)
#else
#define rtw_warn_on(condition) do {} while (0)
#endif

/*********************************** Timer related *****************************************/
_timerHandle rtw_timerCreate( const signed char *pcTimerName, 
							  osdepTickType xTimerPeriodInTicks, 
							  u32 uxAutoReload, 
							  void * pvTimerID, 
							  TIMER_FUN pxCallbackFunction );
u32 rtw_timerDelete( _timerHandle xTimer, 
							   osdepTickType xBlockTime );
u32 rtw_timerIsTimerActive( _timerHandle xTimer );
u32 rtw_timerStop( _timerHandle xTimer, 
							   osdepTickType xBlockTime );
u32 rtw_timerChangePeriod( _timerHandle xTimer, 
							   osdepTickType xNewPeriod, 
							   osdepTickType xBlockTime );

/*********************************** OSDEP API end *****************************************/
#define LIST_CONTAINOR(ptr, type, member) \
	((type *)((char *)(ptr)-(SIZE_T)((char *)&((type *)ptr)->member - (char *)ptr)))

#define time_after(a,b)	((long)(b) - (long)(a) < 0)
#define time_before(a,b)	time_after(b,a)
#define time_after_eq(a,b)	((long)(a) - (long)(b) >= 0)
#define time_before_eq(a,b)	time_after_eq(b,a)
	
#define _RND(sz, r) ((((sz)+((r)-1))/(r))*(r))
#define RND4(x)	(((x >> 2) + (((x & 3) == 0) ?  0: 1)) << 2)

__inline static u32 _RND4(u32 sz)
{
	u32	val;

	val = ((sz >> 2) + ((sz & 3) ? 1: 0)) << 2;
	
	return val;
}

__inline static u32 _RND8(u32 sz)
{
	u32	val;

	val = ((sz >> 3) + ((sz & 7) ? 1: 0)) << 3;
	
	return val;
}

__inline static u32 _RND128(u32 sz)
{
	u32	val;

	val = ((sz >> 7) + ((sz & 127) ? 1: 0)) << 7;
	
	return val;
}

__inline static u32 _RND256(u32 sz)
{
	u32	val;

	val = ((sz >> 8) + ((sz & 255) ? 1: 0)) << 8;
	
	return val;
}

__inline static u32 _RND512(u32 sz)
{
	u32	val;

	val = ((sz >> 9) + ((sz & 511) ? 1: 0)) << 9;
	
	return val;
}

__inline static u32 bitshift(u32 bitmask)
{
	u32 i;

	for (i = 0; i <= 31; i++)
		if (((bitmask>>i) &  0x1) == 1) break;

	return i;
}

/* Macros for handling unaligned memory accesses */

#define RTW_GET_BE16(a) ((u16) (((a)[0] << 8) | (a)[1]))
#define RTW_PUT_BE16(a, val)			\
	do {					\
		(a)[0] = ((u16) (val)) >> 8;	\
		(a)[1] = ((u16) (val)) & 0xff;	\
	} while (0)

#define RTW_GET_LE16(a) ((u16) (((a)[1] << 8) | (a)[0]))
#define RTW_PUT_LE16(a, val)			\
	do {					\
		(a)[1] = ((u16) (val)) >> 8;	\
		(a)[0] = ((u16) (val)) & 0xff;	\
	} while (0)

#define RTW_GET_BE24(a) ((((u32) (a)[0]) << 16) | (((u32) (a)[1]) << 8) | \
			 ((u32) (a)[2]))			 
#define RTW_PUT_BE24(a, val)					\
	do {							\
		(a)[0] = (u8) ((((u32) (val)) >> 16) & 0xff);	\
		(a)[1] = (u8) ((((u32) (val)) >> 8) & 0xff);	\
		(a)[2] = (u8) (((u32) (val)) & 0xff);		\
	} while (0)

#define RTW_GET_BE32(a) ((((u32) (a)[0]) << 24) | (((u32) (a)[1]) << 16) | \
			 (((u32) (a)[2]) << 8) | ((u32) (a)[3]))			 
#define RTW_PUT_BE32(a, val)					\
	do {							\
		(a)[0] = (u8) ((((u32) (val)) >> 24) & 0xff);	\
		(a)[1] = (u8) ((((u32) (val)) >> 16) & 0xff);	\
		(a)[2] = (u8) ((((u32) (val)) >> 8) & 0xff);	\
		(a)[3] = (u8) (((u32) (val)) & 0xff);		\
	} while (0)

#define RTW_GET_LE32(a) ((((u32) (a)[3]) << 24) | (((u32) (a)[2]) << 16) | \
			 (((u32) (a)[1]) << 8) | ((u32) (a)[0]))			 
#define RTW_PUT_LE32(a, val)					\
	do {							\
		(a)[3] = (u8) ((((u32) (val)) >> 24) & 0xff);	\
		(a)[2] = (u8) ((((u32) (val)) >> 16) & 0xff);	\
		(a)[1] = (u8) ((((u32) (val)) >> 8) & 0xff);	\
		(a)[0] = (u8) (((u32) (val)) & 0xff);		\
	} while (0)

#define RTW_GET_BE64(a) ((((u64) (a)[0]) << 56) | (((u64) (a)[1]) << 48) | \
			 (((u64) (a)[2]) << 40) | (((u64) (a)[3]) << 32) | \
			 (((u64) (a)[4]) << 24) | (((u64) (a)[5]) << 16) | \
			 (((u64) (a)[6]) << 8) | ((u64) (a)[7]))			 
#define RTW_PUT_BE64(a, val)				\
	do {						\
		(a)[0] = (u8) (((u64) (val)) >> 56);	\
		(a)[1] = (u8) (((u64) (val)) >> 48);	\
		(a)[2] = (u8) (((u64) (val)) >> 40);	\
		(a)[3] = (u8) (((u64) (val)) >> 32);	\
		(a)[4] = (u8) (((u64) (val)) >> 24);	\
		(a)[5] = (u8) (((u64) (val)) >> 16);	\
		(a)[6] = (u8) (((u64) (val)) >> 8);	\
		(a)[7] = (u8) (((u64) (val)) & 0xff);	\
	} while (0)

#define RTW_GET_LE64(a) ((((u64) (a)[7]) << 56) | (((u64) (a)[6]) << 48) | \
			 (((u64) (a)[5]) << 40) | (((u64) (a)[4]) << 32) | \
			 (((u64) (a)[3]) << 24) | (((u64) (a)[2]) << 16) | \
			 (((u64) (a)[1]) << 8) | ((u64) (a)[0]))

struct osdep_service_ops {
	u8* (*rtw_vmalloc)(u32 sz);
	u8* (*rtw_zvmalloc)(u32 sz);
	void (*rtw_vmfree)(u8 *pbuf, u32 sz);
	u8* (*rtw_malloc)(u32 sz);
	u8* (*rtw_zmalloc)(u32 sz);
	void (*rtw_mfree)(u8 *pbuf, u32 sz);
	void (*rtw_memcpy)(void* dst, void* src, u32 sz);
	int (*rtw_memcmp)(void *dst, void *src, u32 sz);
	void (*rtw_memset)(void *pbuf, int c, u32 sz);
	void (*rtw_init_sema)(_sema *sema, int init_val);
	void (*rtw_free_sema)(_sema *sema);
	void (*rtw_up_sema)(_sema *sema);
	void (*rtw_up_sema_from_isr)(_sema *sema);
	u32 (*rtw_down_timeout_sema)(_sema *sema, u32 timeout);
	void (*rtw_mutex_init)(_mutex *pmutex);
	void (*rtw_mutex_free)(_mutex *pmutex);
	void (*rtw_mutex_get)(_mutex *pmutex);
	void (*rtw_mutex_put)(_mutex *pmutex);
	void (*rtw_enter_critical)(_lock *plock, _irqL *pirqL);
	void (*rtw_exit_critical)(_lock *plock, _irqL *pirqL);
	void (*rtw_enter_critical_bh)(_lock *plock, _irqL *pirqL);
	void (*rtw_exit_critical_bh)(_lock *plock, _irqL *pirqL);
	int (*rtw_enter_critical_mutex)(_mutex *pmutex, _irqL *pirqL);
	void (*rtw_exit_critical_mutex)(_mutex *pmutex, _irqL *pirqL);
	void (*rtw_spinlock_init)(_lock *plock);
	void (*rtw_spinlock_free)(_lock *plock);
	void (*rtw_spin_lock)(_lock *plock);
	void (*rtw_spin_unlock)(_lock *plock);
	void (*rtw_spinlock_irqsave)(_lock *plock, _irqL *irqL);
	void (*rtw_spinunlock_irqsave)(_lock *plock, _irqL *irqL);
	int (*rtw_init_xqueue)( _xqueue* queue, const char* name, u32 message_size, u32 number_of_messages );
	int (*rtw_push_to_xqueue)( _xqueue* queue, void* message, u32 timeout_ms );
	int (*rtw_pop_from_xqueue)( _xqueue* queue, void* message, u32 timeout_ms );
	int (*rtw_deinit_xqueue)( _xqueue* queue );
	u32	(*rtw_get_current_time)(void);
	u32 (*rtw_systime_to_ms)(u32 systime);
	u32 (*rtw_systime_to_sec)(u32 systime);
	u32 (*rtw_ms_to_systime)(u32 ms);
	u32	(*rtw_sec_to_systime)(u32 sec);
	void (*rtw_msleep_os)(int ms);
	void (*rtw_usleep_os)(int us);
	void (*rtw_mdelay_os)(int ms);
	void (*rtw_udelay_os)(int us);
	void (*rtw_yield_os)(void);
	void (*rtw_init_timer)(_timer *ptimer, void *adapter, TIMER_FUN pfunc,void* cntx, const char *name);
	void (*rtw_set_timer)(_timer *ptimer,u32 delay_time);
	u8 (*rtw_cancel_timer)(_timer *ptimer);
	void (*rtw_del_timer)(_timer *ptimer);
	void (*ATOMIC_SET)(ATOMIC_T *v, int i);
	int (*ATOMIC_READ)(ATOMIC_T *v);
	void (*ATOMIC_ADD)(ATOMIC_T *v, int i);
	void (*ATOMIC_SUB)(ATOMIC_T *v, int i);
	void (*ATOMIC_INC)(ATOMIC_T *v);
	void (*ATOMIC_DEC)(ATOMIC_T *v);
	int (*ATOMIC_ADD_RETURN)(ATOMIC_T *v, int i);
	int (*ATOMIC_SUB_RETURN)(ATOMIC_T *v, int i);
	int (*ATOMIC_INC_RETURN)(ATOMIC_T *v);
	int (*ATOMIC_DEC_RETURN)(ATOMIC_T *v);
	u64 (*rtw_modular64)(u64 x, u64 y);
	int (*rtw_get_random_bytes)(void* dst, u32 size);
	u32 (*rtw_getFreeHeapSize)(void);
	int (*rtw_create_task)(struct task_struct *task, const char *name, u32 stack_size, u32 priority, thread_func_t func, void *thctx);
	void (*rtw_delete_task)(struct task_struct *task);
	void (*rtw_wakeup_task)(struct task_struct *task);
	
#if 0	//TODO
	void (*rtw_init_delayed_work)(struct delayed_work *dwork, work_func_t func, const char *name);
	void (*rtw_deinit_delayed_work)(struct delayed_work *dwork);
	int (*rtw_queue_delayed_work)(struct workqueue_struct *wq, struct delayed_work *dwork, unsigned long delay, void* context);
	BOOLEAN (*rtw_cancel_delayed_work)(struct delayed_work *dwork);
#endif	
	void (*rtw_thread_enter)(char *name);
	void (*rtw_thread_exit)(void);
	_timerHandle (*rtw_timerCreate)( const signed char *pcTimerName, 
							  osdepTickType xTimerPeriodInTicks, 
							  u32 uxAutoReload, 
							  void * pvTimerID, 
							  TIMER_FUN pxCallbackFunction );
	u32 (*rtw_timerDelete)( _timerHandle xTimer, 
							   osdepTickType xBlockTime );
	u32 (*rtw_timerIsTimerActive)( _timerHandle xTimer );
	u32 (*rtw_timerStop)( _timerHandle xTimer, 
							   osdepTickType xBlockTime );
	u32 (*rtw_timerChangePeriod)( _timerHandle xTimer, 
							   osdepTickType xNewPeriod, 
							   osdepTickType xBlockTime );

	void (*rtw_acquire_wakelock)(void);
	void (*rtw_release_wakelock)(void);
};
/*********************************** OSDEP API end *****************************************/


#endif	//#ifndef __OSDEP_SERVICE_H_

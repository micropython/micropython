/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_OS_RAWRTOS

#include <string.h>
#include "raw_api.h"
#include "raw_config.h"

#if CONFIG_RAW_MUTEX != 1
	#error "GOS: CONFIG_RAW_MUTEX must be defined in raw_config.h"
#endif

#if CONFIG_RAW_SEMAPHORE != 1
	#error "GOS: CONFIG_RAW_SEMAPHORE must be defined in raw_config.h"
#endif


void _gosInit(void)
{
	#if !GFX_OS_NO_INIT
		#error "GOS: Operating System initialization for RawRTOS is not yet implemented in uGFX. Please set GFX_OS_NO_INIT to TRUE in your gfxconf.h"
	#endif
	#if !GFX_OS_INIT_NO_WARNING
		#warning "GOS: Operating System initialization has been turned off. Make sure you call raw_os_start() before gfxInit() in your application!"
	#endif
}

void _gosDeinit(void)
{
}


void gfxSleepMilliseconds(delaytime_t ms)
{
	systemticks_t ticks = ms*RAW_TICKS_PER_SECOND/1000;
	if(!ticks)ticks = 1;
	raw_sleep(ticks);
}

void gfxSleepMicroseconds(delaytime_t us)
{
	systemticks_t ticks = (us/1000)*RAW_TICKS_PER_SECOND/1000;
	if(!ticks)ticks = 1;
	raw_sleep(ticks);
}

bool_t gfxSemWait(gfxSem* psem, delaytime_t ms)
{
	systemticks_t ticks = ms*RAW_TICKS_PER_SECOND/1000;
	if(!ticks)ticks=1;
	if(raw_semaphore_get((psem), ticks)==RAW_SUCCESS)
		return TRUE;
	return FALSE;
}

bool_t gfxSemWaitI(gfxSem* psem)
{
	if(raw_semaphore_get((psem), TIME_IMMEDIATE)==RAW_SUCCESS)
		return TRUE;
	return FALSE;
}

gfxThreadHandle gfxThreadCreate(void *stackarea, size_t stacksz, threadpriority_t prio, DECLARE_THREAD_FUNCTION((*fn),p), void *param)
{
	RAW_U16 ret;
	gfxThreadHandle taskobj;

	taskobj = gfxAlloc(sizeof(RAW_TASK_OBJ));
	ret = raw_task_create(taskobj, (RAW_U8  *)"uGFX_TASK", param,
	                         prio, 0,  stackarea, 
	                         stacksz/sizeof(PORT_STACK) ,  fn, 1); 

	if (ret != RAW_SUCCESS) {
		for (;;);
	}

	return (taskobj);
}


#endif




#include <FreeRTOS.h>
#include <task.h>

#include "Drivers/interrupts.h"

extern int main(void);

static void mainTask(void *pvParameters) {
	(void)	pvParameters;
	main();
}

/**
 *	This is the systems main entry, some call it a boot thread.
 **/
int FreeRTOS_Main(void) {

	DisableInterrupts();
	InitInterruptController();

   xTaskCreate(mainTask,
			  (portCHAR *)"Main Task",
			  128,
			  NULL, 
			  0, 
			  NULL);

	vTaskStartScheduler();

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1) {
		;
	}
	
	return 0;
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time task stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook	function is 
	called if a task stack overflow is detected.  Note the system/interrupt
	stack is not checked. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()). */
}

#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"
#include "main.h"

extern void wlan_netowrk(void);
extern void console_init(void);
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
	console_init();
#if defined(CONFIG_WIFI_NORMAL) && defined(CONFIG_NETWORK)
	wlan_network();
#endif


    //3 3)Enable Schedule, Start Kernel
#if defined(CONFIG_KERNEL) && !TASK_SCHEDULER_DISABLED
	#ifdef PLATFORM_FREERTOS
	vTaskStartScheduler();
	#endif
#else
	RtlConsolTaskRom(NULL);
#endif
}

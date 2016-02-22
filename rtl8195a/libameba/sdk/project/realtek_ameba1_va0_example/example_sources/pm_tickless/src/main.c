#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"
#include "main.h"
#include <example_entry.h>

#include "freertos_pmu.h"
#include "gpio_irq_api.h"
#include "serial_api.h"

// select uart tx/rx pin with gpio interrupt function
#define UART_TX    PA_4
#define UART_RX    PA_0

#define LOGUART_RX_WAKE PC_1

#define WAKELOCK_EXAMPLE WAKELOCK_USER_BASE

serial_t mysobj;
volatile char rc = 0;

extern void wlan_netowrk(void);
extern void console_init(void);

char cmdbuf[128];
int cmdbuf_index = 0;
void uart_irq_callback(uint32_t id, SerialIrq event)
{
	serial_t *sobj = (void*)id;

	if(event == RxIrq) {
		acquire_wakelock(WAKELOCK_EXAMPLE);

		rc = serial_getc(sobj);

		if (rc == '\r' || rc == '\n') {
			serial_putc(sobj, '\r');
			serial_putc(sobj, '\n');
			serial_putc(sobj, '#');
			serial_putc(sobj, ' ');

			if (cmdbuf_index != 0) {

                /* NOTICE: If you don't want loss any data from treating UART signal as GPIO interrupt,
                 *         you can set FREERTOS_PMU_TICKLESS_PLL_RESERVED to 1 in "platform_opt.h".
                 *         It will reserved PLL clock in tickless and UART can receive the whole data.
                 *         But it also cost more power consumption.
                 **/

				// process command
				printf("cmd(%d): %s\r\n", cmdbuf_index, cmdbuf);

				// release wakelock and reset buf
				cmdbuf_index = 0;
				release_wakelock(WAKELOCK_EXAMPLE);
			}
		}

		if (!(rc == '\r' || rc == '\n' )) {
			// receive command
			serial_putc(sobj, rc);
			cmdbuf[cmdbuf_index] = rc;
			cmdbuf_index++;
			cmdbuf[cmdbuf_index] = '\0';
		}
	}
}

void gpio_uart_rx_irq_callback(uint32_t id, gpio_irq_event event)
{
	acquire_wakelock(WAKELOCK_EXAMPLE);
}

void pre_sleep_process_callback(unsigned int expected_idle_time)
{
	// For peripherals that need turned off before sleep, call disable or deinit peripheral here
}

void post_sleep_process_callback(unsigned int expected_idle_time)
{
	// For peripherals that are turned off before sleep, call enable or init peripheral here
}

void config_uart()
{
	// setup uart
	serial_init(&mysobj, UART_TX, UART_RX);
	serial_baud(&mysobj, 38400);
	serial_format(&mysobj, 8, ParityNone, 1);

	serial_irq_handler(&mysobj, uart_irq_callback, (uint32_t)&mysobj);
	serial_irq_set(&mysobj, RxIrq, 1);
	serial_irq_set(&mysobj, TxIrq, 1);

	// config uart rx as gpio wakeup pin
	gpio_irq_t gpio_rx_wake;
	gpio_irq_init(&gpio_rx_wake, UART_RX, gpio_uart_rx_irq_callback, NULL);
	gpio_irq_set(&gpio_rx_wake, IRQ_FALL, 1);   // Falling Edge Trigger
	gpio_irq_enable(&gpio_rx_wake);
}

void gpio_loguart_rx_irq_callback (uint32_t id, gpio_irq_event event)
{
	/*  WAKELOCK_LOGUART is also handled in log service.
	 *  It is release after a complete command is sent.
	 **/
	acquire_wakelock(WAKELOCK_LOGUART);
}

void config_loguart()
{
	/*  Log uart RX pin doesn't support gpio interrupt.
	 *  To make log uart wake system, we can parallel log uart RX with another gpio interrupt pin.
	 */
	gpio_irq_t gpio_rx_wake;
	gpio_irq_init(&gpio_rx_wake, LOGUART_RX_WAKE, gpio_loguart_rx_irq_callback, NULL);
	gpio_irq_set(&gpio_rx_wake, IRQ_FALL, 1);   // Falling Edge Trigger
	gpio_irq_enable(&gpio_rx_wake);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
	if ( rtl_cryptoEngine_init() != 0 ) {
		DiagPrintf("crypto engine init failed\r\n");
	}

	/* Initialize log uart and at command service */
	console_init();	

	/* pre-processor of application example */
	pre_example_entry();

	/* wlan intialization */
#if defined(CONFIG_WIFI_NORMAL) && defined(CONFIG_NETWORK)
	wlan_network();
#endif

	// setup uart with capability of wakeup system
	config_uart();

	// setup log uart with capability of wakeup system
	config_loguart();

	// By default tickless is disabled because WAKELOCK_OS is locked.
	// Release this wakelock to enable tickless
	release_wakelock(WAKELOCK_OS);

	// Register pre/post sleep callback. They are called when system automatically enter/leave sleep.
	register_pre_sleep_callback(pre_sleep_process_callback);
	register_post_sleep_callback(post_sleep_process_callback);

	/* Execute application example */
	example_entry();

    	/*Enable Schedule, Start Kernel*/
#if defined(CONFIG_KERNEL) && !TASK_SCHEDULER_DISABLED
	#ifdef PLATFORM_FREERTOS
	vTaskStartScheduler();
	#endif
#else
	RtlConsolTaskRom(NULL);
#endif
}

#include <stdio.h>
#include "py/mphal.h"
#include "led.h"


static uint32_t debug_buf[6] __attribute__((aligned(4))) = {
	0xdddddddd, 0x88888888, 0, 0, 0, 0};

void QiMing_F103RC_board_early_init(void)
{

	// GPIO 配置模块存在问题
	// GPIOA->CRH = 0x888444B4;
	// 串口波特率配置有问题
	// USART1->BRR = 0x139; // 波特率115200

	// 存储系统RCC相关配置到缓冲区
	debug_buf[0] = 0xddddddddU;
	debug_buf[1] = 0x88888888U;
	debug_buf[2] = GPIOA->CRH;    // 0x88844484
	debug_buf[3] = GPIOA->CRL;    // 0x44444434, 
	debug_buf[4] = GPIOB->CRL;    //
	debug_buf[5] = GPIOB->CRL;    //

	// 未执行USART1初始化之前 PA9, PA10
	// GOIOA->CRH = 0x88844444
	// GPIOA->CRL = 0x44444434

	// 执行USART1 Pin初始化后, PA9, PA10
	// GOIOA->CRH = 0x88844484
	// GPIOA->CRL = 0x44444434

	led_state(1, 0);
	led_state(2, 1);
	char str[128] = {0};

	snprintf(str, 128, "%lx, %lx, %lx, %lx, %lx, %lx", debug_buf[0], debug_buf[1], debug_buf[2], debug_buf[3], debug_buf[4], debug_buf[5]);

	while (1)
	{

		led_toggle(2);
		for (uint8_t i = 0; i < 10; i++)
		{
			led_toggle(1);
			mp_hal_delay_ms(100);
		}

		mp_hal_stdout_tx_str(str);
	}
}

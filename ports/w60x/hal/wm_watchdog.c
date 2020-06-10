/**
 * @file    wm_watchdog.c
 *
 * @brief   watchdog Driver Module
 *
 * @author  kevin
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#include "wm_debug.h"
#include "wm_regs.h"
#include "wm_irq.h"
#include "wm_cpu.h"
#include "wm_watchdog.h"

static u32 wdg_jumpclear_flag = 0; /*0:donot jump clear, 1: jump clear, 2:close wdg*/
// void WDG_IRQHandler(void)
// {
// 	printf("WDG IRQ\n");
// }

/**
 * @brief          This function is used to clear watchdog irq in case watchdog reset
 *
 * @param          None
 *
 * @return         None
 *
 * @note           None
 */
void tls_watchdog_clr(void)
{
	if (0 == wdg_jumpclear_flag)
	{
	    tls_reg_write32(HR_WDG_INT_CLR, 0x01);
	}
}

/**
 * @brief          This function is used to init watchdog
 *
 * @param[in]      usec    microseconds
 *
 * @return         None
 *
 * @note           None
 */
void tls_watchdog_init(u32 usec)
{

	tls_sys_clk sysclk;

	tls_sys_clk_get(&sysclk);
	tls_irq_enable(WATCHDOG_INT);
	
	tls_reg_write32(HR_WDG_LOAD_VALUE, sysclk.apbclk * usec); 		/* 40M dominant frequency: 40 * 10^6 * (usec / 10^6) */
	tls_reg_write32(HR_WDG_CTRL, 0x3);             /* enable irq & reset */
}


/**
 * @brief          This function is used to deinit watchdog
 *
 * @param[in]     None
 *
 * @return         None
 *
 * @note           None
 */
void tls_watchdog_deinit(void)
{
	tls_irq_disable(WATCHDOG_INT);
	tls_reg_write32(HR_WDG_CTRL, 0);	
	tls_reg_write32(HR_WDG_INT_CLR, 0x01);
}

/**
 * @brief          This function is used to start calculating elapsed time. 
 *
 * @param[in]      None
 *
 * @return         elapsed time, unit:millisecond
 *
 * @note           None
 */
void tls_watchdog_start_cal_elapsed_time(void)
{
	if (tls_reg_read32(HR_WDG_CTRL)&0x3)
	{
		wdg_jumpclear_flag = 1;
		tls_reg_write32(HR_WDG_INT_CLR, 0x01);
	}
	else
	{
		wdg_jumpclear_flag = 2;
		tls_watchdog_init(5*1000*1000);
	}
}

/**
 * @brief          This function is used to stop calculating & return elapsed time. 
 *
 * @param[in]     none
 *
 * @return         elapsed time, unit:millisecond
 *
 * @note           None
 */
u32 tls_watchdog_stop_cal_elapsed_time(void)
{
#define RT_TIME_BASE (40)
	u32 val = 0;

	switch (wdg_jumpclear_flag)
	{
		case 1:
		{
			val = (tls_reg_read32(HR_WDG_LOAD_VALUE) - tls_reg_read32(HR_WDG_CUR_VALUE))/RT_TIME_BASE;
			wdg_jumpclear_flag = 0;
		}
		break;
		
		case 2:
		{
			val = (tls_reg_read32(HR_WDG_LOAD_VALUE) - tls_reg_read32(HR_WDG_CUR_VALUE))/RT_TIME_BASE;
			wdg_jumpclear_flag = 0;
			tls_watchdog_deinit();
		}
		break;

		default:
			wdg_jumpclear_flag = 0;
			break;
	}

	return val;	
}

/**
 * @brief          This function is used to reset system
 *
 * @param          None
 *
 * @return         None
 *
 * @note           None
 */
void tls_sys_reset(void)
{
	tls_reg_write32(HR_WDG_LOCK, 0x1ACCE551);
	tls_reg_write32(HR_WDG_LOAD_VALUE, 0x100);
	tls_reg_write32(HR_WDG_CTRL, 0x3);
	tls_reg_write32(HR_WDG_LOCK, 1);
}


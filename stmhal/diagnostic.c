/*
 * diagnostic.c
 *
 *  Created on: 05.11.2015
 *      Author: badi
 */
#include <stdint.h>
#include "stm32f429xx.h"

static uint16_t pendIrq[DMA2D_IRQn+1];

void getPendingIrq(void)
{
	uint16_t pendNr=0;
	for (uint16_t i=0;i<=DMA2D_IRQn;i++)
	{
		if (1 ==  NVIC_GetPendingIRQ(i))
		{
			pendIrq[pendNr++] = i;
		}
	}

}


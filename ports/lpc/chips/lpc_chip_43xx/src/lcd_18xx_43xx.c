/*
 * @brief LPC18xx/43xx LCD chip driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "chip.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

static LCD_CURSOR_SIZE_OPT_T LCD_Cursor_Size = LCD_CURSOR_64x64;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize the LCD controller */
void Chip_LCD_Init(LPC_LCD_T *pLCD, LCD_CONFIG_T *LCD_ConfigStruct)
{
	uint32_t i, regValue, *pPal;
	uint32_t pcd;

	/* Enable LCD Clock */
	Chip_Clock_EnableOpts(CLK_MX_LCD, true, true, 1);

	/* disable the display */
	pLCD->CTRL &= ~CLCDC_LCDCTRL_ENABLE;

	/* Setting LCD_TIMH register */
	regValue = ( ((((LCD_ConfigStruct->PPL / 16) - 1) & 0x3F) << 2)
				 |         (( (LCD_ConfigStruct->HSW - 1)    & 0xFF) << 8)
				 |         (( (LCD_ConfigStruct->HFP - 1)    & 0xFF) << 16)
				 |         (( (LCD_ConfigStruct->HBP - 1)    & 0xFF) << 24) );
	pLCD->TIMH = regValue;

	/* Setting LCD_TIMV register */
	regValue = ((((LCD_ConfigStruct->LPP - 1) & 0x3FF) << 0)
				|        (((LCD_ConfigStruct->VSW - 1) & 0x03F) << 10)
				|        (((LCD_ConfigStruct->VFP - 1) & 0x0FF) << 16)
				|        (((LCD_ConfigStruct->VBP - 1) & 0x0FF) << 24) );
	pLCD->TIMV = regValue;

	/* Generate the clock and signal polarity control word */
	regValue = 0;
	regValue = (((LCD_ConfigStruct->ACB - 1) & 0x1F) << 6);
	regValue |= (LCD_ConfigStruct->IOE & 1) << 14;
	regValue |= (LCD_ConfigStruct->IPC & 1) << 13;
	regValue |= (LCD_ConfigStruct->IHS & 1) << 12;
	regValue |= (LCD_ConfigStruct->IVS & 1) << 11;

	/* Compute clocks per line based on panel type */
	switch (LCD_ConfigStruct->LCD) {
	case LCD_MONO_4:
		regValue |= ((((LCD_ConfigStruct->PPL / 4) - 1) & 0x3FF) << 16);
		break;

	case LCD_MONO_8:
		regValue |= ((((LCD_ConfigStruct->PPL / 8) - 1) & 0x3FF) << 16);
		break;

	case LCD_CSTN:
		regValue |= (((((LCD_ConfigStruct->PPL * 3) / 8) - 1) & 0x3FF) << 16);
		break;

	case LCD_TFT:
	default:
		regValue |=	 /*1<<26 |*/ (((LCD_ConfigStruct->PPL - 1) & 0x3FF) << 16);
	}

	/* panel clock divisor */
	pcd = 5;// LCD_ConfigStruct->pcd;   // TODO: should be calculated from LCDDCLK
	pcd &= 0x3FF;
	regValue |=  ((pcd >> 5) << 27) | ((pcd) & 0x1F);
	pLCD->POL = regValue;

	/* disable interrupts */
	pLCD->INTMSK = 0;

	/* set bits per pixel */
	regValue = LCD_ConfigStruct->BPP << 1;

	/* set color format RGB */
	regValue |= LCD_ConfigStruct->color_format << 8;
	regValue |= LCD_ConfigStruct->LCD << 4;
	if (LCD_ConfigStruct->Dual == 1) {
		regValue |= 1 << 7;
	}
	pLCD->CTRL = regValue;

	/* clear palette */
	pPal = (uint32_t *) (&(pLCD->PAL));
	for (i = 0; i < 128; i++) {
		*pPal = 0;
		pPal++;
	}
}

/* Shutdown the LCD controller */
void Chip_LCD_DeInit(LPC_LCD_T *pLCD)
{
	Chip_Clock_Disable(CLK_MX_LCD);
}

/* Configure Cursor */
void Chip_LCD_Cursor_Config(LPC_LCD_T *pLCD, LCD_CURSOR_SIZE_OPT_T cursor_size, bool sync)
{
	LCD_Cursor_Size = cursor_size;
	pLCD->CRSR_CFG = ((sync ? 1 : 0) << 1) | cursor_size;
}

/* Write Cursor Image into Internal Cursor Image Buffer */
void Chip_LCD_Cursor_WriteImage(LPC_LCD_T *pLCD, uint8_t cursor_num, void *Image)
{
	int i, j;
	uint32_t *fifoptr, *crsr_ptr = (uint32_t *) Image;

	/* Check if Cursor Size was configured as 32x32 or 64x64*/
	if (LCD_Cursor_Size == LCD_CURSOR_32x32) {
		i = cursor_num * 64;
		j = i + 64;
	}
	else {
		i = 0;
		j = 256;
	}
	fifoptr = (void *) &(pLCD->CRSR_IMG[0]);

	/* Copy Cursor Image content to FIFO */
	for (; i < j; i++) {

		*fifoptr = *crsr_ptr;
		crsr_ptr++;
		fifoptr++;
	}
}

/* Load LCD Palette */
void Chip_LCD_LoadPalette(LPC_LCD_T *pLCD, void *palette)
{
	LCD_PALETTE_ENTRY_T pal_entry = {0}; 
	uint8_t i, *pal_ptr;
	/* This function supports loading of the color palette from
	   the C file generated by the bmp2c utility. It expects the
	   palette to be passed as an array of 32-bit BGR entries having
	   the following format:
	   2:0 - Not used
	   7:3 - Blue
	   10:8 - Not used
	   15:11 - Green
	   18:16 - Not used
	   23:19 - Red
	   31:24 - Not used
	   arg = pointer to input palette table address */
	pal_ptr = (uint8_t *) palette;

	/* 256 entry in the palette table */
	for (i = 0; i < 256 / 2; i++) {
		pal_entry.Bl = (*pal_ptr++) >> 3;	/* blue first */
		pal_entry.Gl = (*pal_ptr++) >> 3;	/* get green */
		pal_entry.Rl = (*pal_ptr++) >> 3;	/* get red */
		pal_ptr++;	/* skip over the unused byte */
		/* do the most significant halfword of the palette */
		pal_entry.Bu = (*pal_ptr++) >> 3;	/* blue first */
		pal_entry.Gu = (*pal_ptr++) >> 3;	/* get green */
		pal_entry.Ru = (*pal_ptr++) >> 3;	/* get red */
		pal_ptr++;	/* skip over the unused byte */

		pLCD->PAL[i] = *((uint32_t *)&pal_entry);
	}
}








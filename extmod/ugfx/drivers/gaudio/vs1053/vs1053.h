/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _VS1053_H
#define _VS1053_H

// Registers
#define SCI_MODE			0x00
	#define SM_DIFF				0x0001
	#define SM_LAYER12			0x0002
	#define SM_RESET			0x0004
	#define SM_CANCEL			0x0008
	#define SM_EARSPEAKER_LO	0x0010
	#define SM_TESTS			0x0020
	#define SM_STREAM			0x0040
	#define SM_EARSPEAKER_HI	0x0080
	#define SM_DACT				0x0100
	#define SM_SDIORD			0x0200
	#define SM_SDISHARE			0x0400
	#define SM_SDINEW			0x0800
	#define SM_ADPCM			0x1000
	#define SM_LINE1			0x4000
	#define SM_CLK_RANGE		0x8000
	#define SCI_MODE_DEFAULTS	(SM_LINE1|SM_SDINEW)
#define SCI_STATUS			0x01
	#define SS_DO_NOT_JUMP		0x8000
	#define SS_SWING_MASK		0x7000
	#define SS_SWING_SHIFT		12
	#define SS_VCM_OVERLOAD		0x0800
	#define SS_VCM_DISABLE		0x0400
	#define SS_VER_MASK			0x00F0
	#define SS_VER_SHIFT		4
	#define SS_APDOWN2			0x0008
	#define SS_APDOWN1			0x0004
	#define SS_SS_AD_CLOCK		0x0002
	#define SS_REFERENCE_SEL	0x0001
#define SCI_BASS			0x02
	#define ST_AMPLITUDE_MASK	0xF000
	#define ST_AMPLITUDE_SHIFT	12
	#define ST_FREQLIMIT_MASK	0x0F00
	#define ST_FREQLIMIT_SHIFT	8
	#define SB_AMPLITUDE_MASK	0x00F0
	#define SB_AMPLITUDE_SHIFT	4
	#define SB_FREQLIMIT_MASK	0x000F
	#define SB_FREQLIMIT_SHIFT	0
#define SCI_CLOCKF			0x03
	#define SC_MULT_1			0x0000
	#define SC_MULT_2			0x2000
	#define SC_MULT_2_5			0x4000
	#define SC_MULT_3			0x6000
	#define SC_MULT_3_5			0x8000
	#define SC_MULT_4			0xa000
	#define SC_MULT_4_5			0xc000
	#define SC_MULT_5			0xe000
	#define SC_ADD_NONE			0x0000
	#define SC_ADD_1			0x0800
	#define SC_ADD_1_5			0x1000
	#define SC_ADD_2			0x1800
	#define SC_FREQ_MASK		0x07FF
#define SCI_DECODE_TIME		0x04
#define SCI_AUDATA			0x05
	#define SR_RATE_MASK		0xFFFE
	#define SR_IS_STEREO		0x0001
#define SCI_WRAM			0x06
#define SCI_WRAMADDR		0x07
	#define WRAMADDR_XDATA		0x1800
	#define WRAMADDR_YDATA		0x5800
	#define WRAMADDR_INSTR		0x8040
	#define WRAMADDR_IO			0xC000
	#define WRAMADDR_EXTRAPARAMS	0x1E02
#define SCI_HDAT0			0x08
#define SCI_HDAT1			0x09
	#define HD_FMT_NONE			0x0000
	#define HD_FMT_WAV			0x7665
	#define HD_FMT_AAC_ADTS		0x4154
	#define HD_FMT_AAC_ADIF		0x4144
	#define HD_FMT_AAC_M4A		0x4D34
	#define HD_FMT_WMA			0x574D
	#define HD_FMT_MIDI			0x4D54
	#define HD_FMT_OGG			0x4F67
	#define HD_FMT_MP3_MIN		0xFFE0
	#define HD_FMT_MP3_MAX		0xFFFF
#define SCI_AIADDR			0x0A
#define SCI_VOL				0x0B
	#define VOL_LEFT_MASK		0xFF00
	#define VOL_LEFT_SHIFT		8
	#define VOL_RIGHT_MASK		0x00FF
	#define VOL_RIGHT_SHIFT		0
	#define VOL_POWERDOWN		0xFFFF
	#define VOL_MAX				0x0000
	#define VOL_MIN				0xFEFE
#define SCI_AICTRL0			0x0C
#define SCI_AICTRL1			0x0D
#define SCI_AICTRL2			0x0E
#define SCI_AICTRL3			0x0F

#endif /* _VS1053_H */

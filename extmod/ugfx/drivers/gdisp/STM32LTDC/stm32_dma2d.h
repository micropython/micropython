#ifndef _STM32_DMA2D_H
#define _STM32_DMA2D_H

#define OPFCCR_ARGB8888		0x00
#define OPFCCR_RGB888		0x01
#define OPFCCR_RGB565		0x02
#define OPFCCR_ARGB1555		0x03
#define OPFCCR_ARGB4444		0x04

#define FGPFCCR_CM_ARGB8888	0x00
#define FGPFCCR_CM_RGB888	0x01
#define FGPFCCR_CM_RGB565	0x02

#define DMA2D_CR_MODE_R2M	((uint32_t)0x00030000)	/* Register-to-memory mode */
#define DMA2D_CR_MODE_M2M	((uint32_t)0x00000000)	/* Register-to-memory mode */

static void dma2d_init(void);

#endif /* _STM32_DMA2D_H */

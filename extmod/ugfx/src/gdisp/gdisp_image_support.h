/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gdisp/gdisp_image_support.h
 * @brief   GDISP image support routines header file.
 *
 * @defgroup Image Image
 * @ingroup GDISP
 * @{
 */

#ifndef _GDISP_IMAGE_SUPPORT_H
#define _GDISP_IMAGE_SUPPORT_H

/* Base endian handling routines */
#define gdispImageGetVar(type, p, idx)				(*(type *)(((uint8_t *)(p))+(idx)))
#define gdispImageGetByte(type, p, idx, shift)		(((type)gdispImageGetVar(uint8_t, p, idx))<<(shift))
#define gdispImageSwap16(w)							((((uint16_t)(w))>>8)|(((uint16_t)(w))<<8))
#define gdispImageSwap32(dw)						((((uint32_t)(w))>>24)|((((uint32_t)(w))&0x00FF0000)>>8)\
													 |((((uint32_t)(w))&0x0000FF00)<<8)|(((uint32_t)(w))<<24))
#define gdispImageSwapWords32(dw)					((((uint32_t)(w))>>16)|(((uint32_t)(w))<<16))
#define gdispImageSwapBytes32(dw)					(((((uint32_t)(w))&0xFF000000)>>8)|((((uint32_t)(w))&0x00FF0000)<<8)\
													 |((((uint32_t)(w))&0x0000FF00)>>8)|(((uint32_t)(w))<<8))

/*
 * Get a uint16_t/uint32_t from memory in the required endianness.
 *	There is no alignment requirement.
 */
#if GFX_CPU_ENDIAN == GFX_CPU_ENDIAN_LITTLE && GFX_CPU_NO_ALIGNMENT_FAULTS
	#define gdispImageGetLE16(p, idx)		gdispImageGetVar(uint16_t, (p), (idx))
	#define gdispImageGetLE32(p, idx)		gdispImageGetVar(uint32_t, (p), (idx))
#else
	#define gdispImageGetLE16(p, idx)		( gdispImageGetByte(uint16_t, (p), (idx)  ,  0) | gdispImageGetByte(uint16_t, (p), (idx)+1,  8))
	#define gdispImageGetLE32(p, idx)		( gdispImageGetByte(uint32_t, (p), (idx)  ,  0) | gdispImageGetByte(uint32_t, (p), (idx)+1,  8)\
										 	 |gdispImageGetByte(uint32_t, (p), (idx)+2, 16) | gdispImageGetByte(uint32_t, (p), (idx)+3, 24))
#endif
#if GFX_CPU_ENDIAN == GFX_CPU_ENDIAN_BIG && GFX_CPU_NO_ALIGNMENT_FAULTS
	#define gdispImageGetBE16(p, idx)		gdispImageGetVar(uint16_t, (p), (idx))
	#define gdispImageGetBE32(p, idx)		gdispImageGetVar(uint32_t, (p), (idx))
#else
	#define gdispImageGetBE16(p, idx)		( gdispImageGetByte(uint16_t, (p), (idx)  ,  8) | gdispImageGetByte(uint16_t, (p), (idx)+1,  0))
	#define gdispImageGetBE32(p, idx)		( gdispImageGetByte(uint32_t, (p), (idx)  , 24) | gdispImageGetByte(uint32_t, (p), (idx)+1, 16)\
										 	 |gdispImageGetByte(uint32_t, (p), (idx)+2,  8) | gdispImageGetByte(uint32_t, (p), (idx)+3,  0))
#endif

/*
 * Get a uint16_t/uint32_t from memory in the required endianness.
 *	These are optimised routines but the memory must be word/dword aligned.
 */
#if GFX_CPU_ENDIAN == GFX_CPU_ENDIAN_LITTLE
	#define gdispImageGetAlignedLE16(p, idx)		gdispImageGetVar(uint16_t, (p), (idx))
	#define gdispImageGetAlignedBE16(p, idx)		gdispImageGetBE16(p, (idx))
	#define gdispImageGetAlignedLE32(p, idx)		gdispImageGetVar(uint32_t, (p), (idx))
	#define gdispImageGetAlignedBE32(p, idx)		gdispImageGetBE32(p, (idx))
#elif GFX_CPU_ENDIAN == GFX_CPU_ENDIAN_BIG
	#define gdispImageGetAlignedLE16(p, idx)		gdispImageGetLE16(p, (idx))
	#define gdispImageGetAlignedBE16(p, idx)		gdispImageGetVar(uint16_t, (p), (idx))
	#define gdispImageGetAlignedLE32(p, idx)		gdispImageGetLE32(p, (idx))
	#define gdispImageGetAlignedBE32(p, idx)		gdispImageGetVar(uint32_t, (p), (idx))
#else
	#define gdispImageGetAlignedLE16(p, idx)		gdispImageGetLE16(p, (idx))
	#define gdispImageGetAlignedBE16(p, idx)		gdispImageGetBE16(p, (idx))
	#define gdispImageGetAlignedLE32(p, idx)		gdispImageGetLE32(p, (idx))
	#define gdispImageGetAlignedBE32(p, idx)		gdispImageGetBE32(p, (idx))
#endif

/*
 * Change a uint16 or uint32 already in a register to the required endianness.
 */
#if GFX_CPU_ENDIAN == GFX_CPU_ENDIAN_LITTLE
	#define gdispImageH16toLE16(w)			(w)
	#define gdispImageH16toBE16(w)			gdispImageSwap16(w)
	#define gdispImageH32toLE32(dw)			(dw)
	#define gdispImageH32toBE32(dw)			gdispImageSwap32(dw)
	#define gdispImageMakeLE16(w)
	#define gdispImageMakeBE16(w)			{ w = gdispImageH16toBE16(w); }
	#define gdispImageMakeLE32(dw)
	#define gdispImageMakeBE32(dw)			{ dw = gdispImageH32toBE32(dw); }
#elif GFX_CPU_ENDIAN == GFX_CPU_ENDIAN_BIG
	#define gdispImageH16toLE16(w)			gdispImageSwap16(w)
	#define gdispImageH16toBE16(w)			(w)
	#define gdispImageH32toLE32(dw)			gdispImageSwap32(dw)
	#define gdispImageH32toBE32(dw)			(dw)
	#define gdispImageMakeLE16(w)			{ w = gdispImageH16toLE16(w); }
	#define gdispImageMakeBE16(w)
	#define gdispImageMakeLE32(dw)			{ dw = gdispImageH32toLE32(dw); }
	#define gdispImageMakeBE32(dw)
#elif GFX_CPU_ENDIAN == GFX_CPU_ENDIAN_WBDWL
	#define gdispImageH16toLE16(w)			gdispImageSwap16(w)
	#define gdispImageH16toBE16(w)			(w)
	#define gdispImageH32toLE32(dw)			gdispImageSwapBytes32(dw)
	#define gdispImageH32toBE32(dw)			gdispImageSwapWords32(dw)
	#define gdispImageMakeLE16(w)			{ w = gdispImageH16toLE16(w); }
	#define gdispImageMakeBE16(w)
	#define gdispImageMakeLE32(dw)			{ dw = gdispImageH32toLE32(dw); }
	#define gdispImageMakeBE32(dw)			{ dw = gdispImageH32toBE32(dw); }
#elif GFX_CPU_ENDIAN == GFX_CPU_ENDIAN_WLDWB
	#define gdispImageH16toLE16(w)			(w)
	#define gdispImageH16toBE16(w)			gdispImageSwap16(w)
	#define gdispImageH32toLE32(dw)			gdispImageSwapWords32(dw)
	#define gdispImageH32toBE32(dw)			gdispImageSwapBytes32(dw)
	#define gdispImageMakeLE16(w)
	#define gdispImageMakeBE16(w)			{ w = gdispImageH16toBE16(w); }
	#define gdispImageMakeLE32(dw)			{ dw = gdispImageH32toLE32(dw); }
	#define gdispImageMakeBE32(dw)			{ dw = gdispImageH32toBE32(dw); }
#else
	uint16_t gdispImageH16toLE16(uint16_t w);
	uint16_t gdispImageH16toBE16(uint16_t w);
	uint32_t gdispImageH32toLE32(uint32_t dw);
	uint32_t gdispImageH32toBE32(uint32_t dw);
	#define gdispImageMakeLE16(w)			{ w = gdispImageH16toLE16(w); }
	#define gdispImageMakeBE16(w)			{ w = gdispImageH16toBE16(w); }
	#define gdispImageMakeLE32(dw)			{ dw = gdispImageH32toLE32(dw); }
	#define gdispImageMakeBE32(dw)			{ dw = gdispImageH32toBE32(dw); }
#endif


	
#ifdef __cplusplus
extern "C" {
#endif

	void *gdispImageAlloc(gdispImage *img, size_t sz);
	void gdispImageFree(gdispImage *img, void *ptr, size_t sz);

	#if GFX_CPU_ENDIAN == GFX_CPU_ENDIAN_UNKNOWN
		extern const uint8_t gdispImageEndianArray[4];
	#endif

#ifdef __cplusplus
}
#endif

#endif /* _GDISP_IMAGE_SUPPORT_H */
/** @} */


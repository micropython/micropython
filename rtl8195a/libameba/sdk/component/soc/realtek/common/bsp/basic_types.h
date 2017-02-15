/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *                                        
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/
#ifndef __BASIC_TYPES_H__
#define __BASIC_TYPES_H__

//#define PLATFORM_FREERTOS
#include <stdint.h>

#define PLATFORM_LITTLE_ENDIAN                  0
#define PLATFORM_BIG_ENDIAN                     1

#define SYSTEM_ENDIAN                           PLATFORM_LITTLE_ENDIAN

#define SUCCESS	0
#define FAIL	(-1)

#undef _SUCCESS
#define _SUCCESS	1

#undef _FAIL
#define _FAIL		0

#ifndef FALSE		
    #define FALSE   0
#endif

#ifndef TRUE
    #define TRUE    (!FALSE)
#endif
		
#define _TRUE        TRUE	
#define _FALSE	     FALSE	

#ifndef NULL
#define NULL 0
#endif

#ifdef __GNUC__
#define __weak                  __attribute__((weak))
#define likely(x)               __builtin_expect ((x), 1)
#define unlikely(x)             __builtin_expect ((x), 0)
#endif

typedef unsigned int	        uint;
typedef	signed int		        sint;

#ifdef __ICCARM__
typedef signed long long        __int64_t;
typedef unsigned long long      __uint64_t;
#endif

#define s8                      int8_t
#define u8                      uint8_t
#define s16                     int16_t
#define u16                     uint16_t
#define s32                     int32_t
#define u32                     uint32_t
#define s64                     int64_t
#define u64                     uint64_t

#ifdef CONFIG_MBED_ENABLED
typedef unsigned int            BOOL;
#else
#ifndef BOOL
typedef unsigned char           BOOL;
#endif
#ifndef bool
#ifndef __cplusplus
typedef unsigned char           bool;
#endif
#endif
#endif

#define UCHAR                   uint8_t
#define USHORT                  uint16_t
#define UINT                    uint32_t
#define ULONG                   uint32_t	

typedef struct { volatile int counter; } atomic_t;

typedef enum _RTK_STATUS_ {
    _EXIT_SUCCESS = 0,
    _EXIT_FAILURE = 1
}RTK_STATUS, *PRTK_STATUS;

#define IN
#define OUT
#define VOID void
#define INOUT
#define NDIS_OID uint
#define NDIS_STATUS uint

#ifndef	PVOID
typedef void * PVOID;
#endif

typedef     u32 dma_addr_t;

typedef     void (*proc_t)(void*);

typedef     unsigned int __kernel_size_t;
typedef     int __kernel_ssize_t;

typedef 	__kernel_size_t	SIZE_T;	
typedef	    __kernel_ssize_t	SSIZE_T;
#define     FIELD_OFFSET(s,field)	((SSIZE_T)&((s*)(0))->field)

#define MEM_ALIGNMENT_OFFSET	(sizeof (SIZE_T))
#define MEM_ALIGNMENT_PADDING	(sizeof(SIZE_T) - 1)

#define SIZE_PTR SIZE_T
#define SSIZE_PTR SSIZE_T

#ifndef ON
#define   ON        1
#endif

#ifndef OFF
#define   OFF       0
#endif

#ifndef ENABLE
#define   ENABLE    1
#endif

#ifndef DISABLE
#define   DISABLE   0
#endif


#define BIT0	0x0001
#define BIT1	0x0002
#define BIT2	0x0004
#define BIT3	0x0008
#define BIT4	0x0010
#define BIT5	0x0020
#define BIT6	0x0040
#define BIT7	0x0080
#define BIT8	0x0100
#define BIT9	0x0200
#define BIT10	0x0400
#define BIT11	0x0800
#define BIT12	0x1000
#define BIT13	0x2000
#define BIT14	0x4000
#define BIT15	0x8000
#define BIT16	0x00010000
#define BIT17	0x00020000
#define BIT18	0x00040000
#define BIT19	0x00080000
#define BIT20	0x00100000
#define BIT21	0x00200000
#define BIT22	0x00400000
#define BIT23	0x00800000
#define BIT24	0x01000000
#define BIT25	0x02000000
#define BIT26	0x04000000
#define BIT27	0x08000000
#define BIT28	0x10000000
#define BIT29	0x20000000
#define BIT30	0x40000000
#define BIT31	0x80000000

#define BIT_(__n)       (1<<(__n))

#ifndef BIT
#define BIT(__n)       (1<<(__n))
#endif

#if defined (__ICCARM__)
#define STRINGIFY(s) #s
#define SECTION(_name) _Pragma( STRINGIFY(location=_name))
#define ALIGNMTO(_bound) _Pragma( STRINGIFY(data_alignment=##_bound##))
#define _PACKED_       __packed
#define _LONG_CALL_
#define _LONG_CALL_ROM_
#define _WEAK          __weak
#else
#define SECTION(_name) __attribute__ ((__section__(_name)))
#define ALIGNMTO(_bound) __attribute__ ((aligned (_bound)))
#define _PACKED_       __attribute__ ((packed))
#define _LONG_CALL_     __attribute__ ((long_call))
#define _LONG_CALL_ROM_     _LONG_CALL_
#define _WEAK           __attribute__ ((weak))
#endif



//port from fw by thomas
// TODO: Belows are Sync from SD7-Driver. It is necessary to check correctness

#define SWAP32(x) ((u32)(                         \
 			(((u32)(x) & (u32)0x000000ff) << 24) |            \
 			(((u32)(x) & (u32)0x0000ff00) <<  8) |            \
 			(((u32)(x) & (u32)0x00ff0000) >>  8) |            \
 			(((u32)(x) & (u32)0xff000000) >> 24)))
 
#define WAP16(x) ((u16)(                         \
 			(((u16)(x) & (u16)0x00ff) <<  8) |            \
 			(((u16)(x) & (u16)0xff00) >>  8)))

#if SYSTEM_ENDIAN == PLATFORM_LITTLE_ENDIAN
#ifndef rtk_le16_to_cpu
#define rtk_cpu_to_le32(x)		((u32)(x))
#define rtk_le32_to_cpu(x)		((u32)(x))
#define rtk_cpu_to_le16(x)		((u16)(x))
#define rtk_le16_to_cpu(x)		((u16)(x))
#define rtk_cpu_to_be32(x)		SWAP32((x))
#define rtk_be32_to_cpu(x)		SWAP32((x))
#define rtk_cpu_to_be16(x)		WAP16((x))
#define rtk_be16_to_cpu(x)		WAP16((x))
#endif

#elif SYSTEM_ENDIAN == PLATFORM_BIG_ENDIAN
#ifndef rtk_le16_to_cpu
#define rtk_cpu_to_le32(x)		SWAP32((x))
#define rtk_le32_to_cpu(x)		SWAP32((x))
#define rtk_cpu_to_le16(x)		WAP16((x))
#define rtk_le16_to_cpu(x)		WAP16((x))
#define rtk_cpu_to_be32(x)		((__u32)(x))
#define rtk_be32_to_cpu(x)		((__u32)(x))
#define rtk_cpu_to_be16(x)		((__u16)(x))
#define rtk_be16_to_cpu(x)		((__u16)(x))
#endif
#endif


/*
 *	Call endian free function when
 *		1. Read/write packet content.
 *		2. Before write integer to IO.
 *		3. After read integer from IO.
*/

//
// Byte Swapping routine.
//
#define EF1Byte	(u8)
#define EF2Byte 	le16_to_cpu
#define EF4Byte	le32_to_cpu

//
// Read LE format data from memory
//
#define ReadEF1Byte(_ptr)		EF1Byte(*((u8 *)(_ptr)))
#define ReadEF2Byte(_ptr)		EF2Byte(*((u16 *)(_ptr)))
#define ReadEF4Byte(_ptr)		EF4Byte(*((u32 *)(_ptr)))

//
// Write LE data to memory
//
#define WriteEF1Byte(_ptr, _val)	(*((u8 *)(_ptr)))=EF1Byte(_val)
#define WriteEF2Byte(_ptr, _val)	(*((u16 *)(_ptr)))=EF2Byte(_val)
#define WriteEF4Byte(_ptr, _val)	(*((u32 *)(_ptr)))=EF4Byte(_val)									

//
//	Example:
//		BIT_LEN_MASK_32(0) => 0x00000000
//		BIT_LEN_MASK_32(1) => 0x00000001
//		BIT_LEN_MASK_32(2) => 0x00000003
//		BIT_LEN_MASK_32(32) => 0xFFFFFFFF
//
#define BIT_LEN_MASK_32(__BitLen) \
	(0xFFFFFFFF >> (32 - (__BitLen)))
//
//	Example:
//		BIT_OFFSET_LEN_MASK_32(0, 2) => 0x00000003
//		BIT_OFFSET_LEN_MASK_32(16, 2) => 0x00030000
//
#define BIT_OFFSET_LEN_MASK_32(__BitOffset, __BitLen) \
	(BIT_LEN_MASK_32(__BitLen) << (__BitOffset)) 

//
//	Description:
//		Return 4-byte value in host byte ordering from
//		4-byte pointer in litten-endian system.
//
#define LE_P4BYTE_TO_HOST_4BYTE(__pStart) \
	(EF4Byte(*((u32 *)(__pStart))))

//
//	Description:
//		Translate subfield (continuous bits in little-endian) of 4-byte value in litten byte to
//		4-byte value in host byte ordering.
//
#define LE_BITS_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
	( \
		( LE_P4BYTE_TO_HOST_4BYTE(__pStart) >> (__BitOffset) ) \
		& \
		BIT_LEN_MASK_32(__BitLen) \
	)

//
//	Description:
//		Mask subfield (continuous bits in little-endian) of 4-byte value in litten byte oredering  
//		and return the result in 4-byte value in host byte ordering.
//
#define LE_BITS_CLEARED_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
	( \
		LE_P4BYTE_TO_HOST_4BYTE(__pStart) \
		& \
		( ~ BIT_OFFSET_LEN_MASK_32(__BitOffset, __BitLen) ) \
	)

//
//	Description:
//		Set subfield of little-endian 4-byte value to specified value.	
//
#define SET_BITS_TO_LE_4BYTE(__pStart, __BitOffset, __BitLen, __Value) \
	*((u32 *)(__pStart)) = \
		EF4Byte( \
			LE_BITS_CLEARED_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
			| \
			( (((u32)__Value) & BIT_LEN_MASK_32(__BitLen)) << (__BitOffset) ) \
		);

		
#define BIT_LEN_MASK_16(__BitLen) \
		(0xFFFF >> (16 - (__BitLen)))
		
#define BIT_OFFSET_LEN_MASK_16(__BitOffset, __BitLen) \
	(BIT_LEN_MASK_16(__BitLen) << (__BitOffset))
	
#define LE_P2BYTE_TO_HOST_2BYTE(__pStart) \
	(EF2Byte(*((u16 *)(__pStart))))
	
#define LE_BITS_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
	( \
		( LE_P2BYTE_TO_HOST_2BYTE(__pStart) >> (__BitOffset) ) \
		& \
		BIT_LEN_MASK_16(__BitLen) \
	)
	
#define LE_BITS_CLEARED_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
	( \
		LE_P2BYTE_TO_HOST_2BYTE(__pStart) \
		& \
		( ~ BIT_OFFSET_LEN_MASK_16(__BitOffset, __BitLen) ) \
	)

#define SET_BITS_TO_LE_2BYTE(__pStart, __BitOffset, __BitLen, __Value) \
	*((u16 *)(__pStart)) = \
		EF2Byte( \
			LE_BITS_CLEARED_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
			| \
			( (((u16)__Value) & BIT_LEN_MASK_16(__BitLen)) << (__BitOffset) ) \
		);
			
#define BIT_LEN_MASK_8(__BitLen) \
		(0xFF >> (8 - (__BitLen)))

#define BIT_OFFSET_LEN_MASK_8(__BitOffset, __BitLen) \
	(BIT_LEN_MASK_8(__BitLen) << (__BitOffset))

#define LE_P1BYTE_TO_HOST_1BYTE(__pStart) \
	(EF1Byte(*((u8 *)(__pStart))))

#define LE_BITS_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
	( \
		( LE_P1BYTE_TO_HOST_1BYTE(__pStart) >> (__BitOffset) ) \
		& \
		BIT_LEN_MASK_8(__BitLen) \
	)

#define LE_BITS_CLEARED_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
	( \
		LE_P1BYTE_TO_HOST_1BYTE(__pStart) \
		& \
		( ~BIT_OFFSET_LEN_MASK_8(__BitOffset, __BitLen) ) \
	)

#define SET_BITS_TO_LE_1BYTE(__pStart, __BitOffset, __BitLen, __Value) \
	*((u8 *)(__pStart)) = \
		EF1Byte( \
			LE_BITS_CLEARED_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
			| \
			( (((u8)__Value) & BIT_LEN_MASK_8(__BitLen)) << (__BitOffset) ) \
		);

//pclint
#define LE_BITS_CLEARED_TO_1BYTE_8BIT(__pStart, __BitOffset, __BitLen) \
	( \
		LE_P1BYTE_TO_HOST_1BYTE(__pStart) \
	)

//pclint
#define SET_BITS_TO_LE_1BYTE_8BIT(__pStart, __BitOffset, __BitLen, __Value) \
{ \
	*((pu1Byte)(__pStart)) = \
		EF1Byte( \
			LE_BITS_CLEARED_TO_1BYTE_8BIT(__pStart, __BitOffset, __BitLen) \
			| \
			((u1Byte)__Value) \
		); \
}

// Get the N-bytes aligment offset from the current length
#define N_BYTE_ALIGMENT(__Value, __Aligment) ((__Aligment == 1) ? (__Value) : (((__Value + __Aligment - 1) / __Aligment) * __Aligment))

typedef unsigned char	BOOLEAN,*PBOOLEAN;

#define TEST_FLAG(__Flag,__testFlag)		(((__Flag) & (__testFlag)) != 0)
#define SET_FLAG(__Flag, __setFlag)			((__Flag) |= __setFlag)
#define CLEAR_FLAG(__Flag, __clearFlag)		((__Flag) &= ~(__clearFlag))
#define CLEAR_FLAGS(__Flag)					((__Flag) = 0)
#define TEST_FLAGS(__Flag, __testFlags)		(((__Flag) & (__testFlags)) == (__testFlags))

/* Define compilor specific symbol */
//
// inline function
//

#if defined ( __ICCARM__ )
#define __inline__                      inline
#define __inline                        inline
#define __inline_definition			//In dialect C99, inline means that a function's definition is provided 
								//only for inlining, and that there is another definition 
								//(without inline) somewhere else in the program. 
								//That means that this program is incomplete, because if 
								//add isn't inlined (for example, when compiling without optimization), 
								//then main will have an unresolved reference to that other definition.

								// Do not inline function is the function body is defined .c file and this 
								// function will be called somewhere else, otherwise there is compile error
#elif defined ( __CC_ARM   )
#define __inline__			__inline	//__linine__ is not supported in keil compilor, use __inline instead
#define inline				__inline
#define __inline_definition			// for dialect C99
#elif defined   (  __GNUC__  )
#define __inline__                      inline
#define __inline                        inline
#define __inline_definition		inline
#endif

//
// pack
//

#if defined (__ICCARM__)

#define RTW_PACK_STRUCT_BEGIN 	_Pragma( STRINGIFY(pack(1)))
#define RTW_PACK_STRUCT_STRUCT 
#define RTW_PACK_STRUCT_END 	_Pragma( STRINGIFY(pack()))
//#define RTW_PACK_STRUCT_USE_INCLUDES

#elif defined (__CC_ARM)

#define RTW_PACK_STRUCT_BEGIN __packed
#define RTW_PACK_STRUCT_STRUCT 
#define RTW_PACK_STRUCT_END

#elif defined (__GNUC__)

#define RTW_PACK_STRUCT_BEGIN
#define RTW_PACK_STRUCT_STRUCT __attribute__ ((__packed__))
#define RTW_PACK_STRUCT_END

#elif defined(PLATFORM_WINDOWS)

#define RTW_PACK_STRUCT_BEGIN
#define RTW_PACK_STRUCT_STRUCT 
#define RTW_PACK_STRUCT_END
#define RTW_PACK_STRUCT_USE_INCLUDES
#endif

// for standard library
#ifdef __ICCARM__
#define __extension__		/* Ignore */
#define	__restrict			/* Ignore */
#endif

typedef struct _RAM_START_FUNCTION_ {
    VOID (*RamStartFun) (VOID);
}RAM_START_FUNCTION, *PRAM_START_FUNCTION;

typedef struct _RAM_FUNCTION_START_TABLE_ {
    VOID (*RamStartFun) (VOID);
    VOID (*RamWakeupFun) (VOID);
    VOID (*RamPatchFun0) (VOID);
    VOID (*RamPatchFun1) (VOID);
    VOID (*RamPatchFun2) (VOID);
}RAM_FUNCTION_START_TABLE, *PRAM_FUNCTION_START_TABLE;

#endif// __BASIC_TYPES_H__

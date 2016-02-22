/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _MISC_H_
#define _MISC_H_

#include <basic_types.h>

#ifdef CONFIG_TIMER_MODULE
extern _LONG_CALL_ u32 HalDelayUs(u32 us);
#endif

extern _LONG_CALL_ u32 HalGetCpuClk(VOID);
extern _LONG_CALL_ u8 HalGetRomInfo(VOID);

extern _LONG_CALL_ void *_memset( void *s, int c, SIZE_T n );
extern _LONG_CALL_ void *_memcpy( void *s1, const void *s2, SIZE_T n );
extern _LONG_CALL_ int _memcmp( const void *av, const void *bv, SIZE_T len );

extern _LONG_CALL_ SIZE_T _strlen(const char *s);
extern _LONG_CALL_ int _strcmp(const char *cs, const char *ct);


#endif  //_MISC_H_

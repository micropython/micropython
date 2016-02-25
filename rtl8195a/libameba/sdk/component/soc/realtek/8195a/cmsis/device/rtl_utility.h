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
#ifndef __RTL_UTILITY_H_
#define __RTL_UTILITY_H_

VOID RtlMemcpy(VOID* dec, VOID* sour, u32 sz);
u32 RtlMemcmp(VOID *dst, VOID *src, u32 sz);
VOID RtlMemset(VOID *pbuf, u32 c, u32 sz);

s8 *
RtlStrncpy(
    IN  s8 *dest, 
    IN  const s8 *src, 
    IN  SIZE_T count
);

s8 *
RtlStrcpy(
    IN  s8 *dest, 
    IN  const s8 *src
);


SIZE_T
RtlStrlen(
    IN  const s8 *s
);


SIZE_T
RtlStrnlen(
    IN  const s8 *s, 
    IN  SIZE_T count
);


int 
RtlStrcmp(
    IN  const s8 *cs, 
    IN  const s8 *ct

);

int
RtlStrncmp(
    IN  const s8 *cs, 
    IN  const s8 *ct, 
    IN  SIZE_T count
);

#endif



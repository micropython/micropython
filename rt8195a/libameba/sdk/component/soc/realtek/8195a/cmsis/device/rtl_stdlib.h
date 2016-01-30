/*
 *  Routines for standard lib access 
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTL_STDLIB_H_
#define _RTL_STDLIB_H_

#include <basic_types.h>
#include <strproc.h>
#include <diag.h>

//
// string operation
//
#define strlen(str)						prvStrLen((const u8*)str)
#define strcmp(str1, str2)				prvStrCmp((const u8*)str1, (const u8*)str2)
#define sscanf(src, format...)				//TODO
#define strtok(str, delim)				prvStrTok(str, delim)
#define strcpy(dst, src)					prvStrCpy((u8 *)dst, (const u8*)src)
#define atoi(str)						prvAtoi(str)
#define strstr(str1, str2)					prvStrStr(str1, str2)							

//
// standard i/o
//
#define snprintf							DiagSnPrintf
#define sprintf							prvDiagSPrintf
#define printf 							prvDiagPrintf

//
// memory management
//
#define malloc 							pvPortMalloc
#define free							vPortFree

#endif //_RTL_STDLIB_H_

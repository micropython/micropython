/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _VA_LIST_H_
#define _VA_LIST_H_

#include "platform_autoconf.h"
#include "basic_types.h"

#ifndef va_arg //this part is adapted from linux (Linux/include/acpi/platform/acenv.h)

typedef s32 acpi_native_int;//this definition is in (Linux/include/acpi/actypes.h)

#ifndef _VALIST
#define _VALIST
    typedef char *va_list;
#endif                          /* _VALIST */

/* Storage alignment properties */
#define  _AUPBND                (sizeof (acpi_native_int) - 1)
#define  _ADNBND                (sizeof (acpi_native_int) - 1)

/* Variable argument list macro definitions */
#define _bnd(X, bnd)            (((sizeof (X)) + (bnd)) & (~(bnd)))
#define va_arg(ap, T)           (*(T *)(((ap) += (_bnd (T, _AUPBND))) - (_bnd (T,_ADNBND))))
#define va_end(ap)              (ap = (va_list) NULL)
#define va_start(ap, A)         (void) ((ap) = (((char *) &(A)) + (_bnd (A,_AUPBND))))

#endif                          /* va_arg */

#endif //_VA_LIST_H_

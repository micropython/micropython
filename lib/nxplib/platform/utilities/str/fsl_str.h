/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _FSL_STR_H
#define _FSL_STR_H

#include "fsl_common.h"

/*!
 * @addtogroup debugconsole
 * @{
 */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @brief A function pointer which is used when format printf log.
 */
typedef void (*printfCb)(char *buf, int32_t *indicator, char val, int len);

/*!
 * @brief This function outputs its parameters according to a formatted string.
 *
 * @note I/O is performed by calling given function pointer using following
 * (*func_ptr)(c);
 *
 * @param[in] fmt   Format string for printf.
 * @param[in] ap  Arguments to printf.
 * @param[in] buf  pointer to the buffer
 * @param cb print callbck function pointer
 *
 * @return Number of characters to be print
 */
int StrFormatPrintf(const char *fmt, va_list ap, char *buf, printfCb cb);

/*!
 * @brief Converts an input line of ASCII characters based upon a provided
 * string format.
 *
 * @param[in] line_ptr The input line of ASCII data.
 * @param[in] format   Format first points to the format string.
 * @param[in] args_ptr The list of parameters.
 *
 * @return Number of input items converted and assigned.
 * @retval IO_EOF When line_ptr is empty string "".
 */
int StrFormatScanf(const char *line_ptr, char *format, va_list args_ptr);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/*! @} */

#endif /* _FSL_STR_H */

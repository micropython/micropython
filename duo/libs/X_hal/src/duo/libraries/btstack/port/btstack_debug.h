/*
 * Copyright (C) 2014 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at 
 * contact@bluekitchen-gmbh.com
 *
 */

/*
 *  btstack_debug.h
 *
 *  allow to funnel debug & error messages 
 */

#ifndef __DEBUG_H
#define __DEBUG_H

#include "btstack_config.h"
#include "btstack_defines.h"
#include "hci_dump.h"

#include <stdio.h>

#ifdef __AVR__
#include <avr/pgmspace.h>
#endif

// Avoid complaints of unused arguments when log levels are disabled.
static inline void __log_unused(const char *format, ...) {}

#ifdef __AVR__
#define HCI_DUMP_LOG(log_level, format, ...) hci_dump_log_P(log_level, PSTR(format), ## __VA_ARGS__)
#define BTSTACK_PRINTF(format, ...)          printf_P(PSTR(format), ## __VA_ARGS__)
#else
#define HCI_DUMP_LOG(log_level, format, ...) hci_dump_log(log_level, format, ## __VA_ARGS__)
#define BTSTACK_PRINTF(format, ...)          printf(format, ## __VA_ARGS__)
#endif

#ifdef ENABLE_LOG_DEBUG
#ifdef ENABLE_LOG_INTO_HCI_DUMP
#define log_debug(format, ...)  HCI_DUMP_LOG(LOG_LEVEL_DEBUG, format,  ## __VA_ARGS__)
#else
#define log_debug(format, ...)  BTSTACK_PRINTF(format "\n",  ## __VA_ARGS__)
#endif
#else
#define log_debug(...) __log_unused(__VA_ARGS__)
#endif

#ifdef ENABLE_LOG_INFO
#ifdef ENABLE_LOG_INTO_HCI_DUMP
#define log_info(format, ...)  HCI_DUMP_LOG(LOG_LEVEL_INFO, format,  ## __VA_ARGS__)
#else
#define log_info(format, ...)  BTSTACK_PRINTF(format "\n",  ## __VA_ARGS__)
#endif
#else
#define log_info(...) __log_unused(__VA_ARGS__)
#endif

#ifdef ENABLE_LOG_ERROR
#ifdef ENABLE_LOG_INTO_HCI_DUMP
#define log_error(format, ...)  HCI_DUMP_LOG(LOG_LEVEL_ERROR, format,  ## __VA_ARGS__)
#else
#define log_error(format, ...)  BTSTACK_PRINTF(format "\n",  ## __VA_ARGS__)
#endif
#else
#define log_error(...) __log_unused(__VA_ARGS__)
#endif

/** 
 * @brief Log Security Manager key via log_info
 * @param key to log
 */
void log_info_key(const char * name, sm_key_t key);

/**
 * @brief Hexdump via log_info
 * @param data
 * @param size
 */
void log_info_hexdump(const void *data, int size);

#endif // __DEBUG_H

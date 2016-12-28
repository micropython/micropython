/**
  Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */


#ifndef SERVICES_DEBUG_H_
#define SERVICES_DEBUG_H_

#include "config.h"

/*
 * This module supports runtime and compile time message filtering.
 *
 * For a message be compiled in the message type has to be >= to then the LOG_LEVEL_AT_COMPILE_TIME
 * For a message to be output the message type has to be >= then the LOG_LEVEL_AT_RUN_TIME
 *
 * This module assumes one of two #defines RELEASE_BUILD or DEBUG_BUILD exist
 * Then the LOG_LEVEL_AT_COMPILE_TIME and LOG_LEVEL_AT_RUN_TIME are set based on RELEASE or DEBUG
 * For a release build
 *     LOG_LEVEL_AT_COMPILE_TIME = WARN_LEVEL
 *         LOG_LEVEL_AT_RUN_TIME = WARN_LEVEL
 * For a debug build:
 *     LOG_LEVEL_AT_COMPILE_TIME = LOG_LEVEL
 *     LOG_LEVEL_AT_RUN_TIME = LOG_LEVEL
 * The API
 * LOG(fmt,...)
 * DEBUG(fmt,...)
 * WARN(fmt,...)
 * ERROR(fmt,...)
 * PANIC(code,fmt,...)
 */

#include <stdint.h>
#include <stddef.h>
#include "panic.h"

typedef enum LoggerOutputLevel {
    DEFAULT_LEVEL   = 0,        // used to select the default logging level
    ALL_LEVEL       = 1,
    TRACE_LEVEL       = 1,
    LOG_LEVEL       = 10,
    DEBUG_LEVEL     = 20,
    INFO_LEVEL      = 30,
    WARN_LEVEL      = 40,
    ERROR_LEVEL     = 50,
    PANIC_LEVEL     = 60,
    NO_LOG_LEVEL    = 70,        // set to not log any messages
} LoggerOutputLevel;

#if !defined(INCLUDE_FILE_INFO_IN_DEBUG)
#define _FILE_PATH          __FILE__
#else
#define _FILE_PATH          NULL
#endif  //

#if defined(DEBUG_BUILD)
#define LOG_LEVEL_AT_COMPILE_TIME LOG_LEVEL
#define LOG_LEVEL_AT_RUN_TIME LOG_LEVEL         // Set to allow all LOG_LEVEL and above messages to be displayed conditionally by levels.
#endif

#if defined(RELEASE_BUILD)
#define LOG_LEVEL_AT_COMPILE_TIME ERROR_LEVEL
#define LOG_LEVEL_AT_RUN_TIME ERROR_LEVEL
#endif

#ifdef __cplusplus
extern "C" {
#endif
void log_print_(int level, int line, const char *func, const char *file, const char *msg, ...);

void log_direct_(const char* s);

/* log print with (formatting arguments) without any extra info or \r\n */
void log_print_direct_(int level, void* reserved, const char *msg, ...);

/**
 * The debug output function.
 */
typedef void (*debug_output_fn)(const char *);

/**
 * Set the debug logger function and optionally the logging level.
 * @param output                The output function. `NULL` to use the existing function.
 * @param debug_output_level    The log level to output.
 */
void set_logger_output(debug_output_fn output, LoggerOutputLevel level);

#define LOG_LEVEL_ACTIVE(level)  (log_level_active(level, NULL))

//int log_level_active(LoggerOutputLevel level, void* reserved);

extern void HAL_Delay_Microseconds(uint32_t delay);

// Short Cuts
#define __LOG_LEVEL_TEST(level) (level >= LOG_LEVEL_AT_COMPILE_TIME && level >= LOG_LEVEL_AT_RUN_TIME)

#if defined(USE_ONLY_PANIC)
#define LOG(fmt, ...)
#define DEBUG(fmt, ...)
#define INFO(fmt, ...)
#define WARN(fmt, ...)
#define ERROR(fmt, ...)
#define PANIC(code,fmt, ...) do {panic_(code, NULL, HAL_Delay_Microseconds);}while(0)
#define DEBUG_D(fmt, ...)
#else
// Macros to use
#define LOG(fmt, ...)    do { if ( __LOG_LEVEL_TEST(LOG_LEVEL)  )  {log_print_(LOG_LEVEL,__LINE__,__PRETTY_FUNCTION__,_FILE_PATH,fmt, ##__VA_ARGS__);}}while(0)
#define DEBUG(fmt, ...)  do { if ( __LOG_LEVEL_TEST(DEBUG_LEVEL))  {log_print_(DEBUG_LEVEL,__LINE__,__PRETTY_FUNCTION__,_FILE_PATH,fmt,##__VA_ARGS__);}}while(0)
#define INFO(fmt, ...)   do { if ( __LOG_LEVEL_TEST(INFO_LEVEL) )  {log_print_(INFO_LEVEL,__LINE__,__PRETTY_FUNCTION__,_FILE_PATH,fmt,##__VA_ARGS__);}}while(0)
#define WARN(fmt, ...)   do { if ( __LOG_LEVEL_TEST(WARN_LEVEL) )  {log_print_(WARN_LEVEL,__LINE__,__PRETTY_FUNCTION__,_FILE_PATH,fmt,##__VA_ARGS__);}}while(0)
#define ERROR(fmt, ...)  do { if ( __LOG_LEVEL_TEST(ERROR_LEVEL) ) {log_print_(ERROR_LEVEL,__LINE__,__PRETTY_FUNCTION__,_FILE_PATH,fmt,##__VA_ARGS__);}}while(0)
#define PANIC(code,fmt, ...)  do { if ( __LOG_LEVEL_TEST(PANIC_LEVEL) ) {log_print_(PANIC_LEVEL,__LINE__,__PRETTY_FUNCTION__,_FILE_PATH,fmt,##__VA_ARGS__);} panic_(code, NULL, HAL_Delay_Microseconds);}while(0)
#define DEBUG_D(fmt, ...)  do { if ( __LOG_LEVEL_TEST(DEBUG_LEVEL))  {log_print_direct_(LOG_LEVEL, nullptr, fmt,##__VA_ARGS__);}}while(0)
#endif
#define SPARK_ASSERT(predicate) do { if (!(predicate)) PANIC(AssertionFailure,"AssertionFailure "#predicate);} while(0);

#ifdef __cplusplus
}
#endif


#endif /* DEBUG_H_ */

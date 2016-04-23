/**
 ******************************************************************************
 * @file    appender.h
 * @author  Matthew McGowan
 ******************************************************************************
  Copyright (c) 2015 Particle Industries, Inc.  All rights reserved.

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
#ifndef CONFIG_H_
#define CONFIG_H_

#if !defined(RELEASE_BUILD) && !defined(DEBUG_BUILD)
#warning  "Defaulting to Release Build"
#define RELEASE_BUILD
#undef  DEBUG_BUILD
#endif

#if !defined(DEBUG_BUILD)
#define USE_ONLY_PANIC // Define to remove all Logging and only have Panic
#endif

// define to include __FILE__ information within the debug output
#define INCLUDE_FILE_INFO_IN_DEBUG
#define MAX_DEBUG_MESSAGE_LENGTH 120

#define RESET_ON_CFOD                   1       // 1 Will do reset 0 will not
#define MAX_SEC_WAIT_CONNECT            8       // Number of second a TCP, spark will wait
#define MAX_FAILED_CONNECTS             2       // Number of time a connect can fail
#define DEFAULT_SEC_INACTIVITY          0
#define DEFAULT_SEC_NETOPS              20

#endif /* CONFIG_H_ */

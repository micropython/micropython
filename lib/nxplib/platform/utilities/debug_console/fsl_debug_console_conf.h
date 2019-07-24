/*
 * Copyright 2017 - 2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_DEBUG_CONSOLE_CONF_H_
#define _FSL_DEBUG_CONSOLE_CONF_H_

/****************Debug console configuration********************/

/*! @brief If Non-blocking mode is needed, please define it at project setting,
 * otherwise blocking mode is the default transfer mode.
 * Warning: If you want to use non-blocking transfer,please make sure the corresponding
 * IO interrupt is enable, otherwise there is no output.
 * And non-blocking is combine with buffer, no matter bare-metal or rtos.
 * Below shows how to configure in your project if you want to use non-blocking mode.
 * For IAR, right click project and select "Options", define it in "C/C++ Compiler->Preprocessor->Defined symbols".
 * For KEIL, click "Options for Target…", define it in "C/C++->Preprocessor Symbols->Define".
 * For ARMGCC, open CmakeLists.txt and add the following lines,
 * "SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG_CONSOLE_TRANSFER_NON_BLOCKING")" for debug target.
 * "SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DDEBUG_CONSOLE_TRANSFER_NON_BLOCKING")" for release target.
 * For MCUxpresso, right click project and select "Properties", define it in "C/C++ Build->Settings->MCU C
 * Complier->Preprocessor".
 *
 */
#ifdef DEBUG_CONSOLE_TRANSFER_NON_BLOCKING
/*! @brief define the transmit buffer length which is used to store the multi task log, buffer is enabled automatically
 * when
 * non-blocking transfer is using,
 * This value will affect the RAM's ultilization, should be set per paltform's capability and software requirement.
 * If it is configured too small, log maybe missed , because the log will not be
 * buffered if the buffer is full, and the print will return immediately with -1.
 * And this value should be multiple of 4 to meet memory alignment.
 *
 */
#ifndef DEBUG_CONSOLE_TRANSMIT_BUFFER_LEN
#define DEBUG_CONSOLE_TRANSMIT_BUFFER_LEN (512U)
#endif /* DEBUG_CONSOLE_TRANSMIT_BUFFER_LEN */

/*! @brief define the receive buffer length which is used to store the user input, buffer is enabled automatically when
 * non-blocking transfer is using,
 * This value will affect the RAM's ultilization, should be set per paltform's capability and software requirement.
 * If it is configured too small, log maybe missed, because buffer will be overwrited if buffer is too small.
 * And this value should be multiple of 4 to meet memory alignment.
 *
 */
#ifndef DEBUG_CONSOLE_RECEIVE_BUFFER_LEN
#define DEBUG_CONSOLE_RECEIVE_BUFFER_LEN (1024U)
#endif /* DEBUG_CONSOLE_RECEIVE_BUFFER_LEN */

/*!@ brief Whether enable the reliable TX function
 * If the macro is zero, the reliable TX function of the debug console is disabled.
 * When the macro is zero, the string of PRINTF will be thrown away after the transmit buffer is full.
 */
#ifndef DEBUG_CONSOLE_TX_RELIABLE_ENABLE
#define DEBUG_CONSOLE_TX_RELIABLE_ENABLE (1U)
#endif /* DEBUG_CONSOLE_RX_ENABLE */

#else
#define DEBUG_CONSOLE_TRANSFER_BLOCKING
#endif /* DEBUG_CONSOLE_TRANSFER_NON_BLOCKING */

/*!@ brief Whether enable the RX function
 * If the macro is zero, the receive function of the debug console is disabled.
 */
#ifndef DEBUG_CONSOLE_RX_ENABLE
#define DEBUG_CONSOLE_RX_ENABLE (1U)
#endif /* DEBUG_CONSOLE_RX_ENABLE */

/*!@ brief define the MAX log length debug console support , that is when you call printf("log", x);, the log
 * length can not bigger than this value.
 * This macro decide the local log buffer length, the buffer locate at stack, the stack maybe overflow if
 * the buffer is too big and current task stack size not big enough.
 */
#ifndef DEBUG_CONSOLE_PRINTF_MAX_LOG_LEN
#define DEBUG_CONSOLE_PRINTF_MAX_LOG_LEN (128U)
#endif /* DEBUG_CONSOLE_PRINTF_MAX_LOG_LEN */

/*!@ brief define the buffer support buffer scanf log length, that is when you call scanf("log", &x);, the log
 * length can not bigger than this value.
 * As same as the DEBUG_CONSOLE_BUFFER_PRINTF_MAX_LOG_LEN.
 */
#ifndef DEBUG_CONSOLE_SCANF_MAX_LOG_LEN
#define DEBUG_CONSOLE_SCANF_MAX_LOG_LEN (20U)
#endif /* DEBUG_CONSOLE_SCANF_MAX_LOG_LEN */

/*! @brief Debug console synchronization
 * User should not change these macro for synchronization mode, but add the
 * corresponding synchronization mechanism per different software environment.
 * Such as, if another RTOS is used,
 * add:
 *  #define DEBUG_CONSOLE_SYNCHRONIZATION_XXXX 3
 * in this configuration file and implement the synchronization in fsl.log.c.
 */
/*! @brief synchronization for baremetal software */
#define DEBUG_CONSOLE_SYNCHRONIZATION_BM 0
/*! @brief synchronization for freertos software */
#define DEBUG_CONSOLE_SYNCHRONIZATION_FREERTOS 1

/*! @brief RTOS synchronization mechanism disable
 * If not defined, default is enable, to avoid multitask log print mess.
 * If other RTOS is used, you can implement the RTOS's specific synchronization mechanism in fsl.log.c
 * If synchronization is disabled, log maybe messed on terminal.
 */
#ifndef DEBUG_CONSOLE_DISABLE_RTOS_SYNCHRONIZATION
#ifdef DEBUG_CONSOLE_TRANSFER_NON_BLOCKING
#ifdef FSL_RTOS_FREE_RTOS
#define DEBUG_CONSOLE_SYNCHRONIZATION_MODE DEBUG_CONSOLE_SYNCHRONIZATION_FREERTOS
#else
#define DEBUG_CONSOLE_SYNCHRONIZATION_MODE DEBUG_CONSOLE_SYNCHRONIZATION_BM
#endif /* FSL_RTOS_FREE_RTOS */
#else
#define DEBUG_CONSOLE_SYNCHRONIZATION_MODE DEBUG_CONSOLE_SYNCHRONIZATION_BM
#endif /* DEBUG_CONSOLE_TRANSFER_NON_BLOCKING */
#endif /* DEBUG_CONSOLE_DISABLE_RTOS_SYNCHRONIZATION */

/*! @brief echo function support
 * If you want to use the echo function,please define DEBUG_CONSOLE_ENABLE_ECHO
 * at your project setting.
 */
#ifndef DEBUG_CONSOLE_ENABLE_ECHO
#define DEBUG_CONSOLE_ENABLE_ECHO_FUNCTION 0
#else
#define DEBUG_CONSOLE_ENABLE_ECHO_FUNCTION 1
#endif /* DEBUG_CONSOLE_ENABLE_ECHO */

/*********************************************************************/

/***************Debug console other configuration*********************/
/*! @brief Definition to printf the float number. */
#ifndef PRINTF_FLOAT_ENABLE
#define PRINTF_FLOAT_ENABLE 0U
#endif /* PRINTF_FLOAT_ENABLE */

/*! @brief Definition to scanf the float number. */
#ifndef SCANF_FLOAT_ENABLE
#define SCANF_FLOAT_ENABLE 0U
#endif /* SCANF_FLOAT_ENABLE */

/*! @brief Definition to support advanced format specifier for printf. */
#ifndef PRINTF_ADVANCED_ENABLE
#define PRINTF_ADVANCED_ENABLE 0U
#endif /* PRINTF_ADVANCED_ENABLE */

/*! @brief Definition to support advanced format specifier for scanf. */
#ifndef SCANF_ADVANCED_ENABLE
#define SCANF_ADVANCED_ENABLE 0U
#endif /* SCANF_ADVANCED_ENABLE */

/*! @brief Definition to select virtual com(USB CDC) as the debug console. */
#ifndef BOARD_USE_VIRTUALCOM
#define BOARD_USE_VIRTUALCOM 0U
#endif
/*******************************************************************/

#endif /* _FSL_DEBUG_CONSOLE_CONF_H_ */

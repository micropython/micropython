/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <stdio.h>
#include <string.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "board.h"

#if defined(BOARD_USART_CMSIS_HEADER)
#include "usart.h"
#include BOARD_USART_CMSIS_HEADER

static void usart_init_helper(void)
{
    uint8_t i = 0;
    for(i=0; i<MP_ARRAY_SIZE(MP_STATE_PORT(pyb_usart_callback)); i++)
    {
        MP_STATE_PORT(pyb_usart_callback)[i] = mp_const_none;
    }
}

void usart_init(void)
{
    usart_init_helper();
}

struct _ARM_DRIVER_USART *usart_find_index(mp_obj_t id, uint8_t *instance)
{
    if(mp_obj_is_str(id))
    {
        const char* usartname = mp_obj_str_get_str(id);
        #if defined(RTE_USART0) && RTE_USART0
        #if defined(MICROPY_HW_USART0_NAME)
        if(strcmp(usartname, MICROPY_HW_USART0_NAME) == 0)
        {
            *instance = 0;
            return (struct _ARM_DRIVER_USART *)&Driver_USART0;
        }
        #endif /* MICROPY_HW_USART0_NAME */
        #endif /*RTE_USART0*/
        #if defined(RTE_USART1) && RTE_USART1
        #if defined(MICROPY_HW_USART1_NAME)
        if(strcmp(usartname, MICROPY_HW_USART1_NAME) == 0)
        {
            *instance = 1;
            return (struct _ARM_DRIVER_USART *)&Driver_USART1;
        }
        #endif /* MICROPY_HW_USART1_NAME */
        #endif /*RTE_USART1*/
        #if defined(RTE_USART2) && RTE_USART2
        #if defined(MICROPY_HW_USART2_NAME)
        if(strcmp(usartname, MICROPY_HW_USART2_NAME) == 0)
        {
            *instance = 2;
            return (struct _ARM_DRIVER_USART *)&Driver_USART2;
        }
        #endif /* MICROPY_HW_USART2_NAME */
        #endif /*RTE_USART2*/
        #if defined(RTE_USART3) && RTE_USART3
        #if defined(MICROPY_HW_USART3_NAME)
        if(strcmp(usartname, MICROPY_HW_USART0_NAME) == 0)
        {
            *instance = 3;
            return (struct _ARM_DRIVER_USART *)&Driver_USART3;
        }
        #endif /* MICROPY_HW_USART3_NAME */
        #endif /*RTE_USART3*/
        #if defined(RTE_USART4) && RTE_USART4
        #if defined(MICROPY_HW_USART4_NAME)
        if(strcmp(usartname, MICROPY_HW_USART4_NAME) == 0)
        {
            *instance = 4;
            return (struct _ARM_DRIVER_USART *)&Driver_USART4;
        }
        #endif /* MICROPY_HW_USART4_NAME */
        #endif /*RTE_USART4*/
        #if defined(RTE_USART5) && RTE_USART5
        #if defined(MICROPY_HW_USART5_NAME)
        if(strcmp(usartname, MICROPY_HW_USART5_NAME) == 0)
        {
            *instance = 5;
            return (struct _ARM_DRIVER_USART *)&Driver_USART5;
        }
        #endif /* MICROPY_HW_USART5_NAME */
        #endif /* RTE_USART5 */
        #if defined(RTE_USART6) && RTE_USART6
        #if defined(MICROPY_HW_USART6_NAME)
        if(strcmp(usartname, MICROPY_HW_USART6_NAME) == 0)
        {
            *instance = 6;
            return (struct _ARM_DRIVER_USART *)&Driver_USART6;
        }
        #endif /* MICROPY_HW_USART6_NAME */
        #endif /* RTE_USART6 */
        #if defined(RTE_USART7) && RTE_USART7
        #if defined(MICROPY_HW_USART7_NAME)
        if(strcmp(usartname, MICROPY_HW_USART7_NAME) == 0)
        {
            *instance = 7;
            return (struct _ARM_DRIVER_USART *)&Driver_USART7;
        }
        #endif /* MICROPY_HW_USART7_NAME */
        #endif /* RTE_USART7 */
        #if defined(RTE_USART8) && RTE_USART8
        #if defined(MICROPY_HW_USART8_NAME)
        if(strcmp(usartname, MICROPY_HW_USART8_NAME) == 0)
        {
            *instance = 8;
            return (struct _ARM_DRIVER_USART *)&Driver_USART8;
        }
        #endif /* MICROPY_HW_USART8_NAME */
        #endif /* RTE_USART8 */
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "USART(%s) doesn't exist", usartname));
    }
    else
    {
        int usart_id = mp_obj_get_int(id);
        #if defined(RTE_USART0) && RTE_USART0
        if(usart_id == 0)
        {
            *instance = 0;
            return (struct _ARM_DRIVER_USART *)&Driver_USART0;
        }
        #endif /* RTE_USART0 */
        #if defined(RTE_USART1) && RTE_USART1
        if(usart_id == 1)
        {
            *instance = 1;
            return (struct _ARM_DRIVER_USART *)&Driver_USART1;
        }
        #endif /* RTE_USART1 */
        #if defined(RTE_USART2) && RTE_USART2
        if(usart_id == 2)
        {
            *instance = 2;
            return (struct _ARM_DRIVER_USART *)&Driver_USART2;
        }
        #endif /* RTE_USART2 */
        #if defined(RTE_USART3) && RTE_USART3
        if(usart_id == 3)
        {
            *instance = 3;
            return (struct _ARM_DRIVER_USART *)&Driver_USART3;
        }
        #endif /* RTE_USART3 */
        #if defined(RTE_USART4) && RTE_USART4
        if(usart_id == 4)
        {
            *instance = 4;
            return (struct _ARM_DRIVER_USART *)&Driver_USART4;
        }
        #endif /* RTE_USART4 */
        #if defined(RTE_USART5) && RTE_USART5
        if(usart_id == 5)
        {
            *instance = 5;
            return (struct _ARM_DRIVER_USART *)&Driver_USART5;
        }
        #endif /* RTE_USART5 */
        #if defined(RTE_USART6) && RTE_USART6
        if(usart_id == 6)
        {
            *instance = 6;
            return (struct _ARM_DRIVER_USART *)&Driver_USART6;
        }
        #endif /* RTE_USART6 */
        #if defined(RTE_USART7) && RTE_USART7
        if(usart_id == 7)
        {
            *instance = 7;
            return (struct _ARM_DRIVER_USART *)&Driver_USART7;
        }
        #endif /* RTE_USART7 */
        #if defined(RTE_USART8) && RTE_USART8
        if(usart_id == 8)
        {
            *instance = 8;
            return (struct _ARM_DRIVER_USART *)&Driver_USART8;
        }
        #endif /* RTE_USART8 */
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "USART(%d) doesn't exist", usart_id));
    }
}

#if defined(RTE_USART0) && RTE_USART0
STATIC void DriverUSART0_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_usart_callback)[0];
    if(*cb != mp_const_none)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /* RTE_USART0 */

#if defined(RTE_USART1) && RTE_USART1
STATIC void DriverUSART1_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_usart_callback)[1];
    if(*cb != mp_const_none)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /* RTE_USART1 */

#if defined(RTE_USART2) && RTE_USART2
STATIC void DriverUSART2_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_usart_callback)[2];
    if(*cb != mp_const_none)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /* RTE_USART2 */

#if defined(RTE_USART3) && RTE_USART3
STATIC void DriverUSART3_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_usart_callback)[3];
    if(*cb != mp_const_none)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /* RTE_USART3 */

#if defined(RTE_USART4) && RTE_USART4
STATIC void DriverUSART4_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_usart_callback)[4];
    if(*cb != mp_const_none)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /* RTE_USART4 */

#if defined(RTE_USART5) && RTE_USART5
STATIC void DriverUSART5_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_usart_callback)[5];
    if(*cb != mp_const_none)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /* RTE_USART5 */

#if defined(RTE_USART6) && RTE_USART6
STATIC void DriverUSART6_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_usart_callback)[5];
    if(*cb != mp_const_none)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /* RTE_USART6 */

#if defined(RTE_USART7) && RTE_USART7
STATIC void DriverUSART7_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_usart_callback)[5];
    if(*cb != mp_const_none)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /* RTE_USART7 */

#if defined(RTE_USART8) && RTE_USART8
STATIC void DriverUSART8_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_usart_callback)[5];
    if(*cb != mp_const_none)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /* RTE_USART8 */

void *usart_find_callback(uint8_t instance)
{
    #if defined(RTE_USART0) && RTE_USART0
    if(instance == 0)
    {
        return DriverUSART0_Signal_Event;
    }
    #endif /* RTE_USART0 */
    #if defined(RTE_USART1) && RTE_USART1
    if(instance == 1)
    {
        return DriverUSART1_Signal_Event;
    }
    #endif /* RTE_USART1 */
    #if defined(RTE_USART2) && RTE_USART2
    if (instance == 2)
    {
        return DriverUSART2_Signal_Event;
    }
    #endif /* RTE_USART2 */
    #if defined(RTE_USART3) && RTE_USART3
    if (instance == 3)
    {
        return DriverUSART3_Signal_Event;
    }
    #endif /* RTE_USART3 */
    #if defined(RTE_USART4) && RTE_USART4
    if (instance == 4)
    {
        return DriverUSART4_Signal_Event;
    }
    #endif /* RTE_USART4 */
    #if defined(RTE_USART5) && RTE_USART5
    if (instance == 5)
    {
        return DriverUSART5_Signal_Event;
    }
    #endif /* RTE_USART5 */
    #if defined(RTE_USART6) && RTE_USART6
    if (instance == 6)
    {
        return DriverUSART6_Signal_Event;
    }
    #endif /* RTE_USART6 */
    #if defined(RTE_USART7) && RTE_USART7
    if (instance == 7)
    {
        return DriverUSART7_Signal_Event;
    }
    #endif /* RTE_USART7 */
    #if defined(RTE_USART8) && RTE_USART8
    if (instance == 8)
    {
        return DriverUSART8_Signal_Event;
    }
    #endif /* RTE_USART8 */
    return NULL;
}

#if defined(LPUART0) && defined(RTE_USART0) && RTE_USART0
uint32_t LPUART0_GetFreq(void)
{
    return BOARD_DebugConsoleSrcFreq();
}
#endif /* LPUART0 */

#if defined(LPUART1) && defined(RTE_USART1) && RTE_USART1
uint32_t LPUART1_GetFreq(void)
{
    return BOARD_DebugConsoleSrcFreq();
}
#endif /* LPUART1 */

#if defined(LPUART2) && defined(RTE_USART2) && RTE_USART2
uint32_t LPUART2_GetFreq(void)
{
    return BOARD_DebugConsoleSrcFreq();
}
#endif /* LPUART2 */

#if defined(LPUART3) && defined(RTE_USART3) && RTE_USART3
uint32_t LPUART3_GetFreq(void)
{
    return BOARD_DebugConsoleSrcFreq();
}
#endif /* LPUART3 */

#if defined(LPUART4) && defined(RTE_USART4) && RTE_USART4
uint32_t LPUART4_GetFreq(void)
{
    return BOARD_DebugConsoleSrcFreq();
}
#endif /* LPUART4 */

#if defined(LPUART5) && defined(RTE_USART5) && RTE_USART5
uint32_t LPUART5_GetFreq(void)
{
    return BOARD_DebugConsoleSrcFreq();
}
#endif /* LPUART5 */

#if defined(LPUART6) && defined(RTE_USART6) && RTE_USART6
uint32_t LPUART6_GetFreq(void)
{
    return BOARD_DebugConsoleSrcFreq();
}
#endif /* LPUART6 */

#if defined(LPUART7) && defined(RTE_USART7) && RTE_USART7
uint32_t LPUART7_GetFreq(void)
{
    return BOARD_DebugConsoleSrcFreq();
}
#endif /* LPUART7 */

#if defined(LPUART8) && defined(RTE_USART8) && RTE_USART8
uint32_t LPUART8_GetFreq(void)
{
    return BOARD_DebugConsoleSrcFreq();
}
#endif /* LPUART8 */

#if defined(UART0) && defined(RTE_USART0) && RTE_USART0
uint32_t UART0_GetFreq(void)
{
    return CLOCK_GetFreq(UART0_CLK_SRC);
}
#endif /* UART0 */

#if defined(UART1) && defined(RTE_USART1) && RTE_USART1
uint32_t UART1_GetFreq(void)
{
    return CLOCK_GetFreq(UART1_CLK_SRC);
}
#endif /* UART1 */

#if defined(UART2) && defined(RTE_USART2) && RTE_USART2
uint32_t UART2_GetFreq(void)
{
    return CLOCK_GetFreq(UART2_CLK_SRC);
}
#endif /* UART2 */

#if defined(UART3) && defined(RTE_USART3) && RTE_USART3
uint32_t UART3_GetFreq(void)
{
    return CLOCK_GetFreq(UART3_CLK_SRC);
}
#endif /* UART3 */

#if defined(UART4) && defined(RTE_USART4) && RTE_USART4
uint32_t UART4_GetFreq(void)
{
    return CLOCK_GetFreq(UART4_CLK_SRC);
}
#endif /* UART4 */

#if defined(UART5) && defined(RTE_USART5) && RTE_USART5
uint32_t UART5_GetFreq(void)
{
    return CLOCK_GetFreq(UART5_CLK_SRC);
}
#endif /* UART5 */

#endif /*BOARD_USART_CMSIS_HEADER*/

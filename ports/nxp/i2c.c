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

#include "mpconfigboard.h"
#if defined(BOARD_I2C_CMSIS_HEADER)
#include BOARD_I2C_CMSIS_HEADER
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "i2c.h"
#include "board.h"

static void i2c_init_helper(void)
{
    uint8_t i = 0;
    for(i=0; i<MP_ARRAY_SIZE(MP_STATE_PORT(pyb_i2c_callback)); i++)
    {
        MP_STATE_PORT(pyb_i2c_callback)[i] = MP_OBJ_NULL;
    }
}

void i2c_init(void)
{
    #if defined(MICROPY_USE_IMXRT1064_MCU) || defined(MICROPY_USE_IMXRT1060_MCU) || defined(MICROPY_USE_IMXRT1050_MCU) || defined(MICROPY_USE_IMXRT1020_MCU)
    CLOCK_SetMux(kCLOCK_Lpi2cMux, 0U);
    CLOCK_SetDiv(kCLOCK_Lpi2cDiv, 5U);
    #endif
    i2c_init_helper();
}


struct _ARM_DRIVER_I2C *i2c_find_index(mp_obj_t id, uint8_t *instance)
{
    if(mp_obj_is_str(id))
    {
        const char* i2cname  = mp_obj_str_get_str(id);
        #if defined(RTE_I2C0) && RTE_I2C0
        #if defined(MICROPY_HW_I2C0_NAME)
        if(strcmp(i2cname, MICROPY_HW_I2C0_NAME) == 0)
        {
            *instance = 0;
            return (struct _ARM_DRIVER_I2C *)&Driver_I2C0;
        }
        #endif /* MICROPY_HW_I2C0_NAME */
        #endif /* RTE_I2C0 */
        #if defined(RTE_I2C1) && RTE_I2C1
        #if defined(MICROPY_HW_I2C1_NAME)
        if(strcmp(i2cname, MICROPY_HW_I2C1_NAME) == 0)
        {
            *instance = 1;
            return (struct _ARM_DRIVER_I2C *)&Driver_I2C1;
        }
        #endif /* MICROPY_HW_I2C1_NAME */
        #endif /* RTE_I2C1 */
        #if defined(RTE_I2C2) && RTE_I2C2
        #if defined(MICROPY_HW_I2C2_NAME)
        if(strcmp(i2cname, MICROPY_HW_I2C2_NAME) == 0)
        {
            *instance = 2;
            return (struct _ARM_DRIVER_I2C *)&Driver_I2C2;
        }
        #endif /* MICROPY_HW_I2C2_NAME */
        #endif /* RTE_I2C2 */
        #if defined(RTE_I2C3) && RTE_I2C3
        #if defined(MICROPY_HW_I2C3_NAME)
        if(strcmp(i2cname, MICROPY_HW_I2C3_NAME) == 0)
        {
            *instance = 3;
            return (struct _ARM_DRIVER_I2C *)&Driver_I2C3;
        }
        #endif /* MICROPY_HW_I2C3_NAME */
        #endif /* RTE_I2C3 */
        #if defined(RTE_I2C4) && RTE_I2C4
        #if defined(MICROPY_HW_I2C4_NAME)
        if(strcmp(i2cname, MICROPY_HW_I2C4_NAME) == 0)
        {
            *instance = 4;
            return (struct _ARM_DRIVER_I2C *)&Driver_I2C4;
        }
        #endif /* MICROPY_HW_I2C4_NAME */
        #endif /* RTE_I2C4 */
        #if defined(RTE_I2C5) && RTE_I2C5
        #if defined(MICROPY_HW_I2C5_NAME)
        if(strcmp(i2cname, MICROPY_HW_I2C5_NAME) == 0)
        {
            *instance = 5;
            return (struct _ARM_DRIVER_I2C *)&Driver_I2C5;
        }
        #endif /* MICROPY_HW_I2C5_NAME */
        #endif /* RTE_I2C5 */
        #if defined(RTE_I2C6) && RTE_I2C6
        #if defined(MICROPY_HW_I2C6_NAME)
        if(strcmp(i2cname, MICROPY_HW_I2C6_NAME) == 0)
        {
            *instance = 6;
            return (struct _ARM_DRIVER_I2C *)&Driver_I2C6;
        }
        #endif /* MICROPY_HW_I2C6_NAME */
        #endif /* RTE_I2C6 */
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "I2C(%s) doesn't exist", i2cname));
    }
    else
    {
        int i2c_id = mp_obj_get_int(id);
        #if defined(RTE_I2C0) && RTE_I2C0
        if(i2c_id == 0)
        {
            *instance = 0;
            return (struct _ARM_DRIVER_I2C *)&Driver_I2C0;
        }
        #endif /*RTE_I2C0*/
        #if defined(RTE_I2C1) && RTE_I2C1
        if(i2c_id == 1)
        {
            *instance = 1;
            return (struct _ARM_DRIVER_I2C *)&Driver_I2C1;
        }
        #endif /*RTE_I2C1*/
        #if defined(RTE_I2C2) && RTE_I2C2
        if(i2c_id == 2)
        {
            *instance = 2;
            return (struct _ARM_DRIVER_I2C *)&Driver_I2C2;
        }
        #endif /*RTE_I2C2*/
        #if defined(RTE_I2C3) && RTE_I2C3
        if(i2c_id == 3)
        {
            *instance = 3;
            return (struct _ARM_DRIVER_I2C *)&Driver_I2C3;
        }
        #endif /*RTE_I2C3*/
        #if defined(RTE_I2C4) && RTE_I2C4
        if(i2c_id == 4)
        {
            *instance = 4;
            return (struct _ARM_DRIVER_I2C *)&Driver_I2C4;
        }
        #endif /*RTE_I2C4*/
        #if defined(RTE_I2C5) && RTE_I2C5
        if(i2c_id == 5)
        {
            *instance = 5;
            return (struct _ARM_DRIVER_I2C *)&Driver_I2C5;
        }
        #endif /*RTE_I2C5*/
        #if defined(RTE_I2C5) && RTE_I2C5
        if(i2c_id == 6)
        {
            *instance = 6;
            return (struct _ARM_DRIVER_I2C *)&Driver_I2C6;
        }
        #endif /*RTE_I2C6*/
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "I2C(%d) doesn't exist", i2c_id));
    }
}

#if defined(RTE_I2C0) && RTE_I2C0
static void DriverI2C0_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_i2c_callback)[0];
    if(*cb != MP_OBJ_NULL)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /*RTE_I2C0*/

#if defined(RTE_I2C1) && RTE_I2C1
static void DriverI2C1_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_i2c_callback)[1];
    if(*cb != MP_OBJ_NULL)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /*RTE_I2C1*/

#if defined(RTE_I2C2) && RTE_I2C2
static void DriverI2C2_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_i2c_callback)[2];
    if(*cb != MP_OBJ_NULL)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /*RTE_I2C2*/

#if defined(RTE_I2C3) && RTE_I2C3
static void DriverI2C3_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_i2c_callback)[3];
    if(*cb != MP_OBJ_NULL)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /*RTE_I2C3*/

#if defined(RTE_I2C4) && RTE_I2C4
static void DriverI2C4_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_i2c_callback)[4];
    if(*cb != MP_OBJ_NULL)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /*RTE_I2C4*/

#if defined(RTE_I2C5) && RTE_I2C5
static void DriverI2C5_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_i2c_callback)[5];
    if(*cb != MP_OBJ_NULL)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /*RTE_I2C5*/


void *i2c_find_callback(uint8_t instance)
{
    #if defined(RTE_I2C0) && RTE_I2C0
    if(instance == 0)
    {
        return DriverI2C0_Signal_Event;
    }
    #endif /*RTE_I2C0*/
    #if defined(RTE_I2C1) && RTE_I2C1
    if(instance == 1)
    {
        return DriverI2C1_Signal_Event;
    }
    #endif /*RTE_I2C1*/
    #if defined(RTE_I2C2) && RTE_I2C2
    if(instance == 2)
    {
        return DriverI2C2_Signal_Event;
    }
    #endif /*RTE_I2C2*/
    #if defined(RTE_I2C3) && RTE_I2C3
    if(instance == 3)
    {
        return DriverI2C3_Signal_Event;
    }
    #endif /*RTE_I2C3*/
    #if defined(RTE_I2C4) && RTE_I2C4
    if(instance == 4)
    {
        return DriverI2C4_Signal_Event;
    }
    #endif /*RTE_I2C4*/
    #if defined(RTE_I2C5) && RTE_I2C5
    if(instance == 5)
    {
        return DriverI2C5_Signal_Event;
    }
    #endif /*RTE_I2C5*/
    return NULL;
}

#if MICROPY_USE_MK64F12_MCU
#if defined(I2C0) && defined(RTE_I2C0) && RTE_I2C0
uint32_t I2C0_GetFreq(void)
{
    return CLOCK_GetFreq(I2C0_CLK_SRC);
}
#endif /*I2C0*/

#if defined(I2C1) && defined(RTE_I2C1) && RTE_I2C1
uint32_t I2C1_GetFreq(void)
{
    return CLOCK_GetFreq(I2C1_CLK_SRC);
}
#endif /*I2C1*/

#if defined(I2C2) && defined(RTE_I2C2) && RTE_I2C2
uint32_t I2C2_GetFreq(void)
{
    return CLOCK_GetFreq(I2C2_CLK_SRC);
}
#endif /*I2C2*/
#endif /* MICROPY_USE_MK64F12_MCU */

#if MICROPY_USE_IMXRT1064_MCU || MICROPY_USE_IMXRT1060_MCU || MICROPY_USE_IMXRT1050_MCU || MICROPY_USE_IMXRT1020_MCU

#if defined(LPI2C1) && defined(RTE_I2C1) && RTE_I2C1
uint32_t LPI2C1_GetFreq(void)
{
    return ((CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8) / (6U));
}
#endif /* LPI2C1 */

#if defined(LPI2C2) && defined(RTE_I2C2) && RTE_I2C2
uint32_t LPI2C2_GetFreq(void)
{
    return ((CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8) / (6U));
}
#endif /* LPI2C2 */

#if defined(LPI2C3) && defined(RTE_I2C3) && RTE_I2C3
uint32_t LPI2C3_GetFreq(void)
{
    return ((CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8) / (6U));
}
#endif /* LPI2C3 */

#if defined(LPI2C4) && defined(RTE_I2C4) && RTE_I2C4
uint32_t LPI2C4_GetFreq(void)
{
    return ((CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8) / (6U));
}
#endif /* LPI2C4 */

#if defined(LPI2C5) && defined(RTE_I2C5) && RTE_I2C5
uint32_t LPI2C5_GetFreq(void)
{
    return ((CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8) / (6U));
}
#endif /* LPI2C5 */

#endif /* MICROPY_USE_IMXRT1064_MCU || MICROPY_USE_IMXRT1060_MCU || MICROPY_USE_IMXRT1050_MCU || MICROPY_USE_IMXRT1020_MCU */

#endif /*BOARD_I2C_CMSIS_HEADER*/
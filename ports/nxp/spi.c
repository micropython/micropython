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

#include "py/runtime.h"
#include "py/mphal.h"

#if defined(BOARD_SPI_CMSIS_HEADER)
#include "spi.h"
#include "fsl_clock.h"

static void spi_init_helper(void)
{
    uint8_t i = 0;
    for(i=0; i< MP_ARRAY_SIZE(MP_STATE_PORT(pyb_spi_callback)); i++)
    {
        MP_STATE_PORT(pyb_spi_callback)[i] = mp_const_none;
    }
}

void spi_init(void)
{
    #if defined(MICROPY_MIN_USE_IMXRT1064_MCU) || defined(MICROPY_MIN_USE_IMXRT1060_MCU) || defined(MICROPY_MIN_USE_IMXRT1050_MCU) || defined(MICROPY_MIN_USE_IMXRT1020_MCU)
    CLOCK_SetMux(kCLOCK_LpspiMux, 1U);
    CLOCK_SetDiv(kCLOCK_LpspiDiv, 7U);
    #endif
}

struct _ARM_DRIVER_SPI *spi_find_index(mp_obj_t id, uint8_t *instance)
{
    if(mp_obj_is_str(id))
    {
        const char *spiname = mp_obj_str_get_str(id);
        #if defined(RTE_SPI0) && RTE_SPI0
        #if defined (MICROPY_HW_SPI0_NAME)
        if(strcmp(spiname, MICROPY_HW_SPI0_NAME) == 0)
        {
            *instance = 0;
            return (struct _ARM_DRIVER_SPI *)&Driver_SPI0;
        }
        #endif /* MICROPY_HW_SPI0_NAME */
        #endif /* RTE_SPI0 */
        #if defined(RTE_SPI1) && RTE_SPI1
        #if defined (MICROPY_HW_SPI1_NAME)
        if(strcmp(spiname, MICROPY_HW_SPI1_NAME) == 0)
        {
            *instance = 1;
            return (struct _ARM_DRIVER_SPI *)&Driver_SPI1;
        }
        #endif /* MICROPY_HW_SPI1_NAME */
        #endif /* RTE_SPI1 */
        #if defined(RTE_SPI2) && RTE_SPI2
        #if defined (MICROPY_HW_SPI2_NAME)
        if(strcmp(spiname, MICROPY_HW_SPI2_NAME) == 0)
        {
            *instance = 2;
            return (struct _ARM_DRIVER_SPI *)&Driver_SPI2;
        }
        #endif /* MICROPY_HW_SPI2_NAME */
        #endif /* RTE_SPI2 */
        #if defined(RTE_SPI3) && RTE_SPI3
        #if defined (MICROPY_HW_SPI3_NAME)
        if(strcmp(spiname, MICROPY_HW_SPI3_NAME) == 0)
        {
            *instance = 3;
            return (struct _ARM_DRIVER_SPI *)&Driver_SPI3;
        }
        #endif /* MICROPY_HW_SPI3_NAME */
        #endif /* RTE_SPI3 */
        #if defined(RTE_SPI4) && RTE_SPI4
        #if defined (MICROPY_HW_SPI4_NAME)
        if(strcmp(spiname, MICROPY_HW_SPI4_NAME) == 0)
        {
            *instance = 4;
            return (struct _ARM_DRIVER_SPI *)&Driver_SPI4;
        }
        #endif /* MICROPY_HW_SPI4_NAME */
        #endif /* RTE_SPI4 */
        #if defined(RTE_SPI5) && RTE_SPI5
        #if defined (MICROPY_HW_SPI5_NAME)
        if(strcmp(spiname, MICROPY_HW_SPI5_NAME) == 0)
        {
            *instance = 5;
            return (struct _ARM_DRIVER_SPI *)&Driver_SPI5;
        }
        #endif /* MICROPY_HW_SPI5_NAME */
        #endif /* RTE_SPI5 */
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "SPI(%s) doesn't exist", spiname));
    }
    else
    {
        int spi_id = mp_obj_get_int(id);
        #if defined(RTE_SPI0) && RTE_SPI0
        if(spi_id == 0 )
        {
            *instance = 0;
            return (struct _ARM_DRIVER_SPI *)&Driver_SPI0;
        }
        #endif /* RTE_SPI0 */
        #if defined(RTE_SPI1) && RTE_SPI1
        if(spi_id == 1 )
        {
            *instance = 1;
            return (struct _ARM_DRIVER_SPI *)&Driver_SPI1;
        }
        #endif /* RTE_SPI1 */
        #if defined(RTE_SPI2) && RTE_SPI2
        if(spi_id == 2 )
        {
            *instance = 2;
            return (struct _ARM_DRIVER_SPI *)&Driver_SPI2;
        }
        #endif /* RTE_SPI2 */
        #if defined(RTE_SPI3) && RTE_SPI3
        if(spi_id == 3 )
        {
            *instance = 3;
            return (struct _ARM_DRIVER_SPI *)&Driver_SPI3;
        }
        #endif /* RTE_SPI3 */
        #if defined(RTE_SPI4) && RTE_SPI4
        if(spi_id == 4 )
        {
            *instance = 4;
            return (struct _ARM_DRIVER_SPI *)&Driver_SPI4;
        }
        #endif /* RTE_SPI4 */
        #if defined(RTE_SPI5) && RTE_SPI5
        if(spi_id == 5 )
        {
            *instance = 5;
            return (struct _ARM_DRIVER_SPI *)&Driver_SPI5;
        }
        #endif /* RTE_SPI5 */
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "SPI(%d) doesn't exist", spi_id));
    }
}

#if defined(RTE_SPI0) && RTE_SPI0
STATIC void DriverSPI0_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_spi_callback)[0];
    if(*cb != mp_const_none)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /* RTE_SPI0 */

#if defined(RTE_SPI1) && RTE_SPI1
STATIC void DriverSPI1_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_spi_callback)[1];
    if(*cb != mp_const_none)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /* RTE_SPI1 */

#if defined(RTE_SPI2) && RTE_SPI2
STATIC void DriverSPI2_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_spi_callback)[2];
    if(*cb != mp_const_none)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /* RTE_SPI2 */

#if defined(RTE_SPI3) && RTE_SPI3
STATIC void DriverSPI3_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_spi_callback)[3];
    if(*cb != mp_const_none)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /* RTE_SPI3 */

#if defined(RTE_SPI4) && RTE_SPI4
STATIC void DriverSPI4_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_spi_callback)[4];
    if(*cb != mp_const_none)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /* RTE_SPI4 */

#if defined(RTE_SPI5) && RTE_SPI5
STATIC void DriverSPI5_Signal_Event(uint32_t event)
{
    mp_obj_t *cb = NULL;
    cb = &MP_STATE_PORT(pyb_spi_callback)[5];
    if(*cb != mp_const_none)
    {
        mp_call_function_1_protected(*cb, mp_obj_new_int(event));
    }
}
#endif /* RTE_SPI5 */

ARM_SPI_SignalEvent_t spi_find_callback(uint8_t instance)
{
    #if defined(RTE_SPI0) && RTE_SPI0
    if(instance == 0)
    {
        return DriverSPI0_Signal_Event;
    }
    #endif /* RTE_SPI0 */
    #if defined(RTE_SPI1) && RTE_SPI1
    if(instance == 1)
    {
        return DriverSPI1_Signal_Event;
    }
    #endif /* RTE_SPI1 */
    #if defined(RTE_SPI2) && RTE_SPI2
    if(instance == 2)
    {
        return DriverSPI2_Signal_Event;
    }
    #endif /* RTE_SPI2 */
    #if defined(RTE_SPI3) && RTE_SPI3
    if(instance == 3)
    {
        return DriverSPI3_Signal_Event;
    }
    #endif /* RTE_SPI3 */
    #if defined(RTE_SPI4) && RTE_SPI4
    if(instance == 4)
    {
        return DriverSPI4_Signal_Event;
    }
    #endif /* RTE_SPI4 */
    #if defined(RTE_SPI5) && RTE_SPI5
    if(instance == 5)
    {
        return DriverSPI5_Signal_Event;
    }
    #endif /* RTE_SPI5 */
    return NULL;
}

#if defined(LPSPI0) && defined(RTE_SPI0) && RTE_SPI0
uint32_t LPSPI0_GetFreq(void)
{
    return CLOCK_GetFreq((clock_name_t) kCLOCK_Lpspi0);
}
#endif /* LPSPI0 */

#if defined(LPSPI1) && defined(RTE_SPI1) && RTE_SPI1
uint32_t LPSPI1_GetFreq(void)
{
    return CLOCK_GetFreq((clock_name_t) kCLOCK_Lpspi1);
}
#endif /* LPSPI1 */

#if defined(LPSPI2) && defined(RTE_SPI2) && RTE_SPI2
uint32_t LPSPI2_GetFreq(void)
{
    return CLOCK_GetFreq((clock_name_t) kCLOCK_Lpspi2);
}
#endif /* LPSPI2 */

#if defined(LPSPI3) && defined(RTE_SPI3) && RTE_SPI3
uint32_t LPSPI3_GetFreq(void)
{
    return CLOCK_GetFreq((clock_name_t) kCLOCK_Lpspi3);
}
#endif /* LPSPI3 */

#if defined(LPSPI4) && defined(RTE_SPI4) && RTE_SPI4
uint32_t LPSPI4_GetFreq(void)
{
    return CLOCK_GetFreq((clock_name_t) kCLOCK_Lpspi4);
}
#endif /* LPSPI4 */

#if defined(LPSPI5) && defined(RTE_SPI5) && RTE_SPI5
uint32_t LPSPI5_GetFreq(void)
{
    return CLOCK_GetFreq((clock_name_t) kCLOCK_Lpspi5);
}
#endif /* LPSPI5 */

#if defined(DSPI0) && defined(RTE_SPI0) && RTE_SPI0
uint32_t DSPI0_GetFreq(void)
{
    return CLOCK_GetBusClkFreq();
}
#endif /* DSPI0 */

#if defined(DSPI1) && defined(RTE_SPI1) && RTE_SPI1
uint32_t DSPI1_GetFreq(void)
{
    return CLOCK_GetBusClkFreq();
}
#endif /* DSPI1 */

#if defined(DSPI2) && defined(RTE_SPI2) && RTE_SPI2
uint32_t DSPI2_GetFreq(void)
{
    return CLOCK_GetBusClkFreq();
}
#endif /* DSPI2 */
#endif  /* BOARD_SPI_CMSIS_HEADER */


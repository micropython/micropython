//*****************************************************************************
//
//  Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#ifndef __HW_MEMMAP_H__
#define __HW_MEMMAP_H__

//*****************************************************************************
//
// The following are defines for the base address of the memories and
// peripherals on the slave_1 interface.
//
//*****************************************************************************
#define FLASH_BASE              0x01000000
#define SRAM_BASE               0x20000000
#define WDT_BASE                0x40000000
#define GPIOA0_BASE             0x40004000
#define GPIOA1_BASE             0x40005000
#define GPIOA2_BASE             0x40006000
#define GPIOA3_BASE             0x40007000
#define GPIOA4_BASE             0x40024000
#define UARTA0_BASE             0x4000C000
#define UARTA1_BASE             0x4000D000
#define I2CA0_BASE              0x40020000
#define TIMERA0_BASE            0x40030000
#define TIMERA1_BASE            0x40031000
#define TIMERA2_BASE            0x40032000
#define TIMERA3_BASE            0x40033000
#define STACKDIE_CTRL_BASE      0x400F5000
#define COMMON_REG_BASE         0x400F7000
#define FLASH_CONTROL_BASE      0x400FD000
#define SYSTEM_CONTROL_BASE     0x400FE000
#define UDMA_BASE               0x400FF000
#define SDHOST_BASE             0x44010000
#define CAMERA_BASE             0x44018000
#define I2S_BASE                0x4401C000
#define SSPI_BASE               0x44020000
#define GSPI_BASE               0x44021000
#define LSPI_BASE               0x44022000
#define ARCM_BASE               0x44025000
#define APPS_CONFIG_BASE        0x44026000
#define GPRCM_BASE              0x4402D000
#define OCP_SHARED_BASE         0x4402E000
#define ADC_BASE                0x4402E800
#define HIB1P2_BASE             0x4402F000
#define HIB3P3_BASE             0x4402F800
#define DTHE_BASE               0x44030000
#define SHAMD5_BASE             0x44035000
#define AES_BASE                0x44037000
#define DES_BASE                0x44039000


#endif // __HW_MEMMAP_H__

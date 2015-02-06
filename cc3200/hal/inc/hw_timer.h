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

//*****************************************************************************
//
// hw_timer.h - Defines and macros used when accessing the timer.
//
//*****************************************************************************

//##### INTERNAL BEGIN #####
//
// This is an auto-generated file.  Do not edit by hand.
// Created by version 6779 of DriverLib.
//
//##### INTERNAL END #####

#ifndef __HW_TIMER_H__
#define __HW_TIMER_H__

//*****************************************************************************
//
// The following are defines for the Timer register offsets.
//
//*****************************************************************************
#define TIMER_O_CFG             0x00000000  // GPTM Configuration
#define TIMER_O_TAMR            0x00000004  // GPTM Timer A Mode
#define TIMER_O_TBMR            0x00000008  // GPTM Timer B Mode
#define TIMER_O_CTL             0x0000000C  // GPTM Control
//##### GARNET BEGIN #####
#define TIMER_O_SYNC            0x00000010  // GPTM Synchronize
//##### GARNET END #####
#define TIMER_O_IMR             0x00000018  // GPTM Interrupt Mask
#define TIMER_O_RIS             0x0000001C  // GPTM Raw Interrupt Status
#define TIMER_O_MIS             0x00000020  // GPTM Masked Interrupt Status
#define TIMER_O_ICR             0x00000024  // GPTM Interrupt Clear
#define TIMER_O_TAILR           0x00000028  // GPTM Timer A Interval Load
#define TIMER_O_TBILR           0x0000002C  // GPTM Timer B Interval Load
#define TIMER_O_TAMATCHR        0x00000030  // GPTM Timer A Match
#define TIMER_O_TBMATCHR        0x00000034  // GPTM Timer B Match
#define TIMER_O_TAPR            0x00000038  // GPTM Timer A Prescale
#define TIMER_O_TBPR            0x0000003C  // GPTM Timer B Prescale
#define TIMER_O_TAPMR           0x00000040  // GPTM TimerA Prescale Match
#define TIMER_O_TBPMR           0x00000044  // GPTM TimerB Prescale Match
#define TIMER_O_TAR             0x00000048  // GPTM Timer A
#define TIMER_O_TBR             0x0000004C  // GPTM Timer B
#define TIMER_O_TAV             0x00000050  // GPTM Timer A Value
#define TIMER_O_TBV             0x00000054  // GPTM Timer B Value
#define TIMER_O_RTCPD           0x00000058  // GPTM RTC Predivide
#define TIMER_O_TAPS            0x0000005C  // GPTM Timer A Prescale Snapshot
#define TIMER_O_TBPS            0x00000060  // GPTM Timer B Prescale Snapshot
#define TIMER_O_TAPV            0x00000064  // GPTM Timer A Prescale Value
#define TIMER_O_TBPV            0x00000068  // GPTM Timer B Prescale Value
#define TIMER_O_DMAEV           0x0000006C  // GPTM DMA Event
#define TIMER_O_PP              0x00000FC0  // GPTM Peripheral Properties


//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_CFG register.
//
//*****************************************************************************
#define TIMER_CFG_M             0x00000007  // GPTM Configuration
#define TIMER_CFG_32_BIT_TIMER  0x00000000  // 32-bit timer configuration
#define TIMER_CFG_32_BIT_RTC    0x00000001  // 32-bit real-time clock (RTC)
                                            // counter configuration
#define TIMER_CFG_16_BIT        0x00000004  // 16-bit timer configuration. The
                                            // function is controlled by bits
                                            // 1:0 of GPTMTAMR and GPTMTBMR

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TAMR register.
//
//*****************************************************************************
//##### GARNET BEGIN #####
#define TIMER_TAMR_TAPLO        0x00000800  // GPTM Timer A PWM Legacy
                                            // Operation
#define TIMER_TAMR_TAMRSU       0x00000400  // GPTM Timer A Match Register
                                            // Update
#define TIMER_TAMR_TAPWMIE      0x00000200  // GPTM Timer A PWM Interrupt
                                            // Enable
#define TIMER_TAMR_TAILD        0x00000100  // GPTM Timer A Interval Load Write
//##### GARNET END #####
#define TIMER_TAMR_TASNAPS      0x00000080  // GPTM Timer A Snap-Shot Mode
#define TIMER_TAMR_TAWOT        0x00000040  // GPTM Timer A Wait-on-Trigger
#define TIMER_TAMR_TAMIE        0x00000020  // GPTM Timer A Match Interrupt
                                            // Enable
#define TIMER_TAMR_TACDIR       0x00000010  // GPTM Timer A Count Direction
#define TIMER_TAMR_TAAMS        0x00000008  // GPTM Timer A Alternate Mode
                                            // Select
#define TIMER_TAMR_TACMR        0x00000004  // GPTM Timer A Capture Mode
#define TIMER_TAMR_TAMR_M       0x00000003  // GPTM Timer A Mode
#define TIMER_TAMR_TAMR_1_SHOT  0x00000001  // One-Shot Timer mode
#define TIMER_TAMR_TAMR_PERIOD  0x00000002  // Periodic Timer mode
#define TIMER_TAMR_TAMR_CAP     0x00000003  // Capture mode

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TBMR register.
//
//*****************************************************************************
//##### GARNET BEGIN #####
#define TIMER_TBMR_TBPLO        0x00000800  // GPTM Timer B PWM Legacy
                                            // Operation
#define TIMER_TBMR_TBMRSU       0x00000400  // GPTM Timer B Match Register
                                            // Update
#define TIMER_TBMR_TBPWMIE      0x00000200  // GPTM Timer B PWM Interrupt
                                            // Enable
#define TIMER_TBMR_TBILD        0x00000100  // GPTM Timer B Interval Load Write
//##### GARNET END #####
#define TIMER_TBMR_TBSNAPS      0x00000080  // GPTM Timer B Snap-Shot Mode
#define TIMER_TBMR_TBWOT        0x00000040  // GPTM Timer B Wait-on-Trigger
#define TIMER_TBMR_TBMIE        0x00000020  // GPTM Timer B Match Interrupt
                                            // Enable
#define TIMER_TBMR_TBCDIR       0x00000010  // GPTM Timer B Count Direction
#define TIMER_TBMR_TBAMS        0x00000008  // GPTM Timer B Alternate Mode
                                            // Select
#define TIMER_TBMR_TBCMR        0x00000004  // GPTM Timer B Capture Mode
#define TIMER_TBMR_TBMR_M       0x00000003  // GPTM Timer B Mode
#define TIMER_TBMR_TBMR_1_SHOT  0x00000001  // One-Shot Timer mode
#define TIMER_TBMR_TBMR_PERIOD  0x00000002  // Periodic Timer mode
#define TIMER_TBMR_TBMR_CAP     0x00000003  // Capture mode

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_CTL register.
//
//*****************************************************************************
#define TIMER_CTL_TBPWML        0x00004000  // GPTM Timer B PWM Output Level
#define TIMER_CTL_TBOTE         0x00002000  // GPTM Timer B Output Trigger
                                            // Enable
#define TIMER_CTL_TBEVENT_M     0x00000C00  // GPTM Timer B Event Mode
#define TIMER_CTL_TBEVENT_POS   0x00000000  // Positive edge
#define TIMER_CTL_TBEVENT_NEG   0x00000400  // Negative edge
#define TIMER_CTL_TBEVENT_BOTH  0x00000C00  // Both edges
#define TIMER_CTL_TBSTALL       0x00000200  // GPTM Timer B Stall Enable
#define TIMER_CTL_TBEN          0x00000100  // GPTM Timer B Enable
#define TIMER_CTL_TAPWML        0x00000040  // GPTM Timer A PWM Output Level
#define TIMER_CTL_TAOTE         0x00000020  // GPTM Timer A Output Trigger
                                            // Enable
#define TIMER_CTL_RTCEN         0x00000010  // GPTM RTC Enable
#define TIMER_CTL_TAEVENT_M     0x0000000C  // GPTM Timer A Event Mode
#define TIMER_CTL_TAEVENT_POS   0x00000000  // Positive edge
#define TIMER_CTL_TAEVENT_NEG   0x00000004  // Negative edge
#define TIMER_CTL_TAEVENT_BOTH  0x0000000C  // Both edges
#define TIMER_CTL_TASTALL       0x00000002  // GPTM Timer A Stall Enable
#define TIMER_CTL_TAEN          0x00000001  // GPTM Timer A Enable
//##### GARNET BEGIN #####

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_SYNC register.
//
//*****************************************************************************
#define TIMER_SYNC_SYNC11_M     0x00C00000  // Synchronize GPTM Timer 11
#define TIMER_SYNC_SYNC11_TA    0x00400000  // A timeout event for Timer A of
                                            // GPTM11 is triggered
#define TIMER_SYNC_SYNC11_TB    0x00800000  // A timeout event for Timer B of
                                            // GPTM11 is triggered
#define TIMER_SYNC_SYNC11_TATB  0x00C00000  // A timeout event for both Timer A
                                            // and Timer B of GPTM11 is
                                            // triggered
#define TIMER_SYNC_SYNC10_M     0x00300000  // Synchronize GPTM Timer 10
#define TIMER_SYNC_SYNC10_TA    0x00100000  // A timeout event for Timer A of
                                            // GPTM10 is triggered
#define TIMER_SYNC_SYNC10_TB    0x00200000  // A timeout event for Timer B of
                                            // GPTM10 is triggered
#define TIMER_SYNC_SYNC10_TATB  0x00300000  // A timeout event for both Timer A
                                            // and Timer B of GPTM10 is
                                            // triggered
#define TIMER_SYNC_SYNC9_M      0x000C0000  // Synchronize GPTM Timer 9
#define TIMER_SYNC_SYNC9_TA     0x00040000  // A timeout event for Timer A of
                                            // GPTM9 is triggered
#define TIMER_SYNC_SYNC9_TB     0x00080000  // A timeout event for Timer B of
                                            // GPTM9 is triggered
#define TIMER_SYNC_SYNC9_TATB   0x000C0000  // A timeout event for both Timer A
                                            // and Timer B of GPTM9 is
                                            // triggered
#define TIMER_SYNC_SYNC8_M      0x00030000  // Synchronize GPTM Timer 8
#define TIMER_SYNC_SYNC8_TA     0x00010000  // A timeout event for Timer A of
                                            // GPTM8 is triggered
#define TIMER_SYNC_SYNC8_TB     0x00020000  // A timeout event for Timer B of
                                            // GPTM8 is triggered
#define TIMER_SYNC_SYNC8_TATB   0x00030000  // A timeout event for both Timer A
                                            // and Timer B of GPTM8 is
                                            // triggered
#define TIMER_SYNC_SYNC7_M      0x0000C000  // Synchronize GPTM Timer 7
#define TIMER_SYNC_SYNC7_TA     0x00004000  // A timeout event for Timer A of
                                            // GPTM7 is triggered
#define TIMER_SYNC_SYNC7_TB     0x00008000  // A timeout event for Timer B of
                                            // GPTM7 is triggered
#define TIMER_SYNC_SYNC7_TATB   0x0000C000  // A timeout event for both Timer A
                                            // and Timer B of GPTM7 is
                                            // triggered
#define TIMER_SYNC_SYNC6_M      0x00003000  // Synchronize GPTM Timer 6
#define TIMER_SYNC_SYNC6_TA     0x00001000  // A timeout event for Timer A of
                                            // GPTM6 is triggered
#define TIMER_SYNC_SYNC6_TB     0x00002000  // A timeout event for Timer B of
                                            // GPTM6 is triggered
#define TIMER_SYNC_SYNC6_TATB   0x00003000  // A timeout event for both Timer A
                                            // and Timer B of GPTM6 is
                                            // triggered
#define TIMER_SYNC_SYNC5_M      0x00000C00  // Synchronize GPTM Timer 5
#define TIMER_SYNC_SYNC5_TA     0x00000400  // A timeout event for Timer A of
                                            // GPTM5 is triggered
#define TIMER_SYNC_SYNC5_TB     0x00000800  // A timeout event for Timer B of
                                            // GPTM5 is triggered
#define TIMER_SYNC_SYNC5_TATB   0x00000C00  // A timeout event for both Timer A
                                            // and Timer B of GPTM5 is
                                            // triggered
#define TIMER_SYNC_SYNC4_M      0x00000300  // Synchronize GPTM Timer 4
#define TIMER_SYNC_SYNC4_TA     0x00000100  // A timeout event for Timer A of
                                            // GPTM4 is triggered
#define TIMER_SYNC_SYNC4_TB     0x00000200  // A timeout event for Timer B of
                                            // GPTM4 is triggered
#define TIMER_SYNC_SYNC4_TATB   0x00000300  // A timeout event for both Timer A
                                            // and Timer B of GPTM4 is
                                            // triggered
#define TIMER_SYNC_SYNC3_M      0x000000C0  // Synchronize GPTM Timer 3
#define TIMER_SYNC_SYNC3_TA     0x00000040  // A timeout event for Timer A of
                                            // GPTM3 is triggered
#define TIMER_SYNC_SYNC3_TB     0x00000080  // A timeout event for Timer B of
                                            // GPTM3 is triggered
#define TIMER_SYNC_SYNC3_TATB   0x000000C0  // A timeout event for both Timer A
                                            // and Timer B of GPTM3 is
                                            // triggered
#define TIMER_SYNC_SYNC2_M      0x00000030  // Synchronize GPTM Timer 2
#define TIMER_SYNC_SYNC2_TA     0x00000010  // A timeout event for Timer A of
                                            // GPTM2 is triggered
#define TIMER_SYNC_SYNC2_TB     0x00000020  // A timeout event for Timer B of
                                            // GPTM2 is triggered
#define TIMER_SYNC_SYNC2_TATB   0x00000030  // A timeout event for both Timer A
                                            // and Timer B of GPTM2 is
                                            // triggered
#define TIMER_SYNC_SYNC1_M      0x0000000C  // Synchronize GPTM Timer 1
#define TIMER_SYNC_SYNC1_TA     0x00000004  // A timeout event for Timer A of
                                            // GPTM1 is triggered
#define TIMER_SYNC_SYNC1_TB     0x00000008  // A timeout event for Timer B of
                                            // GPTM1 is triggered
#define TIMER_SYNC_SYNC1_TATB   0x0000000C  // A timeout event for both Timer A
                                            // and Timer B of GPTM1 is
                                            // triggered
#define TIMER_SYNC_SYNC0_M      0x00000003  // Synchronize GPTM Timer 0
#define TIMER_SYNC_SYNC0_TA     0x00000001  // A timeout event for Timer A of
                                            // GPTM0 is triggered
#define TIMER_SYNC_SYNC0_TB     0x00000002  // A timeout event for Timer B of
                                            // GPTM0 is triggered
#define TIMER_SYNC_SYNC0_TATB   0x00000003  // A timeout event for both Timer A
                                            // and Timer B of GPTM0 is
                                            // triggered
//##### GARNET END #####

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_IMR register.
//
//*****************************************************************************
//##### GARNET BEGIN #####
#define TIMER_IMR_WUEIM         0x00010000  // 32/64-Bit GPTM Write Update
                                            // Error Interrupt Mask
//##### GARNET END #####
#define TIMER_IMR_TBMIM         0x00000800  // GPTM Timer B Mode Match
                                            // Interrupt Mask
#define TIMER_IMR_CBEIM         0x00000400  // GPTM Capture B Event Interrupt
                                            // Mask
#define TIMER_IMR_CBMIM         0x00000200  // GPTM Capture B Match Interrupt
                                            // Mask
#define TIMER_IMR_TBTOIM        0x00000100  // GPTM Timer B Time-Out Interrupt
                                            // Mask
#define TIMER_IMR_TAMIM         0x00000010  // GPTM Timer A Mode Match
                                            // Interrupt Mask
#define TIMER_IMR_RTCIM         0x00000008  // GPTM RTC Interrupt Mask
#define TIMER_IMR_CAEIM         0x00000004  // GPTM Capture A Event Interrupt
                                            // Mask
#define TIMER_IMR_CAMIM         0x00000002  // GPTM Capture A Match Interrupt
                                            // Mask
#define TIMER_IMR_TATOIM        0x00000001  // GPTM Timer A Time-Out Interrupt
                                            // Mask

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_RIS register.
//
//*****************************************************************************
//##### GARNET BEGIN #####
#define TIMER_RIS_WUERIS        0x00010000  // 32/64-Bit GPTM Write Update
                                            // Error Raw Interrupt Status
//##### GARNET END #####
#define TIMER_RIS_TBMRIS        0x00000800  // GPTM Timer B Mode Match Raw
                                            // Interrupt
#define TIMER_RIS_CBERIS        0x00000400  // GPTM Capture B Event Raw
                                            // Interrupt
#define TIMER_RIS_CBMRIS        0x00000200  // GPTM Capture B Match Raw
                                            // Interrupt
#define TIMER_RIS_TBTORIS       0x00000100  // GPTM Timer B Time-Out Raw
                                            // Interrupt
#define TIMER_RIS_TAMRIS        0x00000010  // GPTM Timer A Mode Match Raw
                                            // Interrupt
#define TIMER_RIS_RTCRIS        0x00000008  // GPTM RTC Raw Interrupt
#define TIMER_RIS_CAERIS        0x00000004  // GPTM Capture A Event Raw
                                            // Interrupt
#define TIMER_RIS_CAMRIS        0x00000002  // GPTM Capture A Match Raw
                                            // Interrupt
#define TIMER_RIS_TATORIS       0x00000001  // GPTM Timer A Time-Out Raw
                                            // Interrupt

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_MIS register.
//
//*****************************************************************************
//##### GARNET BEGIN #####
#define TIMER_MIS_WUEMIS        0x00010000  // 32/64-Bit GPTM Write Update
                                            // Error Masked Interrupt Status
//##### GARNET END #####
#define TIMER_MIS_TBMMIS        0x00000800  // GPTM Timer B Mode Match Masked
                                            // Interrupt
#define TIMER_MIS_CBEMIS        0x00000400  // GPTM Capture B Event Masked
                                            // Interrupt
#define TIMER_MIS_CBMMIS        0x00000200  // GPTM Capture B Match Masked
                                            // Interrupt
#define TIMER_MIS_TBTOMIS       0x00000100  // GPTM Timer B Time-Out Masked
                                            // Interrupt
#define TIMER_MIS_TAMMIS        0x00000010  // GPTM Timer A Mode Match Masked
                                            // Interrupt
#define TIMER_MIS_RTCMIS        0x00000008  // GPTM RTC Masked Interrupt
#define TIMER_MIS_CAEMIS        0x00000004  // GPTM Capture A Event Masked
                                            // Interrupt
#define TIMER_MIS_CAMMIS        0x00000002  // GPTM Capture A Match Masked
                                            // Interrupt
#define TIMER_MIS_TATOMIS       0x00000001  // GPTM Timer A Time-Out Masked
                                            // Interrupt

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_ICR register.
//
//*****************************************************************************
//##### GARNET BEGIN #####
#define TIMER_ICR_WUECINT       0x00010000  // 32/64-Bit GPTM Write Update
                                            // Error Interrupt Clear
//##### GARNET END #####
#define TIMER_ICR_TBMCINT       0x00000800  // GPTM Timer B Mode Match
                                            // Interrupt Clear
#define TIMER_ICR_CBECINT       0x00000400  // GPTM Capture B Event Interrupt
                                            // Clear
#define TIMER_ICR_CBMCINT       0x00000200  // GPTM Capture B Match Interrupt
                                            // Clear
#define TIMER_ICR_TBTOCINT      0x00000100  // GPTM Timer B Time-Out Interrupt
                                            // Clear
#define TIMER_ICR_TAMCINT       0x00000010  // GPTM Timer A Mode Match
                                            // Interrupt Clear
#define TIMER_ICR_RTCCINT       0x00000008  // GPTM RTC Interrupt Clear
#define TIMER_ICR_CAECINT       0x00000004  // GPTM Capture A Event Interrupt
                                            // Clear
#define TIMER_ICR_CAMCINT       0x00000002  // GPTM Capture A Match Interrupt
                                            // Clear
#define TIMER_ICR_TATOCINT      0x00000001  // GPTM Timer A Time-Out Raw
                                            // Interrupt

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TAILR register.
//
//*****************************************************************************
//##### GARNET BEGIN #####
#define TIMER_TAILR_M           0xFFFFFFFF  // GPTM Timer A Interval Load
                                            // Register
//##### GARNET END #####
#define TIMER_TAILR_TAILRH_M    0xFFFF0000  // GPTM Timer A Interval Load
                                            // Register High
#define TIMER_TAILR_TAILRL_M    0x0000FFFF  // GPTM Timer A Interval Load
                                            // Register Low
#define TIMER_TAILR_TAILRH_S    16
#define TIMER_TAILR_TAILRL_S    0
//##### GARNET BEGIN #####
#define TIMER_TAILR_S           0
//##### GARNET END #####

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TBILR register.
//
//*****************************************************************************
//##### GARNET BEGIN #####
#define TIMER_TBILR_M           0xFFFFFFFF  // GPTM Timer B Interval Load
                                            // Register
//##### GARNET END #####
#define TIMER_TBILR_TBILRL_M    0x0000FFFF  // GPTM Timer B Interval Load
                                            // Register
#define TIMER_TBILR_TBILRL_S    0
//##### GARNET BEGIN #####
#define TIMER_TBILR_S           0
//##### GARNET END #####

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TAMATCHR
// register.
//
//*****************************************************************************
//##### GARNET BEGIN #####
#define TIMER_TAMATCHR_TAMR_M   0xFFFFFFFF  // GPTM Timer A Match Register
//##### GARNET END #####
#define TIMER_TAMATCHR_TAMRH_M  0xFFFF0000  // GPTM Timer A Match Register High
#define TIMER_TAMATCHR_TAMRL_M  0x0000FFFF  // GPTM Timer A Match Register Low
#define TIMER_TAMATCHR_TAMRH_S  16
#define TIMER_TAMATCHR_TAMRL_S  0
//##### GARNET BEGIN #####
#define TIMER_TAMATCHR_TAMR_S   0
//##### GARNET END #####

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TBMATCHR
// register.
//
//*****************************************************************************
//##### GARNET BEGIN #####
#define TIMER_TBMATCHR_TBMR_M   0xFFFFFFFF  // GPTM Timer B Match Register
//##### GARNET END #####
#define TIMER_TBMATCHR_TBMRL_M  0x0000FFFF  // GPTM Timer B Match Register Low
//##### GARNET BEGIN #####
#define TIMER_TBMATCHR_TBMR_S   0
//##### GARNET END #####
#define TIMER_TBMATCHR_TBMRL_S  0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TAPR register.
//
//*****************************************************************************
//##### GARNET BEGIN #####
#define TIMER_TAPR_TAPSRH_M     0x0000FF00  // GPTM Timer A Prescale High Byte
//##### GARNET END #####
#define TIMER_TAPR_TAPSR_M      0x000000FF  // GPTM Timer A Prescale
//##### GARNET BEGIN #####
#define TIMER_TAPR_TAPSRH_S     8
//##### GARNET END #####
#define TIMER_TAPR_TAPSR_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TBPR register.
//
//*****************************************************************************
//##### GARNET BEGIN #####
#define TIMER_TBPR_TBPSRH_M     0x0000FF00  // GPTM Timer B Prescale High Byte
//##### GARNET END #####
#define TIMER_TBPR_TBPSR_M      0x000000FF  // GPTM Timer B Prescale
//##### GARNET BEGIN #####
#define TIMER_TBPR_TBPSRH_S     8
//##### GARNET END #####
#define TIMER_TBPR_TBPSR_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TAPMR register.
//
//*****************************************************************************
//##### GARNET BEGIN #####
#define TIMER_TAPMR_TAPSMRH_M   0x0000FF00  // GPTM Timer A Prescale Match High
                                            // Byte
//##### GARNET END #####
#define TIMER_TAPMR_TAPSMR_M    0x000000FF  // GPTM TimerA Prescale Match
//##### GARNET BEGIN #####
#define TIMER_TAPMR_TAPSMRH_S   8
//##### GARNET END #####
#define TIMER_TAPMR_TAPSMR_S    0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TBPMR register.
//
//*****************************************************************************
//##### GARNET BEGIN #####
#define TIMER_TBPMR_TBPSMRH_M   0x0000FF00  // GPTM Timer B Prescale Match High
                                            // Byte
//##### GARNET END #####
#define TIMER_TBPMR_TBPSMR_M    0x000000FF  // GPTM TimerB Prescale Match
//##### GARNET BEGIN #####
#define TIMER_TBPMR_TBPSMRH_S   8
//##### GARNET END #####
#define TIMER_TBPMR_TBPSMR_S    0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TAR register.
//
//*****************************************************************************
//##### GARNET BEGIN #####
#define TIMER_TAR_M             0xFFFFFFFF  // GPTM Timer A Register
//##### GARNET END #####
#define TIMER_TAR_TARH_M        0xFFFF0000  // GPTM Timer A Register High
#define TIMER_TAR_TARL_M        0x0000FFFF  // GPTM Timer A Register Low
#define TIMER_TAR_TARH_S        16
#define TIMER_TAR_TARL_S        0
//##### GARNET BEGIN #####
#define TIMER_TAR_S             0
//##### GARNET END #####

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TBR register.
//
//*****************************************************************************
//##### GARNET BEGIN #####
#define TIMER_TBR_M             0xFFFFFFFF  // GPTM Timer B Register
//##### GARNET END #####
#define TIMER_TBR_TBRL_M        0x00FFFFFF  // GPTM Timer B
#define TIMER_TBR_TBRL_S        0
//##### GARNET BEGIN #####
#define TIMER_TBR_S             0
//##### GARNET END #####

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TAV register.
//
//*****************************************************************************
//##### GARNET BEGIN #####
#define TIMER_TAV_M             0xFFFFFFFF  // GPTM Timer A Value
//##### GARNET END #####
#define TIMER_TAV_TAVH_M        0xFFFF0000  // GPTM Timer A Value High
#define TIMER_TAV_TAVL_M        0x0000FFFF  // GPTM Timer A Register Low
#define TIMER_TAV_TAVH_S        16
#define TIMER_TAV_TAVL_S        0
//##### GARNET BEGIN #####
#define TIMER_TAV_S             0
//##### GARNET END #####

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TBV register.
//
//*****************************************************************************
//##### GARNET BEGIN #####
#define TIMER_TBV_M             0xFFFFFFFF  // GPTM Timer B Value
//##### GARNET END #####
#define TIMER_TBV_TBVL_M        0x0000FFFF  // GPTM Timer B Register
#define TIMER_TBV_TBVL_S        0
//##### GARNET BEGIN #####
#define TIMER_TBV_S             0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_RTCPD register.
//
//*****************************************************************************
#define TIMER_RTCPD_RTCPD_M     0x0000FFFF  // RTC Predivide Counter Value
#define TIMER_RTCPD_RTCPD_S     0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TAPS register.
//
//*****************************************************************************
#define TIMER_TAPS_PSS_M        0x0000FFFF  // GPTM Timer A Prescaler Snapshot
#define TIMER_TAPS_PSS_S        0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TBPS register.
//
//*****************************************************************************
#define TIMER_TBPS_PSS_M        0x0000FFFF  // GPTM Timer A Prescaler Value
#define TIMER_TBPS_PSS_S        0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TAPV register.
//
//*****************************************************************************
#define TIMER_TAPV_PSV_M        0x0000FFFF  // GPTM Timer A Prescaler Value
#define TIMER_TAPV_PSV_S        0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TBPV register.
//
//*****************************************************************************
#define TIMER_TBPV_PSV_M        0x0000FFFF  // GPTM Timer B Prescaler Value
#define TIMER_TBPV_PSV_S        0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_PP register.
//
//*****************************************************************************
#define TIMER_PP_SYNCCNT        0x00000020  // Synchronize Start
#define TIMER_PP_CHAIN          0x00000010  // Chain with Other Timers
#define TIMER_PP_SIZE_M         0x0000000F  // Count Size
#define TIMER_PP_SIZE__0        0x00000000  // Timer A and Timer B counters are
                                            // 16 bits each with an 8-bit
                                            // prescale counter
#define TIMER_PP_SIZE__1        0x00000001  // Timer A and Timer B counters are
                                            // 32 bits each with an 16-bit
                                            // prescale counter
//##### GARNET END #####

//*****************************************************************************
//
// The following definitions are deprecated.
//
//*****************************************************************************
#ifndef DEPRECATED

//*****************************************************************************
//
// The following are deprecated defines for the bit fields in the TIMER_O_CFG
// register.
//
//*****************************************************************************
#define TIMER_CFG_CFG_MSK       0x00000007  // Configuration options mask

//*****************************************************************************
//
// The following are deprecated defines for the bit fields in the TIMER_O_CTL
// register.
//
//*****************************************************************************
#define TIMER_CTL_TBEVENT_MSK   0x00000C00  // TimerB event mode mask
#define TIMER_CTL_TAEVENT_MSK   0x0000000C  // TimerA event mode mask

//*****************************************************************************
//
// The following are deprecated defines for the bit fields in the TIMER_O_RIS
// register.
//
//*****************************************************************************
#define TIMER_RIS_CBEMIS        0x00000400  // CaptureB event masked int status
#define TIMER_RIS_CBMMIS        0x00000200  // CaptureB match masked int status
#define TIMER_RIS_TBTOMIS       0x00000100  // TimerB time out masked int stat
#define TIMER_RIS_RTCMIS        0x00000008  // RTC masked int status
#define TIMER_RIS_CAEMIS        0x00000004  // CaptureA event masked int status
#define TIMER_RIS_CAMMIS        0x00000002  // CaptureA match masked int status
#define TIMER_RIS_TATOMIS       0x00000001  // TimerA time out masked int stat

//*****************************************************************************
//
// The following are deprecated defines for the bit fields in the TIMER_O_TAILR
// register.
//
//*****************************************************************************
#define TIMER_TAILR_TAILRH      0xFFFF0000  // TimerB load val in 32 bit mode
#define TIMER_TAILR_TAILRL      0x0000FFFF  // TimerA interval load value

//*****************************************************************************
//
// The following are deprecated defines for the bit fields in the TIMER_O_TBILR
// register.
//
//*****************************************************************************
#define TIMER_TBILR_TBILRL      0x0000FFFF  // TimerB interval load value

//*****************************************************************************
//
// The following are deprecated defines for the bit fields in the
// TIMER_O_TAMATCHR register.
//
//*****************************************************************************
#define TIMER_TAMATCHR_TAMRH    0xFFFF0000  // TimerB match val in 32 bit mode
#define TIMER_TAMATCHR_TAMRL    0x0000FFFF  // TimerA match value

//*****************************************************************************
//
// The following are deprecated defines for the bit fields in the
// TIMER_O_TBMATCHR register.
//
//*****************************************************************************
#define TIMER_TBMATCHR_TBMRL    0x0000FFFF  // TimerB match load value

//*****************************************************************************
//
// The following are deprecated defines for the bit fields in the TIMER_O_TAR
// register.
//
//*****************************************************************************
#define TIMER_TAR_TARH          0xFFFF0000  // TimerB val in 32 bit mode
#define TIMER_TAR_TARL          0x0000FFFF  // TimerA value

//*****************************************************************************
//
// The following are deprecated defines for the bit fields in the TIMER_O_TBR
// register.
//
//*****************************************************************************
#define TIMER_TBR_TBRL          0x0000FFFF  // TimerB value

//*****************************************************************************
//
// The following are deprecated defines for the reset values of the timer
// registers.
//
//*****************************************************************************
#define TIMER_RV_TAILR          0xFFFFFFFF  // TimerA interval load reg RV
#define TIMER_RV_TAR            0xFFFFFFFF  // TimerA register RV
#define TIMER_RV_TAMATCHR       0xFFFFFFFF  // TimerA match register RV
#define TIMER_RV_TBILR          0x0000FFFF  // TimerB interval load reg RV
#define TIMER_RV_TBMATCHR       0x0000FFFF  // TimerB match register RV
#define TIMER_RV_TBR            0x0000FFFF  // TimerB register RV
#define TIMER_RV_TAPR           0x00000000  // TimerA prescale register RV
#define TIMER_RV_CFG            0x00000000  // Configuration register RV
#define TIMER_RV_TBPMR          0x00000000  // TimerB prescale match regi RV
#define TIMER_RV_TAPMR          0x00000000  // TimerA prescale match reg RV
#define TIMER_RV_CTL            0x00000000  // Control register RV
#define TIMER_RV_ICR            0x00000000  // Interrupt clear register RV
#define TIMER_RV_TBMR           0x00000000  // TimerB mode register RV
#define TIMER_RV_MIS            0x00000000  // Masked interrupt status reg RV
#define TIMER_RV_RIS            0x00000000  // Interrupt status register RV
#define TIMER_RV_TBPR           0x00000000  // TimerB prescale register RV
#define TIMER_RV_IMR            0x00000000  // Interrupt mask register RV
#define TIMER_RV_TAMR           0x00000000  // TimerA mode register RV

//*****************************************************************************
//
// The following are deprecated defines for the bit fields in the TIMER_TnMR
// register.
//
//*****************************************************************************
#define TIMER_TNMR_TNAMS        0x00000008  // Alternate mode select
#define TIMER_TNMR_TNCMR        0x00000004  // Capture mode - count or time
#define TIMER_TNMR_TNTMR_MSK    0x00000003  // Timer mode mask
#define TIMER_TNMR_TNTMR_1_SHOT 0x00000001  // Mode - one shot
#define TIMER_TNMR_TNTMR_PERIOD 0x00000002  // Mode - periodic
#define TIMER_TNMR_TNTMR_CAP    0x00000003  // Mode - capture

//*****************************************************************************
//
// The following are deprecated defines for the bit fields in the TIMER_TnPR
// register.
//
//*****************************************************************************
#define TIMER_TNPR_TNPSR        0x000000FF  // TimerN prescale value

//*****************************************************************************
//
// The following are deprecated defines for the bit fields in the TIMER_TnPMR
// register.
//
//*****************************************************************************
#define TIMER_TNPMR_TNPSMR      0x000000FF  // TimerN prescale match value

#endif

#endif // __HW_TIMER_H__

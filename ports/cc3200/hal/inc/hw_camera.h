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

#ifndef __HW_CAMERA_H__
#define __HW_CAMERA_H__

//*****************************************************************************
//
// The following are defines for the CAMERA register offsets.
//
//*****************************************************************************
#define CAMERA_O_CC_REVISION    0x00000000  // This register contains the IP
                                            // revision code ( Parallel Mode)
#define CAMERA_O_CC_SYSCONFIG   0x00000010  // This register controls the
                                            // various parameters of the OCP
                                            // interface (CCP and Parallel Mode)
#define CAMERA_O_CC_SYSSTATUS   0x00000014  // This register provides status
                                            // information about the module
                                            // excluding the interrupt status
                                            // information (CCP and Parallel
                                            // Mode)
#define CAMERA_O_CC_IRQSTATUS   0x00000018  // The interrupt status regroups
                                            // all the status of the module
                                            // internal events that can generate
                                            // an interrupt (CCP & Parallel
                                            // Mode)
#define CAMERA_O_CC_IRQENABLE   0x0000001C  // The interrupt enable register
                                            // allows to enable/disable the
                                            // module internal sources of
                                            // interrupt on an event-by-event
                                            // basis (CCP & Parallel Mode)
#define CAMERA_O_CC_CTRL        0x00000040  // This register controls the
                                            // various parameters of the Camera
                                            // Core block (CCP & Parallel Mode)
#define CAMERA_O_CC_CTRL_DMA    0x00000044  // This register controls the DMA
                                            // interface of the Camera Core
                                            // block (CCP & Parallel Mode)
#define CAMERA_O_CC_CTRL_XCLK   0x00000048  // This register control the value
                                            // of the clock divisor used to
                                            // generate the external clock
                                            // (Parallel Mode)
#define CAMERA_O_CC_FIFO_DATA   0x0000004C  // This register allows to write to
                                            // the FIFO and read from the FIFO
                                            // (CCP & Parallel Mode)
#define CAMERA_O_CC_TEST        0x00000050  // This register shows the status
                                            // of some important variables of
                                            // the camera core module (CCP &
                                            // Parallel Mode)
#define CAMERA_O_CC_GEN_PAR     0x00000054  // This register shows the values
                                            // of the generic parameters of the
                                            // module



//******************************************************************************
//
// The following are defines for the bit fields in the
// CAMERA_O_CC_REVISION register.
//
//******************************************************************************
#define CAMERA_CC_REVISION_REV_M \
                                0x000000FF  // IP revision [7:4] Major revision
                                            // [3:0] Minor revision Examples:
                                            // 0x10 for 1.0 0x21 for 2.1

#define CAMERA_CC_REVISION_REV_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// CAMERA_O_CC_SYSCONFIG register.
//
//******************************************************************************
#define CAMERA_CC_SYSCONFIG_S_IDLE_MODE_M \
                                0x00000018  // Slave interface power management
                                            // req/ack control """00""
                                            // Force-idle. An idle request is
                                            // acknoledged unconditionally"
                                            // """01"" No-idle. An idle request
                                            // is never acknowledged" """10""
                                            // reserved (Smart-idle not
                                            // implemented)"

#define CAMERA_CC_SYSCONFIG_S_IDLE_MODE_S 3
#define CAMERA_CC_SYSCONFIG_SOFT_RESET \
                                0x00000002  // Software reset. Set this bit to
                                            // 1 to trigger a module reset. The
                                            // bit is automatically reset by the
                                            // hardware. During reset it always
                                            // returns 0. 0 Normal mode 1 The
                                            // module is reset

#define CAMERA_CC_SYSCONFIG_AUTO_IDLE \
                                0x00000001  // Internal OCP clock gating
                                            // strategy 0 OCP clock is
                                            // free-running 1 Automatic OCP
                                            // clock gating strategy is applied
                                            // based on the OCP interface
                                            // activity

//******************************************************************************
//
// The following are defines for the bit fields in the
// CAMERA_O_CC_SYSSTATUS register.
//
//******************************************************************************
#define CAMERA_CC_SYSSTATUS_RESET_DONE2 \
                                0x00000001  // Internal Reset Monitoring 0
                                            // Internal module reset is on-going
                                            // 1 Reset completed

//******************************************************************************
//
// The following are defines for the bit fields in the
// CAMERA_O_CC_IRQSTATUS register.
//
//******************************************************************************
#define CAMERA_CC_IRQSTATUS_FS_IRQ \
                                0x00080000  // Frame Start has occurred 0 Event
                                            // false "1 Event is true
                                            // (""pending"")" 0 Event status bit
                                            // unchanged 1 Event status bit is
                                            // reset

#define CAMERA_CC_IRQSTATUS_LE_IRQ \
                                0x00040000  // Line End has occurred 0 Event
                                            // false "1 Event is true
                                            // (""pending"")" 0 Event status bit
                                            // unchanged 1 Event status bit is
                                            // reset

#define CAMERA_CC_IRQSTATUS_LS_IRQ \
                                0x00020000  // Line Start has occurred 0 Event
                                            // false "1 Event is true
                                            // (""pending"")" 0 Event status bit
                                            // unchanged 1 Event status bit is
                                            // reset

#define CAMERA_CC_IRQSTATUS_FE_IRQ \
                                0x00010000  // Frame End has occurred 0 Event
                                            // false "1 Event is true
                                            // (""pending"")" 0 Event status bit
                                            // unchanged 1 Event status bit is
                                            // reset

#define CAMERA_CC_IRQSTATUS_FSP_ERR_IRQ \
                                0x00000800  // FSP code error 0 Event false "1
                                            // Event is true (""pending"")" 0
                                            // Event status bit unchanged 1
                                            // Event status bit is reset

#define CAMERA_CC_IRQSTATUS_FW_ERR_IRQ \
                                0x00000400  // Frame Height Error 0 Event false
                                            // "1 Event is true (""pending"")" 0
                                            // Event status bit unchanged 1
                                            // Event status bit is reset

#define CAMERA_CC_IRQSTATUS_FSC_ERR_IRQ \
                                0x00000200  // False Synchronization Code 0
                                            // Event false "1 Event is true
                                            // (""pending"")" 0 Event status bit
                                            // unchanged 1 Event status bit is
                                            // reset

#define CAMERA_CC_IRQSTATUS_SSC_ERR_IRQ \
                                0x00000100  // Shifted Synchronization Code 0
                                            // Event false "1 Event is true
                                            // (""pending"")" 0 Event status bit
                                            // unchanged 1 Event status bit is
                                            // reset

#define CAMERA_CC_IRQSTATUS_FIFO_NONEMPTY_IRQ \
                                0x00000010  // FIFO is not empty 0 Event false
                                            // "1 Event is true (""pending"")" 0
                                            // Event status bit unchanged 1
                                            // Event status bit is reset

#define CAMERA_CC_IRQSTATUS_FIFO_FULL_IRQ \
                                0x00000008  // FIFO is full 0 Event false "1
                                            // Event is true (""pending"")" 0
                                            // Event status bit unchanged 1
                                            // Event status bit is reset

#define CAMERA_CC_IRQSTATUS_FIFO_THR_IRQ \
                                0x00000004  // FIFO threshold has been reached
                                            // 0 Event false "1 Event is true
                                            // (""pending"")" 0 Event status bit
                                            // unchanged 1 Event status bit is
                                            // reset

#define CAMERA_CC_IRQSTATUS_FIFO_OF_IRQ \
                                0x00000002  // FIFO overflow has occurred 0
                                            // Event false "1 Event is true
                                            // (""pending"")" 0 Event status bit
                                            // unchanged 1 Event status bit is
                                            // reset

#define CAMERA_CC_IRQSTATUS_FIFO_UF_IRQ \
                                0x00000001  // FIFO underflow has occurred 0
                                            // Event false "1 Event is true
                                            // (""pending"")" 0 Event status bit
                                            // unchanged 1 Event status bit is
                                            // reset

//******************************************************************************
//
// The following are defines for the bit fields in the
// CAMERA_O_CC_IRQENABLE register.
//
//******************************************************************************
#define CAMERA_CC_IRQENABLE_FS_IRQ_EN \
                                0x00080000  // Frame Start Interrupt Enable 0
                                            // Event is masked 1 Event generates
                                            // an interrupt when it occurs

#define CAMERA_CC_IRQENABLE_LE_IRQ_EN \
                                0x00040000  // Line End Interrupt Enable 0
                                            // Event is masked 1 Event generates
                                            // an interrupt when it occurs

#define CAMERA_CC_IRQENABLE_LS_IRQ_EN \
                                0x00020000  // Line Start Interrupt Enable 0
                                            // Event is masked 1 Event generates
                                            // an interrupt when it occurs

#define CAMERA_CC_IRQENABLE_FE_IRQ_EN \
                                0x00010000  // Frame End Interrupt Enable 0
                                            // Event is masked 1 Event generates
                                            // an interrupt when it occurs

#define CAMERA_CC_IRQENABLE_FSP_IRQ_EN \
                                0x00000800  // FSP code Interrupt Enable 0
                                            // Event is masked 1 Event generates
                                            // an interrupt when it occurs

#define CAMERA_CC_IRQENABLE_FW_ERR_IRQ_EN \
                                0x00000400  // Frame Height Error Interrupt
                                            // Enable 0 Event is masked 1 Event
                                            // generates an interrupt when it
                                            // occurs

#define CAMERA_CC_IRQENABLE_FSC_ERR_IRQ_EN \
                                0x00000200  // False Synchronization Code
                                            // Interrupt Enable 0 Event is
                                            // masked 1 Event generates an
                                            // interrupt when it occurs

#define CAMERA_CC_IRQENABLE_SSC_ERR_IRQ_EN \
                                0x00000100  // False Synchronization Code
                                            // Interrupt Enable 0 Event is
                                            // masked 1 Event generates an
                                            // interrupt when it occurs

#define CAMERA_CC_IRQENABLE_FIFO_NONEMPTY_IRQ_EN \
                                0x00000010  // FIFO Threshold Interrupt Enable
                                            // 0 Event is masked 1 Event
                                            // generates an interrupt when it
                                            // occurs

#define CAMERA_CC_IRQENABLE_FIFO_FULL_IRQ_EN \
                                0x00000008  // FIFO Threshold Interrupt Enable
                                            // 0 Event is masked 1 Event
                                            // generates an interrupt when it
                                            // occurs

#define CAMERA_CC_IRQENABLE_FIFO_THR_IRQ_EN \
                                0x00000004  // FIFO Threshold Interrupt Enable
                                            // 0 Event is masked 1 Event
                                            // generates an interrupt when it
                                            // occurs

#define CAMERA_CC_IRQENABLE_FIFO_OF_IRQ_EN \
                                0x00000002  // FIFO Overflow Interrupt Enable 0
                                            // Event is masked 1 Event generates
                                            // an interrupt when it occurs

#define CAMERA_CC_IRQENABLE_FIFO_UF_IRQ_EN \
                                0x00000001  // FIFO Underflow Interrupt Enable
                                            // 0 Event is masked 1 Event
                                            // generates an interrupt when it
                                            // occurs

//******************************************************************************
//
// The following are defines for the bit fields in the CAMERA_O_CC_CTRL register.
//
//******************************************************************************
#define CAMERA_CC_CTRL_CC_IF_SYNCHRO \
                                0x00080000  // Synchronize all camera sensor
                                            // inputs This must be set during
                                            // the configuration phase before
                                            // CC_EN set to '1'. This can be
                                            // used in very high frequency to
                                            // avoid dependancy to the IO
                                            // timings. 0 No synchro (most of
                                            // applications) 1 Synchro enabled
                                            // (should never be required)

#define CAMERA_CC_CTRL_CC_RST   0x00040000  // Resets all the internal finite
                                            // states machines of the camera
                                            // core module - by writing a 1 to
                                            // this bit. must be applied when
                                            // CC_EN = 0 Reads returns 0
#define CAMERA_CC_CTRL_CC_FRAME_TRIG \
                                0x00020000  // Set the modality in which CC_EN
                                            // works when a disabling of the
                                            // sensor camera core is wanted "If
                                            // CC_FRAME_TRIG = 1 by writing
                                            // ""0"" to CC_EN" the module is
                                            // disabled at the end of the frame
                                            // "If CC_FRAME_TRIG = 0 by writing
                                            // ""0"" to CC_EN" the module is
                                            // disabled immediately

#define CAMERA_CC_CTRL_CC_EN    0x00010000  // Enables the sensor interface of
                                            // the camera core module "By
                                            // writing ""1"" to this field the
                                            // module is enabled." "By writing
                                            // ""0"" to this field the module is
                                            // disabled at" the end of the frame
                                            // if CC_FRAM_TRIG =1 and is
                                            // disabled immediately if
                                            // CC_FRAM_TRIG = 0
#define CAMERA_CC_CTRL_NOBT_SYNCHRO \
                                0x00002000  // Enables to start at the
                                            // beginning of the frame or not in
                                            // NoBT 0 Acquisition starts when
                                            // Vertical synchro is high 1
                                            // Acquisition starts when Vertical
                                            // synchro goes from low to high
                                            // (beginning of the frame) -
                                            // Recommended.

#define CAMERA_CC_CTRL_BT_CORRECT \
                                0x00001000  // Enables the correction within
                                            // the sync codes in BT mode 0
                                            // correction is not enabled 1
                                            // correction is enabled

#define CAMERA_CC_CTRL_PAR_ORDERCAM \
                                0x00000800  // Enables swap between image-data
                                            // in parallel mode 0 swap is not
                                            // enabled 1 swap is enabled

#define CAMERA_CC_CTRL_PAR_CLK_POL \
                                0x00000400  // Inverts the clock coming from
                                            // the sensor in parallel mode 0
                                            // clock not inverted - data sampled
                                            // on rising edge 1 clock inverted -
                                            // data sampled on falling edge

#define CAMERA_CC_CTRL_NOBT_HS_POL \
                                0x00000200  // Sets the polarity of the
                                            // synchronization signals in NOBT
                                            // parallel mode 0 CAM_P_HS is
                                            // active high 1 CAM_P_HS is active
                                            // low

#define CAMERA_CC_CTRL_NOBT_VS_POL \
                                0x00000100  // Sets the polarity of the
                                            // synchronization signals in NOBT
                                            // parallel mode 0 CAM_P_VS is
                                            // active high 1 CAM_P_VS is active
                                            // low

#define CAMERA_CC_CTRL_PAR_MODE_M \
                                0x0000000E  // Sets the Protocol Mode of the
                                            // Camera Core module in parallel
                                            // mode (when CCP_MODE = 0) """000""
                                            // Parallel NOBT 8-bit" """001""
                                            // Parallel NOBT 10-bit" """010""
                                            // Parallel NOBT 12-bit" """011""
                                            // reserved" """100"" Parallet BT
                                            // 8-bit" """101"" Parallel BT
                                            // 10-bit" """110"" reserved"
                                            // """111"" FIFO test mode. Refer to
                                            // Table 12 - FIFO Write and Read
                                            // access"

#define CAMERA_CC_CTRL_PAR_MODE_S 1
#define CAMERA_CC_CTRL_CCP_MODE 0x00000001  // Set the Camera Core in CCP mode
                                            // 0 CCP mode disabled 1 CCP mode
                                            // enabled
//******************************************************************************
//
// The following are defines for the bit fields in the
// CAMERA_O_CC_CTRL_DMA register.
//
//******************************************************************************
#define CAMERA_CC_CTRL_DMA_DMA_EN \
                                0x00000100  // Sets the number of dma request
                                            // lines 0 DMA interface disabled
                                            // The DMA request line stays
                                            // inactive 1 DMA interface enabled
                                            // The DMA request line is
                                            // operational

#define CAMERA_CC_CTRL_DMA_FIFO_THRESHOLD_M \
                                0x0000007F  // Sets the threshold of the FIFO
                                            // the assertion of the dmarequest
                                            // line takes place when the
                                            // threshold is reached.
                                            // """0000000"" threshold set to 1"
                                            // """0000001"" threshold set to 2"
                                            // … """1111111"" threshold set to
                                            // 128"

#define CAMERA_CC_CTRL_DMA_FIFO_THRESHOLD_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// CAMERA_O_CC_CTRL_XCLK register.
//
//******************************************************************************
#define CAMERA_CC_CTRL_XCLK_XCLK_DIV_M \
                                0x0000001F  // Sets the clock divisor value for
                                            // CAM_XCLK generation. based on
                                            // CAM_MCK (value of CAM_MCLK is
                                            // 96MHz) """00000"" CAM_XCLK Stable
                                            // Low Level" Divider not enabled
                                            // """00001"" CAM_XCLK Stable High
                                            // Level" Divider not enabled from 2
                                            // to 30 CAM_XCLK = CAM_MCLK /
                                            // XCLK_DIV """11111"" Bypass -
                                            // CAM_XCLK = CAM_MCLK"

#define CAMERA_CC_CTRL_XCLK_XCLK_DIV_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// CAMERA_O_CC_FIFO_DATA register.
//
//******************************************************************************
#define CAMERA_CC_FIFO_DATA_FIFO_DATA_M \
                                0xFFFFFFFF  // Writes the 32-bit word into the
                                            // FIFO Reads the 32-bit word from
                                            // the FIFO

#define CAMERA_CC_FIFO_DATA_FIFO_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the CAMERA_O_CC_TEST register.
//
//******************************************************************************
#define CAMERA_CC_TEST_FIFO_RD_POINTER_M \
                                0xFF000000  // FIFO READ Pointer This field
                                            // shows the value of the FIFO read
                                            // pointer Expected value ranges
                                            // from 0 to 127

#define CAMERA_CC_TEST_FIFO_RD_POINTER_S 24
#define CAMERA_CC_TEST_FIFO_WR_POINTER_M \
                                0x00FF0000  // FIFO WRITE pointer This field
                                            // shows the value of the FIFO write
                                            // pointer Expected value ranges
                                            // from 0 to 127

#define CAMERA_CC_TEST_FIFO_WR_POINTER_S 16
#define CAMERA_CC_TEST_FIFO_LEVEL_M \
                                0x0000FF00  // FIFO level (how many 32-bit
                                            // words the FIFO contains) This
                                            // field shows the value of the FIFO
                                            // level and can assume values from
                                            // 0 to 128

#define CAMERA_CC_TEST_FIFO_LEVEL_S 8
#define CAMERA_CC_TEST_FIFO_LEVEL_PEAK_M \
                                0x000000FF  // FIFO level peak This field shows
                                            // the max value of the FIFO level
                                            // and can assume values from 0 to
                                            // 128

#define CAMERA_CC_TEST_FIFO_LEVEL_PEAK_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// CAMERA_O_CC_GEN_PAR register.
//
//******************************************************************************
#define CAMERA_CC_GEN_PAR_CC_FIFO_DEPTH_M \
                                0x00000007  // Camera Core FIFO DEPTH generic
                                            // parameter

#define CAMERA_CC_GEN_PAR_CC_FIFO_DEPTH_S 0



#endif // __HW_CAMERA_H__

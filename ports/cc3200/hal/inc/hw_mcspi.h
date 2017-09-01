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

#ifndef __HW_MCSPI_H__
#define __HW_MCSPI_H__

//*****************************************************************************
//
// The following are defines for the MCSPI register offsets.
//
//*****************************************************************************
#define MCSPI_O_HL_REV          0x00000000  // IP Revision Identifier (X.Y.R)
                                            // Used by software to track
                                            // features bugs and compatibility
#define MCSPI_O_HL_HWINFO       0x00000004  // Information about the IP
                                            // module's hardware configuration
                                            // i.e. typically the module's HDL
                                            // generics (if any). Actual field
                                            // format and encoding is up to the
                                            // module's designer to decide.
#define MCSPI_O_HL_SYSCONFIG    0x00000010  // 0x4402 1010 0x4402 2010 Clock
                                            // management configuration
#define MCSPI_O_REVISION        0x00000100  // 0x4402 1100 0x4402 2100 This
                                            // register contains the hard coded
                                            // RTL revision number.
#define MCSPI_O_SYSCONFIG       0x00000110  // 0x4402 1110 0x4402 2110 This
                                            // register allows controlling
                                            // various parameters of the OCP
                                            // interface.
#define MCSPI_O_SYSSTATUS       0x00000114  // 0x4402 1114 0x4402 2114 This
                                            // register provides status
                                            // information about the module
                                            // excluding the interrupt status
                                            // information
#define MCSPI_O_IRQSTATUS       0x00000118  // 0x4402 1118 0x4402 2118 The
                                            // interrupt status regroups all the
                                            // status of the module internal
                                            // events that can generate an
                                            // interrupt
#define MCSPI_O_IRQENABLE       0x0000011C  // 0x4402 111C 0x4402 211C This
                                            // register allows to enable/disable
                                            // the module internal sources of
                                            // interrupt on an event-by-event
                                            // basis.
#define MCSPI_O_WAKEUPENABLE    0x00000120  // 0x4402 1120 0x4402 2120 The
                                            // wakeup enable register allows to
                                            // enable/disable the module
                                            // internal sources of wakeup on
                                            // event-by-event basis.
#define MCSPI_O_SYST            0x00000124  // 0x4402 1124 0x4402 2124 This
                                            // register is used to check the
                                            // correctness of the system
                                            // interconnect either internally to
                                            // peripheral bus or externally to
                                            // device IO pads when the module is
                                            // configured in system test
                                            // (SYSTEST) mode.
#define MCSPI_O_MODULCTRL       0x00000128  // 0x4402 1128 0x4402 2128 This
                                            // register is dedicated to the
                                            // configuration of the serial port
                                            // interface.
#define MCSPI_O_CH0CONF         0x0000012C  // 0x4402 112C 0x4402 212C This
                                            // register is dedicated to the
                                            // configuration of the channel 0
#define MCSPI_O_CH0STAT         0x00000130  // 0x4402 1130 0x4402 2130 This
                                            // register provides status
                                            // information about transmitter and
                                            // receiver registers of channel 0
#define MCSPI_O_CH0CTRL         0x00000134  // 0x4402 1134 0x4402 2134 This
                                            // register is dedicated to enable
                                            // the channel 0
#define MCSPI_O_TX0             0x00000138  // 0x4402 1138 0x4402 2138 This
                                            // register contains a single SPI
                                            // word to transmit on the serial
                                            // link what ever SPI word length
                                            // is.
#define MCSPI_O_RX0             0x0000013C  // 0x4402 113C 0x4402 213C This
                                            // register contains a single SPI
                                            // word received through the serial
                                            // link what ever SPI word length
                                            // is.
#define MCSPI_O_CH1CONF         0x00000140  // 0x4402 1140 0x4402 2140 This
                                            // register is dedicated to the
                                            // configuration of the channel.
#define MCSPI_O_CH1STAT         0x00000144  // 0x4402 1144 0x4402 2144 This
                                            // register provides status
                                            // information about transmitter and
                                            // receiver registers of channel 1
#define MCSPI_O_CH1CTRL         0x00000148  // 0x4402 1148 0x4402 2148 This
                                            // register is dedicated to enable
                                            // the channel 1
#define MCSPI_O_TX1             0x0000014C  // 0x4402 114C 0x4402 214C This
                                            // register contains a single SPI
                                            // word to transmit on the serial
                                            // link what ever SPI word length
                                            // is.
#define MCSPI_O_RX1             0x00000150  // 0x4402 1150 0x4402 2150 This
                                            // register contains a single SPI
                                            // word received through the serial
                                            // link what ever SPI word length
                                            // is.
#define MCSPI_O_CH2CONF         0x00000154  // 0x4402 1154 0x4402 2154 This
                                            // register is dedicated to the
                                            // configuration of the channel 2
#define MCSPI_O_CH2STAT         0x00000158  // 0x4402 1158 0x4402 2158 This
                                            // register provides status
                                            // information about transmitter and
                                            // receiver registers of channel 2
#define MCSPI_O_CH2CTRL         0x0000015C  // 0x4402 115C 0x4402 215C This
                                            // register is dedicated to enable
                                            // the channel 2
#define MCSPI_O_TX2             0x00000160  // 0x4402 1160 0x4402 2160 This
                                            // register contains a single SPI
                                            // word to transmit on the serial
                                            // link what ever SPI word length
                                            // is.
#define MCSPI_O_RX2             0x00000164  // 0x4402 1164 0x4402 2164 This
                                            // register contains a single SPI
                                            // word received through the serial
                                            // link what ever SPI word length
                                            // is.
#define MCSPI_O_CH3CONF         0x00000168  // 0x4402 1168 0x4402 2168 This
                                            // register is dedicated to the
                                            // configuration of the channel 3
#define MCSPI_O_CH3STAT         0x0000016C  // 0x4402 116C 0x4402 216C This
                                            // register provides status
                                            // information about transmitter and
                                            // receiver registers of channel 3
#define MCSPI_O_CH3CTRL         0x00000170  // 0x4402 1170 0x4402 2170 This
                                            // register is dedicated to enable
                                            // the channel 3
#define MCSPI_O_TX3             0x00000174  // 0x4402 1174 0x4402 2174 This
                                            // register contains a single SPI
                                            // word to transmit on the serial
                                            // link what ever SPI word length
                                            // is.
#define MCSPI_O_RX3             0x00000178  // 0x4402 1178 0x4402 2178 This
                                            // register contains a single SPI
                                            // word received through the serial
                                            // link what ever SPI word length
                                            // is.
#define MCSPI_O_XFERLEVEL       0x0000017C  // 0x4402 117C 0x4402 217C This
                                            // register provides transfer levels
                                            // needed while using FIFO buffer
                                            // during transfer.
#define MCSPI_O_DAFTX           0x00000180  // 0x4402 1180 0x4402 2180 This
                                            // register contains the SPI words
                                            // to transmit on the serial link
                                            // when FIFO used and DMA address is
                                            // aligned on 256 bit.This register
                                            // is an image of one of MCSPI_TX(i)
                                            // register corresponding to the
                                            // channel which have its FIFO
                                            // enabled.
#define MCSPI_O_DAFRX           0x000001A0  // 0x4402 11A0 0x4402 21A0 This
                                            // register contains the SPI words
                                            // to received on the serial link
                                            // when FIFO used and DMA address is
                                            // aligned on 256 bit.This register
                                            // is an image of one of MCSPI_RX(i)
                                            // register corresponding to the
                                            // channel which have its FIFO
                                            // enabled.



//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_HL_REV register.
//
//******************************************************************************
#define MCSPI_HL_REV_SCHEME_M   0xC0000000
#define MCSPI_HL_REV_SCHEME_S   30
#define MCSPI_HL_REV_RSVD_M     0x30000000  // Reserved These bits are
                                            // initialized to zero and writes to
                                            // them are ignored.
#define MCSPI_HL_REV_RSVD_S     28
#define MCSPI_HL_REV_FUNC_M     0x0FFF0000  // Function indicates a software
                                            // compatible module family. If
                                            // there is no level of software
                                            // compatibility a new Func number
                                            // (and hence REVISION) should be
                                            // assigned.
#define MCSPI_HL_REV_FUNC_S     16
#define MCSPI_HL_REV_R_RTL_M    0x0000F800  // RTL Version (R) maintained by IP
                                            // design owner. RTL follows a
                                            // numbering such as X.Y.R.Z which
                                            // are explained in this table. R
                                            // changes ONLY when: (1) PDS
                                            // uploads occur which may have been
                                            // due to spec changes (2) Bug fixes
                                            // occur (3) Resets to '0' when X or
                                            // Y changes. Design team has an
                                            // internal 'Z' (customer invisible)
                                            // number which increments on every
                                            // drop that happens due to DV and
                                            // RTL updates. Z resets to 0 when R
                                            // increments.
#define MCSPI_HL_REV_R_RTL_S    11
#define MCSPI_HL_REV_X_MAJOR_M  0x00000700  // Major Revision (X) maintained by
                                            // IP specification owner. X changes
                                            // ONLY when: (1) There is a major
                                            // feature addition. An example
                                            // would be adding Master Mode to
                                            // Utopia Level2. The Func field (or
                                            // Class/Type in old PID format)
                                            // will remain the same. X does NOT
                                            // change due to: (1) Bug fixes (2)
                                            // Change in feature parameters.
#define MCSPI_HL_REV_X_MAJOR_S  8
#define MCSPI_HL_REV_CUSTOM_M   0x000000C0
#define MCSPI_HL_REV_CUSTOM_S   6
#define MCSPI_HL_REV_Y_MINOR_M  0x0000003F  // Minor Revision (Y) maintained by
                                            // IP specification owner. Y changes
                                            // ONLY when: (1) Features are
                                            // scaled (up or down). Flexibility
                                            // exists in that this feature
                                            // scalability may either be
                                            // represented in the Y change or a
                                            // specific register in the IP that
                                            // indicates which features are
                                            // exactly available. (2) When
                                            // feature creeps from Is-Not list
                                            // to Is list. But this may not be
                                            // the case once it sees silicon; in
                                            // which case X will change. Y does
                                            // NOT change due to: (1) Bug fixes
                                            // (2) Typos or clarifications (3)
                                            // major functional/feature
                                            // change/addition/deletion. Instead
                                            // these changes may be reflected
                                            // via R S X as applicable. Spec
                                            // owner maintains a
                                            // customer-invisible number 'S'
                                            // which changes due to: (1)
                                            // Typos/clarifications (2) Bug
                                            // documentation. Note that this bug
                                            // is not due to a spec change but
                                            // due to implementation.
                                            // Nevertheless the spec tracks the
                                            // IP bugs. An RTL release (say for
                                            // silicon PG1.1) that occurs due to
                                            // bug fix should document the
                                            // corresponding spec number (X.Y.S)
                                            // in its release notes.
#define MCSPI_HL_REV_Y_MINOR_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_HL_HWINFO register.
//
//******************************************************************************
#define MCSPI_HL_HWINFO_RETMODE 0x00000040
#define MCSPI_HL_HWINFO_FFNBYTE_M \
                                0x0000003E

#define MCSPI_HL_HWINFO_FFNBYTE_S 1
#define MCSPI_HL_HWINFO_USEFIFO 0x00000001
//******************************************************************************
//
// The following are defines for the bit fields in the
// MCSPI_O_HL_SYSCONFIG register.
//
//******************************************************************************
#define MCSPI_HL_SYSCONFIG_IDLEMODE_M \
                                0x0000000C  // Configuration of the local
                                            // target state management mode. By
                                            // definition target can handle
                                            // read/write transaction as long as
                                            // it is out of IDLE state. 0x0
                                            // Force-idle mode: local target's
                                            // idle state follows (acknowledges)
                                            // the system's idle requests
                                            // unconditionally i.e. regardless
                                            // of the IP module's internal
                                            // requirements.Backup mode for
                                            // debug only. 0x1 No-idle mode:
                                            // local target never enters idle
                                            // state.Backup mode for debug only.
                                            // 0x2 Smart-idle mode: local
                                            // target's idle state eventually
                                            // follows (acknowledges) the
                                            // system's idle requests depending
                                            // on the IP module's internal
                                            // requirements.IP module shall not
                                            // generate (IRQ- or
                                            // DMA-request-related) wakeup
                                            // events. 0x3 "Smart-idle
                                            // wakeup-capable mode: local
                                            // target's idle state eventually
                                            // follows (acknowledges) the
                                            // system's idle requests depending
                                            // on the IP module's internal
                                            // requirements.IP module may
                                            // generate (IRQ- or
                                            // DMA-request-related) wakeup
                                            // events when in idle state.Mode is
                                            // only relevant if the appropriate
                                            // IP module ""swakeup"" output(s)
                                            // is (are) implemented."

#define MCSPI_HL_SYSCONFIG_IDLEMODE_S 2
#define MCSPI_HL_SYSCONFIG_FREEEMU \
                                0x00000002  // Sensitivity to emulation (debug)
                                            // suspend input signal. 0 IP module
                                            // is sensitive to emulation suspend
                                            // 1 IP module is not sensitive to
                                            // emulation suspend

#define MCSPI_HL_SYSCONFIG_SOFTRESET \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_REVISION register.
//
//******************************************************************************
#define MCSPI_REVISION_REV_M    0x000000FF  // IP revision [7:4] Major revision
                                            // [3:0] Minor revision Examples:
                                            // 0x10 for 1.0 0x21 for 2.1
#define MCSPI_REVISION_REV_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_SYSCONFIG register.
//
//******************************************************************************
#define MCSPI_SYSCONFIG_CLOCKACTIVITY_M \
                                0x00000300  // Clocks activity during wake up
                                            // mode period 0x0 OCP and
                                            // Functional clocks may be switched
                                            // off. 0x1 OCP clock is maintained.
                                            // Functional clock may be
                                            // switched-off. 0x2 Functional
                                            // clock is maintained. OCP clock
                                            // may be switched-off. 0x3 OCP and
                                            // Functional clocks are maintained.

#define MCSPI_SYSCONFIG_CLOCKACTIVITY_S 8
#define MCSPI_SYSCONFIG_SIDLEMODE_M \
                                0x00000018  // Power management 0x0 If an idle
                                            // request is detected the McSPI
                                            // acknowledges it unconditionally
                                            // and goes in Inactive mode.
                                            // Interrupt DMA requests and wake
                                            // up lines are unconditionally
                                            // de-asserted and the module wakeup
                                            // capability is deactivated even if
                                            // the bit
                                            // MCSPI_SYSCONFIG[EnaWakeUp] is
                                            // set. 0x1 If an idle request is
                                            // detected the request is ignored
                                            // and the module does not switch to
                                            // wake up mode and keeps on
                                            // behaving normally. 0x2 If an idle
                                            // request is detected the module
                                            // will switch to idle mode based on
                                            // its internal activity. The wake
                                            // up capability cannot be used. 0x3
                                            // If an idle request is detected
                                            // the module will switch to idle
                                            // mode based on its internal
                                            // activity and the wake up
                                            // capability can be used if the bit
                                            // MCSPI_SYSCONFIG[EnaWakeUp] is
                                            // set.

#define MCSPI_SYSCONFIG_SIDLEMODE_S 3
#define MCSPI_SYSCONFIG_ENAWAKEUP \
                                0x00000004  // WakeUp feature control 0 WakeUp
                                            // capability is disabled 1 WakeUp
                                            // capability is enabled

#define MCSPI_SYSCONFIG_SOFTRESET \
                                0x00000002  // Software reset. During reads it
                                            // always returns 0. 0 (write)
                                            // Normal mode 1 (write) Set this
                                            // bit to 1 to trigger a module
                                            // reset.The bit is automatically
                                            // reset by the hardware.

#define MCSPI_SYSCONFIG_AUTOIDLE \
                                0x00000001  // Internal OCP Clock gating
                                            // strategy 0 OCP clock is
                                            // free-running 1 Automatic OCP
                                            // clock gating strategy is applied
                                            // based on the OCP interface
                                            // activity

//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_SYSSTATUS register.
//
//******************************************************************************
#define MCSPI_SYSSTATUS_RESETDONE \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_IRQSTATUS register.
//
//******************************************************************************
#define MCSPI_IRQSTATUS_EOW     0x00020000
#define MCSPI_IRQSTATUS_WKS     0x00010000
#define MCSPI_IRQSTATUS_RX3_FULL \
                                0x00004000

#define MCSPI_IRQSTATUS_TX3_UNDERFLOW \
                                0x00002000

#define MCSPI_IRQSTATUS_TX3_EMPTY \
                                0x00001000

#define MCSPI_IRQSTATUS_RX2_FULL \
                                0x00000400

#define MCSPI_IRQSTATUS_TX2_UNDERFLOW \
                                0x00000200

#define MCSPI_IRQSTATUS_TX2_EMPTY \
                                0x00000100

#define MCSPI_IRQSTATUS_RX1_FULL \
                                0x00000040

#define MCSPI_IRQSTATUS_TX1_UNDERFLOW \
                                0x00000020

#define MCSPI_IRQSTATUS_TX1_EMPTY \
                                0x00000010

#define MCSPI_IRQSTATUS_RX0_OVERFLOW \
                                0x00000008

#define MCSPI_IRQSTATUS_RX0_FULL \
                                0x00000004

#define MCSPI_IRQSTATUS_TX0_UNDERFLOW \
                                0x00000002

#define MCSPI_IRQSTATUS_TX0_EMPTY \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_IRQENABLE register.
//
//******************************************************************************
#define MCSPI_IRQENABLE_EOW_ENABLE \
                                0x00020000  // End of Word count Interrupt
                                            // Enable. 0 Interrupt disabled 1
                                            // Interrupt enabled

#define MCSPI_IRQENABLE_WKE     0x00010000  // Wake Up event interrupt Enable
                                            // in slave mode when an active
                                            // control signal is detected on the
                                            // SPIEN line programmed in the
                                            // field MCSPI_CH0CONF[SPIENSLV] 0
                                            // Interrupt disabled 1 Interrupt
                                            // enabled
#define MCSPI_IRQENABLE_RX3_FULL_ENABLE \
                                0x00004000  // Receiver register Full Interrupt
                                            // Enable. Ch 3 0 Interrupt disabled
                                            // 1 Interrupt enabled

#define MCSPI_IRQENABLE_TX3_UNDERFLOW_ENABLE \
                                0x00002000  // Transmitter register Underflow
                                            // Interrupt Enable. Ch 3 0
                                            // Interrupt disabled 1 Interrupt
                                            // enabled

#define MCSPI_IRQENABLE_TX3_EMPTY_ENABLE \
                                0x00001000  // Transmitter register Empty
                                            // Interrupt Enable. Ch3 0 Interrupt
                                            // disabled 1 Interrupt enabled

#define MCSPI_IRQENABLE_RX2_FULL_ENABLE \
                                0x00000400  // Receiver register Full Interrupt
                                            // Enable. Ch 2 0 Interrupt disabled
                                            // 1 Interrupt enabled

#define MCSPI_IRQENABLE_TX2_UNDERFLOW_ENABLE \
                                0x00000200  // Transmitter register Underflow
                                            // Interrupt Enable. Ch 2 0
                                            // Interrupt disabled 1 Interrupt
                                            // enabled

#define MCSPI_IRQENABLE_TX2_EMPTY_ENABLE \
                                0x00000100  // Transmitter register Empty
                                            // Interrupt Enable. Ch 2 0
                                            // Interrupt disabled 1 Interrupt
                                            // enabled

#define MCSPI_IRQENABLE_RX1_FULL_ENABLE \
                                0x00000040  // Receiver register Full Interrupt
                                            // Enable. Ch 1 0 Interrupt disabled
                                            // 1 Interrupt enabled

#define MCSPI_IRQENABLE_TX1_UNDERFLOW_ENABLE \
                                0x00000020  // Transmitter register Underflow
                                            // Interrupt Enable. Ch 1 0
                                            // Interrupt disabled 1 Interrupt
                                            // enabled

#define MCSPI_IRQENABLE_TX1_EMPTY_ENABLE \
                                0x00000010  // Transmitter register Empty
                                            // Interrupt Enable. Ch 1 0
                                            // Interrupt disabled 1 Interrupt
                                            // enabled

#define MCSPI_IRQENABLE_RX0_OVERFLOW_ENABLE \
                                0x00000008  // Receiver register Overflow
                                            // Interrupt Enable. Ch 0 0
                                            // Interrupt disabled 1 Interrupt
                                            // enabled

#define MCSPI_IRQENABLE_RX0_FULL_ENABLE \
                                0x00000004  // Receiver register Full Interrupt
                                            // Enable. Ch 0 0 Interrupt disabled
                                            // 1 Interrupt enabled

#define MCSPI_IRQENABLE_TX0_UNDERFLOW_ENABLE \
                                0x00000002  // Transmitter register Underflow
                                            // Interrupt Enable. Ch 0 0
                                            // Interrupt disabled 1 Interrupt
                                            // enabled

#define MCSPI_IRQENABLE_TX0_EMPTY_ENABLE \
                                0x00000001  // Transmitter register Empty
                                            // Interrupt Enable. Ch 0 0
                                            // Interrupt disabled 1 Interrupt
                                            // enabled

//******************************************************************************
//
// The following are defines for the bit fields in the
// MCSPI_O_WAKEUPENABLE register.
//
//******************************************************************************
#define MCSPI_WAKEUPENABLE_WKEN 0x00000001  // WakeUp functionality in slave
                                            // mode when an active control
                                            // signal is detected on the SPIEN
                                            // line programmed in the field
                                            // MCSPI_CH0CONF[SPIENSLV] 0 The
                                            // event is not allowed to wakeup
                                            // the system even if the global
                                            // control bit
                                            // MCSPI_SYSCONF[EnaWakeUp] is set.
                                            // 1 The event is allowed to wakeup
                                            // the system if the global control
                                            // bit MCSPI_SYSCONF[EnaWakeUp] is
                                            // set.
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_SYST register.
//
//******************************************************************************
#define MCSPI_SYST_SSB          0x00000800  // Set status bit 0 No action.
                                            // Writing 0 does not clear already
                                            // set status bits; This bit must be
                                            // cleared prior attempting to clear
                                            // a status bit of the
                                            // <MCSPI_IRQSTATUS> register. 1
                                            // Force to 1 all status bits of
                                            // MCSPI_IRQSTATUS register. Writing
                                            // 1 into this bit sets to 1 all
                                            // status bits contained in the
                                            // <MCSPI_IRQSTATUS> register.
#define MCSPI_SYST_SPIENDIR     0x00000400  // Set the direction of the
                                            // SPIEN[3:0] lines and SPICLK line
                                            // 0 output (as in master mode) 1
                                            // input (as in slave mode)
#define MCSPI_SYST_SPIDATDIR1   0x00000200  // Set the direction of the
                                            // SPIDAT[1] 0 output 1 input
#define MCSPI_SYST_SPIDATDIR0   0x00000100  // Set the direction of the
                                            // SPIDAT[0] 0 output 1 input
#define MCSPI_SYST_WAKD         0x00000080  // SWAKEUP output (signal data
                                            // value of internal signal to
                                            // system). The signal is driven
                                            // high or low according to the
                                            // value written into this register
                                            // bit. 0 The pin is driven low. 1
                                            // The pin is driven high.
#define MCSPI_SYST_SPICLK       0x00000040  // SPICLK line (signal data value)
                                            // If MCSPI_SYST[SPIENDIR] = 1
                                            // (input mode direction) this bit
                                            // returns the value on the CLKSPI
                                            // line (high or low) and a write
                                            // into this bit has no effect. If
                                            // MCSPI_SYST[SPIENDIR] = 0 (output
                                            // mode direction) the CLKSPI line
                                            // is driven high or low according
                                            // to the value written into this
                                            // register.
#define MCSPI_SYST_SPIDAT_1     0x00000020  // SPIDAT[1] line (signal data
                                            // value) If MCSPI_SYST[SPIDATDIR1]
                                            // = 0 (output mode direction) the
                                            // SPIDAT[1] line is driven high or
                                            // low according to the value
                                            // written into this register. If
                                            // MCSPI_SYST[SPIDATDIR1] = 1 (input
                                            // mode direction) this bit returns
                                            // the value on the SPIDAT[1] line
                                            // (high or low) and a write into
                                            // this bit has no effect.
#define MCSPI_SYST_SPIDAT_0     0x00000010  // SPIDAT[0] line (signal data
                                            // value) If MCSPI_SYST[SPIDATDIR0]
                                            // = 0 (output mode direction) the
                                            // SPIDAT[0] line is driven high or
                                            // low according to the value
                                            // written into this register. If
                                            // MCSPI_SYST[SPIDATDIR0] = 1 (input
                                            // mode direction) this bit returns
                                            // the value on the SPIDAT[0] line
                                            // (high or low) and a write into
                                            // this bit has no effect.
#define MCSPI_SYST_SPIEN_3      0x00000008  // SPIEN[3] line (signal data
                                            // value) If MCSPI_SYST[SPIENDIR] =
                                            // 0 (output mode direction) the
                                            // SPIENT[3] line is driven high or
                                            // low according to the value
                                            // written into this register. If
                                            // MCSPI_SYST[SPIENDIR] = 1 (input
                                            // mode direction) this bit returns
                                            // the value on the SPIEN[3] line
                                            // (high or low) and a write into
                                            // this bit has no effect.
#define MCSPI_SYST_SPIEN_2      0x00000004  // SPIEN[2] line (signal data
                                            // value) If MCSPI_SYST[SPIENDIR] =
                                            // 0 (output mode direction) the
                                            // SPIENT[2] line is driven high or
                                            // low according to the value
                                            // written into this register. If
                                            // MCSPI_SYST[SPIENDIR] = 1 (input
                                            // mode direction) this bit returns
                                            // the value on the SPIEN[2] line
                                            // (high or low) and a write into
                                            // this bit has no effect.
#define MCSPI_SYST_SPIEN_1      0x00000002  // SPIEN[1] line (signal data
                                            // value) If MCSPI_SYST[SPIENDIR] =
                                            // 0 (output mode direction) the
                                            // SPIENT[1] line is driven high or
                                            // low according to the value
                                            // written into this register. If
                                            // MCSPI_SYST[SPIENDIR] = 1 (input
                                            // mode direction) this bit returns
                                            // the value on the SPIEN[1] line
                                            // (high or low) and a write into
                                            // this bit has no effect.
#define MCSPI_SYST_SPIEN_0      0x00000001  // SPIEN[0] line (signal data
                                            // value) If MCSPI_SYST[SPIENDIR] =
                                            // 0 (output mode direction) the
                                            // SPIENT[0] line is driven high or
                                            // low according to the value
                                            // written into this register. If
                                            // MCSPI_SYST[SPIENDIR] = 1 (input
                                            // mode direction) this bit returns
                                            // the value on the SPIEN[0] line
                                            // (high or low) and a write into
                                            // this bit has no effect.
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_MODULCTRL register.
//
//******************************************************************************
#define MCSPI_MODULCTRL_FDAA    0x00000100  // FIFO DMA Address 256-bit aligned
                                            // This register is used when a FIFO
                                            // is managed by the module and DMA
                                            // connected to the controller
                                            // provides only 256 bit aligned
                                            // address. If this bit is set the
                                            // enabled channel which uses the
                                            // FIFO has its datas managed
                                            // through MCSPI_DAFTX and
                                            // MCSPI_DAFRX registers instead of
                                            // MCSPI_TX(i) and MCSPI_RX(i)
                                            // registers. 0 FIFO data managed by
                                            // MCSPI_TX(i) and MCSPI_RX(i)
                                            // registers. 1 FIFO data managed by
                                            // MCSPI_DAFTX and MCSPI_DAFRX
                                            // registers.
#define MCSPI_MODULCTRL_MOA     0x00000080  // Multiple word ocp access: This
                                            // register can only be used when a
                                            // channel is enabled using a FIFO.
                                            // It allows the system to perform
                                            // multiple SPI word access for a
                                            // single 32-bit OCP word access.
                                            // This is possible for WL < 16. 0
                                            // Multiple word access disabled 1
                                            // Multiple word access enabled with
                                            // FIFO
#define MCSPI_MODULCTRL_INITDLY_M \
                                0x00000070  // Initial spi delay for first
                                            // transfer: This register is an
                                            // option only available in SINGLE
                                            // master mode The controller waits
                                            // for a delay to transmit the first
                                            // spi word after channel enabled
                                            // and corresponding TX register
                                            // filled. This Delay is based on
                                            // SPI output frequency clock No
                                            // clock output provided to the
                                            // boundary and chip select is not
                                            // active in 4 pin mode within this
                                            // period. 0x0 No delay for first
                                            // spi transfer. 0x1 The controller
                                            // wait 4 spi bus clock 0x2 The
                                            // controller wait 8 spi bus clock
                                            // 0x3 The controller wait 16 spi
                                            // bus clock 0x4 The controller wait
                                            // 32 spi bus clock

#define MCSPI_MODULCTRL_INITDLY_S 4
#define MCSPI_MODULCTRL_SYSTEM_TEST \
                                0x00000008  // Enables the system test mode 0
                                            // Functional mode 1 System test
                                            // mode (SYSTEST)

#define MCSPI_MODULCTRL_MS      0x00000004  // Master/ Slave 0 Master - The
                                            // module generates the SPICLK and
                                            // SPIEN[3:0] 1 Slave - The module
                                            // receives the SPICLK and
                                            // SPIEN[3:0]
#define MCSPI_MODULCTRL_PIN34   0x00000002  // Pin mode selection: This
                                            // register is used to configure the
                                            // SPI pin mode in master or slave
                                            // mode. If asserted the controller
                                            // only use SIMOSOMI and SPICLK
                                            // clock pin for spi transfers. 0
                                            // SPIEN is used as a chip select. 1
                                            // SPIEN is not used.In this mode
                                            // all related option to chip select
                                            // have no meaning.
#define MCSPI_MODULCTRL_SINGLE  0x00000001  // Single channel / Multi Channel
                                            // (master mode only) 0 More than
                                            // one channel will be used in
                                            // master mode. 1 Only one channel
                                            // will be used in master mode. This
                                            // bit must be set in Force SPIEN
                                            // mode.
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_CH0CONF register.
//
//******************************************************************************
#define MCSPI_CH0CONF_CLKG      0x20000000  // Clock divider granularity This
                                            // register defines the granularity
                                            // of channel clock divider: power
                                            // of two or one clock cycle
                                            // granularity. When this bit is set
                                            // the register MCSPI_CHCTRL[EXTCLK]
                                            // must be configured to reach a
                                            // maximum of 4096 clock divider
                                            // ratio. Then The clock divider
                                            // ratio is a concatenation of
                                            // MCSPI_CHCONF[CLKD] and
                                            // MCSPI_CHCTRL[EXTCLK] values 0
                                            // Clock granularity of power of two
                                            // 1 One clock cycle ganularity
#define MCSPI_CH0CONF_FFER      0x10000000  // FIFO enabled for receive:Only
                                            // one channel can have this bit
                                            // field set. 0 The buffer is not
                                            // used to receive data. 1 The
                                            // buffer is used to receive data.
#define MCSPI_CH0CONF_FFEW      0x08000000  // FIFO enabled for Transmit:Only
                                            // one channel can have this bit
                                            // field set. 0 The buffer is not
                                            // used to transmit data. 1 The
                                            // buffer is used to transmit data.
#define MCSPI_CH0CONF_TCS0_M    0x06000000  // Chip Select Time Control This
                                            // 2-bits field defines the number
                                            // of interface clock cycles between
                                            // CS toggling and first or last
                                            // edge of SPI clock. 0x0 0.5 clock
                                            // cycle 0x1 1.5 clock cycle 0x2 2.5
                                            // clock cycle 0x3 3.5 clock cycle
#define MCSPI_CH0CONF_TCS0_S    25
#define MCSPI_CH0CONF_SBPOL     0x01000000  // Start bit polarity 0 Start bit
                                            // polarity is held to 0 during SPI
                                            // transfer. 1 Start bit polarity is
                                            // held to 1 during SPI transfer.
#define MCSPI_CH0CONF_SBE       0x00800000  // Start bit enable for SPI
                                            // transfer 0 Default SPI transfer
                                            // length as specified by WL bit
                                            // field 1 Start bit D/CX added
                                            // before SPI transfer polarity is
                                            // defined by MCSPI_CH0CONF[SBPOL]
#define MCSPI_CH0CONF_SPIENSLV_M \
                                0x00600000  // Channel 0 only and slave mode
                                            // only: SPI slave select signal
                                            // detection. Reserved bits for
                                            // other cases. 0x0 Detection
                                            // enabled only on SPIEN[0] 0x1
                                            // Detection enabled only on
                                            // SPIEN[1] 0x2 Detection enabled
                                            // only on SPIEN[2] 0x3 Detection
                                            // enabled only on SPIEN[3]

#define MCSPI_CH0CONF_SPIENSLV_S 21
#define MCSPI_CH0CONF_FORCE     0x00100000  // Manual SPIEN assertion to keep
                                            // SPIEN active between SPI words.
                                            // (single channel master mode only)
                                            // 0 Writing 0 into this bit drives
                                            // low the SPIEN line when
                                            // MCSPI_CHCONF(i)[EPOL]=0 and
                                            // drives it high when
                                            // MCSPI_CHCONF(i)[EPOL]=1. 1
                                            // Writing 1 into this bit drives
                                            // high the SPIEN line when
                                            // MCSPI_CHCONF(i)[EPOL]=0 and
                                            // drives it low when
                                            // MCSPI_CHCONF(i)[EPOL]=1
#define MCSPI_CH0CONF_TURBO     0x00080000  // Turbo mode 0 Turbo is
                                            // deactivated (recommended for
                                            // single SPI word transfer) 1 Turbo
                                            // is activated to maximize the
                                            // throughput for multi SPI words
                                            // transfer.
#define MCSPI_CH0CONF_IS        0x00040000  // Input Select 0 Data Line0
                                            // (SPIDAT[0]) selected for
                                            // reception. 1 Data Line1
                                            // (SPIDAT[1]) selected for
                                            // reception
#define MCSPI_CH0CONF_DPE1      0x00020000  // Transmission Enable for data
                                            // line 1 (SPIDATAGZEN[1]) 0 Data
                                            // Line1 (SPIDAT[1]) selected for
                                            // transmission 1 No transmission on
                                            // Data Line1 (SPIDAT[1])
#define MCSPI_CH0CONF_DPE0      0x00010000  // Transmission Enable for data
                                            // line 0 (SPIDATAGZEN[0]) 0 Data
                                            // Line0 (SPIDAT[0]) selected for
                                            // transmission 1 No transmission on
                                            // Data Line0 (SPIDAT[0])
#define MCSPI_CH0CONF_DMAR      0x00008000  // DMA Read request The DMA Read
                                            // request line is asserted when the
                                            // channel is enabled and a new data
                                            // is available in the receive
                                            // register of the channel. The DMA
                                            // Read request line is deasserted
                                            // on read completion of the receive
                                            // register of the channel. 0 DMA
                                            // Read Request disabled 1 DMA Read
                                            // Request enabled
#define MCSPI_CH0CONF_DMAW      0x00004000  // DMA Write request. The DMA Write
                                            // request line is asserted when The
                                            // channel is enabled and the
                                            // transmitter register of the
                                            // channel is empty. The DMA Write
                                            // request line is deasserted on
                                            // load completion of the
                                            // transmitter register of the
                                            // channel. 0 DMA Write Request
                                            // disabled 1 DMA Write Request
                                            // enabled
#define MCSPI_CH0CONF_TRM_M     0x00003000  // Transmit/Receive modes 0x0
                                            // Transmit and Receive mode 0x1
                                            // Receive only mode 0x2 Transmit
                                            // only mode 0x3 Reserved
#define MCSPI_CH0CONF_TRM_S     12
#define MCSPI_CH0CONF_WL_M      0x00000F80  // SPI word length 0x00 Reserved
                                            // 0x01 Reserved 0x02 Reserved 0x03
                                            // The SPI word is 4-bits long 0x04
                                            // The SPI word is 5-bits long 0x05
                                            // The SPI word is 6-bits long 0x06
                                            // The SPI word is 7-bits long 0x07
                                            // The SPI word is 8-bits long 0x08
                                            // The SPI word is 9-bits long 0x09
                                            // The SPI word is 10-bits long 0x0A
                                            // The SPI word is 11-bits long 0x0B
                                            // The SPI word is 12-bits long 0x0C
                                            // The SPI word is 13-bits long 0x0D
                                            // The SPI word is 14-bits long 0x0E
                                            // The SPI word is 15-bits long 0x0F
                                            // The SPI word is 16-bits long 0x10
                                            // The SPI word is 17-bits long 0x11
                                            // The SPI word is 18-bits long 0x12
                                            // The SPI word is 19-bits long 0x13
                                            // The SPI word is 20-bits long 0x14
                                            // The SPI word is 21-bits long 0x15
                                            // The SPI word is 22-bits long 0x16
                                            // The SPI word is 23-bits long 0x17
                                            // The SPI word is 24-bits long 0x18
                                            // The SPI word is 25-bits long 0x19
                                            // The SPI word is 26-bits long 0x1A
                                            // The SPI word is 27-bits long 0x1B
                                            // The SPI word is 28-bits long 0x1C
                                            // The SPI word is 29-bits long 0x1D
                                            // The SPI word is 30-bits long 0x1E
                                            // The SPI word is 31-bits long 0x1F
                                            // The SPI word is 32-bits long
#define MCSPI_CH0CONF_WL_S      7
#define MCSPI_CH0CONF_EPOL      0x00000040  // SPIEN polarity 0 SPIEN is held
                                            // high during the active state. 1
                                            // SPIEN is held low during the
                                            // active state.
#define MCSPI_CH0CONF_CLKD_M    0x0000003C  // Frequency divider for SPICLK.
                                            // (only when the module is a Master
                                            // SPI device). A programmable clock
                                            // divider divides the SPI reference
                                            // clock (CLKSPIREF) with a 4-bit
                                            // value and results in a new clock
                                            // SPICLK available to shift-in and
                                            // shift-out data. By default the
                                            // clock divider ratio has a power
                                            // of two granularity when
                                            // MCSPI_CHCONF[CLKG] is cleared
                                            // Otherwise this register is the 4
                                            // LSB bit of a 12-bit register
                                            // concatenated with clock divider
                                            // extension MCSPI_CHCTRL[EXTCLK]
                                            // register.The value description
                                            // below defines the clock ratio
                                            // when MCSPI_CHCONF[CLKG] is set to
                                            // 0. 0x0 1 0x1 2 0x2 4 0x3 8 0x4 16
                                            // 0x5 32 0x6 64 0x7 128 0x8 256 0x9
                                            // 512 0xA 1024 0xB 2048 0xC 4096
                                            // 0xD 8192 0xE 16384 0xF 32768
#define MCSPI_CH0CONF_CLKD_S    2
#define MCSPI_CH0CONF_POL       0x00000002  // SPICLK polarity 0 SPICLK is held
                                            // high during the active state 1
                                            // SPICLK is held low during the
                                            // active state
#define MCSPI_CH0CONF_PHA       0x00000001  // SPICLK phase 0 Data are latched
                                            // on odd numbered edges of SPICLK.
                                            // 1 Data are latched on even
                                            // numbered edges of SPICLK.
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_CH0STAT register.
//
//******************************************************************************
#define MCSPI_CH0STAT_RXFFF     0x00000040
#define MCSPI_CH0STAT_RXFFE     0x00000020
#define MCSPI_CH0STAT_TXFFF     0x00000010
#define MCSPI_CH0STAT_TXFFE     0x00000008
#define MCSPI_CH0STAT_EOT       0x00000004
#define MCSPI_CH0STAT_TXS       0x00000002
#define MCSPI_CH0STAT_RXS       0x00000001
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_CH0CTRL register.
//
//******************************************************************************
#define MCSPI_CH0CTRL_EXTCLK_M  0x0000FF00  // Clock ratio extension: This
                                            // register is used to concatenate
                                            // with MCSPI_CHCONF[CLKD] register
                                            // for clock ratio only when
                                            // granularity is one clock cycle
                                            // (MCSPI_CHCONF[CLKG] set to 1).
                                            // Then the max value reached is
                                            // 4096 clock divider ratio. 0x00
                                            // Clock ratio is CLKD + 1 0x01
                                            // Clock ratio is CLKD + 1 + 16 0xFF
                                            // Clock ratio is CLKD + 1 + 4080
#define MCSPI_CH0CTRL_EXTCLK_S  8
#define MCSPI_CH0CTRL_EN        0x00000001  // Channel Enable 0 "Channel ""i""
                                            // is not active" 1 "Channel ""i""
                                            // is active"
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_TX0 register.
//
//******************************************************************************
#define MCSPI_TX0_TDATA_M       0xFFFFFFFF  // Channel 0 Data to transmit
#define MCSPI_TX0_TDATA_S       0
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_RX0 register.
//
//******************************************************************************
#define MCSPI_RX0_RDATA_M       0xFFFFFFFF  // Channel 0 Received Data
#define MCSPI_RX0_RDATA_S       0
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_CH1CONF register.
//
//******************************************************************************
#define MCSPI_CH1CONF_CLKG      0x20000000  // Clock divider granularity This
                                            // register defines the granularity
                                            // of channel clock divider: power
                                            // of two or one clock cycle
                                            // granularity. When this bit is set
                                            // the register MCSPI_CHCTRL[EXTCLK]
                                            // must be configured to reach a
                                            // maximum of 4096 clock divider
                                            // ratio. Then The clock divider
                                            // ratio is a concatenation of
                                            // MCSPI_CHCONF[CLKD] and
                                            // MCSPI_CHCTRL[EXTCLK] values 0
                                            // Clock granularity of power of two
                                            // 1 One clock cycle ganularity
#define MCSPI_CH1CONF_FFER      0x10000000  // FIFO enabled for receive:Only
                                            // one channel can have this bit
                                            // field set. 0 The buffer is not
                                            // used to receive data. 1 The
                                            // buffer is used to receive data.
#define MCSPI_CH1CONF_FFEW      0x08000000  // FIFO enabled for Transmit:Only
                                            // one channel can have this bit
                                            // field set. 0 The buffer is not
                                            // used to transmit data. 1 The
                                            // buffer is used to transmit data.
#define MCSPI_CH1CONF_TCS1_M    0x06000000  // Chip Select Time Control This
                                            // 2-bits field defines the number
                                            // of interface clock cycles between
                                            // CS toggling and first or last
                                            // edge of SPI clock. 0x0 0.5 clock
                                            // cycle 0x1 1.5 clock cycle 0x2 2.5
                                            // clock cycle 0x3 3.5 clock cycle
#define MCSPI_CH1CONF_TCS1_S    25
#define MCSPI_CH1CONF_SBPOL     0x01000000  // Start bit polarity 0 Start bit
                                            // polarity is held to 0 during SPI
                                            // transfer. 1 Start bit polarity is
                                            // held to 1 during SPI transfer.
#define MCSPI_CH1CONF_SBE       0x00800000  // Start bit enable for SPI
                                            // transfer 0 Default SPI transfer
                                            // length as specified by WL bit
                                            // field 1 Start bit D/CX added
                                            // before SPI transfer polarity is
                                            // defined by MCSPI_CH1CONF[SBPOL]
#define MCSPI_CH1CONF_FORCE     0x00100000  // Manual SPIEN assertion to keep
                                            // SPIEN active between SPI words.
                                            // (single channel master mode only)
                                            // 0 Writing 0 into this bit drives
                                            // low the SPIEN line when
                                            // MCSPI_CHCONF(i)[EPOL]=0 and
                                            // drives it high when
                                            // MCSPI_CHCONF(i)[EPOL]=1. 1
                                            // Writing 1 into this bit drives
                                            // high the SPIEN line when
                                            // MCSPI_CHCONF(i)[EPOL]=0 and
                                            // drives it low when
                                            // MCSPI_CHCONF(i)[EPOL]=1
#define MCSPI_CH1CONF_TURBO     0x00080000  // Turbo mode 0 Turbo is
                                            // deactivated (recommended for
                                            // single SPI word transfer) 1 Turbo
                                            // is activated to maximize the
                                            // throughput for multi SPI words
                                            // transfer.
#define MCSPI_CH1CONF_IS        0x00040000  // Input Select 0 Data Line0
                                            // (SPIDAT[0]) selected for
                                            // reception. 1 Data Line1
                                            // (SPIDAT[1]) selected for
                                            // reception
#define MCSPI_CH1CONF_DPE1      0x00020000  // Transmission Enable for data
                                            // line 1 (SPIDATAGZEN[1]) 0 Data
                                            // Line1 (SPIDAT[1]) selected for
                                            // transmission 1 No transmission on
                                            // Data Line1 (SPIDAT[1])
#define MCSPI_CH1CONF_DPE0      0x00010000  // Transmission Enable for data
                                            // line 0 (SPIDATAGZEN[0]) 0 Data
                                            // Line0 (SPIDAT[0]) selected for
                                            // transmission 1 No transmission on
                                            // Data Line0 (SPIDAT[0])
#define MCSPI_CH1CONF_DMAR      0x00008000  // DMA Read request The DMA Read
                                            // request line is asserted when the
                                            // channel is enabled and a new data
                                            // is available in the receive
                                            // register of the channel. The DMA
                                            // Read request line is deasserted
                                            // on read completion of the receive
                                            // register of the channel. 0 DMA
                                            // Read Request disabled 1 DMA Read
                                            // Request enabled
#define MCSPI_CH1CONF_DMAW      0x00004000  // DMA Write request. The DMA Write
                                            // request line is asserted when The
                                            // channel is enabled and the
                                            // transmitter register of the
                                            // channel is empty. The DMA Write
                                            // request line is deasserted on
                                            // load completion of the
                                            // transmitter register of the
                                            // channel. 0 DMA Write Request
                                            // disabled 1 DMA Write Request
                                            // enabled
#define MCSPI_CH1CONF_TRM_M     0x00003000  // Transmit/Receive modes 0x0
                                            // Transmit and Receive mode 0x1
                                            // Receive only mode 0x2 Transmit
                                            // only mode 0x3 Reserved
#define MCSPI_CH1CONF_TRM_S     12
#define MCSPI_CH1CONF_WL_M      0x00000F80  // SPI word length 0x00 Reserved
                                            // 0x01 Reserved 0x02 Reserved 0x03
                                            // The SPI word is 4-bits long 0x04
                                            // The SPI word is 5-bits long 0x05
                                            // The SPI word is 6-bits long 0x06
                                            // The SPI word is 7-bits long 0x07
                                            // The SPI word is 8-bits long 0x08
                                            // The SPI word is 9-bits long 0x09
                                            // The SPI word is 10-bits long 0x0A
                                            // The SPI word is 11-bits long 0x0B
                                            // The SPI word is 12-bits long 0x0C
                                            // The SPI word is 13-bits long 0x0D
                                            // The SPI word is 14-bits long 0x0E
                                            // The SPI word is 15-bits long 0x0F
                                            // The SPI word is 16-bits long 0x10
                                            // The SPI word is 17-bits long 0x11
                                            // The SPI word is 18-bits long 0x12
                                            // The SPI word is 19-bits long 0x13
                                            // The SPI word is 20-bits long 0x14
                                            // The SPI word is 21-bits long 0x15
                                            // The SPI word is 22-bits long 0x16
                                            // The SPI word is 23-bits long 0x17
                                            // The SPI word is 24-bits long 0x18
                                            // The SPI word is 25-bits long 0x19
                                            // The SPI word is 26-bits long 0x1A
                                            // The SPI word is 27-bits long 0x1B
                                            // The SPI word is 28-bits long 0x1C
                                            // The SPI word is 29-bits long 0x1D
                                            // The SPI word is 30-bits long 0x1E
                                            // The SPI word is 31-bits long 0x1F
                                            // The SPI word is 32-bits long
#define MCSPI_CH1CONF_WL_S      7
#define MCSPI_CH1CONF_EPOL      0x00000040  // SPIEN polarity 0 SPIEN is held
                                            // high during the active state. 1
                                            // SPIEN is held low during the
                                            // active state.
#define MCSPI_CH1CONF_CLKD_M    0x0000003C  // Frequency divider for SPICLK.
                                            // (only when the module is a Master
                                            // SPI device). A programmable clock
                                            // divider divides the SPI reference
                                            // clock (CLKSPIREF) with a 4-bit
                                            // value and results in a new clock
                                            // SPICLK available to shift-in and
                                            // shift-out data. By default the
                                            // clock divider ratio has a power
                                            // of two granularity when
                                            // MCSPI_CHCONF[CLKG] is cleared
                                            // Otherwise this register is the 4
                                            // LSB bit of a 12-bit register
                                            // concatenated with clock divider
                                            // extension MCSPI_CHCTRL[EXTCLK]
                                            // register.The value description
                                            // below defines the clock ratio
                                            // when MCSPI_CHCONF[CLKG] is set to
                                            // 0. 0x0 1 0x1 2 0x2 4 0x3 8 0x4 16
                                            // 0x5 32 0x6 64 0x7 128 0x8 256 0x9
                                            // 512 0xA 1024 0xB 2048 0xC 4096
                                            // 0xD 8192 0xE 16384 0xF 32768
#define MCSPI_CH1CONF_CLKD_S    2
#define MCSPI_CH1CONF_POL       0x00000002  // SPICLK polarity 0 SPICLK is held
                                            // high during the active state 1
                                            // SPICLK is held low during the
                                            // active state
#define MCSPI_CH1CONF_PHA       0x00000001  // SPICLK phase 0 Data are latched
                                            // on odd numbered edges of SPICLK.
                                            // 1 Data are latched on even
                                            // numbered edges of SPICLK.
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_CH1STAT register.
//
//******************************************************************************
#define MCSPI_CH1STAT_RXFFF     0x00000040
#define MCSPI_CH1STAT_RXFFE     0x00000020
#define MCSPI_CH1STAT_TXFFF     0x00000010
#define MCSPI_CH1STAT_TXFFE     0x00000008
#define MCSPI_CH1STAT_EOT       0x00000004
#define MCSPI_CH1STAT_TXS       0x00000002
#define MCSPI_CH1STAT_RXS       0x00000001
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_CH1CTRL register.
//
//******************************************************************************
#define MCSPI_CH1CTRL_EXTCLK_M  0x0000FF00  // Clock ratio extension: This
                                            // register is used to concatenate
                                            // with MCSPI_CHCONF[CLKD] register
                                            // for clock ratio only when
                                            // granularity is one clock cycle
                                            // (MCSPI_CHCONF[CLKG] set to 1).
                                            // Then the max value reached is
                                            // 4096 clock divider ratio. 0x00
                                            // Clock ratio is CLKD + 1 0x01
                                            // Clock ratio is CLKD + 1 + 16 0xFF
                                            // Clock ratio is CLKD + 1 + 4080
#define MCSPI_CH1CTRL_EXTCLK_S  8
#define MCSPI_CH1CTRL_EN        0x00000001  // Channel Enable 0 "Channel ""i""
                                            // is not active" 1 "Channel ""i""
                                            // is active"
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_TX1 register.
//
//******************************************************************************
#define MCSPI_TX1_TDATA_M       0xFFFFFFFF  // Channel 1 Data to transmit
#define MCSPI_TX1_TDATA_S       0
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_RX1 register.
//
//******************************************************************************
#define MCSPI_RX1_RDATA_M       0xFFFFFFFF  // Channel 1 Received Data
#define MCSPI_RX1_RDATA_S       0
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_CH2CONF register.
//
//******************************************************************************
#define MCSPI_CH2CONF_CLKG      0x20000000  // Clock divider granularity This
                                            // register defines the granularity
                                            // of channel clock divider: power
                                            // of two or one clock cycle
                                            // granularity. When this bit is set
                                            // the register MCSPI_CHCTRL[EXTCLK]
                                            // must be configured to reach a
                                            // maximum of 4096 clock divider
                                            // ratio. Then The clock divider
                                            // ratio is a concatenation of
                                            // MCSPI_CHCONF[CLKD] and
                                            // MCSPI_CHCTRL[EXTCLK] values 0
                                            // Clock granularity of power of two
                                            // 1 One clock cycle ganularity
#define MCSPI_CH2CONF_FFER      0x10000000  // FIFO enabled for receive:Only
                                            // one channel can have this bit
                                            // field set. 0 The buffer is not
                                            // used to receive data. 1 The
                                            // buffer is used to receive data.
#define MCSPI_CH2CONF_FFEW      0x08000000  // FIFO enabled for Transmit:Only
                                            // one channel can have this bit
                                            // field set. 0 The buffer is not
                                            // used to transmit data. 1 The
                                            // buffer is used to transmit data.
#define MCSPI_CH2CONF_TCS2_M    0x06000000  // Chip Select Time Control This
                                            // 2-bits field defines the number
                                            // of interface clock cycles between
                                            // CS toggling and first or last
                                            // edge of SPI clock. 0x0 0.5 clock
                                            // cycle 0x1 1.5 clock cycle 0x2 2.5
                                            // clock cycle 0x3 3.5 clock cycle
#define MCSPI_CH2CONF_TCS2_S    25
#define MCSPI_CH2CONF_SBPOL     0x01000000  // Start bit polarity 0 Start bit
                                            // polarity is held to 0 during SPI
                                            // transfer. 1 Start bit polarity is
                                            // held to 1 during SPI transfer.
#define MCSPI_CH2CONF_SBE       0x00800000  // Start bit enable for SPI
                                            // transfer 0 Default SPI transfer
                                            // length as specified by WL bit
                                            // field 1 Start bit D/CX added
                                            // before SPI transfer polarity is
                                            // defined by MCSPI_CH2CONF[SBPOL]
#define MCSPI_CH2CONF_FORCE     0x00100000  // Manual SPIEN assertion to keep
                                            // SPIEN active between SPI words.
                                            // (single channel master mode only)
                                            // 0 Writing 0 into this bit drives
                                            // low the SPIEN line when
                                            // MCSPI_CHCONF(i)[EPOL]=0 and
                                            // drives it high when
                                            // MCSPI_CHCONF(i)[EPOL]=1. 1
                                            // Writing 1 into this bit drives
                                            // high the SPIEN line when
                                            // MCSPI_CHCONF(i)[EPOL]=0 and
                                            // drives it low when
                                            // MCSPI_CHCONF(i)[EPOL]=1
#define MCSPI_CH2CONF_TURBO     0x00080000  // Turbo mode 0 Turbo is
                                            // deactivated (recommended for
                                            // single SPI word transfer) 1 Turbo
                                            // is activated to maximize the
                                            // throughput for multi SPI words
                                            // transfer.
#define MCSPI_CH2CONF_IS        0x00040000  // Input Select 0 Data Line0
                                            // (SPIDAT[0]) selected for
                                            // reception. 1 Data Line1
                                            // (SPIDAT[1]) selected for
                                            // reception
#define MCSPI_CH2CONF_DPE1      0x00020000  // Transmission Enable for data
                                            // line 1 (SPIDATAGZEN[1]) 0 Data
                                            // Line1 (SPIDAT[1]) selected for
                                            // transmission 1 No transmission on
                                            // Data Line1 (SPIDAT[1])
#define MCSPI_CH2CONF_DPE0      0x00010000  // Transmission Enable for data
                                            // line 0 (SPIDATAGZEN[0]) 0 Data
                                            // Line0 (SPIDAT[0]) selected for
                                            // transmission 1 No transmission on
                                            // Data Line0 (SPIDAT[0])
#define MCSPI_CH2CONF_DMAR      0x00008000  // DMA Read request The DMA Read
                                            // request line is asserted when the
                                            // channel is enabled and a new data
                                            // is available in the receive
                                            // register of the channel. The DMA
                                            // Read request line is deasserted
                                            // on read completion of the receive
                                            // register of the channel. 0 DMA
                                            // Read Request disabled 1 DMA Read
                                            // Request enabled
#define MCSPI_CH2CONF_DMAW      0x00004000  // DMA Write request. The DMA Write
                                            // request line is asserted when The
                                            // channel is enabled and the
                                            // transmitter register of the
                                            // channel is empty. The DMA Write
                                            // request line is deasserted on
                                            // load completion of the
                                            // transmitter register of the
                                            // channel. 0 DMA Write Request
                                            // disabled 1 DMA Write Request
                                            // enabled
#define MCSPI_CH2CONF_TRM_M     0x00003000  // Transmit/Receive modes 0x0
                                            // Transmit and Receive mode 0x1
                                            // Receive only mode 0x2 Transmit
                                            // only mode 0x3 Reserved
#define MCSPI_CH2CONF_TRM_S     12
#define MCSPI_CH2CONF_WL_M      0x00000F80  // SPI word length 0x00 Reserved
                                            // 0x01 Reserved 0x02 Reserved 0x03
                                            // The SPI word is 4-bits long 0x04
                                            // The SPI word is 5-bits long 0x05
                                            // The SPI word is 6-bits long 0x06
                                            // The SPI word is 7-bits long 0x07
                                            // The SPI word is 8-bits long 0x08
                                            // The SPI word is 9-bits long 0x09
                                            // The SPI word is 10-bits long 0x0A
                                            // The SPI word is 11-bits long 0x0B
                                            // The SPI word is 12-bits long 0x0C
                                            // The SPI word is 13-bits long 0x0D
                                            // The SPI word is 14-bits long 0x0E
                                            // The SPI word is 15-bits long 0x0F
                                            // The SPI word is 16-bits long 0x10
                                            // The SPI word is 17-bits long 0x11
                                            // The SPI word is 18-bits long 0x12
                                            // The SPI word is 19-bits long 0x13
                                            // The SPI word is 20-bits long 0x14
                                            // The SPI word is 21-bits long 0x15
                                            // The SPI word is 22-bits long 0x16
                                            // The SPI word is 23-bits long 0x17
                                            // The SPI word is 24-bits long 0x18
                                            // The SPI word is 25-bits long 0x19
                                            // The SPI word is 26-bits long 0x1A
                                            // The SPI word is 27-bits long 0x1B
                                            // The SPI word is 28-bits long 0x1C
                                            // The SPI word is 29-bits long 0x1D
                                            // The SPI word is 30-bits long 0x1E
                                            // The SPI word is 31-bits long 0x1F
                                            // The SPI word is 32-bits long
#define MCSPI_CH2CONF_WL_S      7
#define MCSPI_CH2CONF_EPOL      0x00000040  // SPIEN polarity 0 SPIEN is held
                                            // high during the active state. 1
                                            // SPIEN is held low during the
                                            // active state.
#define MCSPI_CH2CONF_CLKD_M    0x0000003C  // Frequency divider for SPICLK.
                                            // (only when the module is a Master
                                            // SPI device). A programmable clock
                                            // divider divides the SPI reference
                                            // clock (CLKSPIREF) with a 4-bit
                                            // value and results in a new clock
                                            // SPICLK available to shift-in and
                                            // shift-out data. By default the
                                            // clock divider ratio has a power
                                            // of two granularity when
                                            // MCSPI_CHCONF[CLKG] is cleared
                                            // Otherwise this register is the 4
                                            // LSB bit of a 12-bit register
                                            // concatenated with clock divider
                                            // extension MCSPI_CHCTRL[EXTCLK]
                                            // register.The value description
                                            // below defines the clock ratio
                                            // when MCSPI_CHCONF[CLKG] is set to
                                            // 0. 0x0 1 0x1 2 0x2 4 0x3 8 0x4 16
                                            // 0x5 32 0x6 64 0x7 128 0x8 256 0x9
                                            // 512 0xA 1024 0xB 2048 0xC 4096
                                            // 0xD 8192 0xE 16384 0xF 32768
#define MCSPI_CH2CONF_CLKD_S    2
#define MCSPI_CH2CONF_POL       0x00000002  // SPICLK polarity 0 SPICLK is held
                                            // high during the active state 1
                                            // SPICLK is held low during the
                                            // active state
#define MCSPI_CH2CONF_PHA       0x00000001  // SPICLK phase 0 Data are latched
                                            // on odd numbered edges of SPICLK.
                                            // 1 Data are latched on even
                                            // numbered edges of SPICLK.
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_CH2STAT register.
//
//******************************************************************************
#define MCSPI_CH2STAT_RXFFF     0x00000040
#define MCSPI_CH2STAT_RXFFE     0x00000020
#define MCSPI_CH2STAT_TXFFF     0x00000010
#define MCSPI_CH2STAT_TXFFE     0x00000008
#define MCSPI_CH2STAT_EOT       0x00000004
#define MCSPI_CH2STAT_TXS       0x00000002
#define MCSPI_CH2STAT_RXS       0x00000001
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_CH2CTRL register.
//
//******************************************************************************
#define MCSPI_CH2CTRL_EXTCLK_M  0x0000FF00  // Clock ratio extension: This
                                            // register is used to concatenate
                                            // with MCSPI_CHCONF[CLKD] register
                                            // for clock ratio only when
                                            // granularity is one clock cycle
                                            // (MCSPI_CHCONF[CLKG] set to 1).
                                            // Then the max value reached is
                                            // 4096 clock divider ratio. 0x00
                                            // Clock ratio is CLKD + 1 0x01
                                            // Clock ratio is CLKD + 1 + 16 0xFF
                                            // Clock ratio is CLKD + 1 + 4080
#define MCSPI_CH2CTRL_EXTCLK_S  8
#define MCSPI_CH2CTRL_EN        0x00000001  // Channel Enable 0 "Channel ""i""
                                            // is not active" 1 "Channel ""i""
                                            // is active"
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_TX2 register.
//
//******************************************************************************
#define MCSPI_TX2_TDATA_M       0xFFFFFFFF  // Channel 2 Data to transmit
#define MCSPI_TX2_TDATA_S       0
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_RX2 register.
//
//******************************************************************************
#define MCSPI_RX2_RDATA_M       0xFFFFFFFF  // Channel 2 Received Data
#define MCSPI_RX2_RDATA_S       0
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_CH3CONF register.
//
//******************************************************************************
#define MCSPI_CH3CONF_CLKG      0x20000000  // Clock divider granularity This
                                            // register defines the granularity
                                            // of channel clock divider: power
                                            // of two or one clock cycle
                                            // granularity. When this bit is set
                                            // the register MCSPI_CHCTRL[EXTCLK]
                                            // must be configured to reach a
                                            // maximum of 4096 clock divider
                                            // ratio. Then The clock divider
                                            // ratio is a concatenation of
                                            // MCSPI_CHCONF[CLKD] and
                                            // MCSPI_CHCTRL[EXTCLK] values 0
                                            // Clock granularity of power of two
                                            // 1 One clock cycle ganularity
#define MCSPI_CH3CONF_FFER      0x10000000  // FIFO enabled for receive:Only
                                            // one channel can have this bit
                                            // field set. 0 The buffer is not
                                            // used to receive data. 1 The
                                            // buffer is used to receive data.
#define MCSPI_CH3CONF_FFEW      0x08000000  // FIFO enabled for Transmit:Only
                                            // one channel can have this bit
                                            // field set. 0 The buffer is not
                                            // used to transmit data. 1 The
                                            // buffer is used to transmit data.
#define MCSPI_CH3CONF_TCS3_M    0x06000000  // Chip Select Time Control This
                                            // 2-bits field defines the number
                                            // of interface clock cycles between
                                            // CS toggling and first or last
                                            // edge of SPI clock. 0x0 0.5 clock
                                            // cycle 0x1 1.5 clock cycle 0x2 2.5
                                            // clock cycle 0x3 3.5 clock cycle
#define MCSPI_CH3CONF_TCS3_S    25
#define MCSPI_CH3CONF_SBPOL     0x01000000  // Start bit polarity 0 Start bit
                                            // polarity is held to 0 during SPI
                                            // transfer. 1 Start bit polarity is
                                            // held to 1 during SPI transfer.
#define MCSPI_CH3CONF_SBE       0x00800000  // Start bit enable for SPI
                                            // transfer 0 Default SPI transfer
                                            // length as specified by WL bit
                                            // field 1 Start bit D/CX added
                                            // before SPI transfer polarity is
                                            // defined by MCSPI_CH3CONF[SBPOL]
#define MCSPI_CH3CONF_FORCE     0x00100000  // Manual SPIEN assertion to keep
                                            // SPIEN active between SPI words.
                                            // (single channel master mode only)
                                            // 0 Writing 0 into this bit drives
                                            // low the SPIEN line when
                                            // MCSPI_CHCONF(i)[EPOL]=0 and
                                            // drives it high when
                                            // MCSPI_CHCONF(i)[EPOL]=1. 1
                                            // Writing 1 into this bit drives
                                            // high the SPIEN line when
                                            // MCSPI_CHCONF(i)[EPOL]=0 and
                                            // drives it low when
                                            // MCSPI_CHCONF(i)[EPOL]=1
#define MCSPI_CH3CONF_TURBO     0x00080000  // Turbo mode 0 Turbo is
                                            // deactivated (recommended for
                                            // single SPI word transfer) 1 Turbo
                                            // is activated to maximize the
                                            // throughput for multi SPI words
                                            // transfer.
#define MCSPI_CH3CONF_IS        0x00040000  // Input Select 0 Data Line0
                                            // (SPIDAT[0]) selected for
                                            // reception. 1 Data Line1
                                            // (SPIDAT[1]) selected for
                                            // reception
#define MCSPI_CH3CONF_DPE1      0x00020000  // Transmission Enable for data
                                            // line 1 (SPIDATAGZEN[1]) 0 Data
                                            // Line1 (SPIDAT[1]) selected for
                                            // transmission 1 No transmission on
                                            // Data Line1 (SPIDAT[1])
#define MCSPI_CH3CONF_DPE0      0x00010000  // Transmission Enable for data
                                            // line 0 (SPIDATAGZEN[0]) 0 Data
                                            // Line0 (SPIDAT[0]) selected for
                                            // transmission 1 No transmission on
                                            // Data Line0 (SPIDAT[0])
#define MCSPI_CH3CONF_DMAR      0x00008000  // DMA Read request The DMA Read
                                            // request line is asserted when the
                                            // channel is enabled and a new data
                                            // is available in the receive
                                            // register of the channel. The DMA
                                            // Read request line is deasserted
                                            // on read completion of the receive
                                            // register of the channel. 0 DMA
                                            // Read Request disabled 1 DMA Read
                                            // Request enabled
#define MCSPI_CH3CONF_DMAW      0x00004000  // DMA Write request. The DMA Write
                                            // request line is asserted when The
                                            // channel is enabled and the
                                            // transmitter register of the
                                            // channel is empty. The DMA Write
                                            // request line is deasserted on
                                            // load completion of the
                                            // transmitter register of the
                                            // channel. 0 DMA Write Request
                                            // disabled 1 DMA Write Request
                                            // enabled
#define MCSPI_CH3CONF_TRM_M     0x00003000  // Transmit/Receive modes 0x0
                                            // Transmit and Receive mode 0x1
                                            // Receive only mode 0x2 Transmit
                                            // only mode 0x3 Reserved
#define MCSPI_CH3CONF_TRM_S     12
#define MCSPI_CH3CONF_WL_M      0x00000F80  // SPI word length 0x00 Reserved
                                            // 0x01 Reserved 0x02 Reserved 0x03
                                            // The SPI word is 4-bits long 0x04
                                            // The SPI word is 5-bits long 0x05
                                            // The SPI word is 6-bits long 0x06
                                            // The SPI word is 7-bits long 0x07
                                            // The SPI word is 8-bits long 0x08
                                            // The SPI word is 9-bits long 0x09
                                            // The SPI word is 10-bits long 0x0A
                                            // The SPI word is 11-bits long 0x0B
                                            // The SPI word is 12-bits long 0x0C
                                            // The SPI word is 13-bits long 0x0D
                                            // The SPI word is 14-bits long 0x0E
                                            // The SPI word is 15-bits long 0x0F
                                            // The SPI word is 16-bits long 0x10
                                            // The SPI word is 17-bits long 0x11
                                            // The SPI word is 18-bits long 0x12
                                            // The SPI word is 19-bits long 0x13
                                            // The SPI word is 20-bits long 0x14
                                            // The SPI word is 21-bits long 0x15
                                            // The SPI word is 22-bits long 0x16
                                            // The SPI word is 23-bits long 0x17
                                            // The SPI word is 24-bits long 0x18
                                            // The SPI word is 25-bits long 0x19
                                            // The SPI word is 26-bits long 0x1A
                                            // The SPI word is 27-bits long 0x1B
                                            // The SPI word is 28-bits long 0x1C
                                            // The SPI word is 29-bits long 0x1D
                                            // The SPI word is 30-bits long 0x1E
                                            // The SPI word is 31-bits long 0x1F
                                            // The SPI word is 32-bits long
#define MCSPI_CH3CONF_WL_S      7
#define MCSPI_CH3CONF_EPOL      0x00000040  // SPIEN polarity 0 SPIEN is held
                                            // high during the active state. 1
                                            // SPIEN is held low during the
                                            // active state.
#define MCSPI_CH3CONF_CLKD_M    0x0000003C  // Frequency divider for SPICLK.
                                            // (only when the module is a Master
                                            // SPI device). A programmable clock
                                            // divider divides the SPI reference
                                            // clock (CLKSPIREF) with a 4-bit
                                            // value and results in a new clock
                                            // SPICLK available to shift-in and
                                            // shift-out data. By default the
                                            // clock divider ratio has a power
                                            // of two granularity when
                                            // MCSPI_CHCONF[CLKG] is cleared
                                            // Otherwise this register is the 4
                                            // LSB bit of a 12-bit register
                                            // concatenated with clock divider
                                            // extension MCSPI_CHCTRL[EXTCLK]
                                            // register.The value description
                                            // below defines the clock ratio
                                            // when MCSPI_CHCONF[CLKG] is set to
                                            // 0. 0x0 1 0x1 2 0x2 4 0x3 8 0x4 16
                                            // 0x5 32 0x6 64 0x7 128 0x8 256 0x9
                                            // 512 0xA 1024 0xB 2048 0xC 4096
                                            // 0xD 8192 0xE 16384 0xF 32768
#define MCSPI_CH3CONF_CLKD_S    2
#define MCSPI_CH3CONF_POL       0x00000002  // SPICLK polarity 0 SPICLK is held
                                            // high during the active state 1
                                            // SPICLK is held low during the
                                            // active state
#define MCSPI_CH3CONF_PHA       0x00000001  // SPICLK phase 0 Data are latched
                                            // on odd numbered edges of SPICLK.
                                            // 1 Data are latched on even
                                            // numbered edges of SPICLK.
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_CH3STAT register.
//
//******************************************************************************
#define MCSPI_CH3STAT_RXFFF     0x00000040
#define MCSPI_CH3STAT_RXFFE     0x00000020
#define MCSPI_CH3STAT_TXFFF     0x00000010
#define MCSPI_CH3STAT_TXFFE     0x00000008
#define MCSPI_CH3STAT_EOT       0x00000004
#define MCSPI_CH3STAT_TXS       0x00000002
#define MCSPI_CH3STAT_RXS       0x00000001
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_CH3CTRL register.
//
//******************************************************************************
#define MCSPI_CH3CTRL_EXTCLK_M  0x0000FF00  // Clock ratio extension: This
                                            // register is used to concatenate
                                            // with MCSPI_CHCONF[CLKD] register
                                            // for clock ratio only when
                                            // granularity is one clock cycle
                                            // (MCSPI_CHCONF[CLKG] set to 1).
                                            // Then the max value reached is
                                            // 4096 clock divider ratio. 0x00
                                            // Clock ratio is CLKD + 1 0x01
                                            // Clock ratio is CLKD + 1 + 16 0xFF
                                            // Clock ratio is CLKD + 1 + 4080
#define MCSPI_CH3CTRL_EXTCLK_S  8
#define MCSPI_CH3CTRL_EN        0x00000001  // Channel Enable 0 "Channel ""i""
                                            // is not active" 1 "Channel ""i""
                                            // is active"
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_TX3 register.
//
//******************************************************************************
#define MCSPI_TX3_TDATA_M       0xFFFFFFFF  // Channel 3 Data to transmit
#define MCSPI_TX3_TDATA_S       0
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_RX3 register.
//
//******************************************************************************
#define MCSPI_RX3_RDATA_M       0xFFFFFFFF  // Channel 3 Received Data
#define MCSPI_RX3_RDATA_S       0
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_XFERLEVEL register.
//
//******************************************************************************
#define MCSPI_XFERLEVEL_WCNT_M  0xFFFF0000  // Spi word counterThis register
                                            // holds the programmable value of
                                            // number of SPI word to be
                                            // transferred on channel which is
                                            // using the FIFO buffer.When
                                            // transfer had started a read back
                                            // in this register returns the
                                            // current SPI word transfer index.
                                            // 0x0000 Counter not used 0x0001
                                            // one word 0xFFFE 65534 spi word
                                            // 0xFFFF 65535 spi word
#define MCSPI_XFERLEVEL_WCNT_S  16
#define MCSPI_XFERLEVEL_AFL_M   0x0000FF00  // Buffer Almost Full This register
                                            // holds the programmable almost
                                            // full level value used to
                                            // determine almost full buffer
                                            // condition. If the user wants an
                                            // interrupt or a DMA read request
                                            // to be issued during a receive
                                            // operation when the data buffer
                                            // holds at least n bytes then the
                                            // buffer MCSPI_MODULCTRL[AFL] must
                                            // be set with n-1.The size of this
                                            // register is defined by the
                                            // generic parameter FFNBYTE. 0x00
                                            // one byte 0x01 2 bytes 0xFE
                                            // 255bytes 0xFF 256bytes
#define MCSPI_XFERLEVEL_AFL_S   8
#define MCSPI_XFERLEVEL_AEL_M   0x000000FF  // Buffer Almost EmptyThis register
                                            // holds the programmable almost
                                            // empty level value used to
                                            // determine almost empty buffer
                                            // condition. If the user wants an
                                            // interrupt or a DMA write request
                                            // to be issued during a transmit
                                            // operation when the data buffer is
                                            // able to receive n bytes then the
                                            // buffer MCSPI_MODULCTRL[AEL] must
                                            // be set with n-1. 0x00 one byte
                                            // 0x01 2 bytes 0xFE 255 bytes 0xFF
                                            // 256bytes
#define MCSPI_XFERLEVEL_AEL_S   0
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_DAFTX register.
//
//******************************************************************************
#define MCSPI_DAFTX_DAFTDATA_M  0xFFFFFFFF  // FIFO Data to transmit with DMA
                                            // 256 bit aligned address. "This
                                            // Register is only is used when
                                            // MCSPI_MODULCTRL[FDAA] is set to
                                            // ""1"" and only one of the
                                            // MCSPI_CH(i)CONF[FFEW] of enabled
                                            // channels is set. If these
                                            // conditions are not respected any
                                            // access to this register return a
                                            // null value."
#define MCSPI_DAFTX_DAFTDATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCSPI_O_DAFRX register.
//
//******************************************************************************
#define MCSPI_DAFRX_DAFRDATA_M  0xFFFFFFFF  // FIFO Data to transmit with DMA
                                            // 256 bit aligned address. "This
                                            // Register is only is used when
                                            // MCSPI_MODULCTRL[FDAA] is set to
                                            // ""1"" and only one of the
                                            // MCSPI_CH(i)CONF[FFEW] of enabled
                                            // channels is set. If these
                                            // conditions are not respected any
                                            // access to this register return a
                                            // null value."
#define MCSPI_DAFRX_DAFRDATA_S  0



#endif // __HW_MCSPI_H__

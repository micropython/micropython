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

#ifndef __HW_MMCHS_H__
#define __HW_MMCHS_H__

//*****************************************************************************
//
// The following are defines for the MMCHS register offsets.
//
//*****************************************************************************
#define MMCHS_O_HL_REV          0x00000000  // IP Revision Identifier (X.Y.R)
                                            // Used by software to track
                                            // features bugs and compatibility
#define MMCHS_O_HL_HWINFO       0x00000004  // Information about the IP
                                            // module's hardware configuration
                                            // i.e. typically the module's HDL
                                            // generics (if any). Actual field
                                            // format and encoding is up to the
                                            // module's designer to decide.
#define MMCHS_O_HL_SYSCONFIG    0x00000010  // Clock management configuration
#define MMCHS_O_SYSCONFIG       0x00000110  // System Configuration Register
                                            // This register allows controlling
                                            // various parameters of the OCP
                                            // interface.
#define MMCHS_O_SYSSTATUS       0x00000114  // System Status Register This
                                            // register provides status
                                            // information about the module
                                            // excluding the interrupt status
                                            // information
#define MMCHS_O_CSRE            0x00000124  // Card status response error This
                                            // register enables the host
                                            // controller to detect card status
                                            // errors of response type R1 R1b
                                            // for all cards and of R5 R5b and
                                            // R6 response for cards types SD or
                                            // SDIO. When a bit MMCHS_CSRE[i] is
                                            // set to 1 if the corresponding bit
                                            // at the same position in the
                                            // response MMCHS_RSP0[i] is set to
                                            // 1 the host controller indicates a
                                            // card error (MMCHS_STAT[CERR])
                                            // interrupt status to avoid the
                                            // host driver reading the response
                                            // register (MMCHS_RSP0). Note: No
                                            // automatic card error detection
                                            // for autoCMD12 is implemented; the
                                            // host system has to check
                                            // autoCMD12 response register
                                            // (MMCHS_RESP76) for possible card
                                            // errors.
#define MMCHS_O_SYSTEST         0x00000128  // System Test register This
                                            // register is used to control the
                                            // signals that connect to I/O pins
                                            // when the module is configured in
                                            // system test (SYSTEST) mode for
                                            // boundary connectivity
                                            // verification. Note: In SYSTEST
                                            // mode a write into MMCHS_CMD
                                            // register will not start a
                                            // transfer. The buffer behaves as a
                                            // stack accessible only by the
                                            // local host (push and pop
                                            // operations). In this mode the
                                            // Transfer Block Size
                                            // (MMCHS_BLK[BLEN]) and the Blocks
                                            // count for current transfer
                                            // (MMCHS_BLK[NBLK]) are needed to
                                            // generate a Buffer write ready
                                            // interrupt (MMCHS_STAT[BWR]) or a
                                            // Buffer read ready interrupt
                                            // (MMCHS_STAT[BRR]) and DMA
                                            // requests if enabled.
#define MMCHS_O_CON             0x0000012C  // Configuration register This
                                            // register is used: - to select the
                                            // functional mode or the SYSTEST
                                            // mode for any card. - to send an
                                            // initialization sequence to any
                                            // card. - to enable the detection
                                            // on DAT[1] of a card interrupt for
                                            // SDIO cards only. and also to
                                            // configure : - specific data and
                                            // command transfers for MMC cards
                                            // only. - the parameters related to
                                            // the card detect and write protect
                                            // input signals.
#define MMCHS_O_PWCNT           0x00000130  // Power counter register This
                                            // register is used to program a mmc
                                            // counter to delay command
                                            // transfers after activating the
                                            // PAD power this value depends on
                                            // PAD characteristics and voltage.
#define MMCHS_O_BLK             0x00000204  // Transfer Length Configuration
                                            // register MMCHS_BLK[BLEN] is the
                                            // block size register.
                                            // MMCHS_BLK[NBLK] is the block
                                            // count register. This register
                                            // shall be used for any card.
#define MMCHS_O_ARG             0x00000208  // Command argument Register This
                                            // register contains command
                                            // argument specified as bit 39-8 of
                                            // Command-Format These registers
                                            // must be initialized prior to
                                            // sending the command itself to the
                                            // card (write action into the
                                            // register MMCHS_CMD register).
                                            // Only exception is for a command
                                            // index specifying stuff bits in
                                            // arguments making a write
                                            // unnecessary.
#define MMCHS_O_CMD             0x0000020C  // Command and transfer mode
                                            // register MMCHS_CMD[31:16] = the
                                            // command register MMCHS_CMD[15:0]
                                            // = the transfer mode. This
                                            // register configures the data and
                                            // command transfers. A write into
                                            // the most significant byte send
                                            // the command. A write into
                                            // MMCHS_CMD[15:0] registers during
                                            // data transfer has no effect. This
                                            // register shall be used for any
                                            // card. Note: In SYSTEST mode a
                                            // write into MMCHS_CMD register
                                            // will not start a transfer.
#define MMCHS_O_RSP10           0x00000210  // Command response[31:0] Register
                                            // This 32-bit register holds bits
                                            // positions [31:0] of command
                                            // response type
                                            // R1/R1b/R2/R3/R4/R5/R5b/R6
#define MMCHS_O_RSP32           0x00000214  // Command response[63:32] Register
                                            // This 32-bit register holds bits
                                            // positions [63:32] of command
                                            // response type R2
#define MMCHS_O_RSP54           0x00000218  // Command response[95:64] Register
                                            // This 32-bit register holds bits
                                            // positions [95:64] of command
                                            // response type R2
#define MMCHS_O_RSP76           0x0000021C  // Command response[127:96]
                                            // Register This 32-bit register
                                            // holds bits positions [127:96] of
                                            // command response type R2
#define MMCHS_O_DATA            0x00000220  // Data Register This register is
                                            // the 32-bit entry point of the
                                            // buffer for read or write data
                                            // transfers. The buffer size is
                                            // 32bits x256(1024 bytes). Bytes
                                            // within a word are stored and read
                                            // in little endian format. This
                                            // buffer can be used as two 512
                                            // byte buffers to transfer data
                                            // efficiently without reducing the
                                            // throughput. Sequential and
                                            // contiguous access is necessary to
                                            // increment the pointer correctly.
                                            // Random or skipped access is not
                                            // allowed. In little endian if the
                                            // local host accesses this register
                                            // byte-wise or 16bit-wise the least
                                            // significant byte (bits [7:0])
                                            // must always be written/read
                                            // first. The update of the buffer
                                            // address is done on the most
                                            // significant byte write for full
                                            // 32-bit DATA register or on the
                                            // most significant byte of the last
                                            // word of block transfer. Example
                                            // 1: Byte or 16-bit access
                                            // Mbyteen[3:0]=0001 (1-byte) =>
                                            // Mbyteen[3:0]=0010 (1-byte) =>
                                            // Mbyteen[3:0]=1100 (2-bytes) OK
                                            // Mbyteen[3:0]=0001 (1-byte) =>
                                            // Mbyteen[3:0]=0010 (1-byte) =>
                                            // Mbyteen[3:0]=0100 (1-byte) OK
                                            // Mbyteen[3:0]=0001 (1-byte) =>
                                            // Mbyteen[3:0]=0010 (1-byte) =>
                                            // Mbyteen[3:0]=1000 (1-byte) Bad
#define MMCHS_O_PSTATE          0x00000224  // Present state register The Host
                                            // can get status of the Host
                                            // Controller from this 32-bit read
                                            // only register.
#define MMCHS_O_HCTL            0x00000228  // Control register This register
                                            // defines the host controls to set
                                            // power wakeup and transfer
                                            // parameters. MMCHS_HCTL[31:24] =
                                            // Wakeup control MMCHS_HCTL[23:16]
                                            // = Block gap control
                                            // MMCHS_HCTL[15:8] = Power control
                                            // MMCHS_HCTL[7:0] = Host control
#define MMCHS_O_SYSCTL          0x0000022C  // SD system control register This
                                            // register defines the system
                                            // controls to set software resets
                                            // clock frequency management and
                                            // data timeout. MMCHS_SYSCTL[31:24]
                                            // = Software resets
                                            // MMCHS_SYSCTL[23:16] = Timeout
                                            // control MMCHS_SYSCTL[15:0] =
                                            // Clock control
#define MMCHS_O_STAT            0x00000230  // Interrupt status register The
                                            // interrupt status regroups all the
                                            // status of the module internal
                                            // events that can generate an
                                            // interrupt. MMCHS_STAT[31:16] =
                                            // Error Interrupt Status
                                            // MMCHS_STAT[15:0] = Normal
                                            // Interrupt Status
#define MMCHS_O_IE              0x00000234  // Interrupt SD enable register
                                            // This register allows to
                                            // enable/disable the module to set
                                            // status bits on an event-by-event
                                            // basis. MMCHS_IE[31:16] = Error
                                            // Interrupt Status Enable
                                            // MMCHS_IE[15:0] = Normal Interrupt
                                            // Status Enable
#define MMCHS_O_ISE             0x00000238  // Interrupt signal enable register
                                            // This register allows to
                                            // enable/disable the module
                                            // internal sources of status on an
                                            // event-by-event basis.
                                            // MMCHS_ISE[31:16] = Error
                                            // Interrupt Signal Enable
                                            // MMCHS_ISE[15:0] = Normal
                                            // Interrupt Signal Enable
#define MMCHS_O_AC12            0x0000023C  // Auto CMD12 Error Status Register
                                            // The host driver may determine
                                            // which of the errors cases related
                                            // to Auto CMD12 has occurred by
                                            // checking this MMCHS_AC12 register
                                            // when an Auto CMD12 Error
                                            // interrupt occurs. This register
                                            // is valid only when Auto CMD12 is
                                            // enabled (MMCHS_CMD[ACEN]) and
                                            // Auto CMD12Error (MMCHS_STAT[ACE])
                                            // is set to 1. Note: These bits are
                                            // automatically reset when starting
                                            // a new adtc command with data.
#define MMCHS_O_CAPA            0x00000240  // Capabilities register This
                                            // register lists the capabilities
                                            // of the MMC/SD/SDIO host
                                            // controller.
#define MMCHS_O_CUR_CAPA        0x00000248  // Maximum current capabilities
                                            // Register This register indicates
                                            // the maximum current capability
                                            // for each voltage. The value is
                                            // meaningful if the voltage support
                                            // is set in the capabilities
                                            // register (MMCHS_CAPA).
                                            // Initialization of this register
                                            // (via a write access to this
                                            // register) depends on the system
                                            // capabilities. The host driver
                                            // shall not modify this register
                                            // after the initilaization. This
                                            // register is only reinitialized by
                                            // a hard reset (via RESETN signal)
#define MMCHS_O_FE              0x00000250  // Force Event Register for Error
                                            // Interrupt status The force Event
                                            // Register is not a physically
                                            // implemented register. Rather it
                                            // is an address at which the Error
                                            // Interrupt Status register can be
                                            // written. The effect of a write to
                                            // this address will be reflected in
                                            // the Error Interrupt Status
                                            // Register if corresponding bit of
                                            // the Error Interrupt Status Enable
                                            // Register is set.
#define MMCHS_O_ADMAES          0x00000254  // ADMA Error Status Register When
                                            // ADMA Error Interrupt is occurred
                                            // the ADMA Error States field in
                                            // this register holds the ADMA
                                            // state and the ADMA System Address
                                            // Register holds the address around
                                            // the error descriptor. For
                                            // recovering the error the Host
                                            // Driver requires the ADMA state to
                                            // identify the error descriptor
                                            // address as follows: ST_STOP:
                                            // Previous location set in the ADMA
                                            // System Address register is the
                                            // error descriptor address ST_FDS:
                                            // Current location set in the ADMA
                                            // System Address register is the
                                            // error descriptor address ST_CADR:
                                            // This sate is never set because do
                                            // not generate ADMA error in this
                                            // state. ST_TFR: Previous location
                                            // set in the ADMA System Address
                                            // register is the error descriptor
                                            // address In case of write
                                            // operation the Host Driver should
                                            // use ACMD22 to get the number of
                                            // written block rather than using
                                            // this information since unwritten
                                            // data may exist in the Host
                                            // Controller. The Host Controller
                                            // generates the ADMA Error
                                            // Interrupt when it detects invalid
                                            // descriptor data (Valid=0) at the
                                            // ST_FDS state. In this case ADMA
                                            // Error State indicates that an
                                            // error occurs at ST_FDS state. The
                                            // Host Driver may find that the
                                            // Valid bit is not set in the error
                                            // descriptor.
#define MMCHS_O_ADMASAL         0x00000258  // ADMA System address Low bits
#define MMCHS_O_REV             0x000002FC  // Versions Register This register
                                            // contains the hard coded RTL
                                            // vendor revision number the
                                            // version number of SD
                                            // specification compliancy and a
                                            // slot status bit. MMCHS_REV[31:16]
                                            // = Host controller version
                                            // MMCHS_REV[15:0] = Slot Interrupt
                                            // Status



//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_HL_REV register.
//
//******************************************************************************
#define MMCHS_HL_REV_SCHEME_M   0xC0000000
#define MMCHS_HL_REV_SCHEME_S   30
#define MMCHS_HL_REV_FUNC_M     0x0FFF0000  // Function indicates a software
                                            // compatible module family. If
                                            // there is no level of software
                                            // compatibility a new Func number
                                            // (and hence REVISION) should be
                                            // assigned.
#define MMCHS_HL_REV_FUNC_S     16
#define MMCHS_HL_REV_R_RTL_M    0x0000F800  // RTL Version (R) maintained by IP
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
#define MMCHS_HL_REV_R_RTL_S    11
#define MMCHS_HL_REV_X_MAJOR_M  0x00000700  // Major Revision (X) maintained by
                                            // IP specification owner. X changes
                                            // ONLY when: (1) There is a major
                                            // feature addition. An example
                                            // would be adding Master Mode to
                                            // Utopia Level2. The Func field (or
                                            // Class/Type in old PID format)
                                            // will remain the same. X does NOT
                                            // change due to: (1) Bug fixes (2)
                                            // Change in feature parameters.
#define MMCHS_HL_REV_X_MAJOR_S  8
#define MMCHS_HL_REV_CUSTOM_M   0x000000C0
#define MMCHS_HL_REV_CUSTOM_S   6
#define MMCHS_HL_REV_Y_MINOR_M  0x0000003F  // Minor Revision (Y) maintained by
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
#define MMCHS_HL_REV_Y_MINOR_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_HL_HWINFO register.
//
//******************************************************************************
#define MMCHS_HL_HWINFO_RETMODE 0x00000040
#define MMCHS_HL_HWINFO_MEM_SIZE_M \
                                0x0000003C

#define MMCHS_HL_HWINFO_MEM_SIZE_S 2
#define MMCHS_HL_HWINFO_MERGE_MEM \
                                0x00000002

#define MMCHS_HL_HWINFO_MADMA_EN \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// MMCHS_O_HL_SYSCONFIG register.
//
//******************************************************************************
#define MMCHS_HL_SYSCONFIG_STANDBYMODE_M \
                                0x00000030  // Configuration of the local
                                            // initiator state management mode.
                                            // By definition initiator may
                                            // generate read/write transaction
                                            // as long as it is out of STANDBY
                                            // state. 0x0 Force-standby mode:
                                            // local initiator is
                                            // unconditionally placed in standby
                                            // state.Backup mode for debug only.
                                            // 0x1 No-standby mode: local
                                            // initiator is unconditionally
                                            // placed out of standby
                                            // state.Backup mode for debug only.
                                            // 0x2 Smart-standby mode: local
                                            // initiator standby status depends
                                            // on local conditions i.e. the
                                            // module's functional requirement
                                            // from the initiator.IP module
                                            // shall not generate
                                            // (initiator-related) wakeup
                                            // events. 0x3 "Smart-Standby
                                            // wakeup-capable mode: local
                                            // initiator standby status depends
                                            // on local conditions i.e. the
                                            // module's functional requirement
                                            // from the initiator. IP module may
                                            // generate (master-related) wakeup
                                            // events when in standby state.Mode
                                            // is only relevant if the
                                            // appropriate IP module ""mwakeup""
                                            // output is implemented."

#define MMCHS_HL_SYSCONFIG_STANDBYMODE_S 4
#define MMCHS_HL_SYSCONFIG_IDLEMODE_M \
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

#define MMCHS_HL_SYSCONFIG_IDLEMODE_S 2
#define MMCHS_HL_SYSCONFIG_FREEEMU \
                                0x00000002  // Sensitivity to emulation (debug)
                                            // suspend input signal.
                                            // Functionality NOT implemented in
                                            // MMCHS. 0 IP module is sensitive
                                            // to emulation suspend 1 IP module
                                            // is not sensitive to emulation
                                            // suspend

#define MMCHS_HL_SYSCONFIG_SOFTRESET \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_SYSCONFIG register.
//
//******************************************************************************
#define MMCHS_SYSCONFIG_STANDBYMODE_M \
                                0x00003000  // Master interface power
                                            // Management standby/wait control.
                                            // The bit field is only useful when
                                            // generic parameter MADMA_EN
                                            // (Master ADMA enable) is set as
                                            // active otherwise it is a read
                                            // only register read a '0'. 0x0
                                            // Force-standby. Mstandby is forced
                                            // unconditionnaly. 0x1 No-standby.
                                            // Mstandby is never asserted. 0x2
                                            // Smart-standby mode: local
                                            // initiator standby status depends
                                            // on local conditions i.e. the
                                            // module's functional requirement
                                            // from the initiator.IP module
                                            // shall not generate
                                            // (initiator-related) wakeup
                                            // events. 0x3 Smart-Standby
                                            // wakeup-capable mode: "local
                                            // initiator standby status depends
                                            // on local conditions i.e. the
                                            // module's functional requirement
                                            // from the initiator. IP module may
                                            // generate (master-related) wakeup
                                            // events when in standby state.Mode
                                            // is only relevant if the
                                            // appropriate IP module ""mwakeup""
                                            // output is implemented."

#define MMCHS_SYSCONFIG_STANDBYMODE_S 12
#define MMCHS_SYSCONFIG_CLOCKACTIVITY_M \
                                0x00000300  // Clocks activity during wake up
                                            // mode period. Bit8: OCP interface
                                            // clock Bit9: Functional clock 0x0
                                            // OCP and Functional clock may be
                                            // switched off. 0x1 OCP clock is
                                            // maintained. Functional clock may
                                            // be switched-off. 0x2 Functional
                                            // clock is maintained. OCP clock
                                            // may be switched-off. 0x3 OCP and
                                            // Functional clocks are maintained.

#define MMCHS_SYSCONFIG_CLOCKACTIVITY_S 8
#define MMCHS_SYSCONFIG_SIDLEMODE_M \
                                0x00000018  // Power management 0x0 If an idle
                                            // request is detected the MMCHS
                                            // acknowledges it unconditionally
                                            // and goes in Inactive mode.
                                            // Interrupt and DMA requests are
                                            // unconditionally de-asserted. 0x1
                                            // If an idle request is detected
                                            // the request is ignored and the
                                            // module keeps on behaving
                                            // normally. 0x2 Smart-idle mode:
                                            // local target's idle state
                                            // eventually follows (acknowledges)
                                            // the system's idle requests
                                            // depending on the IP module's
                                            // internal requirements.IP module
                                            // shall not generate (IRQ- or
                                            // DMA-request-related) wakeup
                                            // events. 0x3 Smart-idle
                                            // wakeup-capable mode: "local
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

#define MMCHS_SYSCONFIG_SIDLEMODE_S 3
#define MMCHS_SYSCONFIG_ENAWAKEUP \
                                0x00000004  // Wakeup feature control 0 Wakeup
                                            // capability is disabled 1 Wakeup
                                            // capability is enabled

#define MMCHS_SYSCONFIG_SOFTRESET \
                                0x00000002

#define MMCHS_SYSCONFIG_AUTOIDLE \
                                0x00000001  // Internal Clock gating strategy 0
                                            // Clocks are free-running 1
                                            // Automatic clock gating strategy
                                            // is applied based on the OCP and
                                            // MMC interface activity

//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_SYSSTATUS register.
//
//******************************************************************************
#define MMCHS_SYSSTATUS_RESETDONE \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_CSRE register.
//
//******************************************************************************
#define MMCHS_CSRE_CSRE_M       0xFFFFFFFF  // Card status response error
#define MMCHS_CSRE_CSRE_S       0
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_SYSTEST register.
//
//******************************************************************************
#define MMCHS_SYSTEST_OBI       0x00010000
#define MMCHS_SYSTEST_SDCD      0x00008000
#define MMCHS_SYSTEST_SDWP      0x00004000
#define MMCHS_SYSTEST_WAKD      0x00002000
#define MMCHS_SYSTEST_SSB       0x00001000
#define MMCHS_SYSTEST_D7D       0x00000800
#define MMCHS_SYSTEST_D6D       0x00000400
#define MMCHS_SYSTEST_D5D       0x00000200
#define MMCHS_SYSTEST_D4D       0x00000100
#define MMCHS_SYSTEST_D3D       0x00000080
#define MMCHS_SYSTEST_D2D       0x00000040
#define MMCHS_SYSTEST_D1D       0x00000020
#define MMCHS_SYSTEST_D0D       0x00000010
#define MMCHS_SYSTEST_DDIR      0x00000008
#define MMCHS_SYSTEST_CDAT      0x00000004
#define MMCHS_SYSTEST_CDIR      0x00000002
#define MMCHS_SYSTEST_MCKD      0x00000001
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_CON register.
//
//******************************************************************************
#define MMCHS_CON_SDMA_LNE      0x00200000  // Slave DMA Level/Edge Request:
                                            // The waveform of the DMA request
                                            // can be configured either edge
                                            // sensitive with early de-assertion
                                            // on first access to MMCHS_DATA
                                            // register or late de-assertion
                                            // request remains active until last
                                            // allowed data written into
                                            // MMCHS_DATA. 0 Slave DMA edge
                                            // sensitive Early DMA de-assertion
                                            // 1 Slave DMA level sensitive Late
                                            // DMA de-assertion
#define MMCHS_CON_DMA_MNS       0x00100000  // DMA Master or Slave selection:
                                            // When this bit is set and the
                                            // controller is configured to use
                                            // the DMA Ocp master interface is
                                            // used to get datas from system
                                            // using ADMA2 procedure (direct
                                            // access to the memory).This option
                                            // is only available if generic
                                            // parameter MADMA_EN is asserted to
                                            // '1'. 0 The controller is slave on
                                            // data transfers with system. 1 The
                                            // controller is master on data
                                            // exchange with system controller
                                            // must be configured as using DMA.
#define MMCHS_CON_DDR           0x00080000  // Dual Data Rate mode: When this
                                            // register is set the controller
                                            // uses both clock edge to emit or
                                            // receive data. Odd bytes are
                                            // transmitted on falling edges and
                                            // even bytes are transmitted on
                                            // rise edges. It only applies on
                                            // Data bytes and CRC Start end bits
                                            // and CRC status are kept full
                                            // cycle. This bit field is only
                                            // meaningful and active for even
                                            // clock divider ratio of
                                            // MMCHS_SYSCTL[CLKD] it is
                                            // insensitive to MMCHS_HCTL[HSPE]
                                            // setting. 0 Standard mode : data
                                            // are transmitted on a single edge
                                            // depending on MMCHS_HCTRL[HSPE]. 1
                                            // Data Bytes and CRC are
                                            // transmitted on both edge.
#define MMCHS_CON_BOOT_CF0      0x00040000
#define MMCHS_CON_BOOT_ACK      0x00020000  // Book acknowledge received: When
                                            // this bit is set the controller
                                            // should receive a boot status on
                                            // DAT0 line after next command
                                            // issued. If no status is received
                                            // a data timeout will be generated.
                                            // 0 No acknowledge to be received 1
                                            // A boot status will be received on
                                            // DAT0 line after issuing a
                                            // command.
#define MMCHS_CON_CLKEXTFREE    0x00010000  // External clock free running:
                                            // This register is used to maintain
                                            // card clock out of transfer
                                            // transaction to enable slave
                                            // module for example to generate a
                                            // synchronous interrupt on DAT[1].
                                            // The Clock will be maintain only
                                            // if MMCHS_SYSCTL[CEN] is set. 0
                                            // External card clock is cut off
                                            // outside active transaction
                                            // period. 1 External card clock is
                                            // maintain even out of active
                                            // transaction period only if
                                            // MMCHS_SYSCTL[CEN] is set.
#define MMCHS_CON_PADEN         0x00008000  // Control Power for MMC Lines:
                                            // This register is only useful when
                                            // MMC PADs contain power saving
                                            // mechanism to minimize its leakage
                                            // power. It works as a GPIO that
                                            // directly control the ACTIVE pin
                                            // of PADs. Excepted for DAT[1] the
                                            // signal is also combine outside
                                            // the module with the dedicated
                                            // power control MMCHS_CON[CTPL]
                                            // bit. 0 ADPIDLE module pin is not
                                            // forced it is automatically
                                            // generated by the MMC fsms. 1
                                            // ADPIDLE module pin is forced to
                                            // active state.
#define MMCHS_CON_OBIE          0x00004000  // Out-of-Band Interrupt Enable MMC
                                            // cards only: This bit enables the
                                            // detection of Out-of-Band
                                            // Interrupt on MMCOBI input pin.
                                            // The usage of the Out-of-Band
                                            // signal (OBI) is optional and
                                            // depends on the system
                                            // integration. 0 Out-of-Band
                                            // interrupt detection disabled 1
                                            // Out-of-Band interrupt detection
                                            // enabled
#define MMCHS_CON_OBIP          0x00002000  // Out-of-Band Interrupt Polarity
                                            // MMC cards only: This bit selects
                                            // the active level of the
                                            // out-of-band interrupt coming from
                                            // MMC cards. The usage of the
                                            // Out-of-Band signal (OBI) is
                                            // optional and depends on the
                                            // system integration. 0 active high
                                            // level 1 active low level
#define MMCHS_CON_CEATA         0x00001000  // CE-ATA control mode MMC cards
                                            // compliant with CE-ATA:By default
                                            // this bit is set to 0. It is use
                                            // to indicate that next commands
                                            // are considered as specific CE-ATA
                                            // commands that potentially use
                                            // 'command completion' features. 0
                                            // Standard MMC/SD/SDIO mode. 1
                                            // CE-ATA mode next commands are
                                            // considered as CE-ATA commands.
#define MMCHS_CON_CTPL          0x00000800  // Control Power for DAT[1] line
                                            // MMC and SD cards: By default this
                                            // bit is set to 0 and the host
                                            // controller automatically disables
                                            // all the input buffers outside of
                                            // a transaction to minimize the
                                            // leakage current. SDIO cards: When
                                            // this bit is set to 1 the host
                                            // controller automatically disables
                                            // all the input buffers except the
                                            // buffer of DAT[1] outside of a
                                            // transaction in order to detect
                                            // asynchronous card interrupt on
                                            // DAT[1] line and minimize the
                                            // leakage current of the buffers. 0
                                            // Disable all the input buffers
                                            // outside of a transaction. 1
                                            // Disable all the input buffers
                                            // except the buffer of DAT[1]
                                            // outside of a transaction.
#define MMCHS_CON_DVAL_M        0x00000600  // Debounce filter value All cards
                                            // This register is used to define a
                                            // debounce period to filter the
                                            // card detect input signal (SDCD).
                                            // The usage of the card detect
                                            // input signal (SDCD) is optional
                                            // and depends on the system
                                            // integration and the type of the
                                            // connector housing that
                                            // accommodates the card. 0x0 33 us
                                            // debounce period 0x1 231 us
                                            // debounce period 0x2 1 ms debounce
                                            // period 0x3 84 ms debounce period
#define MMCHS_CON_DVAL_S        9
#define MMCHS_CON_WPP           0x00000100  // Write protect polarity For SD
                                            // and SDIO cards only This bit
                                            // selects the active level of the
                                            // write protect input signal
                                            // (SDWP). The usage of the write
                                            // protect input signal (SDWP) is
                                            // optional and depends on the
                                            // system integration and the type
                                            // of the connector housing that
                                            // accommodates the card. 0 active
                                            // high level 1 active low level
#define MMCHS_CON_CDP           0x00000080  // Card detect polarity All cards
                                            // This bit selects the active level
                                            // of the card detect input signal
                                            // (SDCD). The usage of the card
                                            // detect input signal (SDCD) is
                                            // optional and depends on the
                                            // system integration and the type
                                            // of the connector housing that
                                            // accommodates the card. 0 active
                                            // high level 1 active low level
#define MMCHS_CON_MIT           0x00000040  // MMC interrupt command Only for
                                            // MMC cards. This bit must be set
                                            // to 1 when the next write access
                                            // to the command register
                                            // (MMCHS_CMD) is for writing a MMC
                                            // interrupt command (CMD40)
                                            // requiring the command timeout
                                            // detection to be disabled for the
                                            // command response. 0 Command
                                            // timeout enabled 1 Command timeout
                                            // disabled
#define MMCHS_CON_DW8           0x00000020  // 8-bit mode MMC select For
                                            // SD/SDIO cards this bit must be
                                            // set to 0. For MMC card this bit
                                            // must be set following a valid
                                            // SWITCH command (CMD6) with the
                                            // correct value and extend CSD
                                            // index written in the argument.
                                            // Prior to this command the MMC
                                            // card configuration register (CSD
                                            // and EXT_CSD) must be verified for
                                            // compliancy with MMC standard
                                            // specification 4.x (see section
                                            // 3.6). 0 1-bit or 4-bit Data width
                                            // (DAT[0] used MMC SD cards) 1
                                            // 8-bit Data width (DAT[7:0] used
                                            // MMC cards)
#define MMCHS_CON_MODE          0x00000010  // Mode select All cards These bits
                                            // select between Functional mode
                                            // and SYSTEST mode. 0 Functional
                                            // mode. Transfers to the
                                            // MMC/SD/SDIO cards follow the card
                                            // protocol. MMC clock is enabled.
                                            // MMC/SD transfers are operated
                                            // under the control of the CMD
                                            // register. 1 SYSTEST mode The
                                            // signal pins are configured as
                                            // general-purpose input/output and
                                            // the 1024-byte buffer is
                                            // configured as a stack memory
                                            // accessible only by the local host
                                            // or system DMA. The pins retain
                                            // their default type (input output
                                            // or in-out). SYSTEST mode is
                                            // operated under the control of the
                                            // SYSTEST register.
#define MMCHS_CON_STR           0x00000008  // Stream command Only for MMC
                                            // cards. This bit must be set to 1
                                            // only for the stream data
                                            // transfers (read or write) of the
                                            // adtc commands. Stream read is a
                                            // class 1 command (CMD11:
                                            // READ_DAT_UNTIL_STOP). Stream
                                            // write is a class 3 command
                                            // (CMD20: WRITE_DAT_UNTIL_STOP). 0
                                            // Block oriented data transfer 1
                                            // Stream oriented data transfer
#define MMCHS_CON_HR            0x00000004  // Broadcast host response Only for
                                            // MMC cards. This register is used
                                            // to force the host to generate a
                                            // 48-bit response for bc command
                                            // type. "It can be used to
                                            // terminate the interrupt mode by
                                            // generating a CMD40 response by
                                            // the core (see section 4.3
                                            // ""Interrupt Mode"" in the MMC [1]
                                            // specification). In order to have
                                            // the host response to be generated
                                            // in open drain mode the register
                                            // MMCHS_CON[OD] must be set to 1."
                                            // When MMCHS_CON[CEATA] is set to 1
                                            // and MMCHS_ARG set to 0x00000000
                                            // when writing 0x00000000 into
                                            // MMCHS_CMD register the host
                                            // controller performs a 'command
                                            // completion signal disable' token
                                            // i.e. CMD line held to '0' during
                                            // 47 cycles followed by a 1. 0 The
                                            // host does not generate a 48-bit
                                            // response instead of a command. 1
                                            // The host generates a 48-bit
                                            // response instead of a command or
                                            // a command completion signal
                                            // disable token.
#define MMCHS_CON_INIT          0x00000002  // Send initialization stream All
                                            // cards. When this bit is set to 1
                                            // and the card is idle an
                                            // initialization sequence is sent
                                            // to the card. "An initialization
                                            // sequence consists of setting the
                                            // CMD line to 1 during 80 clock
                                            // cycles. The initialisation
                                            // sequence is mandatory - but it is
                                            // not required to do it through
                                            // this bit - this bit makes it
                                            // easier. Clock divider
                                            // (MMCHS_SYSCTL[CLKD]) should be
                                            // set to ensure that 80 clock
                                            // periods are greater than 1ms.
                                            // (see section 9.3 ""Power-Up"" in
                                            // the MMC card specification [1] or
                                            // section 6.4 in the SD card
                                            // specification [2])." Note: in
                                            // this mode there is no command
                                            // sent to the card and no response
                                            // is expected 0 The host does not
                                            // send an initialization sequence.
                                            // 1 The host sends an
                                            // initialization sequence.
#define MMCHS_CON_OD            0x00000001  // Card open drain mode. Only for
                                            // MMC cards. This bit must be set
                                            // to 1 for MMC card commands 1 2 3
                                            // and 40 and if the MMC card bus is
                                            // operating in open-drain mode
                                            // during the response phase to the
                                            // command sent. Typically during
                                            // card identification mode when the
                                            // card is either in idle ready or
                                            // ident state. It is also necessary
                                            // to set this bit to 1 for a
                                            // broadcast host response (see
                                            // Broadcast host response register
                                            // MMCHS_CON[HR]) 0 No Open Drain 1
                                            // Open Drain or Broadcast host
                                            // response
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_PWCNT register.
//
//******************************************************************************
#define MMCHS_PWCNT_PWRCNT_M    0x0000FFFF  // Power counter register. This
                                            // register is used to introduce a
                                            // delay between the PAD ACTIVE pin
                                            // assertion and the command issued.
                                            // 0x0000 No additional delay added
                                            // 0x0001 TCF delay (card clock
                                            // period) 0x0002 TCF x 2 delay
                                            // (card clock period) 0xFFFE TCF x
                                            // 65534 delay (card clock period)
                                            // 0xFFFF TCF x 65535 delay (card
                                            // clock period)
#define MMCHS_PWCNT_PWRCNT_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_BLK register.
//
//******************************************************************************
#define MMCHS_BLK_NBLK_M        0xFFFF0000  // Blocks count for current
                                            // transfer This register is enabled
                                            // when Block count Enable
                                            // (MMCHS_CMD[BCE]) is set to 1 and
                                            // is valid only for multiple block
                                            // transfers. Setting the block
                                            // count to 0 results no data blocks
                                            // being transferred. Note: The host
                                            // controller decrements the block
                                            // count after each block transfer
                                            // and stops when the count reaches
                                            // zero. This register can be
                                            // accessed only if no transaction
                                            // is executing (i.e after a
                                            // transaction has stopped). Read
                                            // operations during transfers may
                                            // return an invalid value and write
                                            // operation will be ignored. In
                                            // suspend context the number of
                                            // blocks yet to be transferred can
                                            // be determined by reading this
                                            // register. When restoring transfer
                                            // context prior to issuing a Resume
                                            // command The local host shall
                                            // restore the previously saved
                                            // block count. 0x0000 Stop count
                                            // 0x0001 1 block 0x0002 2 blocks
                                            // 0xFFFF 65535 blocks
#define MMCHS_BLK_NBLK_S        16
#define MMCHS_BLK_BLEN_M        0x00000FFF  // Transfer Block Size. This
                                            // register specifies the block size
                                            // for block data transfers. Read
                                            // operations during transfers may
                                            // return an invalid value and write
                                            // operations are ignored. When a
                                            // CMD12 command is issued to stop
                                            // the transfer a read of the BLEN
                                            // field after transfer completion
                                            // (MMCHS_STAT[TC] set to 1) will
                                            // not return the true byte number
                                            // of data length while the stop
                                            // occurs but the value written in
                                            // this register before transfer is
                                            // launched. 0x000 No data transfer
                                            // 0x001 1 byte block length 0x002 2
                                            // bytes block length 0x003 3 bytes
                                            // block length 0x1FF 511 bytes
                                            // block length 0x200 512 bytes
                                            // block length 0x7FF 2047 bytes
                                            // block length 0x800 2048 bytes
                                            // block length
#define MMCHS_BLK_BLEN_S        0
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_ARG register.
//
//******************************************************************************
#define MMCHS_ARG_ARG_M         0xFFFFFFFF  // Command argument bits [31:0]
#define MMCHS_ARG_ARG_S         0
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_CMD register.
//
//******************************************************************************
#define MMCHS_CMD_INDX_M        0x3F000000  // Command index Binary encoded
                                            // value from 0 to 63 specifying the
                                            // command number send to card 0x00
                                            // CMD0 or ACMD0 0x01 CMD1 or ACMD1
                                            // 0x02 CMD2 or ACMD2 0x03 CMD3 or
                                            // ACMD3 0x04 CMD4 or ACMD4 0x05
                                            // CMD5 or ACMD5 0x06 CMD6 or ACMD6
                                            // 0x07 CMD7 or ACMD7 0x08 CMD8 or
                                            // ACMD8 0x09 CMD9 or ACMD9 0x0A
                                            // CMD10 or ACMD10 0x0B CMD11 or
                                            // ACMD11 0x0C CMD12 or ACMD12 0x0D
                                            // CMD13 or ACMD13 0x0E CMD14 or
                                            // ACMD14 0x0F CMD15 or ACMD15 0x10
                                            // CMD16 or ACMD16 0x11 CMD17 or
                                            // ACMD17 0x12 CMD18 or ACMD18 0x13
                                            // CMD19 or ACMD19 0x14 CMD20 or
                                            // ACMD20 0x15 CMD21 or ACMD21 0x16
                                            // CMD22 or ACMD22 0x17 CMD23 or
                                            // ACMD23 0x18 CMD24 or ACMD24 0x19
                                            // CMD25 or ACMD25 0x1A CMD26 or
                                            // ACMD26 0x1B CMD27 or ACMD27 0x1C
                                            // CMD28 or ACMD28 0x1D CMD29 or
                                            // ACMD29 0x1E CMD30 or ACMD30 0x1F
                                            // CMD31 or ACMD31 0x20 CMD32 or
                                            // ACMD32 0x21 CMD33 or ACMD33 0x22
                                            // CMD34 or ACMD34 0x23 CMD35 or
                                            // ACMD35 0x24 CMD36 or ACMD36 0x25
                                            // CMD37 or ACMD37 0x26 CMD38 or
                                            // ACMD38 0x27 CMD39 or ACMD39 0x28
                                            // CMD40 or ACMD40 0x29 CMD41 or
                                            // ACMD41 0x2A CMD42 or ACMD42 0x2B
                                            // CMD43 or ACMD43 0x2C CMD44 or
                                            // ACMD44 0x2D CMD45 or ACMD45 0x2E
                                            // CMD46 or ACMD46 0x2F CMD47 or
                                            // ACMD47 0x30 CMD48 or ACMD48 0x31
                                            // CMD49 or ACMD49 0x32 CMD50 or
                                            // ACMD50 0x33 CMD51 or ACMD51 0x34
                                            // CMD52 or ACMD52 0x35 CMD53 or
                                            // ACMD53 0x36 CMD54 or ACMD54 0x37
                                            // CMD55 or ACMD55 0x38 CMD56 or
                                            // ACMD56 0x39 CMD57 or ACMD57 0x3A
                                            // CMD58 or ACMD58 0x3B CMD59 or
                                            // ACMD59 0x3C CMD60 or ACMD60 0x3D
                                            // CMD61 or ACMD61 0x3E CMD62 or
                                            // ACMD62 0x3F CMD63 or ACMD63
#define MMCHS_CMD_INDX_S        24
#define MMCHS_CMD_CMD_TYPE_M    0x00C00000  // Command type This register
                                            // specifies three types of special
                                            // command: Suspend Resume and
                                            // Abort. These bits shall be set to
                                            // 00b for all other commands. 0x0
                                            // Others Commands 0x1 "CMD52 for
                                            // writing ""Bus Suspend"" in CCCR"
                                            // 0x2 "CMD52 for writing ""Function
                                            // Select"" in CCCR" 0x3 "Abort
                                            // command CMD12 CMD52 for writing
                                            // "" I/O Abort"" in CCCR"
#define MMCHS_CMD_CMD_TYPE_S    22
#define MMCHS_CMD_DP            0x00200000  // Data present select This
                                            // register indicates that data is
                                            // present and DAT line shall be
                                            // used. It must be set to 0 in the
                                            // following conditions: - command
                                            // using only CMD line - command
                                            // with no data transfer but using
                                            // busy signal on DAT[0] - Resume
                                            // command 0 Command with no data
                                            // transfer 1 Command with data
                                            // transfer
#define MMCHS_CMD_CICE          0x00100000  // Command Index check enable This
                                            // bit must be set to 1 to enable
                                            // index check on command response
                                            // to compare the index field in the
                                            // response against the index of the
                                            // command. If the index is not the
                                            // same in the response as in the
                                            // command it is reported as a
                                            // command index error
                                            // (MMCHS_STAT[CIE] set to1) Note:
                                            // The register CICE cannot be
                                            // configured for an Auto CMD12 then
                                            // index check is automatically
                                            // checked when this command is
                                            // issued. 0 Index check disable 1
                                            // Index check enable
#define MMCHS_CMD_CCCE          0x00080000  // Command CRC check enable This
                                            // bit must be set to 1 to enable
                                            // CRC7 check on command response to
                                            // protect the response against
                                            // transmission errors on the bus.
                                            // If an error is detected it is
                                            // reported as a command CRC error
                                            // (MMCHS_STAT[CCRC] set to 1).
                                            // Note: The register CCCE cannot be
                                            // configured for an Auto CMD12 and
                                            // then CRC check is automatically
                                            // checked when this command is
                                            // issued. 0 CRC7 check disable 1
                                            // CRC7 check enable
#define MMCHS_CMD_RSP_TYPE_M    0x00030000  // Response type This bits defines
                                            // the response type of the command
                                            // 0x0 No response 0x1 Response
                                            // Length 136 bits 0x2 Response
                                            // Length 48 bits 0x3 Response
                                            // Length 48 bits with busy after
                                            // response
#define MMCHS_CMD_RSP_TYPE_S    16
#define MMCHS_CMD_MSBS          0x00000020  // Multi/Single block select This
                                            // bit must be set to 1 for data
                                            // transfer in case of multi block
                                            // command. For any others command
                                            // this bit shall be set to 0. 0
                                            // Single block. If this bit is 0 it
                                            // is not necessary to set the
                                            // register MMCHS_BLK[NBLK]. 1 Multi
                                            // block. When Block Count is
                                            // disabled (MMCHS_CMD[BCE] is set
                                            // to 0) in Multiple block transfers
                                            // (MMCHS_CMD[MSBS] is set to 1) the
                                            // module can perform infinite
                                            // transfer.
#define MMCHS_CMD_DDIR          0x00000010  // Data transfer Direction Select
                                            // This bit defines either data
                                            // transfer will be a read or a
                                            // write. 0 Data Write (host to
                                            // card) 1 Data Read (card to host)
#define MMCHS_CMD_ACEN          0x00000004  // Auto CMD12 Enable SD card only.
                                            // When this bit is set to 1 the
                                            // host controller issues a CMD12
                                            // automatically after the transfer
                                            // completion of the last block. The
                                            // Host Driver shall not set this
                                            // bit to issue commands that do not
                                            // require CMD12 to stop data
                                            // transfer. In particular secure
                                            // commands do not require CMD12. 0
                                            // Auto CMD12 disable 1 Auto CMD12
                                            // enable or CCS detection enabled.
#define MMCHS_CMD_BCE           0x00000002  // Block Count Enable Multiple
                                            // block transfers only. This bit is
                                            // used to enable the block count
                                            // register (MMCHS_BLK[NBLK]). When
                                            // Block Count is disabled
                                            // (MMCHS_CMD[BCE] is set to 0) in
                                            // Multiple block transfers
                                            // (MMCHS_CMD[MSBS] is set to 1) the
                                            // module can perform infinite
                                            // transfer. 0 Block count disabled
                                            // for infinite transfer. 1 Block
                                            // count enabled for multiple block
                                            // transfer with known number of
                                            // blocks
#define MMCHS_CMD_DE            0x00000001  // DMA Enable This bit is used to
                                            // enable DMA mode for host data
                                            // access. 0 DMA mode disable 1 DMA
                                            // mode enable
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_RSP10 register.
//
//******************************************************************************
#define MMCHS_RSP10_RSP1_M      0xFFFF0000  // Command Response [31:16]
#define MMCHS_RSP10_RSP1_S      16
#define MMCHS_RSP10_RSP0_M      0x0000FFFF  // Command Response [15:0]
#define MMCHS_RSP10_RSP0_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_RSP32 register.
//
//******************************************************************************
#define MMCHS_RSP32_RSP3_M      0xFFFF0000  // Command Response [63:48]
#define MMCHS_RSP32_RSP3_S      16
#define MMCHS_RSP32_RSP2_M      0x0000FFFF  // Command Response [47:32]
#define MMCHS_RSP32_RSP2_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_RSP54 register.
//
//******************************************************************************
#define MMCHS_RSP54_RSP5_M      0xFFFF0000  // Command Response [95:80]
#define MMCHS_RSP54_RSP5_S      16
#define MMCHS_RSP54_RSP4_M      0x0000FFFF  // Command Response [79:64]
#define MMCHS_RSP54_RSP4_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_RSP76 register.
//
//******************************************************************************
#define MMCHS_RSP76_RSP7_M      0xFFFF0000  // Command Response [127:112]
#define MMCHS_RSP76_RSP7_S      16
#define MMCHS_RSP76_RSP6_M      0x0000FFFF  // Command Response [111:96]
#define MMCHS_RSP76_RSP6_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_DATA register.
//
//******************************************************************************
#define MMCHS_DATA_DATA_M       0xFFFFFFFF  // Data Register [31:0] In
                                            // functional mode (MMCHS_CON[MODE]
                                            // set to the default value 0) A
                                            // read access to this register is
                                            // allowed only when the buffer read
                                            // enable status is set to 1
                                            // (MMCHS_PSTATE[BRE]) otherwise a
                                            // bad access (MMCHS_STAT[BADA]) is
                                            // signaled. A write access to this
                                            // register is allowed only when the
                                            // buffer write enable status is set
                                            // to 1(MMCHS_STATE[BWE]) otherwise
                                            // a bad access (MMCHS_STAT[BADA])
                                            // is signaled and the data is not
                                            // written.
#define MMCHS_DATA_DATA_S       0
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_PSTATE register.
//
//******************************************************************************
#define MMCHS_PSTATE_CLEV       0x01000000
#define MMCHS_PSTATE_DLEV_M     0x00F00000  // DAT[3:0] line signal level
                                            // DAT[3] => bit 23 DAT[2] => bit 22
                                            // DAT[1] => bit 21 DAT[0] => bit 20
                                            // This status is used to check DAT
                                            // line level to recover from errors
                                            // and for debugging. This is
                                            // especially useful in detecting
                                            // the busy signal level from
                                            // DAT[0]. The value of these
                                            // registers after reset depends on
                                            // the DAT lines level at that time.
#define MMCHS_PSTATE_DLEV_S     20
#define MMCHS_PSTATE_WP         0x00080000
#define MMCHS_PSTATE_CDPL       0x00040000
#define MMCHS_PSTATE_CSS        0x00020000
#define MMCHS_PSTATE_CINS       0x00010000
#define MMCHS_PSTATE_BRE        0x00000800
#define MMCHS_PSTATE_BWE        0x00000400
#define MMCHS_PSTATE_RTA        0x00000200
#define MMCHS_PSTATE_WTA        0x00000100
#define MMCHS_PSTATE_DLA        0x00000004
#define MMCHS_PSTATE_DATI       0x00000002
#define MMCHS_PSTATE_CMDI       0x00000001
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_HCTL register.
//
//******************************************************************************
#define MMCHS_HCTL_OBWE         0x08000000  // Wakeup event enable for
                                            // 'Out-of-Band' Interrupt. This bit
                                            // enables wakeup events for
                                            // 'Out-of-Band' assertion. Wakeup
                                            // is generated if the wakeup
                                            // feature is enabled
                                            // (MMCHS_SYSCONFIG[ENAWAKEUP]). The
                                            // write to this register is ignored
                                            // when MMCHS_CON[OBIE] is not set.
                                            // 0 Disable wakeup on 'Out-of-Band'
                                            // Interrupt 1 Enable wakeup on
                                            // 'Out-of-Band' Interrupt
#define MMCHS_HCTL_REM          0x04000000  // Wakeup event enable on SD card
                                            // removal This bit enables wakeup
                                            // events for card removal
                                            // assertion. Wakeup is generated if
                                            // the wakeup feature is enabled
                                            // (MMCHS_SYSCONFIG[ENAWAKEUP]). 0
                                            // Disable wakeup on card removal 1
                                            // Enable wakeup on card removal
#define MMCHS_HCTL_INS          0x02000000  // Wakeup event enable on SD card
                                            // insertion This bit enables wakeup
                                            // events for card insertion
                                            // assertion. Wakeup is generated if
                                            // the wakeup feature is enabled
                                            // (MMCHS_SYSCONFIG[ENAWAKEUP]). 0
                                            // Disable wakeup on card insertion
                                            // 1 Enable wakeup on card insertion
#define MMCHS_HCTL_IWE          0x01000000  // Wakeup event enable on SD card
                                            // interrupt This bit enables wakeup
                                            // events for card interrupt
                                            // assertion. Wakeup is generated if
                                            // the wakeup feature is enabled
                                            // (MMCHS_SYSCONFIG[ENAWAKEUP]). 0
                                            // Disable wakeup on card interrupt
                                            // 1 Enable wakeup on card interrupt
#define MMCHS_HCTL_IBG          0x00080000  // Interrupt block at gap This bit
                                            // is valid only in 4-bit mode of
                                            // SDIO card to enable interrupt
                                            // detection in the interrupt cycle
                                            // at block gap for a multiple block
                                            // transfer. For MMC cards and for
                                            // SD card this bit should be set to
                                            // 0. 0 Disable interrupt detection
                                            // at the block gap in 4-bit mode 1
                                            // Enable interrupt detection at the
                                            // block gap in 4-bit mode
#define MMCHS_HCTL_RWC          0x00040000  // Read wait control The read wait
                                            // function is optional only for
                                            // SDIO cards. If the card supports
                                            // read wait this bit must be
                                            // enabled then requesting a stop at
                                            // block gap (MMCHS_HCTL[SBGR])
                                            // generates a read wait period
                                            // after the current end of block.
                                            // Be careful if read wait is not
                                            // supported it may cause a conflict
                                            // on DAT line. 0 Disable Read Wait
                                            // Control. Suspend/Resume cannot be
                                            // supported. 1 Enable Read Wait
                                            // Control
#define MMCHS_HCTL_CR           0x00020000  // Continue request This bit is
                                            // used to restart a transaction
                                            // that was stopped by requesting a
                                            // stop at block gap
                                            // (MMCHS_HCTL[SBGR]). Set this bit
                                            // to 1 restarts the transfer. The
                                            // bit is automatically set to 0 by
                                            // the host controller when transfer
                                            // has restarted i.e DAT line is
                                            // active (MMCHS_PSTATE[DLA]) or
                                            // transferring data
                                            // (MMCHS_PSTATE[WTA]). The Stop at
                                            // block gap request must be
                                            // disabled (MMCHS_HCTL[SBGR]=0)
                                            // before setting this bit. 0 No
                                            // affect 1 transfer restart
#define MMCHS_HCTL_SBGR         0x00010000  // Stop at block gap request This
                                            // bit is used to stop executing a
                                            // transaction at the next block
                                            // gap. The transfer can restart
                                            // with a continue request
                                            // (MMHS_HCTL[CR]) or during a
                                            // suspend/resume sequence. In case
                                            // of read transfer the card must
                                            // support read wait control. In
                                            // case of write transfer the host
                                            // driver shall set this bit after
                                            // all block data written. Until the
                                            // transfer completion
                                            // (MMCHS_STAT[TC] set to 1) the
                                            // host driver shall leave this bit
                                            // set to 1. If this bit is set the
                                            // local host shall not write to the
                                            // data register (MMCHS_DATA). 0
                                            // Transfer mode 1 Stop at block gap
#define MMCHS_HCTL_SDVS_M       0x00000E00  // SD bus voltage select All cards.
                                            // The host driver should set to
                                            // these bits to select the voltage
                                            // level for the card according to
                                            // the voltage supported by the
                                            // system (MMCHS_CAPA[VS18VS30VS33])
                                            // before starting a transfer. 0x5
                                            // 1.8V (Typical) 0x6 3.0V (Typical)
                                            // 0x7 3.3V (Typical)
#define MMCHS_HCTL_SDVS_S       9
#define MMCHS_HCTL_SDBP         0x00000100  // SD bus power Before setting this
                                            // bit the host driver shall select
                                            // the SD bus voltage
                                            // (MMCHS_HCTL[SDVS]). If the host
                                            // controller detects the No card
                                            // state this bit is automatically
                                            // set to 0. If the module is power
                                            // off a write in the command
                                            // register (MMCHS_CMD) will not
                                            // start the transfer. A write to
                                            // this bit has no effect if the
                                            // selected SD bus voltage
                                            // MMCHS_HCTL[SDVS] is not supported
                                            // according to capability register
                                            // (MMCHS_CAPA[VS*]). 0 Power off 1
                                            // Power on
#define MMCHS_HCTL_CDSS         0x00000080  // Card Detect Signal Selection
                                            // This bit selects source for the
                                            // card detection.When the source
                                            // for the card detection is
                                            // switched the interrupt should be
                                            // disabled during the switching
                                            // period by clearing the Interrupt
                                            // Status/Signal Enable register in
                                            // order to mask unexpected
                                            // interrupt being caused by the
                                            // glitch. The Interrupt
                                            // Status/Signal Enable should be
                                            // disabled during over the period
                                            // of debouncing. 0 SDCD# is
                                            // selected (for normal use) 1 The
                                            // Card Detect Test Level is
                                            // selected (for test purpose)
#define MMCHS_HCTL_CDTL         0x00000040  // Card Detect Test Level: This bit
                                            // is enabled while the Card Detect
                                            // Signal Selection is set to 1 and
                                            // it indicates card inserted or
                                            // not. 0 No Card 1 Card Inserted
#define MMCHS_HCTL_DMAS_M       0x00000018  // DMA Select Mode: One of
                                            // supported DMA modes can be
                                            // selected. The host driver shall
                                            // check support of DMA modes by
                                            // referring the Capabilities
                                            // register. Use of selected DMA is
                                            // determined by DMA Enable of the
                                            // Transfer Mode register. This
                                            // register is only meaningful when
                                            // MADMA_EN is set to 1. When
                                            // MADMA_EN is set to 0 the bit
                                            // field is read only and returned
                                            // value is 0. 0x0 Reserved 0x1
                                            // Reserved 0x2 32-bit Address ADMA2
                                            // is selected 0x3 Reserved
#define MMCHS_HCTL_DMAS_S       3
#define MMCHS_HCTL_HSPE         0x00000004  // High Speed Enable: Before
                                            // setting this bit the Host Driver
                                            // shall check the High Speed
                                            // Support in the Capabilities
                                            // register. If this bit is set to 0
                                            // (default) the Host Controller
                                            // outputs CMD line and DAT lines at
                                            // the falling edge of the SD Clock.
                                            // If this bit is set to 1 the Host
                                            // Controller outputs CMD line and
                                            // DAT lines at the rising edge of
                                            // the SD Clock.This bit shall not
                                            // be set when dual data rate mode
                                            // is activated in MMCHS_CON[DDR]. 0
                                            // Normal speed mode 1 High speed
                                            // mode
#define MMCHS_HCTL_DTW          0x00000002  // Data transfer width For MMC card
                                            // this bit must be set following a
                                            // valid SWITCH command (CMD6) with
                                            // the correct value and extend CSD
                                            // index written in the argument.
                                            // Prior to this command the MMC
                                            // card configuration register (CSD
                                            // and EXT_CSD) must be verified for
                                            // compliance with MMC standard
                                            // specification 4.x (see section
                                            // 3.6). This register has no effect
                                            // when the MMC 8-bit mode is
                                            // selected (register MMCHS_CON[DW8]
                                            // set to1 ) For SD/SDIO cards this
                                            // bit must be set following a valid
                                            // SET_BUS_WIDTH command (ACMD6)
                                            // with the value written in bit 1
                                            // of the argument. Prior to this
                                            // command the SD card configuration
                                            // register (SCR) must be verified
                                            // for the supported bus width by
                                            // the SD card. 0 1-bit Data width
                                            // (DAT[0] used) 1 4-bit Data width
                                            // (DAT[3:0] used)
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_SYSCTL register.
//
//******************************************************************************
#define MMCHS_SYSCTL_SRD        0x04000000  // Software reset for DAT line This
                                            // bit is set to 1 for reset and
                                            // released to 0 when completed. DAT
                                            // finite state machine in both
                                            // clock domain are also reset. Here
                                            // below are the registers cleared
                                            // by MMCHS_SYSCTL[SRD]: #VALUE! -
                                            // MMCHS_PSTATE: BRE BWE RTA WTA DLA
                                            // and DATI - MMCHS_HCTL: SBGR and
                                            // CR - MMCHS_STAT: BRR BWR BGE and
                                            // TC OCP and MMC buffer data
                                            // management is reinitialized. 0
                                            // Reset completed 1 Software reset
                                            // for DAT line
#define MMCHS_SYSCTL_SRC        0x02000000  // Software reset for CMD line This
                                            // bit is set to 1 for reset and
                                            // released to 0 when completed. CMD
                                            // finite state machine in both
                                            // clock domain are also reset. Here
                                            // below the registers cleared by
                                            // MMCHS_SYSCTL[SRC]: -
                                            // MMCHS_PSTATE: CMDI - MMCHS_STAT:
                                            // CC OCP and MMC command status
                                            // management is reinitialized. 0
                                            // Reset completed 1 Software reset
                                            // for CMD line
#define MMCHS_SYSCTL_SRA        0x01000000  // Software reset for all This bit
                                            // is set to 1 for reset and
                                            // released to 0 when completed.
                                            // This reset affects the entire
                                            // host controller except for the
                                            // card detection circuit and
                                            // capabilities registers. 0 Reset
                                            // completed 1 Software reset for
                                            // all the design
#define MMCHS_SYSCTL_DTO_M      0x000F0000  // Data timeout counter value and
                                            // busy timeout. This value
                                            // determines the interval by which
                                            // DAT lines timeouts are detected.
                                            // The host driver needs to set this
                                            // bitfield based on - the maximum
                                            // read access time (NAC) (Refer to
                                            // the SD Specification Part1
                                            // Physical Layer) - the data read
                                            // access time values (TAAC and
                                            // NSAC) in the card specific data
                                            // register (CSD) of the card - the
                                            // timeout clock base frequency
                                            // (MMCHS_CAPA[TCF]). If the card
                                            // does not respond within the
                                            // specified number of cycles a data
                                            // timeout error occurs
                                            // (MMCHS_STA[DTO]). The
                                            // MMCHS_SYSCTL[DTO] register is
                                            // also used to check busy duration
                                            // to generate busy timeout for
                                            // commands with busy response or
                                            // for busy programming during a
                                            // write command. Timeout on CRC
                                            // status is generated if no CRC
                                            // token is present after a block
                                            // write. 0x0 TCF x 2^13 0x1 TCF x
                                            // 2^14 0xE TCF x 2^27 0xF Reserved
#define MMCHS_SYSCTL_DTO_S      16
#define MMCHS_SYSCTL_CLKD_M     0x0000FFC0  // Clock frequency select These
                                            // bits define the ratio between a
                                            // reference clock frequency (system
                                            // dependant) and the output clock
                                            // frequency on the CLK pin of
                                            // either the memory card (MMC SD or
                                            // SDIO). 0x000 Clock Ref bypass
                                            // 0x001 Clock Ref bypass 0x002
                                            // Clock Ref / 2 0x003 Clock Ref / 3
                                            // 0x3FF Clock Ref / 1023
#define MMCHS_SYSCTL_CLKD_S     6
#define MMCHS_SYSCTL_CEN        0x00000004  // Clock enable This bit controls
                                            // if the clock is provided to the
                                            // card or not. 0 The clock is not
                                            // provided to the card . Clock
                                            // frequency can be changed . 1 The
                                            // clock is provided to the card and
                                            // can be automatically gated when
                                            // MMCHS_SYSCONFIG[AUTOIDLE] is set
                                            // to 1 (default value) . The host
                                            // driver shall wait to set this bit
                                            // to 1 until the Internal clock is
                                            // stable (MMCHS_SYSCTL[ICS]).
#define MMCHS_SYSCTL_ICS        0x00000002
#define MMCHS_SYSCTL_ICE        0x00000001  // Internal clock enable This
                                            // register controls the internal
                                            // clock activity. In very low power
                                            // state the internal clock is
                                            // stopped. Note: The activity of
                                            // the debounce clock (used for
                                            // wakeup events) and the OCP clock
                                            // (used for reads and writes to the
                                            // module register map) are not
                                            // affected by this register. 0 The
                                            // internal clock is stopped (very
                                            // low power state). 1 The internal
                                            // clock oscillates and can be
                                            // automatically gated when
                                            // MMCHS_SYSCONFIG[AUTOIDLE] is set
                                            // to 1 (default value) .
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_STAT register.
//
//******************************************************************************
#define MMCHS_STAT_BADA         0x20000000
#define MMCHS_STAT_CERR         0x10000000
#define MMCHS_STAT_ADMAE        0x02000000
#define MMCHS_STAT_ACE          0x01000000
#define MMCHS_STAT_DEB          0x00400000
#define MMCHS_STAT_DCRC         0x00200000
#define MMCHS_STAT_DTO          0x00100000
#define MMCHS_STAT_CIE          0x00080000
#define MMCHS_STAT_CEB          0x00040000
#define MMCHS_STAT_CCRC         0x00020000
#define MMCHS_STAT_CTO          0x00010000
#define MMCHS_STAT_ERRI         0x00008000
#define MMCHS_STAT_BSR          0x00000400
#define MMCHS_STAT_OBI          0x00000200
#define MMCHS_STAT_CIRQ         0x00000100
#define MMCHS_STAT_CREM         0x00000080
#define MMCHS_STAT_CINS         0x00000040
#define MMCHS_STAT_BRR          0x00000020
#define MMCHS_STAT_BWR          0x00000010
#define MMCHS_STAT_DMA          0x00000008
#define MMCHS_STAT_BGE          0x00000004
#define MMCHS_STAT_TC           0x00000002
#define MMCHS_STAT_CC           0x00000001
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_IE register.
//
//******************************************************************************
#define MMCHS_IE_BADA_ENABLE    0x20000000  // Bad access to data space
                                            // Interrupt Enable 0 Masked 1
                                            // Enabled
#define MMCHS_IE_CERR_ENABLE    0x10000000  // Card error interrupt Enable 0
                                            // Masked 1 Enabled
#define MMCHS_IE_ADMAE_ENABLE   0x02000000  // ADMA error Interrupt Enable 0
                                            // Masked 1 Enabled
#define MMCHS_IE_ACE_ENABLE     0x01000000  // Auto CMD12 error Interrupt
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_IE_DEB_ENABLE     0x00400000  // Data end bit error Interrupt
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_IE_DCRC_ENABLE    0x00200000  // Data CRC error Interrupt Enable
                                            // 0 Masked 1 Enabled
#define MMCHS_IE_DTO_ENABLE     0x00100000  // Data timeout error Interrupt
                                            // Enable 0 The data timeout
                                            // detection is deactivated. The
                                            // host controller provides the
                                            // clock to the card until the card
                                            // sends the data or the transfer is
                                            // aborted. 1 The data timeout
                                            // detection is enabled.
#define MMCHS_IE_CIE_ENABLE     0x00080000  // Command index error Interrupt
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_IE_CEB_ENABLE     0x00040000  // Command end bit error Interrupt
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_IE_CCRC_ENABLE    0x00020000  // Command CRC error Interrupt
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_IE_CTO_ENABLE     0x00010000  // Command timeout error Interrupt
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_IE_NULL           0x00008000  // Fixed to 0 The host driver shall
                                            // control error interrupts using
                                            // the Error Interrupt Signal Enable
                                            // register. Writes to this bit are
                                            // ignored
#define MMCHS_IE_BSR_ENABLE     0x00000400  // Boot status interrupt Enable A
                                            // write to this register when
                                            // MMCHS_CON[BOOT_ACK] is set to 0x0
                                            // is ignored. 0 Masked 1 Enabled
#define MMCHS_IE_OBI_ENABLE     0x00000200  // Out-of-Band interrupt Enable A
                                            // write to this register when
                                            // MMCHS_CON[OBIE] is set to '0' is
                                            // ignored. 0 Masked 1 Enabled
#define MMCHS_IE_CIRQ_ENABLE    0x00000100  // Card interrupt Enable A clear of
                                            // this bit also clears the
                                            // corresponding status bit. During
                                            // 1-bit mode if the interrupt
                                            // routine doesn't remove the source
                                            // of a card interrupt in the SDIO
                                            // card the status bit is reasserted
                                            // when this bit is set to 1. 0
                                            // Masked 1 Enabled
#define MMCHS_IE_CREM_ENABLE    0x00000080  // Card removal Interrupt Enable 0
                                            // Masked 1 Enabled
#define MMCHS_IE_CINS_ENABLE    0x00000040  // Card insertion Interrupt Enable
                                            // 0 Masked 1 Enabled
#define MMCHS_IE_BRR_ENABLE     0x00000020  // Buffer Read Ready Interrupt
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_IE_BWR_ENABLE     0x00000010  // Buffer Write Ready Interrupt
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_IE_DMA_ENABLE     0x00000008  // DMA interrupt Enable 0 Masked 1
                                            // Enabled
#define MMCHS_IE_BGE_ENABLE     0x00000004  // Block Gap Event Interrupt Enable
                                            // 0 Masked 1 Enabled
#define MMCHS_IE_TC_ENABLE      0x00000002  // Transfer completed Interrupt
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_IE_CC_ENABLE      0x00000001  // Command completed Interrupt
                                            // Enable 0 Masked 1 Enabled
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_ISE register.
//
//******************************************************************************
#define MMCHS_ISE_BADA_SIGEN    0x20000000  // Bad access to data space signal
                                            // status Enable 0 Masked 1 Enabled
#define MMCHS_ISE_CERR_SIGEN    0x10000000  // Card error interrupt signal
                                            // status Enable 0 Masked 1 Enabled
#define MMCHS_ISE_ADMAE_SIGEN   0x02000000  // ADMA error signal status Enable
                                            // 0 Masked 1 Enabled
#define MMCHS_ISE_ACE_SIGEN     0x01000000  // Auto CMD12 error signal status
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_ISE_DEB_SIGEN     0x00400000  // Data end bit error signal status
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_ISE_DCRC_SIGEN    0x00200000  // Data CRC error signal status
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_ISE_DTO_SIGEN     0x00100000  // Data timeout error signal status
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_ISE_CIE_SIGEN     0x00080000  // Command index error signal
                                            // status Enable 0 Masked 1 Enabled
#define MMCHS_ISE_CEB_SIGEN     0x00040000  // Command end bit error signal
                                            // status Enable 0 Masked 1 Enabled
#define MMCHS_ISE_CCRC_SIGEN    0x00020000  // Command CRC error signal status
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_ISE_CTO_SIGEN     0x00010000  // Command timeout error signal
                                            // status Enable 0 Masked 1 Enabled
#define MMCHS_ISE_NULL          0x00008000  // Fixed to 0 The host driver shall
                                            // control error interrupts using
                                            // the Error Interrupt Signal Enable
                                            // register. Writes to this bit are
                                            // ignored
#define MMCHS_ISE_BSR_SIGEN     0x00000400  // Boot status signal status
                                            // EnableA write to this register
                                            // when MMCHS_CON[BOOT_ACK] is set
                                            // to 0x0 is ignored. 0 Masked 1
                                            // Enabled
#define MMCHS_ISE_OBI_SIGEN     0x00000200  // Out-Of-Band Interrupt signal
                                            // status Enable A write to this
                                            // register when MMCHS_CON[OBIE] is
                                            // set to '0' is ignored. 0 Masked 1
                                            // Enabled
#define MMCHS_ISE_CIRQ_SIGEN    0x00000100  // Card interrupt signal status
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_ISE_CREM_SIGEN    0x00000080  // Card removal signal status
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_ISE_CINS_SIGEN    0x00000040  // Card insertion signal status
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_ISE_BRR_SIGEN     0x00000020  // Buffer Read Ready signal status
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_ISE_BWR_SIGEN     0x00000010  // Buffer Write Ready signal status
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_ISE_DMA_SIGEN     0x00000008  // DMA interrupt Signal status
                                            // enable 0 Masked 1 Enabled
#define MMCHS_ISE_BGE_SIGEN     0x00000004  // Black Gap Event signal status
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_ISE_TC_SIGEN      0x00000002  // Transfer completed signal status
                                            // Enable 0 Masked 1 Enabled
#define MMCHS_ISE_CC_SIGEN      0x00000001  // Command completed signal status
                                            // Enable 0 Masked 1 Enabled
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_AC12 register.
//
//******************************************************************************
#define MMCHS_AC12_CNI          0x00000080
#define MMCHS_AC12_ACIE         0x00000010
#define MMCHS_AC12_ACEB         0x00000008
#define MMCHS_AC12_ACCE         0x00000004
#define MMCHS_AC12_ACTO         0x00000002
#define MMCHS_AC12_ACNE         0x00000001
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_CAPA register.
//
//******************************************************************************
#define MMCHS_CAPA_BIT64        0x10000000
#define MMCHS_CAPA_VS18         0x04000000
#define MMCHS_CAPA_VS30         0x02000000
#define MMCHS_CAPA_VS33         0x01000000
#define MMCHS_CAPA_SRS          0x00800000
#define MMCHS_CAPA_DS           0x00400000
#define MMCHS_CAPA_HSS          0x00200000
#define MMCHS_CAPA_AD2S         0x00080000
#define MMCHS_CAPA_MBL_M        0x00030000
#define MMCHS_CAPA_MBL_S        16
#define MMCHS_CAPA_BCF_M        0x00003F00
#define MMCHS_CAPA_BCF_S        8
#define MMCHS_CAPA_TCU          0x00000080
#define MMCHS_CAPA_TCF_M        0x0000003F
#define MMCHS_CAPA_TCF_S        0
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_CUR_CAPA register.
//
//******************************************************************************
#define MMCHS_CUR_CAPA_CUR_1V8_M \
                                0x00FF0000

#define MMCHS_CUR_CAPA_CUR_1V8_S 16
#define MMCHS_CUR_CAPA_CUR_3V0_M \
                                0x0000FF00

#define MMCHS_CUR_CAPA_CUR_3V0_S 8
#define MMCHS_CUR_CAPA_CUR_3V3_M \
                                0x000000FF

#define MMCHS_CUR_CAPA_CUR_3V3_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_FE register.
//
//******************************************************************************
#define MMCHS_FE_FE_BADA        0x20000000
#define MMCHS_FE_FE_CERR        0x10000000
#define MMCHS_FE_FE_ADMAE       0x02000000
#define MMCHS_FE_FE_ACE         0x01000000
#define MMCHS_FE_FE_DEB         0x00400000
#define MMCHS_FE_FE_DCRC        0x00200000
#define MMCHS_FE_FE_DTO         0x00100000
#define MMCHS_FE_FE_CIE         0x00080000
#define MMCHS_FE_FE_CEB         0x00040000
#define MMCHS_FE_FE_CCRC        0x00020000
#define MMCHS_FE_FE_CTO         0x00010000
#define MMCHS_FE_FE_CNI         0x00000080
#define MMCHS_FE_FE_ACIE        0x00000010
#define MMCHS_FE_FE_ACEB        0x00000008
#define MMCHS_FE_FE_ACCE        0x00000004
#define MMCHS_FE_FE_ACTO        0x00000002
#define MMCHS_FE_FE_ACNE        0x00000001
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_ADMAES register.
//
//******************************************************************************
#define MMCHS_ADMAES_LME        0x00000004  // ADMA Length Mismatch Error: (1)
                                            // While Block Count Enable being
                                            // set the total data length
                                            // specified by the Descriptor table
                                            // is different from that specified
                                            // by the Block Count and Block
                                            // Length. (2) Total data length can
                                            // not be divided by the block
                                            // length. 0 No Error 1 Error
#define MMCHS_ADMAES_AES_M      0x00000003  // ADMA Error State his field
                                            // indicates the state of ADMA when
                                            // error is occurred during ADMA
                                            // data transfer. "This field never
                                            // indicates ""10"" because ADMA
                                            // never stops in this state." 0x0
                                            // ST_STOP (Stop DMA)Contents of
                                            // SYS_SDR register 0x1 ST_STOP
                                            // (Stop DMA)Points the error
                                            // descriptor 0x2 Never set this
                                            // state(Not used) 0x3 ST_TFR
                                            // (Transfer Data)Points the next of
                                            // the error descriptor
#define MMCHS_ADMAES_AES_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_ADMASAL register.
//
//******************************************************************************
#define MMCHS_ADMASAL_ADMA_A32B_M \
                                0xFFFFFFFF  // ADMA System address 32 bits.This
                                            // register holds byte address of
                                            // executing command of the
                                            // Descriptor table. 32-bit Address
                                            // Descriptor uses lower 32-bit of
                                            // this register. At the start of
                                            // ADMA the Host Driver shall set
                                            // start address of the Descriptor
                                            // table. The ADMA increments this
                                            // register address which points to
                                            // next line when every fetching a
                                            // Descriptor line. When the ADMA
                                            // Error Interrupt is generated this
                                            // register shall hold valid
                                            // Descriptor address depending on
                                            // the ADMA state. The Host Driver
                                            // shall program Descriptor Table on
                                            // 32-bit boundary and set 32-bit
                                            // boundary address to this
                                            // register. ADMA2 ignores lower
                                            // 2-bit of this register and
                                            // assumes it to be 00b.

#define MMCHS_ADMASAL_ADMA_A32B_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MMCHS_O_REV register.
//
//******************************************************************************
#define MMCHS_REV_VREV_M        0xFF000000  // Vendor Version Number: IP
                                            // revision [7:4] Major revision
                                            // [3:0] Minor revision Examples:
                                            // 0x10 for 1.0 0x21 for 2.1
#define MMCHS_REV_VREV_S        24
#define MMCHS_REV_SREV_M        0x00FF0000
#define MMCHS_REV_SREV_S        16
#define MMCHS_REV_SIS           0x00000001  // Slot Interrupt Status This
                                            // status bit indicates the inverted
                                            // state of interrupt signal for the
                                            // module. By a power on reset or by
                                            // setting a software reset for all
                                            // (MMCHS_HCTL[SRA]) the interrupt
                                            // signal shall be de-asserted and
                                            // this status shall read 0.



#endif // __HW_MMCHS_H__

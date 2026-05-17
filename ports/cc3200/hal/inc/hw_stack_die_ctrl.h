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


#ifndef __HW_STACK_DIE_CTRL_H__
#define __HW_STACK_DIE_CTRL_H__

//*****************************************************************************
//
// The following are defines for the STACK_DIE_CTRL register offsets.
//
//*****************************************************************************
#define STACK_DIE_CTRL_O_STK_UP_RESET \
                                0x00000000  // Can be written only by Base
                                            // Processor. Writing to this
                                            // register will reset the stack
                                            // processor reset will be
                                            // de-asserted upon clearing this
                                            // register.

#define STACK_DIE_CTRL_O_SR_MASTER_PRIORITY \
                                0x00000004  // This register defines who among
                                            // base processor and stack
                                            // processor have highest priority
                                            // for Sram Access. Can be written
                                            // only by Base Processor.

#define STACK_DIE_CTRL_O_STK_SR_ACC_CTL_BK2 \
                                0x00000008  // In Spinlock mode this Register
                                            // defines who among base processor
                                            // and stack processor have access
                                            // to Sram Bank2 right now. In
                                            // Handshake mode this Register
                                            // defines who among base processor
                                            // and stack processor have access
                                            // to Sram Bank2 and Bank3 right
                                            // now. Its Clear only register and
                                            // is set by hardware. Lower bit can
                                            // be cleared only by Base Processor
                                            // and Upper bit Cleared only by the
                                            // Stack processor.

#define STACK_DIE_CTRL_O_BASE_UP_ACC_REQ_BK2 \
                                0x0000000C  // In Spinlock mode whenever Base
                                            // processor wants the access to
                                            // Sram Bank2 it should request for
                                            // it by writing into this register.
                                            // It'll get interrupt whenever it
                                            // is granted. In Handshake mode
                                            // this bit will be set by Stack
                                            // processor. Its a set only bit and
                                            // is cleared by HW when the request
                                            // is granted.

#define STACK_DIE_CTRL_O_STK_UP_ACC_REQ_BK2 \
                                0x00000010  // In Spinlock mode Whenever Stack
                                            // processor wants the access to
                                            // Sram Bank2 it should request for
                                            // it by writing into this register.
                                            // It'll get interrupt whenever it
                                            // is granted. In Handshake mode
                                            // this bit will be set by the Base
                                            // processor. Its a set only bit and
                                            // is cleared by HW when the request
                                            // is granted.

#define STACK_DIE_CTRL_O_STK_SR_ACC_CTL_BK3 \
                                0x00000014  // Register defines who among base
                                            // processor and stack processor
                                            // have access to Sram Bank3 right
                                            // now. Its Clear only register and
                                            // is set by hardware. Lower bit can
                                            // be cleared only by Base Processor
                                            // and Upper bit Cleared only by the
                                            // Stack processor.

#define STACK_DIE_CTRL_O_BASE_UP_ACC_REQ_BK3 \
                                0x00000018  // In Spinlock mode whenever Base
                                            // processor wants the access to
                                            // Sram Bank3 it should request for
                                            // it by writing into this register.
                                            // It'll get interrupt whenever it
                                            // is granted. In Handshake mode
                                            // this bit will be set by Stack
                                            // processor. Its a set only bit and
                                            // is cleared by HW when the request
                                            // is granted.

#define STACK_DIE_CTRL_O_STK_UP_ACC_REQ_BK3 \
                                0x0000001C  // In Spinlock mode Whenever Stack
                                            // processor wants the access to
                                            // Sram Bank3 it should request for
                                            // it by writing into this register.
                                            // It'll get interrupt whenever it
                                            // is granted. In Handshake mode
                                            // this bit will be set by the Base
                                            // processor. Its a set only bit and
                                            // is cleared by HW when the request
                                            // is granted.

#define STACK_DIE_CTRL_O_RDSM_CFG_CPU \
                                0x00000020  // Read State Machine timing
                                            // configuration register. Generally
                                            // Bit 4 and 3 will be identical.
                                            // For stacked die always 43 are 0
                                            // and 6:5 == 1 for 120Mhz.

#define STACK_DIE_CTRL_O_RDSM_CFG_EE \
                                0x00000024  // Read State Machine timing
                                            // configuration register. Generally
                                            // Bit 4 and 3 will be identical.
                                            // For stacked die always 43 are 0
                                            // and 6:5 == 1 for 120Mhz.

#define STACK_DIE_CTRL_O_BASE_UP_IRQ_LOG \
                                0x00000028  // Reading this register Base
                                            // procesor will able to know the
                                            // reason for the interrupt. This is
                                            // clear only register - set by HW
                                            // upon an interrupt to Base
                                            // processor and can be cleared only
                                            // by BASE processor.

#define STACK_DIE_CTRL_O_STK_UP_IRQ_LOG \
                                0x0000002C  // Reading this register Stack
                                            // procesor will able to know the
                                            // reason for the interrupt. This is
                                            // clear only register - set by HW
                                            // upon an interrupt to Stack
                                            // processor and can be cleared only
                                            // by Stack processor.

#define STACK_DIE_CTRL_O_STK_CLK_EN \
                                0x00000030  // Can be written only by base
                                            // processor. Controls the enable
                                            // pin of the cgcs for the clocks
                                            // going to CM3 dft ctrl block and
                                            // Sram.

#define STACK_DIE_CTRL_O_SPIN_LOCK_MODE \
                                0x00000034  // Can be written only by the base
                                            // processor. Decides the ram
                                            // sharing mode :: handshake or
                                            // Spinlock mode.

#define STACK_DIE_CTRL_O_BUS_FAULT_ADDR \
                                0x00000038  // Stores the last bus fault
                                            // address.

#define STACK_DIE_CTRL_O_BUS_FAULT_CLR \
                                0x0000003C  // write only registers on read
                                            // returns 0.W Write 1 to clear the
                                            // bust fault to store the new bus
                                            // fault address

#define STACK_DIE_CTRL_O_RESET_CAUSE \
                                0x00000040  // Reset cause value captured from
                                            // the ICR_CLKRST block.

#define STACK_DIE_CTRL_O_WDOG_TIMER_EVENT \
                                0x00000044  // Watchdog timer event value
                                            // captured from the ICR_CLKRST
                                            // block

#define STACK_DIE_CTRL_O_DMA_REQ \
                                0x00000048  // To send Dma Request to bottom
                                            // die.

#define STACK_DIE_CTRL_O_SRAM_JUMP_OFFSET_ADDR \
                                0x0000004C  // Address offset within SRAM to
                                            // which CM3 should jump after
                                            // reset.

#define STACK_DIE_CTRL_O_SW_REG1 \
                                0x00000050  // These are sw registers for
                                            // topdie processor and bottom die
                                            // processor to communicate. Both
                                            // can set and read these registers.
                                            // In case of write clash bottom
                                            // die's processor wins and top die
                                            // processor access is ignored.

#define STACK_DIE_CTRL_O_SW_REG2 \
                                0x00000054  // These are sw registers for
                                            // topdie processor and bottom die
                                            // processor to communicate. Both
                                            // can set and read these registers.
                                            // In case of write clash bottom
                                            // die's processor wins and top die
                                            // processor access is ignored.

#define STACK_DIE_CTRL_O_FMC_SLEEP_CTL \
                                0x00000058  // By posting the request Flash can
                                            // be put into low-power mode
                                            // (Sleep) without powering down the
                                            // Flash. Earlier (in Garnet) this
                                            // was fully h/w controlled and the
                                            // control for this was coming from
                                            // SysCtl while entering into Cortex
                                            // Deep-sleep mode. But for our
                                            // device the D2D i/f doesnt support
                                            // this. The Firmware has to program
                                            // the register in the top-die for
                                            // entering into this mode and wait
                                            // for an interrupt.

#define STACK_DIE_CTRL_O_MISC_CTL \
                                0x0000005C  // Miscellanious control register.

#define STACK_DIE_CTRL_O_SW_DFT_CTL \
                                0x000000FC  // DFT control and status bits

#define STACK_DIE_CTRL_O_PADN_CTL_0 \
                                0x00000100  // Mainly for For controlling the
                                            // pads OEN pins. There are total 60
                                            // pads and hence 60 control registe
                                            // i.e n value varies from 0 to 59.
                                            // Here is the mapping for the
                                            // pad_ctl register number and the
                                            // functionality : 0 D2DPAD_DMAREQ1
                                            // 1 D2DPAD_DMAREQ0 2
                                            // D2DPAD_INT2BASE 3 D2DPAD_PIOSC 4
                                            // D2DPAD_RST_N 5 D2DPAD_POR_RST_N 6
                                            // D2DPAD_HCLK 7 D2DPAD_JTAG_TDO 8
                                            // D2DPAD_JTAG_TCK 9 D2DPAD_JTAG_TMS
                                            // 10 D2DPAD_JTAG_TDI 11-27
                                            // D2DPAD_FROMSTACK[D2D_FROMSTACK_SIZE
                                            // -1:0] 28-56 D2DPAD_TOSTACK
                                            // [D2D_TOSTACK_SIZE -1:0] 57-59
                                            // D2DPAD_SPARE [D2D_SPARE_PAD_SIZE
                                            // -1:0] 0:00




//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_STK_UP_RESET register.
//
//******************************************************************************
#define STACK_DIE_CTRL_STK_UP_RESET_UP_RESET \
                                0x00000001  // 1 :Assert Reset 0 : Deassert the
                                            // Reset

//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_SR_MASTER_PRIORITY register.
//
//******************************************************************************
#define STACK_DIE_CTRL_SR_MASTER_PRIORITY_PRIORITY_M \
                                0x00000003  // 00 : Equal Priority 01 : Stack
                                            // Processor have priority 10 : Base
                                            // Processor have priority 11 :
                                            // Unused

#define STACK_DIE_CTRL_SR_MASTER_PRIORITY_PRIORITY_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_STK_SR_ACC_CTL_BK2 register.
//
//******************************************************************************
#define STACK_DIE_CTRL_STK_SR_ACC_CTL_BK2_STK_UP_ACCSS \
                                0x00000002  // Stack Processor should clear it
                                            // when it is done with the sram
                                            // bank usage. Set by HW It is set
                                            // when Stack Processor is granted
                                            // the access to this bank

#define STACK_DIE_CTRL_STK_SR_ACC_CTL_BK2_BASE_UP_ACCSS \
                                0x00000001  // Base Processor should clear it
                                            // when it is done wth the sram
                                            // usage. Set by HW It is set when
                                            // Base Processor is granted the
                                            // access to this bank

//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_BASE_UP_ACC_REQ_BK2 register.
//
//******************************************************************************
#define STACK_DIE_CTRL_BASE_UP_ACC_REQ_BK2_ACCSS_REQ \
                                0x00000001  // Base Processor will set when
                                            // Sram access is needed in Spin
                                            // Lock mode. In Handshake mode
                                            // Stack Processor will set to
                                            // inform Base Processor that it is
                                            // done with the processing of data
                                            // in SRAM and is now ready to use
                                            // by the base processor.

//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_STK_UP_ACC_REQ_BK2 register.
//
//******************************************************************************
#define STACK_DIE_CTRL_STK_UP_ACC_REQ_BK2_ACCSS_REQ \
                                0x00000001  // Stack Processor will set when
                                            // Sram access is needed in Spin
                                            // Lock mode. In Handshake mode Base
                                            // Processor will set to inform
                                            // Stack Processor to start
                                            // processing the data in the Ram.

//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_STK_SR_ACC_CTL_BK3 register.
//
//******************************************************************************
#define STACK_DIE_CTRL_STK_SR_ACC_CTL_BK3_STK_UP_ACCSS \
                                0x00000002  // Stack Processor should clear it
                                            // when it is done with the sram
                                            // bank usage. Set by HW It is set
                                            // when Stack Processor is granted
                                            // the access to this bank.

#define STACK_DIE_CTRL_STK_SR_ACC_CTL_BK3_BASE_UP_ACCSS \
                                0x00000001  // Base Processor should clear it
                                            // when it is done wth the sram
                                            // usage. Set by HW it is set when
                                            // Base Processor is granted the
                                            // access to this bank.

//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_BASE_UP_ACC_REQ_BK3 register.
//
//******************************************************************************
#define STACK_DIE_CTRL_BASE_UP_ACC_REQ_BK3_ACCSS_REQ \
                                0x00000001  // Base Processor will set when
                                            // Sram access is needed in Spin
                                            // Lock mode. Not used in handshake
                                            // mode.

//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_STK_UP_ACC_REQ_BK3 register.
//
//******************************************************************************
#define STACK_DIE_CTRL_STK_UP_ACC_REQ_BK3_ACCSS_REQ \
                                0x00000001  // Stack Processor will set when
                                            // Sram access is needed in Spin
                                            // Lock mode.

//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_RDSM_CFG_CPU register.
//
//******************************************************************************
#define STACK_DIE_CTRL_RDSM_CFG_CPU_FLCLK_PULSE_WIDTH_M \
                                0x000000C0  // Bank Clock Hi Time 00 : HCLK
                                            // pulse 01 : 1 cycle of HCLK 10 :
                                            // 1.5 cycles of HCLK 11 : 2 cycles
                                            // of HCLK

#define STACK_DIE_CTRL_RDSM_CFG_CPU_FLCLK_PULSE_WIDTH_S 6
#define STACK_DIE_CTRL_RDSM_CFG_CPU_FLCLK_SENSE \
                                0x00000020  // FLCLK 0 : indicates flash clock
                                            // rise aligns on HCLK rise 1 :
                                            // indicates flash clock rise aligns
                                            // on HCLK fall

#define STACK_DIE_CTRL_RDSM_CFG_CPU_PIPELINE_FLDATA \
                                0x00000010  // 0 : Always register flash rdata
                                            // before sending to CPU 1 : Drive
                                            // Flash rdata directly out on MISS
                                            // (Both ICODE / DCODE)

#define STACK_DIE_CTRL_RDSM_CFG_CPU_READ_WAIT_STATE_M \
                                0x0000000F  // Number of wait states inserted

#define STACK_DIE_CTRL_RDSM_CFG_CPU_READ_WAIT_STATE_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_RDSM_CFG_EE register.
//
//******************************************************************************
#define STACK_DIE_CTRL_RDSM_CFG_EE_FLCLK_PULSE_WIDTH_M \
                                0x000000C0  // Bank Clock Hi Time 00 : HCLK
                                            // pulse 01 : 1 cycle of HCLK 10 :
                                            // 1.5 cycles of HCLK 11 : 2 cycles
                                            // of HCLK

#define STACK_DIE_CTRL_RDSM_CFG_EE_FLCLK_PULSE_WIDTH_S 6
#define STACK_DIE_CTRL_RDSM_CFG_EE_FLCLK_SENSE \
                                0x00000020  // FLCLK 0 : indicates flash clock
                                            // rise aligns on HCLK rise 1 :
                                            // indicates flash clock rise aligns
                                            // on HCLK fall

#define STACK_DIE_CTRL_RDSM_CFG_EE_PIPELINE_FLDATA \
                                0x00000010  // 0 : Always register flash rdata
                                            // before sending to CPU 1 : Drive
                                            // Flash rdata directly out on MISS
                                            // (Both ICODE / DCODE)

#define STACK_DIE_CTRL_RDSM_CFG_EE_READ_WAIT_STATE_M \
                                0x0000000F  // Number of wait states inserted

#define STACK_DIE_CTRL_RDSM_CFG_EE_READ_WAIT_STATE_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_BASE_UP_IRQ_LOG register.
//
//******************************************************************************
#define STACK_DIE_CTRL_BASE_UP_IRQ_LOG_SR_BK3_REL \
                                0x00000010  // Set when Relinquish Interrupt
                                            // sent to Base processor for Bank3.

#define STACK_DIE_CTRL_BASE_UP_IRQ_LOG_SR_BK2_RELEASE \
                                0x00000008  // Set when Relinquish Interrupt
                                            // sent to Base processor for Bank2.

#define STACK_DIE_CTRL_BASE_UP_IRQ_LOG_SR_BK3_GRANT \
                                0x00000004  // Set when Bank3 is granted to
                                            // Base processor.

#define STACK_DIE_CTRL_BASE_UP_IRQ_LOG_SR_BK2_GRANT \
                                0x00000002  // Set when Bank2 is granted to
                                            // BAse processor.

#define STACK_DIE_CTRL_BASE_UP_IRQ_LOG_SR_INVAL_ACCSS \
                                0x00000001  // Set when there Base processor do
                                            // an Invalid access to Sram. Ex :
                                            // Accessing the bank which is not
                                            // granted for BAse processor.

//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_STK_UP_IRQ_LOG register.
//
//******************************************************************************
#define STACK_DIE_CTRL_STK_UP_IRQ_LOG_SR_BK3_REL \
                                0x00000008  // Set when Relinquish Interrupt
                                            // sent to Stack processor for
                                            // Bank3.

#define STACK_DIE_CTRL_STK_UP_IRQ_LOG_SR_BK2_REL \
                                0x00000004  // Set when Relinquish Interrupt
                                            // sent to Stack processor for
                                            // Bank2.

#define STACK_DIE_CTRL_STK_UP_IRQ_LOG_SR_BK3_GRANT \
                                0x00000002  // Set when Bank3 is granted to
                                            // Stack processor.

#define STACK_DIE_CTRL_STK_UP_IRQ_LOG_SR_BK2_GRANT \
                                0x00000001  // Set when Bank2 is granted to
                                            // Stack processor.

//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_STK_CLK_EN register.
//
//******************************************************************************
#define STACK_DIE_CTRL_STK_CLK_EN_SR_CLK \
                                0x00000004  // Enable the clock going to sram.

#define STACK_DIE_CTRL_STK_CLK_EN_DFT_CTRL_CLK \
                                0x00000002  // Enable the clock going to dft
                                            // control block

#define STACK_DIE_CTRL_STK_CLK_EN_STK_UP_CLK \
                                0x00000001  // Enable the clock going to Cm3

//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_SPIN_LOCK_MODE register.
//
//******************************************************************************
#define STACK_DIE_CTRL_SPIN_LOCK_MODE_MODE \
                                0x00000001  // 0 : Handshake Mode 1 : Spinlock
                                            // mode.

//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_BUS_FAULT_ADDR register.
//
//******************************************************************************
#define STACK_DIE_CTRL_BUS_FAULT_ADDR_ADDRESS_M \
                                0xFFFFFFFF  // Fault Address

#define STACK_DIE_CTRL_BUS_FAULT_ADDR_ADDRESS_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_BUS_FAULT_CLR register.
//
//******************************************************************************
#define STACK_DIE_CTRL_BUS_FAULT_CLR_CLEAR \
                                0x00000001  // When set it'll clear the bust
                                            // fault address register to store
                                            // the new bus fault address

//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_RESET_CAUSE register.
//
//******************************************************************************
#define STACK_DIE_CTRL_RESET_CAUSE_RST_CAUSE_M \
                                0xFFFFFFFF

#define STACK_DIE_CTRL_RESET_CAUSE_RST_CAUSE_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_WDOG_TIMER_EVENT register.
//
//******************************************************************************
#define STACK_DIE_CTRL_WDOG_TIMER_EVENT_WDOG_TMR_EVNT_M \
                                0xFFFFFFFF

#define STACK_DIE_CTRL_WDOG_TIMER_EVENT_WDOG_TMR_EVNT_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_DMA_REQ register.
//
//******************************************************************************
#define STACK_DIE_CTRL_DMA_REQ_DMAREQ1 \
                                0x00000002  // Generate DMAREQ1 on setting this
                                            // bit.

#define STACK_DIE_CTRL_DMA_REQ_DMAREQ0 \
                                0x00000001  // Generate DMAREQ0 on setting this
                                            // bit.

//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_SRAM_JUMP_OFFSET_ADDR register.
//
//******************************************************************************
#define STACK_DIE_CTRL_SRAM_JUMP_OFFSET_ADDR_ADDR_M \
                                0xFFFFFFFF

#define STACK_DIE_CTRL_SRAM_JUMP_OFFSET_ADDR_ADDR_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_SW_REG1 register.
//
//******************************************************************************
#define STACK_DIE_CTRL_SW_REG1_NEWBITFIELD1_M \
                                0xFFFFFFFF

#define STACK_DIE_CTRL_SW_REG1_NEWBITFIELD1_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_SW_REG2 register.
//
//******************************************************************************
#define STACK_DIE_CTRL_SW_REG2_NEWBITFIELD1_M \
                                0xFFFFFFFF

#define STACK_DIE_CTRL_SW_REG2_NEWBITFIELD1_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_FMC_SLEEP_CTL register.
//
//******************************************************************************
#define STACK_DIE_CTRL_FMC_SLEEP_CTL_FMC_LPM_ACK \
                                0x00000002  // captures the status of of
                                            // fmc_lpm_ack

#define STACK_DIE_CTRL_FMC_SLEEP_CTL_FMC_LPM_REQ \
                                0x00000001  // When set assert
                                            // iflpe2fmc_lpm_req to FMC.

//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_MISC_CTL register.
//
//******************************************************************************
#define STACK_DIE_CTRL_MISC_CTL_WDOG_RESET \
                                0x00000080  // 1 : will reset the async wdog
                                            // timer runing on piosc clock

#define STACK_DIE_CTRL_MISC_CTL_FW_IRQ2 \
                                0x00000020  // Setting this Will send to
                                            // interttupt to CM3

#define STACK_DIE_CTRL_MISC_CTL_FW_IRQ1 \
                                0x00000010  // Setting this Will send to
                                            // interttupt to CM3

#define STACK_DIE_CTRL_MISC_CTL_FW_IRQ0 \
                                0x00000008  // Setting this Will send to
                                            // interttupt to CM3

#define STACK_DIE_CTRL_MISC_CTL_FLB_TEST_MUX_CTL_BK3 \
                                0x00000004  // While testing Flash Setting this
                                            // bit will Control the
                                            // CE/STR/AIN/CLKIN going to flash
                                            // banks 12 and 3. 0 : Control
                                            // signals coming from FMC for Bank
                                            // 3 goes to Bank3 1 : Control
                                            // signals coming from FMC for Bank
                                            // 0 goes to Bank2

#define STACK_DIE_CTRL_MISC_CTL_FLB_TEST_MUX_CTL_BK2 \
                                0x00000002  // While testing Flash Setting this
                                            // bit will Control the
                                            // CE/STR/AIN/CLKIN going to flash
                                            // banks 12 and 3. 0 : Control
                                            // signals coming from FMC for Bank
                                            // 2 goes to Bank2 1 : Control
                                            // signals coming from FMC for Bank
                                            // 0 goes to Bank2

#define STACK_DIE_CTRL_MISC_CTL_FLB_TEST_MUX_CTL_BK1 \
                                0x00000001  // While testing Flash Setting this
                                            // bit will Control the
                                            // CE/STR/AIN/CLKIN going to flash
                                            // banks 12 and 3. 0 : Control
                                            // signals coming from FMC for Bank
                                            // 1 goes to Bank1 1 : Control
                                            // signals coming from FMC for Bank
                                            // 0 goes to Bank1

//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_SW_DFT_CTL register.
//
//******************************************************************************
#define STACK_DIE_CTRL_SW_DFT_CTL_FL_CTRL_OWNS \
                                0x20000000  // when set to '1' all flash
                                            // control signals switch over to
                                            // CM3 control when '0' it is under
                                            // the D2D interface control

#define STACK_DIE_CTRL_SW_DFT_CTL_SWIF_CPU_READ \
                                0x10000000  // 1 indicates in SWIF mode the
                                            // control signals to flash are from
                                            // FMC CPU read controls the clock
                                            // and address. that is one can give
                                            // address via FMC and read through
                                            // IDMEM.

#define STACK_DIE_CTRL_SW_DFT_CTL_CPU_DONE \
                                0x00800000  // 'CPU Done' bit for PBIST. Write
                                            // '1' to indicate test done.

#define STACK_DIE_CTRL_SW_DFT_CTL_CPU_FAIL \
                                0x00400000  // 'CPU Fail' bit for PBIST. Write
                                            // '1' to indicate test failed.

#define STACK_DIE_CTRL_SW_DFT_CTL_FLBK4_OWNS \
                                0x00001000  // when set to '1' flash bank 4
                                            // (EEPROM) is owned by the CM3for
                                            // reads over DCODE bus. When '0'
                                            // access control given to D2D
                                            // interface.

#define STACK_DIE_CTRL_SW_DFT_CTL_FLBK3_OWNS \
                                0x00000800  // when set to '1' flash bank 3 is
                                            // owned by the CM3for reads over
                                            // DCODE bus. When '0' access
                                            // control given to D2D interface.

#define STACK_DIE_CTRL_SW_DFT_CTL_FLBK2_OWNS \
                                0x00000400  // when set to '1' flash bank 2 is
                                            // owned by the CM3for reads over
                                            // DCODE bus. When '0' access
                                            // control given to D2D interface.

#define STACK_DIE_CTRL_SW_DFT_CTL_FLBK1_OWNS \
                                0x00000200  // when set to '1' flash bank 1 is
                                            // owned by the CM3for reads over
                                            // DCODE bus. When '0' access
                                            // control given to D2D interface.

#define STACK_DIE_CTRL_SW_DFT_CTL_FLBK0_OWNS \
                                0x00000100  // when set to '1' flash bank 0 is
                                            // owned by the CM3 for reads over
                                            // DCODE bus. When '0' access
                                            // control given to D2D interface.

//******************************************************************************
//
// The following are defines for the bit fields in the
// STACK_DIE_CTRL_O_PADN_CTL_0 register.
//
//******************************************************************************
#define STACK_DIE_CTRL_PADN_CTL_0_SPARE_PAD_DOUT \
                                0x00000008  // This bit is valid for only the
                                            // spare pads ie for n=57 to 59.
                                            // value to drive at the output of
                                            // the pad

#define STACK_DIE_CTRL_PADN_CTL_0_SPARE_PAD_DIN \
                                0x00000004  // This bit is valid for only the
                                            // spare pads ie for n=57 to 59.
                                            // captures the 'Y' pin of the pad
                                            // which is the data being driven
                                            // into the die

#define STACK_DIE_CTRL_PADN_CTL_0_OEN2X \
                                0x00000002  // OEN2X control when '1' enables
                                            // the output with 1x. Total drive
                                            // strength is decided bu oen1x
                                            // setting + oen2x setting.

#define STACK_DIE_CTRL_PADN_CTL_0_OEN1X \
                                0x00000001  // OEN1X control when '1' enables
                                            // the output with 1x . Total drive
                                            // strength is decided bu oen1x
                                            // setting + oen2x setting.




#endif // __HW_STACK_DIE_CTRL_H__

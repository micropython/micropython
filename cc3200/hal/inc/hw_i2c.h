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

#ifndef __HW_I2C_H__
#define __HW_I2C_H__

//*****************************************************************************
//
// The following are defines for the I2C register offsets.
//
//*****************************************************************************
#define I2C_O_MSA               0x00000000
#define I2C_O_MCS               0x00000004
#define I2C_O_MDR               0x00000008
#define I2C_O_MTPR              0x0000000C
#define I2C_O_MIMR              0x00000010
#define I2C_O_MRIS              0x00000014
#define I2C_O_MMIS              0x00000018
#define I2C_O_MICR              0x0000001C
#define I2C_O_MCR               0x00000020
#define I2C_O_MCLKOCNT          0x00000024
#define I2C_O_MBMON             0x0000002C
#define I2C_O_MBLEN             0x00000030
#define I2C_O_MBCNT             0x00000034
#define I2C_O_SOAR              0x00000800
#define I2C_O_SCSR              0x00000804
#define I2C_O_SDR               0x00000808
#define I2C_O_SIMR              0x0000080C
#define I2C_O_SRIS              0x00000810
#define I2C_O_SMIS              0x00000814
#define I2C_O_SICR              0x00000818
#define I2C_O_SOAR2             0x0000081C
#define I2C_O_SACKCTL           0x00000820
#define I2C_O_FIFODATA          0x00000F00
#define I2C_O_FIFOCTL           0x00000F04
#define I2C_O_FIFOSTATUS        0x00000F08
#define I2C_O_OBSMUXSEL0        0x00000F80
#define I2C_O_OBSMUXSEL1        0x00000F84
#define I2C_O_MUXROUTE          0x00000F88
#define I2C_O_PV                0x00000FB0
#define I2C_O_PP                0x00000FC0
#define I2C_O_PC                0x00000FC4
#define I2C_O_CC                0x00000FC8



//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MSA register.
//
//******************************************************************************
#define I2C_MSA_SA_M          0x000000FE  // I2C Slave Address
#define I2C_MSA_SA_S          1
#define I2C_MSA_RS              0x00000001  // Receive not send
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MCS register.
//
//******************************************************************************
#define I2C_MCS_ACTDMARX        0x80000000  // DMA RX Active Status
#define I2C_MCS_ACTDMATX        0x40000000  // DMA TX Active Status
#define I2C_MCS_CLKTO           0x00000080  // Clock Timeout Error
#define I2C_MCS_BUSBSY          0x00000040  // Bus Busy
#define I2C_MCS_IDLE            0x00000020  // I2C Idle
#define I2C_MCS_ARBLST          0x00000010  // Arbitration Lost
#define I2C_MCS_ACK             0x00000008  // Data Acknowledge Enable
#define I2C_MCS_ADRACK          0x00000004  // Acknowledge Address
#define I2C_MCS_ERROR           0x00000002  // Error
#define I2C_MCS_BUSY            0x00000001  // I2C Busy
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MDR register.
//
//******************************************************************************
#define I2C_MDR_DATA_M        0x000000FF  // Data Transferred
#define I2C_MDR_DATA_S        0
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MTPR register.
//
//******************************************************************************
#define I2C_MTPR_HS             0x00000080  // High-Speed Enable
#define I2C_MTPR_TPR_M        0x0000007F  // SCL Clock Period
#define I2C_MTPR_TPR_S        0
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MIMR register.
//
//******************************************************************************
#define I2C_MIMR_RXFFIM         0x00000800  // Receive FIFO Full Interrupt Mask
#define I2C_MIMR_TXFEIM         0x00000400  // Transmit FIFO Empty Interrupt
                                            // Mask
#define I2C_MIMR_RXIM           0x00000200  // Receive FIFO Request Interrupt
                                            // Mask
#define I2C_MIMR_TXIM           0x00000100  // Transmit FIFO Request Interrupt
                                            // Mask
#define I2C_MIMR_ARBLOSTIM      0x00000080  // Arbitration Lost Interrupt Mask
#define I2C_MIMR_STOPIM         0x00000040  // STOP Detection Interrupt Mask
#define I2C_MIMR_STARTIM        0x00000020  // START Detection Interrupt Mask
#define I2C_MIMR_NACKIM         0x00000010  // Address/Data NACK Interrupt Mask
#define I2C_MIMR_DMATXIM        0x00000008  // Transmit DMA Interrupt Mask
#define I2C_MIMR_DMARXIM        0x00000004  // Receive DMA Interrupt Mask
#define I2C_MIMR_CLKIM          0x00000002  // Clock Timeout Interrupt Mask
#define I2C_MIMR_IM             0x00000001  // Master Interrupt Mask
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MRIS register.
//
//******************************************************************************
#define I2C_MRIS_RXFFRIS        0x00000800  // Receive FIFO Full Raw Interrupt
                                            // Status
#define I2C_MRIS_TXFERIS        0x00000400  // Transmit FIFO Empty Raw
                                            // Interrupt Status
#define I2C_MRIS_RXRIS          0x00000200  // Receive FIFO Request Raw
                                            // Interrupt Status
#define I2C_MRIS_TXRIS          0x00000100  // Transmit Request Raw Interrupt
                                            // Status
#define I2C_MRIS_ARBLOSTRIS     0x00000080  // Arbitration Lost Raw Interrupt
                                            // Status
#define I2C_MRIS_STOPRIS        0x00000040  // STOP Detection Raw Interrupt
                                            // Status
#define I2C_MRIS_STARTRIS       0x00000020  // START Detection Raw Interrupt
                                            // Status
#define I2C_MRIS_NACKRIS        0x00000010  // Address/Data NACK Raw Interrupt
                                            // Status
#define I2C_MRIS_DMATXRIS       0x00000008  // Transmit DMA Raw Interrupt
                                            // Status
#define I2C_MRIS_DMARXRIS       0x00000004  // Receive DMA Raw Interrupt Status
#define I2C_MRIS_CLKRIS         0x00000002  // Clock Timeout Raw Interrupt
                                            // Status
#define I2C_MRIS_RIS            0x00000001  // Master Raw Interrupt Status
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MMIS register.
//
//******************************************************************************
#define I2C_MMIS_RXFFMIS        0x00000800  // Receive FIFO Full Interrupt Mask
#define I2C_MMIS_TXFEMIS        0x00000400  // Transmit FIFO Empty Interrupt
                                            // Mask
#define I2C_MMIS_RXMIS          0x00000200  // Receive FIFO Request Interrupt
                                            // Mask
#define I2C_MMIS_TXMIS          0x00000100  // Transmit Request Interrupt Mask
#define I2C_MMIS_ARBLOSTMIS     0x00000080  // Arbitration Lost Interrupt Mask
#define I2C_MMIS_STOPMIS        0x00000040  // STOP Detection Interrupt Mask
#define I2C_MMIS_STARTMIS       0x00000020  // START Detection Interrupt Mask
#define I2C_MMIS_NACKMIS        0x00000010  // Address/Data NACK Interrupt Mask
#define I2C_MMIS_DMATXMIS       0x00000008  // Transmit DMA Interrupt Status
#define I2C_MMIS_DMARXMIS       0x00000004  // Receive DMA Interrupt Status
#define I2C_MMIS_CLKMIS         0x00000002  // Clock Timeout Masked Interrupt
                                            // Status
#define I2C_MMIS_MIS            0x00000001  // Masked Interrupt Status
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MICR register.
//
//******************************************************************************
#define I2C_MICR_RXFFIC         0x00000800  // Receive FIFO Full Interrupt
                                            // Clear
#define I2C_MICR_TXFEIC         0x00000400  // Transmit FIFO Empty Interrupt
                                            // Clear
#define I2C_MICR_RXIC           0x00000200  // Receive FIFO Request Interrupt
                                            // Clear
#define I2C_MICR_TXIC           0x00000100  // Transmit FIFO Request Interrupt
                                            // Clear
#define I2C_MICR_ARBLOSTIC      0x00000080  // Arbitration Lost Interrupt Clear
#define I2C_MICR_STOPIC         0x00000040  // STOP Detection Interrupt Clear
#define I2C_MICR_STARTIC        0x00000020  // START Detection Interrupt Clear
#define I2C_MICR_NACKIC         0x00000010  // Address/Data NACK Interrupt
                                            // Clear
#define I2C_MICR_DMATXIC        0x00000008  // Transmit DMA Interrupt Clear
#define I2C_MICR_DMARXIC        0x00000004  // Receive DMA Interrupt Clear
#define I2C_MICR_CLKIC          0x00000002  // Clock Timeout Interrupt Clear
#define I2C_MICR_IC             0x00000001  // Master Interrupt Clear
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MCR register.
//
//******************************************************************************
#define I2C_MCR_MMD             0x00000040  // Multi-master Disable
#define I2C_MCR_SFE             0x00000020  // I2C Slave Function Enable
#define I2C_MCR_MFE             0x00000010  // I2C Master Function Enable
#define I2C_MCR_LPBK            0x00000001  // I2C Loopback
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MCLKOCNT register.
//
//******************************************************************************
#define I2C_MCLKOCNT_CNTL_M   0x000000FF  // I2C Master Count
#define I2C_MCLKOCNT_CNTL_S   0
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MBMON register.
//
//******************************************************************************
#define I2C_MBMON_SDA           0x00000002  // I2C SDA Status
#define I2C_MBMON_SCL           0x00000001  // I2C SCL Status
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MBLEN register.
//
//******************************************************************************
#define I2C_MBLEN_CNTL_M      0x000000FF  // I2C Burst Length
#define I2C_MBLEN_CNTL_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MBCNT register.
//
//******************************************************************************
#define I2C_MBCNT_CNTL_M      0x000000FF  // I2C Master Burst Count
#define I2C_MBCNT_CNTL_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SOAR register.
//
//******************************************************************************
#define I2C_SOAR_OAR_M        0x0000007F  // I2C Slave Own Address
#define I2C_SOAR_OAR_S        0
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SCSR register.
//
//******************************************************************************
#define I2C_SCSR_ACTDMARX       0x80000000  // DMA RX Active Status
#define I2C_SCSR_ACTDMATX       0x40000000  // DMA TX Active Status
#define I2C_SCSR_QCMDRW         0x00000020  // Quick Command Read / Write
#define I2C_SCSR_QCMDST         0x00000010  // Quick Command Status
#define I2C_SCSR_OAR2SEL        0x00000008  // OAR2 Address Matched
#define I2C_SCSR_FBR            0x00000004  // First Byte Received
#define I2C_SCSR_TREQ           0x00000002  // Transmit Request
#define I2C_SCSR_DA             0x00000001  // Device Active
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SDR register.
//
//******************************************************************************
#define I2C_SDR_DATA_M        0x000000FF  // Data for Transfer
#define I2C_SDR_DATA_S        0
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SIMR register.
//
//******************************************************************************
#define I2C_SIMR_IM             0x00000100  // Interrupt Mask
#define I2C_SIMR_TXFEIM         0x00000080  // Transmit FIFO Empty Interrupt
                                            // Mask
#define I2C_SIMR_RXIM           0x00000040  // Receive FIFO Request Interrupt
                                            // Mask
#define I2C_SIMR_TXIM           0x00000020  // Transmit FIFO Request Interrupt
                                            // Mask
#define I2C_SIMR_DMATXIM        0x00000010  // Transmit DMA Interrupt Mask
#define I2C_SIMR_DMARXIM        0x00000008  // Receive DMA Interrupt Mask
#define I2C_SIMR_STOPIM         0x00000004  // Stop Condition Interrupt Mask
#define I2C_SIMR_STARTIM        0x00000002  // Start Condition Interrupt Mask
#define I2C_SIMR_DATAIM         0x00000001  // Data Interrupt Mask
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SRIS register.
//
//******************************************************************************
#define I2C_SRIS_RIS            0x00000100  // Raw Interrupt Status
#define I2C_SRIS_TXFERIS        0x00000080  // Transmit FIFO Empty Raw
                                            // Interrupt Status
#define I2C_SRIS_RXRIS          0x00000040  // Receive FIFO Request Raw
                                            // Interrupt Status
#define I2C_SRIS_TXRIS          0x00000020  // Transmit Request Raw Interrupt
                                            // Status
#define I2C_SRIS_DMATXRIS       0x00000010  // Transmit DMA Raw Interrupt
                                            // Status
#define I2C_SRIS_DMARXRIS       0x00000008  // Receive DMA Raw Interrupt Status
#define I2C_SRIS_STOPRIS        0x00000004  // Stop Condition Raw Interrupt
                                            // Status
#define I2C_SRIS_STARTRIS       0x00000002  // Start Condition Raw Interrupt
                                            // Status
#define I2C_SRIS_DATARIS        0x00000001  // Data Raw Interrupt Status
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SMIS register.
//
//******************************************************************************
#define I2C_SMIS_RXFFMIS        0x00000100  // Receive FIFO Full Interrupt Mask
#define I2C_SMIS_TXFEMIS        0x00000080  // Transmit FIFO Empty Interrupt
                                            // Mask
#define I2C_SMIS_RXMIS          0x00000040  // Receive FIFO Request Interrupt
                                            // Mask
#define I2C_SMIS_TXMIS          0x00000020  // Transmit FIFO Request Interrupt
                                            // Mask
#define I2C_SMIS_DMATXMIS       0x00000010  // Transmit DMA Masked Interrupt
                                            // Status
#define I2C_SMIS_DMARXMIS       0x00000008  // Receive DMA Masked Interrupt
                                            // Status
#define I2C_SMIS_STOPMIS        0x00000004  // Stop Condition Masked Interrupt
                                            // Status
#define I2C_SMIS_STARTMIS       0x00000002  // Start Condition Masked Interrupt
                                            // Status
#define I2C_SMIS_DATAMIS        0x00000001  // Data Masked Interrupt Status
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SICR register.
//
//******************************************************************************
#define I2C_SICR_RXFFIC         0x00000100  // Receive FIFO Full Interrupt Mask
#define I2C_SICR_TXFEIC         0x00000080  // Transmit FIFO Empty Interrupt
                                            // Mask
#define I2C_SICR_RXIC           0x00000040  // Receive Request Interrupt Mask
#define I2C_SICR_TXIC           0x00000020  // Transmit Request Interrupt Mask
#define I2C_SICR_DMATXIC        0x00000010  // Transmit DMA Interrupt Clear
#define I2C_SICR_DMARXIC        0x00000008  // Receive DMA Interrupt Clear
#define I2C_SICR_STOPIC         0x00000004  // Stop Condition Interrupt Clear
#define I2C_SICR_STARTIC        0x00000002  // Start Condition Interrupt Clear
#define I2C_SICR_DATAIC         0x00000001  // Data Interrupt Clear
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SOAR2 register.
//
//******************************************************************************
#define I2C_SOAR2_OAR2EN        0x00000080  // I2C Slave Own Address 2 Enable
#define I2C_SOAR2_OAR2_M      0x0000007F  // I2C Slave Own Address 2
#define I2C_SOAR2_OAR2_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SACKCTL register.
//
//******************************************************************************
#define I2C_SACKCTL_ACKOVAL     0x00000002  // I2C Slave ACK Override Value
#define I2C_SACKCTL_ACKOEN      0x00000001  // I2C Slave ACK Override Enable
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_FIFODATA register.
//
//******************************************************************************
#define I2C_FIFODATA_DATA_M   0x000000FF  // I2C FIFO Data Byte
#define I2C_FIFODATA_DATA_S   0
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_FIFOCTL register.
//
//******************************************************************************
#define I2C_FIFOCTL_RXASGNMT    0x80000000  // RX Control Assignment
#define I2C_FIFOCTL_RXFLUSH     0x40000000  // RX FIFO Flush
#define I2C_FIFOCTL_DMARXENA    0x20000000  // DMA RX Channel Enable
#define I2C_FIFOCTL_RXTRIG_M  0x00070000  // RX FIFO Trigger
#define I2C_FIFOCTL_RXTRIG_S  16
#define I2C_FIFOCTL_TXASGNMT    0x00008000  // TX Control Assignment
#define I2C_FIFOCTL_TXFLUSH     0x00004000  // TX FIFO Flush
#define I2C_FIFOCTL_DMATXENA    0x00002000  // DMA TX Channel Enable
#define I2C_FIFOCTL_TXTRIG_M  0x00000007  // TX FIFO Trigger
#define I2C_FIFOCTL_TXTRIG_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_FIFOSTATUS register.
//
//******************************************************************************
#define I2C_FIFOSTATUS_RXABVTRIG \
                                0x00040000  // RX FIFO Above Trigger Level

#define I2C_FIFOSTATUS_RXFF     0x00020000  // RX FIFO Full
#define I2C_FIFOSTATUS_RXFE     0x00010000  // RX FIFO Empty
#define I2C_FIFOSTATUS_TXBLWTRIG \
                                0x00000004  // TX FIFO Below Trigger Level

#define I2C_FIFOSTATUS_TXFF     0x00000002  // TX FIFO Full
#define I2C_FIFOSTATUS_TXFE     0x00000001  // TX FIFO Empty
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_OBSMUXSEL0 register.
//
//******************************************************************************
#define I2C_OBSMUXSEL0_LN3_M  0x07000000  // Observation Mux Lane 3
#define I2C_OBSMUXSEL0_LN3_S  24
#define I2C_OBSMUXSEL0_LN2_M  0x00070000  // Observation Mux Lane 2
#define I2C_OBSMUXSEL0_LN2_S  16
#define I2C_OBSMUXSEL0_LN1_M  0x00000700  // Observation Mux Lane 1
#define I2C_OBSMUXSEL0_LN1_S  8
#define I2C_OBSMUXSEL0_LN0_M  0x00000007  // Observation Mux Lane 0
#define I2C_OBSMUXSEL0_LN0_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_OBSMUXSEL1 register.
//
//******************************************************************************
#define I2C_OBSMUXSEL1_LN7_M  0x07000000  // Observation Mux Lane 7
#define I2C_OBSMUXSEL1_LN7_S  24
#define I2C_OBSMUXSEL1_LN6_M  0x00070000  // Observation Mux Lane 6
#define I2C_OBSMUXSEL1_LN6_S  16
#define I2C_OBSMUXSEL1_LN5_M  0x00000700  // Observation Mux Lane 5
#define I2C_OBSMUXSEL1_LN5_S  8
#define I2C_OBSMUXSEL1_LN4_M  0x00000007  // Observation Mux Lane 4
#define I2C_OBSMUXSEL1_LN4_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MUXROUTE register.
//
//******************************************************************************
#define I2C_MUXROUTE_LN7ROUTE_M \
                                0x70000000  // Lane 7 output is routed to the
                                            // lane pointed to by the offset in
                                            // this bit field

#define I2C_MUXROUTE_LN7ROUTE_S 28
#define I2C_MUXROUTE_LN6ROUTE_M \
                                0x07000000  // Lane 6 output is routed to the
                                            // lane pointed to by the offset in
                                            // this bit field

#define I2C_MUXROUTE_LN6ROUTE_S 24
#define I2C_MUXROUTE_LN5ROUTE_M \
                                0x00700000  // Lane 5 output is routed to the
                                            // lane pointed to by the offset in
                                            // this bit field

#define I2C_MUXROUTE_LN5ROUTE_S 20
#define I2C_MUXROUTE_LN4ROUTE_M \
                                0x00070000  // Lane 4 output is routed to the
                                            // lane pointed to by the offset in
                                            // this bit field

#define I2C_MUXROUTE_LN4ROUTE_S 16
#define I2C_MUXROUTE_LN3ROUTE_M \
                                0x00007000  // Lane 3 output is routed to the
                                            // lane pointed to by the offset in
                                            // this bit field

#define I2C_MUXROUTE_LN3ROUTE_S 12
#define I2C_MUXROUTE_LN2ROUTE_M \
                                0x00000700  // Lane 2 output is routed to the
                                            // lane pointed to by the offset in
                                            // this bit field

#define I2C_MUXROUTE_LN2ROUTE_S 8
#define I2C_MUXROUTE_LN1ROUTE_M \
                                0x00000070  // Lane 1 output is routed to the
                                            // lane pointed to by the offset in
                                            // this bit field

#define I2C_MUXROUTE_LN1ROUTE_S 4
#define I2C_MUXROUTE_LN0ROUTE_M \
                                0x00000007  // Lane 0 output is routed to the
                                            // lane pointed to by the offset in
                                            // this bit field

#define I2C_MUXROUTE_LN0ROUTE_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_PV register.
//
//******************************************************************************
#define I2C_PV_MAJOR_M        0x0000FF00  // Major Revision
#define I2C_PV_MAJOR_S        8
#define I2C_PV_MINOR_M        0x000000FF  // Minor Revision
#define I2C_PV_MINOR_S        0
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_PP register.
//
//******************************************************************************
#define I2C_PP_HS               0x00000001  // High-Speed Capable
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_PC register.
//
//******************************************************************************
#define I2C_PC_HS               0x00000001  // High-Speed Capable
//******************************************************************************
//
// The following are defines for the bit fields in the I2C_O_CC register.
//
//******************************************************************************



#endif // __HW_I2C_H__

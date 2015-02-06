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

#ifndef __HW_UART_H__
#define __HW_UART_H__

//*****************************************************************************
//
// The following are defines for the UART register offsets.
//
//*****************************************************************************
#define UART_O_DR               0x00000000
#define UART_O_RSR              0x00000004
#define UART_O_ECR              0x00000004
#define UART_O_FR               0x00000018
#define UART_O_ILPR             0x00000020
#define UART_O_IBRD             0x00000024
#define UART_O_FBRD             0x00000028
#define UART_O_LCRH             0x0000002C
#define UART_O_CTL              0x00000030
#define UART_O_IFLS             0x00000034
#define UART_O_IM               0x00000038
#define UART_O_RIS              0x0000003C
#define UART_O_MIS              0x00000040
#define UART_O_ICR              0x00000044
#define UART_O_DMACTL           0x00000048
#define UART_O_LCTL             0x00000090
#define UART_O_LSS              0x00000094
#define UART_O_LTIM             0x00000098
#define UART_O_9BITADDR      0x000000A4
#define UART_O_9BITAMASK     0x000000A8
#define UART_O_PP               0x00000FC0
#define UART_O_CC               0x00000FC8



//******************************************************************************
//
// The following are defines for the bit fields in the UART_O_DR register.
//
//******************************************************************************
#define UART_DR_OE              0x00000800  // UART Overrun Error
#define UART_DR_BE              0x00000400  // UART Break Error
#define UART_DR_PE              0x00000200  // UART Parity Error
#define UART_DR_FE              0x00000100  // UART Framing Error
#define UART_DR_DATA_M        0x000000FF  // Data Transmitted or Received
#define UART_DR_DATA_S        0
//******************************************************************************
//
// The following are defines for the bit fields in the UART_O_RSR register.
//
//******************************************************************************
#define UART_RSR_OE             0x00000008  // UART Overrun Error
#define UART_RSR_BE             0x00000004  // UART Break Error
#define UART_RSR_PE             0x00000002  // UART Parity Error
#define UART_RSR_FE             0x00000001  // UART Framing Error
//******************************************************************************
//
// The following are defines for the bit fields in the UART_O_ECR register.
//
//******************************************************************************
#define UART_ECR_DATA_M       0x000000FF  // Error Clear
#define UART_ECR_DATA_S       0
//******************************************************************************
//
// The following are defines for the bit fields in the UART_O_FR register.
//
//******************************************************************************
#define UART_FR_RI              0x00000100  // Ring Indicator
#define UART_FR_TXFE            0x00000080  // UART Transmit FIFO Empty
#define UART_FR_RXFF            0x00000040  // UART Receive FIFO Full
#define UART_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define UART_FR_BUSY            0x00000008  // UART Busy
#define UART_FR_DCD             0x00000004  // Data Carrier Detect
#define UART_FR_DSR             0x00000002  // Data Set Ready
#define UART_FR_CTS             0x00000001  // Clear To Send
//******************************************************************************
//
// The following are defines for the bit fields in the UART_O_ILPR register.
//
//******************************************************************************
#define UART_ILPR_ILPDVSR_M   0x000000FF  // IrDA Low-Power Divisor
#define UART_ILPR_ILPDVSR_S   0
//******************************************************************************
//
// The following are defines for the bit fields in the UART_O_IBRD register.
//
//******************************************************************************
#define UART_IBRD_DIVINT_M    0x0000FFFF  // Integer Baud-Rate Divisor
#define UART_IBRD_DIVINT_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the UART_O_FBRD register.
//
//******************************************************************************
#define UART_FBRD_DIVFRAC_M   0x0000003F  // Fractional Baud-Rate Divisor
#define UART_FBRD_DIVFRAC_S   0
//******************************************************************************
//
// The following are defines for the bit fields in the UART_O_LCRH register.
//
//******************************************************************************
#define UART_LCRH_SPS           0x00000080  // UART Stick Parity Select
#define UART_LCRH_WLEN_M      0x00000060  // UART Word Length 0x00000000 :
                                            // UART_LCRH_WLEN_5 : 5 bits
                                            // (default) 0x00000020 :
                                            // UART_LCRH_WLEN_6 : 6 bits
                                            // 0x00000040 : UART_LCRH_WLEN_7 : 7
                                            // bits 0x00000060 :
                                            // UART_LCRH_WLEN_8 : 8 bits
#define UART_LCRH_WLEN_S      5
#define UART_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART_LCRH_STP2          0x00000008  // UART Two Stop Bits Select
#define UART_LCRH_EPS           0x00000004  // UART Even Parity Select
#define UART_LCRH_PEN           0x00000002  // UART Parity Enable
#define UART_LCRH_BRK           0x00000001  // UART Send Break
#define UART_LCRH_WLEN_M        0x00000060  // UART Word Length
#define UART_LCRH_WLEN_5        0x00000000  // 5 bits (default)
#define UART_LCRH_WLEN_6        0x00000020  // 6 bits
#define UART_LCRH_WLEN_7        0x00000040  // 7 bits
#define UART_LCRH_WLEN_8        0x00000060  // 8 bits
//******************************************************************************
//
// The following are defines for the bit fields in the UART_O_CTL register.
//
//******************************************************************************
#define UART_CTL_CTSEN          0x00008000  // Enable Clear To Send
#define UART_CTL_RTSEN          0x00004000  // Enable Request to Send
#define UART_CTL_RI             0x00002000  // Ring Indicator
#define UART_CTL_DCD            0x00001000  // Data Carrier Detect
#define UART_CTL_RTS            0x00000800  // Request to Send
#define UART_CTL_DTR            0x00000400  // Data Terminal Ready
#define UART_CTL_RXE            0x00000200  // UART Receive Enable
#define UART_CTL_TXE            0x00000100  // UART Transmit Enable
#define UART_CTL_LBE            0x00000080  // UART Loop Back Enable
#define UART_CTL_LIN            0x00000040  // LIN Mode Enable
#define UART_CTL_HSE            0x00000020  // High-Speed Enable
#define UART_CTL_EOT            0x00000010  // End of Transmission
#define UART_CTL_SMART          0x00000008  // ISO 7816 Smart Card Support
#define UART_CTL_SIRLP          0x00000004  // UART SIR Low-Power Mode
#define UART_CTL_SIREN          0x00000002  // UART SIR Enable
#define UART_CTL_UARTEN         0x00000001  // UART Enable
//******************************************************************************
//
// The following are defines for the bit fields in the UART_O_IFLS register.
//
//******************************************************************************
#define UART_IFLS_RX_M        0x00000038  // UART Receive Interrupt FIFO
                                            // Level Select
#define UART_IFLS_RX_S        3
#define UART_IFLS_TX_M        0x00000007  // UART Transmit Interrupt FIFO
                                            // Level Select
#define UART_IFLS_TX_S        0
//******************************************************************************
//
// The following are defines for the bit fields in the UART_O_IM register.
//
//******************************************************************************
#define UART_IM_DMATXIM         0x00020000  // Transmit DMA Interrupt Mask
#define UART_IM_DMARXIM         0x00010000  // Receive DMA Interrupt Mask
#define UART_IM_LME5IM          0x00008000  // LIN Mode Edge 5 Interrupt Mask
#define UART_IM_LME1IM          0x00004000  // LIN Mode Edge 1 Interrupt Mask
#define UART_IM_LMSBIM          0x00002000  // LIN Mode Sync Break Interrupt
                                            // Mask
#define UART_IM_9BITIM       0x00001000  // 9-Bit Mode Interrupt Mask
#define UART_IM_EOTIM           0x00000800  // End of Transmission Interrupt
                                            // Mask
#define UART_IM_OEIM            0x00000400  // UART Overrun Error Interrupt
                                            // Mask
#define UART_IM_BEIM            0x00000200  // UART Break Error Interrupt Mask
#define UART_IM_PEIM            0x00000100  // UART Parity Error Interrupt Mask
#define UART_IM_FEIM            0x00000080  // UART Framing Error Interrupt
                                            // Mask
#define UART_IM_RTIM            0x00000040  // UART Receive Time-Out Interrupt
                                            // Mask
#define UART_IM_TXIM            0x00000020  // UART Transmit Interrupt Mask
#define UART_IM_RXIM            0x00000010  // UART Receive Interrupt Mask
#define UART_IM_DSRMIM          0x00000008  // UART Data Set Ready Modem
                                            // Interrupt Mask
#define UART_IM_DCDMIM          0x00000004  // UART Data Carrier Detect Modem
                                            // Interrupt Mask
#define UART_IM_CTSMIM          0x00000002  // UART Clear to Send Modem
                                            // Interrupt Mask
#define UART_IM_RIMIM           0x00000001  // UART Ring Indicator Modem
                                            // Interrupt Mask
//******************************************************************************
//
// The following are defines for the bit fields in the UART_O_RIS register.
//
//******************************************************************************
#define UART_RIS_DMATXRIS       0x00020000  // Transmit DMA Raw Interrupt
                                            // Status
#define UART_RIS_DMARXRIS       0x00010000  // Receive DMA Raw Interrupt Status
#define UART_RIS_LME5RIS        0x00008000  // LIN Mode Edge 5 Raw Interrupt
                                            // Status
#define UART_RIS_LME1RIS        0x00004000  // LIN Mode Edge 1 Raw Interrupt
                                            // Status
#define UART_RIS_LMSBRIS        0x00002000  // LIN Mode Sync Break Raw
                                            // Interrupt Status
#define UART_RIS_9BITRIS     0x00001000  // 9-Bit Mode Raw Interrupt Status
#define UART_RIS_EOTRIS         0x00000800  // End of Transmission Raw
                                            // Interrupt Status
#define UART_RIS_OERIS          0x00000400  // UART Overrun Error Raw Interrupt
                                            // Status
#define UART_RIS_BERIS          0x00000200  // UART Break Error Raw Interrupt
                                            // Status
#define UART_RIS_PERIS          0x00000100  // UART Parity Error Raw Interrupt
                                            // Status
#define UART_RIS_FERIS          0x00000080  // UART Framing Error Raw Interrupt
                                            // Status
#define UART_RIS_RTRIS          0x00000040  // UART Receive Time-Out Raw
                                            // Interrupt Status
#define UART_RIS_TXRIS          0x00000020  // UART Transmit Raw Interrupt
                                            // Status
#define UART_RIS_RXRIS          0x00000010  // UART Receive Raw Interrupt
                                            // Status
#define UART_RIS_DSRRIS         0x00000008  // UART Data Set Ready Modem Raw
                                            // Interrupt Status
#define UART_RIS_DCDRIS         0x00000004  // UART Data Carrier Detect Modem
                                            // Raw Interrupt Status
#define UART_RIS_CTSRIS         0x00000002  // UART Clear to Send Modem Raw
                                            // Interrupt Status
#define UART_RIS_RIRIS          0x00000001  // UART Ring Indicator Modem Raw
                                            // Interrupt Status
//******************************************************************************
//
// The following are defines for the bit fields in the UART_O_MIS register.
//
//******************************************************************************
#define UART_MIS_DMATXMIS       0x00020000  // Transmit DMA Masked Interrupt
                                            // Status
#define UART_MIS_DMARXMIS       0x00010000  // Receive DMA Masked Interrupt
                                            // Status
#define UART_MIS_LME5MIS        0x00008000  // LIN Mode Edge 5 Masked Interrupt
                                            // Status
#define UART_MIS_LME1MIS        0x00004000  // LIN Mode Edge 1 Masked Interrupt
                                            // Status
#define UART_MIS_LMSBMIS        0x00002000  // LIN Mode Sync Break Masked
                                            // Interrupt Status
#define UART_MIS_9BITMIS     0x00001000  // 9-Bit Mode Masked Interrupt
                                            // Status
#define UART_MIS_EOTMIS         0x00000800  // End of Transmission Masked
                                            // Interrupt Status
#define UART_MIS_OEMIS          0x00000400  // UART Overrun Error Masked
                                            // Interrupt Status
#define UART_MIS_BEMIS          0x00000200  // UART Break Error Masked
                                            // Interrupt Status
#define UART_MIS_PEMIS          0x00000100  // UART Parity Error Masked
                                            // Interrupt Status
#define UART_MIS_FEMIS          0x00000080  // UART Framing Error Masked
                                            // Interrupt Status
#define UART_MIS_RTMIS          0x00000040  // UART Receive Time-Out Masked
                                            // Interrupt Status
#define UART_MIS_TXMIS          0x00000020  // UART Transmit Masked Interrupt
                                            // Status
#define UART_MIS_RXMIS          0x00000010  // UART Receive Masked Interrupt
                                            // Status
#define UART_MIS_DSRMIS         0x00000008  // UART Data Set Ready Modem Masked
                                            // Interrupt Status
#define UART_MIS_DCDMIS         0x00000004  // UART Data Carrier Detect Modem
                                            // Masked Interrupt Status
#define UART_MIS_CTSMIS         0x00000002  // UART Clear to Send Modem Masked
                                            // Interrupt Status
#define UART_MIS_RIMIS          0x00000001  // UART Ring Indicator Modem Masked
                                            // Interrupt Status
//******************************************************************************
//
// The following are defines for the bit fields in the UART_O_ICR register.
//
//******************************************************************************
#define UART_ICR_DMATXIC        0x00020000  // Transmit DMA Interrupt Clear
#define UART_ICR_DMARXIC        0x00010000  // Receive DMA Interrupt Clear
#define UART_ICR_LME5MIC        0x00008000  // LIN Mode Edge 5 Interrupt Clear
#define UART_ICR_LME1MIC        0x00004000  // LIN Mode Edge 1 Interrupt Clear
#define UART_ICR_LMSBMIC        0x00002000  // LIN Mode Sync Break Interrupt
                                            // Clear
#define UART_ICR_9BITIC      0x00001000  // 9-Bit Mode Interrupt Clear
#define UART_ICR_EOTIC          0x00000800  // End of Transmission Interrupt
                                            // Clear
#define UART_ICR_OEIC           0x00000400  // Overrun Error Interrupt Clear
#define UART_ICR_BEIC           0x00000200  // Break Error Interrupt Clear
#define UART_ICR_PEIC           0x00000100  // Parity Error Interrupt Clear
#define UART_ICR_FEIC           0x00000080  // Framing Error Interrupt Clear
#define UART_ICR_RTIC           0x00000040  // Receive Time-Out Interrupt Clear
#define UART_ICR_TXIC           0x00000020  // Transmit Interrupt Clear
#define UART_ICR_RXIC           0x00000010  // Receive Interrupt Clear
#define UART_ICR_DSRMIC         0x00000008  // UART Data Set Ready Modem
                                            // Interrupt Clear
#define UART_ICR_DCDMIC         0x00000004  // UART Data Carrier Detect Modem
                                            // Interrupt Clear
#define UART_ICR_CTSMIC         0x00000002  // UART Clear to Send Modem
                                            // Interrupt Clear
#define UART_ICR_RIMIC          0x00000001  // UART Ring Indicator Modem
                                            // Interrupt Clear
//******************************************************************************
//
// The following are defines for the bit fields in the UART_O_DMACTL register.
//
//******************************************************************************
#define UART_DMACTL_DMAERR      0x00000004  // DMA on Error
#define UART_DMACTL_TXDMAE      0x00000002  // Transmit DMA Enable
#define UART_DMACTL_RXDMAE      0x00000001  // Receive DMA Enable
//******************************************************************************
//
// The following are defines for the bit fields in the UART_O_LCTL register.
//
//******************************************************************************
#define UART_LCTL_BLEN_M      0x00000030  // Sync Break Length 0x00000000 :
                                            // UART_LCTL_BLEN_13T : Sync break
                                            // length is 13T bits (default)
                                            // 0x00000010 : UART_LCTL_BLEN_14T :
                                            // Sync break length is 14T bits
                                            // 0x00000020 : UART_LCTL_BLEN_15T :
                                            // Sync break length is 15T bits
                                            // 0x00000030 : UART_LCTL_BLEN_16T :
                                            // Sync break length is 16T bits
#define UART_LCTL_BLEN_S      4
#define UART_LCTL_MASTER        0x00000001  // LIN Master Enable
//******************************************************************************
//
// The following are defines for the bit fields in the UART_O_LSS register.
//
//******************************************************************************
#define UART_LSS_TSS_M        0x0000FFFF  // Timer Snap Shot
#define UART_LSS_TSS_S        0
//******************************************************************************
//
// The following are defines for the bit fields in the UART_O_LTIM register.
//
//******************************************************************************
#define UART_LTIM_TIMER_M     0x0000FFFF  // Timer Value
#define UART_LTIM_TIMER_S     0
//******************************************************************************
//
// The following are defines for the bit fields in the
// UART_O_9BITADDR register.
//
//******************************************************************************
#define UART_9BITADDR_9BITEN \
                                0x00008000  // Enable 9-Bit Mode

#define UART_9BITADDR_ADDR_M \
                                0x000000FF  // Self Address for 9-Bit Mode

#define UART_9BITADDR_ADDR_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// UART_O_9BITAMASK register.
//
//******************************************************************************
#define UART_9BITAMASK_RANGE_M \
                                0x0000FF00  // Self Address Range for 9-Bit
                                            // Mode

#define UART_9BITAMASK_RANGE_S 8
#define UART_9BITAMASK_MASK_M \
                                0x000000FF  // Self Address Mask for 9-Bit Mode

#define UART_9BITAMASK_MASK_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the UART_O_PP register.
//
//******************************************************************************
#define UART_PP_MSE             0x00000008  // Modem Support Extended
#define UART_PP_MS              0x00000004  // Modem Support
#define UART_PP_NB              0x00000002  // 9-Bit Support
#define UART_PP_SC              0x00000001  // Smart Card Support
//******************************************************************************
//
// The following are defines for the bit fields in the UART_O_CC register.
//
//******************************************************************************
#define UART_CC_CS_M          0x0000000F  // UART Baud Clock Source
                                            // 0x00000005 : UART_CC_CS_PIOSC :
                                            // PIOSC 0x00000000 :
                                            // UART_CC_CS_SYSCLK : The system
                                            // clock (default)
#define UART_CC_CS_S          0



#endif // __HW_UART_H__

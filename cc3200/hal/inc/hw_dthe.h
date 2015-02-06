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

#ifndef __HW_DTHE_H__
#define __HW_DTHE_H__

//*****************************************************************************
//
// The following are defines for the DTHE register offsets.
//
//*****************************************************************************
#define DTHE_O_SHA_IM    0x00000810
#define DTHE_O_SHA_RIS    0x00000814
#define DTHE_O_SHA_MIS    0x00000818
#define DTHE_O_SHA_IC    0x0000081C
#define DTHE_O_AES_IM    0x00000820
#define DTHE_O_AES_RIS    0x00000824
#define DTHE_O_AES_MIS    0x00000828
#define DTHE_O_AES_IC    0x0000082C
#define DTHE_O_DES_IM    0x00000830
#define DTHE_O_DES_RIS    0x00000834
#define DTHE_O_DES_MIS    0x00000838
#define DTHE_O_DES_IC    0x0000083C
#define DTHE_O_EIP_CGCFG      0x00000A00
#define DTHE_O_EIP_CGREQ      0x00000A04
#define DTHE_O_CRC_CTRL       0x00000C00
#define DTHE_O_CRC_SEED       0x00000C10
#define DTHE_O_CRC_DIN        0x00000C14
#define DTHE_O_CRC_RSLT_PP    0x00000C18
#define DTHE_O_RAND_KEY0      0x00000F00
#define DTHE_O_RAND_KEY1      0x00000F04
#define DTHE_O_RAND_KEY2      0x00000F08
#define DTHE_O_RAND_KEY3      0x00000F0C


//******************************************************************************
//
// The following are defines for the bit fields in the
// DTHE_O_SHAMD5_IMST register.
//
//******************************************************************************
#define DTHE_SHAMD5_IMST_DIN  0x00000004  // Data in: this interrupt is
                                            // raised when DMA writes last word
                                            // of input data to internal FIFO of
                                            // the engine
#define DTHE_SHAMD5_IMST_COUT 0x00000002  // Context out: this interrupt is
                                            // raised when DMA complets the
                                            // output context movement from
                                            // internal register
#define DTHE_SHAMD5_IMST_CIN  0x00000001  // context in: this interrupt is
                                            // raised when DMA complets Context
                                            // write to internal register
//******************************************************************************
//
// The following are defines for the bit fields in the
// DTHE_O_SHAMD5_IRIS register.
//
//******************************************************************************
#define DTHE_SHAMD5_IRIS_DIN  0x00000004  // input Data movement is done
#define DTHE_SHAMD5_IRIS_COUT 0x00000002  // Context output is done
#define DTHE_SHAMD5_IRIS_CIN  0x00000001  // context input is done
//******************************************************************************
//
// The following are defines for the bit fields in the
// DTHE_O_SHAMD5_IMIS register.
//
//******************************************************************************
#define DTHE_SHAMD5_IMIS_DIN  0x00000004  // input Data movement is done
#define DTHE_SHAMD5_IMIS_COUT 0x00000002  // Context output is done
#define DTHE_SHAMD5_IMIS_CIN  0x00000001  // context input is done
//******************************************************************************
//
// The following are defines for the bit fields in the
// DTHE_O_SHAMD5_ICIS register.
//
//******************************************************************************
#define DTHE_SHAMD5_ICIS_DIN  0x00000004  // Clear “input Data movement done”
                                            // flag
#define DTHE_SHAMD5_ICIS_COUT 0x00000002  // Clear “Context output done” flag
#define DTHE_SHAMD5_ICIS_CIN  0x00000001  // Clear “context input done” flag
//******************************************************************************
//
// The following are defines for the bit fields in the
// DTHE_O_AES_IMST register.
//
//******************************************************************************
#define DTHE_AES_IMST_DOUT 0x00000008  // Data out: this interrupt is
                                            // raised when DMA finishes writing
                                            // last word of the process result
#define DTHE_AES_IMST_DIN  0x00000004  // Data in: this interrupt is
                                            // raised when DMA writes last word
                                            // of input data to internal FIFO of
                                            // the engine
#define DTHE_AES_IMST_COUT 0x00000002  // Context out: this interrupt is
                                            // raised when DMA complets the
                                            // output context movement from
                                            // internal register
#define DTHE_AES_IMST_CIN  0x00000001  // context in: this interrupt is
                                            // raised when DMA complets Context
                                            // write to internal register
//******************************************************************************
//
// The following are defines for the bit fields in the
// DTHE_O_AES_IRIS register.
//
//******************************************************************************
#define DTHE_AES_IRIS_DOUT 0x00000008  // Output Data movement is done
#define DTHE_AES_IRIS_DIN  0x00000004  // input Data movement is done
#define DTHE_AES_IRIS_COUT 0x00000002  // Context output is done
#define DTHE_AES_IRIS_CIN  0x00000001  // context input is done
//******************************************************************************
//
// The following are defines for the bit fields in the
// DTHE_O_AES_IMIS register.
//
//******************************************************************************
#define DTHE_AES_IMIS_DOUT 0x00000008  // Output Data movement is done
#define DTHE_AES_IMIS_DIN  0x00000004  // input Data movement is done
#define DTHE_AES_IMIS_COUT 0x00000002  // Context output is done
#define DTHE_AES_IMIS_CIN  0x00000001  // context input is done
//******************************************************************************
//
// The following are defines for the bit fields in the
// DTHE_O_AES_ICIS register.
//
//******************************************************************************
#define DTHE_AES_ICIS_DOUT 0x00000008  // Clear “output Data movement
                                            // done” flag
#define DTHE_AES_ICIS_DIN  0x00000004  // Clear “input Data movement done”
                                            // flag
#define DTHE_AES_ICIS_COUT 0x00000002  // Clear “Context output done” flag
#define DTHE_AES_ICIS_CIN  0x00000001  // Clear “context input done” flag
//******************************************************************************
//
// The following are defines for the bit fields in the
// DTHE_O_DES_IMST register.
//
//******************************************************************************
#define DTHE_DES_IMST_DOUT 0x00000008  // Data out: this interrupt is
                                            // raised when DMA finishes writing
                                            // last word of the process result
#define DTHE_DES_IMST_DIN  0x00000004  // Data in: this interrupt is
                                            // raised when DMA writes last word
                                            // of input data to internal FIFO of
                                            // the engine
#define DTHE_DES_IMST_CIN  0x00000001  // context in: this interrupt is
                                            // raised when DMA complets Context
                                            // write to internal register
//******************************************************************************
//
// The following are defines for the bit fields in the
// DTHE_O_DES_IRIS register.
//
//******************************************************************************
#define DTHE_DES_IRIS_DOUT 0x00000008  // Output Data movement is done
#define DTHE_DES_IRIS_DIN  0x00000004  // input Data movement is done
#define DTHE_DES_IRIS_CIN  0x00000001  // context input is done
//******************************************************************************
//
// The following are defines for the bit fields in the
// DTHE_O_DES_IMIS register.
//
//******************************************************************************
#define DTHE_DES_IMIS_DOUT 0x00000008  // Output Data movement is done
#define DTHE_DES_IMIS_DIN  0x00000004  // input Data movement is done
#define DTHE_DES_IMIS_CIN  0x00000001  // context input is done
//******************************************************************************
//
// The following are defines for the bit fields in the
// DTHE_O_DES_ICIS register.
//
//******************************************************************************
#define DTHE_DES_ICIS_DOUT 0x00000008  // Clear “output Data movement
                                            // done” flag
#define DTHE_DES_ICIS_DIN  0x00000004  // Clear “input Data movement done”
                                            // flag
#define DTHE_DES_ICIS_CIN  0x00000001  // Clear "context input done” flag
//******************************************************************************
//
// The following are defines for the bit fields in the
// DTHE_O_EIP_CGCFG register.
//
//******************************************************************************
#define DTHE_EIP_CGCFG_EIP29_CFG \
                                0x00000010  // Clock gating protocol setting
                                            // for EIP29T. 0 – Follow direct
                                            // protocol 1 – Follow idle_req/ack
                                            // protocol.

#define DTHE_EIP_CGCFG_EIP75_CFG \
                                0x00000008  // Clock gating protocol setting
                                            // for EIP75T. 0 – Follow direct
                                            // protocol 1 – Follow idle_req/ack
                                            // protocol.

#define DTHE_EIP_CGCFG_EIP16_CFG \
                                0x00000004  // Clock gating protocol setting
                                            // for DES. 0 – Follow direct
                                            // protocol 1 – Follow idle_req/ack
                                            // protocol.

#define DTHE_EIP_CGCFG_EIP36_CFG \
                                0x00000002  // Clock gating protocol setting
                                            // for AES. 0 – Follow direct
                                            // protocol 1 – Follow idle_req/ack
                                            // protocol.

#define DTHE_EIP_CGCFG_EIP57_CFG \
                                0x00000001  // Clock gating protocol setting
                                            // for SHAMD5. 0 – Follow direct
                                            // protocol 1 – Follow idle_req/ack
                                            // protocol.

//******************************************************************************
//
// The following are defines for the bit fields in the
// DTHE_O_EIP_CGREQ register.
//
//******************************************************************************
#define DTHE_EIP_CGREQ_Key_M  0xF0000000  // When “0x5” write “1” to lower
                                            // bits [4:0] will set the bit.
                                            // Write “0” will be ignored When
                                            // “0x2” write “1” to lower bit
                                            // [4:0] will clear the bit. Write
                                            // “0” will be ignored for other key
                                            // value, regular read write
                                            // operation
#define DTHE_EIP_CGREQ_Key_S  28
#define DTHE_EIP_CGREQ_EIP29_REQ \
                                0x00000010  // 0 – request clock gating 1 –
                                            // request to un-gate the clock.

#define DTHE_EIP_CGREQ_EIP75_REQ \
                                0x00000008  // 0 – request clock gating 1 –
                                            // request to un-gate the clock.

#define DTHE_EIP_CGREQ_EIP16_REQ \
                                0x00000004  // 0 – request clock gating 1 –
                                            // request to un-gate the clock.

#define DTHE_EIP_CGREQ_EIP36_REQ \
                                0x00000002  // 0 – request clock gating 1 –
                                            // request to un-gate the clock.

#define DTHE_EIP_CGREQ_EIP57_REQ \
                                0x00000001  // 0 – request clock gating 1 –
                                            // request to un-gate the clock.

//******************************************************************************
//
// The following are defines for the bit fields in the DTHE_O_CRC_CTRL register.
//
//******************************************************************************
#define DTHE_CRC_CTRL_INIT_M  0x00006000  // Initialize the CRC 00 – use SEED
                                            // register context as starting
                                            // value 10 – all “zero” 11 – all
                                            // “one” This is self clearing. With
                                            // first write to data register this
                                            // value clears to zero and remain
                                            // zero for rest of the operation
                                            // unless written again
#define DTHE_CRC_CTRL_INIT_S  13
#define DTHE_CRC_CTRL_SIZE    0x00001000  // Input data size 0 – 32 bit 1 – 8
                                            // bit
#define DTHE_CRC_CTRL_OINV    0x00000200  // Inverse the bits of result
                                            // before storing to CRC_RSLT_PP0
#define DTHE_CRC_CTRL_OBR     0x00000100  // Bit reverse the output result
                                            // byte before storing to
                                            // CRC_RSLT_PP0. applicable for all
                                            // bytes in word
#define DTHE_CRC_CTRL_IBR     0x00000080  // Bit reverse the input byte. For
                                            // all bytes in word
#define DTHE_CRC_CTRL_ENDIAN_M \
                                0x00000030  // Endian control [0] – swap byte
                                            // in half-word [1] – swap half word

#define DTHE_CRC_CTRL_ENDIAN_S 4
#define DTHE_CRC_CTRL_TYPE_M  0x0000000F  // Type of operation 0000 –
                                            // polynomial 0x8005 0001 –
                                            // polynomial 0x1021 0010 –
                                            // polynomial 0x4C11DB7 0011 –
                                            // polynomial 0x1EDC6F41 1000 – TCP
                                            // checksum TYPE in DTHE_S_CRC_CTRL
                                            // & DTHE_S_CRC_CTRL should be
                                            // exclusive
#define DTHE_CRC_CTRL_TYPE_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the DTHE_O_CRC_SEED register.
//
//******************************************************************************
#define DTHE_CRC_SEED_SEED_M  0xFFFFFFFF  // Starting seed of CRC and
                                            // checksum operation. Please see
                                            // CTRL register for more detail.
                                            // This resister also holds the
                                            // latest result of CRC or checksum
                                            // operation
#define DTHE_CRC_SEED_SEED_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the DTHE_O_CRC_DIN register.
//
//******************************************************************************
#define DTHE_CRC_DIN_DATA_IN_M \
                                0xFFFFFFFF  // Input data for CRC or checksum
                                            // operation

#define DTHE_CRC_DIN_DATA_IN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// DTHE_O_CRC_RSLT_PP register.
//
//******************************************************************************
#define DTHE_CRC_RSLT_PP_RSLT_PP_M \
                                0xFFFFFFFF  // Input data for CRC or checksum
                                            // operation

#define DTHE_CRC_RSLT_PP_RSLT_PP_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// DTHE_O_RAND_KEY0 register.
//
//******************************************************************************
#define DTHE_RAND_KEY0_KEY_M  0xFFFFFFFF  // Device Specific Randon key
                                            // [31:0]
#define DTHE_RAND_KEY0_KEY_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the
// DTHE_O_RAND_KEY1 register.
//
//******************************************************************************
#define DTHE_RAND_KEY1_KEY_M  0xFFFFFFFF  // Device Specific Randon key
                                            // [63:32]
#define DTHE_RAND_KEY1_KEY_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the
// DTHE_O_RAND_KEY2 register.
//
//******************************************************************************
#define DTHE_RAND_KEY2_KEY_M  0xFFFFFFFF  // Device Specific Randon key
                                            // [95:34]
#define DTHE_RAND_KEY2_KEY_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the
// DTHE_O_RAND_KEY3 register.
//
//******************************************************************************
#define DTHE_RAND_KEY3_KEY_M  0xFFFFFFFF  // Device Specific Randon key
                                            // [127:96]
#define DTHE_RAND_KEY3_KEY_S  0



#endif // __HW_DTHE_H__

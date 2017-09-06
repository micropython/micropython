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

#ifndef __HW_SHAMD5_H__
#define __HW_SHAMD5_H__

//*****************************************************************************
//
// The following are defines for the SHAMD5_P register offsets.
//
//*****************************************************************************
#define SHAMD5_O_ODIGEST_A       0x00000000  // WRITE: Outer Digest [127:96] for
                                            // MD5 [159:128] for SHA-1 [255:224]
                                            // for SHA-2 / HMAC Key [31:0] for
                                            // HMAC key proc READ: Outer Digest
                                            // [127:96] for MD5 [159:128] for
                                            // SHA-1 [255:224] for SHA-2
#define SHAMD5_O_ODIGEST_B       0x00000004  // WRITE: Outer Digest [95:64] for
                                            // MD5 [127:96] for SHA-1 [223:192]
                                            // for SHA-2 / HMAC Key [63:32] for
                                            // HMAC key proc READ: Outer Digest
                                            // [95:64] for MD5 [127:96] for
                                            // SHA-1 [223:192] for SHA-2
#define SHAMD5_O_ODIGEST_C       0x00000008  // WRITE: Outer Digest [63:32] for
                                            // MD5 [95:64] for SHA-1 [191:160]
                                            // for SHA-2 / HMAC Key [95:64] for
                                            // HMAC key proc READ: Outer Digest
                                            // [63:32] for MD5 [95:64] for SHA-1
                                            // [191:160] for SHA-2
#define SHAMD5_O_ODIGEST_D       0x0000000C  // WRITE: Outer Digest [31:0] for
                                            // MD5 [63:31] for SHA-1 [159:128]
                                            // for SHA-2 / HMAC Key [127:96] for
                                            // HMAC key proc READ: Outer Digest
                                            // [31:0] for MD5 [63:32] for SHA-1
                                            // [159:128] for SHA-2
#define SHAMD5_O_ODIGEST_E       0x00000010  // WRITE: Outer Digest [31:0] for
                                            // SHA-1 [127:96] for SHA-2 / HMAC
                                            // Key [159:128] for HMAC key proc
                                            // READ: Outer Digest [31:0] for
                                            // SHA-1 [127:96] for SHA-2
#define SHAMD5_O_ODIGEST_F       0x00000014  // WRITE: Outer Digest [95:64] for
                                            // SHA-2 / HMAC Key [191:160] for
                                            // HMAC key proc READ: Outer Digest
                                            // [95:64] for SHA-2
#define SHAMD5_O_ODIGEST_G       0x00000018  // WRITE: Outer Digest [63:32] for
                                            // SHA-2 / HMAC Key [223:192] for
                                            // HMAC key proc READ: Outer Digest
                                            // [63:32] for SHA-2
#define SHAMD5_O_ODIGEST_H       0x0000001C  // WRITE: Outer Digest [31:0] for
                                            // SHA-2 / HMAC Key [255:224] for
                                            // HMAC key proc READ: Outer Digest
                                            // [31:0] for SHA-2
#define SHAMD5_O_IDIGEST_A       0x00000020  // WRITE: Inner / Initial Digest
                                            // [127:96] for MD5 [159:128] for
                                            // SHA-1 [255:224] for SHA-2 / HMAC
                                            // Key [287:256] for HMAC key proc
                                            // READ: Intermediate / Inner Digest
                                            // [127:96] for MD5 [159:128] for
                                            // SHA-1 [255:224] for SHA-2 /
                                            // Result Digest/MAC [127:96] for
                                            // MD5 [159:128] for SHA-1 [223:192]
                                            // for SHA-2 224 [255:224] for SHA-2
                                            // 256
#define SHAMD5_O_IDIGEST_B       0x00000024  // WRITE: Inner / Initial Digest
                                            // [95:64] for MD5 [127:96] for
                                            // SHA-1 [223:192] for SHA-2 / HMAC
                                            // Key [319:288] for HMAC key proc
                                            // READ: Intermediate / Inner Digest
                                            // [95:64] for MD5 [127:96] for
                                            // SHA-1 [223:192] for SHA-2 /
                                            // Result Digest/MAC [95:64] for MD5
                                            // [127:96] for SHA-1 [191:160] for
                                            // SHA-2 224 [223:192] for SHA-2 256
#define SHAMD5_O_IDIGEST_C       0x00000028  // WRITE: Inner / Initial Digest
                                            // [63:32] for MD5 [95:64] for SHA-1
                                            // [191:160] for SHA- 2 / HMAC Key
                                            // [351:320] for HMAC key proc READ:
                                            // Intermediate / Inner Digest
                                            // [63:32] for MD5 [95:64] for SHA-1
                                            // [191:160] for SHA-2 / Result
                                            // Digest/MAC [63:32] for MD5
                                            // [95:64] for SHA-1 [159:128] for
                                            // SHA-2 224 [191:160] for SHA-2 256
#define SHAMD5_O_IDIGEST_D       0x0000002C  // WRITE: Inner / Initial Digest
                                            // [31:0] for MD5 [63:32] for SHA-1
                                            // [159:128] for SHA-2 / HMAC Key
                                            // [383:352] for HMAC key proc READ:
                                            // Intermediate / Inner Digest
                                            // [31:0] for MD5 [63:32] for SHA-1
                                            // [159:128] for SHA-2 / Result
                                            // Digest/MAC [31:0] for MD5 [63:32]
                                            // for SHA-1 [127:96] for SHA-2 224
                                            // [159:128] for SHA-2 256
#define SHAMD5_O_IDIGEST_E       0x00000030  // WRITE: Inner / Initial Digest
                                            // [31:0] for SHA-1 [127:96] for
                                            // SHA-2 / HMAC Key [415:384] for
                                            // HMAC key proc READ: Intermediate
                                            // / Inner Digest [31:0] for SHA-1
                                            // [127:96] for SHA-2 / Result
                                            // Digest/MAC [31:0] for SHA-1
                                            // [95:64] for SHA-2 224 [127:96]
                                            // for SHA-2 256
#define SHAMD5_O_IDIGEST_F       0x00000034  // WRITE: Inner / Initial Digest
                                            // [95:64] for SHA-2 / HMAC Key
                                            // [447:416] for HMAC key proc READ:
                                            // Intermediate / Inner Digest
                                            // [95:64] for SHA-2 / Result
                                            // Digest/MAC [63:32] for SHA-2 224
                                            // [95:64] for SHA-2 256
#define SHAMD5_O_IDIGEST_G       0x00000038  // WRITE: Inner / Initial Digest
                                            // [63:32] for SHA-2 / HMAC Key
                                            // [479:448] for HMAC key proc READ:
                                            // Intermediate / Inner Digest
                                            // [63:32] for SHA-2 / Result
                                            // Digest/MAC [31:0] for SHA-2 224
                                            // [63:32] for SHA-2 256
#define SHAMD5_O_IDIGEST_H       0x0000003C  // WRITE: Inner / Initial Digest
                                            // [31:0] for SHA-2 / HMAC Key
                                            // [511:480] for HMAC key proc READ:
                                            // Intermediate / Inner Digest
                                            // [31:0] for SHA-2 / Result
                                            // Digest/MAC [31:0] for SHA-2 256
#define SHAMD5_O_DIGEST_COUNT    0x00000040  // WRITE: Initial Digest Count
                                            // ([31:6] only [5:0] assumed 0)
                                            // READ: Result / IntermediateDigest
                                            // Count The initial digest byte
                                            // count for hash/HMAC continue
                                            // operations (HMAC Key Processing =
                                            // 0 and Use Algorithm Constants =
                                            // 0) on the Secure World must be
                                            // written to this register prior to
                                            // starting the operation by writing
                                            // to S_HASH_MODE. When either HMAC
                                            // Key Processing is 1 or Use
                                            // Algorithm Constants is 1 this
                                            // register does not need to be
                                            // written it will be overwritten
                                            // with 64 (1 hash block of key XOR
                                            // ipad) or 0 respectively
                                            // automatically. When starting a
                                            // HMAC operation from pre-computes
                                            // (HMAC Key Processing is 0) then
                                            // the value 64 must be written here
                                            // to compensate for the appended
                                            // key XOR ipad block. Note that the
                                            // value written should always be a
                                            // 64 byte multiple the lower 6 bits
                                            // written are ignored. The updated
                                            // digest byte count (initial digest
                                            // byte count + bytes processed) can
                                            // be read from this register when
                                            // the status register indicates
                                            // that the operation is done or
                                            // suspended due to a context switch
                                            // request or when a Secure World
                                            // context out DMA is requested. In
                                            // Advanced DMA mode when not
                                            // suspended with a partial result
                                            // reading the SHAMD5_DIGEST_COUNT
                                            // register triggers the Hash/HMAC
                                            // Engine to start the next context
                                            // input DMA. Therefore reading the
                                            // SHAMD5_DIGEST_COUNT register
                                            // should always be the last
                                            // context-read action if not
                                            // suspended with a partial result
                                            // (i.e. PartHashReady interrupt not
                                            // pending).
#define SHAMD5_O_MODE            0x00000044  // Register SHAMD5_MODE
#define SHAMD5_O_LENGTH          0x00000048  // WRITE: Block Length / Remaining
                                            // Byte Count (bytes) READ:
                                            // Remaining Byte Count. The value
                                            // programmed MUST be a 64-byte
                                            // multiple if Close Hash is set to
                                            // 0. This register is also the
                                            // trigger to start processing: once
                                            // this register is written the core
                                            // will commence requesting input
                                            // data via DMA or IRQ (if
                                            // programmed length > 0) and start
                                            // processing. The remaining byte
                                            // count for the active operation
                                            // can be read from this register
                                            // when the interrupt status
                                            // register indicates that the
                                            // operation is suspended due to a
                                            // context switch request.
#define SHAMD5_O_DATA0_IN        0x00000080  // Data input message 0
#define SHAMD5_O_DATA1_IN        0x00000084  // Data input message 1
#define SHAMD5_O_DATA2_IN        0x00000088  // Data input message 2
#define SHAMD5_O_DATA3_IN        0x0000008C  // Data input message 3
#define SHAMD5_O_DATA4_IN        0x00000090  // Data input message 4
#define SHAMD5_O_DATA5_IN        0x00000094  // Data input message 5
#define SHAMD5_O_DATA6_IN        0x00000098  // Data input message 6
#define SHAMD5_O_DATA7_IN        0x0000009C  // Data input message 7
#define SHAMD5_O_DATA8_IN        0x000000A0  // Data input message 8
#define SHAMD5_O_DATA9_IN        0x000000A4  // Data input message 9
#define SHAMD5_O_DATA10_IN       0x000000A8  // Data input message 10
#define SHAMD5_O_DATA11_IN       0x000000AC  // Data input message 11
#define SHAMD5_O_DATA12_IN       0x000000B0  // Data input message 12
#define SHAMD5_O_DATA13_IN       0x000000B4  // Data input message 13
#define SHAMD5_O_DATA14_IN       0x000000B8  // Data input message 14
#define SHAMD5_O_DATA15_IN       0x000000BC  // Data input message 15
#define SHAMD5_O_REVISION        0x00000100  // Register SHAMD5_REV
#define SHAMD5_O_SYSCONFIG       0x00000110  // Register SHAMD5_SYSCONFIG
#define SHAMD5_O_SYSSTATUS       0x00000114  // Register SHAMD5_SYSSTATUS
#define SHAMD5_O_IRQSTATUS       0x00000118  // Register SHAMD5_IRQSTATUS
#define SHAMD5_O_IRQENABLE       0x0000011C  // Register SHAMD5_IRQENABLE. The
                                            // SHAMD5_IRQENABLE register contains
                                            // an enable bit for each unique
                                            // interrupt for the public side. An
                                            // interrupt is enabled when both
                                            // the global enable in
                                            // SHAMD5_SYSCONFIG (PIT_en) and the
                                            // bit in this register are both set
                                            // to 1. An interrupt that is
                                            // enabled is propagated to the
                                            // SINTREQUEST_P output. Please note
                                            // that the dedicated partial hash
                                            // output (SINTREQUEST_PART_P) is
                                            // not affected by this register it
                                            // is only affected by the global
                                            // enable SHAMD5_SYSCONFIG (PIT_en).
#define SHAMD5_O_HASH512_ODIGEST_A \
                                0x00000200

#define SHAMD5_O_HASH512_ODIGEST_B \
                                0x00000204

#define SHAMD5_O_HASH512_ODIGEST_C \
                                0x00000208

#define SHAMD5_O_HASH512_ODIGEST_D \
                                0x0000020C

#define SHAMD5_O_HASH512_ODIGEST_E \
                                0x00000210

#define SHAMD5_O_HASH512_ODIGEST_F \
                                0x00000214

#define SHAMD5_O_HASH512_ODIGEST_G \
                                0x00000218

#define SHAMD5_O_HASH512_ODIGEST_H \
                                0x0000021C

#define SHAMD5_O_HASH512_ODIGEST_I \
                                0x00000220

#define SHAMD5_O_HASH512_ODIGEST_J \
                                0x00000224

#define SHAMD5_O_HASH512_ODIGEST_K \
                                0x00000228

#define SHAMD5_O_HASH512_ODIGEST_L \
                                0x0000022C

#define SHAMD5_O_HASH512_ODIGEST_M \
                                0x00000230

#define SHAMD5_O_HASH512_ODIGEST_N \
                                0x00000234

#define SHAMD5_O_HASH512_ODIGEST_O \
                                0x00000238

#define SHAMD5_O_HASH512_ODIGEST_P \
                                0x0000023C

#define SHAMD5_O_HASH512_IDIGEST_A \
                                0x00000240

#define SHAMD5_O_HASH512_IDIGEST_B \
                                0x00000244

#define SHAMD5_O_HASH512_IDIGEST_C \
                                0x00000248

#define SHAMD5_O_HASH512_IDIGEST_D \
                                0x0000024C

#define SHAMD5_O_HASH512_IDIGEST_E \
                                0x00000250

#define SHAMD5_O_HASH512_IDIGEST_F \
                                0x00000254

#define SHAMD5_O_HASH512_IDIGEST_G \
                                0x00000258

#define SHAMD5_O_HASH512_IDIGEST_H \
                                0x0000025C

#define SHAMD5_O_HASH512_IDIGEST_I \
                                0x00000260

#define SHAMD5_O_HASH512_IDIGEST_J \
                                0x00000264

#define SHAMD5_O_HASH512_IDIGEST_K \
                                0x00000268

#define SHAMD5_O_HASH512_IDIGEST_L \
                                0x0000026C

#define SHAMD5_O_HASH512_IDIGEST_M \
                                0x00000270

#define SHAMD5_O_HASH512_IDIGEST_N \
                                0x00000274

#define SHAMD5_O_HASH512_IDIGEST_O \
                                0x00000278

#define SHAMD5_O_HASH512_IDIGEST_P \
                                0x0000027C

#define SHAMD5_O_HASH512_DIGEST_COUNT \
                                0x00000280

#define SHAMD5_O_HASH512_MODE    0x00000284
#define SHAMD5_O_HASH512_LENGTH  0x00000288



//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_ODIGEST_A register.
//
//******************************************************************************
#define SHAMD5_ODIGEST_A_DATA_M  0xFFFFFFFF  // data
#define SHAMD5_ODIGEST_A_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_ODIGEST_B register.
//
//******************************************************************************
#define SHAMD5_ODIGEST_B_DATA_M  0xFFFFFFFF  // data
#define SHAMD5_ODIGEST_B_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_ODIGEST_C register.
//
//******************************************************************************
#define SHAMD5_ODIGEST_C_DATA_M  0xFFFFFFFF  // data
#define SHAMD5_ODIGEST_C_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_ODIGEST_D register.
//
//******************************************************************************
#define SHAMD5_ODIGEST_D_DATA_M  0xFFFFFFFF  // data
#define SHAMD5_ODIGEST_D_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_ODIGEST_E register.
//
//******************************************************************************
#define SHAMD5_ODIGEST_E_DATA_M  0xFFFFFFFF  // data
#define SHAMD5_ODIGEST_E_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_ODIGEST_F register.
//
//******************************************************************************
#define SHAMD5_ODIGEST_F_DATA_M  0xFFFFFFFF  // data
#define SHAMD5_ODIGEST_F_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_ODIGEST_G register.
//
//******************************************************************************
#define SHAMD5_ODIGEST_G_DATA_M  0xFFFFFFFF  // data
#define SHAMD5_ODIGEST_G_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_ODIGEST_H register.
//
//******************************************************************************
#define SHAMD5_ODIGEST_H_DATA_M  0xFFFFFFFF  // data
#define SHAMD5_ODIGEST_H_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_IDIGEST_A register.
//
//******************************************************************************
#define SHAMD5_IDIGEST_A_DATA_M  0xFFFFFFFF  // data
#define SHAMD5_IDIGEST_A_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_IDIGEST_B register.
//
//******************************************************************************
#define SHAMD5_IDIGEST_B_DATA_M  0xFFFFFFFF  // data
#define SHAMD5_IDIGEST_B_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_IDIGEST_C register.
//
//******************************************************************************
#define SHAMD5_IDIGEST_C_DATA_M  0xFFFFFFFF  // data
#define SHAMD5_IDIGEST_C_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_IDIGEST_D register.
//
//******************************************************************************
#define SHAMD5_IDIGEST_D_DATA_M  0xFFFFFFFF  // data
#define SHAMD5_IDIGEST_D_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_IDIGEST_E register.
//
//******************************************************************************
#define SHAMD5_IDIGEST_E_DATA_M  0xFFFFFFFF  // data
#define SHAMD5_IDIGEST_E_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_IDIGEST_F register.
//
//******************************************************************************
#define SHAMD5_IDIGEST_F_DATA_M  0xFFFFFFFF  // data
#define SHAMD5_IDIGEST_F_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_IDIGEST_G register.
//
//******************************************************************************
#define SHAMD5_IDIGEST_G_DATA_M  0xFFFFFFFF  // data
#define SHAMD5_IDIGEST_G_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_IDIGEST_H register.
//
//******************************************************************************
#define SHAMD5_IDIGEST_H_DATA_M  0xFFFFFFFF  // data
#define SHAMD5_IDIGEST_H_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_DIGEST_COUNT register.
//
//******************************************************************************
#define SHAMD5_DIGEST_COUNT_DATA_M \
                                0xFFFFFFFF  // data

#define SHAMD5_DIGEST_COUNT_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_MODE register.
//
//******************************************************************************
#define SHAMD5_MODE_HMAC_OUTER_HASH \
                                0x00000080  // The HMAC Outer Hash is performed
                                            // on the hash digest when the inner
                                            // hash hash finished (block length
                                            // exhausted and final hash
                                            // performed if close_hash is 1).
                                            // This bit should normally be set
                                            // together with close_hash to
                                            // finish the inner hash first or
                                            // Block Length should be zero (HMAC
                                            // continue with the just outer hash
                                            // to be done). Auto cleared
                                            // internally when outer hash
                                            // performed. 0 No operation 1 hmac
                                            // processing

#define SHAMD5_MODE_HMAC_KEY_PROC \
                                0x00000020  // Performs HMAC key processing on
                                            // the 512 bit HMAC key loaded into
                                            // the SHAMD5_IDIGEST_{A to H} and
                                            // SHAMD5_ODIGEST_{A to H} register
                                            // block. Once HMAC key processing
                                            // is finished this bit is
                                            // automatically cleared and the
                                            // resulting Inner and Outer digest
                                            // is available from
                                            // SHAMD5_IDIGEST_{A to H} and
                                            // SHAMD5_ODIGEST_{A to H}
                                            // respectively after which regular
                                            // hash processing (using
                                            // SHAMD5_IDIGEST_{A to H} as initial
                                            // digest) will commence until the
                                            // Block Length is exhausted. 0 No
                                            // operation. 1 Hmac processing.

#define SHAMD5_MODE_CLOSE_HASH   0x00000010  // Performs the padding the
                                            // hash/HMAC will be 'closed' at the
                                            // end of the block as per
                                            // MD5/SHA-1/SHA-2 specification
                                            // (i.e. appropriate padding is
                                            // added) or no padding is done
                                            // allowing the hash to be continued
                                            // later. However if the hash/HMAC
                                            // is not closed then the Block
                                            // Length MUST be a multiple of 64
                                            // bytes to ensure correct
                                            // operation. Auto cleared
                                            // internally when hash closed. 0 No
                                            // padding hash computation can be
                                            // contimued. 1 Last packet will be
                                            // padded.
#define SHAMD5_MODE_ALGO_CONSTANT \
                                0x00000008  // The initial digest register will
                                            // be overwritten with the algorithm
                                            // constants for the selected
                                            // algorithm when hashing and the
                                            // initial digest count register
                                            // will be reset to 0. This will
                                            // start a normal hash operation.
                                            // When continuing an existing hash
                                            // or when performing an HMAC
                                            // operation this register must be
                                            // set to 0 and the
                                            // intermediate/inner digest or HMAC
                                            // key and digest count need to be
                                            // written to the context input
                                            // registers prior to writing
                                            // SHAMD5_MODE. Auto cleared
                                            // internally after first block
                                            // processed. 0 Use pre-calculated
                                            // digest (from an other operation)
                                            // 1 Use constants of the selected
                                            // algo.

#define SHAMD5_MODE_ALGO_M       0x00000006  // These bits select the hash
                                            // algorithm to be used for
                                            // processing: 0x0 md5_128 algorithm
                                            // 0x1 sha1_160 algorithm 0x2
                                            // sha2_224 algorithm 0x3 sha2_256
                                            // algorithm
#define SHAMD5_MODE_ALGO_S       1
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_LENGTH register.
//
//******************************************************************************
#define SHAMD5_LENGTH_DATA_M     0xFFFFFFFF  // data
#define SHAMD5_LENGTH_DATA_S     0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_DATA0_IN register.
//
//******************************************************************************
#define SHAMD5_DATA0_IN_DATA0_IN_M \
                                0xFFFFFFFF  // data

#define SHAMD5_DATA0_IN_DATA0_IN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_DATA1_IN register.
//
//******************************************************************************
#define SHAMD5_DATA1_IN_DATA1_IN_M \
                                0xFFFFFFFF  // data

#define SHAMD5_DATA1_IN_DATA1_IN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_DATA2_IN register.
//
//******************************************************************************
#define SHAMD5_DATA2_IN_DATA2_IN_M \
                                0xFFFFFFFF  // data

#define SHAMD5_DATA2_IN_DATA2_IN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_DATA3_IN register.
//
//******************************************************************************
#define SHAMD5_DATA3_IN_DATA3_IN_M \
                                0xFFFFFFFF  // data

#define SHAMD5_DATA3_IN_DATA3_IN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_DATA4_IN register.
//
//******************************************************************************
#define SHAMD5_DATA4_IN_DATA4_IN_M \
                                0xFFFFFFFF  // data

#define SHAMD5_DATA4_IN_DATA4_IN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_DATA5_IN register.
//
//******************************************************************************
#define SHAMD5_DATA5_IN_DATA5_IN_M \
                                0xFFFFFFFF  // data

#define SHAMD5_DATA5_IN_DATA5_IN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_DATA6_IN register.
//
//******************************************************************************
#define SHAMD5_DATA6_IN_DATA6_IN_M \
                                0xFFFFFFFF  // data

#define SHAMD5_DATA6_IN_DATA6_IN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_DATA7_IN register.
//
//******************************************************************************
#define SHAMD5_DATA7_IN_DATA7_IN_M \
                                0xFFFFFFFF  // data

#define SHAMD5_DATA7_IN_DATA7_IN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_DATA8_IN register.
//
//******************************************************************************
#define SHAMD5_DATA8_IN_DATA8_IN_M \
                                0xFFFFFFFF  // data

#define SHAMD5_DATA8_IN_DATA8_IN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_DATA9_IN register.
//
//******************************************************************************
#define SHAMD5_DATA9_IN_DATA9_IN_M \
                                0xFFFFFFFF  // data

#define SHAMD5_DATA9_IN_DATA9_IN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_DATA10_IN register.
//
//******************************************************************************
#define SHAMD5_DATA10_IN_DATA10_IN_M \
                                0xFFFFFFFF  // data

#define SHAMD5_DATA10_IN_DATA10_IN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_DATA11_IN register.
//
//******************************************************************************
#define SHAMD5_DATA11_IN_DATA11_IN_M \
                                0xFFFFFFFF  // data

#define SHAMD5_DATA11_IN_DATA11_IN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_DATA12_IN register.
//
//******************************************************************************
#define SHAMD5_DATA12_IN_DATA12_IN_M \
                                0xFFFFFFFF  // data

#define SHAMD5_DATA12_IN_DATA12_IN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_DATA13_IN register.
//
//******************************************************************************
#define SHAMD5_DATA13_IN_DATA13_IN_M \
                                0xFFFFFFFF  // data

#define SHAMD5_DATA13_IN_DATA13_IN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_DATA14_IN register.
//
//******************************************************************************
#define SHAMD5_DATA14_IN_DATA14_IN_M \
                                0xFFFFFFFF  // data

#define SHAMD5_DATA14_IN_DATA14_IN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_DATA15_IN register.
//
//******************************************************************************
#define SHAMD5_DATA15_IN_DATA15_IN_M \
                                0xFFFFFFFF  // data

#define SHAMD5_DATA15_IN_DATA15_IN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_REVISION register.
//
//******************************************************************************
#define SHAMD5_REVISION_SCHEME_M 0xC0000000
#define SHAMD5_REVISION_SCHEME_S 30
#define SHAMD5_REVISION_FUNC_M   0x0FFF0000  // Function indicates a software
                                            // compatible module family. If
                                            // there is no level of software
                                            // compatibility a new Func number
                                            // (and hence REVISION) should be
                                            // assigned.
#define SHAMD5_REVISION_FUNC_S   16
#define SHAMD5_REVISION_R_RTL_M  0x0000F800  // RTL Version (R) maintained by IP
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
#define SHAMD5_REVISION_R_RTL_S  11
#define SHAMD5_REVISION_X_MAJOR_M \
                                0x00000700  // Major Revision (X) maintained by
                                            // IP specification owner. X changes
                                            // ONLY when: (1) There is a major
                                            // feature addition. An example
                                            // would be adding Master Mode to
                                            // Utopia Level2. The Func field (or
                                            // Class/Type in old PID format)
                                            // will remain the same. X does NOT
                                            // change due to: (1) Bug fixes (2)
                                            // Change in feature parameters.

#define SHAMD5_REVISION_X_MAJOR_S 8
#define SHAMD5_REVISION_CUSTOM_M 0x000000C0
#define SHAMD5_REVISION_CUSTOM_S 6
#define SHAMD5_REVISION_Y_MINOR_M \
                                0x0000003F  // Minor Revision (Y) maintained by
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

#define SHAMD5_REVISION_Y_MINOR_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_SYSCONFIG register.
//
//******************************************************************************
#define SHAMD5_SYSCONFIG_PADVANCED \
                                0x00000080  // If set to 1 Advanced mode is
                                            // enabled for the Secure World. If
                                            // set to 0 Legacy mode is enabled
                                            // for the Secure World.

#define SHAMD5_SYSCONFIG_PCONT_SWT \
                                0x00000040  // Finish all pending data and
                                            // context DMA input requests (but
                                            // will not assert any new requests)
                                            // finish processing all data in the
                                            // module and provide a saved
                                            // context (partial hash result
                                            // updated digest count remaining
                                            // length updated mode information
                                            // where applicable) for the last
                                            // operation that was interrupted so
                                            // that it can be resumed later.

#define SHAMD5_SYSCONFIG_PDMA_EN 0x00000008
#define SHAMD5_SYSCONFIG_PIT_EN  0x00000004
//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_SYSSTATUS register.
//
//******************************************************************************
#define SHAMD5_SYSSTATUS_RESETDONE \
                                0x00000001  // data

//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_IRQSTATUS register.
//
//******************************************************************************
#define SHAMD5_IRQSTATUS_CONTEXT_READY \
                                0x00000008  // indicates that the secure side
                                            // context input registers are
                                            // available for a new context for
                                            // the next packet to be processed.

#define SHAMD5_IRQSTATUS_PARTHASH_READY \
                                0x00000004  // After a secure side context
                                            // switch request this bit will read
                                            // as 1 indicating that the saved
                                            // context is available from the
                                            // secure side context output
                                            // registers. Note that if the
                                            // context switch request coincides
                                            // with a final hash (when hashing)
                                            // or an outer hash (when doing
                                            // HMAC) that PartHashReady will not
                                            // become active but a regular
                                            // Output Ready will occur instead
                                            // (indicating that the result is
                                            // final and therefore no
                                            // continuation is required).

#define SHAMD5_IRQSTATUS_INPUT_READY \
                                0x00000002  // indicates that the secure side
                                            // data FIFO is ready to receive the
                                            // next 64 byte data block.

#define SHAMD5_IRQSTATUS_OUTPUT_READY \
                                0x00000001  // Indicates that a (partial)
                                            // result or saved context is
                                            // available from the secure side
                                            // context output registers.

//******************************************************************************
//
// The following are defines for the bit fields in the SHAMD5_O_IRQENABLE register.
//
//******************************************************************************
#define SHAMD5_IRQENABLE_M_CONTEXT_READY \
                                0x00000008  // mask for context ready

#define SHAMD5_IRQENABLE_M_PARTHASH_READY \
                                0x00000004  // mask for partial hash

#define SHAMD5_IRQENABLE_M_INPUT_READY \
                                0x00000002  // mask for input_ready

#define SHAMD5_IRQENABLE_M_OUTPUT_READY \
                                0x00000001  // mask for output_ready

//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_ODIGEST_A register.
//
//******************************************************************************
#define SHAMD5_HASH512_ODIGEST_A_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_ODIGEST_A_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_ODIGEST_B register.
//
//******************************************************************************
#define SHAMD5_HASH512_ODIGEST_B_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_ODIGEST_B_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_ODIGEST_C register.
//
//******************************************************************************
#define SHAMD5_HASH512_ODIGEST_C_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_ODIGEST_C_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_ODIGEST_D register.
//
//******************************************************************************
#define SHAMD5_HASH512_ODIGEST_D_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_ODIGEST_D_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_ODIGEST_E register.
//
//******************************************************************************
#define SHAMD5_HASH512_ODIGEST_E_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_ODIGEST_E_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_ODIGEST_F register.
//
//******************************************************************************
#define SHAMD5_HASH512_ODIGEST_F_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_ODIGEST_F_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_ODIGEST_G register.
//
//******************************************************************************
#define SHAMD5_HASH512_ODIGEST_G_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_ODIGEST_G_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_ODIGEST_H register.
//
//******************************************************************************
#define SHAMD5_HASH512_ODIGEST_H_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_ODIGEST_H_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_ODIGEST_I register.
//
//******************************************************************************
#define SHAMD5_HASH512_ODIGEST_I_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_ODIGEST_I_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_ODIGEST_J register.
//
//******************************************************************************
#define SHAMD5_HASH512_ODIGEST_J_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_ODIGEST_J_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_ODIGEST_K register.
//
//******************************************************************************
#define SHAMD5_HASH512_ODIGEST_K_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_ODIGEST_K_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_ODIGEST_L register.
//
//******************************************************************************
#define SHAMD5_HASH512_ODIGEST_L_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_ODIGEST_L_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_ODIGEST_M register.
//
//******************************************************************************
#define SHAMD5_HASH512_ODIGEST_M_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_ODIGEST_M_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_ODIGEST_N register.
//
//******************************************************************************
#define SHAMD5_HASH512_ODIGEST_N_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_ODIGEST_N_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_ODIGEST_O register.
//
//******************************************************************************
#define SHAMD5_HASH512_ODIGEST_O_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_ODIGEST_O_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_ODIGEST_P register.
//
//******************************************************************************
#define SHAMD5_HASH512_ODIGEST_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_ODIGEST_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_IDIGEST_A register.
//
//******************************************************************************
#define SHAMD5_HASH512_IDIGEST_A_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_IDIGEST_A_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_IDIGEST_B register.
//
//******************************************************************************
#define SHAMD5_HASH512_IDIGEST_B_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_IDIGEST_B_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_IDIGEST_C register.
//
//******************************************************************************
#define SHAMD5_HASH512_IDIGEST_C_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_IDIGEST_C_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_IDIGEST_D register.
//
//******************************************************************************
#define SHAMD5_HASH512_IDIGEST_D_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_IDIGEST_D_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_IDIGEST_E register.
//
//******************************************************************************
#define SHAMD5_HASH512_IDIGEST_E_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_IDIGEST_E_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_IDIGEST_F register.
//
//******************************************************************************
#define SHAMD5_HASH512_IDIGEST_F_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_IDIGEST_F_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_IDIGEST_G register.
//
//******************************************************************************
#define SHAMD5_HASH512_IDIGEST_G_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_IDIGEST_G_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_IDIGEST_H register.
//
//******************************************************************************
#define SHAMD5_HASH512_IDIGEST_H_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_IDIGEST_H_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_IDIGEST_I register.
//
//******************************************************************************
#define SHAMD5_HASH512_IDIGEST_I_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_IDIGEST_I_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_IDIGEST_J register.
//
//******************************************************************************
#define SHAMD5_HASH512_IDIGEST_J_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_IDIGEST_J_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_IDIGEST_K register.
//
//******************************************************************************
#define SHAMD5_HASH512_IDIGEST_K_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_IDIGEST_K_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_IDIGEST_L register.
//
//******************************************************************************
#define SHAMD5_HASH512_IDIGEST_L_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_IDIGEST_L_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_IDIGEST_M register.
//
//******************************************************************************
#define SHAMD5_HASH512_IDIGEST_M_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_IDIGEST_M_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_IDIGEST_N register.
//
//******************************************************************************
#define SHAMD5_HASH512_IDIGEST_N_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_IDIGEST_N_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_IDIGEST_O register.
//
//******************************************************************************
#define SHAMD5_HASH512_IDIGEST_O_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_IDIGEST_O_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_IDIGEST_P register.
//
//******************************************************************************
#define SHAMD5_HASH512_IDIGEST_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_IDIGEST_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_DIGEST_COUNT register.
//
//******************************************************************************
#define SHAMD5_HASH512_DIGEST_COUNT_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_DIGEST_COUNT_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_MODE register.
//
//******************************************************************************
#define SHAMD5_HASH512_MODE_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_MODE_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// SHAMD5_O_HASH512_LENGTH register.
//
//******************************************************************************
#define SHAMD5_HASH512_LENGTH_DATA_M \
                                0xFFFFFFFF

#define SHAMD5_HASH512_LENGTH_DATA_S 0



#endif // __HW_SHAMD5_H__

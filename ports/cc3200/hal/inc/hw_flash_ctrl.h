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

#ifndef __HW_FLASH_CTRL_H__
#define __HW_FLASH_CTRL_H__

//*****************************************************************************
//
// The following are defines for the FLASH_CTRL register offsets.
//
//*****************************************************************************
#define FLASH_CTRL_O_FMA        0x00000000  // Flash Memory Address (FMA)
                                            // offset 0x000 During a write
                                            // operation this register contains
                                            // a 4-byte-aligned address and
                                            // specifies where the data is
                                            // written. During erase operations
                                            // this register contains a 1
                                            // KB-aligned CPU byte address and
                                            // specifies which block is erased.
                                            // Note that the alignment
                                            // requirements must be met by
                                            // software or the results of the
                                            // operation are unpredictable.
#define FLASH_CTRL_O_FMD        0x00000004  // Flash Memory Data (FMD) offset
                                            // 0x004 This register contains the
                                            // data to be written during the
                                            // programming cycle or read during
                                            // the read cycle. Note that the
                                            // contents of this register are
                                            // undefined for a read access of an
                                            // execute-only block. This register
                                            // is not used during erase cycles.
#define FLASH_CTRL_O_FMC        0x00000008  // Flash Memory Control (FMC)
                                            // offset 0x008 When this register
                                            // is written the Flash memory
                                            // controller initiates the
                                            // appropriate access cycle for the
                                            // location specified by the Flash
                                            // Memory Address (FMA) register .
                                            // If the access is a write access
                                            // the data contained in the Flash
                                            // Memory Data (FMD) register is
                                            // written to the specified address.
                                            // This register must be the final
                                            // register written and initiates
                                            // the memory operation. The four
                                            // control bits in the lower byte of
                                            // this register are used to
                                            // initiate memory operations.
#define FLASH_CTRL_O_FCRIS      0x0000000C  // Flash Controller Raw Interrupt
                                            // Status (FCRIS) offset 0x00C This
                                            // register indicates that the Flash
                                            // memory controller has an
                                            // interrupt condition. An interrupt
                                            // is sent to the interrupt
                                            // controller only if the
                                            // corresponding FCIM register bit
                                            // is set.
#define FLASH_CTRL_O_FCIM       0x00000010  // Flash Controller Interrupt Mask
                                            // (FCIM) offset 0x010 This register
                                            // controls whether the Flash memory
                                            // controller generates interrupts
                                            // to the controller.
#define FLASH_CTRL_O_FCMISC     0x00000014  // Flash Controller Masked
                                            // Interrupt Status and Clear
                                            // (FCMISC) offset 0x014 This
                                            // register provides two functions.
                                            // First it reports the cause of an
                                            // interrupt by indicating which
                                            // interrupt source or sources are
                                            // signalling the interrupt. Second
                                            // it serves as the method to clear
                                            // the interrupt reporting.
#define FLASH_CTRL_O_FMC2       0x00000020  // Flash Memory Control 2 (FMC2)
                                            // offset 0x020 When this register
                                            // is written the Flash memory
                                            // controller initiates the
                                            // appropriate access cycle for the
                                            // location specified by the Flash
                                            // Memory Address (FMA) register .
                                            // If the access is a write access
                                            // the data contained in the Flash
                                            // Write Buffer (FWB) registers is
                                            // written. This register must be
                                            // the final register written as it
                                            // initiates the memory operation.
#define FLASH_CTRL_O_FWBVAL     0x00000030  // Flash Write Buffer Valid
                                            // (FWBVAL) offset 0x030 This
                                            // register provides a bitwise
                                            // status of which FWBn registers
                                            // have been written by the
                                            // processor since the last write of
                                            // the Flash memory write buffer.
                                            // The entries with a 1 are written
                                            // on the next write of the Flash
                                            // memory write buffer. This
                                            // register is cleared after the
                                            // write operation by hardware. A
                                            // protection violation on the write
                                            // operation also clears this
                                            // status. Software can program the
                                            // same 32 words to various Flash
                                            // memory locations by setting the
                                            // FWB[n] bits after they are
                                            // cleared by the write operation.
                                            // The next write operation then
                                            // uses the same data as the
                                            // previous one. In addition if a
                                            // FWBn register change should not
                                            // be written to Flash memory
                                            // software can clear the
                                            // corresponding FWB[n] bit to
                                            // preserve the existing data when
                                            // the next write operation occurs.
#define FLASH_CTRL_O_FWB1       0x00000100  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB2       0x00000104  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB3       0x00000108  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB4       0x0000010C  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB5       0x00000110  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB6       0x00000114  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB7       0x00000118  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB8       0x0000011C  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB9       0x00000120  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB10      0x00000124  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB11      0x00000128  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB12      0x0000012C  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB13      0x00000130  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB14      0x00000134  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB15      0x00000138  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB16      0x0000013C  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB17      0x00000140  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB18      0x00000144  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB19      0x00000148  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB20      0x0000014C  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB21      0x00000150  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB22      0x00000154  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB23      0x00000158  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB24      0x0000015C  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB25      0x00000160  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB26      0x00000164  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB27      0x00000168  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB28      0x0000016C  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB29      0x00000170  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB30      0x00000174  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB31      0x00000178  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FWB32      0x0000017C  // Flash Write Buffer n (FWBn)
                                            // offset 0x100 - 0x17C These 32
                                            // registers hold the contents of
                                            // the data to be written into the
                                            // Flash memory on a buffered Flash
                                            // memory write operation. The
                                            // offset selects one of the 32-bit
                                            // registers. Only FWBn registers
                                            // that have been updated since the
                                            // preceding buffered Flash memory
                                            // write operation are written into
                                            // the Flash memory so it is not
                                            // necessary to write the entire
                                            // bank of registers in order to
                                            // write 1 or 2 words. The FWBn
                                            // registers are written into the
                                            // Flash memory with the FWB0
                                            // register corresponding to the
                                            // address contained in FMA. FWB1 is
                                            // written to the address FMA+0x4
                                            // etc. Note that only data bits
                                            // that are 0 result in the Flash
                                            // memory being modified. A data bit
                                            // that is 1 leaves the content of
                                            // the Flash memory bit at its
                                            // previous value.
#define FLASH_CTRL_O_FSIZE      0x00000FC0  // Flash Size (FSIZE) offset 0xFC0
                                            // This register indicates the size
                                            // of the on-chip Flash memory.
                                            // Important: This register should
                                            // be used to determine the size of
                                            // the Flash memory that is
                                            // implemented on this
                                            // microcontroller. However to
                                            // support legacy software the DC0
                                            // register is available. A read of
                                            // the DC0 register correctly
                                            // identifies legacy memory sizes.
                                            // Software must use the FSIZE
                                            // register for memory sizes that
                                            // are not listed in the DC0
                                            // register description.
#define FLASH_CTRL_O_SSIZE      0x00000FC4  // SRAM Size (SSIZE) offset 0xFC4
                                            // This register indicates the size
                                            // of the on-chip SRAM. Important:
                                            // This register should be used to
                                            // determine the size of the SRAM
                                            // that is implemented on this
                                            // microcontroller. However to
                                            // support legacy software the DC0
                                            // register is available. A read of
                                            // the DC0 register correctly
                                            // identifies legacy memory sizes.
                                            // Software must use the SSIZE
                                            // register for memory sizes that
                                            // are not listed in the DC0
                                            // register description.



//******************************************************************************
//
// The following are defines for the bit fields in the FLASH_CTRL_O_FMA register.
//
//******************************************************************************
#define FLASH_CTRL_FMA_OFFSET_M 0x0003FFFF  // Address Offset Address offset in
                                            // Flash memory where operation is
                                            // performed except for nonvolatile
                                            // registers
#define FLASH_CTRL_FMA_OFFSET_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the FLASH_CTRL_O_FMD register.
//
//******************************************************************************
#define FLASH_CTRL_FMD_DATA_M   0xFFFFFFFF  // Data Value Data value for write
                                            // operation.
#define FLASH_CTRL_FMD_DATA_S   0
//******************************************************************************
//
// The following are defines for the bit fields in the FLASH_CTRL_O_FMC register.
//
//******************************************************************************
#define FLASH_CTRL_FMC_WRKEY_M  0xFFFF0000  // Flash Memory Write Key This
                                            // field contains a write key which
                                            // is used to minimize the incidence
                                            // of accidental Flash memory
                                            // writes. The value 0xA442 must be
                                            // written into this field for a
                                            // Flash memory write to occur.
                                            // Writes to the FMC register
                                            // without this WRKEY value are
                                            // ignored. A read of this field
                                            // returns the value 0.
#define FLASH_CTRL_FMC_WRKEY_S  16
#define FLASH_CTRL_FMC_COMT     0x00000008  // Commit Register Value This bit
                                            // is used to commit writes to
                                            // Flash-memory-resident registers
                                            // and to monitor the progress of
                                            // that process. Value Description 1
                                            // Set this bit to commit (write)
                                            // the register value to a
                                            // Flash-memory-resident register.
                                            // When read a 1 indicates that the
                                            // previous commit access is not
                                            // complete. 0 A write of 0 has no
                                            // effect on the state of this bit.
                                            // When read a 0 indicates that the
                                            // previous commit access is
                                            // complete.
#define FLASH_CTRL_FMC_MERASE1  0x00000004  // Mass Erase Flash Memory This bit
                                            // is used to mass erase the Flash
                                            // main memory and to monitor the
                                            // progress of that process. Value
                                            // Description 1 Set this bit to
                                            // erase the Flash main memory. When
                                            // read a 1 indicates that the
                                            // previous mass erase access is not
                                            // complete. 0 A write of 0 has no
                                            // effect on the state of this bit.
                                            // When read a 0 indicates that the
                                            // previous mass erase access is
                                            // complete.
#define FLASH_CTRL_FMC_ERASE    0x00000002  // Erase a Page of Flash Memory
                                            // This bit is used to erase a page
                                            // of Flash memory and to monitor
                                            // the progress of that process.
                                            // Value Description 1 Set this bit
                                            // to erase the Flash memory page
                                            // specified by the contents of the
                                            // FMA register. When read a 1
                                            // indicates that the previous page
                                            // erase access is not complete. 0 A
                                            // write of 0 has no effect on the
                                            // state of this bit. When read a 0
                                            // indicates that the previous page
                                            // erase access is complete.
#define FLASH_CTRL_FMC_WRITE    0x00000001  // Write a Word into Flash Memory
                                            // This bit is used to write a word
                                            // into Flash memory and to monitor
                                            // the progress of that process.
                                            // Value Description 1 Set this bit
                                            // to write the data stored in the
                                            // FMD register into the Flash
                                            // memory location specified by the
                                            // contents of the FMA register.
                                            // When read a 1 indicates that the
                                            // write update access is not
                                            // complete. 0 A write of 0 has no
                                            // effect on the state of this bit.
                                            // When read a 0 indicates that the
                                            // previous write update access is
                                            // complete.
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FCRIS register.
//
//******************************************************************************
#define FLASH_CTRL_FCRIS_PROGRIS \
                                0x00002000  // Program Verify Error Raw
                                            // Interrupt Status Value
                                            // Description 1 An interrupt is
                                            // pending because the verify of a
                                            // PROGRAM operation failed. 0 An
                                            // interrupt has not occurred. This
                                            // bit is cleared by writing a 1 to
                                            // the PROGMISC bit in the FCMISC
                                            // register.

#define FLASH_CTRL_FCRIS_ERRIS  0x00000800  // Erase Verify Error Raw Interrupt
                                            // Status Value Description 1 An
                                            // interrupt is pending because the
                                            // verify of an ERASE operation
                                            // failed. 0 An interrupt has not
                                            // occurred. This bit is cleared by
                                            // writing a 1 to the ERMISC bit in
                                            // the FCMISC register.
#define FLASH_CTRL_FCRIS_INVDRIS \
                                0x00000400  // Invalid Data Raw Interrupt
                                            // Status Value Description 1 An
                                            // interrupt is pending because a
                                            // bit that was previously
                                            // programmed as a 0 is now being
                                            // requested to be programmed as a
                                            // 1. 0 An interrupt has not
                                            // occurred. This bit is cleared by
                                            // writing a 1 to the INVMISC bit in
                                            // the FCMISC register.

#define FLASH_CTRL_FCRIS_VOLTRIS \
                                0x00000200  // Pump Voltage Raw Interrupt
                                            // Status Value Description 1 An
                                            // interrupt is pending because the
                                            // regulated voltage of the pump
                                            // went out of spec during the Flash
                                            // operation and the operation was
                                            // terminated. 0 An interrupt has
                                            // not occurred. This bit is cleared
                                            // by writing a 1 to the VOLTMISC
                                            // bit in the FCMISC register.

#define FLASH_CTRL_FCRIS_ERIS   0x00000004  // EEPROM Raw Interrupt Status This
                                            // bit provides status EEPROM
                                            // operation. Value Description 1 An
                                            // EEPROM interrupt has occurred. 0
                                            // An EEPROM interrupt has not
                                            // occurred. This bit is cleared by
                                            // writing a 1 to the EMISC bit in
                                            // the FCMISC register.
#define FLASH_CTRL_FCRIS_PRIS   0x00000002  // Programming Raw Interrupt Status
                                            // This bit provides status on
                                            // programming cycles which are
                                            // write or erase actions generated
                                            // through the FMC or FMC2 register
                                            // bits (see page 537 and page 549).
                                            // Value Description 1 The
                                            // programming or erase cycle has
                                            // completed. 0 The programming or
                                            // erase cycle has not completed.
                                            // This status is sent to the
                                            // interrupt controller when the
                                            // PMASK bit in the FCIM register is
                                            // set. This bit is cleared by
                                            // writing a 1 to the PMISC bit in
                                            // the FCMISC register.
#define FLASH_CTRL_FCRIS_ARIS   0x00000001  // Access Raw Interrupt Status
                                            // Value Description 1 A program or
                                            // erase action was attempted on a
                                            // block of Flash memory that
                                            // contradicts the protection policy
                                            // for that block as set in the
                                            // FMPPEn registers. 0 No access has
                                            // tried to improperly program or
                                            // erase the Flash memory. This
                                            // status is sent to the interrupt
                                            // controller when the AMASK bit in
                                            // the FCIM register is set. This
                                            // bit is cleared by writing a 1 to
                                            // the AMISC bit in the FCMISC
                                            // register.
//******************************************************************************
//
// The following are defines for the bit fields in the FLASH_CTRL_O_FCIM register.
//
//******************************************************************************
#define FLASH_CTRL_FCIM_ILLMASK 0x00004000  // Illegal Address Interrupt Mask
                                            // Value Description 1 An interrupt
                                            // is sent to the interrupt
                                            // controller when the ILLARIS bit
                                            // is set. 0 The ILLARIS interrupt
                                            // is suppressed and not sent to the
                                            // interrupt controller.
#define FLASH_CTRL_FCIM_PROGMASK \
                                0x00002000  // PROGVER Interrupt Mask Value
                                            // Description 1 An interrupt is
                                            // sent to the interrupt controller
                                            // when the PROGRIS bit is set. 0
                                            // The PROGRIS interrupt is
                                            // suppressed and not sent to the
                                            // interrupt controller.

#define FLASH_CTRL_FCIM_PREMASK 0x00001000  // PREVER Interrupt Mask Value
                                            // Description 1 An interrupt is
                                            // sent to the interrupt controller
                                            // when the PRERIS bit is set. 0 The
                                            // PRERIS interrupt is suppressed
                                            // and not sent to the interrupt
                                            // controller.
#define FLASH_CTRL_FCIM_ERMASK  0x00000800  // ERVER Interrupt Mask Value
                                            // Description 1 An interrupt is
                                            // sent to the interrupt controller
                                            // when the ERRIS bit is set. 0 The
                                            // ERRIS interrupt is suppressed and
                                            // not sent to the interrupt
                                            // controller.
#define FLASH_CTRL_FCIM_INVDMASK \
                                0x00000400  // Invalid Data Interrupt Mask
                                            // Value Description 1 An interrupt
                                            // is sent to the interrupt
                                            // controller when the INVDRIS bit
                                            // is set. 0 The INVDRIS interrupt
                                            // is suppressed and not sent to the
                                            // interrupt controller.

#define FLASH_CTRL_FCIM_VOLTMASK \
                                0x00000200  // VOLT Interrupt Mask Value
                                            // Description 1 An interrupt is
                                            // sent to the interrupt controller
                                            // when the VOLTRIS bit is set. 0
                                            // The VOLTRIS interrupt is
                                            // suppressed and not sent to the
                                            // interrupt controller.

#define FLASH_CTRL_FCIM_LOCKMASK \
                                0x00000100  // LOCK Interrupt Mask Value
                                            // Description 1 An interrupt is
                                            // sent to the interrupt controller
                                            // when the LOCKRIS bit is set. 0
                                            // The LOCKRIS interrupt is
                                            // suppressed and not sent to the
                                            // interrupt controller.

#define FLASH_CTRL_FCIM_EMASK   0x00000004  // EEPROM Interrupt Mask Value
                                            // Description 1 An interrupt is
                                            // sent to the interrupt controller
                                            // when the ERIS bit is set. 0 The
                                            // ERIS interrupt is suppressed and
                                            // not sent to the interrupt
                                            // controller.
#define FLASH_CTRL_FCIM_PMASK   0x00000002  // Programming Interrupt Mask This
                                            // bit controls the reporting of the
                                            // programming raw interrupt status
                                            // to the interrupt controller.
                                            // Value Description 1 An interrupt
                                            // is sent to the interrupt
                                            // controller when the PRIS bit is
                                            // set. 0 The PRIS interrupt is
                                            // suppressed and not sent to the
                                            // interrupt controller.
#define FLASH_CTRL_FCIM_AMASK   0x00000001  // Access Interrupt Mask This bit
                                            // controls the reporting of the
                                            // access raw interrupt status to
                                            // the interrupt controller. Value
                                            // Description 1 An interrupt is
                                            // sent to the interrupt controller
                                            // when the ARIS bit is set. 0 The
                                            // ARIS interrupt is suppressed and
                                            // not sent to the interrupt
                                            // controller.
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FCMISC register.
//
//******************************************************************************
#define FLASH_CTRL_FCMISC_ILLMISC \
                                0x00004000  // Illegal Address Masked Interrupt
                                            // Status and Clear Value
                                            // Description 1 When read a 1
                                            // indicates that an unmasked
                                            // interrupt was signaled. Writing a
                                            // 1 to this bit clears ILLAMISC and
                                            // also the ILLARIS bit in the FCRIS
                                            // register (see page 540). 0 When
                                            // read a 0 indicates that an
                                            // interrupt has not occurred. A
                                            // write of 0 has no effect on the
                                            // state of this bit.

#define FLASH_CTRL_FCMISC_PROGMISC \
                                0x00002000  // PROGVER Masked Interrupt Status
                                            // and Clear Value Description 1
                                            // When read a 1 indicates that an
                                            // unmasked interrupt was signaled.
                                            // Writing a 1 to this bit clears
                                            // PROGMISC and also the PROGRIS bit
                                            // in the FCRIS register (see page
                                            // 540). 0 When read a 0 indicates
                                            // that an interrupt has not
                                            // occurred. A write of 0 has no
                                            // effect on the state of this bit.

#define FLASH_CTRL_FCMISC_PREMISC \
                                0x00001000  // PREVER Masked Interrupt Status
                                            // and Clear Value Description 1
                                            // When read a 1 indicates that an
                                            // unmasked interrupt was signaled.
                                            // Writing a 1 to this bit clears
                                            // PREMISC and also the PRERIS bit
                                            // in the FCRIS register . 0 When
                                            // read a 0 indicates that an
                                            // interrupt has not occurred. A
                                            // write of 0 has no effect on the
                                            // state of this bit.

#define FLASH_CTRL_FCMISC_ERMISC \
                                0x00000800  // ERVER Masked Interrupt Status
                                            // and Clear Value Description 1
                                            // When read a 1 indicates that an
                                            // unmasked interrupt was signaled.
                                            // Writing a 1 to this bit clears
                                            // ERMISC and also the ERRIS bit in
                                            // the FCRIS register 0 When read a
                                            // 0 indicates that an interrupt has
                                            // not occurred. A write of 0 has no
                                            // effect on the state of this bit.

#define FLASH_CTRL_FCMISC_INVDMISC \
                                0x00000400  // Invalid Data Masked Interrupt
                                            // Status and Clear Value
                                            // Description 1 When read a 1
                                            // indicates that an unmasked
                                            // interrupt was signaled. Writing a
                                            // 1 to this bit clears INVDMISC and
                                            // also the INVDRIS bit in the FCRIS
                                            // register (see page 540). 0 When
                                            // read a 0 indicates that an
                                            // interrupt has not occurred. A
                                            // write of 0 has no effect on the
                                            // state of this bit.

#define FLASH_CTRL_FCMISC_VOLTMISC \
                                0x00000200  // VOLT Masked Interrupt Status and
                                            // Clear Value Description 1 When
                                            // read a 1 indicates that an
                                            // unmasked interrupt was signaled.
                                            // Writing a 1 to this bit clears
                                            // VOLTMISC and also the VOLTRIS bit
                                            // in the FCRIS register (see page
                                            // 540). 0 When read a 0 indicates
                                            // that an interrupt has not
                                            // occurred. A write of 0 has no
                                            // effect on the state of this bit.

#define FLASH_CTRL_FCMISC_LOCKMISC \
                                0x00000100  // LOCK Masked Interrupt Status and
                                            // Clear Value Description 1 When
                                            // read a 1 indicates that an
                                            // unmasked interrupt was signaled.
                                            // Writing a 1 to this bit clears
                                            // LOCKMISC and also the LOCKRIS bit
                                            // in the FCRIS register (see page
                                            // 540). 0 When read a 0 indicates
                                            // that an interrupt has not
                                            // occurred. A write of 0 has no
                                            // effect on the state of this bit.

#define FLASH_CTRL_FCMISC_EMISC 0x00000004  // EEPROM Masked Interrupt Status
                                            // and Clear Value Description 1
                                            // When read a 1 indicates that an
                                            // unmasked interrupt was signaled.
                                            // Writing a 1 to this bit clears
                                            // EMISC and also the ERIS bit in
                                            // the FCRIS register 0 When read a
                                            // 0 indicates that an interrupt has
                                            // not occurred. A write of 0 has no
                                            // effect on the state of this bit.
#define FLASH_CTRL_FCMISC_PMISC 0x00000002  // Programming Masked Interrupt
                                            // Status and Clear Value
                                            // Description 1 When read a 1
                                            // indicates that an unmasked
                                            // interrupt was signaled because a
                                            // programming cycle completed.
                                            // Writing a 1 to this bit clears
                                            // PMISC and also the PRIS bit in
                                            // the FCRIS register 0 When read a
                                            // 0 indicates that a programming
                                            // cycle complete interrupt has not
                                            // occurred. A write of 0 has no
                                            // effect on the state of this bit.
#define FLASH_CTRL_FCMISC_AMISC 0x00000001  // Access Masked Interrupt Status
                                            // and Clear Value Description 1
                                            // When read a 1 indicates that an
                                            // unmasked interrupt was signaled
                                            // because a program or erase action
                                            // was attempted on a block of Flash
                                            // memory that contradicts the
                                            // protection policy for that block
                                            // as set in the FMPPEn registers.
                                            // Writing a 1 to this bit clears
                                            // AMISC and also the ARIS bit in
                                            // the FCRIS register 0 When read a
                                            // 0 indicates that no improper
                                            // accesses have occurred. A write
                                            // of 0 has no effect on the state
                                            // of this bit.
//******************************************************************************
//
// The following are defines for the bit fields in the FLASH_CTRL_O_FMC2 register.
//
//******************************************************************************
#define FLASH_CTRL_FMC2_WRKEY_M 0xFFFF0000  // Flash Memory Write Key This
                                            // field contains a write key which
                                            // is used to minimize the incidence
                                            // of accidental Flash memory
                                            // writes. The value 0xA442 must be
                                            // written into this field for a
                                            // write to occur. Writes to the
                                            // FMC2 register without this WRKEY
                                            // value are ignored. A read of this
                                            // field returns the value 0.
#define FLASH_CTRL_FMC2_WRKEY_S 16
#define FLASH_CTRL_FMC2_WRBUF   0x00000001  // Buffered Flash Memory Write This
                                            // bit is used to start a buffered
                                            // write to Flash memory. Value
                                            // Description 1 Set this bit to
                                            // write the data stored in the FWBn
                                            // registers to the location
                                            // specified by the contents of the
                                            // FMA register. When read a 1
                                            // indicates that the previous
                                            // buffered Flash memory write
                                            // access is not complete. 0 A write
                                            // of 0 has no effect on the state
                                            // of this bit. When read a 0
                                            // indicates that the previous
                                            // buffered Flash memory write
                                            // access is complete.
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWBVAL register.
//
//******************************************************************************
#define FLASH_CTRL_FWBVAL_FWBN_M \
                                0xFFFFFFFF  // Flash Memory Write Buffer Value
                                            // Description 1 The corresponding
                                            // FWBn register has been updated
                                            // since the last buffer write
                                            // operation and is ready to be
                                            // written to Flash memory. 0 The
                                            // corresponding FWBn register has
                                            // no new data to be written. Bit 0
                                            // corresponds to FWB0 offset 0x100
                                            // and bit 31 corresponds to FWB31
                                            // offset 0x13C.

#define FLASH_CTRL_FWBVAL_FWBN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the FLASH_CTRL_O_FWB1 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB1_DATA_M  0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB1_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the FLASH_CTRL_O_FWB2 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB2_DATA_M  0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB2_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the FLASH_CTRL_O_FWB3 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB3_DATA_M  0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB3_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the FLASH_CTRL_O_FWB4 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB4_DATA_M  0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB4_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the FLASH_CTRL_O_FWB5 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB5_DATA_M  0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB5_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the FLASH_CTRL_O_FWB6 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB6_DATA_M  0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB6_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the FLASH_CTRL_O_FWB7 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB7_DATA_M  0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB7_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the FLASH_CTRL_O_FWB8 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB8_DATA_M  0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB8_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the FLASH_CTRL_O_FWB9 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB9_DATA_M  0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB9_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB10 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB10_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB10_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB11 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB11_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB11_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB12 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB12_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB12_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB13 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB13_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB13_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB14 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB14_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB14_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB15 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB15_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB15_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB16 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB16_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB16_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB17 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB17_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB17_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB18 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB18_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB18_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB19 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB19_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB19_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB20 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB20_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB20_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB21 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB21_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB21_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB22 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB22_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB22_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB23 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB23_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB23_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB24 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB24_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB24_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB25 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB25_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB25_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB26 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB26_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB26_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB27 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB27_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB27_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB28 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB28_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB28_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB29 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB29_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB29_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB30 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB30_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB30_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB31 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB31_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB31_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FWB32 register.
//
//******************************************************************************
#define FLASH_CTRL_FWB32_DATA_M 0xFFFFFFFF  // Data Data to be written into the
                                            // Flash memory.
#define FLASH_CTRL_FWB32_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_FSIZE register.
//
//******************************************************************************
#define FLASH_CTRL_FSIZE_SIZE_M 0x0000FFFF  // Flash Size Indicates the size of
                                            // the on-chip Flash memory. Value
                                            // Description 0x0003 8 KB of Flash
                                            // 0x0007 16 KB of Flash 0x000F 32
                                            // KB of Flash 0x001F 64 KB of Flash
                                            // 0x002F 96 KB of Flash 0x003F 128
                                            // KB of Flash 0x005F 192 KB of
                                            // Flash 0x007F 256 KB of Flash
#define FLASH_CTRL_FSIZE_SIZE_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// FLASH_CTRL_O_SSIZE register.
//
//******************************************************************************
#define FLASH_CTRL_SSIZE_SRAM_SIZE_M \
                                0x0000FFFF  // SRAM Size Indicates the size of
                                            // the on-chip SRAM. Value
                                            // Description 0x0007 2 KB of SRAM
                                            // 0x000F 4 KB of SRAM 0x0017 6 KB
                                            // of SRAM 0x001F 8 KB of SRAM
                                            // 0x002F 12 KB of SRAM 0x003F 16 KB
                                            // of SRAM 0x004F 20 KB of SRAM
                                            // 0x005F 24 KB of SRAM 0x007F 32 KB
                                            // of SRAM

#define FLASH_CTRL_SSIZE_SRAM_SIZE_S 0
#define FLASH_CTRL_FMC_WRKEY    0xA4420000  // FLASH write key
#define FLASH_CTRL_FMC2_WRKEY   0xA4420000  // FLASH write key
#define FLASH_CTRL_O_FWBN       FLASH_CTRL_O_FWB1
#define FLASH_ERASE_SIZE        0x00000400
#define FLASH_PROTECT_SIZE      0x00000800
#define FLASH_FMP_BLOCK_0       0x00000001  // Enable for block 0

#define FLASH_FMPRE0            0x400FE200  // Flash Memory Protection Read
                                            // Enable 0
#define FLASH_FMPRE1            0x400FE204  // Flash Memory Protection Read
                                            // Enable 1
#define FLASH_FMPRE2            0x400FE208  // Flash Memory Protection Read
                                            // Enable 2
#define FLASH_FMPRE3            0x400FE20C  // Flash Memory Protection Read
                                            // Enable 3
#define FLASH_FMPRE4            0x400FE210  // Flash Memory Protection Read
                                            // Enable 4
#define FLASH_FMPRE5            0x400FE214  // Flash Memory Protection Read
                                            // Enable 5
#define FLASH_FMPRE6            0x400FE218  // Flash Memory Protection Read
                                            // Enable 6
#define FLASH_FMPRE7            0x400FE21C  // Flash Memory Protection Read
                                            // Enable 7
#define FLASH_FMPRE8            0x400FE220  // Flash Memory Protection Read
                                            // Enable 8
#define FLASH_FMPRE9            0x400FE224  // Flash Memory Protection Read
                                            // Enable 9
#define FLASH_FMPRE10           0x400FE228  // Flash Memory Protection Read
                                            // Enable 10
#define FLASH_FMPRE11           0x400FE22C  // Flash Memory Protection Read
                                            // Enable 11
#define FLASH_FMPRE12           0x400FE230  // Flash Memory Protection Read
                                            // Enable 12
#define FLASH_FMPRE13           0x400FE234  // Flash Memory Protection Read
                                            // Enable 13
#define FLASH_FMPRE14           0x400FE238  // Flash Memory Protection Read
                                            // Enable 14
#define FLASH_FMPRE15           0x400FE23C  // Flash Memory Protection Read
                                            // Enable 15

#define FLASH_FMPPE0            0x400FE400  // Flash Memory Protection Program
                                            // Enable 0
#define FLASH_FMPPE1            0x400FE404  // Flash Memory Protection Program
                                            // Enable 1
#define FLASH_FMPPE2            0x400FE408  // Flash Memory Protection Program
                                            // Enable 2
#define FLASH_FMPPE3            0x400FE40C  // Flash Memory Protection Program
                                            // Enable 3
#define FLASH_FMPPE4            0x400FE410  // Flash Memory Protection Program
                                            // Enable 4
#define FLASH_FMPPE5            0x400FE414  // Flash Memory Protection Program
                                            // Enable 5
#define FLASH_FMPPE6            0x400FE418  // Flash Memory Protection Program
                                            // Enable 6
#define FLASH_FMPPE7            0x400FE41C  // Flash Memory Protection Program
                                            // Enable 7
#define FLASH_FMPPE8            0x400FE420  // Flash Memory Protection Program
                                            // Enable 8
#define FLASH_FMPPE9            0x400FE424  // Flash Memory Protection Program
                                            // Enable 9
#define FLASH_FMPPE10           0x400FE428  // Flash Memory Protection Program
                                            // Enable 10
#define FLASH_FMPPE11           0x400FE42C  // Flash Memory Protection Program
                                            // Enable 11
#define FLASH_FMPPE12           0x400FE430  // Flash Memory Protection Program
                                            // Enable 12
#define FLASH_FMPPE13           0x400FE434  // Flash Memory Protection Program
                                            // Enable 13
#define FLASH_FMPPE14           0x400FE438  // Flash Memory Protection Program
                                            // Enable 14
#define FLASH_FMPPE15           0x400FE43C  // Flash Memory Protection Program
                                            // Enable 15

#define FLASH_USECRL            0x400FE140  // USec Reload
#define FLASH_CTRL_ERASE_SIZE   0x00000400


#endif // __HW_FLASH_CTRL_H__

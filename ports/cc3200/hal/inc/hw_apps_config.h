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


#ifndef __HW_APPS_CONFIG_H__
#define __HW_APPS_CONFIG_H__

//*****************************************************************************
//
// The following are defines for the APPS_CONFIG register offsets.
//
//*****************************************************************************
#define APPS_CONFIG_O_PATCH_TRAP_ADDR_REG \
                                0x00000000  // Patch trap address Register
                                            // array

#define APPS_CONFIG_O_PATCH_TRAP_EN_REG \
                                0x00000078

#define APPS_CONFIG_O_FAULT_STATUS_REG \
                                0x0000007C

#define APPS_CONFIG_O_MEMSS_WR_ERR_CLR_REG \
                                0x00000080

#define APPS_CONFIG_O_MEMSS_WR_ERR_ADDR_REG \
                                0x00000084

#define APPS_CONFIG_O_DMA_DONE_INT_MASK \
                                0x0000008C

#define APPS_CONFIG_O_DMA_DONE_INT_MASK_SET \
                                0x00000090

#define APPS_CONFIG_O_DMA_DONE_INT_MASK_CLR \
                                0x00000094

#define APPS_CONFIG_O_DMA_DONE_INT_STS_CLR \
                                0x00000098

#define APPS_CONFIG_O_DMA_DONE_INT_ACK \
                                0x0000009C

#define APPS_CONFIG_O_DMA_DONE_INT_STS_MASKED \
                                0x000000A0

#define APPS_CONFIG_O_DMA_DONE_INT_STS_RAW \
                                0x000000A4

#define APPS_CONFIG_O_FAULT_STATUS_CLR_REG \
                                0x000000A8

#define APPS_CONFIG_O_RESERVD_REG_0 \
                                0x000000AC

#define APPS_CONFIG_O_GPT_TRIG_SEL \
                                0x000000B0

#define APPS_CONFIG_O_TOP_DIE_SPARE_DIN_REG \
                                0x000000B4

#define APPS_CONFIG_O_TOP_DIE_SPARE_DOUT_REG \
                                0x000000B8




//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_CONFIG_O_PATCH_TRAP_ADDR_REG register.
//
//******************************************************************************
#define APPS_CONFIG_PATCH_TRAP_ADDR_REG_PATCH_TRAP_ADDR_M \
                                0xFFFFFFFF  // When PATCH_TRAP_EN[n] is set bus
                                            // fault is generated for the
                                            // address
                                            // PATCH_TRAP_ADDR_REG[n][31:0] from
                                            // Idcode bus. The exception routine
                                            // should take care to jump to the
                                            // location where the patch
                                            // correspond to this address is
                                            // kept.

#define APPS_CONFIG_PATCH_TRAP_ADDR_REG_PATCH_TRAP_ADDR_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_CONFIG_O_PATCH_TRAP_EN_REG register.
//
//******************************************************************************
#define APPS_CONFIG_PATCH_TRAP_EN_REG_PATCH_TRAP_EN_M \
                                0x3FFFFFFF  // When PATCH_TRAP_EN[n] is set bus
                                            // fault is generated for the
                                            // address PATCH_TRAP_ADD[n][31:0]
                                            // from Idcode bus. The exception
                                            // routine should take care to jump
                                            // to the location where the patch
                                            // correspond to this address is
                                            // kept.

#define APPS_CONFIG_PATCH_TRAP_EN_REG_PATCH_TRAP_EN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_CONFIG_O_FAULT_STATUS_REG register.
//
//******************************************************************************
#define APPS_CONFIG_FAULT_STATUS_REG_PATCH_ERR_INDEX_M \
                                0x0000003E  // This field shows because of
                                            // which patch trap address the
                                            // bus_fault is generated. If the
                                            // PATCH_ERR bit is set, then it
                                            // means the bus fault is generated
                                            // because of
                                            // PATCH_TRAP_ADDR_REG[2^PATCH_ERR_INDEX]

#define APPS_CONFIG_FAULT_STATUS_REG_PATCH_ERR_INDEX_S 1
#define APPS_CONFIG_FAULT_STATUS_REG_PATCH_ERR \
                                0x00000001  // This bit is set when there is a
                                            // bus fault because of patched
                                            // address access to the Apps boot
                                            // rom. Write 0 to clear this
                                            // register.

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_CONFIG_O_MEMSS_WR_ERR_CLR_REG register.
//
//******************************************************************************
#define APPS_CONFIG_MEMSS_WR_ERR_CLR_REG_MEMSS_WR_ERR_CLR \
                                0x00000001  // This bit is set when there is a
                                            // an error in memss write access.
                                            // And the address causing this
                                            // error is captured in
                                            // MEMSS_ERR_ADDR_REG. To capture
                                            // the next error address one have
                                            // to clear this bit.

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_CONFIG_O_MEMSS_WR_ERR_ADDR_REG register.
//
//******************************************************************************
//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_CONFIG_O_DMA_DONE_INT_MASK register.
//
//******************************************************************************
#define APPS_CONFIG_DMA_DONE_INT_MASK_ADC_WR_DMA_DONE_INT_MASK_M \
                                0x0000F000  // 1= disable corresponding
                                            // interrupt;0 = interrupt enabled
                                            // bit 14: ADC channel 7 interrupt
                                            // enable/disable bit 13: ADC
                                            // channel 5 interrupt
                                            // enable/disable bit 12: ADC
                                            // channel 3 interrupt
                                            // enable/disable bit 11: ADC
                                            // channel 1 interrupt
                                            // enable/disable

#define APPS_CONFIG_DMA_DONE_INT_MASK_ADC_WR_DMA_DONE_INT_MASK_S 12
#define APPS_CONFIG_DMA_DONE_INT_MASK_MCASP_WR_DMA_DONE_INT_MASK \
                                0x00000800  // 1= disable corresponding
                                            // interrupt;0 = interrupt enabled

#define APPS_CONFIG_DMA_DONE_INT_MASK_MCASP_RD_DMA_DONE_INT_MASK \
                                0x00000400  // 1= disable corresponding
                                            // interrupt;0 = interrupt enabled

#define APPS_CONFIG_DMA_DONE_INT_MASK_CAM_FIFO_EMPTY_DMA_DONE_INT_MASK \
                                0x00000200  // 1= disable corresponding
                                            // interrupt;0 = interrupt enabled

#define APPS_CONFIG_DMA_DONE_INT_MASK_CAM_THRESHHOLD_DMA_DONE_INT_MASK \
                                0x00000100  // 1= disable corresponding
                                            // interrupt;0 = interrupt enabled

#define APPS_CONFIG_DMA_DONE_INT_MASK_SHSPI_WR_DMA_DONE_INT_MASK \
                                0x00000080  // 1= disable corresponding
                                            // interrupt;0 = interrupt enabled

#define APPS_CONFIG_DMA_DONE_INT_MASK_SHSPI_RD_DMA_DONE_INT_MASK \
                                0x00000040  // 1= disable corresponding
                                            // interrupt;0 = interrupt enabled

#define APPS_CONFIG_DMA_DONE_INT_MASK_HOSTSPI_WR_DMA_DONE_INT_MASK \
                                0x00000020  // 1= disable corresponding
                                            // interrupt;0 = interrupt enabled

#define APPS_CONFIG_DMA_DONE_INT_MASK_HOSTSPI_RD_DMA_DONE_INT_MASK \
                                0x00000010  // 1= disable corresponding
                                            // interrupt;0 = interrupt enabled

#define APPS_CONFIG_DMA_DONE_INT_MASK_APPS_SPI_WR_DMA_DONE_INT_MASK \
                                0x00000008  // 1= disable corresponding
                                            // interrupt;0 = interrupt enabled

#define APPS_CONFIG_DMA_DONE_INT_MASK_APPS_SPI_RD_DMA_DONE_INT_MASK \
                                0x00000004  // 1= disable corresponding
                                            // interrupt;0 = interrupt enabled

#define APPS_CONFIG_DMA_DONE_INT_MASK_SDIOM_WR_DMA_DONE_INT_MASK \
                                0x00000002  // 1= disable corresponding
                                            // interrupt;0 = interrupt enabled

#define APPS_CONFIG_DMA_DONE_INT_MASK_SDIOM_RD_DMA_DONE_INT_MASK \
                                0x00000001  // 1= disable corresponding
                                            // interrupt;0 = interrupt enabled

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_CONFIG_O_DMA_DONE_INT_MASK_SET register.
//
//******************************************************************************
#define APPS_CONFIG_DMA_DONE_INT_MASK_SET_ADC_WR_DMA_DONE_INT_MASK_SET_M \
                                0x0000F000  // write 1 to set mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect bit 14: ADC channel 7 DMA
                                            // Done IRQ bit 13: ADC channel 5
                                            // DMA Done IRQ bit 12: ADC channel
                                            // 3 DMA Done IRQ bit 11: ADC
                                            // channel 1 DMA Done IRQ

#define APPS_CONFIG_DMA_DONE_INT_MASK_SET_ADC_WR_DMA_DONE_INT_MASK_SET_S 12
#define APPS_CONFIG_DMA_DONE_INT_MASK_SET_MCASP_WR_DMA_DONE_INT_MASK_SET \
                                0x00000800  // write 1 to set mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_SET_MCASP_RD_DMA_DONE_INT_MASK_SET \
                                0x00000400  // write 1 to set mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_SET_CAM_FIFO_EMPTY_DMA_DONE_INT_MASK_SET \
                                0x00000200  // write 1 to set mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_SET_CAM_THRESHHOLD_DMA_DONE_INT_MASK_SET \
                                0x00000100  // write 1 to set mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_SET_SHSPI_WR_DMA_DONE_INT_MASK_SET \
                                0x00000080  // write 1 to set mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_SET_SHSPI_RD_DMA_DONE_INT_MASK_SET \
                                0x00000040  // write 1 to set mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_SET_HOSTSPI_WR_DMA_DONE_INT_MASK_SET \
                                0x00000020  // write 1 to set mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_SET_HOSTSPI_RD_DMA_DONE_INT_MASK_SET \
                                0x00000010  // write 1 to set mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_SET_APPS_SPI_WR_DMA_DONE_INT_MASK_SET \
                                0x00000008  // write 1 to set mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_SET_APPS_SPI_RD_DMA_DONE_INT_MASK_SET \
                                0x00000004  // write 1 to set mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_SET_SDIOM_WR_DMA_DONE_INT_MASK_SET \
                                0x00000002  // write 1 to set mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_SET_SDIOM_RD_DMA_DONE_INT_MASK_SET \
                                0x00000001  // write 1 to set mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_CONFIG_O_DMA_DONE_INT_MASK_CLR register.
//
//******************************************************************************
#define APPS_CONFIG_DMA_DONE_INT_MASK_CLR_ADC_WR_DMA_DONE_INT_MASK_CLR_M \
                                0x0000F000  // write 1 to clear mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect bit 14: ADC channel 7 DMA
                                            // Done IRQ mask bit 13: ADC channel
                                            // 5 DMA Done IRQ mask bit 12: ADC
                                            // channel 3 DMA Done IRQ mask bit
                                            // 11: ADC channel 1 DMA Done IRQ
                                            // mask

#define APPS_CONFIG_DMA_DONE_INT_MASK_CLR_ADC_WR_DMA_DONE_INT_MASK_CLR_S 12
#define APPS_CONFIG_DMA_DONE_INT_MASK_CLR_MACASP_WR_DMA_DONE_INT_MASK_CLR \
                                0x00000800  // write 1 to clear mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_CLR_MCASP_RD_DMA_DONE_INT_MASK_CLR \
                                0x00000400  // write 1 to clear mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_CLR_CAM_FIFO_EMPTY_DMA_DONE_INT_MASK_CLR \
                                0x00000200  // write 1 to clear mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_CLR_CAM_THRESHHOLD_DMA_DONE_INT_MASK_CLR \
                                0x00000100  // write 1 to clear mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_CLR_SHSPI_WR_DMA_DONE_INT_MASK_CLR \
                                0x00000080  // write 1 to clear mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_CLR_SHSPI_RD_DMA_DONE_INT_MASK_CLR \
                                0x00000040  // write 1 to clear mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_CLR_HOSTSPI_WR_DMA_DONE_INT_MASK_CLR \
                                0x00000020  // write 1 to clear mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_CLR_HOSTSPI_RD_DMA_DONE_INT_MASK_CLR \
                                0x00000010  // write 1 to clear mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_CLR_APPS_SPI_WR_DMA_DONE_INT_MASK_CLR \
                                0x00000008  // write 1 to clear mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_CLR_APPS_SPI_RD_DMA_DONE_INT_MASK_CLR \
                                0x00000004  // write 1 to clear mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_CLR_SDIOM_WR_DMA_DONE_INT_MASK_CLR \
                                0x00000002  // write 1 to clear mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

#define APPS_CONFIG_DMA_DONE_INT_MASK_CLR_SDIOM_RD_DMA_DONE_INT_MASK_CLR \
                                0x00000001  // write 1 to clear mask of the
                                            // corresponding DMA DONE IRQ;0 = no
                                            // effect

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_CONFIG_O_DMA_DONE_INT_STS_CLR register.
//
//******************************************************************************
#define APPS_CONFIG_DMA_DONE_INT_STS_CLR_DMA_INT_STS_CLR_M \
                                0xFFFFFFFF  // write 1 or 0 to clear all
                                            // DMA_DONE interrupt;

#define APPS_CONFIG_DMA_DONE_INT_STS_CLR_DMA_INT_STS_CLR_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_CONFIG_O_DMA_DONE_INT_ACK register.
//
//******************************************************************************
#define APPS_CONFIG_DMA_DONE_INT_ACK_ADC_WR_DMA_DONE_INT_ACK_M \
                                0x0000F000  // write 1 to clear corresponding
                                            // interrupt; 0 = no effect; bit 14:
                                            // ADC channel 7 DMA Done IRQ bit
                                            // 13: ADC channel 5 DMA Done IRQ
                                            // bit 12: ADC channel 3 DMA Done
                                            // IRQ bit 11: ADC channel 1 DMA
                                            // Done IRQ

#define APPS_CONFIG_DMA_DONE_INT_ACK_ADC_WR_DMA_DONE_INT_ACK_S 12
#define APPS_CONFIG_DMA_DONE_INT_ACK_MCASP_WR_DMA_DONE_INT_ACK \
                                0x00000800  // write 1 to clear corresponding
                                            // interrupt; 0 = no effect;

#define APPS_CONFIG_DMA_DONE_INT_ACK_MCASP_RD_DMA_DONE_INT_ACK \
                                0x00000400  // write 1 to clear corresponding
                                            // interrupt; 0 = no effect;

#define APPS_CONFIG_DMA_DONE_INT_ACK_CAM_FIFO_EMPTY_DMA_DONE_INT_ACK \
                                0x00000200  // write 1 to clear corresponding
                                            // interrupt; 0 = no effect;

#define APPS_CONFIG_DMA_DONE_INT_ACK_CAM_THRESHHOLD_DMA_DONE_INT_ACK \
                                0x00000100  // write 1 to clear corresponding
                                            // interrupt; 0 = no effect;

#define APPS_CONFIG_DMA_DONE_INT_ACK_SHSPI_WR_DMA_DONE_INT_ACK \
                                0x00000080  // write 1 to clear corresponding
                                            // interrupt; 0 = no effect;

#define APPS_CONFIG_DMA_DONE_INT_ACK_SHSPI_RD_DMA_DONE_INT_ACK \
                                0x00000040  // write 1 to clear corresponding
                                            // interrupt; 0 = no effect;

#define APPS_CONFIG_DMA_DONE_INT_ACK_HOSTSPI_WR_DMA_DONE_INT_ACK \
                                0x00000020  // write 1 to clear corresponding
                                            // interrupt; 0 = no effect;

#define APPS_CONFIG_DMA_DONE_INT_ACK_HOSTSPI_RD_DMA_DONE_INT_ACK \
                                0x00000010  // write 1 to clear corresponding
                                            // interrupt; 0 = no effect;

#define APPS_CONFIG_DMA_DONE_INT_ACK_APPS_SPI_WR_DMA_DONE_INT_ACK \
                                0x00000008  // write 1 to clear corresponding
                                            // interrupt; 0 = no effect;

#define APPS_CONFIG_DMA_DONE_INT_ACK_APPS_SPI_RD_DMA_DONE_INT_ACK \
                                0x00000004  // write 1 to clear corresponding
                                            // interrupt; 0 = no effect;

#define APPS_CONFIG_DMA_DONE_INT_ACK_SDIOM_WR_DMA_DONE_INT_ACK \
                                0x00000002  // write 1 to clear corresponding
                                            // interrupt; 0 = no effect;

#define APPS_CONFIG_DMA_DONE_INT_ACK_SDIOM_RD_DMA_DONE_INT_ACK \
                                0x00000001  // write 1 to clear corresponding
                                            // interrupt; 0 = no effect;

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_CONFIG_O_DMA_DONE_INT_STS_MASKED register.
//
//******************************************************************************
#define APPS_CONFIG_DMA_DONE_INT_STS_MASKED_ADC_WR_DMA_DONE_INT_STS_MASKED_M \
                                0x0000F000  // 1= corresponding interrupt is
                                            // active and not masked. read is
                                            // non-destructive;0 = corresponding
                                            // interrupt is inactive or masked
                                            // by DMA_DONE_INT mask bit 14: ADC
                                            // channel 7 DMA Done IRQ bit 13:
                                            // ADC channel 5 DMA Done IRQ bit
                                            // 12: ADC channel 3 DMA Done IRQ
                                            // bit 11: ADC channel 1 DMA Done
                                            // IRQ

#define APPS_CONFIG_DMA_DONE_INT_STS_MASKED_ADC_WR_DMA_DONE_INT_STS_MASKED_S 12
#define APPS_CONFIG_DMA_DONE_INT_STS_MASKED_MCASP_WR_DMA_DONE_INT_STS_MASKED \
                                0x00000800  // 1= corresponding interrupt is
                                            // active and not masked. read is
                                            // non-destructive;0 = corresponding
                                            // interrupt is inactive or masked
                                            // by DMA_DONE_INT mask

#define APPS_CONFIG_DMA_DONE_INT_STS_MASKED_MCASP_RD_DMA_DONE_INT_STS_MASKED \
                                0x00000400  // 1= corresponding interrupt is
                                            // active and not masked. read is
                                            // non-destructive;0 = corresponding
                                            // interrupt is inactive or masked
                                            // by DMA_DONE_INT mask

#define APPS_CONFIG_DMA_DONE_INT_STS_MASKED_CAM_FIFO_EMPTY_DMA_DONE_INT_STS_MASKED \
                                0x00000200  // 1= corresponding interrupt is
                                            // active and not masked. read is
                                            // non-destructive;0 = corresponding
                                            // interrupt is inactive or masked
                                            // by DMA_DONE_INT mask

#define APPS_CONFIG_DMA_DONE_INT_STS_MASKED_CAM_THRESHHOLD_DMA_DONE_INT_STS_MASKED \
                                0x00000100  // 1= corresponding interrupt is
                                            // active and not masked. read is
                                            // non-destructive;0 = corresponding
                                            // interrupt is inactive or masked
                                            // by DMA_DONE_INT mask

#define APPS_CONFIG_DMA_DONE_INT_STS_MASKED_SHSPI_WR_DMA_DONE_INT_STS_MASKED \
                                0x00000080  // 1= corresponding interrupt is
                                            // active and not masked. read is
                                            // non-destructive;0 = corresponding
                                            // interrupt is inactive or masked
                                            // by DMA_DONE_INT mask

#define APPS_CONFIG_DMA_DONE_INT_STS_MASKED_SHSPI_RD_DMA_DONE_INT_STS_MASKED \
                                0x00000040  // 1= corresponding interrupt is
                                            // active and not masked. read is
                                            // non-destructive;0 = corresponding
                                            // interrupt is inactive or masked
                                            // by DMA_DONE_INT mask

#define APPS_CONFIG_DMA_DONE_INT_STS_MASKED_HOSTSPI_WR_DMA_DONE_INT_STS_MASKED \
                                0x00000020  // 1= corresponding interrupt is
                                            // active and not masked. read is
                                            // non-destructive;0 = corresponding
                                            // interrupt is inactive or masked
                                            // by DMA_DONE_INT mask

#define APPS_CONFIG_DMA_DONE_INT_STS_MASKED_HOSTSPI_RD_DMA_DONE_INT_STS_MASKED \
                                0x00000010  // 1= corresponding interrupt is
                                            // active and not masked. read is
                                            // non-destructive;0 = corresponding
                                            // interrupt is inactive or masked
                                            // by DMA_DONE_INT mask

#define APPS_CONFIG_DMA_DONE_INT_STS_MASKED_APPS_SPI_WR_DMA_DONE_INT_STS_MASKED \
                                0x00000008  // 1= corresponding interrupt is
                                            // active and not masked. read is
                                            // non-destructive;0 = corresponding
                                            // interrupt is inactive or masked
                                            // by DMA_DONE_INT mask

#define APPS_CONFIG_DMA_DONE_INT_STS_MASKED_APPS_SPI_RD_DMA_DONE_INT_STS_MASKED \
                                0x00000004  // 1= corresponding interrupt is
                                            // active and not masked. read is
                                            // non-destructive;0 = corresponding
                                            // interrupt is inactive or masked
                                            // by DMA_DONE_INT mask

#define APPS_CONFIG_DMA_DONE_INT_STS_MASKED_SDIOM_WR_DMA_DONE_INT_STS_MASKED \
                                0x00000002  // 1= corresponding interrupt is
                                            // active and not masked. read is
                                            // non-destructive;0 = corresponding
                                            // interrupt is inactive or masked
                                            // by DMA_DONE_INT mask

#define APPS_CONFIG_DMA_DONE_INT_STS_MASKED_SDIOM_RD_DMA_DONE_INT_STS_MASKED \
                                0x00000001  // 1= corresponding interrupt is
                                            // active and not masked. read is
                                            // non-destructive;0 = corresponding
                                            // interrupt is inactive or masked
                                            // by DMA_DONE_INT mask

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_CONFIG_O_DMA_DONE_INT_STS_RAW register.
//
//******************************************************************************
#define APPS_CONFIG_DMA_DONE_INT_STS_RAW_ADC_WR_DMA_DONE_INT_STS_RAW_M \
                                0x0000F000  // 1= corresponding interrupt is
                                            // active. read is non-destructive;0
                                            // = corresponding interrupt is
                                            // inactive bit 14: ADC channel 7
                                            // DMA Done IRQ bit 13: ADC channel
                                            // 5 DMA Done IRQ bit 12: ADC
                                            // channel 3 DMA Done IRQ bit 11:
                                            // ADC channel 1 DMA Done IRQ

#define APPS_CONFIG_DMA_DONE_INT_STS_RAW_ADC_WR_DMA_DONE_INT_STS_RAW_S 12
#define APPS_CONFIG_DMA_DONE_INT_STS_RAW_MCASP_WR_DMA_DONE_INT_STS_RAW \
                                0x00000800  // 1= corresponding interrupt is
                                            // active. read is non-destructive;0
                                            // = corresponding interrupt is
                                            // inactive

#define APPS_CONFIG_DMA_DONE_INT_STS_RAW_MCASP_RD_DMA_DONE_INT_STS_RAW \
                                0x00000400  // 1= corresponding interrupt is
                                            // active. read is non-destructive;0
                                            // = corresponding interrupt is
                                            // inactive

#define APPS_CONFIG_DMA_DONE_INT_STS_RAW_CAM_EPMTY_FIFO_DMA_DONE_INT_STS_RAW \
                                0x00000200  // 1= corresponding interrupt is
                                            // active. read is non-destructive;0
                                            // = corresponding interrupt is
                                            // inactive

#define APPS_CONFIG_DMA_DONE_INT_STS_RAW_CAM_THRESHHOLD_DMA_DONE_INT_STS_RAW \
                                0x00000100  // 1= corresponding interrupt is
                                            // active. read is non-destructive;0
                                            // = corresponding interrupt is
                                            // inactive

#define APPS_CONFIG_DMA_DONE_INT_STS_RAW_SHSPI_WR_DMA_DONE_INT_STS_RAW \
                                0x00000080  // 1= corresponding interrupt is
                                            // active. read is non-destructive;0
                                            // = corresponding interrupt is
                                            // inactive

#define APPS_CONFIG_DMA_DONE_INT_STS_RAW_SHSPI_RD_DMA_DONE_INT_STS_RAW \
                                0x00000040  // 1= corresponding interrupt is
                                            // active. read is non-destructive;0
                                            // = corresponding interrupt is
                                            // inactive

#define APPS_CONFIG_DMA_DONE_INT_STS_RAW_HOSTSPI_WR_DMA_DONE_INT_STS_RAW \
                                0x00000020  // 1= corresponding interrupt is
                                            // active. read is non-destructive;0
                                            // = corresponding interrupt is
                                            // inactive

#define APPS_CONFIG_DMA_DONE_INT_STS_RAW_HOSTSPI_RD_DMA_DONE_INT_STS_RAW \
                                0x00000010  // 1= corresponding interrupt is
                                            // active. read is non-destructive;0
                                            // = corresponding interrupt is
                                            // inactive

#define APPS_CONFIG_DMA_DONE_INT_STS_RAW_APPS_SPI_WR_DMA_DONE_INT_STS_RAW \
                                0x00000008  // 1= corresponding interrupt is
                                            // active. read is non-destructive;0
                                            // = corresponding interrupt is
                                            // inactive

#define APPS_CONFIG_DMA_DONE_INT_STS_RAW_APPS_SPI_RD_DMA_DONE_INT_STS_RAW \
                                0x00000004  // 1= corresponding interrupt is
                                            // active. read is non-destructive;0
                                            // = corresponding interrupt is
                                            // inactive

#define APPS_CONFIG_DMA_DONE_INT_STS_RAW_SDIOM_WR_DMA_DONE_INT_STS_RAW \
                                0x00000002  // 1= corresponding interrupt is
                                            // active. read is non-destructive;0
                                            // = corresponding interrupt is
                                            // inactive

#define APPS_CONFIG_DMA_DONE_INT_STS_RAW_SDIOM_RD_DMA_DONE_INT_STS_RAW \
                                0x00000001  // 1= corresponding interrupt is
                                            // active. read is non-destructive;0
                                            // = corresponding interrupt is
                                            // inactive

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_CONFIG_O_FAULT_STATUS_CLR_REG register.
//
//******************************************************************************
#define APPS_CONFIG_FAULT_STATUS_CLR_REG_PATCH_ERR_CLR \
                                0x00000001  // Write 1 to clear the LSB of
                                            // FAULT_STATUS_REG

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_CONFIG_O_RESERVD_REG_0 register.
//
//******************************************************************************
//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_CONFIG_O_GPT_TRIG_SEL register.
//
//******************************************************************************
#define APPS_CONFIG_GPT_TRIG_SEL_GPT_TRIG_SEL_M \
                                0x000000FF  // This bit is implemented for GPT
                                            // trigger mode select. GPT IP
                                            // support 2 modes: RTC mode and
                                            // external trigger. When this bit
                                            // is set to logic '1': enable
                                            // external trigger mode for APPS
                                            // GPT CP0 and CP1 pin. bit 0: when
                                            // set '1' enable external GPT
                                            // trigger 0 on GPIO0 CP0 pin else
                                            // RTC mode is selected. bit 1: when
                                            // set '1' enable external GPT
                                            // trigger 1 on GPIO0 CP1 pin else
                                            // RTC mode is selected. bit 2: when
                                            // set '1' enable external GPT
                                            // trigger 2 on GPIO1 CP0 pin else
                                            // RTC mode is selected. bit 3: when
                                            // set '1' enable external GPT
                                            // trigger 3 on GPIO1 CP1 pin else
                                            // RTC mode is selected. bit 4: when
                                            // set '1' enable external GPT
                                            // trigger 4 on GPIO2 CP0 pin else
                                            // RTC mode is selected. bit 5: when
                                            // set '1' enable external GPT
                                            // trigger 5 on GPIO2 CP1 pin else
                                            // RTC mode is selected. bit 6: when
                                            // set '1' enable external GPT
                                            // trigger 6 on GPIO3 CP0 pin else
                                            // RTC mode is selected. bit 7: when
                                            // set '1' enable external GPT
                                            // trigger 7 on GPIO3 CP1 pin else
                                            // RTC mode is selected.

#define APPS_CONFIG_GPT_TRIG_SEL_GPT_TRIG_SEL_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_CONFIG_O_TOP_DIE_SPARE_DIN_REG register.
//
//******************************************************************************
#define APPS_CONFIG_TOP_DIE_SPARE_DIN_REG_D2D_SPARE_DIN_M \
                                0x00000007  // Capture data from d2d_spare pads

#define APPS_CONFIG_TOP_DIE_SPARE_DIN_REG_D2D_SPARE_DIN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_CONFIG_O_TOP_DIE_SPARE_DOUT_REG register.
//
//******************************************************************************
#define APPS_CONFIG_TOP_DIE_SPARE_DOUT_REG_D2D_SPARE_DOUT_M \
                                0x00000007  // Send data to d2d_spare pads -
                                            // eventually this will get
                                            // registered in top die

#define APPS_CONFIG_TOP_DIE_SPARE_DOUT_REG_D2D_SPARE_DOUT_S 0



#endif // __HW_APPS_CONFIG_H__

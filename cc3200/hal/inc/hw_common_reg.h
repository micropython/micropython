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

#ifndef __HW_COMMON_REG_H__
#define __HW_COMMON_REG_H__

//*****************************************************************************
//
// The following are defines for the COMMON_REG register offsets.
//
//*****************************************************************************
#define COMMON_REG_O_I2C_Properties_Register \
                                0x00000000

#define COMMON_REG_O_SPI_Properties_Register \
                                0x00000004

#define COMMON_REG_O_APPS_sh_resource_Interrupt_enable \
                                0x0000000C

#define COMMON_REG_O_APPS_sh_resource_Interrupt_status \
                                0x00000010

#define COMMON_REG_O_NWP_sh_resource_Interrupt_enable \
                                0x00000014

#define COMMON_REG_O_NWP_sh_resource_Interrupt_status \
                                0x00000018

#define COMMON_REG_O_Flash_ctrl_reg \
                                0x0000001C

#define COMMON_REG_O_Bus_matrix_M0_segment_access_config \
                                0x00000024

#define COMMON_REG_O_Bus_matrix_M1_segment_access_config \
                                0x00000028

#define COMMON_REG_O_Bus_matrix_M2_segment_access_config \
                                0x0000002C

#define COMMON_REG_O_Bus_matrix_M3_segment_access_config \
                                0x00000030

#define COMMON_REG_O_Bus_matrix_M4_segment_access_config \
                                0x00000034

#define COMMON_REG_O_Bus_matrix_M5_segment_access_config \
                                0x00000038

#define COMMON_REG_O_GPIO_properties_register \
                                0x0000003C

#define COMMON_REG_O_APPS_NW_SEMAPHORE1 \
                                0x00000040

#define COMMON_REG_O_APPS_NW_SEMAPHORE2 \
                                0x00000044

#define COMMON_REG_O_APPS_NW_SEMAPHORE3 \
                                0x00000048

#define COMMON_REG_O_APPS_NW_SEMAPHORE4 \
                                0x0000004C

#define COMMON_REG_O_APPS_NW_SEMAPHORE5 \
                                0x00000050

#define COMMON_REG_O_APPS_NW_SEMAPHORE6 \
                                0x00000054

#define COMMON_REG_O_APPS_NW_SEMAPHORE7 \
                                0x00000058

#define COMMON_REG_O_APPS_NW_SEMAPHORE8 \
                                0x0000005C

#define COMMON_REG_O_APPS_NW_SEMAPHORE9 \
                                0x00000060

#define COMMON_REG_O_APPS_NW_SEMAPHORE10 \
                                0x00000064

#define COMMON_REG_O_APPS_NW_SEMAPHORE11 \
                                0x00000068

#define COMMON_REG_O_APPS_NW_SEMAPHORE12 \
                                0x0000006C

#define COMMON_REG_O_APPS_SEMAPPHORE_PEND \
                                0x00000070

#define COMMON_REG_O_NW_SEMAPPHORE_PEND \
                                0x00000074

#define COMMON_REG_O_SEMAPHORE_STATUS \
                                0x00000078

#define COMMON_REG_O_IDMEM_TIM_Update \
                                0x0000007C

#define COMMON_REG_O_FPGA_ROM_WR_EN \
                                0x00000080

#define COMMON_REG_O_NW_INT_MASK \
                                0x00000084

#define COMMON_REG_O_NW_INT_MASK_SET \
                                0x00000088

#define COMMON_REG_O_NW_INT_MASK_CLR \
                                0x0000008C

#define COMMON_REG_O_NW_INT_STS_CLR \
                                0x00000090

#define COMMON_REG_O_NW_INT_ACK 0x00000094
#define COMMON_REG_O_NW_INT_TRIG \
                                0x00000098

#define COMMON_REG_O_NW_INT_STS_MASKED \
                                0x0000009C

#define COMMON_REG_O_NW_INT_STS_RAW \
                                0x000000A0

#define COMMON_REG_O_APPS_INT_MASK \
                                0x000000A4

#define COMMON_REG_O_APPS_INT_MASK_SET \
                                0x000000A8

#define COMMON_REG_O_APPS_INT_MASK_CLR \
                                0x000000AC

#define COMMON_REG_O_APPS_INT_STS_CLR \
                                0x000000B0

#define COMMON_REG_O_APPS_INT_ACK \
                                0x000000B4

#define COMMON_REG_O_APPS_INT_TRIG \
                                0x000000B8

#define COMMON_REG_O_APPS_INT_STS_MASKED \
                                0x000000BC

#define COMMON_REG_O_APPS_INT_STS_RAW \
                                0x000000C0

#define COMMON_REG_O_IDMEM_TIM_Updated \
                                0x000000C4

#define COMMON_REG_O_APPS_GPIO_TRIG_EN \
                                0x000000C8

#define COMMON_REG_O_EMU_DEBUG_REG \
                                0x000000CC

#define COMMON_REG_O_SEMAPHORE_STATUS2 \
                                0x000000D0

#define COMMON_REG_O_SEMAPHORE_PREV_OWNER1 \
                                0x000000D4

#define COMMON_REG_O_SEMAPHORE_PREV_OWNER2 \
                                0x000000D8




//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_I2C_Properties_Register register.
//
//******************************************************************************
#define COMMON_REG_I2C_Properties_Register_I2C_Properties_Register_M \
                                0x00000003  // • Each semaphore register is of
                                            // 2 bit. • When this register is
                                            // set to 2’b01 – Apps have access
                                            // and when set to 2’b10 – NW have
                                            // access. • Ideally both the master
                                            // can modify any of this 2 bit, but
                                            // assumption apps will write only
                                            // 2’b01 or 2’b00 to this register
                                            // and nw will write only 2’b10 or
                                            // 2’b00. • Implementation is when
                                            // any of the bit of this register
                                            // is set, only next write
                                            // allowedvis 2’b00 – Again
                                            // assumption is one master will not
                                            // write 2’b00 if other is already
                                            // holding the semaphore.

#define COMMON_REG_I2C_Properties_Register_I2C_Properties_Register_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_SPI_Properties_Register register.
//
//******************************************************************************
#define COMMON_REG_SPI_Properties_Register_SPI_Properties_Register_M \
                                0x00000003  // • Each semaphore register is of
                                            // 2 bit. • When this register is
                                            // set to 2’b01 – Apps have access
                                            // and when set to 2’b10 – NW have
                                            // access. • Ideally both the master
                                            // can modify any of this 2 bit, but
                                            // assumption apps will write only
                                            // 2’b01 or 2’b00 to this register
                                            // and nw will write only 2’b10 or
                                            // 2’b00. • Implementation is when
                                            // any of the bit of this register
                                            // is set, only next write
                                            // allowedvis 2’b00 – Again
                                            // assumption is one master will not
                                            // write 2’b00 if other is already
                                            // holding the semaphore.

#define COMMON_REG_SPI_Properties_Register_SPI_Properties_Register_S 0

//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_sh_resource_Interrupt_enable register.
//
//******************************************************************************
#define COMMON_REG_APPS_sh_resource_Interrupt_enable_APPS_sh_resource_Interrupt_enable_M \
                                0x0000000F  // Interrupt enable APPS bit 0 ->
                                            // when '1' enable I2C interrupt bit
                                            // 1 -> when '1' enable SPI
                                            // interrupt bit 3 ->
                                            // when '1' enable GPIO interrupt

#define COMMON_REG_APPS_sh_resource_Interrupt_enable_APPS_sh_resource_Interrupt_enable_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_sh_resource_Interrupt_status register.
//
//******************************************************************************
#define COMMON_REG_APPS_sh_resource_Interrupt_status_APPS_sh_resource_Interrupt_status_M \
                                0x0000000F  // Interrupt enable APPS bit 0 ->
                                            // when '1' enable I2C interrupt bit
                                            // 1 -> when '1' enable SPI
                                            // interrupt bit 3 ->
                                            // when '1' enable GPIO interrupt

#define COMMON_REG_APPS_sh_resource_Interrupt_status_APPS_sh_resource_Interrupt_status_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_NWP_sh_resource_Interrupt_enable register.
//
//******************************************************************************
#define COMMON_REG_NWP_sh_resource_Interrupt_enable_NWP_sh_resource_Interrupt_enable_M \
                                0x0000000F  // Interrupt enable NWP bit 0 ->
                                            // when '1' enable I2C interrupt bit
                                            // 1 -> when '1' enable SPI
                                            // interrupt bit 3 ->
                                            // when '1' enable GPIO interrupt

#define COMMON_REG_NWP_sh_resource_Interrupt_enable_NWP_sh_resource_Interrupt_enable_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_NWP_sh_resource_Interrupt_status register.
//
//******************************************************************************
#define COMMON_REG_NWP_sh_resource_Interrupt_status_NWP_sh_resource_Interrupt_status_M \
                                0x0000000F  // Interrupt enable NWP bit 0 ->
                                            // when '1' enable I2C interrupt bit
                                            // 1 -> when '1' enable SPI
                                            // interrupt bit 3 ->
                                            // when '1' enable GPIO interrupt

#define COMMON_REG_NWP_sh_resource_Interrupt_status_NWP_sh_resource_Interrupt_status_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_Flash_ctrl_reg register.
//
//******************************************************************************
#define COMMON_REG_Flash_ctrl_reg_Flash_ctrl_reg_M \
                                0x00000003  // • Each semaphore register is of
                                            // 2 bit. • When this register is
                                            // set to 2’b01 – Apps have access
                                            // and when set to 2’b10 – NW have
                                            // access. • Ideally both the master
                                            // can modify any of this 2 bit, but
                                            // assumption apps will write only
                                            // 2’b01 or 2’b00 to this register
                                            // and nw will write only 2’b10 or
                                            // 2’b00. • Implementation is when
                                            // any of the bit of this register
                                            // is set, only next write
                                            // allowedvis 2’b00 – Again
                                            // assumption is one master will not
                                            // write 2’b00 if other is already
                                            // holding the semaphore.

#define COMMON_REG_Flash_ctrl_reg_Flash_ctrl_reg_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_Bus_matrix_M0_segment_access_config register.
//
//******************************************************************************
#define COMMON_REG_Bus_matrix_M0_segment_access_config_Bus_matrix_M0_segment_access_config_M \
                                0x0003FFFF  // Master 0 control word matrix to
                                            // each segment. Tieoff. Bit value 1
                                            // indicates segment is accesable.

#define COMMON_REG_Bus_matrix_M0_segment_access_config_Bus_matrix_M0_segment_access_config_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_Bus_matrix_M1_segment_access_config register.
//
//******************************************************************************
#define COMMON_REG_Bus_matrix_M1_segment_access_config_Bus_matrix_M1_segment_access_config_M \
                                0x0003FFFF  // Master 1 control word matrix to
                                            // each segment. Tieoff. Bit value 1
                                            // indicates segment is accesable.

#define COMMON_REG_Bus_matrix_M1_segment_access_config_Bus_matrix_M1_segment_access_config_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_Bus_matrix_M2_segment_access_config register.
//
//******************************************************************************
#define COMMON_REG_Bus_matrix_M2_segment_access_config_Bus_matrix_M2_segment_access_config_M \
                                0x0003FFFF  // Master 2 control word matrix to
                                            // each segment. Tieoff. Bit value 1
                                            // indicates segment is accesable.

#define COMMON_REG_Bus_matrix_M2_segment_access_config_Bus_matrix_M2_segment_access_config_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_Bus_matrix_M3_segment_access_config register.
//
//******************************************************************************
#define COMMON_REG_Bus_matrix_M3_segment_access_config_Bus_matrix_M3_segment_access_config_M \
                                0x0003FFFF  // Master 3 control word matrix to
                                            // each segment. Tieoff. Bit value 1
                                            // indicates segment is accesable.

#define COMMON_REG_Bus_matrix_M3_segment_access_config_Bus_matrix_M3_segment_access_config_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_Bus_matrix_M4_segment_access_config register.
//
//******************************************************************************
#define COMMON_REG_Bus_matrix_M4_segment_access_config_Bus_matrix_M4_segment_access_config_M \
                                0x0003FFFF  // Master 4 control word matrix to
                                            // each segment. Tieoff. Bit value 1
                                            // indicates segment is accesable.

#define COMMON_REG_Bus_matrix_M4_segment_access_config_Bus_matrix_M4_segment_access_config_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_Bus_matrix_M5_segment_access_config register.
//
//******************************************************************************
#define COMMON_REG_Bus_matrix_M5_segment_access_config_Bus_matrix_M5_segment_access_config_M \
                                0x0003FFFF  // Master 5 control word matrix to
                                            // each segment. Tieoff. Bit value 1
                                            // indicates segment is accesable.

#define COMMON_REG_Bus_matrix_M5_segment_access_config_Bus_matrix_M5_segment_access_config_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_GPIO_properties_register register.
//
//******************************************************************************
#define COMMON_REG_GPIO_properties_register_GPIO_properties_register_M \
                                0x000003FF  // Shared GPIO configuration
                                            // register. Bit [1:0] to configure
                                            // GPIO0 Bit [3:2] to configure
                                            // GPIO1 Bit [5:4] to configure
                                            // GPIO2 Bit [7:6] to configure
                                            // GPIO3 Bit [9:8] to configure
                                            // GPIO4 each GPIO can be
                                            // individully selected. When “00”
                                            // GPIO is free resource. When “01”
                                            // GPIO is APPS resource. When “10”
                                            // GPIO is NWP resource. Writing 11
                                            // doesnt have any affect, i.e. If
                                            // one write only relevant gpio
                                            // semaphore and other bits are 1s,
                                            // it'll not disturb the other
                                            // semaphore bits. For example : Say
                                            // If NW wants to take control of
                                            // gpio-1, one should write
                                            // 10'b11_1111_1011 and if one wants
                                            // to release it write
                                            // 10'b11_1111_0011.

#define COMMON_REG_GPIO_properties_register_GPIO_properties_register_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_NW_SEMAPHORE1 register.
//
//******************************************************************************
#define COMMON_REG_APPS_NW_SEMAPHORE1_APPS_NW_SEMAPHORE1_M \
                                0xFFFFFFFF  // • Each semaphore register is of
                                            // 2 bit. • When this register is
                                            // set to 2’b01 – Apps have access
                                            // and when set to 2’b10 – NW have
                                            // access. • Ideally both the master
                                            // can modify any of this 2 bit, but
                                            // assumption apps will write only
                                            // 2’b01 or 2’b00 to this register
                                            // and nw will write only 2’b10 or
                                            // 2’b00. • Implementation is when
                                            // any of the bit of this register
                                            // is set, only next write
                                            // allowedvis 2’b00 – Again
                                            // assumption is one master will not
                                            // write 2’b00 if other is already
                                            // holding the semaphore.

#define COMMON_REG_APPS_NW_SEMAPHORE1_APPS_NW_SEMAPHORE1_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_NW_SEMAPHORE2 register.
//
//******************************************************************************
#define COMMON_REG_APPS_NW_SEMAPHORE2_APPS_NW_SEMAPHORE2_M \
                                0xFFFFFFFF  // • Each semaphore register is of
                                            // 2 bit. • When this register is
                                            // set to 2’b01 – Apps have access
                                            // and when set to 2’b10 – NW have
                                            // access. • Ideally both the master
                                            // can modify any of this 2 bit, but
                                            // assumption apps will write only
                                            // 2’b01 or 2’b00 to this register
                                            // and nw will write only 2’b10 or
                                            // 2’b00. • Implementation is when
                                            // any of the bit of this register
                                            // is set, only next write
                                            // allowedvis 2’b00 – Again
                                            // assumption is one master will not
                                            // write 2’b00 if other is already
                                            // holding the semaphore.

#define COMMON_REG_APPS_NW_SEMAPHORE2_APPS_NW_SEMAPHORE2_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_NW_SEMAPHORE3 register.
//
//******************************************************************************
#define COMMON_REG_APPS_NW_SEMAPHORE3_APPS_NW_SEMAPHORE3_M \
                                0xFFFFFFFF  // • Each semaphore register is of
                                            // 2 bit. • When this register is
                                            // set to 2’b01 – Apps have access
                                            // and when set to 2’b10 – NW have
                                            // access. • Ideally both the master
                                            // can modify any of this 2 bit, but
                                            // assumption apps will write only
                                            // 2’b01 or 2’b00 to this register
                                            // and nw will write only 2’b10 or
                                            // 2’b00. • Implementation is when
                                            // any of the bit of this register
                                            // is set, only next write
                                            // allowedvis 2’b00 – Again
                                            // assumption is one master will not
                                            // write 2’b00 if other is already
                                            // holding the semaphore.

#define COMMON_REG_APPS_NW_SEMAPHORE3_APPS_NW_SEMAPHORE3_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_NW_SEMAPHORE4 register.
//
//******************************************************************************
#define COMMON_REG_APPS_NW_SEMAPHORE4_APPS_NW_SEMAPHORE4_M \
                                0xFFFFFFFF  // • Each semaphore register is of
                                            // 2 bit. • When this register is
                                            // set to 2’b01 – Apps have access
                                            // and when set to 2’b10 – NW have
                                            // access. • Ideally both the master
                                            // can modify any of this 2 bit, but
                                            // assumption apps will write only
                                            // 2’b01 or 2’b00 to this register
                                            // and nw will write only 2’b10 or
                                            // 2’b00. • Implementation is when
                                            // any of the bit of this register
                                            // is set, only next write
                                            // allowedvis 2’b00 – Again
                                            // assumption is one master will not
                                            // write 2’b00 if other is already
                                            // holding the semaphore.

#define COMMON_REG_APPS_NW_SEMAPHORE4_APPS_NW_SEMAPHORE4_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_NW_SEMAPHORE5 register.
//
//******************************************************************************
#define COMMON_REG_APPS_NW_SEMAPHORE5_APPS_NW_SEMAPHORE5_M \
                                0xFFFFFFFF  // • Each semaphore register is of
                                            // 2 bit. • When this register is
                                            // set to 2’b01 – Apps have access
                                            // and when set to 2’b10 – NW have
                                            // access. • Ideally both the master
                                            // can modify any of this 2 bit, but
                                            // assumption apps will write only
                                            // 2’b01 or 2’b00 to this register
                                            // and nw will write only 2’b10 or
                                            // 2’b00. • Implementation is when
                                            // any of the bit of this register
                                            // is set, only next write
                                            // allowedvis 2’b00 – Again
                                            // assumption is one master will not
                                            // write 2’b00 if other is already
                                            // holding the semaphore.

#define COMMON_REG_APPS_NW_SEMAPHORE5_APPS_NW_SEMAPHORE5_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_NW_SEMAPHORE6 register.
//
//******************************************************************************
#define COMMON_REG_APPS_NW_SEMAPHORE6_APPS_NW_SEMAPHORE6_M \
                                0xFFFFFFFF  // • Each semaphore register is of
                                            // 2 bit. • When this register is
                                            // set to 2’b01 – Apps have access
                                            // and when set to 2’b10 – NW have
                                            // access. • Ideally both the master
                                            // can modify any of this 2 bit, but
                                            // assumption apps will write only
                                            // 2’b01 or 2’b00 to this register
                                            // and nw will write only 2’b10 or
                                            // 2’b00. • Implementation is when
                                            // any of the bit of this register
                                            // is set, only next write
                                            // allowedvis 2’b00 – Again
                                            // assumption is one master will not
                                            // write 2’b00 if other is already
                                            // holding the semaphore.

#define COMMON_REG_APPS_NW_SEMAPHORE6_APPS_NW_SEMAPHORE6_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_NW_SEMAPHORE7 register.
//
//******************************************************************************
#define COMMON_REG_APPS_NW_SEMAPHORE7_APPS_NW_SEMAPHORE7_M \
                                0xFFFFFFFF  // • Each semaphore register is of
                                            // 2 bit. • When this register is
                                            // set to 2’b01 – Apps have access
                                            // and when set to 2’b10 – NW have
                                            // access. • Ideally both the master
                                            // can modify any of this 2 bit, but
                                            // assumption apps will write only
                                            // 2’b01 or 2’b00 to this register
                                            // and nw will write only 2’b10 or
                                            // 2’b00. • Implementation is when
                                            // any of the bit of this register
                                            // is set, only next write
                                            // allowedvis 2’b00 – Again
                                            // assumption is one master will not
                                            // write 2’b00 if other is already
                                            // holding the semaphore.

#define COMMON_REG_APPS_NW_SEMAPHORE7_APPS_NW_SEMAPHORE7_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_NW_SEMAPHORE8 register.
//
//******************************************************************************
#define COMMON_REG_APPS_NW_SEMAPHORE8_APPS_NW_SEMAPHORE8_M \
                                0xFFFFFFFF  // • Each semaphore register is of
                                            // 2 bit. • When this register is
                                            // set to 2’b01 – Apps have access
                                            // and when set to 2’b10 – NW have
                                            // access. • Ideally both the master
                                            // can modify any of this 2 bit, but
                                            // assumption apps will write only
                                            // 2’b01 or 2’b00 to this register
                                            // and nw will write only 2’b10 or
                                            // 2’b00. • Implementation is when
                                            // any of the bit of this register
                                            // is set, only next write
                                            // allowedvis 2’b00 – Again
                                            // assumption is one master will not
                                            // write 2’b00 if other is already
                                            // holding the semaphore.

#define COMMON_REG_APPS_NW_SEMAPHORE8_APPS_NW_SEMAPHORE8_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_NW_SEMAPHORE9 register.
//
//******************************************************************************
#define COMMON_REG_APPS_NW_SEMAPHORE9_APPS_NW_SEMAPHORE9_M \
                                0xFFFFFFFF  // • Each semaphore register is of
                                            // 2 bit. • When this register is
                                            // set to 2’b01 – Apps have access
                                            // and when set to 2’b10 – NW have
                                            // access. • Ideally both the master
                                            // can modify any of this 2 bit, but
                                            // assumption apps will write only
                                            // 2’b01 or 2’b00 to this register
                                            // and nw will write only 2’b10 or
                                            // 2’b00. • Implementation is when
                                            // any of the bit of this register
                                            // is set, only next write
                                            // allowedvis 2’b00 – Again
                                            // assumption is one master will not
                                            // write 2’b00 if other is already
                                            // holding the semaphore.

#define COMMON_REG_APPS_NW_SEMAPHORE9_APPS_NW_SEMAPHORE9_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_NW_SEMAPHORE10 register.
//
//******************************************************************************
#define COMMON_REG_APPS_NW_SEMAPHORE10_APPS_NW_SEMAPHORE10_M \
                                0xFFFFFFFF  // • Each semaphore register is of
                                            // 2 bit. • When this register is
                                            // set to 2’b01 – Apps have access
                                            // and when set to 2’b10 – NW have
                                            // access. • Ideally both the master
                                            // can modify any of this 2 bit, but
                                            // assumption apps will write only
                                            // 2’b01 or 2’b00 to this register
                                            // and nw will write only 2’b10 or
                                            // 2’b00. • Implementation is when
                                            // any of the bit of this register
                                            // is set, only next write
                                            // allowedvis 2’b00 – Again
                                            // assumption is one master will not
                                            // write 2’b00 if other is already
                                            // holding the semaphore.

#define COMMON_REG_APPS_NW_SEMAPHORE10_APPS_NW_SEMAPHORE10_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_NW_SEMAPHORE11 register.
//
//******************************************************************************
#define COMMON_REG_APPS_NW_SEMAPHORE11_APPS_NW_SEMAPHORE11_M \
                                0xFFFFFFFF  // • Each semaphore register is of
                                            // 2 bit. • When this register is
                                            // set to 2’b01 – Apps have access
                                            // and when set to 2’b10 – NW have
                                            // access. • Ideally both the master
                                            // can modify any of this 2 bit, but
                                            // assumption apps will write only
                                            // 2’b01 or 2’b00 to this register
                                            // and nw will write only 2’b10 or
                                            // 2’b00. • Implementation is when
                                            // any of the bit of this register
                                            // is set, only next write
                                            // allowedvis 2’b00 – Again
                                            // assumption is one master will not
                                            // write 2’b00 if other is already
                                            // holding the semaphore.

#define COMMON_REG_APPS_NW_SEMAPHORE11_APPS_NW_SEMAPHORE11_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_NW_SEMAPHORE12 register.
//
//******************************************************************************
#define COMMON_REG_APPS_NW_SEMAPHORE12_APPS_NW_SEMAPHORE12_M \
                                0xFFFFFFFF  // APPS NW semaphore register - not
                                            // reflected in status.

#define COMMON_REG_APPS_NW_SEMAPHORE12_APPS_NW_SEMAPHORE12_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_SEMAPPHORE_PEND register.
//
//******************************************************************************
#define COMMON_REG_APPS_SEMAPPHORE_PEND_APPS_SEMAPPHORE_PEND_M \
                                0xFFFFFFFF  // APPS SEMAPOHORE STATUS

#define COMMON_REG_APPS_SEMAPPHORE_PEND_APPS_SEMAPPHORE_PEND_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_NW_SEMAPPHORE_PEND register.
//
//******************************************************************************
#define COMMON_REG_NW_SEMAPPHORE_PEND_NW_SEMAPPHORE_PEND_M \
                                0xFFFFFFFF  // NW SEMAPHORE STATUS

#define COMMON_REG_NW_SEMAPPHORE_PEND_NW_SEMAPPHORE_PEND_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_SEMAPHORE_STATUS register.
//
//******************************************************************************
#define COMMON_REG_SEMAPHORE_STATUS_SEMAPHORE_STATUS_M \
                                0xFFFFFFFF  // SEMAPHORE STATUS 9:8 :semaphore
                                            // status of flash_control 7:6
                                            // :semaphore status of
                                            // gpio_properties 5:4
                                            // :semaphore status of
                                            // spi_propertie 1:0 :semaphore
                                            // status of i2c_propertie

#define COMMON_REG_SEMAPHORE_STATUS_SEMAPHORE_STATUS_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_IDMEM_TIM_Update register.
//
//******************************************************************************
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_FPGA_ROM_WR_EN register.
//
//******************************************************************************
#define COMMON_REG_FPGA_ROM_WR_EN_FPGA_ROM_WR_EN \
                                0x00000001  // when '1' enables Write into
                                            // IDMEM CORE ROM, APPS ROM, NWP ROM

//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_NW_INT_MASK register.
//
//******************************************************************************
#define COMMON_REG_NW_INT_MASK_NW_INT_MASK_M \
                                0xFFFFFFFF  // 1= disable corresponding
                                            // interrupt;0 = interrupt enabled

#define COMMON_REG_NW_INT_MASK_NW_INT_MASK_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_NW_INT_MASK_SET register.
//
//******************************************************************************
#define COMMON_REG_NW_INT_MASK_SET_NW_INT_MASK_SET_M \
                                0xFFFFFFFF  // write 1 to set corresponding bit
                                            // in NW_INT_MASK;0 = no effect

#define COMMON_REG_NW_INT_MASK_SET_NW_INT_MASK_SET_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_NW_INT_MASK_CLR register.
//
//******************************************************************************
#define COMMON_REG_NW_INT_MASK_CLR_NW_INT_MASK_CLR_M \
                                0xFFFFFFFF  // write 1 to clear corresponding
                                            // bit in NW_INT_MASK;0 = no effect

#define COMMON_REG_NW_INT_MASK_CLR_NW_INT_MASK_CLR_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_NW_INT_STS_CLR register.
//
//******************************************************************************
#define COMMON_REG_NW_INT_STS_CLR_NW_INT_STS_CLR_M \
                                0xFFFFFFFF  // write 1 to clear corresponding
                                            // interrupt; 0 = no effect;
                                            // interrupt is not lost if coincide
                                            // with write operation

#define COMMON_REG_NW_INT_STS_CLR_NW_INT_STS_CLR_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_NW_INT_ACK register.
//
//******************************************************************************
#define COMMON_REG_NW_INT_ACK_NW_INT_ACK_M \
                                0xFFFFFFFF  // write 1 to clear corresponding
                                            // interrupt;0 = no effect

#define COMMON_REG_NW_INT_ACK_NW_INT_ACK_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_NW_INT_TRIG register.
//
//******************************************************************************
#define COMMON_REG_NW_INT_TRIG_NW_INT_TRIG_M \
                                0xFFFFFFFF  // Writing a 1 to a bit in this
                                            // register causes the the Host CPU
                                            // if enabled (not masked). This
                                            // register is self-clearing.
                                            // Writing 0 has no effect

#define COMMON_REG_NW_INT_TRIG_NW_INT_TRIG_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_NW_INT_STS_MASKED register.
//
//******************************************************************************
#define COMMON_REG_NW_INT_STS_MASKED_NW_INT_STS_MASKED_M \
                                0xFFFFFFFF  // 1= corresponding interrupt is
                                            // active and not masked. read is
                                            // non-destructive;0 = corresponding
                                            // interrupt is inactive or masked
                                            // by NW_INT mask

#define COMMON_REG_NW_INT_STS_MASKED_NW_INT_STS_MASKED_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_NW_INT_STS_RAW register.
//
//******************************************************************************
#define COMMON_REG_NW_INT_STS_RAW_NW_INT_STS_RAW_M \
                                0xFFFFFFFF  // 1= corresponding interrupt is
                                            // active. read is non-destructive;0
                                            // = corresponding interrupt is
                                            // inactive

#define COMMON_REG_NW_INT_STS_RAW_NW_INT_STS_RAW_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_INT_MASK register.
//
//******************************************************************************
#define COMMON_REG_APPS_INT_MASK_APPS_INT_MASK_M \
                                0xFFFFFFFF  // 1= disable corresponding
                                            // interrupt;0 = interrupt enabled

#define COMMON_REG_APPS_INT_MASK_APPS_INT_MASK_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_INT_MASK_SET register.
//
//******************************************************************************
#define COMMON_REG_APPS_INT_MASK_SET_APPS_INT_MASK_SET_M \
                                0xFFFFFFFF  // write 1 to set corresponding bit
                                            // in APPS_INT_MASK;0 = no effect

#define COMMON_REG_APPS_INT_MASK_SET_APPS_INT_MASK_SET_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_INT_MASK_CLR register.
//
//******************************************************************************
#define COMMON_REG_APPS_INT_MASK_CLR_APPS_INT_MASK_CLR_M \
                                0xFFFFFFFF  // write 1 to clear corresponding
                                            // bit in APPS_INT_MASK;0 = no
                                            // effect

#define COMMON_REG_APPS_INT_MASK_CLR_APPS_INT_MASK_CLR_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_INT_STS_CLR register.
//
//******************************************************************************
#define COMMON_REG_APPS_INT_STS_CLR_APPS_INT_STS_CLR_M \
                                0xFFFFFFFF  // write 1 to clear corresponding
                                            // interrupt; 0 = no effect;
                                            // interrupt is not lost if coincide
                                            // with write operation

#define COMMON_REG_APPS_INT_STS_CLR_APPS_INT_STS_CLR_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_INT_ACK register.
//
//******************************************************************************
#define COMMON_REG_APPS_INT_ACK_APPS_INT_ACK_M \
                                0xFFFFFFFF  // write 1 to clear corresponding
                                            // interrupt;0 = no effect

#define COMMON_REG_APPS_INT_ACK_APPS_INT_ACK_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_INT_TRIG register.
//
//******************************************************************************
#define COMMON_REG_APPS_INT_TRIG_APPS_INT_TRIG_M \
                                0xFFFFFFFF  // Writing a 1 to a bit in this
                                            // register causes the the Host CPU
                                            // if enabled (not masked). This
                                            // register is self-clearing.
                                            // Writing 0 has no effect

#define COMMON_REG_APPS_INT_TRIG_APPS_INT_TRIG_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_INT_STS_MASKED register.
//
//******************************************************************************
#define COMMON_REG_APPS_INT_STS_MASKED_APPS_INT_STS_MASKED_M \
                                0xFFFFFFFF  // 1= corresponding interrupt is
                                            // active and not masked. read is
                                            // non-destructive;0 = corresponding
                                            // interrupt is inactive or masked
                                            // by APPS_INT mask

#define COMMON_REG_APPS_INT_STS_MASKED_APPS_INT_STS_MASKED_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_INT_STS_RAW register.
//
//******************************************************************************
#define COMMON_REG_APPS_INT_STS_RAW_APPS_INT_STS_RAW_M \
                                0xFFFFFFFF  // 1= corresponding interrupt is
                                            // active. read is non-destructive;0
                                            // = corresponding interrupt is
                                            // inactive

#define COMMON_REG_APPS_INT_STS_RAW_APPS_INT_STS_RAW_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_IDMEM_TIM_Updated register.
//
//******************************************************************************
#define COMMON_REG_IDMEM_TIM_Updated_TIM_UPDATED \
                                0x00000001  // toggle in this signal
                                            // indicatesIDMEM_TIM_UPDATE
                                            // register mentioned above is
                                            // updated.

//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_APPS_GPIO_TRIG_EN register.
//
//******************************************************************************
#define COMMON_REG_APPS_GPIO_TRIG_EN_APPS_GPIO_TRIG_EN_M \
                                0x0000001F  // APPS GPIO Trigger EN control.
                                            // Bit 0: when '1' enable GPIO 0
                                            // trigger. This bit enables trigger
                                            // for all GPIO 0 pins (GPIO 0 to
                                            // GPIO7). Bit 1: when '1' enable
                                            // GPIO 1 trigger. This bit enables
                                            // trigger for all GPIO 1 pins (
                                            // GPIO8 to GPIO15). Bit 2: when '1'
                                            // enable GPIO 2 trigger. This bit
                                            // enables trigger for all GPIO 2
                                            // pins (GPIO16 to GPIO23). Bit 3:
                                            // when '1' enable GPIO 3 trigger.
                                            // This bit enables trigger for all
                                            // GPIO 3 pins (GPIO24 to GPIO31).
                                            // Bit 4: when '1' enable GPIO 4
                                            // trigger. This bit enables trigger
                                            // for all GPIO 4 pins.(GPIO32 to
                                            // GPIO39)

#define COMMON_REG_APPS_GPIO_TRIG_EN_APPS_GPIO_TRIG_EN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_EMU_DEBUG_REG register.
//
//******************************************************************************
#define COMMON_REG_EMU_DEBUG_REG_EMU_DEBUG_REG_M \
                                0xFFFFFFFF  // 0 th bit used for stalling APPS
                                            // DMA and 1st bit is used for
                                            // stalling NWP DMA for debug
                                            // purpose. Other bits are unused.

#define COMMON_REG_EMU_DEBUG_REG_EMU_DEBUG_REG_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_SEMAPHORE_STATUS2 register.
//
//******************************************************************************
#define COMMON_REG_SEMAPHORE_STATUS2_SEMPAPHORE_STATUS2_M \
                                0x00FFFFFF  // SEMAPHORE STATUS 23:22
                                            // :semaphore status of
                                            // apps_nw_semaphore11 21:20
                                            // :semaphore status of
                                            // apps_nw_semaphore11 19:18
                                            // :semaphore status of
                                            // apps_nw_semaphore10 17:16
                                            // :semaphore status of
                                            // apps_nw_semaphore9 15:14
                                            // :semaphore status of
                                            // apps_nw_semaphore8 13:12
                                            // :semaphore status of
                                            // apps_nw_semaphore7 11:10
                                            // :semaphore status of
                                            // apps_nw_semaphore6 9:8 :semaphore
                                            // status of apps_nw_semaphore5 7:6
                                            // :semaphore status of
                                            // apps_nw_semaphore4 5:4 :semaphore
                                            // status of apps_nw_semaphore3 3:2
                                            // :semaphore status of
                                            // apps_nw_semaphore2 1:0 :semaphore
                                            // status of apps_nw_semaphore1

#define COMMON_REG_SEMAPHORE_STATUS2_SEMPAPHORE_STATUS2_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_SEMAPHORE_PREV_OWNER1 register.
//
//******************************************************************************
#define COMMON_REG_SEMAPHORE_PREV_OWNER1_SEMAPHORE_PREV_OWNER1_M \
                                0x0003FFFF  // 1:0 : prvious owner of
                                            // i2c_properties_reg[1:0] 3:2 :
                                            // prvious owner of
                                            // spi_properties_reg[1:0] 5:4 :
                                            // prvious owner of
                                            // gpio_properties_reg[1:0] 9:8 :
                                            // prvious owner of
                                            // gpio_properties_reg[3:2] 11:10 :
                                            // prvious owner of
                                            // gpio_properties_reg[5:4] 13:12 :
                                            // prvious owner of
                                            // gpio_properties_reg[7:6] 15:14 :
                                            // prvious owner of
                                            // gpio_properties_reg[9:8] 17:16 :
                                            // prvious owner of
                                            // flash_control_reg[1:0]

#define COMMON_REG_SEMAPHORE_PREV_OWNER1_SEMAPHORE_PREV_OWNER1_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// COMMON_REG_O_SEMAPHORE_PREV_OWNER2 register.
//
//******************************************************************************
#define COMMON_REG_SEMAPHORE_PREV_OWNER2_SEMAPHORE_PREV_OWNER2_M \
                                0x00FFFFFF  // 1:0 : previous owner of
                                            // apps_nw_semaphore1_reg[1:0] 3:2 :
                                            // previous owner of
                                            // apps_nw_semaphore2_reg[1:0] 5:4 :
                                            // previous owner of
                                            // apps_nw_semaphore3_reg[1:0] 7:6 :
                                            // previous owner of
                                            // apps_nw_semaphore4_reg[1:0] 9:8 :
                                            // previous owner of
                                            // apps_nw_semaphore5_reg[1:0] 11:10
                                            // : previous owner of
                                            // apps_nw_semaphore6_reg[1:0] 13:12
                                            // : previous owner of
                                            // apps_nw_semaphore7_reg[1:0] 15:14
                                            // : previous owner of
                                            // apps_nw_semaphore8_reg[1:0] 17:16
                                            // : previous owner of
                                            // apps_nw_semaphore9_reg[1:0] 19:18
                                            // : previous owner of
                                            // apps_nw_semaphore10_reg[1:0]
                                            // 21:20 : previous owner of
                                            // apps_nw_semaphore11_reg[1:0]
                                            // 23:22 : previous owner of
                                            // apps_nw_semaphore12_reg[1:0]

#define COMMON_REG_SEMAPHORE_PREV_OWNER2_SEMAPHORE_PREV_OWNER2_S 0



#endif // __HW_COMMON_REG_H__

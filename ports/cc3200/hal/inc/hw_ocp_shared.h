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

#ifndef __HW_OCP_SHARED_H__
#define __HW_OCP_SHARED_H__

//*****************************************************************************
//
// The following are defines for the OCP_SHARED register offsets.
//
//*****************************************************************************
#define OCP_SHARED_O_SEMAPHORE1 0x00000000
#define OCP_SHARED_O_SEMAPHORE2 0x00000004
#define OCP_SHARED_O_SEMAPHORE3 0x00000008
#define OCP_SHARED_O_SEMAPHORE4 0x0000000C
#define OCP_SHARED_O_SEMAPHORE5 0x00000010
#define OCP_SHARED_O_SEMAPHORE6 0x00000014
#define OCP_SHARED_O_SEMAPHORE7 0x00000018
#define OCP_SHARED_O_SEMAPHORE8 0x0000001C
#define OCP_SHARED_O_SEMAPHORE9 0x00000020
#define OCP_SHARED_O_SEMAPHORE10 \
                                0x00000024

#define OCP_SHARED_O_SEMAPHORE11 \
                                0x00000028

#define OCP_SHARED_O_SEMAPHORE12 \
                                0x0000002C

#define OCP_SHARED_O_IC_LOCKER_ID \
                                0x00000030

#define OCP_SHARED_O_MCU_SEMAPHORE_PEND \
                                0x00000034

#define OCP_SHARED_O_WL_SEMAPHORE_PEND \
                                0x00000038

#define OCP_SHARED_O_PLATFORM_DETECTION_RD_ONLY \
                                0x0000003C

#define OCP_SHARED_O_SEMAPHORES_STATUS_RD_ONLY \
                                0x00000040

#define OCP_SHARED_O_CC3XX_CONFIG_CTRL \
                                0x00000044

#define OCP_SHARED_O_CC3XX_SHARED_MEM_SEL_LSB \
                                0x00000048

#define OCP_SHARED_O_CC3XX_SHARED_MEM_SEL_MSB \
                                0x0000004C

#define OCP_SHARED_O_WLAN_ELP_WAKE_EN \
                                0x00000050

#define OCP_SHARED_O_DEVINIT_ROM_START_ADDR \
                                0x00000054

#define OCP_SHARED_O_DEVINIT_ROM_END_ADDR \
                                0x00000058

#define OCP_SHARED_O_SSBD_SEED  0x0000005C
#define OCP_SHARED_O_SSBD_CHK   0x00000060
#define OCP_SHARED_O_SSBD_POLY_SEL \
                                0x00000064

#define OCP_SHARED_O_SPARE_REG_0 \
                                0x00000068

#define OCP_SHARED_O_SPARE_REG_1 \
                                0x0000006C

#define OCP_SHARED_O_SPARE_REG_2 \
                                0x00000070

#define OCP_SHARED_O_SPARE_REG_3 \
                                0x00000074

#define OCP_SHARED_O_GPIO_PAD_CONFIG_0 \
                                0x000000A0

#define OCP_SHARED_O_GPIO_PAD_CONFIG_1 \
                                0x000000A4

#define OCP_SHARED_O_GPIO_PAD_CONFIG_2 \
                                0x000000A8

#define OCP_SHARED_O_GPIO_PAD_CONFIG_3 \
                                0x000000AC

#define OCP_SHARED_O_GPIO_PAD_CONFIG_4 \
                                0x000000B0

#define OCP_SHARED_O_GPIO_PAD_CONFIG_5 \
                                0x000000B4

#define OCP_SHARED_O_GPIO_PAD_CONFIG_6 \
                                0x000000B8

#define OCP_SHARED_O_GPIO_PAD_CONFIG_7 \
                                0x000000BC

#define OCP_SHARED_O_GPIO_PAD_CONFIG_8 \
                                0x000000C0

#define OCP_SHARED_O_GPIO_PAD_CONFIG_9 \
                                0x000000C4

#define OCP_SHARED_O_GPIO_PAD_CONFIG_10 \
                                0x000000C8

#define OCP_SHARED_O_GPIO_PAD_CONFIG_11 \
                                0x000000CC

#define OCP_SHARED_O_GPIO_PAD_CONFIG_12 \
                                0x000000D0

#define OCP_SHARED_O_GPIO_PAD_CONFIG_13 \
                                0x000000D4

#define OCP_SHARED_O_GPIO_PAD_CONFIG_14 \
                                0x000000D8

#define OCP_SHARED_O_GPIO_PAD_CONFIG_15 \
                                0x000000DC

#define OCP_SHARED_O_GPIO_PAD_CONFIG_16 \
                                0x000000E0

#define OCP_SHARED_O_GPIO_PAD_CONFIG_17 \
                                0x000000E4

#define OCP_SHARED_O_GPIO_PAD_CONFIG_18 \
                                0x000000E8

#define OCP_SHARED_O_GPIO_PAD_CONFIG_19 \
                                0x000000EC

#define OCP_SHARED_O_GPIO_PAD_CONFIG_20 \
                                0x000000F0

#define OCP_SHARED_O_GPIO_PAD_CONFIG_21 \
                                0x000000F4

#define OCP_SHARED_O_GPIO_PAD_CONFIG_22 \
                                0x000000F8

#define OCP_SHARED_O_GPIO_PAD_CONFIG_23 \
                                0x000000FC

#define OCP_SHARED_O_GPIO_PAD_CONFIG_24 \
                                0x00000100

#define OCP_SHARED_O_GPIO_PAD_CONFIG_25 \
                                0x00000104

#define OCP_SHARED_O_GPIO_PAD_CONFIG_26 \
                                0x00000108

#define OCP_SHARED_O_GPIO_PAD_CONFIG_27 \
                                0x0000010C

#define OCP_SHARED_O_GPIO_PAD_CONFIG_28 \
                                0x00000110

#define OCP_SHARED_O_GPIO_PAD_CONFIG_29 \
                                0x00000114

#define OCP_SHARED_O_GPIO_PAD_CONFIG_30 \
                                0x00000118

#define OCP_SHARED_O_GPIO_PAD_CONFIG_31 \
                                0x0000011C

#define OCP_SHARED_O_GPIO_PAD_CONFIG_32 \
                                0x00000120

#define OCP_SHARED_O_GPIO_PAD_CONFIG_33 \
                                0x00000124

#define OCP_SHARED_O_GPIO_PAD_CONFIG_34 \
                                0x00000128

#define OCP_SHARED_O_GPIO_PAD_CONFIG_35 \
                                0x0000012C

#define OCP_SHARED_O_GPIO_PAD_CONFIG_36 \
                                0x00000130

#define OCP_SHARED_O_GPIO_PAD_CONFIG_37 \
                                0x00000134

#define OCP_SHARED_O_GPIO_PAD_CONFIG_38 \
                                0x00000138

#define OCP_SHARED_O_GPIO_PAD_CONFIG_39 \
                                0x0000013C

#define OCP_SHARED_O_GPIO_PAD_CONFIG_40 \
                                0x00000140

#define OCP_SHARED_O_GPIO_PAD_CMN_CONFIG \
                                0x00000144  // This register provide control to
                                            // GPIO_CC3XXV1 IO PAD. Common
                                            // control signals to all bottom Die
                                            // IO's are controlled via this.

#define OCP_SHARED_O_D2D_DEV_PAD_CMN_CONFIG \
                                0x00000148

#define OCP_SHARED_O_D2D_TOSTACK_PAD_CONF \
                                0x0000014C

#define OCP_SHARED_O_D2D_MISC_PAD_CONF \
                                0x00000150

#define OCP_SHARED_O_SOP_CONF_OVERRIDE \
                                0x00000154

#define OCP_SHARED_O_CC3XX_DEBUGSS_STATUS \
                                0x00000158

#define OCP_SHARED_O_CC3XX_DEBUGMUX_SEL \
                                0x0000015C

#define OCP_SHARED_O_ALT_PC_VAL_NW \
                                0x00000160

#define OCP_SHARED_O_ALT_PC_VAL_APPS \
                                0x00000164

#define OCP_SHARED_O_SPARE_REG_4 \
                                0x00000168

#define OCP_SHARED_O_SPARE_REG_5 \
                                0x0000016C

#define OCP_SHARED_O_SH_SPI_CS_MASK \
                                0x00000170

#define OCP_SHARED_O_CC3XX_DEVICE_TYPE \
                                0x00000174

#define OCP_SHARED_O_MEM_TOPMUXCTRL_IFORCE \
                                0x00000178

#define OCP_SHARED_O_CC3XX_DEV_PACKAGE_DETECT \
                                0x0000017C

#define OCP_SHARED_O_AUTONMS_SPICLK_SEL \
                                0x00000180

#define OCP_SHARED_O_CC3XX_DEV_PADCONF \
                                0x00000184

#define OCP_SHARED_O_SPARE_REG_8 \
                                0x00000188

#define OCP_SHARED_O_SPARE_REG_6 \
                                0x0000018C

#define OCP_SHARED_O_SPARE_REG_7 \
                                0x00000190

#define OCP_SHARED_O_APPS_WLAN_ORBIT \
                                0x00000194

#define OCP_SHARED_O_APPS_WLAN_SCRATCH_PAD \
                                0x00000198




//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SEMAPHORE1 register.
//
//******************************************************************************
#define OCP_SHARED_SEMAPHORE1_MEM_SEMAPHORE1_M \
                                0x00000003  // General Purpose Semaphore for SW
                                            // Usage. If any of the 2 bits of a
                                            // given register is set to 1, it
                                            // means that the semaphore is
                                            // locked by one of the masters.
                                            // Each bit represents a master IP
                                            // as follows: {WLAN,NWP}. The JTAG
                                            // cannot capture the semaphore but
                                            // it can release it. As a master IP
                                            // reads the semaphore, it will be
                                            // caputed and the masters
                                            // correlating bit will be set to 1
                                            // (set upon read). As any IP writes
                                            // to this address (independent of
                                            // the written data) the semaphore
                                            // will be set to 2'b00.

#define OCP_SHARED_SEMAPHORE1_MEM_SEMAPHORE1_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SEMAPHORE2 register.
//
//******************************************************************************
#define OCP_SHARED_SEMAPHORE2_MEM_SEMAPHORE2_M \
                                0x00000003  // General Purpose Semaphore for SW
                                            // Usage. If any of the 2 bits of a
                                            // given register is set to 1, it
                                            // means that the semaphore is
                                            // locked by one of the masters.
                                            // Each bit represents a master IP
                                            // as follows: {WLAN,NWP}. The JTAG
                                            // cannot capture the semaphore but
                                            // it can release it. As a master IP
                                            // reads the semaphore, it will be
                                            // caputed and the masters
                                            // correlating bit will be set to 1
                                            // (set upon read). As any IP writes
                                            // to this address (independent of
                                            // the written data) the semaphore
                                            // will be set to 2'b00.

#define OCP_SHARED_SEMAPHORE2_MEM_SEMAPHORE2_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SEMAPHORE3 register.
//
//******************************************************************************
#define OCP_SHARED_SEMAPHORE3_MEM_SEMAPHORE3_M \
                                0x00000003  // General Purpose Semaphore for SW
                                            // Usage. If any of the 2 bits of a
                                            // given register is set to 1, it
                                            // means that the semaphore is
                                            // locked by one of the masters.
                                            // Each bit represents a master IP
                                            // as follows: {WLAN,NWP}. The JTAG
                                            // cannot capture the semaphore but
                                            // it can release it. As a master IP
                                            // reads the semaphore, it will be
                                            // caputed and the masters
                                            // correlating bit will be set to 1
                                            // (set upon read). As any IP writes
                                            // to this address (independent of
                                            // the written data) the semaphore
                                            // will be set to 2'b00.

#define OCP_SHARED_SEMAPHORE3_MEM_SEMAPHORE3_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SEMAPHORE4 register.
//
//******************************************************************************
#define OCP_SHARED_SEMAPHORE4_MEM_SEMAPHORE4_M \
                                0x00000003  // General Purpose Semaphore for SW
                                            // Usage. If any of the 2 bits of a
                                            // given register is set to 1, it
                                            // means that the semaphore is
                                            // locked by one of the masters.
                                            // Each bit represents a master IP
                                            // as follows: {WLAN,NWP}. The JTAG
                                            // cannot capture the semaphore but
                                            // it can release it. As a master IP
                                            // reads the semaphore, it will be
                                            // caputed and the masters
                                            // correlating bit will be set to 1
                                            // (set upon read). As any IP writes
                                            // to this address (independent of
                                            // the written data) the semaphore
                                            // will be set to 2'b00.

#define OCP_SHARED_SEMAPHORE4_MEM_SEMAPHORE4_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SEMAPHORE5 register.
//
//******************************************************************************
#define OCP_SHARED_SEMAPHORE5_MEM_SEMAPHORE5_M \
                                0x00000003  // General Purpose Semaphore for SW
                                            // Usage. If any of the 2 bits of a
                                            // given register is set to 1, it
                                            // means that the semaphore is
                                            // locked by one of the masters.
                                            // Each bit represents a master IP
                                            // as follows: {WLAN,NWP}. The JTAG
                                            // cannot capture the semaphore but
                                            // it can release it. As a master IP
                                            // reads the semaphore, it will be
                                            // caputed and the masters
                                            // correlating bit will be set to 1
                                            // (set upon read). As any IP writes
                                            // to this address (independent of
                                            // the written data) the semaphore
                                            // will be set to 2'b00.

#define OCP_SHARED_SEMAPHORE5_MEM_SEMAPHORE5_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SEMAPHORE6 register.
//
//******************************************************************************
#define OCP_SHARED_SEMAPHORE6_MEM_SEMAPHORE6_M \
                                0x00000003  // General Purpose Semaphore for SW
                                            // Usage. If any of the 2 bits of a
                                            // given register is set to 1, it
                                            // means that the semaphore is
                                            // locked by one of the masters.
                                            // Each bit represents a master IP
                                            // as follows: {WLAN,NWP}. The JTAG
                                            // cannot capture the semaphore but
                                            // it can release it. As a master IP
                                            // reads the semaphore, it will be
                                            // caputed and the masters
                                            // correlating bit will be set to 1
                                            // (set upon read). As any IP writes
                                            // to this address (independent of
                                            // the written data) the semaphore
                                            // will be set to 2'b00.

#define OCP_SHARED_SEMAPHORE6_MEM_SEMAPHORE6_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SEMAPHORE7 register.
//
//******************************************************************************
#define OCP_SHARED_SEMAPHORE7_MEM_SEMAPHORE7_M \
                                0x00000003  // General Purpose Semaphore for SW
                                            // Usage. If any of the 2 bits of a
                                            // given register is set to 1, it
                                            // means that the semaphore is
                                            // locked by one of the masters.
                                            // Each bit represents a master IP
                                            // as follows: {WLAN,NWP}. The JTAG
                                            // cannot capture the semaphore but
                                            // it can release it. As a master IP
                                            // reads the semaphore, it will be
                                            // caputed and the masters
                                            // correlating bit will be set to 1
                                            // (set upon read). As any IP writes
                                            // to this address (independent of
                                            // the written data) the semaphore
                                            // will be set to 2'b00.

#define OCP_SHARED_SEMAPHORE7_MEM_SEMAPHORE7_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SEMAPHORE8 register.
//
//******************************************************************************
#define OCP_SHARED_SEMAPHORE8_MEM_SEMAPHORE8_M \
                                0x00000003  // General Purpose Semaphore for SW
                                            // Usage. If any of the 2 bits of a
                                            // given register is set to 1, it
                                            // means that the semaphore is
                                            // locked by one of the masters.
                                            // Each bit represents a master IP
                                            // as follows: {WLAN,NWP}. The JTAG
                                            // cannot capture the semaphore but
                                            // it can release it. As a master IP
                                            // reads the semaphore, it will be
                                            // caputed and the masters
                                            // correlating bit will be set to 1
                                            // (set upon read). As any IP writes
                                            // to this address (independent of
                                            // the written data) the semaphore
                                            // will be set to 2'b00.

#define OCP_SHARED_SEMAPHORE8_MEM_SEMAPHORE8_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SEMAPHORE9 register.
//
//******************************************************************************
#define OCP_SHARED_SEMAPHORE9_MEM_SEMAPHORE9_M \
                                0x00000003  // General Purpose Semaphore for SW
                                            // Usage. If any of the 2 bits of a
                                            // given register is set to 1, it
                                            // means that the semaphore is
                                            // locked by one of the masters.
                                            // Each bit represents a master IP
                                            // as follows: {WLAN,NWP}. The JTAG
                                            // cannot capture the semaphore but
                                            // it can release it. As a master IP
                                            // reads the semaphore, it will be
                                            // caputed and the masters
                                            // correlating bit will be set to 1
                                            // (set upon read). As any IP writes
                                            // to this address (independent of
                                            // the written data) the semaphore
                                            // will be set to 2'b00.

#define OCP_SHARED_SEMAPHORE9_MEM_SEMAPHORE9_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SEMAPHORE10 register.
//
//******************************************************************************
#define OCP_SHARED_SEMAPHORE10_MEM_SEMAPHORE10_M \
                                0x00000003  // General Purpose Semaphore for SW
                                            // Usage. If any of the 2 bits of a
                                            // given register is set to 1, it
                                            // means that the semaphore is
                                            // locked by one of the masters.
                                            // Each bit represents a master IP
                                            // as follows: {WLAN,NWP}. The JTAG
                                            // cannot capture the semaphore but
                                            // it can release it. As a master IP
                                            // reads the semaphore, it will be
                                            // caputed and the masters
                                            // correlating bit will be set to 1
                                            // (set upon read). As any IP writes
                                            // to this address (independent of
                                            // the written data) the semaphore
                                            // will be set to 2'b00.

#define OCP_SHARED_SEMAPHORE10_MEM_SEMAPHORE10_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SEMAPHORE11 register.
//
//******************************************************************************
#define OCP_SHARED_SEMAPHORE11_MEM_SEMAPHORE11_M \
                                0x00000003  // General Purpose Semaphore for SW
                                            // Usage. If any of the 2 bits of a
                                            // given register is set to 1, it
                                            // means that the semaphore is
                                            // locked by one of the masters.
                                            // Each bit represents a master IP
                                            // as follows: {WLAN,NWP}. The JTAG
                                            // cannot capture the semaphore but
                                            // it can release it. As a master IP
                                            // reads the semaphore, it will be
                                            // caputed and the masters
                                            // correlating bit will be set to 1
                                            // (set upon read). As any IP writes
                                            // to this address (independent of
                                            // the written data) the semaphore
                                            // will be set to 2'b00.

#define OCP_SHARED_SEMAPHORE11_MEM_SEMAPHORE11_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SEMAPHORE12 register.
//
//******************************************************************************
#define OCP_SHARED_SEMAPHORE12_MEM_SEMAPHORE12_M \
                                0x00000003  // General Purpose Semaphore for SW
                                            // Usage. If any of the 2 bits of a
                                            // given register is set to 1, it
                                            // means that the semaphore is
                                            // locked by one of the masters.
                                            // Each bit represents a master IP
                                            // as follows: {WLAN,NWP}. The JTAG
                                            // cannot capture the semaphore but
                                            // it can release it. As a master IP
                                            // reads the semaphore, it will be
                                            // caputed and the masters
                                            // correlating bit will be set to 1
                                            // (set upon read). As any IP writes
                                            // to this address (independent of
                                            // the written data) the semaphore
                                            // will be set to 2'b00.

#define OCP_SHARED_SEMAPHORE12_MEM_SEMAPHORE12_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_IC_LOCKER_ID register.
//
//******************************************************************************
#define OCP_SHARED_IC_LOCKER_ID_MEM_IC_LOCKER_ID_M \
                                0x00000007  // This register is used for
                                            // allowing only one master OCP to
                                            // perform write transactions to the
                                            // OCP slaves. Each bit represents
                                            // an IP in the following format: {
                                            // JTAG,WLAN, NWP mcu}. As any of
                                            // the bits is set to one, the
                                            // correlating IP is preventing the
                                            // other IP's from performing write
                                            // transactions to the slaves. As
                                            // the Inter Connect is locked, the
                                            // only the locking IP can write to
                                            // the register and by that
                                            // releasing the lock. 3'b000 => IC
                                            // is not locked. 3'b001 => IC is
                                            // locked by NWP mcu. 3'b010 => IC
                                            // is locked by WLAN. 3'b100 => IC
                                            // is locked by JTAG.

#define OCP_SHARED_IC_LOCKER_ID_MEM_IC_LOCKER_ID_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_MCU_SEMAPHORE_PEND register.
//
//******************************************************************************
#define OCP_SHARED_MCU_SEMAPHORE_PEND_MEM_MCU_SEMAPHORE_PEND_M \
                                0x0000FFFF  // This register specifies the
                                            // semaphore for which the NWP mcu
                                            // is waiting to be released. It is
                                            // set to the serial number of a
                                            // given locked semaphore after it
                                            // was read by the NWP mcu. Only
                                            // [11:0] is used.

#define OCP_SHARED_MCU_SEMAPHORE_PEND_MEM_MCU_SEMAPHORE_PEND_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_WL_SEMAPHORE_PEND register.
//
//******************************************************************************
#define OCP_SHARED_WL_SEMAPHORE_PEND_MEM_WL_SEMAPHORE_PEND_M \
                                0x0000FFFF  // This register specifies the
                                            // semaphore for which the WLAN is
                                            // waiting to be released. It is set
                                            // to the serial number of a given
                                            // locked semaphore after it was
                                            // read by the WLAN. Only [11:0] is
                                            // used.

#define OCP_SHARED_WL_SEMAPHORE_PEND_MEM_WL_SEMAPHORE_PEND_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_PLATFORM_DETECTION_RD_ONLY register.
//
//******************************************************************************
#define OCP_SHARED_PLATFORM_DETECTION_RD_ONLY_PLATFORM_DETECTION_M \
                                0x0000FFFF  // This information serves the IPs
                                            // for knowing in which platform are
                                            // they integrated at: 0 = CC31XX.

#define OCP_SHARED_PLATFORM_DETECTION_RD_ONLY_PLATFORM_DETECTION_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SEMAPHORES_STATUS_RD_ONLY register.
//
//******************************************************************************
#define OCP_SHARED_SEMAPHORES_STATUS_RD_ONLY_SEMAPHORES_STATUS_M \
                                0x00000FFF  // Captured/released semaphores
                                            // status for the 12 semaphores.
                                            // Each bit of the 12 bits
                                            // represents a semaphore. 0 =>
                                            // Semaphore Free. 1 => Semaphore
                                            // Captured.

#define OCP_SHARED_SEMAPHORES_STATUS_RD_ONLY_SEMAPHORES_STATUS_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_CC3XX_CONFIG_CTRL register.
//
//******************************************************************************
#define OCP_SHARED_CC3XX_CONFIG_CTRL_MEM_IC_TO_EN \
                                0x00000010  // This bit is used to enable
                                            // timeout mechanism for top_ocp_ic
                                            // (for debug puropse). When 1 value
                                            // , in case any ocp slave doesn't
                                            // give sresponse within 16 cylcles
                                            // top_ic will give error response
                                            // itself to avoid bus hange.

#define OCP_SHARED_CC3XX_CONFIG_CTRL_MEM_ALT_PC_EN_APPS \
                                0x00000008  // 1 bit should be accessible only
                                            // in devinit. This will enable 0x4
                                            // hack for apps processor

#define OCP_SHARED_CC3XX_CONFIG_CTRL_MEM_ALT_PC_EN_NW \
                                0x00000004  // 1 bit, should be accessible only
                                            // in devinit. This will enable 0x4
                                            // hack for nw processor

#define OCP_SHARED_CC3XX_CONFIG_CTRL_MEM_EXTEND_NW_ROM \
                                0x00000002  // When set NW can take over apps
                                            // rom and flash via IDCODE bus.
                                            // Apps will able to access this
                                            // register only during devinit and
                                            // reset value should be 0.

#define OCP_SHARED_CC3XX_CONFIG_CTRL_MEM_WLAN_HOST_INTF_SEL \
                                0x00000001  // When this bit is set to 0 WPSI
                                            // host interface wil be selected,
                                            // when this bit is set to 1 , WLAN
                                            // host async bridge will be
                                            // selected.

//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_CC3XX_SHARED_MEM_SEL_LSB register.
//
//******************************************************************************
#define OCP_SHARED_CC3XX_SHARED_MEM_SEL_LSB_MEM_SHARED_MEM_SEL_LSB_M \
                                0x3FFFFFFF  // This register provides memss RAM
                                            // column configuration for column 0
                                            // to 9. 3 bits are allocated per
                                            // column. This register is required
                                            // to be configured before starting
                                            // RAM access. Changing register
                                            // setting while code is running
                                            // will result into unpredictable
                                            // memory behaviour. Register is
                                            // supported to configured ones
                                            // after core is booted up. 3 bit
                                            // encoding per column is as
                                            // follows: when 000 : WLAN, 001:
                                            // NWP, 010: APPS, 011: PHY, 100:
                                            // OCLA column 0 select: bit [2:0]
                                            // :when 000 -> WLAN,001 -> NWP,010
                                            // -> APPS, 011 -> PHY, 100 -> OCLA
                                            // column 1 select: bit [5:3]
                                            // :column 2 select: bit [8 : 6]:
                                            // column 3 select : bit [11: 9]
                                            // column 4 select : bit [14:12]
                                            // column 5 select : bit [17:15]
                                            // column 6 select : bit [20:18]
                                            // column 7 select : bit [23:21]
                                            // column 8 select : bit [26:24]
                                            // column 9 select : bit [29:27]
                                            // column 10 select

#define OCP_SHARED_CC3XX_SHARED_MEM_SEL_LSB_MEM_SHARED_MEM_SEL_LSB_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_CC3XX_SHARED_MEM_SEL_MSB register.
//
//******************************************************************************
#define OCP_SHARED_CC3XX_SHARED_MEM_SEL_MSB_MEM_SHARED_MEM_SEL_MSB_M \
                                0x00000FFF  // This register provides memss RAM
                                            // column configuration for column
                                            // 10 to 15. 3 bits are allocated
                                            // per column. This register is
                                            // required to be configured before
                                            // starting RAM access. Changing
                                            // register setting while code is
                                            // running will result into
                                            // unpredictable memory behaviour.
                                            // Register is supported to
                                            // configured ones after core is
                                            // booted up. 3 bit encoding per
                                            // column is as follows: when 000 :
                                            // WLAN, 001: NWP, 010: APPS, 011:
                                            // PHY, 100: OCLA column 11 select :
                                            // bit [2:0] column 12 select : bit
                                            // [5:3] column 13 select : bit [8 :
                                            // 6] column 14 select :

#define OCP_SHARED_CC3XX_SHARED_MEM_SEL_MSB_MEM_SHARED_MEM_SEL_MSB_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_WLAN_ELP_WAKE_EN register.
//
//******************************************************************************
#define OCP_SHARED_WLAN_ELP_WAKE_EN_MEM_WLAN_ELP_WAKE_EN \
                                0x00000001  // when '1' : signal will enabled
                                            // ELP power doamin when '0': ELP is
                                            // not powered up.

//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_DEVINIT_ROM_START_ADDR register.
//
//******************************************************************************
#define OCP_SHARED_DEVINIT_ROM_START_ADDR_MEM_DEVINIT_ROM_START_ADDR_M \
                                0xFFFFFFFF  // 32 bit, Writable only during
                                            // devinit, and whole 32 bit should
                                            // be output of the config register
                                            // module. This register is not used
                                            // , similar register availble in
                                            // GPRCM space.

#define OCP_SHARED_DEVINIT_ROM_START_ADDR_MEM_DEVINIT_ROM_START_ADDR_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_DEVINIT_ROM_END_ADDR register.
//
//******************************************************************************
#define OCP_SHARED_DEVINIT_ROM_END_ADDR_MEM_DEVINIT_ROM_END_ADDR_M \
                                0xFFFFFFFF  // 32 bit, Writable only during
                                            // devinit, and whole 32 bit should
                                            // be output of the config register
                                            // module.

#define OCP_SHARED_DEVINIT_ROM_END_ADDR_MEM_DEVINIT_ROM_END_ADDR_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SSBD_SEED register.
//
//******************************************************************************
#define OCP_SHARED_SSBD_SEED_MEM_SSBD_SEED_M \
                                0xFFFFFFFF  // 32 bit, Writable only during
                                            // devinit, and whole 32 bit should
                                            // be output of the config register
                                            // module.

#define OCP_SHARED_SSBD_SEED_MEM_SSBD_SEED_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SSBD_CHK register.
//
//******************************************************************************
#define OCP_SHARED_SSBD_CHK_MEM_SSBD_CHK_M \
                                0xFFFFFFFF  // 32 bit, Writable only during
                                            // devinit, and whole 32 bit should
                                            // be output of the config register
                                            // module.

#define OCP_SHARED_SSBD_CHK_MEM_SSBD_CHK_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SSBD_POLY_SEL register.
//
//******************************************************************************
#define OCP_SHARED_SSBD_POLY_SEL_MEM_SSBD_POLY_SEL_M \
                                0x00000003  // 2 bit, Writable only during
                                            // devinit, and whole 2 bit should
                                            // be output of the config register
                                            // module.

#define OCP_SHARED_SSBD_POLY_SEL_MEM_SSBD_POLY_SEL_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SPARE_REG_0 register.
//
//******************************************************************************
#define OCP_SHARED_SPARE_REG_0_MEM_SPARE_REG_0_M \
                                0xFFFFFFFF  // Devinit code should look for
                                            // whether corresponding fuse is
                                            // blown and if blown write to the
                                            // 11th bit of this register to
                                            // disable flshtst interface

#define OCP_SHARED_SPARE_REG_0_MEM_SPARE_REG_0_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SPARE_REG_1 register.
//
//******************************************************************************
#define OCP_SHARED_SPARE_REG_1_MEM_SPARE_REG_1_M \
                                0xFFFFFFFF  // NWP Software register

#define OCP_SHARED_SPARE_REG_1_MEM_SPARE_REG_1_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SPARE_REG_2 register.
//
//******************************************************************************
#define OCP_SHARED_SPARE_REG_2_MEM_SPARE_REG_2_M \
                                0xFFFFFFFF  // NWP Software register

#define OCP_SHARED_SPARE_REG_2_MEM_SPARE_REG_2_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SPARE_REG_3 register.
//
//******************************************************************************
#define OCP_SHARED_SPARE_REG_3_MEM_SPARE_REG_3_M \
                                0xFFFFFFFF  // APPS Software register

#define OCP_SHARED_SPARE_REG_3_MEM_SPARE_REG_3_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_0 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_0_MEM_GPIO_PAD_CONFIG_0_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." "For example in
                                            // case of I2C Value gets latched at
                                            // rising edge of RET33.""" """ 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_0_MEM_GPIO_PAD_CONFIG_0_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_1 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_1_MEM_GPIO_PAD_CONFIG_1_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_1_MEM_GPIO_PAD_CONFIG_1_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_2 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_2_MEM_GPIO_PAD_CONFIG_2_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_2_MEM_GPIO_PAD_CONFIG_2_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_3 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_3_MEM_GPIO_PAD_CONFIG_3_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_3_MEM_GPIO_PAD_CONFIG_3_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_4 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_4_MEM_GPIO_PAD_CONFIG_4_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_4_MEM_GPIO_PAD_CONFIG_4_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_5 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_5_MEM_GPIO_PAD_CONFIG_5_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_5_MEM_GPIO_PAD_CONFIG_5_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_6 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_6_MEM_GPIO_PAD_CONFIG_6_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_6_MEM_GPIO_PAD_CONFIG_6_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_7 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_7_MEM_GPIO_PAD_CONFIG_7_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_7_MEM_GPIO_PAD_CONFIG_7_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_8 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_8_MEM_GPIO_PAD_CONFIG_8_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_8_MEM_GPIO_PAD_CONFIG_8_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_9 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_9_MEM_GPIO_PAD_CONFIG_9_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_9_MEM_GPIO_PAD_CONFIG_9_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_10 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_10_MEM_GPIO_PAD_CONFIG_10_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_10_MEM_GPIO_PAD_CONFIG_10_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_11 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_11_MEM_GPIO_PAD_CONFIG_11_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_11_MEM_GPIO_PAD_CONFIG_11_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_12 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_12_MEM_GPIO_PAD_CONFIG_12_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_12_MEM_GPIO_PAD_CONFIG_12_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_13 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_13_MEM_GPIO_PAD_CONFIG_13_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_13_MEM_GPIO_PAD_CONFIG_13_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_14 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_14_MEM_GPIO_PAD_CONFIG_14_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_14_MEM_GPIO_PAD_CONFIG_14_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_15 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_15_MEM_GPIO_PAD_CONFIG_15_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_15_MEM_GPIO_PAD_CONFIG_15_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_16 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_16_MEM_GPIO_PAD_CONFIG_16_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_16_MEM_GPIO_PAD_CONFIG_16_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_17 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_17_MEM_GPIO_PAD_CONFIG_17_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_17_MEM_GPIO_PAD_CONFIG_17_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_18 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_18_MEM_GPIO_PAD_CONFIG_18_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_18_MEM_GPIO_PAD_CONFIG_18_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_19 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_19_MEM_GPIO_PAD_CONFIG_19_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_19_MEM_GPIO_PAD_CONFIG_19_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_20 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_20_MEM_GPIO_PAD_CONFIG_20_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_20_MEM_GPIO_PAD_CONFIG_20_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_21 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_21_MEM_GPIO_PAD_CONFIG_21_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_21_MEM_GPIO_PAD_CONFIG_21_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_22 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_22_MEM_GPIO_PAD_CONFIG_22_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_22_MEM_GPIO_PAD_CONFIG_22_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_23 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_23_MEM_GPIO_PAD_CONFIG_23_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_23_MEM_GPIO_PAD_CONFIG_23_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_24 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_24_MEM_GPIO_PAD_CONFIG_24_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_24_MEM_GPIO_PAD_CONFIG_24_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_25 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_25_MEM_GPIO_PAD_CONFIG_25_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_25_MEM_GPIO_PAD_CONFIG_25_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_26 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_26_MEM_GPIO_PAD_CONFIG_26_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_26_MEM_GPIO_PAD_CONFIG_26_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_27 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_27_MEM_GPIO_PAD_CONFIG_27_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_27_MEM_GPIO_PAD_CONFIG_27_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_28 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_28_MEM_GPIO_PAD_CONFIG_28_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_28_MEM_GPIO_PAD_CONFIG_28_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_29 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_29_MEM_GPIO_PAD_CONFIG_29_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_29_MEM_GPIO_PAD_CONFIG_29_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_30 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_30_MEM_GPIO_PAD_CONFIG_30_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_30_MEM_GPIO_PAD_CONFIG_30_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_31 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_31_MEM_GPIO_PAD_CONFIG_31_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_31_MEM_GPIO_PAD_CONFIG_31_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_32 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_32_MEM_GPIO_PAD_CONFIG_32_M \
                                0x00000FFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." it can be used
                                            // for I2C type of peripherals. 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_32_MEM_GPIO_PAD_CONFIG_32_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_33 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_33_MEM_GPIO_PAD_CONFIG_33_M \
                                0x0000003F  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 5 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'. IODEN and I8MAEN
                                            // is diesabled for all development
                                            // IO's. These signals are tied to
                                            // logic level '0'. common control
                                            // is implemented for I2MAEN,
                                            // I4MAEN, WKPU, WKPD control .
                                            // refer dev_pad_cmn_config register
                                            // bits.

#define OCP_SHARED_GPIO_PAD_CONFIG_33_MEM_GPIO_PAD_CONFIG_33_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_34 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_34_MEM_GPIO_PAD_CONFIG_34_M \
                                0x0000003F  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 5 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'. IODEN and I8MAEN
                                            // is diesabled for all development
                                            // IO's. These signals are tied to
                                            // logic level '0'. common control
                                            // is implemented for I2MAEN,
                                            // I4MAEN, WKPU, WKPD control .
                                            // refer dev_pad_cmn_config register
                                            // bits.

#define OCP_SHARED_GPIO_PAD_CONFIG_34_MEM_GPIO_PAD_CONFIG_34_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_35 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_35_MEM_GPIO_PAD_CONFIG_35_M \
                                0x0000003F  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 5 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'. IODEN and I8MAEN
                                            // is diesabled for all development
                                            // IO's. These signals are tied to
                                            // logic level '0'. common control
                                            // is implemented for I2MAEN,
                                            // I4MAEN, WKPU, WKPD control .
                                            // refer dev_pad_cmn_config register
                                            // bits.

#define OCP_SHARED_GPIO_PAD_CONFIG_35_MEM_GPIO_PAD_CONFIG_35_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_36 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_36_MEM_GPIO_PAD_CONFIG_36_M \
                                0x0000003F  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 5 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'. IODEN and I8MAEN
                                            // is diesabled for all development
                                            // IO's. These signals are tied to
                                            // logic level '0'. common control
                                            // is implemented for I2MAEN,
                                            // I4MAEN, WKPU, WKPD control .
                                            // refer dev_pad_cmn_config register
                                            // bits.

#define OCP_SHARED_GPIO_PAD_CONFIG_36_MEM_GPIO_PAD_CONFIG_36_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_37 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_37_MEM_GPIO_PAD_CONFIG_37_M \
                                0x0000003F  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 5 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'. IODEN and I8MAEN
                                            // is diesabled for all development
                                            // IO's. These signals are tied to
                                            // logic level '0'. common control
                                            // is implemented for I2MAEN,
                                            // I4MAEN, WKPU, WKPD control .
                                            // refer dev_pad_cmn_config register
                                            // bits.

#define OCP_SHARED_GPIO_PAD_CONFIG_37_MEM_GPIO_PAD_CONFIG_37_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_38 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_38_MEM_GPIO_PAD_CONFIG_38_M \
                                0x0000003F  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 5 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'. IODEN and I8MAEN
                                            // is diesabled for all development
                                            // IO's. These signals are tied to
                                            // logic level '0'. common control
                                            // is implemented for I2MAEN,
                                            // I4MAEN, WKPU, WKPD control .
                                            // refer dev_pad_cmn_config register
                                            // bits.

#define OCP_SHARED_GPIO_PAD_CONFIG_38_MEM_GPIO_PAD_CONFIG_38_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_39 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_39_MEM_GPIO_PAD_CONFIG_39_M \
                                0x0000003F  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 5 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'. IODEN and I8MAEN
                                            // is diesabled for all development
                                            // IO's. These signals are tied to
                                            // logic level '0'. common control
                                            // is implemented for I2MAEN,
                                            // I4MAEN, WKPU, WKPD control .
                                            // refer dev_pad_cmn_config register
                                            // bits.

#define OCP_SHARED_GPIO_PAD_CONFIG_39_MEM_GPIO_PAD_CONFIG_39_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CONFIG_40 register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CONFIG_40_MEM_GPIO_PAD_CONFIG_40_M \
                                0x0007FFFF  // GPIO 0 register: "Bit 0 - 3 is
                                            // used for PAD IO mode selection.
                                            // io_register={ "" 0 =>
                                            // """"CONFMODE[0]"""""" "" 1 =>
                                            // """"CONFMODE[1]"""""" "" 2 =>
                                            // """"CONFMODE[2]"""""" "" 3 =>
                                            // """"CONFMODE[3]"""" 4 =>
                                            // """"IODEN"""" --> When level ‘1’
                                            // this disables the PMOS xtors of
                                            // the output stages making them
                                            // open-drain type." "For example in
                                            // case of I2C Value gets latched at
                                            // rising edge of RET33.""" """ 5 =>
                                            // """"I2MAEN"""" --> Level ‘1’
                                            // enables the approx 2mA output
                                            // stage""" """ 6 => """"I4MAEN""""
                                            // --> Level ‘1’ enables the approx
                                            // 4mA output stage""" """ 7 =>
                                            // """"I8MAEN"""" --> Level ‘1’
                                            // enables the approx 8mA output
                                            // stage. Note: any drive strength
                                            // between 2mA and 14mA can be
                                            // obtained with combination of 2mA
                                            // 4mA and 8mA.""" """ 8 =>
                                            // """"IWKPUEN"""" --> 10uA pull up
                                            // (weak strength)""" """ 9 =>
                                            // """"IWKPDEN"""" --> 10uA pull
                                            // down (weak strength)""" """ 10 =>
                                            // """"IOE_N"""" --> output enable
                                            // value. level ‘0’ enables the IDO
                                            // to PAD path. Else PAD is
                                            // tristated (except for the PU/PD
                                            // which are independent)." "Value
                                            // gets latched at rising edge of
                                            // RET33""" """ 11 =>""""
                                            // IOE_N_OV"""" --> output enable
                                            // overirde. when bit is set to
                                            // logic '1' IOE_N (bit 4) value
                                            // will control IO IOE_N signal else
                                            // IOE_N is control via selected HW
                                            // logic. strong PULL UP and PULL
                                            // Down control is disabled for all
                                            // IO's. both controls are tied to
                                            // logic level '0'.

#define OCP_SHARED_GPIO_PAD_CONFIG_40_MEM_GPIO_PAD_CONFIG_40_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_GPIO_PAD_CMN_CONFIG register.
//
//******************************************************************************
#define OCP_SHARED_GPIO_PAD_CMN_CONFIG_MEM_D2D_ISO_A_EN \
                                0x00000080  // when '1' enable ISO A control to
                                            // D2D Pads else ISO is disabled.
                                            // For these PADS to be functional
                                            // this signals should be set 0.

#define OCP_SHARED_GPIO_PAD_CMN_CONFIG_MEM_D2D_ISO_Y_EN \
                                0x00000040  // when '1' enable ISO Y control to
                                            // D2D Pads else ISO is disabled.
                                            // For these PADS to be functional
                                            // this signals should be set 0.

#define OCP_SHARED_GPIO_PAD_CMN_CONFIG_MEM_PAD_JTAG_IDIEN \
                                0x00000020  // If level ‘1’ enables the PAD to
                                            // ODI path for JTAG PADS [PAD 23,
                                            // 24, 28, 29]. Else ODI is pulled
                                            // ‘Low’ regardless of PAD level."
                                            // "Value gets latched at rising
                                            // edge of RET33.""" """

#define OCP_SHARED_GPIO_PAD_CMN_CONFIG_MEM_PAD_HYSTVAL_M \
                                0x00000018  // 00’: hysteriris = 10% of VDDS
                                            // (difference between upper and
                                            // lower threshold of the schmit
                                            // trigger) ‘01’: hysteriris = 20%
                                            // of VDDS (difference between upper
                                            // and lower threshold of the schmit
                                            // trigger) ‘10’: hysteriris = 30%
                                            // of VDDS (difference between upper
                                            // and lower threshold of the schmit
                                            // trigger) ‘11’: hysteriris = 40%
                                            // of VDDS (difference between upper
                                            // and lower threshold of the schmit
                                            // trigger)" """

#define OCP_SHARED_GPIO_PAD_CMN_CONFIG_MEM_PAD_HYSTVAL_S 3
#define OCP_SHARED_GPIO_PAD_CMN_CONFIG_MEM_PAD_HYSTEN \
                                0x00000004  // If logic ‘0’ there is no
                                            // hysteresis. Set to ‘1’ to enable
                                            // hysteresis. Leave the choice to
                                            // customers"""

#define OCP_SHARED_GPIO_PAD_CMN_CONFIG_MEM_PAD_IBIASEN \
                                0x00000002  // Normal functional operation set
                                            // this to logic ‘1’ to increase the
                                            // speed of the o/p buffer at the
                                            // cost of 0.2uA static current
                                            // consumption per IO. During IDDQ
                                            // test and during Hibernate this
                                            // would be forced to logic ‘0’.
                                            // Value is not latched at rising
                                            // edge of RET33.""

#define OCP_SHARED_GPIO_PAD_CMN_CONFIG_MEM_PAD_IDIEN \
                                0x00000001  // If level ‘1’ enables the PAD to
                                            // ODI path. Else ODI is pulled
                                            // ‘Low’ regardless of PAD level."
                                            // "Value gets latched at rising
                                            // edge of RET33.""" """

//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_D2D_DEV_PAD_CMN_CONFIG register.
//
//******************************************************************************
#define OCP_SHARED_D2D_DEV_PAD_CMN_CONFIG_MEM_DEV_PAD_CMN_CONF_M \
                                0x0000003F  // this register implements common
                                            // IO control to all devement mode
                                            // PADs; these PADs are DEV_PAD33 to
                                            // DEV_PAD39. Bit [1:0] : Drive
                                            // strength control. These 2 bits
                                            // are connected to DEV PAD drive
                                            // strength control. possible drive
                                            // stregnths are 2MA, 4MA and 6 MA
                                            // for the these IO's. bit 0: when
                                            // set to logic value '1' enable 2MA
                                            // drive strength for DEVPAD01 to 07
                                            // bit 1: when set to logic value
                                            // '1' enable 4MA drive strength for
                                            // DEVPAD01 to 07. bit[3:2] : WK
                                            // PULL UP and PULL down control.
                                            // These 2 bits provide IWKPUEN and
                                            // IWKPDEN control for all DEV IO's.
                                            // bit 2: when set to logic value
                                            // '1' enable WKPU to DEVPAD01 to 07
                                            // bit 3: when set to logic value
                                            // '1' enable WKPD to DEVPAD01 to
                                            // 07. bit 4: WK PULL control for
                                            // DEV_PKG_DETECT pin. when '1'
                                            // pullup enabled else it is
                                            // disable. bit 5: when set to logic
                                            // value '1' enable 8MA drive
                                            // strength for DEVPAD01 to 07.

#define OCP_SHARED_D2D_DEV_PAD_CMN_CONFIG_MEM_DEV_PAD_CMN_CONF_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_D2D_TOSTACK_PAD_CONF register.
//
//******************************************************************************
#define OCP_SHARED_D2D_TOSTACK_PAD_CONF_MEM_D2D_TOSTACK_PAD_CONF_M \
                                0x1FFFFFFF  // OEN/OEN2X control. When 0 : Act
                                            // as input buffer else output
                                            // buffer with drive strength 2.
                                            // this register control OEN2X pin
                                            // of D2D TOSTACK PAD: OEN1X and
                                            // OEN2X decoding is as follows:
                                            // "when ""00"" :" "when ""01"" :
                                            // dirve strength is '1' and output
                                            // buffer enabled." "when ""10"" :
                                            // drive strength is 2 and output
                                            // buffer is disabled." "when ""11""
                                            // : dirve strength is '3' and
                                            // output buffer enabled."

#define OCP_SHARED_D2D_TOSTACK_PAD_CONF_MEM_D2D_TOSTACK_PAD_CONF_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_D2D_MISC_PAD_CONF register.
//
//******************************************************************************
#define OCP_SHARED_D2D_MISC_PAD_CONF_MEM_D2D_POR_RESET_N \
                                0x00000200  // This register provide OEN2X
                                            // control to D2D PADS OEN/OEN2X
                                            // control. When 0 : Act as input
                                            // buffer else output buffer with
                                            // drive strength 2.

#define OCP_SHARED_D2D_MISC_PAD_CONF_MEM_D2D_RESET_N \
                                0x00000100  // OEN/OEN2X control. When 0 : Act
                                            // as input buffer else output
                                            // buffer with drive strength 2.

#define OCP_SHARED_D2D_MISC_PAD_CONF_MEM_D2D_HCLK \
                                0x00000080  // OEN/OEN2X control. When 0 : Act
                                            // as input buffer else output
                                            // buffer with drive strength 2.

#define OCP_SHARED_D2D_MISC_PAD_CONF_MEM_D2D_JTAG_TCK \
                                0x00000040  // OEN/OEN2X control. When 0 : Act
                                            // as input buffer else output
                                            // buffer with drive strength 2.

#define OCP_SHARED_D2D_MISC_PAD_CONF_MEM_D2D_JTAG_TMS \
                                0x00000020  // OEN/OEN2X control. When 0 : Act
                                            // as input buffer else output
                                            // buffer with drive strength 2.

#define OCP_SHARED_D2D_MISC_PAD_CONF_MEM_D2D_JTAG_TDI \
                                0x00000010  // OEN/OEN2X control. When 0 : Act
                                            // as input buffer else output
                                            // buffer with drive strength 2.

#define OCP_SHARED_D2D_MISC_PAD_CONF_MEM_D2D_PIOSC \
                                0x00000008  // OEN/OEN2X control. When 0 : Act
                                            // as input buffer else output
                                            // buffer with drive strength 2.

#define OCP_SHARED_D2D_MISC_PAD_CONF_MEM_D2D_SPARE_M \
                                0x00000007  // D2D SPARE PAD OEN/OEN2X control.
                                            // When 0: Act as input buffer else
                                            // output buffer with drive strength
                                            // 2.

#define OCP_SHARED_D2D_MISC_PAD_CONF_MEM_D2D_SPARE_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SOP_CONF_OVERRIDE register.
//
//******************************************************************************
#define OCP_SHARED_SOP_CONF_OVERRIDE_MEM_SOP_CONF_OVERRIDE \
                                0x00000001  // when '1' : signal will ovberride
                                            // SoP setting of JTAG PADS. when
                                            // '0': SoP setting will control
                                            // JTAG PADs [ TDI, TDO, TMS, TCK]

//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_CC3XX_DEBUGSS_STATUS register.
//
//******************************************************************************
#define OCP_SHARED_CC3XX_DEBUGSS_STATUS_APPS_MCU_JTAGNSW \
                                0x00000020  // This register contains debug
                                            // subsystem status bits From APPS
                                            // MCU status bit to indicates
                                            // whether serial wire or 4 pins
                                            // jtag select.

#define OCP_SHARED_CC3XX_DEBUGSS_STATUS_CJTAG_BYPASS_STATUS \
                                0x00000010  // cjtag bypass bit select

#define OCP_SHARED_CC3XX_DEBUGSS_STATUS_SW_INTERFACE_SEL_STATUS \
                                0x00000008  // serial wire interface bit select

#define OCP_SHARED_CC3XX_DEBUGSS_STATUS_APPS_TAP_ENABLE_STATUS \
                                0x00000004  // apps tap enable status

#define OCP_SHARED_CC3XX_DEBUGSS_STATUS_TAPS_ENABLE_STATUS \
                                0x00000002  // tap enable status

#define OCP_SHARED_CC3XX_DEBUGSS_STATUS_SSBD_UNLOCK \
                                0x00000001  // ssbd unlock status

//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_CC3XX_DEBUGMUX_SEL register.
//
//******************************************************************************
#define OCP_SHARED_CC3XX_DEBUGMUX_SEL_MEM_CC3XX_DEBUGMUX_SEL_M \
                                0x0000FFFF  // debug mux select register. Upper
                                            // 8 bits are used for debug module
                                            // selection. Lower 8 bit [7:0] used
                                            // inside debug module for selecting
                                            // module specific signals.
                                            // Bits[15:8: when set x"00" : GPRCM
                                            // debug bus. When "o1" : SDIO debug
                                            // debug bus when x"02" :
                                            // autonoumous SPI when x"03" :
                                            // TOPIC when x"04": memss when
                                            // x"25": mcu debug bus : APPS debug
                                            // when x"45": mcu debug bus : NWP
                                            // debug when x"65": mcu debug bus :
                                            // AHB2VBUS debug when x"85": mcu
                                            // debug bus : VBUS2HAB debug when
                                            // x"95": mcu debug bus : RCM debug
                                            // when x"A5": mcu debug bus :
                                            // crypto debug when x"06": WLAN
                                            // debug bus when x"07": debugss bus
                                            // when x"08": ADC debug when x"09":
                                            // SDIO PHY debug bus then "others"
                                            // : no debug is selected

#define OCP_SHARED_CC3XX_DEBUGMUX_SEL_MEM_CC3XX_DEBUGMUX_SEL_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_ALT_PC_VAL_NW register.
//
//******************************************************************************
#define OCP_SHARED_ALT_PC_VAL_NW_MEM_ALT_PC_VAL_NW_M \
                                0xFFFFFFFF  // 32 bit. Program counter value
                                            // for 0x4 address when Alt_pc_en_nw
                                            // is set.

#define OCP_SHARED_ALT_PC_VAL_NW_MEM_ALT_PC_VAL_NW_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_ALT_PC_VAL_APPS register.
//
//******************************************************************************
#define OCP_SHARED_ALT_PC_VAL_APPS_MEM_ALT_PC_VAL_APPS_M \
                                0xFFFFFFFF  // 32 bit. Program counter value
                                            // for 0x4 address when
                                            // Alt_pc_en_apps is set

#define OCP_SHARED_ALT_PC_VAL_APPS_MEM_ALT_PC_VAL_APPS_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SPARE_REG_4 register.
//
//******************************************************************************
#define OCP_SHARED_SPARE_REG_4_MEM_SPARE_REG_4_M \
                                0xFFFFFFFE  // HW register

#define OCP_SHARED_SPARE_REG_4_MEM_SPARE_REG_4_S 1
#define OCP_SHARED_SPARE_REG_4_INVERT_D2D_INTERFACE \
                                0x00000001  // Data to the top die launched at
                                            // negative edge instead of positive
                                            // edge.

//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SPARE_REG_5 register.
//
//******************************************************************************
#define OCP_SHARED_SPARE_REG_5_MEM_SPARE_REG_5_M \
                                0xFFFFFFFF  // HW register

#define OCP_SHARED_SPARE_REG_5_MEM_SPARE_REG_5_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SH_SPI_CS_MASK register.
//
//******************************************************************************
#define OCP_SHARED_SH_SPI_CS_MASK_MEM_SH_SPI_CS_MASK_M \
                                0x0000000F  // ( chip select 0 is unmasked
                                            // after reset. When ‘1’ : CS is
                                            // unmasked or else masked. Valid
                                            // configurations are 1000, 0100,
                                            // 0010 or 0001. Any other setting
                                            // can lead to unpredictable
                                            // behavior.

#define OCP_SHARED_SH_SPI_CS_MASK_MEM_SH_SPI_CS_MASK_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_CC3XX_DEVICE_TYPE register.
//
//******************************************************************************
#define OCP_SHARED_CC3XX_DEVICE_TYPE_DEVICE_TYPE_reserved_M \
                                0x00000060  // reserved bits tied off "00".

#define OCP_SHARED_CC3XX_DEVICE_TYPE_DEVICE_TYPE_reserved_S 5
#define OCP_SHARED_CC3XX_DEVICE_TYPE_DEVICE_TYPE_M \
                                0x0000001F  // CC3XX Device type information.

#define OCP_SHARED_CC3XX_DEVICE_TYPE_DEVICE_TYPE_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_MEM_TOPMUXCTRL_IFORCE register.
//
//******************************************************************************
#define OCP_SHARED_MEM_TOPMUXCTRL_IFORCE_MEM_TOPMUXCTRL_IFORCE1_M \
                                0x000000F0  // [4] 1: switch between
                                            // WLAN_I2C_SCL and
                                            // TOP_GPIO_PORT4_I2C closes 0:
                                            // switch opens [5] 1: switch
                                            // between WLAN_I2C_SCL and
                                            // TOP_VSENSE_PORT closes 0: switch
                                            // opens [6] 1: switch between
                                            // WLAN_I2C_SCL and WLAN_ANA_TP4
                                            // closes 0: switch opens [7]
                                            // Reserved

#define OCP_SHARED_MEM_TOPMUXCTRL_IFORCE_MEM_TOPMUXCTRL_IFORCE1_S 4
#define OCP_SHARED_MEM_TOPMUXCTRL_IFORCE_MEM_TOPMUXCTRL_IFORCE_M \
                                0x0000000F  // [0] 1: switch between
                                            // WLAN_I2C_SDA and
                                            // TOP_GPIO_PORT3_I2C closes 0:
                                            // switch opens [1] 1: switch
                                            // between WLAN_I2C_SDA and
                                            // TOP_IFORCE_PORT closes 0: switch
                                            // opens [2] 1: switch between
                                            // WLAN_I2C_SDA and WLAN_ANA_TP3
                                            // closes 0: switch opens [3]
                                            // Reserved

#define OCP_SHARED_MEM_TOPMUXCTRL_IFORCE_MEM_TOPMUXCTRL_IFORCE_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_CC3XX_DEV_PACKAGE_DETECT register.
//
//******************************************************************************
#define OCP_SHARED_CC3XX_DEV_PACKAGE_DETECT_DEV_PKG_DETECT \
                                0x00000001  // when '0' indicates package type
                                            // is development.

//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_AUTONMS_SPICLK_SEL register.
//
//******************************************************************************
#define OCP_SHARED_AUTONMS_SPICLK_SEL_MEM_AUTONOMOUS_BYPASS \
                                0x00000002  // This bit is used to bypass MCPSI
                                            // autonomous mode .if this bit is 1
                                            // autonomous MCSPI logic will be
                                            // bypassed and it will act as link
                                            // SPI

#define OCP_SHARED_AUTONMS_SPICLK_SEL_MEM_AUTONMS_SPICLK_SEL \
                                0x00000001  // This bit is used in SPI
                                            // Autonomous mode to switch clock
                                            // from system clock to SPI clk that
                                            // is coming from PAD. When value 1
                                            // PAD SPI clk is used as system
                                            // clock in LPDS mode by SPI as well
                                            // as autonomous wrapper logic.

//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_CC3XX_DEV_PADCONF register.
//
//******************************************************************************
#define OCP_SHARED_CC3XX_DEV_PADCONF_MEM_CC3XX_DEV_PADCONF_M \
                                0x0000FFFF

#define OCP_SHARED_CC3XX_DEV_PADCONF_MEM_CC3XX_DEV_PADCONF_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_IDMEM_TIM_UPDATE register.
//
//******************************************************************************
#define OCP_SHARED_IDMEM_TIM_UPDATE_MEM_IDMEM_TIM_UPDATE_M \
                                0xFFFFFFFF

#define OCP_SHARED_IDMEM_TIM_UPDATE_MEM_IDMEM_TIM_UPDATE_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SPARE_REG_6 register.
//
//******************************************************************************
#define OCP_SHARED_SPARE_REG_6_MEM_SPARE_REG_6_M \
                                0xFFFFFFFF  // NWP Software register

#define OCP_SHARED_SPARE_REG_6_MEM_SPARE_REG_6_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_SPARE_REG_7 register.
//
//******************************************************************************
#define OCP_SHARED_SPARE_REG_7_MEM_SPARE_REG_7_M \
                                0xFFFFFFFF  // NWP Software register

#define OCP_SHARED_SPARE_REG_7_MEM_SPARE_REG_7_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_APPS_WLAN_ORBIT register.
//
//******************************************************************************
#define OCP_SHARED_APPS_WLAN_ORBIT_mem_orbit_spare_M \
                                0xFFFFFC00  // Spare bit

#define OCP_SHARED_APPS_WLAN_ORBIT_mem_orbit_spare_S 10
#define OCP_SHARED_APPS_WLAN_ORBIT_mem_orbit_test_status \
                                0x00000200  // A rising edge on this bit
                                            // indicates that the test case
                                            // passes. This bit would be brought
                                            // out on the pin interface during
                                            // ORBIT.

#define OCP_SHARED_APPS_WLAN_ORBIT_mem_orbit_test_exec \
                                0x00000100  // This register bit is writable by
                                            // the FW and when set to 1 it
                                            // indicates the start of a test
                                            // execution. A failing edge on this
                                            // bit indicates that the test
                                            // execution is complete. This bit
                                            // would be brought out on the pin
                                            // interface during ORBIT.

#define OCP_SHARED_APPS_WLAN_ORBIT_mem_orbit_test_id_M \
                                0x000000FC  // Implies the test case ID that
                                            // needs to run.

#define OCP_SHARED_APPS_WLAN_ORBIT_mem_orbit_test_id_S 2
#define OCP_SHARED_APPS_WLAN_ORBIT_mem_orbit_halt_proc \
                                0x00000002  // This bit is used to trigger the
                                            // execution of test cases within
                                            // the (ROM based) IP.

#define OCP_SHARED_APPS_WLAN_ORBIT_mem_orbit_test_mode \
                                0x00000001  // When this bit is 1 it implies
                                            // ORBIT mode of operation and the
                                            // (ROM based) IP start the
                                            // execution from a test case
                                            // perspective

//******************************************************************************
//
// The following are defines for the bit fields in the
// OCP_SHARED_O_APPS_WLAN_SCRATCH_PAD register.
//
//******************************************************************************
#define OCP_SHARED_APPS_WLAN_SCRATCH_PAD_MEM_APPS_WLAN_SCRATCH_PAD_M \
                                0xFFFFFFFF  // scratch pad register.

#define OCP_SHARED_APPS_WLAN_SCRATCH_PAD_MEM_APPS_WLAN_SCRATCH_PAD_S 0



#endif // __HW_OCP_SHARED_H__

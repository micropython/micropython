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

#ifndef __HW_UDMA_H__
#define __HW_UDMA_H__

//*****************************************************************************
//
// The following are defines for the UDMA register offsets.
//
//*****************************************************************************
#define UDMA_O_STAT             0x00000000
#define UDMA_O_CFG              0x00000004
#define UDMA_O_CTLBASE          0x00000008
#define UDMA_O_ALTBASE          0x0000000C
#define UDMA_O_WAITSTAT         0x00000010
#define UDMA_O_SWREQ            0x00000014
#define UDMA_O_USEBURSTSET      0x00000018
#define UDMA_O_USEBURSTCLR      0x0000001C
#define UDMA_O_REQMASKSET       0x00000020
#define UDMA_O_REQMASKCLR       0x00000024
#define UDMA_O_ENASET           0x00000028
#define UDMA_O_ENACLR           0x0000002C
#define UDMA_O_ALTSET           0x00000030
#define UDMA_O_ALTCLR           0x00000034
#define UDMA_O_PRIOSET          0x00000038
#define UDMA_O_PRIOCLR          0x0000003C
#define UDMA_O_ERRCLR           0x0000004C
#define UDMA_O_CHASGN           0x00000500
#define UDMA_O_CHIS             0x00000504
#define UDMA_O_CHMAP0           0x00000510
#define UDMA_O_CHMAP1           0x00000514
#define UDMA_O_CHMAP2           0x00000518
#define UDMA_O_CHMAP3           0x0000051C
#define UDMA_O_PV               0x00000FB0



//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_STAT register.
//
//******************************************************************************
#define UDMA_STAT_DMACHANS_M  0x001F0000  // Available uDMA Channels Minus 1
#define UDMA_STAT_DMACHANS_S  16
#define UDMA_STAT_STATE_M     0x000000F0  // Control State Machine Status
                                            // 0x00000090 : UDMA_STAT_STATE_DONE
                                            // : Done 0x00000000 :
                                            // UDMA_STAT_STATE_IDLE : Idle
                                            // 0x00000010 :
                                            // UDMA_STAT_STATE_RD_CTRL : Reading
                                            // channel controller data
                                            // 0x00000030 :
                                            // UDMA_STAT_STATE_RD_DSTENDP :
                                            // Reading destination end pointer
                                            // 0x00000040 :
                                            // UDMA_STAT_STATE_RD_SRCDAT :
                                            // Reading source data 0x00000020 :
                                            // UDMA_STAT_STATE_RD_SRCENDP :
                                            // Reading source end pointer
                                            // 0x00000080 :
                                            // UDMA_STAT_STATE_STALL : Stalled
                                            // 0x000000A0 :
                                            // UDMA_STAT_STATE_UNDEF : Undefined
                                            // 0x00000060 : UDMA_STAT_STATE_WAIT
                                            // : Waiting for uDMA request to
                                            // clear 0x00000070 :
                                            // UDMA_STAT_STATE_WR_CTRL : Writing
                                            // channel controller data
                                            // 0x00000050 :
                                            // UDMA_STAT_STATE_WR_DSTDAT :
                                            // Writing destination data
#define UDMA_STAT_STATE_S     4
#define UDMA_STAT_MASTEN        0x00000001  // Master Enable Status
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_CFG register.
//
//******************************************************************************
#define UDMA_CFG_MASTEN         0x00000001  // Controller Master Enable
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_CTLBASE register.
//
//******************************************************************************
#define UDMA_CTLBASE_ADDR_M   0xFFFFFC00  // Channel Control Base Address
#define UDMA_CTLBASE_ADDR_S   10
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_ALTBASE register.
//
//******************************************************************************
#define UDMA_ALTBASE_ADDR_M   0xFFFFFFFF  // Alternate Channel Address
                                            // Pointer
#define UDMA_ALTBASE_ADDR_S   0
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_WAITSTAT register.
//
//******************************************************************************
#define UDMA_WAITSTAT_WAITREQ_M \
                                0xFFFFFFFF  // Channel [n] Wait Status

#define UDMA_WAITSTAT_WAITREQ_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_SWREQ register.
//
//******************************************************************************
#define UDMA_SWREQ_M          0xFFFFFFFF  // Channel [n] Software Request
#define UDMA_SWREQ_S          0
//******************************************************************************
//
// The following are defines for the bit fields in the
// UDMA_O_USEBURSTSET register.
//
//******************************************************************************
#define UDMA_USEBURSTSET_SET_M \
                                0xFFFFFFFF  // Channel [n] Useburst Set

#define UDMA_USEBURSTSET_SET_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// UDMA_O_USEBURSTCLR register.
//
//******************************************************************************
#define UDMA_USEBURSTCLR_CLR_M \
                                0xFFFFFFFF  // Channel [n] Useburst Clear

#define UDMA_USEBURSTCLR_CLR_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_REQMASKSET register.
//
//******************************************************************************
#define UDMA_REQMASKSET_SET_M 0xFFFFFFFF  // Channel [n] Request Mask Set
#define UDMA_REQMASKSET_SET_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_REQMASKCLR register.
//
//******************************************************************************
#define UDMA_REQMASKCLR_CLR_M 0xFFFFFFFF  // Channel [n] Request Mask Clear
#define UDMA_REQMASKCLR_CLR_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_ENASET register.
//
//******************************************************************************
#define UDMA_ENASET_CHENSET_M 0xFFFFFFFF  // Channel [n] Enable Set
#define UDMA_ENASET_CHENSET_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_ENACLR register.
//
//******************************************************************************
#define UDMA_ENACLR_CLR_M     0xFFFFFFFF  // Clear Channel [n] Enable Clear
#define UDMA_ENACLR_CLR_S     0
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_ALTSET register.
//
//******************************************************************************
#define UDMA_ALTSET_SET_M     0xFFFFFFFF  // Channel [n] Alternate Set
#define UDMA_ALTSET_SET_S     0
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_ALTCLR register.
//
//******************************************************************************
#define UDMA_ALTCLR_CLR_M     0xFFFFFFFF  // Channel [n] Alternate Clear
#define UDMA_ALTCLR_CLR_S     0
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_PRIOSET register.
//
//******************************************************************************
#define UDMA_PRIOSET_SET_M    0xFFFFFFFF  // Channel [n] Priority Set
#define UDMA_PRIOSET_SET_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_PRIOCLR register.
//
//******************************************************************************
#define UDMA_PRIOCLR_CLR_M    0xFFFFFFFF  // Channel [n] Priority Clear
#define UDMA_PRIOCLR_CLR_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_ERRCLR register.
//
//******************************************************************************
#define UDMA_ERRCLR_ERRCLR      0x00000001  // uDMA Bus Error Status
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_CHASGN register.
//
//******************************************************************************
#define UDMA_CHASGN_M         0xFFFFFFFF  // Channel [n] Assignment Select
#define UDMA_CHASGN_S         0
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_CHIS register.
//
//******************************************************************************
#define UDMA_CHIS_M           0xFFFFFFFF  // Channel [n] Interrupt Status
#define UDMA_CHIS_S           0
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_CHMAP0 register.
//
//******************************************************************************
#define UDMA_CHMAP0_CH7SEL_M  0xF0000000  // uDMA Channel 7 Source Select
#define UDMA_CHMAP0_CH7SEL_S  28
#define UDMA_CHMAP0_CH6SEL_M  0x0F000000  // uDMA Channel 6 Source Select
#define UDMA_CHMAP0_CH6SEL_S  24
#define UDMA_CHMAP0_CH5SEL_M  0x00F00000  // uDMA Channel 5 Source Select
#define UDMA_CHMAP0_CH5SEL_S  20
#define UDMA_CHMAP0_CH4SEL_M  0x000F0000  // uDMA Channel 4 Source Select
#define UDMA_CHMAP0_CH4SEL_S  16
#define UDMA_CHMAP0_CH3SEL_M  0x0000F000  // uDMA Channel 3 Source Select
#define UDMA_CHMAP0_CH3SEL_S  12
#define UDMA_CHMAP0_CH2SEL_M  0x00000F00  // uDMA Channel 2 Source Select
#define UDMA_CHMAP0_CH2SEL_S  8
#define UDMA_CHMAP0_CH1SEL_M  0x000000F0  // uDMA Channel 1 Source Select
#define UDMA_CHMAP0_CH1SEL_S  4
#define UDMA_CHMAP0_CH0SEL_M  0x0000000F  // uDMA Channel 0 Source Select
#define UDMA_CHMAP0_CH0SEL_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_CHMAP1 register.
//
//******************************************************************************
#define UDMA_CHMAP1_CH15SEL_M 0xF0000000  // uDMA Channel 15 Source Select
#define UDMA_CHMAP1_CH15SEL_S 28
#define UDMA_CHMAP1_CH14SEL_M 0x0F000000  // uDMA Channel 14 Source Select
#define UDMA_CHMAP1_CH14SEL_S 24
#define UDMA_CHMAP1_CH13SEL_M 0x00F00000  // uDMA Channel 13 Source Select
#define UDMA_CHMAP1_CH13SEL_S 20
#define UDMA_CHMAP1_CH12SEL_M 0x000F0000  // uDMA Channel 12 Source Select
#define UDMA_CHMAP1_CH12SEL_S 16
#define UDMA_CHMAP1_CH11SEL_M 0x0000F000  // uDMA Channel 11 Source Select
#define UDMA_CHMAP1_CH11SEL_S 12
#define UDMA_CHMAP1_CH10SEL_M 0x00000F00  // uDMA Channel 10 Source Select
#define UDMA_CHMAP1_CH10SEL_S 8
#define UDMA_CHMAP1_CH9SEL_M  0x000000F0  // uDMA Channel 9 Source Select
#define UDMA_CHMAP1_CH9SEL_S  4
#define UDMA_CHMAP1_CH8SEL_M  0x0000000F  // uDMA Channel 8 Source Select
#define UDMA_CHMAP1_CH8SEL_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_CHMAP2 register.
//
//******************************************************************************
#define UDMA_CHMAP2_CH23SEL_M 0xF0000000  // uDMA Channel 23 Source Select
#define UDMA_CHMAP2_CH23SEL_S 28
#define UDMA_CHMAP2_CH22SEL_M 0x0F000000  // uDMA Channel 22 Source Select
#define UDMA_CHMAP2_CH22SEL_S 24
#define UDMA_CHMAP2_CH21SEL_M 0x00F00000  // uDMA Channel 21 Source Select
#define UDMA_CHMAP2_CH21SEL_S 20
#define UDMA_CHMAP2_CH20SEL_M 0x000F0000  // uDMA Channel 20 Source Select
#define UDMA_CHMAP2_CH20SEL_S 16
#define UDMA_CHMAP2_CH19SEL_M 0x0000F000  // uDMA Channel 19 Source Select
#define UDMA_CHMAP2_CH19SEL_S 12
#define UDMA_CHMAP2_CH18SEL_M 0x00000F00  // uDMA Channel 18 Source Select
#define UDMA_CHMAP2_CH18SEL_S 8
#define UDMA_CHMAP2_CH17SEL_M 0x000000F0  // uDMA Channel 17 Source Select
#define UDMA_CHMAP2_CH17SEL_S 4
#define UDMA_CHMAP2_CH16SEL_M 0x0000000F  // uDMA Channel 16 Source Select
#define UDMA_CHMAP2_CH16SEL_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_CHMAP3 register.
//
//******************************************************************************
#define UDMA_CHMAP3_CH31SEL_M 0xF0000000  // uDMA Channel 31 Source Select
#define UDMA_CHMAP3_CH31SEL_S 28
#define UDMA_CHMAP3_CH30SEL_M 0x0F000000  // uDMA Channel 30 Source Select
#define UDMA_CHMAP3_CH30SEL_S 24
#define UDMA_CHMAP3_CH29SEL_M 0x00F00000  // uDMA Channel 29 Source Select
#define UDMA_CHMAP3_CH29SEL_S 20
#define UDMA_CHMAP3_CH28SEL_M 0x000F0000  // uDMA Channel 28 Source Select
#define UDMA_CHMAP3_CH28SEL_S 16
#define UDMA_CHMAP3_CH27SEL_M 0x0000F000  // uDMA Channel 27 Source Select
#define UDMA_CHMAP3_CH27SEL_S 12
#define UDMA_CHMAP3_CH26SEL_M 0x00000F00  // uDMA Channel 26 Source Select
#define UDMA_CHMAP3_CH26SEL_S 8
#define UDMA_CHMAP3_CH25SEL_M 0x000000F0  // uDMA Channel 25 Source Select
#define UDMA_CHMAP3_CH25SEL_S 4
#define UDMA_CHMAP3_CH24SEL_M 0x0000000F  // uDMA Channel 24 Source Select
#define UDMA_CHMAP3_CH24SEL_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_PV register.
//
//******************************************************************************
#define UDMA_PV_MAJOR_M       0x0000FF00  // Major Revision
#define UDMA_PV_MAJOR_S       8
#define UDMA_PV_MINOR_M       0x000000FF  // Minor Revision
#define UDMA_PV_MINOR_S       0



#endif // __HW_UDMA_H__

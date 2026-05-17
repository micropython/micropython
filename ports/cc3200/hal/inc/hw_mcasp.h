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

#ifndef __HW_MCASP_H__
#define __HW_MCASP_H__

//*****************************************************************************
//
// The following are defines for the MCASP register offsets.
//
//*****************************************************************************
#define MCASP_O_PID             0x00000000
#define MCASP_O_ESYSCONFIG      0x00000004  // Power Idle SYSCONFIG register.
#define MCASP_O_PFUNC           0x00000010
#define MCASP_O_PDIR            0x00000014
#define MCASP_O_PDOUT           0x00000018
#define MCASP_O_PDSET           0x0000001C  // The pin data set register
                                            // (PDSET) is an alias of the pin
                                            // data output register (PDOUT) for
                                            // writes only. Writing a 1 to the
                                            // PDSET bit sets the corresponding
                                            // bit in PDOUT and if PFUNC = 1
                                            // (GPIO function) and PDIR = 1
                                            // (output) drives a logic high on
                                            // the pin.
#define MCASP_O_PDIN            0x0000001C  // The pin data input register
                                            // (PDIN) holds the I/O pin state of
                                            // each of the McASP pins. PDIN
                                            // allows the actual value of the
                                            // pin to be read regardless of the
                                            // state of PFUNC and PDIR.
#define MCASP_O_PDCLR           0x00000020  // The pin data clear register
                                            // (PDCLR) is an alias of the pin
                                            // data output register (PDOUT) for
                                            // writes only. Writing a 1 to the
                                            // PDCLR bit clears the
                                            // corresponding bit in PDOUT and if
                                            // PFUNC = 1 (GPIO function) and
                                            // PDIR = 1 (output) drives a logic
                                            // low on the pin.
#define MCASP_O_TLGC            0x00000030  // for IODFT
#define MCASP_O_TLMR            0x00000034  // for IODFT
#define MCASP_O_TLEC            0x00000038  // for IODFT
#define MCASP_O_GBLCTL          0x00000044
#define MCASP_O_AMUTE           0x00000048
#define MCASP_O_LBCTL           0x0000004C
#define MCASP_O_TXDITCTL        0x00000050
#define MCASP_O_GBLCTLR         0x00000060
#define MCASP_O_RXMASK          0x00000064
#define MCASP_O_RXFMT           0x00000068
#define MCASP_O_RXFMCTL         0x0000006C
#define MCASP_O_ACLKRCTL        0x00000070
#define MCASP_O_AHCLKRCTL       0x00000074
#define MCASP_O_RXTDM           0x00000078
#define MCASP_O_EVTCTLR         0x0000007C
#define MCASP_O_RXSTAT          0x00000080
#define MCASP_O_RXTDMSLOT       0x00000084
#define MCASP_O_RXCLKCHK        0x00000088
#define MCASP_O_REVTCTL         0x0000008C
#define MCASP_O_GBLCTLX         0x000000A0
#define MCASP_O_TXMASK          0x000000A4
#define MCASP_O_TXFMT           0x000000A8
#define MCASP_O_TXFMCTL         0x000000AC
#define MCASP_O_ACLKXCTL        0x000000B0
#define MCASP_O_AHCLKXCTL       0x000000B4
#define MCASP_O_TXTDM           0x000000B8
#define MCASP_O_EVTCTLX         0x000000BC
#define MCASP_O_TXSTAT          0x000000C0
#define MCASP_O_TXTDMSLOT       0x000000C4
#define MCASP_O_TXCLKCHK        0x000000C8
#define MCASP_O_XEVTCTL         0x000000CC
#define MCASP_O_CLKADJEN        0x000000D0
#define MCASP_O_DITCSRA0        0x00000100
#define MCASP_O_DITCSRA1        0x00000104
#define MCASP_O_DITCSRA2        0x00000108
#define MCASP_O_DITCSRA3        0x0000010C
#define MCASP_O_DITCSRA4        0x00000110
#define MCASP_O_DITCSRA5        0x00000114
#define MCASP_O_DITCSRB0        0x00000118
#define MCASP_O_DITCSRB1        0x0000011C
#define MCASP_O_DITCSRB2        0x00000120
#define MCASP_O_DITCSRB3        0x00000124
#define MCASP_O_DITCSRB4        0x00000128
#define MCASP_O_DITCSRB5        0x0000012C
#define MCASP_O_DITUDRA0        0x00000130
#define MCASP_O_DITUDRA1        0x00000134
#define MCASP_O_DITUDRA2        0x00000138
#define MCASP_O_DITUDRA3        0x0000013C
#define MCASP_O_DITUDRA4        0x00000140
#define MCASP_O_DITUDRA5        0x00000144
#define MCASP_O_DITUDRB0        0x00000148
#define MCASP_O_DITUDRB1        0x0000014C
#define MCASP_O_DITUDRB2        0x00000150
#define MCASP_O_DITUDRB3        0x00000154
#define MCASP_O_DITUDRB4        0x00000158
#define MCASP_O_DITUDRB5        0x0000015C
#define MCASP_O_XRSRCTL0        0x00000180
#define MCASP_O_XRSRCTL1        0x00000184
#define MCASP_O_XRSRCTL2        0x00000188
#define MCASP_O_XRSRCTL3        0x0000018C
#define MCASP_O_XRSRCTL4        0x00000190
#define MCASP_O_XRSRCTL5        0x00000194
#define MCASP_O_XRSRCTL6        0x00000198
#define MCASP_O_XRSRCTL7        0x0000019C
#define MCASP_O_XRSRCTL8        0x000001A0
#define MCASP_O_XRSRCTL9        0x000001A4
#define MCASP_O_XRSRCTL10       0x000001A8
#define MCASP_O_XRSRCTL11       0x000001AC
#define MCASP_O_XRSRCTL12       0x000001B0
#define MCASP_O_XRSRCTL13       0x000001B4
#define MCASP_O_XRSRCTL14       0x000001B8
#define MCASP_O_XRSRCTL15       0x000001BC
#define MCASP_O_TXBUF0          0x00000200
#define MCASP_O_TXBUF1          0x00000204
#define MCASP_O_TXBUF2          0x00000208
#define MCASP_O_TXBUF3          0x0000020C
#define MCASP_O_TXBUF4          0x00000210
#define MCASP_O_TXBUF5          0x00000214
#define MCASP_O_TXBUF6          0x00000218
#define MCASP_O_TXBUF7          0x0000021C
#define MCASP_O_TXBUF8          0x00000220
#define MCASP_O_TXBUF9          0x00000224
#define MCASP_O_TXBUF10         0x00000228
#define MCASP_O_TXBUF11         0x0000022C
#define MCASP_O_TXBUF12         0x00000230
#define MCASP_O_TXBUF13         0x00000234
#define MCASP_O_TXBUF14         0x00000238
#define MCASP_O_TXBUF15         0x0000023C
#define MCASP_O_RXBUF0          0x00000280
#define MCASP_O_RXBUF1          0x00000284
#define MCASP_O_RXBUF2          0x00000288
#define MCASP_O_RXBUF3          0x0000028C
#define MCASP_O_RXBUF4          0x00000290
#define MCASP_O_RXBUF5          0x00000294
#define MCASP_O_RXBUF6          0x00000298
#define MCASP_O_RXBUF7          0x0000029C
#define MCASP_O_RXBUF8          0x000002A0
#define MCASP_O_RXBUF9          0x000002A4
#define MCASP_O_RXBUF10         0x000002A8
#define MCASP_O_RXBUF11         0x000002AC
#define MCASP_O_RXBUF12         0x000002B0
#define MCASP_O_RXBUF13         0x000002B4
#define MCASP_O_RXBUF14         0x000002B8
#define MCASP_O_RXBUF15         0x000002BC
#define	MCASP_0_WFIFOCTL	0x00001000
#define	MCASP_0_WFIFOSTS	0x00001004
#define	MCASP_0_RFIFOCTL	0x00001008
#define	MCASP_0_RFIFOSTS	0x0000100C


//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_PID register.
//
//******************************************************************************
#define MCASP_PID_SCHEME_M      0xC0000000
#define MCASP_PID_SCHEME_S      30
#define MCASP_PID_RESV_M        0x30000000
#define MCASP_PID_RESV_S        28
#define MCASP_PID_FUNCTION_M    0x0FFF0000  // McASP
#define MCASP_PID_FUNCTION_S    16
#define MCASP_PID_RTL_M         0x0000F800
#define MCASP_PID_RTL_S         11
#define MCASP_PID_REVMAJOR_M    0x00000700
#define MCASP_PID_REVMAJOR_S    8
#define MCASP_PID_CUSTOM_M      0x000000C0  // non-custom
#define MCASP_PID_CUSTOM_S      6
#define MCASP_PID_REVMINOR_M    0x0000003F
#define MCASP_PID_REVMINOR_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the
// MCASP_O_ESYSCONFIG register.
//
//******************************************************************************
#define MCASP_ESYSCONFIG_RSV_M  0xFFFFFFC0  // Reserved as per PDR 3.5
#define MCASP_ESYSCONFIG_RSV_S  6
#define MCASP_ESYSCONFIG_OTHER_M \
                                0x0000003C  // Reserved for future expansion

#define MCASP_ESYSCONFIG_OTHER_S 2
#define MCASP_ESYSCONFIG_IDLE_MODE_M \
                                0x00000003  // Idle Mode

#define MCASP_ESYSCONFIG_IDLE_MODE_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_PFUNC register.
//
//******************************************************************************
#define MCASP_PFUNC_AFSR        0x80000000  // AFSR PFUNC 31 0 1
#define MCASP_PFUNC_AHCLKR      0x40000000  // AHCLKR PFUNC 30 0 1
#define MCASP_PFUNC_ACLKR       0x20000000  // ACLKR PFUNC 29 0 1
#define MCASP_PFUNC_AFSX        0x10000000  // AFSX PFUNC 28 0 1
#define MCASP_PFUNC_AHCLKX      0x08000000  // AHCLKX PFUNC 27 0 1
#define MCASP_PFUNC_ACLKX       0x04000000  // ACLKX PFUNC 26 0 1
#define MCASP_PFUNC_AMUTE       0x02000000  // AMUTE PFUNC 25 0 1
#define MCASP_PFUNC_RESV1_M     0x01FF0000  // Reserved
#define MCASP_PFUNC_RESV1_S     16
#define MCASP_PFUNC_AXR15       0x00008000  // AXR PFUNC BIT 15 0 1
#define MCASP_PFUNC_AXR14       0x00004000  // AXR PFUNC BIT 14 0 1
#define MCASP_PFUNC_AXR13       0x00002000  // AXR PFUNC BIT 13 0 1
#define MCASP_PFUNC_AXR12       0x00001000  // AXR PFUNC BIT 12 0 1
#define MCASP_PFUNC_AXR11       0x00000800  // AXR PFUNC BIT 11 0 1
#define MCASP_PFUNC_AXR10       0x00000400  // AXR PFUNC BIT 10 0 1
#define MCASP_PFUNC_AXR9        0x00000200  // AXR PFUNC BIT 9 0 1
#define MCASP_PFUNC_AXR8        0x00000100  // AXR PFUNC BIT 8 0 1
#define MCASP_PFUNC_AXR7        0x00000080  // AXR PFUNC BIT 7 0 1
#define MCASP_PFUNC_AXR6        0x00000040  // AXR PFUNC BIT 6 0 1
#define MCASP_PFUNC_AXR5        0x00000020  // AXR PFUNC BIT 5 0 1
#define MCASP_PFUNC_AXR4        0x00000010  // AXR PFUNC BIT 4 0 1
#define MCASP_PFUNC_AXR3        0x00000008  // AXR PFUNC BIT 3 0 1
#define MCASP_PFUNC_AXR2        0x00000004  // AXR PFUNC BIT 2 0 1
#define MCASP_PFUNC_AXR1        0x00000002  // AXR PFUNC BIT 1 0 1
#define MCASP_PFUNC_AXR0        0x00000001  // AXR PFUNC BIT 0 0 1
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_PDIR register.
//
//******************************************************************************
#define MCASP_PDIR_AFSR         0x80000000  // AFSR PDIR 31 0 1
#define MCASP_PDIR_AHCLKR       0x40000000  // AHCLKR PDIR 30 0 1
#define MCASP_PDIR_ACLKR        0x20000000  // ACLKR PDIR 29 0 1
#define MCASP_PDIR_AFSX         0x10000000  // AFSX PDIR 28 0 1
#define MCASP_PDIR_AHCLKX       0x08000000  // AHCLKX PDIR 27 0 1
#define MCASP_PDIR_ACLKX        0x04000000  // ACLKX PDIR 26 0 1
#define MCASP_PDIR_AMUTE        0x02000000  // AMUTE PDIR 25 0 1
#define MCASP_PDIR_RESV_M       0x01FF0000  // Reserved
#define MCASP_PDIR_RESV_S       16
#define MCASP_PDIR_AXR15        0x00008000  // AXR PDIR BIT 15 0 1
#define MCASP_PDIR_AXR14        0x00004000  // AXR PDIR BIT 14 0 1
#define MCASP_PDIR_AXR13        0x00002000  // AXR PDIR BIT 13 0 1
#define MCASP_PDIR_AXR12        0x00001000  // AXR PDIR BIT 12 0 1
#define MCASP_PDIR_AXR11        0x00000800  // AXR PDIR BIT 11 0 1
#define MCASP_PDIR_AXR10        0x00000400  // AXR PDIR BIT 10 0 1
#define MCASP_PDIR_AXR9         0x00000200  // AXR PDIR BIT 9 0 1
#define MCASP_PDIR_AXR8         0x00000100  // AXR PDIR BIT 8 0 1
#define MCASP_PDIR_AXR7         0x00000080  // AXR PDIR BIT 7 0 1
#define MCASP_PDIR_AXR6         0x00000040  // AXR PDIR BIT 6 0 1
#define MCASP_PDIR_AXR5         0x00000020  // AXR PDIR BIT 5 0 1
#define MCASP_PDIR_AXR4         0x00000010  // AXR PDIR BIT 4 0 1
#define MCASP_PDIR_AXR3         0x00000008  // AXR PDIR BIT 3 0 1
#define MCASP_PDIR_AXR2         0x00000004  // AXR PDIR BIT 2 0 1
#define MCASP_PDIR_AXR1         0x00000002  // AXR PDIR BIT 1 0 1
#define MCASP_PDIR_AXR0         0x00000001  // AXR PDIR BIT 0 0 1
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_PDOUT register.
//
//******************************************************************************
#define MCASP_PDOUT_AFSR        0x80000000  // AFSR PDOUT 31 0 1
#define MCASP_PDOUT_AHCLKR      0x40000000  // AHCLKR PDOUT 30 0 1
#define MCASP_PDOUT_ACLKR       0x20000000  // ACLKR PDOUT 29 0 1
#define MCASP_PDOUT_AFSX        0x10000000  // AFSX PDOUT 28 0 1
#define MCASP_PDOUT_AHCLKX      0x08000000  // AHCLKX PDOUT 27 0 1
#define MCASP_PDOUT_ACLKX       0x04000000  // ACLKX PDOUT 26 0 1
#define MCASP_PDOUT_AMUTE       0x02000000  // AMUTE PDOUT 25 0 1
#define MCASP_PDOUT_RESV_M      0x01FF0000  // Reserved
#define MCASP_PDOUT_RESV_S      16
#define MCASP_PDOUT_AXR15       0x00008000  // AXR PDOUT BIT 15 0 1
#define MCASP_PDOUT_AXR14       0x00004000  // AXR PDOUT BIT 14 0 1
#define MCASP_PDOUT_AXR13       0x00002000  // AXR PDOUT BIT 13 0 1
#define MCASP_PDOUT_AXR12       0x00001000  // AXR PDOUT BIT 12 0 1
#define MCASP_PDOUT_AXR11       0x00000800  // AXR PDOUT BIT 11 0 1
#define MCASP_PDOUT_AXR10       0x00000400  // AXR PDOUT BIT 10 0 1
#define MCASP_PDOUT_AXR9        0x00000200  // AXR PDOUT BIT 9 0 1
#define MCASP_PDOUT_AXR8        0x00000100  // AXR PDOUT BIT 8 0 1
#define MCASP_PDOUT_AXR7        0x00000080  // AXR PDOUT BIT 7 0 1
#define MCASP_PDOUT_AXR6        0x00000040  // AXR PDOUT BIT 6 0 1
#define MCASP_PDOUT_AXR5        0x00000020  // AXR PDOUT BIT 5 0 1
#define MCASP_PDOUT_AXR4        0x00000010  // AXR PDOUT BIT 4 0 1
#define MCASP_PDOUT_AXR3        0x00000008  // AXR PDOUT BIT 3 0 1
#define MCASP_PDOUT_AXR2        0x00000004  // AXR PDOUT BIT 2 0 1
#define MCASP_PDOUT_AXR1        0x00000002  // AXR PDOUT BIT 1 0 1
#define MCASP_PDOUT_AXR0        0x00000001  // AXR PDOUT BIT 0 0 1
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_PDSET register.
//
//******************************************************************************
#define MCASP_PDSET_AFSR        0x80000000
#define MCASP_PDSET_AHCLKR      0x40000000
#define MCASP_PDSET_ACLKR       0x20000000
#define MCASP_PDSET_AFSX        0x10000000
#define MCASP_PDSET_AHCLKX      0x08000000
#define MCASP_PDSET_ACLKX       0x04000000
#define MCASP_PDSET_AMUTE       0x02000000
#define MCASP_PDSET_RESV_M      0x01FF0000  // Reserved
#define MCASP_PDSET_RESV_S      16
#define MCASP_PDSET_AXR15       0x00008000
#define MCASP_PDSET_AXR14       0x00004000
#define MCASP_PDSET_AXR13       0x00002000
#define MCASP_PDSET_AXR12       0x00001000
#define MCASP_PDSET_AXR11       0x00000800
#define MCASP_PDSET_AXR10       0x00000400
#define MCASP_PDSET_AXR9        0x00000200
#define MCASP_PDSET_AXR8        0x00000100
#define MCASP_PDSET_AXR7        0x00000080
#define MCASP_PDSET_AXR6        0x00000040
#define MCASP_PDSET_AXR5        0x00000020
#define MCASP_PDSET_AXR4        0x00000010
#define MCASP_PDSET_AXR3        0x00000008
#define MCASP_PDSET_AXR2        0x00000004
#define MCASP_PDSET_AXR1        0x00000002
#define MCASP_PDSET_AXR0        0x00000001
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_PDIN register.
//
//******************************************************************************
#define MCASP_PDIN_AFSR         0x80000000
#define MCASP_PDIN_AHCLKR       0x40000000
#define MCASP_PDIN_ACLKR        0x20000000
#define MCASP_PDIN_AFSX         0x10000000
#define MCASP_PDIN_AHCLKX       0x08000000
#define MCASP_PDIN_ACLKX        0x04000000
#define MCASP_PDIN_AMUTE        0x02000000
#define MCASP_PDIN_RESV_M       0x01FF0000  // Reserved
#define MCASP_PDIN_RESV_S       16
#define MCASP_PDIN_AXR15        0x00008000
#define MCASP_PDIN_AXR14        0x00004000
#define MCASP_PDIN_AXR13        0x00002000
#define MCASP_PDIN_AXR12        0x00001000
#define MCASP_PDIN_AXR11        0x00000800
#define MCASP_PDIN_AXR10        0x00000400
#define MCASP_PDIN_AXR9         0x00000200
#define MCASP_PDIN_AXR8         0x00000100
#define MCASP_PDIN_AXR7         0x00000080
#define MCASP_PDIN_AXR6         0x00000040
#define MCASP_PDIN_AXR5         0x00000020
#define MCASP_PDIN_AXR4         0x00000010
#define MCASP_PDIN_AXR3         0x00000008
#define MCASP_PDIN_AXR2         0x00000004
#define MCASP_PDIN_AXR1         0x00000002
#define MCASP_PDIN_AXR0         0x00000001
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_PDCLR register.
//
//******************************************************************************
#define MCASP_PDCLR_AFSR        0x80000000  // AFSR PDCLR 31 0 1
#define MCASP_PDCLR_AHCLKR      0x40000000  // AHCLKR PDCLR 30 0 1
#define MCASP_PDCLR_ACLKR       0x20000000  // ACLKR PDCLR 29 0 1
#define MCASP_PDCLR_AFSX        0x10000000  // AFSX PDCLR 28 0 1
#define MCASP_PDCLR_AHCLKX      0x08000000  // AHCLKX PDCLR 27 0 1
#define MCASP_PDCLR_ACLKX       0x04000000  // ACLKX PDCLR 26 0 1
#define MCASP_PDCLR_AMUTE       0x02000000  // AMUTE PDCLR 25 0 1
#define MCASP_PDCLR_RESV_M      0x01FF0000  // Reserved
#define MCASP_PDCLR_RESV_S      16
#define MCASP_PDCLR_AXR15       0x00008000  // AXR PDCLR BIT 15 0 1
#define MCASP_PDCLR_AXR14       0x00004000  // AXR PDCLR BIT 14 0 1
#define MCASP_PDCLR_AXR13       0x00002000  // AXR PDCLR BIT 13 0 1
#define MCASP_PDCLR_AXR12       0x00001000  // AXR PDCLR BIT 12 0 1
#define MCASP_PDCLR_AXR11       0x00000800  // AXR PDCLR BIT 11 0 1
#define MCASP_PDCLR_AXR10       0x00000400  // AXR PDCLR BIT 10 0 1
#define MCASP_PDCLR_AXR9        0x00000200  // AXR PDCLR BIT 9 0 1
#define MCASP_PDCLR_AXR8        0x00000100  // AXR PDCLR BIT 8 0 1
#define MCASP_PDCLR_AXR7        0x00000080  // AXR PDCLR BIT 7 0 1
#define MCASP_PDCLR_AXR6        0x00000040  // AXR PDCLR BIT 6 0 1
#define MCASP_PDCLR_AXR5        0x00000020  // AXR PDCLR BIT 5 0 1
#define MCASP_PDCLR_AXR4        0x00000010  // AXR PDCLR BIT 4 0 1
#define MCASP_PDCLR_AXR3        0x00000008  // AXR PDCLR BIT 3 0 1
#define MCASP_PDCLR_AXR2        0x00000004  // AXR PDCLR BIT 2 0 1
#define MCASP_PDCLR_AXR1        0x00000002  // AXR PDCLR BIT 1 0 1
#define MCASP_PDCLR_AXR0        0x00000001  // AXR PDCLR BIT 0 0 1
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TLGC register.
//
//******************************************************************************
#define MCASP_TLGC_RESV_M       0xFFFF0000  // Reserved
#define MCASP_TLGC_RESV_S       16
#define MCASP_TLGC_MT_M         0x0000C000  // MISR on/off trigger command 0x0
                                            // 0x1 0x2 0x3
#define MCASP_TLGC_MT_S         14
#define MCASP_TLGC_RESV1_M      0x00003E00  // Reserved
#define MCASP_TLGC_RESV1_S      9
#define MCASP_TLGC_MMS          0x00000100  // Source of MISR input 0 1
#define MCASP_TLGC_ESEL         0x00000080  // Output enable select 0 1
#define MCASP_TLGC_TOEN         0x00000040  // Test output enable control. 0 1
#define MCASP_TLGC_MC_M         0x00000030  // States of MISR 0x0 0x1 0x2 0x3
#define MCASP_TLGC_MC_S         4
#define MCASP_TLGC_PC_M         0x0000000E  // Pattern code 0x0 0x1 0x2 0x3 0x4
                                            // 0x5 0x6 0x7
#define MCASP_TLGC_PC_S         1
#define MCASP_TLGC_TM           0x00000001  // Tie high; do not write to this
                                            // bit 0 1
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TLMR register.
//
//******************************************************************************
#define MCASP_TLMR_TLMR_M       0xFFFFFFFF  // Contains test result signature.
#define MCASP_TLMR_TLMR_S       0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TLEC register.
//
//******************************************************************************
#define MCASP_TLEC_TLEC_M       0xFFFFFFFF  // Contains number of cycles during
                                            // which MISR sig will be
                                            // accumulated.
#define MCASP_TLEC_TLEC_S       0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_GBLCTL register.
//
//******************************************************************************
#define MCASP_GBLCTL_XFRST      0x00001000  // Frame sync generator reset 0 1
#define MCASP_GBLCTL_XSMRST     0x00000800  // XMT state machine reset 0 1
#define MCASP_GBLCTL_XSRCLR     0x00000400  // XMT serializer clear 0 1
#define MCASP_GBLCTL_XHCLKRST   0x00000200  // XMT High Freq. clk Divider 0 1
#define MCASP_GBLCTL_XCLKRST    0x00000100  // XMT clock divder reset 0 1
#define MCASP_GBLCTL_RFRST      0x00000010  // Frame sync generator reset 0 1
#define MCASP_GBLCTL_RSMRST     0x00000008  // RCV state machine reset 0 1
#define MCASP_GBLCTL_RSRCLR     0x00000004  // RCV serializer clear 0 1
#define MCASP_GBLCTL_RHCLKRST   0x00000002  // RCV High Freq. clk Divider 0 1
#define MCASP_GBLCTL_RCLKRST    0x00000001  // RCV clock divder reset 0 1
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_AMUTE register.
//
//******************************************************************************
#define MCASP_AMUTE_XDMAERR     0x00001000  // MUTETXDMAERR occur 0 1
#define MCASP_AMUTE_RDMAERR     0x00000800  // MUTERXDMAERR occur 0 1
#define MCASP_AMUTE_XCKFAIL     0x00000400  // XMT bad clock 0 1
#define MCASP_AMUTE_RCKFAIL     0x00000200  // RCV bad clock 0 1
#define MCASP_AMUTE_XSYNCERR    0x00000100  // XMT unexpected FS 0 1
#define MCASP_AMUTE_RSYNCERR    0x00000080  // RCV unexpected FS 0 1
#define MCASP_AMUTE_XUNDRN      0x00000040  // XMT underrun occurs 0 1
#define MCASP_AMUTE_ROVRN       0x00000020  // RCV overun occurs 0 1
#define MCASP_AMUTE_INSTAT      0x00000010
#define MCASP_AMUTE_INEN        0x00000008  // drive AMUTE active on mute in
                                            // active 0 1
#define MCASP_AMUTE_INPOL       0x00000004  // Mute input polarity 0 1
#define MCASP_AMUTE_MUTEN_M     0x00000003  // AMUTE pin enable 0x0 0x1 0x2
#define MCASP_AMUTE_MUTEN_S     0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_LBCTL register.
//
//******************************************************************************
#define MCASP_LBCTL_IOLBEN      0x00000010  // IO loopback enable 0 1
#define MCASP_LBCTL_MODE_M      0x0000000C  // Loop back clock source generator
                                            // 0x0 0x1 0x2 0x3
#define MCASP_LBCTL_MODE_S      2
#define MCASP_LBCTL_ORD         0x00000002  // Loopback order 0 1
#define MCASP_LBCTL_DLBEN       0x00000001  // Loop back mode 0 1
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXDITCTL register.
//
//******************************************************************************
#define MCASP_TXDITCTL_VB       0x00000008  // Valib bit for odd TDM 0 1
#define MCASP_TXDITCTL_VA       0x00000004  // Valib bit for even TDM 0 1
#define MCASP_TXDITCTL_DITEN    0x00000001  // XMT DIT Mode Enable 0 1
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_GBLCTLR register.
//
//******************************************************************************
#define MCASP_GBLCTLR_XFRST     0x00001000
#define MCASP_GBLCTLR_XSMRST    0x00000800
#define MCASP_GBLCTLR_XSRCLR    0x00000400
#define MCASP_GBLCTLR_XHCLKRST  0x00000200
#define MCASP_GBLCTLR_XCLKRST   0x00000100
#define MCASP_GBLCTLR_RFRST     0x00000010  // Frame sync generator reset 0 1
#define MCASP_GBLCTLR_RSMRST    0x00000008  // RCV state machine reset 0 1
#define MCASP_GBLCTLR_RSRCLR    0x00000004  // RCV serializer clear 0 1
#define MCASP_GBLCTLR_RHCLKRST  0x00000002  // RCV High Freq. clk Divider 0 1
#define MCASP_GBLCTLR_RCLKRST   0x00000001  // RCV clock divder reset 0 1
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXMASK register.
//
//******************************************************************************
#define MCASP_RXMASK_RMASK31    0x80000000  // RMASK BIT 31 0 1
#define MCASP_RXMASK_RMASK30    0x40000000  // RMASK BIT 30 0 1
#define MCASP_RXMASK_RMASK29    0x20000000  // RMASK BIT 29 0 1
#define MCASP_RXMASK_RMASK28    0x10000000  // RMASK BIT 28 0 1
#define MCASP_RXMASK_RMASK27    0x08000000  // RMASK BIT 27 0 1
#define MCASP_RXMASK_RMASK26    0x04000000  // RMASK BIT 26 0 1
#define MCASP_RXMASK_RMASK25    0x02000000  // RMASK BIT 25 0 1
#define MCASP_RXMASK_RMASK24    0x01000000  // RMASK BIT 24 0 1
#define MCASP_RXMASK_RMASK23    0x00800000  // RMASK BIT 23 0 1
#define MCASP_RXMASK_RMASK22    0x00400000  // RMASK BIT 22 0 1
#define MCASP_RXMASK_RMASK21    0x00200000  // RMASK BIT 21 0 1
#define MCASP_RXMASK_RMASK20    0x00100000  // RMASK BIT 20 0 1
#define MCASP_RXMASK_RMASK19    0x00080000  // RMASK BIT 19 0 1
#define MCASP_RXMASK_RMASK18    0x00040000  // RMASK BIT 18 0 1
#define MCASP_RXMASK_RMASK17    0x00020000  // RMASK BIT 17 0 1
#define MCASP_RXMASK_RMASK16    0x00010000  // RMASK BIT 16 0 1
#define MCASP_RXMASK_RMASK15    0x00008000  // RMASK BIT 15 0 1
#define MCASP_RXMASK_RMASK14    0x00004000  // RMASK BIT 14 0 1
#define MCASP_RXMASK_RMASK13    0x00002000  // RMASK BIT 13 0 1
#define MCASP_RXMASK_RMASK12    0x00001000  // RMASK BIT 12 0 1
#define MCASP_RXMASK_RMASK11    0x00000800  // RMASK BIT 11 0 1
#define MCASP_RXMASK_RMASK10    0x00000400  // RMASK BIT 10 0 1
#define MCASP_RXMASK_RMASK9     0x00000200  // RMASK BIT 9 0 1
#define MCASP_RXMASK_RMASK8     0x00000100  // RMASK BIT 8 0 1
#define MCASP_RXMASK_RMASK7     0x00000080  // RMASK BIT 7 0 1
#define MCASP_RXMASK_RMASK6     0x00000040  // RMASK BIT 6 0 1
#define MCASP_RXMASK_RMASK5     0x00000020  // RMASK BIT 5 0 1
#define MCASP_RXMASK_RMASK4     0x00000010  // RMASK BIT 4 0 1
#define MCASP_RXMASK_RMASK3     0x00000008  // RMASK BIT 3 0 1
#define MCASP_RXMASK_RMASK2     0x00000004  // RMASK BIT 2 0 1
#define MCASP_RXMASK_RMASK1     0x00000002  // RMASK BIT 1 0 1
#define MCASP_RXMASK_RMASK0     0x00000001  // RMASK BIT 0 0 1
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXFMT register.
//
//******************************************************************************
#define MCASP_RXFMT_RDATDLY_M   0x00030000  // RCV Frame sync delay 0x0 0 Bit
                                            // delay 0x1 1 Bit delay 0x2 2 Bit
                                            // delay
#define MCASP_RXFMT_RDATDLY_S   16
#define MCASP_RXFMT_RRVRS       0x00008000  // RCV serial stream bit order 0 1
#define MCASP_RXFMT_RPAD_M      0x00006000  // Pad value 0x0 0x1 0x2
#define MCASP_RXFMT_RPAD_S      13
#define MCASP_RXFMT_RPBIT_M     0x00001F00  // Pad bit position
#define MCASP_RXFMT_RPBIT_S     8
#define MCASP_RXFMT_RSSZ_M      0x000000F0  // RCV slot Size 0x0 0x1 0x2 0x3
                                            // 0x4 0x5 0x6 0x7 0x8 0x9 0xA 0xB
                                            // 0xC 0xD 0xE 0xF
#define MCASP_RXFMT_RSSZ_S      4
#define MCASP_RXFMT_RBUSEL      0x00000008  // Write to RBUF using CPU/DMA 0
                                            // DMA port access 1 CPU port Access
#define MCASP_RXFMT_RROT_M      0x00000007  // Right Rotate Value 0x0 0x1 0x2
                                            // 0x3 0x4 0x5 0x6 0x7
#define MCASP_RXFMT_RROT_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXFMCTL register.
//
//******************************************************************************
#define MCASP_RXFMCTL_RMOD_M    0x0000FF80  // RCV Frame sync mode
#define MCASP_RXFMCTL_RMOD_S    7
#define MCASP_RXFMCTL_FRWID     0x00000010  // RCV Frame sync Duration 0 1
#define MCASP_RXFMCTL_FSRM      0x00000002  // RCV frame sync External 0 1
#define MCASP_RXFMCTL_FSRP      0x00000001  // RCV Frame sync Polarity 0 1
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_ACLKRCTL register.
//
//******************************************************************************
#define MCASP_ACLKRCTL_BUSY     0x00100000
#define MCASP_ACLKRCTL_DIVBUSY  0x00080000
#define MCASP_ACLKRCTL_ADJBUSY  0x00040000
#define MCASP_ACLKRCTL_CLKRADJ_M \
                                0x00030000

#define MCASP_ACLKRCTL_CLKRADJ_S 16
#define MCASP_ACLKRCTL_CLKRP    0x00000080  // RCV Clock Polarity 0 1
#define MCASP_ACLKRCTL_CLKRM    0x00000020  // RCV clock source 0 1
#define MCASP_ACLKRCTL_CLKRDIV_M \
                                0x0000001F  // RCV clock devide ratio

#define MCASP_ACLKRCTL_CLKRDIV_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_AHCLKRCTL register.
//
//******************************************************************************
#define MCASP_AHCLKRCTL_BUSY    0x00100000
#define MCASP_AHCLKRCTL_DIVBUSY 0x00080000
#define MCASP_AHCLKRCTL_ADJBUSY 0x00040000
#define MCASP_AHCLKRCTL_HCLKRADJ_M \
                                0x00030000

#define MCASP_AHCLKRCTL_HCLKRADJ_S 16
#define MCASP_AHCLKRCTL_HCLKRM  0x00008000  // High Freq. RCV clock Source 0 1
#define MCASP_AHCLKRCTL_HCLKRP  0x00004000  // High Freq. clock Polarity Before
                                            // diviser 0 1
#define MCASP_AHCLKRCTL_HCLKRDIV_M \
                                0x00000FFF  // RCV clock Divide Ratio

#define MCASP_AHCLKRCTL_HCLKRDIV_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXTDM register.
//
//******************************************************************************
#define MCASP_RXTDM_RTDMS31     0x80000000  // RCV mode during TDM time slot 31
                                            // 0 1
#define MCASP_RXTDM_RTDMS30     0x40000000  // RCV mode during TDM time slot 30
                                            // 0 1
#define MCASP_RXTDM_RTDMS29     0x20000000  // RCV mode during TDM time slot 29
                                            // 0 1
#define MCASP_RXTDM_RTDMS28     0x10000000  // RCV mode during TDM time slot 28
                                            // 0 1
#define MCASP_RXTDM_RTDMS27     0x08000000  // RCV mode during TDM time slot 27
                                            // 0 1
#define MCASP_RXTDM_RTDMS26     0x04000000  // RCV mode during TDM time slot 26
                                            // 0 1
#define MCASP_RXTDM_RTDMS25     0x02000000  // RCV mode during TDM time slot 25
                                            // 0 1
#define MCASP_RXTDM_RTDMS24     0x01000000  // RCV mode during TDM time slot 24
                                            // 0 1
#define MCASP_RXTDM_RTDMS23     0x00800000  // RCV mode during TDM time slot 23
                                            // 0 1
#define MCASP_RXTDM_RTDMS22     0x00400000  // RCV mode during TDM time slot 22
                                            // 0 1
#define MCASP_RXTDM_RTDMS21     0x00200000  // RCV mode during TDM time slot 21
                                            // 0 1
#define MCASP_RXTDM_RTDMS20     0x00100000  // RCV mode during TDM time slot 20
                                            // 0 1
#define MCASP_RXTDM_RTDMS19     0x00080000  // RCV mode during TDM time slot 19
                                            // 0 1
#define MCASP_RXTDM_RTDMS18     0x00040000  // RCV mode during TDM time slot 18
                                            // 0 1
#define MCASP_RXTDM_RTDMS17     0x00020000  // RCV mode during TDM time slot 17
                                            // 0 1
#define MCASP_RXTDM_RTDMS16     0x00010000  // RCV mode during TDM time slot 16
                                            // 0 1
#define MCASP_RXTDM_RTDMS15     0x00008000  // RCV mode during TDM time slot 15
                                            // 0 1
#define MCASP_RXTDM_RTDMS14     0x00004000  // RCV mode during TDM time slot 14
                                            // 0 1
#define MCASP_RXTDM_RTDMS13     0x00002000  // RCV mode during TDM time slot 13
                                            // 0 1
#define MCASP_RXTDM_RTDMS12     0x00001000  // RCV mode during TDM time slot 12
                                            // 0 1
#define MCASP_RXTDM_RTDMS11     0x00000800  // RCV mode during TDM time slot 11
                                            // 0 1
#define MCASP_RXTDM_RTDMS10     0x00000400  // RCV mode during TDM time slot 10
                                            // 0 1
#define MCASP_RXTDM_RTDMS9      0x00000200  // RCV mode during TDM time slot 9
                                            // 0 1
#define MCASP_RXTDM_RTDMS8      0x00000100  // RCV mode during TDM time slot 8
                                            // 0 1
#define MCASP_RXTDM_RTDMS7      0x00000080  // RCV mode during TDM time slot 7
                                            // 0 1
#define MCASP_RXTDM_RTDMS6      0x00000040  // RCV mode during TDM time slot 6
                                            // 0 1
#define MCASP_RXTDM_RTDMS5      0x00000020  // RCV mode during TDM time slot 5
                                            // 0 1
#define MCASP_RXTDM_RTDMS4      0x00000010  // RCV mode during TDM time slot 4
                                            // 0 1
#define MCASP_RXTDM_RTDMS3      0x00000008  // RCV mode during TDM time slot 3
                                            // 0 1
#define MCASP_RXTDM_RTDMS2      0x00000004  // RCV mode during TDM time slot 2
                                            // 0 1
#define MCASP_RXTDM_RTDMS1      0x00000002  // RCV mode during TDM time slot 1
                                            // 0 1
#define MCASP_RXTDM_RTDMS0      0x00000001  // RCV mode during TDM time slot 0
                                            // 0 1
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_EVTCTLR register.
//
//******************************************************************************
#define MCASP_EVTCTLR_RSTAFRM   0x00000080  // RCV Start of Frame Interrupt 0 1
#define MCASP_EVTCTLR_RDATA     0x00000020  // RCV Data Interrupt 0 1
#define MCASP_EVTCTLR_RLAST     0x00000010  // RCV Last Slot Interrupt 0 1
#define MCASP_EVTCTLR_RDMAERR   0x00000008  // RCV DMA Bus Error 0 1
#define MCASP_EVTCTLR_RCKFAIL   0x00000004  // Bad Clock Interrupt 0 1
#define MCASP_EVTCTLR_RSYNCERR  0x00000002  // RCV Unexpected FSR Interrupt 0 1
#define MCASP_EVTCTLR_ROVRN     0x00000001  // RCV Underrun Flag 0 1
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXSTAT register.
//
//******************************************************************************
#define MCASP_RXSTAT_RERR       0x00000100  // RCV Error 0 1
#define MCASP_RXSTAT_RDMAERR    0x00000080  // RCV DMA bus error 0 1
#define MCASP_RXSTAT_RSTAFRM    0x00000040  // Start of Frame-RCV 0 1
#define MCASP_RXSTAT_RDATA      0x00000020  // Data Ready Flag 0 1
#define MCASP_RXSTAT_RLAST      0x00000010  // Last Slot Interrupt Flag 0 1
#define MCASP_RXSTAT_RTDMSLOT   0x00000008  // EvenOdd Slot 0 1
#define MCASP_RXSTAT_RCKFAIL    0x00000004  // Bad Transmit Flag 0 1
#define MCASP_RXSTAT_RSYNCERR   0x00000002  // Unexpected RCV Frame sync flag 0
                                            // 1
#define MCASP_RXSTAT_ROVRN      0x00000001  // RCV Underrun Flag 0 1
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXTDMSLOT register.
//
//******************************************************************************
#define MCASP_RXTDMSLOT_RSLOTCNT_M \
                                0x000003FF  // Current RCV time slot count

#define MCASP_RXTDMSLOT_RSLOTCNT_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXCLKCHK register.
//
//******************************************************************************
#define MCASP_RXCLKCHK_RCNT_M   0xFF000000  // RCV clock count value
#define MCASP_RXCLKCHK_RCNT_S   24
#define MCASP_RXCLKCHK_RMAX_M   0x00FF0000  // RCV clock maximum boundary
#define MCASP_RXCLKCHK_RMAX_S   16
#define MCASP_RXCLKCHK_RMIN_M   0x0000FF00  // RCV clock minimum boundary
#define MCASP_RXCLKCHK_RMIN_S   8
#define MCASP_RXCLKCHK_RPS_M    0x0000000F  // RCV clock check prescaler 0x0
                                            // 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8
#define MCASP_RXCLKCHK_RPS_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_REVTCTL register.
//
//******************************************************************************
#define MCASP_REVTCTL_RDATDMA   0x00000001  // RCV data DMA request 0 Enable
                                            // DMA Transfer 1 Disable DMA
                                            // Transfer
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_GBLCTLX register.
//
//******************************************************************************
#define MCASP_GBLCTLX_XFRST     0x00001000  // Frame sync generator reset 0 1
#define MCASP_GBLCTLX_XSMRST    0x00000800  // XMT state machine reset 0 1
#define MCASP_GBLCTLX_XSRCLR    0x00000400  // XMT serializer clear 0 1
#define MCASP_GBLCTLX_XHCLKRST  0x00000200  // XMT High Freq. clk Divider 0 1
#define MCASP_GBLCTLX_XCLKRST   0x00000100  // XMT clock divder reset 0 1
#define MCASP_GBLCTLX_RFRST     0x00000010
#define MCASP_GBLCTLX_RSMRST    0x00000008
#define MCASP_GBLCTLX_RSRCLKR   0x00000004
#define MCASP_GBLCTLX_RHCLKRST  0x00000002
#define MCASP_GBLCTLX_RCLKRST   0x00000001
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXMASK register.
//
//******************************************************************************
#define MCASP_TXMASK_XMASK31    0x80000000  // XMASK BIT 31 0 1
#define MCASP_TXMASK_XMASK30    0x40000000  // XMASK BIT 30 0 1
#define MCASP_TXMASK_XMASK29    0x20000000  // XMASK BIT 29 0 1
#define MCASP_TXMASK_XMASK28    0x10000000  // XMASK BIT 28 0 1
#define MCASP_TXMASK_XMASK27    0x08000000  // XMASK BIT 27 0 1
#define MCASP_TXMASK_XMASK26    0x04000000  // XMASK BIT 26 0 1
#define MCASP_TXMASK_XMASK25    0x02000000  // XMASK BIT 25 0 1
#define MCASP_TXMASK_XMASK24    0x01000000  // XMASK BIT 24 0 1
#define MCASP_TXMASK_XMASK23    0x00800000  // XMASK BIT 23 0 1
#define MCASP_TXMASK_XMASK22    0x00400000  // XMASK BIT 22 0 1
#define MCASP_TXMASK_XMASK21    0x00200000  // XMASK BIT 21 0 1
#define MCASP_TXMASK_XMASK20    0x00100000  // XMASK BIT 20 0 1
#define MCASP_TXMASK_XMASK19    0x00080000  // XMASK BIT 19 0 1
#define MCASP_TXMASK_XMASK18    0x00040000  // XMASK BIT 18 0 1
#define MCASP_TXMASK_XMASK17    0x00020000  // XMASK BIT 17 0 1
#define MCASP_TXMASK_XMASK16    0x00010000  // XMASK BIT 16 0 1
#define MCASP_TXMASK_XMASK15    0x00008000  // XMASK BIT 15 0 1
#define MCASP_TXMASK_XMASK14    0x00004000  // XMASK BIT 14 0 1
#define MCASP_TXMASK_XMASK13    0x00002000  // XMASK BIT 13 0 1
#define MCASP_TXMASK_XMASK12    0x00001000  // XMASK BIT 12 0 1
#define MCASP_TXMASK_XMASK11    0x00000800  // XMASK BIT 11 0 1
#define MCASP_TXMASK_XMASK10    0x00000400  // XMASK BIT 10 0 1
#define MCASP_TXMASK_XMASK9     0x00000200  // XMASK BIT 9 0 1
#define MCASP_TXMASK_XMASK8     0x00000100  // XMASK BIT 8 0 1
#define MCASP_TXMASK_XMASK7     0x00000080  // XMASK BIT 7 0 1
#define MCASP_TXMASK_XMASK6     0x00000040  // XMASK BIT 6 0 1
#define MCASP_TXMASK_XMASK5     0x00000020  // XMASK BIT 5 0 1
#define MCASP_TXMASK_XMASK4     0x00000010  // XMASK BIT 4 0 1
#define MCASP_TXMASK_XMASK3     0x00000008  // XMASK BIT 3 0 1
#define MCASP_TXMASK_XMASK2     0x00000004  // XMASK BIT 2 0 1
#define MCASP_TXMASK_XMASK1     0x00000002  // XMASK BIT 1 0 1
#define MCASP_TXMASK_XMASK0     0x00000001  // XMASK BIT 0 0 1
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXFMT register.
//
//******************************************************************************
#define MCASP_TXFMT_XDATDLY_M   0x00030000  // XMT Frame sync delay 0x0 0 Bit
                                            // delay 0x1 1 Bit delay 0x2 2 Bit
                                            // delay
#define MCASP_TXFMT_XDATDLY_S   16
#define MCASP_TXFMT_XRVRS       0x00008000  // XMT serial stream bit order 0 1
#define MCASP_TXFMT_XPAD_M      0x00006000  // Pad value 0x0 0x1 0x2
#define MCASP_TXFMT_XPAD_S      13
#define MCASP_TXFMT_XPBIT_M     0x00001F00  // Pad bit position
#define MCASP_TXFMT_XPBIT_S     8
#define MCASP_TXFMT_XSSZ_M      0x000000F0  // XMT slot Size 0x0 0x1 0x2 0x3
                                            // 0x4 0x5 0x6 0x7 0x8 0x9 0xA 0xB
                                            // 0xC 0xD 0xE 0xF
#define MCASP_TXFMT_XSSZ_S      4
#define MCASP_TXFMT_XBUSEL      0x00000008  // Write to XBUF using CPU/DMA 0
                                            // DMA port access 1 CPU port Access
#define MCASP_TXFMT_XROT_M      0x00000007  // Right Rotate Value 0x0 0x1 0x2
                                            // 0x3 0x4 0x5 0x6 0x7
#define MCASP_TXFMT_XROT_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXFMCTL register.
//
//******************************************************************************
#define MCASP_TXFMCTL_XMOD_M    0x0000FF80  // XMT Frame sync mode
#define MCASP_TXFMCTL_XMOD_S    7
#define MCASP_TXFMCTL_FXWID     0x00000010  // XMT Frame sync Duration 0 1
#define MCASP_TXFMCTL_FSXM      0x00000002  // XMT frame sync External 0 1
#define MCASP_TXFMCTL_FSXP      0x00000001  // XMT Frame sync Polarity 0 1
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_ACLKXCTL register.
//
//******************************************************************************
#define MCASP_ACLKXCTL_BUSY     0x00100000
#define MCASP_ACLKXCTL_DIVBUSY  0x00080000
#define MCASP_ACLKXCTL_ADJBUSY  0x00040000
#define MCASP_ACLKXCTL_CLKXADJ_M \
                                0x00030000

#define MCASP_ACLKXCTL_CLKXADJ_S 16
#define MCASP_ACLKXCTL_CLKXP    0x00000080  // XMT Clock Polarity 0 1
#define MCASP_ACLKXCTL_ASYNC    0x00000040  // XMT/RCV operation sync /Async 0
                                            // 1
#define MCASP_ACLKXCTL_CLKXM    0x00000020  // XMT clock source 0 1
#define MCASP_ACLKXCTL_CLKXDIV_M \
                                0x0000001F  // XMT clock devide ratio

#define MCASP_ACLKXCTL_CLKXDIV_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_AHCLKXCTL register.
//
//******************************************************************************
#define MCASP_AHCLKXCTL_BUSY    0x00100000
#define MCASP_AHCLKXCTL_DIVBUSY 0x00080000
#define MCASP_AHCLKXCTL_ADJBUSY 0x00040000
#define MCASP_AHCLKXCTL_HCLKXADJ_M \
                                0x00030000

#define MCASP_AHCLKXCTL_HCLKXADJ_S 16
#define MCASP_AHCLKXCTL_HCLKXM  0x00008000  // High Freq. XMT clock Source 0 1
#define MCASP_AHCLKXCTL_HCLKXP  0x00004000  // High Freq. clock Polarity Before
                                            // diviser 0 1
#define MCASP_AHCLKXCTL_HCLKXDIV_M \
                                0x00000FFF  // XMT clock Divide Ratio

#define MCASP_AHCLKXCTL_HCLKXDIV_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXTDM register.
//
//******************************************************************************
#define MCASP_TXTDM_XTDMS31     0x80000000  // XMT mode during TDM time slot 31
                                            // 0 1
#define MCASP_TXTDM_XTDMS30     0x40000000  // XMT mode during TDM time slot 30
                                            // 0 1
#define MCASP_TXTDM_XTDMS29     0x20000000  // XMT mode during TDM time slot 29
                                            // 0 1
#define MCASP_TXTDM_XTDMS28     0x10000000  // XMT mode during TDM time slot 28
                                            // 0 1
#define MCASP_TXTDM_XTDMS27     0x08000000  // XMT mode during TDM time slot 27
                                            // 0 1
#define MCASP_TXTDM_XTDMS26     0x04000000  // XMT mode during TDM time slot 26
                                            // 0 1
#define MCASP_TXTDM_XTDMS25     0x02000000  // XMT mode during TDM time slot 25
                                            // 0 1
#define MCASP_TXTDM_XTDMS24     0x01000000  // XMT mode during TDM time slot 24
                                            // 0 1
#define MCASP_TXTDM_XTDMS23     0x00800000  // XMT mode during TDM time slot 23
                                            // 0 1
#define MCASP_TXTDM_XTDMS22     0x00400000  // XMT mode during TDM time slot 22
                                            // 0 1
#define MCASP_TXTDM_XTDMS21     0x00200000  // XMT mode during TDM time slot 21
                                            // 0 1
#define MCASP_TXTDM_XTDMS20     0x00100000  // XMT mode during TDM time slot 20
                                            // 0 1
#define MCASP_TXTDM_XTDMS19     0x00080000  // XMT mode during TDM time slot 19
                                            // 0 1
#define MCASP_TXTDM_XTDMS18     0x00040000  // XMT mode during TDM time slot 18
                                            // 0 1
#define MCASP_TXTDM_XTDMS17     0x00020000  // XMT mode during TDM time slot 17
                                            // 0 1
#define MCASP_TXTDM_XTDMS16     0x00010000  // XMT mode during TDM time slot 16
                                            // 0 1
#define MCASP_TXTDM_XTDMS15     0x00008000  // XMT mode during TDM time slot 15
                                            // 0 1
#define MCASP_TXTDM_XTDMS14     0x00004000  // XMT mode during TDM time slot 14
                                            // 0 1
#define MCASP_TXTDM_XTDMS13     0x00002000  // XMT mode during TDM time slot 13
                                            // 0 1
#define MCASP_TXTDM_XTDMS12     0x00001000  // XMT mode during TDM time slot 12
                                            // 0 1
#define MCASP_TXTDM_XTDMS11     0x00000800  // XMT mode during TDM time slot 11
                                            // 0 1
#define MCASP_TXTDM_XTDMS10     0x00000400  // XMT mode during TDM time slot 10
                                            // 0 1
#define MCASP_TXTDM_XTDMS9      0x00000200  // XMT mode during TDM time slot 9
                                            // 0 1
#define MCASP_TXTDM_XTDMS8      0x00000100  // XMT mode during TDM time slot 8
                                            // 0 1
#define MCASP_TXTDM_XTDMS7      0x00000080  // XMT mode during TDM time slot 7
                                            // 0 1
#define MCASP_TXTDM_XTDMS6      0x00000040  // XMT mode during TDM time slot 6
                                            // 0 1
#define MCASP_TXTDM_XTDMS5      0x00000020  // XMT mode during TDM time slot 5
                                            // 0 1
#define MCASP_TXTDM_XTDMS4      0x00000010  // XMT mode during TDM time slot 4
                                            // 0 1
#define MCASP_TXTDM_XTDMS3      0x00000008  // XMT mode during TDM time slot 3
                                            // 0 1
#define MCASP_TXTDM_XTDMS2      0x00000004  // XMT mode during TDM time slot 2
                                            // 0 1
#define MCASP_TXTDM_XTDMS1      0x00000002  // XMT mode during TDM time slot 1
                                            // 0 1
#define MCASP_TXTDM_XTDMS0      0x00000001  // XMT mode during TDM time slot 0
                                            // 0 1
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_EVTCTLX register.
//
//******************************************************************************
#define MCASP_EVTCTLX_XSTAFRM   0x00000080  // XMT Start of Frame Interrupt 0 1
#define MCASP_EVTCTLX_XDATA     0x00000020  // XMT Data Interrupt 0 1
#define MCASP_EVTCTLX_XLAST     0x00000010  // XMT Last Slot Interrupt 0 1
#define MCASP_EVTCTLX_XDMAERR   0x00000008  // XMT DMA Bus Error 0 1
#define MCASP_EVTCTLX_XCKFAIL   0x00000004  // Bad Clock Interrupt 0 1
#define MCASP_EVTCTLX_XSYNCERR  0x00000002  // XMT Unexpected FSR Interrupt 0 1
#define MCASP_EVTCTLX_XUNDRN    0x00000001  // XMT Underrun Interrupt 0 1
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXSTAT register.
//
//******************************************************************************
#define MCASP_TXSTAT_XERR       0x00000100  // XMT Error 0 1
#define MCASP_TXSTAT_XDMAERR    0x00000080  // XMT DMA bus error 0 1
#define MCASP_TXSTAT_XSTAFRM    0x00000040  // Start of Frame-XMT 0 1
#define MCASP_TXSTAT_XDATA      0x00000020  // Data Ready Flag 0 1
#define MCASP_TXSTAT_XLAST      0x00000010  // Last Slot Interrupt Flag 0 1
#define MCASP_TXSTAT_XTDMSLOT   0x00000008  // EvenOdd Slot 0 1
#define MCASP_TXSTAT_XCKFAIL    0x00000004  // Bad Transmit Flag 0 1
#define MCASP_TXSTAT_XSYNCERR   0x00000002  // Unexpected XMT Frame sync flag 0
                                            // 1
#define MCASP_TXSTAT_XUNDRN     0x00000001  // XMT Underrun Flag 0 1
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXTDMSLOT register.
//
//******************************************************************************
#define MCASP_TXTDMSLOT_XSLOTCNT_M \
                                0x000003FF  // Current XMT time slot count
                                            // during reset the value of this
                                            // register is 0b0101111111 (0x17f)
                                            // and after reset 0

#define MCASP_TXTDMSLOT_XSLOTCNT_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXCLKCHK register.
//
//******************************************************************************
#define MCASP_TXCLKCHK_XCNT_M   0xFF000000  // XMT clock count value
#define MCASP_TXCLKCHK_XCNT_S   24
#define MCASP_TXCLKCHK_XMAX_M   0x00FF0000  // XMT clock maximum boundary
#define MCASP_TXCLKCHK_XMAX_S   16
#define MCASP_TXCLKCHK_XMIN_M   0x0000FF00  // XMT clock minimum boundary
#define MCASP_TXCLKCHK_XMIN_S   8
#define MCASP_TXCLKCHK_RESV     0x00000080  // Reserved
#define MCASP_TXCLKCHK_XPS_M    0x0000000F  // XMT clock check prescaler 0x0
                                            // 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8
#define MCASP_TXCLKCHK_XPS_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_XEVTCTL register.
//
//******************************************************************************
#define MCASP_XEVTCTL_XDATDMA   0x00000001  // XMT data DMA request 0 Enable
                                            // DMA Transfer 1 Disable DMA
                                            // Transfer
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_CLKADJEN register.
//
//******************************************************************************
#define MCASP_CLKADJEN_ENABLE   0x00000001  // One-shot clock adjust enable 0 1
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITCSRA0 register.
//
//******************************************************************************
#define MCASP_DITCSRA0_DITCSRA0_M \
                                0xFFFFFFFF  // Left (Even TDM slot ) Channel
                                            // status

#define MCASP_DITCSRA0_DITCSRA0_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITCSRA1 register.
//
//******************************************************************************
#define MCASP_DITCSRA1_DITCSRA1_M \
                                0xFFFFFFFF  // Left (Even TDM slot ) Channel
                                            // status

#define MCASP_DITCSRA1_DITCSRA1_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITCSRA2 register.
//
//******************************************************************************
#define MCASP_DITCSRA2_DITCSRA2_M \
                                0xFFFFFFFF  // Left (Even TDM slot ) Channel
                                            // status Register

#define MCASP_DITCSRA2_DITCSRA2_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITCSRA3 register.
//
//******************************************************************************
#define MCASP_DITCSRA3_DITCSRA3_M \
                                0xFFFFFFFF  // Left (Even TDM slot ) Channel
                                            // status Register

#define MCASP_DITCSRA3_DITCSRA3_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITCSRA4 register.
//
//******************************************************************************
#define MCASP_DITCSRA4_DITCSRA4_M \
                                0xFFFFFFFF  // Left (Even TDM slot ) Channel
                                            // status

#define MCASP_DITCSRA4_DITCSRA4_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITCSRA5 register.
//
//******************************************************************************
#define MCASP_DITCSRA5_DITCSRA5_M \
                                0xFFFFFFFF  // Left (Even TDM slot ) Channel
                                            // status

#define MCASP_DITCSRA5_DITCSRA5_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITCSRB0 register.
//
//******************************************************************************
#define MCASP_DITCSRB0_DITCSRB0_M \
                                0xFFFFFFFF  // Right (odd TDM slot ) Channel
                                            // status

#define MCASP_DITCSRB0_DITCSRB0_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITCSRB1 register.
//
//******************************************************************************
#define MCASP_DITCSRB1_DITCSRB1_M \
                                0xFFFFFFFF  // Right (odd TDM slot ) Channel
                                            // status

#define MCASP_DITCSRB1_DITCSRB1_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITCSRB2 register.
//
//******************************************************************************
#define MCASP_DITCSRB2_DITCSRB2_M \
                                0xFFFFFFFF  // Right (odd TDM slot ) Channel
                                            // status

#define MCASP_DITCSRB2_DITCSRB2_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITCSRB3 register.
//
//******************************************************************************
#define MCASP_DITCSRB3_DITCSRB3_M \
                                0xFFFFFFFF  // Right (odd TDM slot ) Channel
                                            // status

#define MCASP_DITCSRB3_DITCSRB3_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITCSRB4 register.
//
//******************************************************************************
#define MCASP_DITCSRB4_DITCSRB4_M \
                                0xFFFFFFFF  // Right (odd TDM slot ) Channel
                                            // status

#define MCASP_DITCSRB4_DITCSRB4_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITCSRB5 register.
//
//******************************************************************************
#define MCASP_DITCSRB5_DITCSRB5_M \
                                0xFFFFFFFF  // Right (odd TDM slot ) Channel
                                            // status

#define MCASP_DITCSRB5_DITCSRB5_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITUDRA0 register.
//
//******************************************************************************
#define MCASP_DITUDRA0_DITUDRA0_M \
                                0xFFFFFFFF  // Left (Even TDM slot ) User Data

#define MCASP_DITUDRA0_DITUDRA0_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITUDRA1 register.
//
//******************************************************************************
#define MCASP_DITUDRA1_DITUDRA1_M \
                                0xFFFFFFFF  // Left (Even TDM slot ) User Data

#define MCASP_DITUDRA1_DITUDRA1_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITUDRA2 register.
//
//******************************************************************************
#define MCASP_DITUDRA2_DITUDRA2_M \
                                0xFFFFFFFF  // Left (Even TDM slot ) User Data

#define MCASP_DITUDRA2_DITUDRA2_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITUDRA3 register.
//
//******************************************************************************
#define MCASP_DITUDRA3_DITUDRA3_M \
                                0xFFFFFFFF  // Left (Even TDM slot ) User Data

#define MCASP_DITUDRA3_DITUDRA3_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITUDRA4 register.
//
//******************************************************************************
#define MCASP_DITUDRA4_DITUDRA4_M \
                                0xFFFFFFFF  // Left (Even TDM slot ) User Data

#define MCASP_DITUDRA4_DITUDRA4_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITUDRA5 register.
//
//******************************************************************************
#define MCASP_DITUDRA5_DITUDRA5_M \
                                0xFFFFFFFF  // Left (Even TDM slot ) User Data

#define MCASP_DITUDRA5_DITUDRA5_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITUDRB0 register.
//
//******************************************************************************
#define MCASP_DITUDRB0_DITUDRB0_M \
                                0xFFFFFFFF  // Right (odd TDM slot ) User Data

#define MCASP_DITUDRB0_DITUDRB0_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITUDRB1 register.
//
//******************************************************************************
#define MCASP_DITUDRB1_DITUDRB1_M \
                                0xFFFFFFFF  // Right (odd TDM slot ) User Data

#define MCASP_DITUDRB1_DITUDRB1_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITUDRB2 register.
//
//******************************************************************************
#define MCASP_DITUDRB2_DITUDRB2_M \
                                0xFFFFFFFF  // Right (odd TDM slot ) User Data

#define MCASP_DITUDRB2_DITUDRB2_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITUDRB3 register.
//
//******************************************************************************
#define MCASP_DITUDRB3_DITUDRB3_M \
                                0xFFFFFFFF  // Right (odd TDM slot ) User Data

#define MCASP_DITUDRB3_DITUDRB3_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITUDRB4 register.
//
//******************************************************************************
#define MCASP_DITUDRB4_DITUDRB4_M \
                                0xFFFFFFFF  // Right (odd TDM slot ) User Data

#define MCASP_DITUDRB4_DITUDRB4_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_DITUDRB5 register.
//
//******************************************************************************
#define MCASP_DITUDRB5_DITUDRB5_M \
                                0xFFFFFFFF  // Right (odd TDM slot ) User Data

#define MCASP_DITUDRB5_DITUDRB5_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_XRSRCTL0 register.
//
//******************************************************************************
#define MCASP_XRSRCTL0_RRDY     0x00000020
#define MCASP_XRSRCTL0_XRDY     0x00000010
#define MCASP_XRSRCTL0_DISMOD_M 0x0000000C  // Serializer drive state 0x0 Tri
                                            // state 0x1 Reserved 0x2 Drive pin
                                            // low 0x3 Drive pin high
#define MCASP_XRSRCTL0_DISMOD_S 2
#define MCASP_XRSRCTL0_SRMOD_M  0x00000003  // Serializer Mode 0x0 InActive
                                            // mode 0x1 Transmit mode 0x2
                                            // Receive mode
#define MCASP_XRSRCTL0_SRMOD_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_XRSRCTL1 register.
//
//******************************************************************************
#define MCASP_XRSRCTL1_RRDY     0x00000020
#define MCASP_XRSRCTL1_XRDY     0x00000010
#define MCASP_XRSRCTL1_DISMOD_M 0x0000000C  // Serializer drive state 0x0 Tri
                                            // state 0x1 Reserved 0x2 Drive pin
                                            // low 0x3 Drive pin high
#define MCASP_XRSRCTL1_DISMOD_S 2
#define MCASP_XRSRCTL1_SRMOD_M  0x00000003  // Serializer Mode 0x0 InActive
                                            // mode 0x1 Transmit mode 0x2
                                            // Receive mode
#define MCASP_XRSRCTL1_SRMOD_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_XRSRCTL2 register.
//
//******************************************************************************
#define MCASP_XRSRCTL2_RRDY     0x00000020
#define MCASP_XRSRCTL2_XRDY     0x00000010
#define MCASP_XRSRCTL2_DISMOD_M 0x0000000C  // Serializer drive state 0x0 Tri
                                            // state 0x1 Reserved 0x2 Drive pin
                                            // low 0x3 Drive pin high
#define MCASP_XRSRCTL2_DISMOD_S 2
#define MCASP_XRSRCTL2_SRMOD_M  0x00000003  // Serializer Mode 0x0 InActive
                                            // mode 0x1 Transmit mode 0x2
                                            // Receive mode
#define MCASP_XRSRCTL2_SRMOD_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_XRSRCTL3 register.
//
//******************************************************************************
#define MCASP_XRSRCTL3_RRDY     0x00000020
#define MCASP_XRSRCTL3_XRDY     0x00000010
#define MCASP_XRSRCTL3_DISMOD_M 0x0000000C  // Serializer drive state 0x0 Tri
                                            // state 0x1 Reserved 0x2 Drive pin
                                            // low 0x3 Drive pin high
#define MCASP_XRSRCTL3_DISMOD_S 2
#define MCASP_XRSRCTL3_SRMOD_M  0x00000003  // Serializer Mode 0x0 InActive
                                            // mode 0x1 Transmit mode 0x2
                                            // Receive mode
#define MCASP_XRSRCTL3_SRMOD_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_XRSRCTL4 register.
//
//******************************************************************************
#define MCASP_XRSRCTL4_RRDY     0x00000020
#define MCASP_XRSRCTL4_XRDY     0x00000010
#define MCASP_XRSRCTL4_DISMOD_M 0x0000000C  // Serializer drive state 0x0 Tri
                                            // state 0x1 Reserved 0x2 Drive pin
                                            // low 0x3 Drive pin high
#define MCASP_XRSRCTL4_DISMOD_S 2
#define MCASP_XRSRCTL4_SRMOD_M  0x00000003  // Serializer Mode 0x0 InActive
                                            // mode 0x1 Transmit mode 0x2
                                            // Receive mode
#define MCASP_XRSRCTL4_SRMOD_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_XRSRCTL5 register.
//
//******************************************************************************
#define MCASP_XRSRCTL5_RRDY     0x00000020
#define MCASP_XRSRCTL5_XRDY     0x00000010
#define MCASP_XRSRCTL5_DISMOD_M 0x0000000C  // Serializer drive state 0x0 Tri
                                            // state 0x1 Reserved 0x2 Drive pin
                                            // low 0x3 Drive pin high
#define MCASP_XRSRCTL5_DISMOD_S 2
#define MCASP_XRSRCTL5_SRMOD_M  0x00000003  // Serializer Mode 0x0 InActive
                                            // mode 0x1 Transmit mode 0x2
                                            // Receive mode
#define MCASP_XRSRCTL5_SRMOD_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_XRSRCTL6 register.
//
//******************************************************************************
#define MCASP_XRSRCTL6_RRDY     0x00000020
#define MCASP_XRSRCTL6_XRDY     0x00000010
#define MCASP_XRSRCTL6_DISMOD_M 0x0000000C  // Serializer drive state 0x0 Tri
                                            // state 0x1 Reserved 0x2 Drive pin
                                            // low 0x3 Drive pin high
#define MCASP_XRSRCTL6_DISMOD_S 2
#define MCASP_XRSRCTL6_SRMOD_M  0x00000003  // Serializer Mode 0x0 InActive
                                            // mode 0x1 Transmit mode 0x2
                                            // Receive mode
#define MCASP_XRSRCTL6_SRMOD_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_XRSRCTL7 register.
//
//******************************************************************************
#define MCASP_XRSRCTL7_RRDY     0x00000020
#define MCASP_XRSRCTL7_XRDY     0x00000010
#define MCASP_XRSRCTL7_DISMOD_M 0x0000000C  // Serializer drive state 0x0 Tri
                                            // state 0x1 Reserved 0x2 Drive pin
                                            // low 0x3 Drive pin high
#define MCASP_XRSRCTL7_DISMOD_S 2
#define MCASP_XRSRCTL7_SRMOD_M  0x00000003  // Serializer Mode 0x0 InActive
                                            // mode 0x1 Transmit mode 0x2
                                            // Receive mode
#define MCASP_XRSRCTL7_SRMOD_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_XRSRCTL8 register.
//
//******************************************************************************
#define MCASP_XRSRCTL8_RRDY     0x00000020
#define MCASP_XRSRCTL8_XRDY     0x00000010
#define MCASP_XRSRCTL8_DISMOD_M 0x0000000C  // Serializer drive state 0x0 Tri
                                            // state 0x1 Reserved 0x2 Drive pin
                                            // low 0x3 Drive pin high
#define MCASP_XRSRCTL8_DISMOD_S 2
#define MCASP_XRSRCTL8_SRMOD_M  0x00000003  // Serializer Mode 0x0 InActive
                                            // mode 0x1 Transmit mode 0x2
                                            // Receive mode
#define MCASP_XRSRCTL8_SRMOD_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_XRSRCTL9 register.
//
//******************************************************************************
#define MCASP_XRSRCTL9_RRDY     0x00000020
#define MCASP_XRSRCTL9_XRDY     0x00000010
#define MCASP_XRSRCTL9_DISMOD_M 0x0000000C  // Serializer drive state 0x0 Tri
                                            // state 0x1 Reserved 0x2 Drive pin
                                            // low 0x3 Drive pin high
#define MCASP_XRSRCTL9_DISMOD_S 2
#define MCASP_XRSRCTL9_SRMOD_M  0x00000003  // Serializer Mode 0x0 InActive
                                            // mode 0x1 Transmit mode 0x2
                                            // Receive mode
#define MCASP_XRSRCTL9_SRMOD_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_XRSRCTL10 register.
//
//******************************************************************************
#define MCASP_XRSRCTL10_RRDY    0x00000020
#define MCASP_XRSRCTL10_XRDY    0x00000010
#define MCASP_XRSRCTL10_DISMOD_M \
                                0x0000000C  // Serializer drive state 0x0 Tri
                                            // state 0x1 Reserved 0x2 Drive pin
                                            // low 0x3 Drive pin high

#define MCASP_XRSRCTL10_DISMOD_S 2
#define MCASP_XRSRCTL10_SRMOD_M 0x00000003  // Serializer Mode 0x0 InActive
                                            // mode 0x1 Transmit mode 0x2
                                            // Receive mode
#define MCASP_XRSRCTL10_SRMOD_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_XRSRCTL11 register.
//
//******************************************************************************
#define MCASP_XRSRCTL11_RRDY    0x00000020
#define MCASP_XRSRCTL11_XRDY    0x00000010
#define MCASP_XRSRCTL11_DISMOD_M \
                                0x0000000C  // Serializer drive state 0x0 Tri
                                            // state 0x1 Reserved 0x2 Drive pin
                                            // low 0x3 Drive pin high

#define MCASP_XRSRCTL11_DISMOD_S 2
#define MCASP_XRSRCTL11_SRMOD_M 0x00000003  // Serializer Mode 0x0 InActive
                                            // mode 0x1 Transmit mode 0x2
                                            // Receive mode
#define MCASP_XRSRCTL11_SRMOD_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_XRSRCTL12 register.
//
//******************************************************************************
#define MCASP_XRSRCTL12_RRDY    0x00000020
#define MCASP_XRSRCTL12_XRDY    0x00000010
#define MCASP_XRSRCTL12_DISMOD_M \
                                0x0000000C  // Serializer drive state 0x0 Tri
                                            // state 0x1 Reserved 0x2 Drive pin
                                            // low 0x3 Drive pin high

#define MCASP_XRSRCTL12_DISMOD_S 2
#define MCASP_XRSRCTL12_SRMOD_M 0x00000003  // Serializer Mode 0x0 InActive
                                            // mode 0x1 Transmit mode 0x2
                                            // Receive mode
#define MCASP_XRSRCTL12_SRMOD_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_XRSRCTL13 register.
//
//******************************************************************************
#define MCASP_XRSRCTL13_RRDY    0x00000020
#define MCASP_XRSRCTL13_XRDY    0x00000010
#define MCASP_XRSRCTL13_DISMOD_M \
                                0x0000000C  // Serializer drive state 0x0 Tri
                                            // state 0x1 Reserved 0x2 Drive pin
                                            // low 0x3 Drive pin high

#define MCASP_XRSRCTL13_DISMOD_S 2
#define MCASP_XRSRCTL13_SRMOD_M 0x00000003  // Serializer Mode 0x0 InActive
                                            // mode 0x1 Transmit mode 0x2
                                            // Receive mode
#define MCASP_XRSRCTL13_SRMOD_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_XRSRCTL14 register.
//
//******************************************************************************
#define MCASP_XRSRCTL14_RRDY    0x00000020
#define MCASP_XRSRCTL14_XRDY    0x00000010
#define MCASP_XRSRCTL14_DISMOD_M \
                                0x0000000C  // Serializer drive state 0x0 Tri
                                            // state 0x1 Reserved 0x2 Drive pin
                                            // low 0x3 Drive pin high

#define MCASP_XRSRCTL14_DISMOD_S 2
#define MCASP_XRSRCTL14_SRMOD_M 0x00000003  // Serializer Mode 0x0 InActive
                                            // mode 0x1 Transmit mode 0x2
                                            // Receive mode
#define MCASP_XRSRCTL14_SRMOD_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_XRSRCTL15 register.
//
//******************************************************************************
#define MCASP_XRSRCTL15_RRDY    0x00000020
#define MCASP_XRSRCTL15_XRDY    0x00000010
#define MCASP_XRSRCTL15_DISMOD_M \
                                0x0000000C  // Serializer drive state 0x0 Tri
                                            // state 0x1 Reserved 0x2 Drive pin
                                            // low 0x3 Drive pin high

#define MCASP_XRSRCTL15_DISMOD_S 2
#define MCASP_XRSRCTL15_SRMOD_M 0x00000003  // Serializer Mode 0x0 InActive
                                            // mode 0x1 Transmit mode 0x2
                                            // Receive mode
#define MCASP_XRSRCTL15_SRMOD_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXBUF0 register.
//
//******************************************************************************
#define MCASP_TXBUF0_XBUF0_M    0xFFFFFFFF  // Transmit Buffer 0
#define MCASP_TXBUF0_XBUF0_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXBUF1 register.
//
//******************************************************************************
#define MCASP_TXBUF1_XBUF1_M    0xFFFFFFFF  // Transmit Buffer 1
#define MCASP_TXBUF1_XBUF1_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXBUF2 register.
//
//******************************************************************************
#define MCASP_TXBUF2_XBUF2_M    0xFFFFFFFF  // Transmit Buffer 2
#define MCASP_TXBUF2_XBUF2_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXBUF3 register.
//
//******************************************************************************
#define MCASP_TXBUF3_XBUF3_M    0xFFFFFFFF  // Transmit Buffer 3
#define MCASP_TXBUF3_XBUF3_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXBUF4 register.
//
//******************************************************************************
#define MCASP_TXBUF4_XBUF4_M    0xFFFFFFFF  // Transmit Buffer 4
#define MCASP_TXBUF4_XBUF4_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXBUF5 register.
//
//******************************************************************************
#define MCASP_TXBUF5_XBUF5_M    0xFFFFFFFF  // Transmit Buffer 5
#define MCASP_TXBUF5_XBUF5_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXBUF6 register.
//
//******************************************************************************
#define MCASP_TXBUF6_XBUF6_M    0xFFFFFFFF  // Transmit Buffer 6
#define MCASP_TXBUF6_XBUF6_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXBUF7 register.
//
//******************************************************************************
#define MCASP_TXBUF7_XBUF7_M    0xFFFFFFFF  // Transmit Buffer 7
#define MCASP_TXBUF7_XBUF7_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXBUF8 register.
//
//******************************************************************************
#define MCASP_TXBUF8_XBUF8_M    0xFFFFFFFF  // Transmit Buffer 8
#define MCASP_TXBUF8_XBUF8_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXBUF9 register.
//
//******************************************************************************
#define MCASP_TXBUF9_XBUF9_M    0xFFFFFFFF  // Transmit Buffer 9
#define MCASP_TXBUF9_XBUF9_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXBUF10 register.
//
//******************************************************************************
#define MCASP_TXBUF10_XBUF10_M  0xFFFFFFFF  // Transmit Buffer 10
#define MCASP_TXBUF10_XBUF10_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXBUF11 register.
//
//******************************************************************************
#define MCASP_TXBUF11_XBUF11_M  0xFFFFFFFF  // Transmit Buffer 11
#define MCASP_TXBUF11_XBUF11_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXBUF12 register.
//
//******************************************************************************
#define MCASP_TXBUF12_XBUF12_M  0xFFFFFFFF  // Transmit Buffer 12
#define MCASP_TXBUF12_XBUF12_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXBUF13 register.
//
//******************************************************************************
#define MCASP_TXBUF13_XBUF13_M  0xFFFFFFFF  // Transmit Buffer 13
#define MCASP_TXBUF13_XBUF13_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXBUF14 register.
//
//******************************************************************************
#define MCASP_TXBUF14_XBUF14_M  0xFFFFFFFF  // Transmit Buffer 14
#define MCASP_TXBUF14_XBUF14_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_TXBUF15 register.
//
//******************************************************************************
#define MCASP_TXBUF15_XBUF15_M  0xFFFFFFFF  // Transmit Buffer 15
#define MCASP_TXBUF15_XBUF15_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXBUF0 register.
//
//******************************************************************************
#define MCASP_RXBUF0_RBUF0_M    0xFFFFFFFF  // Receive Buffer 0
#define MCASP_RXBUF0_RBUF0_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXBUF1 register.
//
//******************************************************************************
#define MCASP_RXBUF1_RBUF1_M    0xFFFFFFFF  // Receive Buffer 1
#define MCASP_RXBUF1_RBUF1_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXBUF2 register.
//
//******************************************************************************
#define MCASP_RXBUF2_RBUF2_M    0xFFFFFFFF  // Receive Buffer 2
#define MCASP_RXBUF2_RBUF2_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXBUF3 register.
//
//******************************************************************************
#define MCASP_RXBUF3_RBUF3_M    0xFFFFFFFF  // Receive Buffer 3
#define MCASP_RXBUF3_RBUF3_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXBUF4 register.
//
//******************************************************************************
#define MCASP_RXBUF4_RBUF4_M    0xFFFFFFFF  // Receive Buffer 4
#define MCASP_RXBUF4_RBUF4_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXBUF5 register.
//
//******************************************************************************
#define MCASP_RXBUF5_RBUF5_M    0xFFFFFFFF  // Receive Buffer 5
#define MCASP_RXBUF5_RBUF5_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXBUF6 register.
//
//******************************************************************************
#define MCASP_RXBUF6_RBUF6_M    0xFFFFFFFF  // Receive Buffer 6
#define MCASP_RXBUF6_RBUF6_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXBUF7 register.
//
//******************************************************************************
#define MCASP_RXBUF7_RBUF7_M    0xFFFFFFFF  // Receive Buffer 7
#define MCASP_RXBUF7_RBUF7_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXBUF8 register.
//
//******************************************************************************
#define MCASP_RXBUF8_RBUF8_M    0xFFFFFFFF  // Receive Buffer 8
#define MCASP_RXBUF8_RBUF8_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXBUF9 register.
//
//******************************************************************************
#define MCASP_RXBUF9_RBUF9_M    0xFFFFFFFF  // Receive Buffer 9
#define MCASP_RXBUF9_RBUF9_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXBUF10 register.
//
//******************************************************************************
#define MCASP_RXBUF10_RBUF10_M  0xFFFFFFFF  // Receive Buffer 10
#define MCASP_RXBUF10_RBUF10_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXBUF11 register.
//
//******************************************************************************
#define MCASP_RXBUF11_RBUF11_M  0xFFFFFFFF  // Receive Buffer 11
#define MCASP_RXBUF11_RBUF11_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXBUF12 register.
//
//******************************************************************************
#define MCASP_RXBUF12_RBUF12_M  0xFFFFFFFF  // Receive Buffer 12
#define MCASP_RXBUF12_RBUF12_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXBUF13 register.
//
//******************************************************************************
#define MCASP_RXBUF13_RBUF13_M  0xFFFFFFFF  // Receive Buffer 13
#define MCASP_RXBUF13_RBUF13_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXBUF14 register.
//
//******************************************************************************
#define MCASP_RXBUF14_RBUF14_M  0xFFFFFFFF  // Receive Buffer 14
#define MCASP_RXBUF14_RBUF14_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the MCASP_O_RXBUF15 register.
//
//******************************************************************************
#define MCASP_RXBUF15_RBUF15_M  0xFFFFFFFF  // Receive Buffer 15
#define MCASP_RXBUF15_RBUF15_S  0



#endif // __HW_MCASP_H__

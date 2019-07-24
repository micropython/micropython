# ----------------------------------------------------------------------------
#          ATMEL Microcontroller Software Support  -  ROUSSET  -
# ----------------------------------------------------------------------------
#  DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
#  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
#  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
#  DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
#  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
#  OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# ----------------------------------------------------------------------------
# File Name           : AT91SAM7X256.tcl
# Object              : AT91SAM7X256 definitions
# Generated           : AT91 SW Application Group  11/02/2005 (15:17:30)
# 
# CVS Reference       : /AT91SAM7X256.pl/1.14/Tue Sep 13 15:06:52 2005//
# CVS Reference       : /SYS_SAM7X.pl/1.3/Tue Feb  1 17:01:43 2005//
# CVS Reference       : /MC_SAM7X.pl/1.2/Fri May 20 14:13:04 2005//
# CVS Reference       : /PMC_SAM7X.pl/1.4/Tue Feb  8 13:58:10 2005//
# CVS Reference       : /RSTC_SAM7X.pl/1.2/Wed Jul 13 14:57:50 2005//
# CVS Reference       : /UDP_SAM7X.pl/1.1/Tue May 10 11:35:35 2005//
# CVS Reference       : /PWM_SAM7X.pl/1.1/Tue May 10 11:53:07 2005//
# CVS Reference       : /AIC_6075B.pl/1.3/Fri May 20 14:01:30 2005//
# CVS Reference       : /PIO_6057A.pl/1.2/Thu Feb  3 10:18:28 2005//
# CVS Reference       : /RTTC_6081A.pl/1.2/Tue Nov  9 14:43:58 2004//
# CVS Reference       : /PITC_6079A.pl/1.2/Tue Nov  9 14:43:56 2004//
# CVS Reference       : /WDTC_6080A.pl/1.3/Tue Nov  9 14:44:00 2004//
# CVS Reference       : /VREG_6085B.pl/1.1/Tue Feb  1 16:05:48 2005//
# CVS Reference       : /PDC_6074C.pl/1.2/Thu Feb  3 08:48:54 2005//
# CVS Reference       : /DBGU_6059D.pl/1.1/Mon Jan 31 13:15:32 2005//
# CVS Reference       : /SPI_6088D.pl/1.3/Fri May 20 14:08:59 2005//
# CVS Reference       : /US_6089C.pl/1.1/Mon Jul 12 18:23:26 2004//
# CVS Reference       : /SSC_6078B.pl/1.1/Wed Jul 13 15:19:19 2005//
# CVS Reference       : /TWI_6061A.pl/1.1/Tue Jul 13 07:38:06 2004//
# CVS Reference       : /TC_6082A.pl/1.7/Fri Mar 11 12:52:17 2005//
# CVS Reference       : /CAN_6019B.pl/1.1/Tue Mar  8 12:42:22 2005//
# CVS Reference       : /EMACB_6119A.pl/1.6/Wed Jul 13 15:05:35 2005//
# CVS Reference       : /ADC_6051C.pl/1.1/Fri Oct 17 09:12:38 2003//
# ----------------------------------------------------------------------------


# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR System Peripherals
# *****************************************************************************

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Advanced Interrupt Controller
# *****************************************************************************
# -------- AIC_SMR : (AIC Offset: 0x0) Control Register -------- 
set AT91C_AIC_PRIOR       [expr 0x7 <<  0 ]
set 	AT91C_AIC_PRIOR_LOWEST               0x0
set 	AT91C_AIC_PRIOR_HIGHEST              0x7
set AT91C_AIC_SRCTYPE     [expr 0x3 <<  5 ]
set 	AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL       [expr 0x0 <<  5 ]
set 	AT91C_AIC_SRCTYPE_EXT_LOW_LEVEL        [expr 0x0 <<  5 ]
set 	AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE    [expr 0x1 <<  5 ]
set 	AT91C_AIC_SRCTYPE_EXT_NEGATIVE_EDGE    [expr 0x1 <<  5 ]
set 	AT91C_AIC_SRCTYPE_HIGH_LEVEL           [expr 0x2 <<  5 ]
set 	AT91C_AIC_SRCTYPE_POSITIVE_EDGE        [expr 0x3 <<  5 ]
# -------- AIC_CISR : (AIC Offset: 0x114) AIC Core Interrupt Status Register -------- 
set AT91C_AIC_NFIQ        [expr 0x1 <<  0 ]
set AT91C_AIC_NIRQ        [expr 0x1 <<  1 ]
# -------- AIC_DCR : (AIC Offset: 0x138) AIC Debug Control Register (Protect) -------- 
set AT91C_AIC_DCR_PROT    [expr 0x1 <<  0 ]
set AT91C_AIC_DCR_GMSK    [expr 0x1 <<  1 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Peripheral DMA Controller
# *****************************************************************************
# -------- PDC_PTCR : (PDC Offset: 0x20) PDC Transfer Control Register -------- 
set AT91C_PDC_RXTEN       [expr 0x1 <<  0 ]
set AT91C_PDC_RXTDIS      [expr 0x1 <<  1 ]
set AT91C_PDC_TXTEN       [expr 0x1 <<  8 ]
set AT91C_PDC_TXTDIS      [expr 0x1 <<  9 ]
# -------- PDC_PTSR : (PDC Offset: 0x24) PDC Transfer Status Register -------- 
set AT91C_PDC_RXTEN       [expr 0x1 <<  0 ]
set AT91C_PDC_TXTEN       [expr 0x1 <<  8 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Debug Unit
# *****************************************************************************
# -------- DBGU_CR : (DBGU Offset: 0x0) Debug Unit Control Register -------- 
set AT91C_US_RSTRX        [expr 0x1 <<  2 ]
set AT91C_US_RSTTX        [expr 0x1 <<  3 ]
set AT91C_US_RXEN         [expr 0x1 <<  4 ]
set AT91C_US_RXDIS        [expr 0x1 <<  5 ]
set AT91C_US_TXEN         [expr 0x1 <<  6 ]
set AT91C_US_TXDIS        [expr 0x1 <<  7 ]
set AT91C_US_RSTSTA       [expr 0x1 <<  8 ]
# -------- DBGU_MR : (DBGU Offset: 0x4) Debug Unit Mode Register -------- 
set AT91C_US_PAR          [expr 0x7 <<  9 ]
set 	AT91C_US_PAR_EVEN                 [expr 0x0 <<  9 ]
set 	AT91C_US_PAR_ODD                  [expr 0x1 <<  9 ]
set 	AT91C_US_PAR_SPACE                [expr 0x2 <<  9 ]
set 	AT91C_US_PAR_MARK                 [expr 0x3 <<  9 ]
set 	AT91C_US_PAR_NONE                 [expr 0x4 <<  9 ]
set 	AT91C_US_PAR_MULTI_DROP           [expr 0x6 <<  9 ]
set AT91C_US_CHMODE       [expr 0x3 << 14 ]
set 	AT91C_US_CHMODE_NORMAL               [expr 0x0 << 14 ]
set 	AT91C_US_CHMODE_AUTO                 [expr 0x1 << 14 ]
set 	AT91C_US_CHMODE_LOCAL                [expr 0x2 << 14 ]
set 	AT91C_US_CHMODE_REMOTE               [expr 0x3 << 14 ]
# -------- DBGU_IER : (DBGU Offset: 0x8) Debug Unit Interrupt Enable Register -------- 
set AT91C_US_RXRDY        [expr 0x1 <<  0 ]
set AT91C_US_TXRDY        [expr 0x1 <<  1 ]
set AT91C_US_ENDRX        [expr 0x1 <<  3 ]
set AT91C_US_ENDTX        [expr 0x1 <<  4 ]
set AT91C_US_OVRE         [expr 0x1 <<  5 ]
set AT91C_US_FRAME        [expr 0x1 <<  6 ]
set AT91C_US_PARE         [expr 0x1 <<  7 ]
set AT91C_US_TXEMPTY      [expr 0x1 <<  9 ]
set AT91C_US_TXBUFE       [expr 0x1 << 11 ]
set AT91C_US_RXBUFF       [expr 0x1 << 12 ]
set AT91C_US_COMM_TX      [expr 0x1 << 30 ]
set AT91C_US_COMM_RX      [expr 0x1 << 31 ]
# -------- DBGU_IDR : (DBGU Offset: 0xc) Debug Unit Interrupt Disable Register -------- 
set AT91C_US_RXRDY        [expr 0x1 <<  0 ]
set AT91C_US_TXRDY        [expr 0x1 <<  1 ]
set AT91C_US_ENDRX        [expr 0x1 <<  3 ]
set AT91C_US_ENDTX        [expr 0x1 <<  4 ]
set AT91C_US_OVRE         [expr 0x1 <<  5 ]
set AT91C_US_FRAME        [expr 0x1 <<  6 ]
set AT91C_US_PARE         [expr 0x1 <<  7 ]
set AT91C_US_TXEMPTY      [expr 0x1 <<  9 ]
set AT91C_US_TXBUFE       [expr 0x1 << 11 ]
set AT91C_US_RXBUFF       [expr 0x1 << 12 ]
set AT91C_US_COMM_TX      [expr 0x1 << 30 ]
set AT91C_US_COMM_RX      [expr 0x1 << 31 ]
# -------- DBGU_IMR : (DBGU Offset: 0x10) Debug Unit Interrupt Mask Register -------- 
set AT91C_US_RXRDY        [expr 0x1 <<  0 ]
set AT91C_US_TXRDY        [expr 0x1 <<  1 ]
set AT91C_US_ENDRX        [expr 0x1 <<  3 ]
set AT91C_US_ENDTX        [expr 0x1 <<  4 ]
set AT91C_US_OVRE         [expr 0x1 <<  5 ]
set AT91C_US_FRAME        [expr 0x1 <<  6 ]
set AT91C_US_PARE         [expr 0x1 <<  7 ]
set AT91C_US_TXEMPTY      [expr 0x1 <<  9 ]
set AT91C_US_TXBUFE       [expr 0x1 << 11 ]
set AT91C_US_RXBUFF       [expr 0x1 << 12 ]
set AT91C_US_COMM_TX      [expr 0x1 << 30 ]
set AT91C_US_COMM_RX      [expr 0x1 << 31 ]
# -------- DBGU_CSR : (DBGU Offset: 0x14) Debug Unit Channel Status Register -------- 
set AT91C_US_RXRDY        [expr 0x1 <<  0 ]
set AT91C_US_TXRDY        [expr 0x1 <<  1 ]
set AT91C_US_ENDRX        [expr 0x1 <<  3 ]
set AT91C_US_ENDTX        [expr 0x1 <<  4 ]
set AT91C_US_OVRE         [expr 0x1 <<  5 ]
set AT91C_US_FRAME        [expr 0x1 <<  6 ]
set AT91C_US_PARE         [expr 0x1 <<  7 ]
set AT91C_US_TXEMPTY      [expr 0x1 <<  9 ]
set AT91C_US_TXBUFE       [expr 0x1 << 11 ]
set AT91C_US_RXBUFF       [expr 0x1 << 12 ]
set AT91C_US_COMM_TX      [expr 0x1 << 30 ]
set AT91C_US_COMM_RX      [expr 0x1 << 31 ]
# -------- DBGU_FNTR : (DBGU Offset: 0x48) Debug Unit FORCE_NTRST Register -------- 
set AT91C_US_FORCE_NTRST  [expr 0x1 <<  0 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Parallel Input Output Controler
# *****************************************************************************

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Clock Generator Controler
# *****************************************************************************
# -------- CKGR_MOR : (CKGR Offset: 0x0) Main Oscillator Register -------- 
set AT91C_CKGR_MOSCEN     [expr 0x1 <<  0 ]
set AT91C_CKGR_OSCBYPASS  [expr 0x1 <<  1 ]
set AT91C_CKGR_OSCOUNT    [expr 0xFF <<  8 ]
# -------- CKGR_MCFR : (CKGR Offset: 0x4) Main Clock Frequency Register -------- 
set AT91C_CKGR_MAINF      [expr 0xFFFF <<  0 ]
set AT91C_CKGR_MAINRDY    [expr 0x1 << 16 ]
# -------- CKGR_PLLR : (CKGR Offset: 0xc) PLL B Register -------- 
set AT91C_CKGR_DIV        [expr 0xFF <<  0 ]
set 	AT91C_CKGR_DIV_0                    0x0
set 	AT91C_CKGR_DIV_BYPASS               0x1
set AT91C_CKGR_PLLCOUNT   [expr 0x3F <<  8 ]
set AT91C_CKGR_OUT        [expr 0x3 << 14 ]
set 	AT91C_CKGR_OUT_0                    [expr 0x0 << 14 ]
set 	AT91C_CKGR_OUT_1                    [expr 0x1 << 14 ]
set 	AT91C_CKGR_OUT_2                    [expr 0x2 << 14 ]
set 	AT91C_CKGR_OUT_3                    [expr 0x3 << 14 ]
set AT91C_CKGR_MUL        [expr 0x7FF << 16 ]
set AT91C_CKGR_USBDIV     [expr 0x3 << 28 ]
set 	AT91C_CKGR_USBDIV_0                    [expr 0x0 << 28 ]
set 	AT91C_CKGR_USBDIV_1                    [expr 0x1 << 28 ]
set 	AT91C_CKGR_USBDIV_2                    [expr 0x2 << 28 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Power Management Controler
# *****************************************************************************
# -------- PMC_SCER : (PMC Offset: 0x0) System Clock Enable Register -------- 
set AT91C_PMC_PCK         [expr 0x1 <<  0 ]
set AT91C_PMC_UDP         [expr 0x1 <<  7 ]
set AT91C_PMC_PCK0        [expr 0x1 <<  8 ]
set AT91C_PMC_PCK1        [expr 0x1 <<  9 ]
set AT91C_PMC_PCK2        [expr 0x1 << 10 ]
set AT91C_PMC_PCK3        [expr 0x1 << 11 ]
# -------- PMC_SCDR : (PMC Offset: 0x4) System Clock Disable Register -------- 
set AT91C_PMC_PCK         [expr 0x1 <<  0 ]
set AT91C_PMC_UDP         [expr 0x1 <<  7 ]
set AT91C_PMC_PCK0        [expr 0x1 <<  8 ]
set AT91C_PMC_PCK1        [expr 0x1 <<  9 ]
set AT91C_PMC_PCK2        [expr 0x1 << 10 ]
set AT91C_PMC_PCK3        [expr 0x1 << 11 ]
# -------- PMC_SCSR : (PMC Offset: 0x8) System Clock Status Register -------- 
set AT91C_PMC_PCK         [expr 0x1 <<  0 ]
set AT91C_PMC_UDP         [expr 0x1 <<  7 ]
set AT91C_PMC_PCK0        [expr 0x1 <<  8 ]
set AT91C_PMC_PCK1        [expr 0x1 <<  9 ]
set AT91C_PMC_PCK2        [expr 0x1 << 10 ]
set AT91C_PMC_PCK3        [expr 0x1 << 11 ]
# -------- CKGR_MOR : (PMC Offset: 0x20) Main Oscillator Register -------- 
set AT91C_CKGR_MOSCEN     [expr 0x1 <<  0 ]
set AT91C_CKGR_OSCBYPASS  [expr 0x1 <<  1 ]
set AT91C_CKGR_OSCOUNT    [expr 0xFF <<  8 ]
# -------- CKGR_MCFR : (PMC Offset: 0x24) Main Clock Frequency Register -------- 
set AT91C_CKGR_MAINF      [expr 0xFFFF <<  0 ]
set AT91C_CKGR_MAINRDY    [expr 0x1 << 16 ]
# -------- CKGR_PLLR : (PMC Offset: 0x2c) PLL B Register -------- 
set AT91C_CKGR_DIV        [expr 0xFF <<  0 ]
set 	AT91C_CKGR_DIV_0                    0x0
set 	AT91C_CKGR_DIV_BYPASS               0x1
set AT91C_CKGR_PLLCOUNT   [expr 0x3F <<  8 ]
set AT91C_CKGR_OUT        [expr 0x3 << 14 ]
set 	AT91C_CKGR_OUT_0                    [expr 0x0 << 14 ]
set 	AT91C_CKGR_OUT_1                    [expr 0x1 << 14 ]
set 	AT91C_CKGR_OUT_2                    [expr 0x2 << 14 ]
set 	AT91C_CKGR_OUT_3                    [expr 0x3 << 14 ]
set AT91C_CKGR_MUL        [expr 0x7FF << 16 ]
set AT91C_CKGR_USBDIV     [expr 0x3 << 28 ]
set 	AT91C_CKGR_USBDIV_0                    [expr 0x0 << 28 ]
set 	AT91C_CKGR_USBDIV_1                    [expr 0x1 << 28 ]
set 	AT91C_CKGR_USBDIV_2                    [expr 0x2 << 28 ]
# -------- PMC_MCKR : (PMC Offset: 0x30) Master Clock Register -------- 
set AT91C_PMC_CSS         [expr 0x3 <<  0 ]
set 	AT91C_PMC_CSS_SLOW_CLK             0x0
set 	AT91C_PMC_CSS_MAIN_CLK             0x1
set 	AT91C_PMC_CSS_PLL_CLK              0x3
set AT91C_PMC_PRES        [expr 0x7 <<  2 ]
set 	AT91C_PMC_PRES_CLK                  [expr 0x0 <<  2 ]
set 	AT91C_PMC_PRES_CLK_2                [expr 0x1 <<  2 ]
set 	AT91C_PMC_PRES_CLK_4                [expr 0x2 <<  2 ]
set 	AT91C_PMC_PRES_CLK_8                [expr 0x3 <<  2 ]
set 	AT91C_PMC_PRES_CLK_16               [expr 0x4 <<  2 ]
set 	AT91C_PMC_PRES_CLK_32               [expr 0x5 <<  2 ]
set 	AT91C_PMC_PRES_CLK_64               [expr 0x6 <<  2 ]
# -------- PMC_PCKR : (PMC Offset: 0x40) Programmable Clock Register -------- 
set AT91C_PMC_CSS         [expr 0x3 <<  0 ]
set 	AT91C_PMC_CSS_SLOW_CLK             0x0
set 	AT91C_PMC_CSS_MAIN_CLK             0x1
set 	AT91C_PMC_CSS_PLL_CLK              0x3
set AT91C_PMC_PRES        [expr 0x7 <<  2 ]
set 	AT91C_PMC_PRES_CLK                  [expr 0x0 <<  2 ]
set 	AT91C_PMC_PRES_CLK_2                [expr 0x1 <<  2 ]
set 	AT91C_PMC_PRES_CLK_4                [expr 0x2 <<  2 ]
set 	AT91C_PMC_PRES_CLK_8                [expr 0x3 <<  2 ]
set 	AT91C_PMC_PRES_CLK_16               [expr 0x4 <<  2 ]
set 	AT91C_PMC_PRES_CLK_32               [expr 0x5 <<  2 ]
set 	AT91C_PMC_PRES_CLK_64               [expr 0x6 <<  2 ]
# -------- PMC_IER : (PMC Offset: 0x60) PMC Interrupt Enable Register -------- 
set AT91C_PMC_MOSCS       [expr 0x1 <<  0 ]
set AT91C_PMC_LOCK        [expr 0x1 <<  2 ]
set AT91C_PMC_MCKRDY      [expr 0x1 <<  3 ]
set AT91C_PMC_PCK0RDY     [expr 0x1 <<  8 ]
set AT91C_PMC_PCK1RDY     [expr 0x1 <<  9 ]
set AT91C_PMC_PCK2RDY     [expr 0x1 << 10 ]
set AT91C_PMC_PCK3RDY     [expr 0x1 << 11 ]
# -------- PMC_IDR : (PMC Offset: 0x64) PMC Interrupt Disable Register -------- 
set AT91C_PMC_MOSCS       [expr 0x1 <<  0 ]
set AT91C_PMC_LOCK        [expr 0x1 <<  2 ]
set AT91C_PMC_MCKRDY      [expr 0x1 <<  3 ]
set AT91C_PMC_PCK0RDY     [expr 0x1 <<  8 ]
set AT91C_PMC_PCK1RDY     [expr 0x1 <<  9 ]
set AT91C_PMC_PCK2RDY     [expr 0x1 << 10 ]
set AT91C_PMC_PCK3RDY     [expr 0x1 << 11 ]
# -------- PMC_SR : (PMC Offset: 0x68) PMC Status Register -------- 
set AT91C_PMC_MOSCS       [expr 0x1 <<  0 ]
set AT91C_PMC_LOCK        [expr 0x1 <<  2 ]
set AT91C_PMC_MCKRDY      [expr 0x1 <<  3 ]
set AT91C_PMC_PCK0RDY     [expr 0x1 <<  8 ]
set AT91C_PMC_PCK1RDY     [expr 0x1 <<  9 ]
set AT91C_PMC_PCK2RDY     [expr 0x1 << 10 ]
set AT91C_PMC_PCK3RDY     [expr 0x1 << 11 ]
# -------- PMC_IMR : (PMC Offset: 0x6c) PMC Interrupt Mask Register -------- 
set AT91C_PMC_MOSCS       [expr 0x1 <<  0 ]
set AT91C_PMC_LOCK        [expr 0x1 <<  2 ]
set AT91C_PMC_MCKRDY      [expr 0x1 <<  3 ]
set AT91C_PMC_PCK0RDY     [expr 0x1 <<  8 ]
set AT91C_PMC_PCK1RDY     [expr 0x1 <<  9 ]
set AT91C_PMC_PCK2RDY     [expr 0x1 << 10 ]
set AT91C_PMC_PCK3RDY     [expr 0x1 << 11 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Reset Controller Interface
# *****************************************************************************
# -------- RSTC_RCR : (RSTC Offset: 0x0) Reset Control Register -------- 
set AT91C_RSTC_PROCRST    [expr 0x1 <<  0 ]
set AT91C_RSTC_PERRST     [expr 0x1 <<  2 ]
set AT91C_RSTC_EXTRST     [expr 0x1 <<  3 ]
set AT91C_RSTC_KEY        [expr 0xFF << 24 ]
# -------- RSTC_RSR : (RSTC Offset: 0x4) Reset Status Register -------- 
set AT91C_RSTC_URSTS      [expr 0x1 <<  0 ]
set AT91C_RSTC_BODSTS     [expr 0x1 <<  1 ]
set AT91C_RSTC_RSTTYP     [expr 0x7 <<  8 ]
set 	AT91C_RSTC_RSTTYP_POWERUP              [expr 0x0 <<  8 ]
set 	AT91C_RSTC_RSTTYP_WAKEUP               [expr 0x1 <<  8 ]
set 	AT91C_RSTC_RSTTYP_WATCHDOG             [expr 0x2 <<  8 ]
set 	AT91C_RSTC_RSTTYP_SOFTWARE             [expr 0x3 <<  8 ]
set 	AT91C_RSTC_RSTTYP_USER                 [expr 0x4 <<  8 ]
set 	AT91C_RSTC_RSTTYP_BROWNOUT             [expr 0x5 <<  8 ]
set AT91C_RSTC_NRSTL      [expr 0x1 << 16 ]
set AT91C_RSTC_SRCMP      [expr 0x1 << 17 ]
# -------- RSTC_RMR : (RSTC Offset: 0x8) Reset Mode Register -------- 
set AT91C_RSTC_URSTEN     [expr 0x1 <<  0 ]
set AT91C_RSTC_URSTIEN    [expr 0x1 <<  4 ]
set AT91C_RSTC_ERSTL      [expr 0xF <<  8 ]
set AT91C_RSTC_BODIEN     [expr 0x1 << 16 ]
set AT91C_RSTC_KEY        [expr 0xFF << 24 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Real Time Timer Controller Interface
# *****************************************************************************
# -------- RTTC_RTMR : (RTTC Offset: 0x0) Real-time Mode Register -------- 
set AT91C_RTTC_RTPRES     [expr 0xFFFF <<  0 ]
set AT91C_RTTC_ALMIEN     [expr 0x1 << 16 ]
set AT91C_RTTC_RTTINCIEN  [expr 0x1 << 17 ]
set AT91C_RTTC_RTTRST     [expr 0x1 << 18 ]
# -------- RTTC_RTAR : (RTTC Offset: 0x4) Real-time Alarm Register -------- 
set AT91C_RTTC_ALMV       [expr 0x0 <<  0 ]
# -------- RTTC_RTVR : (RTTC Offset: 0x8) Current Real-time Value Register -------- 
set AT91C_RTTC_CRTV       [expr 0x0 <<  0 ]
# -------- RTTC_RTSR : (RTTC Offset: 0xc) Real-time Status Register -------- 
set AT91C_RTTC_ALMS       [expr 0x1 <<  0 ]
set AT91C_RTTC_RTTINC     [expr 0x1 <<  1 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Periodic Interval Timer Controller Interface
# *****************************************************************************
# -------- PITC_PIMR : (PITC Offset: 0x0) Periodic Interval Mode Register -------- 
set AT91C_PITC_PIV        [expr 0xFFFFF <<  0 ]
set AT91C_PITC_PITEN      [expr 0x1 << 24 ]
set AT91C_PITC_PITIEN     [expr 0x1 << 25 ]
# -------- PITC_PISR : (PITC Offset: 0x4) Periodic Interval Status Register -------- 
set AT91C_PITC_PITS       [expr 0x1 <<  0 ]
# -------- PITC_PIVR : (PITC Offset: 0x8) Periodic Interval Value Register -------- 
set AT91C_PITC_CPIV       [expr 0xFFFFF <<  0 ]
set AT91C_PITC_PICNT      [expr 0xFFF << 20 ]
# -------- PITC_PIIR : (PITC Offset: 0xc) Periodic Interval Image Register -------- 
set AT91C_PITC_CPIV       [expr 0xFFFFF <<  0 ]
set AT91C_PITC_PICNT      [expr 0xFFF << 20 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Watchdog Timer Controller Interface
# *****************************************************************************
# -------- WDTC_WDCR : (WDTC Offset: 0x0) Periodic Interval Image Register -------- 
set AT91C_WDTC_WDRSTT     [expr 0x1 <<  0 ]
set AT91C_WDTC_KEY        [expr 0xFF << 24 ]
# -------- WDTC_WDMR : (WDTC Offset: 0x4) Watchdog Mode Register -------- 
set AT91C_WDTC_WDV        [expr 0xFFF <<  0 ]
set AT91C_WDTC_WDFIEN     [expr 0x1 << 12 ]
set AT91C_WDTC_WDRSTEN    [expr 0x1 << 13 ]
set AT91C_WDTC_WDRPROC    [expr 0x1 << 14 ]
set AT91C_WDTC_WDDIS      [expr 0x1 << 15 ]
set AT91C_WDTC_WDD        [expr 0xFFF << 16 ]
set AT91C_WDTC_WDDBGHLT   [expr 0x1 << 28 ]
set AT91C_WDTC_WDIDLEHLT  [expr 0x1 << 29 ]
# -------- WDTC_WDSR : (WDTC Offset: 0x8) Watchdog Status Register -------- 
set AT91C_WDTC_WDUNF      [expr 0x1 <<  0 ]
set AT91C_WDTC_WDERR      [expr 0x1 <<  1 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Voltage Regulator Mode Controller Interface
# *****************************************************************************
# -------- VREG_MR : (VREG Offset: 0x0) Voltage Regulator Mode Register -------- 
set AT91C_VREG_PSTDBY     [expr 0x1 <<  0 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Memory Controller Interface
# *****************************************************************************
# -------- MC_RCR : (MC Offset: 0x0) MC Remap Control Register -------- 
set AT91C_MC_RCB          [expr 0x1 <<  0 ]
# -------- MC_ASR : (MC Offset: 0x4) MC Abort Status Register -------- 
set AT91C_MC_UNDADD       [expr 0x1 <<  0 ]
set AT91C_MC_MISADD       [expr 0x1 <<  1 ]
set AT91C_MC_ABTSZ        [expr 0x3 <<  8 ]
set 	AT91C_MC_ABTSZ_BYTE                 [expr 0x0 <<  8 ]
set 	AT91C_MC_ABTSZ_HWORD                [expr 0x1 <<  8 ]
set 	AT91C_MC_ABTSZ_WORD                 [expr 0x2 <<  8 ]
set AT91C_MC_ABTTYP       [expr 0x3 << 10 ]
set 	AT91C_MC_ABTTYP_DATAR                [expr 0x0 << 10 ]
set 	AT91C_MC_ABTTYP_DATAW                [expr 0x1 << 10 ]
set 	AT91C_MC_ABTTYP_FETCH                [expr 0x2 << 10 ]
set AT91C_MC_MST0         [expr 0x1 << 16 ]
set AT91C_MC_MST1         [expr 0x1 << 17 ]
set AT91C_MC_SVMST0       [expr 0x1 << 24 ]
set AT91C_MC_SVMST1       [expr 0x1 << 25 ]
# -------- MC_FMR : (MC Offset: 0x60) MC Flash Mode Register -------- 
set AT91C_MC_FRDY         [expr 0x1 <<  0 ]
set AT91C_MC_LOCKE        [expr 0x1 <<  2 ]
set AT91C_MC_PROGE        [expr 0x1 <<  3 ]
set AT91C_MC_NEBP         [expr 0x1 <<  7 ]
set AT91C_MC_FWS          [expr 0x3 <<  8 ]
set 	AT91C_MC_FWS_0FWS                 [expr 0x0 <<  8 ]
set 	AT91C_MC_FWS_1FWS                 [expr 0x1 <<  8 ]
set 	AT91C_MC_FWS_2FWS                 [expr 0x2 <<  8 ]
set 	AT91C_MC_FWS_3FWS                 [expr 0x3 <<  8 ]
set AT91C_MC_FMCN         [expr 0xFF << 16 ]
# -------- MC_FCR : (MC Offset: 0x64) MC Flash Command Register -------- 
set AT91C_MC_FCMD         [expr 0xF <<  0 ]
set 	AT91C_MC_FCMD_START_PROG           0x1
set 	AT91C_MC_FCMD_LOCK                 0x2
set 	AT91C_MC_FCMD_PROG_AND_LOCK        0x3
set 	AT91C_MC_FCMD_UNLOCK               0x4
set 	AT91C_MC_FCMD_ERASE_ALL            0x8
set 	AT91C_MC_FCMD_SET_GP_NVM           0xB
set 	AT91C_MC_FCMD_CLR_GP_NVM           0xD
set 	AT91C_MC_FCMD_SET_SECURITY         0xF
set AT91C_MC_PAGEN        [expr 0x3FF <<  8 ]
set AT91C_MC_KEY          [expr 0xFF << 24 ]
# -------- MC_FSR : (MC Offset: 0x68) MC Flash Command Register -------- 
set AT91C_MC_FRDY         [expr 0x1 <<  0 ]
set AT91C_MC_LOCKE        [expr 0x1 <<  2 ]
set AT91C_MC_PROGE        [expr 0x1 <<  3 ]
set AT91C_MC_SECURITY     [expr 0x1 <<  4 ]
set AT91C_MC_GPNVM0       [expr 0x1 <<  8 ]
set AT91C_MC_GPNVM1       [expr 0x1 <<  9 ]
set AT91C_MC_GPNVM2       [expr 0x1 << 10 ]
set AT91C_MC_GPNVM3       [expr 0x1 << 11 ]
set AT91C_MC_GPNVM4       [expr 0x1 << 12 ]
set AT91C_MC_GPNVM5       [expr 0x1 << 13 ]
set AT91C_MC_GPNVM6       [expr 0x1 << 14 ]
set AT91C_MC_GPNVM7       [expr 0x1 << 15 ]
set AT91C_MC_LOCKS0       [expr 0x1 << 16 ]
set AT91C_MC_LOCKS1       [expr 0x1 << 17 ]
set AT91C_MC_LOCKS2       [expr 0x1 << 18 ]
set AT91C_MC_LOCKS3       [expr 0x1 << 19 ]
set AT91C_MC_LOCKS4       [expr 0x1 << 20 ]
set AT91C_MC_LOCKS5       [expr 0x1 << 21 ]
set AT91C_MC_LOCKS6       [expr 0x1 << 22 ]
set AT91C_MC_LOCKS7       [expr 0x1 << 23 ]
set AT91C_MC_LOCKS8       [expr 0x1 << 24 ]
set AT91C_MC_LOCKS9       [expr 0x1 << 25 ]
set AT91C_MC_LOCKS10      [expr 0x1 << 26 ]
set AT91C_MC_LOCKS11      [expr 0x1 << 27 ]
set AT91C_MC_LOCKS12      [expr 0x1 << 28 ]
set AT91C_MC_LOCKS13      [expr 0x1 << 29 ]
set AT91C_MC_LOCKS14      [expr 0x1 << 30 ]
set AT91C_MC_LOCKS15      [expr 0x1 << 31 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Serial Parallel Interface
# *****************************************************************************
# -------- SPI_CR : (SPI Offset: 0x0) SPI Control Register -------- 
set AT91C_SPI_SPIEN       [expr 0x1 <<  0 ]
set AT91C_SPI_SPIDIS      [expr 0x1 <<  1 ]
set AT91C_SPI_SWRST       [expr 0x1 <<  7 ]
set AT91C_SPI_LASTXFER    [expr 0x1 << 24 ]
# -------- SPI_MR : (SPI Offset: 0x4) SPI Mode Register -------- 
set AT91C_SPI_MSTR        [expr 0x1 <<  0 ]
set AT91C_SPI_PS          [expr 0x1 <<  1 ]
set 	AT91C_SPI_PS_FIXED                [expr 0x0 <<  1 ]
set 	AT91C_SPI_PS_VARIABLE             [expr 0x1 <<  1 ]
set AT91C_SPI_PCSDEC      [expr 0x1 <<  2 ]
set AT91C_SPI_FDIV        [expr 0x1 <<  3 ]
set AT91C_SPI_MODFDIS     [expr 0x1 <<  4 ]
set AT91C_SPI_LLB         [expr 0x1 <<  7 ]
set AT91C_SPI_PCS         [expr 0xF << 16 ]
set AT91C_SPI_DLYBCS      [expr 0xFF << 24 ]
# -------- SPI_RDR : (SPI Offset: 0x8) Receive Data Register -------- 
set AT91C_SPI_RD          [expr 0xFFFF <<  0 ]
set AT91C_SPI_RPCS        [expr 0xF << 16 ]
# -------- SPI_TDR : (SPI Offset: 0xc) Transmit Data Register -------- 
set AT91C_SPI_TD          [expr 0xFFFF <<  0 ]
set AT91C_SPI_TPCS        [expr 0xF << 16 ]
set AT91C_SPI_LASTXFER    [expr 0x1 << 24 ]
# -------- SPI_SR : (SPI Offset: 0x10) Status Register -------- 
set AT91C_SPI_RDRF        [expr 0x1 <<  0 ]
set AT91C_SPI_TDRE        [expr 0x1 <<  1 ]
set AT91C_SPI_MODF        [expr 0x1 <<  2 ]
set AT91C_SPI_OVRES       [expr 0x1 <<  3 ]
set AT91C_SPI_ENDRX       [expr 0x1 <<  4 ]
set AT91C_SPI_ENDTX       [expr 0x1 <<  5 ]
set AT91C_SPI_RXBUFF      [expr 0x1 <<  6 ]
set AT91C_SPI_TXBUFE      [expr 0x1 <<  7 ]
set AT91C_SPI_NSSR        [expr 0x1 <<  8 ]
set AT91C_SPI_TXEMPTY     [expr 0x1 <<  9 ]
set AT91C_SPI_SPIENS      [expr 0x1 << 16 ]
# -------- SPI_IER : (SPI Offset: 0x14) Interrupt Enable Register -------- 
set AT91C_SPI_RDRF        [expr 0x1 <<  0 ]
set AT91C_SPI_TDRE        [expr 0x1 <<  1 ]
set AT91C_SPI_MODF        [expr 0x1 <<  2 ]
set AT91C_SPI_OVRES       [expr 0x1 <<  3 ]
set AT91C_SPI_ENDRX       [expr 0x1 <<  4 ]
set AT91C_SPI_ENDTX       [expr 0x1 <<  5 ]
set AT91C_SPI_RXBUFF      [expr 0x1 <<  6 ]
set AT91C_SPI_TXBUFE      [expr 0x1 <<  7 ]
set AT91C_SPI_NSSR        [expr 0x1 <<  8 ]
set AT91C_SPI_TXEMPTY     [expr 0x1 <<  9 ]
# -------- SPI_IDR : (SPI Offset: 0x18) Interrupt Disable Register -------- 
set AT91C_SPI_RDRF        [expr 0x1 <<  0 ]
set AT91C_SPI_TDRE        [expr 0x1 <<  1 ]
set AT91C_SPI_MODF        [expr 0x1 <<  2 ]
set AT91C_SPI_OVRES       [expr 0x1 <<  3 ]
set AT91C_SPI_ENDRX       [expr 0x1 <<  4 ]
set AT91C_SPI_ENDTX       [expr 0x1 <<  5 ]
set AT91C_SPI_RXBUFF      [expr 0x1 <<  6 ]
set AT91C_SPI_TXBUFE      [expr 0x1 <<  7 ]
set AT91C_SPI_NSSR        [expr 0x1 <<  8 ]
set AT91C_SPI_TXEMPTY     [expr 0x1 <<  9 ]
# -------- SPI_IMR : (SPI Offset: 0x1c) Interrupt Mask Register -------- 
set AT91C_SPI_RDRF        [expr 0x1 <<  0 ]
set AT91C_SPI_TDRE        [expr 0x1 <<  1 ]
set AT91C_SPI_MODF        [expr 0x1 <<  2 ]
set AT91C_SPI_OVRES       [expr 0x1 <<  3 ]
set AT91C_SPI_ENDRX       [expr 0x1 <<  4 ]
set AT91C_SPI_ENDTX       [expr 0x1 <<  5 ]
set AT91C_SPI_RXBUFF      [expr 0x1 <<  6 ]
set AT91C_SPI_TXBUFE      [expr 0x1 <<  7 ]
set AT91C_SPI_NSSR        [expr 0x1 <<  8 ]
set AT91C_SPI_TXEMPTY     [expr 0x1 <<  9 ]
# -------- SPI_CSR : (SPI Offset: 0x30) Chip Select Register -------- 
set AT91C_SPI_CPOL        [expr 0x1 <<  0 ]
set AT91C_SPI_NCPHA       [expr 0x1 <<  1 ]
set AT91C_SPI_CSAAT       [expr 0x1 <<  3 ]
set AT91C_SPI_BITS        [expr 0xF <<  4 ]
set 	AT91C_SPI_BITS_8                    [expr 0x0 <<  4 ]
set 	AT91C_SPI_BITS_9                    [expr 0x1 <<  4 ]
set 	AT91C_SPI_BITS_10                   [expr 0x2 <<  4 ]
set 	AT91C_SPI_BITS_11                   [expr 0x3 <<  4 ]
set 	AT91C_SPI_BITS_12                   [expr 0x4 <<  4 ]
set 	AT91C_SPI_BITS_13                   [expr 0x5 <<  4 ]
set 	AT91C_SPI_BITS_14                   [expr 0x6 <<  4 ]
set 	AT91C_SPI_BITS_15                   [expr 0x7 <<  4 ]
set 	AT91C_SPI_BITS_16                   [expr 0x8 <<  4 ]
set AT91C_SPI_SCBR        [expr 0xFF <<  8 ]
set AT91C_SPI_DLYBS       [expr 0xFF << 16 ]
set AT91C_SPI_DLYBCT      [expr 0xFF << 24 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Usart
# *****************************************************************************
# -------- US_CR : (USART Offset: 0x0) Debug Unit Control Register -------- 
set AT91C_US_RSTRX        [expr 0x1 <<  2 ]
set AT91C_US_RSTTX        [expr 0x1 <<  3 ]
set AT91C_US_RXEN         [expr 0x1 <<  4 ]
set AT91C_US_RXDIS        [expr 0x1 <<  5 ]
set AT91C_US_TXEN         [expr 0x1 <<  6 ]
set AT91C_US_TXDIS        [expr 0x1 <<  7 ]
set AT91C_US_RSTSTA       [expr 0x1 <<  8 ]
set AT91C_US_STTBRK       [expr 0x1 <<  9 ]
set AT91C_US_STPBRK       [expr 0x1 << 10 ]
set AT91C_US_STTTO        [expr 0x1 << 11 ]
set AT91C_US_SENDA        [expr 0x1 << 12 ]
set AT91C_US_RSTIT        [expr 0x1 << 13 ]
set AT91C_US_RSTNACK      [expr 0x1 << 14 ]
set AT91C_US_RETTO        [expr 0x1 << 15 ]
set AT91C_US_DTREN        [expr 0x1 << 16 ]
set AT91C_US_DTRDIS       [expr 0x1 << 17 ]
set AT91C_US_RTSEN        [expr 0x1 << 18 ]
set AT91C_US_RTSDIS       [expr 0x1 << 19 ]
# -------- US_MR : (USART Offset: 0x4) Debug Unit Mode Register -------- 
set AT91C_US_USMODE       [expr 0xF <<  0 ]
set 	AT91C_US_USMODE_NORMAL               0x0
set 	AT91C_US_USMODE_RS485                0x1
set 	AT91C_US_USMODE_HWHSH                0x2
set 	AT91C_US_USMODE_MODEM                0x3
set 	AT91C_US_USMODE_ISO7816_0            0x4
set 	AT91C_US_USMODE_ISO7816_1            0x6
set 	AT91C_US_USMODE_IRDA                 0x8
set 	AT91C_US_USMODE_SWHSH                0xC
set AT91C_US_CLKS         [expr 0x3 <<  4 ]
set 	AT91C_US_CLKS_CLOCK                [expr 0x0 <<  4 ]
set 	AT91C_US_CLKS_FDIV1                [expr 0x1 <<  4 ]
set 	AT91C_US_CLKS_SLOW                 [expr 0x2 <<  4 ]
set 	AT91C_US_CLKS_EXT                  [expr 0x3 <<  4 ]
set AT91C_US_CHRL         [expr 0x3 <<  6 ]
set 	AT91C_US_CHRL_5_BITS               [expr 0x0 <<  6 ]
set 	AT91C_US_CHRL_6_BITS               [expr 0x1 <<  6 ]
set 	AT91C_US_CHRL_7_BITS               [expr 0x2 <<  6 ]
set 	AT91C_US_CHRL_8_BITS               [expr 0x3 <<  6 ]
set AT91C_US_SYNC         [expr 0x1 <<  8 ]
set AT91C_US_PAR          [expr 0x7 <<  9 ]
set 	AT91C_US_PAR_EVEN                 [expr 0x0 <<  9 ]
set 	AT91C_US_PAR_ODD                  [expr 0x1 <<  9 ]
set 	AT91C_US_PAR_SPACE                [expr 0x2 <<  9 ]
set 	AT91C_US_PAR_MARK                 [expr 0x3 <<  9 ]
set 	AT91C_US_PAR_NONE                 [expr 0x4 <<  9 ]
set 	AT91C_US_PAR_MULTI_DROP           [expr 0x6 <<  9 ]
set AT91C_US_NBSTOP       [expr 0x3 << 12 ]
set 	AT91C_US_NBSTOP_1_BIT                [expr 0x0 << 12 ]
set 	AT91C_US_NBSTOP_15_BIT               [expr 0x1 << 12 ]
set 	AT91C_US_NBSTOP_2_BIT                [expr 0x2 << 12 ]
set AT91C_US_CHMODE       [expr 0x3 << 14 ]
set 	AT91C_US_CHMODE_NORMAL               [expr 0x0 << 14 ]
set 	AT91C_US_CHMODE_AUTO                 [expr 0x1 << 14 ]
set 	AT91C_US_CHMODE_LOCAL                [expr 0x2 << 14 ]
set 	AT91C_US_CHMODE_REMOTE               [expr 0x3 << 14 ]
set AT91C_US_MSBF         [expr 0x1 << 16 ]
set AT91C_US_MODE9        [expr 0x1 << 17 ]
set AT91C_US_CKLO         [expr 0x1 << 18 ]
set AT91C_US_OVER         [expr 0x1 << 19 ]
set AT91C_US_INACK        [expr 0x1 << 20 ]
set AT91C_US_DSNACK       [expr 0x1 << 21 ]
set AT91C_US_MAX_ITER     [expr 0x1 << 24 ]
set AT91C_US_FILTER       [expr 0x1 << 28 ]
# -------- US_IER : (USART Offset: 0x8) Debug Unit Interrupt Enable Register -------- 
set AT91C_US_RXRDY        [expr 0x1 <<  0 ]
set AT91C_US_TXRDY        [expr 0x1 <<  1 ]
set AT91C_US_RXBRK        [expr 0x1 <<  2 ]
set AT91C_US_ENDRX        [expr 0x1 <<  3 ]
set AT91C_US_ENDTX        [expr 0x1 <<  4 ]
set AT91C_US_OVRE         [expr 0x1 <<  5 ]
set AT91C_US_FRAME        [expr 0x1 <<  6 ]
set AT91C_US_PARE         [expr 0x1 <<  7 ]
set AT91C_US_TIMEOUT      [expr 0x1 <<  8 ]
set AT91C_US_TXEMPTY      [expr 0x1 <<  9 ]
set AT91C_US_ITERATION    [expr 0x1 << 10 ]
set AT91C_US_TXBUFE       [expr 0x1 << 11 ]
set AT91C_US_RXBUFF       [expr 0x1 << 12 ]
set AT91C_US_NACK         [expr 0x1 << 13 ]
set AT91C_US_RIIC         [expr 0x1 << 16 ]
set AT91C_US_DSRIC        [expr 0x1 << 17 ]
set AT91C_US_DCDIC        [expr 0x1 << 18 ]
set AT91C_US_CTSIC        [expr 0x1 << 19 ]
# -------- US_IDR : (USART Offset: 0xc) Debug Unit Interrupt Disable Register -------- 
set AT91C_US_RXRDY        [expr 0x1 <<  0 ]
set AT91C_US_TXRDY        [expr 0x1 <<  1 ]
set AT91C_US_RXBRK        [expr 0x1 <<  2 ]
set AT91C_US_ENDRX        [expr 0x1 <<  3 ]
set AT91C_US_ENDTX        [expr 0x1 <<  4 ]
set AT91C_US_OVRE         [expr 0x1 <<  5 ]
set AT91C_US_FRAME        [expr 0x1 <<  6 ]
set AT91C_US_PARE         [expr 0x1 <<  7 ]
set AT91C_US_TIMEOUT      [expr 0x1 <<  8 ]
set AT91C_US_TXEMPTY      [expr 0x1 <<  9 ]
set AT91C_US_ITERATION    [expr 0x1 << 10 ]
set AT91C_US_TXBUFE       [expr 0x1 << 11 ]
set AT91C_US_RXBUFF       [expr 0x1 << 12 ]
set AT91C_US_NACK         [expr 0x1 << 13 ]
set AT91C_US_RIIC         [expr 0x1 << 16 ]
set AT91C_US_DSRIC        [expr 0x1 << 17 ]
set AT91C_US_DCDIC        [expr 0x1 << 18 ]
set AT91C_US_CTSIC        [expr 0x1 << 19 ]
# -------- US_IMR : (USART Offset: 0x10) Debug Unit Interrupt Mask Register -------- 
set AT91C_US_RXRDY        [expr 0x1 <<  0 ]
set AT91C_US_TXRDY        [expr 0x1 <<  1 ]
set AT91C_US_RXBRK        [expr 0x1 <<  2 ]
set AT91C_US_ENDRX        [expr 0x1 <<  3 ]
set AT91C_US_ENDTX        [expr 0x1 <<  4 ]
set AT91C_US_OVRE         [expr 0x1 <<  5 ]
set AT91C_US_FRAME        [expr 0x1 <<  6 ]
set AT91C_US_PARE         [expr 0x1 <<  7 ]
set AT91C_US_TIMEOUT      [expr 0x1 <<  8 ]
set AT91C_US_TXEMPTY      [expr 0x1 <<  9 ]
set AT91C_US_ITERATION    [expr 0x1 << 10 ]
set AT91C_US_TXBUFE       [expr 0x1 << 11 ]
set AT91C_US_RXBUFF       [expr 0x1 << 12 ]
set AT91C_US_NACK         [expr 0x1 << 13 ]
set AT91C_US_RIIC         [expr 0x1 << 16 ]
set AT91C_US_DSRIC        [expr 0x1 << 17 ]
set AT91C_US_DCDIC        [expr 0x1 << 18 ]
set AT91C_US_CTSIC        [expr 0x1 << 19 ]
# -------- US_CSR : (USART Offset: 0x14) Debug Unit Channel Status Register -------- 
set AT91C_US_RXRDY        [expr 0x1 <<  0 ]
set AT91C_US_TXRDY        [expr 0x1 <<  1 ]
set AT91C_US_RXBRK        [expr 0x1 <<  2 ]
set AT91C_US_ENDRX        [expr 0x1 <<  3 ]
set AT91C_US_ENDTX        [expr 0x1 <<  4 ]
set AT91C_US_OVRE         [expr 0x1 <<  5 ]
set AT91C_US_FRAME        [expr 0x1 <<  6 ]
set AT91C_US_PARE         [expr 0x1 <<  7 ]
set AT91C_US_TIMEOUT      [expr 0x1 <<  8 ]
set AT91C_US_TXEMPTY      [expr 0x1 <<  9 ]
set AT91C_US_ITERATION    [expr 0x1 << 10 ]
set AT91C_US_TXBUFE       [expr 0x1 << 11 ]
set AT91C_US_RXBUFF       [expr 0x1 << 12 ]
set AT91C_US_NACK         [expr 0x1 << 13 ]
set AT91C_US_RIIC         [expr 0x1 << 16 ]
set AT91C_US_DSRIC        [expr 0x1 << 17 ]
set AT91C_US_DCDIC        [expr 0x1 << 18 ]
set AT91C_US_CTSIC        [expr 0x1 << 19 ]
set AT91C_US_RI           [expr 0x1 << 20 ]
set AT91C_US_DSR          [expr 0x1 << 21 ]
set AT91C_US_DCD          [expr 0x1 << 22 ]
set AT91C_US_CTS          [expr 0x1 << 23 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Synchronous Serial Controller Interface
# *****************************************************************************
# -------- SSC_CR : (SSC Offset: 0x0) SSC Control Register -------- 
set AT91C_SSC_RXEN        [expr 0x1 <<  0 ]
set AT91C_SSC_RXDIS       [expr 0x1 <<  1 ]
set AT91C_SSC_TXEN        [expr 0x1 <<  8 ]
set AT91C_SSC_TXDIS       [expr 0x1 <<  9 ]
set AT91C_SSC_SWRST       [expr 0x1 << 15 ]
# -------- SSC_RCMR : (SSC Offset: 0x10) SSC Receive Clock Mode Register -------- 
set AT91C_SSC_CKS         [expr 0x3 <<  0 ]
set 	AT91C_SSC_CKS_DIV                  0x0
set 	AT91C_SSC_CKS_TK                   0x1
set 	AT91C_SSC_CKS_RK                   0x2
set AT91C_SSC_CKO         [expr 0x7 <<  2 ]
set 	AT91C_SSC_CKO_NONE                 [expr 0x0 <<  2 ]
set 	AT91C_SSC_CKO_CONTINOUS            [expr 0x1 <<  2 ]
set 	AT91C_SSC_CKO_DATA_TX              [expr 0x2 <<  2 ]
set AT91C_SSC_CKI         [expr 0x1 <<  5 ]
set AT91C_SSC_CKG         [expr 0x3 <<  6 ]
set 	AT91C_SSC_CKG_NONE                 [expr 0x0 <<  6 ]
set 	AT91C_SSC_CKG_LOW                  [expr 0x1 <<  6 ]
set 	AT91C_SSC_CKG_HIGH                 [expr 0x2 <<  6 ]
set AT91C_SSC_START       [expr 0xF <<  8 ]
set 	AT91C_SSC_START_CONTINOUS            [expr 0x0 <<  8 ]
set 	AT91C_SSC_START_TX                   [expr 0x1 <<  8 ]
set 	AT91C_SSC_START_LOW_RF               [expr 0x2 <<  8 ]
set 	AT91C_SSC_START_HIGH_RF              [expr 0x3 <<  8 ]
set 	AT91C_SSC_START_FALL_RF              [expr 0x4 <<  8 ]
set 	AT91C_SSC_START_RISE_RF              [expr 0x5 <<  8 ]
set 	AT91C_SSC_START_LEVEL_RF             [expr 0x6 <<  8 ]
set 	AT91C_SSC_START_EDGE_RF              [expr 0x7 <<  8 ]
set 	AT91C_SSC_START_0                    [expr 0x8 <<  8 ]
set AT91C_SSC_STOP        [expr 0x1 << 12 ]
set AT91C_SSC_STTDLY      [expr 0xFF << 16 ]
set AT91C_SSC_PERIOD      [expr 0xFF << 24 ]
# -------- SSC_RFMR : (SSC Offset: 0x14) SSC Receive Frame Mode Register -------- 
set AT91C_SSC_DATLEN      [expr 0x1F <<  0 ]
set AT91C_SSC_LOOP        [expr 0x1 <<  5 ]
set AT91C_SSC_MSBF        [expr 0x1 <<  7 ]
set AT91C_SSC_DATNB       [expr 0xF <<  8 ]
set AT91C_SSC_FSLEN       [expr 0xF << 16 ]
set AT91C_SSC_FSOS        [expr 0x7 << 20 ]
set 	AT91C_SSC_FSOS_NONE                 [expr 0x0 << 20 ]
set 	AT91C_SSC_FSOS_NEGATIVE             [expr 0x1 << 20 ]
set 	AT91C_SSC_FSOS_POSITIVE             [expr 0x2 << 20 ]
set 	AT91C_SSC_FSOS_LOW                  [expr 0x3 << 20 ]
set 	AT91C_SSC_FSOS_HIGH                 [expr 0x4 << 20 ]
set 	AT91C_SSC_FSOS_TOGGLE               [expr 0x5 << 20 ]
set AT91C_SSC_FSEDGE      [expr 0x1 << 24 ]
# -------- SSC_TCMR : (SSC Offset: 0x18) SSC Transmit Clock Mode Register -------- 
set AT91C_SSC_CKS         [expr 0x3 <<  0 ]
set 	AT91C_SSC_CKS_DIV                  0x0
set 	AT91C_SSC_CKS_TK                   0x1
set 	AT91C_SSC_CKS_RK                   0x2
set AT91C_SSC_CKO         [expr 0x7 <<  2 ]
set 	AT91C_SSC_CKO_NONE                 [expr 0x0 <<  2 ]
set 	AT91C_SSC_CKO_CONTINOUS            [expr 0x1 <<  2 ]
set 	AT91C_SSC_CKO_DATA_TX              [expr 0x2 <<  2 ]
set AT91C_SSC_CKI         [expr 0x1 <<  5 ]
set AT91C_SSC_CKG         [expr 0x3 <<  6 ]
set 	AT91C_SSC_CKG_NONE                 [expr 0x0 <<  6 ]
set 	AT91C_SSC_CKG_LOW                  [expr 0x1 <<  6 ]
set 	AT91C_SSC_CKG_HIGH                 [expr 0x2 <<  6 ]
set AT91C_SSC_START       [expr 0xF <<  8 ]
set 	AT91C_SSC_START_CONTINOUS            [expr 0x0 <<  8 ]
set 	AT91C_SSC_START_TX                   [expr 0x1 <<  8 ]
set 	AT91C_SSC_START_LOW_RF               [expr 0x2 <<  8 ]
set 	AT91C_SSC_START_HIGH_RF              [expr 0x3 <<  8 ]
set 	AT91C_SSC_START_FALL_RF              [expr 0x4 <<  8 ]
set 	AT91C_SSC_START_RISE_RF              [expr 0x5 <<  8 ]
set 	AT91C_SSC_START_LEVEL_RF             [expr 0x6 <<  8 ]
set 	AT91C_SSC_START_EDGE_RF              [expr 0x7 <<  8 ]
set 	AT91C_SSC_START_0                    [expr 0x8 <<  8 ]
set AT91C_SSC_STTDLY      [expr 0xFF << 16 ]
set AT91C_SSC_PERIOD      [expr 0xFF << 24 ]
# -------- SSC_TFMR : (SSC Offset: 0x1c) SSC Transmit Frame Mode Register -------- 
set AT91C_SSC_DATLEN      [expr 0x1F <<  0 ]
set AT91C_SSC_DATDEF      [expr 0x1 <<  5 ]
set AT91C_SSC_MSBF        [expr 0x1 <<  7 ]
set AT91C_SSC_DATNB       [expr 0xF <<  8 ]
set AT91C_SSC_FSLEN       [expr 0xF << 16 ]
set AT91C_SSC_FSOS        [expr 0x7 << 20 ]
set 	AT91C_SSC_FSOS_NONE                 [expr 0x0 << 20 ]
set 	AT91C_SSC_FSOS_NEGATIVE             [expr 0x1 << 20 ]
set 	AT91C_SSC_FSOS_POSITIVE             [expr 0x2 << 20 ]
set 	AT91C_SSC_FSOS_LOW                  [expr 0x3 << 20 ]
set 	AT91C_SSC_FSOS_HIGH                 [expr 0x4 << 20 ]
set 	AT91C_SSC_FSOS_TOGGLE               [expr 0x5 << 20 ]
set AT91C_SSC_FSDEN       [expr 0x1 << 23 ]
set AT91C_SSC_FSEDGE      [expr 0x1 << 24 ]
# -------- SSC_SR : (SSC Offset: 0x40) SSC Status Register -------- 
set AT91C_SSC_TXRDY       [expr 0x1 <<  0 ]
set AT91C_SSC_TXEMPTY     [expr 0x1 <<  1 ]
set AT91C_SSC_ENDTX       [expr 0x1 <<  2 ]
set AT91C_SSC_TXBUFE      [expr 0x1 <<  3 ]
set AT91C_SSC_RXRDY       [expr 0x1 <<  4 ]
set AT91C_SSC_OVRUN       [expr 0x1 <<  5 ]
set AT91C_SSC_ENDRX       [expr 0x1 <<  6 ]
set AT91C_SSC_RXBUFF      [expr 0x1 <<  7 ]
set AT91C_SSC_CP0         [expr 0x1 <<  8 ]
set AT91C_SSC_CP1         [expr 0x1 <<  9 ]
set AT91C_SSC_TXSYN       [expr 0x1 << 10 ]
set AT91C_SSC_RXSYN       [expr 0x1 << 11 ]
set AT91C_SSC_TXENA       [expr 0x1 << 16 ]
set AT91C_SSC_RXENA       [expr 0x1 << 17 ]
# -------- SSC_IER : (SSC Offset: 0x44) SSC Interrupt Enable Register -------- 
set AT91C_SSC_TXRDY       [expr 0x1 <<  0 ]
set AT91C_SSC_TXEMPTY     [expr 0x1 <<  1 ]
set AT91C_SSC_ENDTX       [expr 0x1 <<  2 ]
set AT91C_SSC_TXBUFE      [expr 0x1 <<  3 ]
set AT91C_SSC_RXRDY       [expr 0x1 <<  4 ]
set AT91C_SSC_OVRUN       [expr 0x1 <<  5 ]
set AT91C_SSC_ENDRX       [expr 0x1 <<  6 ]
set AT91C_SSC_RXBUFF      [expr 0x1 <<  7 ]
set AT91C_SSC_CP0         [expr 0x1 <<  8 ]
set AT91C_SSC_CP1         [expr 0x1 <<  9 ]
set AT91C_SSC_TXSYN       [expr 0x1 << 10 ]
set AT91C_SSC_RXSYN       [expr 0x1 << 11 ]
# -------- SSC_IDR : (SSC Offset: 0x48) SSC Interrupt Disable Register -------- 
set AT91C_SSC_TXRDY       [expr 0x1 <<  0 ]
set AT91C_SSC_TXEMPTY     [expr 0x1 <<  1 ]
set AT91C_SSC_ENDTX       [expr 0x1 <<  2 ]
set AT91C_SSC_TXBUFE      [expr 0x1 <<  3 ]
set AT91C_SSC_RXRDY       [expr 0x1 <<  4 ]
set AT91C_SSC_OVRUN       [expr 0x1 <<  5 ]
set AT91C_SSC_ENDRX       [expr 0x1 <<  6 ]
set AT91C_SSC_RXBUFF      [expr 0x1 <<  7 ]
set AT91C_SSC_CP0         [expr 0x1 <<  8 ]
set AT91C_SSC_CP1         [expr 0x1 <<  9 ]
set AT91C_SSC_TXSYN       [expr 0x1 << 10 ]
set AT91C_SSC_RXSYN       [expr 0x1 << 11 ]
# -------- SSC_IMR : (SSC Offset: 0x4c) SSC Interrupt Mask Register -------- 
set AT91C_SSC_TXRDY       [expr 0x1 <<  0 ]
set AT91C_SSC_TXEMPTY     [expr 0x1 <<  1 ]
set AT91C_SSC_ENDTX       [expr 0x1 <<  2 ]
set AT91C_SSC_TXBUFE      [expr 0x1 <<  3 ]
set AT91C_SSC_RXRDY       [expr 0x1 <<  4 ]
set AT91C_SSC_OVRUN       [expr 0x1 <<  5 ]
set AT91C_SSC_ENDRX       [expr 0x1 <<  6 ]
set AT91C_SSC_RXBUFF      [expr 0x1 <<  7 ]
set AT91C_SSC_CP0         [expr 0x1 <<  8 ]
set AT91C_SSC_CP1         [expr 0x1 <<  9 ]
set AT91C_SSC_TXSYN       [expr 0x1 << 10 ]
set AT91C_SSC_RXSYN       [expr 0x1 << 11 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Two-wire Interface
# *****************************************************************************
# -------- TWI_CR : (TWI Offset: 0x0) TWI Control Register -------- 
set AT91C_TWI_START       [expr 0x1 <<  0 ]
set AT91C_TWI_STOP        [expr 0x1 <<  1 ]
set AT91C_TWI_MSEN        [expr 0x1 <<  2 ]
set AT91C_TWI_MSDIS       [expr 0x1 <<  3 ]
set AT91C_TWI_SWRST       [expr 0x1 <<  7 ]
# -------- TWI_MMR : (TWI Offset: 0x4) TWI Master Mode Register -------- 
set AT91C_TWI_IADRSZ      [expr 0x3 <<  8 ]
set 	AT91C_TWI_IADRSZ_NO                   [expr 0x0 <<  8 ]
set 	AT91C_TWI_IADRSZ_1_BYTE               [expr 0x1 <<  8 ]
set 	AT91C_TWI_IADRSZ_2_BYTE               [expr 0x2 <<  8 ]
set 	AT91C_TWI_IADRSZ_3_BYTE               [expr 0x3 <<  8 ]
set AT91C_TWI_MREAD       [expr 0x1 << 12 ]
set AT91C_TWI_DADR        [expr 0x7F << 16 ]
# -------- TWI_CWGR : (TWI Offset: 0x10) TWI Clock Waveform Generator Register -------- 
set AT91C_TWI_CLDIV       [expr 0xFF <<  0 ]
set AT91C_TWI_CHDIV       [expr 0xFF <<  8 ]
set AT91C_TWI_CKDIV       [expr 0x7 << 16 ]
# -------- TWI_SR : (TWI Offset: 0x20) TWI Status Register -------- 
set AT91C_TWI_TXCOMP      [expr 0x1 <<  0 ]
set AT91C_TWI_RXRDY       [expr 0x1 <<  1 ]
set AT91C_TWI_TXRDY       [expr 0x1 <<  2 ]
set AT91C_TWI_OVRE        [expr 0x1 <<  6 ]
set AT91C_TWI_UNRE        [expr 0x1 <<  7 ]
set AT91C_TWI_NACK        [expr 0x1 <<  8 ]
# -------- TWI_IER : (TWI Offset: 0x24) TWI Interrupt Enable Register -------- 
set AT91C_TWI_TXCOMP      [expr 0x1 <<  0 ]
set AT91C_TWI_RXRDY       [expr 0x1 <<  1 ]
set AT91C_TWI_TXRDY       [expr 0x1 <<  2 ]
set AT91C_TWI_OVRE        [expr 0x1 <<  6 ]
set AT91C_TWI_UNRE        [expr 0x1 <<  7 ]
set AT91C_TWI_NACK        [expr 0x1 <<  8 ]
# -------- TWI_IDR : (TWI Offset: 0x28) TWI Interrupt Disable Register -------- 
set AT91C_TWI_TXCOMP      [expr 0x1 <<  0 ]
set AT91C_TWI_RXRDY       [expr 0x1 <<  1 ]
set AT91C_TWI_TXRDY       [expr 0x1 <<  2 ]
set AT91C_TWI_OVRE        [expr 0x1 <<  6 ]
set AT91C_TWI_UNRE        [expr 0x1 <<  7 ]
set AT91C_TWI_NACK        [expr 0x1 <<  8 ]
# -------- TWI_IMR : (TWI Offset: 0x2c) TWI Interrupt Mask Register -------- 
set AT91C_TWI_TXCOMP      [expr 0x1 <<  0 ]
set AT91C_TWI_RXRDY       [expr 0x1 <<  1 ]
set AT91C_TWI_TXRDY       [expr 0x1 <<  2 ]
set AT91C_TWI_OVRE        [expr 0x1 <<  6 ]
set AT91C_TWI_UNRE        [expr 0x1 <<  7 ]
set AT91C_TWI_NACK        [expr 0x1 <<  8 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR PWMC Channel Interface
# *****************************************************************************
# -------- PWMC_CMR : (PWMC_CH Offset: 0x0) PWMC Channel Mode Register -------- 
set AT91C_PWMC_CPRE       [expr 0xF <<  0 ]
set 	AT91C_PWMC_CPRE_MCK                  0x0
set 	AT91C_PWMC_CPRE_MCK/2                0x1
set 	AT91C_PWMC_CPRE_MCK/4                0x2
set 	AT91C_PWMC_CPRE_MCK/8                0x3
set 	AT91C_PWMC_CPRE_MCK/16               0x4
set 	AT91C_PWMC_CPRE_MCK/32               0x5
set 	AT91C_PWMC_CPRE_MCK/64               0x6
set 	AT91C_PWMC_CPRE_MCK/128              0x7
set 	AT91C_PWMC_CPRE_MCK/256              0x8
set 	AT91C_PWMC_CPRE_MCK/512              0x9
set 	AT91C_PWMC_CPRE_MCK/1024             0xA
set 	AT91C_PWMC_CPRE_MCKA                 0xB
set 	AT91C_PWMC_CPRE_MCKB                 0xC
set AT91C_PWMC_CALG       [expr 0x1 <<  8 ]
set AT91C_PWMC_CPOL       [expr 0x1 <<  9 ]
set AT91C_PWMC_CPD        [expr 0x1 << 10 ]
# -------- PWMC_CDTYR : (PWMC_CH Offset: 0x4) PWMC Channel Duty Cycle Register -------- 
set AT91C_PWMC_CDTY       [expr 0x0 <<  0 ]
# -------- PWMC_CPRDR : (PWMC_CH Offset: 0x8) PWMC Channel Period Register -------- 
set AT91C_PWMC_CPRD       [expr 0x0 <<  0 ]
# -------- PWMC_CCNTR : (PWMC_CH Offset: 0xc) PWMC Channel Counter Register -------- 
set AT91C_PWMC_CCNT       [expr 0x0 <<  0 ]
# -------- PWMC_CUPDR : (PWMC_CH Offset: 0x10) PWMC Channel Update Register -------- 
set AT91C_PWMC_CUPD       [expr 0x0 <<  0 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Pulse Width Modulation Controller Interface
# *****************************************************************************
# -------- PWMC_MR : (PWMC Offset: 0x0) PWMC Mode Register -------- 
set AT91C_PWMC_DIVA       [expr 0xFF <<  0 ]
set AT91C_PWMC_PREA       [expr 0xF <<  8 ]
set 	AT91C_PWMC_PREA_MCK                  [expr 0x0 <<  8 ]
set 	AT91C_PWMC_PREA_MCK/2                [expr 0x1 <<  8 ]
set 	AT91C_PWMC_PREA_MCK/4                [expr 0x2 <<  8 ]
set 	AT91C_PWMC_PREA_MCK/8                [expr 0x3 <<  8 ]
set 	AT91C_PWMC_PREA_MCK/16               [expr 0x4 <<  8 ]
set 	AT91C_PWMC_PREA_MCK/32               [expr 0x5 <<  8 ]
set 	AT91C_PWMC_PREA_MCK/64               [expr 0x6 <<  8 ]
set 	AT91C_PWMC_PREA_MCK/128              [expr 0x7 <<  8 ]
set 	AT91C_PWMC_PREA_MCK/256              [expr 0x8 <<  8 ]
set AT91C_PWMC_DIVB       [expr 0xFF << 16 ]
set AT91C_PWMC_PREB       [expr 0xF << 24 ]
set 	AT91C_PWMC_PREB_MCK                  [expr 0x0 << 24 ]
set 	AT91C_PWMC_PREB_MCK/2                [expr 0x1 << 24 ]
set 	AT91C_PWMC_PREB_MCK/4                [expr 0x2 << 24 ]
set 	AT91C_PWMC_PREB_MCK/8                [expr 0x3 << 24 ]
set 	AT91C_PWMC_PREB_MCK/16               [expr 0x4 << 24 ]
set 	AT91C_PWMC_PREB_MCK/32               [expr 0x5 << 24 ]
set 	AT91C_PWMC_PREB_MCK/64               [expr 0x6 << 24 ]
set 	AT91C_PWMC_PREB_MCK/128              [expr 0x7 << 24 ]
set 	AT91C_PWMC_PREB_MCK/256              [expr 0x8 << 24 ]
# -------- PWMC_ENA : (PWMC Offset: 0x4) PWMC Enable Register -------- 
set AT91C_PWMC_CHID0      [expr 0x1 <<  0 ]
set AT91C_PWMC_CHID1      [expr 0x1 <<  1 ]
set AT91C_PWMC_CHID2      [expr 0x1 <<  2 ]
set AT91C_PWMC_CHID3      [expr 0x1 <<  3 ]
# -------- PWMC_DIS : (PWMC Offset: 0x8) PWMC Disable Register -------- 
set AT91C_PWMC_CHID0      [expr 0x1 <<  0 ]
set AT91C_PWMC_CHID1      [expr 0x1 <<  1 ]
set AT91C_PWMC_CHID2      [expr 0x1 <<  2 ]
set AT91C_PWMC_CHID3      [expr 0x1 <<  3 ]
# -------- PWMC_SR : (PWMC Offset: 0xc) PWMC Status Register -------- 
set AT91C_PWMC_CHID0      [expr 0x1 <<  0 ]
set AT91C_PWMC_CHID1      [expr 0x1 <<  1 ]
set AT91C_PWMC_CHID2      [expr 0x1 <<  2 ]
set AT91C_PWMC_CHID3      [expr 0x1 <<  3 ]
# -------- PWMC_IER : (PWMC Offset: 0x10) PWMC Interrupt Enable Register -------- 
set AT91C_PWMC_CHID0      [expr 0x1 <<  0 ]
set AT91C_PWMC_CHID1      [expr 0x1 <<  1 ]
set AT91C_PWMC_CHID2      [expr 0x1 <<  2 ]
set AT91C_PWMC_CHID3      [expr 0x1 <<  3 ]
# -------- PWMC_IDR : (PWMC Offset: 0x14) PWMC Interrupt Disable Register -------- 
set AT91C_PWMC_CHID0      [expr 0x1 <<  0 ]
set AT91C_PWMC_CHID1      [expr 0x1 <<  1 ]
set AT91C_PWMC_CHID2      [expr 0x1 <<  2 ]
set AT91C_PWMC_CHID3      [expr 0x1 <<  3 ]
# -------- PWMC_IMR : (PWMC Offset: 0x18) PWMC Interrupt Mask Register -------- 
set AT91C_PWMC_CHID0      [expr 0x1 <<  0 ]
set AT91C_PWMC_CHID1      [expr 0x1 <<  1 ]
set AT91C_PWMC_CHID2      [expr 0x1 <<  2 ]
set AT91C_PWMC_CHID3      [expr 0x1 <<  3 ]
# -------- PWMC_ISR : (PWMC Offset: 0x1c) PWMC Interrupt Status Register -------- 
set AT91C_PWMC_CHID0      [expr 0x1 <<  0 ]
set AT91C_PWMC_CHID1      [expr 0x1 <<  1 ]
set AT91C_PWMC_CHID2      [expr 0x1 <<  2 ]
set AT91C_PWMC_CHID3      [expr 0x1 <<  3 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR USB Device Interface
# *****************************************************************************
# -------- UDP_FRM_NUM : (UDP Offset: 0x0) USB Frame Number Register -------- 
set AT91C_UDP_FRM_NUM     [expr 0x7FF <<  0 ]
set AT91C_UDP_FRM_ERR     [expr 0x1 << 16 ]
set AT91C_UDP_FRM_OK      [expr 0x1 << 17 ]
# -------- UDP_GLB_STATE : (UDP Offset: 0x4) USB Global State Register -------- 
set AT91C_UDP_FADDEN      [expr 0x1 <<  0 ]
set AT91C_UDP_CONFG       [expr 0x1 <<  1 ]
set AT91C_UDP_ESR         [expr 0x1 <<  2 ]
set AT91C_UDP_RSMINPR     [expr 0x1 <<  3 ]
set AT91C_UDP_RMWUPE      [expr 0x1 <<  4 ]
# -------- UDP_FADDR : (UDP Offset: 0x8) USB Function Address Register -------- 
set AT91C_UDP_FADD        [expr 0xFF <<  0 ]
set AT91C_UDP_FEN         [expr 0x1 <<  8 ]
# -------- UDP_IER : (UDP Offset: 0x10) USB Interrupt Enable Register -------- 
set AT91C_UDP_EPINT0      [expr 0x1 <<  0 ]
set AT91C_UDP_EPINT1      [expr 0x1 <<  1 ]
set AT91C_UDP_EPINT2      [expr 0x1 <<  2 ]
set AT91C_UDP_EPINT3      [expr 0x1 <<  3 ]
set AT91C_UDP_EPINT4      [expr 0x1 <<  4 ]
set AT91C_UDP_EPINT5      [expr 0x1 <<  5 ]
set AT91C_UDP_RXSUSP      [expr 0x1 <<  8 ]
set AT91C_UDP_RXRSM       [expr 0x1 <<  9 ]
set AT91C_UDP_EXTRSM      [expr 0x1 << 10 ]
set AT91C_UDP_SOFINT      [expr 0x1 << 11 ]
set AT91C_UDP_WAKEUP      [expr 0x1 << 13 ]
# -------- UDP_IDR : (UDP Offset: 0x14) USB Interrupt Disable Register -------- 
set AT91C_UDP_EPINT0      [expr 0x1 <<  0 ]
set AT91C_UDP_EPINT1      [expr 0x1 <<  1 ]
set AT91C_UDP_EPINT2      [expr 0x1 <<  2 ]
set AT91C_UDP_EPINT3      [expr 0x1 <<  3 ]
set AT91C_UDP_EPINT4      [expr 0x1 <<  4 ]
set AT91C_UDP_EPINT5      [expr 0x1 <<  5 ]
set AT91C_UDP_RXSUSP      [expr 0x1 <<  8 ]
set AT91C_UDP_RXRSM       [expr 0x1 <<  9 ]
set AT91C_UDP_EXTRSM      [expr 0x1 << 10 ]
set AT91C_UDP_SOFINT      [expr 0x1 << 11 ]
set AT91C_UDP_WAKEUP      [expr 0x1 << 13 ]
# -------- UDP_IMR : (UDP Offset: 0x18) USB Interrupt Mask Register -------- 
set AT91C_UDP_EPINT0      [expr 0x1 <<  0 ]
set AT91C_UDP_EPINT1      [expr 0x1 <<  1 ]
set AT91C_UDP_EPINT2      [expr 0x1 <<  2 ]
set AT91C_UDP_EPINT3      [expr 0x1 <<  3 ]
set AT91C_UDP_EPINT4      [expr 0x1 <<  4 ]
set AT91C_UDP_EPINT5      [expr 0x1 <<  5 ]
set AT91C_UDP_RXSUSP      [expr 0x1 <<  8 ]
set AT91C_UDP_RXRSM       [expr 0x1 <<  9 ]
set AT91C_UDP_EXTRSM      [expr 0x1 << 10 ]
set AT91C_UDP_SOFINT      [expr 0x1 << 11 ]
set AT91C_UDP_WAKEUP      [expr 0x1 << 13 ]
# -------- UDP_ISR : (UDP Offset: 0x1c) USB Interrupt Status Register -------- 
set AT91C_UDP_EPINT0      [expr 0x1 <<  0 ]
set AT91C_UDP_EPINT1      [expr 0x1 <<  1 ]
set AT91C_UDP_EPINT2      [expr 0x1 <<  2 ]
set AT91C_UDP_EPINT3      [expr 0x1 <<  3 ]
set AT91C_UDP_EPINT4      [expr 0x1 <<  4 ]
set AT91C_UDP_EPINT5      [expr 0x1 <<  5 ]
set AT91C_UDP_RXSUSP      [expr 0x1 <<  8 ]
set AT91C_UDP_RXRSM       [expr 0x1 <<  9 ]
set AT91C_UDP_EXTRSM      [expr 0x1 << 10 ]
set AT91C_UDP_SOFINT      [expr 0x1 << 11 ]
set AT91C_UDP_ENDBUSRES   [expr 0x1 << 12 ]
set AT91C_UDP_WAKEUP      [expr 0x1 << 13 ]
# -------- UDP_ICR : (UDP Offset: 0x20) USB Interrupt Clear Register -------- 
set AT91C_UDP_EPINT0      [expr 0x1 <<  0 ]
set AT91C_UDP_EPINT1      [expr 0x1 <<  1 ]
set AT91C_UDP_EPINT2      [expr 0x1 <<  2 ]
set AT91C_UDP_EPINT3      [expr 0x1 <<  3 ]
set AT91C_UDP_EPINT4      [expr 0x1 <<  4 ]
set AT91C_UDP_EPINT5      [expr 0x1 <<  5 ]
set AT91C_UDP_RXSUSP      [expr 0x1 <<  8 ]
set AT91C_UDP_RXRSM       [expr 0x1 <<  9 ]
set AT91C_UDP_EXTRSM      [expr 0x1 << 10 ]
set AT91C_UDP_SOFINT      [expr 0x1 << 11 ]
set AT91C_UDP_WAKEUP      [expr 0x1 << 13 ]
# -------- UDP_RST_EP : (UDP Offset: 0x28) USB Reset Endpoint Register -------- 
set AT91C_UDP_EP0         [expr 0x1 <<  0 ]
set AT91C_UDP_EP1         [expr 0x1 <<  1 ]
set AT91C_UDP_EP2         [expr 0x1 <<  2 ]
set AT91C_UDP_EP3         [expr 0x1 <<  3 ]
set AT91C_UDP_EP4         [expr 0x1 <<  4 ]
set AT91C_UDP_EP5         [expr 0x1 <<  5 ]
# -------- UDP_CSR : (UDP Offset: 0x30) USB Endpoint Control and Status Register -------- 
set AT91C_UDP_TXCOMP      [expr 0x1 <<  0 ]
set AT91C_UDP_RX_DATA_BK0 [expr 0x1 <<  1 ]
set AT91C_UDP_RXSETUP     [expr 0x1 <<  2 ]
set AT91C_UDP_ISOERROR    [expr 0x1 <<  3 ]
set AT91C_UDP_TXPKTRDY    [expr 0x1 <<  4 ]
set AT91C_UDP_FORCESTALL  [expr 0x1 <<  5 ]
set AT91C_UDP_RX_DATA_BK1 [expr 0x1 <<  6 ]
set AT91C_UDP_DIR         [expr 0x1 <<  7 ]
set AT91C_UDP_EPTYPE      [expr 0x7 <<  8 ]
set 	AT91C_UDP_EPTYPE_CTRL                 [expr 0x0 <<  8 ]
set 	AT91C_UDP_EPTYPE_ISO_OUT              [expr 0x1 <<  8 ]
set 	AT91C_UDP_EPTYPE_BULK_OUT             [expr 0x2 <<  8 ]
set 	AT91C_UDP_EPTYPE_INT_OUT              [expr 0x3 <<  8 ]
set 	AT91C_UDP_EPTYPE_ISO_IN               [expr 0x5 <<  8 ]
set 	AT91C_UDP_EPTYPE_BULK_IN              [expr 0x6 <<  8 ]
set 	AT91C_UDP_EPTYPE_INT_IN               [expr 0x7 <<  8 ]
set AT91C_UDP_DTGLE       [expr 0x1 << 11 ]
set AT91C_UDP_EPEDS       [expr 0x1 << 15 ]
set AT91C_UDP_RXBYTECNT   [expr 0x7FF << 16 ]
# -------- UDP_TXVC : (UDP Offset: 0x74) Transceiver Control Register -------- 
set AT91C_UDP_TXVDIS      [expr 0x1 <<  8 ]
set AT91C_UDP_PUON        [expr 0x1 <<  9 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Timer Counter Channel Interface
# *****************************************************************************
# -------- TC_CCR : (TC Offset: 0x0) TC Channel Control Register -------- 
set AT91C_TC_CLKEN        [expr 0x1 <<  0 ]
set AT91C_TC_CLKDIS       [expr 0x1 <<  1 ]
set AT91C_TC_SWTRG        [expr 0x1 <<  2 ]
# -------- TC_CMR : (TC Offset: 0x4) TC Channel Mode Register: Capture Mode / Waveform Mode -------- 
set AT91C_TC_CLKS         [expr 0x7 <<  0 ]
set 	AT91C_TC_CLKS_TIMER_DIV1_CLOCK     0x0
set 	AT91C_TC_CLKS_TIMER_DIV2_CLOCK     0x1
set 	AT91C_TC_CLKS_TIMER_DIV3_CLOCK     0x2
set 	AT91C_TC_CLKS_TIMER_DIV4_CLOCK     0x3
set 	AT91C_TC_CLKS_TIMER_DIV5_CLOCK     0x4
set 	AT91C_TC_CLKS_XC0                  0x5
set 	AT91C_TC_CLKS_XC1                  0x6
set 	AT91C_TC_CLKS_XC2                  0x7
set AT91C_TC_CLKS         [expr 0x7 <<  0 ]
set 	AT91C_TC_CLKS_TIMER_DIV1_CLOCK     0x0
set 	AT91C_TC_CLKS_TIMER_DIV2_CLOCK     0x1
set 	AT91C_TC_CLKS_TIMER_DIV3_CLOCK     0x2
set 	AT91C_TC_CLKS_TIMER_DIV4_CLOCK     0x3
set 	AT91C_TC_CLKS_TIMER_DIV5_CLOCK     0x4
set 	AT91C_TC_CLKS_XC0                  0x5
set 	AT91C_TC_CLKS_XC1                  0x6
set 	AT91C_TC_CLKS_XC2                  0x7
set AT91C_TC_CLKI         [expr 0x1 <<  3 ]
set AT91C_TC_CLKI         [expr 0x1 <<  3 ]
set AT91C_TC_BURST        [expr 0x3 <<  4 ]
set 	AT91C_TC_BURST_NONE                 [expr 0x0 <<  4 ]
set 	AT91C_TC_BURST_XC0                  [expr 0x1 <<  4 ]
set 	AT91C_TC_BURST_XC1                  [expr 0x2 <<  4 ]
set 	AT91C_TC_BURST_XC2                  [expr 0x3 <<  4 ]
set AT91C_TC_BURST        [expr 0x3 <<  4 ]
set 	AT91C_TC_BURST_NONE                 [expr 0x0 <<  4 ]
set 	AT91C_TC_BURST_XC0                  [expr 0x1 <<  4 ]
set 	AT91C_TC_BURST_XC1                  [expr 0x2 <<  4 ]
set 	AT91C_TC_BURST_XC2                  [expr 0x3 <<  4 ]
set AT91C_TC_CPCSTOP      [expr 0x1 <<  6 ]
set AT91C_TC_LDBSTOP      [expr 0x1 <<  6 ]
set AT91C_TC_CPCDIS       [expr 0x1 <<  7 ]
set AT91C_TC_LDBDIS       [expr 0x1 <<  7 ]
set AT91C_TC_ETRGEDG      [expr 0x3 <<  8 ]
set 	AT91C_TC_ETRGEDG_NONE                 [expr 0x0 <<  8 ]
set 	AT91C_TC_ETRGEDG_RISING               [expr 0x1 <<  8 ]
set 	AT91C_TC_ETRGEDG_FALLING              [expr 0x2 <<  8 ]
set 	AT91C_TC_ETRGEDG_BOTH                 [expr 0x3 <<  8 ]
set AT91C_TC_EEVTEDG      [expr 0x3 <<  8 ]
set 	AT91C_TC_EEVTEDG_NONE                 [expr 0x0 <<  8 ]
set 	AT91C_TC_EEVTEDG_RISING               [expr 0x1 <<  8 ]
set 	AT91C_TC_EEVTEDG_FALLING              [expr 0x2 <<  8 ]
set 	AT91C_TC_EEVTEDG_BOTH                 [expr 0x3 <<  8 ]
set AT91C_TC_EEVT         [expr 0x3 << 10 ]
set 	AT91C_TC_EEVT_TIOB                 [expr 0x0 << 10 ]
set 	AT91C_TC_EEVT_XC0                  [expr 0x1 << 10 ]
set 	AT91C_TC_EEVT_XC1                  [expr 0x2 << 10 ]
set 	AT91C_TC_EEVT_XC2                  [expr 0x3 << 10 ]
set AT91C_TC_ABETRG       [expr 0x1 << 10 ]
set AT91C_TC_ENETRG       [expr 0x1 << 12 ]
set AT91C_TC_WAVESEL      [expr 0x3 << 13 ]
set 	AT91C_TC_WAVESEL_UP                   [expr 0x0 << 13 ]
set 	AT91C_TC_WAVESEL_UPDOWN               [expr 0x1 << 13 ]
set 	AT91C_TC_WAVESEL_UP_AUTO              [expr 0x2 << 13 ]
set 	AT91C_TC_WAVESEL_UPDOWN_AUTO          [expr 0x3 << 13 ]
set AT91C_TC_CPCTRG       [expr 0x1 << 14 ]
set AT91C_TC_WAVE         [expr 0x1 << 15 ]
set AT91C_TC_WAVE         [expr 0x1 << 15 ]
set AT91C_TC_ACPA         [expr 0x3 << 16 ]
set 	AT91C_TC_ACPA_NONE                 [expr 0x0 << 16 ]
set 	AT91C_TC_ACPA_SET                  [expr 0x1 << 16 ]
set 	AT91C_TC_ACPA_CLEAR                [expr 0x2 << 16 ]
set 	AT91C_TC_ACPA_TOGGLE               [expr 0x3 << 16 ]
set AT91C_TC_LDRA         [expr 0x3 << 16 ]
set 	AT91C_TC_LDRA_NONE                 [expr 0x0 << 16 ]
set 	AT91C_TC_LDRA_RISING               [expr 0x1 << 16 ]
set 	AT91C_TC_LDRA_FALLING              [expr 0x2 << 16 ]
set 	AT91C_TC_LDRA_BOTH                 [expr 0x3 << 16 ]
set AT91C_TC_ACPC         [expr 0x3 << 18 ]
set 	AT91C_TC_ACPC_NONE                 [expr 0x0 << 18 ]
set 	AT91C_TC_ACPC_SET                  [expr 0x1 << 18 ]
set 	AT91C_TC_ACPC_CLEAR                [expr 0x2 << 18 ]
set 	AT91C_TC_ACPC_TOGGLE               [expr 0x3 << 18 ]
set AT91C_TC_LDRB         [expr 0x3 << 18 ]
set 	AT91C_TC_LDRB_NONE                 [expr 0x0 << 18 ]
set 	AT91C_TC_LDRB_RISING               [expr 0x1 << 18 ]
set 	AT91C_TC_LDRB_FALLING              [expr 0x2 << 18 ]
set 	AT91C_TC_LDRB_BOTH                 [expr 0x3 << 18 ]
set AT91C_TC_AEEVT        [expr 0x3 << 20 ]
set 	AT91C_TC_AEEVT_NONE                 [expr 0x0 << 20 ]
set 	AT91C_TC_AEEVT_SET                  [expr 0x1 << 20 ]
set 	AT91C_TC_AEEVT_CLEAR                [expr 0x2 << 20 ]
set 	AT91C_TC_AEEVT_TOGGLE               [expr 0x3 << 20 ]
set AT91C_TC_ASWTRG       [expr 0x3 << 22 ]
set 	AT91C_TC_ASWTRG_NONE                 [expr 0x0 << 22 ]
set 	AT91C_TC_ASWTRG_SET                  [expr 0x1 << 22 ]
set 	AT91C_TC_ASWTRG_CLEAR                [expr 0x2 << 22 ]
set 	AT91C_TC_ASWTRG_TOGGLE               [expr 0x3 << 22 ]
set AT91C_TC_BCPB         [expr 0x3 << 24 ]
set 	AT91C_TC_BCPB_NONE                 [expr 0x0 << 24 ]
set 	AT91C_TC_BCPB_SET                  [expr 0x1 << 24 ]
set 	AT91C_TC_BCPB_CLEAR                [expr 0x2 << 24 ]
set 	AT91C_TC_BCPB_TOGGLE               [expr 0x3 << 24 ]
set AT91C_TC_BCPC         [expr 0x3 << 26 ]
set 	AT91C_TC_BCPC_NONE                 [expr 0x0 << 26 ]
set 	AT91C_TC_BCPC_SET                  [expr 0x1 << 26 ]
set 	AT91C_TC_BCPC_CLEAR                [expr 0x2 << 26 ]
set 	AT91C_TC_BCPC_TOGGLE               [expr 0x3 << 26 ]
set AT91C_TC_BEEVT        [expr 0x3 << 28 ]
set 	AT91C_TC_BEEVT_NONE                 [expr 0x0 << 28 ]
set 	AT91C_TC_BEEVT_SET                  [expr 0x1 << 28 ]
set 	AT91C_TC_BEEVT_CLEAR                [expr 0x2 << 28 ]
set 	AT91C_TC_BEEVT_TOGGLE               [expr 0x3 << 28 ]
set AT91C_TC_BSWTRG       [expr 0x3 << 30 ]
set 	AT91C_TC_BSWTRG_NONE                 [expr 0x0 << 30 ]
set 	AT91C_TC_BSWTRG_SET                  [expr 0x1 << 30 ]
set 	AT91C_TC_BSWTRG_CLEAR                [expr 0x2 << 30 ]
set 	AT91C_TC_BSWTRG_TOGGLE               [expr 0x3 << 30 ]
# -------- TC_SR : (TC Offset: 0x20) TC Channel Status Register -------- 
set AT91C_TC_COVFS        [expr 0x1 <<  0 ]
set AT91C_TC_LOVRS        [expr 0x1 <<  1 ]
set AT91C_TC_CPAS         [expr 0x1 <<  2 ]
set AT91C_TC_CPBS         [expr 0x1 <<  3 ]
set AT91C_TC_CPCS         [expr 0x1 <<  4 ]
set AT91C_TC_LDRAS        [expr 0x1 <<  5 ]
set AT91C_TC_LDRBS        [expr 0x1 <<  6 ]
set AT91C_TC_ETRGS        [expr 0x1 <<  7 ]
set AT91C_TC_CLKSTA       [expr 0x1 << 16 ]
set AT91C_TC_MTIOA        [expr 0x1 << 17 ]
set AT91C_TC_MTIOB        [expr 0x1 << 18 ]
# -------- TC_IER : (TC Offset: 0x24) TC Channel Interrupt Enable Register -------- 
set AT91C_TC_COVFS        [expr 0x1 <<  0 ]
set AT91C_TC_LOVRS        [expr 0x1 <<  1 ]
set AT91C_TC_CPAS         [expr 0x1 <<  2 ]
set AT91C_TC_CPBS         [expr 0x1 <<  3 ]
set AT91C_TC_CPCS         [expr 0x1 <<  4 ]
set AT91C_TC_LDRAS        [expr 0x1 <<  5 ]
set AT91C_TC_LDRBS        [expr 0x1 <<  6 ]
set AT91C_TC_ETRGS        [expr 0x1 <<  7 ]
# -------- TC_IDR : (TC Offset: 0x28) TC Channel Interrupt Disable Register -------- 
set AT91C_TC_COVFS        [expr 0x1 <<  0 ]
set AT91C_TC_LOVRS        [expr 0x1 <<  1 ]
set AT91C_TC_CPAS         [expr 0x1 <<  2 ]
set AT91C_TC_CPBS         [expr 0x1 <<  3 ]
set AT91C_TC_CPCS         [expr 0x1 <<  4 ]
set AT91C_TC_LDRAS        [expr 0x1 <<  5 ]
set AT91C_TC_LDRBS        [expr 0x1 <<  6 ]
set AT91C_TC_ETRGS        [expr 0x1 <<  7 ]
# -------- TC_IMR : (TC Offset: 0x2c) TC Channel Interrupt Mask Register -------- 
set AT91C_TC_COVFS        [expr 0x1 <<  0 ]
set AT91C_TC_LOVRS        [expr 0x1 <<  1 ]
set AT91C_TC_CPAS         [expr 0x1 <<  2 ]
set AT91C_TC_CPBS         [expr 0x1 <<  3 ]
set AT91C_TC_CPCS         [expr 0x1 <<  4 ]
set AT91C_TC_LDRAS        [expr 0x1 <<  5 ]
set AT91C_TC_LDRBS        [expr 0x1 <<  6 ]
set AT91C_TC_ETRGS        [expr 0x1 <<  7 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Timer Counter Interface
# *****************************************************************************
# -------- TCB_BCR : (TCB Offset: 0xc0) TC Block Control Register -------- 
set AT91C_TCB_SYNC        [expr 0x1 <<  0 ]
# -------- TCB_BMR : (TCB Offset: 0xc4) TC Block Mode Register -------- 
set AT91C_TCB_TC0XC0S     [expr 0x3 <<  0 ]
set 	AT91C_TCB_TC0XC0S_TCLK0                0x0
set 	AT91C_TCB_TC0XC0S_NONE                 0x1
set 	AT91C_TCB_TC0XC0S_TIOA1                0x2
set 	AT91C_TCB_TC0XC0S_TIOA2                0x3
set AT91C_TCB_TC1XC1S     [expr 0x3 <<  2 ]
set 	AT91C_TCB_TC1XC1S_TCLK1                [expr 0x0 <<  2 ]
set 	AT91C_TCB_TC1XC1S_NONE                 [expr 0x1 <<  2 ]
set 	AT91C_TCB_TC1XC1S_TIOA0                [expr 0x2 <<  2 ]
set 	AT91C_TCB_TC1XC1S_TIOA2                [expr 0x3 <<  2 ]
set AT91C_TCB_TC2XC2S     [expr 0x3 <<  4 ]
set 	AT91C_TCB_TC2XC2S_TCLK2                [expr 0x0 <<  4 ]
set 	AT91C_TCB_TC2XC2S_NONE                 [expr 0x1 <<  4 ]
set 	AT91C_TCB_TC2XC2S_TIOA0                [expr 0x2 <<  4 ]
set 	AT91C_TCB_TC2XC2S_TIOA1                [expr 0x3 <<  4 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Control Area Network MailBox Interface
# *****************************************************************************
# -------- CAN_MMR : (CAN_MB Offset: 0x0) CAN Message Mode Register -------- 
set AT91C_CAN_MTIMEMARK   [expr 0xFFFF <<  0 ]
set AT91C_CAN_PRIOR       [expr 0xF << 16 ]
set AT91C_CAN_MOT         [expr 0x7 << 24 ]
set 	AT91C_CAN_MOT_DIS                  [expr 0x0 << 24 ]
set 	AT91C_CAN_MOT_RX                   [expr 0x1 << 24 ]
set 	AT91C_CAN_MOT_RXOVERWRITE          [expr 0x2 << 24 ]
set 	AT91C_CAN_MOT_TX                   [expr 0x3 << 24 ]
set 	AT91C_CAN_MOT_CONSUMER             [expr 0x4 << 24 ]
set 	AT91C_CAN_MOT_PRODUCER             [expr 0x5 << 24 ]
# -------- CAN_MAM : (CAN_MB Offset: 0x4) CAN Message Acceptance Mask Register -------- 
set AT91C_CAN_MIDvB       [expr 0x3FFFF <<  0 ]
set AT91C_CAN_MIDvA       [expr 0x7FF << 18 ]
set AT91C_CAN_MIDE        [expr 0x1 << 29 ]
# -------- CAN_MID : (CAN_MB Offset: 0x8) CAN Message ID Register -------- 
set AT91C_CAN_MIDvB       [expr 0x3FFFF <<  0 ]
set AT91C_CAN_MIDvA       [expr 0x7FF << 18 ]
set AT91C_CAN_MIDE        [expr 0x1 << 29 ]
# -------- CAN_MFID : (CAN_MB Offset: 0xc) CAN Message Family ID Register -------- 
# -------- CAN_MSR : (CAN_MB Offset: 0x10) CAN Message Status Register -------- 
set AT91C_CAN_MTIMESTAMP  [expr 0xFFFF <<  0 ]
set AT91C_CAN_MDLC        [expr 0xF << 16 ]
set AT91C_CAN_MRTR        [expr 0x1 << 20 ]
set AT91C_CAN_MABT        [expr 0x1 << 22 ]
set AT91C_CAN_MRDY        [expr 0x1 << 23 ]
set AT91C_CAN_MMI         [expr 0x1 << 24 ]
# -------- CAN_MDL : (CAN_MB Offset: 0x14) CAN Message Data Low Register -------- 
# -------- CAN_MDH : (CAN_MB Offset: 0x18) CAN Message Data High Register -------- 
# -------- CAN_MCR : (CAN_MB Offset: 0x1c) CAN Message Control Register -------- 
set AT91C_CAN_MDLC        [expr 0xF << 16 ]
set AT91C_CAN_MRTR        [expr 0x1 << 20 ]
set AT91C_CAN_MACR        [expr 0x1 << 22 ]
set AT91C_CAN_MTCR        [expr 0x1 << 23 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Control Area Network Interface
# *****************************************************************************
# -------- CAN_MR : (CAN Offset: 0x0) CAN Mode Register -------- 
set AT91C_CAN_CANEN       [expr 0x1 <<  0 ]
set AT91C_CAN_LPM         [expr 0x1 <<  1 ]
set AT91C_CAN_ABM         [expr 0x1 <<  2 ]
set AT91C_CAN_OVL         [expr 0x1 <<  3 ]
set AT91C_CAN_TEOF        [expr 0x1 <<  4 ]
set AT91C_CAN_TTM         [expr 0x1 <<  5 ]
set AT91C_CAN_TIMFRZ      [expr 0x1 <<  6 ]
set AT91C_CAN_DRPT        [expr 0x1 <<  7 ]
# -------- CAN_IER : (CAN Offset: 0x4) CAN Interrupt Enable Register -------- 
set AT91C_CAN_MB0         [expr 0x1 <<  0 ]
set AT91C_CAN_MB1         [expr 0x1 <<  1 ]
set AT91C_CAN_MB2         [expr 0x1 <<  2 ]
set AT91C_CAN_MB3         [expr 0x1 <<  3 ]
set AT91C_CAN_MB4         [expr 0x1 <<  4 ]
set AT91C_CAN_MB5         [expr 0x1 <<  5 ]
set AT91C_CAN_MB6         [expr 0x1 <<  6 ]
set AT91C_CAN_MB7         [expr 0x1 <<  7 ]
set AT91C_CAN_MB8         [expr 0x1 <<  8 ]
set AT91C_CAN_MB9         [expr 0x1 <<  9 ]
set AT91C_CAN_MB10        [expr 0x1 << 10 ]
set AT91C_CAN_MB11        [expr 0x1 << 11 ]
set AT91C_CAN_MB12        [expr 0x1 << 12 ]
set AT91C_CAN_MB13        [expr 0x1 << 13 ]
set AT91C_CAN_MB14        [expr 0x1 << 14 ]
set AT91C_CAN_MB15        [expr 0x1 << 15 ]
set AT91C_CAN_ERRA        [expr 0x1 << 16 ]
set AT91C_CAN_WARN        [expr 0x1 << 17 ]
set AT91C_CAN_ERRP        [expr 0x1 << 18 ]
set AT91C_CAN_BOFF        [expr 0x1 << 19 ]
set AT91C_CAN_SLEEP       [expr 0x1 << 20 ]
set AT91C_CAN_WAKEUP      [expr 0x1 << 21 ]
set AT91C_CAN_TOVF        [expr 0x1 << 22 ]
set AT91C_CAN_TSTP        [expr 0x1 << 23 ]
set AT91C_CAN_CERR        [expr 0x1 << 24 ]
set AT91C_CAN_SERR        [expr 0x1 << 25 ]
set AT91C_CAN_AERR        [expr 0x1 << 26 ]
set AT91C_CAN_FERR        [expr 0x1 << 27 ]
set AT91C_CAN_BERR        [expr 0x1 << 28 ]
# -------- CAN_IDR : (CAN Offset: 0x8) CAN Interrupt Disable Register -------- 
set AT91C_CAN_MB0         [expr 0x1 <<  0 ]
set AT91C_CAN_MB1         [expr 0x1 <<  1 ]
set AT91C_CAN_MB2         [expr 0x1 <<  2 ]
set AT91C_CAN_MB3         [expr 0x1 <<  3 ]
set AT91C_CAN_MB4         [expr 0x1 <<  4 ]
set AT91C_CAN_MB5         [expr 0x1 <<  5 ]
set AT91C_CAN_MB6         [expr 0x1 <<  6 ]
set AT91C_CAN_MB7         [expr 0x1 <<  7 ]
set AT91C_CAN_MB8         [expr 0x1 <<  8 ]
set AT91C_CAN_MB9         [expr 0x1 <<  9 ]
set AT91C_CAN_MB10        [expr 0x1 << 10 ]
set AT91C_CAN_MB11        [expr 0x1 << 11 ]
set AT91C_CAN_MB12        [expr 0x1 << 12 ]
set AT91C_CAN_MB13        [expr 0x1 << 13 ]
set AT91C_CAN_MB14        [expr 0x1 << 14 ]
set AT91C_CAN_MB15        [expr 0x1 << 15 ]
set AT91C_CAN_ERRA        [expr 0x1 << 16 ]
set AT91C_CAN_WARN        [expr 0x1 << 17 ]
set AT91C_CAN_ERRP        [expr 0x1 << 18 ]
set AT91C_CAN_BOFF        [expr 0x1 << 19 ]
set AT91C_CAN_SLEEP       [expr 0x1 << 20 ]
set AT91C_CAN_WAKEUP      [expr 0x1 << 21 ]
set AT91C_CAN_TOVF        [expr 0x1 << 22 ]
set AT91C_CAN_TSTP        [expr 0x1 << 23 ]
set AT91C_CAN_CERR        [expr 0x1 << 24 ]
set AT91C_CAN_SERR        [expr 0x1 << 25 ]
set AT91C_CAN_AERR        [expr 0x1 << 26 ]
set AT91C_CAN_FERR        [expr 0x1 << 27 ]
set AT91C_CAN_BERR        [expr 0x1 << 28 ]
# -------- CAN_IMR : (CAN Offset: 0xc) CAN Interrupt Mask Register -------- 
set AT91C_CAN_MB0         [expr 0x1 <<  0 ]
set AT91C_CAN_MB1         [expr 0x1 <<  1 ]
set AT91C_CAN_MB2         [expr 0x1 <<  2 ]
set AT91C_CAN_MB3         [expr 0x1 <<  3 ]
set AT91C_CAN_MB4         [expr 0x1 <<  4 ]
set AT91C_CAN_MB5         [expr 0x1 <<  5 ]
set AT91C_CAN_MB6         [expr 0x1 <<  6 ]
set AT91C_CAN_MB7         [expr 0x1 <<  7 ]
set AT91C_CAN_MB8         [expr 0x1 <<  8 ]
set AT91C_CAN_MB9         [expr 0x1 <<  9 ]
set AT91C_CAN_MB10        [expr 0x1 << 10 ]
set AT91C_CAN_MB11        [expr 0x1 << 11 ]
set AT91C_CAN_MB12        [expr 0x1 << 12 ]
set AT91C_CAN_MB13        [expr 0x1 << 13 ]
set AT91C_CAN_MB14        [expr 0x1 << 14 ]
set AT91C_CAN_MB15        [expr 0x1 << 15 ]
set AT91C_CAN_ERRA        [expr 0x1 << 16 ]
set AT91C_CAN_WARN        [expr 0x1 << 17 ]
set AT91C_CAN_ERRP        [expr 0x1 << 18 ]
set AT91C_CAN_BOFF        [expr 0x1 << 19 ]
set AT91C_CAN_SLEEP       [expr 0x1 << 20 ]
set AT91C_CAN_WAKEUP      [expr 0x1 << 21 ]
set AT91C_CAN_TOVF        [expr 0x1 << 22 ]
set AT91C_CAN_TSTP        [expr 0x1 << 23 ]
set AT91C_CAN_CERR        [expr 0x1 << 24 ]
set AT91C_CAN_SERR        [expr 0x1 << 25 ]
set AT91C_CAN_AERR        [expr 0x1 << 26 ]
set AT91C_CAN_FERR        [expr 0x1 << 27 ]
set AT91C_CAN_BERR        [expr 0x1 << 28 ]
# -------- CAN_SR : (CAN Offset: 0x10) CAN Status Register -------- 
set AT91C_CAN_MB0         [expr 0x1 <<  0 ]
set AT91C_CAN_MB1         [expr 0x1 <<  1 ]
set AT91C_CAN_MB2         [expr 0x1 <<  2 ]
set AT91C_CAN_MB3         [expr 0x1 <<  3 ]
set AT91C_CAN_MB4         [expr 0x1 <<  4 ]
set AT91C_CAN_MB5         [expr 0x1 <<  5 ]
set AT91C_CAN_MB6         [expr 0x1 <<  6 ]
set AT91C_CAN_MB7         [expr 0x1 <<  7 ]
set AT91C_CAN_MB8         [expr 0x1 <<  8 ]
set AT91C_CAN_MB9         [expr 0x1 <<  9 ]
set AT91C_CAN_MB10        [expr 0x1 << 10 ]
set AT91C_CAN_MB11        [expr 0x1 << 11 ]
set AT91C_CAN_MB12        [expr 0x1 << 12 ]
set AT91C_CAN_MB13        [expr 0x1 << 13 ]
set AT91C_CAN_MB14        [expr 0x1 << 14 ]
set AT91C_CAN_MB15        [expr 0x1 << 15 ]
set AT91C_CAN_ERRA        [expr 0x1 << 16 ]
set AT91C_CAN_WARN        [expr 0x1 << 17 ]
set AT91C_CAN_ERRP        [expr 0x1 << 18 ]
set AT91C_CAN_BOFF        [expr 0x1 << 19 ]
set AT91C_CAN_SLEEP       [expr 0x1 << 20 ]
set AT91C_CAN_WAKEUP      [expr 0x1 << 21 ]
set AT91C_CAN_TOVF        [expr 0x1 << 22 ]
set AT91C_CAN_TSTP        [expr 0x1 << 23 ]
set AT91C_CAN_CERR        [expr 0x1 << 24 ]
set AT91C_CAN_SERR        [expr 0x1 << 25 ]
set AT91C_CAN_AERR        [expr 0x1 << 26 ]
set AT91C_CAN_FERR        [expr 0x1 << 27 ]
set AT91C_CAN_BERR        [expr 0x1 << 28 ]
set AT91C_CAN_RBSY        [expr 0x1 << 29 ]
set AT91C_CAN_TBSY        [expr 0x1 << 30 ]
set AT91C_CAN_OVLY        [expr 0x1 << 31 ]
# -------- CAN_BR : (CAN Offset: 0x14) CAN Baudrate Register -------- 
set AT91C_CAN_PHASE2      [expr 0x7 <<  0 ]
set AT91C_CAN_PHASE1      [expr 0x7 <<  4 ]
set AT91C_CAN_PROPAG      [expr 0x7 <<  8 ]
set AT91C_CAN_SYNC        [expr 0x3 << 12 ]
set AT91C_CAN_BRP         [expr 0x7F << 16 ]
set AT91C_CAN_SMP         [expr 0x1 << 24 ]
# -------- CAN_TIM : (CAN Offset: 0x18) CAN Timer Register -------- 
set AT91C_CAN_TIMER       [expr 0xFFFF <<  0 ]
# -------- CAN_TIMESTP : (CAN Offset: 0x1c) CAN Timestamp Register -------- 
set AT91C_CAN_MTIMESTAMP  [expr 0xFFFF <<  0 ]
# -------- CAN_ECR : (CAN Offset: 0x20) CAN Error Counter Register -------- 
set AT91C_CAN_REC         [expr 0xFF <<  0 ]
set AT91C_CAN_TEC         [expr 0xFF << 16 ]
# -------- CAN_TCR : (CAN Offset: 0x24) CAN Transfer Command Register -------- 
set AT91C_CAN_MB0         [expr 0x1 <<  0 ]
set AT91C_CAN_MB1         [expr 0x1 <<  1 ]
set AT91C_CAN_MB2         [expr 0x1 <<  2 ]
set AT91C_CAN_MB3         [expr 0x1 <<  3 ]
set AT91C_CAN_MB4         [expr 0x1 <<  4 ]
set AT91C_CAN_MB5         [expr 0x1 <<  5 ]
set AT91C_CAN_MB6         [expr 0x1 <<  6 ]
set AT91C_CAN_MB7         [expr 0x1 <<  7 ]
set AT91C_CAN_MB8         [expr 0x1 <<  8 ]
set AT91C_CAN_MB9         [expr 0x1 <<  9 ]
set AT91C_CAN_MB10        [expr 0x1 << 10 ]
set AT91C_CAN_MB11        [expr 0x1 << 11 ]
set AT91C_CAN_MB12        [expr 0x1 << 12 ]
set AT91C_CAN_MB13        [expr 0x1 << 13 ]
set AT91C_CAN_MB14        [expr 0x1 << 14 ]
set AT91C_CAN_MB15        [expr 0x1 << 15 ]
set AT91C_CAN_TIMRST      [expr 0x1 << 31 ]
# -------- CAN_ACR : (CAN Offset: 0x28) CAN Abort Command Register -------- 
set AT91C_CAN_MB0         [expr 0x1 <<  0 ]
set AT91C_CAN_MB1         [expr 0x1 <<  1 ]
set AT91C_CAN_MB2         [expr 0x1 <<  2 ]
set AT91C_CAN_MB3         [expr 0x1 <<  3 ]
set AT91C_CAN_MB4         [expr 0x1 <<  4 ]
set AT91C_CAN_MB5         [expr 0x1 <<  5 ]
set AT91C_CAN_MB6         [expr 0x1 <<  6 ]
set AT91C_CAN_MB7         [expr 0x1 <<  7 ]
set AT91C_CAN_MB8         [expr 0x1 <<  8 ]
set AT91C_CAN_MB9         [expr 0x1 <<  9 ]
set AT91C_CAN_MB10        [expr 0x1 << 10 ]
set AT91C_CAN_MB11        [expr 0x1 << 11 ]
set AT91C_CAN_MB12        [expr 0x1 << 12 ]
set AT91C_CAN_MB13        [expr 0x1 << 13 ]
set AT91C_CAN_MB14        [expr 0x1 << 14 ]
set AT91C_CAN_MB15        [expr 0x1 << 15 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Ethernet MAC 10/100
# *****************************************************************************
# -------- EMAC_NCR : (EMAC Offset: 0x0)  -------- 
set AT91C_EMAC_LB         [expr 0x1 <<  0 ]
set AT91C_EMAC_LLB        [expr 0x1 <<  1 ]
set AT91C_EMAC_RE         [expr 0x1 <<  2 ]
set AT91C_EMAC_TE         [expr 0x1 <<  3 ]
set AT91C_EMAC_MPE        [expr 0x1 <<  4 ]
set AT91C_EMAC_CLRSTAT    [expr 0x1 <<  5 ]
set AT91C_EMAC_INCSTAT    [expr 0x1 <<  6 ]
set AT91C_EMAC_WESTAT     [expr 0x1 <<  7 ]
set AT91C_EMAC_BP         [expr 0x1 <<  8 ]
set AT91C_EMAC_TSTART     [expr 0x1 <<  9 ]
set AT91C_EMAC_THALT      [expr 0x1 << 10 ]
set AT91C_EMAC_TPFR       [expr 0x1 << 11 ]
set AT91C_EMAC_TZQ        [expr 0x1 << 12 ]
# -------- EMAC_NCFGR : (EMAC Offset: 0x4) Network Configuration Register -------- 
set AT91C_EMAC_SPD        [expr 0x1 <<  0 ]
set AT91C_EMAC_FD         [expr 0x1 <<  1 ]
set AT91C_EMAC_JFRAME     [expr 0x1 <<  3 ]
set AT91C_EMAC_CAF        [expr 0x1 <<  4 ]
set AT91C_EMAC_NBC        [expr 0x1 <<  5 ]
set AT91C_EMAC_MTI        [expr 0x1 <<  6 ]
set AT91C_EMAC_UNI        [expr 0x1 <<  7 ]
set AT91C_EMAC_BIG        [expr 0x1 <<  8 ]
set AT91C_EMAC_EAE        [expr 0x1 <<  9 ]
set AT91C_EMAC_CLK        [expr 0x3 << 10 ]
set 	AT91C_EMAC_CLK_HCLK_8               [expr 0x0 << 10 ]
set 	AT91C_EMAC_CLK_HCLK_16              [expr 0x1 << 10 ]
set 	AT91C_EMAC_CLK_HCLK_32              [expr 0x2 << 10 ]
set 	AT91C_EMAC_CLK_HCLK_64              [expr 0x3 << 10 ]
set AT91C_EMAC_RTY        [expr 0x1 << 12 ]
set AT91C_EMAC_PAE        [expr 0x1 << 13 ]
set AT91C_EMAC_RBOF       [expr 0x3 << 14 ]
set 	AT91C_EMAC_RBOF_OFFSET_0             [expr 0x0 << 14 ]
set 	AT91C_EMAC_RBOF_OFFSET_1             [expr 0x1 << 14 ]
set 	AT91C_EMAC_RBOF_OFFSET_2             [expr 0x2 << 14 ]
set 	AT91C_EMAC_RBOF_OFFSET_3             [expr 0x3 << 14 ]
set AT91C_EMAC_RLCE       [expr 0x1 << 16 ]
set AT91C_EMAC_DRFCS      [expr 0x1 << 17 ]
set AT91C_EMAC_EFRHD      [expr 0x1 << 18 ]
set AT91C_EMAC_IRXFCS     [expr 0x1 << 19 ]
# -------- EMAC_NSR : (EMAC Offset: 0x8) Network Status Register -------- 
set AT91C_EMAC_LINKR      [expr 0x1 <<  0 ]
set AT91C_EMAC_MDIO       [expr 0x1 <<  1 ]
set AT91C_EMAC_IDLE       [expr 0x1 <<  2 ]
# -------- EMAC_TSR : (EMAC Offset: 0x14) Transmit Status Register -------- 
set AT91C_EMAC_UBR        [expr 0x1 <<  0 ]
set AT91C_EMAC_COL        [expr 0x1 <<  1 ]
set AT91C_EMAC_RLES       [expr 0x1 <<  2 ]
set AT91C_EMAC_TGO        [expr 0x1 <<  3 ]
set AT91C_EMAC_BEX        [expr 0x1 <<  4 ]
set AT91C_EMAC_COMP       [expr 0x1 <<  5 ]
set AT91C_EMAC_UND        [expr 0x1 <<  6 ]
# -------- EMAC_RSR : (EMAC Offset: 0x20) Receive Status Register -------- 
set AT91C_EMAC_BNA        [expr 0x1 <<  0 ]
set AT91C_EMAC_REC        [expr 0x1 <<  1 ]
set AT91C_EMAC_OVR        [expr 0x1 <<  2 ]
# -------- EMAC_ISR : (EMAC Offset: 0x24) Interrupt Status Register -------- 
set AT91C_EMAC_MFD        [expr 0x1 <<  0 ]
set AT91C_EMAC_RCOMP      [expr 0x1 <<  1 ]
set AT91C_EMAC_RXUBR      [expr 0x1 <<  2 ]
set AT91C_EMAC_TXUBR      [expr 0x1 <<  3 ]
set AT91C_EMAC_TUNDR      [expr 0x1 <<  4 ]
set AT91C_EMAC_RLEX       [expr 0x1 <<  5 ]
set AT91C_EMAC_TXERR      [expr 0x1 <<  6 ]
set AT91C_EMAC_TCOMP      [expr 0x1 <<  7 ]
set AT91C_EMAC_LINK       [expr 0x1 <<  9 ]
set AT91C_EMAC_ROVR       [expr 0x1 << 10 ]
set AT91C_EMAC_HRESP      [expr 0x1 << 11 ]
set AT91C_EMAC_PFRE       [expr 0x1 << 12 ]
set AT91C_EMAC_PTZ        [expr 0x1 << 13 ]
# -------- EMAC_IER : (EMAC Offset: 0x28) Interrupt Enable Register -------- 
set AT91C_EMAC_MFD        [expr 0x1 <<  0 ]
set AT91C_EMAC_RCOMP      [expr 0x1 <<  1 ]
set AT91C_EMAC_RXUBR      [expr 0x1 <<  2 ]
set AT91C_EMAC_TXUBR      [expr 0x1 <<  3 ]
set AT91C_EMAC_TUNDR      [expr 0x1 <<  4 ]
set AT91C_EMAC_RLEX       [expr 0x1 <<  5 ]
set AT91C_EMAC_TXERR      [expr 0x1 <<  6 ]
set AT91C_EMAC_TCOMP      [expr 0x1 <<  7 ]
set AT91C_EMAC_LINK       [expr 0x1 <<  9 ]
set AT91C_EMAC_ROVR       [expr 0x1 << 10 ]
set AT91C_EMAC_HRESP      [expr 0x1 << 11 ]
set AT91C_EMAC_PFRE       [expr 0x1 << 12 ]
set AT91C_EMAC_PTZ        [expr 0x1 << 13 ]
# -------- EMAC_IDR : (EMAC Offset: 0x2c) Interrupt Disable Register -------- 
set AT91C_EMAC_MFD        [expr 0x1 <<  0 ]
set AT91C_EMAC_RCOMP      [expr 0x1 <<  1 ]
set AT91C_EMAC_RXUBR      [expr 0x1 <<  2 ]
set AT91C_EMAC_TXUBR      [expr 0x1 <<  3 ]
set AT91C_EMAC_TUNDR      [expr 0x1 <<  4 ]
set AT91C_EMAC_RLEX       [expr 0x1 <<  5 ]
set AT91C_EMAC_TXERR      [expr 0x1 <<  6 ]
set AT91C_EMAC_TCOMP      [expr 0x1 <<  7 ]
set AT91C_EMAC_LINK       [expr 0x1 <<  9 ]
set AT91C_EMAC_ROVR       [expr 0x1 << 10 ]
set AT91C_EMAC_HRESP      [expr 0x1 << 11 ]
set AT91C_EMAC_PFRE       [expr 0x1 << 12 ]
set AT91C_EMAC_PTZ        [expr 0x1 << 13 ]
# -------- EMAC_IMR : (EMAC Offset: 0x30) Interrupt Mask Register -------- 
set AT91C_EMAC_MFD        [expr 0x1 <<  0 ]
set AT91C_EMAC_RCOMP      [expr 0x1 <<  1 ]
set AT91C_EMAC_RXUBR      [expr 0x1 <<  2 ]
set AT91C_EMAC_TXUBR      [expr 0x1 <<  3 ]
set AT91C_EMAC_TUNDR      [expr 0x1 <<  4 ]
set AT91C_EMAC_RLEX       [expr 0x1 <<  5 ]
set AT91C_EMAC_TXERR      [expr 0x1 <<  6 ]
set AT91C_EMAC_TCOMP      [expr 0x1 <<  7 ]
set AT91C_EMAC_LINK       [expr 0x1 <<  9 ]
set AT91C_EMAC_ROVR       [expr 0x1 << 10 ]
set AT91C_EMAC_HRESP      [expr 0x1 << 11 ]
set AT91C_EMAC_PFRE       [expr 0x1 << 12 ]
set AT91C_EMAC_PTZ        [expr 0x1 << 13 ]
# -------- EMAC_MAN : (EMAC Offset: 0x34) PHY Maintenance Register -------- 
set AT91C_EMAC_DATA       [expr 0xFFFF <<  0 ]
set AT91C_EMAC_CODE       [expr 0x3 << 16 ]
set AT91C_EMAC_REGA       [expr 0x1F << 18 ]
set AT91C_EMAC_PHYA       [expr 0x1F << 23 ]
set AT91C_EMAC_RW         [expr 0x3 << 28 ]
set AT91C_EMAC_SOF        [expr 0x3 << 30 ]
# -------- EMAC_USRIO : (EMAC Offset: 0xc0) USER Input Output Register -------- 
set AT91C_EMAC_RMII       [expr 0x1 <<  0 ]
set AT91C_EMAC_CLKEN      [expr 0x1 <<  1 ]
# -------- EMAC_WOL : (EMAC Offset: 0xc4) Wake On LAN Register -------- 
set AT91C_EMAC_IP         [expr 0xFFFF <<  0 ]
set AT91C_EMAC_MAG        [expr 0x1 << 16 ]
set AT91C_EMAC_ARP        [expr 0x1 << 17 ]
set AT91C_EMAC_SA1        [expr 0x1 << 18 ]
set AT91C_EMAC_MTI        [expr 0x1 << 19 ]
# -------- EMAC_REV : (EMAC Offset: 0xfc) Revision Register -------- 
set AT91C_EMAC_REVREF     [expr 0xFFFF <<  0 ]
set AT91C_EMAC_PARTREF    [expr 0xFFFF << 16 ]

# *****************************************************************************
#              SOFTWARE API DEFINITION  FOR Analog to Digital Convertor
# *****************************************************************************
# -------- ADC_CR : (ADC Offset: 0x0) ADC Control Register -------- 
set AT91C_ADC_SWRST       [expr 0x1 <<  0 ]
set AT91C_ADC_START       [expr 0x1 <<  1 ]
# -------- ADC_MR : (ADC Offset: 0x4) ADC Mode Register -------- 
set AT91C_ADC_TRGEN       [expr 0x1 <<  0 ]
set 	AT91C_ADC_TRGEN_DIS                  0x0
set 	AT91C_ADC_TRGEN_EN                   0x1
set AT91C_ADC_TRGSEL      [expr 0x7 <<  1 ]
set 	AT91C_ADC_TRGSEL_TIOA0                [expr 0x0 <<  1 ]
set 	AT91C_ADC_TRGSEL_TIOA1                [expr 0x1 <<  1 ]
set 	AT91C_ADC_TRGSEL_TIOA2                [expr 0x2 <<  1 ]
set 	AT91C_ADC_TRGSEL_TIOA3                [expr 0x3 <<  1 ]
set 	AT91C_ADC_TRGSEL_TIOA4                [expr 0x4 <<  1 ]
set 	AT91C_ADC_TRGSEL_TIOA5                [expr 0x5 <<  1 ]
set 	AT91C_ADC_TRGSEL_EXT                  [expr 0x6 <<  1 ]
set AT91C_ADC_LOWRES      [expr 0x1 <<  4 ]
set 	AT91C_ADC_LOWRES_10_BIT               [expr 0x0 <<  4 ]
set 	AT91C_ADC_LOWRES_8_BIT                [expr 0x1 <<  4 ]
set AT91C_ADC_SLEEP       [expr 0x1 <<  5 ]
set 	AT91C_ADC_SLEEP_NORMAL_MODE          [expr 0x0 <<  5 ]
set 	AT91C_ADC_SLEEP_MODE                 [expr 0x1 <<  5 ]
set AT91C_ADC_PRESCAL     [expr 0x3F <<  8 ]
set AT91C_ADC_STARTUP     [expr 0x1F << 16 ]
set AT91C_ADC_SHTIM       [expr 0xF << 24 ]
# -------- 	ADC_CHER : (ADC Offset: 0x10) ADC Channel Enable Register -------- 
set AT91C_ADC_CH0         [expr 0x1 <<  0 ]
set AT91C_ADC_CH1         [expr 0x1 <<  1 ]
set AT91C_ADC_CH2         [expr 0x1 <<  2 ]
set AT91C_ADC_CH3         [expr 0x1 <<  3 ]
set AT91C_ADC_CH4         [expr 0x1 <<  4 ]
set AT91C_ADC_CH5         [expr 0x1 <<  5 ]
set AT91C_ADC_CH6         [expr 0x1 <<  6 ]
set AT91C_ADC_CH7         [expr 0x1 <<  7 ]
# -------- 	ADC_CHDR : (ADC Offset: 0x14) ADC Channel Disable Register -------- 
set AT91C_ADC_CH0         [expr 0x1 <<  0 ]
set AT91C_ADC_CH1         [expr 0x1 <<  1 ]
set AT91C_ADC_CH2         [expr 0x1 <<  2 ]
set AT91C_ADC_CH3         [expr 0x1 <<  3 ]
set AT91C_ADC_CH4         [expr 0x1 <<  4 ]
set AT91C_ADC_CH5         [expr 0x1 <<  5 ]
set AT91C_ADC_CH6         [expr 0x1 <<  6 ]
set AT91C_ADC_CH7         [expr 0x1 <<  7 ]
# -------- 	ADC_CHSR : (ADC Offset: 0x18) ADC Channel Status Register -------- 
set AT91C_ADC_CH0         [expr 0x1 <<  0 ]
set AT91C_ADC_CH1         [expr 0x1 <<  1 ]
set AT91C_ADC_CH2         [expr 0x1 <<  2 ]
set AT91C_ADC_CH3         [expr 0x1 <<  3 ]
set AT91C_ADC_CH4         [expr 0x1 <<  4 ]
set AT91C_ADC_CH5         [expr 0x1 <<  5 ]
set AT91C_ADC_CH6         [expr 0x1 <<  6 ]
set AT91C_ADC_CH7         [expr 0x1 <<  7 ]
# -------- ADC_SR : (ADC Offset: 0x1c) ADC Status Register -------- 
set AT91C_ADC_EOC0        [expr 0x1 <<  0 ]
set AT91C_ADC_EOC1        [expr 0x1 <<  1 ]
set AT91C_ADC_EOC2        [expr 0x1 <<  2 ]
set AT91C_ADC_EOC3        [expr 0x1 <<  3 ]
set AT91C_ADC_EOC4        [expr 0x1 <<  4 ]
set AT91C_ADC_EOC5        [expr 0x1 <<  5 ]
set AT91C_ADC_EOC6        [expr 0x1 <<  6 ]
set AT91C_ADC_EOC7        [expr 0x1 <<  7 ]
set AT91C_ADC_OVRE0       [expr 0x1 <<  8 ]
set AT91C_ADC_OVRE1       [expr 0x1 <<  9 ]
set AT91C_ADC_OVRE2       [expr 0x1 << 10 ]
set AT91C_ADC_OVRE3       [expr 0x1 << 11 ]
set AT91C_ADC_OVRE4       [expr 0x1 << 12 ]
set AT91C_ADC_OVRE5       [expr 0x1 << 13 ]
set AT91C_ADC_OVRE6       [expr 0x1 << 14 ]
set AT91C_ADC_OVRE7       [expr 0x1 << 15 ]
set AT91C_ADC_DRDY        [expr 0x1 << 16 ]
set AT91C_ADC_GOVRE       [expr 0x1 << 17 ]
set AT91C_ADC_ENDRX       [expr 0x1 << 18 ]
set AT91C_ADC_RXBUFF      [expr 0x1 << 19 ]
# -------- ADC_LCDR : (ADC Offset: 0x20) ADC Last Converted Data Register -------- 
set AT91C_ADC_LDATA       [expr 0x3FF <<  0 ]
# -------- ADC_IER : (ADC Offset: 0x24) ADC Interrupt Enable Register -------- 
set AT91C_ADC_EOC0        [expr 0x1 <<  0 ]
set AT91C_ADC_EOC1        [expr 0x1 <<  1 ]
set AT91C_ADC_EOC2        [expr 0x1 <<  2 ]
set AT91C_ADC_EOC3        [expr 0x1 <<  3 ]
set AT91C_ADC_EOC4        [expr 0x1 <<  4 ]
set AT91C_ADC_EOC5        [expr 0x1 <<  5 ]
set AT91C_ADC_EOC6        [expr 0x1 <<  6 ]
set AT91C_ADC_EOC7        [expr 0x1 <<  7 ]
set AT91C_ADC_OVRE0       [expr 0x1 <<  8 ]
set AT91C_ADC_OVRE1       [expr 0x1 <<  9 ]
set AT91C_ADC_OVRE2       [expr 0x1 << 10 ]
set AT91C_ADC_OVRE3       [expr 0x1 << 11 ]
set AT91C_ADC_OVRE4       [expr 0x1 << 12 ]
set AT91C_ADC_OVRE5       [expr 0x1 << 13 ]
set AT91C_ADC_OVRE6       [expr 0x1 << 14 ]
set AT91C_ADC_OVRE7       [expr 0x1 << 15 ]
set AT91C_ADC_DRDY        [expr 0x1 << 16 ]
set AT91C_ADC_GOVRE       [expr 0x1 << 17 ]
set AT91C_ADC_ENDRX       [expr 0x1 << 18 ]
set AT91C_ADC_RXBUFF      [expr 0x1 << 19 ]
# -------- ADC_IDR : (ADC Offset: 0x28) ADC Interrupt Disable Register -------- 
set AT91C_ADC_EOC0        [expr 0x1 <<  0 ]
set AT91C_ADC_EOC1        [expr 0x1 <<  1 ]
set AT91C_ADC_EOC2        [expr 0x1 <<  2 ]
set AT91C_ADC_EOC3        [expr 0x1 <<  3 ]
set AT91C_ADC_EOC4        [expr 0x1 <<  4 ]
set AT91C_ADC_EOC5        [expr 0x1 <<  5 ]
set AT91C_ADC_EOC6        [expr 0x1 <<  6 ]
set AT91C_ADC_EOC7        [expr 0x1 <<  7 ]
set AT91C_ADC_OVRE0       [expr 0x1 <<  8 ]
set AT91C_ADC_OVRE1       [expr 0x1 <<  9 ]
set AT91C_ADC_OVRE2       [expr 0x1 << 10 ]
set AT91C_ADC_OVRE3       [expr 0x1 << 11 ]
set AT91C_ADC_OVRE4       [expr 0x1 << 12 ]
set AT91C_ADC_OVRE5       [expr 0x1 << 13 ]
set AT91C_ADC_OVRE6       [expr 0x1 << 14 ]
set AT91C_ADC_OVRE7       [expr 0x1 << 15 ]
set AT91C_ADC_DRDY        [expr 0x1 << 16 ]
set AT91C_ADC_GOVRE       [expr 0x1 << 17 ]
set AT91C_ADC_ENDRX       [expr 0x1 << 18 ]
set AT91C_ADC_RXBUFF      [expr 0x1 << 19 ]
# -------- ADC_IMR : (ADC Offset: 0x2c) ADC Interrupt Mask Register -------- 
set AT91C_ADC_EOC0        [expr 0x1 <<  0 ]
set AT91C_ADC_EOC1        [expr 0x1 <<  1 ]
set AT91C_ADC_EOC2        [expr 0x1 <<  2 ]
set AT91C_ADC_EOC3        [expr 0x1 <<  3 ]
set AT91C_ADC_EOC4        [expr 0x1 <<  4 ]
set AT91C_ADC_EOC5        [expr 0x1 <<  5 ]
set AT91C_ADC_EOC6        [expr 0x1 <<  6 ]
set AT91C_ADC_EOC7        [expr 0x1 <<  7 ]
set AT91C_ADC_OVRE0       [expr 0x1 <<  8 ]
set AT91C_ADC_OVRE1       [expr 0x1 <<  9 ]
set AT91C_ADC_OVRE2       [expr 0x1 << 10 ]
set AT91C_ADC_OVRE3       [expr 0x1 << 11 ]
set AT91C_ADC_OVRE4       [expr 0x1 << 12 ]
set AT91C_ADC_OVRE5       [expr 0x1 << 13 ]
set AT91C_ADC_OVRE6       [expr 0x1 << 14 ]
set AT91C_ADC_OVRE7       [expr 0x1 << 15 ]
set AT91C_ADC_DRDY        [expr 0x1 << 16 ]
set AT91C_ADC_GOVRE       [expr 0x1 << 17 ]
set AT91C_ADC_ENDRX       [expr 0x1 << 18 ]
set AT91C_ADC_RXBUFF      [expr 0x1 << 19 ]
# -------- ADC_CDR0 : (ADC Offset: 0x30) ADC Channel Data Register 0 -------- 
set AT91C_ADC_DATA        [expr 0x3FF <<  0 ]
# -------- ADC_CDR1 : (ADC Offset: 0x34) ADC Channel Data Register 1 -------- 
set AT91C_ADC_DATA        [expr 0x3FF <<  0 ]
# -------- ADC_CDR2 : (ADC Offset: 0x38) ADC Channel Data Register 2 -------- 
set AT91C_ADC_DATA        [expr 0x3FF <<  0 ]
# -------- ADC_CDR3 : (ADC Offset: 0x3c) ADC Channel Data Register 3 -------- 
set AT91C_ADC_DATA        [expr 0x3FF <<  0 ]
# -------- ADC_CDR4 : (ADC Offset: 0x40) ADC Channel Data Register 4 -------- 
set AT91C_ADC_DATA        [expr 0x3FF <<  0 ]
# -------- ADC_CDR5 : (ADC Offset: 0x44) ADC Channel Data Register 5 -------- 
set AT91C_ADC_DATA        [expr 0x3FF <<  0 ]
# -------- ADC_CDR6 : (ADC Offset: 0x48) ADC Channel Data Register 6 -------- 
set AT91C_ADC_DATA        [expr 0x3FF <<  0 ]
# -------- ADC_CDR7 : (ADC Offset: 0x4c) ADC Channel Data Register 7 -------- 
set AT91C_ADC_DATA        [expr 0x3FF <<  0 ]

# *****************************************************************************
#               REGISTER ADDRESS DEFINITION FOR AT91SAM7X256
# *****************************************************************************
# ========== Register definition for SYS peripheral ========== 
# ========== Register definition for AIC peripheral ========== 
set AT91C_AIC_IVR   0xFFFFF100
set AT91C_AIC_SMR   0xFFFFF000
set AT91C_AIC_FVR   0xFFFFF104
set AT91C_AIC_DCR   0xFFFFF138
set AT91C_AIC_EOICR 0xFFFFF130
set AT91C_AIC_SVR   0xFFFFF080
set AT91C_AIC_FFSR  0xFFFFF148
set AT91C_AIC_ICCR  0xFFFFF128
set AT91C_AIC_ISR   0xFFFFF108
set AT91C_AIC_IMR   0xFFFFF110
set AT91C_AIC_IPR   0xFFFFF10C
set AT91C_AIC_FFER  0xFFFFF140
set AT91C_AIC_IECR  0xFFFFF120
set AT91C_AIC_ISCR  0xFFFFF12C
set AT91C_AIC_FFDR  0xFFFFF144
set AT91C_AIC_CISR  0xFFFFF114
set AT91C_AIC_IDCR  0xFFFFF124
set AT91C_AIC_SPU   0xFFFFF134
# ========== Register definition for PDC_DBGU peripheral ========== 
set AT91C_DBGU_TCR  0xFFFFF30C
set AT91C_DBGU_RNPR 0xFFFFF310
set AT91C_DBGU_TNPR 0xFFFFF318
set AT91C_DBGU_TPR  0xFFFFF308
set AT91C_DBGU_RPR  0xFFFFF300
set AT91C_DBGU_RCR  0xFFFFF304
set AT91C_DBGU_RNCR 0xFFFFF314
set AT91C_DBGU_PTCR 0xFFFFF320
set AT91C_DBGU_PTSR 0xFFFFF324
set AT91C_DBGU_TNCR 0xFFFFF31C
# ========== Register definition for DBGU peripheral ========== 
set AT91C_DBGU_EXID 0xFFFFF244
set AT91C_DBGU_BRGR 0xFFFFF220
set AT91C_DBGU_IDR  0xFFFFF20C
set AT91C_DBGU_CSR  0xFFFFF214
set AT91C_DBGU_CIDR 0xFFFFF240
set AT91C_DBGU_MR   0xFFFFF204
set AT91C_DBGU_IMR  0xFFFFF210
set AT91C_DBGU_CR   0xFFFFF200
set AT91C_DBGU_FNTR 0xFFFFF248
set AT91C_DBGU_THR  0xFFFFF21C
set AT91C_DBGU_RHR  0xFFFFF218
set AT91C_DBGU_IER  0xFFFFF208
# ========== Register definition for PIOA peripheral ========== 
set AT91C_PIOA_ODR  0xFFFFF414
set AT91C_PIOA_SODR 0xFFFFF430
set AT91C_PIOA_ISR  0xFFFFF44C
set AT91C_PIOA_ABSR 0xFFFFF478
set AT91C_PIOA_IER  0xFFFFF440
set AT91C_PIOA_PPUDR 0xFFFFF460
set AT91C_PIOA_IMR  0xFFFFF448
set AT91C_PIOA_PER  0xFFFFF400
set AT91C_PIOA_IFDR 0xFFFFF424
set AT91C_PIOA_OWDR 0xFFFFF4A4
set AT91C_PIOA_MDSR 0xFFFFF458
set AT91C_PIOA_IDR  0xFFFFF444
set AT91C_PIOA_ODSR 0xFFFFF438
set AT91C_PIOA_PPUSR 0xFFFFF468
set AT91C_PIOA_OWSR 0xFFFFF4A8
set AT91C_PIOA_BSR  0xFFFFF474
set AT91C_PIOA_OWER 0xFFFFF4A0
set AT91C_PIOA_IFER 0xFFFFF420
set AT91C_PIOA_PDSR 0xFFFFF43C
set AT91C_PIOA_PPUER 0xFFFFF464
set AT91C_PIOA_OSR  0xFFFFF418
set AT91C_PIOA_ASR  0xFFFFF470
set AT91C_PIOA_MDDR 0xFFFFF454
set AT91C_PIOA_CODR 0xFFFFF434
set AT91C_PIOA_MDER 0xFFFFF450
set AT91C_PIOA_PDR  0xFFFFF404
set AT91C_PIOA_IFSR 0xFFFFF428
set AT91C_PIOA_OER  0xFFFFF410
set AT91C_PIOA_PSR  0xFFFFF408
# ========== Register definition for PIOB peripheral ========== 
set AT91C_PIOB_OWDR 0xFFFFF6A4
set AT91C_PIOB_MDER 0xFFFFF650
set AT91C_PIOB_PPUSR 0xFFFFF668
set AT91C_PIOB_IMR  0xFFFFF648
set AT91C_PIOB_ASR  0xFFFFF670
set AT91C_PIOB_PPUDR 0xFFFFF660
set AT91C_PIOB_PSR  0xFFFFF608
set AT91C_PIOB_IER  0xFFFFF640
set AT91C_PIOB_CODR 0xFFFFF634
set AT91C_PIOB_OWER 0xFFFFF6A0
set AT91C_PIOB_ABSR 0xFFFFF678
set AT91C_PIOB_IFDR 0xFFFFF624
set AT91C_PIOB_PDSR 0xFFFFF63C
set AT91C_PIOB_IDR  0xFFFFF644
set AT91C_PIOB_OWSR 0xFFFFF6A8
set AT91C_PIOB_PDR  0xFFFFF604
set AT91C_PIOB_ODR  0xFFFFF614
set AT91C_PIOB_IFSR 0xFFFFF628
set AT91C_PIOB_PPUER 0xFFFFF664
set AT91C_PIOB_SODR 0xFFFFF630
set AT91C_PIOB_ISR  0xFFFFF64C
set AT91C_PIOB_ODSR 0xFFFFF638
set AT91C_PIOB_OSR  0xFFFFF618
set AT91C_PIOB_MDSR 0xFFFFF658
set AT91C_PIOB_IFER 0xFFFFF620
set AT91C_PIOB_BSR  0xFFFFF674
set AT91C_PIOB_MDDR 0xFFFFF654
set AT91C_PIOB_OER  0xFFFFF610
set AT91C_PIOB_PER  0xFFFFF600
# ========== Register definition for CKGR peripheral ========== 
set AT91C_CKGR_MOR  0xFFFFFC20
set AT91C_CKGR_PLLR 0xFFFFFC2C
set AT91C_CKGR_MCFR 0xFFFFFC24
# ========== Register definition for PMC peripheral ========== 
set AT91C_PMC_IDR   0xFFFFFC64
set AT91C_PMC_MOR   0xFFFFFC20
set AT91C_PMC_PLLR  0xFFFFFC2C
set AT91C_PMC_PCER  0xFFFFFC10
set AT91C_PMC_PCKR  0xFFFFFC40
set AT91C_PMC_MCKR  0xFFFFFC30
set AT91C_PMC_SCDR  0xFFFFFC04
set AT91C_PMC_PCDR  0xFFFFFC14
set AT91C_PMC_SCSR  0xFFFFFC08
set AT91C_PMC_PCSR  0xFFFFFC18
set AT91C_PMC_MCFR  0xFFFFFC24
set AT91C_PMC_SCER  0xFFFFFC00
set AT91C_PMC_IMR   0xFFFFFC6C
set AT91C_PMC_IER   0xFFFFFC60
set AT91C_PMC_SR    0xFFFFFC68
# ========== Register definition for RSTC peripheral ========== 
set AT91C_RSTC_RCR  0xFFFFFD00
set AT91C_RSTC_RMR  0xFFFFFD08
set AT91C_RSTC_RSR  0xFFFFFD04
# ========== Register definition for RTTC peripheral ========== 
set AT91C_RTTC_RTSR 0xFFFFFD2C
set AT91C_RTTC_RTMR 0xFFFFFD20
set AT91C_RTTC_RTVR 0xFFFFFD28
set AT91C_RTTC_RTAR 0xFFFFFD24
# ========== Register definition for PITC peripheral ========== 
set AT91C_PITC_PIVR 0xFFFFFD38
set AT91C_PITC_PISR 0xFFFFFD34
set AT91C_PITC_PIIR 0xFFFFFD3C
set AT91C_PITC_PIMR 0xFFFFFD30
# ========== Register definition for WDTC peripheral ========== 
set AT91C_WDTC_WDCR 0xFFFFFD40
set AT91C_WDTC_WDSR 0xFFFFFD48
set AT91C_WDTC_WDMR 0xFFFFFD44
# ========== Register definition for VREG peripheral ========== 
set AT91C_VREG_MR   0xFFFFFD60
# ========== Register definition for MC peripheral ========== 
set AT91C_MC_ASR    0xFFFFFF04
set AT91C_MC_RCR    0xFFFFFF00
set AT91C_MC_FCR    0xFFFFFF64
set AT91C_MC_AASR   0xFFFFFF08
set AT91C_MC_FSR    0xFFFFFF68
set AT91C_MC_FMR    0xFFFFFF60
# ========== Register definition for PDC_SPI1 peripheral ========== 
set AT91C_SPI1_PTCR 0xFFFE4120
set AT91C_SPI1_RPR  0xFFFE4100
set AT91C_SPI1_TNCR 0xFFFE411C
set AT91C_SPI1_TPR  0xFFFE4108
set AT91C_SPI1_TNPR 0xFFFE4118
set AT91C_SPI1_TCR  0xFFFE410C
set AT91C_SPI1_RCR  0xFFFE4104
set AT91C_SPI1_RNPR 0xFFFE4110
set AT91C_SPI1_RNCR 0xFFFE4114
set AT91C_SPI1_PTSR 0xFFFE4124
# ========== Register definition for SPI1 peripheral ========== 
set AT91C_SPI1_IMR  0xFFFE401C
set AT91C_SPI1_IER  0xFFFE4014
set AT91C_SPI1_MR   0xFFFE4004
set AT91C_SPI1_RDR  0xFFFE4008
set AT91C_SPI1_IDR  0xFFFE4018
set AT91C_SPI1_SR   0xFFFE4010
set AT91C_SPI1_TDR  0xFFFE400C
set AT91C_SPI1_CR   0xFFFE4000
set AT91C_SPI1_CSR  0xFFFE4030
# ========== Register definition for PDC_SPI0 peripheral ========== 
set AT91C_SPI0_PTCR 0xFFFE0120
set AT91C_SPI0_TPR  0xFFFE0108
set AT91C_SPI0_TCR  0xFFFE010C
set AT91C_SPI0_RCR  0xFFFE0104
set AT91C_SPI0_PTSR 0xFFFE0124
set AT91C_SPI0_RNPR 0xFFFE0110
set AT91C_SPI0_RPR  0xFFFE0100
set AT91C_SPI0_TNCR 0xFFFE011C
set AT91C_SPI0_RNCR 0xFFFE0114
set AT91C_SPI0_TNPR 0xFFFE0118
# ========== Register definition for SPI0 peripheral ========== 
set AT91C_SPI0_IER  0xFFFE0014
set AT91C_SPI0_SR   0xFFFE0010
set AT91C_SPI0_IDR  0xFFFE0018
set AT91C_SPI0_CR   0xFFFE0000
set AT91C_SPI0_MR   0xFFFE0004
set AT91C_SPI0_IMR  0xFFFE001C
set AT91C_SPI0_TDR  0xFFFE000C
set AT91C_SPI0_RDR  0xFFFE0008
set AT91C_SPI0_CSR  0xFFFE0030
# ========== Register definition for PDC_US1 peripheral ========== 
set AT91C_US1_RNCR  0xFFFC4114
set AT91C_US1_PTCR  0xFFFC4120
set AT91C_US1_TCR   0xFFFC410C
set AT91C_US1_PTSR  0xFFFC4124
set AT91C_US1_TNPR  0xFFFC4118
set AT91C_US1_RCR   0xFFFC4104
set AT91C_US1_RNPR  0xFFFC4110
set AT91C_US1_RPR   0xFFFC4100
set AT91C_US1_TNCR  0xFFFC411C
set AT91C_US1_TPR   0xFFFC4108
# ========== Register definition for US1 peripheral ========== 
set AT91C_US1_IF    0xFFFC404C
set AT91C_US1_NER   0xFFFC4044
set AT91C_US1_RTOR  0xFFFC4024
set AT91C_US1_CSR   0xFFFC4014
set AT91C_US1_IDR   0xFFFC400C
set AT91C_US1_IER   0xFFFC4008
set AT91C_US1_THR   0xFFFC401C
set AT91C_US1_TTGR  0xFFFC4028
set AT91C_US1_RHR   0xFFFC4018
set AT91C_US1_BRGR  0xFFFC4020
set AT91C_US1_IMR   0xFFFC4010
set AT91C_US1_FIDI  0xFFFC4040
set AT91C_US1_CR    0xFFFC4000
set AT91C_US1_MR    0xFFFC4004
# ========== Register definition for PDC_US0 peripheral ========== 
set AT91C_US0_TNPR  0xFFFC0118
set AT91C_US0_RNPR  0xFFFC0110
set AT91C_US0_TCR   0xFFFC010C
set AT91C_US0_PTCR  0xFFFC0120
set AT91C_US0_PTSR  0xFFFC0124
set AT91C_US0_TNCR  0xFFFC011C
set AT91C_US0_TPR   0xFFFC0108
set AT91C_US0_RCR   0xFFFC0104
set AT91C_US0_RPR   0xFFFC0100
set AT91C_US0_RNCR  0xFFFC0114
# ========== Register definition for US0 peripheral ========== 
set AT91C_US0_BRGR  0xFFFC0020
set AT91C_US0_NER   0xFFFC0044
set AT91C_US0_CR    0xFFFC0000
set AT91C_US0_IMR   0xFFFC0010
set AT91C_US0_FIDI  0xFFFC0040
set AT91C_US0_TTGR  0xFFFC0028
set AT91C_US0_MR    0xFFFC0004
set AT91C_US0_RTOR  0xFFFC0024
set AT91C_US0_CSR   0xFFFC0014
set AT91C_US0_RHR   0xFFFC0018
set AT91C_US0_IDR   0xFFFC000C
set AT91C_US0_THR   0xFFFC001C
set AT91C_US0_IF    0xFFFC004C
set AT91C_US0_IER   0xFFFC0008
# ========== Register definition for PDC_SSC peripheral ========== 
set AT91C_SSC_TNCR  0xFFFD411C
set AT91C_SSC_RPR   0xFFFD4100
set AT91C_SSC_RNCR  0xFFFD4114
set AT91C_SSC_TPR   0xFFFD4108
set AT91C_SSC_PTCR  0xFFFD4120
set AT91C_SSC_TCR   0xFFFD410C
set AT91C_SSC_RCR   0xFFFD4104
set AT91C_SSC_RNPR  0xFFFD4110
set AT91C_SSC_TNPR  0xFFFD4118
set AT91C_SSC_PTSR  0xFFFD4124
# ========== Register definition for SSC peripheral ========== 
set AT91C_SSC_RHR   0xFFFD4020
set AT91C_SSC_RSHR  0xFFFD4030
set AT91C_SSC_TFMR  0xFFFD401C
set AT91C_SSC_IDR   0xFFFD4048
set AT91C_SSC_THR   0xFFFD4024
set AT91C_SSC_RCMR  0xFFFD4010
set AT91C_SSC_IER   0xFFFD4044
set AT91C_SSC_TSHR  0xFFFD4034
set AT91C_SSC_SR    0xFFFD4040
set AT91C_SSC_CMR   0xFFFD4004
set AT91C_SSC_TCMR  0xFFFD4018
set AT91C_SSC_CR    0xFFFD4000
set AT91C_SSC_IMR   0xFFFD404C
set AT91C_SSC_RFMR  0xFFFD4014
# ========== Register definition for TWI peripheral ========== 
set AT91C_TWI_IER   0xFFFB8024
set AT91C_TWI_CR    0xFFFB8000
set AT91C_TWI_SR    0xFFFB8020
set AT91C_TWI_IMR   0xFFFB802C
set AT91C_TWI_THR   0xFFFB8034
set AT91C_TWI_IDR   0xFFFB8028
set AT91C_TWI_IADR  0xFFFB800C
set AT91C_TWI_MMR   0xFFFB8004
set AT91C_TWI_CWGR  0xFFFB8010
set AT91C_TWI_RHR   0xFFFB8030
# ========== Register definition for PWMC_CH3 peripheral ========== 
set AT91C_PWMC_CH3_CUPDR 0xFFFCC270
set AT91C_PWMC_CH3_Reserved 0xFFFCC274
set AT91C_PWMC_CH3_CPRDR 0xFFFCC268
set AT91C_PWMC_CH3_CDTYR 0xFFFCC264
set AT91C_PWMC_CH3_CCNTR 0xFFFCC26C
set AT91C_PWMC_CH3_CMR 0xFFFCC260
# ========== Register definition for PWMC_CH2 peripheral ========== 
set AT91C_PWMC_CH2_Reserved 0xFFFCC254
set AT91C_PWMC_CH2_CMR 0xFFFCC240
set AT91C_PWMC_CH2_CCNTR 0xFFFCC24C
set AT91C_PWMC_CH2_CPRDR 0xFFFCC248
set AT91C_PWMC_CH2_CUPDR 0xFFFCC250
set AT91C_PWMC_CH2_CDTYR 0xFFFCC244
# ========== Register definition for PWMC_CH1 peripheral ========== 
set AT91C_PWMC_CH1_Reserved 0xFFFCC234
set AT91C_PWMC_CH1_CUPDR 0xFFFCC230
set AT91C_PWMC_CH1_CPRDR 0xFFFCC228
set AT91C_PWMC_CH1_CCNTR 0xFFFCC22C
set AT91C_PWMC_CH1_CDTYR 0xFFFCC224
set AT91C_PWMC_CH1_CMR 0xFFFCC220
# ========== Register definition for PWMC_CH0 peripheral ========== 
set AT91C_PWMC_CH0_Reserved 0xFFFCC214
set AT91C_PWMC_CH0_CPRDR 0xFFFCC208
set AT91C_PWMC_CH0_CDTYR 0xFFFCC204
set AT91C_PWMC_CH0_CMR 0xFFFCC200
set AT91C_PWMC_CH0_CUPDR 0xFFFCC210
set AT91C_PWMC_CH0_CCNTR 0xFFFCC20C
# ========== Register definition for PWMC peripheral ========== 
set AT91C_PWMC_IDR  0xFFFCC014
set AT91C_PWMC_DIS  0xFFFCC008
set AT91C_PWMC_IER  0xFFFCC010
set AT91C_PWMC_VR   0xFFFCC0FC
set AT91C_PWMC_ISR  0xFFFCC01C
set AT91C_PWMC_SR   0xFFFCC00C
set AT91C_PWMC_IMR  0xFFFCC018
set AT91C_PWMC_MR   0xFFFCC000
set AT91C_PWMC_ENA  0xFFFCC004
# ========== Register definition for UDP peripheral ========== 
set AT91C_UDP_IMR   0xFFFB0018
set AT91C_UDP_FADDR 0xFFFB0008
set AT91C_UDP_NUM   0xFFFB0000
set AT91C_UDP_FDR   0xFFFB0050
set AT91C_UDP_ISR   0xFFFB001C
set AT91C_UDP_CSR   0xFFFB0030
set AT91C_UDP_IDR   0xFFFB0014
set AT91C_UDP_ICR   0xFFFB0020
set AT91C_UDP_RSTEP 0xFFFB0028
set AT91C_UDP_TXVC  0xFFFB0074
set AT91C_UDP_GLBSTATE 0xFFFB0004
set AT91C_UDP_IER   0xFFFB0010
# ========== Register definition for TC0 peripheral ========== 
set AT91C_TC0_SR    0xFFFA0020
set AT91C_TC0_RC    0xFFFA001C
set AT91C_TC0_RB    0xFFFA0018
set AT91C_TC0_CCR   0xFFFA0000
set AT91C_TC0_CMR   0xFFFA0004
set AT91C_TC0_IER   0xFFFA0024
set AT91C_TC0_RA    0xFFFA0014
set AT91C_TC0_IDR   0xFFFA0028
set AT91C_TC0_CV    0xFFFA0010
set AT91C_TC0_IMR   0xFFFA002C
# ========== Register definition for TC1 peripheral ========== 
set AT91C_TC1_RB    0xFFFA0058
set AT91C_TC1_CCR   0xFFFA0040
set AT91C_TC1_IER   0xFFFA0064
set AT91C_TC1_IDR   0xFFFA0068
set AT91C_TC1_SR    0xFFFA0060
set AT91C_TC1_CMR   0xFFFA0044
set AT91C_TC1_RA    0xFFFA0054
set AT91C_TC1_RC    0xFFFA005C
set AT91C_TC1_IMR   0xFFFA006C
set AT91C_TC1_CV    0xFFFA0050
# ========== Register definition for TC2 peripheral ========== 
set AT91C_TC2_CMR   0xFFFA0084
set AT91C_TC2_CCR   0xFFFA0080
set AT91C_TC2_CV    0xFFFA0090
set AT91C_TC2_RA    0xFFFA0094
set AT91C_TC2_RB    0xFFFA0098
set AT91C_TC2_IDR   0xFFFA00A8
set AT91C_TC2_IMR   0xFFFA00AC
set AT91C_TC2_RC    0xFFFA009C
set AT91C_TC2_IER   0xFFFA00A4
set AT91C_TC2_SR    0xFFFA00A0
# ========== Register definition for TCB peripheral ========== 
set AT91C_TCB_BMR   0xFFFA00C4
set AT91C_TCB_BCR   0xFFFA00C0
# ========== Register definition for CAN_MB0 peripheral ========== 
set AT91C_CAN_MB0_MDL 0xFFFD0214
set AT91C_CAN_MB0_MAM 0xFFFD0204
set AT91C_CAN_MB0_MCR 0xFFFD021C
set AT91C_CAN_MB0_MID 0xFFFD0208
set AT91C_CAN_MB0_MSR 0xFFFD0210
set AT91C_CAN_MB0_MFID 0xFFFD020C
set AT91C_CAN_MB0_MDH 0xFFFD0218
set AT91C_CAN_MB0_MMR 0xFFFD0200
# ========== Register definition for CAN_MB1 peripheral ========== 
set AT91C_CAN_MB1_MDL 0xFFFD0234
set AT91C_CAN_MB1_MID 0xFFFD0228
set AT91C_CAN_MB1_MMR 0xFFFD0220
set AT91C_CAN_MB1_MSR 0xFFFD0230
set AT91C_CAN_MB1_MAM 0xFFFD0224
set AT91C_CAN_MB1_MDH 0xFFFD0238
set AT91C_CAN_MB1_MCR 0xFFFD023C
set AT91C_CAN_MB1_MFID 0xFFFD022C
# ========== Register definition for CAN_MB2 peripheral ========== 
set AT91C_CAN_MB2_MCR 0xFFFD025C
set AT91C_CAN_MB2_MDH 0xFFFD0258
set AT91C_CAN_MB2_MID 0xFFFD0248
set AT91C_CAN_MB2_MDL 0xFFFD0254
set AT91C_CAN_MB2_MMR 0xFFFD0240
set AT91C_CAN_MB2_MAM 0xFFFD0244
set AT91C_CAN_MB2_MFID 0xFFFD024C
set AT91C_CAN_MB2_MSR 0xFFFD0250
# ========== Register definition for CAN_MB3 peripheral ========== 
set AT91C_CAN_MB3_MFID 0xFFFD026C
set AT91C_CAN_MB3_MAM 0xFFFD0264
set AT91C_CAN_MB3_MID 0xFFFD0268
set AT91C_CAN_MB3_MCR 0xFFFD027C
set AT91C_CAN_MB3_MMR 0xFFFD0260
set AT91C_CAN_MB3_MSR 0xFFFD0270
set AT91C_CAN_MB3_MDL 0xFFFD0274
set AT91C_CAN_MB3_MDH 0xFFFD0278
# ========== Register definition for CAN_MB4 peripheral ========== 
set AT91C_CAN_MB4_MID 0xFFFD0288
set AT91C_CAN_MB4_MMR 0xFFFD0280
set AT91C_CAN_MB4_MDH 0xFFFD0298
set AT91C_CAN_MB4_MFID 0xFFFD028C
set AT91C_CAN_MB4_MSR 0xFFFD0290
set AT91C_CAN_MB4_MCR 0xFFFD029C
set AT91C_CAN_MB4_MDL 0xFFFD0294
set AT91C_CAN_MB4_MAM 0xFFFD0284
# ========== Register definition for CAN_MB5 peripheral ========== 
set AT91C_CAN_MB5_MSR 0xFFFD02B0
set AT91C_CAN_MB5_MCR 0xFFFD02BC
set AT91C_CAN_MB5_MFID 0xFFFD02AC
set AT91C_CAN_MB5_MDH 0xFFFD02B8
set AT91C_CAN_MB5_MID 0xFFFD02A8
set AT91C_CAN_MB5_MMR 0xFFFD02A0
set AT91C_CAN_MB5_MDL 0xFFFD02B4
set AT91C_CAN_MB5_MAM 0xFFFD02A4
# ========== Register definition for CAN_MB6 peripheral ========== 
set AT91C_CAN_MB6_MFID 0xFFFD02CC
set AT91C_CAN_MB6_MID 0xFFFD02C8
set AT91C_CAN_MB6_MAM 0xFFFD02C4
set AT91C_CAN_MB6_MSR 0xFFFD02D0
set AT91C_CAN_MB6_MDL 0xFFFD02D4
set AT91C_CAN_MB6_MCR 0xFFFD02DC
set AT91C_CAN_MB6_MDH 0xFFFD02D8
set AT91C_CAN_MB6_MMR 0xFFFD02C0
# ========== Register definition for CAN_MB7 peripheral ========== 
set AT91C_CAN_MB7_MCR 0xFFFD02FC
set AT91C_CAN_MB7_MDH 0xFFFD02F8
set AT91C_CAN_MB7_MFID 0xFFFD02EC
set AT91C_CAN_MB7_MDL 0xFFFD02F4
set AT91C_CAN_MB7_MID 0xFFFD02E8
set AT91C_CAN_MB7_MMR 0xFFFD02E0
set AT91C_CAN_MB7_MAM 0xFFFD02E4
set AT91C_CAN_MB7_MSR 0xFFFD02F0
# ========== Register definition for CAN peripheral ========== 
set AT91C_CAN_TCR   0xFFFD0024
set AT91C_CAN_IMR   0xFFFD000C
set AT91C_CAN_IER   0xFFFD0004
set AT91C_CAN_ECR   0xFFFD0020
set AT91C_CAN_TIMESTP 0xFFFD001C
set AT91C_CAN_MR    0xFFFD0000
set AT91C_CAN_IDR   0xFFFD0008
set AT91C_CAN_ACR   0xFFFD0028
set AT91C_CAN_TIM   0xFFFD0018
set AT91C_CAN_SR    0xFFFD0010
set AT91C_CAN_BR    0xFFFD0014
set AT91C_CAN_VR    0xFFFD00FC
# ========== Register definition for EMAC peripheral ========== 
set AT91C_EMAC_ISR  0xFFFDC024
set AT91C_EMAC_SA4H 0xFFFDC0B4
set AT91C_EMAC_SA1L 0xFFFDC098
set AT91C_EMAC_ELE  0xFFFDC078
set AT91C_EMAC_LCOL 0xFFFDC05C
set AT91C_EMAC_RLE  0xFFFDC088
set AT91C_EMAC_WOL  0xFFFDC0C4
set AT91C_EMAC_DTF  0xFFFDC058
set AT91C_EMAC_TUND 0xFFFDC064
set AT91C_EMAC_NCR  0xFFFDC000
set AT91C_EMAC_SA4L 0xFFFDC0B0
set AT91C_EMAC_RSR  0xFFFDC020
set AT91C_EMAC_SA3L 0xFFFDC0A8
set AT91C_EMAC_TSR  0xFFFDC014
set AT91C_EMAC_IDR  0xFFFDC02C
set AT91C_EMAC_RSE  0xFFFDC074
set AT91C_EMAC_ECOL 0xFFFDC060
set AT91C_EMAC_TID  0xFFFDC0B8
set AT91C_EMAC_HRB  0xFFFDC090
set AT91C_EMAC_TBQP 0xFFFDC01C
set AT91C_EMAC_USRIO 0xFFFDC0C0
set AT91C_EMAC_PTR  0xFFFDC038
set AT91C_EMAC_SA2H 0xFFFDC0A4
set AT91C_EMAC_ROV  0xFFFDC070
set AT91C_EMAC_ALE  0xFFFDC054
set AT91C_EMAC_RJA  0xFFFDC07C
set AT91C_EMAC_RBQP 0xFFFDC018
set AT91C_EMAC_TPF  0xFFFDC08C
set AT91C_EMAC_NCFGR 0xFFFDC004
set AT91C_EMAC_HRT  0xFFFDC094
set AT91C_EMAC_USF  0xFFFDC080
set AT91C_EMAC_FCSE 0xFFFDC050
set AT91C_EMAC_TPQ  0xFFFDC0BC
set AT91C_EMAC_MAN  0xFFFDC034
set AT91C_EMAC_FTO  0xFFFDC040
set AT91C_EMAC_REV  0xFFFDC0FC
set AT91C_EMAC_IMR  0xFFFDC030
set AT91C_EMAC_SCF  0xFFFDC044
set AT91C_EMAC_PFR  0xFFFDC03C
set AT91C_EMAC_MCF  0xFFFDC048
set AT91C_EMAC_NSR  0xFFFDC008
set AT91C_EMAC_SA2L 0xFFFDC0A0
set AT91C_EMAC_FRO  0xFFFDC04C
set AT91C_EMAC_IER  0xFFFDC028
set AT91C_EMAC_SA1H 0xFFFDC09C
set AT91C_EMAC_CSE  0xFFFDC068
set AT91C_EMAC_SA3H 0xFFFDC0AC
set AT91C_EMAC_RRE  0xFFFDC06C
set AT91C_EMAC_STE  0xFFFDC084
# ========== Register definition for PDC_ADC peripheral ========== 
set AT91C_ADC_PTSR  0xFFFD8124
set AT91C_ADC_PTCR  0xFFFD8120
set AT91C_ADC_TNPR  0xFFFD8118
set AT91C_ADC_TNCR  0xFFFD811C
set AT91C_ADC_RNPR  0xFFFD8110
set AT91C_ADC_RNCR  0xFFFD8114
set AT91C_ADC_RPR   0xFFFD8100
set AT91C_ADC_TCR   0xFFFD810C
set AT91C_ADC_TPR   0xFFFD8108
set AT91C_ADC_RCR   0xFFFD8104
# ========== Register definition for ADC peripheral ========== 
set AT91C_ADC_CDR2  0xFFFD8038
set AT91C_ADC_CDR3  0xFFFD803C
set AT91C_ADC_CDR0  0xFFFD8030
set AT91C_ADC_CDR5  0xFFFD8044
set AT91C_ADC_CHDR  0xFFFD8014
set AT91C_ADC_SR    0xFFFD801C
set AT91C_ADC_CDR4  0xFFFD8040
set AT91C_ADC_CDR1  0xFFFD8034
set AT91C_ADC_LCDR  0xFFFD8020
set AT91C_ADC_IDR   0xFFFD8028
set AT91C_ADC_CR    0xFFFD8000
set AT91C_ADC_CDR7  0xFFFD804C
set AT91C_ADC_CDR6  0xFFFD8048
set AT91C_ADC_IER   0xFFFD8024
set AT91C_ADC_CHER  0xFFFD8010
set AT91C_ADC_CHSR  0xFFFD8018
set AT91C_ADC_MR    0xFFFD8004
set AT91C_ADC_IMR   0xFFFD802C

# *****************************************************************************
#               BASE ADDRESS DEFINITIONS FOR AT91SAM7X256
# *****************************************************************************
set AT91C_BASE_SYS       0xFFFFF000
set AT91C_BASE_AIC       0xFFFFF000
set AT91C_BASE_PDC_DBGU  0xFFFFF300
set AT91C_BASE_DBGU      0xFFFFF200
set AT91C_BASE_PIOA      0xFFFFF400
set AT91C_BASE_PIOB      0xFFFFF600
set AT91C_BASE_CKGR      0xFFFFFC20
set AT91C_BASE_PMC       0xFFFFFC00
set AT91C_BASE_RSTC      0xFFFFFD00
set AT91C_BASE_RTTC      0xFFFFFD20
set AT91C_BASE_PITC      0xFFFFFD30
set AT91C_BASE_WDTC      0xFFFFFD40
set AT91C_BASE_VREG      0xFFFFFD60
set AT91C_BASE_MC        0xFFFFFF00
set AT91C_BASE_PDC_SPI1  0xFFFE4100
set AT91C_BASE_SPI1      0xFFFE4000
set AT91C_BASE_PDC_SPI0  0xFFFE0100
set AT91C_BASE_SPI0      0xFFFE0000
set AT91C_BASE_PDC_US1   0xFFFC4100
set AT91C_BASE_US1       0xFFFC4000
set AT91C_BASE_PDC_US0   0xFFFC0100
set AT91C_BASE_US0       0xFFFC0000
set AT91C_BASE_PDC_SSC   0xFFFD4100
set AT91C_BASE_SSC       0xFFFD4000
set AT91C_BASE_TWI       0xFFFB8000
set AT91C_BASE_PWMC_CH3  0xFFFCC260
set AT91C_BASE_PWMC_CH2  0xFFFCC240
set AT91C_BASE_PWMC_CH1  0xFFFCC220
set AT91C_BASE_PWMC_CH0  0xFFFCC200
set AT91C_BASE_PWMC      0xFFFCC000
set AT91C_BASE_UDP       0xFFFB0000
set AT91C_BASE_TC0       0xFFFA0000
set AT91C_BASE_TC1       0xFFFA0040
set AT91C_BASE_TC2       0xFFFA0080
set AT91C_BASE_TCB       0xFFFA0000
set AT91C_BASE_CAN_MB0   0xFFFD0200
set AT91C_BASE_CAN_MB1   0xFFFD0220
set AT91C_BASE_CAN_MB2   0xFFFD0240
set AT91C_BASE_CAN_MB3   0xFFFD0260
set AT91C_BASE_CAN_MB4   0xFFFD0280
set AT91C_BASE_CAN_MB5   0xFFFD02A0
set AT91C_BASE_CAN_MB6   0xFFFD02C0
set AT91C_BASE_CAN_MB7   0xFFFD02E0
set AT91C_BASE_CAN       0xFFFD0000
set AT91C_BASE_EMAC      0xFFFDC000
set AT91C_BASE_PDC_ADC   0xFFFD8100
set AT91C_BASE_ADC       0xFFFD8000

# *****************************************************************************
#               PERIPHERAL ID DEFINITIONS FOR AT91SAM7X256
# *****************************************************************************
set AT91C_ID_FIQ     0
set AT91C_ID_SYS     1
set AT91C_ID_PIOA    2
set AT91C_ID_PIOB    3
set AT91C_ID_SPI0    4
set AT91C_ID_SPI1    5
set AT91C_ID_US0     6
set AT91C_ID_US1     7
set AT91C_ID_SSC     8
set AT91C_ID_TWI     9
set AT91C_ID_PWMC   10
set AT91C_ID_UDP    11
set AT91C_ID_TC0    12
set AT91C_ID_TC1    13
set AT91C_ID_TC2    14
set AT91C_ID_CAN    15
set AT91C_ID_EMAC   16
set AT91C_ID_ADC    17
set AT91C_ID_18_Reserved 18
set AT91C_ID_19_Reserved 19
set AT91C_ID_20_Reserved 20
set AT91C_ID_21_Reserved 21
set AT91C_ID_22_Reserved 22
set AT91C_ID_23_Reserved 23
set AT91C_ID_24_Reserved 24
set AT91C_ID_25_Reserved 25
set AT91C_ID_26_Reserved 26
set AT91C_ID_27_Reserved 27
set AT91C_ID_28_Reserved 28
set AT91C_ID_29_Reserved 29
set AT91C_ID_IRQ0   30
set AT91C_ID_IRQ1   31

# *****************************************************************************
#               PIO DEFINITIONS FOR AT91SAM7X256
# *****************************************************************************
set AT91C_PIO_PA0        [expr 1 <<  0 ]
set AT91C_PA0_RXD0     $AT91C_PIO_PA0
set AT91C_PIO_PA1        [expr 1 <<  1 ]
set AT91C_PA1_TXD0     $AT91C_PIO_PA1
set AT91C_PIO_PA10       [expr 1 << 10 ]
set AT91C_PA10_TWD      $AT91C_PIO_PA10
set AT91C_PIO_PA11       [expr 1 << 11 ]
set AT91C_PA11_TWCK     $AT91C_PIO_PA11
set AT91C_PIO_PA12       [expr 1 << 12 ]
set AT91C_PA12_SPI0_NPCS0 $AT91C_PIO_PA12
set AT91C_PIO_PA13       [expr 1 << 13 ]
set AT91C_PA13_SPI0_NPCS1 $AT91C_PIO_PA13
set AT91C_PA13_PCK1     $AT91C_PIO_PA13
set AT91C_PIO_PA14       [expr 1 << 14 ]
set AT91C_PA14_SPI0_NPCS2 $AT91C_PIO_PA14
set AT91C_PA14_IRQ1     $AT91C_PIO_PA14
set AT91C_PIO_PA15       [expr 1 << 15 ]
set AT91C_PA15_SPI0_NPCS3 $AT91C_PIO_PA15
set AT91C_PA15_TCLK2    $AT91C_PIO_PA15
set AT91C_PIO_PA16       [expr 1 << 16 ]
set AT91C_PA16_SPI0_MISO $AT91C_PIO_PA16
set AT91C_PIO_PA17       [expr 1 << 17 ]
set AT91C_PA17_SPI0_MOSI $AT91C_PIO_PA17
set AT91C_PIO_PA18       [expr 1 << 18 ]
set AT91C_PA18_SPI0_SPCK $AT91C_PIO_PA18
set AT91C_PIO_PA19       [expr 1 << 19 ]
set AT91C_PA19_CANRX    $AT91C_PIO_PA19
set AT91C_PIO_PA2        [expr 1 <<  2 ]
set AT91C_PA2_SCK0     $AT91C_PIO_PA2
set AT91C_PA2_SPI1_NPCS1 $AT91C_PIO_PA2
set AT91C_PIO_PA20       [expr 1 << 20 ]
set AT91C_PA20_CANTX    $AT91C_PIO_PA20
set AT91C_PIO_PA21       [expr 1 << 21 ]
set AT91C_PA21_TF       $AT91C_PIO_PA21
set AT91C_PA21_SPI1_NPCS0 $AT91C_PIO_PA21
set AT91C_PIO_PA22       [expr 1 << 22 ]
set AT91C_PA22_TK       $AT91C_PIO_PA22
set AT91C_PA22_SPI1_SPCK $AT91C_PIO_PA22
set AT91C_PIO_PA23       [expr 1 << 23 ]
set AT91C_PA23_TD       $AT91C_PIO_PA23
set AT91C_PA23_SPI1_MOSI $AT91C_PIO_PA23
set AT91C_PIO_PA24       [expr 1 << 24 ]
set AT91C_PA24_RD       $AT91C_PIO_PA24
set AT91C_PA24_SPI1_MISO $AT91C_PIO_PA24
set AT91C_PIO_PA25       [expr 1 << 25 ]
set AT91C_PA25_RK       $AT91C_PIO_PA25
set AT91C_PA25_SPI1_NPCS1 $AT91C_PIO_PA25
set AT91C_PIO_PA26       [expr 1 << 26 ]
set AT91C_PA26_RF       $AT91C_PIO_PA26
set AT91C_PA26_SPI1_NPCS2 $AT91C_PIO_PA26
set AT91C_PIO_PA27       [expr 1 << 27 ]
set AT91C_PA27_DRXD     $AT91C_PIO_PA27
set AT91C_PA27_PCK3     $AT91C_PIO_PA27
set AT91C_PIO_PA28       [expr 1 << 28 ]
set AT91C_PA28_DTXD     $AT91C_PIO_PA28
set AT91C_PIO_PA29       [expr 1 << 29 ]
set AT91C_PA29_FIQ      $AT91C_PIO_PA29
set AT91C_PA29_SPI1_NPCS3 $AT91C_PIO_PA29
set AT91C_PIO_PA3        [expr 1 <<  3 ]
set AT91C_PA3_RTS0     $AT91C_PIO_PA3
set AT91C_PA3_SPI1_NPCS2 $AT91C_PIO_PA3
set AT91C_PIO_PA30       [expr 1 << 30 ]
set AT91C_PA30_IRQ0     $AT91C_PIO_PA30
set AT91C_PA30_PCK2     $AT91C_PIO_PA30
set AT91C_PIO_PA4        [expr 1 <<  4 ]
set AT91C_PA4_CTS0     $AT91C_PIO_PA4
set AT91C_PA4_SPI1_NPCS3 $AT91C_PIO_PA4
set AT91C_PIO_PA5        [expr 1 <<  5 ]
set AT91C_PA5_RXD1     $AT91C_PIO_PA5
set AT91C_PIO_PA6        [expr 1 <<  6 ]
set AT91C_PA6_TXD1     $AT91C_PIO_PA6
set AT91C_PIO_PA7        [expr 1 <<  7 ]
set AT91C_PA7_SCK1     $AT91C_PIO_PA7
set AT91C_PA7_SPI0_NPCS1 $AT91C_PIO_PA7
set AT91C_PIO_PA8        [expr 1 <<  8 ]
set AT91C_PA8_RTS1     $AT91C_PIO_PA8
set AT91C_PA8_SPI0_NPCS2 $AT91C_PIO_PA8
set AT91C_PIO_PA9        [expr 1 <<  9 ]
set AT91C_PA9_CTS1     $AT91C_PIO_PA9
set AT91C_PA9_SPI0_NPCS3 $AT91C_PIO_PA9
set AT91C_PIO_PB0        [expr 1 <<  0 ]
set AT91C_PB0_ETXCK_EREFCK $AT91C_PIO_PB0
set AT91C_PB0_PCK0     $AT91C_PIO_PB0
set AT91C_PIO_PB1        [expr 1 <<  1 ]
set AT91C_PB1_ETXEN    $AT91C_PIO_PB1
set AT91C_PIO_PB10       [expr 1 << 10 ]
set AT91C_PB10_ETX2     $AT91C_PIO_PB10
set AT91C_PB10_SPI1_NPCS1 $AT91C_PIO_PB10
set AT91C_PIO_PB11       [expr 1 << 11 ]
set AT91C_PB11_ETX3     $AT91C_PIO_PB11
set AT91C_PB11_SPI1_NPCS2 $AT91C_PIO_PB11
set AT91C_PIO_PB12       [expr 1 << 12 ]
set AT91C_PB12_ETXER    $AT91C_PIO_PB12
set AT91C_PB12_TCLK0    $AT91C_PIO_PB12
set AT91C_PIO_PB13       [expr 1 << 13 ]
set AT91C_PB13_ERX2     $AT91C_PIO_PB13
set AT91C_PB13_SPI0_NPCS1 $AT91C_PIO_PB13
set AT91C_PIO_PB14       [expr 1 << 14 ]
set AT91C_PB14_ERX3     $AT91C_PIO_PB14
set AT91C_PB14_SPI0_NPCS2 $AT91C_PIO_PB14
set AT91C_PIO_PB15       [expr 1 << 15 ]
set AT91C_PB15_ERXDV_ECRSDV $AT91C_PIO_PB15
set AT91C_PIO_PB16       [expr 1 << 16 ]
set AT91C_PB16_ECOL     $AT91C_PIO_PB16
set AT91C_PB16_SPI1_NPCS3 $AT91C_PIO_PB16
set AT91C_PIO_PB17       [expr 1 << 17 ]
set AT91C_PB17_ERXCK    $AT91C_PIO_PB17
set AT91C_PB17_SPI0_NPCS3 $AT91C_PIO_PB17
set AT91C_PIO_PB18       [expr 1 << 18 ]
set AT91C_PB18_EF100    $AT91C_PIO_PB18
set AT91C_PB18_ADTRG    $AT91C_PIO_PB18
set AT91C_PIO_PB19       [expr 1 << 19 ]
set AT91C_PB19_PWM0     $AT91C_PIO_PB19
set AT91C_PB19_TCLK1    $AT91C_PIO_PB19
set AT91C_PIO_PB2        [expr 1 <<  2 ]
set AT91C_PB2_ETX0     $AT91C_PIO_PB2
set AT91C_PIO_PB20       [expr 1 << 20 ]
set AT91C_PB20_PWM1     $AT91C_PIO_PB20
set AT91C_PB20_PCK0     $AT91C_PIO_PB20
set AT91C_PIO_PB21       [expr 1 << 21 ]
set AT91C_PB21_PWM2     $AT91C_PIO_PB21
set AT91C_PB21_PCK1     $AT91C_PIO_PB21
set AT91C_PIO_PB22       [expr 1 << 22 ]
set AT91C_PB22_PWM3     $AT91C_PIO_PB22
set AT91C_PB22_PCK2     $AT91C_PIO_PB22
set AT91C_PIO_PB23       [expr 1 << 23 ]
set AT91C_PB23_TIOA0    $AT91C_PIO_PB23
set AT91C_PB23_DCD1     $AT91C_PIO_PB23
set AT91C_PIO_PB24       [expr 1 << 24 ]
set AT91C_PB24_TIOB0    $AT91C_PIO_PB24
set AT91C_PB24_DSR1     $AT91C_PIO_PB24
set AT91C_PIO_PB25       [expr 1 << 25 ]
set AT91C_PB25_TIOA1    $AT91C_PIO_PB25
set AT91C_PB25_DTR1     $AT91C_PIO_PB25
set AT91C_PIO_PB26       [expr 1 << 26 ]
set AT91C_PB26_TIOB1    $AT91C_PIO_PB26
set AT91C_PB26_RI1      $AT91C_PIO_PB26
set AT91C_PIO_PB27       [expr 1 << 27 ]
set AT91C_PB27_TIOA2    $AT91C_PIO_PB27
set AT91C_PB27_PWM0     $AT91C_PIO_PB27
set AT91C_PIO_PB28       [expr 1 << 28 ]
set AT91C_PB28_TIOB2    $AT91C_PIO_PB28
set AT91C_PB28_PWM1     $AT91C_PIO_PB28
set AT91C_PIO_PB29       [expr 1 << 29 ]
set AT91C_PB29_PCK1     $AT91C_PIO_PB29
set AT91C_PB29_PWM2     $AT91C_PIO_PB29
set AT91C_PIO_PB3        [expr 1 <<  3 ]
set AT91C_PB3_ETX1     $AT91C_PIO_PB3
set AT91C_PIO_PB30       [expr 1 << 30 ]
set AT91C_PB30_PCK2     $AT91C_PIO_PB30
set AT91C_PB30_PWM3     $AT91C_PIO_PB30
set AT91C_PIO_PB4        [expr 1 <<  4 ]
set AT91C_PB4_ECRS     $AT91C_PIO_PB4
set AT91C_PIO_PB5        [expr 1 <<  5 ]
set AT91C_PB5_ERX0     $AT91C_PIO_PB5
set AT91C_PIO_PB6        [expr 1 <<  6 ]
set AT91C_PB6_ERX1     $AT91C_PIO_PB6
set AT91C_PIO_PB7        [expr 1 <<  7 ]
set AT91C_PB7_ERXER    $AT91C_PIO_PB7
set AT91C_PIO_PB8        [expr 1 <<  8 ]
set AT91C_PB8_EMDC     $AT91C_PIO_PB8
set AT91C_PIO_PB9        [expr 1 <<  9 ]
set AT91C_PB9_EMDIO    $AT91C_PIO_PB9

# *****************************************************************************
#               MEMORY MAPPING DEFINITIONS FOR AT91SAM7X256
# *****************************************************************************
set AT91C_ISRAM	 0x00200000
set AT91C_ISRAM_SIZE	 0x00010000
set AT91C_IFLASH	 0x00100000
set AT91C_IFLASH_SIZE	 0x00040000


# *****************************************************************************
#               ATTRIBUTES DEFINITIONS FOR AT91SAM7X256
# *****************************************************************************
array set AT91SAM7X256_att {
	DBGU 	{ LP 	DBGU_att }
	PMC 	{ LP 	PMC_att }
	VREG 	{ LP 	VREG_att }
	RSTC 	{ LP 	RSTC_att }
	SSC 	{ LP 	SSC_att }
	WDTC 	{ LP 	WDTC_att }
	USART 	{ LP 	US1_att 	US0_att }
	SPI 	{ LP 	SPI1_att 	SPI0_att }
	PITC 	{ LP 	PITC_att }
	TCB 	{ LP 	TCB_att }
	CKGR 	{ LP 	CKGR_att }
	AIC 	{ LP 	AIC_att }
	TWI 	{ LP 	TWI_att }
	ADC 	{ LP 	ADC_att }
	PWMC_CH 	{ LP 	PWMC_CH3_att 	PWMC_CH2_att 	PWMC_CH1_att 	PWMC_CH0_att }
	RTTC 	{ LP 	RTTC_att }
	UDP 	{ LP 	UDP_att }
	EMAC 	{ LP 	EMAC_att }
	CAN_MB 	{ LP 	CAN_MB0_att 	CAN_MB1_att 	CAN_MB2_att 	CAN_MB3_att 	CAN_MB4_att 	CAN_MB5_att 	CAN_MB6_att 	CAN_MB7_att }
	TC 	{ LP 	TC0_att 	TC1_att 	TC2_att }
	SYS 	{ LP 	SYS_att }
	MC 	{ LP 	MC_att }
	PIO 	{ LP 	PIOA_att 	PIOB_att }
	CAN 	{ LP 	CAN_att }
	PWMC 	{ LP 	PWMC_att }
	PDC 	{ LP 	PDC_DBGU_att 	PDC_SPI1_att 	PDC_SPI0_att 	PDC_US1_att 	PDC_US0_att 	PDC_SSC_att 	PDC_ADC_att }

}
# ========== Peripheral attributes for DBGU peripheral ========== 
array set DBGU_att {
	EXID 	{ R AT91C_DBGU_EXID 	RO }
	BRGR 	{ R AT91C_DBGU_BRGR 	RW }
	IDR 	{ R AT91C_DBGU_IDR 	WO }
	CSR 	{ R AT91C_DBGU_CSR 	RO }
	CIDR 	{ R AT91C_DBGU_CIDR 	RO }
	MR 	{ R AT91C_DBGU_MR 	RW }
	IMR 	{ R AT91C_DBGU_IMR 	RO }
	CR 	{ R AT91C_DBGU_CR 	WO }
	FNTR 	{ R AT91C_DBGU_FNTR 	RW }
	THR 	{ R AT91C_DBGU_THR 	WO }
	RHR 	{ R AT91C_DBGU_RHR 	RO }
	IER 	{ R AT91C_DBGU_IER 	WO }
	listeReg 	{ EXID BRGR IDR CSR CIDR MR IMR CR FNTR THR RHR IER  }

}

# ========== Peripheral attributes for PMC peripheral ========== 
array set PMC_att {
	IDR 	{ R AT91C_PMC_IDR 	WO }
	MOR 	{ R AT91C_PMC_MOR 	RW }
	PLLR 	{ R AT91C_PMC_PLLR 	RW }
	PCER 	{ R AT91C_PMC_PCER 	WO }
	PCKR 	{ R AT91C_PMC_PCKR 	RW }
	MCKR 	{ R AT91C_PMC_MCKR 	RW }
	SCDR 	{ R AT91C_PMC_SCDR 	WO }
	PCDR 	{ R AT91C_PMC_PCDR 	WO }
	SCSR 	{ R AT91C_PMC_SCSR 	RO }
	PCSR 	{ R AT91C_PMC_PCSR 	RO }
	MCFR 	{ R AT91C_PMC_MCFR 	RO }
	SCER 	{ R AT91C_PMC_SCER 	WO }
	IMR 	{ R AT91C_PMC_IMR 	RO }
	IER 	{ R AT91C_PMC_IER 	WO }
	SR 	{ R AT91C_PMC_SR 	RO }
	listeReg 	{ IDR MOR PLLR PCER PCKR MCKR SCDR PCDR SCSR PCSR MCFR SCER IMR IER SR  }

}

# ========== Peripheral attributes for VREG peripheral ========== 
array set VREG_att {
	MR 	{ R AT91C_VREG_MR 	RW }
	listeReg 	{ MR  }

}

# ========== Peripheral attributes for RSTC peripheral ========== 
array set RSTC_att {
	RCR 	{ R AT91C_RSTC_RCR 	WO }
	RMR 	{ R AT91C_RSTC_RMR 	RW }
	RSR 	{ R AT91C_RSTC_RSR 	RO }
	listeReg 	{ RCR RMR RSR  }

}

# ========== Peripheral attributes for SSC peripheral ========== 
array set SSC_att {
	RHR 	{ R AT91C_SSC_RHR 	RO }
	RSHR 	{ R AT91C_SSC_RSHR 	RO }
	TFMR 	{ R AT91C_SSC_TFMR 	RW }
	IDR 	{ R AT91C_SSC_IDR 	WO }
	THR 	{ R AT91C_SSC_THR 	WO }
	RCMR 	{ R AT91C_SSC_RCMR 	RW }
	IER 	{ R AT91C_SSC_IER 	WO }
	TSHR 	{ R AT91C_SSC_TSHR 	RW }
	SR 	{ R AT91C_SSC_SR 	RO }
	CMR 	{ R AT91C_SSC_CMR 	RW }
	TCMR 	{ R AT91C_SSC_TCMR 	RW }
	CR 	{ R AT91C_SSC_CR 	WO }
	IMR 	{ R AT91C_SSC_IMR 	RO }
	RFMR 	{ R AT91C_SSC_RFMR 	RW }
	listeReg 	{ RHR RSHR TFMR IDR THR RCMR IER TSHR SR CMR TCMR CR IMR RFMR  }

}

# ========== Peripheral attributes for WDTC peripheral ========== 
array set WDTC_att {
	WDCR 	{ R AT91C_WDTC_WDCR 	WO }
	WDSR 	{ R AT91C_WDTC_WDSR 	RO }
	WDMR 	{ R AT91C_WDTC_WDMR 	RW }
	listeReg 	{ WDCR WDSR WDMR  }

}

# ========== Peripheral attributes for USART peripheral ========== 
array set US1_att {
	IF 	{ R AT91C_US1_IF 	RW }
	NER 	{ R AT91C_US1_NER 	RO }
	RTOR 	{ R AT91C_US1_RTOR 	RW }
	CSR 	{ R AT91C_US1_CSR 	RO }
	IDR 	{ R AT91C_US1_IDR 	WO }
	IER 	{ R AT91C_US1_IER 	WO }
	THR 	{ R AT91C_US1_THR 	WO }
	TTGR 	{ R AT91C_US1_TTGR 	RW }
	RHR 	{ R AT91C_US1_RHR 	RO }
	BRGR 	{ R AT91C_US1_BRGR 	RW }
	IMR 	{ R AT91C_US1_IMR 	RO }
	FIDI 	{ R AT91C_US1_FIDI 	RW }
	CR 	{ R AT91C_US1_CR 	WO }
	MR 	{ R AT91C_US1_MR 	RW }
	listeReg 	{ IF NER RTOR CSR IDR IER THR TTGR RHR BRGR IMR FIDI CR MR  }

}
array set US0_att {
	BRGR 	{ R AT91C_US0_BRGR 	RW }
	NER 	{ R AT91C_US0_NER 	RO }
	CR 	{ R AT91C_US0_CR 	WO }
	IMR 	{ R AT91C_US0_IMR 	RO }
	FIDI 	{ R AT91C_US0_FIDI 	RW }
	TTGR 	{ R AT91C_US0_TTGR 	RW }
	MR 	{ R AT91C_US0_MR 	RW }
	RTOR 	{ R AT91C_US0_RTOR 	RW }
	CSR 	{ R AT91C_US0_CSR 	RO }
	RHR 	{ R AT91C_US0_RHR 	RO }
	IDR 	{ R AT91C_US0_IDR 	WO }
	THR 	{ R AT91C_US0_THR 	WO }
	IF 	{ R AT91C_US0_IF 	RW }
	IER 	{ R AT91C_US0_IER 	WO }
	listeReg 	{ BRGR NER CR IMR FIDI TTGR MR RTOR CSR RHR IDR THR IF IER  }

}

# ========== Peripheral attributes for SPI peripheral ========== 
array set SPI1_att {
	IMR 	{ R AT91C_SPI1_IMR 	RO }
	IER 	{ R AT91C_SPI1_IER 	WO }
	MR 	{ R AT91C_SPI1_MR 	RW }
	RDR 	{ R AT91C_SPI1_RDR 	RO }
	IDR 	{ R AT91C_SPI1_IDR 	WO }
	SR 	{ R AT91C_SPI1_SR 	RO }
	TDR 	{ R AT91C_SPI1_TDR 	WO }
	CR 	{ R AT91C_SPI1_CR 	RO }
	CSR 	{ R AT91C_SPI1_CSR 	RW }
	listeReg 	{ IMR IER MR RDR IDR SR TDR CR CSR  }

}
array set SPI0_att {
	IER 	{ R AT91C_SPI0_IER 	WO }
	SR 	{ R AT91C_SPI0_SR 	RO }
	IDR 	{ R AT91C_SPI0_IDR 	WO }
	CR 	{ R AT91C_SPI0_CR 	RO }
	MR 	{ R AT91C_SPI0_MR 	RW }
	IMR 	{ R AT91C_SPI0_IMR 	RO }
	TDR 	{ R AT91C_SPI0_TDR 	WO }
	RDR 	{ R AT91C_SPI0_RDR 	RO }
	CSR 	{ R AT91C_SPI0_CSR 	RW }
	listeReg 	{ IER SR IDR CR MR IMR TDR RDR CSR  }

}

# ========== Peripheral attributes for PITC peripheral ========== 
array set PITC_att {
	PIVR 	{ R AT91C_PITC_PIVR 	RO }
	PISR 	{ R AT91C_PITC_PISR 	RO }
	PIIR 	{ R AT91C_PITC_PIIR 	RO }
	PIMR 	{ R AT91C_PITC_PIMR 	RW }
	listeReg 	{ PIVR PISR PIIR PIMR  }

}

# ========== Peripheral attributes for TCB peripheral ========== 
array set TCB_att {
	BMR 	{ R AT91C_TCB_BMR 	RW }
	BCR 	{ R AT91C_TCB_BCR 	WO }
	listeReg 	{ BMR BCR  }

}

# ========== Peripheral attributes for CKGR peripheral ========== 
array set CKGR_att {
	MOR 	{ R AT91C_CKGR_MOR 	RW }
	PLLR 	{ R AT91C_CKGR_PLLR 	RW }
	MCFR 	{ R AT91C_CKGR_MCFR 	RO }
	listeReg 	{ MOR PLLR MCFR  }

}

# ========== Peripheral attributes for AIC peripheral ========== 
array set AIC_att {
	IVR 	{ R AT91C_AIC_IVR 	RO }
	SMR 	{ R AT91C_AIC_SMR 	RW }
	FVR 	{ R AT91C_AIC_FVR 	RO }
	DCR 	{ R AT91C_AIC_DCR 	RW }
	EOICR 	{ R AT91C_AIC_EOICR 	WO }
	SVR 	{ R AT91C_AIC_SVR 	RW }
	FFSR 	{ R AT91C_AIC_FFSR 	RO }
	ICCR 	{ R AT91C_AIC_ICCR 	WO }
	ISR 	{ R AT91C_AIC_ISR 	RO }
	IMR 	{ R AT91C_AIC_IMR 	RO }
	IPR 	{ R AT91C_AIC_IPR 	RO }
	FFER 	{ R AT91C_AIC_FFER 	WO }
	IECR 	{ R AT91C_AIC_IECR 	WO }
	ISCR 	{ R AT91C_AIC_ISCR 	WO }
	FFDR 	{ R AT91C_AIC_FFDR 	WO }
	CISR 	{ R AT91C_AIC_CISR 	RO }
	IDCR 	{ R AT91C_AIC_IDCR 	WO }
	SPU 	{ R AT91C_AIC_SPU 	RW }
	listeReg 	{ IVR SMR FVR DCR EOICR SVR FFSR ICCR ISR IMR IPR FFER IECR ISCR FFDR CISR IDCR SPU  }

}

# ========== Peripheral attributes for TWI peripheral ========== 
array set TWI_att {
	IER 	{ R AT91C_TWI_IER 	WO }
	CR 	{ R AT91C_TWI_CR 	WO }
	SR 	{ R AT91C_TWI_SR 	RO }
	IMR 	{ R AT91C_TWI_IMR 	RO }
	THR 	{ R AT91C_TWI_THR 	WO }
	IDR 	{ R AT91C_TWI_IDR 	WO }
	IADR 	{ R AT91C_TWI_IADR 	RW }
	MMR 	{ R AT91C_TWI_MMR 	RW }
	CWGR 	{ R AT91C_TWI_CWGR 	RW }
	RHR 	{ R AT91C_TWI_RHR 	RO }
	listeReg 	{ IER CR SR IMR THR IDR IADR MMR CWGR RHR  }

}

# ========== Peripheral attributes for ADC peripheral ========== 
array set ADC_att {
	CDR2 	{ R AT91C_ADC_CDR2 	RO }
	CDR3 	{ R AT91C_ADC_CDR3 	RO }
	CDR0 	{ R AT91C_ADC_CDR0 	RO }
	CDR5 	{ R AT91C_ADC_CDR5 	RO }
	CHDR 	{ R AT91C_ADC_CHDR 	WO }
	SR 	{ R AT91C_ADC_SR 	RO }
	CDR4 	{ R AT91C_ADC_CDR4 	RO }
	CDR1 	{ R AT91C_ADC_CDR1 	RO }
	LCDR 	{ R AT91C_ADC_LCDR 	RO }
	IDR 	{ R AT91C_ADC_IDR 	WO }
	CR 	{ R AT91C_ADC_CR 	WO }
	CDR7 	{ R AT91C_ADC_CDR7 	RO }
	CDR6 	{ R AT91C_ADC_CDR6 	RO }
	IER 	{ R AT91C_ADC_IER 	WO }
	CHER 	{ R AT91C_ADC_CHER 	WO }
	CHSR 	{ R AT91C_ADC_CHSR 	RO }
	MR 	{ R AT91C_ADC_MR 	RW }
	IMR 	{ R AT91C_ADC_IMR 	RO }
	listeReg 	{ CDR2 CDR3 CDR0 CDR5 CHDR SR CDR4 CDR1 LCDR IDR CR CDR7 CDR6 IER CHER CHSR MR IMR  }

}

# ========== Peripheral attributes for PWMC_CH peripheral ========== 
array set PWMC_CH3_att {
	CUPDR 	{ R AT91C_PWMC_CH3_CUPDR 	WO }
	Reserved 	{ R AT91C_PWMC_CH3_Reserved 	WO }
	CPRDR 	{ R AT91C_PWMC_CH3_CPRDR 	RW }
	CDTYR 	{ R AT91C_PWMC_CH3_CDTYR 	RW }
	CCNTR 	{ R AT91C_PWMC_CH3_CCNTR 	RO }
	CMR 	{ R AT91C_PWMC_CH3_CMR 	RW }
	listeReg 	{ CUPDR Reserved CPRDR CDTYR CCNTR CMR  }

}
array set PWMC_CH2_att {
	Reserved 	{ R AT91C_PWMC_CH2_Reserved 	WO }
	CMR 	{ R AT91C_PWMC_CH2_CMR 	RW }
	CCNTR 	{ R AT91C_PWMC_CH2_CCNTR 	RO }
	CPRDR 	{ R AT91C_PWMC_CH2_CPRDR 	RW }
	CUPDR 	{ R AT91C_PWMC_CH2_CUPDR 	WO }
	CDTYR 	{ R AT91C_PWMC_CH2_CDTYR 	RW }
	listeReg 	{ Reserved CMR CCNTR CPRDR CUPDR CDTYR  }

}
array set PWMC_CH1_att {
	Reserved 	{ R AT91C_PWMC_CH1_Reserved 	WO }
	CUPDR 	{ R AT91C_PWMC_CH1_CUPDR 	WO }
	CPRDR 	{ R AT91C_PWMC_CH1_CPRDR 	RW }
	CCNTR 	{ R AT91C_PWMC_CH1_CCNTR 	RO }
	CDTYR 	{ R AT91C_PWMC_CH1_CDTYR 	RW }
	CMR 	{ R AT91C_PWMC_CH1_CMR 	RW }
	listeReg 	{ Reserved CUPDR CPRDR CCNTR CDTYR CMR  }

}
array set PWMC_CH0_att {
	Reserved 	{ R AT91C_PWMC_CH0_Reserved 	WO }
	CPRDR 	{ R AT91C_PWMC_CH0_CPRDR 	RW }
	CDTYR 	{ R AT91C_PWMC_CH0_CDTYR 	RW }
	CMR 	{ R AT91C_PWMC_CH0_CMR 	RW }
	CUPDR 	{ R AT91C_PWMC_CH0_CUPDR 	WO }
	CCNTR 	{ R AT91C_PWMC_CH0_CCNTR 	RO }
	listeReg 	{ Reserved CPRDR CDTYR CMR CUPDR CCNTR  }

}

# ========== Peripheral attributes for RTTC peripheral ========== 
array set RTTC_att {
	RTSR 	{ R AT91C_RTTC_RTSR 	RO }
	RTMR 	{ R AT91C_RTTC_RTMR 	RW }
	RTVR 	{ R AT91C_RTTC_RTVR 	RO }
	RTAR 	{ R AT91C_RTTC_RTAR 	RW }
	listeReg 	{ RTSR RTMR RTVR RTAR  }

}

# ========== Peripheral attributes for UDP peripheral ========== 
array set UDP_att {
	IMR 	{ R AT91C_UDP_IMR 	RO }
	FADDR 	{ R AT91C_UDP_FADDR 	RW }
	NUM 	{ R AT91C_UDP_NUM 	RO }
	FDR 	{ R AT91C_UDP_FDR 	RW }
	ISR 	{ R AT91C_UDP_ISR 	RO }
	CSR 	{ R AT91C_UDP_CSR 	RW }
	IDR 	{ R AT91C_UDP_IDR 	WO }
	ICR 	{ R AT91C_UDP_ICR 	RO }
	RSTEP 	{ R AT91C_UDP_RSTEP 	RO }
	TXVC 	{ R AT91C_UDP_TXVC 	RW }
	GLBSTATE 	{ R AT91C_UDP_GLBSTATE 	RW }
	IER 	{ R AT91C_UDP_IER 	WO }
	listeReg 	{ IMR FADDR NUM FDR ISR CSR IDR ICR RSTEP TXVC GLBSTATE IER  }

}

# ========== Peripheral attributes for EMAC peripheral ========== 
array set EMAC_att {
	ISR 	{ R AT91C_EMAC_ISR 	RW }
	SA4H 	{ R AT91C_EMAC_SA4H 	RW }
	SA1L 	{ R AT91C_EMAC_SA1L 	RW }
	ELE 	{ R AT91C_EMAC_ELE 	RW }
	LCOL 	{ R AT91C_EMAC_LCOL 	RW }
	RLE 	{ R AT91C_EMAC_RLE 	RW }
	WOL 	{ R AT91C_EMAC_WOL 	RW }
	DTF 	{ R AT91C_EMAC_DTF 	RW }
	TUND 	{ R AT91C_EMAC_TUND 	RW }
	NCR 	{ R AT91C_EMAC_NCR 	RW }
	SA4L 	{ R AT91C_EMAC_SA4L 	RW }
	RSR 	{ R AT91C_EMAC_RSR 	RW }
	SA3L 	{ R AT91C_EMAC_SA3L 	RW }
	TSR 	{ R AT91C_EMAC_TSR 	RW }
	IDR 	{ R AT91C_EMAC_IDR 	WO }
	RSE 	{ R AT91C_EMAC_RSE 	RW }
	ECOL 	{ R AT91C_EMAC_ECOL 	RW }
	TID 	{ R AT91C_EMAC_TID 	RW }
	HRB 	{ R AT91C_EMAC_HRB 	RW }
	TBQP 	{ R AT91C_EMAC_TBQP 	RW }
	USRIO 	{ R AT91C_EMAC_USRIO 	RW }
	PTR 	{ R AT91C_EMAC_PTR 	RW }
	SA2H 	{ R AT91C_EMAC_SA2H 	RW }
	ROV 	{ R AT91C_EMAC_ROV 	RW }
	ALE 	{ R AT91C_EMAC_ALE 	RW }
	RJA 	{ R AT91C_EMAC_RJA 	RW }
	RBQP 	{ R AT91C_EMAC_RBQP 	RW }
	TPF 	{ R AT91C_EMAC_TPF 	RW }
	NCFGR 	{ R AT91C_EMAC_NCFGR 	RW }
	HRT 	{ R AT91C_EMAC_HRT 	RW }
	USF 	{ R AT91C_EMAC_USF 	RW }
	FCSE 	{ R AT91C_EMAC_FCSE 	RW }
	TPQ 	{ R AT91C_EMAC_TPQ 	RW }
	MAN 	{ R AT91C_EMAC_MAN 	RW }
	FTO 	{ R AT91C_EMAC_FTO 	RW }
	REV 	{ R AT91C_EMAC_REV 	RO }
	IMR 	{ R AT91C_EMAC_IMR 	RO }
	SCF 	{ R AT91C_EMAC_SCF 	RW }
	PFR 	{ R AT91C_EMAC_PFR 	RW }
	MCF 	{ R AT91C_EMAC_MCF 	RW }
	NSR 	{ R AT91C_EMAC_NSR 	RO }
	SA2L 	{ R AT91C_EMAC_SA2L 	RW }
	FRO 	{ R AT91C_EMAC_FRO 	RW }
	IER 	{ R AT91C_EMAC_IER 	WO }
	SA1H 	{ R AT91C_EMAC_SA1H 	RW }
	CSE 	{ R AT91C_EMAC_CSE 	RW }
	SA3H 	{ R AT91C_EMAC_SA3H 	RW }
	RRE 	{ R AT91C_EMAC_RRE 	RW }
	STE 	{ R AT91C_EMAC_STE 	RW }
	listeReg 	{ ISR SA4H SA1L ELE LCOL RLE WOL DTF TUND NCR SA4L RSR SA3L TSR IDR RSE ECOL TID HRB TBQP USRIO PTR SA2H ROV ALE RJA RBQP TPF NCFGR HRT USF FCSE TPQ MAN FTO REV IMR SCF PFR MCF NSR SA2L FRO IER SA1H CSE SA3H RRE STE  }

}

# ========== Peripheral attributes for CAN_MB peripheral ========== 
array set CAN_MB0_att {
	MDL 	{ R AT91C_CAN_MB0_MDL 	RW }
	MAM 	{ R AT91C_CAN_MB0_MAM 	RW }
	MCR 	{ R AT91C_CAN_MB0_MCR 	WO }
	MID 	{ R AT91C_CAN_MB0_MID 	RW }
	MSR 	{ R AT91C_CAN_MB0_MSR 	RO }
	MFID 	{ R AT91C_CAN_MB0_MFID 	RO }
	MDH 	{ R AT91C_CAN_MB0_MDH 	RW }
	MMR 	{ R AT91C_CAN_MB0_MMR 	RW }
	listeReg 	{ MDL MAM MCR MID MSR MFID MDH MMR  }

}
array set CAN_MB1_att {
	MDL 	{ R AT91C_CAN_MB1_MDL 	RW }
	MID 	{ R AT91C_CAN_MB1_MID 	RW }
	MMR 	{ R AT91C_CAN_MB1_MMR 	RW }
	MSR 	{ R AT91C_CAN_MB1_MSR 	RO }
	MAM 	{ R AT91C_CAN_MB1_MAM 	RW }
	MDH 	{ R AT91C_CAN_MB1_MDH 	RW }
	MCR 	{ R AT91C_CAN_MB1_MCR 	WO }
	MFID 	{ R AT91C_CAN_MB1_MFID 	RO }
	listeReg 	{ MDL MID MMR MSR MAM MDH MCR MFID  }

}
array set CAN_MB2_att {
	MCR 	{ R AT91C_CAN_MB2_MCR 	WO }
	MDH 	{ R AT91C_CAN_MB2_MDH 	RW }
	MID 	{ R AT91C_CAN_MB2_MID 	RW }
	MDL 	{ R AT91C_CAN_MB2_MDL 	RW }
	MMR 	{ R AT91C_CAN_MB2_MMR 	RW }
	MAM 	{ R AT91C_CAN_MB2_MAM 	RW }
	MFID 	{ R AT91C_CAN_MB2_MFID 	RO }
	MSR 	{ R AT91C_CAN_MB2_MSR 	RO }
	listeReg 	{ MCR MDH MID MDL MMR MAM MFID MSR  }

}
array set CAN_MB3_att {
	MFID 	{ R AT91C_CAN_MB3_MFID 	RO }
	MAM 	{ R AT91C_CAN_MB3_MAM 	RW }
	MID 	{ R AT91C_CAN_MB3_MID 	RW }
	MCR 	{ R AT91C_CAN_MB3_MCR 	WO }
	MMR 	{ R AT91C_CAN_MB3_MMR 	RW }
	MSR 	{ R AT91C_CAN_MB3_MSR 	RO }
	MDL 	{ R AT91C_CAN_MB3_MDL 	RW }
	MDH 	{ R AT91C_CAN_MB3_MDH 	RW }
	listeReg 	{ MFID MAM MID MCR MMR MSR MDL MDH  }

}
array set CAN_MB4_att {
	MID 	{ R AT91C_CAN_MB4_MID 	RW }
	MMR 	{ R AT91C_CAN_MB4_MMR 	RW }
	MDH 	{ R AT91C_CAN_MB4_MDH 	RW }
	MFID 	{ R AT91C_CAN_MB4_MFID 	RO }
	MSR 	{ R AT91C_CAN_MB4_MSR 	RO }
	MCR 	{ R AT91C_CAN_MB4_MCR 	WO }
	MDL 	{ R AT91C_CAN_MB4_MDL 	RW }
	MAM 	{ R AT91C_CAN_MB4_MAM 	RW }
	listeReg 	{ MID MMR MDH MFID MSR MCR MDL MAM  }

}
array set CAN_MB5_att {
	MSR 	{ R AT91C_CAN_MB5_MSR 	RO }
	MCR 	{ R AT91C_CAN_MB5_MCR 	WO }
	MFID 	{ R AT91C_CAN_MB5_MFID 	RO }
	MDH 	{ R AT91C_CAN_MB5_MDH 	RW }
	MID 	{ R AT91C_CAN_MB5_MID 	RW }
	MMR 	{ R AT91C_CAN_MB5_MMR 	RW }
	MDL 	{ R AT91C_CAN_MB5_MDL 	RW }
	MAM 	{ R AT91C_CAN_MB5_MAM 	RW }
	listeReg 	{ MSR MCR MFID MDH MID MMR MDL MAM  }

}
array set CAN_MB6_att {
	MFID 	{ R AT91C_CAN_MB6_MFID 	RO }
	MID 	{ R AT91C_CAN_MB6_MID 	RW }
	MAM 	{ R AT91C_CAN_MB6_MAM 	RW }
	MSR 	{ R AT91C_CAN_MB6_MSR 	RO }
	MDL 	{ R AT91C_CAN_MB6_MDL 	RW }
	MCR 	{ R AT91C_CAN_MB6_MCR 	WO }
	MDH 	{ R AT91C_CAN_MB6_MDH 	RW }
	MMR 	{ R AT91C_CAN_MB6_MMR 	RW }
	listeReg 	{ MFID MID MAM MSR MDL MCR MDH MMR  }

}
array set CAN_MB7_att {
	MCR 	{ R AT91C_CAN_MB7_MCR 	WO }
	MDH 	{ R AT91C_CAN_MB7_MDH 	RW }
	MFID 	{ R AT91C_CAN_MB7_MFID 	RO }
	MDL 	{ R AT91C_CAN_MB7_MDL 	RW }
	MID 	{ R AT91C_CAN_MB7_MID 	RW }
	MMR 	{ R AT91C_CAN_MB7_MMR 	RW }
	MAM 	{ R AT91C_CAN_MB7_MAM 	RW }
	MSR 	{ R AT91C_CAN_MB7_MSR 	RO }
	listeReg 	{ MCR MDH MFID MDL MID MMR MAM MSR  }

}

# ========== Peripheral attributes for TC peripheral ========== 
array set TC0_att {
	SR 	{ R AT91C_TC0_SR 	RO }
	RC 	{ R AT91C_TC0_RC 	RW }
	RB 	{ R AT91C_TC0_RB 	RW }
	CCR 	{ R AT91C_TC0_CCR 	WO }
	CMR 	{ R AT91C_TC0_CMR 	RW }
	IER 	{ R AT91C_TC0_IER 	WO }
	RA 	{ R AT91C_TC0_RA 	RW }
	IDR 	{ R AT91C_TC0_IDR 	WO }
	CV 	{ R AT91C_TC0_CV 	RW }
	IMR 	{ R AT91C_TC0_IMR 	RO }
	listeReg 	{ SR RC RB CCR CMR IER RA IDR CV IMR  }

}
array set TC1_att {
	RB 	{ R AT91C_TC1_RB 	RW }
	CCR 	{ R AT91C_TC1_CCR 	WO }
	IER 	{ R AT91C_TC1_IER 	WO }
	IDR 	{ R AT91C_TC1_IDR 	WO }
	SR 	{ R AT91C_TC1_SR 	RO }
	CMR 	{ R AT91C_TC1_CMR 	RW }
	RA 	{ R AT91C_TC1_RA 	RW }
	RC 	{ R AT91C_TC1_RC 	RW }
	IMR 	{ R AT91C_TC1_IMR 	RO }
	CV 	{ R AT91C_TC1_CV 	RW }
	listeReg 	{ RB CCR IER IDR SR CMR RA RC IMR CV  }

}
array set TC2_att {
	CMR 	{ R AT91C_TC2_CMR 	RW }
	CCR 	{ R AT91C_TC2_CCR 	WO }
	CV 	{ R AT91C_TC2_CV 	RW }
	RA 	{ R AT91C_TC2_RA 	RW }
	RB 	{ R AT91C_TC2_RB 	RW }
	IDR 	{ R AT91C_TC2_IDR 	WO }
	IMR 	{ R AT91C_TC2_IMR 	RO }
	RC 	{ R AT91C_TC2_RC 	RW }
	IER 	{ R AT91C_TC2_IER 	WO }
	SR 	{ R AT91C_TC2_SR 	RO }
	listeReg 	{ CMR CCR CV RA RB IDR IMR RC IER SR  }

}

# ========== Peripheral attributes for SYS peripheral ========== 
array set SYS_att {
	listeReg 	{  }

}

# ========== Peripheral attributes for MC peripheral ========== 
array set MC_att {
	ASR 	{ R AT91C_MC_ASR 	RO }
	RCR 	{ R AT91C_MC_RCR 	WO }
	FCR 	{ R AT91C_MC_FCR 	WO }
	AASR 	{ R AT91C_MC_AASR 	RO }
	FSR 	{ R AT91C_MC_FSR 	RO }
	FMR 	{ R AT91C_MC_FMR 	RW }
	listeReg 	{ ASR RCR FCR AASR FSR FMR  }

}

# ========== Peripheral attributes for PIO peripheral ========== 
array set PIOA_att {
	ODR 	{ R AT91C_PIOA_ODR 	WO }
	SODR 	{ R AT91C_PIOA_SODR 	WO }
	ISR 	{ R AT91C_PIOA_ISR 	RO }
	ABSR 	{ R AT91C_PIOA_ABSR 	RO }
	IER 	{ R AT91C_PIOA_IER 	WO }
	PPUDR 	{ R AT91C_PIOA_PPUDR 	WO }
	IMR 	{ R AT91C_PIOA_IMR 	RO }
	PER 	{ R AT91C_PIOA_PER 	WO }
	IFDR 	{ R AT91C_PIOA_IFDR 	WO }
	OWDR 	{ R AT91C_PIOA_OWDR 	WO }
	MDSR 	{ R AT91C_PIOA_MDSR 	RO }
	IDR 	{ R AT91C_PIOA_IDR 	WO }
	ODSR 	{ R AT91C_PIOA_ODSR 	RO }
	PPUSR 	{ R AT91C_PIOA_PPUSR 	RO }
	OWSR 	{ R AT91C_PIOA_OWSR 	RO }
	BSR 	{ R AT91C_PIOA_BSR 	WO }
	OWER 	{ R AT91C_PIOA_OWER 	WO }
	IFER 	{ R AT91C_PIOA_IFER 	WO }
	PDSR 	{ R AT91C_PIOA_PDSR 	RO }
	PPUER 	{ R AT91C_PIOA_PPUER 	WO }
	OSR 	{ R AT91C_PIOA_OSR 	RO }
	ASR 	{ R AT91C_PIOA_ASR 	WO }
	MDDR 	{ R AT91C_PIOA_MDDR 	WO }
	CODR 	{ R AT91C_PIOA_CODR 	WO }
	MDER 	{ R AT91C_PIOA_MDER 	WO }
	PDR 	{ R AT91C_PIOA_PDR 	WO }
	IFSR 	{ R AT91C_PIOA_IFSR 	RO }
	OER 	{ R AT91C_PIOA_OER 	WO }
	PSR 	{ R AT91C_PIOA_PSR 	RO }
	listeReg 	{ ODR SODR ISR ABSR IER PPUDR IMR PER IFDR OWDR MDSR IDR ODSR PPUSR OWSR BSR OWER IFER PDSR PPUER OSR ASR MDDR CODR MDER PDR IFSR OER PSR  }

}
array set PIOB_att {
	OWDR 	{ R AT91C_PIOB_OWDR 	WO }
	MDER 	{ R AT91C_PIOB_MDER 	WO }
	PPUSR 	{ R AT91C_PIOB_PPUSR 	RO }
	IMR 	{ R AT91C_PIOB_IMR 	RO }
	ASR 	{ R AT91C_PIOB_ASR 	WO }
	PPUDR 	{ R AT91C_PIOB_PPUDR 	WO }
	PSR 	{ R AT91C_PIOB_PSR 	RO }
	IER 	{ R AT91C_PIOB_IER 	WO }
	CODR 	{ R AT91C_PIOB_CODR 	WO }
	OWER 	{ R AT91C_PIOB_OWER 	WO }
	ABSR 	{ R AT91C_PIOB_ABSR 	RO }
	IFDR 	{ R AT91C_PIOB_IFDR 	WO }
	PDSR 	{ R AT91C_PIOB_PDSR 	RO }
	IDR 	{ R AT91C_PIOB_IDR 	WO }
	OWSR 	{ R AT91C_PIOB_OWSR 	RO }
	PDR 	{ R AT91C_PIOB_PDR 	WO }
	ODR 	{ R AT91C_PIOB_ODR 	WO }
	IFSR 	{ R AT91C_PIOB_IFSR 	RO }
	PPUER 	{ R AT91C_PIOB_PPUER 	WO }
	SODR 	{ R AT91C_PIOB_SODR 	WO }
	ISR 	{ R AT91C_PIOB_ISR 	RO }
	ODSR 	{ R AT91C_PIOB_ODSR 	RO }
	OSR 	{ R AT91C_PIOB_OSR 	RO }
	MDSR 	{ R AT91C_PIOB_MDSR 	RO }
	IFER 	{ R AT91C_PIOB_IFER 	WO }
	BSR 	{ R AT91C_PIOB_BSR 	WO }
	MDDR 	{ R AT91C_PIOB_MDDR 	WO }
	OER 	{ R AT91C_PIOB_OER 	WO }
	PER 	{ R AT91C_PIOB_PER 	WO }
	listeReg 	{ OWDR MDER PPUSR IMR ASR PPUDR PSR IER CODR OWER ABSR IFDR PDSR IDR OWSR PDR ODR IFSR PPUER SODR ISR ODSR OSR MDSR IFER BSR MDDR OER PER  }

}

# ========== Peripheral attributes for CAN peripheral ========== 
array set CAN_att {
	TCR 	{ R AT91C_CAN_TCR 	WO }
	IMR 	{ R AT91C_CAN_IMR 	RO }
	IER 	{ R AT91C_CAN_IER 	WO }
	ECR 	{ R AT91C_CAN_ECR 	RO }
	TIMESTP 	{ R AT91C_CAN_TIMESTP 	RO }
	MR 	{ R AT91C_CAN_MR 	RW }
	IDR 	{ R AT91C_CAN_IDR 	WO }
	ACR 	{ R AT91C_CAN_ACR 	WO }
	TIM 	{ R AT91C_CAN_TIM 	RO }
	SR 	{ R AT91C_CAN_SR 	RO }
	BR 	{ R AT91C_CAN_BR 	RW }
	VR 	{ R AT91C_CAN_VR 	RO }
	listeReg 	{ TCR IMR IER ECR TIMESTP MR IDR ACR TIM SR BR VR  }

}

# ========== Peripheral attributes for PWMC peripheral ========== 
array set PWMC_att {
	IDR 	{ R AT91C_PWMC_IDR 	WO }
	DIS 	{ R AT91C_PWMC_DIS 	WO }
	IER 	{ R AT91C_PWMC_IER 	WO }
	VR 	{ R AT91C_PWMC_VR 	RO }
	ISR 	{ R AT91C_PWMC_ISR 	RO }
	SR 	{ R AT91C_PWMC_SR 	RO }
	IMR 	{ R AT91C_PWMC_IMR 	RO }
	MR 	{ R AT91C_PWMC_MR 	RW }
	ENA 	{ R AT91C_PWMC_ENA 	WO }
	listeReg 	{ IDR DIS IER VR ISR SR IMR MR ENA  }

}

# ========== Peripheral attributes for PDC peripheral ========== 
array set PDC_DBGU_att {
	TCR 	{ R AT91C_DBGU_TCR 	RW }
	RNPR 	{ R AT91C_DBGU_RNPR 	RW }
	TNPR 	{ R AT91C_DBGU_TNPR 	RW }
	TPR 	{ R AT91C_DBGU_TPR 	RW }
	RPR 	{ R AT91C_DBGU_RPR 	RW }
	RCR 	{ R AT91C_DBGU_RCR 	RW }
	RNCR 	{ R AT91C_DBGU_RNCR 	RW }
	PTCR 	{ R AT91C_DBGU_PTCR 	WO }
	PTSR 	{ R AT91C_DBGU_PTSR 	RO }
	TNCR 	{ R AT91C_DBGU_TNCR 	RW }
	listeReg 	{ TCR RNPR TNPR TPR RPR RCR RNCR PTCR PTSR TNCR  }

}
array set PDC_SPI1_att {
	PTCR 	{ R AT91C_SPI1_PTCR 	WO }
	RPR 	{ R AT91C_SPI1_RPR 	RW }
	TNCR 	{ R AT91C_SPI1_TNCR 	RW }
	TPR 	{ R AT91C_SPI1_TPR 	RW }
	TNPR 	{ R AT91C_SPI1_TNPR 	RW }
	TCR 	{ R AT91C_SPI1_TCR 	RW }
	RCR 	{ R AT91C_SPI1_RCR 	RW }
	RNPR 	{ R AT91C_SPI1_RNPR 	RW }
	RNCR 	{ R AT91C_SPI1_RNCR 	RW }
	PTSR 	{ R AT91C_SPI1_PTSR 	RO }
	listeReg 	{ PTCR RPR TNCR TPR TNPR TCR RCR RNPR RNCR PTSR  }

}
array set PDC_SPI0_att {
	PTCR 	{ R AT91C_SPI0_PTCR 	WO }
	TPR 	{ R AT91C_SPI0_TPR 	RW }
	TCR 	{ R AT91C_SPI0_TCR 	RW }
	RCR 	{ R AT91C_SPI0_RCR 	RW }
	PTSR 	{ R AT91C_SPI0_PTSR 	RO }
	RNPR 	{ R AT91C_SPI0_RNPR 	RW }
	RPR 	{ R AT91C_SPI0_RPR 	RW }
	TNCR 	{ R AT91C_SPI0_TNCR 	RW }
	RNCR 	{ R AT91C_SPI0_RNCR 	RW }
	TNPR 	{ R AT91C_SPI0_TNPR 	RW }
	listeReg 	{ PTCR TPR TCR RCR PTSR RNPR RPR TNCR RNCR TNPR  }

}
array set PDC_US1_att {
	RNCR 	{ R AT91C_US1_RNCR 	RW }
	PTCR 	{ R AT91C_US1_PTCR 	WO }
	TCR 	{ R AT91C_US1_TCR 	RW }
	PTSR 	{ R AT91C_US1_PTSR 	RO }
	TNPR 	{ R AT91C_US1_TNPR 	RW }
	RCR 	{ R AT91C_US1_RCR 	RW }
	RNPR 	{ R AT91C_US1_RNPR 	RW }
	RPR 	{ R AT91C_US1_RPR 	RW }
	TNCR 	{ R AT91C_US1_TNCR 	RW }
	TPR 	{ R AT91C_US1_TPR 	RW }
	listeReg 	{ RNCR PTCR TCR PTSR TNPR RCR RNPR RPR TNCR TPR  }

}
array set PDC_US0_att {
	TNPR 	{ R AT91C_US0_TNPR 	RW }
	RNPR 	{ R AT91C_US0_RNPR 	RW }
	TCR 	{ R AT91C_US0_TCR 	RW }
	PTCR 	{ R AT91C_US0_PTCR 	WO }
	PTSR 	{ R AT91C_US0_PTSR 	RO }
	TNCR 	{ R AT91C_US0_TNCR 	RW }
	TPR 	{ R AT91C_US0_TPR 	RW }
	RCR 	{ R AT91C_US0_RCR 	RW }
	RPR 	{ R AT91C_US0_RPR 	RW }
	RNCR 	{ R AT91C_US0_RNCR 	RW }
	listeReg 	{ TNPR RNPR TCR PTCR PTSR TNCR TPR RCR RPR RNCR  }

}
array set PDC_SSC_att {
	TNCR 	{ R AT91C_SSC_TNCR 	RW }
	RPR 	{ R AT91C_SSC_RPR 	RW }
	RNCR 	{ R AT91C_SSC_RNCR 	RW }
	TPR 	{ R AT91C_SSC_TPR 	RW }
	PTCR 	{ R AT91C_SSC_PTCR 	WO }
	TCR 	{ R AT91C_SSC_TCR 	RW }
	RCR 	{ R AT91C_SSC_RCR 	RW }
	RNPR 	{ R AT91C_SSC_RNPR 	RW }
	TNPR 	{ R AT91C_SSC_TNPR 	RW }
	PTSR 	{ R AT91C_SSC_PTSR 	RO }
	listeReg 	{ TNCR RPR RNCR TPR PTCR TCR RCR RNPR TNPR PTSR  }

}
array set PDC_ADC_att {
	PTSR 	{ R AT91C_ADC_PTSR 	RO }
	PTCR 	{ R AT91C_ADC_PTCR 	WO }
	TNPR 	{ R AT91C_ADC_TNPR 	RW }
	TNCR 	{ R AT91C_ADC_TNCR 	RW }
	RNPR 	{ R AT91C_ADC_RNPR 	RW }
	RNCR 	{ R AT91C_ADC_RNCR 	RW }
	RPR 	{ R AT91C_ADC_RPR 	RW }
	TCR 	{ R AT91C_ADC_TCR 	RW }
	TPR 	{ R AT91C_ADC_TPR 	RW }
	RCR 	{ R AT91C_ADC_RCR 	RW }
	listeReg 	{ PTSR PTCR TNPR TNCR RNPR RNCR RPR TCR TPR RCR  }

}

# ========== PIO information ========== 

array set def_PIOA_att {
 	PA0 	{  RXD0  }
 	PA1 	{  TXD0  }
 	PA10 	{  TWD  }
 	PA11 	{  TWCK  }
 	PA12 	{  SPI0_NPCS0  }
 	PA13 	{  SPI0_NPCS1   PCK1  }
 	PA14 	{  SPI0_NPCS2   IRQ1  }
 	PA15 	{  SPI0_NPCS3   TCLK2  }
 	PA16 	{  SPI0_MISO  }
 	PA17 	{  SPI0_MOSI  }
 	PA18 	{  SPI0_SPCK  }
 	PA19 	{  CANRX  }
 	PA2 	{  SCK0   SPI1_NPCS1  }
 	PA20 	{  CANTX  }
 	PA21 	{  TF   SPI1_NPCS0  }
 	PA22 	{  TK   SPI1_SPCK  }
 	PA23 	{  TD   SPI1_MOSI  }
 	PA24 	{  RD   SPI1_MISO  }
 	PA25 	{  RK   SPI1_NPCS1  }
 	PA26 	{  RF   SPI1_NPCS2  }
 	PA27 	{  DRXD   PCK3  }
 	PA28 	{  DTXD  }
 	PA29 	{  FIQ   SPI1_NPCS3  }
 	PA3 	{  RTS0   SPI1_NPCS2  }
 	PA30 	{  IRQ0   PCK2  }
 	PA4 	{  CTS0   SPI1_NPCS3  }
 	PA5 	{  RXD1  }
 	PA6 	{  TXD1  }
 	PA7 	{  SCK1   SPI0_NPCS1  }
 	PA8 	{  RTS1   SPI0_NPCS2  }
 	PA9 	{  CTS1   SPI0_NPCS3  }
 }

array set def_PIOB_att {
 	PB0 	{  ETXCK_EREFCK   PCK0  }
 	PB1 	{  ETXEN  }
 	PB10 	{  ETX2   SPI1_NPCS1  }
 	PB11 	{  ETX3   SPI1_NPCS2  }
 	PB12 	{  ETXER   TCLK0  }
 	PB13 	{  ERX2   SPI0_NPCS1  }
 	PB14 	{  ERX3   SPI0_NPCS2  }
 	PB15 	{  ERXDV_ECRSDV  }
 	PB16 	{  ECOL   SPI1_NPCS3  }
 	PB17 	{  ERXCK   SPI0_NPCS3  }
 	PB18 	{  EF100   ADTRG  }
 	PB19 	{  PWM0   TCLK1  }
 	PB2 	{  ETX0  }
 	PB20 	{  PWM1   PCK0  }
 	PB21 	{  PWM2   PCK1  }
 	PB22 	{  PWM3   PCK2  }
 	PB23 	{  TIOA0   DCD1  }
 	PB24 	{  TIOB0   DSR1  }
 	PB25 	{  TIOA1   DTR1  }
 	PB26 	{  TIOB1   RI1  }
 	PB27 	{  TIOA2   PWM0  }
 	PB28 	{  TIOB2   PWM1  }
 	PB29 	{  PCK1   PWM2  }
 	PB3 	{  ETX1  }
 	PB30 	{  PCK2   PWM3  }
 	PB4 	{  ECRS  }
 	PB5 	{  ERX0  }
 	PB6 	{  ERX1  }
 	PB7 	{  ERXER  }
 	PB8 	{  EMDC  }
 	PB9 	{  EMDIO  }
 }

// *FORMAT-OFF*
/* Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#ifndef DCD_ENSEMBLE_DEF_H
#define DCD_ENSEMBLE_DEF_H

// Alif USB ID definitions (Global Id Register)
#define GSNPSID_HIGH            0x55330000  // Core Identification Number
#define GSNPSID_LOW             0x0000330B  // Release number 3.30b

// Alif USB2.0 turnaround time
#define GUSB2PHYCFG0_USBTRDTIM_16BIT    0x5 // MAC interface is 16-bit UTMI+
#define GUSB2PHYCFG0_USBTRDTIM_8BIT     0x9 // MAC interface is 8-bit UTMI+/ULPI

// DCFG USB device speed settings
#define DCFG_DEVSPD_HS          0x00        // High-speed 480MBit/s
#define DCFG_DEVSPD_FS          0x01        // Full-speed 12MBit/s

// DSTS USB device speed
#define DSTS_CONNECTSPD_HS      0x0         // High-speed
#define DSTS_CONNECTSPD_FS      0x1         // Full-speed

// Physical endpoints
enum {
    PHY_EP0 = 0,                    // USB Control EP0 OUT
    PHY_EP1 = 1,                    // USB Control EP0 IN
    PHY_EP_MAX = 7
} PHY_EP;

// USB Registers Access Types
#define _rw volatile uint32_t
#define __w volatile uint32_t
#define __r volatile const uint32_t

// USB Device Controller Base Address  
#define USB_BASE                0x48200000UL

// USB GBL Registers
volatile struct {
    union {
        _rw gsbuscfg0;                  // Global SoC Bus Configuration Register 0
        struct {                        
            _rw incrbrstena    : 1;     // Undefined length INCR burst type enable
            _rw incr4brstena   : 1;     // INCR4 burst type enable
            _rw incr8brstena   : 1;     // INCR8 burst type enable
            _rw incr16brstena  : 1;     // INCR16 burst type enable
            _rw incr32brstena  : 1;     // INCR32 burst type enable
            _rw incr64brstena  : 1;     // INCR64 burst type enable
            _rw incr128brstena : 1;     // INCR128 burst type enable
            _rw incr256brstena : 1;     // INCR256 burst type enable
            __r : 2;                    // Reserved
            _rw desbigend      : 1;     // Descriptor access is big endian
            _rw datbigend      : 1;     // Data access is big endian
            __r : 4;                    // Reserved
            _rw deswrreqinfo   : 4;     // Descriptor write request info
            _rw datwrreqinfo   : 4;     // Data write request info 
            _rw desrdreqinfo   : 4;     // Descriptor read request info
            _rw datrdreqinfo   : 4;     // Data read request info
        } gsbuscfg0_b;
    };

    union {
        _rw gsbuscfg1;                  // Global SoC Bus Configuration Register 1
        struct {
            __r : 8;                    // Reserved
            _rw pipetranslimit : 4;     // AXI pipelined transfers burst request limit
            _rw en1kpage       : 1;     // 1 KB page boundary
            __r : 19;                   // Reserved
        } gsbuscfg1_b;
    };

    __r : 32; __r : 32;

    union {
        _rw gctl;                       // Global Core Control Register
        struct {
            _rw dsblclkgtng      : 1;   // Disable clock gating
            __r gblhibernationen : 1;   // Hibernation enable status at the global level
            __r : 1;                    // Reserved
            _rw disscramble      : 1;   // Disable scrambling
            _rw scaledown        : 2;   // Scale-down mode
            _rw ramclksel        : 2;   // RAM clock select
            __r : 2;                    // Reserved
            _rw sofitpsync       : 1;   // Reserved
            _rw coresoftreset    : 1;   // Core soft reset
            _rw prtcapdir        : 2;   // Port capability direction
            _rw frmscldwn        : 2;   // Frame scales down
            __r : 1;                    // Reserved
            _rw bypssetaddr      : 1;   // Bypass set address in Device mode
            __r : 14;                   // Reserved
        } gctl_b;
    };

    __r : 32;

    union {
        _rw gsts;                       // Global Status Register
        struct {
            __r curmod        : 2;      // Current operation mode (Device/Host)
            __r : 2;                    // Reserved
            _rw buserraddrvld : 1;      // Bus error address valid
            _rw csrtimeout    : 1;      // CSR timeout
            __r device_ip     : 1;      // Device interrupt pending
            __r host_ip       : 1;      // Host interrupt pending
            __r adp_ip        : 1;      // Not used
            __r bc_ip         : 1;      // Not used
            __r otg_ip        : 1;      // Not used
            __r ssic_ip       : 1;      // Not used
            __r : 8;                    // Reserved
            __r cbelt         : 12;     // Current BELT value
        } gsts_b;
    };

    union {
        _rw guctl1;                             // Global User Control Register 1
        struct {
            _rw loa_filter_en             : 1;  // LOA filter enable
            _rw ovrld_l1_susp_com         : 1;  // Overload UTMI_L1_SUSPEND_COM
            _rw hc_parchk_disable         : 1;  // Host parameter check disable
            _rw hc_errata_enable          : 1;  // Host Exit Latency Delta enable
            _rw l1_susp_thrld_for_host    : 4;  // L1 suspend threshold in Host mode
            _rw l1_susp_thrld_en_for_host : 1;  // L1 suspend threshold enable in Host mode
            _rw dev_hs_nyet_bulk_spr      : 1;  // HS bulk OUT short packet gets NYET in Device mode
            _rw resume_opmode_hs_host     : 1;  // Opmode in HS resume in Host mode
            __r : 1;                            // Reserved
            _rw disusb2refclkgtng         : 1;  // Disable REF_CLK gating for 2.0 PHY
            __r : 2;                            // Reserved
            _rw parkmode_disable_fsls     : 1;  // Disable park mode of FS/LS bus instances
            _rw parkmode_disable_hs       : 1;  // Disable park mode of HS bus instances
            __r : 1;                            // Reserved
            _rw nak_per_enh_hs            : 1;  // Performance enchancement for HS in NAK
            _rw nak_per_enh_fs            : 1;  // Performance enchancement for FS in NAK
            _rw dev_lsp_tail_lock_dis     : 1;  // Disable device LSP lock logic for tail TRB
            _rw ip_gap_add_on             : 2;  // Inter packet gap add on
            _rw dev_l1_exit_by_hw         : 1;  // Device in L1 hardware exit
            __r : 2;                            // Reserved
            _rw dev_trb_out_spr_ind       : 1;  // OUT in TRB status short packet indication
            _rw tx_ipgap_linecheck_dis    : 1;  // Disable TX IPGAP LineState check
            _rw filter_se0_fsls_eop       : 1;  // Filter SE0 detection in FS/LS or EOP
            __r : 1;                            // Reserved
            _rw dev_decouple_l1l2_evt     : 1;  // Device decoupled L1/L2 event
        } guctl1_b;
    };

    union {
        _rw gsnpsid;                    // Global ID Register
    };

    __r : 32;

    union {
        _rw guid;                       // Global User ID Register
        struct {                        // Reset value is 0x12345678
            _rw userid : 32;
        } guid_b;
    };

    union { // <- host only, leaving unimplemented for now [FIXME]
        _rw guctl;
        struct {
        } guctl_b;
    };

    union {
        _rw gbuserraddrlo;              // Global SoC Bus Error Address Register (LOW)
        struct {
            _rw buserraddr : 32;        // Bus address (LOW)
        } gbuserraddrlo_b;
    };

    union {
        _rw gbuserraddrhi;              // Global SoC Bus Error Address Register (HIGH)
        struct {
            _rw buserraddr : 32;        // Bus address (HIGH)
        } gbuserraddrhi_b;
    };

    __r : 32; __r : 32;

    __r ghwparams[8];                   // Global HW Parameters Registers [0-7]

    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32;

    // base + 0x9C here

    union {
        _rw guctl2;                     // Global User Control Register 2
        struct {
            __r : 11;                   // TODO:
            _rw disablecfc         : 1;
            _rw enableepcacheevict : 1;
            __r : 1;
            _rw rst_actbitlater    : 1;
            __r : 4;
            _rw en_hp_pm_timer     : 7;
            __r : 6;
        } guctl2_b;
    };

    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;

    // base + 0x100 here

    union {
        _rw gusb2phycfg0;                       // Global USB2 PHY Configuration Register
        struct {
            _rw toutcal                  : 3;   // HS/FS timeout calibration
            _rw phyif                    : 1;   // PHY interface (8 or 16 bit)
            __r ulpi_utmi_sel            : 1;   // ULPI or UTMI+ selection
            __r fsintf                   : 1;   // Full-Speed interface select (allways 0)
            _rw suspendusb20             : 1;   // Suspend USB2.0 HS/FS/Ls PHY
            __w physel                   : 1;   // USB2.0 HS or USB1.1 FS
            _rw enblslpm                 : 1;   // Enable UTMI_SLEEP and UTMI_L1_SUSPEND_n
            _rw xcvrdly                  : 1;   // Transceiver Delay
            _rw usbtrdtim                : 4;   // USB2.0 turnaround time (in PHY clocks)
            __r : 5;                            // Reserved
            _rw lsipd                    : 3;   // LS inter-packet time
            _rw lstrd                    : 3;   // LS turnaround time
            _rw ovrd_fsls_disc_time      : 1;   // Overriding the FS/LS disconnect time to 32 us
            __r inv_sel_hsic             : 1;   // Reserved
            __r hsic_con_width_adj       : 2;   // Reserved
            _rw ulpi_lpm_with_opmode_chk : 1;   // Reserved (keep at 0)
            _rw u2_freeclk_exists        : 1;   // U2 Free Clock Esists
            _rw physoftrst               : 1;   // UTMI PHY soft reset
        } gusb2phycfg0_b;
    };

    __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;

    // base + 0x200 here

    union {
        _rw gtxfifosiz[4];              // Global Transmit FIFO Size Registers [0-3]
        struct {
            _rw txfdep : 16;            // TxFIFO Depth (0xa on reset)
            _rw txfstaddr : 16;         // TxFIFO RAM start address (0x271 on reset)
        } gtxfifosiz_b[4];
    };

    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;

    // base + 0x280 here

    union {
        _rw grxfifosiz[4];              // Global Receive FIFO Size Registers [0-3]
        struct {
            _rw rxfdep : 16;            // RxFIFO depth (0x105 on reset)
            _rw rxfstaddr : 16;         // RxFIFO RAM start address (0x271)
        } grxfifosiz_b[4];
    };

    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;

    // base + 0x300 here

    union {
        _rw gevntadrlo0;                // Global Event Buffer Address (Low) Register
        struct {
            _rw evntadrlo : 32;         // Event buffer address (Low)
        } gevntadrlo0_b;
    };

    union {
        _rw gevntadrhi0;                // Global Event Buffer Address (High) Register
        struct {
            _rw evntadrhi : 32;         // Event buffer address (High)
        } gevntadrhi0_b;
    };

    union {
        _rw gevntsiz0;                  // Global Event Buffer Size Register
        struct {        
            _rw eventsiz : 16;          // Event buffer size in bytes
            __r : 15;                   // Reserved
            _rw evntintrptmask : 1;     // Event interrupt mask
        } gevntsiz0_b;
    };

    union {
        _rw gevntcount0;                // Global Event Buffer Count Register
        struct {
            _rw evntcount : 16;         // Event count
            __r : 15;
            _rw evnt_handler_busy : 1;  // Event handler busy
        } gevntcount0_b;
    };

    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;

    // base + 0x500 here

    __r ghwparams8;                 // Global HW Parameters Register 8

    __r : 32; __r : 32; __r : 32;

    // base + 0x510 here

    union {
        _rw gtxfifopridev;
        struct {
            _rw gtxfifopridev : 4;
            __r : 28;
        } gtxfifopridev_b;
    };

    __r : 32;

    union {
        _rw gtxfifoprihst;
        struct {
            _rw gtxfifoprihst : 2;
            __r : 30;
        } gtxfifoprihst_b;
    };

    union {
        _rw grxfifoprihst;
        struct {
            _rw grxfifoprihst : 2;
            __r : 30;
        } grxfifoprihst_b;
    };

    __r : 32; __r : 32; __r : 32; __r : 32;

    // base + 0x530 here

    union {
        _rw gfladj;
        struct {
            _rw gfladj_30mhz : 6;
            __r : 1;
            _rw gfladj_30mhz_sdbnd_sel : 1;
            _rw gfladj_refclk_fladj : 14;
            __r : 1;
            _rw gfladj_refclk_lpm_sel : 1;
            _rw gfladj_refclk_240mhz_decr : 7;
            _rw gfladj_refclk_240mhzdecr_pls1 : 1;
        } gfladj_b;
    };

    __r : 32; __r : 32; __r : 32;

    // base + 0x540 here

    union {
        _rw gusb2rhbctl0;
        struct {
            _rw ovrd_l1timeout : 4;
            __r : 28;
        } gusb2rhbctl0_b;
    };

} *ugbl = (void *) (USB_BASE + 0xC100);

volatile struct {
    union {
        _rw dcfg;                       // Device Configuration Register
        struct {
            _rw devspd : 3;             // Device Speed
            _rw devaddr : 7;            // Device Address
            __r : 2;                    // Reserved
            _rw intrnum : 5;            // Interrupt number
            _rw nump : 5;               // Number of Receive Buffers
            _rw lpmcap : 1;             // LPM Capable
            _rw ignstrmpp : 1;          // Ignore Stream PP
            __r : 8;
        } dcfg_b;
    };

    union {
        _rw dctl;                       // Device Control register
        struct {
            __r : 1;                    // Reserved
            _rw tstctl : 4;             // Test Control
            __w ulstchngreq : 4;        // USB/Link state change request
            __r : 7;                    // Reserved
            _rw css : 1;                // Controller Save State (CSS)
            _rw crs : 1;                // Controller Restore State (CRC)
            _rw l1hibernationen : 1;    // L1 Hibernation Enable    
            _rw keepconnect : 1;        // Keep Connect
            _rw lpm_nyet_thres : 4;     // LPM NYET Threshold
            _rw hirdthres : 5;          // HIRD Threshold
            __r : 1;                    // Reserved
            _rw csftrst : 1;            // Core Soft Reset
            _rw run_stop : 1;           // Start/Stop the device controller operation
        } dctl_b;
    };

    union {
        _rw devten;                     // Device Event Enable Register
        struct {
            _rw dissconnevten : 1;      // Disconnect Detected Event Enable
            _rw usbrstevten : 1;        // USB Reset Enable
            _rw connectdoneevten : 1;   // Connection Done Enable
            _rw ulstcngen : 1;          // USB/Link State Change Event Enable
            _rw wkupevten : 1;          // L2 or L2L1 Resume Detected Event Enable
            _rw hibernationreqevten : 1;// Generation of the Hibernation Request Event Enable
            _rw u3l2l1suspen : 1;       // L2 or L2L1 Suspend Event Enable
            _rw softevten : 1;          // Start of (micro)frame
            _rw l1suspen : 1;           // L1 Suspend Event Enable
            _rw errticerrevten : 1;     // Erratic Error Event Enable
            __r : 2;                    // Reserved
            _rw vendevtstrcvden : 1;    //Vendor Device Test LMP Received Event
            __r : 1;                    // Reserved
            _rw l1wkupevten : 1;        // L1 Resume Detected Event Enable
            __r : 1;                    // Reserved
            _rw eccerren : 1;           // ECC Error Enable
            __r : 15;                   // Reserved
        } devten_b;
    };

    union {                             // Device Status Register
        _rw dsts;
        struct {
            __r connectspd : 3;         // Connection Speed
            __r soffn : 14;             // Frame/Microframe Number of the Received SOF
            __r rxfifoempty : 1;        // RxFIFO Empty
            __r usblnkst : 4;           // USB/Link State
            __r devctrlhlt : 1;         // Device Controller Halted
            __r coreidle : 1;           // Core Idle
            __r sss : 1;                // SSS Save State Status
            __r rss : 1;                // RSS Restore State Status
            __r : 2;
            _rw sre : 1;                // Save Restore Error
            __r dcnrd : 1;              // Device Controller Not Ready (not used)
            __r : 2;
        } dsts_b;
    };

    union {                             // Device Generic Command Register
        _rw dgcmdpar;
        struct {
            _rw parameter : 32;
        } dgcmdpar_b;
    };

    union {                             // Device Generic Comand Register
        _rw dgcmd;
        struct {
            _rw cmdtyp : 8;             // Generic Command Type
            _rw cmdioc : 1;             // Command Interrupt on Complete
            __r : 1;
            _rw cmdact : 1;             // Command Active
            __r : 1;
            __r cmdstatus : 4;          // Command Status
            __r : 16;
        } dgcmd_b;
    };

    __r : 32; __r : 32;

    union {
        _rw dalepena;                   // Device Active USB Endpoint Enable Register
        struct {
            //_rw usbactep : 32;        // USB Active Endpoints
            __r : 24;                   // Reserved
            _rw usbactep3in : 1;        // USB Active EP3-IN
            _rw usbactep3out : 1;       // USB Active EP3-OUT
            _rw usbactep2in : 1;        // USB Active EP2-IN
            _rw usbactep2out : 1;       // USB Active EP2-OUT
            _rw usbactep1in : 1;        // USB Active EP1-IN
            _rw usbactep1out : 1;       // USB Active EP1-OUT
            _rw usbactep0in : 1;        // USB Active EP0-IN
            _rw usbactep0out : 1;       // USB Active EP0-OUT
        } dalepena_b;
    };

    __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;

    // base + 0x100 here

    struct {
        union {
            _rw params[3];
            struct {
                union {
                    _rw par2;           // DEPCMD Parameter 2 Register
                    struct {
                        _rw parameter : 32;
                    } par2_b;
                };

                union {
                    _rw par1;           // DEPCMD Parameter 1 Register
                    struct {
                        _rw parameter : 32;
                    } par1_b;
                };

                union {
                    _rw par0;           // DEPCMD Parameter 0 Register
                    struct {
                        _rw parameter : 32;
                    } par0_b;
                };
            };
        };

        union {
            _rw depcmd;                 // Device Physical Endpoint-n Command Registers
            struct {
                _rw cmdtyp : 4;         // Command type
                __r : 4;                // Reserved 
                _rw cmdioc : 1;         // Command Interrupt on Complete
                __r : 1;                // Reserved
                _rw cmdact : 1;         // Command Active
                _rw hipri_forcerm : 1;  // HighPriority/ForceRM
                _rw cmdstatus : 4;      // Command Completion Status
                _rw commandparam : 16;  // Command Parameters or Event Parameters
            } depcmd_b;
        };
    } depcmd[8];

    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;

    // base + 0x300 here

    union {                             // Device interrupt Moderation Register
        _rw dev_imod0;
        struct {
            _rw device_imodi : 16;      // Moderation Interval
            _rw device_imodc : 16;      // Interrupt Moderation Down Counter
        } dev_imod0_b;
    };
} *udev = (void *) (USB_BASE + 0xC700);

// Device Event Buffer Structure
typedef union {
    _rw val : 32;

    struct {
        _rw sig : 1;    // 0 = DEPEVT
        _rw ep  : 5;    // Physical Endpoint Number (0–31)
        _rw evt : 4;    // Event Type
                        //      0x1 = XferComplete
                        //      0x2 = XferInProgress
                        //      0x3 = XferNotReady
                        //      0x6 = StreamEvt
                        //      0x7 = EPCmdCmplt
        __r : 2;        // Reserved
        _rw sts : 4;    // Event-specific status (meaning varies by type)
        _rw par : 16;   // Stream ID, IsoMicroFrameNum, etc.
    } depevt;           // Device Endpoint-Specific Event (DEPEVT)
    
    struct {
        _rw sig  : 8;   // 0x01 = DEVT
        _rw evt  : 5;   // Event Type (see below)
        __r : 3;        // Reserved
        _rw info : 9;   // Event-specific information (e.g., link state)
        __r : 7;        // Reserved
    } devt;             // Device Specific Events (DEVT)

} evt_t;

// DEPEVT event type
enum {
    DEPEVT_XFERCOMPLETE = 0x1,  // EP/Stream trnsfer completed
    DEPEVT_XFERINPROGRESS = 0x2,// Ep/Stream specific event happaned, continue transfer
    DEPEVT_XFERNOTREADY = 0x3,  // No TRBs are available for the endpoint
    DEPEVT_STREAMEVT = 0x6,     // Search withing EP transfer resource cache
    DEPEVT_EPCMDCMPLT = 0x7     // EP Command complete
} DEPEVT;

// DEPEVT XferNotReady Event Status Control Data Stage
#define DEPEVT_XFERNOTREADY_STS_CTRLDAT 0x1
// DEPEVT XferNotReady Event Status Control Status Stage
#define DEPEVT_XFERNOTREADY_STS_CTRLSTS 0x2

// DEPEVT Event Status Structure
typedef volatile union {
    _rw val : 4;
    
    struct {                        // XferNotReady Event Status
        _rw stage : 2;              // Requested stage
        __r : 1;                    // Not Used
        _rw active : 1;             // XferActive
    } xfernotready;
    
    struct {                        // XferComplete Event Status
        __r : 1;                    // Not Used
        _rw sp_lst : 1;             // Short packet reception or the lasp packet of iso interval
        _rw ioc : 1;                // IOC bit of the TRB
        _rw missedisoc : 1;         // Interval did not complete successfully
    } xfercomplete;

    struct {                        // XferComplete Event Status
        __r : 1;                    // Not Used
        _rw sp_lst : 1;             // Short packet reception or the lasp packet of iso interval
        _rw ioc : 1;                // IOC bit of the TRB
        _rw lst : 1;                // LST bit of the TRB
    } xferinprogress;

    _rw stream : 4;             // Stream Event Status
    
    _rw epcmdcmplt : 4;         // EP Command Complete Event Status

} depevt_sts_t;

// DEVT event type
enum {
    DEVT_DISCONNEVT = 0x0,      // Disconnect detected event
    DEVT_USBRST = 0x1,          // USB reset
    DEVT_CONNECTDONE = 0x2,     // Connection Done
    DEVT_ULSTCHNG = 0x03,       // USB/Link State Change
    DEVT_WKUPEVT = 0x4,         // Resume/Remote Wakeup Detected Event
    DEVT_HIBRQ = 0x5,           // Hibernation Request Event (not supported)
    DEVT_USBSUSP = 0x6,         // USB Suspended Entry Event
    DEVT_SOF = 0x7,             // Start of (micro-)Frame
    DEVT_L1SUSP = 0x8,          // L1 Suspend Event
    DEVT_ERRTICERR = 0x9,       // Erratic Error Event
    DEVT_CMDCMPLT = 0xA,        // Generic Command Complete Event
    DEVT_EVNTOVERFLOW = 0xB,    // Event Buffer Overflow Event
    DEVT_VNDDEVTSTRCVED = 0xC,  // Vendor Device Test LMP Received Event
    DEVT_L1RESM = 0xE,          // L1 Resume/RemoteWake Event
    DEVT_ECCERR = 0x10          // ECC Error
} DEVT;

// Link state for DEVT_ULSTCHNG event
enum {
    DEVT_ULSTCHNG_ON = 0x0,             // ON state
    DEVT_ULSTCHNG_L1SLP = 0x02,         // L1 state (sleep)
    DEVT_ULSTCHNG_L2SUSP = 0x3,         // L2 state (suspend)
    DEVT_ULSTCHNG_DISCONNECTED = 0x4,   // Disconnected state
    DEVT_ULSTCHNG_EARLYSUSP = 0x5,      // Early suspend
    DEVT_ULSTCHNG_RESET = 0xE,          // Reset
    DEVT_ULSTCHNG_RESUME = 0xF          // Resume
} DEVT_ULSTCHNG_LINKSTATE;

// Command type the SW driver is requesting the controller to perform
enum {
    CMDTYP_RESERVED = 0,
    CMDTYP_DEPCFG = 1,      // Set Endpoint Configuration 64- or 96-bit param
    CMDTYP_DEPXFERCFG = 2,  // Set Endpoint Transfare Resource Configuration: 32-bit param
    CMDTYP_DEPGETSTATE = 3, // Get Endpoint State: No param
    CMDTYP_DEPSSTALL = 4,   // Set Stall: No param 
    CMDTYP_DEPCSTALL = 5,   // Clear Stall: No param
    CMDTYP_DEPSTRTXFER = 6, // Start Transfer 64-bit param 
    CMDTYP_DEPUPDXFER = 7,  // Update Transfare: No param
    CMDTYP_DEPENDXFER = 8,  // End Transfer: No param
    CMDTYP_DEPSTARTCFG = 9  // Start New Configuration: No param
} DEPCMD_CMDTYP;

// DEPCFG Parameters Structure
typedef volatile struct {
    struct {                        // Parameter 2
        __r : 1;                    // Reserved
        _rw  epstate: 30;           // Endpoint State (if p0.configaction is 1)
        __r : 1;                    // Reserved
    };

    struct {                        // Parameter 1
        _rw intrnum: 5;             // Interrupt number
        __r : 3;                    // Reserved
        _rw xfercmplen : 1;         // XferComplete Enable
        _rw xferinprogen : 1;       // XferInProgress Enable
        _rw xfernrdyen : 1;         // XferNotReady Enable
        __r : 2;                    // Reserved
        _rw streamevten : 1;        // Stream Event Enable
        _rw  tbrnotupdt: 1;         // Does not update HWO in TBR descriptor
        _rw  ebcen: 1;              // External Buffer Control Mode Enable
        _rw  bintervalm1: 8;        // bInterval value
        _rw  strmcap: 1;            // Stream Capable
        _rw  epnum: 5;              // USB Endpoint Number
        __r : 1;                    // Reserved
        _rw  fifobased: 1;          // FIFO-based data stream
    };

    struct {                        // Parameter 0
        __r : 1;                    // Reserved
        _rw eptype : 2;             // Endpoint Type
        _rw mps : 11;               // Maximum Packet size
        __r : 3;                    // Reserved
        _rw fifonum : 5;            // FIFO Number
        _rw brstsiz : 4;            // Burst Size
        __r : 4;                    // Reserved
        _rw configaction : 2;       // Config Action
    };

} depcfg_params_t;

// DEPCFG Param0: Config Actions
enum {
    DEPCFG_CONFIGACTION_INIT = 0,   // Initialize endpoint state
    DEPCFG_CONFIGACTION_RESTORE = 1,// Restore endpoint state
    DEPCFG_CONFIGACTION_MODIFY = 2  // Modify endpoint state
} DEPCFG_CONFIGACTION;

// DEPCFG Param0: Transfer type supported by USB endpoint
enum {
    DEPCFG_EPTYPE_CONTROL = 0,      // Control
    DEPCFG_EPTYPE_ISOCHRONOUS = 1,  // Isochronous
    DEPCFG_EPTYPE_BULK = 2,         // Bulk
    DEPCFG_EPTYPE_INTERRUPT = 3     // Interrupt
} DEPCFG_EPTYPE;

// DEPCFERCFG Parameters Structure
typedef volatile struct {
    __r : 32;                       // Parameter 2 (Reserved)
    __r : 32;                       // Parameter 1 (Reserved)
    struct {                        // Parameter 0
        _rw numxferres : 16;        // Number of Transfer Resources
        __r : 16;                   // Reserved
    };
} depxfercfg_params_t;

// DEPSTRTXFER Parameters Structure
typedef volatile struct {
    __r par2: 32;                   // Parameter 2 (Reserved)
    _rw tdaddrlow: 32;              // Transfer Descriptor Address (Low)
    _rw tdaddrhigh: 32;             // Transfer Descriptor Address (High)
} depstrtxfer_params_t;

// TRB Control and Status Structure
typedef struct {
    _rw bptrl: 32;                  // Buffer Pointer Low
    _rw bptrh: 32;                  // Buffer Pointer High
    struct {
        _rw bufsiz : 24;            // Buffer Size
        _rw pcm1: 2;                // Packet Count M1
        _rw spr: 1;                 // Short Packet Received
        __r : 1;                    // Reserved
        _rw trbsts: 4;              // TRB Status
    };
    struct {
        _rw hwo : 1;                // Hardware Owner of Descriptor
        _rw lst : 1;                // Last TRB
        __r chn : 1;                // Chain Buffers
        __r csp : 1;                // Continue on Short Packet
        _rw trbctl : 6;             // TRB Control
        _rw ispimi : 1;             // Interrupt on Short Packet / MissedIsoc
        _rw intcmpl : 1;            // Interrupt on Complete
        __r : 2;                    // Reserved
        _rw streamid : 16;          // Stream ID / SOF Number
        __r : 2;                    // Reserved
    };
} trb_t;

// TRB Status
enum {
    TRB_STATUS_OK = 0x0,            // OK
    TRB_STATUS_MISSEDISOC = 0x1,    // Isochronous interval missed or incomplete
    TRB_STATUS_SETUPPEND = 0x2,     // Setup Pending
    TRB_STATUS_XFERINPROG = 0x4,    // Transfer In Progress 
    TRB_STATUS_ZLPPENDING = 0xF     // Zero-length-packet pending
} TRB_STATUS;

// TRB Control
enum {
    TRBCTL_NORMAL = 0x1,            // Normal
    TRBCTL_CTL_SETUP = 0x2,         // Control-Setup
    TRBCTL_CTL_STAT2 = 0x3,         // Control-Status-2 (SETUP w/o data stage)
    TRBCTL_CTL_STAT3 = 0x4,         // Control-Status-3 (SETUP w/ data stage)
    TRBCTL_CTL_DATA = 0x5,          // Control-Data (the 1st TRB of a data statge)
    TRBCTL_ISO_FIRST = 0x6,         // Isochronous-First (the 1st TRB of a SERVICE Interval)
    TRBCTL_ISO = 0x7,               // Isochronous
    TRBCTL_LINK = 0x8,              // Link TRB
    TRBCTL_NORMAL_ZLP = 0x9         // Normal-ZLP (Bulk IN)
} TRBCTL;

#endif // DCD_ENSEMBLE_DEF_H

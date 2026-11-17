// *FORMAT-OFF*
/*
 * Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 */

#ifndef __ALIF_DCD_REG_H__
#define __ALIF_DCD_REG_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//--------------------------------------------------------------------------------------------------

#define _rw volatile uint32_t
#define __w volatile uint32_t
#define __r volatile const uint32_t

volatile struct {
    union {
        _rw gsbuscfg0;
        struct {
            _rw incrbrstena    : 1;
            _rw incr4brstena   : 1;
            _rw incr8brstena   : 1;
            _rw incr16brstena  : 1;
            _rw incr32brstena  : 1;
            _rw incr64brstena  : 1;
            _rw incr128brstena : 1;
            _rw incr256brstena : 1;
            __r : 2;
            _rw desbigend      : 1;
            _rw datbigend      : 1;
            __r : 4;
            _rw deswrreqinfo   : 4;
            _rw datwrreqinfo   : 4;
            _rw desrdreqinfo   : 4;
            _rw datrdreqinfo   : 4;
        } gsbuscfg0_b;
    };

    union {
        _rw gsbuscfg1;
        struct {
            __r : 8;
            _rw pipetranslimit : 4;
            _rw en1kpage       : 1;
            __r : 19;
        } gsbuscfg1_b;
    };

    __r : 32; __r : 32;

    union {
        _rw gctl;
        struct {
            _rw dsblclkgtng      : 1;
            _rw gblhibernationen : 1;
            __r : 1;
            _rw disscramble      : 1;
            _rw scaledown        : 2;
            _rw ramclksel        : 2;
            __r : 2;
            _rw sofitpsync       : 1;
            _rw coresoftreset    : 1;
            _rw prtcapdir        : 2;
            _rw frmscldwn        : 2;
            __r : 1;
            _rw bypssetaddr      : 1;
            __r : 14;
        } gctl_b;
    };

    __r : 32;

    union {
        _rw gsts;
        struct {
            __r curmod        : 2;
            __r : 2;
            _rw buserraddrvld : 1;
            _rw csrtimeout    : 1;
            __r device_ip     : 1;
            __r host_ip       : 1;
            __r adp_ip        : 1;
            __r bc_ip         : 1;
            __r otg_ip        : 1;
            __r ssic_ip       : 1;
            __r : 8;
            __r cbelt         : 12;
        } gsts_b;
    };

    union {
        _rw guctl1;
        struct {
            _rw loa_filter_en             : 1;
            _rw ovrld_l1_susp_com         : 1;
            _rw hc_parchk_disable         : 1;
            _rw hc_errata_enable          : 1;
            _rw l1_susp_thrld_for_host    : 4;
            _rw l1_susp_thrld_en_for_host : 1;
            _rw dev_hs_nyet_bulk_spr      : 1;
            _rw resume_opmode_hs_host     : 1;
            __r : 1;
            _rw disusb2refclkgtng         : 1;
            __r : 2;
            _rw parkmode_disable_fsls     : 1;
            _rw parkmode_disable_hs       : 1;
            __r : 1;
            _rw nak_per_enh_hs            : 1;
            _rw nak_per_enh_fs            : 1;
            _rw dev_lsp_tail_lock_dis     : 1;
            _rw ip_gap_add_on             : 2;
            _rw dev_l1_exit_by_hw         : 1;
            __r : 2;
            _rw dev_trb_out_spr_ind       : 1;
            _rw tx_ipgap_linecheck_dis    : 1;
            _rw filter_se0_fsls_eop       : 1;
            __r : 1;
            _rw dev_decouple_l1l2_evt     : 1;
        } guctl1_b;
    };

    union {
        _rw gsnpsid;
    };

    __r : 32;

    union {
        _rw guid;
        struct {
            _rw userid : 32;
        } guid_b;
    };

    union { // <- host only, leaving unimplemented for now [FIXME]
        _rw guctl;
        struct {
        } guctl_b;
    };

    union {
        _rw gbuserraddrlo;
        struct {
            _rw buserraddr : 32;
        } gbuserraddrlo_b;
    };

    union {
        _rw gbuserraddrhi;
        struct {
            _rw buserraddr : 32;
        } gbuserraddrhi_b;
    };

    __r : 32; __r : 32;

    __r ghwparams[8];

    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32; __r : 32;
    __r : 32; __r : 32; __r : 32;

    // base + 0x9C here

    union {
        _rw guctl2;
        struct {
            __r : 11;
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
        _rw gusb2phycfg0;
        struct {
            _rw toutcal                  : 3;
            _rw phyif                    : 1;
            __r ulpi_utmi_sel            : 1;
            __r fsintf                   : 1;
            _rw suspendusb20             : 1;
            _rw physel                   : 1;
            _rw enblslpm                 : 1;
            _rw xcvrdly                  : 1;
            _rw usbtrdtim                : 4;
            __r : 5;
            _rw lsipd                    : 3;
            _rw lstrd                    : 3;
            _rw ovrd_fsls_disc_time      : 1;
            __r inv_sel_hsic             : 1;
            __r hsic_con_width_adj       : 2;
            _rw ulpi_lpm_with_opmode_chk : 1;
            _rw u2_freeclk_exists        : 1;
            _rw physoftrst               : 1;
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
        _rw gtxfifosiz[4];
        struct {
            _rw txfdep : 16;
            _rw txfstaddr : 16;
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
        _rw grxfifosiz[4];
        struct {
            _rw rxfdep : 16;
            _rw rxfstaddr : 16;
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
        _rw gevntadrlo0;
        struct {
            _rw evntadrlo : 32;
        } gevntadrlo0_b;
    };

    union {
        _rw gevntadrhi0;
        struct {
            _rw evntadrhi : 32;
        } gevntadrhi0_b;
    };

    union {
        _rw gevntsiz0;
        struct {
            _rw eventsiz : 16;
            __r : 15;
            _rw evntintrptmask : 1;
        } gevntsiz0_b;
    };

    union {
        _rw gevntcount0;
        struct {
            _rw evntcount : 16;
            __r : 15;
            _rw evnt_handler_busy : 1;
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

    __r ghwparams8;

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
        _rw dcfg;
        struct {
            _rw devspd : 3;
            _rw devaddr : 7;
            __r : 2;
            _rw intrnum : 5;
            _rw nump : 5;
            _rw lpmcap : 1;
            _rw ignstrmpp : 1;
            __r : 8;
        } dcfg_b;
    };

    union {
        _rw dctl;
        struct {
            __r : 1;
            _rw tstctl : 4;
            __w ulstchngreq : 4;
            __r : 7;
            _rw css : 1;
            _rw crs : 1;
            _rw l1hibernationen : 1;
            _rw keepconnect : 1;
            _rw lpm_nyet_thres : 4;
            _rw hirdthres : 5;
            __r : 1;
            _rw csftrst : 1;
            _rw run_stop : 1;
        } dctl_b;
    };

    union {
        _rw devten;
        struct {
            _rw dissconnevten : 1;
            _rw usbrstevten : 1;
            _rw connectdoneevten : 1;
            _rw ulstcngen : 1;
            _rw wkupevten : 1;
            _rw hibernationreqevten : 1;
            _rw u3l2l1suspen : 1;
            _rw softevten : 1;
            _rw l1suspen : 1;
            _rw errticerrevten : 1;
            __r : 2;
            _rw vendevtstrcvden : 1;
            __r : 1;
            _rw l1wkupevten : 1;
            __r : 1;
            _rw eccerren : 1;
            __r : 15;
        } devten_b;
    };

    union {
        _rw dsts;
        struct {
            __r connectspd : 3;
            __r soffn : 14;
            __r rxfifoempty : 1;
            __r usblnkst : 4;
            __r devctrlhlt : 1;
            __r coreidle : 1;
            __r sss : 1;
            __r rss : 1;
            __r : 2;
            _rw sre : 1;
            __r dcnrd : 1;
            __r : 2;
        } dsts_b;
    };

    union {
        _rw dgcmdpar;
        struct {
            _rw parameter : 32;
        } dgcmdpar_b;
    };

    union {
        _rw dgcmd;
        struct {
            _rw cmdtyp : 8;
            _rw cmdioc : 1;
            __r : 1;
            _rw cmdact : 1;
            __r : 1;
            __r cmdstatus : 4;
            __r : 16;
        } dgcmd_b;
    };

    __r : 32; __r : 32;

    union {
        _rw dalepena;
        struct {
            _rw usbactep : 32;
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
            _rw par2;
            struct {
                _rw parameter : 32;
            } par2_b;
        };

        union {
            _rw par1;
            struct {
                _rw parameter : 32;
            } par1_b;
        };

        union {
            _rw par0;
            struct {
                _rw parameter : 32;
            } par0_b;
        };

        union {
            _rw depcmd;
            struct {
                _rw cmdtyp : 4;
                __r : 4;
                _rw cmdioc : 1;
                __r : 1;
                _rw cmdact : 1;
                _rw hipri_forcerm : 1;
                _rw cmdstatus : 4;
                _rw commandparam : 16;
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

    union {
        _rw dev_imod0;
        struct {
            _rw device_imodi : 16;
            _rw device_imocd : 16;
        } dev_imod0_b;
    };
} *udev = (void *) (USB_BASE + 0xC700);

//--------------------------------------------------------------------------------------------------

typedef union {
    _rw val : 32;
    struct {
        _rw sig  : 8;
        _rw evt  : 5;
        __r : 3;
        _rw info : 9;
        __r : 7;
    } devt;
    struct {
        _rw sig : 1;
        _rw ep  : 5;
        _rw evt : 4;
        __r : 2;
        _rw sts : 4;
        _rw par : 16;
    } depevt;
} evt_t;

//--------------------------------------------------------------------------------------------------

enum {
    CMDTYP_RESERVED = 0,
    CMDTYP_DEPCFG,
    CMDTYP_DEPXFERCFG,
    CMDTYP_DEPGETSTATE,
    CMDTYP_DEPSSTALL,
    CMDTYP_DEPCSTALL,
    CMDTYP_DEPSTRTXFER,
    CMDTYP_DEPUPDXFER,
    CMDTYP_DEPENDXFER,
    CMDTYP_DEPSTARTCFG
} DEPCMD_CMDTYP;

enum {
    DEVT_DISCONNEVT = 0,
    DEVT_USBRST,
    DEVT_CONNECTDONE,
    DEVT_ULSTCHNG,
    DEVT_WKUPEVT,
    // DEVT_HIBRQ not implemented
    DEVT_USBSUSP = 6,
    DEVT_SOF,
    DEVT_L1SUSP,
    DEVT_ERRTICERR,
    DEVT_CMDCMPLT,
    DEVT_EVNTOVERFLOW,
    DEVT_VNDDEVTSTRCVED,
    // reserved
    DEVT_L1RESM = 14,
    // reserved
    DEVT_ECCERR = 16
} DEVT;

enum {
    // reserved
    DEPEVT_XFERCOMPLETE = 1,
    DEPEVT_XFERINPROGRESS,
    DEPEVT_XFERNOTREADY,
    // not implemented
    DEPEVT_STREAMEVT = 6,
    DEPEVT_EPCMDCMPLT
} DEPEVT;

enum {
    // reserved
    TRBCTL_NORMAL = 1,
    TRBCTL_CTL_SETUP,
    TRBCTL_CTL_STAT2,
    TRBCTL_CTL_STAT3,
    TRBCTL_CTL_DATA,
    TRBCTL_ISO_FIRST,
    TRBCTL_ISO,
    TRBCTL_LINK,
    TRBCTL_NORMAL_ZLP
} TRBCTL;

//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // __ALIF_DCD_REG_H__

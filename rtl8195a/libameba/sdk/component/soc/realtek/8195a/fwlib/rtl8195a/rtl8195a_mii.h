/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _RTL8195A_MII_H_
#define _RTL8195A_MII_H_

#include "basic_types.h"
#include "hal_api.h"


#define ETHERNET_REG_BASE     0x40050000
#define ETHERNET_MODULE_BASE  ETHERNET_REG_BASE + 0x0000
#define CPU_INTERFACE_BASE    ETHERNET_REG_BASE + 0x1300

/* Ethernet Module registers */
#define REG_RTL_MII_IDR0    0x0000  // Table 2 IDR0 (Offset 0000h-0003h, R/W)
#define REG_RTL_MII_IDR4    0x0004  // Table 3 IDR4 (Offset 0004h-0007h, R/W)
#define REG_RTL_MII_MAR0    0x0008  // Table 4 MAR0 (Offset 0008h-000bh, R/W)
#define REG_RTL_MII_MAR4    0x000C  // Table 5 MAR4 (Offset 000ch-000fh, R/W)
#define REG_RTL_MII_CR      0x0038  // Table 21 Command Register          (COM_REG, Offset 0038-003Bh, R/W)
#define REG_RTL_MII_IMRISR  0x003C  // Table 22 + Table 23
#define REG_RTL_MII_TCR     0x0040  // Table 24 Transmit Configuration Register (TC_REG, Offset 0040h-0043h, R/W)
#define REG_RTL_MII_RCR     0x0044  // Table 25 Receive Configuration Register  (RC_REG, Offset 0044h-0047h, R/W)
#define REG_RTL_MII_CTCR    0x0048  // Table 26 CPU Tag Control Register  (CPUTAG_REG, Offset 0048h-004bh, R/W)
#define REG_RTL_MII_CONFIG  0x004C  // Table 27 Configuration Register (CONFIG_REG, Offset 004ch-004fh, R/W)
#define REG_RTL_MII_CTCR1   0x0050  // Table 28 CPUTAG1 Register          (CPUTAG1_REG, Offset 0050h-0053h, R/W)
#define REG_RTL_MII_MSR     0x0058  // Table 29 Media Status Register     (MS_reg: Offset 0058h – 005bh, R/W)
#define REG_RTL_MII_MIIAR   0x005C  // Table 30 MII Access Register (MIIA_REG, Offset 005c-005fh, R/W)
#define REG_RTL_MII_VR      0x0064  // Table 32 VLAN Register (VLAN_REG, Offset 0064-0067h, R/W)
#define REG_RTL_MII_IMR0    0x00D0  // Table 50 IMR0_REG (IMR0_REG, Offset D0h-D3h)
#define REG_RTL_MII_IMR1    0x00D4  // Table 51 IMR1_REG (IMR1_REG, Offset d4h-d7h)
#define REG_RTL_MII_ISR1    0x00D8  // Table 52 ISR1 Register (ISR1_REG, Offset D8h-DBh)
#define REG_RTL_MII_INTR    0x00DC  // Table 53 Interrupt routing register (INTR_REG, Offset DCh-DFh)
#define REG_RTL_MII_CCR     0x00E4  // Table xx Clock Control Register (CLKCTL_REG, Offset E4h-E7h)

/* CPU Interface registers */
#define REG_RTL_MII_TXFDP1  0x1300  // Table 55 TxFDP1 register (TXFDP1_REG, offset 1300h-1303h)
#define REG_RTL_MII_TXCDO1  0x1304  // Table 56 TxCDO1 register (TXCDO1_REG, offset 1304h-1305h)
#define REG_RTL_MII_TXFDP2  0x1310  // Table 57 TxFDP2 register (TXFDP2_REG, offset 1310h-1313h)
#define REG_RTL_MII_TXCDO2  0x1314  // Table 58 TxCDO2 register (TXCDO2_REG, offset 1314h-1315h)
#define REG_RTL_MII_TXFDP3  0x1320  // Table 59 TxFDP3 register (TXFDP3_REG, offset 1320h-1323h)
#define REG_RTL_MII_TXCDO3  0x1324  // Table 60 TxCDO3 register (TXCDO3_REG, offset 1324h-1325h)
#define REG_RTL_MII_TXFDP4  0x1330  // Table 61 TxFDP4 register (TXFDP4_REG, offset 1330h-1333h)
#define REG_RTL_MII_TXCDO4  0x1334  // Table 62 TxCDO4 register (TXCDO4_REG, offset 1334h-1335h)
#define REG_RTL_MII_TXFDP5  0x1340  // Table 63 TxFDP5 register (TXFDP5_REG, offset 1340h-1343h)
#define REG_RTL_MII_TXCDO5  0x1344  // Table 64 TxCDO5 register (TXCDO5_REG, offset 1344h-1345h)
#define REG_RTL_MII_RXFDP2  0x1390  // Table 66 RxFDP2 register (RXFDP#_REG, offset 1390h-1393h)
#define REG_RTL_MII_RXFDP1  0x13F0  // Table 71 RxFDP1 register (RXFDP1_REG, offset 13F0h-13F3h)
#define REG_RTL_MII_RXRS1   0x13F6  // Table 73 Rx Ring Size1 register (RX_RS1_REG, offset 13F6h-13F7h)

#define REG_RTL_MII_RX_PSE1   0x142C  // Table 77 Rx_Pse_Des_Thres_1_h (RX_PSE1_REG, Offset 142ch)
#define REG_RTL_MII_ETNRXCPU1 0x1430  // Table 79 EhtrntRxCPU_Des_Num1 (ETNRXCPU1_REG, Offset 1430h-1433h)
#define REG_RTL_MII_IOCMD     0x1434  // Table 80 Ethernet_IO_CMD (ETN_IO_CMD_REG, Offset 1434h-1437h)
#define REG_RTL_MII_IOCMD1    0x1438  // Table 81 Ethernet_IO_CMD1 (IO_CMD1_REG: Offset 1438h-143bh)


#define HAL_MII_READ32(addr)         \
    HAL_READ32(ETHERNET_REG_BASE, addr)
#define HAL_MII_WRITE32(addr, value) \
    HAL_WRITE32(ETHERNET_REG_BASE, addr, value)
#define HAL_MII_READ16(addr)         \
    HAL_READ16(ETHERNET_REG_BASE, addr)
#define HAL_MII_WRITE16(addr, value) \
    HAL_WRITE16(ETHERNET_REG_BASE, addr, value)
#define HAL_MII_READ8(addr)          \
    HAL_READ8(ETHERNET_REG_BASE, addr)
#define HAL_MII_WRITE8(addr, value)  \
    HAL_WRITE8(ETHERNET_REG_BASE, addr, value)

#define  CMD_CONFIG  0x00081000

//2014-04-29 yclin (disable [27] r_en_precise_dma)
// #define CMD1_CONFIG  0x39000000
#define CMD1_CONFIG  0x31000000

// #define MAX_RX_DESC_SIZE  6
#define MAX_RX_DESC_SIZE  1
#define MAX_TX_DESC_SIZE  5

// 0058h
#define BIT_SHIFT_MSR_FORCE_SPEED_SELECT    16
#define BIT_MASK_MSR_FORCE_SPEED_SELECT     0x3
#define BIT_MSR_FORCE_SPEED_SELECT(x)(((x) & BIT_MASK_MSR_FORCE_SPEED_SELECT) << BIT_SHIFT_MSR_FORCE_SPEED_SELECT)
#define BIT_INVC_MSR_FORCE_SPEED_SELECT (~(BIT_MASK_MSR_FORCE_SPEED_SELECT << BIT_SHIFT_MSR_FORCE_SPEED_SELECT))

#define BIT_SHIFT_MSR_FORCE_SPEED_MODE_ENABLE    10
#define BIT_MASK_MSR_FORCE_SPEED_MODE_ENABLE     0x1
#define BIT_MSR_FORCE_SPEED_MODE_ENABLE(x)(((x) & BIT_MASK_MSR_FORCE_SPEED_MODE_ENABLE) << BIT_SHIFT_MSR_FORCE_SPEED_MODE_ENABLE)
#define BIT_INVC_MSR_FORCE_SPEED_MODE_ENABLE (~(BIT_MASK_MSR_FORCE_SPEED_MODE_ENABLE << BIT_SHIFT_MSR_FORCE_SPEED_MODE_ENABLE))

// 1434h
#define BIT_SHIFT_IOCMD_RXENABLE      5
#define BIT_MASK_IOCMD_RXENABLE       0x1
#define BIT_IOCMD_RXENABLE(x)(((x) & BIT_MASK_IOCMD_RXENABLE) << BIT_SHIFT_IOCMD_RXENABLE)
#define BIT_INVC_IOCMD_RXENABLE (~(BIT_MASK_IOCMD_RXENABLE << BIT_SHIFT_IOCMD_RXENABLE))

#define BIT_SHIFT_IOCMD_TXENABLE      4
#define BIT_MASK_IOCMD_TXENABLE       0x1
#define BIT_IOCMD_TXENABLE(x)(((x) & BIT_MASK_IOCMD_TXENABLE) << BIT_SHIFT_IOCMD_TXENABLE)
#define BIT_INVC_IOCMD_TXENABLE (~(BIT_MASK_IOCMD_TXENABLE << BIT_SHIFT_IOCMD_TXENABLE))

#define BIT_SHIFT_IOCMD_FIRST_DMATX_ENABLE      0
#define BIT_MASK_IOCMD_FIRST_DMATX_ENABLE       0x1
#define BIT_IOCMD_FIRST_DMATX_ENABLE(x)(((x) & BIT_MASK_IOCMD_FIRST_DMATX_ENABLE) << BIT_SHIFT_IOCMD_FIRST_DMATX_ENABLE)
#define BIT_INVC_IOCMD_FIRST_DMATX_ENABLE (~(BIT_MASK_IOCMD_FIRST_DMATX_ENABLE << BIT_SHIFT_IOCMD_FIRST_DMATX_ENABLE))

// 1438h
#define BIT_SHIFT_IOCMD1_FIRST_DMARX_ENABLE    16
#define BIT_MASK_IOCMD1_FIRST_DMARX_ENABLE     0x1
#define BIT_IOCMD1_FIRST_DMARX_ENABLE(x)(((x) & BIT_MASK_IOCMD1_FIRST_DMARX_ENABLE) << BIT_SHIFT_IOCMD1_FIRST_DMARX_ENABLE)
#define BIT_INVC_IOCMD1_FIRST_DMARX_ENABLE (~(BIT_MASK_IOCMD1_FIRST_DMARX_ENABLE << BIT_SHIFT_IOCMD1_FIRST_DMARX_ENABLE))


/**
 * 1.4.1.7 Tx command descriptor used in RL6266
 *   5 dobule words
 */
typedef struct _TX_INFO_ {
    union {
        struct {
            u32 own:1;          //31
            u32 eor:1;          //30
            u32 fs:1;           //29
            u32 ls:1;           //28
            u32 ipcs:1;         //27
            u32 l4cs:1;         //26
            u32 keep:1;         //25
            u32 blu:1;          //24
            u32 crc:1;          //23
            u32 vsel:1;         //22
            u32 dislrn:1;       //21
            u32 cputag_ipcs:1;  //20
            u32 cputag_l4cs:1;  //19
            u32 cputag_psel:1;  //18
            u32 rsvd:1;         //17
            u32 data_length:17; //0~16
        } bit;
        u32 dw; //double word
    } opts1;

    u32 addr;

    union {
        struct {
            u32 cputag:1;          //31
            u32 aspri:1;           //30
            u32 cputag_pri:3;      //27~29
            u32 tx_vlan_action:2;  //25~26
            u32 tx_pppoe_action:2; //23~24
            u32 tx_pppoe_idx:3;    //20~22
            u32 efid:1;            //19
            u32 enhance_fid:3;     //16~18
            u32 vidl:8;            // 8~15
            u32 prio:3;            // 5~7
            u32 cfi:1;             // 4
            u32 vidh:4;            // 0~3
        } bit;
        u32 dw; //double word
    } opts2;

    union {
        struct {
            u32 extspa:3;           //29~31
            u32 tx_portmask:6;      //23~28
            u32 tx_dst_stream_id:7; //16~22
            u32 rsvd:14;            // 2~15
            u32 l34keep:1;          // 1
            u32 ptp:1;              // 0
        } bit;
        u32 dw; //double word
    } opts3;

    union {
        struct {
            u32 lgsen:1;  //31
            u32 lgmss:11; //20~30
            u32 rsvd:20;  // 0~19
        } bit;
        u32 dw; //double word
    } opts4;

} TX_INFO, *PTX_INFO;

typedef struct _RX_INFO_ {
    union{
        struct{
            u32 own:1;          //31
            u32 eor:1;          //30
            u32 fs:1;           //29
            u32 ls:1;           //28
            u32 crcerr:1;       //27
            u32 ipv4csf:1;      //26
            u32 l4csf:1;        //25
            u32 rcdf:1;         //24
            u32 ipfrag:1;       //23
            u32 pppoetag:1;     //22
            u32 rwt:1;          //21
            u32 pkttype:4;      //20-17
            u32 l3routing:1;    //16
            u32 origformat:1;   //15
            u32 pctrl:1;        //14
#ifdef CONFIG_RG_JUMBO_FRAME
            u32 data_length:14; //13~0
#else
            u32 rsvd:2;         //13~12
            u32 data_length:12; //11~0
#endif
        }bit;
        u32 dw; //double word
    }opts1;

    u32 addr;

    union{
        struct{
            u32 cputag:1;               //31
            u32 ptp_in_cpu_tag_exist:1; //30
            u32 svlan_tag_exist:1;      //29
            u32 rsvd_2:2;               //27~28
            u32 pon_stream_id:7;        //20~26
            u32 rsvd_1:3;               //17~19
            u32 ctagva:1;               //16
            u32 cvlan_tag:16;           //15~0
        }bit;
        u32 dw; //double word
    }opts2;

    union{
        struct{
            u32 src_port_num:5;      //27~31
            u32 dst_port_mask:6;     //21~26
            u32 reason:8;            //13~20
            u32 internal_priority:3; //10~12
            u32 ext_port_ttl_1:5;    //5~9
            u32 rsvd:5;              //4~0
        }bit;
        u32 dw; //double word
    }opts3;
} RX_INFO, *PRX_INFO;

/**
 * GMAC_STATUS_REGS
 */
// TX/RX Descriptor Common
#define BIT_SHIFT_GMAC_DESCOWN        31
#define BIT_MASK_GMAC_DESCOWN        0x1
#define BIT_GMAC_DESCOWN(x)(((x) & BIT_MASK_GMAC_DESCOWN) << BIT_SHIFT_GMAC_DESCOWN)
#define BIT_INVC_GMAC_DESCOWN (~(BIT_MASK_GMAC_DESCOWN << BIT_SHIFT_GMAC_DESCOWN))

#define BIT_SHIFT_GMAC_RINGEND        30
#define BIT_MASK_GMAC_RINGEND        0x1
#define BIT_GMAC_RINGEND(x)(((x) & BIT_MASK_GMAC_RINGEND) << BIT_SHIFT_GMAC_RINGEND)
#define BIT_INVC_GMAC_RINGEND (~(BIT_MASK_GMAC_RINGEND << BIT_SHIFT_GMAC_RINGEND))

#define BIT_SHIFT_GMAC_FIRSTFRAG      29
#define BIT_MASK_GMAC_FIRSTFRAG      0x1
#define BIT_GMAC_FIRSTFRAG(x)(((x) & BIT_MASK_GMAC_FIRSTFRAG) << BIT_SHIFT_GMAC_FIRSTFRAG)
#define BIT_INVC_GMAC_FIRSTFRAG (~(BIT_MASK_GMAC_FIRSTFRAG << BIT_SHIFT_GMAC_FIRSTFRAG))

#define BIT_SHIFT_GMAC_LASTFRAG       28
#define BIT_MASK_GMAC_LASTFRAG       0x1
#define BIT_GMAC_LASTFRAG(x)(((x) & BIT_MASK_GMAC_LASTFRAG) << BIT_SHIFT_GMAC_LASTFRAG)
#define BIT_INVC_GMAC_LASTFRAG (~(BIT_MASK_GMAC_LASTFRAG << BIT_SHIFT_GMAC_LASTFRAG))

// TX Descriptor opts1
#define BIT_SHIFT_GMAC_IPCS           27
#define BIT_MASK_GMAC_IPCS           0x1
#define BIT_GMAC_IPCS(x)(((x) & BIT_MASK_GMAC_IPCS) << BIT_SHIFT_GMAC_IPCS)
#define BIT_INVC_GMAC_IPCS (~(BIT_MASK_GMAC_IPCS << BIT_SHIFT_GMAC_IPCS))

#define BIT_SHIFT_GMAC_L4CS           26
#define BIT_MASK_GMAC_L4CS           0x1
#define BIT_GMAC_L4CS(x)(((x) & BIT_MASK_GMAC_L4CS) << BIT_SHIFT_GMAC_L4CS)
#define BIT_INVC_GMAC_L4CS (~(BIT_MASK_GMAC_L4CS << BIT_SHIFT_GMAC_L4CS))

#define BIT_SHIFT_GMAC_KEEP           25
#define BIT_MASK_GMAC_KEEP           0x1
#define BIT_GMAC_KEEP(x)(((x) & BIT_MASK_GMAC_KEEP) << BIT_SHIFT_GMAC_KEEP)
#define BIT_INVC_GMAC_KEEP (~(BIT_MASK_GMAC_KEEP << BIT_SHIFT_GMAC_KEEP))

#define BIT_SHIFT_GMAC_BLU            24
#define BIT_MASK_GMAC_BLU            0x1
#define BIT_GMAC_BLU(x)(((x) & BIT_MASK_GMAC_BLU) << BIT_SHIFT_GMAC_BLU)
#define BIT_INVC_GMAC_BLU (~(BIT_MASK_GMAC_BLU << BIT_SHIFT_GMAC_BLU))

#define BIT_SHIFT_GMAC_TXCRC          23
#define BIT_MASK_GMAC_TXCRC          0x1
#define BIT_GMAC_TXCRC(x)(((x) & BIT_MASK_GMAC_TXCRC) << BIT_SHIFT_GMAC_TXCRC)
#define BIT_INVC_GMAC_TXCRC (~(BIT_MASK_GMAC_TXCRC << BIT_SHIFT_GMAC_TXCRC))

#define BIT_SHIFT_GMAC_VSEL           22
#define BIT_MASK_GMAC_VSEL           0x1
#define BIT_GMAC_VSEL(x)(((x) & BIT_MASK_GMAC_VSEL) << BIT_SHIFT_GMAC_VSEL)
#define BIT_INVC_GMAC_VSEL (~(BIT_MASK_GMAC_VSEL << BIT_SHIFT_GMAC_VSEL))

#define BIT_SHIFT_GMAC_DISLRN         21
#define BIT_MASK_GMAC_DISLRN         0x1
#define BIT_GMAC_DISLRN(x)(((x) & BIT_MASK_GMAC_DISLRN) << BIT_SHIFT_GMAC_DISLRN)
#define BIT_INVC_GMAC_DISLRN (~(BIT_MASK_GMAC_DISLRN << BIT_SHIFT_GMAC_DISLRN))

#define BIT_SHIFT_GMAC_CPUTAG_IPCS    20
#define BIT_MASK_GMAC_CPUTAG_IPCS    0x1
#define BIT_GMAC_CPUTAG_IPCS(x)(((x) & BIT_MASK_GMAC_CPUTAG_IPCS) << BIT_SHIFT_GMAC_CPUTAG_IPCS)
#define BIT_INVC_GMAC_CPUTAG_IPCS (~(BIT_MASK_GMAC_CPUTAG_IPCS << BIT_SHIFT_GMAC_CPUTAG_IPCS))

#define BIT_SHIFT_GMAC_CPUTAG_L4CS    19
#define BIT_MASK_GMAC_CPUTAG_L4CS    0x1
#define BIT_GMAC_CPUTAG_L4CS(x)(((x) & BIT_MASK_GMAC_CPUTAG_L4CS) << BIT_SHIFT_GMAC_CPUTAG_L4CS)
#define BIT_INVC_GMAC_CPUTAG_L4CS (~(BIT_MASK_GMAC_CPUTAG_L4CS << BIT_SHIFT_GMAC_CPUTAG_L4CS))

#define BIT_SHIFT_GMAC_CPUTAG_PSEL    18
#define BIT_MASK_GMAC_CPUTAG_PSEL    0x1
#define BIT_GMAC_CPUTAG_PSEL(x)(((x) & BIT_MASK_GMAC_CPUTAG_PSEL) << BIT_SHIFT_GMAC_CPUTAG_PSEL)
#define BIT_INVC_GMAC_CPUTAG_PSEL (~(BIT_MASK_GMAC_CPUTAG_PSEL << BIT_SHIFT_GMAC_CPUTAG_PSEL))


#if 0
enum RE8670_STATUS_REGS
{
    /*TX/RX share */
    DescOwn     = (1 << 31), /* Descriptor is owned by NIC */
    RingEnd     = (1 << 30), /* End of descriptor ring */
    FirstFrag       = (1 << 29), /* First segment of a packet */
    LastFrag        = (1 << 28), /* Final segment of a packet */

    /*Tx descriptor opt1*/
    IPCS        = (1 << 27),
    L4CS        = (1 << 26),
    KEEP        = (1 << 25),
    BLU         = (1 << 24),
    TxCRC       = (1 << 23),
    VSEL        = (1 << 22),
    DisLrn      = (1 << 21),
    CPUTag_ipcs     = (1 << 20),
    CPUTag_l4cs = (1 << 19),

    /*Tx descriptor opt2*/
    CPUTag      = (1 << 31),
    aspri       = (1 << 30),
    CPRI        = (1 << 27),
    TxVLAN_int  = (0 << 25),  //intact
    TxVLAN_ins  = (1 << 25),  //insert
    TxVLAN_rm   = (2 << 25),  //remove
    TxVLAN_re   = (3 << 25),  //remark
    //TxPPPoEAct    = (1 << 23),
    TxPPPoEAct  = 23,
    //TxPPPoEIdx    = (1 << 20),
    TxPPPoEIdx  = 20,
    Efid            = (1 << 19),
    //Enhan_Fid = (1 << 16),
    Enhan_Fid   = 16,
    /*Tx descriptor opt3*/
    SrcExtPort  = 29,
    TxDesPortM  = 23,
    TxDesStrID  = 16,
    TxDesVCM    = 0,
    /*Tx descriptor opt4*/
    /*Rx descriptor  opt1*/
    CRCErr  = (1 << 27),
    IPV4CSF     = (1 << 26),
    L4CSF       = (1 << 25),
    RCDF        = (1 << 24),
    IP_FRAG     = (1 << 23),
    PPPoE_tag   = (1 << 22),
    RWT         = (1 << 21),
    PktType     = (1 << 17),
    RxProtoIP   = 1,
    RxProtoPPTP = 2,
    RxProtoICMP = 3,
    RxProtoIGMP = 4,
    RxProtoTCP  = 5,   
    RxProtoUDP  = 6,
    RxProtoIPv6 = 7,
    RxProtoICMPv6   = 8,
    RxProtoTCPv6    = 9,
    RxProtoUDPv6    = 10,
    L3route     = (1 << 16),
    OrigFormat  = (1 << 15),
    PCTRL       = (1 << 14),
    /*Rx descriptor opt2*/
    PTPinCPU    = (1 << 30),
    SVlanTag        = (1 << 29),
    /*Rx descriptor opt3*/
    SrcPort     = (1 << 27),
    DesPortM    = (1 << 21),
    Reason      = (1 << 13),
    IntPriority = (1 << 10),
    ExtPortTTL  = (1 << 5),
};

enum _DescStatusBit {
    DescOwn     = (1 << 31), /* Descriptor is owned by NIC */
    RingEnd     = (1 << 30), /* End of descriptor ring */
    FirstFrag   = (1 << 29), /* First segment of a packet */
    LastFrag    = (1 << 28), /* Final segment of a packet */

    /* Tx private */
    LargeSend   = (1 << 27), /* TCP Large Send Offload (TSO) */
    MSSShift    = 16,        /* MSS value position */
    MSSMask     = 0xfff,     /* MSS value + LargeSend bit: 12 bits */
    IPCS        = (1 << 18), /* Calculate IP checksum */
    UDPCS       = (1 << 17), /* Calculate UDP/IP checksum */
    TCPCS       = (1 << 16), /* Calculate TCP/IP checksum */
    TxVlanTag   = (1 << 17), /* Add VLAN tag */

    /* Rx private */
    PID1        = (1 << 18), /* Protocol ID bit 1/2 */
    PID0        = (1 << 17), /* Protocol ID bit 2/2 */

#define RxProtoUDP  (PID1)
#define RxProtoTCP  (PID0)
#define RxProtoIP   (PID1 | PID0)
#define RxProtoMask RxProtoIP

    IPFail      = (1 << 16), /* IP checksum failed */
    UDPFail     = (1 << 15), /* UDP/IP checksum failed */
    TCPFail     = (1 << 14), /* TCP/IP checksum failed */
    RxVlanTag   = (1 << 16), /* VLAN tag available */
};
#endif

typedef struct _PHY_MODE_INFO_ {
    u8 PhyAddress;
    u8 PhyMode;
    u8 PhyInterface;
} PHY_MODE_INFO, *PPHY_MODE_INFO;

typedef enum _PHY_MODE_SWITCH_ {
    PHY_MODE_DISABLE = 0,
    PHY_MODE_ENABLE  = 1
} PHY_MODE_SWITCH, *PPHY_MODE_SWITCH;

typedef enum _PHY_INTERFACE_SELECT_ {
    PHY_INTERFACE_ONE_WORKS  = 0,
    PHY_INTERFACE_ZERO_WORKS = 1
} PHY_INTERFACE_SELECT, *PPHY_INTERFACE_SELECT;

typedef enum _GMAC_MSR_FORCE_SPEED_ {
    FORCE_SPD_100M = 0,
    FORCE_SPD_10M  = 1,
    FORCE_SPD_GIGA = 2,
    NO_FORCE_SPD   = 3
}GMAC_MSR_FORCE_SPEED, *PGMAC_MSR_FORCE_SPEED;

// typedef enum _GMAC_INTERRUPT_MASK_ {
//     GMAC_IMR_ROK      = BIT0,
//     GMAC_IMR_CNT_WRAP = BIT1,
//     GMAC_IMR_RER_RUNT = BIT2,
//     // BIT3 Reserved
//     GMAC_IMR_RER_OVF  = BIT4,
//     GMAC_IMR_RDU      = BIT5,
//     GMAC_IMR_TOK_TI   = BIT6,
//     GMAC_IMR_TER      = BIT7,
//     GMAC_IMR_LINKCHG  = BIT8,
//     GMAC_IMR_TDU      = BIT9,
//     GMAC_IMR_SWINT    = BIT10,
//     GMAC_IMR_RDU2     = BIT11,
//     GMAC_IMR_RDU3     = BIT12,
//     GMAC_IMR_RDU4     = BIT13,
//     GMAC_IMR_RDU5     = BIT14,
//     GMAC_IMR_RDU6     = BIT15,
// } GMAC_INTERRUPT_MASK, *PGMAC_INTERRUPT_MASK;

typedef enum _GMAC_INTERRUPT_MASK_ {
    GMAC_IMR_ROK      = BIT16,
    GMAC_IMR_CNT_WRAP = BIT17,
    GMAC_IMR_RER_RUNT = BIT18,
    // BIT19 Reserved
    GMAC_IMR_RER_OVF  = BIT20,
    GMAC_IMR_RDU      = BIT21,
    GMAC_IMR_TOK_TI   = BIT22,
    GMAC_IMR_TER      = BIT23,
    GMAC_IMR_LINKCHG  = BIT24,
    GMAC_IMR_TDU      = BIT25,
    GMAC_IMR_SWINT    = BIT26,
    GMAC_IMR_RDU2     = BIT27,
    GMAC_IMR_RDU3     = BIT28,
    GMAC_IMR_RDU4     = BIT29,
    GMAC_IMR_RDU5     = BIT30,
    GMAC_IMR_RDU6     = BIT31,
} GMAC_INTERRUPT_MASK, *PGMAC_INTERRUPT_MASK;

typedef enum _GMAC_INTERRUPT_STATUS_ {
    GMAC_ISR_ROK      = BIT0,
    GMAC_ISR_CNT_WRAP = BIT1,
    GMAC_ISR_RER_RUNT = BIT2,
    // BIT3 Reserved
    GMAC_ISR_RER_OVF  = BIT4,
    GMAC_ISR_RDU      = BIT5,
    GMAC_ISR_TOK_TI   = BIT6,
    GMAC_ISR_TER      = BIT7,
    GMAC_ISR_LINKCHG  = BIT8,
    GMAC_ISR_TDU      = BIT9,
    GMAC_ISR_SWINT    = BIT10,
    GMAC_ISR_RDU2     = BIT11,
    GMAC_ISR_RDU3     = BIT12,
    GMAC_ISR_RDU4     = BIT13,
    GMAC_ISR_RDU5     = BIT14,
    GMAC_ISR_RDU6     = BIT15,
} GMAC_INTERRUPT_STATUS, *PGMAC_INTERRUPT_STATUS;

typedef enum _GMAC_TX_VLAN_ACTION_ {
    INTACT			= 0,
    INSERT_VLAN_HDR = 1,
    REMOVE_VLAN_HDR = 2,
    REMARKING_VID   = 3
}GMAC_TX_VLAN_ACTION, *PGMAC_TX_VLAN_ACTION;

typedef enum _GMAC_RX_PACKET_TYPE_ {
    TYPE_ETHERNET	= 0,
    TYPE_IPV4		= 1,
    TYPE_IPV4_PPTP	= 2,
    TYPE_IPV4_ICMP	= 3,
	TYPE_IPV4_IGMP	= 4,
	TYPE_IPV4_TCP	= 5,
	TYPE_IPV4_UDP	= 6,
	TYPE_IPV6		= 7,
	TYPE_ICMPV6		= 8,
	TYPE_IPV6_TCP	= 9,
	TYPE_IPV6_UDP	= 10	
}GMAC_RX_PACKET_TYPE, *PGMAC_RX_PACKET_TYPE;


/*

// Memory Map of DW_apb_ssi
#define REG_DW_SSI_CTRLR0          0x00  // 16 bits
#define REG_DW_SSI_CTRLR1          0x04  // 16 bits
#define REG_DW_SSI_SSIENR          0x08  //  1 bit
#define REG_DW_SSI_RX_SAMPLE_DLY   0xF0  //  8 bits
#define REG_DW_SSI_RSVD_0          0xF4  // 32 bits
#define REG_DW_SSI_RSVD_1          0xF8  // 32 bits
#define REG_DW_SSI_RSVD_2          0xFC  // 32 bits

// CTRLR0          0x00  // 16 bits, 6.2.1
// DFS Reset Value: 0x7
#define BIT_SHIFT_CTRLR0_DFS       0
#define BIT_MASK_CTRLR0_DFS        0xF
#define BIT_CTRLR0_DFS(x)(((x) & BIT_MASK_CTRLR0_DFS) << BIT_SHIFT_CTRLR0_DFS)
#define BIT_INVC_CTRLR0_DFS (~(BIT_MASK_CTRLR0_DFS << BIT_SHIFT_CTRLR0_DFS))

#define BIT_SHIFT_CTRLR0_FRF       4
#define BIT_MASK_CTRLR0_FRF        0x3
#define BIT_CTRLR0_FRF(x)(((x) & BIT_MASK_CTRLR0_FRF) << BIT_SHIFT_CTRLR0_FRF)
#define BIT_INVC_CTRLR0_FRF (~(BIT_MASK_CTRLR0_FRF << BIT_SHIFT_CTRLR0_FRF))

#define BIT_SHIFT_CTRLR0_SCPH      6
#define BIT_MASK_CTRLR0_SCPH       0x1
#define BIT_CTRLR0_SCPH(x)(((x) & BIT_MASK_CTRLR0_SCPH) << BIT_SHIFT_CTRLR0_SCPH)
#define BIT_INVC_CTRLR0_SCPH (~(BIT_MASK_CTRLR0_SCPH << BIT_SHIFT_CTRLR0_SCPH))

// CTRLR1          0x04  // 16 bits
#define BIT_SHIFT_CTRLR1_NDF    0
#define BIT_MASK_CTRLR1_NDF     0xFFFF
#define BIT_CTRLR1_NDF(x)(((x) & BIT_MASK_CTRLR1_NDF) << BIT_SHIFT_CTRLR1_NDF)
#define BIT_INVC_CTRLR1_NDF (~(BIT_MASK_CTRLR1_NDF << BIT_SHIFT_CTRLR1_NDF))

// TXFLTR          0x18        // Variable Length
#define BIT_SHIFT_TXFTLR_TFT    0
#define BIT_MASK_TXFTLR_TFT     0x3F  // (TX_ABW-1):0
#define BIT_TXFTLR_TFT(x)(((x) & BIT_MASK_TXFTLR_TFT) << BIT_SHIFT_TXFTLR_TFT)
#define BIT_INVC_TXFTLR_TFT (~(BIT_MASK_TXFTLR_TFT << BIT_SHIFT_TXFTLR_TFT))

// TXFLR           0x20  // see     [READ ONLY]
#define BIT_MASK_TXFLR_TXTFL    0x7F  // (TX_ABW):0

// RXFLR           0x24  // see     [READ ONLY]
#define BIT_MASK_RXFLR_RXTFL    0x7F  // (RX_ABW):0

// SR              0x28  //  7 bits [READ ONLY]
#define BIT_SR_BUSY  BIT0
#define BIT_SR_TFNF  BIT1
#define BIT_SR_TFE   BIT2
#define BIT_SR_RFNE  BIT3
#define BIT_SR_RFF   BIT4
#define BIT_SR_TXE   BIT5
#define BIT_SR_DCOL  BIT6

#define BIT_IMR_TXEIM  BIT0
#define BIT_IMR_TXOIM  BIT1
#define BIT_IMR_RXUIM  BIT2
#define BIT_IMR_RXOIM  BIT3
#define BIT_IMR_RXFIM  BIT4
#define BIT_IMR_MSTIM  BIT5

// ISR             0x30  //  6 bits [READ ONLY]
#define BIT_ISR_TXEIS  BIT0
#define BIT_ISR_TXOIS  BIT1
#define BIT_ISR_RXUIS  BIT2
#define BIT_ISR_RXOIS  BIT3
#define BIT_ISR_RXFIS  BIT4
#define BIT_ISR_MSTIS  BIT5

*/

BOOL
HalMiiGmacInitRtl8195a(
        IN VOID *Data
        );

BOOL
HalMiiInitRtl8195a(
        IN VOID *Data
        );

BOOL
HalMiiGmacResetRtl8195a(
        IN VOID *Data
        );

BOOL
HalMiiGmacEnablePhyModeRtl8195a(
        IN VOID *Data
        );

u32
HalMiiGmacXmitRtl8195a(
        IN VOID *Data
        );

VOID
HalMiiGmacCleanTxRingRtl8195a(
        IN VOID *Data
        );

VOID
HalMiiGmacFillTxInfoRtl8195a(
        IN VOID *Data
        );

VOID
HalMiiGmacFillRxInfoRtl8195a(
        IN VOID *Data
        );

VOID
HalMiiGmacTxRtl8195a(
        IN VOID *Data
        );

VOID
HalMiiGmacRxRtl8195a(
        IN VOID *Data
        );

VOID
HalMiiGmacSetDefaultEthIoCmdRtl8195a(
        IN VOID *Data
        );

VOID
HalMiiGmacInitIrqRtl8195a(
        IN VOID *Data
        );

u32
HalMiiGmacGetInterruptStatusRtl8195a(
		VOID
        );

VOID
HalMiiGmacClearInterruptStatusRtl8195a(
        u32 IsrStatus
        );

#endif


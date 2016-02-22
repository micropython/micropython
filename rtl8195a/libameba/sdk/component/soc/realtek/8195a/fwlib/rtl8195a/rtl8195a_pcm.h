/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _RTL8195A_PCM_H_
#define _RTL8195A_PCM_H_

#include "basic_types.h"
#include "hal_api.h"

#define HAL_PCMX_READ32(PcmIndex, addr)     \
                HAL_READ32(PCM0_REG_BASE+ (PcmIndex*PCM1_REG_OFF), addr)
#define HAL_PCMX_WRITE32(PcmIndex, addr, value)     \
                HAL_WRITE32((PCM0_REG_BASE+ (PcmIndex*PCM1_REG_OFF)), addr, value)

#define REG_PCM_TRXBSA_OFF     0x004
#define REG_PCM_CTL            0x000
#define REG_PCM_CHCNR03        0x004
#define REG_PCM_TSR03          0x008
#define REG_PCM_BSIZE03        0x00C

#define REG_PCM_CH0TXBSA       0x010
#define REG_PCM_CH1TXBSA       0x014
#define REG_PCM_CH2TXBSA       0x018
#define REG_PCM_CH3TXBSA       0x01c
#define REG_PCM_CH0RXBSA       0x020
#define REG_PCM_CH1RXBSA       0x024
#define REG_PCM_CH2RXBSA       0x028
#define REG_PCM_CH3RXBSA       0x02c

#define REG_PCM_IMR03          0x030
#define REG_PCM_ISR03          0x034

#define REG_PCM_CHCNR47        0x038
#define REG_PCM_TSR47          0x03c
#define REG_PCM_BSIZE47        0x040
#define REG_PCM_CH4TXBSA       0x044
#define REG_PCM_CH5TXBSA       0x048
#define REG_PCM_CH6TXBSA       0x04c
#define REG_PCM_CH7TXBSA       0x050
#define REG_PCM_CH4RXBSA       0x054
#define REG_PCM_CH5RXBSA       0x058
#define REG_PCM_CH6RXBSA       0x05c
#define REG_PCM_CH7RXBSA       0x060

#define REG_PCM_IMR47          0x064
#define REG_PCM_ISR47          0x068

#define REG_PCM_CHCNR811       0x06c
#define REG_PCM_TSR811         0x070
#define REG_PCM_BSIZE811       0x074
#define REG_PCM_CH8TXBSA       0x078
#define REG_PCM_CH9TXBSA       0x07c
#define REG_PCM_CH10TXBSA      0x080
#define REG_PCM_CH11TXBSA      0x084
#define REG_PCM_CH8RXBSA       0x088
#define REG_PCM_CH9RXBSA       0x08c
#define REG_PCM_CH10RXBSA      0x090
#define REG_PCM_CH11RXBSA      0x094

#define REG_PCM_IMR811         0x098
#define REG_PCM_ISR811         0x09c

#define REG_PCM_CHCNR1215      0x0a0
#define REG_PCM_TSR1215        0x0a4
#define REG_PCM_BSIZE1215      0x0a8
#define REG_PCM_CH12TXBSA      0x0ac
#define REG_PCM_CH13TXBSA      0x0b0
#define REG_PCM_CH14TXBSA      0x0b4
#define REG_PCM_CH15TXBSA      0x0b8
#define REG_PCM_CH12RXBSA      0x0bc
#define REG_PCM_CH13RXBSA      0x0c0
#define REG_PCM_CH14RXBSA      0x0c4
#define REG_PCM_CH15RXBSA      0x0c8

#define REG_PCM_IMR1215        0x0cc
#define REG_PCM_ISR1215        0x0d0

#define REG_PCM_INTMAP         0x0d4
#define REG_PCM_WTSR03         0x0d8
#define REG_PCM_WTSR47         0x0dc

#define REG_PCM_RX_BUFOW       0x0e0

/* template 
#define BIT_SHIFT_CTLX_ 7
#define BIT_MASK_CTLX_  0x1
#define BIT_CTLX_(x) (((x) & BIT_MASK_CTLX_) << BIT_SHIFT_CTLX_)
#define BIT_INV_CTLX_ (~(BIT_MASK_CTLX_ << BIT_SHIFT_CTLX_))
*/
#define BIT_SHIFT_CTLX_SLAVE_SEL 8
#define BIT_MASK_CTLX_SLAVE_SEL  0x1
#define BIT_CTLX_SLAVE_SEL(x) (((x) & BIT_MASK_CTLX_SLAVE_SEL) << BIT_SHIFT_CTLX_SLAVE_SEL)
#define BIT_INV_CTLX_SLAVE_SEL (~(BIT_MASK_CTLX_SLAVE_SEL << BIT_SHIFT_CTLX_SLAVE_SEL))

#define BIT_SHIFT_CTLX_FSINV 9
#define BIT_MASK_CTLX_FSINV 0x1
#define BIT_CTLX_FSINV(x) (((x) & BIT_MASK_CTLX_FSINV) << BIT_SHIFT_CTLX_FSINV)
#define BIT_INV_CTLX_FSINV (~(BIT_MASK_CTLX_FSINV << BIT_SHIFT_CTLX_FSINV))

#define BIT_SHIFT_CTLX_PCM_EN 12
#define BIT_MASK_CTLX_PCM_EN  0x1
#define BIT_CTLX_PCM_EN(x) (((x) & BIT_MASK_CTLX_PCM_EN) << BIT_SHIFT_CTLX_PCM_EN)
#define BIT_INV_CTLX_PCM_EN (~(BIT_MASK_CTLX_PCM_EN << BIT_SHIFT_CTLX_PCM_EN))

#define BIT_SHIFT_CTLX_LINEARMODE 13
#define BIT_MASK_CTLX_LINEARMODE  0x1
#define BIT_CTLX_LINEARMODE(x) (((x) & BIT_MASK_CTLX_LINEARMODE) << BIT_SHIFT_CTLX_LINEARMODE)
#define BIT_INV_CTLX_LINEARMODE (~(BIT_MASK_CTLX_LINEARMODE << BIT_SHIFT_CTLX_LINEARMODE))

#define BIT_SHIFT_CTLX_LOOP_BACK 14
#define BIT_MASK_CTLX_LOOP_BACK  0x1
#define BIT_CTLX_LOOP_BACK(x) (((x) & BIT_MASK_CTLX_LOOP_BACK) << BIT_SHIFT_CTLX_LOOP_BACK)
#define BIT_INV_CTLX_LOOP_BACK (~(BIT_MASK_CTLX_LOOP_BACK << BIT_SHIFT_CTLX_LOOP_BACK))

#define BIT_SHIFT_CTLX_ENDIAN_SWAP 17
#define BIT_MASK_CTLX_ENDIAN_SWAP  0x1
#define BIT_CTLX_ENDIAN_SWAP(x) (((x) & BIT_MASK_CTLX_ENDIAN_SWAP) << BIT_SHIFT_CTLX_ENDIAN_SWAP)
#define BIT_INV_CTLX_ENDIAN_SWAP (~(BIT_MASK_CTLX_ENDIAN_SWAP << BIT_SHIFT_CTLX_ENDIAN_SWAP))

#define BIT_SHIFT_CHCNR03_CH0RE 24
#define BIT_MASK_CHCNR03_CH0RE  0x1
#define BIT_CHCNR03_CH0RE(x) (((x) & BIT_MASK_CHCNR03_CH0RE) << BIT_SHIFT_CHCNR03_CH0RE)
#define BIT_INV_CHCNR03_CH0RE (~(BIT_MASK_CHCNR03_CH0RE << BIT_SHIFT_CHCNR03_CH0RE))

#define BIT_SHIFT_CHCNR03_CH0TE 25
#define BIT_MASK_CHCNR03_CH0TE  0x1
#define BIT_CHCNR03_CH0TE(x) (((x) & BIT_MASK_CHCNR03_CH0TE) << BIT_SHIFT_CHCNR03_CH0TE)
#define BIT_INV_CHCNR03_CH0TE (~(BIT_MASK_CHCNR03_CH0TE << BIT_SHIFT_CHCNR03_CH0TE))

#define BIT_SHIFT_CHCNR03_CH1RE 16
#define BIT_MASK_CHCNR03_CH1RE  0x1
#define BIT_CHCNR03_CH1RE(x) (((x) & BIT_MASK_CHCNR03_CH1RE) << BIT_SHIFT_CHCNR03_CH1RE)
#define BIT_INV_CHCNR03_CH1RE (~(BIT_MASK_CHCNR03_CH1RE << BIT_SHIFT_CHCNR03_CH1RE))

#define BIT_SHIFT_CHCNR03_CH1TE 17
#define BIT_MASK_CHCNR03_CH1TE  0x1
#define BIT_CHCNR03_CH1TE(x) (((x) & BIT_MASK_CHCNR03_CH1TE) << BIT_SHIFT_CHCNR03_CH1TE)
#define BIT_INV_CHCNR03_CH1TE (~(BIT_MASK_CHCNR03_CH1TE << BIT_SHIFT_CHCNR03_CH1TE))

#define BIT_SHIFT_CHCNR03_CH2RE 8
#define BIT_MASK_CHCNR03_CH2RE  0x1
#define BIT_CHCNR03_CH2RE(x) (((x) & BIT_MASK_CHCNR03_CH2RE) << BIT_SHIFT_CHCNR03_CH2RE)
#define BIT_INV_CHCNR03_CH2RE (~(BIT_MASK_CHCNR03_CH2RE << BIT_SHIFT_CHCNR03_CH2RE))

#define BIT_SHIFT_CHCNR03_CH2TE 9
#define BIT_MASK_CHCNR03_CH2TE  0x1
#define BIT_CHCNR03_CH2TE(x) (((x) & BIT_MASK_CHCNR03_CH2TE) << BIT_SHIFT_CHCNR03_CH2TE)
#define BIT_INV_CHCNR03_CH2TE (~(BIT_MASK_CHCNR03_CH2TE << BIT_SHIFT_CHCNR03_CH2TE))

#define BIT_SHIFT_CHCNR03_CH3RE 0
#define BIT_MASK_CHCNR03_CH3RE  0x1
#define BIT_CHCNR03_CH3RE(x) (((x) & BIT_MASK_CHCNR03_CH3RE) << BIT_SHIFT_CHCNR03_CH3RE)
#define BIT_INV_CHCNR03_CH3RE (~(BIT_MASK_CHCNR03_CH3RE << BIT_SHIFT_CHCNR03_CH3RE))

#define BIT_SHIFT_CHCNR03_CH3TE 1
#define BIT_MASK_CHCNR03_CH3TE  0x1
#define BIT_CHCNR03_CH3TE(x) (((x) & BIT_MASK_CHCNR03_CH3TE) << BIT_SHIFT_CHCNR03_CH3TE)
#define BIT_INV_CHCNR03_CH3TE (~(BIT_MASK_CHCNR03_CH3TE << BIT_SHIFT_CHCNR03_CH3TE))

#define BIT_SHIFT_CHCNR03_CH0MUA 26
#define BIT_MASK_CHCNR03_CH0MUA  0x1
#define BIT_CHCNR03_CH0MUA(x) (((x) & BIT_MASK_CHCNR03_CH0MUA) << BIT_SHIFT_CHCNR03_CH0MUA)
#define BIT_INV_CHCNR03_CH0MUA (~(BIT_MASK_CHCNR03_CH0MUA << BIT_SHIFT_CHCNR03_CH0MUA))

#define BIT_SHIFT_CHCNR03_CH0BAND 27
#define BIT_MASK_CHCNR03_CH0BAND  0x1
#define BIT_CHCNR03_CH0BAND(x) (((x) & BIT_MASK_CHCNR03_CH0BAND) << BIT_SHIFT_CHCNR03_CH0BAND)
#define BIT_INV_CHCNR03_CH0BAND (~(BIT_MASK_CHCNR03_CH0BAND << BIT_SHIFT_CHCNR03_CH0BAND))

#define BIT_SHIFT_TSR03_CH0TSA 24
#define BIT_MASK_TSR03_CH0TSA  0x1F
#define BIT_TSR03_CH0TSA(x) (((x) & BIT_MASK_TSR03_CH0TSA) << BIT_SHIFT_TSR03_CH0TSA)
#define BIT_INV_TSR03_CH0TSA (~(BIT_MASK_TSR03_CH0TSA << BIT_SHIFT_TSR03_CH0TSA))

#define BIT_SHIFT_BSIZE03_CH0BSIZE 24
#define BIT_MASK_BSIZE03_CH0BSIZE  0xFF
#define BIT_BSIZE03_CH0BSIZE(x) (((x) & BIT_MASK_BSIZE03_CH0BSIZE) << BIT_SHIFT_BSIZE03_CH0BSIZE)
#define BIT_INV_BSIZE03_CH0BSIZE (~(BIT_MASK_BSIZE03_CH0BSIZE << BIT_SHIFT_BSIZE03_CH0BSIZE))

typedef struct _PCM_CTL_REG_ {
    u32                    FCNT           :8; // Bit 0-7
    u32                    SlaveMode      :1; // Bit 8
    u32                    FsInv          :1; // Bit 9 
    u32                    Rsvd10to11     :1; // Bit 10-11
    u32                    Pcm_En         :1; // Bit 12
    u32                    LinearMode     :1; // Bit 13
    u32                    LoopBack       :1; // Bit 14
    u32                    Rsvd15to16     :2; // Bit 15-16
    u32                    EndianSwap     :1; // Bit 17
    u32                    Rsvd18to31    :14; // Bit 18-31
} PCM_CTL_REG, *PPCM_CTL_REG;



typedef struct _PCM_CHCNR03_REG_ {
    u32                      CH3RE     :1; // Bit 0
    u32                      CH3TE     :1; // Bit 1
    u32                      CH3MuA    :1; // Bit 2
    u32                      CH3Band   :1; // Bit 3
    u32                      CH3SlicSel:4; // Bit 4-7
    u32                      CH2RE     :1; // Bit 8
    u32                      CH2TE     :1; // Bit 9
    u32                      CH2MuA    :1; // Bit 10
    u32                      CH2Band   :1; // Bit 11
    u32                      CH2SlicSel:4; // Bit 12-15
    u32                      CH1RE     :1; // Bit 16
    u32                      CH1TE     :1; // Bit 17
    u32                      CH1MuA    :1; // Bit 18
    u32                      CH1Band   :1; // Bit 19
    u32                      CH1SlicSel:4; // Bit 20-23
    u32                      CH0RE     :1; // Bit 24
    u32                      CH0TE     :1; // Bit 25
    u32                      CH0MuA    :1; // Bit 26
    u32                      CH0Band   :1; // Bit 27
    u32                      CH0SlicSel:4; // Bit 28-31
}PCM_CHCNR03_REG, *PPCM_CHCNR03_REG;

typedef struct _PCM_TSR03_REG_ {
    u32                      CH3TSA    :5; // Bit 0-4
    u32                      Rsvd5to7  :3; // Bit 5-7
    u32                      CH2TSA    :5; // Bit 8-12
    u32                      Rsvd13to15:3; // Bit 13-15
    u32                      CH1TSA    :5; // Bit 16-20
    u32                      Rsvd21to23:3; // Bit 21-23
    u32                      CH0TSA    :5; // Bit 24-28
    u32                      Rsvd29to31:3; // Bit 29-31
}PCM_TSR03_REG, *PPCM_TSR03_REG;

typedef struct _PCM_BSIZE03_REG_ {
    u32                      CH3BSize  :8; // Bit 0-7
    u32                      CH2BSize  :8; // Bit 8-15
    u32                      CH1BSize  :8; // Bit 16-23
    u32                      CH0BSize  :8; // Bit 24-31
}PCM_BSIZE03_REG, *PPCM_BSIZE03_REG;

typedef struct _PCM_ISR03_REG_ {
    u32                      CH3RXP1UA :1; // Bit 0
    u32                      CH3RXP0UA :1; // Bit 1
    u32                      CH3TXP1UA :1; // Bit 2
    u32                      CH3TXP0UA :1; // Bit 3
    u32                      CH3RXP1IP :1; // Bit 4
    u32                      CH3RXP0IP :1; // Bit 5
    u32                      CH3TXP1IP :1; // Bit 6
    u32                      CH3TXP0IP :1; // Bit 7
    u32                      CH2RXP1UA :1; // Bit 8
    u32                      CH2RXP0UA :1; // Bit 9
    u32                      CH2TXP1UA :1; // Bit 10
    u32                      CH2TXP0UA :1; // Bit 11
    u32                      CH2RXP1IP :1; // Bit 12
    u32                      CH2RXP0IP :1; // Bit 13
    u32                      CH2TXP1IP :1; // Bit 14
    u32                      CH2TXP0IP :1; // Bit 15
    u32                      CH1RXP1UA :1; // Bit 16
    u32                      CH1RXP0UA :1; // Bit 17
    u32                      CH1TXP1UA :1; // Bit 18
    u32                      CH1TXP0UA :1; // Bit 19
    u32                      CH1RXP1IP :1; // Bit 20
    u32                      CH1RXP0IP :1; // Bit 21
    u32                      CH1TXP1IP :1; // Bit 22
    u32                      CH1TXP0IP :1; // Bit 23
    u32                      CH0RXP1UA :1; // Bit 24
    u32                      CH0RXP0UA :1; // Bit 25
    u32                      CH0TXP1UA :1; // Bit 26
    u32                      CH0TXP0UA :1; // Bit 27
    u32                      CH0RXP1IP :1; // Bit 28
    u32                      CH0RXP0IP :1; // Bit 29
    u32                      CH0TXP1IP :1; // Bit 30
    u32                      CH0TXP0IP :1; // Bit 31
}PCM_ISR03_REG, *PPCM_ISR03_REG;

typedef enum _PCM_ISR015 {
    PcmCh3P1RBU          = 0x00000001, //ch0-3
    PcmCh3P0RBU          = 0x00000002,
    PcmCh3P1TBU          = 0x00000004,
    PcmCh3P0TBU          = 0x00000008,
    PcmCh3P1ROK          = 0x00000010,
    PcmCh3P0ROK          = 0x00000020,
    PcmCh3P1TOK          = 0x00000040,
    PcmCh3P0TOK          = 0x00000080,
    PcmCh2P1RBU          = 0x00000100,
    PcmCh2P0RBU          = 0x00000200,
    PcmCh2P1TBU          = 0x00000400,
    PcmCh2P0TBU          = 0x00000800,
    PcmCh2P1ROK          = 0x00001000,
    PcmCh2P0ROK          = 0x00002000,
    PcmCh2P1TOK          = 0x00004000,
    PcmCh2P0TOK          = 0x00008000,
    PcmCh1P1RBU          = 0x00010000,
    PcmCh1P0RBU          = 0x00020000,
    PcmCh1P1TBU          = 0x00040000,
    PcmCh1P0TBU          = 0x00080000,
    PcmCh1P1ROK          = 0x00100000,
    PcmCh1P0ROK          = 0x00200000,
    PcmCh1P1TOK          = 0x00400000,
    PcmCh1P0TOK          = 0x00800000,
    PcmCh0P1RBU          = 0x01000000,
    PcmCh0P0RBU          = 0x02000000,
    PcmCh0P1TBU          = 0x04000000,
    PcmCh0P0TBU          = 0x08000000,
    PcmCh0P1ROK          = 0x10000000,
    PcmCh0P0ROK          = 0x20000000,
    PcmCh0P1TOK          = 0x40000000,
    PcmCh0P0TOK          = 0x80000000,

    PcmCh7P1RBU          = 0x00000001, //ch4-7
    PcmCh7P0RBU          = 0x00000002,
    PcmCh7P1TBU          = 0x00000004,
    PcmCh7P0TBU          = 0x00000008,
    PcmCh7P1ROK          = 0x00000010,
    PcmCh7P0ROK          = 0x00000020,
    PcmCh7P1TOK          = 0x00000040,
    PcmCh7P0TOK          = 0x00000080,
    PcmCh6P1RBU          = 0x00000100,
    PcmCh6P0RBU          = 0x00000200,
    PcmCh6P1TBU          = 0x00000400,
    PcmCh6P0TBU          = 0x00000800,
    PcmCh6P1ROK          = 0x00001000,
    PcmCh6P0ROK          = 0x00002000,
    PcmCh6P1TOK          = 0x00004000,
    PcmCh6P0TOK          = 0x00008000,
    PcmCh5P1RBU          = 0x00010000,
    PcmCh5P0RBU          = 0x00020000,
    PcmCh5P1TBU          = 0x00040000,
    PcmCh5P0TBU          = 0x00080000,
    PcmCh5P1ROK          = 0x00100000,
    PcmCh5P0ROK          = 0x00200000,
    PcmCh5P1TOK          = 0x00400000,
    PcmCh5P0TOK          = 0x00800000,
    PcmCh4P1RBU          = 0x01000000,
    PcmCh4P0RBU          = 0x02000000,
    PcmCh4P1TBU          = 0x04000000,
    PcmCh4P0TBU          = 0x08000000,
    PcmCh4P1ROK          = 0x10000000,
    PcmCh4P0ROK          = 0x20000000,
    PcmCh4P1TOK          = 0x40000000,
    PcmCh4P0TOK          = 0x80000000,

    PcmCh11P1RBU         = 0x00000001, //ch8-11
    PcmCh11P0RBU         = 0x00000002,
    PcmCh11P1TBU         = 0x00000004,
    PcmCh11P0TBU         = 0x00000008,
    PcmCh11P1ROK         = 0x00000010,
    PcmCh11P0ROK         = 0x00000020,
    PcmCh11P1TOK         = 0x00000040,
    PcmCh11P0TOK         = 0x00000080,
    PcmCh10P1RBU         = 0x00000100,
    PcmCh10P0RBU         = 0x00000200,
    PcmCh10P1TBU         = 0x00000400,
    PcmCh10P0TBU         = 0x00000800,
    PcmCh10P1ROK         = 0x00001000,
    PcmCh10P0ROK         = 0x00002000,
    PcmCh10P1TOK         = 0x00004000,
    PcmCh10P0TOK         = 0x00008000,
    PcmCh9P1RBU          = 0x00010000,
    PcmCh9P0RBU          = 0x00020000,
    PcmCh9P1TBU          = 0x00040000,
    PcmCh9P0TBU          = 0x00080000,
    PcmCh9P1ROK          = 0x00100000,
    PcmCh9P0ROK          = 0x00200000,
    PcmCh9P1TOK          = 0x00400000,
    PcmCh9P0TOK          = 0x00800000,
    PcmCh8P1RBU          = 0x01000000,
    PcmCh8P0RBU          = 0x02000000,
    PcmCh8P1TBU          = 0x04000000,
    PcmCh8P0TBU          = 0x08000000,
    PcmCh8P1ROK          = 0x10000000,
    PcmCh8P0ROK          = 0x20000000,
    PcmCh8P1TOK          = 0x40000000,
    PcmCh8P0TOK          = 0x80000000,

    PcmCh15P1RBU         = 0x00000001, //ch12-15
    PcmCh15P0RBU         = 0x00000002,
    PcmCh15P1TBU         = 0x00000004,
    PcmCh15P0TBU         = 0x00000008,
    PcmCh15P1ROK         = 0x00000010,
    PcmCh15P0ROK         = 0x00000020,
    PcmCh15P1TOK         = 0x00000040,
    PcmCh15P0TOK         = 0x00000080,
    PcmCh14P1RBU         = 0x00000100,
    PcmCh14P0RBU         = 0x00000200,
    PcmCh14P1TBU         = 0x00000400,
    PcmCh14P0TBU         = 0x00000800,
    PcmCh14P1ROK         = 0x00001000,
    PcmCh14P0ROK         = 0x00002000,
    PcmCh14P1TOK         = 0x00004000,
    PcmCh14P0TOK         = 0x00008000,
    PcmCh13P1RBU         = 0x00010000,
    PcmCh13P0RBU         = 0x00020000,
    PcmCh13P1TBU         = 0x00040000,
    PcmCh13P0TBU         = 0x00080000,
    PcmCh13P1ROK         = 0x00100000,
    PcmCh13P0ROK         = 0x00200000,
    PcmCh13P1TOK         = 0x00400000,
    PcmCh13P0TOK         = 0x00800000,
    PcmCh12P1RBU         = 0x01000000,
    PcmCh12P0RBU         = 0x02000000,
    PcmCh12P1TBU         = 0x04000000,
    PcmCh12P0TBU         = 0x08000000,
    PcmCh12P1ROK         = 0x10000000,
    PcmCh12P0ROK         = 0x20000000,
    PcmCh12P1TOK         = 0x40000000,
    PcmCh12P0TOK         = 0x80000000
}PCM_ISR015, *PPCM_ISR015;

VOID
HalPcmOnOffRtl8195a(
    IN  VOID              *Data
);

BOOL
HalPcmInitRtl8195a(
    IN  VOID		  *Data
);

BOOL
HalPcmSettingRtl8195a(
    IN  VOID		  *Data
);

BOOL
HalPcmEnRtl8195a(
    IN  VOID		  *Data
);

BOOL
HalPcmIsrEnAndDisRtl8195a(
    IN  VOID		  *Data
);

BOOL
HalPcmDumpRegRtl8195a(
    IN  VOID		  *Data
);

BOOL
HalPcmRtl8195a(
    IN  VOID		  *Data
);

#endif /* _RTL8195A_PCM_H_ */



// dpgeorge: this file taken from w5500/w5500.c and adapted to W5200

//*****************************************************************************
//
//! \file w5500.c
//! \brief W5500 HAL Interface.
//! \version 1.0.1
//! \date 2013/10/21
//! \par  Revision history
//!       <2014/05/01> V1.0.2
//!         1. Implicit type casting -> Explicit type casting. Refer to M20140501
//!            Fixed the problem on porting into under 32bit MCU
//!            Issued by Mathias ClauBen, wizwiki forum ID Think01 and bobh
//!            Thank for your interesting and serious advices.
//!       <2013/10/21> 1st Release
//!       <2013/12/20> V1.0.1
//!         1. Remove warning
//!         2. WIZCHIP_READ_BUF WIZCHIP_WRITE_BUF in case _WIZCHIP_IO_MODE_SPI_FDM_
//!            for loop optimized(removed). refer to M20131220
//! \author MidnightCow
//! \copyright
//!
//! Copyright (c)  2013, WIZnet Co., LTD.
//! All rights reserved.
//! 
//! Redistribution and use in source and binary forms, with or without 
//! modification, are permitted provided that the following conditions 
//! are met: 
//! 
//!     * Redistributions of source code must retain the above copyright 
//! notice, this list of conditions and the following disclaimer. 
//!     * Redistributions in binary form must reproduce the above copyright
//! notice, this list of conditions and the following disclaimer in the
//! documentation and/or other materials provided with the distribution. 
//!     * Neither the name of the <ORGANIZATION> nor the names of its 
//! contributors may be used to endorse or promote products derived 
//! from this software without specific prior written permission. 
//! 
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//! CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//! SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//! INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//! CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//! ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
//! THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#include "w5200.h"

#if WIZCHIP_USE_MAX_BUFFER
// This option is intended to be used when MACRAW mode is enabled, to allow
// the single raw socket to use all the available buffer space.
#define SMASK (16 * 1024 - 1) /* tx buffer mask */
#define RMASK (16 * 1024 - 1) /* rx buffer mask */
#define SSIZE (16 * 1024) /* max tx buffer size */
#define RSIZE (16 * 1024) /* max rx buffer size */
#else
#define SMASK (0x7ff) /* tx buffer mask */
#define RMASK (0x7ff) /* rx buffer mask */
#define SSIZE (2048) /* max tx buffer size */
#define RSIZE (2048) /* max rx buffer size */
#endif

#define TXBUF_BASE (0x8000)
#define RXBUF_BASE (0xc000)
#define SBASE(sn) (TXBUF_BASE + SSIZE * (sn)) /* tx buffer base for socket sn */
#define RBASE(sn) (RXBUF_BASE + RSIZE * (sn)) /* rx buffer base for socket sn */

uint8_t WIZCHIP_READ(uint32_t AddrSel) {
    WIZCHIP_CRITICAL_ENTER();
    WIZCHIP.CS._select();

    uint8_t spi_data[4] = {
        AddrSel >> 8,
        AddrSel,
        0x00,
        0x01,
    };
    WIZCHIP.IF.SPI._write_bytes(spi_data, 4);
    uint8_t ret;
    WIZCHIP.IF.SPI._read_bytes(&ret, 1);

    WIZCHIP.CS._deselect();
    WIZCHIP_CRITICAL_EXIT();

    return ret;
}

void WIZCHIP_WRITE(uint32_t AddrSel, uint8_t wb) {
    WIZCHIP_CRITICAL_ENTER();
    WIZCHIP.CS._select();

    uint8_t spi_data[5] = {
        AddrSel >> 8,
        AddrSel,
        0x80,
        0x01,
        wb,
    };
    WIZCHIP.IF.SPI._write_bytes(spi_data, 5);

    WIZCHIP.CS._deselect();
    WIZCHIP_CRITICAL_EXIT();
}

void WIZCHIP_READ_BUF(uint32_t AddrSel, uint8_t* pBuf, uint16_t len) {
    WIZCHIP_CRITICAL_ENTER();
    WIZCHIP.CS._select();

    uint8_t spi_data[4] = {
        AddrSel >> 8,
        AddrSel,
        0x00 | ((len >> 8) & 0x7f),
        len & 0xff,
    };
    WIZCHIP.IF.SPI._write_bytes(spi_data, 4);
    WIZCHIP.IF.SPI._read_bytes(pBuf, len);

    WIZCHIP.CS._deselect();
    WIZCHIP_CRITICAL_EXIT();
}

void WIZCHIP_WRITE_BUF(uint32_t AddrSel, uint8_t* pBuf, uint16_t len) {
    WIZCHIP_CRITICAL_ENTER();
    WIZCHIP.CS._select();

    uint8_t spi_data[4] = {
        AddrSel >> 8,
        AddrSel,
        0x80 | ((len >> 8) & 0x7f),
        len & 0xff,
    };
    WIZCHIP.IF.SPI._write_bytes(spi_data, 4);
    WIZCHIP.IF.SPI._write_bytes(pBuf, len);

    WIZCHIP.CS._deselect();
    WIZCHIP_CRITICAL_EXIT();
}

uint16_t getSn_TX_FSR(uint8_t sn) {
    uint16_t val = 0, val1 = 0;
    do {
        val1 = (WIZCHIP_READ(Sn_TX_FSR(sn)) << 8) | WIZCHIP_READ(Sn_TX_FSR(sn) + 1);
        if (val1 != 0) {
            val = (WIZCHIP_READ(Sn_TX_FSR(sn)) << 8) | WIZCHIP_READ(Sn_TX_FSR(sn) + 1);
        }
    } while (val != val1);
    return val;
}

uint16_t getSn_RX_RSR(uint8_t sn) {
    uint16_t val = 0, val1 = 0;
    do {
        val1 = (WIZCHIP_READ(Sn_RX_RSR(sn)) << 8) | WIZCHIP_READ(Sn_RX_RSR(sn) + 1);
        if (val1 != 0) {
            val = (WIZCHIP_READ(Sn_RX_RSR(sn)) << 8) | WIZCHIP_READ(Sn_RX_RSR(sn) + 1);
        }
    } while (val != val1);
    return val;
}

void wiz_send_data(uint8_t sn, uint8_t *wizdata, uint16_t len) {
    if (len == 0) {
        return;
    }

    uint16_t ptr = getSn_TX_WR(sn);
    uint16_t offset = ptr & SMASK;
    uint32_t addr = offset + SBASE(sn);

    if (offset + len > SSIZE) {
        // implement wrap-around circular buffer
        uint16_t size = SSIZE - offset;
        WIZCHIP_WRITE_BUF(addr, wizdata, size);
        WIZCHIP_WRITE_BUF(SBASE(sn), wizdata + size, len - size);
    } else {
        WIZCHIP_WRITE_BUF(addr, wizdata, len);
    }

    ptr += len;
    setSn_TX_WR(sn, ptr);
}

void wiz_recv_data(uint8_t sn, uint8_t *wizdata, uint16_t len) {
    if (len == 0) {
        return;
    }

    uint16_t ptr = getSn_RX_RD(sn);
    uint16_t offset = ptr & RMASK;
    uint16_t addr = RBASE(sn) + offset;

    if (offset + len > RSIZE) {
        // implement wrap-around circular buffer
        uint16_t size = RSIZE - offset;
        WIZCHIP_READ_BUF(addr, wizdata, size);
        WIZCHIP_READ_BUF(RBASE(sn), wizdata + size, len - size);
    } else {
        WIZCHIP_READ_BUF(addr, wizdata, len);
    }

    ptr += len;
    setSn_RX_RD(sn, ptr);
}

void wiz_recv_ignore(uint8_t sn, uint16_t len) {
    uint16_t ptr = getSn_RX_RD(sn);
    ptr += len;
    setSn_RX_RD(sn, ptr);
}

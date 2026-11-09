/**
 * Copyright (c) 2011-2025 Bill Greiman, 2026 Tim Cocks for Adafruit Industries
 * This file is part of the SdFat library for SD memory cards.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifdef ARDUINO_ARCH_RP2040
#include <math.h>

#include <algorithm>  // Required for std::max, std::min
#define DEBUG_FILE "PioSdioCard.cpp"
#include "../SdCardInfo.h"
#include "DbgLog.h"
#include "PioSdioCard.h"
#include "PioSdioCard.pio.h"
// CIRCUITPY-CHANGE: cooperate with CircuitPython's rp2pio PIO allocator
#include "common-hal/rp2pio/pio_alloc.h"
//------------------------------------------------------------------------------
// USE_DEBUG_MODE 0 - no debug, 1 - print message, 2 - Use scope/analyzer.
#define USE_DEBUG_MODE 0

const uint PIO_CLK_DIV_RUN = 1;

const uint PIN_SDIO_UNDEFINED = 63u;

const uint DAT_FIFO_DEPTH = 8;
//==============================================================================
// Command definitions.
enum { RSP_R0 = 0, RSP_R1 = 1, RSP_R2 = 2, RSP_R3 = 3, RSP_R6 = 6, RSP_R7 = 7 };
static const CmdRsp_t CMD0_R0(CMD0, RSP_R0);
static const CmdRsp_t CMD2_R2(CMD2, RSP_R2);
static const CmdRsp_t CMD3_R6(CMD3, RSP_R6);
static const CmdRsp_t CMD6_R1(CMD6, RSP_R1);
static const CmdRsp_t CMD7_R1(CMD7, RSP_R1);
static const CmdRsp_t CMD8_R7(CMD8, RSP_R7);
static const CmdRsp_t CMD9_R2(CMD9, RSP_R2);
static const CmdRsp_t CMD10_R2(CMD10, RSP_R2);
static const CmdRsp_t CMD12_R1(CMD12, RSP_R1);
static const CmdRsp_t CMD13_R1(CMD13, RSP_R1);
static const CmdRsp_t CMD18_R1(CMD18, RSP_R1);
static const CmdRsp_t CMD25_R1(CMD25, RSP_R1);
static const CmdRsp_t CMD32_R1(CMD32, RSP_R1);
static const CmdRsp_t CMD33_R1(CMD33, RSP_R1);
static const CmdRsp_t CMD38_R1(CMD38, RSP_R1);
static const CmdRsp_t CMD55_R1(CMD55, RSP_R1);
static const CmdRsp_t ACMD6_R1(ACMD6, RSP_R1);
static const CmdRsp_t ACMD13_R1(ACMD13, RSP_R1);
static const CmdRsp_t ACMD41_R3(ACMD41, RSP_R3);
static const CmdRsp_t ACMD51_R1(ACMD51, RSP_R1);
//==============================================================================
class Timeout {
 public:
  explicit Timeout(uint ms) : _usStart(0), _usTimeout(1000 * ms) {}
  bool timedOut() {
    if (_usStart) {
      return (usSinceBoot() - _usStart) > _usTimeout;
    }
    _usStart = usSinceBoot();
    return false;
  }
  uint32_t usSinceBoot() { return to_us_since_boot(get_absolute_time()); }
  uint32_t _usStart;
  uint32_t _usTimeout;
};
//==============================================================================
#if USE_DEBUG_MODE
//------------------------------------------------------------------------------
static inline void gpioStatus(uint gpio) {
  logmsgln("gpio", gpio, " drive: ", gpio_get_drive_strength(gpio));
  //  logmsgln("gpio", gpio,
  //           " drive: ", static_cast<int>(gpio_get_drive_strength(gpio)));
  logmsgln("gpio", gpio, " slew:  ", gpio_get_slew_rate(gpio));
  logmsgln("gpio", gpio, " hyst:  ", gpio_is_input_hysteresis_enabled(gpio));
  logmsgln("gpio", gpio, " pull:  ", gpio_is_pulled_up(gpio));
}
//------------------------------------------------------------------------------
static inline void pioRegs(PIO pio) {
  logmsgln("ctrl:    0b", Bin(pio->ctrl));
  logmsgln("fstat:   0b", Bin(pio->fstat));
  logmsgln("fdebug:  0b", Bin(pio->fdebug));
  logmsgln("flevel:  0b", Bin(pio->flevel));
  logmsgln("padout:  0b", Bin(pio->dbg_padout));
  logmsgln("padoe:   0b", Bin(pio->dbg_padoe));
  logmsgln("cfginfo: 0x", Hex(pio->dbg_cfginfo));
  logmsgln("sync_bypass: 0b", Bin(pio->input_sync_bypass));
}
//------------------------------------------------------------------------------
static inline void pioSmRegs(PIO pio, uint sm) {
  logmsgln("sm", sm, " clkdiv: 0x", Hex(pio->sm[sm].clkdiv));
  logmsgln("sm", sm, " execctrl: 0x", Hex(pio->sm[sm].execctrl));
  logmsgln("sm", sm, " shiftctrl: 0x", Hex(pio->sm[sm].shiftctrl));
  logmsgln("sm", sm, " addr: 0x", Hex(pio->sm[sm].addr));
  logmsgln("sm", sm, " pinctrl: 0x", Hex(pio->sm[sm].pinctrl));
}
//------------------------------------------------------------------------------
#endif  // USE_DEBUG_MODE
#define sdError(code)               \
  {                                 \
    setSdErrorCode(code, __LINE__); \
    DBG_MSG(#code);                 \
  }
#define SDIO_FAIL() DBG_MSG("SDIO_FAIL")
//==============================================================================
// CRC functions.
//------------------------------------------------------------------------------
// See this library's extras folder.
static const uint8_t crc7_table[256] = {
    0x00, 0x12, 0x24, 0x36, 0x48, 0x5a, 0x6c, 0x7e,  // 00 - 07
    0x90, 0x82, 0xb4, 0xa6, 0xd8, 0xca, 0xfc, 0xee,  // 08 - 0f
    0x32, 0x20, 0x16, 0x04, 0x7a, 0x68, 0x5e, 0x4c,  // 10 - 17
    0xa2, 0xb0, 0x86, 0x94, 0xea, 0xf8, 0xce, 0xdc,  // 18 - 1f
    0x64, 0x76, 0x40, 0x52, 0x2c, 0x3e, 0x08, 0x1a,  // 20 - 27
    0xf4, 0xe6, 0xd0, 0xc2, 0xbc, 0xae, 0x98, 0x8a,  // 28 - 2f
    0x56, 0x44, 0x72, 0x60, 0x1e, 0x0c, 0x3a, 0x28,  // 30 - 37
    0xc6, 0xd4, 0xe2, 0xf0, 0x8e, 0x9c, 0xaa, 0xb8,  // 38 - 3f
    0xc8, 0xda, 0xec, 0xfe, 0x80, 0x92, 0xa4, 0xb6,  // 40 - 47
    0x58, 0x4a, 0x7c, 0x6e, 0x10, 0x02, 0x34, 0x26,  // 48 - 4f
    0xfa, 0xe8, 0xde, 0xcc, 0xb2, 0xa0, 0x96, 0x84,  // 50 - 57
    0x6a, 0x78, 0x4e, 0x5c, 0x22, 0x30, 0x06, 0x14,  // 58 - 5f
    0xac, 0xbe, 0x88, 0x9a, 0xe4, 0xf6, 0xc0, 0xd2,  // 60 - 67
    0x3c, 0x2e, 0x18, 0x0a, 0x74, 0x66, 0x50, 0x42,  // 68 - 6f
    0x9e, 0x8c, 0xba, 0xa8, 0xd6, 0xc4, 0xf2, 0xe0,  // 70 - 77
    0x0e, 0x1c, 0x2a, 0x38, 0x46, 0x54, 0x62, 0x70,  // 78 - 7f
    0x82, 0x90, 0xa6, 0xb4, 0xca, 0xd8, 0xee, 0xfc,  // 80 - 87
    0x12, 0x00, 0x36, 0x24, 0x5a, 0x48, 0x7e, 0x6c,  // 88 - 8f
    0xb0, 0xa2, 0x94, 0x86, 0xf8, 0xea, 0xdc, 0xce,  // 90 - 97
    0x20, 0x32, 0x04, 0x16, 0x68, 0x7a, 0x4c, 0x5e,  // 98 - 9f
    0xe6, 0xf4, 0xc2, 0xd0, 0xae, 0xbc, 0x8a, 0x98,  // a0 - a7
    0x76, 0x64, 0x52, 0x40, 0x3e, 0x2c, 0x1a, 0x08,  // a8 - af
    0xd4, 0xc6, 0xf0, 0xe2, 0x9c, 0x8e, 0xb8, 0xaa,  // b0 - b7
    0x44, 0x56, 0x60, 0x72, 0x0c, 0x1e, 0x28, 0x3a,  // b8 - bf
    0x4a, 0x58, 0x6e, 0x7c, 0x02, 0x10, 0x26, 0x34,  // c0 - c7
    0xda, 0xc8, 0xfe, 0xec, 0x92, 0x80, 0xb6, 0xa4,  // c8 - cf
    0x78, 0x6a, 0x5c, 0x4e, 0x30, 0x22, 0x14, 0x06,  // d0 - d7
    0xe8, 0xfa, 0xcc, 0xde, 0xa0, 0xb2, 0x84, 0x96,  // d8 - df
    0x2e, 0x3c, 0x0a, 0x18, 0x66, 0x74, 0x42, 0x50,  // e0 - e7
    0xbe, 0xac, 0x9a, 0x88, 0xf6, 0xe4, 0xd2, 0xc0,  // e8 - ef
    0x1c, 0x0e, 0x38, 0x2a, 0x54, 0x46, 0x70, 0x62,  // f0 - f7
    0x8c, 0x9e, 0xa8, 0xba, 0xc4, 0xd6, 0xe0, 0xf2   // f8 - ff
};
//------------------------------------------------------------------------------
inline static uint8_t CRC7(const uint8_t* data, uint8_t n) {
  uint8_t crc = 0;
  for (uint8_t i = 0; i < n; i++) {
    crc = crc7_table[crc ^ data[i]];
  }
  return crc | 1;
}
//------------------------------------------------------------------------------
// Modified from sdio_crc16_4bit_checksum() in
// https://github.com/ZuluSCSI/ZuluSCSI-firmware
//
static inline __attribute__((always_inline)) uint64_t crc16(uint64_t crc,
                                                            uint32_t data_in) {
  // Shift out 8 bits for each line
  uint32_t data_out = crc >> 32;
  crc <<= 32;

  // XOR outgoing data to itself with 4 bit delay
  data_out ^= (data_out >> 16);

  // XOR incoming data to outgoing data with 4 bit delay
  data_out ^= (data_in >> 16);

  // XOR outgoing and incoming data to accumulator at each tap
  uint64_t xorred = data_out ^ data_in;
  crc ^= xorred;
  crc ^= xorred << (5 * 4);
  crc ^= xorred << (12 * 4);
  return crc;
}
//------------------------------------------------------------------------------
//==============================================================================
// add to PioSdioCard class int the future.
// PioSdioCard::PioSdioCard()
// PioSdioCard::~PioSdioCard()
//------------------------------------------------------------------------------
bool PioSdioCard::cardAcmd(uint32_t rca, CmdRsp_t cmdRsp, uint32_t arg) {
  return cardCommand(CMD55_R1, rca) && cardCommand(cmdRsp, arg);
}
//------------------------------------------------------------------------------
bool PioSdioCard::begin(PioSdioConfig sdioConfig) {
  pioEnd();
  Timeout timeout(SD_INIT_TIMEOUT);
  uint32_t arg;
  m_curState = IDLE_STATE;
  m_errorCode = SD_CARD_ERROR_NONE;
  m_highCapacity = false;
  m_initDone = false;
  m_version2 = false;
  m_clkPin = sdioConfig.clkPin();
  m_cmdPin = sdioConfig.cmdPin();
  m_dat0Pin = sdioConfig.dat0Pin();

  // Four PIO cycles per SD CLK cycle.
  m_clkDiv = ceil((0.00025 * clock_get_hz(clk_sys)) / SD_MAX_INIT_RATE_KHZ);

#if USE_DEBUG_MODE == 2
  Serial.println();
  pioRegs(m_pio);
  pioSmRegs(m_pio, m_sm0);
  pioSmRegs(m_pio, m_sm1);
  gpioStatus(m_clkPin);
  gpioStatus(m_cmdPin);
  while (Serial.read() >= 0) {
  }
  Serial.println("Logic Analyzer on then type any char");
  while (!Serial.available()) {
  }
#endif  // USE_DEBUG_MODE
  // A few cards still require clocks after power-up.
  powerUpClockCycles();
  if (!pioInit()) {
    goto fail;
  }
  pioConfig(m_clkDiv);
  if (!cardCommand(CMD0_R0, 0)) {
    sdError(SD_CARD_ERROR_CMD0);
    goto fail;
  }
  if (cardCommand(CMD8_R7, 0X1AA)) {
    if (m_cardRsp != 0X1AA) {
      sdError(SD_CARD_ERROR_CMD8);
      goto fail;
    }
    m_version2 = true;
  } else {
    m_version2 = false;
    m_errorCode = SD_CARD_ERROR_NONE;
  }
  arg = m_version2 ? 0X40300000 : 0x00300000;
  while (true) {
    if (!cardAcmd(0, ACMD41_R3, arg)) {
      sdError(SD_CARD_ERROR_ACMD41);
      goto fail;
    }
    if (m_cardRsp & 0x80000000) {
      break;
    }
    if (timeout.timedOut()) {
      sdError(SD_CARD_ERROR_ACMD41);
      goto fail;
    }
  }
  m_ocr = m_cardRsp;
  if (m_cardRsp & 0x40000000) {
    // Is high capacity.
    m_highCapacity = true;
  }
  if (!cardCommand(CMD2_R2, 0)) {
    sdError(SD_CARD_ERROR_CMD2);
    goto fail;
  }
  if (!cardCommand(CMD3_R6, 0)) {
    sdError(SD_CARD_ERROR_CMD3);
    goto fail;
  }
  m_rca = m_cardRsp & 0xFFFF0000;
  if (!cardCommand(CMD9_R2, m_rca, &m_csd)) {
    sdError(SD_CARD_ERROR_CMD9);
    goto fail;
  }
  if (!cardCommand(CMD10_R2, m_rca, &m_cid)) {
    sdError(SD_CARD_ERROR_CMD10);
    goto fail;
  }
  if (!cardCommand(CMD7_R1, m_rca)) {
    sdError(SD_CARD_ERROR_CMD7);
    goto fail;
  }

  if (!cardAcmd(m_rca, ACMD6_R1, 2)) {
    sdError(SD_CARD_ERROR_ACMD6);
    goto fail;
  }
  m_clkDiv = sdioConfig.clkDiv();
  pioConfig(m_clkDiv);
  if (!cardAcmd(m_rca, ACMD51_R1, 0)) {
    sdError(SD_CARD_ERROR_ACMD51);
    goto fail;
  }
  if (!readData(&m_scr, sizeof(m_scr))) {
    DBG_MSG("readData");
    goto fail;
  }
  if (!cardAcmd(m_rca, ACMD13_R1, 0)) {
    sdError(SD_CARD_ERROR_ACMD13);
    SDIO_FAIL();
    goto fail;
  }

  if (!readData(&m_sds, sizeof(m_sds))) {
    SDIO_FAIL();
    goto fail;
  }
  m_initDone = true;

  return true;
fail:
  return false;
}
//------------------------------------------------------------------------------
bool PioSdioCard::cardCMD6(uint32_t arg, uint8_t* status) {
  if (!cardCommand(CMD6_R1, arg)) {
    sdError(SD_CARD_ERROR_CMD6);
    goto fail;
  }
  if (!readData(status, 64)) {
    SDIO_FAIL();
    goto fail;
  }
  return true;
fail:
  return false;
}
//------------------------------------------------------------------------------
bool PioSdioCard::cardCommand(CmdRsp_t cmd, uint32_t arg, void* rsp) {
  uint8_t buf[6];
  uint nRsp = cmd.rsp == RSP_R0 ? 0 : cmd.rsp == RSP_R2 ? 17 : 6;
  const io_ro_8* rxFifo = reinterpret_cast<io_ro_8*>(&m_pio->rxf[m_sm0]);
  io_wo_8* txFifo = reinterpret_cast<io_wo_8*>(&m_pio->txf[m_sm0]);
  pio_sm_set_enabled(m_pio, m_sm1, false);
  pio_sm_init(m_pio, m_sm0, m_cmdRspOffset, &m_cmdConfig);
  pio_sm_exec(m_pio, m_sm0, pio_encode_set(pio_pindirs, 1));
  *txFifo = 55;
  pio_sm_exec(m_pio, m_sm0, pio_encode_out(pio_x, 8));
  *txFifo = nRsp ? 8 * nRsp - 1 : 0;
  pio_sm_exec(m_pio, m_sm0, pio_encode_out(pio_y, 8));
  pio_sm_set_enabled(m_pio, m_sm0, true);
  uint n = 0;
  buf[n++] = (uint8_t)(cmd.idx | 0x40);
  buf[n++] = (uint8_t)(arg >> 24U);
  buf[n++] = (uint8_t)(arg >> 16U);
  buf[n++] = (uint8_t)(arg >> 8U);
  buf[n++] = (uint8_t)arg;
  buf[n++] = CRC7(buf, 5);
  *txFifo = 0XFF;
  for (uint i = 0; i < n; i++) {
    while (pio_sm_is_tx_fifo_full(m_pio, m_sm0)) {
    }
    *txFifo = buf[i];
  }

  Timeout timeout(SD_CMD_TIMEOUT);
  if (!nRsp) {
    uint32_t fdebug_tx_stall = 1u << (PIO_FDEBUG_TXSTALL_LSB + m_sm0);
    m_pio->fdebug = fdebug_tx_stall;
    while (!(m_pio->fdebug & fdebug_tx_stall)) {
      if (timeout.timedOut()) {
        goto fail;
      }
    }
    goto done;
  }
  uint8_t rtn[20];

  for (uint i = 0; i < nRsp; i++) {
    while (pio_sm_is_rx_fifo_empty(m_pio, m_sm0)) {
      if (timeout.timedOut()) {
        goto fail;
      }
    }
    rtn[i] = *rxFifo;
  }
  if (cmd.rsp == RSP_R3) {
    if (rtn[0] != 0X3F || rtn[5] != 0XFF) {
      goto fail;
    }
  } else {
    uint8_t crc;
    if (cmd.rsp == RSP_R2) {
      crc = CRC7(rtn + 1, nRsp - 2);
    } else {
      crc = CRC7(rtn, nRsp - 1);
    }
    if (rtn[nRsp - 1] != crc) {
#if USE_DEBUG_MODE
      Serial.printf("CHK: %02X, CRC: %02X\n", rtn[nRsp - 1], crc);
      for (uint i = 0; i < nRsp; i++) {
        Serial.printf(" %02X", rtn[i]);
      }
      Serial.println();
#endif  // USE_DEBUG_MODE
      sdError(SD_CARD_ERROR_READ_CRC);
      goto fail;
    }
  }
  if (nRsp == 6) {
    m_cardRsp = (rtn[1] << 24) | (rtn[2] << 16) | (rtn[3] << 8) | rtn[4];
    if (rsp) {
      *reinterpret_cast<uint32_t*>(rsp) = m_cardRsp;
    }
  } else if (rsp && nRsp == 17) {
    memcpy(rsp, rtn + 1, 16);
  }

done:
  pio_sm_set_enabled(m_pio, m_sm0, false);
  return true;

fail:
#if USE_DEBUG_MODE
  DBG_MSG("CMD", cmd.idx, " failed");
#endif  // USE_DEBUG_MODE
  pio_sm_set_enabled(m_pio, m_sm0, false);
  return false;
}
//------------------------------------------------------------------------------
void PioSdioCard::end() { pioEnd(); }
//------------------------------------------------------------------------------
bool PioSdioCard::erase(uint32_t firstSector, uint32_t lastSector) {
  Timeout timeout(SD_ERASE_TIMEOUT);
  if (!syncDevice()) {
    SDIO_FAIL();
    goto fail;
  }
  // check for single sector erase
  if (!m_csd.eraseSingleBlock()) {
    // erase size mask
    uint8_t m = m_csd.eraseSize() - 1;
    if ((firstSector & m) != 0 || ((lastSector + 1) & m) != 0) {
      // error card can't erase specified area
      sdError(SD_CARD_ERROR_ERASE_SINGLE_SECTOR);
      goto fail;
    }
  }
  if (!m_highCapacity) {
    firstSector <<= 9;
    lastSector <<= 9;
  }
  if (!cardCommand(CMD32_R1, firstSector)) {
    sdError(SD_CARD_ERROR_CMD32);
    goto fail;
  }
  if (!cardCommand(CMD33_R1, lastSector)) {
    sdError(SD_CARD_ERROR_CMD33);
    goto fail;
  }
  if (!cardCommand(CMD38_R1, 0)) {
    sdError(SD_CARD_ERROR_CMD38);
    goto fail;
  }
  while (isBusy()) {
    if (timeout.timedOut()) {
      sdError(SD_CARD_ERROR_ERASE_TIMEOUT);
      goto fail;
    }
  }
  return true;
fail:
  return false;
}
//------------------------------------------------------------------------------
uint8_t PioSdioCard::errorCode() const { return m_errorCode; }
//------------------------------------------------------------------------------
uint32_t PioSdioCard::errorData() const { return m_cardRsp; }
//------------------------------------------------------------------------------
uint32_t PioSdioCard::errorLine() const { return m_errorLine; }
//------------------------------------------------------------------------------
bool PioSdioCard::isBusy() {
  return gpio_get(m_dat0Pin) ? false : !(status() & CARD_STATUS_READY_FOR_DATA);
}
//------------------------------------------------------------------------------
void PioSdioCard::pioConfig(float clkDiv) {
  m_cmdConfig =
      pio_cmd_rsp_program_config(m_cmdRspOffset, m_cmdPin, m_clkPin, clkDiv);
  m_rdClkConfig =
      pio_rd_clk_program_config(m_rdClkOffset, m_dat0Pin, m_clkPin, clkDiv);
  m_rdDataConfig =
      pio_rd_data_program_config(m_rdDataOffset, m_dat0Pin, clkDiv);
  m_wrDataConfig =
      pio_wr_data_program_config(m_wrDataOffset, m_dat0Pin, m_clkPin, clkDiv);
  m_wrRespConfig =
      pio_wr_resp_program_config(m_wrRespOffset, m_dat0Pin, m_clkPin, clkDiv);
}
//------------------------------------------------------------------------------
void PioSdioCard::pioEnd() {
  if (!m_pio) {
    return;
  }
  // CIRCUITPY-CHANGE: release only the two state machines we claimed (see
  // pioInit) rather than every SM on the block.
  if (m_sm0 >= 0) {
    pio_sm_set_enabled(m_pio, m_sm0, false);
    pio_sm_unclaim(m_pio, m_sm0);
    m_sm0 = -1;
  }
  if (m_sm1 >= 0) {
    pio_sm_set_enabled(m_pio, m_sm1, false);
    pio_sm_unclaim(m_pio, m_sm1);
    m_sm1 = -1;
  }
  if (m_cmdRspOffset >= 0) {
    pio_remove_program(m_pio, &cmd_rsp_program, m_cmdRspOffset);
    m_cmdRspOffset = -1;
  }
  if (m_rdClkOffset >= 0) {
    pio_remove_program(m_pio, &rd_clk_program, m_rdClkOffset);
    m_rdClkOffset = -1;
  }
  if (m_rdDataOffset >= 0) {
    pio_remove_program(m_pio, &rd_data_program, m_rdDataOffset);
    m_rdDataOffset = -1;
  }
  if (m_wrDataOffset >= 0) {
    pio_remove_program(m_pio, &wr_data_program, m_wrDataOffset);
    m_wrDataOffset = -1;
  }
  if (m_wrRespOffset >= 0) {
    pio_remove_program(m_pio, &wr_resp_program, m_wrRespOffset);
    m_wrRespOffset = -1;
  }
  m_pio = nullptr;
}
//------------------------------------------------------------------------------
bool PioSdioCard::pioInit() {
  uint pin[] = {m_clkPin,      m_cmdPin,      m_dat0Pin,
                m_dat0Pin + 1, m_dat0Pin + 2, m_dat0Pin + 3};
  // CIRCUITPY-CHANGE: the PIO's gpio_base (0 or 16). input_sync_bypass bits are
  // relative to this base, so it must be subtracted from the raw GPIO numbers
  // below (a raw `1 << pin` is UB for pins >= 32, e.g. Fruit Jam's GPIO34-39,
  // and silently becomes a no-op on Cortex-M). Declared before the first goto so
  // the error paths don't bypass its initialization.
  uint gpio_base = 0;
  // Declared before the first goto for the same reason as gpio_base.
  uint pio_index = 0;
  uint16_t pio_instructions[PIO_INSTRUCTION_COUNT];
  pio_program_t pio_program = {.instructions = nullptr,
                               .length = PIO_INSTRUCTION_COUNT,
                               .origin = -1,
                               .pio_version = 0,
#if PICO_PIO_VERSION > 0
                               .used_gpio_ranges = 0x0
#endif
  };
  // CIRCUITPY-CHANGE: pick a PIO through CircuitPython's rp2pio allocator and
  // claim only the two state machines this driver uses, rather than seizing an
  // entire PIO block via the raw SDK. This lets sdioio coexist with rp2pio and
  // other PIO users. The five programs total total_pio_length (31) instructions,
  // so this effectively requires an empty PIO; find_pio enforces that, which in
  // turn guarantees the chosen PIO has no other user whose gpio_base the
  // pio_set_gpio_base() call below could disturb.
  pio_index = rp2pio_statemachine_find_pio(total_pio_length, 2);
  if (pio_index >= NUM_PIOS) {
    sdError(SD_CARD_ERROR_ADD_PIO_PROGRAM);
    goto fail;
  }
  m_pio = pio_get_instance(pio_index);
  m_sm0 = pio_claim_unused_sm(m_pio, false);
  m_sm1 = pio_claim_unused_sm(m_pio, false);
  if (m_sm0 < 0 || m_sm1 < 0) {
    sdError(SD_CARD_ERROR_ADD_PIO_PROGRAM);
    goto fail;
  }
#if PICO_PIO_USE_GPIO_BASE
  if (std::max({m_clkPin, m_cmdPin, m_dat0Pin + 3}) > 31) {
    if (std::min({m_clkPin, m_cmdPin, m_dat0Pin}) < 16 ||
        pio_set_gpio_base(m_pio, 16) != PICO_OK) {
      sdError(SD_CARD_ERROR_ADD_PIO_PROGRAM);
      goto fail;
    }
    gpio_base = 16;
  } else if (pio_set_gpio_base(m_pio, 0) != PICO_OK) {
    sdError(SD_CARD_ERROR_ADD_PIO_PROGRAM);
    goto fail;
  }
#endif  // PICO_PIO_USE_GPIO_BASE
  rd_data_patch_program(&pio_program, pio_instructions, m_clkPin);
  m_rdDataOffset = pio_add_program(m_pio, &pio_program);
  if (m_rdDataOffset < 0) {
    DBG_MSG("m_rdDataOffset: ", m_rdDataOffset);
    sdError(SD_CARD_ERROR_ADD_PIO_PROGRAM);
    goto fail;
  }
  m_cmdRspOffset = pio_add_program(m_pio, &cmd_rsp_program);
  if (m_cmdRspOffset < 0) {
    sdError(SD_CARD_ERROR_ADD_PIO_PROGRAM);
    goto fail;
  }
  m_rdClkOffset = pio_add_program(m_pio, &rd_clk_program);
  if (m_rdClkOffset < 0) {
    sdError(SD_CARD_ERROR_ADD_PIO_PROGRAM);
    goto fail;
  }
  m_wrDataOffset = pio_add_program(m_pio, &wr_data_program);
  if (m_wrDataOffset < 0) {
    sdError(SD_CARD_ERROR_ADD_PIO_PROGRAM);
    goto fail;
  }
  m_wrRespOffset = pio_add_program(m_pio, &wr_resp_program);
  if (m_wrRespOffset < 0) {
    sdError(SD_CARD_ERROR_ADD_PIO_PROGRAM);
    goto fail;
  }
  for (uint i = 0; i < 6U; i++) {
    gpio_pull_up(pin[i]);
  }
  gpio_set_drive_strength(m_clkPin, GPIO_DRIVE_STRENGTH_8MA);
  gpio_set_slew_rate(m_clkPin, GPIO_SLEW_RATE_FAST);

  for (uint i = 0; i < 6U; i++) {
    pio_gpio_init(m_pio, pin[i]);
  }
  // input_sync_bypass bits are relative to the PIO's gpio_base (0 or 16), so
  // subtract it. Using the raw pin numbers here is undefined for pins >= 32
  // (Fruit Jam's SD pins are GPIO34-39) and evaluates to 0 on Cortex-M, leaving
  // the input synchronizers in place -- benign at the ~400 kHz init rate but a
  // source of CMD/DAT mis-sampling at the 25 MHz run rate.
  m_pio->input_sync_bypass |= (1u << (m_clkPin - gpio_base)) |
      (1u << (m_cmdPin - gpio_base)) | (0xFu << (m_dat0Pin - gpio_base));
  if (pio_sm_set_consecutive_pindirs(m_pio, m_sm0, m_clkPin, 1, true) !=
          PICO_OK ||
      pio_sm_set_consecutive_pindirs(m_pio, m_sm0, m_cmdPin, 1, true) !=
          PICO_OK ||
      pio_sm_set_consecutive_pindirs(m_pio, m_sm0, m_dat0Pin, 4, false) !=
          PICO_OK) {
    sdError(SD_CARD_ERROR_ADD_PIO_PROGRAM);
    goto fail;
  }
  return true;

fail:
  pioEnd();
  return false;
}
//------------------------------------------------------------------------------
// A few cards still need at least 74 clocks after power-up. from the spec:
//
// After 1 ms VDD stable time, host provides at least 74 clocks while keeping
// CMD high before issuing the first command.  In the case of SPI mode, CS
// shall be held high during 74 clock cycles.
//
// The original April 15, 2001 spec explains the 74 clocks:
//
// The additional 10 clocks (over the 64 clocks after which the card should be
// ready for communication) is provided to eliminate power-up synchronization
// problems.
//
void PioSdioCard::powerUpClockCycles() {
  // Two clk_sys per SD clock cycle.
  uint32_t nWait = ceil(0.0005 * clock_get_hz(clk_sys) / SD_MAX_INIT_RATE_KHZ);
  gpio_init(m_cmdPin);
  gpio_set_drive_strength(m_cmdPin, GPIO_DRIVE_STRENGTH_8MA);
  gpio_set_dir(m_cmdPin, true);
  gpio_put(m_cmdPin, 1);
  gpio_init(m_clkPin);
  gpio_set_drive_strength(m_clkPin, GPIO_DRIVE_STRENGTH_8MA);
  gpio_set_dir(m_clkPin, true);

  // Send 80 SD CLK cycles with CMD high.  End with CLK low.
  for (uint i = 0; i <= 160; i++) {
    gpio_put(m_clkPin, 1 & i);
    busy_wait_at_least_cycles(nWait);
  }
}
//------------------------------------------------------------------------------
bool PioSdioCard::readCID(cid_t* cid) {
  memcpy(cid, &m_cid, sizeof(cid_t));
  return true;
}
//------------------------------------------------------------------------------
bool PioSdioCard::readCSD(csd_t* csd) {
  memcpy(csd, &m_csd, sizeof(csd_t));
  return true;
}
//------------------------------------------------------------------------------
bool __time_critical_func(PioSdioCard::readData)(void* dst, size_t count) {
  uint32_t buf[128];
  uint n32 = count / 4;
  uint nr = n32 + 2;
  const uint mask = (1ul << m_sm0) | (1ul << m_sm1);
  io_wo_8* txFifo = reinterpret_cast<io_wo_8*>(&m_pio->txf[m_sm1]);
  pio_sm_init(m_pio, m_sm0, m_rdDataOffset, &m_rdDataConfig);
  pio_sm_init(m_pio, m_sm1, m_rdClkOffset, &m_rdClkConfig);
  pio_set_sm_mask_enabled(m_pio, mask, true);

  uint nf = nr < DAT_FIFO_DEPTH ? nr : DAT_FIFO_DEPTH;
  for (uint it = 0; it < nf; it++) {
    *txFifo = 0XFF;
  }
  io_ro_32* rxFifo = reinterpret_cast<io_ro_32*>(&m_pio->rxf[m_sm0]);
  uint32_t* dst32 = (uint)dst & 3 ? buf : reinterpret_cast<uint32_t*>(dst);
  uint64_t crc = 0;
  uint64_t chk = 0;
  Timeout timeout(SD_READ_TIMEOUT);
  uint ir = 0;
  if (nf < nr) {
    uint nb = nr - nf;
    while (true) {
      while (pio_sm_get_rx_fifo_level(m_pio, m_sm0) < 4) {
        if (timeout.timedOut()) {
          sdError(SD_CARD_ERROR_READ_TIMEOUT);
          goto fail;
        }
      }
      uint32_t tmp = *rxFifo;
      *txFifo = 0XFF;
      dst32[ir++] = __builtin_bswap32(tmp);
      crc = crc16(crc, tmp);
      tmp = *rxFifo;
      *txFifo = 0XFF;
      dst32[ir++] = __builtin_bswap32(tmp);
      crc = crc16(crc, tmp);
      if (ir == nb) {
        break;
      }
      tmp = *rxFifo;
      *txFifo = 0XFF;
      dst32[ir++] = __builtin_bswap32(tmp);
      crc = crc16(crc, tmp);
      tmp = *rxFifo;
      *txFifo = 0XFF;
      dst32[ir++] = __builtin_bswap32(tmp);
      crc = crc16(crc, tmp);
    }
  }
  for (; ir < nr; ir++) {
    while (pio_sm_is_rx_fifo_empty(m_pio, m_sm0)) {
      if (timeout.timedOut()) {
        sdError(SD_CARD_ERROR_READ_TIMEOUT);
        goto fail;
      }
    }
    uint32_t tmp = *rxFifo;
    if (ir < n32) {
      dst32[ir] = __builtin_bswap32(tmp);
      crc = crc16(crc, tmp);
    } else {
      chk <<= 32;
      chk |= tmp;
    }
  }
  if (crc != chk) {
#if USE_DEBUG_MODE
    Serial.printf("crc: %llX\r\nchk: %llX\r\n", crc, chk);
#endif  // USE_DEBUG_MODE
    sdError(SD_CARD_ERROR_READ_CRC);
    goto fail;
  }
  pio_set_sm_mask_enabled(m_pio, mask, false);
  if (dst32 == buf) {
    memcpy(dst, buf, count);
  }
  return true;

fail:
  pio_set_sm_mask_enabled(m_pio, mask, false);
  return false;
}
//------------------------------------------------------------------------------
bool PioSdioCard::readData(uint8_t* dst) { return readData(dst, 512); }
//------------------------------------------------------------------------------
bool PioSdioCard::readOCR(uint32_t* ocr) {
  *ocr = m_ocr;
  return true;
}
//------------------------------------------------------------------------------
bool PioSdioCard::readSCR(scr_t* scr) {
  memcpy(scr, &m_scr, sizeof(scr_t));
  return true;
}
//------------------------------------------------------------------------------
bool PioSdioCard::readSDS(sds_t* sds) {
  memcpy(sds, &m_sds, sizeof(sds_t));
  return true;
}
//------------------------------------------------------------------------------
bool PioSdioCard::readSector(Sector_t sector, uint8_t* dst) {
  if (m_curState != READ_STATE || sector != m_curSector) {
    if (!syncDevice()) {
      SDIO_FAIL();
      goto fail;
    }
    if (!readStart(sector)) {
      sdError(SD_CARD_ERROR_READ_START);
      goto fail;
    }
    m_curSector = sector;
    m_curState = READ_STATE;
  }
  if (!readData(dst, 512)) {
    SDIO_FAIL();
    goto fail;
  }
  m_curSector++;
  return true;

fail:
  return false;
}
//------------------------------------------------------------------------------
bool PioSdioCard::readSectors(Sector_t sector, uint8_t* dst, size_t ns) {
  for (size_t i = 0; i < ns; i++) {
    if (!readSector(sector + i, dst + i * 512UL)) {
      SDIO_FAIL();
      goto fail;
    }
  }
  return true;

fail:
  return false;
}
//------------------------------------------------------------------------------
bool PioSdioCard::readStart(Sector_t sector) {
  uint arg = m_highCapacity ? sector : 512 * sector;
  if (!cardCommand(CMD18_R1, arg)) {
    sdError(SD_CARD_ERROR_CMD18);
    goto fail;
  }
  return true;

fail:
  return false;
}
//------------------------------------------------------------------------------
bool PioSdioCard::readStop() {
  if (!syncDevice()) {
    SDIO_FAIL();
    return false;
  }
  return true;
}
//------------------------------------------------------------------------------
uint32_t PioSdioCard::status() {
  return cardCommand(CMD13_R1, m_rca) ? m_cardRsp : CARD_STATUS_ERROR;
}
//------------------------------------------------------------------------------
Sector_t PioSdioCard::sectorCount() { return m_csd.capacity(); }
//------------------------------------------------------------------------------
bool PioSdioCard::syncDevice() {
  if (m_curState != IDLE_STATE) {
    Timeout timeout(SD_INIT_TIMEOUT);
    while (!gpio_get(m_dat0Pin)) {
      if (timeout.timedOut()) {
        sdError(SD_CARD_ERROR_CMD12);
        goto fail;
      }
    }
    if (!cardCommand(CMD12_R1, 0)) {
      sdError(SD_CARD_ERROR_CMD12);
      goto fail;
    }
    while (isBusy()) {
      if (timeout.timedOut()) {
        sdError(SD_CARD_ERROR_CMD12);
        goto fail;
      }
    }
    m_curState = IDLE_STATE;
  }
  return true;

fail:
  return false;
}
//------------------------------------------------------------------------------
uint8_t PioSdioCard::type() const {
  return !m_initDone       ? 0
         : !m_version2     ? SD_CARD_TYPE_SD1
         : !m_highCapacity ? SD_CARD_TYPE_SD2
                           : SD_CARD_TYPE_SDHC;
}
//------------------------------------------------------------------------------
bool PioSdioCard::writeSector(Sector_t sector, const uint8_t* src) {
  return writeSectors(sector, src, 1);
}
//------------------------------------------------------------------------------
bool PioSdioCard::writeSectors(Sector_t sector, const uint8_t* src, size_t ns) {
  if (m_curState != WRITE_STATE || m_curSector != sector) {
    if (!syncDevice()) {
      SDIO_FAIL();
      goto fail;
    }
    if (!writeStart(sector)) {
      sdError(SD_CARD_ERROR_WRITE_START);
      goto fail;
    }
    m_curSector = sector;
    m_curState = WRITE_STATE;
  }
  for (size_t i = 0; i < ns; i++, src += 512) {
    if (!writeData(src)) {
      sdError(SD_CARD_ERROR_WRITE_DATA);
      goto fail;
    }
  }
  m_curSector += ns;
  return true;
fail:
  return false;
}
//------------------------------------------------------------------------------
bool __time_critical_func(PioSdioCard::writeData)(const uint8_t* src) {
  const uint32_t* src32;
  uint32_t buf[128];
  if ((uint)src & 3) {
    memcpy(buf, src, 512);
    src32 = (const uint32_t*)buf;
  } else {
    src32 = (const uint32_t*)src;
  }
  uint32_t tmp;
  io_wo_32* txFifo = reinterpret_cast<io_wo_32*>(&m_pio->txf[m_sm0]);
  io_ro_32* rxFifo = reinterpret_cast<io_ro_32*>(&m_pio->rxf[m_sm1]);
  uint8_t rsp;
  uint mask = (1ul << m_sm0) | (1ul << m_sm1);
  uint64_t crc = 0;

  Timeout timeout(SD_WRITE_TIMEOUT);
  while (!gpio_get(m_dat0Pin)) {
    if (timeout.timedOut()) {
      sdError(SD_CARD_ERROR_WRITE_TIMEOUT);
      goto fail;
    }
  }
  pio_sm_init(m_pio, m_sm0, m_wrDataOffset, &m_wrDataConfig);
  pio_sm_init(m_pio, m_sm1, m_wrRespOffset, &m_wrRespConfig);
  *txFifo = 1048;  // 8 + 1024 + 16 + 1 - 1;
  pio_sm_exec(m_pio, m_sm0, pio_encode_out(pio_x, 32));
  pio_sm_exec(m_pio, m_sm0, pio_encode_set(pio_pindirs, 0XF));
  *txFifo = 0xFFFFFFF0;
  pio_set_sm_mask_enabled(m_pio, mask, true);
  for (int i = 0; i < 128;) {
    while (pio_sm_get_tx_fifo_level(m_pio, m_sm0) > 4) {
      if (timeout.timedOut()) {
        sdError(SD_CARD_ERROR_WRITE_FIFO);
        goto fail;
      }
    }
    tmp = __builtin_bswap32(src32[i++]);
    crc = crc16(crc, tmp);
    *txFifo = tmp;
    tmp = __builtin_bswap32(src32[i++]);
    crc = crc16(crc, tmp);
    *txFifo = tmp;
    tmp = __builtin_bswap32(src32[i++]);
    crc = crc16(crc, tmp);
    *txFifo = tmp;
    tmp = __builtin_bswap32(src32[i++]);
    crc = crc16(crc, tmp);
    *txFifo = tmp;
  }
  while (pio_sm_get_tx_fifo_level(m_pio, m_sm0) > 5) {
    if (timeout.timedOut()) {
      sdError(SD_CARD_ERROR_WRITE_FIFO);
      goto fail;
    }
  }
  *txFifo = static_cast<uint32_t>(crc >> 32);
  *txFifo = static_cast<uint32_t>(crc);
  *txFifo = 0xFFFFFFFF;

  while (pio_sm_is_rx_fifo_empty(m_pio, m_sm1)) {
    if (timeout.timedOut()) {
      sdError(SD_CARD_ERROR_READ_FIFO);
      goto fail;
    }
  }
  rsp = *rxFifo;
  if ((rsp & 0X1F) != 0b101) {
#if USE_DEBUG_MODE
    Serial.printf("wr rsp: %02X\n", rsp);
#endif  // USE_DEBUG_MODE
    sdError(SD_CARD_ERROR_WRITE_DATA);
    goto fail;
  }
  return true;
fail:
  pio_set_sm_mask_enabled(m_pio, mask, false);
  return false;
}
//------------------------------------------------------------------------------
bool PioSdioCard::writeStart(Sector_t sector) {
  uint arg = m_highCapacity ? sector : 512 * sector;
  if (!cardCommand(CMD25_R1, arg)) {
    sdError(SD_CARD_ERROR_CMD25);
    goto fail;
  }
  return true;
fail:
  return false;
}
//------------------------------------------------------------------------------
bool PioSdioCard::writeStop() {
  if (!syncDevice()) {
    SDIO_FAIL();
    return false;
  }
  return true;
}
#endif  //  ARDUINO_ARCH_RP2040

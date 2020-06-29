/**
 * \file
 *
 * \brief SD/MMC protocol definitions.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef SD_MMC_PROTOCOL_H_INCLUDED
#define SD_MMC_PROTOCOL_H_INCLUDED

#include "compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup sd_mmc_protocol SD/MMC Protocol Definition
 * \ingroup sd_mmc_stack_group
 * @{
 */

// SD/MMC/SDIO default clock frequency for initialization (400KHz)
#define SDMMC_CLOCK_INIT 400000

/**
 * \name Macros for command definition
 *
 * Commands types:
 * - broadcast commands (bc), no response
 * - broadcast commands with response (bcr) (Note: No open drain on SD card)
 * - addressed (point-to-point) commands (ac), no data transfer on DAT lines
 * - addressed (point-to-point) data transfer commands (adtc), data transfer
 * on DAT lines
 *
 * Specific MMC norms:
 * CMD1, CMD2 & CMD3 are processed in the open-drain mode.
 * The CMD line is driven with push-pull drivers.
 *
 * Specific SD norms:
 * There is no open drain mode in SD memory card.
 *
 ***************************************
 * Responses types:
 *
 * R1, R3, R4 & R5 use a 48 bits response protected by a 7bit CRC checksum
 * - R1 receiv data not specified
 * - R3 receiv OCR
 * - R4, R5 RCA management (MMC only)
 * - R6, R7 RCA management (SD only)
 *
 * R1b assert the BUSY signal and respond with R1.
 * If the busy signal is asserted, it is done two clock cycles (Nsr time)
 * after the end bit of the command. The DAT0 line is driven low.
 * DAT1-DAT7 lines are driven by the card though their values are not relevant.
 *
 * R2 use a 136 bits response protected by a 7bit CRC checksum
 * The content is CID or CSD
 *
 * Specific MMC norms:
 * - R4 (Fast I/O) return RCA
 * - R5 (interrupt request) return RCA null
 *
 * Specific SD norms:
 * - R6 (Published RCA) return RCA
 * - R7 (Card interface condition) return RCA null
 *
 * @{
 */

//! Value to define a SD/MMC/SDIO command
typedef uint32_t sdmmc_cmd_def_t;

//! \name Flags used to define a SD/MMC/SDIO command
//! @{
#define SDMMC_CMD_GET_INDEX(cmd) (cmd & 0x3F)
//! Have response (MCI only)
#define SDMMC_RESP_PRESENT (1lu << 8)
//! 8 bit response (SPI only)
#define SDMMC_RESP_8 (1lu << 9)
//! 32 bit response (SPI only)
#define SDMMC_RESP_32 (1lu << 10)
//! 136 bit response (MCI only)
#define SDMMC_RESP_136 (1lu << 11)
//! Expect valid crc (MCI only)
#define SDMMC_RESP_CRC (1lu << 12)
//! Card may send busy
#define SDMMC_RESP_BUSY (1lu << 13)
// Open drain for a braodcast command (bc)
// or to enter in inactive state (MCI only)
#define SDMMC_CMD_OPENDRAIN (1lu << 14)
//! To signal a data write operation
#define SDMMC_CMD_WRITE (1lu << 15)
//! To signal a SDIO tranfer in multi byte mode
#define SDMMC_CMD_SDIO_BYTE (1lu << 16)
//! To signal a SDIO tranfer in block mode
#define SDMMC_CMD_SDIO_BLOCK (1lu << 17)
//! To signal a data transfer in stream mode
#define SDMMC_CMD_STREAM (1lu << 18)
//! To signal a data transfer in single block mode
#define SDMMC_CMD_SINGLE_BLOCK (1lu << 19)
//! To signal a data transfer in multi block mode
#define SDMMC_CMD_MULTI_BLOCK (1lu << 20)
//! @}

//! \name Set of flags to define a reponse type
//! @{
#define SDMMC_CMD_NO_RESP (0)
#define SDMMC_CMD_R1 (SDMMC_RESP_PRESENT | SDMMC_RESP_CRC)
#define SDMMC_CMD_R1B (SDMMC_RESP_PRESENT | SDMMC_RESP_CRC | SDMMC_RESP_BUSY)
#define SDMMC_CMD_R2 (SDMMC_RESP_PRESENT | SDMMC_RESP_8 | SDMMC_RESP_136 | SDMMC_RESP_CRC)
#define SDMMC_CMD_R3 (SDMMC_RESP_PRESENT | SDMMC_RESP_32)
#define SDMMC_CMD_R4 (SDMMC_RESP_PRESENT | SDMMC_RESP_32)
#define SDMMC_CMD_R5 (SDMMC_RESP_PRESENT | SDMMC_RESP_8 | SDMMC_RESP_CRC)
#define SDMMC_CMD_R6 (SDMMC_RESP_PRESENT | SDMMC_RESP_CRC)
#define SDMMC_CMD_R7 (SDMMC_RESP_PRESENT | SDMMC_RESP_32 | SDMMC_RESP_CRC)
//! @}

//! \name SD/MMC/SDIO command definitions
//! SDMMC_CMDx are include in SD and MMC norms
//! MMC_CMDx   are include in MMC norms only
//! SD_CMDx    are include in SD norms only
//! SDIO_CMDx  are include in SDIO norms only
//! @{

/*
 * --- Basic commands and read-stream command (class 0 and class 1) ---
 */

/** Cmd0(bc): Reset all cards to idle state */
#define SDMMC_SPI_CMD0_GO_IDLE_STATE (0 | SDMMC_CMD_R1)
#define SDMMC_MCI_CMD0_GO_IDLE_STATE (0 | SDMMC_CMD_NO_RESP | SDMMC_CMD_OPENDRAIN)
/** MMC Cmd1(bcr, R3): Ask the card to send its Operating Conditions */
#define MMC_SPI_CMD1_SEND_OP_COND (1 | SDMMC_CMD_R1)
#define MMC_MCI_CMD1_SEND_OP_COND (1 | SDMMC_CMD_R3 | SDMMC_CMD_OPENDRAIN)
/** Cmd2(bcr, R2): Ask the card to send its CID number (stuff but arg 0 used) */
#define SDMMC_CMD2_ALL_SEND_CID (2 | SDMMC_CMD_R2 | SDMMC_CMD_OPENDRAIN)
/** SD Cmd3(bcr, R6): Ask the card to publish a new relative address (RCA) */
#define SD_CMD3_SEND_RELATIVE_ADDR (3 | SDMMC_CMD_R6 | SDMMC_CMD_OPENDRAIN)
/** MMC Cmd3(ac, R1): Assigns relative address to the card */
#define MMC_CMD3_SET_RELATIVE_ADDR (3 | SDMMC_CMD_R1)
/** Cmd4(bc): Program the DSR of all cards (MCI only) */
#define SDMMC_CMD4_SET_DSR (4 | SDMMC_CMD_NO_RESP)
/** MMC Cmd5(ac, R1b): Toggle the card between Sleep state and Standby state. */
#define MMC_CMD5_SLEEP_AWAKE (5 | SDMMC_CMD_R1B)
/** Cmd7(ac, R1/R1b): Select/Deselect card
 * For SD: R1b only from the selected card.
 * For MMC: R1 while selecting from Stand-By State to Transfer State;
 *          R1b while selecting from Disconnected State to Programming State.
 */
#define SDMMC_CMD7_SELECT_CARD_CMD (7 | SDMMC_CMD_R1B)
#define SDMMC_CMD7_DESELECT_CARD_CMD (7 | SDMMC_CMD_R1)
/** MMC Cmd8(adtc, R1): Send EXT_CSD register as a block of data */
#define MMC_CMD8_SEND_EXT_CSD (8 | SDMMC_CMD_R1 | SDMMC_CMD_SINGLE_BLOCK)
/** SD Cmd8(bcr, R7) : Send SD Memory Card interface condition */
#define SD_CMD8_SEND_IF_COND (8 | SDMMC_CMD_R7 | SDMMC_CMD_OPENDRAIN)
/** Cmd9 SPI (R1): Addressed card sends its card-specific data (CSD) */
#define SDMMC_SPI_CMD9_SEND_CSD (9 | SDMMC_CMD_R1 | SDMMC_CMD_SINGLE_BLOCK)
/** Cmd9 MCI (ac, R2): Addressed card sends its card-specific data (CSD) */
#define SDMMC_MCI_CMD9_SEND_CSD (9 | SDMMC_CMD_R2)
/** Cmd10(ac, R2): Addressed card sends its card identification (CID) */
#define SDMMC_CMD10_SEND_CID (10 | SDMMC_CMD_R2)
/**
 * MMC Cmd11(adtc, R1): Read data stream from the card, starting at the given
 * address, until a STOP_TRANSMISSION follows.
 */
#define MMC_CMD11_READ_DAT_UNTIL_STOP (11 | SDMMC_CMD_R1)
/* SD Cmd11 MCI (ac, R1): Voltage switching */
#define SD_CMD11_READ_DAT_UNTIL_STOP (11 | SDMMC_CMD_R1)
/** Cmd12(ac, R1b): Force the card to stop transmission */
#define SDMMC_CMD12_STOP_TRANSMISSION (12 | SDMMC_CMD_R1B)
/** Cmd13(R2): Addressed card sends its status register. */
#define SDMMC_SPI_CMD13_SEND_STATUS (13 | SDMMC_CMD_R2)
/** Cmd13(ac, R1): Addressed card sends its status register. */
#define SDMMC_MCI_CMD13_SEND_STATUS (13 | SDMMC_CMD_R1)
/** MMC Cmd14(adtc, R1): Read the reversed bus testing data pattern from a card. */
#define MMC_CMD14_BUSTEST_R (14 | SDMMC_CMD_R1)
/** Cmd15(ac): Send an addressed card into the Inactive State. */
// Note: It is a ac cmd, but it must be send like bc cmd to open drain
#define SDMMC_CMD15_GO_INACTIVE_STATE (15 | SDMMC_CMD_NO_RESP | SDMMC_CMD_OPENDRAIN)
/** MMC Cmd19(adtc, R1): Send the bus test data pattern */
#define MMC_CMD19_BUSTEST_W (19 | SDMMC_CMD_R1)
/** Cmd58(R3): Reads the OCR register of a card */
#define SDMMC_SPI_CMD58_READ_OCR (58 | SDMMC_CMD_R3)
/** Cmd59(R1): Turns the CRC option on or off */
#define SDMMC_SPI_CMD59_CRC_ON_OFF (59 | SDMMC_CMD_R1)

/*
 * --- Block-oriented read commands (class 2) ---
 */
/** Cmd16(ac, R1): Set the block length (in bytes) */
#define SDMMC_CMD16_SET_BLOCKLEN (16 | SDMMC_CMD_R1)
/** Cmd17(adtc, R1): Read single block */
#define SDMMC_CMD17_READ_SINGLE_BLOCK (17 | SDMMC_CMD_R1 | SDMMC_CMD_SINGLE_BLOCK)
/** Cmd18(adtc, R1): Read multiple block */
#define SDMMC_CMD18_READ_MULTIPLE_BLOCK (18 | SDMMC_CMD_R1 | SDMMC_CMD_MULTI_BLOCK)

/*
 * --- Sequential write commands (class 3) ---
 */

/**
 * MMC Cmd20(adtc, R1): Write a data stream from the host, starting at the
 * given address, until a STOP_TRANSMISSION follows.
 */
#define MMC_CMD20_WRITE_DAT_UNTIL_STOP (20 | SDMMC_CMD_R1)

/*
 * --- Block-oriented write commands (class 4) ---
 */
/** MMC Cmd23(ac, R1): Set block count */
#define MMC_CMD23_SET_BLOCK_COUNT (23 | SDMMC_CMD_R1)
/** Cmd24(adtc, R1): Write block */
#define SDMMC_CMD24_WRITE_BLOCK (24 | SDMMC_CMD_R1 | SDMMC_CMD_WRITE | SDMMC_CMD_SINGLE_BLOCK)
/** Cmd25(adtc, R1): Write multiple block */
#define SDMMC_CMD25_WRITE_MULTIPLE_BLOCK (25 | SDMMC_CMD_R1 | SDMMC_CMD_WRITE | SDMMC_CMD_MULTI_BLOCK)
/** MMC Cmd26(adtc, R1): Programming of the card identification register. */
#define MMC_CMD26_PROGRAM_CID (26 | SDMMC_CMD_R1)
/** Cmd27(adtc, R1): Programming of the programmable bits of the CSD. */
#define SDMMC_CMD27_PROGRAM_CSD (27 | SDMMC_CMD_R1)

/*
 * --- Erase commands  (class 5) ---
 */
/** SD Cmd32(ac, R1): */
#define SD_CMD32_ERASE_WR_BLK_START (32 | SDMMC_CMD_R1)
/** SD Cmd33(ac, R1): */
#define SD_CMD33_ERASE_WR_BLK_END (33 | SDMMC_CMD_R1)
/** MMC Cmd35(ac, R1): */
#define MMC_CMD35_ERASE_GROUP_START (35 | SDMMC_CMD_R1)
/** MMC Cmd36(ac, R1): */
#define MMC_CMD36_ERASE_GROUP_END (36 | SDMMC_CMD_R1)
/** Cmd38(ac, R1B): */
#define SDMMC_CMD38_ERASE (38 | SDMMC_CMD_R1B)

/*
 * --- Block Oriented Write Protection Commands (class 6) ---
 */
/** Cmd28(ac, R1b): Set write protection */
#define SDMMC_CMD28_SET_WRITE_PROT (28 | SDMMC_CMD_R1B)
/** Cmd29(ac, R1b): Clr write protection */
#define SDMMC_CMD29_CLR_WRITE_PROT (29 | SDMMC_CMD_R1B)
/** Cmd30(adtc, R1b): Send write protection */
#define SDMMC_CMD30_SEND_WRITE_PROT (30 | SDMMC_CMD_R1)

/*
 * --- Lock Card (class 7) ---
 */
/** Cmd42(adtc, R1): Used to set/reset the password or lock/unlock the card. */
#define SDMMC_CMD42_LOCK_UNLOCK (42 | SDMMC_CMD_R1)

/*
 * --- Application-specific commands (class 8) ---
 */
/**
 * Cmd55(ac, R1): Indicate to the card that the next command is an application
 * specific command rather than a standard command.
 */
#define SDMMC_CMD55_APP_CMD (55 | SDMMC_CMD_R1)
/**
 * Cmd 56(adtc, R1): Used either to transfer a data block to the card or to get
 * a data block from the card for general purpose/application specific commands.
 */
#define SDMMC_CMD56_GEN_CMD (56 | SDMMC_CMD_R1)

/**
 * MMC Cmd6(ac, R1b) : Switche the mode of operation of the selected card
 * or modifies the EXT_CSD registers.
 */
#define MMC_CMD6_SWITCH (6 | SDMMC_CMD_R1B)
/**
 * SD Cmd6(adtc, R1) : Check switchable function (mode 0)
 * and switch card function (mode 1).
 */
#define SD_CMD6_SWITCH_FUNC (6 | SDMMC_CMD_R1 | SDMMC_CMD_SINGLE_BLOCK)
/** ACMD6(ac, R1): Define the data bus width */
#define SD_ACMD6_SET_BUS_WIDTH (6 | SDMMC_CMD_R1)
/** ACMD13(adtc, R1): Send the SD Status. */
#define SD_ACMD13_SD_STATUS (13 | SDMMC_CMD_R1)
/**
 * ACMD22(adtc, R1): Send the number of the written (with-out errors) write
 * blocks.
 */
#define SD_ACMD22_SEND_NUM_WR_BLOCKS (22 | SDMMC_CMD_R1)
/**
 * ACMD23(ac, R1): Set the number of write blocks to be pre-erased before
 * writing
 */
#define SD_ACMD23_SET_WR_BLK_ERASE_COUNT (23 | SDMMC_CMD_R1)
/**
 * ACMD41(bcr, R3): Send host capacity support information (HCS) and asks the
 * accessed card to send its operating condition register (OCR) content
 * in the response
 */
#define SD_MCI_ACMD41_SD_SEND_OP_COND (41 | SDMMC_CMD_R3 | SDMMC_CMD_OPENDRAIN)
/**
 * ACMD41(R1): Send host capacity support information (HCS) and activates the
 * card's initilization process
 */
#define SD_SPI_ACMD41_SD_SEND_OP_COND (41 | SDMMC_CMD_R1)
/**
 * ACMD42(ac, R1): Connect[1]/Disconnect[0] the 50 KOhm pull-up resistor on
 * CD/DAT3 (pin 1) of the card.
 */
#define SD_ACMD42_SET_CLR_CARD_DETECT (42 | SDMMC_CMD_R1)
/** ACMD51(adtc, R1): Read the SD Configuration Register (SCR). */
#define SD_ACMD51_SEND_SCR (51 | SDMMC_CMD_R1 | SDMMC_CMD_SINGLE_BLOCK)

/*
 * --- I/O mode commands (class 9) ---
 */
/** MMC Cmd39(ac, R4): Used to write and read 8 bit (register) data fields. */
#define MMC_CMD39_FAST_IO (39 | SDMMC_CMD_R4)
/** MMC Cmd40(bcr, R5): Set the system into interrupt mode */
#define MMC_CMD40_GO_IRQ_STATE (40 | SDMMC_CMD_R5 | SDMMC_CMD_OPENDRAIN)
/** SDIO Cmd5(R4): Send operation condition */
#define SDIO_CMD5_SEND_OP_COND (5 | SDMMC_CMD_R4 | SDMMC_CMD_OPENDRAIN)
/** SDIO CMD52(R5): Direct IO read/write */
#define SDIO_CMD52_IO_RW_DIRECT (52 | SDMMC_CMD_R5)
/** SDIO CMD53(R5): Extended IO read/write */
#define SDIO_CMD53_IO_R_BYTE_EXTENDED (53 | SDMMC_CMD_R5 | SDMMC_CMD_SDIO_BYTE)
#define SDIO_CMD53_IO_W_BYTE_EXTENDED (53 | SDMMC_CMD_R5 | SDMMC_CMD_SDIO_BYTE | SDMMC_CMD_WRITE)
#define SDIO_CMD53_IO_R_BLOCK_EXTENDED (53 | SDMMC_CMD_R5 | SDMMC_CMD_SDIO_BLOCK)
#define SDIO_CMD53_IO_W_BLOCK_EXTENDED (53 | SDMMC_CMD_R5 | SDMMC_CMD_SDIO_BLOCK | SDMMC_CMD_WRITE)
//! @}
//! @}

//! \name Macros for command argument definition
//! @{

//! \name MMC CMD6 argument structure
//! @{
//! [31:26] Set to 0
//! [25:24] Access
#define MMC_CMD6_ACCESS_COMMAND_SET (0lu << 24)
#define MMC_CMD6_ACCESS_SET_BITS (1lu << 24)
#define MMC_CMD6_ACCESS_CLEAR_BITS (2lu << 24)
#define MMC_CMD6_ACCESS_WRITE_BYTE (3lu << 24)
//! [23:16] Index for Mode Segment
#define MMC_CMD6_INDEX_CMD_SET (EXT_CSD_CMD_SET_INDEX << 16)
#define MMC_CMD6_INDEX_CMD_SET_REV (EXT_CSD_CMD_SET_REV_INDEX << 16)
#define MMC_CMD6_INDEX_POWER_CLASS (EXT_CSD_POWER_CLASS_INDEX << 16)
#define MMC_CMD6_INDEX_HS_TIMING (EXT_CSD_HS_TIMING_INDEX << 16)
#define MMC_CMD6_INDEX_BUS_WIDTH (EXT_CSD_BUS_WIDTH_INDEX << 16)
#define MMC_CMD6_INDEX_ERASED_MEM_CONT (EXT_CSD_ERASED_MEM_CONT_INDEX << 16)
#define MMC_CMD6_INDEX_BOOT_CONFIG (EXT_CSD_BOOT_CONFIG_INDEX << 16)
#define MMC_CMD6_INDEX_BOOT_BUS_WIDTH (EXT_CSD_BOOT_BUS_WIDTH_INDEX << 16)
#define MMC_CMD6_INDEX_ERASE_GROUP_DEF (EXT_CSD_ERASE_GROUP_DEF_INDEX << 16)
//! [15:8] Value
#define MMC_CMD6_VALUE_BUS_WIDTH_1BIT (0x0lu << 8)
#define MMC_CMD6_VALUE_BUS_WIDTH_4BIT (0x1lu << 8)
#define MMC_CMD6_VALUE_BUS_WIDTH_8BIT (0x2lu << 8)
#define MMC_CMD6_VALUE_HS_TIMING_ENABLE (0x1lu << 8)
#define MMC_CMD6_VALUE_HS_TIMING_DISABLE (0x0lu << 8)
//! [7:3] Set to 0
//! [2:0] Cmd Set
//! @}

//! \name SD CMD6 argument structure
//! @{
//! CMD6 arg[ 3: 0] function group 1, access mode
#define SD_CMD6_GRP1_HIGH_SPEED (0x1lu << 0)
#define SD_CMD6_GRP1_DEFAULT (0x0lu << 0)
//! CMD6 arg[ 7: 4] function group 2, command system
#define SD_CMD6_GRP2_NO_INFLUENCE (0xFlu << 4)
#define SD_CMD6_GRP2_DEFAULT (0x0lu << 4)
//! CMD6 arg[11: 8] function group 3, 0xF or 0x0
#define SD_CMD6_GRP3_NO_INFLUENCE (0xFlu << 8)
#define SD_CMD6_GRP3_DEFAULT (0x0lu << 8)
//! CMD6 arg[15:12] function group 4, 0xF or 0x0
#define SD_CMD6_GRP4_NO_INFLUENCE (0xFlu << 12)
#define SD_CMD6_GRP4_DEFAULT (0x0lu << 12)
//! CMD6 arg[19:16] function group 5, 0xF or 0x0
#define SD_CMD6_GRP5_NO_INFLUENCE (0xFlu << 16)
#define SD_CMD6_GRP5_DEFAULT (0x0lu << 16)
//! CMD6 arg[23:20] function group 6, 0xF or 0x0
#define SD_CMD6_GRP6_NO_INFLUENCE (0xFlu << 20)
#define SD_CMD6_GRP6_DEFAULT (0x0lu << 20)
//! CMD6 arg[30:24] reserved 0
//! CMD6 arg[31   ] Mode, 0: Check, 1: Switch
#define SD_CMD6_MODE_CHECK (0lu << 31)
#define SD_CMD6_MODE_SWITCH (1lu << 31)
//! @}

//! \name SD CMD8 argument structure
//! @{
#define SD_CMD8_PATTERN 0xAA
#define SD_CMD8_MASK_PATTERN 0xFF
#define SD_CMD8_HIGH_VOLTAGE 0x100
#define SD_CMD8_MASK_VOLTAGE 0xF00
//! @}

//! \name SD ACMD41 arguments
//! @{
#define SD_ACMD41_HCS (1lu << 30) //!< (SD) Host Capacity Support
                                  //! @}
//! @}

//! \name SDIO definitions
//! @{

//! \name SDIO state (in R5)
//! @{
#define SDIO_R5_COM_CRC_ERROR (1lu << 15)                                            /**< CRC check error */
#define SDIO_R5_ILLEGAL_COMMAND (1lu << 14)                                          /**< Illegal command */
#define SDIO_R5_STATE (3lu << 12)                                                    /**< SDIO R5 state mask */
#define SDIO_R5_STATE_DIS (0lu << 12)                                                /**< Disabled */
#define SDIO_R5_STATE_CMD (1lu << 12)                                                /**< DAT lines free */
#define SDIO_R5_STATE_TRN (2lu << 12)                                                /**< Transfer */
#define SDIO_R5_STATE_RFU (3lu << 12)                                                /**< Reserved */
#define SDIO_R5_ERROR (1lu << 11)                                                    /**< General error */
#define SDIO_R5_FUNC_NUM (1lu << 9)                                                  /**< Invalid function number */
#define SDIO_R5_OUT_OF_RANGE (1lu << 8)                                              /**< Argument out of range */
#define SDIO_R5_STATUS_ERR (SDIO_R5_ERROR | SDIO_R5_FUNC_NUM | SDIO_R5_OUT_OF_RANGE) //!< Errro status bits mask
                                                                                     //! @}

//! \name SDIO state (in R6)
//! @{
/** The CRC check of the previous command failed. */
#define SDIO_R6_COM_CRC_ERROR (1lu << 15)
/** Command not legal for the card state. */
#define SDIO_R6_ILLEGAL_COMMAND (1lu << 14)
/** A general or an unknown error occurred during the operation. */
#define SDIO_R6_ERROR (1lu << 13)
/** Status bits mask for SDIO R6 */
#define SDIO_STATUS_R6 (SDIO_R6_COM_CRC_ERROR | SDIO_R6_ILLEGAL_COMMAND | SDIO_R6_ERROR)
//! @}

//! \name SDIO CMD52 argument bit offset
//! @{
//! CMD52 arg[ 7: 0] Write data or stuff bits
#define SDIO_CMD52_WR_DATA 0
//! CMD52 arg[    8] Reserved
#define SDIO_CMD52_STUFF0 8
//! CMD52 arg[25: 9] Register address
#define SDIO_CMD52_REG_ADRR 9
//! CMD52 arg[   26] Reserved
#define SDIO_CMD52_STUFF1 26
//! CMD52 arg[   27] Read after Write flag
#define SDIO_CMD52_RAW_FLAG 27
//! CMD52 arg[30:28] Number of the function
#define SDIO_CMD52_FUNCTION_NUM 28
//! CMD52 arg[   31] Direction, 1:write, 0:read.
#define SDIO_CMD52_RW_FLAG 31
#define SDIO_CMD52_READ_FLAG 0
#define SDIO_CMD52_WRITE_FLAG 1
//! @}

//! \name SDIO CMD53 argument structure
//! @{
/**
 * [ 8: 0] Byte mode: number of bytes to transfer,
 *                    0 cause 512 bytes transfer.
 *         Block mode: number of blocks to transfer,
 *                    0 set count to infinite.
 */
#define SDIO_CMD53_COUNT 0
//! CMD53 arg[25: 9] Start Address I/O register
#define SDIO_CMD53_REG_ADDR 9
//! CMD53 arg[   26] 1:Incrementing address, 0: fixed
#define SDIO_CMD53_OP_CODE 26
//! CMD53 arg[   27] (Optional) 1:block mode
#define SDIO_CMD53_BLOCK_MODE 27
//! CMD53 arg[30:28] Number of the function
#define SDIO_CMD53_FUNCTION_NUM 28
//! CMD53 arg[   31] Direction, 1:WR, 0:RD
#define SDIO_CMD53_RW_FLAG 31
#define SDIO_CMD53_READ_FLAG 0
#define SDIO_CMD53_WRITE_FLAG 1
//! @}

//! \name SDIO Functions
//! @{
#define SDIO_CIA 0 /**< SDIO Function 0 (CIA) */
#define SDIO_FN0 0 /**< SDIO Function 0 */
#define SDIO_FN1 1 /**< SDIO Function 1 */
#define SDIO_FN2 2 /**< SDIO Function 2 */
#define SDIO_FN3 3 /**< SDIO Function 3 */
#define SDIO_FN4 4 /**< SDIO Function 4 */
#define SDIO_FN5 5 /**< SDIO Function 5 */
#define SDIO_FN6 6 /**< SDIO Function 6 */
#define SDIO_FN7 7 /**< SDIO Function 7 */
                   //! @}

//! \name SDIO Card Common Control Registers (CCCR)
//! @{
#define SDIO_CCCR_SDIO_REV 0x00         /**< CCCR/SDIO revision (RO) */
#define SDIO_CCCR_REV (0xFlu << 0)      /**< CCCR/FBR Version */
#define SDIO_CCCR_REV_1_00 (0x0lu << 0) /**< CCCR/FBR Version 1.00 */
#define SDIO_CCCR_REV_1_10 (0x1lu << 0) /**< CCCR/FBR Version 1.10 */
#define SDIO_CCCR_REV_2_00 (0x2lu << 0) /**< CCCR/FBR Version 2.00 */
#define SDIO_CCCR_REV_3_00 (0x3lu << 0) /**< CCCR/FBR Version 3.00 */
#define SDIO_SDIO_REV (0xFlu << 4)      /**< SDIO Spec */
#define SDIO_SDIO_REV_1_00 (0x0lu << 4) /**< SDIO Spec 1.00 */
#define SDIO_SDIO_REV_1_10 (0x1lu << 4) /**< SDIO Spec 1.10 */
#define SDIO_SDIO_REV_1_20 (0x2lu << 4) /**< SDIO Spec 1.20(unreleased) */
#define SDIO_SDIO_REV_2_00 (0x3lu << 4) /**< SDIO Spec Version 2.00 */
#define SDIO_SDIO_REV_3_00 (0x4lu << 4) /**< SDIO Spec Version 3.00 */
#define SDIO_CCCR_SD_REV 0x01           /**< SD Spec Revision (RO) */
#define SDIO_SD_REV (0xFlu << 0)        /**< SD Physical Spec */
#define SDIO_SD_REV_1_01 (0x0lu << 0)   /**< SD 1.01 (Mar 2000) */
#define SDIO_SD_REV_1_10 (0x1lu << 0)   /**< SD 1.10 (Oct 2004) */
#define SDIO_SD_REV_2_00 (0x2lu << 0)   /**< SD 2.00 (May 2006) */
#define SDIO_SD_REV_3_00 (0x3lu << 0)   /**< SD 3.00 */
#define SDIO_CCCR_IOE 0x02              /**< I/O Enable (R/W) */
#define SDIO_IOE (0xFElu << 1)          /**< Functions Enable/Disable */
#define SDIO_IOE_FN1 (0x1lu << 1)       /**< Function 1 Enable/Disable */
#define SDIO_IOE_FN2 (0x1lu << 2)       /**< Function 2 Enable/Disable */
#define SDIO_IOE_FN3 (0x1lu << 3)       /**< Function 3 Enable/Disable */
#define SDIO_IOE_FN4 (0x1lu << 4)       /**< Function 4 Enable/Disable */
#define SDIO_IOE_FN5 (0x1lu << 5)       /**< Function 5 Enable/Disable */
#define SDIO_IOE_FN6 (0x1lu << 6)       /**< Function 6 Enable/Disable */
#define SDIO_IOE_FN7 (0x1lu << 7)       /**< Function 7 Enable/Disable */
#define SDIO_CCCR_IOR 0x03              /**< I/O Ready (RO) */
#define SDIO_IOR (0xFElu << 1)          /**< Functions ready */
#define SDIO_IOR_FN1 (0x1lu << 1)       /**< Function 1 ready */
#define SDIO_IOR_FN2 (0x1lu << 2)       /**< Function 2 ready */
#define SDIO_IOR_FN3 (0x1lu << 3)       /**< Function 3 ready */
#define SDIO_IOR_FN4 (0x1lu << 4)       /**< Function 4 ready */
#define SDIO_IOR_FN5 (0x1lu << 5)       /**< Function 5 ready */
#define SDIO_IOR_FN6 (0x1lu << 6)       /**< Function 6 ready */
#define SDIO_IOR_FN7 (0x1lu << 7)       /**< Function 7 ready */
#define SDIO_CCCR_IEN 0x04              /**< Int Enable */
#define SDIO_IENM (0x1lu << 0)          /**< Int Enable Master (R/W) */
#define SDIO_IEN (0xFElu << 1)          /**< Functions Int Enable */
#define SDIO_IEN_FN1 (0x1lu << 1)       /**< Function 1 Int Enable */
#define SDIO_IEN_FN2 (0x1lu << 2)       /**< Function 2 Int Enable */
#define SDIO_IEN_FN3 (0x1lu << 3)       /**< Function 3 Int Enable */
#define SDIO_IEN_FN4 (0x1lu << 4)       /**< Function 4 Int Enable */
#define SDIO_IEN_FN5 (0x1lu << 5)       /**< Function 5 Int Enable */
#define SDIO_IEN_FN6 (0x1lu << 6)       /**< Function 6 Int Enable */
#define SDIO_IEN_FN7 (0x1lu << 7)       /**< Function 7 Int Enable */
#define SDIO_CCCR_INT 0x05              /**< Int Pending */
#define SDIO_INT (0xFElu << 1)          /**< Functions Int pending */
#define SDIO_INT_FN1 (0x1lu << 1)       /**< Function 1 Int pending */
#define SDIO_INT_FN2 (0x1lu << 2)       /**< Function 2 Int pending */
#define SDIO_INT_FN3 (0x1lu << 3)       /**< Function 3 Int pending */
#define SDIO_INT_FN4 (0x1lu << 4)       /**< Function 4 Int pending */
#define SDIO_INT_FN5 (0x1lu << 5)       /**< Function 5 Int pending */
#define SDIO_INT_FN6 (0x1lu << 6)       /**< Function 6 Int pending */
#define SDIO_INT_FN7 (0x1lu << 7)       /**< Function 7 Int pending */
#define SDIO_CCCR_IOA 0x06              /**< I/O Abort */
#define SDIO_AS (0x7lu << 0)            /**< Abort Select In Order (WO) */
#define SDIO_AS_FN1 (0x1lu << 0)        /**< Abort function 1 IO */
#define SDIO_AS_FN2 (0x2lu << 0)        /**< Abort function 2 IO */
#define SDIO_AS_FN3 (0x3lu << 0)        /**< Abort function 3 IO */
#define SDIO_AS_FN4 (0x4lu << 0)        /**< Abort function 4 IO */
#define SDIO_AS_FN5 (0x5lu << 0)        /**< Abort function 5 IO */
#define SDIO_AS_FN6 (0x6lu << 0)        /**< Abort function 6 IO */
#define SDIO_AS_FN7 (0x7lu << 0)        /**< Abort function 7 IO */
#define SDIO_RES (0x1lu << 3)           /**< IO CARD RESET (WO) */
#define SDIO_CCCR_BUS_CTRL 0x07         /**< Bus Interface Control */
#define SDIO_BUSWIDTH (0x3lu << 0)      /**< Data bus width (R/W) */
#define SDIO_BUSWIDTH_1B (0x0lu << 0)   /**< 1-bit data bus */
#define SDIO_BUSWIDTH_4B (0x2lu << 0)   /**< 4-bit data bus */
/** Enable Continuous SPI interrupt (R/W) */
#define SDIO_BUS_ECSI (0x1lu << 5)
/** Support Continuous SPI interrupt (RO) */
#define SDIO_BUS_SCSI (0x1lu << 6)
/** Connect(0)/Disconnect(1) pull-up on CD/DAT[3] (R/W) */
#define SDIO_BUS_CD_DISABLE (0x1lu << 7)
#define SDIO_CCCR_CAP 0x08 /**< Card Capability */
/** Support Direct Commands during data transfer (RO) */
#define SDIO_CAP_SDC (0x1lu << 0)
/** Support Multi-Block (RO) */
#define SDIO_CAP_SMB (0x1lu << 1)
/** Support Read Wait (RO) */
#define SDIO_CAP_SRW (0x1lu << 2)
/** Support Suspend/Resume (RO) */
#define SDIO_CAP_SBS (0x1lu << 3)
/** Support interrupt between blocks of data in 4-bit SD mode (RO) */
#define SDIO_CAP_S4MI (0x1lu << 4)
/** Enable interrupt between blocks of data in 4-bit SD mode (R/W) */
#define SDIO_CAP_E4MI (0x1lu << 5)
/** Low-Speed Card (RO) */
#define SDIO_CAP_LSC (0x1lu << 6)
/** 4-bit support for Low-Speed Card (RO) */
#define SDIO_CAP_4BLS (0x1lu << 7)
/** Pointer to CIS (3B, LSB first) */
#define SDIO_CCCR_CIS_PTR 0x09
/** Bus Suspend */
#define SDIO_CCCR_BUS_SUSPEND 0x0C
/** Bus Status (transfer on DAT[x] lines) (RO) */
#define SDIO_BS (0x1lu << 0)
/** Bus Release Request/Status (R/W) */
#define SDIO_BR (0x1lu << 1)
#define SDIO_CCCR_FUN_SEL 0x0D    /**< Function select */
#define SDIO_DF (0x1lu << 7)      /**< Resume Data Flag (RO) */
#define SDIO_FS (0xFlu << 0)      /**< Select Function (R/W) */
#define SDIO_FS_CIA (0x0lu << 0)  /**< Select CIA (function 0) */
#define SDIO_FS_FN1 (0x1lu << 0)  /**< Select Function 1 */
#define SDIO_FS_FN2 (0x2lu << 0)  /**< Select Function 2 */
#define SDIO_FS_FN3 (0x3lu << 0)  /**< Select Function 3 */
#define SDIO_FS_FN4 (0x4lu << 0)  /**< Select Function 4 */
#define SDIO_FS_FN5 (0x5lu << 0)  /**< Select Function 5 */
#define SDIO_FS_FN6 (0x6lu << 0)  /**< Select Function 6 */
#define SDIO_FS_FN7 (0x7lu << 0)  /**< Select Function 7 */
#define SDIO_FS_MEM (0x8lu << 0)  /**< Select memory in combo card */
#define SDIO_CCCR_EXEC 0x0E       /**< Exec Flags (RO) */
#define SDIO_EXM (0x1lu << 0)     /**< Executing status of memory */
#define SDIO_EX (0xFElu << 1)     /**< Executing functions status */
#define SDIO_EX_FN1 (0x1lu << 1)  /**< Executing status of func 1 */
#define SDIO_EX_FN2 (0x1lu << 2)  /**< Executing status of func 2 */
#define SDIO_EX_FN3 (0x1lu << 3)  /**< Executing status of func 3 */
#define SDIO_EX_FN4 (0x1lu << 4)  /**< Executing status of func 4 */
#define SDIO_EX_FN5 (0x1lu << 5)  /**< Executing status of func 5 */
#define SDIO_EX_FN6 (0x1lu << 6)  /**< Executing status of func 6 */
#define SDIO_EX_FN7 (0x1lu << 7)  /**< Executing status of func 7 */
#define SDIO_CCCR_READY 0x0F      /**< Ready Flags (RO) */
#define SDIO_RFM (0x1lu << 0)     /**< Ready Flag for memory */
#define SDIO_RF (0xFElu)          /**< Ready Flag for functions */
#define SDIO_RF_FN1 (0x1lu << 1)  /**< Ready Flag for function 1 */
#define SDIO_RF_FN2 (0x1lu << 2)  /**< Ready Flag for function 2 */
#define SDIO_RF_FN3 (0x1lu << 3)  /**< Ready Flag for function 3 */
#define SDIO_RF_FN4 (0x1lu << 4)  /**< Ready Flag for function 4 */
#define SDIO_RF_FN5 (0x1lu << 5)  /**< Ready Flag for function 5 */
#define SDIO_RF_FN6 (0x1lu << 6)  /**< Ready Flag for function 6 */
#define SDIO_RF_FN7 (0x1lu << 7)  /**< Ready Flag for function 7 */
#define SDIO_CCCR_FN0_BLKSIZ 0x10 /**< FN0 Block Size (2B, LSB first) (R/W) */
#define SDIO_CCCR_POWER 0x12      /**< Power Control */
#define SDIO_SMPC (0x1lu << 0)    /**< Support Master Power Control*/
#define SDIO_EMPC (0x1lu << 1)    /**< Enable Master Power Control */
#define SDIO_CCCR_HS 0x13         /**< High-Speed */
#define SDIO_SHS (0x1lu << 0)     /**< Support High-Speed (RO) */
#define SDIO_EHS (0x1lu << 1)     /**< Enable High-Speed (R/W) */
                                  //! @}

//! \name SDIO Function Basic Registers (FBR)
//! @{
#define SDIO_FBR_ADDR(fn, x) (0x100 * (fn) + (x))
#define SDIO_FBR_CSA_IF 0x0          /**< CSA and function interface code (RO) */
#define SDIO_IFC (0xFUL << 0)        /**< Standard SDIO Fun Interface Code */
#define SDIO_IFC_NO_IF (0x0UL << 0)  /**< No SDIO standard interface */
#define SDIO_IFC_UART (0x1UL << 0)   /**< UART */
#define SDIO_IFC_TA_BT (0x2UL << 0)  /**< Type-A Bluetooth */
#define SDIO_IFC_TB_BT (0x3UL << 0)  /**< Type-B Bluetooth */
#define SDIO_IFC_GPS (0x4UL << 0)    /**< GPS */
#define SDIO_IFC_CAMERA (0x5UL << 0) /**< Camera */
#define SDIO_IFC_PHS (0x6UL << 0)    /**< PHS */
#define SDIO_IFC_WLAN (0x7UL << 0)   /**< WLAN */
#define SDIO_IFC_ATA (0x8UL << 0)    /**< Embedded SDIO-ATA */
#define SDIO_IFC_EXT (0xFUL << 0)    /**< Check EXT interface code */
#define SDIO_SCSA (0x1UL << 6)       /**< Function supports Code Storage Area (CSA) */
#define SDIO_FBR_CSA (0x1UL << 7)    /**< Function CSA enable */
#define SDIO_FBR_EXT_IF 0x1          /**< Extended function interface code (RO) */
#define SDIO_FBR_PWR 0x2             /**< function power control */
#define SDIO_SPS (0x1UL << 0)        /**< function support power selection (RO) */
#define SDIO_EPS (0x1UL << 1)        /**< Low Current Mode/High Current Mode (R/W) */
#define SDIO_FBR_CIS_PTR 0x9         /**< Address pointer to function CIS (3B, LSB first) (RO) */
#define SDIO_FBR_CSA_PTR 0xC         /**< Address pointer to CSA (3B, LSB first) (R/W) */
#define SDIO_FBR_CSA_DATA 0xF        /**< Read/Write fifo to CSA (R/W) */
#define SDIO_FBR_BLK_SIZ 0x10        /**< Block size (2B, LSB first) (R/W) */
                                     //! @}

//! \name SDIO Card Metaformat
//! @{
/** Null tuple (PCMCIA 3.1.9) */
#define SDIO_CISTPL_NULL 0x00
/** Device tuple (PCMCIA 3.2.2) */
#define SDIO_CISTPL_DEVICE 0x01
/** Checksum control (PCMCIA 3.1.1) */
#define SDIO_CISTPL_CHECKSUM 0x10
/** Level 1 version (PCMCIA 3.2.10) */
#define SDIO_CISTPL_VERS_1 0x15
/** Alternate Language String (PCMCIA 3.2.1) */
#define SDIO_CISTPL_ALTSTR 0x16
/** Manufacturer Identification String (PCMCIA 3.2.9) */
#define SDIO_CISTPL_MANFID 0x20
/** Function Identification (PCMCIA 3.2.7) */
#define SDIO_CISTPL_FUNCID 0x21
/** Function Extensions (PCMCIA 3.2.6) */
#define SDIO_CISTPL_FUNCE 0x22
/** Additional information for SDIO (PCMCIA 6.1.2) */
#define SDIO_CISTPL_SDIO_STD 0x91
/** Reserved for future SDIO (PCMCIA 6.1.3) */
#define SDIO_CISTPL_SDIO_EXT 0x92
/** The End-of-chain Tuple (PCMCIA 3.1.2) */
#define SDIO_CISTPL_END 0xFF
//! @}

//! @}

//! \name CSD, OCR, SCR, Switch status, extend CSD definitions
//! @{

/**
 * \brief Macro function to extract a bits field from a large SD MMC register
 * Used by : CSD, SCR, Switch status
 */
static inline uint32_t SDMMC_UNSTUFF_BITS(uint8_t *reg, uint16_t reg_size, uint16_t pos, uint8_t size)
{
	uint32_t value;
	value = reg[((reg_size - pos + 7) / 8) - 1] >> (pos % 8);
	if (((pos % 8) + size) > 8) {
		value |= (uint32_t)reg[((reg_size - pos + 7) / 8) - 2] << (8 - (pos % 8));
	}
	if (((pos % 8) + size) > 16) {
		value |= (uint32_t)reg[((reg_size - pos + 7) / 8) - 3] << (16 - (pos % 8));
	}
	if (((pos % 8) + size) > 24) {
		value |= (uint32_t)reg[((reg_size - pos + 7) / 8) - 3] << (24 - (pos % 8));
	}
	value &= ((uint32_t)1 << size) - 1;
	return value;
}

	//! \name CSD Fields
	//! @{
#define CSD_REG_BIT_SIZE 128                 //!< 128 bits
#define CSD_REG_BSIZE (CSD_REG_BIT_SIZE / 8) //!< 16 bytes
#define CSD_STRUCTURE(csd, pos, size) SDMMC_UNSTUFF_BITS(csd, CSD_REG_BIT_SIZE, pos, size)
#define CSD_STRUCTURE_VERSION(csd) CSD_STRUCTURE(csd, 126, 2)
#define SD_CSD_VER_1_0 0
#define SD_CSD_VER_2_0 1
#define MMC_CSD_VER_1_0 0
#define MMC_CSD_VER_1_1 1
#define MMC_CSD_VER_1_2 2
#define CSD_TRAN_SPEED(csd) CSD_STRUCTURE(csd, 96, 8)
#define SD_CSD_1_0_C_SIZE(csd) CSD_STRUCTURE(csd, 62, 12)
#define SD_CSD_1_0_C_SIZE_MULT(csd) CSD_STRUCTURE(csd, 47, 3)
#define SD_CSD_1_0_READ_BL_LEN(csd) CSD_STRUCTURE(csd, 80, 4)
#define SD_CSD_2_0_C_SIZE(csd) CSD_STRUCTURE(csd, 48, 22)
#define MMC_CSD_C_SIZE(csd) CSD_STRUCTURE(csd, 62, 12)
#define MMC_CSD_C_SIZE_MULT(csd) CSD_STRUCTURE(csd, 47, 3)
#define MMC_CSD_READ_BL_LEN(csd) CSD_STRUCTURE(csd, 80, 4)
#define MMC_CSD_SPEC_VERS(csd) CSD_STRUCTURE(csd, 122, 4)
	//! @}

	//! \name OCR Register Fields
	//! @{
#define OCR_REG_BSIZE (32 / 8) /**< 32 bits, 4 bytes */
#define OCR_VDD_170_195 (1lu << 7)
#define OCR_VDD_20_21 (1lu << 8)
#define OCR_VDD_21_22 (1lu << 9)
#define OCR_VDD_22_23 (1lu << 10)
#define OCR_VDD_23_24 (1lu << 11)
#define OCR_VDD_24_25 (1lu << 12)
#define OCR_VDD_25_26 (1lu << 13)
#define OCR_VDD_26_27 (1lu << 14)
#define OCR_VDD_27_28 (1lu << 15)
#define OCR_VDD_28_29 (1lu << 16)
#define OCR_VDD_29_30 (1lu << 17)
#define OCR_VDD_30_31 (1lu << 18)
#define OCR_VDD_31_32 (1lu << 19)
#define OCR_VDD_32_33 (1lu << 20)
#define OCR_VDD_33_34 (1lu << 21)
#define OCR_VDD_34_35 (1lu << 22)
#define OCR_VDD_35_36 (1lu << 23)
#define OCR_SDIO_S18R (1lu << 24)          /**< Switching to 1.8V Accepted */
#define OCR_SDIO_MP (1lu << 27)            /**< Memory Present */
#define OCR_SDIO_NF (7lu << 28)            /**< Number of I/O Functions */
#define OCR_ACCESS_MODE_MASK (3lu << 29)   /**< (MMC) Access mode mask */
#define OCR_ACCESS_MODE_BYTE (0lu << 29)   /**< (MMC) Byte access mode */
#define OCR_ACCESS_MODE_SECTOR (2lu << 29) /**< (MMC) Sector access mode */
#define OCR_CCS (1lu << 30)                /**< (SD) Card Capacity Status */
#define OCR_POWER_UP_BUSY (1lu << 31)      /**< Card power up status bit */
	                                       //! @}

	//! \name SD SCR Register Fields
	//! @{
#define SD_SCR_REG_BIT_SIZE 64                     //!< 64 bits
#define SD_SCR_REG_BSIZE (SD_SCR_REG_BIT_SIZE / 8) //!< 8 bytes
#define SD_SCR_STRUCTURE(scr, pos, size) SDMMC_UNSTUFF_BITS(scr, SD_SCR_REG_BIT_SIZE, pos, size)
#define SD_SCR_SCR_STRUCTURE(scr) SD_SCR_STRUCTURE(scr, 60, 4)
#define SD_SCR_SCR_STRUCTURE_1_0 0
#define SD_SCR_SD_SPEC(scr) SD_SCR_STRUCTURE(scr, 56, 4)
#define SD_SCR_SD_SPEC_1_0_01 0
#define SD_SCR_SD_SPEC_1_10 1
#define SD_SCR_SD_SPEC_2_00 2
#define SD_SCR_DATA_STATUS_AFTER_ERASE(scr) SD_SCR_STRUCTURE(scr, 55, 1)
#define SD_SCR_SD_SECURITY(scr) SD_SCR_STRUCTURE(scr, 52, 3)
#define SD_SCR_SD_SECURITY_NO 0
#define SD_SCR_SD_SECURITY_NOTUSED 1
#define SD_SCR_SD_SECURITY_1_01 2
#define SD_SCR_SD_SECURITY_2_00 3
#define SD_SCR_SD_SECURITY_3_00 4
#define SD_SCR_SD_BUS_WIDTHS(scr) SD_SCR_STRUCTURE(scr, 48, 4)
#define SD_SCR_SD_BUS_WIDTH_1BITS (1lu << 0)
#define SD_SCR_SD_BUS_WIDTH_4BITS (1lu << 2)
#define SD_SCR_SD_SPEC3(scr) SD_SCR_STRUCTURE(scr, 47, 1)
#define SD_SCR_SD_SPEC_3_00 1
#define SD_SCR_SD_EX_SECURITY(scr) SD_SCR_STRUCTURE(scr, 43, 4)
#define SD_SCR_SD_CMD_SUPPORT(scr) SD_SCR_STRUCTURE(scr, 32, 2)
	//! @}

	//! \name SD Switch Status Fields
	//! @{
#define SD_SW_STATUS_BIT_SIZE 512                      //!< 512 bits
#define SD_SW_STATUS_BSIZE (SD_SW_STATUS_BIT_SIZE / 8) //!< 64 bytes
#define SD_SW_STATUS_STRUCTURE(sd_sw_status, pos, size)                                                                \
	SDMMC_UNSTUFF_BITS(sd_sw_status, SD_SW_STATUS_BIT_SIZE, pos, size)
#define SD_SW_STATUS_MAX_CURRENT_CONSUMPTION(status) SD_SW_STATUS_STRUCTURE(status, 496, 16)
#define SD_SW_STATUS_FUN_GRP6_INFO(status) SD_SW_STATUS_STRUCTURE(status, 480, 16)
#define SD_SW_STATUS_FUN_GRP5_INFO(status) SD_SW_STATUS_STRUCTURE(status, 464, 16)
#define SD_SW_STATUS_FUN_GRP4_INFO(status) SD_SW_STATUS_STRUCTURE(status, 448, 16)
#define SD_SW_STATUS_FUN_GRP3_INFO(status) SD_SW_STATUS_STRUCTURE(status, 432, 16)
#define SD_SW_STATUS_FUN_GRP2_INFO(status) SD_SW_STATUS_STRUCTURE(status, 416, 16)
#define SD_SW_STATUS_FUN_GRP1_INFO(status) SD_SW_STATUS_STRUCTURE(status, 400, 16)
#define SD_SW_STATUS_FUN_GRP6_RC(status) SD_SW_STATUS_STRUCTURE(status, 396, 4)
#define SD_SW_STATUS_FUN_GRP5_RC(status) SD_SW_STATUS_STRUCTURE(status, 392, 4)
#define SD_SW_STATUS_FUN_GRP4_RC(status) SD_SW_STATUS_STRUCTURE(status, 388, 4)
#define SD_SW_STATUS_FUN_GRP3_RC(status) SD_SW_STATUS_STRUCTURE(status, 384, 4)
#define SD_SW_STATUS_FUN_GRP2_RC(status) SD_SW_STATUS_STRUCTURE(status, 380, 4)
#define SD_SW_STATUS_FUN_GRP1_RC(status) SD_SW_STATUS_STRUCTURE(status, 376, 4)
#define SD_SW_STATUS_FUN_GRP_RC_ERROR 0xFU
#define SD_SW_STATUS_DATA_STRUCT_VER(status) SD_SW_STATUS_STRUCTURE(status, 368, 8)
#define SD_SW_STATUS_FUN_GRP6_BUSY(status) SD_SW_STATUS_STRUCTURE(status, 352, 16)
#define SD_SW_STATUS_FUN_GRP5_BUSY(status) SD_SW_STATUS_STRUCTURE(status, 336, 16)
#define SD_SW_STATUS_FUN_GRP4_BUSY(status) SD_SW_STATUS_STRUCTURE(status, 320, 16)
#define SD_SW_STATUS_FUN_GRP3_BUSY(status) SD_SW_STATUS_STRUCTURE(status, 304, 16)
#define SD_SW_STATUS_FUN_GRP2_BUSY(status) SD_SW_STATUS_STRUCTURE(status, 288, 16)
#define SD_SW_STATUS_FUN_GRP1_BUSY(status) SD_SW_STATUS_STRUCTURE(status, 272, 16)
	//! @}

	//! \name Card Status Fields
	//! @{
#define CARD_STATUS_APP_CMD (1lu << 5)
#define CARD_STATUS_SWITCH_ERROR (1lu << 7)
#define CARD_STATUS_READY_FOR_DATA (1lu << 8)
#define CARD_STATUS_STATE_IDLE (0lu << 9)
#define CARD_STATUS_STATE_READY (1lu << 9)
#define CARD_STATUS_STATE_IDENT (2lu << 9)
#define CARD_STATUS_STATE_STBY (3lu << 9)
#define CARD_STATUS_STATE_TRAN (4lu << 9)
#define CARD_STATUS_STATE_DATA (5lu << 9)
#define CARD_STATUS_STATE_RCV (6lu << 9)
#define CARD_STATUS_STATE_PRG (7lu << 9)
#define CARD_STATUS_STATE_DIS (8lu << 9)
#define CARD_STATUS_STATE (0xFlu << 9)
#define CARD_STATUS_ERASE_RESET (1lu << 13)
#define CARD_STATUS_WP_ERASE_SKIP (1lu << 15)
#define CARD_STATUS_CIDCSD_OVERWRITE (1lu << 16)
#define CARD_STATUS_OVERRUN (1lu << 17)
#define CARD_STATUS_UNERRUN (1lu << 18)
#define CARD_STATUS_ERROR (1lu << 19)
#define CARD_STATUS_CC_ERROR (1lu << 20)
#define CARD_STATUS_CARD_ECC_FAILED (1lu << 21)
#define CARD_STATUS_ILLEGAL_COMMAND (1lu << 22)
#define CARD_STATUS_COM_CRC_ERROR (1lu << 23)
#define CARD_STATUS_UNLOCK_FAILED (1lu << 24)
#define CARD_STATUS_CARD_IS_LOCKED (1lu << 25)
#define CARD_STATUS_WP_VIOLATION (1lu << 26)
#define CARD_STATUS_ERASE_PARAM (1lu << 27)
#define CARD_STATUS_ERASE_SEQ_ERROR (1lu << 28)
#define CARD_STATUS_BLOCK_LEN_ERROR (1lu << 29)
#define CARD_STATUS_ADDRESS_MISALIGN (1lu << 30)
#define CARD_STATUS_ADDR_OUT_OF_RANGE (1lu << 31)

#define CARD_STATUS_ERR_RD_WR                                                                                          \
	(CARD_STATUS_ADDR_OUT_OF_RANGE | CARD_STATUS_ADDRESS_MISALIGN | CARD_STATUS_BLOCK_LEN_ERROR                        \
	 | CARD_STATUS_WP_VIOLATION | CARD_STATUS_ILLEGAL_COMMAND | CARD_STATUS_CC_ERROR | CARD_STATUS_ERROR)
	//! @}

	//! \name SD Status Field
	//! @{
#define SD_STATUS_BSIZE (512 / 8) /**< 512 bits, 64bytes */
	                              //! @}

	//! \name MMC Extended CSD Register Field
	//! @{
#define EXT_CSD_BSIZE 512 /**< 512 bytes. */
/* Below belongs to Properties Segment */
#define EXT_CSD_S_CMD_SET_INDEX 504lu
#define EXT_CSD_BOOT_INFO_INDEX 228lu
#define EXT_CSD_BOOT_SIZE_MULTI_INDEX 226lu
#define EXT_CSD_ACC_SIZE_INDEX 225lu
#define EXT_CSD_HC_ERASE_GRP_SIZE_INDEX 224lu
#define EXT_CSD_ERASE_TIMEOUT_MULT_INDEX 223lu
#define EXT_CSD_REL_WR_SEC_C_INDEX 222lu
#define EXT_CSD_HC_WP_GRP_SIZE_INDEX 221lu
#define EXT_CSD_S_C_VCC_INDEX 220lu
#define EXT_CSD_S_C_VCCQ_INDEX 219lu
#define EXT_CSD_S_A_TIMEOUT_INDEX 217lu
#define EXT_CSD_SEC_COUNT_INDEX 212lu
#define EXT_CSD_MIN_PERF_W_8_52_INDEX 210lu
#define EXT_CSD_MIN_PERF_R_8_52_INDEX 209lu
#define EXT_CSD_MIN_PERF_W_8_26_4_52_INDEX 208lu
#define EXT_CSD_MIN_PERF_R_8_26_4_52_INDEX 207lu
#define EXT_CSD_MIN_PERF_W_4_26_INDEX 206lu
#define EXT_CSD_MIN_PERF_R_4_26_INDEX 205lu
#define EXT_CSD_PWR_CL_26_360_INDEX 203lu
#define EXT_CSD_PWR_CL_52_360_INDEX 202lu
#define EXT_CSD_PWR_CL_26_195_INDEX 201lu
#define EXT_CSD_PWR_CL_52_195_INDEX 200lu
#define EXT_CSD_CARD_TYPE_INDEX 196lu
/* MMC card type */
#define MMC_CTYPE_26MHZ 0x1
#define MMC_CTYPE_52MHZ 0x2
#define EXT_CSD_CSD_STRUCTURE_INDEX 194lu
#define EXT_CSD_EXT_CSD_REV_INDEX 192lu

/* Below belongs to Mode Segment */
#define EXT_CSD_CMD_SET_INDEX 191lu
#define EXT_CSD_CMD_SET_REV_INDEX 189lu
#define EXT_CSD_POWER_CLASS_INDEX 187lu
#define EXT_CSD_HS_TIMING_INDEX 185lu
#define EXT_CSD_BUS_WIDTH_INDEX 183lu
#define EXT_CSD_ERASED_MEM_CONT_INDEX 181lu
#define EXT_CSD_BOOT_CONFIG_INDEX 179lu
#define EXT_CSD_BOOT_BUS_WIDTH_INDEX 177lu
#define EXT_CSD_ERASE_GROUP_DEF_INDEX 175lu
	//! @}
//! @}

//! \name Definition for SPI mode only
//! @{

//! SPI commands start with a start bit "0" and a transmit bit "1"
#define SPI_CMD_ENCODE(x) (0x40 | (x & 0x3F))

//! \name Register R1 definition for SPI mode
//! The R1 register is always send after a command.
//! @{
#define R1_SPI_IDLE (1lu << 0)
#define R1_SPI_ERASE_RESET (1lu << 1)
#define R1_SPI_ILLEGAL_COMMAND (1lu << 2)
#define R1_SPI_COM_CRC (1lu << 3)
#define R1_SPI_ERASE_SEQ (1lu << 4)
#define R1_SPI_ADDRESS (1lu << 5)
#define R1_SPI_PARAMETER (1lu << 6)
// R1 bit 7 is always zero, reuse this bit for error
#define R1_SPI_ERROR (1lu << 7)
//! @}

//! \name Register R2 definition for SPI mode
//! The R2 register can be send after R1 register.
//! @{
#define R2_SPI_CARD_LOCKED (1lu << 0)
#define R2_SPI_WP_ERASE_SKIP (1lu << 1)
#define R2_SPI_LOCK_UNLOCK_FAIL R2_SPI_WP_ERASE_SKIP
#define R2_SPI_ERROR (1lu << 2)
#define R2_SPI_CC_ERROR (1lu << 3)
#define R2_SPI_CARD_ECC_ERROR (1lu << 4)
#define R2_SPI_WP_VIOLATION (1lu << 5)
#define R2_SPI_ERASE_PARAM (1lu << 6)
#define R2_SPI_OUT_OF_RANGE (1lu << 7)
#define R2_SPI_CSD_OVERWRITE R2_SPI_OUT_OF_RANGE
//! @}

//! \name Control Tokens in SPI Mode
//! @{
//! \name Tokens used for a read operation
//! @{
#define SPI_TOKEN_SINGLE_MULTI_READ 0xFE
#define SPI_TOKEN_DATA_ERROR_VALID(token) (((token)&0xF0) == 0)
#define SPI_TOKEN_DATA_ERROR_ERRORS (0x0F)
#define SPI_TOKEN_DATA_ERROR_ERROR (1lu << 0)
#define SPI_TOKEN_DATA_ERROR_CC_ERROR (1lu << 1)
#define SPI_TOKEN_DATA_ERROR_ECC_ERROR (1lu << 2)
#define SPI_TOKEN_DATA_ERROR_OUT_RANGE (1lu << 3)
	//! @}
	//! \name Tokens used for a write operation
	//! @{
#define SPI_TOKEN_SINGLE_WRITE 0xFE
#define SPI_TOKEN_MULTI_WRITE 0xFC
#define SPI_TOKEN_STOP_TRAN 0xFD
#define SPI_TOKEN_DATA_RESP_VALID(token) ((((token) & (1 << 4)) == 0) && (((token) & (1 << 0)) == 1))
#define SPI_TOKEN_DATA_RESP_CODE(token) ((token)&0x1E)
#define SPI_TOKEN_DATA_RESP_ACCEPTED (2lu << 1)
#define SPI_TOKEN_DATA_RESP_CRC_ERR (5lu << 1)
#define SPI_TOKEN_DATA_RESP_WRITE_ERR (6lu << 1)
	//! @}
	//! @}
	//! @}

	//! @} end of sd_mmc_protocol

#ifdef __cplusplus
}
#endif

#endif /* SD_MMC_PROTOCOL_H_INCLUDED */

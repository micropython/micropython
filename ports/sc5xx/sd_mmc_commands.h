/*
 * (C) Copyright 2015-2016 - Analog Devices, Inc.  All rights reserved.
 *
 * FILE:     sd_mmc_commands.h
 */

#ifndef __SD_MMC_COMMANDS_H__
#define __SD_MMC_COMMANDS_H__

#define GENERIC_COMMAND (BITM_MSI_CMD_USEHOLDREG | BITM_MSI_CMD_WTPRIVDATA | BITM_MSI_CMD_STARTCMD)

#define NO_RESPONSE     0x00
#define R1_RESPONSE     0x40
#define R1B_RESPONSE    0x40
#define R2_RESPONSE     0xC0
#define R3_RESPONSE     0x40
#define R6_RESPONSE     0x40
#define R7_RESPONSE     0x40

/******** Commands common to SD/SDIO/Multimedia cards ********/
#define     SD_MMC_CMD0      0      /* Resets all cards to idle state */
#define     SD_MMC_CMD2      2      /* Requests the CID numbers on CMD line of all connected cards */ 
#define     SD_MMC_CMD3      3      /* Send relative address(SD/SDIO/SDHC), Set relative address (MMC)  */
#define     SD_MMC_CMD4      4      /* Programs the DSR of all cards */
#define     SDIO_MMC_CMD5    5
#define     SD_MMC_CMD6      6      /* Select/De-select card    */
#define     SD_MMC_CMD7      7      /* Select/De-select card                                            */
#define     SD_MMC_CMD8      8      /* Send SD Memory card interface condition (SDHC)                   */
#define     SD_MMC_CMD9      9      /* Send Card Specific Data (CSD)                                    */
#define     SD_MMC_CMD10    10      /* Send Card Identification Data (CID)                              */
#define     SD_MMC_CMD12    12      /* Stop Transmission                                                */
#define     SD_MMC_CMD13    13      /* Send Status                                                      */
#define     SD_MMC_CMD15    15      /* Go Inactive State                                                */
#define     SD_MMC_CMD16    16      /* Set Block Length                                                 */
#define     SD_MMC_CMD17    17      /* Read Single Block                                                */
#define     SD_MMC_CMD18    18      /* Read Multiple Blocks                                             */
#define     SD_MMC_CMD24    24      /* Write Single Block                                               */
#define     SD_MMC_CMD25    25      /* Write Multiple Blocks                                            */
#define     SD_MMC_CMD27    27      /* Program CSD                                                      */
#define     SD_MMC_CMD28    28      /* Set Write Protect                                                */
#define     SD_MMC_CMD29    29      /* Clear Write Protect                                              */
#define     SD_MMC_CMD30    30      /* Send Write Protect                                               */
#define     SD_MMC_CMD35    35
#define     SD_MMC_CMD36    36
#define     SD_MMC_CMD38    38      /* Erase selected blocks                                            */
#define     SD_MMC_CMD42    42      /* Lock/Unlock card (set/reset card password)                       */
#define     SD_MMC_CMD55    55
#define     SD_MMC_CMD56    56

/******** Commands specific to Multimedia card ********/
#define     MMC_CMD1         1      /* Reads MMC OCR value */
#define     MMC_CMD11       11
#define     MMC_CMD14       14
#define     MMC_CMD19       19
#define     MMC_CMD20       20 
#define     MMC_CMD23       23
#define     MMC_CMD26       26                                            
/*#define     MMC_CMD34       34*/      /* Untag block (sector for MMC) selected for erase                  */
#define     MMC_CMD39       39    
#define     MMC_CMD40       40      /* Go Interrupt request state */

/******** Commands specific to SD/SDIO cards ********/
/*#define     SD_CMD6          6*/      /* Checks switchable mode & switch card function                    */
#define     SD_CMD32        32      /* Erase write block start                                          */
#define     SD_CMD33        33      /* Erase write block end  */
#define     SD_CMD34        34
#define     SD_CMD37        37
#define     SD_CMD50        50
#define     SD_CMD57        57

#define     SD_ACMD6         6      /* Set bus width                                                    */
#define     SD_ACMD13       13      /* Reads SD memory card status                                      */
#define     SD_ACMD22       22      /* Reads number of written write blocks in SD memory card           */
#define     SD_ACMD41       41      /* Reads SD memory card OCR value                                   */
#define     SD_ACMD42       42      /* Connect/disconnect pullup on DAT3 line                           */
#define     SD_ACMD51       51      /* Reads SD Configuration register                                  */

/******** Commands specific to SDIO cards ********/
#define     SDIO_CMD52      52      /* IO read/write direct - SDIO command                              */
#define     SDIO_CMD53      53      /* Command used for SDIO block operations                           */

/*********************************************************************

SD/SDIO/MM card commands - in terms of command classes

*********************************************************************/

/******** Basic Commands (Command Class 0)********/
#define     SD_MMC_CMD_GO_IDLE_STATE            SD_MMC_CMD0 | NO_RESPONSE    /* Go Idle state                                                    */ 
#define     MMC_CMD_SEND_OP_COND                MMC_CMD1 | R3_RESPONSE        /* Reads MMC OCR value                                              */
#define     SD_MMC_CMD_ALL_SEND_CID             SD_MMC_CMD2 | R2_RESPONSE    /* Get CID of all cards in the bus                                  */ 
#define     SD_CMD_SEND_RELATIVE_ADDR           SD_MMC_CMD3 | R6_RESPONSE     /* Send relative address (SD/SDIO/SDHC), Set relative address (MMC) */
#define     MMC_CMD_SET_RELATIVE_ADDR           SD_MMC_CMD3 | R6_RESPONSE     /* Send relative address (SD/SDIO/SDHC), Set relative address (MMC) */
#define     SD_MMC_CMD_SET_DSR                  SD_MMC_CMD4 | NO_RESPONSE     /* Send relative address (SD/SDIO/SDHC), Set relative address (MMC) */
#define     MMC_CMD_SLEEP_AWAKE                 MMC_CMD5 | R1B_RESPONSE
#define     MMC_CMD_SWITCH                      SD_MMC_CMD6 | R1B_RESPONSE
#define     SD_MMC_CMD_SELECT_DESELECT_CARD     SD_MMC_CMD7 | R1B_RESPONSE     /* Select card                                                      */
#define     SD_CMD_SEND_IF_COND                 SD_MMC_CMD8 | R7_RESPONSE     /* Send SD Card interface condition (SDHC)                          */
#define     MMC_CMD_SEND_EXT_CSD                 SD_MMC_CMD8 | R1_RESPONSE     /* Send SD Card interface condition (SDHC)                          */
#define     SD_MMC_CMD_SEND_CSD                 SD_MMC_CMD9 | R2_RESPONSE   /* Send Card Specific Data (CSD)                                    */
#define     SD_MMC_CMD_SEND_CID                 SD_MMC_CMD10 | R2_RESPONSE   /* Send Card Identification Data (CID)                              */
#define     SD_MMC_CMD_STOP_TRANSMISSION        SD_MMC_CMD12 | R1B_RESPONSE   /* Stop Transmission                                                */
#define     SD_MMC_CMD_SEND_STATUS              SD_MMC_CMD13 | R1_RESPONSE  /* read addressed card's Status register                            */
#define     SD_MMC_CMD_GO_INACTIVE_STATE        SD_MMC_CMD15 | NO_RESPONSE  /* Go Inactive State                                                */

/******** Block Read Commands (Command Class 2)********/
#define     SD_MMC_CMD_SET_BLOCKLEN             SD_MMC_CMD16 | R1_RESPONSE  /* Set Read/Write Block Length                                      */
#define     SD_MMC_CMD_READ_SINGLE_BLOCK        SD_MMC_CMD17 | R1_RESPONSE   /* Read Single Block                                                */
#define     SD_MMC_CMD_READ_MULTIPLE_BLOCK      SD_MMC_CMD18 | R1_RESPONSE   /* Read Multiple Blocks                                             */

/******** Block Write Commands (Command Class 4)********/
#define     SD_MMC_CMD_WRITE_SINGLE_BLOCK       SD_MMC_CMD24 | R1_RESPONSE    /* Write Single Block                                               */
#define     SD_MMC_CMD_WRITE_MULTIPLE_BLOCK     SD_MMC_CMD25 | R1_RESPONSE   /* Write Multiple Blocks                                            */
#define     SD_MMC_CMD_PROGRAM_CSD              SD_MMC_CMD27 | R1_RESPONSE   /* Program CSD                                                      */

/******** Erase Commands (Command Class 5)********/
#define     SD_CMD_ERASE_WR_BLK_START          SD_MMC_CMD32 | R1_RESPONSE    /* Erase write block start                                          */
#define     SD_CMD_ERASE_WR_BLK_END            SD_MMC_CMD33 | R1_RESPONSE   /* Erase write block end                                            */
#define     SD_MMC_CMD_ERASE                    SD_MMC_CMD38 | R1B_RESPONSE   /* Erase selected blocks                                            */
#define     MMC_CMD_UNTAG_BLOCK                 MMC_CMD34       /* Untag block selected for erase                                   */

/******** Write Protection Commands (Command Class 6)********/
#define     SD_MMC_CMD_SET_WRITE_PROT           SD_MMC_CMD28 | R1B_RESPONSE   /* Set Write Protect                                                */
#define     SD_MMC_CMD_CLR_WRITE_PROT           SD_MMC_CMD29 | R1B_RESPONSE   /* Clear Write Protect                                              */
#define     SD_MMC_CMD_SEND_WRITE_PROT          SD_MMC_CMD30 | R1_RESPONSE   /* Send Write Protect                                               */

/******** Lock Card Commands (Command Class 7)********/
#define     SD_MMC_CMD_LOCK_UNLOCK              SD_MMC_CMD42        /* Lock card (set card password)                                    */

/******** Application Specific Commands (Command Class 8)********/
#define     SD_MMC_CMD_APP_CMD                  SD_MMC_CMD55 | R1_RESPONSE        /* Set SD card for application specific command                     */
#define     SD_CMD_SET_BUS_WIDTH                SD_ACMD6 | R1_RESPONSE        /* Set bus width                                                    */
#define     SD_CMD_GET_MEMORY_STATUS            SD_ACMD13 | R1_RESPONSE       /* Reads SD memory card status                                      */
#define     SD_CMD_GET_WRITTEN_BLOCK_COUNT      SD_ACMD22 | R1_RESPONSE       /* Reads number of written write blocks in SD memory card           */
#define     SD_CMD_GET_OCR_VALUE                SD_ACMD41 | R3_RESPONSE       /* Reads SD memory card OCR value                                   */
#define     SD_CMD_DISCONNECT_DAT3_PULLUP       SD_ACMD42 | R1_RESPONSE      /* disconnect pullup on DAT3 line                                   */
#define     SD_CMD_GET_CONFIG_REG               SD_ACMD51 | R1_RESPONSE     /* Reads SD Configuration register                                  */

/******** I/O Mode Commands (Command Class 9)********/
#define     SDIO_CMD_GET_SUPPORT                SDIO_CMD5       /* Inquire SDIO support                                             */
#define     SDIO_CMD_READ_DIRECT                SDIO_CMD52      /* SDIO read direct                                                 */
#define     SDIO_CMD_WRITE_DIRECT               SDIO_CMD52      /* SDIO write direct                                                */
#define     SDIO_CMD_BLOCK                      SDIO_CMD53      /* Command used for SDIO block operations                           */
#define     MMC_CMD_GO_IRQ_STATE                MMC_CMD40       /* Go Interrupt request state                                       */

/******** Switch Commands (Command Class 10)********/
/*#define     SD_CMD_GET_SWITCH_MODE_INFO         SD_CMD6*/         /* Checks switchable mode & switch card function                    */
#define     MMC_BUSTEST_W       MMC_CMD19 | R1_RESPONSE
#define     MMC_BUSTEST_R       MMC_CMD14 | R1_RESPONSE

#endif /* __SD_MMC_COMMANDS_H__ */

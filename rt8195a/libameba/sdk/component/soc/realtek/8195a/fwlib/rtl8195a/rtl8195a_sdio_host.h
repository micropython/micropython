	/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _RTL8195A_SDIO_HOST_H_
#define _RTL8195A_SDIO_HOST_H_

#include "hal_api.h"
#include "osdep_api.h"




#ifdef CONFIG_SDIO_HOST_VERIFY

#define HAL_MMC_HOST_READ32(addr)            			HAL_READ32(SDIO_HOST_REG_BASE, addr)
#define HAL_MMC_HOST_WRITE32(addr, value)    			HAL_WRITE32(SDIO_HOST_REG_BASE, addr, value)
#define HAL_MMC_HOST_READ16(addr)						HAL_READ16(SDIO_HOST_REG_BASE, addr)
#define HAL_MMC_HOST_WRITE16(addr, value)    			HAL_WRITE16(SDIO_HOST_REG_BASE, addr, value)
#define HAL_MMC_HOST_READ8(addr)						HAL_READ8(SDIO_HOST_REG_BASE, addr)
#define HAL_MMC_HOST_WRITE8(addr, value)    			HAL_WRITE8(SDIO_HOST_REG_BASE, addr, value)

/* RTL8195A Register */
// REG_SOC_HCI_COM_FUNC_EN (0x214)
#define SD_DEVICE_IP_ON_BLK					BIT0
#define SD_DEVICE_IP_OFF_BLK				BIT1
#define SD_HOST_IP_BLK						BIT2

// REG_PESOC_HCI_CLK_CTRL0 (0x240)
#define SD_HOST_CLKEN_IN_CPU_RUN_MODE		BIT2

// REG_HCI_PINMUX_CTRL (0x2A0)
#define SD_DEVICE_MODE_PINMUX_EN			BIT0
#define SD_HOST_MODE_PINMUX_EN				BIT1

// 0x40059000
#define SD_HOST_CARD_DETECT_CIRCUIT			BIT10


	
/* SD Host Register */
#define REG_SDMA_SYS_ADDR_ARG				0x00 // 4byte
#define REG_BLOCK_SIZE						0x04 // 2byte
#define REG_BLOCK_COUNT						0x06 // 2byte
#define REG_ARGUMENT1						0x08 // 4byte
#define REG_TRANSFER_MODE					0x0C // 2byte
#define REG_COMMAND							0x0E // 2byte
#define REG_RESPONSE0						0x10 // 4byte
#define REG_RESPONSE2						0x14 // 4byte
#define REG_RESPONSE4						0x18 // 4byte
#define REG_RESPONSE6						0x1C // 4byte
#define REG_BUFFER_DATA_PORT				0x20 // 4byte
#define REG_PRESENT_STATE					0x24 // 4byte
#define REG_HOST_CONTROL1					0x28 // 1byte
#define REG_POWER_CONTROL					0x29 // 1byte
#define REG_BLOCK_GAP_CONTROL				0x2A // 1byte
#define REG_WAKEUP_CONTROL					0x2B // 1byte
#define REG_CLOCK_CONTROL					0x2C // 2byte
#define REG_TIMEOUT_CONTROL					0x2E // 1byte
#define REG_SW_RESET						0x2F // 1byte
#define REG_NORMAL_INT_STATUS				0x30 // 2byte
#define REG_ERROR_INT_STATUS				0x32 // 2byte
#define REG_NORMAL_INT_STATUS_ENABLE		0x34 // 2byte
#define	REG_ERROR_INT_STATUS_ENABLE			0x36 // 2byte
#define REG_NORMAL_INT_SIGNAL_ENABLE		0x38 // 2byte
#define REG_ERROR_INT_SIGNAL_ENABLE			0x3A // 2byte
#define REG_CAPABILITIES					0x40 // 8byte
#define REG_ADMA_ADDRESS					0x58 // 8byte

// Transfer Mode (0x0C)
#define BIT_DMA_EN							BIT0
#define BIT_BLK_CNT_EN						BIT1
#define BIT_AUTO_CMD12_EN					BIT2
#define BIT_AUTO_CMD23_EN					BIT3
#define BIT_READ_TRANS						BIT4
#define BIT_MULTI_BLK						BIT5

// Present State (0x24)
#define BIT_CMD_INHIBIT_CMD					BIT0
#define BIT_CMD_INHIBIT_DAT					BIT1
#define BIT_CARD_INSERTED					BIT16
#define BIT_WRITE_PROTECT_SWITCH_PIN		BIT19

// Power Control (0x29)
#define BIT_POWER_33						0xE
#define BIT_POWER_30						0xC
#define BIT_POWER_18						0xA

// Clock Control (0x2C)
#define BIT_INTERNAL_CLK_EN					BIT0
#define BIT_INTERNAL_CLK_STABLE				BIT1
#define BIT_SD_CLK_EN						BIT2

// Software Reset (0x2F)
#define BIT_SW_RESET_ALL					BIT0
#define BIT_SW_RESET_CMD_LINE				BIT1
#define BIT_SW_RESET_DAT_LINE				BIT2
	
// Norma Interrupt Status (0x30)
#define BIT_COMMAND_COMPLETE				BIT0
#define BIT_TRANSFER_COMPLETE				BIT1
#define BIT_BLOCK_GAP_EVENT					BIT2
#define BIT_DMA_INT							BIT3
#define BIT_BUFFER_WRITE_RDY				BIT4
#define BIT_BUFFER_READ_RDY					BIT5
#define BIT_CARD_INSERTION					BIT6
#define BIT_CARD_REMOVAL					BIT7
#define BIT_CARD_INT						BIT8
#define BIT_ERROR_INT						BIT15

// Error Interrupt Status (0x32)
#define BIT_DATA_TIME_OUT_ERROR				BIT4
#define BIT_DATA_CRC_ERROR					BIT5
#define BIT_ADMA_ERROR						BIT9

// Capabilities (0x40)
#define BIT_VDD_33							BIT24
#define BIT_VDD_30							BIT25
#define BIT_VDD_18							BIT26


#define ENABLE								1
#define DISABLE 							0

#define ADMA_DESC_NUM						50

#define BUFFER_UNIT_SIZE					512
	
typedef enum _MMC_HOST_TEST_FUNC_ {
	MMC_HOST_TEST_HW_INIT,			// 0
	MMC_HOST_TEST_CARD_INIT,		// 1
	MMC_HOST_TEST_SEND_CMD, 		// 2
	MMC_HOST_TEST_DEBUG,			// 3
	MMC_HOST_TEST_SW_RESET,			// 4
	MMC_HOST_TEST_READ_SINGLE,		// 5
	MMC_HOST_TEST_WRITE_SINGLE,		// 6
	MMC_HOST_TEST_READ_MULTI,		// 7
	MMC_HOST_TEST_WRITE_MULTI,		// 8
	MMC_HOST_TEST_SINGLE_LONGRUN,	// 9
	MMC_HOST_TEST_MULTI_LONGRUN,	// 10
	MMC_HOST_TEST_CARD_DETECTION,	// 11
	MMC_HOST_TEST_WRITE_PROTECT,	// 12
	MMC_HOST_TEST_REGISTER_RW		// 13
}MMC_HOST_TEST_FUNC;
	
typedef enum _RESPONSE_TYPE_ {
	No_Response,					// 00b
	Response_136,					// 01b
	Response_48,					// 10b
	Response_48_Busy				// 11b
}RESPONSE_TYPE;
	
typedef enum _COMMAND_TYPE_ {
	Normal, 						// 00b
	Suspend,						// 01b
	Resume, 						// 10b
	Abort							// 11b
}COMMAND_TYPE;
	
typedef enum _DATA_PRESENT_ {
	No_Data_Present,				// 00b
	Data_Present,					// 01b
}DATA_PRESENT;

typedef enum _SUPPLY_VOLTAGE_ {
	MMC_VDD_27_28	= BIT15,
	MMC_VDD_28_29	= BIT16,
	MMC_VDD_29_30	= BIT17,
	MMC_VDD_30_31	= BIT18,
	MMC_VDD_31_32	= BIT19,
	MMC_VDD_32_33	= BIT20,
	MMC_VDD_33_34	= BIT21,
	MMC_VDD_34_35	= BIT22,
	MMC_VDD_35_36	= BIT23,
}SUPPLY_VOLTAGE;
	
typedef enum _COMMAND_INDEX_ {
	GO_IDLE_STATE			= 0,
	ALL_SEND_CID			= 2,
	SEND_RELATIVE_ADDR		= 3,
	SET_BUS_WIDTH			= 6,
	SELECT_CARD				= 7,
	SEND_IF_COND			= 8,
	SEND_CSD				= 9,
	STOP_TRANSMISSION		= 12,
	SEND_STATUS				= 13,
	READ_SINGLE_BLOCK   	= 17,
	READ_MULTIPLE_BLOCK 	= 18,
	WRITE_BLOCK				= 24,
	WRITE_MULTIPLE_BLOCK	= 25,
	SD_SEND_OP_COND 		= 41,
	APP_CMD					= 55,
}COMMAND_INDEX;

typedef enum _TRANSFER_CONFIG_ {
	Read_Data				= 0,
	Write_Data				= 1,
	Single_Block			= 0,
	Multiple_Block			= 1,
}TRANSFER_CONFIG;

typedef enum _ERROR_STATUS_ {
	General_Error,					// 0
	CRC_Error,						// 1	
	TIME_OUT_ERROR,					// 2
	CRC_Error_NeedCMD12,			// 3
	Transfer_OK						// 4
}ERROR_STATUS;

typedef enum _CARD_CURRENT_STATE_ {
	IDLE_STATE,
	READY_STATE,
	IDENT_STATE,
	STBY_STATE,
	TRAN_STATE,
	DATA_STATE,
	RCV_STATE,
	PRG_STATE,
	DIS_STATE,
	UNKNOWN_STATE
}CARD_CURRENT_STATE;

typedef struct _COMMAND_FORMAT_
{
	u16 Resp_Type:2;
	u16 Rsvd0:1;
	u16 CMD_CRC_Chk:1;
	u16 CMD_Idx_Chk:1;
	u16 Data_Present:1;
	u16 CMD_Type:2;
	u16 CMD_Idx:6;
	u16 Rsvd1:2;
}COMMAND_FORMAT, *PCOMMAND_FPRMAT;

typedef struct _MMC_COMMAND
{
	COMMAND_FORMAT	Cmd_Format;
	u32 Arg;
}MMC_COMMAND;

typedef struct _MMC_HOST_
{
	u32 OCR_Avail;
	u32 Resp[4];
	u32 CID[4];
	u32 RCA;
}MMC_HOST, *PMMC_HOST;

typedef struct _ADMA_ATTR_
{
	u16 Valid:1;
	u16 End:1;
	u16 Int:1;
	u16 Rsvd1:1;
	u16 Act1:1;
	u16 Act2:1;
	u16 Rsvd2:10;
}ADMA_ATTR, *PADMA_ATTR;
// 24 bytes
typedef struct _ADMA_DESC_TABLE_
{
	// 1st buffer desc
	ADMA_ATTR Attribute1;
	u16	Length1;
	u32 Address1;
	// 2nd buffer desc
	ADMA_ATTR Attribute2;
	u16	Length2;
	u32 Address2;
	// 3rd buffer desc
	ADMA_ATTR Attribute3;
	u16	Length3;
	u32 Address3;
}ADMA_DESC_TABLE, *PADMA_DESC_TABLE;
// 1024 bytes
typedef struct _ADMA_BUFFER_
{
	u8 Data1[512];  /* 1st buffer */
	u8 Data2[512];  /* 2nd buffer */
}ADMA_BUFFER, *PADMA_BUFFER;


VOID
SdHostTestApp(
    IN  u8      *argv[]
);
#endif	// end of "#ifdef CONFIG_SDIO_HOST_VERIFY"

#endif	/* #ifndef _RTL8195A_SDIO_HOST_H_ */

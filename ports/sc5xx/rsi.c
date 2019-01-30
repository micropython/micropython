/*
 * (C) Copyright 2014-2016 Analog Devices, Inc. All Rights Reserved.
 *
 * This software is proprietary and confidential.  By using this software
 * you agree to the terms of the associated Analog Devices License Agreement.
 *
 * Project Name:  Power_On_Self_Test
 *
 * Hardware:      ADSP-SC5xx EZ-Board
 *
 * Description:   This example performs push button and LED tests on the ADSP-SC5xx EZ-Board.
 */

#include "rsi.h"
#include <stdlib.h>
#include <services/pwr/adi_pwr.h>
#include <sys/platform.h>
#include "py/mpprint.h"

#if !defined(ADI_CACHE_LINE_LENGTH)
/* The ADI_CACHE_* macros were introduced in CCES 2.4.0 in <sys/platform.h>.
 * If using an older toolchain, define them here.
 */
 #if defined(__ADSPARM__)
  #define ADI_CACHE_LINE_LENGTH (32uL)
  #define ADI_CACHE_ALIGN __attribute__((aligned(ADI_CACHE_LINE_LENGTH)))
 #elif defined(__ADSP215xx__)
  #define ADI_CACHE_LINE_LENGTH (64uL)
  #define ADI_CACHE_ALIGN _Pragma("align 64")
 #else
  #error Unknown ADI_CACHE_LINE_LENGTH
 #endif
 #define ADI_CACHE_ROUND_UP_SIZE(size, type) \
    (((((((size) * sizeof(type)) \
           + (ADI_CACHE_LINE_LENGTH - 1uL)) \
          / ADI_CACHE_LINE_LENGTH) * ADI_CACHE_LINE_LENGTH) \
      + (sizeof(type) - 1uL)) / sizeof(type))
#endif

/* define shorter forms of the ADI transfer types */
#define TRANS_READ   (ADI_RSI_TRANSFER_DMA_BLCK_READ)
#define TRANS_WRITE  (ADI_RSI_TRANSFER_DMA_BLCK_WRITE)
#define TRANS_NONE   (ADI_RSI_TRANSFER_NONE)

/* define shorter forms of the ADI response types */
#define RESPONSE_NONE  (ADI_RSI_RESPONSE_TYPE_NONE)
#define RESPONSE_LONG  (ADI_RSI_RESPONSE_TYPE_LONG)
#define RESPONSE_SHORT (ADI_RSI_RESPONSE_TYPE_SHORT)

/* define shorter forms of the ADI flags */
#define CRCDIS  (ADI_RSI_CMDFLAG_CRCDIS)
#define CHKBUSY (ADI_RSI_CMDFLAG_CHKBUSY)

/* Device handle for the rsi driver*/
ADI_RSI_HANDLE hDevice;

#define DEBUG_STATEMENT(statement) mp_printf(&mp_plat_print, statement)
#define DEBUG_PRINT(statement...) mp_printf(&mp_plat_print, statement)

/* structure for holding all MMC and SD card details */
static struct
{
    RSI_SD_CARD_REGISTERS sd;       /* SD card details */
    RSI_MMC_CARD_REGISTERS mmc;     /* MMC or eMMC details */
    SD_MMC_CARD_TYPE    type;
    RSI_DATA_BUS_WIDTH  max_bus_width;
    uint32_t    sclk;
    uint32_t    rsiclk;
} adi_rsi_card_registers;

/* local function declarations */
static uint32_t sd_mmc_send_command( uint16_t command,
        							 uint32_t argument,
        							 ADI_RSI_RESPONSE_TYPE resp,
        							 ADI_RSI_TRANSFER transfer,
        							 uint32_t flags);		/* sends a command to over the RSI */
static SD_MMC_CARD_TYPE sd_mmc_identification_sdv2(uint32_t);	/* performs SD Version 2.0 or later card detection */
static SD_MMC_CARD_TYPE sd_mmc_identification_sdv1(void);		/* performs SD Version 1.x or earlier card detection */
static SD_MMC_CARD_TYPE sd_mmc_identification_mmc(void);		/* performs MMC/eMMC detection */
void RSI_SetOptimumIdentificationSpeed(void);
void RSI_GetClocks(void);

static uint32_t sd_mmc_get_sd_cid(void);
static uint32_t sd_mmc_get_sd_scr_register(void);
static uint32_t sd_mmc_get_sd_ssr_register(void);
static uint32_t sd_mmc_get_wait_until_ready(void);
static uint32_t sd_mmc_send_csd(void);
static uint32_t sd_mmc_read_csd_extended(void);
static uint32_t sd_mmc_get_mmc_cid(void);
static uint32_t card_address(uint32_t block_num)
{    
	return (adi_rsi_card_registers.type == SD_MMC_CARD_TYPE_SD_V2X_HIGH_CAPACITY) ? block_num : block_num * 512;
}

/*
 *  RSI_WaitForCard()
 *  This routine waits for the card to be detected via a rising edge on RSI_DATA3
 *
 *  Note: The pull-down resistor on the RSI_DATA3 signal appears to be too strong for
 *        the cards. Therefore when a card is inserted the interrupt is not generated.
 *        The interrupt is detected during the RSI_PortSetup routine if a card is inserted
 *        while the RSI pins are enabled. There would appear to be a small transition time
 *        during the enabling of the RSI and the pull down resistor being applied that
 *        allows the RSI to detect the rising edge on the RSI_DATA3 signal.
 *
 *        It should also be noted that the MMC spec does not specifically detail the use
 *        of the DAT3 signal as a card detect feature.
 *
 *        One final note is that when an SD card is detected the pull-up resistor on the card
 *        is then disabled during the device detection. Therefore if reloading the program
 *        without powering down the card the SD card will not be detected even if inserted
 *        during the execution of the RSI_PortSetup routine. This is due to the fact that
 *        the pull-up is still disabled.
 */
void RSI_WaitForCard(void)
{
	if ( ADI_RSI_SUCCESS != adi_rsi_IsCardPresent(hDevice))
	{
		/* wait for a device to be detected */
		DEBUG_STATEMENT ( "Waiting for card to be detected...\n" );

		while ( ADI_RSI_SUCCESS != adi_rsi_IsCardPresent(hDevice));
	}

	DEBUG_STATEMENT ( "Card Detected\n" );

	RSI_SetOptimumIdentificationSpeed();
}

/*
 * RSI_WaitForCardRemoved()
 *
 * Conversely, for SC573, due to multiplexing of Card Detect and CAN recieve signals,
 * the SD card should be removed before the test finished, otherwise any subsequent
 * CAN tests will fail.
 *
 */
void RSI_WaitForCardRemoved(void)
{
	if ( ADI_RSI_SUCCESS == adi_rsi_IsCardPresent(hDevice))
	{
		/* wait for the card to be removed  */
		DEBUG_STATEMENT ( "Waiting for card to be removed...\n" );

		while ( ADI_RSI_SUCCESS == adi_rsi_IsCardPresent(hDevice))
		{
		    __asm("nop");
		}
	}

	DEBUG_STATEMENT ( "Card Removed\n" );
}


RSI_DATA_BUS_WIDTH GetMaxBusWidth(void)
{
    return adi_rsi_card_registers.max_bus_width;
}

SD_MMC_SPEED_GRADE GetMMCTransferSpeed(void)
{
    SD_MMC_SPEED_GRADE mmc_speed_grade = SD_MMC_SPEED_GRADE_ERROR;

    if(adi_rsi_card_registers.type == SD_MMC_CARD_TYPE_MMC)
    {
        if(adi_rsi_card_registers.mmc.csd.tran_speed == 0x2A)
        {
            mmc_speed_grade = MMC_SPEED_GRADE_20MHZ;
        }
        else if(adi_rsi_card_registers.mmc.csd.tran_speed == 0x32)
        {
            mmc_speed_grade = MMC_SPEED_GRADE_26MHZ;
        }
    }
    return mmc_speed_grade;
}

SD_MMC_SPEED_GRADE GetMMCHighTransferSpeed(void)
{
    SD_MMC_SPEED_GRADE mmc_speed_grade = SD_MMC_SPEED_GRADE_ERROR;

    if( adi_rsi_card_registers.type == SD_MMC_CARD_TYPE_MMC )
    {
        if(adi_rsi_card_registers.mmc.extcsd.card_type == 1)
        {
            mmc_speed_grade = MMC_SPEED_GRADE_26MHZ;
        }
        else if(adi_rsi_card_registers.mmc.csd.tran_speed > 1)
        {
            mmc_speed_grade = MMC_SPEED_GRADE_52MHZ;
        }
    }
    return mmc_speed_grade;
}

static void SetOptimumCLKDIV(uint32_t sclk, uint32_t card_speed)
{
	uint32_t  clkdiv = 0;
	uint32_t  rsi_freq = 0;

    if(sclk <= card_speed)
    {
//        DEBUG_PRINT("Setting RSI clock frequency to %luHz, by setting divisor to 1\n", sclk);

        if(ADI_RSI_SUCCESS != adi_rsi_SetClock(hDevice, 1u, ADI_RSI_CLK_MODE_ENABLE))
        	DEBUG_PRINT("Error, unable to set RSI clock frequency to %luHz via bypass\n", sclk);
    }
    else
    {
        do{
        	/* We  have configured MSI module to run at 50MHz. */
        	clkdiv++;
        	rsi_freq = (sclk/(2*(clkdiv)));
        } while(rsi_freq > card_speed);
        clkdiv *= 2;

//        DEBUG_PRINT("System clock frequency is %luHz, using an rsi divisor of %lu\n", sclk, clkdiv);
//        DEBUG_PRINT("Setting RSI clock frequency to %luHz\n", rsi_freq);

        if(ADI_RSI_SUCCESS != adi_rsi_SetClock(hDevice, clkdiv, ADI_RSI_CLK_MODE_ENABLE))
        	DEBUG_PRINT("Error, unable to set RSI clock frequency to %luHz\n", clkdiv);

        adi_rsi_card_registers.rsiclk = rsi_freq;
    }
}

void MMCSetOptimumSpeed(void)
{
    uint32_t card_speed;
    uint32_t sclk = adi_rsi_card_registers.sclk;

    DEBUG_STATEMENT("Getting MMC card transfer speed...\n");
    card_speed = (uint32_t)GetMMCTransferSpeed();
    DEBUG_PRINT("MMC card transfer speed is %luHz\n", card_speed);

    SetOptimumCLKDIV(sclk, card_speed);
}

void MMCSetOptimumHighSpeed(SD_MMC_SPEED_GRADE card_speed)
{
    uint32_t sclk = adi_rsi_card_registers.sclk;

    DEBUG_PRINT("MMC card transfer speed is %luHz\n", (uint32_t)card_speed);

    SetOptimumCLKDIV(sclk, card_speed);
}

void RSI_SetOptimumSDSpeed(void)
{
    uint32_t card_speed;
    uint32_t sclk = adi_rsi_card_registers.sclk;

    DEBUG_STATEMENT("SD Card transfer speed is 25000000Hz\n");
    card_speed = 25000000;

    SetOptimumCLKDIV(sclk, card_speed);
}

void RSI_SetOptimumIdentificationSpeed(void)
{
    uint32_t card_speed;
    uint32_t sclk = adi_rsi_card_registers.sclk;

    card_speed = 400000;
//    DEBUG_PRINT("Card identification must be run at %luHz or less\n", card_speed);

    SetOptimumCLKDIV(sclk, card_speed);
}

/*
 *  sd_mmc_read_block_dma()
 */
uint32_t sd_mmc_read_block_dma(uint32_t card_address, void * pDestination)
{
    uint32_t error = 0;     /* assume no error */

	adi_rsi_SetBlockCntAndLen(hDevice, 1u, 512);
	adi_rsi_SubmitRxBuffer(hDevice, pDestination, 512, 1);

    error = sd_mmc_send_command(SD_MMC_CMD_READ_SINGLE_BLOCK, card_address, RESPONSE_SHORT, TRANS_READ, CRCDIS); /* issue the block read command */

    void *pBuffer = NULL;
    adi_rsi_GetRxBuffer (hDevice, &pBuffer);

    return error;
}

/*
 *  sd_mmc_read_mblocks_dma
 */
uint32_t sd_mmc_read_mblocks_dma(uint32_t card_address, void * pDestination, uint32_t num_blocks)
{
    uint32_t error = 0;

	adi_rsi_SetBlockCntAndLen(hDevice, num_blocks, 512);
	adi_rsi_SubmitRxBuffer(hDevice, pDestination, 512, num_blocks);

	error = sd_mmc_send_command(SD_MMC_CMD_READ_MULTIPLE_BLOCK, card_address, RESPONSE_SHORT, TRANS_READ, CRCDIS);

    void *pBuffer = NULL;
    adi_rsi_GetRxBuffer (hDevice, &pBuffer);

    error = sd_mmc_send_command(SD_MMC_CMD_STOP_TRANSMISSION, 0, RESPONSE_SHORT, TRANS_NONE, CRCDIS);

    return error;
}

/*
 *  sd_mmc_write_block_dma
 */
uint32_t sd_mmc_write_block_dma(uint32_t card_address, void * pSource)
{
    uint32_t error = 0;

	adi_rsi_SetBlockCntAndLen(hDevice, 1u, 512);
	adi_rsi_SubmitTxBuffer(hDevice, pSource, 512, 1);

    error = sd_mmc_send_command(SD_MMC_CMD_WRITE_SINGLE_BLOCK, card_address, RESPONSE_SHORT, TRANS_WRITE, CRCDIS);

    void *pBuffer = NULL;
    adi_rsi_GetTxBuffer (hDevice, &pBuffer);

    /* wait until the device becomes ready */
    sd_mmc_get_wait_until_ready();

    return error;
}

/*
 *  sd_mmc_write_mblocks_dma
 */
uint32_t sd_mmc_write_mblocks_dma(uint32_t card_address, void * pSource, uint32_t num_blocks)
{
    uint32_t error = 0;
    uint32_t i = 0;

	adi_rsi_SetBlockCntAndLen(hDevice, num_blocks, 512);
	adi_rsi_SubmitTxBuffer(hDevice, pSource, 512, num_blocks);

	error = sd_mmc_send_command(SD_MMC_CMD_WRITE_MULTIPLE_BLOCK, card_address, RESPONSE_SHORT, TRANS_WRITE, CRCDIS);

    void *pBuffer = NULL;
    adi_rsi_GetTxBuffer (hDevice, &pBuffer);

    /* workaround for some MMC cards that are fussy with regards to the timing
		of the STOP_TRANSMISSION signal */
    for(i=0; i<1024; i++);

    error = sd_mmc_send_command(SD_MMC_CMD_STOP_TRANSMISSION, 0, RESPONSE_SHORT, TRANS_NONE, CRCDIS);

    /* wait until the device becomes ready */
    sd_mmc_get_wait_until_ready();

    return error;
}

/*
 *  sd_mmc_send_command
 */
static uint32_t sd_mmc_send_command( uint16_t command,
		                             uint32_t argument,
		                             ADI_RSI_RESPONSE_TYPE resp,
		                             ADI_RSI_TRANSFER transfer,
		                             uint32_t flags)
{
	ADI_RSI_RESULT result;

	result = adi_rsi_SetDataMode(hDevice, transfer, ADI_RSI_CEATA_MODE_NONE);

	result = adi_rsi_SendCommand(hDevice, command, argument, flags, resp);

	while (ADI_RSI_NOT_FINISHED == (result = adi_rsi_CheckCommand(hDevice, resp)));

	return result;
}

/*
 *  sd_mmc_identification()
 *
 * SD V2.x cards identified with a valid response to CMD8, invalid response is unusable
 * SD V1.x cards identified with no response to CMD8, CMD55 then ACMD41 response returns ready
 * MMC cards identified with
 */
SD_MMC_CARD_TYPE sd_mmc_identification(void)
{
    uint32_t error;
    uint32_t response;

//    DEBUG_STATEMENT("Starting card identification procedure\n");
    /* send card to IDLE state before performing card identification */
//    DEBUG_STATEMENT("Sending GO_IDLE_STATE command\n");

    error = sd_mmc_send_command(SD_MMC_CMD_GO_IDLE_STATE, 0, RESPONSE_NONE, TRANS_NONE, 0);

//    DEBUG_STATEMENT("Starting SD Version 2.0 identification...\n");
    /* 2.7-3.6V, 0xAA check pattern */
//    DEBUG_STATEMENT("Sending SEND_IF_COND command for 2.7V to 3.6V compatibility\n");
    error = sd_mmc_send_command(SD_CMD_SEND_IF_COND, 0x000001AA, RESPONSE_SHORT, TRANS_NONE, CRCDIS);

    if(!error)
    {
//         DEBUG_STATEMENT("Card is compatible, likely an SD Version 2.0 compliant card...\n");
        /* it's a version 2.0 or later SD memory card */
        adi_rsi_GetShortResponse(hDevice, &response);

        adi_rsi_card_registers.type = sd_mmc_identification_sdv2(response);
    }
    else
    {
        DEBUG_STATEMENT("Card is not compatible or did not respond\n");
        DEBUG_STATEMENT("Starting SD Version 1.x identification...\n");
        /* check if it's a version 1.X SD memory card */
        adi_rsi_card_registers.type = sd_mmc_identification_sdv1();
    }

    if( adi_rsi_card_registers.type == SD_MMC_CARD_TYPE_SD_V1X ||
        adi_rsi_card_registers.type == SD_MMC_CARD_TYPE_SD_V2X ||
        adi_rsi_card_registers.type == SD_MMC_CARD_TYPE_SD_V2X_HIGH_CAPACITY)
    {
//        DEBUG_STATEMENT("Requesting CID register...\n");
        error = sd_mmc_get_sd_cid();
//        DEBUG_STATEMENT("Requesting RCA...\n");
        error = sd_mmc_send_command(SD_CMD_SEND_RELATIVE_ADDR, 0, RESPONSE_SHORT, TRANS_NONE, 0);

        if(!error)
        {
        	adi_rsi_GetShortResponse(hDevice, &response);
            adi_rsi_card_registers.sd.rca = (response & 0xFFFF0000)>>16;
//            DEBUG_PRINT("RCA of 0x%04X sent from card\n", adi_rsi_card_registers.sd.rca);

//            DEBUG_STATEMENT("Requesting Card Status register...\n");
            error = sd_mmc_send_command(SD_MMC_CMD_SEND_STATUS,(adi_rsi_card_registers.sd.rca <<16), RESPONSE_SHORT, TRANS_NONE, CRCDIS);

            adi_rsi_GetShortResponse(hDevice, &response);
            adi_rsi_card_registers.sd.csr = response;

//            DEBUG_STATEMENT("Requesting CSD register...\n");
            error = sd_mmc_send_csd();
            if(error)
            {
                while(1);
            }

//            DEBUG_STATEMENT("Requesting SCR register...\n");
            error = sd_mmc_get_sd_scr_register();
            if(error)
            {
                while(1);
            }
//            DEBUG_STATEMENT("Optimizing RSI interface speed...\n");
            RSI_SetOptimumSDSpeed();
            adi_rsi_card_registers.max_bus_width = RSI_DATA_BUS_WIDTH_4;
//            DEBUG_STATEMENT("Setting RSI bus width to 4-bit... ");
            error = sd_mmc_set_bus_width(4);
            if(error)
            {
                while(1);
            }

//            DEBUG_STATEMENT("Requesting SSR register...\n");
            error = sd_mmc_get_sd_ssr_register();
            if(error)
            {
                while(1);
            }
        }
    }

    if(adi_rsi_card_registers.type == UNUSABLE_CARD)
    {
        DEBUG_STATEMENT("Starting MMC identification...\n");
        adi_rsi_card_registers.type = sd_mmc_identification_mmc();
        if(adi_rsi_card_registers.type == SD_MMC_CARD_TYPE_MMC)
        {
            DEBUG_STATEMENT("Requesting CID register...\n");
            error = sd_mmc_get_mmc_cid();
            DEBUG_PRINT("Setting RCA to 0x%04X...\n", MMC_RCA);
            error = sd_mmc_send_command(MMC_CMD_SET_RELATIVE_ADDR, MMC_RCA<<16, RESPONSE_SHORT, TRANS_NONE, 0);
            if(!error)
            {
                DEBUG_STATEMENT("RCA accepted\n");
                adi_rsi_card_registers.mmc.rca = MMC_RCA;
                DEBUG_STATEMENT("Requesting CSD register...\n");
                error =  sd_mmc_send_csd();
                DEBUG_STATEMENT("Optimizing RSI interface speed...\n");
                MMCSetOptimumSpeed();
                error = sd_mmc_select();
                DEBUG_STATEMENT("Performing MMC bus test to determine bus width of connected device\n");
                adi_rsi_card_registers.max_bus_width = mmc_bus_test();
                DEBUG_PRINT("%d-bit bus width detected\n", adi_rsi_card_registers.max_bus_width);
                DEBUG_STATEMENT("Setting MMC bus width to maximum supported bus width...\n");
                error = sd_mmc_set_bus_width(adi_rsi_card_registers.max_bus_width);

                DEBUG_STATEMENT("Requesting Extended CSD register...\n ");
                sd_mmc_read_csd_extended();
            }
        }
    }
    DEBUG_STATEMENT("Card identification procedure completed!\n");

    return adi_rsi_card_registers.type;
}

/*
 *  sd_mmc_identification_sdv2(uint32_t response)
 *
 *  Verifies if the device is an SD Version 2.0 or later device based on the response
 */
static SD_MMC_CARD_TYPE sd_mmc_identification_sdv2(uint32_t inResponse)
{
    uint32_t error;
    uint32_t response;

    SD_MMC_CARD_TYPE type = UNUSABLE_CARD;

    if(inResponse & 0x00000100)
    {
//        DEBUG_STATEMENT("Voltage range was accepted, checking echo back pattern...\n");
        /* voltage was accepted */
        if((inResponse & 0x000000FF) == 0xAA)
        {
//            DEBUG_STATEMENT("Echo back pattern correct\n");
            type = SD_MMC_CARD_TYPE_SD_V2X;

            do{
                 /* card with compatible voltage range */
//                DEBUG_STATEMENT("Requesting OCR register...\n");
                error = sd_mmc_send_command(SD_MMC_CMD_APP_CMD, 0, RESPONSE_SHORT, TRANS_NONE, CRCDIS);
                if (ADI_RSI_SUCCESS != error) DEBUG_PRINT("SD_MMC_SEND_COMMAND FAILURE %x\n", (unsigned int)error);

                adi_rsi_GetShortResponse(hDevice, &response);

                error = sd_mmc_send_command(SD_CMD_GET_OCR_VALUE, 0x40FF8000, RESPONSE_SHORT, TRANS_NONE, CRCDIS);
                if (ADI_RSI_SUCCESS != error) DEBUG_PRINT("SD_MMC_SEND_COMMAND FAILURE %x\n", (unsigned int)error);

                adi_rsi_GetShortResponse(hDevice, &response);

//                DEBUG_STATEMENT("OCR register received\n");
                adi_rsi_card_registers.sd.ocr = response;

            }while(!(adi_rsi_card_registers.sd.ocr & (unsigned int)SD_OCR_CARD_POWER_UP_STATUS));


            if(type == SD_MMC_CARD_TYPE_SD_V2X)
            {
//                DEBUG_STATEMENT("Checking if card is a high capacity card...\n");
                if(adi_rsi_card_registers.sd.ocr & SD_OCR_CARD_CAPACITY_STATUS)
                {
                    DEBUG_STATEMENT("SD Version 2.0 or later High Capacity card detected successfully!\n");
                    type = SD_MMC_CARD_TYPE_SD_V2X_HIGH_CAPACITY;
                }
                else
                {
                    DEBUG_STATEMENT("SD Version 2.0 or later Standard Capacity card detected successfully!\n");
                }
            }
        }
    }
    return type;
}

/*
 *  sd_mmc_identification_sdv1()
 *
 *  Verifies if the device is an SD Version 2.0 device with a voltage mismatch,
 *  or whether it's a version 1.x SD card or not an SD card at all
 */
SD_MMC_CARD_TYPE sd_mmc_identification_sdv1(void)
{
    uint32_t error;
    uint32_t response;

    SD_MMC_CARD_TYPE type = SD_MMC_CARD_TYPE_SD_V1X;

    do
    {
        /* It's either a:                                               */
        /*  version 2.00 or later memory card with a voltage mismatch   */
        /*  version 1.x SD memory card                                  */
        /*  MMC card                                                    */
        DEBUG_STATEMENT("Requesting OCR register...\n");
        error = sd_mmc_send_command(SD_MMC_CMD_APP_CMD, 0, RESPONSE_SHORT, TRANS_NONE, CRCDIS);
        error = sd_mmc_send_command(SD_CMD_GET_OCR_VALUE, 0x00FF8000, RESPONSE_SHORT, TRANS_NONE, CRCDIS);

       if(error)
       {
    	    /* not an SD card */
    	    DEBUG_STATEMENT("Card not responding or invalid operating condition, setting card type to UNUSABLE_CARD\n");
    	    type = UNUSABLE_CARD;
    	    break;
       }
       else
       {
    	   adi_rsi_GetShortResponse(hDevice, &response);

    	   DEBUG_STATEMENT("OCR register received\n");
    	   adi_rsi_card_registers.sd.ocr = response;
       }

    }while(!(adi_rsi_card_registers.sd.ocr & (unsigned int)SD_OCR_CARD_POWER_UP_STATUS));

    if(type == SD_MMC_CARD_TYPE_SD_V1X)
    {
        DEBUG_STATEMENT("OCR register received and device no longer busy\n");
        DEBUG_STATEMENT("SD Version 1.x card detected successfully!\n");
    }

    return type;
}

/*
 *  sd_mmc_identification_mmc()
 *
 *  Verifies if the device is an MMC device
 */
SD_MMC_CARD_TYPE sd_mmc_identification_mmc()
{
    uint32_t error;
    uint32_t response;

    SD_MMC_CARD_TYPE type = SD_MMC_CARD_TYPE_MMC;

    DEBUG_STATEMENT("Sending GO_IDLE_STATE command...\n");
    error = sd_mmc_send_command(SD_MMC_CMD_GO_IDLE_STATE,0, RESPONSE_NONE, TRANS_NONE, CRCDIS);

    do
    {
        DEBUG_STATEMENT("\n    Requesting card operating conditions...");
        error = sd_mmc_send_command(MMC_CMD_SEND_OP_COND,
                                    SD_MMC_SECTOR_MODE_SUPPORT | SD_MMC_VDD_OPERATING_RANGE, RESPONSE_SHORT, TRANS_NONE, CRCDIS);
        if(error)
        {
     	    /* not an SD card */
     	    DEBUG_STATEMENT("Card not responding or invalid operating condition, setting card type to UNUSABLE_CARD\n");
     	    type = UNUSABLE_CARD;
     	    break;
        }
        else
        {
     	   adi_rsi_GetShortResponse(hDevice, &response);

     	   DEBUG_STATEMENT("OCR register received\n");
     	   adi_rsi_card_registers.sd.ocr = response;
        }
    }while(!(response & 0x80000000));

    if(type == SD_MMC_CARD_TYPE_MMC)
    {
        DEBUG_STATEMENT("MMC card detected successfully!\n");
        adi_rsi_card_registers.mmc.ocr = response;
    }

    return type;
}

static uint32_t sd_mmc_get_sd_cid(void)
{
    uint32_t error;
    uint32_t response[4];

    error = sd_mmc_send_command(SD_MMC_CMD_ALL_SEND_CID, 0, RESPONSE_LONG, TRANS_NONE, 0);

    if(!error)
    {
        adi_rsi_GetLongResponse(hDevice, response);

        adi_rsi_card_registers.sd.cid.mid = (response[0] & 0xFF000000) >> 24;
        adi_rsi_card_registers.sd.cid.oid[0] = (response[0] & 0x00FF0000) >> 16;
        adi_rsi_card_registers.sd.cid.oid[1] = (response[0] & 0x0000FF00) >> 8;
        adi_rsi_card_registers.sd.cid.pnm[0] = (response[0] & 0x000000FF);
        adi_rsi_card_registers.sd.cid.pnm[1] = (response[1] & 0xFF000000) >> 24;
        adi_rsi_card_registers.sd.cid.pnm[2] = (response[1] & 0x00FF0000) >> 16;
        adi_rsi_card_registers.sd.cid.pnm[3] = (response[1] & 0x0000FF00) >> 8;
        adi_rsi_card_registers.sd.cid.pnm[4] = (response[1] & 0x000000FF);
        adi_rsi_card_registers.sd.cid.prv = (response[2] & 0xFF000000) >> 24;
        adi_rsi_card_registers.sd.cid.psn = ((response[2] & 0x00FFFFFF)<< 8) | ((response[3] & (0x7F8000000>>1))>> 22);
        adi_rsi_card_registers.sd.cid.mdt = (response[3] & (0x0007FF80>>1))  >> 6;
        adi_rsi_card_registers.sd.cid.crc = (response[3] & (0x0000007F>>1));
    }

    return error;
}

static uint32_t sd_mmc_get_mmc_cid(void)
{
    uint32_t error;
    uint32_t response[4];

    error = sd_mmc_send_command(SD_MMC_CMD_ALL_SEND_CID, 0, RESPONSE_LONG, TRANS_NONE, 0);

    if(!error)
    {
    	adi_rsi_GetLongResponse(hDevice, response);

        adi_rsi_card_registers.mmc.cid.mid = ADI_MMC_CID_MID(response[0]);
        adi_rsi_card_registers.mmc.cid.cbx = ADI_MMC_CID_CBX(response[0]);
        adi_rsi_card_registers.mmc.cid.oid = ADI_MMC_CID_OID(response[0]);
        adi_rsi_card_registers.mmc.cid.pnm[0] = ADI_MMC_CID_PNM0(response[0]);
        adi_rsi_card_registers.mmc.cid.pnm[1] = ADI_MMC_CID_PNM1(response[1]);
        adi_rsi_card_registers.mmc.cid.pnm[2] = ADI_MMC_CID_PNM2(response[1]);
        adi_rsi_card_registers.mmc.cid.pnm[3] = ADI_MMC_CID_PNM3(response[1]);
        adi_rsi_card_registers.mmc.cid.pnm[4] = ADI_MMC_CID_PNM4(response[1]);
        adi_rsi_card_registers.mmc.cid.pnm[5] = ADI_MMC_CID_PNM5(response[2]);
        adi_rsi_card_registers.mmc.cid.prv = ADI_MMC_CID_PRV(response[2]);
        adi_rsi_card_registers.mmc.cid.psn = ADI_MMC_CID_PSN(response[2], response[3]);
        adi_rsi_card_registers.mmc.cid.mdt = ADI_MMC_CID_MDT(response[3]);
        adi_rsi_card_registers.mmc.cid.crc = ADI_MMC_CID_CRC(response[3]);
    }

    return error;
}

static uint32_t sd_mmc_send_csd(void)
{
    uint32_t error;
    uint32_t response[4];

    if( adi_rsi_card_registers.type == SD_MMC_CARD_TYPE_SD_V1X ||
        adi_rsi_card_registers.type == SD_MMC_CARD_TYPE_SD_V2X ||
        adi_rsi_card_registers.type == SD_MMC_CARD_TYPE_SD_V2X_HIGH_CAPACITY)
    {
        error = sd_mmc_send_command(SD_MMC_CMD_SEND_STATUS, adi_rsi_card_registers.sd.rca<<16, RESPONSE_SHORT, TRANS_NONE, CRCDIS);

        adi_rsi_GetShortResponse(hDevice, &response[0]);

        if((response[0] & SD_CSR_CURRENT_STATE) == SD_CSR_CURRENT_STATE_TRANSFER)
        {
            error = sd_mmc_send_command(   SD_MMC_CMD_SELECT_DESELECT_CARD,
                                           ~adi_rsi_card_registers.sd.rca<<16, RESPONSE_SHORT, TRANS_NONE, CRCDIS);
        }

        error = sd_mmc_send_command(SD_MMC_CMD_SEND_CSD, (adi_rsi_card_registers.sd.rca<<16), RESPONSE_LONG, TRANS_NONE, CRCDIS);

        if(!error)
        {
            adi_rsi_GetLongResponse(hDevice, response);

            adi_rsi_card_registers.sd.csd.csd_structure = (SD_CSD_STRUCTURE)((response[0] & 0xC0000000)>>30);
            adi_rsi_card_registers.sd.csd.taac = (response[0] & 0x00FF0000)>>16;
            adi_rsi_card_registers.sd.csd.nsac = (response[0] & 0x0000FF00)>>8;
            adi_rsi_card_registers.sd.csd.tran_speed = response[0] & 0x000000FF;
            adi_rsi_card_registers.sd.csd.ccc = (response[1] & 0xFFF00000)>>20;
            adi_rsi_card_registers.sd.csd.read_bl_len = (response[1] & 0x000F0000)>>16;
            adi_rsi_card_registers.sd.csd.read_bl_partial = (response[1] & 0x00008000)>>15;
            adi_rsi_card_registers.sd.csd.write_blk_misalign = (response[1] & 0x00004000)>>14;
            adi_rsi_card_registers.sd.csd.read_blk_misalign = (response[1] & 0x00002000)>>13;
            adi_rsi_card_registers.sd.csd.dsr_imp = (response[1] & 0x00001000)>>12;
            if(adi_rsi_card_registers.sd.csd.csd_structure == SD_CSD_STRUCTURE_VERSION_1_0)
            {
                adi_rsi_card_registers.sd.csd.c_size = ((response[1] & 0x000003FF)<<2) | ((response[2] & 0xC0000000)>>30);
                adi_rsi_card_registers.sd.csd.vdd_r_curr_min = (SD_CSD_VDD_CURR_MIN)(response[2] & 0x38000000)>>27;
                adi_rsi_card_registers.sd.csd.vdd_r_curr_max = (SD_CSD_VDD_CURR_MAX)(response[2] & 0x07000000)>>24;
                adi_rsi_card_registers.sd.csd.vdd_w_curr_min = (SD_CSD_VDD_CURR_MIN)(response[2] & 0x00E00000)>>21;
                adi_rsi_card_registers.sd.csd.vdd_w_curr_max = (SD_CSD_VDD_CURR_MAX)(response[2] & 0x001C0000)>>18;
                adi_rsi_card_registers.sd.csd.c_size_mult = (response[2] & 0x00038000)>>15;
            }
            else if(adi_rsi_card_registers.sd.csd.csd_structure == SD_CSD_STRUCTURE_VERSION_2_0)
            {
                adi_rsi_card_registers.sd.csd.vdd_r_curr_min = SD_CSD_VDD_CURR_MIN_NOT_APPLICABLE;
                adi_rsi_card_registers.sd.csd.vdd_r_curr_max = SD_CSD_VDD_CURR_MAX_NOT_APPLICABLE;
                adi_rsi_card_registers.sd.csd.vdd_w_curr_min = SD_CSD_VDD_CURR_MIN_NOT_APPLICABLE;
                adi_rsi_card_registers.sd.csd.vdd_w_curr_max = SD_CSD_VDD_CURR_MAX_NOT_APPLICABLE;
                adi_rsi_card_registers.sd.csd.c_size_mult = 0;
                adi_rsi_card_registers.sd.csd.c_size = ((response[1] & 0x0000003F)<<16) | ((response[2] & 0xFFFF0000)>>16);
            }
            else
            {
                while(1); /* error */
            }
            adi_rsi_card_registers.sd.csd.erase_blk_en = (response[2] & 0x00004000)>>14;
            adi_rsi_card_registers.sd.csd.sector_size = (response[2] & 0x00003F80)>>7;
            adi_rsi_card_registers.sd.csd.wp_grp_size = response[2] & 0x0000007F;
            adi_rsi_card_registers.sd.csd.wp_grp_enable = (response[3] & (0x80000000>>1))>>30;
            adi_rsi_card_registers.sd.csd.r2w_factor = (response[3] & (0x1C000000>>1))>>25;
            adi_rsi_card_registers.sd.csd.write_bl_length = (response[3] & (0x03C00000>>1))>>21;
            adi_rsi_card_registers.sd.csd.write_bl_partial = (response[3] & (0x00200000>>1))>>20;
            adi_rsi_card_registers.sd.csd.file_format_grp = (response[3] & (0x00008000>>1))>>14;
            adi_rsi_card_registers.sd.csd.copy = (response[3] & (0x00004000>>1))>>13;
            adi_rsi_card_registers.sd.csd.perm_write_protect = (response[3] & (0x00002000>>1))>>12;
            adi_rsi_card_registers.sd.csd.tmp_write_protect = (response[3] & (0x00001000>>1))>>11;
            adi_rsi_card_registers.sd.csd.file_format = (SD_CSD_FILE_FORMAT)(response[3] & (0x00000C00>>1))>>9;
            adi_rsi_card_registers.sd.csd.crc = (response[3] & (0x000000FE>>1))>>0;
        }
    }
    else
    {
        error = sd_mmc_send_command(SD_MMC_CMD_SEND_CSD, (adi_rsi_card_registers.mmc.rca<<16), RESPONSE_LONG, TRANS_NONE, CRCDIS);

        if(!error)
        {
            adi_rsi_GetLongResponse(hDevice, response);

            adi_rsi_card_registers.mmc.csd.csd_structure = (MMC_CSD_STRUCTURE)((response[0] & 0xC0000000)>>30);
            adi_rsi_card_registers.mmc.csd.spec_vers = (MMC_CSD_SPEC_VERS)((response[0] & 0x3C000000)>>26);
            adi_rsi_card_registers.mmc.csd.taac = (response[0] & 0x00FF0000)>>16;
            adi_rsi_card_registers.mmc.csd.nsac = (response[0] & 0x0000FF00)>>8;
            adi_rsi_card_registers.mmc.csd.tran_speed = response[0] & 0x000000FF;
            adi_rsi_card_registers.mmc.csd.ccc = (response[1] & 0xFFF00000)>>20;
            adi_rsi_card_registers.mmc.csd.read_bl_len = (response[1] & 0x000F0000)>>16;
            adi_rsi_card_registers.mmc.csd.read_bl_partial = (response[1] & 0x00008000)>>15;
            adi_rsi_card_registers.mmc.csd.write_blk_misalign = (response[1] & 0x00004000)>>14;
            adi_rsi_card_registers.mmc.csd.read_blk_misalign = (response[1] & 0x00002000)>>13;
            adi_rsi_card_registers.mmc.csd.dsr_imp = (response[1] & 0x00001000)>>12;
            adi_rsi_card_registers.mmc.csd.c_size = ((response[1] & 0x000003FF)<<2) | ((response[2] & 0xC0000000)>>30);
            adi_rsi_card_registers.mmc.csd.vdd_r_curr_min = (MMC_CSD_VDD_CURR_MIN)(response[2] & 0x38000000)>>27;
            adi_rsi_card_registers.mmc.csd.vdd_r_curr_max = (MMC_CSD_VDD_CURR_MAX)(response[2] & 0x07000000)>>24;
            adi_rsi_card_registers.mmc.csd.vdd_w_curr_min = (MMC_CSD_VDD_CURR_MIN)(response[2] & 0x00E00000)>>21;
            adi_rsi_card_registers.mmc.csd.vdd_w_curr_max = (MMC_CSD_VDD_CURR_MAX)(response[2] & 0x001C0000)>>18;
            adi_rsi_card_registers.mmc.csd.c_size_mult = (response[2] & 0x00038000)>>15;
            adi_rsi_card_registers.mmc.csd.erase_grp_size = (response[2] & 0x00007C00)>>10;
            adi_rsi_card_registers.mmc.csd.erase_grp_mult = (response[2] & 0x000003E0)>>5;
            adi_rsi_card_registers.mmc.csd.wp_grp_size = response[2] & 0x0000001F;
            adi_rsi_card_registers.mmc.csd.wp_grp_enable = (response[3] & (0x80000000>>1))>>30;
            adi_rsi_card_registers.mmc.csd.default_ecc = (MMC_CSD_ECC)(response[3] & (0x60000000>>1))>>28;
            adi_rsi_card_registers.mmc.csd.r2w_factor = (response[3] & (0x1C000000>>1))>>25;
            adi_rsi_card_registers.mmc.csd.write_bl_length = (response[3] & (0x03C00000>>1))>>21;
            adi_rsi_card_registers.mmc.csd.write_bl_partial = (response[3] & (0x00200000>>1))>>20;
            adi_rsi_card_registers.mmc.csd.content_prot_app = (response[3] & (0x00010000>>1))>>15;
            adi_rsi_card_registers.mmc.csd.file_format_grp = (response[3] & (0x00008000>>1))>>14;
            adi_rsi_card_registers.mmc.csd.copy = (response[3] & (0x00004000>>1))>>13;
            adi_rsi_card_registers.mmc.csd.perm_write_protect = (response[3] & (0x00002000>>1))>>12;
            adi_rsi_card_registers.mmc.csd.tmp_write_protect = (response[3] & (0x00001000>>1))>>11;
            adi_rsi_card_registers.mmc.csd.file_format = (MMC_CSD_FILE_FORMAT)(response[3] & (0x00000C00>>1))>>9;
            adi_rsi_card_registers.mmc.csd.ecc = (MMC_CSD_ECC)((response[3] & (0x00000300>>1))>>7);
            adi_rsi_card_registers.mmc.csd.crc = (response[3] & (0x000000FE>>1))>>0;
        }
    }

    return error;
}

static uint32_t sd_mmc_read_csd_extended(void)
{
    uint32_t error = 0;
    ADI_CACHE_ALIGN static uint8_t ext_csd_temp[ADI_CACHE_ROUND_UP_SIZE(512u, uint8_t)];

    adi_rsi_SetBlockCntAndLen(hDevice, 1u, 512u);
    adi_rsi_SubmitRxBuffer(hDevice, ext_csd_temp, 512, 1);

    error = sd_mmc_send_command(MMC_CMD_SEND_EXT_CSD, 0, RESPONSE_SHORT, TRANS_READ, CRCDIS);

    void *pBuffer;
    adi_rsi_GetRxBuffer (hDevice, &pBuffer);

    adi_rsi_card_registers.mmc.extcsd.s_cmd_set = ext_csd_temp[504];
    adi_rsi_card_registers.mmc.extcsd.boot_info = ext_csd_temp[238];
    adi_rsi_card_registers.mmc.extcsd.boot_size_mult = ext_csd_temp[226];
    adi_rsi_card_registers.mmc.extcsd.acc_size = ext_csd_temp[225];
    adi_rsi_card_registers.mmc.extcsd.hc_erase_grp_size = ext_csd_temp[224];
    adi_rsi_card_registers.mmc.extcsd.erase_timeout_mult = ext_csd_temp[223];
    adi_rsi_card_registers.mmc.extcsd.rel_wr_sec_c = ext_csd_temp[222];
    adi_rsi_card_registers.mmc.extcsd.hc_wp_grp_size = ext_csd_temp[221];
    adi_rsi_card_registers.mmc.extcsd.s_c_vcc = ext_csd_temp[220];
    adi_rsi_card_registers.mmc.extcsd.s_c_vccq = ext_csd_temp[219];
    adi_rsi_card_registers.mmc.extcsd.s_a_timeout = ext_csd_temp[217];
    adi_rsi_card_registers.mmc.extcsd.sec_count = (uint32_t)ext_csd_temp[212];
    adi_rsi_card_registers.mmc.extcsd.min_perf_w_8_52 = ext_csd_temp[210];
    adi_rsi_card_registers.mmc.extcsd.min_perf_r_8_52 = ext_csd_temp[209];
    adi_rsi_card_registers.mmc.extcsd.min_perf_w_8_26_4_52 = ext_csd_temp[208];
    adi_rsi_card_registers.mmc.extcsd.min_perf_r_8_26_4_52 = ext_csd_temp[207];
    adi_rsi_card_registers.mmc.extcsd.min_perf_w_4_26 = ext_csd_temp[206];
    adi_rsi_card_registers.mmc.extcsd.min_perf_r_4_26 = ext_csd_temp[205];
    adi_rsi_card_registers.mmc.extcsd.pwr_cl_26_360 = ext_csd_temp[203];
    adi_rsi_card_registers.mmc.extcsd.pwr_cl_52_360 = ext_csd_temp[202];
    adi_rsi_card_registers.mmc.extcsd.pwr_cl_26_195 = ext_csd_temp[201];
    adi_rsi_card_registers.mmc.extcsd.pwr_cl_52_195 = ext_csd_temp[200];
    adi_rsi_card_registers.mmc.extcsd.card_type = ext_csd_temp[196];
    adi_rsi_card_registers.mmc.extcsd.csd_structure = ext_csd_temp[194];
    adi_rsi_card_registers.mmc.extcsd.ext_csd_rev = ext_csd_temp[192];
    adi_rsi_card_registers.mmc.extcsd.cmd_set = ext_csd_temp[191];
    adi_rsi_card_registers.mmc.extcsd.cmd_set_rev = ext_csd_temp[189];
    adi_rsi_card_registers.mmc.extcsd.power_class = ext_csd_temp[187];
    adi_rsi_card_registers.mmc.extcsd.hs_timing = ext_csd_temp[185];
    adi_rsi_card_registers.mmc.extcsd.bus_width = ext_csd_temp[183];
    adi_rsi_card_registers.mmc.extcsd.erased_mem_cont = ext_csd_temp[181];
    adi_rsi_card_registers.mmc.extcsd.boot_config = ext_csd_temp[179];
    adi_rsi_card_registers.mmc.extcsd.boot_bus_width = ext_csd_temp[177];
    adi_rsi_card_registers.mmc.extcsd.erase_group_def = ext_csd_temp[175];

    return error;
}

uint32_t sd_mmc_select(void)
{
    uint32_t error = 0;
    uint32_t rca = 0;
    switch(adi_rsi_card_registers.type)
    {
        case SD_MMC_CARD_TYPE_SD_V1X:
        case SD_MMC_CARD_TYPE_SD_V2X:
        case SD_MMC_CARD_TYPE_SD_V2X_HIGH_CAPACITY:
            rca = adi_rsi_card_registers.sd.rca;
            break;
        case SD_MMC_CARD_TYPE_MMC:
        case SD_MMC_CARD_TYPE_MMC_CARD_HIGH_CAPACITY:
            rca = adi_rsi_card_registers.mmc.rca;
            break;
        case UNUSABLE_CARD:
        case CARD_SLOT_NOT_INITIALIZED:
            error = 1;
            break;
        default:
            error = 1;
            break;
    }

    if(!error)
    {
        error = sd_mmc_send_command(SD_MMC_CMD_SELECT_DESELECT_CARD,(rca<<16), RESPONSE_SHORT, TRANS_NONE, CRCDIS);
    }

    return error;
}

uint32_t mmc_set_high_speed()
{
    uint32_t error = 0;
    SD_MMC_SPEED_GRADE speed_grade;

    speed_grade = GetMMCHighTransferSpeed();

    if((speed_grade == MMC_SPEED_GRADE_26MHZ) || (speed_grade == MMC_SPEED_GRADE_52MHZ))
    {
        error = sd_mmc_send_command(MMC_CMD_SWITCH, 0x03B90100, RESPONSE_SHORT, TRANS_NONE, CRCDIS);
        if(!error)
        {
            MMCSetOptimumHighSpeed(speed_grade);
        }
    }

    return error;
}

uint32_t sd_mmc_set_bus_width(unsigned short width)
{
    uint32_t error = 0;
    uint32_t argument;
    uint32_t clock_control_reg = 0;

    if((adi_rsi_card_registers.type == SD_MMC_CARD_TYPE_MMC)
	   || (adi_rsi_card_registers.type == SD_MMC_CARD_TYPE_MMC_CARD_HIGH_CAPACITY))
    {
        switch(width){
                case 1:
                	    argument = 0x03B70000;
                        clock_control_reg |= 0x0000;
                    break;
                case 4:
                	    argument = 0x03B70100;
                        clock_control_reg |= 0x0800;
                    break;
                case 8:
                	    argument = 0x03B70200;
                        clock_control_reg |= 0x1000;
                    break;
                default:
                    error = 1;
                    break;
        }

        if(!error)
        {
            error = sd_mmc_send_command(MMC_CMD_SWITCH, argument, RESPONSE_SHORT, TRANS_NONE, CRCDIS);

            if(!error)
            {
                sd_mmc_get_wait_until_ready();

                if(ADI_RSI_SUCCESS != adi_rsi_SetBusWidth(hDevice, width))
                    DEBUG_STATEMENT("Error cannot set bus width.\n");
            }
        }
    }

    else if((adi_rsi_card_registers.type == SD_MMC_CARD_TYPE_SD_V1X)
            || (adi_rsi_card_registers.type == SD_MMC_CARD_TYPE_SD_V2X)
            || (adi_rsi_card_registers.type == SD_MMC_CARD_TYPE_SD_V2X_HIGH_CAPACITY))
    {
        error = sd_mmc_send_command(SD_MMC_CMD_APP_CMD, adi_rsi_card_registers.sd.rca<<16, RESPONSE_SHORT, TRANS_NONE, CRCDIS);
        error = sd_mmc_send_command(SD_CMD_DISCONNECT_DAT3_PULLUP, 0, RESPONSE_SHORT, TRANS_NONE, 0);
        error = sd_mmc_send_command(SD_MMC_CMD_APP_CMD, adi_rsi_card_registers.sd.rca<<16, RESPONSE_SHORT, TRANS_NONE, CRCDIS);
        if(width == 1)
        {


            error = sd_mmc_send_command(SD_CMD_SET_BUS_WIDTH, 0, RESPONSE_SHORT, TRANS_NONE, CRCDIS);
            if(!error)
            {
                if(ADI_RSI_SUCCESS != adi_rsi_SetBusWidth(hDevice, width))
                	DEBUG_STATEMENT("Error cannot set bus width.\n");
            }
        }
        else if(width == 4)
        {

            error = sd_mmc_send_command(SD_CMD_SET_BUS_WIDTH, 2, RESPONSE_SHORT, TRANS_NONE, CRCDIS);
            if(!error)
            {

                if(ADI_RSI_SUCCESS != adi_rsi_SetBusWidth(hDevice, width))
                	DEBUG_STATEMENT("Error cannot set bus width.\n");

            }
        }
    }
    else
    {
        error = 1;
    }

    return error;
}

static uint32_t sd_mmc_get_wait_until_ready(void)
{
    uint32_t error = 0;
    uint32_t rca = 0;
    uint32_t status = 0;
    switch(adi_rsi_card_registers.type)
    {
        case SD_MMC_CARD_TYPE_SD_V1X:
        case SD_MMC_CARD_TYPE_SD_V2X:
        case SD_MMC_CARD_TYPE_SD_V2X_HIGH_CAPACITY:
            rca = adi_rsi_card_registers.sd.rca<<16;
            break;
        case SD_MMC_CARD_TYPE_MMC:
        case SD_MMC_CARD_TYPE_MMC_CARD_HIGH_CAPACITY:
            rca = adi_rsi_card_registers.mmc.rca<<16;
            break;
        case UNUSABLE_CARD:
        case CARD_SLOT_NOT_INITIALIZED:
            error = 1;
            break;
        default:
            error = 1;
            break;
    }

    if(!error)
    {
        while((status & 0x00000100) != 0x00000100)
        {
            error = sd_mmc_send_command(SD_MMC_CMD_SEND_STATUS, rca, RESPONSE_SHORT, TRANS_NONE, CRCDIS);

            adi_rsi_GetShortResponse(hDevice, &status);
        }
    }

    return error;
}

static uint32_t sd_mmc_get_sd_scr_register(void)
{
    uint32_t error = 0;
    uint32_t response;
    ADI_CACHE_ALIGN static uint8_t buffer[ADI_CACHE_ROUND_UP_SIZE(8u, uint8_t)];
    int i = 0;

    for(i = 0; i < 8; i++)
    	buffer[i] = 0xff;

	adi_rsi_SetBlockCntAndLen(hDevice, 1u, 8u);
	adi_rsi_SubmitRxBuffer(hDevice, buffer, 8, 1);

	error = sd_mmc_send_command(SD_MMC_CMD_SEND_STATUS, adi_rsi_card_registers.sd.rca<<16, RESPONSE_SHORT, TRANS_NONE, CRCDIS);

	adi_rsi_GetShortResponse(hDevice, &response);

	if((response & SD_CSR_CURRENT_STATE) == SD_CSR_CURRENT_STATE_STANDBY)
	{
		error = sd_mmc_send_command(   SD_MMC_CMD_SELECT_DESELECT_CARD,
                                           adi_rsi_card_registers.sd.rca<<16, RESPONSE_SHORT, TRANS_NONE, CRCDIS);

	}

	error = sd_mmc_send_command(    SD_MMC_CMD_APP_CMD,
                                    adi_rsi_card_registers.sd.rca<<16, RESPONSE_SHORT, TRANS_NONE, 0);

	error = sd_mmc_send_command(    SD_CMD_GET_CONFIG_REG,
                                    0, RESPONSE_SHORT, TRANS_READ, 0);

	if(!error)
	{
        void *pBuffer = NULL;
        adi_rsi_GetRxBuffer (hDevice, &pBuffer);

        adi_rsi_card_registers.sd.scr.scr_structure = (SD_SCR_STRUCTURE)((buffer[0] & 0xF0)>>4);
        adi_rsi_card_registers.sd.scr.sd_spec = (SD_SCR_SD_SPEC)(buffer[0] & 0x0F);
        adi_rsi_card_registers.sd.scr.data_stat_after_erase = (buffer[1] & 0x80)>>7;
        adi_rsi_card_registers.sd.scr.sd_security = (SD_SCR_SD_SECURITY)((buffer[1] & 0x70)>>4);
        adi_rsi_card_registers.sd.scr.sd_bus_widths = (SD_SCR_SD_BUS_WIDTHS)(buffer[1] & 0x0F);
    }

    return error;
}

static uint32_t sd_mmc_get_sd_ssr_register(void)
{
    uint32_t error = 0;
    uint32_t response;
    ADI_CACHE_ALIGN static uint8_t buffer[ADI_CACHE_ROUND_UP_SIZE(64u, uint8_t)];

	adi_rsi_SetBlockCntAndLen(hDevice, 1u, 64u);
	adi_rsi_SubmitRxBuffer(hDevice, buffer, 64, 1);

	adi_rsi_GetShortResponse(hDevice, &response);

    if((response & SD_CSR_CURRENT_STATE) == SD_CSR_CURRENT_STATE_STANDBY)
    {
        error = sd_mmc_send_command(   SD_MMC_CMD_SELECT_DESELECT_CARD,
                                       adi_rsi_card_registers.sd.rca<<16,
                                       RESPONSE_SHORT,
                                       TRANS_NONE,
                                       CRCDIS);
    }

    if(!error)
    {
        error = sd_mmc_send_command(    SD_MMC_CMD_APP_CMD,
                                        adi_rsi_card_registers.sd.rca<<16,
                                        RESPONSE_SHORT,
                                        TRANS_NONE,
                                        CRCDIS);

       	if(!error)
       	{
       		error = sd_mmc_send_command(    SD_CMD_GET_MEMORY_STATUS | BITM_MSI_CMD_DXPECT,
       										0/*adi_rsi_card_registers.sd.rca<<16*/,
       										RESPONSE_SHORT,
       										TRANS_READ,
       										CRCDIS);

           	if(!error)
           	{

                void *pBuffer;
                adi_rsi_GetRxBuffer (hDevice, &pBuffer);

                adi_rsi_card_registers.sd.ssr.dat_bus_width = (SD_SSR_DAT_BUS_WIDTH)((buffer[0] & 0xC0)>>6);
                adi_rsi_card_registers.sd.ssr.secured_mode = (SD_SSR_SECURED_MODE)((buffer[0] & 0x20)>>5);
                adi_rsi_card_registers.sd.ssr.sd_card_type = (SD_SSR_SD_CARD_TYPE)((buffer[2] << 8)|(buffer[3]));
                adi_rsi_card_registers.sd.ssr.size_of_protected_area = (buffer[4]<<24)|(buffer[5]<<16)|(buffer[6]<<8)|(buffer[7]);
                adi_rsi_card_registers.sd.ssr.speed_class = (SD_SSR_SPEED_CLASS)buffer[8];
                adi_rsi_card_registers.sd.ssr.performance_move = (SD_SSR_SPEED_CLASS)buffer[9];
                adi_rsi_card_registers.sd.ssr.au_size = (SD_SSR_AU_SIZE)((buffer[10] & 0xF0)>>4);
                adi_rsi_card_registers.sd.ssr.erase_size = (buffer[12]<<8) | buffer[13];
                adi_rsi_card_registers.sd.ssr.erase_timeout = (buffer[14] & 0xFC)>>2;
                adi_rsi_card_registers.sd.ssr.erase_offset = buffer[14] & 0x3;
            }
        }
    }

    return error;
}

RSI_DATA_BUS_WIDTH mmc_bus_test()
{
    RSI_DATA_BUS_WIDTH width = RSI_DATA_BUS_WIDTH_4;

    /* For MMC cards we assume a maximum bus width of that supported by the
     * RSI */
    if(ADI_RSI_MAX_BUS_WIDTH == RSI_DATA_BUS_WIDTH_8)
    {
    	width = RSI_DATA_BUS_WIDTH_8;
    }

    return width;
}

void RSI_GetClocks(void)
{
	uint32_t sclk = 0;
	ADI_PWR_RESULT ePwrResult;

	/* We need to configure MSI module to run at 50MHz. The "adi_pwr_Get***Freq()" api
	* will return the OCLK_1. The MSI module runs at OCLK_1/2.
	* Hence, while calculating the "RSI_FREQ" using CLKDIV, we need to divide
	* the clock value by 2 (received using "adi_pwr_Get***Freq()" api) to get OCLK_1/2.*/

	/* Here, OCLK1_1/2(50MHz) is used as a clock source to MSI.*/
    uint32_t oclk1;
    ePwrResult = adi_pwr_GetOutClkFreq(1, &oclk1); // Modified CCLK1 to OCLK1
    if (ePwrResult==ADI_PWR_SUCCESS)
    {
    	sclk = oclk1/2;
    }

	if (ePwrResult != ADI_PWR_SUCCESS)
	{
		DEBUG_PRINT("\Error, unable to get input frequency. Setting to %lu\n", sclk);
	}

	adi_rsi_card_registers.sclk = sclk;
}

uint32_t RSI_ReadBlock(uint32_t block_num, void * pReadBuffer)
{
    uint32_t error = 0;

    error = sd_mmc_read_block_dma(card_address(block_num), pReadBuffer);

    return error;
}

uint32_t RSI_WriteBlock(uint32_t block_num, void * pWriteBuffer)
{
    uint32_t error = 0;

    error = sd_mmc_write_block_dma(card_address(block_num), pWriteBuffer);

    return error;
}

uint32_t RSI_ReadMultipleBlocks(uint32_t block_num, void * pReadBuffer, uint32_t num_blocks)
{
    uint32_t error = 0;

    error = sd_mmc_read_mblocks_dma(card_address(block_num), pReadBuffer, num_blocks);

    return error;
}

uint32_t RSI_WriteMultipleBlocks(uint32_t block_num, void * pWriteBuffer, uint32_t num_blocks)
{
    uint32_t error = 0;

    error = sd_mmc_write_mblocks_dma(card_address(block_num), pWriteBuffer, num_blocks);

    return error;
}

uint64_t RSI_GetSDCapacity()
{
    uint64_t card_size = 0;
    if (adi_rsi_card_registers.sd.csd.csd_structure == SD_CSD_STRUCTURE_VERSION_1_0) {
        uint64_t mult = adi_rsi_card_registers.sd.csd.c_size_mult << 8;
        uint64_t block_nr = (adi_rsi_card_registers.sd.csd.c_size + 1) * mult;
        uint64_t block_len = adi_rsi_card_registers.sd.csd.read_bl_len << 12;
        card_size = mult * block_nr * block_len;
    }
    else if (adi_rsi_card_registers.sd.csd.csd_structure == SD_CSD_STRUCTURE_VERSION_2_0) {
        card_size = (adi_rsi_card_registers.sd.csd.c_size + 1) * 512 * 1024;
    }
    return card_size;
}

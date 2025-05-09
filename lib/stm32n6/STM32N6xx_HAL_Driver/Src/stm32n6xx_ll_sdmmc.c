/**
  ******************************************************************************
  * @file    stm32n6xx_ll_sdmmc.c
  * @author  MCD Application Team
  * @brief   SDMMC Low Layer HAL module driver.
  *
  *          This file provides firmware functions to manage the following
  *          functionalities of the SDMMC peripheral:
  *           + Initialization/de-initialization functions
  *           + I/O operation functions
  *           + Peripheral Control functions
  *           + Peripheral State functions
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @verbatim
  ==============================================================================
                       ##### SDMMC peripheral features #####
  ==============================================================================
    [..] The SD/SDMMC MMC card host interface (SDMMC) provides an interface between the AHB
         peripheral bus and MultiMedia cards (MMCs), SD memory cards, SDMMC cards and CE-ATA
         devices.

    [..] The SDMMC features include the following:
         (+) Full compliance with MultiMediaCard System Specification Version 4.51. Card support
             for three different databus modes: 1-bit (default), 4-bit and 8-bit.
         (+) Full compatibility with previous versions of MultiMediaCards (backward compatibility).
         (+) Full compliance with SD memory card specifications version 4.1.
             (SDR104 SDMMC_CK speed limited to maximum allowed IO speed, SPI mode and
              UHS-II mode not supported).
         (+) Full compliance with SDIO card specification version 4.0. Card support
             for two different databus modes: 1-bit (default) and 4-bit.
             (SDR104 SDMMC_CK speed limited to maximum allowed IO speed, SPI mode and
              UHS-II mode not supported).
         (+) Data transfer up to 208 Mbyte/s for the 8 bit mode. (depending maximum allowed IO speed).
         (+) Data and command output enable signals to control external bidirectional drivers

                           ##### How to use this driver #####
  ==============================================================================
    [..]
      This driver is a considered as a driver of service for external devices drivers
      that interfaces with the SDMMC peripheral.
      According to the device used (SD card/ MMC card / SDMMC card ...), a set of APIs
      is used in the device's driver to perform SDMMC operations and functionalities.

      This driver is almost transparent for the final user, it is only used to implement other
      functionalities of the external device.

    [..]
      (+) The SDMMC clock is coming from output of PLL1_Q or PLL2_R.
       Before start working with SDMMC peripheral make sure that the PLL is well configured.
          The SDMMC peripheral uses two clock signals:
          (++) PLL1_Q bus clock (default after reset)
          (++) PLL2_R bus clock

      (+) Enable/Disable peripheral clock using RCC peripheral macros related to SDMMC
          peripheral.

      (+) Enable the Power ON State using the SDMMC_PowerState_ON(SDMMCx)
          function and disable it using the function SDMMC_PowerState_OFF(SDMMCx).

      (+) Enable/Disable the peripheral interrupts using the macros __SDMMC_ENABLE_IT(hSDMMC, IT)
          and __SDMMC_DISABLE_IT(hSDMMC, IT) if you need to use interrupt mode.

      (+) When using the DMA mode
          (++) Configure the IDMA mode (Single buffer or double)
          (++) Configure the buffer address
          (++) Configure Data Path State Machine

      (+) To control the CPSM (Command Path State Machine) and send
          commands to the card use the SDMMC_SendCommand(SDMMCx),
          SDMMC_GetCommandResponse() and SDMMC_GetResponse() functions. First, user has
          to fill the command structure (pointer to SDMMC_CmdInitTypeDef) according
          to the selected command to be sent.
          The parameters that should be filled are:
           (++) Command Argument
           (++) Command Index
           (++) Command Response type
           (++) Command Wait
           (++) CPSM Status (Enable or Disable).

          -@@- To check if the command is well received, read the SDMMC_CMDRESP
              register using the SDMMC_GetCommandResponse().
              The SDMMC responses registers (SDMMC_RESP1 to SDMMC_RESP2), use the
              SDMMC_GetResponse() function.

      (+) To control the DPSM (Data Path State Machine) and send/receive
           data to/from the card use the SDMMC_DataConfig(), SDMMC_GetDataCounter(),
          SDMMC_ReadFIFO(), SDMMC_WriteFIFO() and SDMMC_GetFIFOCount() functions.

    *** Read Operations ***
    =======================
    [..]
      (#) First, user has to fill the data structure (pointer to
          SDMMC_DataInitTypeDef) according to the selected data type to be received.
          The parameters that should be filled are:
           (++) Data TimeOut
           (++) Data Length
           (++) Data Block size
           (++) Data Transfer direction: should be from card (To SDMMC)
           (++) Data Transfer mode
           (++) DPSM Status (Enable or Disable)

      (#) Configure the SDMMC resources to receive the data from the card
          according to selected transfer mode (Refer to Step 8, 9 and 10).

      (#) Send the selected Read command (refer to step 11).

      (#) Use the SDMMC flags/interrupts to check the transfer status.

    *** Write Operations ***
    ========================
    [..]
     (#) First, user has to fill the data structure (pointer to
         SDMMC_DataInitTypeDef) according to the selected data type to be received.
         The parameters that should be filled are:
          (++) Data TimeOut
          (++) Data Length
          (++) Data Block size
          (++) Data Transfer direction:  should be to card (To CARD)
          (++) Data Transfer mode
          (++) DPSM Status (Enable or Disable)

     (#) Configure the SDMMC resources to send the data to the card according to
         selected transfer mode.

     (#) Send the selected Write command.

     (#) Use the SDMMC flags/interrupts to check the transfer status.

    *** Command management operations ***
    =====================================
    [..]
     (#) The commands used for Read/Write/Erase operations are managed in
         separate functions.
         Each function allows to send the needed command with the related argument,
         then check the response.
         By the same approach, you could implement a command and check the response.

  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup SDMMC_LL SDMMC Low Layer
  * @brief Low layer module for SD
  * @{
  */

#if defined (SDMMC1) || defined (SDMMC2)
#if defined (HAL_SD_MODULE_ENABLED) || defined (HAL_MMC_MODULE_ENABLED)  || defined (HAL_SDIO_MODULE_ENABLED)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static uint32_t SDMMC_GetCmdError(SDMMC_TypeDef *SDMMCx);

/* Exported functions --------------------------------------------------------*/

/** @defgroup SDMMC_LL_Exported_Functions SDMMC Low Layer Exported Functions
  * @{
  */

/** @defgroup HAL_SDMMC_LL_Group1 Initialization de-initialization functions
  *  @brief    Initialization and Configuration functions
  *
@verbatim
 ===============================================================================
              ##### Initialization/de-initialization functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the SDMMC according to the specified
  *         parameters in the SDMMC_InitTypeDef and create the associated handle.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @param  Init: SDMMC initialization structure
  * @retval HAL status
  */
HAL_StatusTypeDef SDMMC_Init(SDMMC_TypeDef *SDMMCx, SDMMC_InitTypeDef Init)
{
  uint32_t tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_SDMMC_ALL_INSTANCE(SDMMCx));
  assert_param(IS_SDMMC_CLOCK_EDGE(Init.ClockEdge));
  assert_param(IS_SDMMC_CLOCK_POWER_SAVE(Init.ClockPowerSave));
  assert_param(IS_SDMMC_BUS_WIDE(Init.BusWide));
  assert_param(IS_SDMMC_HARDWARE_FLOW_CONTROL(Init.HardwareFlowControl));
  assert_param(IS_SDMMC_CLKDIV(Init.ClockDiv));

  /* Set SDMMC configuration parameters */
  tmpreg |= (Init.ClockEdge           | \
             Init.ClockPowerSave      | \
             Init.BusWide             | \
             Init.HardwareFlowControl | \
             Init.ClockDiv
            );

  /* Write to SDMMC CLKCR */
  MODIFY_REG(SDMMCx->CLKCR, CLKCR_CLEAR_MASK, tmpreg);

  return HAL_OK;
}


/**
  * @}
  */

/** @defgroup HAL_SDMMC_LL_Group2 IO operation functions
  *  @brief   Data transfers functions
  *
@verbatim
 ===============================================================================
                      ##### I/O operation functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to manage the SDMMC data
    transfers.

@endverbatim
  * @{
  */

/**
  * @brief  Read data (word) from Rx FIFO in blocking mode (polling)
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval HAL status
  */
uint32_t SDMMC_ReadFIFO(const SDMMC_TypeDef *SDMMCx)
{
  /* Read data from Rx FIFO */
  return (SDMMCx->FIFO);
}

/**
  * @brief  Write data (word) to Tx FIFO in blocking mode (polling)
  * @param  SDMMCx: Pointer to SDMMC register base
  * @param  pWriteData: pointer to data to write
  * @retval HAL status
  */
HAL_StatusTypeDef SDMMC_WriteFIFO(SDMMC_TypeDef *SDMMCx, uint32_t *pWriteData)
{
  /* Write data to FIFO */
  SDMMCx->FIFO = *pWriteData;

  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup HAL_SDMMC_LL_Group3 Peripheral Control functions
  *  @brief   management functions
  *
@verbatim
 ===============================================================================
                      ##### Peripheral Control functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the SDMMC data
    transfers.

@endverbatim
  * @{
  */

/**
  * @brief  Set SDMMC Power state to ON.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval HAL status
  */
HAL_StatusTypeDef SDMMC_PowerState_ON(SDMMC_TypeDef *SDMMCx)
{
  /* Set power state to ON */
  SDMMCx->POWER |= SDMMC_POWER_PWRCTRL;

  return HAL_OK;
}

/**
  * @brief  Set SDMMC Power state to Power-Cycle.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval HAL status
  */
HAL_StatusTypeDef SDMMC_PowerState_Cycle(SDMMC_TypeDef *SDMMCx)
{
  /* Set power state to Power Cycle*/
  SDMMCx->POWER |= SDMMC_POWER_PWRCTRL_1;

  return HAL_OK;
}

/**
  * @brief  Set SDMMC Power state to OFF.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval HAL status
  */
HAL_StatusTypeDef SDMMC_PowerState_OFF(SDMMC_TypeDef *SDMMCx)
{
  /* Set power state to OFF */
  SDMMCx->POWER &= ~(SDMMC_POWER_PWRCTRL);

  return HAL_OK;
}

/**
  * @brief  Get SDMMC Power state.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval Power status of the controller. The returned value can be one of the
  *         following values:
  *            - 0x00: Power OFF
  *            - 0x02: Power UP
  *            - 0x03: Power ON
  */
uint32_t SDMMC_GetPowerState(const SDMMC_TypeDef *SDMMCx)
{
  return (SDMMCx->POWER & SDMMC_POWER_PWRCTRL);
}

/**
  * @brief  Configure the SDMMC command path according to the specified parameters in
  *         SDMMC_CmdInitTypeDef structure and send the command
  * @param  SDMMCx: Pointer to SDMMC register base
  * @param  Command: pointer to a SDMMC_CmdInitTypeDef structure that contains
  *         the configuration information for the SDMMC command
  * @retval HAL status
  */
HAL_StatusTypeDef SDMMC_SendCommand(SDMMC_TypeDef *SDMMCx, const SDMMC_CmdInitTypeDef *Command)
{
  uint32_t tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_SDMMC_CMD_INDEX(Command->CmdIndex));
  assert_param(IS_SDMMC_RESPONSE(Command->Response));
  assert_param(IS_SDMMC_WAIT(Command->WaitForInterrupt));
  assert_param(IS_SDMMC_CPSM(Command->CPSM));

  /* Set the SDMMC Argument value */
  SDMMCx->ARG = Command->Argument;

  /* Set SDMMC command parameters */
  tmpreg |= (uint32_t)(Command->CmdIndex         | \
                       Command->Response         | \
                       Command->WaitForInterrupt | \
                       Command->CPSM);

  /* Write to SDMMC CMD register */
  MODIFY_REG(SDMMCx->CMD, CMD_CLEAR_MASK, tmpreg);

  return HAL_OK;
}

/**
  * @brief  Return the command index of last command for which response received
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval Command index of the last command response received
  */
uint8_t SDMMC_GetCommandResponse(const SDMMC_TypeDef *SDMMCx)
{
  return (uint8_t)(SDMMCx->RESPCMD);
}


/**
  * @brief  Return the response received from the card for the last command
  * @param  SDMMCx: Pointer to SDMMC register base
  * @param  Response: Specifies the SDMMC response register.
  *          This parameter can be one of the following values:
  *            @arg SDMMC_RESP1: Response Register 1
  *            @arg SDMMC_RESP2: Response Register 2
  *            @arg SDMMC_RESP3: Response Register 3
  *            @arg SDMMC_RESP4: Response Register 4
  * @retval The Corresponding response register value
  */
uint32_t SDMMC_GetResponse(const SDMMC_TypeDef *SDMMCx, uint32_t Response)
{
  uint32_t tmp;

  /* Check the parameters */
  assert_param(IS_SDMMC_RESP(Response));

  /* Get the response */
  tmp = (uint32_t)(&(SDMMCx->RESP1)) + Response;

  return (*(__IO uint32_t *) tmp);
}

/**
  * @brief  Configure the SDMMC data path according to the specified
  *         parameters in the SDMMC_DataInitTypeDef.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @param  Data : pointer to a SDMMC_DataInitTypeDef structure
  *         that contains the configuration information for the SDMMC data.
  * @retval HAL status
  */
HAL_StatusTypeDef SDMMC_ConfigData(SDMMC_TypeDef *SDMMCx, const SDMMC_DataInitTypeDef *Data)
{
  uint32_t tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_SDMMC_DATA_LENGTH(Data->DataLength));
  assert_param(IS_SDMMC_BLOCK_SIZE(Data->DataBlockSize));
  assert_param(IS_SDMMC_TRANSFER_DIR(Data->TransferDir));
  assert_param(IS_SDMMC_TRANSFER_MODE(Data->TransferMode));
  assert_param(IS_SDMMC_DPSM(Data->DPSM));

  /* Set the SDMMC Data TimeOut value */
  SDMMCx->DTIMER = Data->DataTimeOut;

  /* Set the SDMMC DataLength value */
  SDMMCx->DLEN = Data->DataLength;

  /* Set the SDMMC data configuration parameters */
  tmpreg |= (uint32_t)(Data->DataBlockSize | \
                       Data->TransferDir   | \
                       Data->TransferMode  | \
                       Data->DPSM);

  /* Write to SDMMC DCTRL */
  MODIFY_REG(SDMMCx->DCTRL, DCTRL_CLEAR_MASK, tmpreg);

  return HAL_OK;

}

/**
  * @brief  Returns number of remaining data bytes to be transferred.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval Number of remaining data bytes to be transferred
  */
uint32_t SDMMC_GetDataCounter(const SDMMC_TypeDef *SDMMCx)
{
  return (SDMMCx->DCOUNT);
}

/**
  * @brief  Get the FIFO data
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval Data received
  */
uint32_t SDMMC_GetFIFOCount(const SDMMC_TypeDef *SDMMCx)
{
  return (SDMMCx->FIFO);
}

/**
  * @brief  Sets one of the two options of inserting read wait interval.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @param  SDMMC_ReadWaitMode: SDMMC Read Wait operation mode.
  *          This parameter can be:
  *            @arg SDMMC_READ_WAIT_MODE_CLK: Read Wait control by stopping SDMMCCLK
  *            @arg SDMMC_READ_WAIT_MODE_DATA2: Read Wait control using SDMMC_DATA2
  * @retval None
  */
HAL_StatusTypeDef SDMMC_SetSDMMCReadWaitMode(SDMMC_TypeDef *SDMMCx, uint32_t SDMMC_ReadWaitMode)
{
  /* Check the parameters */
  assert_param(IS_SDMMC_READWAIT_MODE(SDMMC_ReadWaitMode));

  /* Set SDMMC read wait mode */
  MODIFY_REG(SDMMCx->DCTRL, SDMMC_DCTRL_RWMOD, SDMMC_ReadWaitMode);

  return HAL_OK;
}

/**
  * @}
  */


/** @defgroup HAL_SDMMC_LL_Group4 Command management functions
  *  @brief   Data transfers functions
  *
@verbatim
 ===============================================================================
                   ##### Commands management functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to manage the needed commands.

@endverbatim
  * @{
  */

/**
  * @brief  Send the Data Block Length command and check the response
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval HAL status
  */
uint32_t SDMMC_CmdBlockLength(SDMMC_TypeDef *SDMMCx, uint32_t BlockSize)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Set Block Size for Card */
  sdmmc_cmdinit.Argument         = (uint32_t)BlockSize;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_SET_BLOCKLEN;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_SET_BLOCKLEN, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Send the Data Block number command and check the response
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval HAL status
  */
uint32_t SDMMC_CmdBlockCount(SDMMC_TypeDef *SDMMCx, uint32_t BlockCount)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Set Block Size for Card */
  sdmmc_cmdinit.Argument         = (uint32_t)BlockCount;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_SET_BLOCK_COUNT;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_SET_BLOCK_COUNT, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Send the Read Single Block command and check the response
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval HAL status
  */
uint32_t SDMMC_CmdReadSingleBlock(SDMMC_TypeDef *SDMMCx, uint32_t ReadAdd)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Set Block Size for Card */
  sdmmc_cmdinit.Argument         = (uint32_t)ReadAdd;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_READ_SINGLE_BLOCK;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_READ_SINGLE_BLOCK, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Send the Read Multi Block command and check the response
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval HAL status
  */
uint32_t SDMMC_CmdReadMultiBlock(SDMMC_TypeDef *SDMMCx, uint32_t ReadAdd)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Set Block Size for Card */
  sdmmc_cmdinit.Argument         = (uint32_t)ReadAdd;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_READ_MULT_BLOCK;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_READ_MULT_BLOCK, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Send the Write Single Block command and check the response
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval HAL status
  */
uint32_t SDMMC_CmdWriteSingleBlock(SDMMC_TypeDef *SDMMCx, uint32_t WriteAdd)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Set Block Size for Card */
  sdmmc_cmdinit.Argument         = (uint32_t)WriteAdd;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_WRITE_SINGLE_BLOCK;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_WRITE_SINGLE_BLOCK, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Send the Write Multi Block command and check the response
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval HAL status
  */
uint32_t SDMMC_CmdWriteMultiBlock(SDMMC_TypeDef *SDMMCx, uint32_t WriteAdd)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Set Block Size for Card */
  sdmmc_cmdinit.Argument         = (uint32_t)WriteAdd;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_WRITE_MULT_BLOCK;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_WRITE_MULT_BLOCK, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Send the Start Address Erase command for SD and check the response
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval HAL status
  */
uint32_t SDMMC_CmdSDEraseStartAdd(SDMMC_TypeDef *SDMMCx, uint32_t StartAdd)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Set Block Size for Card */
  sdmmc_cmdinit.Argument         = (uint32_t)StartAdd;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_SD_ERASE_GRP_START;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_SD_ERASE_GRP_START, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Send the End Address Erase command for SD and check the response
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval HAL status
  */
uint32_t SDMMC_CmdSDEraseEndAdd(SDMMC_TypeDef *SDMMCx, uint32_t EndAdd)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Set Block Size for Card */
  sdmmc_cmdinit.Argument         = (uint32_t)EndAdd;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_SD_ERASE_GRP_END;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_SD_ERASE_GRP_END, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Send the Start Address Erase command and check the response
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval HAL status
  */
uint32_t SDMMC_CmdEraseStartAdd(SDMMC_TypeDef *SDMMCx, uint32_t StartAdd)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Set Block Size for Card */
  sdmmc_cmdinit.Argument         = (uint32_t)StartAdd;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_ERASE_GRP_START;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_ERASE_GRP_START, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Send the End Address Erase command and check the response
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval HAL status
  */
uint32_t SDMMC_CmdEraseEndAdd(SDMMC_TypeDef *SDMMCx, uint32_t EndAdd)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Set Block Size for Card */
  sdmmc_cmdinit.Argument         = (uint32_t)EndAdd;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_ERASE_GRP_END;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_ERASE_GRP_END, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Send the Erase command and check the response
  * @param  SDMMCx Pointer to SDMMC register base
  * @param  EraseType Type of erase to be performed
  * @retval HAL status
  */
uint32_t SDMMC_CmdErase(SDMMC_TypeDef *SDMMCx, uint32_t EraseType)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Set Block Size for Card */
  sdmmc_cmdinit.Argument         = EraseType;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_ERASE;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_ERASE, SDMMC_MAXERASETIMEOUT);

  return errorstate;
}

/**
  * @brief  Send the Stop Transfer command and check the response.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval HAL status
  */
uint32_t SDMMC_CmdStopTransfer(SDMMC_TypeDef *SDMMCx)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Send CMD12 STOP_TRANSMISSION  */
  sdmmc_cmdinit.Argument         = 0U;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_STOP_TRANSMISSION;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;

  __SDMMC_CMDSTOP_ENABLE(SDMMCx);
  __SDMMC_CMDTRANS_DISABLE(SDMMCx);

  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_STOP_TRANSMISSION, SDMMC_STOPTRANSFERTIMEOUT);

  __SDMMC_CMDSTOP_DISABLE(SDMMCx);

  /* Ignore Address Out Of Range Error, Not relevant at end of memory */
  if (errorstate == SDMMC_ERROR_ADDR_OUT_OF_RANGE)
  {
    errorstate = SDMMC_ERROR_NONE;
  }

  return errorstate;
}

/**
  * @brief  Send the Select Deselect command and check the response.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @param  addr: Address of the card to be selected
  * @retval HAL status
  */
uint32_t SDMMC_CmdSelDesel(SDMMC_TypeDef *SDMMCx, uint32_t Addr)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Send CMD7 SDMMC_SEL_DESEL_CARD */
  sdmmc_cmdinit.Argument         = (uint32_t)Addr;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_SEL_DESEL_CARD;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_SEL_DESEL_CARD, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Send the Go Idle State command and check the response.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval HAL status
  */
uint32_t SDMMC_CmdGoIdleState(SDMMC_TypeDef *SDMMCx)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  sdmmc_cmdinit.Argument         = 0U;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_GO_IDLE_STATE;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_NO;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdError(SDMMCx);

  return errorstate;
}

/**
  * @brief  Send the Operating Condition command and check the response.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval HAL status
  */
uint32_t SDMMC_CmdOperCond(SDMMC_TypeDef *SDMMCx)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Send CMD8 to verify SD card interface operating condition */
  /* Argument: - [31:12]: Reserved (shall be set to '0')
  - [11:8]: Supply Voltage (VHS) 0x1 (Range: 2.7-3.6 V)
  - [7:0]: Check Pattern (recommended 0xAA) */
  /* CMD Response: R7 */
  sdmmc_cmdinit.Argument         = SDMMC_CHECK_PATTERN;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_HS_SEND_EXT_CSD;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp7(SDMMCx);

  return errorstate;
}

/**
  * @brief  Send the Application command to verify that that the next command
  *         is an application specific com-mand rather than a standard command
  *         and check the response.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @param  Argument: Command Argument
  * @retval HAL status
  */
uint32_t SDMMC_CmdAppCommand(SDMMC_TypeDef *SDMMCx, uint32_t Argument)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  sdmmc_cmdinit.Argument         = (uint32_t)Argument;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_APP_CMD;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  /* If there is a HAL_ERROR, it is a MMC card, else
  it is a SD card: SD card 2.0 (voltage range mismatch)
     or SD card 1.x */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_APP_CMD, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Send the command asking the accessed card to send its operating
  *         condition register (OCR)
  * @param  SDMMCx: Pointer to SDMMC register base
  * @param  Argument: Command Argument
  * @retval HAL status
  */
uint32_t SDMMC_CmdAppOperCommand(SDMMC_TypeDef *SDMMCx, uint32_t Argument)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  sdmmc_cmdinit.Argument         = Argument;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_SD_APP_OP_COND;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp3(SDMMCx);

  return errorstate;
}

/**
  * @brief  Send the Bus Width command and check the response.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @param  BusWidth: BusWidth
  * @retval HAL status
  */
uint32_t SDMMC_CmdBusWidth(SDMMC_TypeDef *SDMMCx, uint32_t BusWidth)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  sdmmc_cmdinit.Argument         = (uint32_t)BusWidth;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_APP_SD_SET_BUSWIDTH;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_APP_SD_SET_BUSWIDTH, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Send the Send SCR command and check the response.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval HAL status
  */
uint32_t SDMMC_CmdSendSCR(SDMMC_TypeDef *SDMMCx)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Send CMD51 SD_APP_SEND_SCR */
  sdmmc_cmdinit.Argument         = 0U;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_SD_APP_SEND_SCR;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_SD_APP_SEND_SCR, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Send the Send CID command and check the response.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval HAL status
  */
uint32_t SDMMC_CmdSendCID(SDMMC_TypeDef *SDMMCx)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Send CMD2 ALL_SEND_CID */
  sdmmc_cmdinit.Argument         = 0U;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_ALL_SEND_CID;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_LONG;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp2(SDMMCx);

  return errorstate;
}

/**
  * @brief  Send the Send CSD command and check the response.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @param  Argument: Command Argument
  * @retval HAL status
  */
uint32_t SDMMC_CmdSendCSD(SDMMC_TypeDef *SDMMCx, uint32_t Argument)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Send CMD9 SEND_CSD */
  sdmmc_cmdinit.Argument         = Argument;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_SEND_CSD;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_LONG;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp2(SDMMCx);

  return errorstate;
}

/**
  * @brief  Send the Send CSD command and check the response.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @param  pRCA: Card RCA
  * @retval HAL status
  */
uint32_t SDMMC_CmdSetRelAdd(SDMMC_TypeDef *SDMMCx, uint16_t *pRCA)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Send CMD3 SD_CMD_SET_REL_ADDR */
  sdmmc_cmdinit.Argument         = 0U;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_SET_REL_ADDR;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp6(SDMMCx, SDMMC_CMD_SET_REL_ADDR, pRCA);

  return errorstate;
}

/**
  * @brief  Send the Set Relative Address command to MMC card (not SD card).
  * @param  SDMMCx Pointer to SDMMC register base
  * @param  RCA Card RCA
  * @retval HAL status
  */
uint32_t SDMMC_CmdSetRelAddMmc(SDMMC_TypeDef *SDMMCx, uint16_t RCA)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Send CMD3 SD_CMD_SET_REL_ADDR */
  sdmmc_cmdinit.Argument         = ((uint32_t)RCA << 16U);
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_SET_REL_ADDR;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_SET_REL_ADDR, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Send the Sleep command to MMC card (not SD card).
  * @param  SDMMCx Pointer to SDMMC register base
  * @param  Argument Argument of the command (RCA and Sleep/Awake)
  * @retval HAL status
  */
uint32_t SDMMC_CmdSleepMmc(SDMMC_TypeDef *SDMMCx, uint32_t Argument)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Send CMD5 SDMMC_CMD_MMC_SLEEP_AWAKE */
  sdmmc_cmdinit.Argument         = Argument;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_MMC_SLEEP_AWAKE;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_MMC_SLEEP_AWAKE, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Send the Status command and check the response.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @param  Argument: Command Argument
  * @retval HAL status
  */
uint32_t SDMMC_CmdSendStatus(SDMMC_TypeDef *SDMMCx, uint32_t Argument)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  sdmmc_cmdinit.Argument         = Argument;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_SEND_STATUS;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_SEND_STATUS, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Send the Status register command and check the response.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval HAL status
  */
uint32_t SDMMC_CmdStatusRegister(SDMMC_TypeDef *SDMMCx)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  sdmmc_cmdinit.Argument         = 0U;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_SD_APP_STATUS;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_SD_APP_STATUS, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Sends host capacity support information and activates the card's
  *         initialization process. Send SDMMC_CMD_SEND_OP_COND command
  * @param  SDMMCx: Pointer to SDMMC register base
  * @parame Argument: Argument used for the command
  * @retval HAL status
  */
uint32_t SDMMC_CmdOpCondition(SDMMC_TypeDef *SDMMCx, uint32_t Argument)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  sdmmc_cmdinit.Argument         = Argument;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_SEND_OP_COND;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp3(SDMMCx);

  return errorstate;
}

/**
  * @brief  Checks switchable function and switch card function. SDMMC_CMD_HS_SWITCH command
  * @param  SDMMCx: Pointer to SDMMC register base
  * @parame Argument: Argument used for the command
  * @retval HAL status
  */
uint32_t SDMMC_CmdSwitch(SDMMC_TypeDef *SDMMCx, uint32_t Argument)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Send CMD6 to activate SDR50 Mode and Power Limit 1.44W */
  /* CMD Response: R1 */
  sdmmc_cmdinit.Argument         = Argument; /* SDMMC_SDR25_SWITCH_PATTERN*/
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_HS_SWITCH;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_HS_SWITCH, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Send the command asking the accessed card to send its operating
  *         condition register (OCR)
  * @param  None
  * @retval HAL status
  */
uint32_t SDMMC_CmdVoltageSwitch(SDMMC_TypeDef *SDMMCx)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  sdmmc_cmdinit.Argument         = 0x00000000;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_VOLTAGE_SWITCH;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_VOLTAGE_SWITCH, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Send the Send EXT_CSD command and check the response.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @param  Argument: Command Argument
  * @retval HAL status
  */
uint32_t SDMMC_CmdSendEXTCSD(SDMMC_TypeDef *SDMMCx, uint32_t Argument)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  /* Send CMD9 SEND_CSD */
  sdmmc_cmdinit.Argument         = Argument;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_HS_SEND_EXT_CSD;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp1(SDMMCx, SDMMC_CMD_HS_SEND_EXT_CSD, SDMMC_CMDTIMEOUT);

  return errorstate;
}

/**
  * @brief  Execute a cmd52 to write single byte data and read single byte data if needed
  * @param  SDMMCx: Pointer to SDMMC register base
  * @param  Argument:  SDMMC command argument which is sent to a card as part of a command message
  * @param  pData: pointer to read response if needed
  * @retval SD Card error state
  */
uint32_t SDMMC_SDIO_CmdReadWriteDirect(SDMMC_TypeDef *SDMMCx, uint32_t Argument, uint8_t *pResponse)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  sdmmc_cmdinit.Argument         = Argument;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_SDMMC_RW_DIRECT;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp5(SDMMCx, SDMMC_CMD_SDMMC_RW_DIRECT, pResponse);

  return errorstate;
}

/**
  * @brief  Execute a cmd53 to write or read multiple data with a single command
  * @param  SDMMCx: Pointer to SDMMC register base
  * @param  Argument: SDMMC command argument which is sent to a card as part of a command message
  * @retval SD Card error state
  */
uint32_t SDMMC_SDIO_CmdReadWriteExtended(SDMMC_TypeDef *SDMMCx, uint32_t Argument)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinit;
  uint32_t errorstate;

  sdmmc_cmdinit.Argument         = Argument;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_SDMMC_RW_EXTENDED;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp5(SDMMCx, SDMMC_CMD_SDMMC_RW_EXTENDED, NULL);

  return errorstate;
}

/**
  * @brief  Execute a cmd5 to write or read multiple data with a single command
  * @param  SDMMCx: Pointer to SDMMC register base
  * @param  Argument: SDMMC command argument which is sent to a card as part of a command message
  * @retval SD Card error state
  */
uint32_t SDMMC_CmdSendOperationcondition(SDMMC_TypeDef *SDMMCx, uint32_t Argument, uint32_t *pResp)
{
  SDMMC_CmdInitTypeDef sdmmc_cmdinit;
  uint32_t             errorstate;

  sdmmc_cmdinit.Argument         = Argument;
  sdmmc_cmdinit.CmdIndex         = SDMMC_CMD_SDMMC_SEN_OP_COND;
  sdmmc_cmdinit.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
  (void)SDMMC_SendCommand(SDMMCx, &sdmmc_cmdinit);

  /* Check for error conditions */
  errorstate = SDMMC_GetCmdResp4(SDMMCx, pResp);

  return errorstate;
}
/**
  * @}
  */


/** @defgroup HAL_SDMMC_LL_Group5 Responses management functions
  *  @brief   Responses functions
  *
@verbatim
 ===============================================================================
                   ##### Responses management functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to manage the needed responses.

@endverbatim
  * @{
  */
/**
  * @brief  Checks for error conditions for R1 response.
  * @param  hsd: SD handle
  * @param  SD_CMD: The sent command index
  * @retval SD Card error state
  */
uint32_t SDMMC_GetCmdResp1(SDMMC_TypeDef *SDMMCx, uint8_t SD_CMD, uint32_t Timeout)
{
  uint32_t response_r1;
  uint32_t sta_reg;

  /* 8 is the number of required instructions cycles for the below loop statement.
  The Timeout is expressed in ms */
  uint32_t count = Timeout * (SystemCoreClock / 8U / 1000U);

  do
  {
    if (count-- == 0U)
    {
      return SDMMC_ERROR_TIMEOUT;
    }
    sta_reg = SDMMCx->STA;
  } while (((sta_reg & (SDMMC_FLAG_CCRCFAIL | SDMMC_FLAG_CMDREND | SDMMC_FLAG_CTIMEOUT |
                        SDMMC_FLAG_BUSYD0END)) == 0U) || ((sta_reg & SDMMC_FLAG_CMDACT) != 0U));

  if (__SDMMC_GET_FLAG(SDMMCx, SDMMC_FLAG_CTIMEOUT))
  {
    __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_FLAG_CTIMEOUT);

    return SDMMC_ERROR_CMD_RSP_TIMEOUT;
  }
  else if (__SDMMC_GET_FLAG(SDMMCx, SDMMC_FLAG_CCRCFAIL))
  {
    __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_FLAG_CCRCFAIL);

    return SDMMC_ERROR_CMD_CRC_FAIL;
  }
  else
  {
    /* Nothing to do */
  }

  /* Clear all the static flags */
  __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_STATIC_CMD_FLAGS);

  /* Check response received is of desired command */
  if (SDMMC_GetCommandResponse(SDMMCx) != SD_CMD)
  {
    return SDMMC_ERROR_CMD_CRC_FAIL;
  }

  /* We have received response, retrieve it for analysis  */
  response_r1 = SDMMC_GetResponse(SDMMCx, SDMMC_RESP1);

  if ((response_r1 & SDMMC_OCR_ERRORBITS) == SDMMC_ALLZERO)
  {
    return SDMMC_ERROR_NONE;
  }
  else if ((response_r1 & SDMMC_OCR_ADDR_OUT_OF_RANGE) == SDMMC_OCR_ADDR_OUT_OF_RANGE)
  {
    return SDMMC_ERROR_ADDR_OUT_OF_RANGE;
  }
  else if ((response_r1 & SDMMC_OCR_ADDR_MISALIGNED) == SDMMC_OCR_ADDR_MISALIGNED)
  {
    return SDMMC_ERROR_ADDR_MISALIGNED;
  }
  else if ((response_r1 & SDMMC_OCR_BLOCK_LEN_ERR) == SDMMC_OCR_BLOCK_LEN_ERR)
  {
    return SDMMC_ERROR_BLOCK_LEN_ERR;
  }
  else if ((response_r1 & SDMMC_OCR_ERASE_SEQ_ERR) == SDMMC_OCR_ERASE_SEQ_ERR)
  {
    return SDMMC_ERROR_ERASE_SEQ_ERR;
  }
  else if ((response_r1 & SDMMC_OCR_BAD_ERASE_PARAM) == SDMMC_OCR_BAD_ERASE_PARAM)
  {
    return SDMMC_ERROR_BAD_ERASE_PARAM;
  }
  else if ((response_r1 & SDMMC_OCR_WRITE_PROT_VIOLATION) == SDMMC_OCR_WRITE_PROT_VIOLATION)
  {
    return SDMMC_ERROR_WRITE_PROT_VIOLATION;
  }
  else if ((response_r1 & SDMMC_OCR_LOCK_UNLOCK_FAILED) == SDMMC_OCR_LOCK_UNLOCK_FAILED)
  {
    return SDMMC_ERROR_LOCK_UNLOCK_FAILED;
  }
  else if ((response_r1 & SDMMC_OCR_COM_CRC_FAILED) == SDMMC_OCR_COM_CRC_FAILED)
  {
    return SDMMC_ERROR_COM_CRC_FAILED;
  }
  else if ((response_r1 & SDMMC_OCR_ILLEGAL_CMD) == SDMMC_OCR_ILLEGAL_CMD)
  {
    return SDMMC_ERROR_ILLEGAL_CMD;
  }
  else if ((response_r1 & SDMMC_OCR_CARD_ECC_FAILED) == SDMMC_OCR_CARD_ECC_FAILED)
  {
    return SDMMC_ERROR_CARD_ECC_FAILED;
  }
  else if ((response_r1 & SDMMC_OCR_CC_ERROR) == SDMMC_OCR_CC_ERROR)
  {
    return SDMMC_ERROR_CC_ERR;
  }
  else if ((response_r1 & SDMMC_OCR_STREAM_READ_UNDERRUN) == SDMMC_OCR_STREAM_READ_UNDERRUN)
  {
    return SDMMC_ERROR_STREAM_READ_UNDERRUN;
  }
  else if ((response_r1 & SDMMC_OCR_STREAM_WRITE_OVERRUN) == SDMMC_OCR_STREAM_WRITE_OVERRUN)
  {
    return SDMMC_ERROR_STREAM_WRITE_OVERRUN;
  }
  else if ((response_r1 & SDMMC_OCR_CID_CSD_OVERWRITE) == SDMMC_OCR_CID_CSD_OVERWRITE)
  {
    return SDMMC_ERROR_CID_CSD_OVERWRITE;
  }
  else if ((response_r1 & SDMMC_OCR_WP_ERASE_SKIP) == SDMMC_OCR_WP_ERASE_SKIP)
  {
    return SDMMC_ERROR_WP_ERASE_SKIP;
  }
  else if ((response_r1 & SDMMC_OCR_CARD_ECC_DISABLED) == SDMMC_OCR_CARD_ECC_DISABLED)
  {
    return SDMMC_ERROR_CARD_ECC_DISABLED;
  }
  else if ((response_r1 & SDMMC_OCR_ERASE_RESET) == SDMMC_OCR_ERASE_RESET)
  {
    return SDMMC_ERROR_ERASE_RESET;
  }
  else if ((response_r1 & SDMMC_OCR_AKE_SEQ_ERROR) == SDMMC_OCR_AKE_SEQ_ERROR)
  {
    return SDMMC_ERROR_AKE_SEQ_ERR;
  }
  else
  {
    return SDMMC_ERROR_GENERAL_UNKNOWN_ERR;
  }
}

/**
  * @brief  Checks for error conditions for R2 (CID or CSD) response.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
uint32_t SDMMC_GetCmdResp2(SDMMC_TypeDef *SDMMCx)
{
  uint32_t sta_reg;
  /* 8 is the number of required instructions cycles for the below loop statement.
  The SDMMC_CMDTIMEOUT is expressed in ms */
  uint32_t count = SDMMC_CMDTIMEOUT * (SystemCoreClock / 8U / 1000U);

  do
  {
    if (count-- == 0U)
    {
      return SDMMC_ERROR_TIMEOUT;
    }
    sta_reg = SDMMCx->STA;
  } while (((sta_reg & (SDMMC_FLAG_CCRCFAIL | SDMMC_FLAG_CMDREND | SDMMC_FLAG_CTIMEOUT)) == 0U) ||
           ((sta_reg & SDMMC_FLAG_CMDACT) != 0U));

  if (__SDMMC_GET_FLAG(SDMMCx, SDMMC_FLAG_CTIMEOUT))
  {
    __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_FLAG_CTIMEOUT);

    return SDMMC_ERROR_CMD_RSP_TIMEOUT;
  }
  else if (__SDMMC_GET_FLAG(SDMMCx, SDMMC_FLAG_CCRCFAIL))
  {
    __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_FLAG_CCRCFAIL);

    return SDMMC_ERROR_CMD_CRC_FAIL;
  }
  else
  {
    /* No error flag set */
    /* Clear all the static flags */
    __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_STATIC_CMD_FLAGS);
  }

  return SDMMC_ERROR_NONE;
}

/**
  * @brief  Checks for error conditions for R3 (OCR) response.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
uint32_t SDMMC_GetCmdResp3(SDMMC_TypeDef *SDMMCx)
{
  uint32_t sta_reg;
  /* 8 is the number of required instructions cycles for the below loop statement.
  The SDMMC_CMDTIMEOUT is expressed in ms */
  uint32_t count = SDMMC_CMDTIMEOUT * (SystemCoreClock / 8U / 1000U);

  do
  {
    if (count-- == 0U)
    {
      return SDMMC_ERROR_TIMEOUT;
    }
    sta_reg = SDMMCx->STA;
  } while (((sta_reg & (SDMMC_FLAG_CCRCFAIL | SDMMC_FLAG_CMDREND | SDMMC_FLAG_CTIMEOUT)) == 0U) ||
           ((sta_reg & SDMMC_FLAG_CMDACT) != 0U));

  if (__SDMMC_GET_FLAG(SDMMCx, SDMMC_FLAG_CTIMEOUT))
  {
    __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_FLAG_CTIMEOUT);

    return SDMMC_ERROR_CMD_RSP_TIMEOUT;
  }
  else
  {
    /* Clear all the static flags */
    __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_STATIC_CMD_FLAGS);
  }

  return SDMMC_ERROR_NONE;
}

/**
  * @brief  Checks for error conditions for R4 response.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @param  pResp: pointer to response
  * @retval error state
  */
uint32_t SDMMC_GetCmdResp4(SDMMC_TypeDef *SDMMCx, uint32_t *pResp)
{
  uint32_t sta_reg;

  /* 8 is the number of required instructions cycles for the below loop statement.
  The SDMMC_CMDTIMEOUT is expressed in ms */
  uint32_t count = SDMMC_CMDTIMEOUT * (SystemCoreClock / 8U / 1000U);

  do
  {
    if (count-- == 0U)
    {
      return SDMMC_ERROR_TIMEOUT;
    }
    sta_reg = SDMMCx->STA;
  } while (((sta_reg & (SDMMC_FLAG_CCRCFAIL | SDMMC_FLAG_CMDREND | SDMMC_FLAG_CTIMEOUT)) == 0U) ||
           ((sta_reg & SDMMC_FLAG_CMDACT) != 0U));

  if (__SDMMC_GET_FLAG(SDMMCx, SDMMC_FLAG_CTIMEOUT))
  {
    __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_FLAG_CTIMEOUT);

    return SDMMC_ERROR_CMD_RSP_TIMEOUT;
  }
  else
  {
    /* Clear all the static flags */
    __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_STATIC_CMD_FLAGS);
  }

  /* Clear all the static flags */
  __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_STATIC_CMD_FLAGS);

  /* We have received response, retrieve it.  */
  *pResp = SDMMC_GetResponse(SDMMCx, SDMMC_RESP1);

  return SDMMC_ERROR_NONE;
}

/**
  * @brief  Checks for error conditions for R5 (cmd52/cmd53) response.
  * @param  SDMMCx: Pointer to SDMMC register base
  * @param  SDIO_CMD: The sent command index
  * @param  pData: pointer to the read/write buffer needed for cmd52
  * @retval SDIO Card error state
  */
uint32_t SDMMC_GetCmdResp5(SDMMC_TypeDef *SDMMCx, uint8_t SDIO_CMD, uint8_t *pData)
{
  uint32_t response_r5;
  uint32_t sta_reg;

  /* 8 is the number of required instructions cycles for the below loop statement.
  The SDMMC_CMDTIMEOUT is expressed in ms */
  uint32_t count = SDMMC_CMDTIMEOUT * (SystemCoreClock / 8U / 1000U);

  do
  {
    if (count-- == 0U)
    {
      return SDMMC_ERROR_TIMEOUT;
    }
    sta_reg = SDMMCx->STA;
  } while (((sta_reg & (SDMMC_FLAG_CCRCFAIL | SDMMC_FLAG_CMDREND | SDMMC_FLAG_CTIMEOUT)) == 0U) ||
           ((sta_reg & SDMMC_FLAG_CMDACT) != 0U));

  if (__SDMMC_GET_FLAG(SDMMCx, SDMMC_FLAG_CTIMEOUT))
  {
    __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_FLAG_CTIMEOUT);

    return SDMMC_ERROR_CMD_RSP_TIMEOUT;
  }
  else if (__SDMMC_GET_FLAG(SDMMCx, SDMMC_FLAG_CCRCFAIL))
  {
    __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_FLAG_CCRCFAIL);

    return SDMMC_ERROR_CMD_CRC_FAIL;
  }
  else
  {
    /* Nothing to do */
  }

  /* Check response received is of desired command */
  if (SDMMC_GetCommandResponse(SDMMCx) != SDIO_CMD)
  {
    return SDMMC_ERROR_CMD_CRC_FAIL;
  }

  /* Clear all the static flags */
  __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_STATIC_CMD_FLAGS);

  /* We have received response, retrieve it.  */
  response_r5 = SDMMC_GetResponse(SDMMCx, SDMMC_RESP1);

  if ((response_r5 & SDMMC_SDIO_R5_ERRORBITS) == SDMMC_ALLZERO)
  {
    /* we only want 8 bit read or write data and the 8 bit response flags are masked in the data pointer */
    if (pData != NULL)
    {
      *pData = (uint8_t)(response_r5 & 0xFFU);
    }

    return SDMMC_ERROR_NONE;
  }
  else if ((response_r5 & SDMMC_SDIO_R5_OUT_OF_RANGE) == SDMMC_SDIO_R5_OUT_OF_RANGE)
  {
    return SDMMC_ERROR_ADDR_OUT_OF_RANGE;
  }
  else if ((response_r5 & SDMMC_SDIO_R5_INVALID_FUNCTION_NUMBER) == SDMMC_SDIO_R5_INVALID_FUNCTION_NUMBER)
  {
    return SDMMC_ERROR_INVALID_PARAMETER;
  }
  else if ((response_r5 & SDMMC_SDIO_R5_ILLEGAL_CMD) == SDMMC_SDIO_R5_ILLEGAL_CMD)
  {
    return SDMMC_ERROR_ILLEGAL_CMD;
  }
  else if ((response_r5 & SDMMC_SDIO_R5_COM_CRC_FAILED) == SDMMC_SDIO_R5_COM_CRC_FAILED)
  {
    return SDMMC_ERROR_COM_CRC_FAILED;
  }
  else
  {
    return SDMMC_ERROR_GENERAL_UNKNOWN_ERR;
  }
}

/**
  * @brief  Checks for error conditions for R6 (RCA) response.
  * @param  hsd: SD handle
  * @param  SD_CMD: The sent command index
  * @param  pRCA: Pointer to the variable that will contain the SD card relative
  *         address RCA
  * @retval SD Card error state
  */
uint32_t SDMMC_GetCmdResp6(SDMMC_TypeDef *SDMMCx, uint8_t SD_CMD, uint16_t *pRCA)
{
  uint32_t response_r1;
  uint32_t sta_reg;

  /* 8 is the number of required instructions cycles for the below loop statement.
  The SDMMC_CMDTIMEOUT is expressed in ms */
  uint32_t count = SDMMC_CMDTIMEOUT * (SystemCoreClock / 8U / 1000U);

  do
  {
    if (count-- == 0U)
    {
      return SDMMC_ERROR_TIMEOUT;
    }
    sta_reg = SDMMCx->STA;
  } while (((sta_reg & (SDMMC_FLAG_CCRCFAIL | SDMMC_FLAG_CMDREND | SDMMC_FLAG_CTIMEOUT)) == 0U) ||
           ((sta_reg & SDMMC_FLAG_CMDACT) != 0U));

  if (__SDMMC_GET_FLAG(SDMMCx, SDMMC_FLAG_CTIMEOUT))
  {
    __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_FLAG_CTIMEOUT);

    return SDMMC_ERROR_CMD_RSP_TIMEOUT;
  }
  else if (__SDMMC_GET_FLAG(SDMMCx, SDMMC_FLAG_CCRCFAIL))
  {
    __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_FLAG_CCRCFAIL);

    return SDMMC_ERROR_CMD_CRC_FAIL;
  }
  else
  {
    /* Nothing to do */
  }

  /* Check response received is of desired command */
  if (SDMMC_GetCommandResponse(SDMMCx) != SD_CMD)
  {
    return SDMMC_ERROR_CMD_CRC_FAIL;
  }

  /* Clear all the static flags */
  __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_STATIC_CMD_FLAGS);

  /* We have received response, retrieve it.  */
  response_r1 = SDMMC_GetResponse(SDMMCx, SDMMC_RESP1);

  if ((response_r1 & (SDMMC_R6_GENERAL_UNKNOWN_ERROR | SDMMC_R6_ILLEGAL_CMD |
                      SDMMC_R6_COM_CRC_FAILED)) == SDMMC_ALLZERO)
  {
    *pRCA = (uint16_t)(response_r1 >> 16);

    return SDMMC_ERROR_NONE;
  }
  else if ((response_r1 & SDMMC_R6_ILLEGAL_CMD) == SDMMC_R6_ILLEGAL_CMD)
  {
    return SDMMC_ERROR_ILLEGAL_CMD;
  }
  else if ((response_r1 & SDMMC_R6_COM_CRC_FAILED) == SDMMC_R6_COM_CRC_FAILED)
  {
    return SDMMC_ERROR_COM_CRC_FAILED;
  }
  else
  {
    return SDMMC_ERROR_GENERAL_UNKNOWN_ERR;
  }
}

/**
  * @brief  Checks for error conditions for R7 response.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
uint32_t SDMMC_GetCmdResp7(SDMMC_TypeDef *SDMMCx)
{
  uint32_t sta_reg;
  /* 8 is the number of required instructions cycles for the below loop statement.
  The SDMMC_CMDTIMEOUT is expressed in ms */
  uint32_t count = SDMMC_CMDTIMEOUT * (SystemCoreClock / 8U / 1000U);

  do
  {
    if (count-- == 0U)
    {
      return SDMMC_ERROR_TIMEOUT;
    }
    sta_reg = SDMMCx->STA;
  } while (((sta_reg & (SDMMC_FLAG_CCRCFAIL | SDMMC_FLAG_CMDREND | SDMMC_FLAG_CTIMEOUT)) == 0U) ||
           ((sta_reg & SDMMC_FLAG_CMDACT) != 0U));

  if (__SDMMC_GET_FLAG(SDMMCx, SDMMC_FLAG_CTIMEOUT))
  {
    /* Card is not SD V2.0 compliant */
    __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_FLAG_CTIMEOUT);

    return SDMMC_ERROR_CMD_RSP_TIMEOUT;
  }

  else if (__SDMMC_GET_FLAG(SDMMCx, SDMMC_FLAG_CCRCFAIL))
  {
    /* Card is not SD V2.0 compliant */
    __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_FLAG_CCRCFAIL);

    return SDMMC_ERROR_CMD_CRC_FAIL;
  }
  else
  {
    /* Nothing to do */
  }

  if (__SDMMC_GET_FLAG(SDMMCx, SDMMC_FLAG_CMDREND))
  {
    /* Card is SD V2.0 compliant */
    __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_FLAG_CMDREND);
  }

  return SDMMC_ERROR_NONE;

}

/**
  * @}
  */


/** @defgroup HAL_SDMMC_LL_Group6 Linked List functions
  *  @brief   Linked List management functions
  *
@verbatim
 ===============================================================================
                   ##### Linked List management functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to manage the needed functions.

@endverbatim
  * @{
  */

/**
  * @brief  Build new Linked List node.
  * @param  pNode: Pointer to new node to add.
  * @param  pNodeConf: Pointer to configuration parameters for new node to add.
  * @retval Error status
  */
uint32_t SDMMC_DMALinkedList_BuildNode(SDMMC_DMALinkNodeTypeDef *pNode, const SDMMC_DMALinkNodeConfTypeDef *pNodeConf)
{

  if ((pNode == NULL) || (pNodeConf == NULL))
  {
    return SDMMC_ERROR_INVALID_PARAMETER;
  }
  /* Configure the Link Node registers*/
  pNode->IDMABASER = pNodeConf->BufferAddress;
  pNode->IDMABSIZE = pNodeConf->BufferSize;
  pNode->IDMALAR   = SDMMC_IDMALAR_ULS | SDMMC_IDMALAR_ABR;

  return SDMMC_ERROR_NONE;
}

/**
  * @brief  Insert new Linked List node.
  * @param  pLinkedList: Pointer to the linkedlist that contains transfer nodes
  * @param  pPrevNode: Pointer to previous node .
  * @param  pNewNode: Pointer to new node to add.
  * @retval Error status
  */
uint32_t SDMMC_DMALinkedList_InsertNode(SDMMC_DMALinkedListTypeDef *pLinkedList, SDMMC_DMALinkNodeTypeDef *pPrevNode,
                                        SDMMC_DMALinkNodeTypeDef *pNode)
{
  uint32_t link_list_offset;
  uint32_t node_address = (uint32_t) pNode;

  /* First Node */
  if (pLinkedList->NodesCounter == 0U)
  {

    pLinkedList->pHeadNode = pNode;
    pLinkedList->pTailNode = pNode;
    pLinkedList->NodesCounter = 1U;

  }
  else if (pPrevNode == pLinkedList->pTailNode)
  {
    if (pNode <= pLinkedList->pHeadNode)
    {
      /* Node Address should greater than Head Node Address*/
      return SDMMC_ERROR_INVALID_PARAMETER;
    }

    /*Last Node, no next node */
    MODIFY_REG(pPrevNode->IDMALAR, SDMMC_IDMALAR_ULA, 0U);

    /*link Prev node with new one */
    MODIFY_REG(pPrevNode->IDMALAR, SDMMC_IDMALAR_ULA, SDMMC_IDMALAR_ULA);
    MODIFY_REG(pPrevNode->IDMALAR, SDMMC_IDMALAR_IDMALA, (node_address - (uint32_t)pLinkedList->pHeadNode));

    pLinkedList->NodesCounter ++;
    pLinkedList->pTailNode = pNode;

  }
  else
  {

    if (pNode <= pLinkedList->pHeadNode)
    {
      /* Node Address should greater than Head Node Address*/
      return SDMMC_ERROR_INVALID_PARAMETER;
    }

    /*link New node with Next one */
    link_list_offset = pNode->IDMALAR;
    MODIFY_REG(pPrevNode->IDMALAR, SDMMC_IDMALAR_IDMALA, link_list_offset);

    /*link Prev node with new one */
    MODIFY_REG(pPrevNode->IDMALAR, SDMMC_IDMALAR_ULA, SDMMC_IDMALAR_ULA);
    MODIFY_REG(pPrevNode->IDMALAR, SDMMC_IDMALAR_IDMALA, (node_address - (uint32_t)pLinkedList->pHeadNode));

    pLinkedList->NodesCounter ++;

  }
  return SDMMC_ERROR_NONE;
}

/**
  * @brief  Remove  node from the Linked List.
  * @param  pLinkedList: Pointer to the linkedlist that contains transfer nodes
  * @param  pNode: Pointer to new node to add.
  * @retval Error status
   */
uint32_t SDMMC_DMALinkedList_RemoveNode(SDMMC_DMALinkedListTypeDef *pLinkedList, SDMMC_DMALinkNodeTypeDef *pNode)
{
  uint32_t count = 0U;
  uint32_t linked_list_offset;
  SDMMC_DMALinkNodeTypeDef *prev_node = NULL;
  SDMMC_DMALinkNodeTypeDef *curr_node ;

  /* First Node */
  if (pLinkedList->NodesCounter == 0U)
  {

    return SDMMC_ERROR_INVALID_PARAMETER;
  }
  else
  {
    curr_node = pLinkedList->pHeadNode;
    while ((curr_node != pNode) && (count <= pLinkedList->NodesCounter))
    {
      prev_node = curr_node;
      curr_node = (SDMMC_DMALinkNodeTypeDef *)((prev_node->IDMALAR & SDMMC_IDMALAR_IDMALA) +
                                               (uint32_t)pLinkedList->pHeadNode);
      count++;
    }

    if ((count == 0U) || (count > pLinkedList->NodesCounter))
    {
      /* Node not found in the linked list */
      return SDMMC_ERROR_INVALID_PARAMETER;
    }

    pLinkedList->NodesCounter--;

    if (pLinkedList->NodesCounter == 0U)
    {
      pLinkedList->pHeadNode = 0U;
      pLinkedList->pTailNode = 0U;
    }
    else
    {
      /*link prev node with next one */
      linked_list_offset = curr_node->IDMALAR;
      MODIFY_REG(prev_node->IDMALAR, SDMMC_IDMALAR_IDMALA, linked_list_offset);
      /* Configure the new Link Node registers*/
      pNode->IDMALAR    |= linked_list_offset;

      pLinkedList->pTailNode = prev_node;
    }
  }
  return SDMMC_ERROR_NONE;
}

/**
  * @brief  Lock Linked List Node
  * @param  pNode: Pointer to node to lock.
  * @retval Error status

  */
uint32_t SDMMC_DMALinkedList_LockNode(SDMMC_DMALinkNodeTypeDef *pNode)
{

  if (pNode == NULL)
  {
    return SDMMC_ERROR_INVALID_PARAMETER;
  }

  MODIFY_REG(pNode->IDMALAR, SDMMC_IDMALAR_ABR, 0U);

  return SDMMC_ERROR_NONE;
}

/**
  * @brief  Unlock Linked List Node
  * @param  pNode: Pointer to node to unlock.
  * @retval Error status

  */
uint32_t SDMMC_DMALinkedList_UnlockNode(SDMMC_DMALinkNodeTypeDef *pNode)
{

  if (pNode == NULL)
  {
    return SDMMC_ERROR_INVALID_PARAMETER;
  }

  MODIFY_REG(pNode->IDMALAR, SDMMC_IDMALAR_ABR, SDMMC_IDMALAR_ABR);

  return SDMMC_ERROR_NONE;
}

/**
  * @brief  Enable Linked List circular mode
  * @param  pLinkedList: Pointer to the linkedlist that contains transfer nodes
  * @retval Error status

  */
uint32_t SDMMC_DMALinkedList_EnableCircularMode(SDMMC_DMALinkedListTypeDef *pLinkedList)
{

  if (pLinkedList == NULL)
  {
    return SDMMC_ERROR_INVALID_PARAMETER;
  }

  MODIFY_REG(pLinkedList->pTailNode->IDMALAR, SDMMC_IDMALAR_ULA | SDMMC_IDMALAR_IDMALA, SDMMC_IDMALAR_ULA);

  return SDMMC_ERROR_NONE;
}

/**
  * @brief  Disable DMA Linked List Circular mode
  * @param  pLinkedList: Pointer to the linkedlist that contains transfer nodes
  * @retval Error status
   */
uint32_t SDMMC_DMALinkedList_DisableCircularMode(SDMMC_DMALinkedListTypeDef *pLinkedList)
{

  if (pLinkedList == NULL)
  {
    return SDMMC_ERROR_INVALID_PARAMETER;
  }

  MODIFY_REG(pLinkedList->pTailNode->IDMALAR, SDMMC_IDMALAR_ULA, 0U);

  return SDMMC_ERROR_NONE;
}

/**
  * @}
  */


/* Private function ----------------------------------------------------------*/
/** @addtogroup SD_Private_Functions
  * @{
  */

/**
  * @brief  Checks for error conditions for CMD0.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
static uint32_t SDMMC_GetCmdError(SDMMC_TypeDef *SDMMCx)
{
  /* 8 is the number of required instructions cycles for the below loop statement.
  The SDMMC_CMDTIMEOUT is expressed in ms */
  uint32_t count = SDMMC_CMDTIMEOUT * (SystemCoreClock / 8U / 1000U);

  do
  {
    if (count-- == 0U)
    {
      return SDMMC_ERROR_TIMEOUT;
    }

  } while (!__SDMMC_GET_FLAG(SDMMCx, SDMMC_FLAG_CMDSENT));

  /* Clear all the static flags */
  __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_STATIC_CMD_FLAGS);

  return SDMMC_ERROR_NONE;
}

/**
  * @}
  */

#endif /* HAL_SD_MODULE_ENABLED || HAL_MMC_MODULE_ENABLED */
#endif /* SDMMC1 || SDMMC2 */
/**
  * @}
  */

/**
  * @}
  */

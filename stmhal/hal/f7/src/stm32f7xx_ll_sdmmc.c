/**
  ******************************************************************************
  * @file    stm32f7xx_ll_sdmmc.c
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    23-September-2016
  * @brief   SDMMC Low Layer HAL module driver.
  *
  *          This file provides firmware functions to manage the following
  *          functionalities of the SDMMC peripheral:
  *           + Initialization/de-initialization functions
  *           + I/O operation functions
  *           + Peripheral Control functions
  *           + Peripheral State functions
  *
  @verbatim
  ==============================================================================
                       ##### SDMMC peripheral features #####
  ==============================================================================
    [..] The SD/SDMMC MMC card host interface (SDMMC) provides an interface between the APB2
         peripheral bus and MultiMedia cards (MMCs), SD memory cards, SDMMC cards and CE-ATA
         devices.

    [..] The SDMMC features include the following:
         (+) Full compliance with MultiMedia Card System Specification Version 4.2. Card support
             for three different databus modes: 1-bit (default), 4-bit and 8-bit
         (+) Full compatibility with previous versions of MultiMedia Cards (forward compatibility)
         (+) Full compliance with SD Memory Card Specifications Version 2.0
         (+) Full compliance with SD I/O Card Specification Version 2.0: card support for two
             different data bus modes: 1-bit (default) and 4-bit
         (+) Full support of the CE-ATA features (full compliance with CE-ATA digital protocol
             Rev1.1)
         (+) Data transfer up to 48 MHz for the 8 bit mode
         (+) Data and command output enable signals to control external bidirectional drivers.


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
      (+) The SDMMC clock (SDMMCCLK = 48 MHz) is coming from a specific output of PLL
          (PLL48CLK). Before start working with SDMMC peripheral make sure that the
          PLL is well configured.
          The SDMMC peripheral uses two clock signals:
          (++) SDMMC adapter clock (SDMMCCLK = 48 MHz)
          (++) APB2 bus clock (PCLK2)

          -@@- PCLK2 and SDMMC_CK clock frequencies must respect the following condition:
               Frequency(PCLK2) >= (3 / 8 x Frequency(SDMMC_CK))

      (+) Enable/Disable peripheral clock using RCC peripheral macros related to SDMMC
          peripheral.

      (+) Enable the Power ON State using the SDMMC_PowerState_ON(SDMMCx)
          function and disable it using the function SDMMC_PowerState_OFF(SDMMCx).

      (+) Enable/Disable the clock using the __SDMMC_ENABLE()/__SDMMC_DISABLE() macros.

      (+) Enable/Disable the peripheral interrupts using the macros __SDMMC_ENABLE_IT(hSDMMC, IT)
          and __SDMMC_DISABLE_IT(hSDMMC, IT) if you need to use interrupt mode.

      (+) When using the DMA mode
          (++) Configure the DMA in the MSP layer of the external device
          (++) Active the needed channel Request
          (++) Enable the DMA using __SDMMC_DMA_ENABLE() macro or Disable it using the macro
               __SDMMC_DMA_DISABLE().

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
          SDMMC_ReadFIFO(), DIO_WriteFIFO() and SDMMC_GetFIFOCount() functions.

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

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/** @addtogroup STM32F7xx_HAL_Driver
  * @{
  */

/** @defgroup SDMMC_LL SDMMC Low Layer
  * @brief Low layer module for SD
  * @{
  */

#if defined (HAL_SD_MODULE_ENABLED) || defined(HAL_MMC_MODULE_ENABLED)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
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
  assert_param(IS_SDMMC_CLOCK_BYPASS(Init.ClockBypass));
  assert_param(IS_SDMMC_CLOCK_POWER_SAVE(Init.ClockPowerSave));
  assert_param(IS_SDMMC_BUS_WIDE(Init.BusWide));
  assert_param(IS_SDMMC_HARDWARE_FLOW_CONTROL(Init.HardwareFlowControl));
  assert_param(IS_SDMMC_CLKDIV(Init.ClockDiv));

  /* Set SDMMC configuration parameters */
  tmpreg |= (Init.ClockEdge           |\
             Init.ClockBypass         |\
             Init.ClockPowerSave      |\
             Init.BusWide             |\
             Init.HardwareFlowControl |\
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
uint32_t SDMMC_ReadFIFO(SDMMC_TypeDef *SDMMCx)
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
  SDMMCx->POWER = SDMMC_POWER_PWRCTRL;

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
  SDMMCx->POWER = (uint32_t)0x00000000;

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
uint32_t SDMMC_GetPowerState(SDMMC_TypeDef *SDMMCx)
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
HAL_StatusTypeDef SDMMC_SendCommand(SDMMC_TypeDef *SDMMCx, SDMMC_CmdInitTypeDef *Command)
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
  tmpreg |= (uint32_t)(Command->CmdIndex         |\
                       Command->Response         |\
                       Command->WaitForInterrupt |\
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
uint8_t SDMMC_GetCommandResponse(SDMMC_TypeDef *SDMMCx)
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
uint32_t SDMMC_GetResponse(SDMMC_TypeDef *SDMMCx, uint32_t Response)
{
  __IO uint32_t tmp = 0;

  /* Check the parameters */
  assert_param(IS_SDMMC_RESP(Response));

  /* Get the response */
  tmp = (uint32_t)&(SDMMCx->RESP1) + Response;

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
HAL_StatusTypeDef SDMMC_DataConfig(SDMMC_TypeDef *SDMMCx, SDMMC_DataInitTypeDef* Data)
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
  tmpreg |= (uint32_t)(Data->DataBlockSize |\
                       Data->TransferDir   |\
                       Data->TransferMode  |\
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
uint32_t SDMMC_GetDataCounter(SDMMC_TypeDef *SDMMCx)
{
  return (SDMMCx->DCOUNT);
}

/**
  * @brief  Get the FIFO data
  * @param  SDMMCx: Pointer to SDMMC register base
  * @retval Data received
  */
uint32_t SDMMC_GetFIFOCount(SDMMC_TypeDef *SDMMCx)
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

/**
  * @}
  */

#endif /* (HAL_SD_MODULE_ENABLED) || (HAL_MMC_MODULE_ENABLED) */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/**
  ******************************************************************************
  * @file    stm32f4xx_ll_sdmmc.c
  * @author  MCD Application Team
  * @version V1.5.1
  * @date    01-July-2016
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
    [..] The SD/SDIO MMC card host interface (SDIO) provides an interface between the APB2
         peripheral bus and MultiMedia cards (MMCs), SD memory cards, SDIO cards and CE-ATA
         devices.

    [..] The SDIO features include the following:
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
      that interfaces with the SDIO peripheral.
      According to the device used (SD card/ MMC card / SDIO card ...), a set of APIs 
      is used in the device's driver to perform SDIO operations and functionalities.
   
      This driver is almost transparent for the final user, it is only used to implement other
      functionalities of the external device.
   
    [..]
      (+) The SDIO clock (SDIOCLK = 48 MHz) is coming from a specific output of PLL 
          (PLL48CLK). Before start working with SDIO peripheral make sure that the
          PLL is well configured.
          The SDIO peripheral uses two clock signals:
          (++) SDIO adapter clock (SDIOCLK = 48 MHz)
          (++) APB2 bus clock (PCLK2)
       
          -@@- PCLK2 and SDIO_CK clock frequencies must respect the following condition:
               Frequency(PCLK2) >= (3 / 8 x Frequency(SDIO_CK))
  
      (+) Enable/Disable peripheral clock using RCC peripheral macros related to SDIO
          peripheral.

      (+) Enable the Power ON State using the SDIO_PowerState_ON(SDIOx) 
          function and disable it using the function SDIO_PowerState_OFF(SDIOx).
                
      (+) Enable/Disable the clock using the __SDIO_ENABLE()/__SDIO_DISABLE() macros.
  
      (+) Enable/Disable the peripheral interrupts using the macros __SDIO_ENABLE_IT(hsdio, IT) 
          and __SDIO_DISABLE_IT(hsdio, IT) if you need to use interrupt mode. 
  
      (+) When using the DMA mode 
          (++) Configure the DMA in the MSP layer of the external device
          (++) Active the needed channel Request 
          (++) Enable the DMA using __SDIO_DMA_ENABLE() macro or Disable it using the macro
               __SDIO_DMA_DISABLE().
  
      (+) To control the CPSM (Command Path State Machine) and send 
          commands to the card use the SDIO_SendCommand(SDIOx), 
          SDIO_GetCommandResponse() and SDIO_GetResponse() functions. First, user has
          to fill the command structure (pointer to SDIO_CmdInitTypeDef) according 
          to the selected command to be sent.
          The parameters that should be filled are:
           (++) Command Argument
           (++) Command Index
           (++) Command Response type
           (++) Command Wait
           (++) CPSM Status (Enable or Disable).
  
          -@@- To check if the command is well received, read the SDIO_CMDRESP
              register using the SDIO_GetCommandResponse().
              The SDIO responses registers (SDIO_RESP1 to SDIO_RESP2), use the
              SDIO_GetResponse() function.
  
      (+) To control the DPSM (Data Path State Machine) and send/receive 
           data to/from the card use the SDIO_DataConfig(), SDIO_GetDataCounter(), 
          SDIO_ReadFIFO(), DIO_WriteFIFO() and SDIO_GetFIFOCount() functions.
  
    *** Read Operations ***
    =======================
    [..]
      (#) First, user has to fill the data structure (pointer to
          SDIO_DataInitTypeDef) according to the selected data type to be received.
          The parameters that should be filled are:
           (++) Data Timeout
           (++) Data Length
           (++) Data Block size
           (++) Data Transfer direction: should be from card (To SDIO)
           (++) Data Transfer mode
           (++) DPSM Status (Enable or Disable)
                                     
      (#) Configure the SDIO resources to receive the data from the card
          according to selected transfer mode (Refer to Step 8, 9 and 10).
  
      (#) Send the selected Read command (refer to step 11).
                    
      (#) Use the SDIO flags/interrupts to check the transfer status.
  
    *** Write Operations ***
    ========================
    [..]
     (#) First, user has to fill the data structure (pointer to
         SDIO_DataInitTypeDef) according to the selected data type to be received.
         The parameters that should be filled are:
          (++) Data Timeout
          (++) Data Length
          (++) Data Block size
          (++) Data Transfer direction:  should be to card (To CARD)
          (++) Data Transfer mode
          (++) DPSM Status (Enable or Disable)
  
     (#) Configure the SDIO resources to send the data to the card according to 
         selected transfer mode.
                     
     (#) Send the selected Write command.
                    
     (#) Use the SDIO flags/interrupts to check the transfer status.
  
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
#include "stm32f4xx_hal.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @defgroup SDMMC_LL SDMMC Low Layer
  * @brief Low layer module for SD and MMC driver
  * @{
  */

#if defined(HAL_SD_MODULE_ENABLED) || defined(HAL_MMC_MODULE_ENABLED)
#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || \
    defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) || \
    defined(STM32F401xC) || defined(STM32F401xE) || defined(STM32F411xE) || defined(STM32F446xx) || \
    defined(STM32F469xx) || defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || \
    defined(STM32F412Rx) || defined(STM32F412Cx)
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup SDMMC_LL_Exported_Functions SDMMC_LL Exported Functions
  * @{
  */

/** @defgroup HAL_SDMMC_LL_Group1 Initialization/de-initialization functions 
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
  * @brief  Initializes the SDIO according to the specified
  *         parameters in the SDIO_InitTypeDef and create the associated handle.
  * @param  SDIOx: Pointer to SDIO register base
  * @param  Init: SDIO initialization structure   
  * @retval HAL status
  */
HAL_StatusTypeDef SDIO_Init(SDIO_TypeDef *SDIOx, SDIO_InitTypeDef Init)
{
  uint32_t tmpreg = 0U; 

  /* Check the parameters */
  assert_param(IS_SDIO_ALL_INSTANCE(SDIOx));
  assert_param(IS_SDIO_CLOCK_EDGE(Init.ClockEdge)); 
  assert_param(IS_SDIO_CLOCK_BYPASS(Init.ClockBypass));
  assert_param(IS_SDIO_CLOCK_POWER_SAVE(Init.ClockPowerSave));
  assert_param(IS_SDIO_BUS_WIDE(Init.BusWide));
  assert_param(IS_SDIO_HARDWARE_FLOW_CONTROL(Init.HardwareFlowControl));
  assert_param(IS_SDIO_CLKDIV(Init.ClockDiv));
  
  /* Set SDIO configuration parameters */
  tmpreg |= (Init.ClockEdge           |\
             Init.ClockBypass         |\
             Init.ClockPowerSave      |\
             Init.BusWide             |\
             Init.HardwareFlowControl |\
             Init.ClockDiv
             ); 
  
  /* Write to SDIO CLKCR */
  MODIFY_REG(SDIOx->CLKCR, CLKCR_CLEAR_MASK, tmpreg);  

  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup HAL_SDMMC_LL_Group2 I/O operation functions 
 *  @brief   Data transfers functions 
 *
@verbatim   
 ===============================================================================
                      ##### I/O operation functions #####
 ===============================================================================  
    [..]
    This subsection provides a set of functions allowing to manage the SDIO data 
    transfers.

@endverbatim
  * @{
  */

/**
  * @brief  Read data (word) from Rx FIFO in blocking mode (polling) 
  * @param  SDIOx: Pointer to SDIO register base
  * @retval HAL status
  */
uint32_t SDIO_ReadFIFO(SDIO_TypeDef *SDIOx)
{
  /* Read data from Rx FIFO */ 
  return (SDIOx->FIFO);
}

/**
  * @brief  Write data (word) to Tx FIFO in blocking mode (polling) 
  * @param  SDIOx: Pointer to SDIO register base
  * @param  pWriteData: pointer to data to write
  * @retval HAL status
  */
HAL_StatusTypeDef SDIO_WriteFIFO(SDIO_TypeDef *SDIOx, uint32_t *pWriteData)
{ 
  /* Write data to FIFO */ 
  SDIOx->FIFO = *pWriteData;

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
    This subsection provides a set of functions allowing to control the SDIO data 
    transfers.

@endverbatim
  * @{
  */

/**
  * @brief  Set SDIO Power state to ON. 
  * @param  SDIOx: Pointer to SDIO register base
  * @retval HAL status
  */
HAL_StatusTypeDef SDIO_PowerState_ON(SDIO_TypeDef *SDIOx)
{  
  /* Set power state to ON */ 
  SDIOx->POWER = SDIO_POWER_PWRCTRL;
  
  return HAL_OK; 
}

/**
  * @brief  Set SDIO Power state to OFF. 
  * @param  SDIOx: Pointer to SDIO register base
  * @retval HAL status
  */
HAL_StatusTypeDef SDIO_PowerState_OFF(SDIO_TypeDef *SDIOx)
{
  /* Set power state to OFF */
  SDIOx->POWER = (uint32_t)0x00000000U;
  
  return HAL_OK;
}

/**
  * @brief  Get SDIO Power state. 
  * @param  SDIOx: Pointer to SDIO register base
  * @retval Power status of the controller. The returned value can be one of the 
  *         following values:
  *            - 0x00: Power OFF
  *            - 0x02: Power UP
  *            - 0x03: Power ON 
  */
uint32_t SDIO_GetPowerState(SDIO_TypeDef *SDIOx)  
{
  return (SDIOx->POWER & SDIO_POWER_PWRCTRL);
}

/**
  * @brief  Configure the SDIO command path according to the specified parameters in
  *         SDIO_CmdInitTypeDef structure and send the command 
  * @param  SDIOx: Pointer to SDIO register base
  * @param  SDIO_CmdInitStruct: pointer to a SDIO_CmdInitTypeDef structure that contains 
  *         the configuration information for the SDIO command
  * @retval HAL status
  */
HAL_StatusTypeDef SDIO_SendCommand(SDIO_TypeDef *SDIOx, SDIO_CmdInitTypeDef *SDIO_CmdInitStruct)
{
  uint32_t tmpreg = 0U;
  
  /* Check the parameters */
  assert_param(IS_SDIO_CMD_INDEX(SDIO_CmdInitStruct->CmdIndex));
  assert_param(IS_SDIO_RESPONSE(SDIO_CmdInitStruct->Response));
  assert_param(IS_SDIO_WAIT(SDIO_CmdInitStruct->WaitForInterrupt));
  assert_param(IS_SDIO_CPSM(SDIO_CmdInitStruct->CPSM));

  /* Set the SDIO Argument value */
  SDIOx->ARG = SDIO_CmdInitStruct->Argument;

  /* Set SDIO command parameters */
  tmpreg |= (uint32_t)(SDIO_CmdInitStruct->CmdIndex         |\
                       SDIO_CmdInitStruct->Response         |\
                       SDIO_CmdInitStruct->WaitForInterrupt |\
                       SDIO_CmdInitStruct->CPSM);
  
  /* Write to SDIO CMD register */
  MODIFY_REG(SDIOx->CMD, CMD_CLEAR_MASK, tmpreg); 
  
  return HAL_OK;  
}

/**
  * @brief  Return the command index of last command for which response received
  * @param  SDIOx: Pointer to SDIO register base
  * @retval Command index of the last command response received
  */
uint8_t SDIO_GetCommandResponse(SDIO_TypeDef *SDIOx)
{
  return (uint8_t)(SDIOx->RESPCMD);
}


/**
  * @brief  Return the response received from the card for the last command
  * @param  SDIO_RESP: Specifies the SDIO response register. 
  *          This parameter can be one of the following values:
  *            @arg SDIO_RESP1: Response Register 1
  *            @arg SDIO_RESP2: Response Register 2
  *            @arg SDIO_RESP3: Response Register 3
  *            @arg SDIO_RESP4: Response Register 4  
  * @retval The Corresponding response register value
  */
uint32_t SDIO_GetResponse(uint32_t SDIO_RESP)
{
  __IO uint32_t tmp = 0U;

  /* Check the parameters */
  assert_param(IS_SDIO_RESP(SDIO_RESP));

  /* Get the response */
  tmp = SDIO_RESP_ADDR + SDIO_RESP;
  
  return (*(__IO uint32_t *) tmp);
}  

/**
  * @brief  Configure the SDIO data path according to the specified 
  *         parameters in the SDIO_DataInitTypeDef.
  * @param  SDIOx: Pointer to SDIO register base  
  * @param  SDIO_DataInitStruct : pointer to a SDIO_DataInitTypeDef structure 
  *         that contains the configuration information for the SDIO command.
  * @retval HAL status
  */
HAL_StatusTypeDef SDIO_DataConfig(SDIO_TypeDef *SDIOx, SDIO_DataInitTypeDef* SDIO_DataInitStruct)
{
  uint32_t tmpreg = 0U;
  
  /* Check the parameters */
  assert_param(IS_SDIO_DATA_LENGTH(SDIO_DataInitStruct->DataLength));
  assert_param(IS_SDIO_BLOCK_SIZE(SDIO_DataInitStruct->DataBlockSize));
  assert_param(IS_SDIO_TRANSFER_DIR(SDIO_DataInitStruct->TransferDir));
  assert_param(IS_SDIO_TRANSFER_MODE(SDIO_DataInitStruct->TransferMode));
  assert_param(IS_SDIO_DPSM(SDIO_DataInitStruct->DPSM));

  /* Set the SDIO Data Timeout value */
  SDIOx->DTIMER = SDIO_DataInitStruct->DataTimeOut;

  /* Set the SDIO DataLength value */
  SDIOx->DLEN = SDIO_DataInitStruct->DataLength;

  /* Set the SDIO data configuration parameters */
  tmpreg |= (uint32_t)(SDIO_DataInitStruct->DataBlockSize |\
                       SDIO_DataInitStruct->TransferDir   |\
                       SDIO_DataInitStruct->TransferMode  |\
                       SDIO_DataInitStruct->DPSM);
  
  /* Write to SDIO DCTRL */
  MODIFY_REG(SDIOx->DCTRL, DCTRL_CLEAR_MASK, tmpreg);

  return HAL_OK;

}

/**
  * @brief  Returns number of remaining data bytes to be transferred.
  * @param  SDIOx: Pointer to SDIO register base
  * @retval Number of remaining data bytes to be transferred
  */
uint32_t SDIO_GetDataCounter(SDIO_TypeDef *SDIOx)
{
  return (SDIOx->DCOUNT);
}

/**
  * @brief  Get the FIFO data
  * @param  SDIOx: Pointer to SDIO register base 
  * @retval Data received
  */
uint32_t SDIO_GetFIFOCount(SDIO_TypeDef *SDIOx)
{
  return (SDIOx->FIFO);
}


/**
  * @brief  Sets one of the two options of inserting read wait interval.
  * @param  SDIO_ReadWaitMode: SD I/O Read Wait operation mode.
  *          This parameter can be:
  *            @arg SDIO_READ_WAIT_MODE_CLK: Read Wait control by stopping SDIOCLK
  *            @arg SDIO_READ_WAIT_MODE_DATA2: Read Wait control using SDIO_DATA2
  * @retval None
  */
HAL_StatusTypeDef SDIO_SetSDIOReadWaitMode(uint32_t SDIO_ReadWaitMode)
{
  /* Check the parameters */
  assert_param(IS_SDIO_READWAIT_MODE(SDIO_ReadWaitMode));
  
  *(__IO uint32_t *)DCTRL_RWMOD_BB = SDIO_ReadWaitMode;
  
  return HAL_OK;  
}

/**
  * @}
  */

/**
  * @}
  */
#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx ||
          STM32F401xC || STM32F401xE || STM32F411xE || STM32F446xx || STM32F469xx || STM32F479xx || STM32F412Zx || STM32F412Vx ||
          STM32F412Rx || STM32F412Cx */
#endif /* (HAL_SD_MODULE_ENABLED) || (HAL_MMC_MODULE_ENABLED) */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

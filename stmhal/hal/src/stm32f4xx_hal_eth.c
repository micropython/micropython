/**
  ******************************************************************************
  * @file    stm32f4xx_hal_eth.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   ETH HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Ethernet (ETH) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions 
  *           + Peripheral State and Errors functions
  *
  @verbatim
  ==============================================================================
                    ##### How to use this driver #####
  ==============================================================================
    [..]
      (#)Declare a ETH_HandleTypeDef handle structure, for example:
         ETH_HandleTypeDef  heth;
        
      (#)Fill parameters of Init structure in heth handle
  
      (#)Call HAL_ETH_Init() API to initialize the Ethernet peripheral (MAC, DMA, ...) 

      (#)Initialize the ETH low level resources through the HAL_ETH_MspInit() API:
          (##) Enable the Ethernet interface clock using 
               (+++) __ETHMAC_CLK_ENABLE();
               (+++) __ETHMACTX_CLK_ENABLE();
               (+++) __ETHMACRX_CLK_ENABLE();
           
          (##) Initialize the related GPIO clocks
          (##) Configure Ethernet pin-out
          (##) Configure Ethernet NVIC interrupt (IT mode)   
    
      (#)Initialize Ethernet DMA Descriptors in chain mode and point to allocated buffers:
          (##) HAL_ETH_DMATxDescListInit(); for Transmission process
          (##) HAL_ETH_DMARxDescListInit(); for Reception process

      (#)Enable MAC and DMA transmission and reception:
          (##) HAL_ETH_Start();

      (#)Prepare ETH DMA TX Descriptors and give the hand to ETH DMA to transfer 
         the frame to MAC TX FIFO:
         (##) HAL_ETH_TransmitFrame();

      (#)Poll for a received frame in ETH RX DMA Descriptors and get received 
         frame parameters
         (##) HAL_ETH_GetReceivedFrame(); (should be called into an infinite loop)

      (#) Get a received frame when an ETH RX interrupt occurs:
         (##) HAL_ETH_GetReceivedFrame_IT(); (called in IT mode only)

      (#) Communicate with external PHY device:
         (##) Read a specific register from the PHY  
              HAL_ETH_ReadPHYRegister();
         (##) Write data to a specific RHY register:
              HAL_ETH_WritePHYRegister();

      (#) Configure the Ethernet MAC after ETH peripheral initialization
          HAL_ETH_ConfigMAC(); all MAC parameters should be filled.
      
      (#) Configure the Ethernet DMA after ETH peripheral initialization
          HAL_ETH_ConfigDMA(); all DMA parameters should be filled.

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
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

/** @defgroup ETH 
  * @brief ETH HAL module driver
  * @{
  */

#ifdef HAL_ETH_MODULE_ENABLED

#if defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void ETH_MACDMAConfig(ETH_HandleTypeDef *heth, uint32_t err);
static void ETH_MACAddressConfig(ETH_HandleTypeDef *heth, uint32_t MacAddr, uint8_t *Addr);
static void ETH_MACReceptionEnable(ETH_HandleTypeDef *heth);
static void ETH_MACReceptionDisable(ETH_HandleTypeDef *heth);
static void ETH_MACTransmissionEnable(ETH_HandleTypeDef *heth);
static void ETH_MACTransmissionDisable(ETH_HandleTypeDef *heth);
static void ETH_DMATransmissionEnable(ETH_HandleTypeDef *heth);
static void ETH_DMATransmissionDisable(ETH_HandleTypeDef *heth);
static void ETH_DMAReceptionEnable(ETH_HandleTypeDef *heth);
static void ETH_DMAReceptionDisable(ETH_HandleTypeDef *heth);
static void ETH_FlushTransmitFIFO(ETH_HandleTypeDef *heth);

/* Private functions ---------------------------------------------------------*/

/** @defgroup ETH_Private_Functions
  * @{
  */

/** @defgroup ETH_Group1 Initialization and de-initialization functions 
  *  @brief    Initialization and Configuration functions 
  *
  @verbatim    
  ===============================================================================
            ##### Initialization and de-initialization functions #####
  ===============================================================================
  [..]  This section provides functions allowing to:
      (+) Initialize and configure the Ethernet peripheral
      (+) De-initialize the Ethernet peripheral

  @endverbatim
  * @{
  */

/**
  * @brief  Initializes the Ethernet MAC and DMA according to default
  *         parameters.
  * @param  heth: ETH handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_Init(ETH_HandleTypeDef *heth)
{
  uint32_t tmpreg = 0, phyreg = 0;
  uint32_t hclk = 60000000;
  uint32_t timeout = 0;
  uint32_t err = ETH_SUCCESS;
  
  /* Check the ETH peripheral state */
  if(heth == NULL)
  {
    return HAL_ERROR;
  }
  
  /* Check parameters */
  assert_param(IS_ETH_AUTONEGOTIATION(heth->Init.AutoNegotiation));
  assert_param(IS_ETH_RX_MODE(heth->Init.RxMode));
  assert_param(IS_ETH_CHECKSUM_MODE(heth->Init.ChecksumMode));
  assert_param(IS_ETH_MEDIA_INTERFACE(heth->Init.MediaInterface));  
  
  if(heth->State == HAL_ETH_STATE_RESET)
  {
    /* Init the low level hardware : GPIO, CLOCK, NVIC. */
    HAL_ETH_MspInit(heth);
  }
  
  /* Enable SYSCFG Clock */
  __SYSCFG_CLK_ENABLE();
  
  /* Select MII or RMII Mode*/
  SYSCFG->PMC &= ~(SYSCFG_PMC_MII_RMII_SEL);
  SYSCFG->PMC |= (uint32_t)heth->Init.MediaInterface;
  
  /* Ethernet Software reset */
  /* Set the SWR bit: resets all MAC subsystem internal registers and logic */
  /* After reset all the registers holds their respective reset values */
  (heth->Instance)->DMABMR |= ETH_DMABMR_SR;
  
  /* Wait for software reset */
  while (((heth->Instance)->DMABMR & ETH_DMABMR_SR) != (uint32_t)RESET)
  {
  }
  
  /*-------------------------------- MAC Initialization ----------------------*/
  /* Get the ETHERNET MACMIIAR value */
  tmpreg = (heth->Instance)->MACMIIAR;
  /* Clear CSR Clock Range CR[2:0] bits */
  tmpreg &= MACMIIAR_CR_MASK;
  
  /* Get hclk frequency value */
  hclk = HAL_RCC_GetHCLKFreq();
  
  /* Set CR bits depending on hclk value */
  if((hclk >= 20000000)&&(hclk < 35000000))
  {
    /* CSR Clock Range between 20-35 MHz */
    tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div16;
  }
  else if((hclk >= 35000000)&&(hclk < 60000000))
  {
    /* CSR Clock Range between 35-60 MHz */ 
    tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div26;
  }  
  else if((hclk >= 60000000)&&(hclk < 100000000))
  {
    /* CSR Clock Range between 60-100 MHz */ 
    tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div42;
  }  
  else if((hclk >= 100000000)&&(hclk < 150000000))
  {
    /* CSR Clock Range between 100-150 MHz */ 
    tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div62;
  }
  else /* ((hclk >= 150000000)&&(hclk <= 168000000)) */
  {
    /* CSR Clock Range between 150-168 MHz */ 
    tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div102;    
  }
  
  /* Write to ETHERNET MAC MIIAR: Configure the ETHERNET CSR Clock Range */
  (heth->Instance)->MACMIIAR = (uint32_t)tmpreg;
  
  /*-------------------- PHY initialization and configuration ----------------*/
  /* Put the PHY in reset mode */
  if((HAL_ETH_WritePHYRegister(heth, PHY_BCR, PHY_RESET)) != HAL_OK)
  {
    /* In case of write timeout */
    err = ETH_ERROR;
    
    /* Config MAC and DMA */
    ETH_MACDMAConfig(heth, err);
    
    /* Set the ETH peripheral state to READY */
    heth->State = HAL_ETH_STATE_READY;
    
    /* Return HAL_ERROR */
    return HAL_ERROR;
  }
  
  /* Delay to assure PHY reset */
  HAL_Delay(PHY_RESET_DELAY);
  
  if((heth->Init).AutoNegotiation != ETH_AUTONEGOTIATION_DISABLE)
  {
    /* We wait for linked status */
    do
    {
      timeout++;
      HAL_ETH_ReadPHYRegister(heth, PHY_BSR, &phyreg);
    } while (((phyreg & PHY_LINKED_STATUS) != PHY_LINKED_STATUS) && (timeout < PHY_READ_TO));
    
    if(timeout == PHY_READ_TO)
    {
      /* In case of write timeout */
      err = ETH_ERROR;
      
      /* Config MAC and DMA */
      ETH_MACDMAConfig(heth, err);
      
      /* Set the ETH peripheral state to READY */
      heth->State = HAL_ETH_STATE_READY;
      
      /* Return HAL_ERROR */
      return HAL_ERROR;
    }
    
    /* Reset Timeout counter */
    timeout = 0; 
    
    /* Enable Auto-Negotiation */
    if((HAL_ETH_WritePHYRegister(heth, PHY_BCR, PHY_AUTONEGOTIATION)) != HAL_OK)
    {
      /* In case of write timeout */
      err = ETH_ERROR;
      
      /* Config MAC and DMA */
      ETH_MACDMAConfig(heth, err);
      
      /* Set the ETH peripheral state to READY */
      heth->State = HAL_ETH_STATE_READY;
      
      /* Return HAL_ERROR */
      return HAL_ERROR;   
    }
    
    /* Wait until the auto-negotiation will be completed */
    do
    {
      timeout++;
      HAL_ETH_ReadPHYRegister(heth, PHY_BSR, &phyreg);
    } while (((phyreg & PHY_AUTONEGO_COMPLETE) != PHY_AUTONEGO_COMPLETE) && (timeout < PHY_READ_TO));
    
    if(timeout == PHY_READ_TO)
    {
      /* In case of timeout */
      err = ETH_ERROR;
      
      /* Config MAC and DMA */
      ETH_MACDMAConfig(heth, err);
      
      /* Set the ETH peripheral state to READY */
      heth->State = HAL_ETH_STATE_READY;
      
      /* Return HAL_ERROR */
      return HAL_ERROR;
    }
    
    /* Reset Timeout counter */
    timeout = 0;
    
    /* Read the result of the auto-negotiation */
    HAL_ETH_ReadPHYRegister(heth, PHY_SR, &phyreg);
    
    /* Configure the MAC with the Duplex Mode fixed by the auto-negotiation process */
    if((phyreg & PHY_DUPLEX_STATUS) != (uint32_t)RESET)
    {
      /* Set Ethernet duplex mode to Full-duplex following the auto-negotiation */
      (heth->Init).DuplexMode = ETH_MODE_FULLDUPLEX;  
    }
    else
    {
      /* Set Ethernet duplex mode to Half-duplex following the auto-negotiation */
      (heth->Init).DuplexMode = ETH_MODE_HALFDUPLEX;           
    }
    /* Configure the MAC with the speed fixed by the auto-negotiation process */
    if((phyreg & PHY_SPEED_STATUS) == PHY_SPEED_STATUS)
    {  
      /* Set Ethernet speed to 10M following the auto-negotiation */
      (heth->Init).Speed = ETH_SPEED_10M; 
    }
    else
    {   
      /* Set Ethernet speed to 100M following the auto-negotiation */ 
      (heth->Init).Speed = ETH_SPEED_100M;
    }
  }
  else /* AutoNegotiation Disable */
  {
    /* Check parameters */
    assert_param(IS_ETH_SPEED(heth->Init.Speed));
    assert_param(IS_ETH_DUPLEX_MODE(heth->Init.DuplexMode));
    
    /* Set MAC Speed and Duplex Mode */
    if(HAL_ETH_WritePHYRegister(heth, PHY_BCR, ((uint16_t)((heth->Init).DuplexMode >> 3) |
                                                (uint16_t)((heth->Init).Speed >> 1))) != HAL_OK)
    {
      /* In case of write timeout */
      err = ETH_ERROR;
      
      /* Config MAC and DMA */
      ETH_MACDMAConfig(heth, err);
      
      /* Set the ETH peripheral state to READY */
      heth->State = HAL_ETH_STATE_READY;
      
      /* Return HAL_ERROR */
      return HAL_ERROR;
    }  
    
    /* Delay to assure PHY configuration */
    HAL_Delay(PHY_CONFIG_DELAY);
  }
  
  /* Config MAC and DMA */
  ETH_MACDMAConfig(heth, err);
  
  /* Set ETH HAL State to Ready */
  heth->State= HAL_ETH_STATE_READY;
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  De-Initializes the ETH peripheral. 
  * @param  heth: ETH handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_DeInit(ETH_HandleTypeDef *heth)
{
  /* Set the ETH peripheral state to BUSY */
  heth->State = HAL_ETH_STATE_BUSY;
  
  /* De-Init the low level hardware : GPIO, CLOCK, NVIC. */
  HAL_ETH_MspDeInit(heth);
  
  /* Set ETH HAL state to Disabled */
  heth->State= HAL_ETH_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(heth);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the DMA Tx descriptors in chain mode.
  * @param  heth: ETH handle  
  * @param  DMATxDescTab: Pointer to the first Tx desc list 
  * @param  TxBuff: Pointer to the first TxBuffer list
  * @param  TxBuffCount: Number of the used Tx desc in the list
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_DMATxDescListInit(ETH_HandleTypeDef *heth, ETH_DMADescTypeDef *DMATxDescTab, uint8_t *TxBuff, uint32_t TxBuffCount)
{
  uint32_t i = 0;
  ETH_DMADescTypeDef *dmatxdesc;
  
  /* Process Locked */
  __HAL_LOCK(heth);
  
  /* Set the ETH peripheral state to BUSY */
  heth->State = HAL_ETH_STATE_BUSY;
  
  /* Set the DMATxDescToSet pointer with the first one of the DMATxDescTab list */
  heth->TxDesc = DMATxDescTab;
  
  /* Fill each DMATxDesc descriptor with the right values */   
  for(i=0; i < TxBuffCount; i++)
  {
    /* Get the pointer on the ith member of the Tx Desc list */
    dmatxdesc = DMATxDescTab + i;
    
    /* Set Second Address Chained bit */
    dmatxdesc->Status = ETH_DMATXDESC_TCH;  
    
    /* Set Buffer1 address pointer */
    dmatxdesc->Buffer1Addr = (uint32_t)(&TxBuff[i*ETH_TX_BUF_SIZE]);
    
    if ((heth->Init).ChecksumMode == ETH_CHECKSUM_BY_HARDWARE)
    {
      /* Set the DMA Tx descriptors checksum insertion */
      dmatxdesc->Status |= ETH_DMATXDESC_CHECKSUMTCPUDPICMPFULL;
    }
    
    /* Initialize the next descriptor with the Next Descriptor Polling Enable */
    if(i < (TxBuffCount-1))
    {
      /* Set next descriptor address register with next descriptor base address */
      dmatxdesc->Buffer2NextDescAddr = (uint32_t)(DMATxDescTab+i+1);
    }
    else
    {
      /* For last descriptor, set next descriptor address register equal to the first descriptor base address */ 
      dmatxdesc->Buffer2NextDescAddr = (uint32_t) DMATxDescTab;  
    }
  }
  
  /* Set Transmit Descriptor List Address Register */
  (heth->Instance)->DMATDLAR = (uint32_t) DMATxDescTab;
  
  /* Set ETH HAL State to Ready */
  heth->State= HAL_ETH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(heth);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the DMA Rx descriptors in chain mode.
  * @param  heth: ETH handle  
  * @param  DMARxDescTab: Pointer to the first Rx desc list 
  * @param  RxBuff: Pointer to the first RxBuffer list
  * @param  RxBuffCount: Number of the used Rx desc in the list
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_DMARxDescListInit(ETH_HandleTypeDef *heth, ETH_DMADescTypeDef *DMARxDescTab, uint8_t *RxBuff, uint32_t RxBuffCount)
{
  uint32_t i = 0;
  ETH_DMADescTypeDef *DMARxDesc;
  
  /* Process Locked */
  __HAL_LOCK(heth);
  
  /* Set the ETH peripheral state to BUSY */
  heth->State = HAL_ETH_STATE_BUSY;
  
  /* Set the Ethernet RxDesc pointer with the first one of the DMARxDescTab list */
  heth->RxDesc = DMARxDescTab; 
  
  /* Fill each DMARxDesc descriptor with the right values */
  for(i=0; i < RxBuffCount; i++)
  {
    /* Get the pointer on the ith member of the Rx Desc list */
    DMARxDesc = DMARxDescTab+i;
    
    /* Set Own bit of the Rx descriptor Status */
    DMARxDesc->Status = ETH_DMARXDESC_OWN;
    
    /* Set Buffer1 size and Second Address Chained bit */
    DMARxDesc->ControlBufferSize = ETH_DMARXDESC_RCH | ETH_RX_BUF_SIZE;  
    
    /* Set Buffer1 address pointer */
    DMARxDesc->Buffer1Addr = (uint32_t)(&RxBuff[i*ETH_RX_BUF_SIZE]);
    
    if((heth->Init).RxMode == ETH_RXINTERRUPT_MODE)
    {
      /* Enable Ethernet DMA Rx Descriptor interrupt */
      DMARxDesc->ControlBufferSize &= ~ETH_DMARXDESC_DIC;
    }
    
    /* Initialize the next descriptor with the Next Descriptor Polling Enable */
    if(i < (RxBuffCount-1))
    {
      /* Set next descriptor address register with next descriptor base address */
      DMARxDesc->Buffer2NextDescAddr = (uint32_t)(DMARxDescTab+i+1); 
    }
    else
    {
      /* For last descriptor, set next descriptor address register equal to the first descriptor base address */ 
      DMARxDesc->Buffer2NextDescAddr = (uint32_t)(DMARxDescTab); 
    }
  }
  
  /* Set Receive Descriptor List Address Register */
  (heth->Instance)->DMARDLAR = (uint32_t) DMARxDescTab;
  
  /* Set ETH HAL State to Ready */
  heth->State= HAL_ETH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(heth);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the ETH MSP.
  * @param  heth: ETH handle
  * @retval None
  */
__weak void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
  the HAL_ETH_MspInit could be implemented in the user file
  */
}

/**
  * @brief  DeInitializes ETH MSP.
  * @param  heth: ETH handle
  * @retval None
  */
__weak void HAL_ETH_MspDeInit(ETH_HandleTypeDef *heth)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
  the HAL_ETH_MspDeInit could be implemented in the user file
  */
}

/**
  * @}
  */

/** @defgroup ETH_Group2 IO operation functions 
  *  @brief   Data transfers functions 
  *
  @verbatim   
  ==============================================================================
                          ##### IO operation functions #####
  ==============================================================================  
  [..]  This section provides functions allowing to:
        (+) Transmit a frame
            HAL_ETH_TransmitFrame();
        (+) Receive a frame
            HAL_ETH_GetReceivedFrame();
            HAL_ETH_GetReceivedFrame_IT();
        (+) Read from an External PHY register
            HAL_ETH_ReadPHYRegister();
        (+) Writo to an External PHY register
            HAL_ETH_WritePHYRegister();

  @endverbatim
  
  * @{
  */

/**
  * @brief  Sends an Ethernet frame. 
  * @param  heth: ETH handle
  * @param  FrameLength: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_TransmitFrame(ETH_HandleTypeDef *heth, uint32_t FrameLength)
{
  uint32_t bufcount = 0, size = 0, i = 0;
  
  /* Process Locked */
  __HAL_LOCK(heth);
  
  /* Set the ETH peripheral state to BUSY */
  heth->State = HAL_ETH_STATE_BUSY;
  
  if (FrameLength == 0) 
  {
    /* Set ETH HAL state to READY */
    heth->State = HAL_ETH_STATE_READY;
    
    /* Process Unlocked */
    __HAL_UNLOCK(heth);
    
    return  HAL_ERROR;                                    
  }  
  
  /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
  if(((heth->TxDesc)->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET)
  {  
    /* OWN bit set */
    heth->State = HAL_ETH_STATE_BUSY_TX;
    
    /* Process Unlocked */
    __HAL_UNLOCK(heth);
    
    return HAL_ERROR;
  }
  
  /* Get the number of needed Tx buffers for the current frame */
  if (FrameLength > ETH_TX_BUF_SIZE)
  {
    bufcount = FrameLength/ETH_TX_BUF_SIZE;
    if (FrameLength % ETH_TX_BUF_SIZE) 
    {
      bufcount++;
    }
  }
  else 
  {  
    bufcount = 1;
  }
  if (bufcount == 1)
  {
    /* Set LAST and FIRST segment */
    heth->TxDesc->Status |=ETH_DMATXDESC_FS|ETH_DMATXDESC_LS;
    /* Set frame size */
    heth->TxDesc->ControlBufferSize = (FrameLength & ETH_DMATXDESC_TBS1);
    /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
    heth->TxDesc->Status |= ETH_DMATXDESC_OWN;
    /* Point to next descriptor */
    heth->TxDesc= (ETH_DMADescTypeDef *)(heth->TxDesc->Buffer2NextDescAddr);
  }
  else
  {
    for (i=0; i< bufcount; i++)
    {
      /* Clear FIRST and LAST segment bits */
      heth->TxDesc->Status &= ~(ETH_DMATXDESC_FS | ETH_DMATXDESC_LS);
      
      if (i == 0) 
      {
        /* Setting the first segment bit */
        heth->TxDesc->Status |= ETH_DMATXDESC_FS;  
      }
      
      /* Program size */
      heth->TxDesc->ControlBufferSize = (ETH_TX_BUF_SIZE & ETH_DMATXDESC_TBS1);
      
      if (i == (bufcount-1))
      {
        /* Setting the last segment bit */
        heth->TxDesc->Status |= ETH_DMATXDESC_LS;
        size = FrameLength - (bufcount-1)*ETH_TX_BUF_SIZE;
        heth->TxDesc->ControlBufferSize = (size & ETH_DMATXDESC_TBS1);
      }
      
      /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
      heth->TxDesc->Status |= ETH_DMATXDESC_OWN;
      /* point to next descriptor */
      heth->TxDesc = (ETH_DMADescTypeDef *)(heth->TxDesc->Buffer2NextDescAddr);
    }
  }
  
  /* When Tx Buffer unavailable flag is set: clear it and resume transmission */
  if (((heth->Instance)->DMASR & ETH_DMASR_TBUS) != (uint32_t)RESET)
  {
    /* Clear TBUS ETHERNET DMA flag */
    (heth->Instance)->DMASR = ETH_DMASR_TBUS;
    /* Resume DMA transmission*/
    (heth->Instance)->DMATPDR = 0;
  }
  
  /* Set ETH HAL State to Ready */
  heth->State = HAL_ETH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(heth);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Checks for received frames. 
  * @param  heth: ETH handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_GetReceivedFrame(ETH_HandleTypeDef *heth)
{
  uint32_t framelength = 0;
  
  /* Process Locked */
  __HAL_LOCK(heth);
  
  /* Check the ETH state to BUSY */
  heth->State = HAL_ETH_STATE_BUSY;
  
  /* Check if segment is not owned by DMA */
  /* (((heth->RxDesc->Status & ETH_DMARXDESC_OWN) == (uint32_t)RESET) && ((heth->RxDesc->Status & ETH_DMARXDESC_LS) != (uint32_t)RESET)) */
  if(((heth->RxDesc->Status & ETH_DMARXDESC_OWN) == (uint32_t)RESET))
  {
    /* Check if last segment */
    if(((heth->RxDesc->Status & ETH_DMARXDESC_LS) != (uint32_t)RESET)) 
    {
      /* increment segment count */
      (heth->RxFrameInfos).SegCount++;
      
      /* Check if last segment is first segment: one segment contains the frame */
      if ((heth->RxFrameInfos).SegCount == 1)
      {
        (heth->RxFrameInfos).FSRxDesc =heth->RxDesc;
      }
      
      heth->RxFrameInfos.LSRxDesc = heth->RxDesc;
      
      /* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
      framelength = (((heth->RxDesc)->Status & ETH_DMARXDESC_FL) >> ETH_DMARXDESC_FRAMELENGTHSHIFT) - 4;
      heth->RxFrameInfos.length = framelength;
      
      /* Get the address of the buffer start address */
      heth->RxFrameInfos.buffer = ((heth->RxFrameInfos).FSRxDesc)->Buffer1Addr;
      /* point to next descriptor */
      heth->RxDesc = (ETH_DMADescTypeDef*) ((heth->RxDesc)->Buffer2NextDescAddr);
      
      /* Set HAL State to Ready */
      heth->State = HAL_ETH_STATE_READY;
      
      /* Process Unlocked */
      __HAL_UNLOCK(heth);
      
      /* Return function status */
      return HAL_OK;
    }
    /* Check if first segment */
    else if((heth->RxDesc->Status & ETH_DMARXDESC_FS) != (uint32_t)RESET)
    {
      (heth->RxFrameInfos).FSRxDesc = heth->RxDesc;
      (heth->RxFrameInfos).LSRxDesc = NULL;
      (heth->RxFrameInfos).SegCount = 1;
      /* Point to next descriptor */
      heth->RxDesc = (ETH_DMADescTypeDef*) (heth->RxDesc->Buffer2NextDescAddr);
    }
    /* Check if intermediate segment */ 
    else
    {
      (heth->RxFrameInfos).SegCount++;
      /* Point to next descriptor */
      heth->RxDesc = (ETH_DMADescTypeDef*) (heth->RxDesc->Buffer2NextDescAddr);
    } 
  }
  
  /* Set ETH HAL State to Ready */
  heth->State = HAL_ETH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(heth);
  
  return HAL_ERROR;
}

/**
  * @brief  Gets the Received frame in interrupt mode. 
  * @param  heth: ETH handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_GetReceivedFrame_IT(ETH_HandleTypeDef *heth)
{
  uint32_t descriptorscancounter = 0;
  
  /* Process Locked */
  __HAL_LOCK(heth);
  
  /* Set ETH HAL State to BUSY */
  heth->State = HAL_ETH_STATE_BUSY;
  
  /* Scan descriptors owned by CPU */
  while (((heth->RxDesc->Status & ETH_DMARXDESC_OWN) == (uint32_t)RESET) && (descriptorscancounter < ETH_RXBUFNB))
  {
    /* Just for security */
    descriptorscancounter++;
    
    /* Check if first segment in frame */
    /* ((heth->RxDesc->Status & ETH_DMARXDESC_FS) != (uint32_t)RESET) && ((heth->RxDesc->Status & ETH_DMARXDESC_LS) == (uint32_t)RESET)) */  
    if((heth->RxDesc->Status & (ETH_DMARXDESC_FS | ETH_DMARXDESC_LS)) == (uint32_t)ETH_DMARXDESC_FS)
    { 
      heth->RxFrameInfos.FSRxDesc = heth->RxDesc;
      heth->RxFrameInfos.SegCount = 1;   
      /* Point to next descriptor */
      heth->RxDesc = (ETH_DMADescTypeDef*) (heth->RxDesc->Buffer2NextDescAddr);
    }
    /* Check if intermediate segment */
    /* ((heth->RxDesc->Status & ETH_DMARXDESC_LS) == (uint32_t)RESET)&& ((heth->RxDesc->Status & ETH_DMARXDESC_FS) == (uint32_t)RESET)) */
    else if ((heth->RxDesc->Status & (ETH_DMARXDESC_LS | ETH_DMARXDESC_FS)) == (uint32_t)RESET)
    {
      /* Increment segment count */
      (heth->RxFrameInfos.SegCount)++;
      /* Point to next descriptor */
      heth->RxDesc = (ETH_DMADescTypeDef*)(heth->RxDesc->Buffer2NextDescAddr);
    }
    /* Should be last segment */
    else
    { 
      /* Last segment */
      heth->RxFrameInfos.LSRxDesc = heth->RxDesc;
      
      /* Increment segment count */
      (heth->RxFrameInfos.SegCount)++;
      
      /* Check if last segment is first segment: one segment contains the frame */
      if ((heth->RxFrameInfos.SegCount) == 1)
      {
        heth->RxFrameInfos.FSRxDesc = heth->RxDesc;
      }
      
      /* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
      heth->RxFrameInfos.length = (((heth->RxDesc)->Status & ETH_DMARXDESC_FL) >> ETH_DMARXDESC_FRAMELENGTHSHIFT) - 4;
      
      /* Get the address of the buffer start address */ 
      heth->RxFrameInfos.buffer =((heth->RxFrameInfos).FSRxDesc)->Buffer1Addr;
      
      /* Point to next descriptor */      
      heth->RxDesc = (ETH_DMADescTypeDef*) (heth->RxDesc->Buffer2NextDescAddr);
      
      /* Set HAL State to Ready */
      heth->State = HAL_ETH_STATE_READY;
      
      /* Process Unlocked */
      __HAL_UNLOCK(heth);
  
      /* Return function status */
      return HAL_OK;
    }
  }

  /* Set HAL State to Ready */
  heth->State = HAL_ETH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(heth);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  This function handles ETH interrupt request.
  * @param  heth: ETH handle
  * @retval HAL status
  */
void HAL_ETH_IRQHandler(ETH_HandleTypeDef *heth)
{
  /* Frame received */
  if (__HAL_ETH_DMA_GET_FLAG(heth, ETH_DMA_FLAG_R)) 
  {
    /* Receive complete callback */
    HAL_ETH_RxCpltCallback(heth);
    
     /* Clear the Eth DMA Rx IT pending bits */
    __HAL_ETH_DMA_CLEAR_IT(heth, ETH_DMA_IT_R);

    /* Set HAL State to Ready */
    heth->State = HAL_ETH_STATE_READY;
    
    /* Process Unlocked */
    __HAL_UNLOCK(heth);

  }
  /* Frame transmitted */
  else if (__HAL_ETH_DMA_GET_FLAG(heth, ETH_DMA_FLAG_T)) 
  {
    /* Transfer complete callback */
    HAL_ETH_TxCpltCallback(heth);
    
    /* Clear the Eth DMA Tx IT pending bits */
    __HAL_ETH_DMA_CLEAR_IT(heth, ETH_DMA_IT_T);

    /* Set HAL State to Ready */
    heth->State = HAL_ETH_STATE_READY;
    
    /* Process Unlocked */
    __HAL_UNLOCK(heth);
  }
  
  /* Clear the interrupt flags */
  __HAL_ETH_DMA_CLEAR_IT(heth, ETH_DMA_IT_NIS);
  
  /* ETH DMA Error */
  if(__HAL_ETH_DMA_GET_FLAG(heth, ETH_DMA_FLAG_AIS))
  {
    /* Ethernet Error callback */
    HAL_ETH_ErrorCallback(heth);

    /* Clear the interrupt flags */
    __HAL_ETH_DMA_CLEAR_IT(heth, ETH_DMA_FLAG_AIS);
  
    /* Set HAL State to Ready */
    heth->State = HAL_ETH_STATE_READY;
    
    /* Process Unlocked */
    __HAL_UNLOCK(heth);
  }
}

/**
  * @brief  Tx Transfer completed callbacks.
  * @param  heth: ETH handle
  * @retval None
  */
__weak void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *heth)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
  the HAL_ETH_TxCpltCallback could be implemented in the user file
  */ 
}

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  heth: ETH handle
  * @retval None
  */
__weak void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
  the HAL_ETH_TxCpltCallback could be implemented in the user file
  */ 
}

/**
  * @brief  Ethernet transfer error callbacks
  * @param  heth: ETH handle
  * @retval None
  */
__weak void HAL_ETH_ErrorCallback(ETH_HandleTypeDef *heth)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
  the HAL_ETH_TxCpltCallback could be implemented in the user file
  */ 
}

/**
  * @brief  Reads a PHY register
  * @param heth: ETH handle                 
  * @param PHYReg: PHY register address, is the index of one of the 32 PHY register. 
  *                This parameter can be one of the following values: 
  *                   @arg PHY_BCR: Transceiver Basic Control Register 
  *                   @arg PHY_BSR: Transceiver Basic Status Register   
  *                   @arg More PHY register could be read depending on the used PHY
  * @param RegValue: PHY register value                  
  * @retval HAL_TIMEOUT: in case of timeout
  *         MACMIIDR register value: Data read from the selected PHY register (correct read )
  */
HAL_StatusTypeDef HAL_ETH_ReadPHYRegister(ETH_HandleTypeDef *heth, uint16_t PHYReg, uint32_t *RegValue)
{
  uint32_t tmpreg = 0;     
  uint32_t timeout = 0;
  
  /* Check parameters */
  assert_param(IS_ETH_PHY_ADDRESS(heth->Init.PhyAddress));
  
  /* Check the ETH peripheral state */
  if(heth->State == HAL_ETH_STATE_BUSY_RD)
  {
    return HAL_BUSY;
  }
  /* Set ETH HAL State to BUSY_RD */
  heth->State = HAL_ETH_STATE_BUSY_RD;
  
  /* Get the ETHERNET MACMIIAR value */
  tmpreg = heth->Instance->MACMIIAR;
  
  /* Keep only the CSR Clock Range CR[2:0] bits value */
  tmpreg &= ~MACMIIAR_CR_MASK;
  
  /* Prepare the MII address register value */
  tmpreg |=(((uint32_t)heth->Init.PhyAddress << 11) & ETH_MACMIIAR_PA); /* Set the PHY device address   */
  tmpreg |=(((uint32_t)PHYReg<<6) & ETH_MACMIIAR_MR);                   /* Set the PHY register address */
  tmpreg &= ~ETH_MACMIIAR_MW;                                           /* Set the read mode            */
  tmpreg |= ETH_MACMIIAR_MB;                                            /* Set the MII Busy bit         */
  
  /* Write the result value into the MII Address register */
  heth->Instance->MACMIIAR = tmpreg;
  
  /* Check for the Busy flag */
  do
  {
    timeout++;
    tmpreg = heth->Instance->MACMIIAR;
  } while (((tmpreg & ETH_MACMIIAR_MB) == ETH_MACMIIAR_MB) && (timeout < PHY_READ_TO));
  
  /* Return ERROR in case of timeout */
  if(timeout == PHY_READ_TO)
  {
    /* Set ETH HAL State to READY */
    heth->State = HAL_ETH_STATE_READY;
    /* Return HAL_TIMEOUT */
    return HAL_TIMEOUT;
  }
  
  /* Get MACMIIDR value */
  *RegValue = (uint16_t)(heth->Instance->MACMIIDR);
  
  /* Set ETH HAL State to READY */
  heth->State = HAL_ETH_STATE_READY;
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Writes to a PHY register.
  * @param  heth: ETH handle  
  * @param  PHYReg: PHY register address, is the index of one of the 32 PHY register. 
  *          This parameter can be one of the following values: 
  *             @arg PHY_BCR: Transceiver Control Register  
  *             @arg More PHY register could be written depending on the used PHY
  * @param  RegValue: the value to write
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_WritePHYRegister(ETH_HandleTypeDef *heth, uint16_t PHYReg, uint32_t RegValue)
{
  uint32_t tmpreg = 0;
  uint32_t timeout = 0;
  
  /* Check parameters */
  assert_param(IS_ETH_PHY_ADDRESS(heth->Init.PhyAddress));
  
  /* Check the ETH peripheral state */
  if(heth->State == HAL_ETH_STATE_BUSY_WR)
  {
    return HAL_BUSY;
  }
  /* Set ETH HAL State to BUSY_WR */
  heth->State = HAL_ETH_STATE_BUSY_WR;
  
  /* Get the ETHERNET MACMIIAR value */
  tmpreg = heth->Instance->MACMIIAR;
  
  /* Keep only the CSR Clock Range CR[2:0] bits value */
  tmpreg &= ~MACMIIAR_CR_MASK;
  
  /* Prepare the MII register address value */
  tmpreg |=(((uint32_t)heth->Init.PhyAddress<<11) & ETH_MACMIIAR_PA); /* Set the PHY device address */
  tmpreg |=(((uint32_t)PHYReg<<6) & ETH_MACMIIAR_MR);                 /* Set the PHY register address */
  tmpreg |= ETH_MACMIIAR_MW;                                          /* Set the write mode */
  tmpreg |= ETH_MACMIIAR_MB;                                          /* Set the MII Busy bit */
  
  /* Give the value to the MII data register */
  heth->Instance->MACMIIDR = (uint16_t)RegValue;
  
  /* Write the result value into the MII Address register */
  heth->Instance->MACMIIAR = tmpreg;
  
  /* Check for the Busy flag */
  do
  {
    timeout++;
    tmpreg = heth->Instance->MACMIIAR;
  } while (((tmpreg & ETH_MACMIIAR_MB) == ETH_MACMIIAR_MB) && (timeout < PHY_WRITE_TO));
  
  /* Return TIMETOUT in case of timeout */
  if(timeout == PHY_WRITE_TO)
  {
    /* Set ETH HAL State to READY */
    heth->State = HAL_ETH_STATE_READY;
    
    return HAL_TIMEOUT;
  }
  
  /* Set ETH HAL State to READY */
  heth->State = HAL_ETH_STATE_READY;
  
  /* Return function status */
  return HAL_OK; 
}

/**
  * @}
  */

/** @defgroup ETH_Group3 Peripheral Control functions
 *  @brief    Peripheral Control functions 
 *
@verbatim   
 ===============================================================================
                  ##### Peripheral Control functions #####
 ===============================================================================  
    [..]  This section provides functions allowing to:
      (+) Enable MAC and DMA transmission and reception.
          HAL_ETH_Start();
      (+) Disable MAC and DMA transmission and reception. 
          HAL_ETH_Stop();
      (+) Set the MAC configuration in runtime mode
          HAL_ETH_ConfigMAC();
      (+) Set the DMA configuration in runtime mode
          HAL_ETH_ConfigDMA();

@endverbatim
  * @{
  */ 

 /**
  * @brief  Enables Ethernet MAC and DMA reception/transmission 
  * @param  heth: ETH handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_Start(ETH_HandleTypeDef *heth)
{  
  /* Process Locked */
  __HAL_LOCK(heth);
  
  /* Set the ETH peripheral state to BUSY */
  heth->State = HAL_ETH_STATE_BUSY;
  
  /* Enable transmit state machine of the MAC for transmission on the MII */
  ETH_MACTransmissionEnable(heth);
  
  /* Enable receive state machine of the MAC for reception from the MII */
  ETH_MACReceptionEnable(heth);
  
  /* Flush Transmit FIFO */
  ETH_FlushTransmitFIFO(heth);
  
  /* Start DMA transmission */
  ETH_DMATransmissionEnable(heth);
  
  /* Start DMA reception */
  ETH_DMAReceptionEnable(heth);
  
  /* Set the ETH state to READY*/
  heth->State= HAL_ETH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(heth);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stop Ethernet MAC and DMA reception/transmission 
  * @param  heth: ETH handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_Stop(ETH_HandleTypeDef *heth)
{  
  /* Process Locked */
  __HAL_LOCK(heth);
  
  /* Set the ETH peripheral state to BUSY */
  heth->State = HAL_ETH_STATE_BUSY;
  
  /* Stop DMA transmission */
  ETH_DMATransmissionDisable(heth);
  
  /* Stop DMA reception */
  ETH_DMAReceptionDisable(heth);
  
  /* Disable receive state machine of the MAC for reception from the MII */
  ETH_MACReceptionDisable(heth);
  
  /* Flush Transmit FIFO */
  ETH_FlushTransmitFIFO(heth);
  
  /* Disable transmit state machine of the MAC for transmission on the MII */
  ETH_MACTransmissionDisable(heth);
  
  /* Set the ETH state*/
  heth->State = HAL_ETH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(heth);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Set ETH MAC Configuration.
  * @param  heth: ETH handle
  * @param  macconf: MAC Configuration structure  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_ConfigMAC(ETH_HandleTypeDef *heth, ETH_MACInitTypeDef *macconf)
{
  uint32_t tmpreg = 0;
  
  /* Process Locked */
  __HAL_LOCK(heth);
  
  /* Set the ETH peripheral state to BUSY */
  heth->State= HAL_ETH_STATE_BUSY;
  
  assert_param(IS_ETH_SPEED(heth->Init.Speed));
  assert_param(IS_ETH_DUPLEX_MODE(heth->Init.DuplexMode)); 
  
  if (macconf != NULL)
  {
    /* Check the parameters */
    assert_param(IS_ETH_WATCHDOG(macconf->Watchdog));
    assert_param(IS_ETH_JABBER(macconf->Jabber));
    assert_param(IS_ETH_INTER_FRAME_GAP(macconf->InterFrameGap));
    assert_param(IS_ETH_CARRIER_SENSE(macconf->CarrierSense));
    assert_param(IS_ETH_RECEIVE_OWN(macconf->ReceiveOwn));
    assert_param(IS_ETH_LOOPBACK_MODE(macconf->LoopbackMode));
    assert_param(IS_ETH_CHECKSUM_OFFLOAD(macconf->ChecksumOffload));
    assert_param(IS_ETH_RETRY_TRANSMISSION(macconf->RetryTransmission));
    assert_param(IS_ETH_AUTOMATIC_PADCRC_STRIP(macconf->AutomaticPadCRCStrip));
    assert_param(IS_ETH_BACKOFF_LIMIT(macconf->BackOffLimit));
    assert_param(IS_ETH_DEFERRAL_CHECK(macconf->DeferralCheck));
    assert_param(IS_ETH_RECEIVE_ALL(macconf->ReceiveAll));
    assert_param(IS_ETH_SOURCE_ADDR_FILTER(macconf->SourceAddrFilter));
    assert_param(IS_ETH_CONTROL_FRAMES(macconf->PassControlFrames));
    assert_param(IS_ETH_BROADCAST_FRAMES_RECEPTION(macconf->BroadcastFramesReception));
    assert_param(IS_ETH_DESTINATION_ADDR_FILTER(macconf->DestinationAddrFilter));
    assert_param(IS_ETH_PROMISCIOUS_MODE(macconf->PromiscuousMode));
    assert_param(IS_ETH_MULTICAST_FRAMES_FILTER(macconf->MulticastFramesFilter));
    assert_param(IS_ETH_UNICAST_FRAMES_FILTER(macconf->UnicastFramesFilter));
    assert_param(IS_ETH_PAUSE_TIME(macconf->PauseTime));
    assert_param(IS_ETH_ZEROQUANTA_PAUSE(macconf->ZeroQuantaPause));
    assert_param(IS_ETH_PAUSE_LOW_THRESHOLD(macconf->PauseLowThreshold));
    assert_param(IS_ETH_UNICAST_PAUSE_FRAME_DETECT(macconf->UnicastPauseFrameDetect));
    assert_param(IS_ETH_RECEIVE_FLOWCONTROL(macconf->ReceiveFlowControl));
    assert_param(IS_ETH_TRANSMIT_FLOWCONTROL(macconf->TransmitFlowControl));
    assert_param(IS_ETH_VLAN_TAG_COMPARISON(macconf->VLANTagComparison));
    assert_param(IS_ETH_VLAN_TAG_IDENTIFIER(macconf->VLANTagIdentifier));
    
    /*------------------------ ETHERNET MACCR Configuration --------------------*/
    /* Get the ETHERNET MACCR value */
    tmpreg = (heth->Instance)->MACCR;
    /* Clear WD, PCE, PS, TE and RE bits */
    tmpreg &= MACCR_CLEAR_MASK;
    
    tmpreg |= (uint32_t)(macconf->Watchdog | 
                         macconf->Jabber | 
                         macconf->InterFrameGap |
                         macconf->CarrierSense |
                         (heth->Init).Speed | 
                         macconf->ReceiveOwn |
                         macconf->LoopbackMode |
                         (heth->Init).DuplexMode | 
                         macconf->ChecksumOffload |    
                         macconf->RetryTransmission | 
                         macconf->AutomaticPadCRCStrip | 
                         macconf->BackOffLimit | 
                         macconf->DeferralCheck);
    
    /* Write to ETHERNET MACCR */
    (heth->Instance)->MACCR = (uint32_t)tmpreg;
    
    /* Wait until the write operation will be taken into account :
    at least four TX_CLK/RX_CLK clock cycles */
    tmpreg = (heth->Instance)->MACCR;
    HAL_Delay(ETH_REG_WRITE_DELAY);
    (heth->Instance)->MACCR = tmpreg; 
    
    /*----------------------- ETHERNET MACFFR Configuration --------------------*/ 
    /* Write to ETHERNET MACFFR */  
    (heth->Instance)->MACFFR = (uint32_t)(macconf->ReceiveAll | 
                                          macconf->SourceAddrFilter |
                                          macconf->PassControlFrames |
                                          macconf->BroadcastFramesReception | 
                                          macconf->DestinationAddrFilter |
                                          macconf->PromiscuousMode |
                                          macconf->MulticastFramesFilter |
                                          macconf->UnicastFramesFilter);
     
     /* Wait until the write operation will be taken into account :
     at least four TX_CLK/RX_CLK clock cycles */
     tmpreg = (heth->Instance)->MACFFR;
     HAL_Delay(ETH_REG_WRITE_DELAY);
     (heth->Instance)->MACFFR = tmpreg;
     
     /*--------------- ETHERNET MACHTHR and MACHTLR Configuration ---------------*/
     /* Write to ETHERNET MACHTHR */
     (heth->Instance)->MACHTHR = (uint32_t)macconf->HashTableHigh;
     
     /* Write to ETHERNET MACHTLR */
     (heth->Instance)->MACHTLR = (uint32_t)macconf->HashTableLow;
     /*----------------------- ETHERNET MACFCR Configuration --------------------*/
     
     /* Get the ETHERNET MACFCR value */  
     tmpreg = (heth->Instance)->MACFCR;
     /* Clear xx bits */
     tmpreg &= MACFCR_CLEAR_MASK;
     
     tmpreg |= (uint32_t)((macconf->PauseTime << 16) | 
                          macconf->ZeroQuantaPause |
                          macconf->PauseLowThreshold |
                          macconf->UnicastPauseFrameDetect | 
                          macconf->ReceiveFlowControl |
                          macconf->TransmitFlowControl); 
     
     /* Write to ETHERNET MACFCR */
     (heth->Instance)->MACFCR = (uint32_t)tmpreg;
     
     /* Wait until the write operation will be taken into account :
     at least four TX_CLK/RX_CLK clock cycles */
     tmpreg = (heth->Instance)->MACFCR;
     HAL_Delay(ETH_REG_WRITE_DELAY);
     (heth->Instance)->MACFCR = tmpreg;
     
     /*----------------------- ETHERNET MACVLANTR Configuration -----------------*/
     (heth->Instance)->MACVLANTR = (uint32_t)(macconf->VLANTagComparison | 
                                              macconf->VLANTagIdentifier);
      
      /* Wait until the write operation will be taken into account :
      at least four TX_CLK/RX_CLK clock cycles */
      tmpreg = (heth->Instance)->MACVLANTR;
      HAL_Delay(ETH_REG_WRITE_DELAY);
      (heth->Instance)->MACVLANTR = tmpreg;
  }
  else /* macconf == NULL : here we just configure Speed and Duplex mode */
  {
    /*------------------------ ETHERNET MACCR Configuration --------------------*/
    /* Get the ETHERNET MACCR value */
    tmpreg = (heth->Instance)->MACCR;
    
    /* Clear FES and DM bits */
    tmpreg &= ~((uint32_t)0x00004800);
    
    tmpreg |= (uint32_t)(heth->Init.Speed | heth->Init.DuplexMode);
    
    /* Write to ETHERNET MACCR */
    (heth->Instance)->MACCR = (uint32_t)tmpreg;
    
    /* Wait until the write operation will be taken into account:
    at least four TX_CLK/RX_CLK clock cycles */
    tmpreg = (heth->Instance)->MACCR;
    HAL_Delay(ETH_REG_WRITE_DELAY);
    (heth->Instance)->MACCR = tmpreg;
  }
  
  /* Set the ETH state to Ready */
  heth->State= HAL_ETH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(heth);
  
  /* Return function status */
  return HAL_OK;  
}

/**
  * @brief  Sets ETH DMA Configuration.
  * @param  heth: ETH handle
  * @param  dmaconf: DMA Configuration structure  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_ConfigDMA(ETH_HandleTypeDef *heth, ETH_DMAInitTypeDef *dmaconf)
{
  uint32_t tmpreg = 0;

  /* Process Locked */
  __HAL_LOCK(heth);
  
  /* Set the ETH peripheral state to BUSY */
  heth->State= HAL_ETH_STATE_BUSY;

  /* Check parameters */
  assert_param(IS_ETH_DROP_TCPIP_CHECKSUM_FRAME(dmaconf->DropTCPIPChecksumErrorFrame));
  assert_param(IS_ETH_RECEIVE_STORE_FORWARD(dmaconf->ReceiveStoreForward));
  assert_param(IS_ETH_FLUSH_RECEIVE_FRAME(dmaconf->FlushReceivedFrame));
  assert_param(IS_ETH_TRANSMIT_STORE_FORWARD(dmaconf->TransmitStoreForward));
  assert_param(IS_ETH_TRANSMIT_THRESHOLD_CONTROL(dmaconf->TransmitThresholdControl));
  assert_param(IS_ETH_FORWARD_ERROR_FRAMES(dmaconf->ForwardErrorFrames));
  assert_param(IS_ETH_FORWARD_UNDERSIZED_GOOD_FRAMES(dmaconf->ForwardUndersizedGoodFrames));
  assert_param(IS_ETH_RECEIVE_THRESHOLD_CONTROL(dmaconf->ReceiveThresholdControl));
  assert_param(IS_ETH_SECOND_FRAME_OPERATE(dmaconf->SecondFrameOperate));
  assert_param(IS_ETH_ADDRESS_ALIGNED_BEATS(dmaconf->AddressAlignedBeats));
  assert_param(IS_ETH_FIXED_BURST(dmaconf->FixedBurst));
  assert_param(IS_ETH_RXDMA_BURST_LENGTH(dmaconf->RxDMABurstLength));
  assert_param(IS_ETH_TXDMA_BURST_LENGTH(dmaconf->TxDMABurstLength));
  assert_param(IS_ETH_ENHANCED_DESCRIPTOR_FORMAT(dmaconf->EnhancedDescriptorFormat));
  assert_param(IS_ETH_DMA_DESC_SKIP_LENGTH(dmaconf->DescriptorSkipLength));
  assert_param(IS_ETH_DMA_ARBITRATION_ROUNDROBIN_RXTX(dmaconf->DMAArbitration));
  
  /*----------------------- ETHERNET DMAOMR Configuration --------------------*/
  /* Get the ETHERNET DMAOMR value */
  tmpreg = (heth->Instance)->DMAOMR;
  /* Clear xx bits */
  tmpreg &= DMAOMR_CLEAR_MASK;

  tmpreg |= (uint32_t)(dmaconf->DropTCPIPChecksumErrorFrame | 
                       dmaconf->ReceiveStoreForward |
                       dmaconf->FlushReceivedFrame |
                       dmaconf->TransmitStoreForward | 
                       dmaconf->TransmitThresholdControl |
                       dmaconf->ForwardErrorFrames |
                       dmaconf->ForwardUndersizedGoodFrames |
                       dmaconf->ReceiveThresholdControl |
                       dmaconf->SecondFrameOperate);

  /* Write to ETHERNET DMAOMR */
  (heth->Instance)->DMAOMR = (uint32_t)tmpreg;

  /* Wait until the write operation will be taken into account:
  at least four TX_CLK/RX_CLK clock cycles */
  tmpreg = (heth->Instance)->DMAOMR;
  HAL_Delay(ETH_REG_WRITE_DELAY);
  (heth->Instance)->DMAOMR = tmpreg;

  /*----------------------- ETHERNET DMABMR Configuration --------------------*/
  (heth->Instance)->DMABMR = (uint32_t)(dmaconf->AddressAlignedBeats | 
                                         dmaconf->FixedBurst |
                                         dmaconf->RxDMABurstLength | /* !! if 4xPBL is selected for Tx or Rx it is applied for the other */
                                         dmaconf->TxDMABurstLength |
                                         dmaconf->EnhancedDescriptorFormat |
                                         (dmaconf->DescriptorSkipLength << 2) |
                                         dmaconf->DMAArbitration | 
                                         ETH_DMABMR_USP); /* Enable use of separate PBL for Rx and Tx */

   /* Wait until the write operation will be taken into account:
      at least four TX_CLK/RX_CLK clock cycles */
   tmpreg = (heth->Instance)->DMABMR;
   HAL_Delay(ETH_REG_WRITE_DELAY);
   (heth->Instance)->DMABMR = tmpreg;

   /* Set the ETH state to Ready */
   heth->State= HAL_ETH_STATE_READY;
   
   /* Process Unlocked */
   __HAL_UNLOCK(heth);
   
   /* Return function status */
   return HAL_OK; 
}

/**
  * @}
  */

/** @defgroup ETH_Group4 Peripheral State functions 
  *  @brief   Peripheral State functions 
  *
  @verbatim   
  ===============================================================================
                         ##### Peripheral State functions #####
  ===============================================================================  
  [..]
  This subsection permits to get in run-time the status of the peripheral 
  and the data flow.
       (+) Get the ETH handle state:
           HAL_ETH_GetState();
           

  @endverbatim
  * @{
  */

/**
  * @brief  Return the ETH HAL state
  * @param  heth: ETH handle
  * @retval HAL state
  */
HAL_ETH_StateTypeDef HAL_ETH_GetState(ETH_HandleTypeDef *heth)
{  
  /* Return ETH state */
  return heth->State;
}

/**
  * @}
  */

/**
  * @brief  Configures Ethernet MAC and DMA with default parameters.
  * @param  heth: ETH handle
  * @param  err: Ethernet Init error
  * @retval HAL status
  */
static void ETH_MACDMAConfig(ETH_HandleTypeDef *heth, uint32_t err)
{
  ETH_MACInitTypeDef macinit;
  ETH_DMAInitTypeDef dmainit;
  uint32_t tmpreg = 0;
  
  if (err != ETH_SUCCESS) /* Auto-negotiation failed */
  {
    /* Set Ethernet duplex mode to Full-duplex */
    (heth->Init).DuplexMode = ETH_MODE_FULLDUPLEX;
    
    /* Set Ethernet speed to 100M */
    (heth->Init).Speed = ETH_SPEED_100M;
  }
  
  /* Ethernet MAC default initialization **************************************/
  macinit.Watchdog = ETH_WATCHDOG_ENABLE;
  macinit.Jabber = ETH_JABBER_ENABLE;
  macinit.InterFrameGap = ETH_INTERFRAMEGAP_96BIT;
  macinit.CarrierSense = ETH_CARRIERSENCE_ENABLE;
  macinit.ReceiveOwn = ETH_RECEIVEOWN_ENABLE;
  macinit.LoopbackMode = ETH_LOOPBACKMODE_DISABLE;
  if(heth->Init.ChecksumMode == ETH_CHECKSUM_BY_HARDWARE)
  {
    macinit.ChecksumOffload = ETH_CHECKSUMOFFLAOD_ENABLE;
  }
  else
  {
    macinit.ChecksumOffload = ETH_CHECKSUMOFFLAOD_DISABLE;
  }
  macinit.RetryTransmission = ETH_RETRYTRANSMISSION_DISABLE;
  macinit.AutomaticPadCRCStrip = ETH_AUTOMATICPADCRCSTRIP_DISABLE;
  macinit.BackOffLimit = ETH_BACKOFFLIMIT_10;
  macinit.DeferralCheck = ETH_DEFFERRALCHECK_DISABLE;
  macinit.ReceiveAll = ETH_RECEIVEAll_DISABLE;
  macinit.SourceAddrFilter = ETH_SOURCEADDRFILTER_DISABLE;
  macinit.PassControlFrames = ETH_PASSCONTROLFRAMES_BLOCKALL;
  macinit.BroadcastFramesReception = ETH_BROADCASTFRAMESRECEPTION_ENABLE;
  macinit.DestinationAddrFilter = ETH_DESTINATIONADDRFILTER_NORMAL;
  macinit.PromiscuousMode = ETH_PROMISCIOUSMODE_DISABLE;
  macinit.MulticastFramesFilter = ETH_MULTICASTFRAMESFILTER_PERFECT;
  macinit.UnicastFramesFilter = ETH_UNICASTFRAMESFILTER_PERFECT;
  macinit.HashTableHigh = 0x0;
  macinit.HashTableLow = 0x0;
  macinit.PauseTime = 0x0;
  macinit.ZeroQuantaPause = ETH_ZEROQUANTAPAUSE_DISABLE;
  macinit.PauseLowThreshold = ETH_PAUSELOWTHRESHOLD_MINUS4;
  macinit.UnicastPauseFrameDetect = ETH_UNICASTPAUSEFRAMEDETECT_DISABLE;
  macinit.ReceiveFlowControl = ETH_RECEIVEFLOWCONTROL_DISABLE;
  macinit.TransmitFlowControl = ETH_TRANSMITFLOWCONTROL_DISABLE;
  macinit.VLANTagComparison = ETH_VLANTAGCOMPARISON_16BIT;
  macinit.VLANTagIdentifier = 0x0;
  
  /*------------------------ ETHERNET MACCR Configuration --------------------*/
  /* Get the ETHERNET MACCR value */
  tmpreg = (heth->Instance)->MACCR;
  /* Clear WD, PCE, PS, TE and RE bits */
  tmpreg &= MACCR_CLEAR_MASK;
  /* Set the WD bit according to ETH Watchdog value */
  /* Set the JD: bit according to ETH Jabber value */
  /* Set the IFG bit according to ETH InterFrameGap value */
  /* Set the DCRS bit according to ETH CarrierSense value */
  /* Set the FES bit according to ETH Speed value */ 
  /* Set the DO bit according to ETH ReceiveOwn value */ 
  /* Set the LM bit according to ETH LoopbackMode value */
  /* Set the DM bit according to ETH Mode value */ 
  /* Set the IPCO bit according to ETH ChecksumOffload value */
  /* Set the DR bit according to ETH RetryTransmission value */
  /* Set the ACS bit according to ETH AutomaticPadCRCStrip value */
  /* Set the BL bit according to ETH BackOffLimit value */
  /* Set the DC bit according to ETH DeferralCheck value */
  tmpreg |= (uint32_t)(macinit.Watchdog | 
                       macinit.Jabber | 
                       macinit.InterFrameGap |
                       macinit.CarrierSense |
                       (heth->Init).Speed | 
                       macinit.ReceiveOwn |
                       macinit.LoopbackMode |
                       (heth->Init).DuplexMode | 
                       macinit.ChecksumOffload |    
                       macinit.RetryTransmission | 
                       macinit.AutomaticPadCRCStrip | 
                       macinit.BackOffLimit | 
                       macinit.DeferralCheck);
  
  /* Write to ETHERNET MACCR */
  (heth->Instance)->MACCR = (uint32_t)tmpreg;
  
  /* Wait until the write operation will be taken into account:
     at least four TX_CLK/RX_CLK clock cycles */
  tmpreg = (heth->Instance)->MACCR;
  HAL_Delay(ETH_REG_WRITE_DELAY);
  (heth->Instance)->MACCR = tmpreg; 
  
  /*----------------------- ETHERNET MACFFR Configuration --------------------*/ 
  /* Set the RA bit according to ETH ReceiveAll value */
  /* Set the SAF and SAIF bits according to ETH SourceAddrFilter value */
  /* Set the PCF bit according to ETH PassControlFrames value */
  /* Set the DBF bit according to ETH BroadcastFramesReception value */
  /* Set the DAIF bit according to ETH DestinationAddrFilter value */
  /* Set the PR bit according to ETH PromiscuousMode value */
  /* Set the PM, HMC and HPF bits according to ETH MulticastFramesFilter value */
  /* Set the HUC and HPF bits according to ETH UnicastFramesFilter value */
  /* Write to ETHERNET MACFFR */  
  (heth->Instance)->MACFFR = (uint32_t)(macinit.ReceiveAll | 
                                        macinit.SourceAddrFilter |
                                        macinit.PassControlFrames |
                                        macinit.BroadcastFramesReception | 
                                        macinit.DestinationAddrFilter |
                                        macinit.PromiscuousMode |
                                        macinit.MulticastFramesFilter |
                                        macinit.UnicastFramesFilter);
   
   /* Wait until the write operation will be taken into account:
      at least four TX_CLK/RX_CLK clock cycles */
   tmpreg = (heth->Instance)->MACFFR;
   HAL_Delay(ETH_REG_WRITE_DELAY);
   (heth->Instance)->MACFFR = tmpreg;
   
   /*--------------- ETHERNET MACHTHR and MACHTLR Configuration --------------*/
   /* Write to ETHERNET MACHTHR */
   (heth->Instance)->MACHTHR = (uint32_t)macinit.HashTableHigh;
   
   /* Write to ETHERNET MACHTLR */
   (heth->Instance)->MACHTLR = (uint32_t)macinit.HashTableLow;
   /*----------------------- ETHERNET MACFCR Configuration -------------------*/
   
   /* Get the ETHERNET MACFCR value */  
   tmpreg = (heth->Instance)->MACFCR;
   /* Clear xx bits */
   tmpreg &= MACFCR_CLEAR_MASK;
   
   /* Set the PT bit according to ETH PauseTime value */
   /* Set the DZPQ bit according to ETH ZeroQuantaPause value */
   /* Set the PLT bit according to ETH PauseLowThreshold value */
   /* Set the UP bit according to ETH UnicastPauseFrameDetect value */
   /* Set the RFE bit according to ETH ReceiveFlowControl value */
   /* Set the TFE bit according to ETH TransmitFlowControl value */ 
   tmpreg |= (uint32_t)((macinit.PauseTime << 16) | 
                        macinit.ZeroQuantaPause |
                        macinit.PauseLowThreshold |
                        macinit.UnicastPauseFrameDetect | 
                        macinit.ReceiveFlowControl |
                        macinit.TransmitFlowControl); 
   
   /* Write to ETHERNET MACFCR */
   (heth->Instance)->MACFCR = (uint32_t)tmpreg;
   
   /* Wait until the write operation will be taken into account:
   at least four TX_CLK/RX_CLK clock cycles */
   tmpreg = (heth->Instance)->MACFCR;
   HAL_Delay(ETH_REG_WRITE_DELAY);
   (heth->Instance)->MACFCR = tmpreg;
   
   /*----------------------- ETHERNET MACVLANTR Configuration ----------------*/
   /* Set the ETV bit according to ETH VLANTagComparison value */
   /* Set the VL bit according to ETH VLANTagIdentifier value */  
   (heth->Instance)->MACVLANTR = (uint32_t)(macinit.VLANTagComparison | 
                                            macinit.VLANTagIdentifier);
    
    /* Wait until the write operation will be taken into account:
       at least four TX_CLK/RX_CLK clock cycles */
    tmpreg = (heth->Instance)->MACVLANTR;
    HAL_Delay(ETH_REG_WRITE_DELAY);
    (heth->Instance)->MACVLANTR = tmpreg;
    
    /* Ethernet DMA default initialization ************************************/
    dmainit.DropTCPIPChecksumErrorFrame = ETH_DROPTCPIPCHECKSUMERRORFRAME_ENABLE;
    dmainit.ReceiveStoreForward = ETH_RECEIVESTOREFORWARD_ENABLE;
    dmainit.FlushReceivedFrame = ETH_FLUSHRECEIVEDFRAME_ENABLE;
    dmainit.TransmitStoreForward = ETH_TRANSMITSTOREFORWARD_ENABLE;  
    dmainit.TransmitThresholdControl = ETH_TRANSMITTHRESHOLDCONTROL_64BYTES;
    dmainit.ForwardErrorFrames = ETH_FORWARDERRORFRAMES_DISABLE;
    dmainit.ForwardUndersizedGoodFrames = ETH_FORWARDUNDERSIZEDGOODFRAMES_DISABLE;
    dmainit.ReceiveThresholdControl = ETH_RECEIVEDTHRESHOLDCONTROL_64BYTES;
    dmainit.SecondFrameOperate = ETH_SECONDFRAMEOPERARTE_ENABLE;
    dmainit.AddressAlignedBeats = ETH_ADDRESSALIGNEDBEATS_ENABLE;
    dmainit.FixedBurst = ETH_FIXEDBURST_ENABLE;
    dmainit.RxDMABurstLength = ETH_RXDMABURSTLENGTH_32BEAT;
    dmainit.TxDMABurstLength = ETH_TXDMABURSTLENGTH_32BEAT;
    dmainit.EnhancedDescriptorFormat = ETH_DMAENHANCEDDESCRIPTOR_ENABLE;
    dmainit.DescriptorSkipLength = 0x0;
    dmainit.DMAArbitration = ETH_DMAARBITRATION_ROUNDROBIN_RXTX_1_1;
    
    /* Get the ETHERNET DMAOMR value */
    tmpreg = (heth->Instance)->DMAOMR;
    /* Clear xx bits */
    tmpreg &= DMAOMR_CLEAR_MASK;
    
    /* Set the DT bit according to ETH DropTCPIPChecksumErrorFrame value */
    /* Set the RSF bit according to ETH ReceiveStoreForward value */
    /* Set the DFF bit according to ETH FlushReceivedFrame value */
    /* Set the TSF bit according to ETH TransmitStoreForward value */
    /* Set the TTC bit according to ETH TransmitThresholdControl value */
    /* Set the FEF bit according to ETH ForwardErrorFrames value */
    /* Set the FUF bit according to ETH ForwardUndersizedGoodFrames value */
    /* Set the RTC bit according to ETH ReceiveThresholdControl value */
    /* Set the OSF bit according to ETH SecondFrameOperate value */
    tmpreg |= (uint32_t)(dmainit.DropTCPIPChecksumErrorFrame | 
                         dmainit.ReceiveStoreForward |
                         dmainit.FlushReceivedFrame |
                         dmainit.TransmitStoreForward | 
                         dmainit.TransmitThresholdControl |
                         dmainit.ForwardErrorFrames |
                         dmainit.ForwardUndersizedGoodFrames |
                         dmainit.ReceiveThresholdControl |
                         dmainit.SecondFrameOperate);
    
    /* Write to ETHERNET DMAOMR */
    (heth->Instance)->DMAOMR = (uint32_t)tmpreg;
    
    /* Wait until the write operation will be taken into account:
       at least four TX_CLK/RX_CLK clock cycles */
    tmpreg = (heth->Instance)->DMAOMR;
    HAL_Delay(ETH_REG_WRITE_DELAY);
    (heth->Instance)->DMAOMR = tmpreg;
    
    /*----------------------- ETHERNET DMABMR Configuration ------------------*/
    /* Set the AAL bit according to ETH AddressAlignedBeats value */
    /* Set the FB bit according to ETH FixedBurst value */
    /* Set the RPBL and 4*PBL bits according to ETH RxDMABurstLength value */
    /* Set the PBL and 4*PBL bits according to ETH TxDMABurstLength value */
    /* Set the Enhanced DMA descriptors bit according to ETH EnhancedDescriptorFormat value*/
    /* Set the DSL bit according to ETH DesciptorSkipLength value */
    /* Set the PR and DA bits according to ETH DMAArbitration value */
    (heth->Instance)->DMABMR = (uint32_t)(dmainit.AddressAlignedBeats | 
                                          dmainit.FixedBurst |
                                          dmainit.RxDMABurstLength |    /* !! if 4xPBL is selected for Tx or Rx it is applied for the other */
                                          dmainit.TxDMABurstLength |
                                          dmainit.EnhancedDescriptorFormat |
                                          (dmainit.DescriptorSkipLength << 2) |
                                          dmainit.DMAArbitration |
                                          ETH_DMABMR_USP); /* Enable use of separate PBL for Rx and Tx */
     
     /* Wait until the write operation will be taken into account:
        at least four TX_CLK/RX_CLK clock cycles */
     tmpreg = (heth->Instance)->DMABMR;
     HAL_Delay(ETH_REG_WRITE_DELAY);
     (heth->Instance)->DMABMR = tmpreg;

     if((heth->Init).RxMode == ETH_RXINTERRUPT_MODE)
     {
       /* Enable the Ethernet Rx Interrupt */
       __HAL_ETH_DMA_ENABLE_IT((heth), ETH_DMA_IT_NIS | ETH_DMA_IT_R);
     }

     /* Initialize MAC address in ethernet MAC */ 
     ETH_MACAddressConfig(heth, ETH_MAC_ADDRESS0, heth->Init.MACAddr);
}

/**
  * @brief  Configures the selected MAC address.
  * @param  heth: ETH handle
  * @param  MacAddr: The MAC address to configure
  *          This parameter can be one of the following values:
  *             @arg ETH_MAC_Address0: MAC Address0 
  *             @arg ETH_MAC_Address1: MAC Address1 
  *             @arg ETH_MAC_Address2: MAC Address2
  *             @arg ETH_MAC_Address3: MAC Address3
  * @param  Addr: Pointer to MAC address buffer data (6 bytes)
  * @retval HAL status
  */
static void ETH_MACAddressConfig(ETH_HandleTypeDef *heth, uint32_t MacAddr, uint8_t *Addr)
{
  uint32_t tmpreg;
  
  /* Check the parameters */
  assert_param(IS_ETH_MAC_ADDRESS0123(MacAddr));
  
  /* Calculate the selected MAC address high register */
  tmpreg = ((uint32_t)Addr[5] << 8) | (uint32_t)Addr[4];
  /* Load the selected MAC address high register */
  (*(__IO uint32_t *)((uint32_t)(ETH_MAC_ADDR_HBASE + MacAddr))) = tmpreg;
  /* Calculate the selected MAC address low register */
  tmpreg = ((uint32_t)Addr[3] << 24) | ((uint32_t)Addr[2] << 16) | ((uint32_t)Addr[1] << 8) | Addr[0];
  
  /* Load the selected MAC address low register */
  (*(__IO uint32_t *)((uint32_t)(ETH_MAC_ADDR_LBASE + MacAddr))) = tmpreg;
}

/**
  * @brief  Enables the MAC transmission.
  * @param  heth: ETH handle  
  * @retval None
  */
static void ETH_MACTransmissionEnable(ETH_HandleTypeDef *heth)
{ 
  __IO uint32_t tmpreg = 0;
  
  /* Enable the MAC transmission */
  (heth->Instance)->MACCR |= ETH_MACCR_TE;
  
  /* Wait until the write operation will be taken into account:
     at least four TX_CLK/RX_CLK clock cycles */
  tmpreg = (heth->Instance)->MACCR;
  HAL_Delay(ETH_REG_WRITE_DELAY);
  (heth->Instance)->MACCR = tmpreg;
}

/**
  * @brief  Disables the MAC transmission.
  * @param  heth: ETH handle  
  * @retval None
  */
static void ETH_MACTransmissionDisable(ETH_HandleTypeDef *heth)
{ 
  __IO uint32_t tmpreg = 0;
  
  /* Disable the MAC transmission */
  (heth->Instance)->MACCR &= ~ETH_MACCR_TE;
  
  /* Wait until the write operation will be taken into account:
     at least four TX_CLK/RX_CLK clock cycles */
  tmpreg = (heth->Instance)->MACCR;
  HAL_Delay(ETH_REG_WRITE_DELAY);
  (heth->Instance)->MACCR = tmpreg;
}

/**
  * @brief  Enables the MAC reception.
  * @param  heth: ETH handle   
  * @retval None
  */
static void ETH_MACReceptionEnable(ETH_HandleTypeDef *heth)
{ 
  __IO uint32_t tmpreg = 0;
  
  /* Enable the MAC reception */
  (heth->Instance)->MACCR |= ETH_MACCR_RE;
  
  /* Wait until the write operation will be taken into account:
     at least four TX_CLK/RX_CLK clock cycles */
  tmpreg = (heth->Instance)->MACCR;
  HAL_Delay(ETH_REG_WRITE_DELAY);
  (heth->Instance)->MACCR = tmpreg;
}

/**
  * @brief  Disables the MAC reception.
  * @param  heth: ETH handle   
  * @retval None
  */
static void ETH_MACReceptionDisable(ETH_HandleTypeDef *heth)
{ 
  __IO uint32_t tmpreg = 0;
  
  /* Disable the MAC reception */
  (heth->Instance)->MACCR &= ~ETH_MACCR_RE; 
  
  /* Wait until the write operation will be taken into account:
     at least four TX_CLK/RX_CLK clock cycles */
  tmpreg = (heth->Instance)->MACCR;
  HAL_Delay(ETH_REG_WRITE_DELAY);
  (heth->Instance)->MACCR = tmpreg;
}

/**
  * @brief  Enables the DMA transmission.
  * @param  heth: ETH handle   
  * @retval None
  */
static void ETH_DMATransmissionEnable(ETH_HandleTypeDef *heth)
{
  /* Enable the DMA transmission */
  (heth->Instance)->DMAOMR |= ETH_DMAOMR_ST;  
}

/**
  * @brief  Disables the DMA transmission.
  * @param  heth: ETH handle   
  * @retval None
  */
static void ETH_DMATransmissionDisable(ETH_HandleTypeDef *heth)
{ 
  /* Disable the DMA transmission */
  (heth->Instance)->DMAOMR &= ~ETH_DMAOMR_ST;
}

/**
  * @brief  Enables the DMA reception.
  * @param  heth: ETH handle 
  * @retval None
  */
static void ETH_DMAReceptionEnable(ETH_HandleTypeDef *heth)
{  
  /* Enable the DMA reception */
  (heth->Instance)->DMAOMR |= ETH_DMAOMR_SR;  
}

/**
  * @brief  Disables the DMA reception.
  * @param  heth: ETH handle 
  * @retval None
  */
static void ETH_DMAReceptionDisable(ETH_HandleTypeDef *heth)
{ 
  /* Disable the DMA reception */
  (heth->Instance)->DMAOMR &= ~ETH_DMAOMR_SR;
}

/**
  * @brief  Clears the ETHERNET transmit FIFO.
  * @param  heth: ETH handle
  * @retval None
  */
static void ETH_FlushTransmitFIFO(ETH_HandleTypeDef *heth)
{
  __IO uint32_t tmpreg = 0;
  
  /* Set the Flush Transmit FIFO bit */
  (heth->Instance)->DMAOMR |= ETH_DMAOMR_FTF;
  
  /* Wait until the write operation will be taken into account:
     at least four TX_CLK/RX_CLK clock cycles */
  tmpreg = (heth->Instance)->DMAOMR;
  HAL_Delay(ETH_REG_WRITE_DELAY);
  (heth->Instance)->DMAOMR = tmpreg;
}

/**
  * @}
  */

#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */
#endif /* HAL_ETH_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

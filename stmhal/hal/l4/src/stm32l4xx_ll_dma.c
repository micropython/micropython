/**
  ******************************************************************************
  * @file    stm32l4xx_ll_dma.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   DMA LL module driver.
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
#if defined(USE_FULL_LL_DRIVER)

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_ll_dma.h"
#include "stm32l4xx_ll_bus.h"
#ifdef  USE_FULL_ASSERT
#include "stm32_assert.h"
#else
#define assert_param(expr) ((void)0)
#endif

/** @addtogroup STM32L4xx_LL_Driver
  * @{
  */

#if defined (DMA1) || defined (DMA2)

/** @defgroup DMA_LL DMA
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @addtogroup DMA_LL_Private_Macros
  * @{
  */
#define IS_LL_DMA_DIRECTION(__VALUE__)          (((__VALUE__) == LL_DMA_DIRECTION_PERIPH_TO_MEMORY) || \
                                                 ((__VALUE__) == LL_DMA_DIRECTION_MEMORY_TO_PERIPH) || \
                                                 ((__VALUE__) == LL_DMA_DIRECTION_MEMORY_TO_MEMORY))

#define IS_LL_DMA_MODE(__VALUE__)               (((__VALUE__) == LL_DMA_MODE_NORMAL) || \
                                                 ((__VALUE__) == LL_DMA_MODE_CIRCULAR))

#define IS_LL_DMA_PERIPHINCMODE(__VALUE__)      (((__VALUE__) == LL_DMA_PERIPH_INCREMENT) || \
                                                 ((__VALUE__) == LL_DMA_PERIPH_NOINCREMENT))

#define IS_LL_DMA_MEMORYINCMODE(__VALUE__)      (((__VALUE__) == LL_DMA_MEMORY_INCREMENT) || \
                                                 ((__VALUE__) == LL_DMA_MEMORY_NOINCREMENT))

#define IS_LL_DMA_PERIPHDATASIZE(__VALUE__)     (((__VALUE__) == LL_DMA_PDATAALIGN_BYTE)      || \
                                                 ((__VALUE__) == LL_DMA_PDATAALIGN_HALFWORD)  || \
                                                 ((__VALUE__) == LL_DMA_PDATAALIGN_WORD))

#define IS_LL_DMA_MEMORYDATASIZE(__VALUE__)     (((__VALUE__) == LL_DMA_MDATAALIGN_BYTE)      || \
                                                 ((__VALUE__) == LL_DMA_MDATAALIGN_HALFWORD)  || \
                                                 ((__VALUE__) == LL_DMA_MDATAALIGN_WORD))

#define IS_LL_DMA_NBDATA(__VALUE__)             ((__VALUE__)  <= (uint32_t)0x0000FFFFU)

#define IS_LL_DMA_PERIPHREQUEST(__VALUE__)      (((__VALUE__) == LL_DMA_REQUEST_0)  || \
                                                 ((__VALUE__) == LL_DMA_REQUEST_1)  || \
                                                 ((__VALUE__) == LL_DMA_REQUEST_2)  || \
                                                 ((__VALUE__) == LL_DMA_REQUEST_3)  || \
                                                 ((__VALUE__) == LL_DMA_REQUEST_4)  || \
                                                 ((__VALUE__) == LL_DMA_REQUEST_5)  || \
                                                 ((__VALUE__) == LL_DMA_REQUEST_6)  || \
                                                 ((__VALUE__) == LL_DMA_REQUEST_7))

#define IS_LL_DMA_PRIORITY(__VALUE__)           (((__VALUE__) == LL_DMA_PRIORITY_LOW)    || \
                                                 ((__VALUE__) == LL_DMA_PRIORITY_MEDIUM) || \
                                                 ((__VALUE__) == LL_DMA_PRIORITY_HIGH)   || \
                                                 ((__VALUE__) == LL_DMA_PRIORITY_VERYHIGH))

#define IS_LL_DMA_ALL_CHANNEL_INSTANCE(INSTANCE, CHANNEL)  ((((INSTANCE) == DMA1) && \
                                                         (((CHANNEL) == LL_DMA_CHANNEL_1) || \
                                                          ((CHANNEL) == LL_DMA_CHANNEL_2) || \
                                                          ((CHANNEL) == LL_DMA_CHANNEL_3) || \
                                                          ((CHANNEL) == LL_DMA_CHANNEL_4) || \
                                                          ((CHANNEL) == LL_DMA_CHANNEL_5) || \
                                                          ((CHANNEL) == LL_DMA_CHANNEL_6) || \
                                                          ((CHANNEL) == LL_DMA_CHANNEL_7))) || \
                                                         (((INSTANCE) == DMA2) && \
                                                         (((CHANNEL) == LL_DMA_CHANNEL_1) || \
                                                          ((CHANNEL) == LL_DMA_CHANNEL_2) || \
                                                          ((CHANNEL) == LL_DMA_CHANNEL_3) || \
                                                          ((CHANNEL) == LL_DMA_CHANNEL_4) || \
                                                          ((CHANNEL) == LL_DMA_CHANNEL_5) || \
                                                          ((CHANNEL) == LL_DMA_CHANNEL_6) || \
                                                          ((CHANNEL) == LL_DMA_CHANNEL_7))))
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @addtogroup DMA_LL_Exported_Functions
  * @{
  */

/** @addtogroup DMA_LL_EF_Init
  * @{
  */

/**
  * @brief  De-initialize the DMA registers to their default reset values.
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: DMA registers are de-initialized
  *          - ERROR: DMA registers are not de-initialized
  */
uint32_t LL_DMA_DeInit(DMA_TypeDef *DMAx, uint32_t Channel)
{
  DMA_Channel_TypeDef *tmp = (DMA_Channel_TypeDef *)DMA1_Channel1;
  ErrorStatus status = SUCCESS;

  /* Check the DMA Instance DMAx and Channel parameters*/
  assert_param(IS_LL_DMA_ALL_CHANNEL_INSTANCE(DMAx, Channel) || (Channel == LL_DMA_CHANNEL_ALL));

  if (Channel == LL_DMA_CHANNEL_ALL)
  {
    if (DMAx == DMA1)
    {
      /* Force reset of DMA clock */
      LL_AHB1_GRP1_ForceReset(LL_AHB1_GRP1_PERIPH_DMA1);

      /* Release reset of DMA clock */
      LL_AHB1_GRP1_ReleaseReset(LL_AHB1_GRP1_PERIPH_DMA1);
    }
#if defined(DMA2)
    else if (DMAx == DMA2)
    {
      /* Force reset of DMA clock */
      LL_AHB1_GRP1_ForceReset(LL_AHB1_GRP1_PERIPH_DMA2);

      /* Release reset of DMA clock */
      LL_AHB1_GRP1_ReleaseReset(LL_AHB1_GRP1_PERIPH_DMA2);
    }
#endif
    else
    {
      status = ERROR;
    }
  }
  else
  {
    tmp = (DMA_Channel_TypeDef *)(__LL_DMA_GET_CHANNEL_INSTANCE(DMAx, Channel));

    /* Disable the selected DMAx_Channely */
    CLEAR_BIT(tmp->CCR, DMA_CCR_EN);

    /* Reset DMAx_Channely control register */
    LL_DMA_WriteReg(tmp, CCR, 0U);

    /* Reset DMAx_Channely remaining bytes register */
    LL_DMA_WriteReg(tmp, CNDTR, 0U);

    /* Reset DMAx_Channely peripheral address register */
    LL_DMA_WriteReg(tmp, CPAR, 0U);

    /* Reset DMAx_Channely memory address register */
    LL_DMA_WriteReg(tmp, CMAR, 0U);

    if (Channel == LL_DMA_CHANNEL_1)
    {
      /* Reset interrupt pending bits for DMAx Channel1 */
      LL_DMA_ClearFlag_GI1(DMAx);

      /* Reset Request register field for DMAx Channel1 */
      LL_DMA_SetPeriphRequest(DMAx, Channel, LL_DMA_REQUEST_0);
    }
    else if (Channel == LL_DMA_CHANNEL_2)
    {
      /* Reset interrupt pending bits for DMAx Channel2 */
      LL_DMA_ClearFlag_GI2(DMAx);

      /* Reset Request register field for DMAx Channel2 */
      LL_DMA_SetPeriphRequest(DMAx, Channel, LL_DMA_REQUEST_0);
    }
    else if (Channel == LL_DMA_CHANNEL_3)
    {
      /* Reset interrupt pending bits for DMAx Channel3 */
      LL_DMA_ClearFlag_GI3(DMAx);

      /* Reset Request register field for DMAx Channel3 */
      LL_DMA_SetPeriphRequest(DMAx, Channel, LL_DMA_REQUEST_0);
    }
    else if (Channel == LL_DMA_CHANNEL_4)
    {
      /* Reset interrupt pending bits for DMAx Channel4 */
      LL_DMA_ClearFlag_GI4(DMAx);


      /* Reset Request register field for DMAx Channel4 */
      LL_DMA_SetPeriphRequest(DMAx, Channel, LL_DMA_REQUEST_0);
    }
    else if (Channel == LL_DMA_CHANNEL_5)
    {
      /* Reset interrupt pending bits for DMAx Channel5 */
      LL_DMA_ClearFlag_GI5(DMAx);

      /* Reset Request register field for DMAx Channel5 */
      LL_DMA_SetPeriphRequest(DMAx, Channel, LL_DMA_REQUEST_0);
    }

    else if (Channel == LL_DMA_CHANNEL_6)
    {
      /* Reset interrupt pending bits for DMAx Channel6 */
      LL_DMA_ClearFlag_GI6(DMAx);

      /* Reset Request register field for DMAx Channel6 */
      LL_DMA_SetPeriphRequest(DMAx, Channel, LL_DMA_REQUEST_0);
    }
    else if (Channel == LL_DMA_CHANNEL_7)
    {
      /* Reset interrupt pending bits for DMAx Channel7 */
      LL_DMA_ClearFlag_GI7(DMAx);

      /* Reset Request register field for DMAx Channel7 */
      LL_DMA_SetPeriphRequest(DMAx, Channel, LL_DMA_REQUEST_0);
    }
    else
    {
      status = ERROR;
    }
  }

  return status;
}

/**
  * @brief  Initialize the DMA registers according to the specified parameters in DMA_InitStruct.
  * @note   To convert DMAx_Channely Instance to DMAx Instance and Channely, use helper macros :
  *         @arg @ref __LL_DMA_GET_INSTANCE
  *         @arg @ref __LL_DMA_GET_CHANNEL
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  * @param  DMA_InitStruct pointer to a @ref LL_DMA_InitTypeDef structure.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: DMA registers are initialized
  *          - ERROR: Not applicable
  */
uint32_t LL_DMA_Init(DMA_TypeDef *DMAx, uint32_t Channel, LL_DMA_InitTypeDef *DMA_InitStruct)
{
  /* Check the DMA Instance DMAx and Channel parameters*/
  assert_param(IS_LL_DMA_ALL_CHANNEL_INSTANCE(DMAx, Channel));

  /* Check the DMA parameters from DMA_InitStruct */
  assert_param(IS_LL_DMA_DIRECTION(DMA_InitStruct->Direction));
  assert_param(IS_LL_DMA_MODE(DMA_InitStruct->Mode));
  assert_param(IS_LL_DMA_PERIPHINCMODE(DMA_InitStruct->PeriphOrM2MSrcIncMode));
  assert_param(IS_LL_DMA_MEMORYINCMODE(DMA_InitStruct->MemoryOrM2MDstIncMode));
  assert_param(IS_LL_DMA_PERIPHDATASIZE(DMA_InitStruct->PeriphOrM2MSrcDataSize));
  assert_param(IS_LL_DMA_MEMORYDATASIZE(DMA_InitStruct->MemoryOrM2MDstDataSize));
  assert_param(IS_LL_DMA_NBDATA(DMA_InitStruct->NbData));
  assert_param(IS_LL_DMA_PERIPHREQUEST(DMA_InitStruct->PeriphRequest));
  assert_param(IS_LL_DMA_PRIORITY(DMA_InitStruct->Priority));

  /*---------------------------- DMAx CCR Configuration ------------------------
   * Configure DMAx_Channely: data transfer direction, data transfer mode,
   *                          peripheral and memory increment mode,
   *                          data size alignment and  priority level with parameters :
   * - Direction:      DMA_CCR_DIR and DMA_CCR_MEM2MEM bits
   * - Mode:           DMA_CCR_CIRC bit
   * - PeriphOrM2MSrcIncMode:  DMA_CCR_PINC bit
   * - MemoryOrM2MDstIncMode:  DMA_CCR_MINC bit
   * - PeriphOrM2MSrcDataSize: DMA_CCR_PSIZE[1:0] bits
   * - MemoryOrM2MDstDataSize: DMA_CCR_MSIZE[1:0] bits
   * - Priority:               DMA_CCR_PL[1:0] bits
   */
  LL_DMA_ConfigTransfer(DMAx, Channel, DMA_InitStruct->Direction              | \
                        DMA_InitStruct->Mode                   | \
                        DMA_InitStruct->PeriphOrM2MSrcIncMode  | \
                        DMA_InitStruct->MemoryOrM2MDstIncMode  | \
                        DMA_InitStruct->PeriphOrM2MSrcDataSize | \
                        DMA_InitStruct->MemoryOrM2MDstDataSize | \
                        DMA_InitStruct->Priority);

  /*-------------------------- DMAx CMAR Configuration -------------------------
   * Configure the memory or destination base address with parameter :
   * - MemoryOrM2MDstAddress: DMA_CMAR_MA[31:0] bits
   */
  LL_DMA_SetMemoryAddress(DMAx, Channel, DMA_InitStruct->MemoryOrM2MDstAddress);

  /*-------------------------- DMAx CPAR Configuration -------------------------
   * Configure the peripheral or source base address with parameter :
   * - PeriphOrM2MSrcAddress: DMA_CPAR_PA[31:0] bits
   */
  LL_DMA_SetPeriphAddress(DMAx, Channel, DMA_InitStruct->PeriphOrM2MSrcAddress);
  
  /*--------------------------- DMAx CNDTR Configuration -----------------------
   * Configure the peripheral base address with parameter :
   * - NbData: DMA_CNDTR_NDT[15:0] bits
   */
  LL_DMA_SetDataLength(DMAx, Channel, DMA_InitStruct->NbData);

  /*--------------------------- DMAx CSELR Configuration -----------------------
   * Configure the peripheral base address with parameter :
   * - PeriphRequest: DMA_CSELR[31:0] bits
   */
  LL_DMA_SetPeriphRequest(DMAx, Channel, DMA_InitStruct->PeriphRequest);

  return SUCCESS;
}

/**
  * @brief  Set each @ref LL_DMA_InitTypeDef field to default value.
  * @param  DMA_InitStruct Pointer to a @ref LL_DMA_InitTypeDef structure.
  * @retval None
  */
void LL_DMA_StructInit(LL_DMA_InitTypeDef *DMA_InitStruct)
{
  /* Set DMA_InitStruct fields to default values */
  DMA_InitStruct->PeriphOrM2MSrcAddress  = (uint32_t)0x00000000U;
  DMA_InitStruct->MemoryOrM2MDstAddress  = (uint32_t)0x00000000U;
  DMA_InitStruct->Direction              = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
  DMA_InitStruct->Mode                   = LL_DMA_MODE_NORMAL;
  DMA_InitStruct->PeriphOrM2MSrcIncMode  = LL_DMA_PERIPH_NOINCREMENT;
  DMA_InitStruct->MemoryOrM2MDstIncMode  = LL_DMA_MEMORY_NOINCREMENT;
  DMA_InitStruct->PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
  DMA_InitStruct->MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
  DMA_InitStruct->NbData                 = (uint32_t)0x00000000U;
  DMA_InitStruct->PeriphRequest          = LL_DMA_REQUEST_0;
  DMA_InitStruct->Priority               = LL_DMA_PRIORITY_LOW;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* DMA1 || DMA2 */

/**
  * @}
  */

#endif /* USE_FULL_LL_DRIVER */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

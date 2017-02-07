/**
  ******************************************************************************
  * @file    stm32f7xx_hal_i2c.c
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    23-September-2016
  * @brief   I2C HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Inter Integrated Circuit (I2C) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral State and Errors functions
  *
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
    [..]
    The I2C HAL driver can be used as follows:

    (#) Declare a I2C_HandleTypeDef handle structure, for example:
        I2C_HandleTypeDef  hi2c;

    (#)Initialize the I2C low level resources by implementing the HAL_I2C_MspInit() API:
        (##) Enable the I2Cx interface clock
        (##) I2C pins configuration
            (+++) Enable the clock for the I2C GPIOs
            (+++) Configure I2C pins as alternate function open-drain
        (##) NVIC configuration if you need to use interrupt process
            (+++) Configure the I2Cx interrupt priority
            (+++) Enable the NVIC I2C IRQ Channel
        (##) DMA Configuration if you need to use DMA process
            (+++) Declare a DMA_HandleTypeDef handle structure for the transmit or receive stream
            (+++) Enable the DMAx interface clock using
            (+++) Configure the DMA handle parameters
            (+++) Configure the DMA Tx or Rx stream
            (+++) Associate the initialized DMA handle to the hi2c DMA Tx or Rx handle
            (+++) Configure the priority and enable the NVIC for the transfer complete interrupt on
                  the DMA Tx or Rx stream

    (#) Configure the Communication Clock Timing, Own Address1, Master Addressing mode, Dual Addressing mode,
        Own Address2, Own Address2 Mask, General call and Nostretch mode in the hi2c Init structure.

    (#) Initialize the I2C registers by calling the HAL_I2C_Init(), configures also the low level Hardware
        (GPIO, CLOCK, NVIC...etc) by calling the customized HAL_I2C_MspInit(&hi2c) API.

    (#) To check if target device is ready for communication, use the function HAL_I2C_IsDeviceReady()

    (#) For I2C IO and IO MEM operations, three operation modes are available within this driver :

    *** Polling mode IO operation ***
    =================================
    [..]
      (+) Transmit in master mode an amount of data in blocking mode using HAL_I2C_Master_Transmit()
      (+) Receive in master mode an amount of data in blocking mode using HAL_I2C_Master_Receive()
      (+) Transmit in slave mode an amount of data in blocking mode using HAL_I2C_Slave_Transmit()
      (+) Receive in slave mode an amount of data in blocking mode using HAL_I2C_Slave_Receive()

    *** Polling mode IO MEM operation ***
    =====================================
    [..]
      (+) Write an amount of data in blocking mode to a specific memory address using HAL_I2C_Mem_Write()
      (+) Read an amount of data in blocking mode from a specific memory address using HAL_I2C_Mem_Read()


    *** Interrupt mode IO operation ***
    ===================================
    [..]
      (+) Transmit in master mode an amount of data in non-blocking mode using HAL_I2C_Master_Transmit_IT()
      (+) At transmission end of transfer, HAL_I2C_MasterTxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_MasterTxCpltCallback()
      (+) Receive in master mode an amount of data in non-blocking mode using HAL_I2C_Master_Receive_IT()
      (+) At reception end of transfer, HAL_I2C_MasterRxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_MasterRxCpltCallback()
      (+) Transmit in slave mode an amount of data in non-blocking mode using HAL_I2C_Slave_Transmit_IT()
      (+) At transmission end of transfer, HAL_I2C_SlaveTxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_SlaveTxCpltCallback()
      (+) Receive in slave mode an amount of data in non-blocking mode using HAL_I2C_Slave_Receive_IT()
      (+) At reception end of transfer, HAL_I2C_SlaveRxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_SlaveRxCpltCallback()
      (+) In case of transfer Error, HAL_I2C_ErrorCallback() function is executed and user can
           add his own code by customization of function pointer HAL_I2C_ErrorCallback()
      (+) Abort a master I2C process communication with Interrupt using HAL_I2C_Master_Abort_IT()
      (+) End of abort process, HAL_I2C_MasterRxCpltCallback() or HAL_I2C_MasterTxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_MasterRxCpltCallback() or HAL_I2C_MasterTxCpltCallback()
      (+) Discard a slave I2C process communication using __HAL_I2C_GENERATE_NACK() macro.
           This action will inform Master to generate a Stop condition to discard the communication.


    *** Interrupt mode IO sequential operation ***
    ===================================
    [..]
      (@) These interfaces allow to manage a sequential transfer with a repeated start condition
          when a direction change during transfer
    [..]
      (+) A specific option field manage the different steps of a sequential transfer
      (+) Option field values are defined through I2C_XFEROPTIONS and are listed below:
      (++) I2C_FIRST_AND_LAST_FRAME: No sequential usage, functionnal is same as associated interfaces in no sequential mode
      (++) I2C_FIRST_FRAME: Sequential usage, this option allow to manage a sequence with start condition, address
                            and data to transfer without a final stop condition
	  (++) I2C_FIRST_AND_NEXT_FRAME: Sequential usage (Master only), this option allow to manage a sequence with start condition, address
                            and data to transfer without a final stop condition, an then permit a call the same master sequential interface
                            several times (like HAL_I2C_Master_Sequential_Transmit_IT() then HAL_I2C_Master_Sequential_Transmit_IT())
      (++) I2C_NEXT_FRAME: Sequential usage, this option allow to manage a sequence with a restart condition, address
                            and with new data to transfer if the direction change or manage only the new data to transfer
                            if no direction change and without a final stop condition in both cases
      (++) I2C_LAST_FRAME: Sequential usage, this option allow to manage a sequance with a restart condition, address
                            and with new data to transfer if the direction change or manage only the new data to transfer
                            if no direction change and with a final stop condition in both cases

      (+) Differents sequential I2C interfaces are listed below:
      (++) Sequential transmit in master I2C mode an amount of data in non-blocking mode using HAL_I2C_Master_Sequential_Transmit_IT()
      (+++) At transmission end of current frame transfer, HAL_I2C_MasterTxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_MasterTxCpltCallback()
      (++) Sequential receive in master I2C mode an amount of data in non-blocking mode using HAL_I2C_Master_Sequential_Receive_IT()
      (+++) At reception end of current frame transfer, HAL_I2C_MasterRxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_MasterRxCpltCallback()
      (++) Abort a master I2C process communication with Interrupt using HAL_I2C_Master_Abort_IT()
      (+++) End of abort process, HAL_I2C_AbortCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_AbortCpltCallback()
      (+++) mean HAL_I2C_MasterTxCpltCallback() in case of previous state was master transmit
      (+++) mean HAL_I2c_MasterRxCpltCallback() in case of previous state was master receive
      (++) Enable/disable the Address listen mode in slave I2C mode using HAL_I2C_EnableListen_IT() HAL_I2C_DisableListen_IT()
      (+++) When address slave I2C match, HAL_I2C_AddrCallback() is executed and user can
           add his own code to check the Address Match Code and the transmission direction request by master (Write/Read).
      (+++) At Listen mode end HAL_I2C_ListenCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_ListenCpltCallback()
      (++) Sequential transmit in slave I2C mode an amount of data in non-blocking mode using HAL_I2C_Slave_Sequential_Transmit_IT()
      (+++) At transmission end of current frame transfer, HAL_I2C_SlaveTxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_SlaveTxCpltCallback()
      (++) Sequential receive in slave I2C mode an amount of data in non-blocking mode using HAL_I2C_Slave_Sequential_Receive_IT()
      (+++) At reception end of current frame transfer, HAL_I2C_SlaveRxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_SlaveRxCpltCallback()
      (++) In case of transfer Error, HAL_I2C_ErrorCallback() function is executed and user can
           add his own code by customization of function pointer HAL_I2C_ErrorCallback()
      (++) Abort a master I2C process communication with Interrupt using HAL_I2C_Master_Abort_IT()
      (++) End of abort process, HAL_I2C_AbortCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_AbortCpltCallback()
      (++) Discard a slave I2C process communication using __HAL_I2C_GENERATE_NACK() macro.
           This action will inform Master to generate a Stop condition to discard the communication.

    *** Interrupt mode IO MEM operation ***
    =======================================
    [..]
      (+) Write an amount of data in non-blocking mode with Interrupt to a specific memory address using
          HAL_I2C_Mem_Write_IT()
      (+) At Memory end of write transfer, HAL_I2C_MemTxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_MemTxCpltCallback()
      (+) Read an amount of data in non-blocking mode with Interrupt from a specific memory address using
          HAL_I2C_Mem_Read_IT()
      (+) At Memory end of read transfer, HAL_I2C_MemRxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_MemRxCpltCallback()
      (+) In case of transfer Error, HAL_I2C_ErrorCallback() function is executed and user can
           add his own code by customization of function pointer HAL_I2C_ErrorCallback()

    *** DMA mode IO operation ***
    ==============================
    [..]
      (+) Transmit in master mode an amount of data in non-blocking mode (DMA) using
          HAL_I2C_Master_Transmit_DMA()
      (+) At transmission end of transfer, HAL_I2C_MasterTxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_MasterTxCpltCallback()
      (+) Receive in master mode an amount of data in non-blocking mode (DMA) using
          HAL_I2C_Master_Receive_DMA()
      (+) At reception end of transfer, HAL_I2C_MasterRxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_MasterRxCpltCallback()
      (+) Transmit in slave mode an amount of data in non-blocking mode (DMA) using
          HAL_I2C_Slave_Transmit_DMA()
      (+) At transmission end of transfer, HAL_I2C_SlaveTxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_SlaveTxCpltCallback()
      (+) Receive in slave mode an amount of data in non-blocking mode (DMA) using
          HAL_I2C_Slave_Receive_DMA()
      (+) At reception end of transfer, HAL_I2C_SlaveRxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_SlaveRxCpltCallback()
      (+) In case of transfer Error, HAL_I2C_ErrorCallback() function is executed and user can
           add his own code by customization of function pointer HAL_I2C_ErrorCallback()
      (+) Abort a master I2C process communication with Interrupt using HAL_I2C_Master_Abort_IT()
      (+) End of abort process, HAL_I2C_MasterRxCpltCallback() or HAL_I2C_MasterTxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_MasterRxCpltCallback() or HAL_I2C_MasterTxCpltCallback()
      (+) Discard a slave I2C process communication using __HAL_I2C_GENERATE_NACK() macro.
           This action will inform Master to generate a Stop condition to discard the communication.

    *** DMA mode IO MEM operation ***
    =================================
    [..]
      (+) Write an amount of data in non-blocking mode with DMA to a specific memory address using
          HAL_I2C_Mem_Write_DMA()
      (+) At Memory end of write transfer, HAL_I2C_MemTxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_MemTxCpltCallback()
      (+) Read an amount of data in non-blocking mode with DMA from a specific memory address using
          HAL_I2C_Mem_Read_DMA()
      (+) At Memory end of read transfer, HAL_I2C_MemRxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_I2C_MemRxCpltCallback()
      (+) In case of transfer Error, HAL_I2C_ErrorCallback() function is executed and user can
           add his own code by customization of function pointer HAL_I2C_ErrorCallback()


     *** I2C HAL driver macros list ***
     ==================================
     [..]
       Below the list of most used macros in I2C HAL driver.

      (+) __HAL_I2C_ENABLE: Enable the I2C peripheral
      (+) __HAL_I2C_DISABLE: Disable the I2C peripheral
      (+) __HAL_I2C_GENERATE_NACK: Generate a Non-Acknowledge I2C peripheral in Slave mode
      (+) __HAL_I2C_GET_FLAG: Check whether the specified I2C flag is set or not
      (+) __HAL_I2C_CLEAR_FLAG: Clear the specified I2C pending flag
      (+) __HAL_I2C_ENABLE_IT: Enable the specified I2C interrupt
      (+) __HAL_I2C_DISABLE_IT: Disable the specified I2C interrupt

     [..]
       (@) You can refer to the I2C HAL driver header file for more useful macros

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

/** @defgroup I2C I2C
  * @brief I2C HAL module driver
  * @{
  */

#ifdef HAL_I2C_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/** @defgroup I2C_Private_Define I2C Private Define
  * @{
  */
#define TIMING_CLEAR_MASK   ((uint32_t)0xF0FFFFFFU)  /*!<  I2C TIMING clear register Mask */
#define I2C_TIMEOUT_ADDR    ((uint32_t)10000U)       /*!< 10 s  */
#define I2C_TIMEOUT_BUSY    ((uint32_t)25U)          /*!< 25 ms */
#define I2C_TIMEOUT_DIR     ((uint32_t)25U)          /*!< 25 ms */
#define I2C_TIMEOUT_RXNE    ((uint32_t)25U)          /*!< 25 ms */
#define I2C_TIMEOUT_STOPF   ((uint32_t)25U)          /*!< 25 ms */
#define I2C_TIMEOUT_TC      ((uint32_t)25U)          /*!< 25 ms */
#define I2C_TIMEOUT_TCR     ((uint32_t)25U)          /*!< 25 ms */
#define I2C_TIMEOUT_TXIS    ((uint32_t)25U)          /*!< 25 ms */
#define I2C_TIMEOUT_FLAG    ((uint32_t)25U)          /*!< 25 ms */

#define MAX_NBYTE_SIZE      255U
#define SlaveAddr_SHIFT     7U
#define SlaveAddr_MSK       0x06U

/* Private define for @ref PreviousState usage */
#define I2C_STATE_MSK             ((uint32_t)((HAL_I2C_STATE_BUSY_TX | HAL_I2C_STATE_BUSY_RX) & (~((uint32_t)HAL_I2C_STATE_READY)))) /*!< Mask State define, keep only RX and TX bits            */
#define I2C_STATE_NONE            ((uint32_t)(HAL_I2C_MODE_NONE))                                                        /*!< Default Value                                          */
#define I2C_STATE_MASTER_BUSY_TX  ((uint32_t)((HAL_I2C_STATE_BUSY_TX & I2C_STATE_MSK) | HAL_I2C_MODE_MASTER))            /*!< Master Busy TX, combinaison of State LSB and Mode enum */
#define I2C_STATE_MASTER_BUSY_RX  ((uint32_t)((HAL_I2C_STATE_BUSY_RX & I2C_STATE_MSK) | HAL_I2C_MODE_MASTER))            /*!< Master Busy RX, combinaison of State LSB and Mode enum */
#define I2C_STATE_SLAVE_BUSY_TX   ((uint32_t)((HAL_I2C_STATE_BUSY_TX & I2C_STATE_MSK) | HAL_I2C_MODE_SLAVE))             /*!< Slave Busy TX, combinaison of State LSB and Mode enum  */
#define I2C_STATE_SLAVE_BUSY_RX   ((uint32_t)((HAL_I2C_STATE_BUSY_RX & I2C_STATE_MSK) | HAL_I2C_MODE_SLAVE))             /*!< Slave Busy RX, combinaison of State LSB and Mode enum  */
#define I2C_STATE_MEM_BUSY_TX     ((uint32_t)((HAL_I2C_STATE_BUSY_TX & I2C_STATE_MSK) | HAL_I2C_MODE_MEM))               /*!< Memory Busy TX, combinaison of State LSB and Mode enum */
#define I2C_STATE_MEM_BUSY_RX     ((uint32_t)((HAL_I2C_STATE_BUSY_RX & I2C_STATE_MSK) | HAL_I2C_MODE_MEM))               /*!< Memory Busy RX, combinaison of State LSB and Mode enum */

/* Private define to centralize the enable/disable of Interrupts */
#define I2C_XFER_TX_IT          ((uint32_t)0x00000001U)
#define I2C_XFER_RX_IT          ((uint32_t)0x00000002U)
#define I2C_XFER_LISTEN_IT      ((uint32_t)0x00000004U)

#define I2C_XFER_ERROR_IT       ((uint32_t)0x00000011U)
#define I2C_XFER_CPLT_IT        ((uint32_t)0x00000012U)
#define I2C_XFER_RELOAD_IT      ((uint32_t)0x00000012U)

/* Private define Sequential Transfer Options default/reset value */
#define I2C_NO_OPTION_FRAME     ((uint32_t)0xFFFF0000U)
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
#define I2C_GET_DMA_REMAIN_DATA(__HANDLE__) ((((__HANDLE__)->State) == HAL_I2C_STATE_BUSY_TX)   ? \
                                            ((uint32_t)((__HANDLE__)->hdmatx->Instance->NDTR)) : \
                                            ((uint32_t)((__HANDLE__)->hdmarx->Instance->NDTR)))

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/** @defgroup I2C_Private_Functions I2C Private Functions
  * @{
  */
/* Private functions to handle DMA transfer */
static void I2C_DMAMasterTransmitCplt(DMA_HandleTypeDef *hdma);
static void I2C_DMAMasterReceiveCplt(DMA_HandleTypeDef *hdma);
static void I2C_DMASlaveTransmitCplt(DMA_HandleTypeDef *hdma);
static void I2C_DMASlaveReceiveCplt(DMA_HandleTypeDef *hdma);
static void I2C_DMAError(DMA_HandleTypeDef *hdma);
static void I2C_DMAAbort(DMA_HandleTypeDef *hdma);

/* Private functions to handle IT transfer */
static void I2C_ITAddrCplt(I2C_HandleTypeDef *hi2c, uint32_t ITFlags);
static void I2C_ITMasterSequentialCplt(I2C_HandleTypeDef *hi2c);
static void I2C_ITSlaveSequentialCplt(I2C_HandleTypeDef *hi2c);
static void I2C_ITMasterCplt(I2C_HandleTypeDef *hi2c, uint32_t ITFlags);
static void I2C_ITSlaveCplt(I2C_HandleTypeDef *hi2c, uint32_t ITFlags);
static void I2C_ITListenCplt(I2C_HandleTypeDef *hi2c, uint32_t ITFlags);
static void I2C_ITError(I2C_HandleTypeDef *hi2c, uint32_t ErrorCode);

/* Private functions to handle IT transfer */
static HAL_StatusTypeDef I2C_RequestMemoryWrite(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef I2C_RequestMemoryRead(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout, uint32_t Tickstart);

/* Private functions for I2C transfer IRQ handler */
static HAL_StatusTypeDef I2C_Master_ISR_IT(struct __I2C_HandleTypeDef *hi2c, uint32_t ITFlags, uint32_t ITSources);
static HAL_StatusTypeDef I2C_Slave_ISR_IT(struct __I2C_HandleTypeDef *hi2c, uint32_t ITFlags, uint32_t ITSources);
static HAL_StatusTypeDef I2C_Master_ISR_DMA(struct __I2C_HandleTypeDef *hi2c, uint32_t ITFlags, uint32_t ITSources);
static HAL_StatusTypeDef I2C_Slave_ISR_DMA(struct __I2C_HandleTypeDef *hi2c, uint32_t ITFlags, uint32_t ITSources);

/* Private functions to handle flags during polling transfer */
static HAL_StatusTypeDef I2C_WaitOnFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, FlagStatus Status, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef I2C_WaitOnTXISFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef I2C_WaitOnRXNEFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef I2C_WaitOnSTOPFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef I2C_IsAcknowledgeFailed(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart);

/* Private functions to centralize the enable/disable of Interrupts */
static HAL_StatusTypeDef I2C_Enable_IRQ(I2C_HandleTypeDef *hi2c, uint16_t InterruptRequest);
static HAL_StatusTypeDef I2C_Disable_IRQ(I2C_HandleTypeDef *hi2c, uint16_t InterruptRequest);

/* Private functions to flush TXDR register */
static void I2C_Flush_TXDR(I2C_HandleTypeDef *hi2c);

/* Private functions to handle  start, restart or stop a transfer */
static void I2C_TransferConfig(I2C_HandleTypeDef *hi2c,  uint16_t DevAddress, uint8_t Size, uint32_t Mode, uint32_t Request);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @defgroup I2C_Exported_Functions I2C Exported Functions
  * @{
  */

/** @defgroup I2C_Exported_Functions_Group1 Initialization and de-initialization functions
 *  @brief    Initialization and Configuration functions
 *
@verbatim
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This subsection provides a set of functions allowing to initialize and
          deinitialize the I2Cx peripheral:

      (+) User must Implement HAL_I2C_MspInit() function in which he configures
          all related peripherals resources (CLOCK, GPIO, DMA, IT and NVIC ).

      (+) Call the function HAL_I2C_Init() to configure the selected device with
          the selected configuration:
        (++) Clock Timing
        (++) Own Address 1
        (++) Addressing mode (Master, Slave)
        (++) Dual Addressing mode
        (++) Own Address 2
        (++) Own Address 2 Mask
        (++) General call mode
        (++) Nostretch mode

      (+) Call the function HAL_I2C_DeInit() to restore the default configuration
          of the selected I2Cx peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the I2C according to the specified parameters
  *         in the I2C_InitTypeDef and initialize the associated handle.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Init(I2C_HandleTypeDef *hi2c)
{
  /* Check the I2C handle allocation */
  if(hi2c == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_I2C_ALL_INSTANCE(hi2c->Instance));
  assert_param(IS_I2C_OWN_ADDRESS1(hi2c->Init.OwnAddress1));
  assert_param(IS_I2C_ADDRESSING_MODE(hi2c->Init.AddressingMode));
  assert_param(IS_I2C_DUAL_ADDRESS(hi2c->Init.DualAddressMode));
  assert_param(IS_I2C_OWN_ADDRESS2(hi2c->Init.OwnAddress2));
  assert_param(IS_I2C_OWN_ADDRESS2_MASK(hi2c->Init.OwnAddress2Masks));
  assert_param(IS_I2C_GENERAL_CALL(hi2c->Init.GeneralCallMode));
  assert_param(IS_I2C_NO_STRETCH(hi2c->Init.NoStretchMode));

  if(hi2c->State == HAL_I2C_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hi2c->Lock = HAL_UNLOCKED;

    /* Init the low level hardware : GPIO, CLOCK, CORTEX...etc */
    HAL_I2C_MspInit(hi2c);
  }

  hi2c->State = HAL_I2C_STATE_BUSY;

  /* Disable the selected I2C peripheral */
  __HAL_I2C_DISABLE(hi2c);

  /*---------------------------- I2Cx TIMINGR Configuration ------------------*/
  /* Configure I2Cx: Frequency range */
  hi2c->Instance->TIMINGR = hi2c->Init.Timing & TIMING_CLEAR_MASK;

  /*---------------------------- I2Cx OAR1 Configuration ---------------------*/
  /* Disable Own Address1 before set the Own Address1 configuration */
  hi2c->Instance->OAR1 &= ~I2C_OAR1_OA1EN;

  /* Configure I2Cx: Own Address1 and ack own address1 mode */
  if(hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT)
  {
    hi2c->Instance->OAR1 = (I2C_OAR1_OA1EN | hi2c->Init.OwnAddress1);
  }
  else /* I2C_ADDRESSINGMODE_10BIT */
  {
    hi2c->Instance->OAR1 = (I2C_OAR1_OA1EN | I2C_OAR1_OA1MODE | hi2c->Init.OwnAddress1);
  }

  /*---------------------------- I2Cx CR2 Configuration ----------------------*/
  /* Configure I2Cx: Addressing Master mode */
  if(hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_10BIT)
  {
    hi2c->Instance->CR2 = (I2C_CR2_ADD10);
  }
  /* Enable the AUTOEND by default, and enable NACK (should be disable only during Slave process */
  hi2c->Instance->CR2 |= (I2C_CR2_AUTOEND | I2C_CR2_NACK);

  /*---------------------------- I2Cx OAR2 Configuration ---------------------*/
  /* Disable Own Address2 before set the Own Address2 configuration */
  hi2c->Instance->OAR2 &= ~I2C_DUALADDRESS_ENABLE;

  /* Configure I2Cx: Dual mode and Own Address2 */
  hi2c->Instance->OAR2 = (hi2c->Init.DualAddressMode | hi2c->Init.OwnAddress2 | (hi2c->Init.OwnAddress2Masks << 8));

  /*---------------------------- I2Cx CR1 Configuration ----------------------*/
  /* Configure I2Cx: Generalcall and NoStretch mode */
  hi2c->Instance->CR1 = (hi2c->Init.GeneralCallMode | hi2c->Init.NoStretchMode);

  /* Enable the selected I2C peripheral */
  __HAL_I2C_ENABLE(hi2c);

  hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
  hi2c->State = HAL_I2C_STATE_READY;
  hi2c->PreviousState = I2C_STATE_NONE;
  hi2c->Mode = HAL_I2C_MODE_NONE;

  return HAL_OK;
}

/**
  * @brief  DeInitialize the I2C peripheral.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef *hi2c)
{
  /* Check the I2C handle allocation */
  if(hi2c == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_I2C_ALL_INSTANCE(hi2c->Instance));

  hi2c->State = HAL_I2C_STATE_BUSY;

  /* Disable the I2C Peripheral Clock */
  __HAL_I2C_DISABLE(hi2c);

  /* DeInit the low level hardware: GPIO, CLOCK, NVIC */
  HAL_I2C_MspDeInit(hi2c);

  hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
  hi2c->State = HAL_I2C_STATE_RESET;
  hi2c->PreviousState = I2C_STATE_NONE;
  hi2c->Mode = HAL_I2C_MODE_NONE;

  /* Release Lock */
  __HAL_UNLOCK(hi2c);

  return HAL_OK;
}

/**
  * @brief Initialize the I2C MSP.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2c);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_MspInit could be implemented in the user file
   */
}

/**
  * @brief DeInitialize the I2C MSP.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2c);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup I2C_Exported_Functions_Group2 Input and Output operation functions
 *  @brief   Data transfers functions
 *
@verbatim
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to manage the I2C data
    transfers.

    (#) There are two modes of transfer:
       (++) Blocking mode : The communication is performed in the polling mode.
            The status of all data processing is returned by the same function
            after finishing transfer.
       (++) No-Blocking mode : The communication is performed using Interrupts
            or DMA. These functions return the status of the transfer startup.
            The end of the data processing will be indicated through the
            dedicated I2C IRQ when using Interrupt mode or the DMA IRQ when
            using DMA mode.

    (#) Blocking mode functions are :
        (++) HAL_I2C_Master_Transmit()
        (++) HAL_I2C_Master_Receive()
        (++) HAL_I2C_Slave_Transmit()
        (++) HAL_I2C_Slave_Receive()
        (++) HAL_I2C_Mem_Write()
        (++) HAL_I2C_Mem_Read()
        (++) HAL_I2C_IsDeviceReady()

    (#) No-Blocking mode functions with Interrupt are :
        (++) HAL_I2C_Master_Transmit_IT()
        (++) HAL_I2C_Master_Receive_IT()
        (++) HAL_I2C_Slave_Transmit_IT()
        (++) HAL_I2C_Slave_Receive_IT()
        (++) HAL_I2C_Mem_Write_IT()
        (++) HAL_I2C_Mem_Read_IT()

    (#) No-Blocking mode functions with DMA are :
        (++) HAL_I2C_Master_Transmit_DMA()
        (++) HAL_I2C_Master_Receive_DMA()
        (++) HAL_I2C_Slave_Transmit_DMA()
        (++) HAL_I2C_Slave_Receive_DMA()
        (++) HAL_I2C_Mem_Write_DMA()
        (++) HAL_I2C_Mem_Read_DMA()

    (#) A set of Transfer Complete Callbacks are provided in non Blocking mode:
        (++) HAL_I2C_MemTxCpltCallback()
        (++) HAL_I2C_MemRxCpltCallback()
        (++) HAL_I2C_MasterTxCpltCallback()
        (++) HAL_I2C_MasterRxCpltCallback()
        (++) HAL_I2C_SlaveTxCpltCallback()
        (++) HAL_I2C_SlaveRxCpltCallback()
        (++) HAL_I2C_ErrorCallback()

@endverbatim
  * @{
  */

/**
  * @brief  Transmits in master mode an amount of data in blocking mode.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  DevAddress: Target device address. The device 7 bits address value
  *         in datasheet must be shift at right before call interface
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tickstart = 0;

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hi2c);

    /* Init tickstart for timeout management*/
    tickstart = HAL_GetTick();

    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY, tickstart) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    hi2c->State     = HAL_I2C_STATE_BUSY_TX;
    hi2c->Mode      = HAL_I2C_MODE_MASTER;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr  = pData;
    hi2c->XferCount = Size;
    hi2c->XferISR   = NULL;

    /* Send Slave Address */
    /* Set NBYTES to write and reload if hi2c->XferCount > MAX_NBYTE_SIZE and generate RESTART */
    if(hi2c->XferCount > MAX_NBYTE_SIZE)
    {
      hi2c->XferSize = MAX_NBYTE_SIZE;
      I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, I2C_RELOAD_MODE, I2C_GENERATE_START_WRITE);
    }
    else
    {
      hi2c->XferSize = hi2c->XferCount;
      I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, I2C_AUTOEND_MODE, I2C_GENERATE_START_WRITE);
    }

    while(hi2c->XferSize > 0)
    {
      /* Wait until TXIS flag is set */
      if(I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
      {
        if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
        {
          return HAL_ERROR;
        }
        else
        {
          return HAL_TIMEOUT;
        }
      }
      /* Write data to TXDR */
      hi2c->Instance->TXDR = (*hi2c->pBuffPtr++);
      hi2c->XferCount--;
      hi2c->XferSize--;

      if((hi2c->XferSize == 0) && (hi2c->XferCount!=0))
      {
        /* Wait until TCR flag is set */
        if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TCR, RESET, Timeout, tickstart) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }

        if(hi2c->XferCount > MAX_NBYTE_SIZE)
        {
          hi2c->XferSize = MAX_NBYTE_SIZE;
          I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
        }
        else
        {
          hi2c->XferSize = hi2c->XferCount;
          I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
        }
      }
    }

    /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
    /* Wait until STOPF flag is set */
    if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2(hi2c);

    hi2c->State = HAL_I2C_STATE_READY;
    hi2c->Mode  = HAL_I2C_MODE_NONE;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receives in master mode an amount of data in blocking mode.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  DevAddress: Target device address. The device 7 bits address value
  *         in datasheet must be shift at right before call interface
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tickstart = 0;

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hi2c);

    /* Init tickstart for timeout management*/
    tickstart = HAL_GetTick();

    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY, tickstart) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    hi2c->State     = HAL_I2C_STATE_BUSY_RX;
    hi2c->Mode      = HAL_I2C_MODE_MASTER;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr  = pData;
    hi2c->XferCount = Size;
    hi2c->XferISR   = NULL;

    /* Send Slave Address */
    /* Set NBYTES to write and reload if hi2c->XferCount > MAX_NBYTE_SIZE and generate RESTART */
    if(hi2c->XferCount > MAX_NBYTE_SIZE)
    {
      hi2c->XferSize = MAX_NBYTE_SIZE;
      I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, I2C_RELOAD_MODE, I2C_GENERATE_START_READ);
    }
    else
    {
      hi2c->XferSize = hi2c->XferCount;
      I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, I2C_AUTOEND_MODE, I2C_GENERATE_START_READ);
    }

    while(hi2c->XferSize > 0)
    {
      /* Wait until RXNE flag is set */
      if(I2C_WaitOnRXNEFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
      {
        if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
        {
          return HAL_ERROR;
        }
        else
        {
          return HAL_TIMEOUT;
        }
      }

      /* Read data from RXDR */
      (*hi2c->pBuffPtr++) = hi2c->Instance->RXDR;
      hi2c->XferSize--;
      hi2c->XferCount--;

      if((hi2c->XferSize == 0) && (hi2c->XferCount != 0))
      {
        /* Wait until TCR flag is set */
        if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TCR, RESET, Timeout, tickstart) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }

        if(hi2c->XferCount > MAX_NBYTE_SIZE)
        {
          hi2c->XferSize = MAX_NBYTE_SIZE;
          I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
        }
        else
        {
          hi2c->XferSize = hi2c->XferCount;
          I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
        }
      }
    }

    /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
    /* Wait until STOPF flag is set */
    if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2(hi2c);

    hi2c->State = HAL_I2C_STATE_READY;
    hi2c->Mode  = HAL_I2C_MODE_NONE;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Transmits in slave mode an amount of data in blocking mode.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Slave_Transmit(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tickstart = 0;

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL ) || (Size == 0))
    {
      return  HAL_ERROR;
    }
    /* Process Locked */
    __HAL_LOCK(hi2c);

    /* Init tickstart for timeout management*/
    tickstart = HAL_GetTick();

    hi2c->State     = HAL_I2C_STATE_BUSY_TX;
    hi2c->Mode      = HAL_I2C_MODE_SLAVE;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr  = pData;
    hi2c->XferCount = Size;
    hi2c->XferISR   = NULL;

    /* Enable Address Acknowledge */
    hi2c->Instance->CR2 &= ~I2C_CR2_NACK;

    /* Wait until ADDR flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, RESET, Timeout, tickstart) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;
      return HAL_TIMEOUT;
    }

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_FLAG(hi2c,I2C_FLAG_ADDR);

    /* If 10bit addressing mode is selected */
    if(hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_10BIT)
    {
      /* Wait until ADDR flag is set */
      if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, RESET, Timeout, tickstart) != HAL_OK)
      {
        /* Disable Address Acknowledge */
        hi2c->Instance->CR2 |= I2C_CR2_NACK;
        return HAL_TIMEOUT;
      }

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_FLAG(hi2c,I2C_FLAG_ADDR);
    }

    /* Wait until DIR flag is set Transmitter mode */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_DIR, RESET, Timeout, tickstart) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;
      return HAL_TIMEOUT;
    }

    while(hi2c->XferCount > 0)
    {
      /* Wait until TXIS flag is set */
      if(I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
      {
        /* Disable Address Acknowledge */
        hi2c->Instance->CR2 |= I2C_CR2_NACK;

        if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
        {
          return HAL_ERROR;
        }
        else
        {
          return HAL_TIMEOUT;
        }
      }

      /* Write data to TXDR */
      hi2c->Instance->TXDR = (*hi2c->pBuffPtr++);
      hi2c->XferCount--;
    }

    /* Wait until STOP flag is set */
    if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;

      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Normal use case for Transmitter mode */
        /* A NACK is generated to confirm the end of transfer */
        hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }

    /* Clear STOP flag */
    __HAL_I2C_CLEAR_FLAG(hi2c,I2C_FLAG_STOPF);

    /* Wait until BUSY flag is reset */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, Timeout, tickstart) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;
      return HAL_TIMEOUT;
    }

    /* Disable Address Acknowledge */
    hi2c->Instance->CR2 |= I2C_CR2_NACK;

    hi2c->State = HAL_I2C_STATE_READY;
    hi2c->Mode  = HAL_I2C_MODE_NONE;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receive in slave mode an amount of data in blocking mode
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Slave_Receive(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tickstart = 0;

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL ) || (Size == 0))
    {
      return  HAL_ERROR;
    }
        /* Process Locked */
    __HAL_LOCK(hi2c);

    /* Init tickstart for timeout management*/
    tickstart = HAL_GetTick();

    hi2c->State     = HAL_I2C_STATE_BUSY_RX;
    hi2c->Mode      = HAL_I2C_MODE_SLAVE;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr  = pData;
    hi2c->XferCount = Size;
    hi2c->XferISR   = NULL;

    /* Enable Address Acknowledge */
    hi2c->Instance->CR2 &= ~I2C_CR2_NACK;

    /* Wait until ADDR flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, RESET, Timeout, tickstart) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;
      return HAL_TIMEOUT;
    }

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_FLAG(hi2c,I2C_FLAG_ADDR);

    /* Wait until DIR flag is reset Receiver mode */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_DIR, SET, Timeout, tickstart) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;
      return HAL_TIMEOUT;
    }

    while(hi2c->XferCount > 0)
    {
      /* Wait until RXNE flag is set */
      if(I2C_WaitOnRXNEFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
      {
        /* Disable Address Acknowledge */
        hi2c->Instance->CR2 |= I2C_CR2_NACK;

        /* Store Last receive data if any */
        if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE) == SET)
        {
          /* Read data from RXDR */
          (*hi2c->pBuffPtr++) = hi2c->Instance->RXDR;
          hi2c->XferCount--;
        }

        if(hi2c->ErrorCode == HAL_I2C_ERROR_TIMEOUT)
        {
          return HAL_TIMEOUT;
        }
        else
        {
          return HAL_ERROR;
        }
      }

      /* Read data from RXDR */
      (*hi2c->pBuffPtr++) = hi2c->Instance->RXDR;
      hi2c->XferCount--;
    }

    /* Wait until STOP flag is set */
    if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;

      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }

    /* Clear STOP flag */
    __HAL_I2C_CLEAR_FLAG(hi2c,I2C_FLAG_STOPF);

    /* Wait until BUSY flag is reset */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, Timeout, tickstart) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;
      return HAL_TIMEOUT;
    }

    /* Disable Address Acknowledge */
    hi2c->Instance->CR2 |= I2C_CR2_NACK;

    hi2c->State = HAL_I2C_STATE_READY;
    hi2c->Mode  = HAL_I2C_MODE_NONE;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Transmit in master mode an amount of data in non-blocking mode with Interrupt
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  DevAddress: Target device address. The device 7 bits address value
  *         in datasheet must be shift at right before call interface
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Transmit_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
  uint32_t xfermode = 0;

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State       = HAL_I2C_STATE_BUSY_TX;
    hi2c->Mode        = HAL_I2C_MODE_MASTER;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;
    hi2c->XferISR     = I2C_Master_ISR_IT;

    if(hi2c->XferCount > MAX_NBYTE_SIZE)
    {
      hi2c->XferSize = MAX_NBYTE_SIZE;
      xfermode = I2C_RELOAD_MODE;
    }
    else
    {
      hi2c->XferSize = hi2c->XferCount;
      xfermode = I2C_AUTOEND_MODE;
    }

    /* Send Slave Address */
    /* Set NBYTES to write and reload if hi2c->XferCount > MAX_NBYTE_SIZE */
    I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, xfermode, I2C_GENERATE_START_WRITE);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */

    /* Enable ERR, TC, STOP, NACK, TXI interrupt */
    /* possible to enable all of these */
    /* I2C_IT_ERRI | I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_TXI */
    I2C_Enable_IRQ(hi2c, I2C_XFER_TX_IT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receive in master mode an amount of data in non-blocking mode with Interrupt
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  DevAddress: Target device address. The device 7 bits address value
  *         in datasheet must be shift at right before call interface
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Receive_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
  uint32_t xfermode = 0;

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State       = HAL_I2C_STATE_BUSY_RX;
    hi2c->Mode        = HAL_I2C_MODE_MASTER;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;
    hi2c->XferISR     = I2C_Master_ISR_IT;

    if(hi2c->XferCount > MAX_NBYTE_SIZE)
    {
      hi2c->XferSize = MAX_NBYTE_SIZE;
      xfermode = I2C_RELOAD_MODE;
    }
    else
    {
      hi2c->XferSize = hi2c->XferCount;
      xfermode = I2C_AUTOEND_MODE;
    }

    /* Send Slave Address */
    /* Set NBYTES to write and reload if hi2c->XferCount > MAX_NBYTE_SIZE */
    I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, xfermode, I2C_GENERATE_START_READ);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */

    /* Enable ERR, TC, STOP, NACK, RXI interrupt */
    /* possible to enable all of these */
    /* I2C_IT_ERRI | I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_TXI */
    I2C_Enable_IRQ(hi2c, I2C_XFER_RX_IT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Transmit in slave mode an amount of data in non-blocking mode with Interrupt
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Slave_Transmit_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size)
{
  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State       = HAL_I2C_STATE_BUSY_TX;
    hi2c->Mode        = HAL_I2C_MODE_SLAVE;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Enable Address Acknowledge */
    hi2c->Instance->CR2 &= ~I2C_CR2_NACK;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferSize    = hi2c->XferCount;
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;
    hi2c->XferISR     = I2C_Slave_ISR_IT;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */

    /* Enable ERR, TC, STOP, NACK, TXI interrupt */
    /* possible to enable all of these */
    /* I2C_IT_ERRI | I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_TXI */
    I2C_Enable_IRQ(hi2c, I2C_XFER_TX_IT | I2C_XFER_LISTEN_IT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receive in slave mode an amount of data in non-blocking mode with Interrupt
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Slave_Receive_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size)
{
  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State       = HAL_I2C_STATE_BUSY_RX;
    hi2c->Mode        = HAL_I2C_MODE_SLAVE;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Enable Address Acknowledge */
    hi2c->Instance->CR2 &= ~I2C_CR2_NACK;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferSize    = hi2c->XferCount;
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;
    hi2c->XferISR     = I2C_Slave_ISR_IT;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */

    /* Enable ERR, TC, STOP, NACK, RXI interrupt */
    /* possible to enable all of these */
    /* I2C_IT_ERRI | I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_TXI */
    I2C_Enable_IRQ(hi2c, I2C_XFER_RX_IT | I2C_XFER_LISTEN_IT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Transmit in master mode an amount of data in non-blocking mode with DMA
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  DevAddress: Target device address. The device 7 bits address value
  *         in datasheet must be shift at right before call interface
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
  uint32_t xfermode = 0;

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State       = HAL_I2C_STATE_BUSY_TX;
    hi2c->Mode        = HAL_I2C_MODE_MASTER;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;
    hi2c->XferISR     = I2C_Master_ISR_DMA;

    if(hi2c->XferCount > MAX_NBYTE_SIZE)
    {
      hi2c->XferSize = MAX_NBYTE_SIZE;
      xfermode = I2C_RELOAD_MODE;
    }
    else
    {
      hi2c->XferSize = hi2c->XferCount;
      xfermode = I2C_AUTOEND_MODE;
    }

    /* Set the I2C DMA transfer complete callback */
    hi2c->hdmatx->XferCpltCallback = I2C_DMAMasterTransmitCplt;

    /* Set the DMA error callback */
    hi2c->hdmatx->XferErrorCallback = I2C_DMAError;

    /* Set the unused DMA callbacks to NULL */
    hi2c->hdmatx->XferHalfCpltCallback = NULL;
    hi2c->hdmatx->XferAbortCallback = NULL;

    /* Enable the DMA channel */
    HAL_DMA_Start_IT(hi2c->hdmatx, (uint32_t)pData, (uint32_t)&hi2c->Instance->TXDR, hi2c->XferSize);

    /* Send Slave Address */
    /* Set NBYTES to write and reload if hi2c->XferCount > MAX_NBYTE_SIZE and generate RESTART */
    I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, xfermode, I2C_GENERATE_START_WRITE);

    /* Update XferCount value */
    hi2c->XferCount -= hi2c->XferSize;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */
    /* Enable ERR and NACK interrupts */
    I2C_Enable_IRQ(hi2c, I2C_XFER_ERROR_IT);

    /* Enable DMA Request */
    hi2c->Instance->CR1 |= I2C_CR1_TXDMAEN;

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receive in master mode an amount of data in non-blocking mode with DMA
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  DevAddress: Target device address. The device 7 bits address value
  *         in datasheet must be shift at right before call interface
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Receive_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
  uint32_t xfermode = 0;

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State       = HAL_I2C_STATE_BUSY_RX;
    hi2c->Mode        = HAL_I2C_MODE_MASTER;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;
    hi2c->XferISR     = I2C_Master_ISR_DMA;

    if(hi2c->XferCount > MAX_NBYTE_SIZE)
    {
      hi2c->XferSize = MAX_NBYTE_SIZE;
      xfermode = I2C_RELOAD_MODE;
    }
    else
    {
      hi2c->XferSize = hi2c->XferCount;
      xfermode = I2C_AUTOEND_MODE;
    }

    if(hi2c->XferSize > 0)
    {
      /* Set the I2C DMA transfer complete callback */
      hi2c->hdmarx->XferCpltCallback = I2C_DMAMasterReceiveCplt;

      /* Set the DMA error callback */
      hi2c->hdmarx->XferErrorCallback = I2C_DMAError;

      /* Set the unused DMA callbacks to NULL */
      hi2c->hdmarx->XferHalfCpltCallback = NULL;
      hi2c->hdmarx->XferAbortCallback = NULL;

      /* Enable the DMA channel */
      HAL_DMA_Start_IT(hi2c->hdmarx, (uint32_t)&hi2c->Instance->RXDR, (uint32_t)pData, hi2c->XferSize);

      /* Send Slave Address */
      /* Set NBYTES to write and reload if hi2c->XferCount > MAX_NBYTE_SIZE and generate RESTART */
      I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, xfermode, I2C_GENERATE_START_READ);

      /* Update XferCount value */
      hi2c->XferCount -= hi2c->XferSize;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      /* Note : The I2C interrupts must be enabled after unlocking current process
                to avoid the risk of I2C interrupt handle execution before current
                process unlock */
      /* Enable ERR and NACK interrupts */
      I2C_Enable_IRQ(hi2c, I2C_XFER_ERROR_IT);

      /* Enable DMA Request */
      hi2c->Instance->CR1 |= I2C_CR1_RXDMAEN;
    }
    else
    {
      hi2c->State = HAL_I2C_STATE_READY;
      hi2c->Mode = HAL_I2C_MODE_NONE;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);
    }
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Transmit in slave mode an amount of data in non-blocking mode with DMA
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Slave_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size)
{
  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }
    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State       = HAL_I2C_STATE_BUSY_TX;
    hi2c->Mode        = HAL_I2C_MODE_SLAVE;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferSize    = hi2c->XferCount;
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;
    hi2c->XferISR     = I2C_Slave_ISR_DMA;

    /* Set the I2C DMA transfer complete callback */
    hi2c->hdmatx->XferCpltCallback = I2C_DMASlaveTransmitCplt;

    /* Set the DMA error callback */
    hi2c->hdmatx->XferErrorCallback = I2C_DMAError;

    /* Set the unused DMA callbacks to NULL */
    hi2c->hdmatx->XferHalfCpltCallback = NULL;
    hi2c->hdmatx->XferAbortCallback = NULL;

    /* Enable the DMA channel */
    HAL_DMA_Start_IT(hi2c->hdmatx, (uint32_t)pData, (uint32_t)&hi2c->Instance->TXDR, hi2c->XferSize);

    /* Enable Address Acknowledge */
    hi2c->Instance->CR2 &= ~I2C_CR2_NACK;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */
    /* Enable ERR, STOP, NACK, ADDR interrupts */
    I2C_Enable_IRQ(hi2c, I2C_XFER_LISTEN_IT);

    /* Enable DMA Request */
    hi2c->Instance->CR1 |= I2C_CR1_TXDMAEN;

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receive in slave mode an amount of data in non-blocking mode with DMA
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Slave_Receive_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size)
{
  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }
    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State       = HAL_I2C_STATE_BUSY_RX;
    hi2c->Mode        = HAL_I2C_MODE_SLAVE;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferSize    = hi2c->XferCount;
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;
    hi2c->XferISR     = I2C_Slave_ISR_DMA;

    /* Set the I2C DMA transfer complete callback */
    hi2c->hdmarx->XferCpltCallback = I2C_DMASlaveReceiveCplt;

    /* Set the DMA error callback */
    hi2c->hdmarx->XferErrorCallback = I2C_DMAError;

    /* Set the unused DMA callbacks to NULL */
    hi2c->hdmarx->XferHalfCpltCallback = NULL;
    hi2c->hdmarx->XferAbortCallback = NULL;

    /* Enable the DMA channel */
    HAL_DMA_Start_IT(hi2c->hdmarx, (uint32_t)&hi2c->Instance->RXDR, (uint32_t)pData, hi2c->XferSize);

    /* Enable Address Acknowledge */
    hi2c->Instance->CR2 &= ~I2C_CR2_NACK;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */
    /* Enable ERR, STOP, NACK, ADDR interrupts */
    I2C_Enable_IRQ(hi2c, I2C_XFER_LISTEN_IT);

    /* Enable DMA Request */
    hi2c->Instance->CR1 |= I2C_CR1_RXDMAEN;

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}
/**
  * @brief  Write an amount of data in blocking mode to a specific memory address
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  MemAddSize: Size of internal memory address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tickstart = 0;

  /* Check the parameters */
  assert_param(IS_I2C_MEMADD_SIZE(MemAddSize));

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    /* Init tickstart for timeout management*/
    tickstart = HAL_GetTick();

    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY, tickstart) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    hi2c->State     = HAL_I2C_STATE_BUSY_TX;
    hi2c->Mode      = HAL_I2C_MODE_MEM;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr  = pData;
    hi2c->XferCount = Size;
    hi2c->XferISR   = NULL;

    /* Send Slave Address and Memory Address */
    if(I2C_RequestMemoryWrite(hi2c, DevAddress, MemAddress, MemAddSize, Timeout, tickstart) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    /* Set NBYTES to write and reload if hi2c->XferCount > MAX_NBYTE_SIZE */
    if(hi2c->XferCount > MAX_NBYTE_SIZE)
    {
      hi2c->XferSize = MAX_NBYTE_SIZE;
      I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
    }
    else
    {
      hi2c->XferSize = hi2c->XferCount;
      I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
    }

    do
    {
      /* Wait until TXIS flag is set */
      if(I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
      {
        if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
        {
          return HAL_ERROR;
        }
        else
        {
          return HAL_TIMEOUT;
        }
      }

      /* Write data to TXDR */
      hi2c->Instance->TXDR = (*hi2c->pBuffPtr++);
      hi2c->XferCount--;
      hi2c->XferSize--;

      if((hi2c->XferSize == 0) && (hi2c->XferCount!=0))
      {
        /* Wait until TCR flag is set */
        if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TCR, RESET, Timeout, tickstart) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }

        if(hi2c->XferCount > MAX_NBYTE_SIZE)
        {
          hi2c->XferSize = MAX_NBYTE_SIZE;
          I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
        }
        else
        {
          hi2c->XferSize = hi2c->XferCount;
          I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
        }
      }

    }while(hi2c->XferCount > 0);

    /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
    /* Wait until STOPF flag is reset */
    if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2(hi2c);

    hi2c->State = HAL_I2C_STATE_READY;
    hi2c->Mode  = HAL_I2C_MODE_NONE;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Read an amount of data in blocking mode from a specific memory address
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  MemAddSize: Size of internal memory address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tickstart = 0;

  /* Check the parameters */
  assert_param(IS_I2C_MEMADD_SIZE(MemAddSize));

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    /* Init tickstart for timeout management*/
    tickstart = HAL_GetTick();

    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY, tickstart) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    hi2c->State     = HAL_I2C_STATE_BUSY_RX;
    hi2c->Mode      = HAL_I2C_MODE_MEM;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr  = pData;
    hi2c->XferCount = Size;
    hi2c->XferISR   = NULL;

    /* Send Slave Address and Memory Address */
    if(I2C_RequestMemoryRead(hi2c, DevAddress, MemAddress, MemAddSize, Timeout, tickstart) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    /* Send Slave Address */
    /* Set NBYTES to write and reload if hi2c->XferCount > MAX_NBYTE_SIZE and generate RESTART */
    if(hi2c->XferCount > MAX_NBYTE_SIZE)
    {
      hi2c->XferSize = MAX_NBYTE_SIZE;
      I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, I2C_RELOAD_MODE, I2C_GENERATE_START_READ);
    }
    else
    {
      hi2c->XferSize = hi2c->XferCount;
      I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, I2C_AUTOEND_MODE, I2C_GENERATE_START_READ);
    }

    do
    {
      /* Wait until RXNE flag is set */
      if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_RXNE, RESET, Timeout, tickstart) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }

      /* Read data from RXDR */
      (*hi2c->pBuffPtr++) = hi2c->Instance->RXDR;
      hi2c->XferSize--;
      hi2c->XferCount--;

      if((hi2c->XferSize == 0) && (hi2c->XferCount != 0))
      {
        /* Wait until TCR flag is set */
        if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TCR, RESET, Timeout, tickstart) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }

        if(hi2c->XferCount > MAX_NBYTE_SIZE)
        {
          hi2c->XferSize = MAX_NBYTE_SIZE;
          I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
        }
        else
        {
          hi2c->XferSize = hi2c->XferCount;
          I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
        }
      }
    }while(hi2c->XferCount > 0);

    /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
    /* Wait until STOPF flag is reset */
    if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2(hi2c);

    hi2c->State = HAL_I2C_STATE_READY;
    hi2c->Mode  = HAL_I2C_MODE_NONE;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}
/**
  * @brief  Write an amount of data in non-blocking mode with Interrupt to a specific memory address
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  MemAddSize: Size of internal memory address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Mem_Write_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
  uint32_t tickstart = 0;
  uint32_t xfermode = 0;

  /* Check the parameters */
  assert_param(IS_I2C_MEMADD_SIZE(MemAddSize));

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    /* Init tickstart for timeout management*/
    tickstart = HAL_GetTick();

    hi2c->State       = HAL_I2C_STATE_BUSY_TX;
    hi2c->Mode        = HAL_I2C_MODE_MEM;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;
    hi2c->XferISR     = I2C_Master_ISR_IT;

    if(hi2c->XferCount > MAX_NBYTE_SIZE)
    {
      hi2c->XferSize = MAX_NBYTE_SIZE;
      xfermode = I2C_RELOAD_MODE;
    }
    else
    {
      hi2c->XferSize = hi2c->XferCount;
      xfermode = I2C_AUTOEND_MODE;
    }

    /* Send Slave Address and Memory Address */
    if(I2C_RequestMemoryWrite(hi2c, DevAddress, MemAddress, MemAddSize, I2C_TIMEOUT_FLAG, tickstart) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    /* Set NBYTES to write and reload if hi2c->XferCount > MAX_NBYTE_SIZE and generate RESTART */
    I2C_TransferConfig(hi2c,DevAddress, hi2c->XferSize, xfermode, I2C_NO_STARTSTOP);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */

    /* Enable ERR, TC, STOP, NACK, TXI interrupt */
    /* possible to enable all of these */
    /* I2C_IT_ERRI | I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_TXI */
    I2C_Enable_IRQ(hi2c, I2C_XFER_TX_IT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Read an amount of data in non-blocking mode with Interrupt from a specific memory address
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  MemAddSize: Size of internal memory address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Mem_Read_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
  uint32_t tickstart = 0;
  uint32_t xfermode = 0;

  /* Check the parameters */
  assert_param(IS_I2C_MEMADD_SIZE(MemAddSize));

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    /* Init tickstart for timeout management*/
    tickstart = HAL_GetTick();

    hi2c->State       = HAL_I2C_STATE_BUSY_RX;
    hi2c->Mode        = HAL_I2C_MODE_MEM;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;
    hi2c->XferISR     = I2C_Master_ISR_IT;

    if(hi2c->XferCount > MAX_NBYTE_SIZE)
    {
      hi2c->XferSize = MAX_NBYTE_SIZE;
      xfermode = I2C_RELOAD_MODE;
    }
    else
    {
      hi2c->XferSize = hi2c->XferCount;
      xfermode = I2C_AUTOEND_MODE;
    }

    /* Send Slave Address and Memory Address */
    if(I2C_RequestMemoryRead(hi2c, DevAddress, MemAddress, MemAddSize, I2C_TIMEOUT_FLAG, tickstart) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    /* Set NBYTES to write and reload if hi2c->XferCount > MAX_NBYTE_SIZE and generate RESTART */
    I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, xfermode, I2C_GENERATE_START_READ);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */

    /* Enable ERR, TC, STOP, NACK, RXI interrupt */
    /* possible to enable all of these */
    /* I2C_IT_ERRI | I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_TXI */
    I2C_Enable_IRQ(hi2c, I2C_XFER_RX_IT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}
/**
  * @brief  Write an amount of data in non-blocking mode with DMA to a specific memory address
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  MemAddSize: Size of internal memory address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Mem_Write_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
  uint32_t tickstart = 0;
  uint32_t xfermode = 0;

  /* Check the parameters */
  assert_param(IS_I2C_MEMADD_SIZE(MemAddSize));

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    /* Init tickstart for timeout management*/
    tickstart = HAL_GetTick();

    hi2c->State       = HAL_I2C_STATE_BUSY_TX;
    hi2c->Mode        = HAL_I2C_MODE_MEM;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;
    hi2c->XferISR     = I2C_Master_ISR_DMA;

    if(hi2c->XferCount > MAX_NBYTE_SIZE)
    {
      hi2c->XferSize = MAX_NBYTE_SIZE;
      xfermode = I2C_RELOAD_MODE;
    }
    else
    {
      hi2c->XferSize = hi2c->XferCount;
      xfermode = I2C_AUTOEND_MODE;
    }

    /* Send Slave Address and Memory Address */
    if(I2C_RequestMemoryWrite(hi2c, DevAddress, MemAddress, MemAddSize, I2C_TIMEOUT_FLAG, tickstart) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    /* Set the I2C DMA transfer complete callback */
    hi2c->hdmatx->XferCpltCallback = I2C_DMAMasterTransmitCplt;

    /* Set the DMA error callback */
    hi2c->hdmatx->XferErrorCallback = I2C_DMAError;

    /* Set the unused DMA callbacks to NULL */
    hi2c->hdmatx->XferHalfCpltCallback = NULL;
    hi2c->hdmatx->XferAbortCallback = NULL;

    /* Enable the DMA channel */
    HAL_DMA_Start_IT(hi2c->hdmatx, (uint32_t)pData, (uint32_t)&hi2c->Instance->TXDR, hi2c->XferSize);

    /* Send Slave Address */
    /* Set NBYTES to write and reload if hi2c->XferCount > MAX_NBYTE_SIZE and generate RESTART */
    I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, xfermode, I2C_NO_STARTSTOP);

    /* Update XferCount value */
    hi2c->XferCount -= hi2c->XferSize;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */
    /* Enable ERR and NACK interrupts */
    I2C_Enable_IRQ(hi2c, I2C_XFER_ERROR_IT);

    /* Enable DMA Request */
    hi2c->Instance->CR1 |= I2C_CR1_TXDMAEN;

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Reads an amount of data in non-blocking mode with DMA from a specific memory address.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  MemAddSize: Size of internal memory address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be read
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Mem_Read_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
  uint32_t tickstart = 0;
  uint32_t xfermode = 0;

  /* Check the parameters */
  assert_param(IS_I2C_MEMADD_SIZE(MemAddSize));

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    /* Init tickstart for timeout management*/
    tickstart = HAL_GetTick();

    hi2c->State       = HAL_I2C_STATE_BUSY_RX;
    hi2c->Mode        = HAL_I2C_MODE_MEM;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;
    hi2c->XferISR     = I2C_Master_ISR_DMA;

    if(hi2c->XferCount > MAX_NBYTE_SIZE)
    {
      hi2c->XferSize = MAX_NBYTE_SIZE;
      xfermode = I2C_RELOAD_MODE;
    }
    else
    {
      hi2c->XferSize = hi2c->XferCount;
      xfermode = I2C_AUTOEND_MODE;
    }

    /* Send Slave Address and Memory Address */
    if(I2C_RequestMemoryRead(hi2c, DevAddress, MemAddress, MemAddSize, I2C_TIMEOUT_FLAG, tickstart) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    /* Set the I2C DMA transfer complete callback */
    hi2c->hdmarx->XferCpltCallback = I2C_DMAMasterReceiveCplt;

    /* Set the DMA error callback */
    hi2c->hdmarx->XferErrorCallback = I2C_DMAError;

    /* Set the unused DMA callbacks to NULL */
    hi2c->hdmarx->XferHalfCpltCallback = NULL;
    hi2c->hdmarx->XferAbortCallback = NULL;

    /* Enable the DMA channel */
    HAL_DMA_Start_IT(hi2c->hdmarx, (uint32_t)&hi2c->Instance->RXDR, (uint32_t)pData, hi2c->XferSize);

    /* Set NBYTES to write and reload if hi2c->XferCount > MAX_NBYTE_SIZE and generate RESTART */
    I2C_TransferConfig(hi2c,DevAddress, hi2c->XferSize, xfermode, I2C_GENERATE_START_READ);

    /* Update XferCount value */
    hi2c->XferCount -= hi2c->XferSize;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Enable DMA Request */
    hi2c->Instance->CR1 |= I2C_CR1_RXDMAEN;

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */
    /* Enable ERR and NACK interrupts */
    I2C_Enable_IRQ(hi2c, I2C_XFER_ERROR_IT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Checks if target device is ready for communication.
  * @note   This function is used with Memory devices
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  DevAddress: Target device address
  * @param  Trials: Number of trials
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Trials, uint32_t Timeout)
{
  uint32_t tickstart = 0;

  __IO uint32_t I2C_Trials = 0;

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_BUSY;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    do
    {
      /* Generate Start */
      hi2c->Instance->CR2 = I2C_GENERATE_START(hi2c->Init.AddressingMode,DevAddress);

      /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
      /* Wait until STOPF flag is set or a NACK flag is set*/
      tickstart = HAL_GetTick();
      while((__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == RESET) && (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == RESET) && (hi2c->State != HAL_I2C_STATE_TIMEOUT))
      {
      	if(Timeout != HAL_MAX_DELAY)
      	{
          if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
          {
            /* Device is ready */
            hi2c->State = HAL_I2C_STATE_READY;
            /* Process Unlocked */
            __HAL_UNLOCK(hi2c);
            return HAL_TIMEOUT;
          }
        }
      }

      /* Check if the NACKF flag has not been set */
      if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == RESET)
      {
        /* Wait until STOPF flag is reset */
        if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_STOPF, RESET, Timeout, tickstart) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }

        /* Clear STOP Flag */
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

        /* Device is ready */
        hi2c->State = HAL_I2C_STATE_READY;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_OK;
      }
      else
      {
        /* Wait until STOPF flag is reset */
        if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_STOPF, RESET, Timeout, tickstart) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }

        /* Clear NACK Flag */
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

        /* Clear STOP Flag, auto generated with autoend*/
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);
      }

      /* Check if the maximum allowed number of trials has been reached */
      if (I2C_Trials++ == Trials)
      {
        /* Generate Stop */
        hi2c->Instance->CR2 |= I2C_CR2_STOP;

        /* Wait until STOPF flag is reset */
        if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_STOPF, RESET, Timeout, tickstart) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }

        /* Clear STOP Flag */
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);
      }
    }while(I2C_Trials < Trials);

    hi2c->State = HAL_I2C_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_TIMEOUT;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Sequential transmit in master I2C mode an amount of data in non-blocking mode with Interrupt.
  * @note   This interface allow to manage repeated start condition when a direction change during transfer
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  DevAddress: Target device address. The device 7 bits address value
  *         in datasheet must be shift at right before call interface
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  XferOptions: Options of Transfer, value of @ref I2C_XFEROPTIONS
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Sequential_Transmit_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t XferOptions)
{
  uint32_t xfermode = 0;
  uint32_t xferrequest = I2C_GENERATE_START_WRITE;

  /* Check the parameters */
  assert_param(IS_I2C_TRANSFER_OPTIONS_REQUEST(XferOptions));

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State     = HAL_I2C_STATE_BUSY_TX;
    hi2c->Mode      = HAL_I2C_MODE_MASTER;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferOptions = XferOptions;
    hi2c->XferISR     = I2C_Master_ISR_IT;

    /* If size > MAX_NBYTE_SIZE, use reload mode */
    if(hi2c->XferCount > MAX_NBYTE_SIZE)
    {
      hi2c->XferSize = MAX_NBYTE_SIZE;
      xfermode = I2C_RELOAD_MODE;
    }
    else
    {
      hi2c->XferSize = hi2c->XferCount;
      xfermode = hi2c->XferOptions;
	}

    /* If transfer direction not change, do not generate Restart Condition */
    /* Mean Previous state is same as current state */
    if(hi2c->PreviousState == I2C_STATE_MASTER_BUSY_TX)
    {
    xferrequest = I2C_NO_STARTSTOP;
    }

    /* Send Slave Address and set NBYTES to write */
    I2C_TransferConfig(hi2c, DevAddress, hi2c->XferSize, xfermode, xferrequest);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */
    I2C_Enable_IRQ(hi2c, I2C_XFER_TX_IT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Sequential receive in master I2C mode an amount of data in non-blocking mode with Interrupt
  * @note   This interface allow to manage repeated start condition when a direction change during transfer
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  DevAddress: Target device address. The device 7 bits address value
  *         in datasheet must be shift at right before call interface
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  XferOptions: Options of Transfer, value of @ref I2C_XFEROPTIONS
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Sequential_Receive_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t XferOptions)
{
  uint32_t xfermode = 0;
  uint32_t xferrequest = I2C_GENERATE_START_READ;

  /* Check the parameters */
  assert_param(IS_I2C_TRANSFER_OPTIONS_REQUEST(XferOptions));

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State     = HAL_I2C_STATE_BUSY_RX;
    hi2c->Mode      = HAL_I2C_MODE_MASTER;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferOptions = XferOptions;
    hi2c->XferISR     = I2C_Master_ISR_IT;

    /* If hi2c->XferCount > MAX_NBYTE_SIZE, use reload mode */
    if(hi2c->XferCount > MAX_NBYTE_SIZE)
    {
      hi2c->XferSize = MAX_NBYTE_SIZE;
      xfermode = I2C_RELOAD_MODE;
    }
    else
    {
      hi2c->XferSize = hi2c->XferCount;
      xfermode = hi2c->XferOptions;
	}

    /* If transfer direction not change, do not generate Restart Condition */
    /* Mean Previous state is same as current state */
    if(hi2c->PreviousState == I2C_STATE_MASTER_BUSY_RX)
    {
      xferrequest = I2C_NO_STARTSTOP;
    }

    /* Send Slave Address and set NBYTES to read */
    I2C_TransferConfig(hi2c,DevAddress, hi2c->XferSize, xfermode, xferrequest);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */
    I2C_Enable_IRQ(hi2c, I2C_XFER_RX_IT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Sequential transmit in slave/device I2C mode an amount of data in non-blocking mode with Interrupt
  * @note   This interface allow to manage repeated start condition when a direction change during transfer
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  XferOptions: Options of Transfer, value of @ref I2C_XFEROPTIONS
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Slave_Sequential_Transmit_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t XferOptions)
{
  /* Check the parameters */
  assert_param(IS_I2C_TRANSFER_OPTIONS_REQUEST(XferOptions));

  if(hi2c->State == HAL_I2C_STATE_LISTEN)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Disable Interrupts, to prevent preemption during treatment in case of multicall */
    I2C_Disable_IRQ(hi2c, I2C_XFER_LISTEN_IT | I2C_XFER_TX_IT);

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State     = HAL_I2C_STATE_BUSY_TX_LISTEN;
    hi2c->Mode      = HAL_I2C_MODE_SLAVE;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    /* Enable Address Acknowledge */
    hi2c->Instance->CR2 &= ~I2C_CR2_NACK;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferSize    = hi2c->XferCount;
    hi2c->XferOptions = XferOptions;
    hi2c->XferISR     = I2C_Slave_ISR_IT;

    if(I2C_GET_DIR(hi2c) == I2C_DIRECTION_RECEIVE)
    {
      /* Clear ADDR flag after prepare the transfer parameters */
      /* This action will generate an acknowledge to the Master */
      __HAL_I2C_CLEAR_FLAG(hi2c,I2C_FLAG_ADDR);
    }

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
    to avoid the risk of I2C interrupt handle execution before current
    process unlock */
    /* REnable ADDR interrupt */
    I2C_Enable_IRQ(hi2c, I2C_XFER_TX_IT | I2C_XFER_LISTEN_IT);

    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;
  }
}

/**
  * @brief  Sequential receive in slave/device I2C mode an amount of data in non-blocking mode with Interrupt
  * @note   This interface allow to manage repeated start condition when a direction change during transfer
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  XferOptions: Options of Transfer, value of @ref I2C_XFEROPTIONS
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Slave_Sequential_Receive_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t XferOptions)
{
  /* Check the parameters */
  assert_param(IS_I2C_TRANSFER_OPTIONS_REQUEST(XferOptions));

  if(hi2c->State == HAL_I2C_STATE_LISTEN)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Disable Interrupts, to prevent preemption during treatment in case of multicall */
    I2C_Disable_IRQ(hi2c, I2C_XFER_LISTEN_IT | I2C_XFER_RX_IT);

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State     = HAL_I2C_STATE_BUSY_RX_LISTEN;
    hi2c->Mode      = HAL_I2C_MODE_SLAVE;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    /* Enable Address Acknowledge */
    hi2c->Instance->CR2 &= ~I2C_CR2_NACK;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferSize    = hi2c->XferCount;
    hi2c->XferOptions = XferOptions;
    hi2c->XferISR     = I2C_Slave_ISR_IT;

    if(I2C_GET_DIR(hi2c) == I2C_DIRECTION_TRANSMIT)
    {
      /* Clear ADDR flag after prepare the transfer parameters */
      /* This action will generate an acknowledge to the Master */
      __HAL_I2C_CLEAR_FLAG(hi2c,I2C_FLAG_ADDR);
    }

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
    to avoid the risk of I2C interrupt handle execution before current
    process unlock */
    /* REnable ADDR interrupt */
    I2C_Enable_IRQ(hi2c, I2C_XFER_RX_IT | I2C_XFER_LISTEN_IT);

    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;
  }
}

/**
  * @brief  Enable the Address listen mode with Interrupt.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_EnableListen_IT(I2C_HandleTypeDef *hi2c)
{
  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    hi2c->State = HAL_I2C_STATE_LISTEN;
    hi2c->XferISR = I2C_Slave_ISR_IT;

    /* Enable the Address Match interrupt */
    I2C_Enable_IRQ(hi2c, I2C_XFER_LISTEN_IT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Disable the Address listen mode with Interrupt.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_DisableListen_IT(I2C_HandleTypeDef *hi2c)
{
  /* Declaration of tmp to prevent undefined behavior of volatile usage */
  uint32_t tmp;

  /* Disable Address listen mode only if a transfer is not ongoing */
  if(hi2c->State == HAL_I2C_STATE_LISTEN)
  {
    tmp = (uint32_t)(hi2c->State) & I2C_STATE_MSK;
    hi2c->PreviousState = tmp | (uint32_t)(hi2c->Mode);
    hi2c->State = HAL_I2C_STATE_READY;
    hi2c->Mode = HAL_I2C_MODE_NONE;
    hi2c->XferISR = NULL;

    /* Disable the Address Match interrupt */
    I2C_Disable_IRQ(hi2c, I2C_XFER_LISTEN_IT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Abort a master I2C IT or DMA process communication with Interrupt.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shift at right before call interface
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Abort_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress)
{
  if(hi2c->Mode == HAL_I2C_MODE_MASTER)
  {
    /* Process Locked */
    __HAL_LOCK(hi2c);

    /* Disable Interrupts */
    I2C_Disable_IRQ(hi2c, I2C_XFER_RX_IT);
    I2C_Disable_IRQ(hi2c, I2C_XFER_TX_IT);

    /* Set State at HAL_I2C_STATE_ABORT */
    hi2c->State = HAL_I2C_STATE_ABORT;

    /* Set NBYTES to 1 to generate a dummy read on I2C peripheral */
    /* Set AUTOEND mode, this will generate a NACK then STOP condition to abort the current transfer */
    I2C_TransferConfig(hi2c, 0, 1, I2C_AUTOEND_MODE, I2C_GENERATE_STOP);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */
    I2C_Enable_IRQ(hi2c, I2C_XFER_CPLT_IT);

    return HAL_OK;
  }
  else
  {
    /* Wrong usage of abort function */
    /* This function should be used only in case of abort monitored by master device */
    return HAL_ERROR;
  }
}

/**
  * @}
  */

/** @defgroup I2C_IRQ_Handler_and_Callbacks IRQ Handler and Callbacks
 * @{
 */

/**
  * @brief  This function handles I2C event interrupt request.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_EV_IRQHandler(I2C_HandleTypeDef *hi2c)
{
  /* Get current IT Flags and IT sources value */
  uint32_t itflags   = READ_REG(hi2c->Instance->ISR);
  uint32_t itsources = READ_REG(hi2c->Instance->CR1);

  /* I2C events treatment -------------------------------------*/
  if(hi2c->XferISR != NULL)
  {
    hi2c->XferISR(hi2c, itflags, itsources);
  }
}

/**
  * @brief  This function handles I2C error interrupt request.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_ER_IRQHandler(I2C_HandleTypeDef *hi2c)
{
  uint32_t itflags   = READ_REG(hi2c->Instance->ISR);
  uint32_t itsources = READ_REG(hi2c->Instance->CR1);

  /* I2C Bus error interrupt occurred ------------------------------------*/
  if(((itflags & I2C_FLAG_BERR) != RESET) && ((itsources & I2C_IT_ERRI) != RESET))
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_BERR;

    /* Clear BERR flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_BERR);
  }

  /* I2C Over-Run/Under-Run interrupt occurred ----------------------------------------*/
  if(((itflags & I2C_FLAG_OVR) != RESET) && ((itsources & I2C_IT_ERRI) != RESET))
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_OVR;

    /* Clear OVR flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_OVR);
  }

  /* I2C Arbitration Loss error interrupt occurred -------------------------------------*/
  if(((itflags & I2C_FLAG_ARLO) != RESET) && ((itsources & I2C_IT_ERRI) != RESET))
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_ARLO;

    /* Clear ARLO flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_ARLO);
  }

  /* Call the Error Callback in case of Error detected */
  if((hi2c->ErrorCode & (HAL_I2C_ERROR_BERR | HAL_I2C_ERROR_OVR | HAL_I2C_ERROR_ARLO)) !=  HAL_I2C_ERROR_NONE)
  {
    I2C_ITError(hi2c, hi2c->ErrorCode);
  }
}

/**
  * @brief  Master Tx Transfer completed callback.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2c);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_MasterTxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Master Rx Transfer completed callback.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2c);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_MasterRxCpltCallback could be implemented in the user file
   */
}

/** @brief  Slave Tx Transfer completed callback.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2c);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_SlaveTxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Slave Rx Transfer completed callback.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2c);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_SlaveRxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Slave Address Match callback.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  TransferDirection: Master request Transfer Direction (Write/Read), value of @ref I2C_XFEROPTIONS
  * @param  AddrMatchCode: Address Match Code
  * @retval None
  */
__weak void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2c);
  UNUSED(TransferDirection);
  UNUSED(AddrMatchCode);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_AddrCallback() could be implemented in the user file
   */
}

/**
  * @brief  Listen Complete callback.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2c);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_ListenCpltCallback() could be implemented in the user file
   */
}

/**
  * @brief  Memory Tx Transfer completed callback.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2c);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_MemTxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Memory Rx Transfer completed callback.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2c);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_MemRxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  I2C error callback.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2c);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_ErrorCallback could be implemented in the user file
   */
}

/**
  * @brief  I2C abort callback.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2c);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_AbortCpltCallback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup I2C_Exported_Functions_Group3 Peripheral State, Mode and Error functions
 *  @brief   Peripheral State, Mode and Error functions
 *
@verbatim
 ===============================================================================
            ##### Peripheral State, Mode and Error functions #####
 ===============================================================================
    [..]
    This subsection permit to get in run-time the status of the peripheral
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  Return the I2C handle state.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @retval HAL state
  */
HAL_I2C_StateTypeDef HAL_I2C_GetState(I2C_HandleTypeDef *hi2c)
{
  /* Return I2C handle state */
  return hi2c->State;
}

/**
  * @brief  Returns the I2C Master, Slave, Memory or no mode.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval HAL mode
  */
HAL_I2C_ModeTypeDef HAL_I2C_GetMode(I2C_HandleTypeDef *hi2c)
{
  return hi2c->Mode;
}

/**
* @brief  Return the I2C error code.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *              the configuration information for the specified I2C.
* @retval I2C Error Code
*/
uint32_t HAL_I2C_GetError(I2C_HandleTypeDef *hi2c)
{
  return hi2c->ErrorCode;
}

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup I2C_Private_Functions
  * @{
  */

/**
  * @brief  Interrupt Sub-Routine which handle the Interrupt Flags Master Mode with Interrupt.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  ITFlags: Interrupt flags to handle.
  * @param  ITSources: Interrupt sources enabled.
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_Master_ISR_IT(struct __I2C_HandleTypeDef *hi2c, uint32_t ITFlags, uint32_t ITSources)
{
  uint16_t devaddress = 0;

  /* Process Locked */
  __HAL_LOCK(hi2c);

  if(((ITFlags & I2C_FLAG_AF) != RESET) && ((ITSources & I2C_IT_NACKI) != RESET))
  {
    /* Clear NACK Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

    /* Set corresponding Error Code */
    /* No need to generate STOP, it is automatically done */
    /* Error callback will be send during stop flag treatment */
    hi2c->ErrorCode |= HAL_I2C_ERROR_AF;

    /* Flush TX register */
    I2C_Flush_TXDR(hi2c);
  }
  else if(((ITFlags & I2C_FLAG_RXNE) != RESET) && ((ITSources & I2C_IT_RXI) != RESET))
  {
    /* Read data from RXDR */
    (*hi2c->pBuffPtr++) = hi2c->Instance->RXDR;
    hi2c->XferSize--;
    hi2c->XferCount--;
  }
  else if(((ITFlags & I2C_FLAG_TXIS) != RESET) && ((ITSources & I2C_IT_TXI) != RESET))
  {
    /* Write data to TXDR */
    hi2c->Instance->TXDR = (*hi2c->pBuffPtr++);
    hi2c->XferSize--;
    hi2c->XferCount--;
  }
  else if(((ITFlags & I2C_FLAG_TCR) != RESET) && ((ITSources & I2C_IT_TCI) != RESET))
  {
    if((hi2c->XferSize == 0) && (hi2c->XferCount != 0))
    {
      devaddress = (hi2c->Instance->CR2 & I2C_CR2_SADD);

      if(hi2c->XferCount > MAX_NBYTE_SIZE)
      {
        hi2c->XferSize = MAX_NBYTE_SIZE;
        I2C_TransferConfig(hi2c, devaddress, hi2c->XferSize, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
      }
      else
      {
        hi2c->XferSize = hi2c->XferCount;
        if(hi2c->XferOptions != I2C_NO_OPTION_FRAME)
        {
          I2C_TransferConfig(hi2c, devaddress, hi2c->XferSize, hi2c->XferOptions, I2C_NO_STARTSTOP);
        }
        else
        {
          I2C_TransferConfig(hi2c, devaddress, hi2c->XferSize, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
        }
      }
    }
    else
    {
      /* Call TxCpltCallback() if no stop mode is set */
      if((I2C_GET_STOP_MODE(hi2c) != I2C_AUTOEND_MODE)&&(hi2c->Mode == HAL_I2C_MODE_MASTER))
      {
        /* Call I2C Master Sequential complete process */
        I2C_ITMasterSequentialCplt(hi2c);
      }
      else
      {
        /* Wrong size Status regarding TCR flag event */
        /* Call the corresponding callback to inform upper layer of End of Transfer */
        I2C_ITError(hi2c, HAL_I2C_ERROR_SIZE);
      }
    }
  }
  else if(((ITFlags & I2C_FLAG_TC) != RESET) && ((ITSources & I2C_IT_TCI) != RESET))
  {
    if(hi2c->XferCount == 0)
    {
      if((I2C_GET_STOP_MODE(hi2c) != I2C_AUTOEND_MODE)&&(hi2c->Mode == HAL_I2C_MODE_MASTER))
      {
        /* Call I2C Master Sequential complete process */
        I2C_ITMasterSequentialCplt(hi2c);
      }
    }
    else
    {
      /* Wrong size Status regarding TC flag event */
      /* Call the corresponding callback to inform upper layer of End of Transfer */
      I2C_ITError(hi2c, HAL_I2C_ERROR_SIZE);
    }
  }

  if(((ITFlags & I2C_FLAG_STOPF) != RESET) && ((ITSources & I2C_IT_STOPI) != RESET))
  {
    /* Call I2C Master complete process */
    I2C_ITMasterCplt(hi2c, ITFlags);
  }

  /* Process Unlocked */
  __HAL_UNLOCK(hi2c);

  return HAL_OK;
}

/**
  * @brief  Interrupt Sub-Routine which handle the Interrupt Flags Slave Mode with Interrupt.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  ITFlags: Interrupt flags to handle.
  * @param  ITSources: Interrupt sources enabled.
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_Slave_ISR_IT(struct __I2C_HandleTypeDef *hi2c, uint32_t ITFlags, uint32_t ITSources)
{
  /* Process locked */
  __HAL_LOCK(hi2c);

  if(((ITFlags & I2C_FLAG_AF) != RESET) && ((ITSources & I2C_IT_NACKI) != RESET))
  {
    /* Check that I2C transfer finished */
    /* if yes, normal use case, a NACK is sent by the MASTER when Transfer is finished */
    /* Mean XferCount == 0*/
    /* So clear Flag NACKF only */
    if(hi2c->XferCount == 0)
    {
      if(((hi2c->XferOptions == I2C_FIRST_AND_LAST_FRAME) || (hi2c->XferOptions == I2C_LAST_FRAME)) && \
        (hi2c->State == HAL_I2C_STATE_LISTEN))
      {
        /* Call I2C Listen complete process */
        I2C_ITListenCplt(hi2c, ITFlags);
      }
      else if((hi2c->XferOptions != I2C_NO_OPTION_FRAME) && (hi2c->State == HAL_I2C_STATE_BUSY_TX_LISTEN))
      {
        /* Clear NACK Flag */
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

        /* Flush TX register */
        I2C_Flush_TXDR(hi2c);

        /* Last Byte is Transmitted */
        /* Call I2C Slave Sequential complete process */
        I2C_ITSlaveSequentialCplt(hi2c);
      }
      else
      {
        /* Clear NACK Flag */
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);
      }
    }
    else
    {
      /* if no, error use case, a Non-Acknowledge of last Data is generated by the MASTER*/
      /* Clear NACK Flag */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

      /* Set ErrorCode corresponding to a Non-Acknowledge */
      hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
    }
  }
  else if(((ITFlags & I2C_FLAG_RXNE) != RESET) && ((ITSources & I2C_IT_RXI) != RESET))
  {
    if(hi2c->XferCount > 0)
    {
      /* Read data from RXDR */
      (*hi2c->pBuffPtr++) = hi2c->Instance->RXDR;
      hi2c->XferSize--;
      hi2c->XferCount--;
    }

    if((hi2c->XferCount == 0) && \
       (hi2c->XferOptions != I2C_NO_OPTION_FRAME))
    {
      /* Call I2C Slave Sequential complete process */
      I2C_ITSlaveSequentialCplt(hi2c);
   }
  }
  else if(((ITFlags & I2C_FLAG_ADDR) != RESET) && ((ITSources & I2C_IT_ADDRI) != RESET))
  {
    I2C_ITAddrCplt(hi2c, ITFlags);
  }
  else if(((ITFlags & I2C_FLAG_TXIS) != RESET) && ((ITSources & I2C_IT_TXI) != RESET))
  {
    /* Write data to TXDR only if XferCount not reach "0" */
    /* A TXIS flag can be set, during STOP treatment      */
    /* Check if all Datas have already been sent */
    /* If it is the case, this last write in TXDR is not sent, correspond to a dummy TXIS event */
    if(hi2c->XferCount > 0)
    {
      /* Write data to TXDR */
      hi2c->Instance->TXDR = (*hi2c->pBuffPtr++);
      hi2c->XferCount--;
      hi2c->XferSize--;
    }
    else
    {
      if((hi2c->XferOptions == I2C_NEXT_FRAME) || (hi2c->XferOptions == I2C_FIRST_FRAME))
      {
        /* Last Byte is Transmitted */
        /* Call I2C Slave Sequential complete process */
        I2C_ITSlaveSequentialCplt(hi2c);
      }
    }
  }

  /* Check if STOPF is set */
  if(((ITFlags & I2C_FLAG_STOPF) != RESET) && ((ITSources & I2C_IT_STOPI) != RESET))
  {
    /* Call I2C Slave complete process */
    I2C_ITSlaveCplt(hi2c, ITFlags);
  }

  /* Process Unlocked */
  __HAL_UNLOCK(hi2c);

  return HAL_OK;
}

/**
  * @brief  Interrupt Sub-Routine which handle the Interrupt Flags Master Mode with DMA.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  ITFlags: Interrupt flags to handle.
  * @param  ITSources: Interrupt sources enabled.
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_Master_ISR_DMA(struct __I2C_HandleTypeDef *hi2c, uint32_t ITFlags, uint32_t ITSources)
{
  uint16_t devaddress = 0;
  uint32_t xfermode = 0;

  /* Process Locked */
  __HAL_LOCK(hi2c);

  if(((ITFlags & I2C_FLAG_AF) != RESET) && ((ITSources & I2C_IT_NACKI) != RESET))
  {
    /* Clear NACK Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

    /* Set corresponding Error Code */
    hi2c->ErrorCode |= HAL_I2C_ERROR_AF;

    /* No need to generate STOP, it is automatically done */
    /* But enable STOP interrupt, to treat it */
    /* Error callback will be send during stop flag treatment */
    I2C_Enable_IRQ(hi2c, I2C_XFER_CPLT_IT);

    /* Flush TX register */
    I2C_Flush_TXDR(hi2c);
  }
  else if(((ITFlags & I2C_FLAG_TCR) != RESET) && ((ITSources & I2C_IT_TCI) != RESET))
  {
    /* Disable TC interrupt */
    __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_TCI);

    if(hi2c->XferCount != 0)
    {
      /* Recover Slave address */
      devaddress = (hi2c->Instance->CR2 & I2C_CR2_SADD);

      /* Prepare the new XferSize to transfer */
      if(hi2c->XferCount > MAX_NBYTE_SIZE)
      {
        hi2c->XferSize = MAX_NBYTE_SIZE;
        xfermode = I2C_RELOAD_MODE;
      }
      else
      {
        hi2c->XferSize = hi2c->XferCount;
        xfermode = I2C_AUTOEND_MODE;
      }

      /* Set the new XferSize in Nbytes register */
      I2C_TransferConfig(hi2c, devaddress, hi2c->XferSize, xfermode, I2C_NO_STARTSTOP);

      /* Update XferCount value */
      hi2c->XferCount -= hi2c->XferSize;

      /* Enable DMA Request */
      if(hi2c->State == HAL_I2C_STATE_BUSY_RX)
      {
        hi2c->Instance->CR1 |= I2C_CR1_RXDMAEN;
      }
      else
      {
        hi2c->Instance->CR1 |= I2C_CR1_TXDMAEN;
      }
    }
    else
    {
      /* Wrong size Status regarding TCR flag event */
      /* Call the corresponding callback to inform upper layer of End of Transfer */
      I2C_ITError(hi2c, HAL_I2C_ERROR_SIZE);
    }
  }
  else if(((ITFlags & I2C_FLAG_STOPF) != RESET) && ((ITSources & I2C_IT_STOPI) != RESET))
  {
    /* Call I2C Master complete process */
    I2C_ITMasterCplt(hi2c, ITFlags);
  }

  /* Process Unlocked */
  __HAL_UNLOCK(hi2c);

  return HAL_OK;
}

/**
  * @brief  Interrupt Sub-Routine which handle the Interrupt Flags Slave Mode with DMA.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  ITFlags: Interrupt flags to handle.
  * @param  ITSources: Interrupt sources enabled.
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_Slave_ISR_DMA(struct __I2C_HandleTypeDef *hi2c, uint32_t ITFlags, uint32_t ITSources)
{
  /* Process locked */
  __HAL_LOCK(hi2c);

  if(((ITFlags & I2C_FLAG_AF) != RESET) && ((ITSources & I2C_IT_NACKI) != RESET))
  {
    /* Check that I2C transfer finished */
    /* if yes, normal use case, a NACK is sent by the MASTER when Transfer is finished */
    /* Mean XferCount == 0 */
    /* So clear Flag NACKF only */
    if(I2C_GET_DMA_REMAIN_DATA(hi2c) == 0)
    {
      /* Clear NACK Flag */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);
    }
    else
    {
      /* if no, error use case, a Non-Acknowledge of last Data is generated by the MASTER*/
      /* Clear NACK Flag */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

      /* Set ErrorCode corresponding to a Non-Acknowledge */
      hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
    }
  }
  else if(((ITFlags & I2C_FLAG_ADDR) != RESET) && ((ITSources & I2C_IT_ADDRI) != RESET))
  {
    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_ADDR);
  }
  else if(((ITFlags & I2C_FLAG_STOPF) != RESET) && ((ITSources & I2C_IT_STOPI) != RESET))
  {
    /* Call I2C Slave complete process */
    I2C_ITSlaveCplt(hi2c, ITFlags);
  }

  /* Process Unlocked */
  __HAL_UNLOCK(hi2c);

  return HAL_OK;
}

/**
  * @brief  Master sends target device address followed by internal memory address for write request.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  MemAddSize: Size of internal memory address
  * @param  Timeout: Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_RequestMemoryWrite(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout, uint32_t Tickstart)
{
  I2C_TransferConfig(hi2c,DevAddress,MemAddSize, I2C_RELOAD_MODE, I2C_GENERATE_START_WRITE);

  /* Wait until TXIS flag is set */
  if(I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout, Tickstart) != HAL_OK)
  {
    if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
    {
      return HAL_ERROR;
    }
    else
    {
      return HAL_TIMEOUT;
    }
  }

  /* If Memory address size is 8Bit */
  if(MemAddSize == I2C_MEMADD_SIZE_8BIT)
  {
    /* Send Memory Address */
    hi2c->Instance->TXDR = I2C_MEM_ADD_LSB(MemAddress);
  }
  /* If Memory address size is 16Bit */
  else
  {
    /* Send MSB of Memory Address */
    hi2c->Instance->TXDR = I2C_MEM_ADD_MSB(MemAddress);

    /* Wait until TXIS flag is set */
    if(I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout, Tickstart) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }

    /* Send LSB of Memory Address */
    hi2c->Instance->TXDR = I2C_MEM_ADD_LSB(MemAddress);
  }

  /* Wait until TCR flag is set */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TCR, RESET, Timeout, Tickstart) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

return HAL_OK;
}

/**
  * @brief  Master sends target device address followed by internal memory address for read request.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  MemAddSize: Size of internal memory address
  * @param  Timeout: Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_RequestMemoryRead(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout, uint32_t Tickstart)
{
  I2C_TransferConfig(hi2c,DevAddress,MemAddSize, I2C_SOFTEND_MODE, I2C_GENERATE_START_WRITE);

  /* Wait until TXIS flag is set */
  if(I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout, Tickstart) != HAL_OK)
  {
    if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
    {
      return HAL_ERROR;
    }
    else
    {
      return HAL_TIMEOUT;
    }
  }

  /* If Memory address size is 8Bit */
  if(MemAddSize == I2C_MEMADD_SIZE_8BIT)
  {
    /* Send Memory Address */
    hi2c->Instance->TXDR = I2C_MEM_ADD_LSB(MemAddress);
  }
  /* If Memory address size is 16Bit */
  else
  {
    /* Send MSB of Memory Address */
    hi2c->Instance->TXDR = I2C_MEM_ADD_MSB(MemAddress);

    /* Wait until TXIS flag is set */
    if(I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout, Tickstart) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }

    /* Send LSB of Memory Address */
    hi2c->Instance->TXDR = I2C_MEM_ADD_LSB(MemAddress);
  }

  /* Wait until TC flag is set */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TC, RESET, Timeout, Tickstart) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  return HAL_OK;
}

/**
  * @brief  I2C Address complete process callback.
  * @param  hi2c: I2C handle.
  * @param  ITFlags: Interrupt flags to handle.
  * @retval None
  */
static void I2C_ITAddrCplt(I2C_HandleTypeDef *hi2c, uint32_t ITFlags)
{
  uint8_t transferdirection = 0;
  uint16_t slaveaddrcode = 0;
  uint16_t ownadd1code = 0;
  uint16_t ownadd2code = 0;

  /* In case of Listen state, need to inform upper layer of address match code event */
  if((hi2c->State & HAL_I2C_STATE_LISTEN) == HAL_I2C_STATE_LISTEN)
  {
    transferdirection = I2C_GET_DIR(hi2c);
    slaveaddrcode     = I2C_GET_ADDR_MATCH(hi2c);
    ownadd1code       = I2C_GET_OWN_ADDRESS1(hi2c);
    ownadd2code       = I2C_GET_OWN_ADDRESS2(hi2c);

    /* If 10bits addressing mode is selected */
    if(hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_10BIT)
    {
      if((slaveaddrcode & SlaveAddr_MSK) == ((ownadd1code >> SlaveAddr_SHIFT) & SlaveAddr_MSK))
      {
        slaveaddrcode = ownadd1code;
        hi2c->AddrEventCount++;
        if(hi2c->AddrEventCount == 2)
        {
          /* Reset Address Event counter  */
          hi2c->AddrEventCount = 0;

          /* Clear ADDR flag */
          __HAL_I2C_CLEAR_FLAG(hi2c,I2C_FLAG_ADDR);

          /* Process Unlocked */
          __HAL_UNLOCK(hi2c);

          /* Call Slave Addr callback */
          HAL_I2C_AddrCallback(hi2c, transferdirection, slaveaddrcode);
        }
      }
      else
      {
        slaveaddrcode = ownadd2code;

        /* Disable ADDR Interrupts */
        I2C_Disable_IRQ(hi2c, I2C_XFER_LISTEN_IT);

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        /* Call Slave Addr callback */
        HAL_I2C_AddrCallback(hi2c, transferdirection, slaveaddrcode);
      }
    }
    /* else 7 bits addressing mode is selected */
    else
    {
      /* Disable ADDR Interrupts */
      I2C_Disable_IRQ(hi2c, I2C_XFER_LISTEN_IT);

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      /* Call Slave Addr callback */
      HAL_I2C_AddrCallback(hi2c, transferdirection, slaveaddrcode);
    }
  }
  /* Else clear address flag only */
  else
  {
    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_ADDR);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);
  }
}

/**
  * @brief  I2C Master sequential complete process.
  * @param  hi2c: I2C handle.
  * @retval None
  */
static void I2C_ITMasterSequentialCplt(I2C_HandleTypeDef *hi2c)
{
  /* Reset I2C handle mode */
  hi2c->Mode = HAL_I2C_MODE_NONE;

  /* No Generate Stop, to permit restart mode */
  /* The stop will be done at the end of transfer, when I2C_AUTOEND_MODE enable */
  if (hi2c->State == HAL_I2C_STATE_BUSY_TX)
  {
    hi2c->State         = HAL_I2C_STATE_READY;
    hi2c->PreviousState = I2C_STATE_MASTER_BUSY_TX;
    hi2c->XferISR       = NULL;

    /* Disable Interrupts */
    I2C_Disable_IRQ(hi2c, I2C_XFER_TX_IT);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Call the corresponding callback to inform upper layer of End of Transfer */
    HAL_I2C_MasterTxCpltCallback(hi2c);
  }
  /* hi2c->State == HAL_I2C_STATE_BUSY_RX */
  else
  {
    hi2c->State         = HAL_I2C_STATE_READY;
    hi2c->PreviousState = I2C_STATE_MASTER_BUSY_RX;
    hi2c->XferISR       = NULL;

    /* Disable Interrupts */
    I2C_Disable_IRQ(hi2c, I2C_XFER_RX_IT);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Call the corresponding callback to inform upper layer of End of Transfer */
    HAL_I2C_MasterRxCpltCallback(hi2c);
  }
}

/**
  * @brief  I2C Slave sequential complete process.
  * @param  hi2c: I2C handle.
  * @retval None
  */
static void I2C_ITSlaveSequentialCplt(I2C_HandleTypeDef *hi2c)
{
  /* Reset I2C handle mode */
  hi2c->Mode = HAL_I2C_MODE_NONE;

  if(hi2c->State == HAL_I2C_STATE_BUSY_TX_LISTEN)
  {
    /* Remove HAL_I2C_STATE_SLAVE_BUSY_TX, keep only HAL_I2C_STATE_LISTEN */
    hi2c->State         = HAL_I2C_STATE_LISTEN;
    hi2c->PreviousState = I2C_STATE_SLAVE_BUSY_TX;

    /* Disable Interrupts */
    I2C_Disable_IRQ(hi2c, I2C_XFER_TX_IT);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Call the Tx complete callback to inform upper layer of the end of transmit process */
    HAL_I2C_SlaveTxCpltCallback(hi2c);
  }

  else if(hi2c->State == HAL_I2C_STATE_BUSY_RX_LISTEN)
  {
    /* Remove HAL_I2C_STATE_SLAVE_BUSY_RX, keep only HAL_I2C_STATE_LISTEN */
    hi2c->State         = HAL_I2C_STATE_LISTEN;
    hi2c->PreviousState = I2C_STATE_SLAVE_BUSY_RX;

    /* Disable Interrupts */
    I2C_Disable_IRQ(hi2c, I2C_XFER_RX_IT);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Call the Rx complete callback to inform upper layer of the end of receive process */
    HAL_I2C_SlaveRxCpltCallback(hi2c);
  }
}

/**
  * @brief  I2C Master complete process.
  * @param  hi2c: I2C handle.
  * @param  ITFlags: Interrupt flags to handle.
  * @retval None
  */
static void I2C_ITMasterCplt(I2C_HandleTypeDef *hi2c, uint32_t ITFlags)
{
  /* Clear STOP Flag */
  __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

  /* Clear Configuration Register 2 */
  I2C_RESET_CR2(hi2c);

  /* Reset handle parameters */
  hi2c->PreviousState = I2C_STATE_NONE;
  hi2c->XferISR       = NULL;
  hi2c->XferOptions   = I2C_NO_OPTION_FRAME;

  if((ITFlags & I2C_FLAG_AF) != RESET)
  {
    /* Clear NACK Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

    /* Set acknowledge error code */
    hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
  }

  /* Flush TX register */
  I2C_Flush_TXDR(hi2c);

  /* Disable Interrupts */
  I2C_Disable_IRQ(hi2c, I2C_XFER_TX_IT| I2C_XFER_RX_IT);

  /* Call the corresponding callback to inform upper layer of End of Transfer */
  if(hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
  {
    /* Call the corresponding callback to inform upper layer of End of Transfer */
    I2C_ITError(hi2c, hi2c->ErrorCode);
  }
  /* hi2c->State == HAL_I2C_STATE_BUSY_TX */
  else if(hi2c->State == HAL_I2C_STATE_BUSY_TX)
  {
    hi2c->State = HAL_I2C_STATE_READY;

    if (hi2c->Mode == HAL_I2C_MODE_MEM)
    {
      hi2c->Mode = HAL_I2C_MODE_NONE;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      /* Call the corresponding callback to inform upper layer of End of Transfer */
      HAL_I2C_MemTxCpltCallback(hi2c);
    }
    else
    {
      hi2c->Mode = HAL_I2C_MODE_NONE;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      /* Call the corresponding callback to inform upper layer of End of Transfer */
      HAL_I2C_MasterTxCpltCallback(hi2c);
    }
  }
  /* hi2c->State == HAL_I2C_STATE_BUSY_RX */
  else if(hi2c->State == HAL_I2C_STATE_BUSY_RX)
  {
    hi2c->State = HAL_I2C_STATE_READY;

    if (hi2c->Mode == HAL_I2C_MODE_MEM)
    {
      hi2c->Mode = HAL_I2C_MODE_NONE;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      HAL_I2C_MemRxCpltCallback(hi2c);
    }
    else
    {
      hi2c->Mode = HAL_I2C_MODE_NONE;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      HAL_I2C_MasterRxCpltCallback(hi2c);
    }
  }
}

/**
  * @brief  I2C Slave complete process.
  * @param  hi2c: I2C handle.
  * @param  ITFlags: Interrupt flags to handle.
  * @retval None
  */
static void I2C_ITSlaveCplt(I2C_HandleTypeDef *hi2c, uint32_t ITFlags)
{
  /* Clear STOP Flag */
  __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

  /* Clear ADDR flag */
  __HAL_I2C_CLEAR_FLAG(hi2c,I2C_FLAG_ADDR);

  /* Disable all interrupts */
  I2C_Disable_IRQ(hi2c, I2C_XFER_LISTEN_IT | I2C_XFER_TX_IT | I2C_XFER_RX_IT);

  /* Disable Address Acknowledge */
  hi2c->Instance->CR2 |= I2C_CR2_NACK;

  /* Clear Configuration Register 2 */
  I2C_RESET_CR2(hi2c);

  /* Flush TX register */
  I2C_Flush_TXDR(hi2c);

  /* If a DMA is ongoing, Update handle size context */
  if(((hi2c->Instance->CR1 & I2C_CR1_TXDMAEN) == I2C_CR1_TXDMAEN) ||
     ((hi2c->Instance->CR1 & I2C_CR1_RXDMAEN) == I2C_CR1_RXDMAEN))
  {
    if((hi2c->XferSize - I2C_GET_DMA_REMAIN_DATA(hi2c)) != hi2c->XferSize)
    {
      hi2c->XferSize = I2C_GET_DMA_REMAIN_DATA(hi2c);
      hi2c->XferCount += hi2c->XferSize;

      /* Set ErrorCode corresponding to a Non-Acknowledge */
      hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
    }
  }

  /* Store Last receive data if any */
  if(((ITFlags & I2C_FLAG_RXNE) != RESET))
  {
    /* Read data from RXDR */
    (*hi2c->pBuffPtr++) = hi2c->Instance->RXDR;

    if((hi2c->XferSize > 0))
    {
      hi2c->XferSize--;
      hi2c->XferCount--;

      /* Set ErrorCode corresponding to a Non-Acknowledge */
      hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
    }
  }

  hi2c->PreviousState = I2C_STATE_NONE;
  hi2c->Mode = HAL_I2C_MODE_NONE;
  hi2c->XferISR = NULL;

  if(hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
  {
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;
    hi2c->State = HAL_I2C_STATE_READY;

    /* Call the corresponding callback to inform upper layer of End of Transfer */
    I2C_ITError(hi2c, hi2c->ErrorCode);
  }
  else if(hi2c->XferOptions != I2C_NO_OPTION_FRAME)
  {
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;
    hi2c->State = HAL_I2C_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Call the Listen Complete callback, to inform upper layer of the end of Listen usecase */
    HAL_I2C_ListenCpltCallback(hi2c);
  }
  /* Call the corresponding callback to inform upper layer of End of Transfer */
  else if(hi2c->State == HAL_I2C_STATE_BUSY_RX)
  {
    hi2c->State = HAL_I2C_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Call the Slave Rx Complete callback */
    HAL_I2C_SlaveRxCpltCallback(hi2c);
  }
  else
  {
    hi2c->State = HAL_I2C_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Call the Slave Tx Complete callback */
    HAL_I2C_SlaveTxCpltCallback(hi2c);
  }
}

/**
  * @brief  I2C Listen complete process.
  * @param  hi2c: I2C handle.
  * @param  ITFlags: Interrupt flags to handle.
  * @retval None
  */
static void I2C_ITListenCplt(I2C_HandleTypeDef *hi2c, uint32_t ITFlags)
{
  /* Reset handle parameters */
  hi2c->XferOptions = I2C_NO_OPTION_FRAME;
  hi2c->PreviousState = I2C_STATE_NONE;
  hi2c->State = HAL_I2C_STATE_READY;
  hi2c->Mode = HAL_I2C_MODE_NONE;
  hi2c->XferISR = NULL;

  /* Store Last receive data if any */
  if(((ITFlags & I2C_FLAG_RXNE) != RESET))
  {
    /* Read data from RXDR */
    (*hi2c->pBuffPtr++) = hi2c->Instance->RXDR;

    if((hi2c->XferSize > 0))
    {
      hi2c->XferSize--;
      hi2c->XferCount--;

      /* Set ErrorCode corresponding to a Non-Acknowledge */
      hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
    }
  }

  /* Disable all Interrupts*/
  I2C_Disable_IRQ(hi2c, I2C_XFER_LISTEN_IT | I2C_XFER_RX_IT | I2C_XFER_TX_IT);

  /* Clear NACK Flag */
  __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

  /* Process Unlocked */
  __HAL_UNLOCK(hi2c);

  /* Call the Listen Complete callback, to inform upper layer of the end of Listen usecase */
  HAL_I2C_ListenCpltCallback(hi2c);
}

/**
  * @brief  I2C interrupts error process.
  * @param  hi2c: I2C handle.
  * @param  ErrorCode: Error code to handle.
  * @retval None
  */
static void I2C_ITError(I2C_HandleTypeDef *hi2c, uint32_t ErrorCode)
{
  /* Reset handle parameters */
  hi2c->Mode          = HAL_I2C_MODE_NONE;
  hi2c->XferOptions   = I2C_NO_OPTION_FRAME;
  hi2c->XferCount     = 0;

  /* Set new error code */
  hi2c->ErrorCode |= ErrorCode;

  /* Disable Interrupts */
  if((hi2c->State == HAL_I2C_STATE_LISTEN)         ||
     (hi2c->State == HAL_I2C_STATE_BUSY_TX_LISTEN) ||
     (hi2c->State == HAL_I2C_STATE_BUSY_RX_LISTEN))
  {
    /* Disable all interrupts, except interrupts related to LISTEN state */
    I2C_Disable_IRQ(hi2c, I2C_XFER_RX_IT | I2C_XFER_TX_IT);

    /* keep HAL_I2C_STATE_LISTEN if set */
    hi2c->State         = HAL_I2C_STATE_LISTEN;
    hi2c->PreviousState = I2C_STATE_NONE;
    hi2c->XferISR       = I2C_Slave_ISR_IT;
  }
  else
  {
    /* Disable all interrupts */
    I2C_Disable_IRQ(hi2c, I2C_XFER_LISTEN_IT | I2C_XFER_RX_IT | I2C_XFER_TX_IT);

    /* Set HAL_I2C_STATE_READY */
    hi2c->State         = HAL_I2C_STATE_READY;
    hi2c->PreviousState = I2C_STATE_NONE;
    hi2c->XferISR       = NULL;
  }

  /* Abort DMA TX transfer if any */
  if((hi2c->Instance->CR1 & I2C_CR1_TXDMAEN) == I2C_CR1_TXDMAEN)
  {
    hi2c->Instance->CR1 &= ~I2C_CR1_TXDMAEN;

    /* Set the I2C DMA Abort callback :
       will lead to call HAL_I2C_ErrorCallback() at end of DMA abort procedure */
    hi2c->hdmatx->XferAbortCallback = I2C_DMAAbort;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Abort DMA TX */
    if(HAL_DMA_Abort_IT(hi2c->hdmatx) != HAL_OK)
    {
      /* Call Directly XferAbortCallback function in case of error */
      hi2c->hdmatx->XferAbortCallback(hi2c->hdmatx);
    }
  }
  /* Abort DMA RX transfer if any */
  else if((hi2c->Instance->CR1 & I2C_CR1_RXDMAEN) == I2C_CR1_RXDMAEN)
  {
    hi2c->Instance->CR1 &= ~I2C_CR1_RXDMAEN;

    /* Set the I2C DMA Abort callback :
       will lead to call HAL_I2C_ErrorCallback() at end of DMA abort procedure */
    hi2c->hdmarx->XferAbortCallback = I2C_DMAAbort;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Abort DMA RX */
    if(HAL_DMA_Abort_IT(hi2c->hdmarx) != HAL_OK)
    {
      /* Call Directly hi2c->hdmarx->XferAbortCallback function in case of error */
      hi2c->hdmarx->XferAbortCallback(hi2c->hdmarx);
    }
  }
  else if(hi2c->ErrorCode == HAL_I2C_ERROR_ABORT)
  {
    hi2c->ErrorCode &= ~HAL_I2C_ERROR_ABORT;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Call the corresponding callback to inform upper layer of End of Transfer */
    HAL_I2C_AbortCpltCallback(hi2c);
  }
  else
  {
    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Call the corresponding callback to inform upper layer of End of Transfer */
    HAL_I2C_ErrorCallback(hi2c);
  }
}

/**
  * @brief  I2C Tx data register flush process.
  * @param  hi2c: I2C handle.
  * @retval None
  */
static void I2C_Flush_TXDR(I2C_HandleTypeDef *hi2c)
{
  /* If a pending TXIS flag is set */
  /* Write a dummy data in TXDR to clear it */
  if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXIS) != RESET)
  {
     hi2c->Instance->TXDR = 0x00;
  }

  /* Flush TX register if not empty */
  if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXE) == RESET)
  {
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_TXE);
  }
}

/**
  * @brief  DMA I2C master transmit process complete callback.
  * @param  hdma: DMA handle
  * @retval None
  */
static void I2C_DMAMasterTransmitCplt(DMA_HandleTypeDef *hdma)
{
  I2C_HandleTypeDef* hi2c = (I2C_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  /* Disable DMA Request */
  hi2c->Instance->CR1 &= ~I2C_CR1_TXDMAEN;

  /* If last transfer, enable STOP interrupt */
  if(hi2c->XferCount == 0)
  {
    /* Enable STOP interrupt */
    I2C_Enable_IRQ(hi2c, I2C_XFER_CPLT_IT);
  }
  /* else prepare a new DMA transfer and enable TCReload interrupt */
  else
  {
    /* Update Buffer pointer */
    hi2c->pBuffPtr += hi2c->XferSize;

    /* Set the XferSize to transfer */
    if(hi2c->XferCount > MAX_NBYTE_SIZE)
    {
      hi2c->XferSize = MAX_NBYTE_SIZE;
    }
    else
    {
      hi2c->XferSize = hi2c->XferCount;
    }

    /* Enable the DMA channel */
    HAL_DMA_Start_IT(hi2c->hdmatx, (uint32_t)hi2c->pBuffPtr, (uint32_t)&hi2c->Instance->TXDR, hi2c->XferSize);

    /* Enable TC interrupts */
    I2C_Enable_IRQ(hi2c, I2C_XFER_RELOAD_IT);
  }
}

/**
  * @brief  DMA I2C slave transmit process complete callback.
  * @param  hdma: DMA handle
  * @retval None
  */
static void I2C_DMASlaveTransmitCplt(DMA_HandleTypeDef *hdma)
{
  /* No specific action, Master fully manage the generation of STOP condition */
  /* Mean that this generation can arrive at any time, at the end or during DMA process */
  /* So STOP condition should be manage through Interrupt treatment */
}

/**
  * @brief DMA I2C master receive process complete callback.
  * @param  hdma: DMA handle
  * @retval None
  */
static void I2C_DMAMasterReceiveCplt(DMA_HandleTypeDef *hdma)
{
  I2C_HandleTypeDef* hi2c = (I2C_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  /* Disable DMA Request */
  hi2c->Instance->CR1 &= ~I2C_CR1_RXDMAEN;

  /* If last transfer, enable STOP interrupt */
  if(hi2c->XferCount == 0)
  {
    /* Enable STOP interrupt */
    I2C_Enable_IRQ(hi2c, I2C_XFER_CPLT_IT);
  }
  /* else prepare a new DMA transfer and enable TCReload interrupt */
  else
  {
    /* Update Buffer pointer */
    hi2c->pBuffPtr += hi2c->XferSize;

    /* Set the XferSize to transfer */
    if(hi2c->XferCount > MAX_NBYTE_SIZE)
    {
      hi2c->XferSize = MAX_NBYTE_SIZE;
    }
    else
    {
      hi2c->XferSize = hi2c->XferCount;
    }

    /* Enable the DMA channel */
    HAL_DMA_Start_IT(hi2c->hdmarx, (uint32_t)&hi2c->Instance->RXDR, (uint32_t)hi2c->pBuffPtr, hi2c->XferSize);

    /* Enable TC interrupts */
    I2C_Enable_IRQ(hi2c, I2C_XFER_RELOAD_IT);
  }
}

/**
  * @brief  DMA I2C slave receive process complete callback.
  * @param hdma: DMA handle
  * @retval None
  */
static void I2C_DMASlaveReceiveCplt(DMA_HandleTypeDef *hdma)
{
  /* No specific action, Master fully manage the generation of STOP condition */
  /* Mean that this generation can arrive at any time, at the end or during DMA process */
  /* So STOP condition should be manage through Interrupt treatment */
}

/**
  * @brief  DMA I2C communication error callback.
  * @param hdma: DMA handle
  * @retval None
  */
static void I2C_DMAError(DMA_HandleTypeDef *hdma)
{
  I2C_HandleTypeDef* hi2c = ( I2C_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  /* Disable Acknowledge */
  hi2c->Instance->CR2 |= I2C_CR2_NACK;

  /* Call the corresponding callback to inform upper layer of End of Transfer */
  I2C_ITError(hi2c, HAL_I2C_ERROR_DMA);
}

/**
  * @brief DMA I2C communication abort callback
  *        (To be called at end of DMA Abort procedure).
  * @param hdma: DMA handle.
  * @retval None
  */
static void I2C_DMAAbort(DMA_HandleTypeDef *hdma)
{
  I2C_HandleTypeDef* hi2c = ( I2C_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  /* Disable Acknowledge */
  hi2c->Instance->CR2 |= I2C_CR2_NACK;

  /* Reset AbortCpltCallback */
  hi2c->hdmatx->XferAbortCallback = NULL;
  hi2c->hdmarx->XferAbortCallback = NULL;

  /* Check if come from abort from user */
  if(hi2c->ErrorCode == HAL_I2C_ERROR_ABORT)
  {
    hi2c->ErrorCode &= ~HAL_I2C_ERROR_ABORT;

    /* Call the corresponding callback to inform upper layer of End of Transfer */
    HAL_I2C_AbortCpltCallback(hi2c);
  }
  else
  {
    /* Call the corresponding callback to inform upper layer of End of Transfer */
    HAL_I2C_ErrorCallback(hi2c);
  }
}

/**
  * @brief  This function handles I2C Communication Timeout.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  Flag: Specifies the I2C flag to check.
  * @param  Status: The new Flag status (SET or RESET).
  * @param  Timeout: Timeout duration
  * @param  Tickstart: Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, FlagStatus Status, uint32_t Timeout, uint32_t Tickstart)
{
  while((__HAL_I2C_GET_FLAG(hi2c, Flag) ? SET : RESET) == Status)
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0)||((HAL_GetTick() - Tickstart ) > Timeout))
      {
        hi2c->State= HAL_I2C_STATE_READY;
        hi2c->Mode = HAL_I2C_MODE_NONE;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }
  }
  return HAL_OK;
}

/**
  * @brief  This function handles I2C Communication Timeout for specific usage of TXIS flag.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  Timeout: Timeout duration
  * @param  Tickstart: Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnTXISFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart)
{
  while(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXIS) == RESET)
  {
    /* Check if a NACK is detected */
    if(I2C_IsAcknowledgeFailed(hi2c, Timeout, Tickstart) != HAL_OK)
    {
      return HAL_ERROR;
    }

    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0)||((HAL_GetTick() - Tickstart) > Timeout))
      {
        hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
        hi2c->State= HAL_I2C_STATE_READY;
        hi2c->Mode = HAL_I2C_MODE_NONE;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_TIMEOUT;
      }
    }
  }
  return HAL_OK;
}

/**
  * @brief  This function handles I2C Communication Timeout for specific usage of STOP flag.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  Timeout: Timeout duration
  * @param  Tickstart: Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnSTOPFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart)
{
  while(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == RESET)
  {
    /* Check if a NACK is detected */
    if(I2C_IsAcknowledgeFailed(hi2c, Timeout, Tickstart) != HAL_OK)
    {
      return HAL_ERROR;
    }

    /* Check for the Timeout */
    if((Timeout == 0)||((HAL_GetTick() - Tickstart) > Timeout))
    {
      hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
      hi2c->State= HAL_I2C_STATE_READY;
      hi2c->Mode = HAL_I2C_MODE_NONE;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      return HAL_TIMEOUT;
    }
  }
  return HAL_OK;
}

/**
  * @brief  This function handles I2C Communication Timeout for specific usage of RXNE flag.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  Timeout: Timeout duration
  * @param  Tickstart: Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnRXNEFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart)
{
  while(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE) == RESET)
  {
    /* Check if a NACK is detected */
    if(I2C_IsAcknowledgeFailed(hi2c, Timeout, Tickstart) != HAL_OK)
    {
      return HAL_ERROR;
    }

    /* Check if a STOPF is detected */
    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == SET)
    {
      /* Clear STOP Flag */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

      /* Clear Configuration Register 2 */
      I2C_RESET_CR2(hi2c);

      hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
      hi2c->State= HAL_I2C_STATE_READY;
      hi2c->Mode = HAL_I2C_MODE_NONE;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      return HAL_ERROR;
    }

    /* Check for the Timeout */
    if((Timeout == 0)||((HAL_GetTick() - Tickstart) > Timeout))
    {
      hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
      hi2c->State= HAL_I2C_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      return HAL_TIMEOUT;
    }
  }
  return HAL_OK;
}

/**
  * @brief  This function handles Acknowledge failed detection during an I2C Communication.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  Timeout: Timeout duration
  * @param  Tickstart: Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_IsAcknowledgeFailed(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart)
{
  if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET)
  {
    /* Wait until STOP Flag is reset */
    /* AutoEnd should be initiate after AF */
    while(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == RESET)
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
      if((Timeout == 0)||((HAL_GetTick() - Tickstart) > Timeout))
        {
          hi2c->State= HAL_I2C_STATE_READY;
          hi2c->Mode = HAL_I2C_MODE_NONE;

          /* Process Unlocked */
          __HAL_UNLOCK(hi2c);
          return HAL_TIMEOUT;
        }
      }
    }

    /* Clear NACKF Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    /* Flush TX register */
    I2C_Flush_TXDR(hi2c);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2(hi2c);

    hi2c->ErrorCode = HAL_I2C_ERROR_AF;
    hi2c->State= HAL_I2C_STATE_READY;
    hi2c->Mode = HAL_I2C_MODE_NONE;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_ERROR;
  }
  return HAL_OK;
}

/**
  * @brief  Handles I2Cx communication when starting transfer or during transfer (TC or TCR flag are set).
  * @param  hi2c: I2C handle.
  * @param  DevAddress: Specifies the slave address to be programmed.
  * @param  Size: Specifies the number of bytes to be programmed.
  *   This parameter must be a value between 0 and 255.
  * @param  Mode: New state of the I2C START condition generation.
  *   This parameter can be a value of @ref I2C_RELOAD_END_MODE.
  * @param  Request: New state of the I2C START condition generation.
  *   This parameter can be a value of I2C_START_STOP_MODE.
  * @retval None
  */
static void I2C_TransferConfig(I2C_HandleTypeDef *hi2c,  uint16_t DevAddress, uint8_t Size, uint32_t Mode, uint32_t Request)
{
  uint32_t tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_I2C_ALL_INSTANCE(hi2c->Instance));
  assert_param(IS_TRANSFER_MODE(Mode));
  assert_param(IS_TRANSFER_REQUEST(Request));

  /* Get the CR2 register value */
  tmpreg = hi2c->Instance->CR2;

  /* clear tmpreg specific bits */
  tmpreg &= (uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP));

  /* update tmpreg */
  tmpreg |= (uint32_t)(((uint32_t)DevAddress & I2C_CR2_SADD) | (((uint32_t)Size << 16 ) & I2C_CR2_NBYTES) | \
            (uint32_t)Mode | (uint32_t)Request);

  /* update CR2 register */
  hi2c->Instance->CR2 = tmpreg;
}

/**
  * @brief  Manage the enabling of Interrupts.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  InterruptRequest: Value of @ref I2C_Interrupt_configuration_definition.
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_Enable_IRQ(I2C_HandleTypeDef *hi2c, uint16_t InterruptRequest)
{
  uint32_t tmpisr = 0;

  if((hi2c->XferISR == I2C_Master_ISR_DMA) || \
     (hi2c->XferISR == I2C_Slave_ISR_DMA))
  {
    if((InterruptRequest & I2C_XFER_LISTEN_IT) == I2C_XFER_LISTEN_IT)
    {
      /* Enable ERR, STOP, NACK and ADDR interrupts */
      tmpisr |= I2C_IT_ADDRI | I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_ERRI;
    }

    if((InterruptRequest & I2C_XFER_ERROR_IT) == I2C_XFER_ERROR_IT)
    {
      /* Enable ERR and NACK interrupts */
      tmpisr |= I2C_IT_ERRI | I2C_IT_NACKI;
    }

    if((InterruptRequest & I2C_XFER_CPLT_IT) == I2C_XFER_CPLT_IT)
    {
      /* Enable STOP interrupts */
      tmpisr |= I2C_IT_STOPI;
    }

    if((InterruptRequest & I2C_XFER_RELOAD_IT) == I2C_XFER_RELOAD_IT)
    {
      /* Enable TC interrupts */
      tmpisr |= I2C_IT_TCI;
    }
  }
  else
  {
    if((InterruptRequest & I2C_XFER_LISTEN_IT) == I2C_XFER_LISTEN_IT)
    {
      /* Enable ERR, STOP, NACK, and ADDR interrupts */
      tmpisr |= I2C_IT_ADDRI | I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_ERRI;
    }

    if((InterruptRequest & I2C_XFER_TX_IT) == I2C_XFER_TX_IT)
    {
      /* Enable ERR, TC, STOP, NACK and RXI interrupts */
      tmpisr |= I2C_IT_ERRI | I2C_IT_TCI | I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_TXI;
    }

    if((InterruptRequest & I2C_XFER_RX_IT) == I2C_XFER_RX_IT)
    {
      /* Enable ERR, TC, STOP, NACK and TXI interrupts */
      tmpisr |= I2C_IT_ERRI | I2C_IT_TCI | I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_RXI;
    }

    if((InterruptRequest & I2C_XFER_CPLT_IT) == I2C_XFER_CPLT_IT)
    {
      /* Enable STOP interrupts */
      tmpisr |= I2C_IT_STOPI;
    }
  }

  /* Enable interrupts only at the end */
  /* to avoid the risk of I2C interrupt handle execution before */
  /* all interrupts requested done */
  __HAL_I2C_ENABLE_IT(hi2c, tmpisr);

  return HAL_OK;
}

/**
  * @brief  Manage the disabling of Interrupts.
  * @param  hi2c: Pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C.
  * @param  InterruptRequest: Value of @ref I2C_Interrupt_configuration_definition.
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_Disable_IRQ(I2C_HandleTypeDef *hi2c, uint16_t InterruptRequest)
{
  uint32_t tmpisr = 0;

  if((InterruptRequest & I2C_XFER_TX_IT) == I2C_XFER_TX_IT)
  {
    /* Disable TC and TXI interrupts */
    tmpisr |= I2C_IT_TCI | I2C_IT_TXI;

    if((hi2c->State & HAL_I2C_STATE_LISTEN) != HAL_I2C_STATE_LISTEN)
    {
      /* Disable NACK and STOP interrupts */
      tmpisr |= I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_ERRI;
    }
  }

  if((InterruptRequest & I2C_XFER_RX_IT) == I2C_XFER_RX_IT)
  {
    /* Disable TC and RXI interrupts */
    tmpisr |= I2C_IT_TCI | I2C_IT_RXI;

    if((hi2c->State & HAL_I2C_STATE_LISTEN) != HAL_I2C_STATE_LISTEN)
    {
      /* Disable NACK and STOP interrupts */
      tmpisr |= I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_ERRI;
    }
  }

  if((InterruptRequest & I2C_XFER_LISTEN_IT) == I2C_XFER_LISTEN_IT)
  {
    /* Disable ADDR, NACK and STOP interrupts */
    tmpisr |= I2C_IT_ADDRI | I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_ERRI;
  }

  if((InterruptRequest & I2C_XFER_ERROR_IT) == I2C_XFER_ERROR_IT)
  {
    /* Enable ERR and NACK interrupts */
    tmpisr |= I2C_IT_ERRI | I2C_IT_NACKI;
  }

  if((InterruptRequest & I2C_XFER_CPLT_IT) == I2C_XFER_CPLT_IT)
  {
    /* Enable STOP interrupts */
    tmpisr |= I2C_IT_STOPI;
  }

  if((InterruptRequest & I2C_XFER_RELOAD_IT) == I2C_XFER_RELOAD_IT)
  {
    /* Enable TC interrupts */
    tmpisr |= I2C_IT_TCI;
  }

  /* Disable interrupts only at the end */
  /* to avoid a breaking situation like at "t" time */
  /* all disable interrupts request are not done */
  __HAL_I2C_DISABLE_IT(hi2c, tmpisr);

  return HAL_OK;
}

/**
  * @}
  */

#endif /* HAL_I2C_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

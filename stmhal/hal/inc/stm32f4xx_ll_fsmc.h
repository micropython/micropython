/**
  ******************************************************************************
  * @file    stm32f4xx_ll_fsmc.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   Header file of FSMC HAL module.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_LL_FSMC_H
#define __STM32F4xx_LL_FSMC_H

#ifdef __cplusplus
 extern "C" {
#endif

#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx)

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup FSMC
  * @{
  */ 

/* Exported typedef ----------------------------------------------------------*/ 
#define FSMC_NORSRAM_TypeDef            FSMC_Bank1_TypeDef
#define FSMC_NORSRAM_EXTENDED_TypeDef   FSMC_Bank1E_TypeDef
#define FSMC_NAND_TypeDef               FSMC_Bank2_3_TypeDef
#define FSMC_PCCARD_TypeDef             FSMC_Bank4_TypeDef

#define FSMC_NORSRAM_DEVICE             FSMC_Bank1            
#define FSMC_NORSRAM_EXTENDED_DEVICE    FSMC_Bank1E   
#define FSMC_NAND_DEVICE                FSMC_Bank2_3             
#define FSMC_PCCARD_DEVICE              FSMC_Bank4    

/** 
  * @brief  FSMC_NORSRAM Configuration Structure definition  
  */ 
typedef struct
{
  uint32_t NSBank;                       /*!< Specifies the NORSRAM memory device that will be used.
                                              This parameter can be a value of @ref FSMC_NORSRAM_Bank                     */  
                                                    
  uint32_t DataAddressMux;               /*!< Specifies whether the address and data values are
                                              multiplexed on the data bus or not. 
                                              This parameter can be a value of @ref FSMC_Data_Address_Bus_Multiplexing    */
  
  uint32_t MemoryType;                   /*!< Specifies the type of external memory attached to
                                              the corresponding memory device.
                                              This parameter can be a value of @ref FSMC_Memory_Type                      */
                                              
  uint32_t MemoryDataWidth;              /*!< Specifies the external memory device width.
                                              This parameter can be a value of @ref FSMC_NORSRAM_Data_Width               */
  
  uint32_t BurstAccessMode;              /*!< Enables or disables the burst access mode for Flash memory,
                                              valid only with synchronous burst Flash memories.
                                              This parameter can be a value of @ref FSMC_Burst_Access_Mode                */
                                               
  uint32_t WaitSignalPolarity;           /*!< Specifies the wait signal polarity, valid only when accessing
                                              the Flash memory in burst mode.
                                              This parameter can be a value of @ref FSMC_Wait_Signal_Polarity             */
  
  uint32_t WrapMode;                     /*!< Enables or disables the Wrapped burst access mode for Flash
                                              memory, valid only when accessing Flash memories in burst mode.
                                              This parameter can be a value of @ref FSMC_Wrap_Mode                        */
  
  uint32_t WaitSignalActive;             /*!< Specifies if the wait signal is asserted by the memory one
                                              clock cycle before the wait state or during the wait state,
                                              valid only when accessing memories in burst mode. 
                                              This parameter can be a value of @ref FSMC_Wait_Timing                      */
  
  uint32_t WriteOperation;               /*!< Enables or disables the write operation in the selected device by the FSMC. 
                                              This parameter can be a value of @ref FSMC_Write_Operation                  */
  
  uint32_t WaitSignal;                   /*!< Enables or disables the wait state insertion via wait
                                              signal, valid for Flash memory access in burst mode. 
                                              This parameter can be a value of @ref FSMC_Wait_Signal                      */
  
  uint32_t ExtendedMode;                 /*!< Enables or disables the extended mode.
                                              This parameter can be a value of @ref FSMC_Extended_Mode                    */
  
  uint32_t AsynchronousWait;             /*!< Enables or disables wait signal during asynchronous transfers,
                                              valid only with asynchronous Flash memories.
                                              This parameter can be a value of @ref FSMC_AsynchronousWait                 */
  
  uint32_t WriteBurst;                   /*!< Enables or disables the write burst operation.
                                              This parameter can be a value of @ref FSMC_Write_Burst                      */                                     

}FSMC_NORSRAM_InitTypeDef;


/** 
  * @brief  FSMC_NORSRAM Timing parameters structure definition  
  */
typedef struct
{
  uint32_t AddressSetupTime;             /*!< Defines the number of HCLK cycles to configure
                                              the duration of the address setup time. 
                                              This parameter can be a value between Min_Data = 0 and Max_Data = 15.
                                              @note This parameter is not used with synchronous NOR Flash memories.      */
  
  uint32_t AddressHoldTime;              /*!< Defines the number of HCLK cycles to configure
                                              the duration of the address hold time.
                                              This parameter can be a value between Min_Data = 1 and Max_Data = 15. 
                                              @note This parameter is not used with synchronous NOR Flash memories.      */
  
  uint32_t DataSetupTime;                /*!< Defines the number of HCLK cycles to configure
                                              the duration of the data setup time.
                                              This parameter can be a value between Min_Data = 1 and Max_Data = 255.
                                              @note This parameter is used for SRAMs, ROMs and asynchronous multiplexed 
                                              NOR Flash memories.                                                        */
  
  uint32_t BusTurnAroundDuration;        /*!< Defines the number of HCLK cycles to configure
                                              the duration of the bus turnaround.
                                              This parameter can be a value between Min_Data = 0 and Max_Data = 15.
                                              @note This parameter is only used for multiplexed NOR Flash memories.      */
  
  uint32_t CLKDivision;                  /*!< Defines the period of CLK clock output signal, expressed in number of 
                                              HCLK cycles. This parameter can be a value between Min_Data = 2 and Max_Data = 16.
                                              @note This parameter is not used for asynchronous NOR Flash, SRAM or ROM 
                                              accesses.                                                                  */
  
  uint32_t DataLatency;                  /*!< Defines the number of memory clock cycles to issue
                                              to the memory before getting the first data.
                                              The parameter value depends on the memory type as shown below:
                                              - It must be set to 0 in case of a CRAM
                                              - It is don't care in asynchronous NOR, SRAM or ROM accesses
                                              - It may assume a value between Min_Data = 2 and Max_Data = 17 in NOR Flash memories
                                                with synchronous burst mode enable                                       */
  
  uint32_t AccessMode;                   /*!< Specifies the asynchronous access mode. 
                                              This parameter can be a value of @ref FSMC_Access_Mode                      */
  
}FSMC_NORSRAM_TimingTypeDef;

/** 
  * @brief  FSMC_NAND Configuration Structure definition  
  */ 
typedef struct
{
  uint32_t NandBank;               /*!< Specifies the NAND memory device that will be used.
                                        This parameter can be a value of @ref FSMC_NAND_Bank                   */           
  
  uint32_t Waitfeature;            /*!< Enables or disables the Wait feature for the NAND Memory device.
                                        This parameter can be any value of @ref FSMC_Wait_feature              */
  
  uint32_t MemoryDataWidth;        /*!< Specifies the external memory device width.
                                        This parameter can be any value of @ref FSMC_NAND_Data_Width           */
  
  uint32_t EccComputation;         /*!< Enables or disables the ECC computation.
                                        This parameter can be any value of @ref FSMC_ECC                       */
  
  uint32_t ECCPageSize;            /*!< Defines the page size for the extended ECC.
                                        This parameter can be any value of @ref FSMC_ECC_Page_Size             */
  
  uint32_t TCLRSetupTime;          /*!< Defines the number of HCLK cycles to configure the
                                        delay between CLE low and RE low.
                                        This parameter can be a value between Min_Data = 0 and Max_Data = 255  */
  
  uint32_t TARSetupTime;           /*!< Defines the number of HCLK cycles to configure the
                                        delay between ALE low and RE low.
                                        This parameter can be a number between Min_Data = 0 and Max_Data = 255 */
                                     
}FSMC_NAND_InitTypeDef;  

/** 
  * @brief  FSMC_NAND_PCCARD Timing parameters structure definition
  */
typedef struct
{
  uint32_t SetupTime;            /*!< Defines the number of HCLK cycles to setup address before
                                      the command assertion for NAND-Flash read or write access
                                      to common/Attribute or I/O memory space (depending on
                                      the memory space timing to be configured).
                                      This parameter can be a value between Min_Data = 0 and Max_Data = 255    */
  
  uint32_t WaitSetupTime;        /*!< Defines the minimum number of HCLK cycles to assert the
                                      command for NAND-Flash read or write access to
                                      common/Attribute or I/O memory space (depending on the
                                      memory space timing to be configured). 
                                      This parameter can be a number between Min_Data = 0 and Max_Data = 255   */
  
  uint32_t HoldSetupTime;        /*!< Defines the number of HCLK clock cycles to hold address
                                      (and data for write access) after the command de-assertion
                                      for NAND-Flash read or write access to common/Attribute
                                      or I/O memory space (depending on the memory space timing
                                      to be configured).
                                      This parameter can be a number between Min_Data = 0 and Max_Data = 255   */
  
  uint32_t HiZSetupTime;         /*!< Defines the number of HCLK clock cycles during which the
                                      data bus is kept in HiZ after the start of a NAND-Flash
                                      write access to common/Attribute or I/O memory space (depending
                                      on the memory space timing to be configured).
                                      This parameter can be a number between Min_Data = 0 and Max_Data = 255   */
  
}FSMC_NAND_PCC_TimingTypeDef;

/** 
  * @brief  FSMC_NAND Configuration Structure definition  
  */ 
typedef struct
{
  uint32_t Waitfeature;            /*!< Enables or disables the Wait feature for the PCCARD Memory device.
                                        This parameter can be any value of @ref FSMC_Wait_feature              */
  
  uint32_t TCLRSetupTime;          /*!< Defines the number of HCLK cycles to configure the
                                        delay between CLE low and RE low.
                                        This parameter can be a value between Min_Data = 0 and Max_Data = 255  */
  
  uint32_t TARSetupTime;           /*!< Defines the number of HCLK cycles to configure the
                                        delay between ALE low and RE low.
                                        This parameter can be a number between Min_Data = 0 and Max_Data = 255 */
                                     
}FSMC_PCCARD_InitTypeDef;  

/* Exported constants --------------------------------------------------------*/

/** @defgroup FSMC_NOR_SRAM_Controller 
  * @{
  */ 
  
/** @defgroup FSMC_NORSRAM_Bank 
  * @{
  */
#define FSMC_NORSRAM_BANK1                       ((uint32_t)0x00000000)
#define FSMC_NORSRAM_BANK2                       ((uint32_t)0x00000002)
#define FSMC_NORSRAM_BANK3                       ((uint32_t)0x00000004)
#define FSMC_NORSRAM_BANK4                       ((uint32_t)0x00000006)

#define IS_FSMC_NORSRAM_BANK(BANK) (((BANK) == FSMC_NORSRAM_BANK1) || \
                                    ((BANK) == FSMC_NORSRAM_BANK2) || \
                                    ((BANK) == FSMC_NORSRAM_BANK3) || \
                                    ((BANK) == FSMC_NORSRAM_BANK4))
/**
  * @}
  */

/** @defgroup FSMC_Data_Address_Bus_Multiplexing 
  * @{
  */

#define FSMC_DATA_ADDRESS_MUX_DISABLE            ((uint32_t)0x00000000)
#define FSMC_DATA_ADDRESS_MUX_ENABLE             ((uint32_t)0x00000002)

#define IS_FSMC_MUX(MUX) (((MUX) == FSMC_DATA_ADDRESS_MUX_DISABLE) || \
                          ((MUX) == FSMC_DATA_ADDRESS_MUX_ENABLE))
/**
  * @}
  */

/** @defgroup FSMC_Memory_Type 
  * @{
  */

#define FSMC_MEMORY_TYPE_SRAM                    ((uint32_t)0x00000000)
#define FSMC_MEMORY_TYPE_PSRAM                   ((uint32_t)0x00000004)
#define FSMC_MEMORY_TYPE_NOR                     ((uint32_t)0x00000008)


#define IS_FSMC_MEMORY(MEMORY) (((MEMORY) == FSMC_MEMORY_TYPE_SRAM) || \
                                ((MEMORY) == FSMC_MEMORY_TYPE_PSRAM)|| \
                                ((MEMORY) == FSMC_MEMORY_TYPE_NOR))
/**
  * @}
  */

/** @defgroup FSMC_NORSRAM_Data_Width 
  * @{
  */

#define FSMC_NORSRAM_MEM_BUS_WIDTH_8             ((uint32_t)0x00000000)
#define FSMC_NORSRAM_MEM_BUS_WIDTH_16            ((uint32_t)0x00000010)
#define FSMC_NORSRAM_MEM_BUS_WIDTH_32            ((uint32_t)0x00000020)

#define IS_FSMC_NORSRAM_MEMORY_WIDTH(WIDTH) (((WIDTH) == FSMC_NORSRAM_MEM_BUS_WIDTH_8)  || \
                                             ((WIDTH) == FSMC_NORSRAM_MEM_BUS_WIDTH_16) || \
                                             ((WIDTH) == FSMC_NORSRAM_MEM_BUS_WIDTH_32))
/**
  * @}
  */

/** @defgroup FSMC_NORSRAM_Flash_Access 
  * @{
  */
#define FSMC_NORSRAM_FLASH_ACCESS_ENABLE         ((uint32_t)0x00000040)
#define FSMC_NORSRAM_FLASH_ACCESS_DISABLE        ((uint32_t)0x00000000)
/**
  * @}
  */

/** @defgroup FSMC_Burst_Access_Mode 
  * @{
  */

#define FSMC_BURST_ACCESS_MODE_DISABLE           ((uint32_t)0x00000000) 
#define FSMC_BURST_ACCESS_MODE_ENABLE            ((uint32_t)0x00000100)

#define IS_FSMC_BURSTMODE(STATE) (((STATE) == FSMC_BURST_ACCESS_MODE_DISABLE) || \
                                  ((STATE) == FSMC_BURST_ACCESS_MODE_ENABLE))
/**
  * @}
  */
    

/** @defgroup FSMC_Wait_Signal_Polarity 
  * @{
  */
#define FSMC_WAIT_SIGNAL_POLARITY_LOW            ((uint32_t)0x00000000)
#define FSMC_WAIT_SIGNAL_POLARITY_HIGH           ((uint32_t)0x00000200)

#define IS_FSMC_WAIT_POLARITY(POLARITY) (((POLARITY) == FSMC_WAIT_SIGNAL_POLARITY_LOW) || \
                                         ((POLARITY) == FSMC_WAIT_SIGNAL_POLARITY_HIGH))
/**
  * @}
  */

/** @defgroup FSMC_Wrap_Mode 
  * @{
  */
#define FSMC_WRAP_MODE_DISABLE                   ((uint32_t)0x00000000)
#define FSMC_WRAP_MODE_ENABLE                    ((uint32_t)0x00000400)

#define IS_FSMC_WRAP_MODE(MODE) (((MODE) == FSMC_WRAP_MODE_DISABLE) || \
                                 ((MODE) == FSMC_WRAP_MODE_ENABLE)) 
/**
  * @}
  */

/** @defgroup FSMC_Wait_Timing 
  * @{
  */
#define FSMC_WAIT_TIMING_BEFORE_WS               ((uint32_t)0x00000000)
#define FSMC_WAIT_TIMING_DURING_WS               ((uint32_t)0x00000800)

#define IS_FSMC_WAIT_SIGNAL_ACTIVE(ACTIVE) (((ACTIVE) == FSMC_WAIT_TIMING_BEFORE_WS) || \
                                            ((ACTIVE) == FSMC_WAIT_TIMING_DURING_WS)) 
/**
  * @}
  */

/** @defgroup FSMC_Write_Operation 
  * @{
  */
#define FSMC_WRITE_OPERATION_DISABLE             ((uint32_t)0x00000000)
#define FSMC_WRITE_OPERATION_ENABLE              ((uint32_t)0x00001000)

#define IS_FSMC_WRITE_OPERATION(OPERATION) (((OPERATION) == FSMC_WRITE_OPERATION_DISABLE) || \
                                            ((OPERATION) == FSMC_WRITE_OPERATION_ENABLE))                        
/**
  * @}
  */

/** @defgroup FSMC_Wait_Signal 
  * @{
  */
#define FSMC_WAIT_SIGNAL_DISABLE                 ((uint32_t)0x00000000)
#define FSMC_WAIT_SIGNAL_ENABLE                  ((uint32_t)0x00002000)

#define IS_FSMC_WAITE_SIGNAL(SIGNAL) (((SIGNAL) == FSMC_WAIT_SIGNAL_DISABLE) || \
                                      ((SIGNAL) == FSMC_WAIT_SIGNAL_ENABLE)) 

/**
  * @}
  */

/** @defgroup FSMC_Extended_Mode 
  * @{
  */
#define FSMC_EXTENDED_MODE_DISABLE               ((uint32_t)0x00000000)
#define FSMC_EXTENDED_MODE_ENABLE                ((uint32_t)0x00004000)

#define IS_FSMC_EXTENDED_MODE(MODE) (((MODE) == FSMC_EXTENDED_MODE_DISABLE) || \
                                     ((MODE) == FSMC_EXTENDED_MODE_ENABLE))
/**
  * @}
  */

/** @defgroup FSMC_AsynchronousWait 
  * @{
  */
#define FSMC_ASYNCHRONOUS_WAIT_DISABLE           ((uint32_t)0x00000000)
#define FSMC_ASYNCHRONOUS_WAIT_ENABLE            ((uint32_t)0x00008000)

#define IS_FSMC_ASYNWAIT(STATE) (((STATE) == FSMC_ASYNCHRONOUS_WAIT_DISABLE) || \
                                 ((STATE) == FSMC_ASYNCHRONOUS_WAIT_ENABLE))

/**
  * @}
  */  

/** @defgroup FSMC_Write_Burst 
  * @{
  */

#define FSMC_WRITE_BURST_DISABLE                 ((uint32_t)0x00000000)
#define FSMC_WRITE_BURST_ENABLE                  ((uint32_t)0x00080000)

#define IS_FSMC_WRITE_BURST(BURST) (((BURST) == FSMC_WRITE_BURST_DISABLE) || \
                                    ((BURST) == FSMC_WRITE_BURST_ENABLE)) 

/**
  * @}
  */
  
/** @defgroup FSMC_Continous_Clock 
  * @{
  */

#define FSMC_CONTINUOUS_CLOCK_SYNC_ONLY          ((uint32_t)0x00000000)
#define FSMC_CONTINUOUS_CLOCK_SYNC_ASYNC         ((uint32_t)0x00100000)

#define IS_FSMC_CONTINOUS_CLOCK(CCLOCK) (((CCLOCK) == FSMC_CONTINUOUS_CLOCK_SYNC_ONLY) || \
                                         ((CCLOCK) == FSMC_CONTINUOUS_CLOCK_SYNC_ASYNC)) 

/**
  * @}
  */
  
/** @defgroup FSMC_Address_Setup_Time 
  * @{
  */
#define IS_FSMC_ADDRESS_SETUP_TIME(TIME) ((TIME) <= 15)
/**
  * @}
  */

/** @defgroup FSMC_Address_Hold_Time 
  * @{
  */
#define IS_FSMC_ADDRESS_HOLD_TIME(TIME) (((TIME) > 0) && ((TIME) <= 15))
/**
  * @}
  */

/** @defgroup FSMC_Data_Setup_Time 
  * @{
  */
#define IS_FSMC_DATASETUP_TIME(TIME) (((TIME) > 0) && ((TIME) <= 255))
/**
  * @}
  */

/** @defgroup FSMC_Bus_Turn_around_Duration 
  * @{
  */
#define IS_FSMC_TURNAROUND_TIME(TIME) ((TIME) <= 15)
/**
  * @}
  */

/** @defgroup FSMC_CLK_Division 
  * @{
  */
#define IS_FSMC_CLK_DIV(DIV) (((DIV) > 1) && ((DIV) <= 16))
/**
  * @}
  */

/** @defgroup FSMC_Data_Latency 
  * @{
  */
#define IS_FSMC_DATA_LATENCY(LATENCY) (((LATENCY) > 1) && ((LATENCY) <= 17))
/**
  * @}
  */  

/** @defgroup FSMC_Access_Mode 
  * @{
  */
#define FSMC_ACCESS_MODE_A                        ((uint32_t)0x00000000)
#define FSMC_ACCESS_MODE_B                        ((uint32_t)0x10000000) 
#define FSMC_ACCESS_MODE_C                        ((uint32_t)0x20000000)
#define FSMC_ACCESS_MODE_D                        ((uint32_t)0x30000000)

#define IS_FSMC_ACCESS_MODE(MODE) (((MODE) == FSMC_ACCESS_MODE_A) || \
                                   ((MODE) == FSMC_ACCESS_MODE_B) || \
                                   ((MODE) == FSMC_ACCESS_MODE_C) || \
                                   ((MODE) == FSMC_ACCESS_MODE_D))
/**
  * @}
  */
    
/**
  * @}
  */  

/** @defgroup FSMC_NAND_Controller 
  * @{
  */

/** @defgroup FSMC_NAND_Bank 
  * @{
  */  
#define FSMC_NAND_BANK2                          ((uint32_t)0x00000010)
#define FSMC_NAND_BANK3                          ((uint32_t)0x00000100)

#define IS_FSMC_NAND_BANK(BANK) (((BANK) == FSMC_NAND_BANK2) || \
                                 ((BANK) == FSMC_NAND_BANK3))  

/**
  * @}
  */

/** @defgroup FSMC_Wait_feature 
  * @{
  */
#define FSMC_NAND_PCC_WAIT_FEATURE_DISABLE           ((uint32_t)0x00000000)
#define FSMC_NAND_PCC_WAIT_FEATURE_ENABLE            ((uint32_t)0x00000002)

#define IS_FSMC_WAIT_FEATURE(FEATURE) (((FEATURE) == FSMC_NAND_PCC_WAIT_FEATURE_DISABLE) || \
                                       ((FEATURE) == FSMC_NAND_PCC_WAIT_FEATURE_ENABLE))                                                
/**
  * @}
  */

/** @defgroup FSMC_PCR_Memory_Type 
  * @{
  */
#define FSMC_PCR_MEMORY_TYPE_PCCARD        ((uint32_t)0x00000000)
#define FSMC_PCR_MEMORY_TYPE_NAND          ((uint32_t)0x00000008)
/**
  * @}
  */

/** @defgroup FSMC_NAND_Data_Width 
  * @{
  */
#define FSMC_NAND_PCC_MEM_BUS_WIDTH_8                ((uint32_t)0x00000000)
#define FSMC_NAND_PCC_MEM_BUS_WIDTH_16               ((uint32_t)0x00000010)

#define IS_FSMC_NAND_MEMORY_WIDTH(WIDTH) (((WIDTH) == FSMC_NAND_PCC_MEM_BUS_WIDTH_8) || \
                                          ((WIDTH) == FSMC_NAND_PCC_MEM_BUS_WIDTH_16))
/**
  * @}
  */

/** @defgroup FSMC_ECC 
  * @{
  */
#define FSMC_NAND_ECC_DISABLE                    ((uint32_t)0x00000000)
#define FSMC_NAND_ECC_ENABLE                     ((uint32_t)0x00000040)

#define IS_FSMC_ECC_STATE(STATE) (((STATE) == FSMC_NAND_ECC_DISABLE) || \
                                  ((STATE) == FSMC_NAND_ECC_ENABLE))
/**
  * @}
  */

/** @defgroup FSMC_ECC_Page_Size 
  * @{
  */
#define FSMC_NAND_ECC_PAGE_SIZE_256BYTE          ((uint32_t)0x00000000)
#define FSMC_NAND_ECC_PAGE_SIZE_512BYTE          ((uint32_t)0x00020000)
#define FSMC_NAND_ECC_PAGE_SIZE_1024BYTE         ((uint32_t)0x00040000)
#define FSMC_NAND_ECC_PAGE_SIZE_2048BYTE         ((uint32_t)0x00060000)
#define FSMC_NAND_ECC_PAGE_SIZE_4096BYTE         ((uint32_t)0x00080000)
#define FSMC_NAND_ECC_PAGE_SIZE_8192BYTE         ((uint32_t)0x000A0000)

#define IS_FSMC_ECCPAGE_SIZE(SIZE) (((SIZE) == FSMC_NAND_ECC_PAGE_SIZE_256BYTE)  || \
                                    ((SIZE) == FSMC_NAND_ECC_PAGE_SIZE_512BYTE)  || \
                                    ((SIZE) == FSMC_NAND_ECC_PAGE_SIZE_1024BYTE) || \
                                    ((SIZE) == FSMC_NAND_ECC_PAGE_SIZE_2048BYTE) || \
                                    ((SIZE) == FSMC_NAND_ECC_PAGE_SIZE_4096BYTE) || \
                                    ((SIZE) == FSMC_NAND_ECC_PAGE_SIZE_8192BYTE))
/**
  * @}
  */

/** @defgroup FSMC_TCLR_Setup_Time 
  * @{
  */
#define IS_FSMC_TCLR_TIME(TIME) ((TIME) <= 255)
/**
  * @}
  */

/** @defgroup FSMC_TAR_Setup_Time 
  * @{
  */
#define IS_FSMC_TAR_TIME(TIME) ((TIME) <= 255)
/**
  * @}
  */

/** @defgroup FSMC_Setup_Time 
  * @{
  */
#define IS_FSMC_SETUP_TIME(TIME) ((TIME) <= 255)
/**
  * @}
  */

/** @defgroup FSMC_Wait_Setup_Time 
  * @{
  */
#define IS_FSMC_WAIT_TIME(TIME) ((TIME) <= 255)
/**
  * @}
  */

/** @defgroup FSMC_Hold_Setup_Time 
  * @{
  */
#define IS_FSMC_HOLD_TIME(TIME) ((TIME) <= 255)
/**
  * @}
  */

/** @defgroup FSMC_HiZ_Setup_Time 
  * @{
  */
#define IS_FSMC_HIZ_TIME(TIME) ((TIME) <= 255)
/**
  * @}
  */  
    
/**
  * @}
  */  
  

/** @defgroup FSMC_NORSRAM_Device_Instance
  * @{
  */
#define IS_FSMC_NORSRAM_DEVICE(INSTANCE) ((INSTANCE) == FSMC_NORSRAM_DEVICE)

/**
  * @}
  */

/** @defgroup FSMC_NORSRAM_EXTENDED_Device_Instance
  * @{
  */
#define IS_FSMC_NORSRAM_EXTENDED_DEVICE(INSTANCE) ((INSTANCE) == FSMC_NORSRAM_EXTENDED_DEVICE)

/**
  * @}
  */
  
  /** @defgroup FSMC_NAND_Device_Instance
  * @{
  */
#define IS_FSMC_NAND_DEVICE(INSTANCE) ((INSTANCE) == FSMC_NAND_DEVICE)

/**
  * @}
  */  

/** @defgroup FSMC_PCCARD_Device_Instance
  * @{
  */
#define IS_FSMC_PCCARD_DEVICE(INSTANCE) ((INSTANCE) == FSMC_PCCARD_DEVICE)

/**
  * @}
  */ 
   
/** @defgroup FSMC_Interrupt_definition 
  * @brief FSMC Interrupt definition
  * @{
  */  
#define FSMC_IT_RISING_EDGE                ((uint32_t)0x00000008)
#define FSMC_IT_LEVEL                      ((uint32_t)0x00000010)
#define FSMC_IT_FALLING_EDGE               ((uint32_t)0x00000020)
#define FSMC_IT_REFRESH_ERROR              ((uint32_t)0x00004000)

#define IS_FSMC_IT(IT) ((((IT) & (uint32_t)0xFFFFBFC7) == 0x00000000) && ((IT) != 0x00000000))
#define IS_FSMC_GET_IT(IT) (((IT) == FSMC_IT_RISING_EDGE)  || \
                            ((IT) == FSMC_IT_LEVEL)        || \
                            ((IT) == FSMC_IT_FALLING_EDGE) || \
                            ((IT) == FSMC_IT_REFRESH_ERROR)) 
/**
  * @}
  */
    
/** @defgroup FSMC_Flag_definition 
  * @brief FSMC Flag definition
  * @{
  */ 
#define FSMC_FLAG_RISING_EDGE                    ((uint32_t)0x00000001)
#define FSMC_FLAG_LEVEL                          ((uint32_t)0x00000002)
#define FSMC_FLAG_FALLING_EDGE                   ((uint32_t)0x00000004)
#define FSMC_FLAG_FEMPT                          ((uint32_t)0x00000040)

#define IS_FSMC_GET_FLAG(FLAG) (((FLAG) == FSMC_FLAG_RISING_EDGE)  || \
                                ((FLAG) == FSMC_FLAG_LEVEL)        || \
                                ((FLAG) == FSMC_FLAG_FALLING_EDGE) || \
                                ((FLAG) == FSMC_FLAG_FEMPT))

#define IS_FSMC_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0xFFFFFFF8) == 0x00000000) && ((FLAG) != 0x00000000))
                               

/**
  * @}
  */


/* Exported macro ------------------------------------------------------------*/


/** @defgroup FSMC_NOR_Macros
 *  @brief macros to handle NOR device enable/disable and read/write operations
 *  @{
 */
 
/**
  * @brief  Enable the NORSRAM device access.
  * @param  __INSTANCE__: FSMC_NORSRAM Instance
  * @param  __BANK__: FSMC_NORSRAM Bank    
  * @retval none
  */ 
#define __FSMC_NORSRAM_ENABLE(__INSTANCE__, __BANK__)  ((__INSTANCE__)->BTCR[(__BANK__)] |= FSMC_BCR1_MBKEN)

/**
  * @brief  Disable the NORSRAM device access.
  * @param  __INSTANCE__: FSMC_NORSRAM Instance
  * @param  __BANK__: FSMC_NORSRAM Bank   
  * @retval none
  */ 
#define __FSMC_NORSRAM_DISABLE(__INSTANCE__, __BANK__) ((__INSTANCE__)->BTCR[(__BANK__)] &= ~FSMC_BCR1_MBKEN)  

/**
  * @}
  */ 
  

/** @defgroup FSMC_NAND_Macros
 *  @brief macros to handle NAND device enable/disable
 *  @{
 */
 
/**
  * @brief  Enable the NAND device access.
  * @param  __INSTANCE__: FSMC_NAND Instance
  * @param  __BANK__: FSMC_NAND Bank    
  * @retval none
  */  
#define __FSMC_NAND_ENABLE(__INSTANCE__, __BANK__)  (((__BANK__) == FSMC_NAND_BANK2)? ((__INSTANCE__)->PCR2 |= FSMC_PCR2_PBKEN): \
                                                    ((__INSTANCE__)->PCR3 |= FSMC_PCR3_PBKEN))
                                         

/**
  * @brief  Disable the NAND device access.
  * @param  __INSTANCE__: FSMC_NAND Instance
  * @param  __BANK__: FSMC_NAND Bank  
  * @retval none
  */                                          
#define __FSMC_NAND_DISABLE(__INSTANCE__, __BANK__) (((__BANK__) == FSMC_NAND_BANK2)? ((__INSTANCE__)->PCR2 &= ~FSMC_PCR2_PBKEN): \
                                                   ((__INSTANCE__)->PCR3 &= ~FSMC_PCR3_PBKEN))
                                         
                                        
/**
  * @}
  */ 
  
/** @defgroup FSMC_PCCARD_Macros
 *  @brief macros to handle SRAM read/write operations 
 *  @{
 */

/**
  * @brief  Enable the PCCARD device access.
  * @param  __INSTANCE__: FSMC_PCCARD Instance  
  * @retval none
  */ 
#define __FSMC_PCCARD_ENABLE(__INSTANCE__)  ((__INSTANCE__)->PCR4 |= FSMC_PCR4_PBKEN)

/**
  * @brief  Disable the PCCARD device access.
  * @param  __INSTANCE__: FSMC_PCCARD Instance   
  * @retval none
  */ 
#define __FSMC_PCCARD_DISABLE(__INSTANCE__) ((__INSTANCE__)->PCR4 &= ~FSMC_PCR4_PBKEN)

/**
  * @}
  */
  
/** @defgroup FSMC_Interrupt
 *  @brief macros to handle FSMC interrupts
 * @{
 */ 

/**
  * @brief  Enable the NAND device interrupt.
  * @param  __INSTANCE__: FSMC_NAND Instance
  * @param  __BANK__: FSMC_NAND Bank 
  * @param  __INTERRUPT__: FSMC_NAND interrupt 
  *         This parameter can be any combination of the following values:
  *            @arg FSMC_IT_RISING_EDGE: Interrupt rising edge.
  *            @arg FSMC_IT_LEVEL: Interrupt level.
  *            @arg FSMC_IT_FALLING_EDGE: Interrupt falling edge.        
  * @retval None
  */  
#define __FSMC_NAND_ENABLE_IT(__INSTANCE__, __BANK__, __INTERRUPT__)  (((__BANK__) == FSMC_NAND_BANK2)? ((__INSTANCE__)->SR2 |= (__INTERRUPT__)): \
                                                                                                      ((__INSTANCE__)->SR3 |= (__INTERRUPT__)))

/**
  * @brief  Disable the NAND device interrupt.
  * @param  __INSTANCE__: FSMC_NAND Instance
  * @param  __BANK__: FSMC_NAND Bank 
  * @param  __INTERRUPT__: FSMC_NAND interrupt
  *         This parameter can be any combination of the following values:
  *            @arg FSMC_IT_RISING_EDGE: Interrupt rising edge.
  *            @arg FSMC_IT_LEVEL: Interrupt level.
  *            @arg FSMC_IT_FALLING_EDGE: Interrupt falling edge.    
  * @retval None
  */
#define __FSMC_NAND_DISABLE_IT(__INSTANCE__, __BANK__, __INTERRUPT__)  (((__BANK__) == FSMC_NAND_BANK2)? ((__INSTANCE__)->SR2 &= ~(__INTERRUPT__)): \
                                                                                                      ((__INSTANCE__)->SR3 &= ~(__INTERRUPT__))) 
                                                                                                                             
/**
  * @brief  Get flag status of the NAND device.
  * @param  __INSTANCE__: FSMC_NAND Instance
  * @param  __BANK__: FSMC_NAND Bank 
  * @param  __FLAG__: FSMC_NAND flag
  *         This parameter can be any combination of the following values:
  *            @arg FSMC_FLAG_RISING_EDGE: Interrupt rising edge flag.
  *            @arg FSMC_FLAG_LEVEL: Interrupt level edge flag.
  *            @arg FSMC_FLAG_FALLING_EDGE: Interrupt falling edge flag.
  *            @arg FSMC_FLAG_FEMPT: FIFO empty flag.   
  * @retval The state of FLAG (SET or RESET).
  */
#define __FSMC_NAND_GET_FLAG(__INSTANCE__, __BANK__, __FLAG__)  (((__BANK__) == FSMC_NAND_BANK2)? (((__INSTANCE__)->SR2 &(__FLAG__)) == (__FLAG__)): \
                                                                                                (((__INSTANCE__)->SR3 &(__FLAG__)) == (__FLAG__)))
/**
  * @brief  Clear flag status of the NAND device.
  * @param  __INSTANCE__: FSMC_NAND Instance
  * @param  __BANK__: FSMC_NAND Bank 
  * @param  __FLAG__: FSMC_NAND flag
  *         This parameter can be any combination of the following values:
  *            @arg FSMC_FLAG_RISING_EDGE: Interrupt rising edge flag.
  *            @arg FSMC_FLAG_LEVEL: Interrupt level edge flag.
  *            @arg FSMC_FLAG_FALLING_EDGE: Interrupt falling edge flag.
  *            @arg FSMC_FLAG_FEMPT: FIFO empty flag.   
  * @retval None
  */
#define __FSMC_NAND_CLEAR_FLAG(__INSTANCE__, __BANK__, __FLAG__)  (((__BANK__) == FSMC_NAND_BANK2)? ((__INSTANCE__)->SR2 &= ~(__FLAG__)): \
                                                                                                  ((__INSTANCE__)->SR3 &= ~(__FLAG__))) 
/**
  * @brief  Enable the PCCARD device interrupt.
  * @param  __INSTANCE__: FSMC_PCCARD Instance  
  * @param  __INTERRUPT__: FSMC_PCCARD interrupt 
  *         This parameter can be any combination of the following values:
  *            @arg FSMC_IT_RISING_EDGE: Interrupt rising edge.
  *            @arg FSMC_IT_LEVEL: Interrupt level.
  *            @arg FSMC_IT_FALLING_EDGE: Interrupt falling edge.        
  * @retval None
  */ 
#define __FSMC_PCCARD_ENABLE_IT(__INSTANCE__, __INTERRUPT__)  ((__INSTANCE__)->SR4 |= (__INTERRUPT__))

/**
  * @brief  Disable the PCCARD device interrupt.
  * @param  __INSTANCE__: FSMC_PCCARD Instance  
  * @param  __INTERRUPT__: FSMC_PCCARD interrupt 
  *         This parameter can be any combination of the following values:
  *            @arg FSMC_IT_RISING_EDGE: Interrupt rising edge.
  *            @arg FSMC_IT_LEVEL: Interrupt level.
  *            @arg FSMC_IT_FALLING_EDGE: Interrupt falling edge.        
  * @retval None
  */ 
#define __FSMC_PCCARD_DISABLE_IT(__INSTANCE__, __INTERRUPT__)  ((__INSTANCE__)->SR4 &= ~(__INTERRUPT__)) 

/**
  * @brief  Get flag status of the PCCARD device.
  * @param  __INSTANCE__: FSMC_PCCARD Instance    
  * @param  __FLAG__: FSMC_PCCARD flag
  *         This parameter can be any combination of the following values:
  *            @arg FSMC_FLAG_RISING_EDGE: Interrupt rising edge flag.
  *            @arg FSMC_FLAG_LEVEL: Interrupt level edge flag.
  *            @arg FSMC_FLAG_FALLING_EDGE: Interrupt falling edge flag.
  *            @arg FSMC_FLAG_FEMPT: FIFO empty flag.   
  * @retval The state of FLAG (SET or RESET).
  */
#define __FSMC_PCCARD_GET_FLAG(__INSTANCE__, __FLAG__)  (((__INSTANCE__)->SR4 &(__FLAG__)) == (__FLAG__))

/**
  * @brief  Clear flag status of the PCCARD device.
  * @param  __INSTANCE__: FSMC_PCCARD Instance   
  * @param  __FLAG__: FSMC_PCCARD flag
  *         This parameter can be any combination of the following values:
  *            @arg FSMC_FLAG_RISING_EDGE: Interrupt rising edge flag.
  *            @arg FSMC_FLAG_LEVEL: Interrupt level edge flag.
  *            @arg FSMC_FLAG_FALLING_EDGE: Interrupt falling edge flag.
  *            @arg FSMC_FLAG_FEMPT: FIFO empty flag.   
  * @retval None
  */
#define __FSMC_PCCARD_CLEAR_FLAG(__INSTANCE__, __FLAG__)  ((__INSTANCE__)->SR4 &= ~(__FLAG__))

/**
  * @}
  */ 

/* Exported functions --------------------------------------------------------*/

/* FSMC_NORSRAM Controller functions ******************************************/
/* Initialization/de-initialization functions */
HAL_StatusTypeDef  FSMC_NORSRAM_Init(FSMC_NORSRAM_TypeDef *Device, FSMC_NORSRAM_InitTypeDef *Init);
HAL_StatusTypeDef  FSMC_NORSRAM_Timing_Init(FSMC_NORSRAM_TypeDef *Device, FSMC_NORSRAM_TimingTypeDef *Timing, uint32_t Bank);
HAL_StatusTypeDef  FSMC_NORSRAM_Extended_Timing_Init(FSMC_NORSRAM_EXTENDED_TypeDef *Device, FSMC_NORSRAM_TimingTypeDef *Timing, uint32_t Bank, uint32_t ExtendedMode);
HAL_StatusTypeDef  FSMC_NORSRAM_DeInit(FSMC_NORSRAM_TypeDef *Device, FSMC_NORSRAM_EXTENDED_TypeDef *ExDevice, uint32_t Bank);

/* FSMC_NORSRAM Control functions */
HAL_StatusTypeDef  FSMC_NORSRAM_WriteOperation_Enable(FSMC_NORSRAM_TypeDef *Device, uint32_t Bank);
HAL_StatusTypeDef  FSMC_NORSRAM_WriteOperation_Disable(FSMC_NORSRAM_TypeDef *Device, uint32_t Bank);

/* FSMC_NAND Controller functions *********************************************/
/* Initialization/de-initialization functions */
HAL_StatusTypeDef  FSMC_NAND_Init(FSMC_NAND_TypeDef *Device, FSMC_NAND_InitTypeDef *Init);
HAL_StatusTypeDef  FSMC_NAND_CommonSpace_Timing_Init(FSMC_NAND_TypeDef *Device, FSMC_NAND_PCC_TimingTypeDef *Timing, uint32_t Bank);
HAL_StatusTypeDef  FSMC_NAND_AttributeSpace_Timing_Init(FSMC_NAND_TypeDef *Device, FSMC_NAND_PCC_TimingTypeDef *Timing, uint32_t Bank);
HAL_StatusTypeDef  FSMC_NAND_DeInit(FSMC_NAND_TypeDef *Device, uint32_t Bank);

/* FSMC_NAND Control functions */
HAL_StatusTypeDef  FSMC_NAND_ECC_Enable(FSMC_NAND_TypeDef *Device, uint32_t Bank);
HAL_StatusTypeDef  FSMC_NAND_ECC_Disable(FSMC_NAND_TypeDef *Device, uint32_t Bank);
HAL_StatusTypeDef  FSMC_NAND_GetECC(FSMC_NAND_TypeDef *Device, uint32_t *ECCval, uint32_t Bank, uint32_t Timeout);

/* FSMC_PCCARD Controller functions *******************************************/
/* Initialization/de-initialization functions */
HAL_StatusTypeDef  FSMC_PCCARD_Init(FSMC_PCCARD_TypeDef *Device, FSMC_PCCARD_InitTypeDef *Init);
HAL_StatusTypeDef  FSMC_PCCARD_CommonSpace_Timing_Init(FSMC_PCCARD_TypeDef *Device, FSMC_NAND_PCC_TimingTypeDef *Timing);
HAL_StatusTypeDef  FSMC_PCCARD_AttributeSpace_Timing_Init(FSMC_PCCARD_TypeDef *Device, FSMC_NAND_PCC_TimingTypeDef *Timing);
HAL_StatusTypeDef  FSMC_PCCARD_IOSpace_Timing_Init(FSMC_PCCARD_TypeDef *Device, FSMC_NAND_PCC_TimingTypeDef *Timing); 
HAL_StatusTypeDef  FSMC_PCCARD_DeInit(FSMC_PCCARD_TypeDef *Device);

/* FSMC APIs, macros and typedefs redefinition */
#define FMC_NORSRAM_TypeDef                   FSMC_NORSRAM_TypeDef
#define FMC_NORSRAM_EXTENDED_TypeDef          FSMC_NORSRAM_EXTENDED_TypeDef
#define FMC_NORSRAM_InitTypeDef               FSMC_NORSRAM_InitTypeDef
#define FMC_NORSRAM_TimingTypeDef             FSMC_NORSRAM_TimingTypeDef

#define FMC_NORSRAM_Init                      FSMC_NORSRAM_Init
#define FMC_NORSRAM_Timing_Init               FSMC_NORSRAM_Timing_Init
#define FMC_NORSRAM_Extended_Timing_Init      FSMC_NORSRAM_Extended_Timing_Init
#define FMC_NORSRAM_DeInit                    FSMC_NORSRAM_DeInit
#define FMC_NORSRAM_WriteOperation_Enable     FSMC_NORSRAM_WriteOperation_Enable
#define FMC_NORSRAM_WriteOperation_Disable    FSMC_NORSRAM_WriteOperation_Disable

#define __FMC_NORSRAM_ENABLE                  __FSMC_NORSRAM_ENABLE
#define __FMC_NORSRAM_DISABLE                 __FSMC_NORSRAM_DISABLE 

#define FMC_NAND_InitTypeDef                  FSMC_NAND_InitTypeDef
#define FMC_PCCARD_InitTypeDef                FSMC_PCCARD_InitTypeDef
#define FMC_NAND_PCC_TimingTypeDef            FSMC_NAND_PCC_TimingTypeDef

#define FMC_NAND_Init                         FSMC_NAND_Init
#define FMC_NAND_CommonSpace_Timing_Init      FSMC_NAND_CommonSpace_Timing_Init
#define FMC_NAND_AttributeSpace_Timing_Init   FSMC_NAND_AttributeSpace_Timing_Init
#define FMC_NAND_DeInit                       FSMC_NAND_DeInit
#define FMC_NAND_ECC_Enable                   FSMC_NAND_ECC_Enable
#define FMC_NAND_ECC_Disable                  FSMC_NAND_ECC_Disable
#define FMC_NAND_GetECC                       FSMC_NAND_GetECC
#define FMC_PCCARD_Init                       FSMC_PCCARD_Init
#define FMC_PCCARD_CommonSpace_Timing_Init    FSMC_PCCARD_CommonSpace_Timing_Init
#define FMC_PCCARD_AttributeSpace_Timing_Init FSMC_PCCARD_AttributeSpace_Timing_Init
#define FMC_PCCARD_IOSpace_Timing_Init        FSMC_PCCARD_IOSpace_Timing_Init
#define FMC_PCCARD_DeInit                     FSMC_PCCARD_DeInit

#define __FMC_NAND_ENABLE                     __FSMC_NAND_ENABLE
#define __FMC_NAND_DISABLE                    __FSMC_NAND_DISABLE
#define __FMC_PCCARD_ENABLE                   __FSMC_PCCARD_ENABLE
#define __FMC_PCCARD_DISABLE                  __FSMC_PCCARD_DISABLE
#define __FMC_NAND_ENABLE_IT                  __FSMC_NAND_ENABLE_IT
#define __FMC_NAND_DISABLE_IT                 __FSMC_NAND_DISABLE_IT
#define __FMC_NAND_GET_FLAG                   __FSMC_NAND_GET_FLAG
#define __FMC_NAND_CLEAR_FLAG                 __FSMC_NAND_CLEAR_FLAG
#define __FMC_PCCARD_ENABLE_IT                __FSMC_PCCARD_ENABLE_IT
#define __FMC_PCCARD_DISABLE_IT               __FSMC_PCCARD_DISABLE_IT
#define __FMC_PCCARD_GET_FLAG                 __FSMC_PCCARD_GET_FLAG
#define __FMC_PCCARD_CLEAR_FLAG               __FSMC_PCCARD_CLEAR_FLAG

#define FMC_NORSRAM_TypeDef                   FSMC_NORSRAM_TypeDef
#define FMC_NORSRAM_EXTENDED_TypeDef          FSMC_NORSRAM_EXTENDED_TypeDef
#define FMC_NAND_TypeDef                      FSMC_NAND_TypeDef
#define FMC_PCCARD_TypeDef                    FSMC_PCCARD_TypeDef

#define FMC_NORSRAM_DEVICE                    FSMC_NORSRAM_DEVICE            
#define FMC_NORSRAM_EXTENDED_DEVICE           FSMC_NORSRAM_EXTENDED_DEVICE   
#define FMC_NAND_DEVICE                       FSMC_NAND_DEVICE             
#define FMC_PCCARD_DEVICE                     FSMC_PCCARD_DEVICE 

#define FMC_NAND_BANK2                        FSMC_NAND_BANK2

#define FMC_IT_RISING_EDGE                    FSMC_IT_RISING_EDGE
#define FMC_IT_LEVEL                          FSMC_IT_LEVEL
#define FMC_IT_FALLING_EDGE                   FSMC_IT_FALLING_EDGE
#define FMC_IT_REFRESH_ERROR                  FSMC_IT_REFRESH_ERROR

#define FMC_FLAG_RISING_EDGE                  FSMC_FLAG_RISING_EDGE
#define FMC_FLAG_LEVEL                        FSMC_FLAG_LEVEL
#define FMC_FLAG_FALLING_EDGE                 FSMC_FLAG_FALLING_EDGE
#define FMC_FLAG_FEMPT                        FSMC_FLAG_FEMPT

#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx */

/**
  * @}
  */ 

/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_LL_FSMC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

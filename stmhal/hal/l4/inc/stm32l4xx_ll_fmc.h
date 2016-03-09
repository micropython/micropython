/**
  ******************************************************************************
  * @file    stm32l4xx_ll_fmc.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   Header file of FMC HAL module.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32L4xx_LL_FMC_H
#define __STM32L4xx_LL_FMC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal_def.h"

/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @addtogroup FMC_LL FMC Low Layer
  * @{
  */ 

/** @addtogroup FMC_LL_Private_Macros FMC Low Layer Private Macros
  * @{
  */
#define IS_FMC_NORSRAM_BANK(__BANK__)          (((__BANK__) == FMC_NORSRAM_BANK1) || \
                                                ((__BANK__) == FMC_NORSRAM_BANK2) || \
                                                ((__BANK__) == FMC_NORSRAM_BANK3) || \
                                                ((__BANK__) == FMC_NORSRAM_BANK4))


#define IS_FMC_MUX(__MUX__)                    (((__MUX__) == FMC_DATA_ADDRESS_MUX_DISABLE) || \
                                                ((__MUX__) == FMC_DATA_ADDRESS_MUX_ENABLE))


#define IS_FMC_MEMORY(__MEMORY__)              (((__MEMORY__) == FMC_MEMORY_TYPE_SRAM) || \
                                                ((__MEMORY__) == FMC_MEMORY_TYPE_PSRAM)|| \
                                                ((__MEMORY__) == FMC_MEMORY_TYPE_NOR))

#define IS_FMC_NORSRAM_MEMORY_WIDTH(__WIDTH__) (((__WIDTH__) == FMC_NORSRAM_MEM_BUS_WIDTH_8)  || \
                                                ((__WIDTH__) == FMC_NORSRAM_MEM_BUS_WIDTH_16) || \
                                                ((__WIDTH__) == FMC_NORSRAM_MEM_BUS_WIDTH_32))

#define IS_FMC_PAGESIZE(__SIZE__)              (((__SIZE__) == FMC_PAGE_SIZE_NONE) || \
                                                ((__SIZE__) == FMC_PAGE_SIZE_128) || \
                                                ((__SIZE__) == FMC_PAGE_SIZE_256) || \
                                                ((__SIZE__) == FMC_PAGE_SIZE_512) || \
                                                ((__SIZE__) == FMC_PAGE_SIZE_1024))

#define IS_FMC_WRITE_BURST(__BURST__)          (((__BURST__) == FMC_WRITE_BURST_DISABLE) || \
                                                ((__BURST__) == FMC_WRITE_BURST_ENABLE)) 

#define IS_FMC_CONTINOUS_CLOCK(__CCLOCK__)     (((__CCLOCK__) == FMC_CONTINUOUS_CLOCK_SYNC_ONLY) || \
                                                ((__CCLOCK__) == FMC_CONTINUOUS_CLOCK_SYNC_ASYNC)) 

#define IS_FMC_ACCESS_MODE(__MODE__)           (((__MODE__) == FMC_ACCESS_MODE_A) || \
                                                ((__MODE__) == FMC_ACCESS_MODE_B) || \
                                                ((__MODE__) == FMC_ACCESS_MODE_C) || \
                                                ((__MODE__) == FMC_ACCESS_MODE_D))


#define IS_FMC_NAND_BANK(__BANK__)             ((__BANK__) == FMC_NAND_BANK3)  

#define IS_FMC_WAIT_FEATURE(__FEATURE__)       (((__FEATURE__) == FMC_NAND_WAIT_FEATURE_DISABLE) || \
                                                ((__FEATURE__) == FMC_NAND_WAIT_FEATURE_ENABLE))

#define IS_FMC_NAND_MEMORY_WIDTH(__WIDTH__)    (((__WIDTH__) == FMC_NAND_MEM_BUS_WIDTH_8) || \
                                                ((__WIDTH__) == FMC_NAND_MEM_BUS_WIDTH_16))

#define IS_FMC_ECC_STATE(__STATE__)            (((__STATE__) == FMC_NAND_ECC_DISABLE) || \
                                                ((__STATE__) == FMC_NAND_ECC_ENABLE))


#define IS_FMC_ECCPAGE_SIZE(__SIZE__)          (((__SIZE__) == FMC_NAND_ECC_PAGE_SIZE_256BYTE)  || \
                                                ((__SIZE__) == FMC_NAND_ECC_PAGE_SIZE_512BYTE)  || \
                                                ((__SIZE__) == FMC_NAND_ECC_PAGE_SIZE_1024BYTE) || \
                                                ((__SIZE__) == FMC_NAND_ECC_PAGE_SIZE_2048BYTE) || \
                                                ((__SIZE__) == FMC_NAND_ECC_PAGE_SIZE_4096BYTE) || \
                                                ((__SIZE__) == FMC_NAND_ECC_PAGE_SIZE_8192BYTE))


/** @defgroup FMC_NORSRAM_Device_Instance FMC NOR/SRAM Device Instance
  * @{
  */
#define IS_FMC_NORSRAM_DEVICE(__INSTANCE__) ((__INSTANCE__) == FMC_NORSRAM_DEVICE)
/**
  * @}
  */

/** @defgroup FMC_NORSRAM_EXTENDED_Device_Instance FMC NOR/SRAM EXTENDED Device Instance
  * @{
  */
#define IS_FMC_NORSRAM_EXTENDED_DEVICE(__INSTANCE__) ((__INSTANCE__) == FMC_NORSRAM_EXTENDED_DEVICE)
/**
  * @}
  */
  
/** @defgroup FMC_NAND_Device_Instance FMC NAND Device Instance
  * @{
  */
#define IS_FMC_NAND_DEVICE(__INSTANCE__) ((__INSTANCE__) == FMC_NAND_DEVICE)
/**
  * @}
  */  

#define IS_FMC_BURSTMODE(__STATE__)            (((__STATE__) == FMC_BURST_ACCESS_MODE_DISABLE) || \
                                                ((__STATE__) == FMC_BURST_ACCESS_MODE_ENABLE))

#define IS_FMC_WAIT_POLARITY(__POLARITY__)     (((__POLARITY__) == FMC_WAIT_SIGNAL_POLARITY_LOW) || \
                                                ((__POLARITY__) == FMC_WAIT_SIGNAL_POLARITY_HIGH))

#define IS_FMC_WAIT_SIGNAL_ACTIVE(__ACTIVE__)  (((__ACTIVE__) == FMC_WAIT_TIMING_BEFORE_WS) || \
                                                ((__ACTIVE__) == FMC_WAIT_TIMING_DURING_WS)) 


#define IS_FMC_WRITE_OPERATION(__OPERATION__)  (((__OPERATION__) == FMC_WRITE_OPERATION_DISABLE) || \
                                                ((__OPERATION__) == FMC_WRITE_OPERATION_ENABLE))

#define IS_FMC_WAITE_SIGNAL(__SIGNAL__)        (((__SIGNAL__) == FMC_WAIT_SIGNAL_DISABLE) || \
                                                ((__SIGNAL__) == FMC_WAIT_SIGNAL_ENABLE)) 

#define IS_FMC_EXTENDED_MODE(__MODE__)         (((__MODE__) == FMC_EXTENDED_MODE_DISABLE) || \
                                                ((__MODE__) == FMC_EXTENDED_MODE_ENABLE))

#define IS_FMC_ASYNWAIT(__STATE__)             (((__STATE__) == FMC_ASYNCHRONOUS_WAIT_DISABLE) || \
                                                ((__STATE__) == FMC_ASYNCHRONOUS_WAIT_ENABLE))

/** @defgroup FMC_Address_Setup_Time 
  * @{
  */
#define IS_FMC_ADDRESS_SETUP_TIME(__TIME__) ((__TIME__) <= 15)
/**
  * @}
  */

/** @defgroup FMC_Address_Hold_Time 
  * @{
  */
#define IS_FMC_ADDRESS_HOLD_TIME(__TIME__) (((__TIME__) > 0) && ((__TIME__) <= 15))
/**
  * @}
  */

/** @defgroup FMC_Data_Setup_Time 
  * @{
  */
#define IS_FMC_DATASETUP_TIME(__TIME__) (((__TIME__) > 0) && ((__TIME__) <= 255))
/**
  * @}
  */

/** @defgroup FMC_Bus_Turn_around_Duration 
  * @{
  */
#define IS_FMC_TURNAROUND_TIME(__TIME__) ((__TIME__) <= 15)
/**
  * @}
  */

/** @defgroup FMC_CLK_Division 
  * @{
  */
#define IS_FMC_CLK_DIV(__DIV__) (((__DIV__) > 1) && ((__DIV__) <= 16))
/**
  * @}
  */

/** @defgroup FMC_Data_Latency 
  * @{
  */
#define IS_FMC_DATA_LATENCY(__LATENCY__) (((__LATENCY__) > 1) && ((__LATENCY__) <= 17))
/**
  * @}
  */  

/** @defgroup FMC_TCLR_Setup_Time FMC TCLR Setup Time
  * @{
  */
#define IS_FMC_TCLR_TIME(__TIME__) ((__TIME__) <= 255)
/**
  * @}
  */

/** @defgroup FMC_TAR_Setup_Time 
  * @{
  */
#define IS_FMC_TAR_TIME(__TIME__) ((__TIME__) <= 255)
/**
  * @}
  */

/** @defgroup FMC_Setup_Time 
  * @{
  */
#define IS_FMC_SETUP_TIME(__TIME__) ((__TIME__) <= 255)
/**
  * @}
  */

/** @defgroup FMC_Wait_Setup_Time 
  * @{
  */
#define IS_FMC_WAIT_TIME(__TIME__) ((__TIME__) <= 255)
/**
  * @}
  */

/** @defgroup FMC_Hold_Setup_Time 
  * @{
  */
#define IS_FMC_HOLD_TIME(__TIME__) ((__TIME__) <= 255)
/**
  * @}
  */

/** @defgroup FMC_HiZ_Setup_Time 
  * @{
  */
#define IS_FMC_HIZ_TIME(__TIME__) ((__TIME__) <= 255)
/**
  * @}
  */  
    
/**
  * @}
  */ 

/* Exported typedef ----------------------------------------------------------*/ 
/** @addtogroup FMC_LL_Exported_Typedef FMC Low Layer Exported Typedef
  * @{
  */
#define FMC_NORSRAM_TypeDef            FMC_Bank1_TypeDef
#define FMC_NORSRAM_EXTENDED_TypeDef   FMC_Bank1E_TypeDef
#define FMC_NAND_TypeDef               FMC_Bank3_TypeDef

#define FMC_NORSRAM_DEVICE             FMC_Bank1_R
#define FMC_NORSRAM_EXTENDED_DEVICE    FMC_Bank1E_R
#define FMC_NAND_DEVICE                FMC_Bank3_R

/** 
  * @brief  FMC_NORSRAM Configuration Structure definition
  */ 
typedef struct
{
  uint32_t NSBank;                       /*!< Specifies the NORSRAM memory device that will be used.
                                              This parameter can be a value of @ref FMC_NORSRAM_Bank                     */

  uint32_t DataAddressMux;               /*!< Specifies whether the address and data values are
                                              multiplexed on the data bus or not. 
                                              This parameter can be a value of @ref FMC_Data_Address_Bus_Multiplexing    */

  uint32_t MemoryType;                   /*!< Specifies the type of external memory attached to
                                              the corresponding memory device.
                                              This parameter can be a value of @ref FMC_Memory_Type                      */

  uint32_t MemoryDataWidth;              /*!< Specifies the external memory device width.
                                              This parameter can be a value of @ref FMC_NORSRAM_Data_Width               */

  uint32_t BurstAccessMode;              /*!< Enables or disables the burst access mode for Flash memory,
                                              valid only with synchronous burst Flash memories.
                                              This parameter can be a value of @ref FMC_Burst_Access_Mode                */

  uint32_t WaitSignalPolarity;           /*!< Specifies the wait signal polarity, valid only when accessing
                                              the Flash memory in burst mode.
                                              This parameter can be a value of @ref FMC_Wait_Signal_Polarity             */

  uint32_t WaitSignalActive;             /*!< Specifies if the wait signal is asserted by the memory one
                                              clock cycle before the wait state or during the wait state,
                                              valid only when accessing memories in burst mode. 
                                              This parameter can be a value of @ref FMC_Wait_Timing                      */

  uint32_t WriteOperation;               /*!< Enables or disables the write operation in the selected device by the FMC. 
                                              This parameter can be a value of @ref FMC_Write_Operation                  */

  uint32_t WaitSignal;                   /*!< Enables or disables the wait state insertion via wait
                                              signal, valid for Flash memory access in burst mode. 
                                              This parameter can be a value of @ref FMC_Wait_Signal                      */

  uint32_t ExtendedMode;                 /*!< Enables or disables the extended mode.
                                              This parameter can be a value of @ref FMC_Extended_Mode                    */

  uint32_t AsynchronousWait;             /*!< Enables or disables wait signal during asynchronous transfers,
                                              valid only with asynchronous Flash memories.
                                              This parameter can be a value of @ref FMC_AsynchronousWait                 */

  uint32_t WriteBurst;                   /*!< Enables or disables the write burst operation.
                                              This parameter can be a value of @ref FMC_Write_Burst                      */

  uint32_t ContinuousClock;              /*!< Enables or disables the FMC clock output to external memory devices.
                                              This parameter is only enabled through the FMC_BCR1 register, and don't care 
                                              through FMC_BCR2..4 registers.
                                              This parameter can be a value of @ref FMC_Continous_Clock                  */

  uint32_t WriteFifo;                    /*!< Enables or disables the write FIFO used by the FMC controller.
                                              This parameter is only enabled through the FMC_BCR1 register, and don't care 
                                              through FMC_BCR2..4 registers.
                                              This parameter can be a value of @ref FMC_Write_FIFO.
                                              @note This Parameter is not available for STM32L47x/L48x devices.         */

  uint32_t PageSize;                     /*!< Specifies the memory page size.
                                              This parameter can be a value of @ref FMC_Page_Size                        */

}FMC_NORSRAM_InitTypeDef;

/** 
  * @brief  FMC_NORSRAM Timing parameters structure definition  
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
                                              This parameter can be a value of @ref FMC_Access_Mode                      */
}FMC_NORSRAM_TimingTypeDef;

/** 
  * @brief  FMC_NAND Configuration Structure definition  
  */ 
typedef struct
{
  uint32_t NandBank;               /*!< Specifies the NAND memory device that will be used.
                                        This parameter can be a value of @ref FMC_NAND_Bank                    */

  uint32_t Waitfeature;            /*!< Enables or disables the Wait feature for the NAND Memory device.
                                        This parameter can be any value of @ref FMC_Wait_feature               */

  uint32_t MemoryDataWidth;        /*!< Specifies the external memory device width.
                                        This parameter can be any value of @ref FMC_NAND_Data_Width            */

  uint32_t EccComputation;         /*!< Enables or disables the ECC computation.
                                        This parameter can be any value of @ref FMC_ECC                        */

  uint32_t ECCPageSize;            /*!< Defines the page size for the extended ECC.
                                        This parameter can be any value of @ref FMC_ECC_Page_Size              */

  uint32_t TCLRSetupTime;          /*!< Defines the number of HCLK cycles to configure the
                                        delay between CLE low and RE low.
                                        This parameter can be a value between Min_Data = 0 and Max_Data = 255  */

  uint32_t TARSetupTime;           /*!< Defines the number of HCLK cycles to configure the
                                        delay between ALE low and RE low.
                                        This parameter can be a number between Min_Data = 0 and Max_Data = 255 */
}FMC_NAND_InitTypeDef;

/** 
  * @brief  FMC_NAND Timing parameters structure definition
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
}FMC_NAND_PCC_TimingTypeDef;

/**
  * @}
  */ 

/* Exported constants --------------------------------------------------------*/
/** @addtogroup FMC_LL_Exported_Constants FMC Low Layer Exported Constants
  * @{
  */

/** @defgroup FMC_NOR_SRAM_Exported_constants FMC NOR/SRAM Exported constants
  * @{
  */

/** @defgroup FMC_NORSRAM_Bank FMC NOR/SRAM Bank
  * @{
  */
#define FMC_NORSRAM_BANK1                       ((uint32_t)0x00000000)
#define FMC_NORSRAM_BANK2                       ((uint32_t)0x00000002)
#define FMC_NORSRAM_BANK3                       ((uint32_t)0x00000004)
#define FMC_NORSRAM_BANK4                       ((uint32_t)0x00000006)
/**
  * @}
  */

/** @defgroup FMC_Data_Address_Bus_Multiplexing FMC Data Address Bus Multiplexing
  * @{
  */
#define FMC_DATA_ADDRESS_MUX_DISABLE            ((uint32_t)0x00000000)
#define FMC_DATA_ADDRESS_MUX_ENABLE             ((uint32_t)FMC_BCRx_MUXEN)
/**
  * @}
  */

/** @defgroup FMC_Memory_Type FMC Memory Type
  * @{
  */
#define FMC_MEMORY_TYPE_SRAM                    ((uint32_t)0x00000000)
#define FMC_MEMORY_TYPE_PSRAM                   ((uint32_t)FMC_BCRx_MTYP_0)
#define FMC_MEMORY_TYPE_NOR                     ((uint32_t)FMC_BCRx_MTYP_1)
/**
  * @}
  */

/** @defgroup FMC_NORSRAM_Data_Width FMC NORSRAM Data Width
  * @{
  */
#define FMC_NORSRAM_MEM_BUS_WIDTH_8             ((uint32_t)0x00000000)
#define FMC_NORSRAM_MEM_BUS_WIDTH_16            ((uint32_t)FMC_BCRx_MWID_0)
#define FMC_NORSRAM_MEM_BUS_WIDTH_32            ((uint32_t)FMC_BCRx_MWID_1)
/**
  * @}
  */

/** @defgroup FMC_NORSRAM_Flash_Access FMC NORSRAM Flash Access
  * @{
  */
#define FMC_NORSRAM_FLASH_ACCESS_ENABLE         ((uint32_t)FMC_BCRx_FACCEN)
#define FMC_NORSRAM_FLASH_ACCESS_DISABLE        ((uint32_t)0x00000000)
/**
  * @}
  */

/** @defgroup FMC_Burst_Access_Mode FMC Burst Access Mode
  * @{
  */
#define FMC_BURST_ACCESS_MODE_DISABLE           ((uint32_t)0x00000000) 
#define FMC_BURST_ACCESS_MODE_ENABLE            ((uint32_t)FMC_BCRx_BURSTEN)
/**
  * @}
  */
    

/** @defgroup FMC_Wait_Signal_Polarity FMC Wait Signal Polarity
  * @{
  */
#define FMC_WAIT_SIGNAL_POLARITY_LOW            ((uint32_t)0x00000000)
#define FMC_WAIT_SIGNAL_POLARITY_HIGH           ((uint32_t)FMC_BCRx_WAITPOL)
/**
  * @}
  */

/** @defgroup FMC_Wait_Timing FMC Wait Timing
  * @{
  */
#define FMC_WAIT_TIMING_BEFORE_WS               ((uint32_t)0x00000000)
#define FMC_WAIT_TIMING_DURING_WS               ((uint32_t)FMC_BCRx_WAITCFG)

/**
  * @}
  */

/** @defgroup FMC_Write_Operation FMC Write Operation
  * @{
  */
#define FMC_WRITE_OPERATION_DISABLE             ((uint32_t)0x00000000)
#define FMC_WRITE_OPERATION_ENABLE              ((uint32_t)FMC_BCRx_WREN)
/**
  * @}
  */

/** @defgroup FMC_Wait_Signal FMC Wait Signal
  * @{
  */
#define FMC_WAIT_SIGNAL_DISABLE                 ((uint32_t)0x00000000)
#define FMC_WAIT_SIGNAL_ENABLE                  ((uint32_t)FMC_BCRx_WAITEN)
/**
  * @}
  */

/** @defgroup FMC_Extended_Mode FMC Extended Mode
  * @{
  */
#define FMC_EXTENDED_MODE_DISABLE               ((uint32_t)0x00000000)
#define FMC_EXTENDED_MODE_ENABLE                ((uint32_t)FMC_BCRx_EXTMOD)
/**
  * @}
  */

/** @defgroup FMC_AsynchronousWait FMC Asynchronous Wait
  * @{
  */
#define FMC_ASYNCHRONOUS_WAIT_DISABLE           ((uint32_t)0x00000000)
#define FMC_ASYNCHRONOUS_WAIT_ENABLE            ((uint32_t)FMC_BCRx_ASYNCWAIT)
/**
  * @}
  */  

/** @defgroup FMC_Page_Size FMC Page Size
  * @{
  */
#define FMC_PAGE_SIZE_NONE                      ((uint32_t)0x00000000)
#define FMC_PAGE_SIZE_128                       ((uint32_t)FMC_BCRx_CPSIZE_0)
#define FMC_PAGE_SIZE_256                       ((uint32_t)FMC_BCRx_CPSIZE_1)
#define FMC_PAGE_SIZE_512                       ((uint32_t)(FMC_BCRx_CPSIZE_0 | FMC_BCRx_CPSIZE_1))
#define FMC_PAGE_SIZE_1024                      ((uint32_t)FMC_BCRx_CPSIZE_2)
/**
  * @}
  */  

/** @defgroup FMC_Write_Burst FMC Write Burst
  * @{
  */
#define FMC_WRITE_BURST_DISABLE                 ((uint32_t)0x00000000)
#define FMC_WRITE_BURST_ENABLE                  ((uint32_t)FMC_BCRx_CBURSTRW)
/**
  * @}
  */
  
/** @defgroup FMC_Continous_Clock FMC Continous Clock
  * @{
  */
#define FMC_CONTINUOUS_CLOCK_SYNC_ONLY          ((uint32_t)0x00000000)
#define FMC_CONTINUOUS_CLOCK_SYNC_ASYNC         ((uint32_t)FMC_BCR1_CCLKEN)
/**
  * @}
  */

/** @defgroup FMC_Access_Mode FMC Access Mode
  * @{
  */
#define FMC_ACCESS_MODE_A                       ((uint32_t)0x00000000)
#define FMC_ACCESS_MODE_B                       ((uint32_t)FMC_BTRx_ACCMOD_0) 
#define FMC_ACCESS_MODE_C                       ((uint32_t)FMC_BTRx_ACCMOD_1)
#define FMC_ACCESS_MODE_D                       ((uint32_t)(FMC_BTRx_ACCMOD_0|FMC_BTRx_ACCMOD_1))
/**
  * @}
  */ 

/**
  * @}
  */
    
/**
  * @}
  */  

/** @defgroup FMC_NAND_Controller FMC NAND Exported constants
  * @{
  */

/** @defgroup FMC_NAND_Bank FMC NAND Bank
  * @{
  */  
#define FMC_NAND_BANK3                          ((uint32_t)0x00000100)
/**
  * @}
  */

/** @defgroup FMC_Wait_feature FMC Wait feature
  * @{
  */
#define FMC_NAND_WAIT_FEATURE_DISABLE           ((uint32_t)0x00000000)
#define FMC_NAND_WAIT_FEATURE_ENABLE            ((uint32_t)FMC_PCR_PWAITEN)
/**
  * @}
  */

/** @defgroup FMC_PCR_Memory_Type FMC PCR Memory Type
  * @{
  */
#define FMC_PCR_MEMORY_TYPE_NAND                ((uint32_t)FMC_PCR_PTYP)
/**
  * @}
  */

/** @defgroup FMC_NAND_Data_Width FMC NAND Data Width
  * @{
  */
#define FMC_NAND_MEM_BUS_WIDTH_8                ((uint32_t)0x00000000)
#define FMC_NAND_MEM_BUS_WIDTH_16               ((uint32_t)FMC_PCR_PWID_0)
/**
  * @}
  */

/** @defgroup FMC_ECC FMC NAND ECC
  * @{
  */
#define FMC_NAND_ECC_DISABLE                    ((uint32_t)0x00000000)
#define FMC_NAND_ECC_ENABLE                     ((uint32_t)FMC_PCR_ECCEN)
/**
  * @}
  */

/** @defgroup FMC_ECC_Page_Size FMC NAND ECC Page Size
  * @{
  */
#define FMC_NAND_ECC_PAGE_SIZE_256BYTE          ((uint32_t)0x00000000)
#define FMC_NAND_ECC_PAGE_SIZE_512BYTE          ((uint32_t)FMC_PCR_ECCPS_0)
#define FMC_NAND_ECC_PAGE_SIZE_1024BYTE         ((uint32_t)FMC_PCR_ECCPS_1)
#define FMC_NAND_ECC_PAGE_SIZE_2048BYTE         ((uint32_t)FMC_PCR_ECCPS_0|FMC_PCR_ECCPS_1)
#define FMC_NAND_ECC_PAGE_SIZE_4096BYTE         ((uint32_t)FMC_PCR_ECCPS_2)
#define FMC_NAND_ECC_PAGE_SIZE_8192BYTE         ((uint32_t)FMC_PCR_ECCPS_0|FMC_PCR_ECCPS_2)
/**
  * @}
  */

/**
  * @}
  */  

/**
  * @}
  */ 

/** @defgroup FMC_Interrupt_definition FMC Interrupt definition
  * @brief FMC Interrupt definition
  * @{
  */  
#define FMC_IT_RISING_EDGE                      ((uint32_t)FMC_SR_IREN)
#define FMC_IT_LEVEL                            ((uint32_t)FMC_SR_ILEN)
#define FMC_IT_FALLING_EDGE                     ((uint32_t)FMC_SR_IFEN)
/**
  * @}
  */
    
/** @defgroup FMC_Flag_definition FMC Flag definition
  * @brief FMC Flag definition
  * @{
  */ 
#define FMC_FLAG_RISING_EDGE                    ((uint32_t)FMC_SR_IRS)
#define FMC_FLAG_LEVEL                          ((uint32_t)FMC_SR_ILS)
#define FMC_FLAG_FALLING_EDGE                   ((uint32_t)FMC_SR_IFS)
#define FMC_FLAG_FEMPT                          ((uint32_t)FMC_SR_FEMPT)
/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/** @defgroup FMC_Exported_Macros FMC Low Layer Exported Macros
  * @{
  */ 
  
/** @defgroup FMC_NOR_Macros FMC NOR/SRAM Exported Macros
 *  @brief macros to handle NOR device enable/disable and read/write operations
 *  @{
 */
 
/**
  * @brief  Enable the NORSRAM device access.
  * @param  __INSTANCE__: FMC_NORSRAM Instance
  * @param  __BANK__: FMC_NORSRAM Bank     
  * @retval none
  */ 
#define __FMC_NORSRAM_ENABLE(__INSTANCE__, __BANK__)  SET_BIT((__INSTANCE__)->BTCR[(__BANK__)], FMC_BCRx_MBKEN)

/**
  * @brief  Disable the NORSRAM device access.
  * @param  __INSTANCE__: FMC_NORSRAM Instance
  * @param  __BANK__: FMC_NORSRAM Bank   
  * @retval none
  */ 
#define __FMC_NORSRAM_DISABLE(__INSTANCE__, __BANK__) CLEAR_BIT((__INSTANCE__)->BTCR[(__BANK__)], FMC_BCRx_MBKEN)  

/**
  * @}
  */ 

/** @defgroup FMC_NAND_Macros FMC NAND Macros
 *  @brief macros to handle NAND device enable/disable
 *  @{
 */
 
/**
  * @brief  Enable the NAND device access.
  * @param  __INSTANCE__: FMC_NAND Instance
  * @param  __BANK__: FMC_NAND Bank    
  * @retval none
  */  
#define __FMC_NAND_ENABLE(__INSTANCE__, __BANK__)  SET_BIT((__INSTANCE__)->PCR, FMC_PCR_PBKEN)

/**
  * @brief  Disable the NAND device access.
  * @param  __INSTANCE__: FMC_NAND Instance
  * @param  __BANK__: FMC_NAND Bank  
  * @retval None
  */
#define __FMC_NAND_DISABLE(__INSTANCE__, __BANK__) CLEAR_BIT((__INSTANCE__)->PCR, FMC_PCR_PBKEN)

/**
  * @}
  */ 
    
/** @defgroup FMC_Interrupt FMC Interrupt
 *  @brief macros to handle FMC interrupts
 * @{
 */ 

/**
  * @brief  Enable the NAND device interrupt.
  * @param  __INSTANCE__:  FMC_NAND Instance
  * @param  __BANK__:      FMC_NAND Bank     
  * @param  __INTERRUPT__: FMC_NAND interrupt 
  *         This parameter can be any combination of the following values:
  *            @arg FMC_IT_RISING_EDGE: Interrupt rising edge.
  *            @arg FMC_IT_LEVEL: Interrupt level.
  *            @arg FMC_IT_FALLING_EDGE: Interrupt falling edge.       
  * @retval None
  */  
#define __FMC_NAND_ENABLE_IT(__INSTANCE__, __BANK__, __INTERRUPT__)  SET_BIT((__INSTANCE__)->SR, (__INTERRUPT__))

/**
  * @brief  Disable the NAND device interrupt.
  * @param  __INSTANCE__:  FMC_NAND Instance
  * @param  __BANK__:      FMC_NAND Bank    
  * @param  __INTERRUPT__: FMC_NAND interrupt
  *         This parameter can be any combination of the following values:
  *            @arg FMC_IT_RISING_EDGE: Interrupt rising edge.
  *            @arg FMC_IT_LEVEL: Interrupt level.
  *            @arg FMC_IT_FALLING_EDGE: Interrupt falling edge.   
  * @retval None
  */
#define __FMC_NAND_DISABLE_IT(__INSTANCE__, __BANK__, __INTERRUPT__)  CLEAR_BIT((__INSTANCE__)->SR, (__INTERRUPT__))
                                                                                                                           
/**
  * @brief  Get flag status of the NAND device.
  * @param  __INSTANCE__: FMC_NAND Instance
  * @param  __BANK__:     FMC_NAND Bank     
  * @param  __FLAG__: FMC_NAND flag
  *         This parameter can be any combination of the following values:
  *            @arg FMC_FLAG_RISING_EDGE: Interrupt rising edge flag.
  *            @arg FMC_FLAG_LEVEL: Interrupt level edge flag.
  *            @arg FMC_FLAG_FALLING_EDGE: Interrupt falling edge flag.
  *            @arg FMC_FLAG_FEMPT: FIFO empty flag.   
  * @retval The state of FLAG (SET or RESET).
  */
#define __FMC_NAND_GET_FLAG(__INSTANCE__, __BANK__, __FLAG__)  (((__INSTANCE__)->SR &(__FLAG__)) == (__FLAG__))

/**
  * @brief  Clear flag status of the NAND device.
  * @param  __INSTANCE__: FMC_NAND Instance  
  * @param  __BANK__:     FMC_NAND Bank  
  * @param  __FLAG__: FMC_NAND flag
  *         This parameter can be any combination of the following values:
  *            @arg FMC_FLAG_RISING_EDGE: Interrupt rising edge flag.
  *            @arg FMC_FLAG_LEVEL: Interrupt level edge flag.
  *            @arg FMC_FLAG_FALLING_EDGE: Interrupt falling edge flag.
  *            @arg FMC_FLAG_FEMPT: FIFO empty flag.   
  * @retval None
  */
#define __FMC_NAND_CLEAR_FLAG(__INSTANCE__, __BANK__, __FLAG__)  CLEAR_BIT((__INSTANCE__)->SR, (__FLAG__))  


/* Exported functions --------------------------------------------------------*/
/** @addgroup FMC_LL_Exported_Functions FMC Low Layer Exported Functions
  * @{
  */

/* FMC_LL_NORSRAM Controller functions *******************************************/
/** @addgroup FMC_LL_NORSRAM_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
/* Initialization/de-initialization functions */
HAL_StatusTypeDef  FMC_NORSRAM_Init(FMC_NORSRAM_TypeDef *Device, FMC_NORSRAM_InitTypeDef *Init);
HAL_StatusTypeDef  FMC_NORSRAM_Timing_Init(FMC_NORSRAM_TypeDef *Device, FMC_NORSRAM_TimingTypeDef *Timing, uint32_t Bank);
HAL_StatusTypeDef  FMC_NORSRAM_Extended_Timing_Init(FMC_NORSRAM_EXTENDED_TypeDef *Device, FMC_NORSRAM_TimingTypeDef *Timing, uint32_t Bank, uint32_t ExtendedMode);
HAL_StatusTypeDef  FMC_NORSRAM_DeInit(FMC_NORSRAM_TypeDef *Device, FMC_NORSRAM_EXTENDED_TypeDef *ExDevice, uint32_t Bank);
/**
  * @}
  */ 

/** @addtogroup FMC_LL_NORSRAM_Exported_Functions_Group2 Peripheral Control functions 
  * @{
  */
/* FMC_NORSRAM Control functions */
HAL_StatusTypeDef  FMC_NORSRAM_WriteOperation_Enable(FMC_NORSRAM_TypeDef *Device, uint32_t Bank);
HAL_StatusTypeDef  FMC_NORSRAM_WriteOperation_Disable(FMC_NORSRAM_TypeDef *Device, uint32_t Bank);
/**
  * @}
  */ 

/* FMC_NAND Controller functions **********************************************/
/** @addtogroup FMC_LL_NAND_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
/* Initialization/de-initialization functions */
HAL_StatusTypeDef  FMC_NAND_Init(FMC_NAND_TypeDef *Device, FMC_NAND_InitTypeDef *Init);
HAL_StatusTypeDef  FMC_NAND_CommonSpace_Timing_Init(FMC_NAND_TypeDef *Device, FMC_NAND_PCC_TimingTypeDef *Timing, uint32_t Bank);
HAL_StatusTypeDef  FMC_NAND_AttributeSpace_Timing_Init(FMC_NAND_TypeDef *Device, FMC_NAND_PCC_TimingTypeDef *Timing, uint32_t Bank);
HAL_StatusTypeDef  FMC_NAND_DeInit(FMC_NAND_TypeDef *Device, uint32_t Bank);
/**
  * @}
  */ 

/** @defgroup FMC_LL_NAND_Exported_Functions_Group2 FMC Low Layer Peripheral Control functions 
  * @{
  */
/* FMC_NAND Control functions */
HAL_StatusTypeDef  FMC_NAND_ECC_Enable(FMC_NAND_TypeDef *Device, uint32_t Bank);
HAL_StatusTypeDef  FMC_NAND_ECC_Disable(FMC_NAND_TypeDef *Device, uint32_t Bank);
HAL_StatusTypeDef  FMC_NAND_GetECC(FMC_NAND_TypeDef *Device, uint32_t *ECCval, uint32_t Bank, uint32_t Timeout);
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32L4xx_LL_FMC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

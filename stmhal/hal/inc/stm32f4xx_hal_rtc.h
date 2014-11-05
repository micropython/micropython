/**
  ******************************************************************************
  * @file    stm32f4xx_hal_rtc.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   Header file of RTC HAL module.
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
#ifndef __STM32F4xx_HAL_RTC_H
#define __STM32F4xx_HAL_RTC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup RTC
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 
/** 
  * @brief  HAL State structures definition  
  */ 
typedef enum
{
  HAL_RTC_STATE_RESET             = 0x00,  /*!< RTC not yet initialized or disabled */
  HAL_RTC_STATE_READY             = 0x01,  /*!< RTC initialized and ready for use   */
  HAL_RTC_STATE_BUSY              = 0x02,  /*!< RTC process is ongoing              */     
  HAL_RTC_STATE_TIMEOUT           = 0x03,  /*!< RTC timeout state                   */  
  HAL_RTC_STATE_ERROR             = 0x04   /*!< RTC error state                     */      
                                                                        
}HAL_RTCStateTypeDef;

/** 
  * @brief  RTC Configuration Structure definition  
  */
typedef struct
{
  uint32_t HourFormat;      /*!< Specifies the RTC Hour Format.
                                 This parameter can be a value of @ref RTC_Hour_Formats */         

  uint32_t AsynchPrediv;    /*!< Specifies the RTC Asynchronous Predivider value.
                                 This parameter must be a number between Min_Data = 0x00 and Max_Data = 0x7F */        
                               
  uint32_t SynchPrediv;     /*!< Specifies the RTC Synchronous Predivider value.
                                 This parameter must be a number between Min_Data = 0x00 and Max_Data = 0x7FFF */   
  
  uint32_t OutPut;          /*!< Specifies which signal will be routed to the RTC output.   
                                 This parameter can be a value of @ref RTC_Output_selection_Definitions */      
  
  uint32_t OutPutPolarity;  /*!< Specifies the polarity of the output signal.  
                                 This parameter can be a value of @ref RTC_Output_Polarity_Definitions */ 
  
  uint32_t OutPutType;      /*!< Specifies the RTC Output Pin mode.   
                                 This parameter can be a value of @ref RTC_Output_Type_ALARM_OUT */             
}RTC_InitTypeDef;

/** 
  * @brief  RTC Time structure definition  
  */
typedef struct
{
  uint8_t Hours;            /*!< Specifies the RTC Time Hour.
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 12 if the RTC_HourFormat_12 is selected.
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 23 if the RTC_HourFormat_24 is selected  */

  uint8_t Minutes;          /*!< Specifies the RTC Time Minutes.
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 59 */
  
  uint8_t Seconds;          /*!< Specifies the RTC Time Seconds.
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 59 */
  
  uint32_t SubSeconds;      /*!< Specifies the RTC Time SubSeconds.
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 59 */

  uint8_t TimeFormat;       /*!< Specifies the RTC AM/PM Time.
                                 This parameter can be a value of @ref RTC_AM_PM_Definitions */ 
  
  uint32_t DayLightSaving;  /*!< Specifies DayLight Save Operation.
                                 This parameter can be a value of @ref RTC_DayLightSaving_Definitions */
  
  uint32_t StoreOperation;  /*!< Specifies RTC_StoreOperation value to be written in the BCK bit 
                                 in CR register to store the operation.
                                 This parameter can be a value of @ref RTC_StoreOperation_Definitions */
}RTC_TimeTypeDef; 
  
/** 
  * @brief  RTC Date structure definition  
  */
typedef struct
{
  uint8_t WeekDay;  /*!< Specifies the RTC Date WeekDay.
                         This parameter can be a value of @ref RTC_WeekDay_Definitions */
  
  uint8_t Month;    /*!< Specifies the RTC Date Month (in BCD format).
                         This parameter can be a value of @ref RTC_Month_Date_Definitions */

  uint8_t Date;     /*!< Specifies the RTC Date.
                         This parameter must be a number between Min_Data = 1 and Max_Data = 31 */
  
  uint8_t Year;     /*!< Specifies the RTC Date Year.
                         This parameter must be a number between Min_Data = 0 and Max_Data = 99 */
                        
}RTC_DateTypeDef;

/** 
  * @brief  RTC Alarm structure definition  
  */
typedef struct
{
  RTC_TimeTypeDef AlarmTime;     /*!< Specifies the RTC Alarm Time members */
    
  uint32_t AlarmMask;            /*!< Specifies the RTC Alarm Masks.
                                      This parameter can be a value of @ref RTC_AlarmMask_Definitions */
  
  uint32_t AlarmSubSecondMask;   /*!< Specifies the RTC Alarm SubSeconds Masks.
                                      This parameter can be a value of @ref RTC_Alarm_Sub_Seconds_Masks_Definitions */                                   

  uint32_t AlarmDateWeekDaySel;  /*!< Specifies the RTC Alarm is on Date or WeekDay.
                                     This parameter can be a value of @ref RTC_AlarmDateWeekDay_Definitions */
  
  uint8_t AlarmDateWeekDay;      /*!< Specifies the RTC Alarm Date/WeekDay.
                                      If the Alarm Date is selected, this parameter must be set to a value in the 1-31 range.
                                      If the Alarm WeekDay is selected, this parameter can be a value of @ref RTC_WeekDay_Definitions */
                                                                     
  uint32_t Alarm;                /*!< Specifies the alarm .
                                      This parameter can be a value of @ref RTC_Alarms_Definitions */                            
}RTC_AlarmTypeDef;

/** 
  * @brief  Time Handle Structure definition  
  */ 
typedef struct
{
  RTC_TypeDef                 *Instance;  /*!< Register base address    */
   
  RTC_InitTypeDef             Init;       /*!< RTC required parameters  */ 
  
  HAL_LockTypeDef             Lock;       /*!< RTC locking object       */
  
  __IO HAL_RTCStateTypeDef    State;      /*!< Time communication state */
    
}RTC_HandleTypeDef;

/* Exported constants --------------------------------------------------------*/
/** @defgroup RTC_Exported_Constants
  * @{
  */ 

/* Masks Definition */
#define RTC_TR_RESERVED_MASK    ((uint32_t)0x007F7F7F)
#define RTC_DR_RESERVED_MASK    ((uint32_t)0x00FFFF3F) 
#define RTC_INIT_MASK           ((uint32_t)0xFFFFFFFF)  
#define RTC_RSF_MASK            ((uint32_t)0xFFFFFF5F)
#define RTC_FLAGS_MASK          ((uint32_t)(RTC_FLAG_TSOVF | RTC_FLAG_TSF | RTC_FLAG_WUTF | \
                                            RTC_FLAG_ALRBF | RTC_FLAG_ALRAF | RTC_FLAG_INITF | \
                                            RTC_FLAG_RSF | RTC_FLAG_INITS | RTC_FLAG_WUTWF | \
                                            RTC_FLAG_ALRBWF | RTC_FLAG_ALRAWF | RTC_FLAG_TAMP1F | \
                                            RTC_FLAG_RECALPF | RTC_FLAG_SHPF))

#define RTC_TIMEOUT_VALUE       1000
 
/** @defgroup RTC_Hour_Formats 
  * @{
  */ 
#define RTC_HOURFORMAT_24              ((uint32_t)0x00000000)
#define RTC_HOURFORMAT_12              ((uint32_t)0x00000040)

#define IS_RTC_HOUR_FORMAT(FORMAT)     (((FORMAT) == RTC_HOURFORMAT_12) || \
                                        ((FORMAT) == RTC_HOURFORMAT_24))
/**
  * @}
  */ 
  
/** @defgroup RTC_Output_selection_Definitions 
  * @{
  */ 
#define RTC_OUTPUT_DISABLE             ((uint32_t)0x00000000)
#define RTC_OUTPUT_ALARMA              ((uint32_t)0x00200000)
#define RTC_OUTPUT_ALARMB              ((uint32_t)0x00400000)
#define RTC_OUTPUT_WAKEUP              ((uint32_t)0x00600000)
 
#define IS_RTC_OUTPUT(OUTPUT) (((OUTPUT) == RTC_OUTPUT_DISABLE) || \
                               ((OUTPUT) == RTC_OUTPUT_ALARMA)  || \
                               ((OUTPUT) == RTC_OUTPUT_ALARMB)  || \
                               ((OUTPUT) == RTC_OUTPUT_WAKEUP))
/**
  * @}
  */ 

/** @defgroup RTC_Output_Polarity_Definitions 
  * @{
  */ 
#define RTC_OUTPUT_POLARITY_HIGH       ((uint32_t)0x00000000)
#define RTC_OUTPUT_POLARITY_LOW        ((uint32_t)0x00100000)

#define IS_RTC_OUTPUT_POL(POL) (((POL) == RTC_OUTPUT_POLARITY_HIGH) || \
                                ((POL) == RTC_OUTPUT_POLARITY_LOW))
/**
  * @}
  */ 

/** @defgroup RTC_Output_Type_ALARM_OUT 
  * @{
  */ 
#define RTC_OUTPUT_TYPE_OPENDRAIN      ((uint32_t)0x00000000)
#define RTC_OUTPUT_TYPE_PUSHPULL       ((uint32_t)0x00040000)

#define IS_RTC_OUTPUT_TYPE(TYPE) (((TYPE) == RTC_OUTPUT_TYPE_OPENDRAIN) || \
                                  ((TYPE) == RTC_OUTPUT_TYPE_PUSHPULL))

/**
  * @}
  */ 

/** @defgroup RTC_Asynchronous_Predivider 
  * @{
  */ 
#define IS_RTC_ASYNCH_PREDIV(PREDIV)   ((PREDIV) <= (uint32_t)0x7F) 
/**
  * @}
  */ 


/** @defgroup RTC_Synchronous_Predivider 
  * @{
  */ 
#define IS_RTC_SYNCH_PREDIV(PREDIV)    ((PREDIV) <= (uint32_t)0x7FFF)
/**
  * @}
  */ 

/** @defgroup RTC_Time_Definitions 
  * @{
  */ 
#define IS_RTC_HOUR12(HOUR)            (((HOUR) > (uint32_t)0) && ((HOUR) <= (uint32_t)12))
#define IS_RTC_HOUR24(HOUR)            ((HOUR) <= (uint32_t)23)
#define IS_RTC_MINUTES(MINUTES)        ((MINUTES) <= (uint32_t)59)
#define IS_RTC_SECONDS(SECONDS)        ((SECONDS) <= (uint32_t)59)
/**
  * @}
  */ 

/** @defgroup RTC_AM_PM_Definitions 
  * @{
  */ 
#define RTC_HOURFORMAT12_AM            ((uint8_t)0x00)
#define RTC_HOURFORMAT12_PM            ((uint8_t)0x40)

#define IS_RTC_HOURFORMAT12(PM)  (((PM) == RTC_HOURFORMAT12_AM) || ((PM) == RTC_HOURFORMAT12_PM))
/**
  * @}
  */ 

/** @defgroup RTC_DayLightSaving_Definitions 
  * @{
  */ 
#define RTC_DAYLIGHTSAVING_SUB1H       ((uint32_t)0x00020000)
#define RTC_DAYLIGHTSAVING_ADD1H       ((uint32_t)0x00010000)
#define RTC_DAYLIGHTSAVING_NONE        ((uint32_t)0x00000000)

#define IS_RTC_DAYLIGHT_SAVING(SAVE) (((SAVE) == RTC_DAYLIGHTSAVING_SUB1H) || \
                                      ((SAVE) == RTC_DAYLIGHTSAVING_ADD1H) || \
                                      ((SAVE) == RTC_DAYLIGHTSAVING_NONE))
/**
  * @}
  */

/** @defgroup RTC_StoreOperation_Definitions 
  * @{
  */ 
#define RTC_STOREOPERATION_RESET        ((uint32_t)0x00000000)
#define RTC_STOREOPERATION_SET          ((uint32_t)0x00040000)

#define IS_RTC_STORE_OPERATION(OPERATION) (((OPERATION) == RTC_STOREOPERATION_RESET) || \
                                           ((OPERATION) == RTC_STOREOPERATION_SET))
/**
  * @}
  */

/** @defgroup RTC_Input_parameter_format_definitions 
  * @{
  */ 
#define FORMAT_BIN                      ((uint32_t)0x000000000)
#define FORMAT_BCD                      ((uint32_t)0x000000001)

#define IS_RTC_FORMAT(FORMAT) (((FORMAT) == FORMAT_BIN) || ((FORMAT) == FORMAT_BCD))
/**
  * @}
  */

/** @defgroup RTC_Year_Date_Definitions 
  * @{
  */ 
#define IS_RTC_YEAR(YEAR)              ((YEAR) <= (uint32_t)99)
/**
  * @}
  */ 

/** @defgroup RTC_Month_Date_Definitions 
  * @{
  */ 

/* Coded in BCD format */
#define RTC_MONTH_JANUARY              ((uint8_t)0x01)
#define RTC_MONTH_FEBRUARY             ((uint8_t)0x02)
#define RTC_MONTH_MARCH                ((uint8_t)0x03)
#define RTC_MONTH_APRIL                ((uint8_t)0x04)
#define RTC_MONTH_MAY                  ((uint8_t)0x05)
#define RTC_MONTH_JUNE                 ((uint8_t)0x06)
#define RTC_MONTH_JULY                 ((uint8_t)0x07)
#define RTC_MONTH_AUGUST               ((uint8_t)0x08)
#define RTC_MONTH_SEPTEMBER            ((uint8_t)0x09)
#define RTC_MONTH_OCTOBER              ((uint8_t)0x10)
#define RTC_MONTH_NOVEMBER             ((uint8_t)0x11)
#define RTC_MONTH_DECEMBER             ((uint8_t)0x12)

#define IS_RTC_MONTH(MONTH)            (((MONTH) >= (uint32_t)1) && ((MONTH) <= (uint32_t)12))
#define IS_RTC_DATE(DATE)              (((DATE) >= (uint32_t)1) && ((DATE) <= (uint32_t)31))
/**
  * @}
  */ 

/** @defgroup RTC_WeekDay_Definitions 
  * @{
  */   
#define RTC_WEEKDAY_MONDAY             ((uint8_t)0x01)
#define RTC_WEEKDAY_TUESDAY            ((uint8_t)0x02)
#define RTC_WEEKDAY_WEDNESDAY          ((uint8_t)0x03)
#define RTC_WEEKDAY_THURSDAY           ((uint8_t)0x04)
#define RTC_WEEKDAY_FRIDAY             ((uint8_t)0x05)
#define RTC_WEEKDAY_SATURDAY           ((uint8_t)0x06)
#define RTC_WEEKDAY_SUNDAY             ((uint8_t)0x07)

#define IS_RTC_WEEKDAY(WEEKDAY) (((WEEKDAY) == RTC_WEEKDAY_MONDAY)    || \
                                 ((WEEKDAY) == RTC_WEEKDAY_TUESDAY)   || \
                                 ((WEEKDAY) == RTC_WEEKDAY_WEDNESDAY) || \
                                 ((WEEKDAY) == RTC_WEEKDAY_THURSDAY)  || \
                                 ((WEEKDAY) == RTC_WEEKDAY_FRIDAY)    || \
                                 ((WEEKDAY) == RTC_WEEKDAY_SATURDAY)  || \
                                 ((WEEKDAY) == RTC_WEEKDAY_SUNDAY))
/**
  * @}
  */ 
                                    
/** @defgroup RTC_Alarm_Definitions
  * @{
  */ 
#define IS_RTC_ALARM_DATE_WEEKDAY_DATE(DATE) (((DATE) >(uint32_t) 0) && ((DATE) <= (uint32_t)31))
#define IS_RTC_ALARM_DATE_WEEKDAY_WEEKDAY(WEEKDAY) (((WEEKDAY) == RTC_WEEKDAY_MONDAY)    || \
                                                    ((WEEKDAY) == RTC_WEEKDAY_TUESDAY)   || \
                                                    ((WEEKDAY) == RTC_WEEKDAY_WEDNESDAY) || \
                                                    ((WEEKDAY) == RTC_WEEKDAY_THURSDAY)  || \
                                                    ((WEEKDAY) == RTC_WEEKDAY_FRIDAY)    || \
                                                    ((WEEKDAY) == RTC_WEEKDAY_SATURDAY)  || \
                                                    ((WEEKDAY) == RTC_WEEKDAY_SUNDAY))
/**
  * @}
  */ 


/** @defgroup RTC_AlarmDateWeekDay_Definitions 
  * @{
  */ 
#define RTC_ALARMDATEWEEKDAYSEL_DATE      ((uint32_t)0x00000000)
#define RTC_ALARMDATEWEEKDAYSEL_WEEKDAY   ((uint32_t)0x40000000)

#define IS_RTC_ALARM_DATE_WEEKDAY_SEL(SEL) (((SEL) == RTC_ALARMDATEWEEKDAYSEL_DATE) || \
                                            ((SEL) == RTC_ALARMDATEWEEKDAYSEL_WEEKDAY))
/**
  * @}
  */ 


/** @defgroup RTC_AlarmMask_Definitions 
  * @{
  */ 
#define RTC_ALARMMASK_NONE                ((uint32_t)0x00000000)
#define RTC_ALARMMASK_DATEWEEKDAY         RTC_ALRMAR_MSK4
#define RTC_ALARMMASK_HOURS               RTC_ALRMAR_MSK3
#define RTC_ALARMMASK_MINUTES             RTC_ALRMAR_MSK2
#define RTC_ALARMMASK_SECONDS             RTC_ALRMAR_MSK1
#define RTC_ALARMMASK_ALL                 ((uint32_t)0x80808080)

#define IS_ALARM_MASK(MASK)  (((MASK) & 0x7F7F7F7F) == (uint32_t)RESET)
/**
  * @}
  */ 

/** @defgroup RTC_Alarms_Definitions 
  * @{
  */ 
#define RTC_ALARM_A                       RTC_CR_ALRAE
#define RTC_ALARM_B                       RTC_CR_ALRBE

#define IS_ALARM(ALARM)      (((ALARM) == RTC_ALARM_A) || ((ALARM) == RTC_ALARM_B))
/**
  * @}
  */ 

/** @defgroup RTC_Alarm_Sub_Seconds_Value
  * @{
  */ 
#define IS_RTC_ALARM_SUB_SECOND_VALUE(VALUE) ((VALUE) <= (uint32_t)0x00007FFF)
/**
  * @}
  */

  /** @defgroup RTC_Alarm_Sub_Seconds_Masks_Definitions
  * @{
  */ 
#define RTC_ALARMSUBSECONDMASK_ALL         ((uint32_t)0x00000000)  /*!< All Alarm SS fields are masked. 
                                                                        There is no comparison on sub seconds 
                                                                        for Alarm */
#define RTC_ALARMSUBSECONDMASK_SS14_1      ((uint32_t)0x01000000)  /*!< SS[14:1] are don't care in Alarm 
                                                                        comparison. Only SS[0] is compared.    */
#define RTC_ALARMSUBSECONDMASK_SS14_2      ((uint32_t)0x02000000)  /*!< SS[14:2] are don't care in Alarm 
                                                                        comparison. Only SS[1:0] are compared  */
#define RTC_ALARMSUBSECONDMASK_SS14_3      ((uint32_t)0x03000000)  /*!< SS[14:3] are don't care in Alarm 
                                                                        comparison. Only SS[2:0] are compared  */
#define RTC_ALARMSUBSECONDMASK_SS14_4      ((uint32_t)0x04000000)  /*!< SS[14:4] are don't care in Alarm 
                                                                        comparison. Only SS[3:0] are compared  */
#define RTC_ALARMSUBSECONDMASK_SS14_5      ((uint32_t)0x05000000)  /*!< SS[14:5] are don't care in Alarm 
                                                                        comparison. Only SS[4:0] are compared  */
#define RTC_ALARMSUBSECONDMASK_SS14_6      ((uint32_t)0x06000000)  /*!< SS[14:6] are don't care in Alarm 
                                                                        comparison. Only SS[5:0] are compared  */
#define RTC_ALARMSUBSECONDMASK_SS14_7      ((uint32_t)0x07000000)  /*!< SS[14:7] are don't care in Alarm 
                                                                        comparison. Only SS[6:0] are compared  */
#define RTC_ALARMSUBSECONDMASK_SS14_8      ((uint32_t)0x08000000)  /*!< SS[14:8] are don't care in Alarm 
                                                                        comparison. Only SS[7:0] are compared  */
#define RTC_ALARMSUBSECONDMASK_SS14_9      ((uint32_t)0x09000000)  /*!< SS[14:9] are don't care in Alarm 
                                                                        comparison. Only SS[8:0] are compared  */
#define RTC_ALARMSUBSECONDMASK_SS14_10     ((uint32_t)0x0A000000)  /*!< SS[14:10] are don't care in Alarm 
                                                                        comparison. Only SS[9:0] are compared  */
#define RTC_ALARMSUBSECONDMASK_SS14_11     ((uint32_t)0x0B000000)  /*!< SS[14:11] are don't care in Alarm 
                                                                        comparison. Only SS[10:0] are compared */
#define RTC_ALARMSUBSECONDMASK_SS14_12     ((uint32_t)0x0C000000)  /*!< SS[14:12] are don't care in Alarm 
                                                                        comparison.Only SS[11:0] are compared  */
#define RTC_ALARMSUBSECONDMASK_SS14_13     ((uint32_t)0x0D000000)  /*!< SS[14:13] are don't care in Alarm 
                                                                        comparison. Only SS[12:0] are compared */
#define RTC_ALARMSUBSECONDMASK_SS14        ((uint32_t)0x0E000000)  /*!< SS[14] is don't care in Alarm 
                                                                        comparison.Only SS[13:0] are compared  */
#define RTC_ALARMSUBSECONDMASK_None        ((uint32_t)0x0F000000)  /*!< SS[14:0] are compared and must match 
                                                                        to activate alarm. */

#define IS_RTC_ALARM_SUB_SECOND_MASK(MASK)   (((MASK) == RTC_ALARMSUBSECONDMASK_ALL) || \
                                              ((MASK) == RTC_ALARMSUBSECONDMASK_SS14_1) || \
                                              ((MASK) == RTC_ALARMSUBSECONDMASK_SS14_2) || \
                                              ((MASK) == RTC_ALARMSUBSECONDMASK_SS14_3) || \
                                              ((MASK) == RTC_ALARMSUBSECONDMASK_SS14_4) || \
                                              ((MASK) == RTC_ALARMSUBSECONDMASK_SS14_5) || \
                                              ((MASK) == RTC_ALARMSUBSECONDMASK_SS14_6) || \
                                              ((MASK) == RTC_ALARMSUBSECONDMASK_SS14_7) || \
                                              ((MASK) == RTC_ALARMSUBSECONDMASK_SS14_8) || \
                                              ((MASK) == RTC_ALARMSUBSECONDMASK_SS14_9) || \
                                              ((MASK) == RTC_ALARMSUBSECONDMASK_SS14_10) || \
                                              ((MASK) == RTC_ALARMSUBSECONDMASK_SS14_11) || \
                                              ((MASK) == RTC_ALARMSUBSECONDMASK_SS14_12) || \
                                              ((MASK) == RTC_ALARMSUBSECONDMASK_SS14_13) || \
                                              ((MASK) == RTC_ALARMSUBSECONDMASK_SS14) || \
                                              ((MASK) == RTC_ALARMSUBSECONDMASK_None))
/**
  * @}
  */   

/** @defgroup RTC_Interrupts_Definitions 
  * @{
  */ 
#define RTC_IT_TS                         ((uint32_t)0x00008000)
#define RTC_IT_WUT                        ((uint32_t)0x00004000)
#define RTC_IT_ALRB                       ((uint32_t)0x00002000)
#define RTC_IT_ALRA                       ((uint32_t)0x00001000)
#define RTC_IT_TAMP                       ((uint32_t)0x00000004) /* Used only to Enable the Tamper Interrupt */
#define RTC_IT_TAMP1                      ((uint32_t)0x00020000)
#define RTC_IT_TAMP2                      ((uint32_t)0x00040000)
/**
  * @}
  */

/** @defgroup RTC_Flags_Definitions 
  * @{
  */ 
#define RTC_FLAG_RECALPF                  ((uint32_t)0x00010000)
#define RTC_FLAG_TAMP2F                   ((uint32_t)0x00004000)
#define RTC_FLAG_TAMP1F                   ((uint32_t)0x00002000)
#define RTC_FLAG_TSOVF                    ((uint32_t)0x00001000)
#define RTC_FLAG_TSF                      ((uint32_t)0x00000800)
#define RTC_FLAG_WUTF                     ((uint32_t)0x00000400)
#define RTC_FLAG_ALRBF                    ((uint32_t)0x00000200)
#define RTC_FLAG_ALRAF                    ((uint32_t)0x00000100)
#define RTC_FLAG_INITF                    ((uint32_t)0x00000040)
#define RTC_FLAG_RSF                      ((uint32_t)0x00000020)
#define RTC_FLAG_INITS                    ((uint32_t)0x00000010)
#define RTC_FLAG_SHPF                     ((uint32_t)0x00000008)
#define RTC_FLAG_WUTWF                    ((uint32_t)0x00000004)
#define RTC_FLAG_ALRBWF                   ((uint32_t)0x00000002)
#define RTC_FLAG_ALRAWF                   ((uint32_t)0x00000001)
/**
  * @}
  */

/**
  * @}
  */ 
  
/* Exported macro ------------------------------------------------------------*/

/** @brief Reset RTC handle state
  * @param  __HANDLE__: specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_RTC_STATE_RESET)

/**
  * @brief  Disable the write protection for RTC registers.
  * @param  __HANDLE__: specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_WRITEPROTECTION_DISABLE(__HANDLE__)             \
                        do{                                       \
                            (__HANDLE__)->Instance->WPR = 0xCA;   \
                            (__HANDLE__)->Instance->WPR = 0x53;   \
                          } while(0)

/**
  * @brief  Enable the write protection for RTC registers.
  * @param  __HANDLE__: specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_WRITEPROTECTION_ENABLE(__HANDLE__)              \
                        do{                                       \
                            (__HANDLE__)->Instance->WPR = 0xFF;   \
                          } while(0)                            
 
/**
  * @brief  Enable the RTC ALARMA peripheral.
  * @param  __HANDLE__: specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_ALARMA_ENABLE(__HANDLE__)                           ((__HANDLE__)->Instance->CR |= (RTC_CR_ALRAE))

/**
  * @brief  Disable the RTC ALARMA peripheral.
  * @param  __HANDLE__: specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_ALARMA_DISABLE(__HANDLE__)                          ((__HANDLE__)->Instance->CR &= ~(RTC_CR_ALRAE))

/**
  * @brief  Enable the RTC ALARMB peripheral.
  * @param  __HANDLE__: specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_ALARMB_ENABLE(__HANDLE__)                           ((__HANDLE__)->Instance->CR |= (RTC_CR_ALRBE))

/**
  * @brief  Disable the RTC ALARMB peripheral.
  * @param  __HANDLE__: specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_ALARMB_DISABLE(__HANDLE__)                          ((__HANDLE__)->Instance->CR &= ~(RTC_CR_ALRBE))

/**
  * @brief  Enable the RTC Alarm interrupt.
  * @param  __HANDLE__: specifies the RTC handle.
  * @param  __INTERRUPT__: specifies the RTC Alarm interrupt sources to be enabled or disabled. 
  *          This parameter can be any combination of the following values:
  *             @arg RTC_IT_ALRA: Alarm A interrupt
  *             @arg RTC_IT_ALRB: Alarm B interrupt  
  * @retval None
  */   
#define __HAL_RTC_ALARM_ENABLE_IT(__HANDLE__, __INTERRUPT__)          ((__HANDLE__)->Instance->CR |= (__INTERRUPT__))

/**
  * @brief  Disable the RTC Alarm interrupt.
  * @param  __HANDLE__: specifies the RTC handle.
  * @param  __INTERRUPT__: specifies the RTC Alarm interrupt sources to be enabled or disabled. 
  *         This parameter can be any combination of the following values:
  *            @arg RTC_IT_ALRA: Alarm A interrupt
  *            @arg RTC_IT_ALRB: Alarm B interrupt  
  * @retval None
  */
#define __HAL_RTC_ALARM_DISABLE_IT(__HANDLE__, __INTERRUPT__)         ((__HANDLE__)->Instance->CR &= ~(__INTERRUPT__))

/**
  * @brief  Check whether the specified RTC Alarm interrupt has occurred or not.
  * @param  __HANDLE__: specifies the RTC handle.
  * @param  __FLAG__: specifies the RTC Alarm interrupt sources to be enabled or disabled.
  *         This parameter can be:
  *            @arg RTC_IT_ALRA: Alarm A interrupt
  *            @arg RTC_IT_ALRB: Alarm B interrupt  
  * @retval None
  */
#define __HAL_RTC_ALARM_GET_IT(__HANDLE__, __FLAG__)                  ((((((__HANDLE__)->Instance->ISR)& ((__FLAG__)>> 4)) & 0x0000FFFF) != RESET)? SET : RESET)

/**
  * @brief  Get the selected RTC Alarm's flag status.
  * @param  __HANDLE__: specifies the RTC handle.
  * @param  __FLAG__: specifies the RTC Alarm Flag sources to be enabled or disabled.
  *         This parameter can be:
  *            @arg RTC_FLAG_ALRAF
  *            @arg RTC_FLAG_ALRBF
  *            @arg RTC_FLAG_ALRAWF     
  *            @arg RTC_FLAG_ALRBWF    
  * @retval None
  */
#define __HAL_RTC_ALARM_GET_FLAG(__HANDLE__, __FLAG__)                (((((__HANDLE__)->Instance->ISR) & (__FLAG__)) != RESET)? SET : RESET)

/**
  * @brief  Clear the RTC Alarm's pending flags.
  * @param  __HANDLE__: specifies the RTC handle.
  * @param  __FLAG__: specifies the RTC Alarm Flag sources to be enabled or disabled.
  *          This parameter can be:
  *             @arg RTC_FLAG_ALRAF
  *             @arg RTC_FLAG_ALRBF 
  * @retval None
  */
#define __HAL_RTC_ALARM_CLEAR_FLAG(__HANDLE__, __FLAG__)                  ((__HANDLE__)->Instance->ISR) = (~(((__FLAG__) | RTC_ISR_INIT)& 0x0000FFFF)|((__HANDLE__)->Instance->ISR & RTC_ISR_INIT))
  
  
#define RTC_EXTI_LINE_ALARM_EVENT             ((uint32_t)0x00020000)  /*!< External interrupt line 17 Connected to the RTC Alarm event */
#define RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT  ((uint32_t)0x00200000)  /*!< External interrupt line 21 Connected to the RTC Tamper and Time Stamp events */                                               
#define RTC_EXTI_LINE_WAKEUPTIMER_EVENT       ((uint32_t)0x00400000)  /*!< External interrupt line 22 Connected to the RTC Wakeup event */                                               

/**
  * @brief  Enable the RTC Exti line.
  * @param  __EXTILINE__: specifies the RTC Exti sources to be enabled or disabled.
  *         This parameter can be:
  *            @arg RTC_EXTI_LINE_ALARM_EVENT
  *            @arg RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT
  *            @arg RTC_EXTI_LINE_WAKEUPTIMER_EVENT
  * @retval None
  */
#define __HAL_RTC_EXTI_ENABLE_IT(__EXTILINE__)   (EXTI->IMR |= (__EXTILINE__))

/* alias define maintained for legacy */
#define __HAL_RTC_ENABLE_IT   __HAL_RTC_EXTI_ENABLE_IT

/**
  * @brief  Disable the RTC Exti line.
  * @param  __EXTILINE__: specifies the RTC Exti sources to be enabled or disabled.
  *         This parameter can be:
  *            @arg RTC_EXTI_LINE_ALARM_EVENT
  *            @arg RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT
  *            @arg RTC_EXTI_LINE_WAKEUPTIMER_EVENT
  * @retval None
  */
#define __HAL_RTC_EXTI_DISABLE_IT(__EXTILINE__)  (EXTI->IMR &= ~(__EXTILINE__))

/* alias define maintained for legacy */
#define __HAL_RTC_DISABLE_IT   __HAL_RTC_EXTI_DISABLE_IT

/**
  * @brief  Generates a Software interrupt on selected EXTI line.
  * @param  __EXTILINE__: specifies the RTC Exti sources to be enabled or disabled.
  *         This parameter can be:
  *            @arg RTC_EXTI_LINE_ALARM_EVENT
  *            @arg RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT
  *            @arg RTC_EXTI_LINE_WAKEUPTIMER_EVENT
  * @retval None
  */
#define __HAL_RTC_EXTI_GENERATE_SWIT(__EXTI_LINE__) (EXTI->SWIER |= (__EXTI_LINE__))

/**
  * @brief  Clear the RTC Exti flags.
  * @param  __FLAG__: specifies the RTC Exti sources to be enabled or disabled.
  *         This parameter can be:
  *            @arg RTC_EXTI_LINE_ALARM_EVENT
  *            @arg RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT
  *            @arg RTC_EXTI_LINE_WAKEUPTIMER_EVENT
  * @retval None
  */
#define __HAL_RTC_EXTI_CLEAR_FLAG(__FLAG__)  (EXTI->PR = (__FLAG__))

/* alias define maintained for legacy */
#define __HAL_RTC_CLEAR_FLAG   __HAL_RTC_EXTI_CLEAR_FLAG


/* Include RTC HAL Extension module */
#include "stm32f4xx_hal_rtc_ex.h"

/* Exported functions --------------------------------------------------------*/

/* Initialization and de-initialization functions  ****************************/
HAL_StatusTypeDef HAL_RTC_Init(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTC_DeInit(RTC_HandleTypeDef *hrtc);
void       HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc);
void       HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc);

/* RTC Time and Date functions ************************************************/
HAL_StatusTypeDef HAL_RTC_SetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_GetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_SetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_GetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format);

/* RTC Alarm functions ********************************************************/
HAL_StatusTypeDef HAL_RTC_SetAlarm(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_SetAlarm_IT(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_DeactivateAlarm(RTC_HandleTypeDef *hrtc, uint32_t Alarm);
HAL_StatusTypeDef HAL_RTC_GetAlarm(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Alarm, uint32_t Format);
void                HAL_RTC_AlarmIRQHandler(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef   HAL_RTC_PollForAlarmAEvent(RTC_HandleTypeDef *hrtc, uint32_t Timeout);
void         HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);

/* Peripheral Control functions ***********************************************/
HAL_StatusTypeDef   HAL_RTC_WaitForSynchro(RTC_HandleTypeDef* hrtc);

/* Peripheral State functions *************************************************/
HAL_RTCStateTypeDef HAL_RTC_GetState(RTC_HandleTypeDef *hrtc);

HAL_StatusTypeDef  RTC_EnterInitMode(RTC_HandleTypeDef* hrtc);
uint8_t            RTC_ByteToBcd2(uint8_t Value);
uint8_t            RTC_Bcd2ToByte(uint8_t Value);

/**
  * @}
  */ 

/**
  * @}
  */ 
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_RTC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

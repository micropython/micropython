#include <stdio.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "systick.h"
#include "rtc.h"

static RTC_HandleTypeDef RtcHandle;
static machine_uint_t rtc_info;

#define RTC_INFO_USE_EXISTING (0)
#define RTC_INFO_USE_LSE (1)
#define RTC_INFO_USE_LSI (3)

// Note: LSI is around (32KHz), these dividers should work either way
// ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)
#define RTC_ASYNCH_PREDIV (0x7f)
#define RTC_SYNCH_PREDIV  (0x00ff)

#if 0
void rtc_init(void) {
    // Enable the PWR clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    // Allow access to RTC
    PWR_BackupAccessCmd(ENABLE);

    if (RTC_ReadBackupRegister(RTC_BKP_DR0) == 0x32F2) {
        // RTC still alive, so don't re-init it
        // wait for RTC APB register synchronisation
        RTC_WaitForSynchro();
        rtc_info = RTC_INFO_USE_EXISTING;
        return;
    }

    uint32_t timeout = 10000000;

    // Enable the PWR clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    // Allow access to RTC
    PWR_BackupAccessCmd(ENABLE);

    // Enable the LSE OSC
    RCC_LSEConfig(RCC_LSE_ON);

    // Wait till LSE is ready
    machine_uint_t sys_tick = sys_tick_counter;
    while((RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) && (--timeout > 0)) {
    }

    // record how long it took for the RTC to start up
    rtc_info = (sys_tick_counter - sys_tick) << 2;

    // If LSE timed out, use LSI instead
    if (timeout == 0) {
        // Disable the LSE OSC
        RCC_LSEConfig(RCC_LSE_OFF);

        // Enable the LSI OSC
        RCC_LSICmd(ENABLE);

        // Wait till LSI is ready
        while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) {
        }

        // Use LSI as the RTC Clock Source
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

        // record that we are using the LSI
        rtc_info |= RTC_INFO_USE_LSI;
    } else {
        // Use LSE as the RTC Clock Source
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

        // record that we are using the LSE
        rtc_info |= RTC_INFO_USE_LSE;
    }

    // Note: LSI is around (32KHz), these dividers should work either way
    // ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)
    uint32_t uwSynchPrediv = 0xFF;
    uint32_t uwAsynchPrediv = 0x7F;

    // Enable the RTC Clock
    RCC_RTCCLKCmd(ENABLE);

    // Wait for RTC APB registers synchronisation
    RTC_WaitForSynchro();

    // Configure the RTC data register and RTC prescaler
    RTC_InitTypeDef RTC_InitStructure;
    RTC_InitStructure.RTC_AsynchPrediv = uwAsynchPrediv;
    RTC_InitStructure.RTC_SynchPrediv = uwSynchPrediv;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStructure);

    // Set the date (BCD)
    RTC_DateTypeDef RTC_DateStructure;
    RTC_DateStructure.RTC_Year = 0x13;
    RTC_DateStructure.RTC_Month = RTC_Month_October;
    RTC_DateStructure.RTC_Date = 0x26;
    RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Saturday;
    RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);

    // Set the time (BCD)
    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
    RTC_TimeStructure.RTC_Hours   = 0x01;
    RTC_TimeStructure.RTC_Minutes = 0x53;
    RTC_TimeStructure.RTC_Seconds = 0x00;
    RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);

    // Indicator for the RTC configuration
    RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
}
#endif

static void RTC_CalendarConfig(void);

void rtc_init(void) {
  /*##-1- Configure the RTC peripheral #######################################*/
  RtcHandle.Instance = RTC;

  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follow:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */ 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  
    machine_uint_t tick = HAL_GetTick();

  if(HAL_RTC_Init(&RtcHandle) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler(); 
    return;
  }
  
    // record how long it took for the RTC to start up
    rtc_info = HAL_GetTick() - tick;

  /*##-2- Check if Data stored in BackUp register0: No Need to reconfigure RTC#*/
  /* Read the Back Up Register 0 Data */
  if(HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR0) != 0x32F2)
  {  
    /* Configure RTC Calendar */
    RTC_CalendarConfig();
  }
  else
  {
    /* Check if the Power On Reset flag is set */  
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
    {
      /* Turn on LED2: Power on reset occured */
      //BSP_LED_On(LED2);
    }
    /* Check if Pin Reset flag is set */
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
    {
      /* Turn on LED4: External reset occured */
      //BSP_LED_On(LED4);
    }
    /* Clear source Reset Flag */
    __HAL_RCC_CLEAR_RESET_FLAGS();
  }

}

static void RTC_CalendarConfig(void)
{
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;
    
  /*##-1- Configure the Date #################################################*/
  /* Set Date: Tuesday February 18th 2014 */
  sdatestructure.Year = 0x14;
  sdatestructure.Month = RTC_MONTH_FEBRUARY;
  sdatestructure.Date = 0x18;
  sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;
  
  if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler(); 
    return;
  } 
  
  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:00:00 */
  stimestructure.Hours = 0x02;
  stimestructure.Minutes = 0x00;
  stimestructure.Seconds = 0x00;
  stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
  
  if(HAL_RTC_SetTime(&RtcHandle,&stimestructure,FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler(); 
    return;
  }
  
  /*##-3- Writes a data in a RTC Backup data Register0 #######################*/
  HAL_RTCEx_BKUPWrite(&RtcHandle,RTC_BKP_DR0,0x32F2);  
}

/******************************************************************************/
// Micro Python bindings

mp_obj_t pyb_rtc_info(void) {
    return mp_obj_new_int(rtc_info);
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_rtc_info_obj, pyb_rtc_info);

mp_obj_t pyb_rtc_read(void) {
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
    HAL_RTC_GetTime(&RtcHandle, &time, FORMAT_BIN);
    HAL_RTC_GetDate(&RtcHandle, &date, FORMAT_BIN);
    printf("%02d-%02d-%04d %02d:%02d:%02d\n",date.Date, date.Month, 2000 + date.Year, time.Hours, time.Minutes, time.Seconds);
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_rtc_read_obj, pyb_rtc_read);

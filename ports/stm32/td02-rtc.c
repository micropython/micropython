#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"
#include "extint.h"
#include "td02-rtc.h"
#include "timer.h"
#include "irq.h"

#if defined(MICROPY_PY_TIGER) && MICROPY_HW_ENABLE_RTC

#ifndef RTC_SYNCH_PREDIV
#define RTC_SYNCH_PREDIV 0xFF
#endif

#define OSCILLATOR_START_TIMEOUT 5000 // milliseconds

enum
{
    RTC_INITIAL = 0,        // gleich nach Power-ON
    RTC_INSTALLING,         // 1  ; Installation dauert noch an
    RTC_NO_RTC,             // 2  ; keine RTC-Hardware
    RTC_PRESENT,        	// 3  ; OK, RTC-Hardware anwesend
    RTC_RETRY               // 4  ; Retrying to find RTC after Err
};

enum
{
    RTC_NO_VOLTAGE_DROP = 0,     // 0; There was no voltage drop
    RTC_VOLTAGE_LOW         // 1; Voltage of clock had been gone
};

struct i_uhr_s
{
    uint32_t sek;
    uint32_t ms;
};

struct i_uhr_s i_uhr = 
{
    1,            // sek
    1             // ms
};


RTC_HandleTypeDef RTCHandle;

static bool rtc_need_init_finalise = false;

static uint8_t rtc_opt_status = RTC_INITIAL;
static uint8_t rtc_voltage_status = RTC_VOLTAGE_LOW;

bool real_rtc = false;
bool inst_ok = false;
uint32_t oscillator_start_time_counter;
static volatile uint8_t interrupt = 0xFF;

static bool rtc_running = false;

bool Rtc_EnterInitMode(void)
{
	  /* Check if the Initialization mode is set */
    if((RTCHandle.Instance->ISR & RTC_ISR_INITF) == 0)
    {
        // entering initialization mode
        RTCHandle.Instance->ISR |= RTC_ISR_INIT;
        // Poll INITF bit of in the RTC_ISR register
        while ((RTCHandle.Instance->ISR & RTC_ISR_INITF) == 0)
		{
		}
    }
    return true;
}

static void RTC_CalendarConfig(void) 
{
    // set the date to 1st Jan 2015
	
	
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;
	
    date.Year = 15;
    date.Month = 1;
    date.Date = 1;
    date.WeekDay = RTC_WEEKDAY_THURSDAY;
	
	HAL_RTC_SetDate(&RTCHandle, &date, RTC_FORMAT_BIN);
	
    time.Hours = 0;
    time.Minutes = 0;
    time.Seconds = 0;
    
	time.TimeFormat = RTC_HOURFORMAT12_AM;
	time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	time.StoreOperation = RTC_STOREOPERATION_SET;
	
	HAL_RTC_SetTime(&RTCHandle, &time, RTC_FORMAT_BIN);
}

void ddr_int(void)
{
	if(real_rtc)
    {
        if(rtc_opt_status == RTC_INSTALLING)
        {
            // External low-speed oscillator ready
			
            if(RCC->BDCR & RCC_BDCR_LSERDY)
            {
                rtc_init_finalise();
                rtc_opt_status = RTC_PRESENT;
            }
            else
            {
                if(++oscillator_start_time_counter >= OSCILLATOR_START_TIMEOUT)
                {
                    // We have problems with starting the oscillator.
                    // So switch to not using of the real RTC
                    real_rtc = false;
                    rtc_opt_status = RTC_NO_RTC;
                }
            }
        }
    }
    else
    {
        struct i_uhr_s * p_i_uhr = &i_uhr;
        
        if( --p_i_uhr->ms == 0 )
        {
            p_i_uhr->ms = 1000;
            p_i_uhr->sek++;
        }
    }
    
}

void Set_Clock_and_Prescalers(void)
{    
    // To obtain an internal clock frequency of 1 Hz 
    // the asynchronous prescaler division factor is set to 128,
    // and the synchronous division factor to 256
    RTCHandle.Instance->PRER |= RTC_PRER_PREDIV_A;
    RTCHandle.Instance->PRER |= 255;
}

static void PYB_RTC_MspInit_Kick(void)
{
	// Backup domain software reset
	__HAL_RCC_BACKUPRESET_FORCE(); //RCC->BDCR |= RCC_BDCR_BDRST
	// release reset    
	__HAL_RCC_BACKUPRESET_RELEASE(); //RCC->BDCR &= ~RCC_BDCR_BDRST 
	

	PWR->CSR1 |= PWR_CSR1_BRE; // Backup regulator enabled (used to maintain backup SRAM content in Standby and VBAT modes)

	// LSE oscillator drive capability
	RCC->BDCR |= ((RCC_BDCR_LSEDRV_1 | RCC_BDCR_LSEDRV_0));         // 11: High driving capability
	// External low-speed oscillator enable
	RCC->BDCR |= RCC_BDCR_LSEON;
	
	
	rtc_running = true;
	rtc_need_init_finalise = true;
}

void Init_Alarms(void)
{
    // Alarm A enable
    //p_stm32f7_rtc->rtc_cr.bit.alrae = 1;    // 1: Alarm A enabled
    RTCHandle.Instance->CR &= ~RTC_CR_ALRAE;    // 0: Alarm A disabled
    
    // Alarm A interrupt enable
    RTCHandle.Instance->CR |= RTC_CR_ALRAE;   // 1: Alarm A interrupt enabled
    
    // Output polarity
    //p_stm32f7_rtc->rtc_cr.bit.pol = 1;      // 1: The pin is low when ALRAF/ALRBF/WUTF is asserted
    RTCHandle.Instance->CR &= ~RTC_CR_POL;      // 0: The pin is high when ALRAF/ALRBF/WUTF is asserted
    
    // RTC_ALARM on PC13 output type
    RTCHandle.Instance->OR &= ~RTC_OR_ALARMTYPE;   // open-drain output
    //p_stm32f7_rtc->rtc_or.bit.rtc_alarm_type = 1;   // push-pull output
    
    // Output selection
	RTCHandle.Instance->CR &= ~RTC_CR_OSEL_1;
    RTCHandle.Instance->CR |= RTC_CR_OSEL_0;
    
}

void rtc_init_start(bool force_init)
{	
	RTCHandle.Instance = RTC;
	
    // Power interface clock enable
	__HAL_RCC_PWR_CLK_ENABLE();  //p_stm32f7_rcc->rcc_apb1enr.bit.pwren = 1;
  
    
    // Disable backup domain write protection
	//HAL_PWR_EnableBkUpAccess
	HAL_PWR_EnableBkUpAccess(); //p_stm32f7_pwr->pwr_cr1.bit.dbp = 1; Access to RTC and RTC Backup registers and backup SRAM enabled
	 
	//p_stm32f7_rtc->rtc_isr.bit.inits == 1
    if((RTCHandle.Instance->ISR & RTC_ISR_INITS))
    {
        rtc_voltage_status = RTC_NO_VOLTAGE_DROP;
        return;   // Clock already initialized
    }
	
	//HAL_GPIO_Init(GPIOC, &alarm_init);
	//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,GPIO_PIN_RESET);
	
	mp_hal_pin_open_drain(pyb_pin_RTC_OUT);
	mp_hal_pin_low(pyb_pin_RTC_OUT);
	mp_hal_pin_output(pyb_pin_RTC_OUT);
	

	rtc_need_init_finalise = false;
	
	if (!force_init) 
	{
		
		if(rtc_running)
		{
			// remove Backup Domain write protection
            HAL_PWR_EnableBkUpAccess();
            // Clear source Reset Flag
            __HAL_RCC_CLEAR_RESET_FLAGS();
			
			__HAL_RTC_WRITEPROTECTION_DISABLE(&RTCHandle);
			Rtc_EnterInitMode();
			Set_Clock_and_Prescalers();
			__HAL_RTC_WRITEPROTECTION_ENABLE(&RTCHandle);
			return;
		}	
	}
    PYB_RTC_MspInit_Kick();	
	oscillator_start_time_counter = 0;
}

void rtc_init_finalise(void)
{
	 // RTC clock source selection
    if (!rtc_need_init_finalise) 
	{
        return;
    }
 
	//RCC->BDCR &= ~RCC_BDCR_RTCSEL_1;   
    RCC->BDCR |= RCC_BDCR_RTCSEL_0;        // 01: LSE oscillator clock used as the RTC clock
    
    mp_hal_pin_input(pyb_pin_RTC_OUT);
	
	//ENTERING PYB_RTC_Init first time
    
    // RTC clock enable
	RCC->BDCR |= RCC_BDCR_RTCEN; //PYB_RTC_MspInit_Finalise
	
	//while (PYB_RTC_Init(&RTCHandle) != HAL_OK) 
	//rtc_startup_tick = HAL_GetTick(); //
//	HAL_PWR_EnableBkUpAccess();
//	PYB_RTC_MspInit_Kick(); //PYB_RTC_MspInit_Kick

	//ENTERING PYB_RTC_Init second time

    // enable RTC registers write access
    __HAL_RTC_WRITEPROTECTION_DISABLE(&RTCHandle);

	mp_printf(&mp_plat_print, "Enter Init Mode\n");
	//(RTC_EnterInitMode(hrtc) 
    Rtc_EnterInitMode();
    mp_printf(&mp_plat_print, "Leave Init Mode\n");
	//else
	
    // Bypass the shadow registers
	RTCHandle.Instance->CR |= RTC_CR_BYPSHAD;
	Set_Clock_and_Prescalers();
    Init_Alarms();
    
   
    // Exit the initialization mode by clearing the INIT bit
	RTCHandle.Instance->ISR &= ~RTC_ISR_INIT;
    
    __HAL_RTC_WRITEPROTECTION_ENABLE(&RTCHandle);
	
	//END PYB_RTC_Init
	RTC_CalendarConfig();
    __HAL_RCC_CLEAR_RESET_FLAGS();
    rtc_need_init_finalise = false;
}

mp_obj_t rtc_set_alarm(mp_obj_t self_in, mp_obj_t time ) 
{
	    // set date and time
		if (mp_obj_is_type(time, &mp_type_list) ) 
		{
			mp_obj_t *items;
			mp_obj_get_array_fixed_n(time, 3, &items);

			uint8_t hours, minutes, days;
			
			hours = mp_obj_get_int(items[0]);
			if(hours > 23) hours = 23;
			minutes = mp_obj_get_int(items[1]);
			if(minutes > 59) minutes = 59;
			days = mp_obj_get_int(items[2]);
			if(days > 31) days = 31;
			
			
			__HAL_RTC_WRITEPROTECTION_DISABLE(&RTCHandle);
			
			Rtc_EnterInitMode();
			
			RTCHandle.Instance->CR &= ~RTC_CR_ALRAE; 
			RTCHandle.Instance->ALRMAR |= RTC_ALRMAR_MSK1;
			RTCHandle.Instance->ALRMAR &= ~( RTC_ALRMAR_MSK2 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK4 );
			RTCHandle.Instance->ALRMASSR &= ~( RTC_ALRMASSR_MASKSS );
			
			RTCHandle.Instance->ALRMAR &= ~( RTC_ALRMAR_PM );
			
			RTCHandle.Instance->ALRMAR |= (((days) / 10)  << RTC_ALRMAR_DT_Pos);
			RTCHandle.Instance->ALRMAR |= (((days) % 10)  << RTC_ALRMAR_DU_Pos);
			
			RTCHandle.Instance->ALRMAR |= (((hours) / 10)  << RTC_ALRMAR_HT_Pos);
			RTCHandle.Instance->ALRMAR |= (((hours) % 10)  << RTC_ALRMAR_HU_Pos);
			
			RTCHandle.Instance->ALRMAR |= (((minutes) / 10)  << RTC_ALRMAR_MNT_Pos);
			RTCHandle.Instance->ALRMAR |= (((minutes) % 10)  << RTC_ALRMAR_MNU_Pos);
			
			RTCHandle.Instance->CR |= RTC_CR_ALRAE; 
			
			RTCHandle.Instance->CR |= RTC_CR_POL;
			
			RTCHandle.Instance->ISR &= ~RTC_ISR_INIT;
			
			__HAL_RTC_WRITEPROTECTION_ENABLE(&RTCHandle);
			
		}
		else
		{
			mp_raise_TypeError(MP_ERROR_TEXT("Type must be a list"));
		}

        return mp_const_none;
    
}
static MP_DEFINE_CONST_FUN_OBJ_2(rtc_set_alarm_obj, rtc_set_alarm);

mp_obj_t pyb_rtc_wakeup(size_t n_args, const mp_obj_t *args) 
{
    // wut is wakeup counter start value, wucksel is clock source
    // counter is decremented at wucksel rate, and wakes the MCU when it gets to 0
    // wucksel=0b000 is RTC/16 (RTC runs at 32768Hz)
    // wucksel=0b001 is RTC/8
    // wucksel=0b010 is RTC/4
    // wucksel=0b011 is RTC/2
    // wucksel=0b100 is 1Hz clock
    // wucksel=0b110 is 1Hz clock with 0x10000 added to wut
    // so a 1 second wakeup could be wut=2047, wucksel=0b000, or wut=4095, wucksel=0b001, etc

    rtc_init_finalise();

    // disable wakeup IRQ while we configure it
    HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);

    bool enable = false;
    mp_int_t wucksel;
    mp_int_t wut;
    mp_obj_t callback = mp_const_none;
    if (n_args <= 3) 
	{
        if (args[1] == mp_const_none)
		{
            // disable wakeup
        } 
		else 
		{
            // time given in ms
            mp_int_t ms = mp_obj_get_int(args[1]);
            mp_int_t div = 2;
            wucksel = 3;
            while (div <= 16 && ms > 2000 * div) 
			{
                div *= 2;
                wucksel -= 1;
            }
            if (div <= 16) 
			{             
                wut = 32768 / div * ms / 1000;
            } 
			else 
			{
                // use 1Hz clock
                wucksel = 4;
                wut = ms / 1000;
                if (wut > 0x10000) 
				{
                    // wut too large for 16-bit register, try to offset by 0x10000
                    wucksel = 6;
                    wut -= 0x10000;
                    if (wut > 0x10000) 
					{
                        // wut still too large
                        mp_raise_ValueError(MP_ERROR_TEXT("wakeup value too large"));
                    }
                }
            }
            // wut register should be 1 less than desired value, but guard against wut=0
            if (wut > 0) 
			{
                wut -= 1;
            }
            enable = true;
        }
        if (n_args == 3) 
		{
            callback = args[2];
        }
    } 
	else 
	{
        // config values given directly
        wucksel = mp_obj_get_int(args[1]);
        wut = mp_obj_get_int(args[2]);
        callback = args[3];
        enable = true;
    }

    // set the callback
    MP_STATE_PORT(pyb_extint_callback)[EXTI_RTC_WAKEUP] = callback;

    // disable register write protection
    RTC->WPR = 0xca;
    RTC->WPR = 0x53;

    // clear WUTE
    RTC->CR &= ~RTC_CR_WUTE;

    // wait until WUTWF is set
    while (!LL_RTC_IsActiveFlag_WUTW(RTC)) {}

    if (enable) 
	{
        // program WUT
        RTC->WUTR = wut;

        // set WUTIE to enable wakeup interrupts
        // set WUTE to enable wakeup
        // program WUCKSEL
        RTC->CR = (RTC->CR & ~7) | (1 << 14) | (1 << 10) | (wucksel & 7);

        // enable register write protection
        RTC->WPR = 0xff;

        // enable external interrupts on line EXTI_RTC_WAKEUP
        
       
        
        EXTI->IMR |= 1 << EXTI_RTC_WAKEUP;
        EXTI->RTSR |= 1 << EXTI_RTC_WAKEUP;
       

        // clear interrupt flags
        RTC->ISR &= ~RTC_ISR_WUTF;
       

        EXTI->PR = 1 << EXTI_RTC_WAKEUP;

        NVIC_SetPriority(RTC_WKUP_IRQn, IRQ_PRI_RTC_WKUP);
        HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
    } 
	else 
	{
        // clear WUTIE to disable interrupts
        RTC->CR &= ~RTC_CR_WUTIE;

        // enable register write protection
        RTC->WPR = 0xff;

        // disable external interrupts on line EXTI_RTC_WAKEUP
        EXTI->IMR &= ~(1 << EXTI_RTC_WAKEUP);
       
    }

    return mp_const_none;
}

uint64_t mp_hal_time_ns(void)
{
    uint64_t ns = 0;
    #if MICROPY_HW_ENABLE_RTC
    // Get current according to the RTC.
    rtc_init_finalise();
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
    HAL_RTC_GetTime(&RTCHandle, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&RTCHandle, &date, RTC_FORMAT_BIN);
    ns = timeutils_seconds_since_epoch(2000 + date.Year, date.Month, date.Date, time.Hours, time.Minutes, time.Seconds);
    ns *= 1000000000ULL;
    uint32_t usec = ((RTC_SYNCH_PREDIV - time.SubSeconds) * (1000000 / 64)) / ((RTC_SYNCH_PREDIV + 1) / 64);
    ns += usec * 1000;
    #endif
    return ns;
}

/******************************************************************************/
// MicroPython bindings

typedef struct _tiger_rtc_obj_t 
{
    mp_obj_base_t base;
} tiger_rtc_obj_t;

static const tiger_rtc_obj_t tiger_rtc_obj = {{&tiger_rtc_type}};

/// \classmethod \constructor()
/// Create an RTC object.
static mp_obj_t tiger_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
	
	real_rtc = args[0];
	
	rtc_init_start(true);
    rtc_init_finalise();
	
	if(inst_ok)
	{
		inst_ok = false;
		del1mInt(interrupt);
	}

	if(!inst_ok)
	{
		interrupt = add1msInt(ddr_int);
	}
	
	
	oscillator_start_time_counter = 0;
	
	inst_ok = true;
    // return constant object
    return MP_OBJ_FROM_PTR(&tiger_rtc_obj);
}

// force rtc to re-initialise
mp_obj_t tiger_rtc_reset(mp_obj_t self_in) 
{
    rtc_init_start(true);
    rtc_init_finalise();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(tiger_rtc_reset_obj, tiger_rtc_reset);


#define MEG_DIV_64 (1000000 / 64)
#define MEG_DIV_SCALE ((RTC_SYNCH_PREDIV + 1) / 64)

#if defined(MICROPY_HW_RTC_USE_US) && MICROPY_HW_RTC_USE_US
uint32_t rtc_subsec_to_us(uint32_t ss) 
{
    return ((RTC_SYNCH_PREDIV - ss) * MEG_DIV_64) / MEG_DIV_SCALE;
}

uint32_t rtc_us_to_subsec(uint32_t us) 
{
    return RTC_SYNCH_PREDIV - (us * MEG_DIV_SCALE / MEG_DIV_64);
}
#else
#define rtc_us_to_subsec
#define rtc_subsec_to_us
#endif



/* \method datetime([datetimetuple])
/// Get or set the date and time of the RTC.
///
/// With no arguments, this method returns an 8-tuple with the current
/// date and time.  With 1 argument (being an 8-tuple) it sets the date
/// and time.
///
/// The 8-tuple has the following format:
///
///     (year, month, day, weekday, hours, minutes, seconds, subseconds)
///
/// `weekday` is 1-7 for Monday through Sunday.
///
 *`subseconds` counts down from 255 to 0 */

mp_obj_t tiger_rtc_datetime(size_t n_args, const mp_obj_t *args) 
{
    rtc_init_finalise();
    if (n_args == 1) 
	{
        // get date and time
        // note: need to call get time then get date to correctly access the registers
		RTC_DateTypeDef date;
		RTC_TimeTypeDef time;
		
        HAL_RTC_GetTime(&RTCHandle, &time, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&RTCHandle, &date, RTC_FORMAT_BIN);
		
        mp_obj_t tuple[7] = 
		{
			mp_obj_new_int(time.Hours),
			mp_obj_new_int(time.Minutes),
			mp_obj_new_int(time.Seconds),
			mp_obj_new_int(date.Date),
            mp_obj_new_int(date.Month),
            mp_obj_new_int(date.Year+2000),
            mp_obj_new_int(date.WeekDay),
        };
        return mp_obj_new_tuple(7, tuple);
    }
	else 
	{
        // set date and time
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[1], 7, &items);
		
		RTC_DateTypeDef date;
		RTC_TimeTypeDef time;

		time.Hours = mp_obj_get_int(items[0]);
		if (time.Hours > 23) time.Hours = 23;     // hours
	//	mp_printf(&mp_plat_print, "hours %d\n", time.Hours);
		
		time.Minutes = mp_obj_get_int(items[1]);
		if (time.Minutes > 59) time.Minutes = 59;   // minutes
	//	mp_printf(&mp_plat_print, "minutes %d\n", time.Minutes);
		
		time.Seconds = mp_obj_get_int(items[2]);
		if (time.Seconds > 59) time.Seconds = 59;   // seconds
	//	mp_printf(&mp_plat_print, "seconds %d\n", time.Seconds);
		
		date.Date = mp_obj_get_int(items[3]);
		if (date.Date > 31) date.Date = 31;      // days 
		if (date.Date < 1) date.Date = 1;      // days 
	//	mp_printf(&mp_plat_print, "days %d\n", date.Date);
		
		date.Month = mp_obj_get_int(items[4]);
		if (date.Month > 12) date.Month = 12;    // months
		if (date.Month < 1) date.Month = 1;
	//	mp_printf(&mp_plat_print, "months %d\n", date.Month);
		
        date.Year = mp_obj_get_int(items[5]);
		if (date.Year < 2000) date.Year = 0;
		else date.Year -= 2000;
	//	mp_printf(&mp_plat_print, "years %d\n", date.Year);
        
        date.WeekDay = mp_obj_get_int(items[6]);
		if ( date.WeekDay > 7 ) date.WeekDay = 7;   // day of the week
		if ( date.WeekDay < 1 ) date.WeekDay = 1;
	//	mp_printf(&mp_plat_print, "Week Day %d\n", date.WeekDay);
		
		time.TimeFormat = RTC_HOURFORMAT12_AM;
		time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		time.StoreOperation = RTC_STOREOPERATION_SET;
		
		HAL_RTC_SetDate(&RTCHandle, &date, RTC_FORMAT_BIN);
		HAL_RTC_SetTime(&RTCHandle, &time, RTC_FORMAT_BIN);
		
        return mp_const_none;
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(tiger_rtc_datetime_obj, 1, 2, tiger_rtc_datetime);

/* calibration(None)
// calibration(cal)
// When an integer argument is provided, check that it falls in the range [-511 to 512]
// and set the calibration value; otherwise return calibration value */

static const mp_rom_map_elem_t tiger_rtc_locals_dict_table[] = 
{
   
    { MP_ROM_QSTR(MP_QSTR_datetime), MP_ROM_PTR(&tiger_rtc_datetime_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setAlarm), MP_ROM_PTR(&rtc_set_alarm_obj) },
	{ MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&tiger_rtc_reset_obj) },
	
	
};
static MP_DEFINE_CONST_DICT(tiger_rtc_locals_dict, tiger_rtc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE
(   
    tiger_rtc_type,
    MP_QSTR_RTC1,
    MP_TYPE_FLAG_NONE,
    make_new, tiger_rtc_make_new,
    locals_dict, &tiger_rtc_locals_dict
);


#endif
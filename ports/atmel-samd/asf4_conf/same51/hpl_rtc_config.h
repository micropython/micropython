/* Auto-generated config file hpl_rtc_config.h */
#ifndef HPL_RTC_CONFIG_H
#define HPL_RTC_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Basic settings

#ifndef CONF_RTC_ENABLE
#define CONF_RTC_ENABLE 1
#endif

// <q> Force reset RTC on initialization
// <i> Force RTC to reset on initialization.
// <i> Note that the previous power down data in RTC is lost if it's enabled.
// <id> rtc_arch_init_reset
#ifndef CONF_RTC_INIT_RESET
#define CONF_RTC_INIT_RESET 0
#endif

// <o> Prescaler configuration
// <0x0=>OFF(Peripheral clock divided by 1)
// <0x1=>Peripheral clock divided by 1
// <0x2=>Peripheral clock divided by 2
// <0x3=>Peripheral clock divided by 4
// <0x4=>Peripheral clock divided by 8
// <0x5=>Peripheral clock divided by 16
// <0x6=>Peripheral clock divided by 32
// <0x7=>Peripheral clock divided by 64
// <0x8=>Peripheral clock divided by 128
// <0x9=>Peripheral clock divided by 256
// <0xA=>Peripheral clock divided by 512
// <0xB=>Peripheral clock divided by 1024
// <i> These bits define the RTC clock relative to the peripheral clock
// <id> rtc_arch_prescaler
#ifndef CONF_RTC_PRESCALER
#define CONF_RTC_PRESCALER 0xb

#endif

// <o> Compare Value <1-4294967295>
// <i> These bits define the RTC Compare value, the ticks period is equal to reciprocal of (rtc clock/prescaler/compare value),
// <i> by default 1K clock input, 1 prescaler, 1 compare value, the ticks period equals to 1ms.
// <id> rtc_arch_comp_val

#ifndef CONF_RTC_COMP_VAL
#define CONF_RTC_COMP_VAL 1

#endif

// <e> Event control
// <id> rtc_event_control
#ifndef CONF_RTC_EVENT_CONTROL_ENABLE
#define CONF_RTC_EVENT_CONTROL_ENABLE 0
#endif

// <q> Periodic Interval 0 Event Output
// <i> This bit indicates whether Periodic interval 0 event is enabled and will be generated
// <id> rtc_pereo0
#ifndef CONF_RTC_PEREO0
#define CONF_RTC_PEREO0 0
#endif
// <q> Periodic Interval 1 Event Output
// <i> This bit indicates whether Periodic interval 1 event is enabled and will be generated
// <id> rtc_pereo1
#ifndef CONF_RTC_PEREO1
#define CONF_RTC_PEREO1 0
#endif
// <q> Periodic Interval 2 Event Output
// <i> This bit indicates whether Periodic interval 2 event is enabled and will be generated
// <id> rtc_pereo2
#ifndef CONF_RTC_PEREO2
#define CONF_RTC_PEREO2 0
#endif
// <q> Periodic Interval 3 Event Output
// <i> This bit indicates whether Periodic interval 3 event is enabled and will be generated
// <id> rtc_pereo3
#ifndef CONF_RTC_PEREO3
#define CONF_RTC_PEREO3 0
#endif
// <q> Periodic Interval 4 Event Output
// <i> This bit indicates whether Periodic interval 4 event is enabled and will be generated
// <id> rtc_pereo4
#ifndef CONF_RTC_PEREO4
#define CONF_RTC_PEREO4 0
#endif
// <q> Periodic Interval 5 Event Output
// <i> This bit indicates whether Periodic interval 5 event is enabled and will be generated
// <id> rtc_pereo5
#ifndef CONF_RTC_PEREO5
#define CONF_RTC_PEREO5 0
#endif
// <q> Periodic Interval 6 Event Output
// <i> This bit indicates whether Periodic interval 6 event is enabled and will be generated
// <id> rtc_pereo6
#ifndef CONF_RTC_PEREO6
#define CONF_RTC_PEREO6 0
#endif
// <q> Periodic Interval 7 Event Output
// <i> This bit indicates whether Periodic interval 7 event is enabled and will be generated
// <id> rtc_pereo7
#ifndef CONF_RTC_PEREO7
#define CONF_RTC_PEREO7 0
#endif

// <q> Compare 0 Event Output
// <i> This bit indicates whether Compare O event is enabled and will be generated
// <id> rtc_cmpeo0
#ifndef CONF_RTC_COMPE0
#define CONF_RTC_COMPE0 0
#endif

// <q> Compare 1 Event Output
// <i> This bit indicates whether Compare 1 event is enabled and will be generated
// <id> rtc_cmpeo1
#ifndef CONF_RTC_COMPE1
#define CONF_RTC_COMPE1 0
#endif
// <q> Overflow Event Output
// <i> This bit indicates whether Overflow event is enabled and will be generated
// <id> rtc_ovfeo
#ifndef CONF_RTC_OVFEO
#define CONF_RTC_OVFEO 0
#endif

// <q> Tamper Event Output
// <i> This bit indicates whether Tamper event output is enabled and will be generated
// <id> rtc_tampereo
#ifndef CONF_RTC_TAMPEREO
#define CONF_RTC_TAMPEREO 0
#endif

// <q> Tamper Event Input
// <i> This bit indicates whether Tamper event input is enabled and will be generated
// <id> rtc_tampevei
#ifndef CONF_RTC_TAMPEVEI
#define CONF_RTC_TAMPEVEI 0
#endif
// </e>

// </h>

// <<< end of configuration section >>>

#endif // HPL_RTC_CONFIG_H

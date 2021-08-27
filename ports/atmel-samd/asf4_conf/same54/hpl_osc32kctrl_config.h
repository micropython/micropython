/* Auto-generated config file hpl_osc32kctrl_config.h */
#ifndef HPL_OSC32KCTRL_CONFIG_H
#define HPL_OSC32KCTRL_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <e> RTC Source configuration
// <id> enable_rtc_source
#ifndef CONF_RTCCTRL_CONFIG
#define CONF_RTCCTRL_CONFIG 0
#endif

// <h> RTC source control
// <y> RTC Clock Source Selection
// <GCLK_GENCTRL_SRC_OSCULP32K"> 32kHz Ultra Low Power Internal Oscillator (OSCULP32K)
// <GCLK_GENCTRL_SRC_XOSC32K"> 32kHz External Crystal Oscillator (XOSC32K)
// <i> This defines the clock source for RTC
// <id> rtc_source_oscillator
#ifndef CONF_RTCCTRL_SRC
#define CONF_RTCCTRL_SRC GCLK_GENCTRL_SRC_OSCULP32K
#endif

// <q> Use 1 kHz output
// <id> rtc_1khz_selection
#ifndef CONF_RTCCTRL_1KHZ
#define CONF_RTCCTRL_1KHZ 1
#endif

#if CONF_RTCCTRL_SRC == GCLK_GENCTRL_SRC_OSCULP32K
#define CONF_RTCCTRL (CONF_RTCCTRL_1KHZ ? OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K_Val : OSC32KCTRL_RTCCTRL_RTCSEL_ULP32K_Val)
#elif CONF_RTCCTRL_SRC == GCLK_GENCTRL_SRC_XOSC32K
#define CONF_RTCCTRL (CONF_RTCCTRL_1KHZ ? OSC32KCTRL_RTCCTRL_RTCSEL_XOSC1K_Val : OSC32KCTRL_RTCCTRL_RTCSEL_XOSC32K_Val)
#else
#error unexpected CONF_RTCCTRL_SRC
#endif

// </h>
// </e>

// <e> 32kHz External Crystal Oscillator Configuration
// <i> Indicates whether configuration for External 32K Osc is enabled or not
// <id> enable_xosc32k
#ifndef CONF_XOSC32K_CONFIG
#define CONF_XOSC32K_CONFIG 1
#endif

// <h> 32kHz External Crystal Oscillator Control
// <q> Oscillator enable
// <i> Indicates whether 32kHz External Crystal Oscillator is enabled or not
// <id> xosc32k_arch_enable
#ifndef CONF_XOSC32K_ENABLE
#define CONF_XOSC32K_ENABLE 1
#endif

// <o> Start-Up Time
// <0x0=>62592us
// <0x1=>125092us
// <0x2=>500092us
// <0x3=>1000092us
// <0x4=>2000092us
// <0x5=>4000092us
// <0x6=>8000092us
// <id> xosc32k_arch_startup
#ifndef CONF_XOSC32K_STARTUP
#define CONF_XOSC32K_STARTUP 0x0
#endif

// <q> On Demand Control
// <i> Indicates whether On Demand Control is enabled or not
// <id> xosc32k_arch_ondemand
#ifndef CONF_XOSC32K_ONDEMAND
#define CONF_XOSC32K_ONDEMAND 1
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> xosc32k_arch_runstdby
#ifndef CONF_XOSC32K_RUNSTDBY
#define CONF_XOSC32K_RUNSTDBY 0
#endif

// <q> 1kHz Output Enable
// <i> Indicates whether 1kHz Output is enabled or not
// <id> xosc32k_arch_en1k
#ifndef CONF_XOSC32K_EN1K
#define CONF_XOSC32K_EN1K 0
#endif

// <q> 32kHz Output Enable
// <i> Indicates whether 32kHz Output is enabled or not
// <id> xosc32k_arch_en32k
#ifndef CONF_XOSC32K_EN32K
#define CONF_XOSC32K_EN32K 0
#endif

// <q> Clock Switch Back
// <i> Indicates whether Clock Switch Back is enabled or not
// <id> xosc32k_arch_swben
#ifndef CONF_XOSC32K_SWBEN
#define CONF_XOSC32K_SWBEN 0
#endif

// <q> Clock Failure Detector
// <i> Indicates whether Clock Failure Detector is enabled or not
// <id> xosc32k_arch_cfden
#ifndef CONF_XOSC32K_CFDEN
#define CONF_XOSC32K_CFDEN 0
#endif

// <q> Clock Failure Detector Event Out
// <i> Indicates whether Clock Failure Detector Event Out is enabled or not
// <id> xosc32k_arch_cfdeo
#ifndef CONF_XOSC32K_CFDEO
#define CONF_XOSC32K_CFDEO 0
#endif

// <q> Crystal connected to XIN32/XOUT32 Enable
// <i> Indicates whether the connections between the I/O pads and the external clock or crystal oscillator is enabled or not
// <id> xosc32k_arch_xtalen
#ifndef CONF_XOSC32K_XTALEN
#define CONF_XOSC32K_XTALEN 0
#endif

// <o> Control Gain Mode
// <0x0=>Low Power mode
// <0x1=>Standard mode
// <0x2=>High Speed mode
// <id> xosc32k_arch_cgm
#ifndef CONF_XOSC32K_CGM
#define CONF_XOSC32K_CGM 0x1
#endif

// </h>
// </e>

// <e> 32kHz Ultra Low Power Internal Oscillator Configuration
// <i> Indicates whether configuration for OSCULP32K is enabled or not
// <id> enable_osculp32k
#ifndef CONF_OSCULP32K_CONFIG
#define CONF_OSCULP32K_CONFIG 1
#endif

// <h> 32kHz Ultra Low Power Internal Oscillator Control

// <q> Oscillator Calibration Control
// <i> Indicates whether Oscillator Calibration is enabled or not
// <id> osculp32k_calib_enable
#ifndef CONF_OSCULP32K_CALIB_ENABLE
#define CONF_OSCULP32K_CALIB_ENABLE 0
#endif

// <o> Oscillator Calibration <0x0-0x3F>
// <id> osculp32k_calib
#ifndef CONF_OSCULP32K_CALIB
#define CONF_OSCULP32K_CALIB 0x0
#endif

// </h>
// </e>

// <<< end of configuration section >>>

#endif // HPL_OSC32KCTRL_CONFIG_H

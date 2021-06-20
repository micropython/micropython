/* Auto-generated config file hpl_sysctrl_config.h */
#ifndef HPL_SYSCTRL_CONFIG_H
#define HPL_SYSCTRL_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

#define CONF_DFLL_OPEN_LOOP_MODE 0
#define CONF_DFLL_CLOSED_LOOP_MODE 1

#define CONF_XOSC_STARTUP_TIME_31MCS 0
#define CONF_XOSC_STARTUP_TIME_61MCS 1
#define CONF_XOSC_STARTUP_TIME_122MCS 2
#define CONF_XOSC_STARTUP_TIME_244MCS 3
#define CONF_XOSC_STARTUP_TIME_488MCS 4
#define CONF_XOSC_STARTUP_TIME_977MCS 5
#define CONF_XOSC_STARTUP_TIME_1953MCS 6
#define CONF_XOSC_STARTUP_TIME_3906MCS 7
#define CONF_XOSC_STARTUP_TIME_7813MCS 8
#define CONF_XOSC_STARTUP_TIME_15625MCS 9
#define CONF_XOSC_STARTUP_TIME_31250MCS 10
#define CONF_XOSC_STARTUP_TIME_62500MCS 11
#define CONF_XOSC_STARTUP_TIME_125000MCS 12
#define CONF_XOSC_STARTUP_TIME_250000MCS 13
#define CONF_XOSC_STARTUP_TIME_500000MCS 14
#define CONF_XOSC_STARTUP_TIME_1000000MCS 15

#define CONF_OSC_STARTUP_TIME_92MCS 0
#define CONF_OSC_STARTUP_TIME_122MCS 1
#define CONF_OSC_STARTUP_TIME_183MCS 2
#define CONF_OSC_STARTUP_TIME_305MCS 3
#define CONF_OSC_STARTUP_TIME_549MCS 4
#define CONF_OSC_STARTUP_TIME_1038MCS 5
#define CONF_OSC_STARTUP_TIME_2014MCS 6
#define CONF_OSC_STARTUP_TIME_3967MCS 7

#define CONF_XOSC32K_STARTUP_TIME_122MCS 0
#define CONF_XOSC32K_STARTUP_TIME_1068MCS 1
#define CONF_XOSC32K_STARTUP_TIME_65592MCS 2
#define CONF_XOSC32K_STARTUP_TIME_125092MCS 3
#define CONF_XOSC32K_STARTUP_TIME_500092MCS 4
#define CONF_XOSC32K_STARTUP_TIME_1000092MCS 5
#define CONF_XOSC32K_STARTUP_TIME_2000092MCS 6
#define CONF_XOSC32K_STARTUP_TIME_4000092MCS 7

// <e> 8MHz Internal Oscillator Configuration
// <i> Indicates whether configuration for OSC8M is enabled or not
// <id> enable_osc8m
#ifndef CONF_OSC8M_CONFIG
#define CONF_OSC8M_CONFIG 1
#endif

// <h> 8MHz Internal Oscillator (OSC8M) Control
// <q> Internal 8M Oscillator Enable
// <i> Indicates whether Internal 8 Mhz Oscillator is enabled or not
// <id> osc8m_arch_enable
#ifndef CONF_OSC8M_ENABLE
#define CONF_OSC8M_ENABLE 1
#endif

// <q> On Demand Control
// <i> Indicates whether On Demand Control is enabled or not.
// <i> If enabled, the oscillator will only be running when requested by a peripheral.
// <i> If disabled, the oscillator will always be running when enabled.
// <id> osc8m_arch_ondemand
#ifndef CONF_OSC8M_ONDEMAND
#define CONF_OSC8M_ONDEMAND 1
#endif

// <q> Run In Standby
// <i> Run In standby Mode
// <i> If this bit is 0: The oscillator is disabled in standby sleep mode.
// <i> If this bit is 1: The oscillator is not stopped in standby sleep mode.
// <id> osc8m_arch_runstdby
#ifndef CONF_OSC8M_RUNSTDBY
#define CONF_OSC8M_RUNSTDBY 0
#endif

// <y> Prescaler
// <SYSCTRL_OSC8M_PRESC_0_Val"> 1
// <SYSCTRL_OSC8M_PRESC_1_Val"> 2
// <SYSCTRL_OSC8M_PRESC_2_Val"> 4
// <SYSCTRL_OSC8M_PRESC_3_Val"> 8
// <i> Prescaler for Internal 8Mhz OSC
// <i> Default: No Prescaling
// <id> osc8m_presc
#ifndef CONF_OSC8M_PRESC
#define CONF_OSC8M_PRESC SYSCTRL_OSC8M_PRESC_3_Val
#endif

// <q> Overwrite Default Osc Calibration
//   <i> Overwrite Default Osc Calibration
// <id> osc8m_arch_overwrite_calibration
#ifndef CONF_OSC8M_OVERWRITE_CALIBRATION
#define CONF_OSC8M_OVERWRITE_CALIBRATION 0
#endif

//  <o>Osc Calibration Value <0-65535>
//      <i> Set the Oscillator Calibration Value
//  <i> Default: 1
// <id> osc8m_arch_calib
#ifndef CONF_OSC8M_CALIB
#define CONF_OSC8M_CALIB 0
#endif

// </h>
// </e>

// <e> 32kHz Internal Oscillator Configuration
// <i> Indicates whether configuration for OSC32K is enabled or not
// <id> enable_osc32k
#ifndef CONF_OSC32K_CONFIG
#define CONF_OSC32K_CONFIG 1
#endif

// <h> 32kHz Internal Oscillator (OSC32K) Control
// <q> Internal 32K Oscillator Enable
// <i> Indicates whether Internal 32K Oscillator is enabled or not
// <id> osc32k_arch_enable
#ifndef CONF_OSC32K_ENABLE
#define CONF_OSC32K_ENABLE 1
#endif

// <q> On Demand Control
// <i> Enable On Demand
// <i> If this bit is 0: The oscillator is always on, if enabled.
// <i> If this bit is 1, the oscillator will only be running when requested by a peripheral.
// <id> osc32k_arch_ondemand
#ifndef CONF_OSC32K_ONDEMAND
#define CONF_OSC32K_ONDEMAND 1
#endif

// <q> Run In Standby
// <i> Run In standby Mode
// <i> If this bit is 0: The oscillator is disabled in standby sleep mode.
// <i> If this bit is 1: The oscillator is not stopped in standby sleep mode.
// <id> osc32k_arch_runstdby
#ifndef CONF_OSC32K_RUNSTDBY
#define CONF_OSC32K_RUNSTDBY 0
#endif

// <q> Enable 32Khz Output
// <i> Enable 32 Khz Output
// <id> osc32k_arch_en32k
#ifndef CONF_OSC32K_EN32K
#define CONF_OSC32K_EN32K 1
#endif

// <q> Enable 1K
// <i> Enable 1K
// <id> osc32k_arch_en1k
#ifndef CONF_OSC32K_EN1K
#define CONF_OSC32K_EN1K 0
#endif

// <q> Write Lock
// <i> Write Lock
// <id> osc32k_arch_wrtlock
#ifndef CONF_OSC32K_WRTLOCK
#define CONF_OSC32K_WRTLOCK 0
#endif

// <y> Start up time for the 32K Oscillator
// <CONF_OSC_STARTUP_TIME_92MCS"> 3 Clock Cycles (92us)
// <CONF_OSC_STARTUP_TIME_122MCS"> 4 Clock Cycles (122us)
// <CONF_OSC_STARTUP_TIME_183MCS"> 6 Clock Cycles (183us)
// <CONF_OSC_STARTUP_TIME_305MCS"> 10 Clock Cycles (305us)
// <CONF_OSC_STARTUP_TIME_549MCS"> 18 Clock Cycles (549us)
// <CONF_OSC_STARTUP_TIME_1038MCS"> 34 Clock Cycles (1038us)
// <CONF_OSC_STARTUP_TIME_2014MCS"> 66 Clock Cycles (2014us)
// <CONF_OSC_STARTUP_TIME_3967MCS"> 130 Clock Cycles (3967us)
// <i> Start Up Time for the 32K Oscillator
// <i> Default: 10 Clock Cycles (305us)
// <id> osc32k_arch_startup
#ifndef CONF_OSC32K_STARTUP
#define CONF_OSC32K_STARTUP CONF_OSC_STARTUP_TIME_92MCS
#endif

// <q> Overwrite Default Osc Calibration
//   <i> Overwrite Default Osc Calibration
// <id> osc32k_arch_overwrite_calibration
#ifndef CONF_OSC32K_OVERWRITE_CALIBRATION
#define CONF_OSC32K_OVERWRITE_CALIBRATION 0
#endif

//  <o>Osc Calibration Value <0-65535>
//      <i> Set the Oscillator Calibration Value
//  <i> Default: 0
// <id> osc32k_arch_calib
#ifndef CONF_OSC32K_CALIB
#define CONF_OSC32K_CALIB 0
#endif

// </h>
// </e>

// <e> 32kHz External Crystal Oscillator Configuration
// <i> Indicates whether configuration for External 32K Osc is enabled or not
// <id> enable_xosc32k
#ifndef CONF_XOSC32K_CONFIG
#define CONF_XOSC32K_CONFIG 0
#endif

// <h> 32kHz External Crystal Oscillator (XOSC32K) Control
// <q> External 32K Oscillator Enable
// <i> Indicates whether External 32K Oscillator is enabled or not
// <id> xosc32k_arch_enable
#ifndef CONF_XOSC32K_ENABLE
#define CONF_XOSC32K_ENABLE 0
#endif

// <q> On Demand
// <i> Enable On Demand.
// <i> If this bit is 0: The oscillator is always on, if enabled.
// <i> If this bit is 1: the oscillator will only be running when requested by a peripheral.
// <id> xosc32k_arch_ondemand
#ifndef CONF_XOSC32K_ONDEMAND
#define CONF_XOSC32K_ONDEMAND 1
#endif

// <q> Run In Standby
// <i> Run In standby Mode
// <i> If this bit is 0: The oscillator is disabled in standby sleep mode.
// <i> If this bit is 1: The oscillator is not stopped in standby sleep mode.
// <id> xosc32k_arch_runstdby
#ifndef CONF_XOSC32K_RUNSTDBY
#define CONF_XOSC32K_RUNSTDBY 0
#endif

// <q> Enable 1K
// <i> Enable 1K
// <id> xosc32k_arch_en1k
#ifndef CONF_XOSC32K_EN1K
#define CONF_XOSC32K_EN1K 0
#endif

// <q> Enable 32Khz Output
// <i> Enable 32 Khz Output
// <id> xosc32k_arch_en32k
#ifndef CONF_XOSC32K_EN32K
#define CONF_XOSC32K_EN32K 0
#endif

// <q> Enable XTAL
// <i> Enable XTAL
// <id> xosc32k_arch_xtalen
#ifndef CONF_XOSC32K_XTALEN
#define CONF_XOSC32K_XTALEN 0
#endif

// <q> Write Lock
// <i> Write Lock
// <id> xosc32k_arch_wrtlock
#ifndef CONF_XOSC32K_WRTLOCK
#define CONF_XOSC32K_WRTLOCK 0
#endif

// <q> Automatic Amplitude Control Enable
// <i> Indicates whether Automatic Amplitude Control is Enabled or not
// <id> xosc32k_arch_aampen
#ifndef CONF_XOSC32K_AAMPEN
#define CONF_XOSC32K_AAMPEN 0
#endif

// <y> Start up time for the 32K Oscillator
// <CONF_XOSC32K_STARTUP_TIME_122MCS"> 122 us
// <CONF_XOSC32K_STARTUP_TIME_1068MCS"> 1068 us
// <CONF_XOSC32K_STARTUP_TIME_65592MCS"> 62592 us
// <CONF_XOSC32K_STARTUP_TIME_125092MCS"> 1125092 us
// <CONF_XOSC32K_STARTUP_TIME_500092MCS"> 500092 us
// <CONF_XOSC32K_STARTUP_TIME_1000092MCS"> 1000092 us
// <CONF_XOSC32K_STARTUP_TIME_2000092MCS"> 2000092 us
// <CONF_XOSC32K_STARTUP_TIME_4000092MCS"> 4000092 us
// <i> Start Up Time for the 32K Oscillator
// <i> Default: 122 us
// <id> xosc32k_arch_startup
#ifndef CONF_XOSC32K_STARTUP
#define CONF_XOSC32K_STARTUP CONF_XOSC32K_STARTUP_TIME_2000092MCS
#endif

// </h>
// </e>

// <e> External Multipurpose Crystal Oscillator Configuration
// <i> Indicates whether configuration for External Multipurpose Osc is enabled or not
// <id> enable_xosc
#ifndef CONF_XOSC_CONFIG
#define CONF_XOSC_CONFIG 0
#endif

// <o> Frequency <400000-32000000>
// <i> Oscillation frequency of the resonator connected to the External Multipurpose Crystal Oscillator.
// <id> xosc_frequency
#ifndef CONF_XOSC_FREQUENCY
#define CONF_XOSC_FREQUENCY 400000
#endif

// <h> External Multipurpose Crystal Oscillator (XOSC) Control
// <q> Enable
// <i> Indicates whether External Multipurpose Oscillator is enabled or not
// <id> xosc_arch_enable
#ifndef CONF_XOSC_ENABLE
#define CONF_XOSC_ENABLE 0
#endif

// <q> On Demand
// <i> Enable On Demand
// <i> If this bit is 0: The oscillator is always on, if enabled.
// <i> If this bit is 1: the oscillator will only be running when requested by a peripheral.
// <id> xosc_arch_ondemand
#ifndef CONF_XOSC_ONDEMAND
#define CONF_XOSC_ONDEMAND 1
#endif

// <q> Run In Standby
// <i> Run In standby Mode
// <i> If this bit is 0: The oscillator is disabled in standby sleep mode.
// <i> If this bit is 1: The oscillator is not stopped in standby sleep mode.
// <id> xosc_arch_runstdby
#ifndef CONF_XOSC_RUNSTDBY
#define CONF_XOSC_RUNSTDBY 0
#endif

// <q> Enable XTAL
// <i> Enable XTAL
// <id> xosc_arch_xtalen
#ifndef CONF_XOSC_XTALEN
#define CONF_XOSC_XTALEN 0
#endif

// <q> Automatic Amplitude Control Enable
// <i> Indicates whether Automatic Amplitude Control is Enabled or not
// <id> xosc_arch_ampgc
#ifndef CONF_XOSC_AMPGC
#define CONF_XOSC_AMPGC 0
#endif

// <y> Gain of the Oscillator
// <SYSCTRL_XOSC_GAIN_0_Val"> 2Mhz
// <SYSCTRL_XOSC_GAIN_1_Val"> 4Mhz
// <SYSCTRL_XOSC_GAIN_2_Val"> 8Mhz
// <SYSCTRL_XOSC_GAIN_3_Val"> 16Mhz
// <SYSCTRL_XOSC_GAIN_4_Val"> 30Mhz
// <i> Select the Gain of the oscillator
// <id> xosc_arch_gain
#ifndef CONF_XOSC_GAIN
#define CONF_XOSC_GAIN SYSCTRL_XOSC_GAIN_0_Val
#endif

// <y> Start up time for the External Oscillator
// <CONF_XOSC_STARTUP_TIME_31MCS"> 31 us
// <CONF_XOSC_STARTUP_TIME_61MCS"> 61 us
// <CONF_XOSC_STARTUP_TIME_122MCS"> 122 us
// <CONF_XOSC_STARTUP_TIME_244MCS"> 244 us
// <CONF_XOSC_STARTUP_TIME_488MCS"> 488 us
// <CONF_XOSC_STARTUP_TIME_977MCS"> 977 us
// <CONF_XOSC_STARTUP_TIME_1953MCS"> 1953 us
// <CONF_XOSC_STARTUP_TIME_3906MCS"> 3906 us
// <CONF_XOSC_STARTUP_TIME_7813MCS"> 7813 us
// <CONF_XOSC_STARTUP_TIME_15625MCS"> 15625 us
// <CONF_XOSC_STARTUP_TIME_31250MCS"> 31250 us
// <CONF_XOSC_STARTUP_TIME_62500MCS"> 62500 us
// <CONF_XOSC_STARTUP_TIME_125000MCS"> 125000 us
// <CONF_XOSC_STARTUP_TIME_250000MCS"> 250000 us
// <CONF_XOSC_STARTUP_TIME_500000MCS"> 500000 us
// <CONF_XOSC_STARTUP_TIME_1000000MCS"> 1000000 us
// <i> Start Up Time for the External Oscillator
// <i> Default: 31 us
// <id> xosc_arch_startup
#ifndef CONF_XOSC_STARTUP
#define CONF_XOSC_STARTUP CONF_XOSC_STARTUP_TIME_31MCS
#endif

// </h>
// </e>

// <e> 32kHz Ultra Low Power Internal Oscillator Configuration
// <i> Indicates whether configuration for OSCULP32K is enabled or not
// <id> enable_osculp32k
#ifndef CONF_OSCULP32K_CONFIG
#define CONF_OSCULP32K_CONFIG 1
#endif

// <h> 32kHz Ultra Low Power Internal Oscillator (OSCULP32K) Control
// <q> Write Lock
// <i> Locks the OSCULP32K register for future writes to fix the OSCULP32K configuration
// <id> osculp32k_arch_wrtlock
#ifndef CONF_OSCULP32K_WRTLOCK
#define CONF_OSCULP32K_WRTLOCK 0
#endif

// <q> Overwrite Default Osc Calibration
//   <i> Overwrite Default Osc Calibration
// <id> osculp32k_arch_overwrite_calibration
#ifndef CONF_OSCULP32K_OVERWRITE_CALIBRATION
#define CONF_OSCULP32K_OVERWRITE_CALIBRATION 0
#endif

//  <o>Osc Calibration Value <0-255>
//      <i> Set the Oscillator Calibration Value
//  <i> Default: 0
// <id> osculp32k_arch_calib
#ifndef CONF_OSCULP32K_CALIB
#define CONF_OSCULP32K_CALIB 0
#endif

// </h>
// </e>

// <e> DFLL Configuration
// <i> Indicates whether configuration for DFLL is enabled or not
// <id> enable_dfll48m
#ifndef CONF_DFLL_CONFIG
#define CONF_DFLL_CONFIG 1
#endif

// <y> Reference Clock Source
// <GCLK_CLKCTRL_GEN_GCLK0_Val"> Generic clock generator 0
// <GCLK_CLKCTRL_GEN_GCLK1_Val"> Generic clock generator 1
// <GCLK_CLKCTRL_GEN_GCLK2_Val"> Generic clock generator 2
// <GCLK_CLKCTRL_GEN_GCLK3_Val"> Generic clock generator 3
// <GCLK_CLKCTRL_GEN_GCLK4_Val"> Generic clock generator 4
// <GCLK_CLKCTRL_GEN_GCLK5_Val"> Generic clock generator 5
// <GCLK_CLKCTRL_GEN_GCLK6_Val"> Generic clock generator 6
// <GCLK_CLKCTRL_GEN_GCLK7_Val"> Generic clock generator 7
// <i> Select the clock source.
// <id> dfll48m_ref_clock
#ifndef CONF_DFLL_GCLK
#define CONF_DFLL_GCLK GCLK_CLKCTRL_GEN_GCLK4_Val
#endif

// <h> DFLL Control
// <q> DFLL Enable
// <i> Indicates whether DFLL is enabled or not
// <id> dfll48m_arch_enable
#ifndef CONF_DFLL_ENABLE
#define CONF_DFLL_ENABLE 1
#endif

// <q> Wait Lock
// <i> Indicates whether Wait Lock is Enables or not
// <id> dfll48m_arch_waitlock
#ifndef CONF_DFLL_WAITLOCK
#define CONF_DFLL_WAITLOCK 0
#endif

// <q> Bypass Coarse Lock
// <i> Indicates whether Bypass coarse lock is enabled or not
// <id> dfll48m_arch_bplckc
#ifndef CONF_DFLL_BPLCKC
#define CONF_DFLL_BPLCKC 0
#endif

// <q> Quick Lock Disable
// <i> Quick Lock Disable
// <id> dfll48m_arch_qldis
#ifndef CONF_DFLL_QLDIS
#define CONF_DFLL_QLDIS 0
#endif

// <q> Chill Cycle Disable
// <i> Chill Cycle Disable
// <id> dfll48m_arch_ccdis
#ifndef CONF_DFLL_CCDIS
#define CONF_DFLL_CCDIS 1
#endif

// <q> On Demand
// <i> Enable On Demand
// <i> If this bit is 0: The DFLL is always on, if enabled.
// <i> If this bit is 1: the DFLL will only be running when requested by a peripheral.
// <id> dfll48m_arch_ondemand
#ifndef CONF_DFLL_ONDEMAND
#define CONF_DFLL_ONDEMAND 0
#endif

// <q> Run In Standby
// <i> Run In standby Mode
// <i> If this bit is 0: The DFLL is disabled in standby sleep mode.
// <i> If this bit is 1: The DFLL is not stopped in standby sleep mode.
// <id> dfll48m_arch_runstdby
#ifndef CONF_DFLL_RUNSTDBY
#define CONF_DFLL_RUNSTDBY 0
#endif

// <q> USB Clock Recovery Mode
// <i> USB Clock Recovery Mode
// <id> dfll48m_arch_usbcrm
#ifndef CONF_DFLL_USBCRM
#define CONF_DFLL_USBCRM 1
#endif

#if CONF_DFLL_USBCRM == 1
#if CONF_DFLL_QLDIS == 1
#warning QLDIS must be cleared to speed up the lock phase
#endif
#if CONF_DFLL_CCDIS == 0
#warning CCDIS should be set to speed up the lock phase
#endif
#endif

// <q> Lose Lock After Wake
// <i> Lose Lock After Wake
// <id> dfll48m_arch_llaw
#ifndef CONF_DFLL_LLAW
#define CONF_DFLL_LLAW 0
#endif

// <q> Stable DFLL Frequency
// <i> Stable DFLL Frequency
// <i> If 0: FINE calibration tracks changes in output frequency.
// <i> If 1: FINE calibration register value will be fixed after a fine lock.
// <id> dfll48m_arch_stable
#ifndef CONF_DFLL_STABLE
#define CONF_DFLL_STABLE 0
#endif

// <y> Operating Mode Selection
// <CONF_DFLL_OPEN_LOOP_MODE"> Open Loop Mode
// <CONF_DFLL_CLOSED_LOOP_MODE"> Closed Loop Mode
// <i> Mode
// <id> dfll48m_mode
#ifndef CONF_DFLL_MODE
#define CONF_DFLL_MODE CONF_DFLL_CLOSED_LOOP_MODE
#endif

// <o> Coarse Maximum Step <0x0-0x1F>
// <id> dfll_arch_cstep
#ifndef CONF_DFLL_CSTEP
#define CONF_DFLL_CSTEP 1
#endif

// <o> Fine Maximum Step <0x0-0x3FF>
// <id> dfll_arch_fstep
#ifndef CONF_DFLL_FSTEP
#define CONF_DFLL_FSTEP 1
#endif

//  <o>DFLL Multiply Factor<0-65535>
//      <i> Set the DFLL Multiply Factor
//  <i> Default: 0
//  <id> dfll48m_mul
#ifndef CONF_DFLL_MUL
#define CONF_DFLL_MUL 48000
#endif

// <e> DFLL Calibration Overwrite
// <i> Indicates whether Overwrite Calibration value of DFLL
// <id> dfll48m_arch_calibration
#ifndef CONF_DFLL_OVERWRITE_CALIBRATION
#define CONF_DFLL_OVERWRITE_CALIBRATION 0
#endif

// <o> Coarse Value <0x0-0x3F>
// <id> dfll48m_arch_coarse
#ifndef CONF_DFLL_COARSE
#define CONF_DFLL_COARSE (0x1f)
#endif

// <o> Fine Value <0x0-0x3FF>
// <id> dfll48m_arch_fine
#ifndef CONF_DFLL_FINE
#define CONF_DFLL_FINE (0x200)
#endif

#if CONF_DFLL_OVERWRITE_CALIBRATION == 0
#define CONF_DEFAULT_CORASE                                                                                            \
    ((FUSES_DFLL48M_COARSE_CAL_Msk & (*((uint32_t *)FUSES_DFLL48M_COARSE_CAL_ADDR))) >> FUSES_DFLL48M_COARSE_CAL_Pos)

#define CONF_DFLLVAL                                                                                                   \
    SYSCTRL_DFLLVAL_COARSE(((CONF_DEFAULT_CORASE) == 0x3F) ? 0x1F : (CONF_DEFAULT_CORASE))                             \
    | SYSCTRL_DFLLVAL_FINE(512)

#else
#define CONF_DFLLVAL SYSCTRL_DFLLVAL_COARSE(CONF_DFLL_COARSE) | SYSCTRL_DFLLVAL_FINE(CONF_DFLL_FINE)

#endif
// </e>

// </h>
// </e>

// <e> DPLL Configuration
// <i> Indicates whether configuration for DPLL is enabled or not
// <id> enable_fdpll96m
#ifndef CONF_DPLL_CONFIG
#define CONF_DPLL_CONFIG 0
#endif

// <y> Reference Clock Source
// <GCLK_GENCTRL_SRC_XOSC32K"> 32kHz External Crystal Oscillator (XOSC32K)
// <GCLK_GENCTRL_SRC_XOSC"> External Crystal Oscillator 0.4-32MHz (XOSC)
// <GCLK_CLKCTRL_GEN_GCLK0_Val"> Generic clock generator 0
// <GCLK_CLKCTRL_GEN_GCLK1_Val"> Generic clock generator 1
// <GCLK_CLKCTRL_GEN_GCLK2_Val"> Generic clock generator 2
// <GCLK_CLKCTRL_GEN_GCLK3_Val"> Generic clock generator 3
// <GCLK_CLKCTRL_GEN_GCLK4_Val"> Generic clock generator 4
// <GCLK_CLKCTRL_GEN_GCLK5_Val"> Generic clock generator 5
// <GCLK_CLKCTRL_GEN_GCLK6_Val"> Generic clock generator 6
// <GCLK_CLKCTRL_GEN_GCLK7_Val"> Generic clock generator 7
// <i> Select the clock source.
// <id> fdpll96m_ref_clock
#ifndef CONF_DPLL_GCLK
#define CONF_DPLL_GCLK GCLK_CLKCTRL_GEN_GCLK3_Val
#endif

#if (CONF_DPLL_GCLK == GCLK_GENCTRL_SRC_XOSC32K)
#define CONF_DPLL_REFCLK SYSCTRL_DPLLCTRLB_REFCLK_REF0_Val
#elif (CONF_DPLL_GCLK == GCLK_GENCTRL_SRC_XOSC)
#define CONF_DPLL_REFCLK SYSCTRL_DPLLCTRLB_REFCLK_REF1_Val
#else
#define CONF_DPLL_REFCLK SYSCTRL_DPLLCTRLB_REFCLK_GCLK_Val
#endif

// <h> DPLL Control
// <q> ON Demand
// <i> Enable On Demand
// <i> If this bit is 0: The DFLL is always on, if enabled.
// <i> If this bit is 1: the DFLL will only be running when requested by a peripheral.
// <id> fdpll96m_arch_ondemand
#ifndef CONF_DPLL_ONDEMAND
#define CONF_DPLL_ONDEMAND 1
#endif

// <q> Run In Standby
// <i> Run In standby Mode
// <i> If this bit is 0: The DFLL is disabled in standby sleep mode.
// <i> If this bit is 1: The DFLL is not stopped in standby sleep mode.
// <id> fdpll96m_arch_runstdby
#ifndef CONF_DPLL_RUNSTDBY
#define CONF_DPLL_RUNSTDBY 0
#endif

// <q> DPLL Enable
// <i> Indicates whether DPLL is enabled or not
// <id> fdpll96m_arch_enable
#ifndef CONF_DPLL_ENABLE
#define CONF_DPLL_ENABLE 0
#endif

// <q> Lock ByPass
// <i> Enabling it makes the CLK_FDPLL96M always running otherwise it will be turned off when lock signal is low
// <id> fdpll96m_arch_lbypass
#ifndef CONF_DPLL_LBYPASS
#define CONF_DPLL_LBYPASS 0
#endif

// <o>Clock Divider <0-2047>
// <i> Clock Division Factor (Applicable if reference clock is XOSC)
// <id> fdpll96m_clock_div
#ifndef CONF_DPLL_DIV
#define CONF_DPLL_DIV 0
#endif

// <o>DPLL LDRFRAC<0-15>
// <i> Set the  fractional part of the frequency multiplier.
// <id> fdpll96m_ldrfrac
#ifndef CONF_DPLL_LDRFRAC
#define CONF_DPLL_LDRFRAC 13
#endif

// <o>DPLL LDR <0-4095>
// <i> Set the  integer part of the frequency multiplier.
// <id> fdpll96m_ldr
#ifndef CONF_DPLL_LDR
#define CONF_DPLL_LDR 1463
#endif

// </h>
// </e>

#define CONF_DPLL_LTIME SYSCTRL_DPLLCTRLB_LTIME_DEFAULT_Val
#define CONF_DPLL_WUF 0
#define CONF_DPLL_LPEN 0
#define CONF_DPLL_FILTER SYSCTRL_DPLLCTRLB_FILTER_DEFAULT_Val

// <<< end of configuration section >>>

#endif // HPL_SYSCTRL_CONFIG_H

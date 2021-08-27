/* Auto-generated config file hpl_oscctrl_config.h */
#ifndef HPL_OSCCTRL_CONFIG_H
#define HPL_OSCCTRL_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <e> External Multipurpose Crystal Oscillator Configuration
// <i> Indicates whether configuration for XOSC0 is enabled or not
// <id> enable_xosc0
#ifndef CONF_XOSC0_CONFIG
#define CONF_XOSC0_CONFIG 0
#endif

// <o> Frequency <8000000-48000000>
// <i> Oscillation frequency of the resonator connected to the External Multipurpose Crystal Oscillator.
// <id> xosc0_frequency
#ifndef CONF_XOSC_FREQUENCY
#define CONF_XOSC0_FREQUENCY 12000000
#endif

// <h> External Multipurpose Crystal Oscillator Control
// <q> Oscillator enable
// <i> Indicates whether External Multipurpose Crystal Oscillator is enabled or not
// <id> xosc0_arch_enable
#ifndef CONF_XOSC0_ENABLE
#define CONF_XOSC0_ENABLE 0
#endif

// <o> Start-Up Time
// <0x0=>31us
// <0x1=>61us
// <0x2=>122us
// <0x3=>244us
// <0x4=>488us
// <0x5=>977us
// <0x6=>1953us
// <0x7=>3906us
// <0x8=>7813us
// <0x9=>15625us
// <0xA=>31250us
// <0xB=>62500us
// <0xC=>125000us
// <0xD=>250000us
// <0xE=>500000us
// <0xF=>1000000us
// <id> xosc0_arch_startup
#ifndef CONF_XOSC0_STARTUP
#define CONF_XOSC0_STARTUP 0
#endif

// <q> Clock Switch Back
// <i> Indicates whether Clock Switch Back is enabled or not
// <id> xosc0_arch_swben
#ifndef CONF_XOSC0_SWBEN
#define CONF_XOSC0_SWBEN 0
#endif

// <q> Clock Failure Detector
// <i> Indicates whether Clock Failure Detector is enabled or not
// <id> xosc0_arch_cfden
#ifndef CONF_XOSC0_CFDEN
#define CONF_XOSC0_CFDEN 0
#endif

// <q> Automatic Loop Control Enable
// <i> Indicates whether Automatic Loop Control is enabled or not
// <id> xosc0_arch_enalc
#ifndef CONF_XOSC0_ENALC
#define CONF_XOSC0_ENALC 0
#endif

// <q> Low Buffer Gain Enable
// <i> Indicates whether Low Buffer Gain is enabled or not
// <id> xosc0_arch_lowbufgain
#ifndef CONF_XOSC0_LOWBUFGAIN
#define CONF_XOSC0_LOWBUFGAIN 0
#endif

// <q> On Demand Control
// <i> Indicates whether On Demand Control is enabled or not
// <id> xosc0_arch_ondemand
#ifndef CONF_XOSC0_ONDEMAND
#define CONF_XOSC0_ONDEMAND 0
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> xosc0_arch_runstdby
#ifndef CONF_XOSC0_RUNSTDBY
#define CONF_XOSC0_RUNSTDBY 0
#endif

// <q> Crystal connected to XIN/XOUT Enable
// <i> Indicates whether the connections between the I/O pads and the external clock or crystal oscillator is enabled or not
// <id> xosc0_arch_xtalen
#ifndef CONF_XOSC0_XTALEN
#define CONF_XOSC0_XTALEN 0
#endif
// </h>
// </e>

#if CONF_XOSC0_FREQUENCY >= 32000000
#define CONF_XOSC0_CFDPRESC 0x0
#define CONF_XOSC0_IMULT 0x7
#define CONF_XOSC0_IPTAT 0x3
#elif CONF_XOSC0_FREQUENCY >= 24000000
#define CONF_XOSC0_CFDPRESC 0x1
#define CONF_XOSC0_IMULT 0x6
#define CONF_XOSC0_IPTAT 0x3
#elif CONF_XOSC0_FREQUENCY >= 16000000
#define CONF_XOSC0_CFDPRESC 0x2
#define CONF_XOSC0_IMULT 0x5
#define CONF_XOSC0_IPTAT 0x3
#elif CONF_XOSC0_FREQUENCY >= 8000000
#define CONF_XOSC0_CFDPRESC 0x3
#define CONF_XOSC0_IMULT 0x4
#define CONF_XOSC0_IPTAT 0x3
#endif

// <e> External Multipurpose Crystal Oscillator Configuration
// <i> Indicates whether configuration for XOSC1 is enabled or not
// <id> enable_xosc1
#ifndef CONF_XOSC1_CONFIG
#define CONF_XOSC1_CONFIG 0
#endif

// <o> Frequency <8000000-48000000>
// <i> Oscillation frequency of the resonator connected to the External Multipurpose Crystal Oscillator.
// <id> xosc1_frequency
#ifndef CONF_XOSC_FREQUENCY
#define CONF_XOSC1_FREQUENCY 12000000
#endif

// <h> External Multipurpose Crystal Oscillator Control
// <q> Oscillator enable
// <i> Indicates whether External Multipurpose Crystal Oscillator is enabled or not
// <id> xosc1_arch_enable
#ifndef CONF_XOSC1_ENABLE
#define CONF_XOSC1_ENABLE 0
#endif

// <o> Start-Up Time
// <0x0=>31us
// <0x1=>61us
// <0x2=>122us
// <0x3=>244us
// <0x4=>488us
// <0x5=>977us
// <0x6=>1953us
// <0x7=>3906us
// <0x8=>7813us
// <0x9=>15625us
// <0xA=>31250us
// <0xB=>62500us
// <0xC=>125000us
// <0xD=>250000us
// <0xE=>500000us
// <0xF=>1000000us
// <id> xosc1_arch_startup
#ifndef CONF_XOSC1_STARTUP
#define CONF_XOSC1_STARTUP 0
#endif

// <q> Clock Switch Back
// <i> Indicates whether Clock Switch Back is enabled or not
// <id> xosc1_arch_swben
#ifndef CONF_XOSC1_SWBEN
#define CONF_XOSC1_SWBEN 0
#endif

// <q> Clock Failure Detector
// <i> Indicates whether Clock Failure Detector is enabled or not
// <id> xosc1_arch_cfden
#ifndef CONF_XOSC1_CFDEN
#define CONF_XOSC1_CFDEN 0
#endif

// <q> Automatic Loop Control Enable
// <i> Indicates whether Automatic Loop Control is enabled or not
// <id> xosc1_arch_enalc
#ifndef CONF_XOSC1_ENALC
#define CONF_XOSC1_ENALC 0
#endif

// <q> Low Buffer Gain Enable
// <i> Indicates whether Low Buffer Gain is enabled or not
// <id> xosc1_arch_lowbufgain
#ifndef CONF_XOSC1_LOWBUFGAIN
#define CONF_XOSC1_LOWBUFGAIN 0
#endif

// <q> On Demand Control
// <i> Indicates whether On Demand Control is enabled or not
// <id> xosc1_arch_ondemand
#ifndef CONF_XOSC1_ONDEMAND
#define CONF_XOSC1_ONDEMAND 0
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> xosc1_arch_runstdby
#ifndef CONF_XOSC1_RUNSTDBY
#define CONF_XOSC1_RUNSTDBY 0
#endif

// <q> Crystal connected to XIN/XOUT Enable
// <i> Indicates whether the connections between the I/O pads and the external clock or crystal oscillator is enabled or not
// <id> xosc1_arch_xtalen
#ifndef CONF_XOSC1_XTALEN
#define CONF_XOSC1_XTALEN 0
#endif
// </h>
// </e>

#if CONF_XOSC1_FREQUENCY >= 32000000
#define CONF_XOSC1_CFDPRESC 0x0
#define CONF_XOSC1_IMULT 0x7
#define CONF_XOSC1_IPTAT 0x3
#elif CONF_XOSC1_FREQUENCY >= 24000000
#define CONF_XOSC1_CFDPRESC 0x1
#define CONF_XOSC1_IMULT 0x6
#define CONF_XOSC1_IPTAT 0x3
#elif CONF_XOSC1_FREQUENCY >= 16000000
#define CONF_XOSC1_CFDPRESC 0x2
#define CONF_XOSC1_IMULT 0x5
#define CONF_XOSC1_IPTAT 0x3
#elif CONF_XOSC1_FREQUENCY >= 8000000
#define CONF_XOSC1_CFDPRESC 0x3
#define CONF_XOSC1_IMULT 0x4
#define CONF_XOSC1_IPTAT 0x3
#endif

// <e> DFLL Configuration
// <i> Indicates whether configuration for DFLL is enabled or not
// <id> enable_dfll
#ifndef CONF_DFLL_CONFIG
#define CONF_DFLL_CONFIG 0
#endif

// <y> Reference Clock Source
// <GCLK_PCHCTRL_GEN_GCLK0_Val"> Generic clock generator 0
// <GCLK_PCHCTRL_GEN_GCLK1_Val"> Generic clock generator 1
// <GCLK_PCHCTRL_GEN_GCLK2_Val"> Generic clock generator 2
// <GCLK_PCHCTRL_GEN_GCLK3_Val"> Generic clock generator 3
// <GCLK_PCHCTRL_GEN_GCLK4_Val"> Generic clock generator 4
// <GCLK_PCHCTRL_GEN_GCLK5_Val"> Generic clock generator 5
// <GCLK_PCHCTRL_GEN_GCLK6_Val"> Generic clock generator 6
// <GCLK_PCHCTRL_GEN_GCLK7_Val"> Generic clock generator 7
// <GCLK_PCHCTRL_GEN_GCLK8_Val"> Generic clock generator 8
// <GCLK_PCHCTRL_GEN_GCLK9_Val"> Generic clock generator 9
// <GCLK_PCHCTRL_GEN_GCLK10_Val"> Generic clock generator 10
// <GCLK_PCHCTRL_GEN_GCLK11_Val"> Generic clock generator 11
// <i> Select the clock source
// <id> dfll_ref_clock
#ifndef CONF_DFLL_GCLK
#define CONF_DFLL_GCLK GCLK_PCHCTRL_GEN_GCLK3_Val
#endif

// <h> Digital Frequency Locked Loop Control
// <q> DFLL Enable
// <i> Indicates whether DFLL is enabled or not
// <id> dfll_arch_enable
#ifndef CONF_DFLL_ENABLE
#define CONF_DFLL_ENABLE 1
#endif

// <q> On Demand Control
// <i> Indicates whether On Demand Control is enabled or not
// <id> dfll_arch_ondemand
#ifndef CONF_DFLL_ONDEMAND
#define CONF_DFLL_ONDEMAND 0
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> dfll_arch_runstdby
#ifndef CONF_DFLL_RUNSTDBY
#define CONF_DFLL_RUNSTDBY 0
#endif

// <q> USB Clock Recovery Mode
// <i> Indicates whether USB Clock Recovery Mode is enabled or not
// <id> dfll_arch_usbcrm
#ifndef CONF_DFLL_USBCRM
#define CONF_DFLL_USBCRM 1
#endif

// <q> Wait Lock
// <i> Indicates whether Wait Lock is enabled or not
// <id> dfll_arch_waitlock
#ifndef CONF_DFLL_WAITLOCK
#define CONF_DFLL_WAITLOCK 1
#endif

// <q> Bypass Coarse Lock
// <i> Indicates whether Bypass Coarse Lock is enabled or not
// <id> dfll_arch_bplckc
#ifndef CONF_DFLL_BPLCKC
#define CONF_DFLL_BPLCKC 0
#endif

// <q> Quick Lock Disable
// <i> Indicates whether Quick Lock Disable is enabled or not
// <id> dfll_arch_qldis
#ifndef CONF_DFLL_QLDIS
#define CONF_DFLL_QLDIS 0
#endif

// <q> Chill Cycle Disable
// <i> Indicates whether Chill Cycle Disable is enabled or not
// <id> dfll_arch_ccdis
#ifndef CONF_DFLL_CCDIS
#define CONF_DFLL_CCDIS 1
#endif

// <q> Lose Lock After Wake
// <i> Indicates whether Lose Lock After Wake is enabled or not
// <id> dfll_arch_llaw
#ifndef CONF_DFLL_LLAW
#define CONF_DFLL_LLAW 0
#endif

// <q> Stable DFLL Frequency
// <i> Indicates whether Stable DFLL Frequency is enabled or not
// <id> dfll_arch_stable
#ifndef CONF_DFLL_STABLE
#define CONF_DFLL_STABLE 0
#endif

// <o> Operating Mode Selection
// <0=>Open Loop Mode
// <1=>Closed Loop Mode
// <id> dfll_mode
#ifndef CONF_DFLL_MODE
#define CONF_DFLL_MODE 0x0
#endif

// <o> Coarse Maximum Step <0x0-0x1F>
// <id> dfll_arch_cstep
#ifndef CONF_DFLL_CSTEP
#define CONF_DFLL_CSTEP 0x1
#endif

// <o> Fine Maximum Step <0x0-0xFF>
// <id> dfll_arch_fstep
#ifndef CONF_DFLL_FSTEP
#define CONF_DFLL_FSTEP 0x1
#endif

// <o> DFLL Multiply Factor <0x0-0xFFFF>
//  <id> dfll_mul
#ifndef CONF_DFLL_MUL
#define CONF_DFLL_MUL 0x0
#endif

// <e> DFLL Calibration Overwrite
// <i> Indicates whether Overwrite Calibration value of DFLL
// <id> dfll_arch_calibration
#ifndef CONF_DFLL_OVERWRITE_CALIBRATION
#define CONF_DFLL_OVERWRITE_CALIBRATION 0
#endif

// <o> Coarse Value <0x0-0x3F>
// <id> dfll_arch_coarse
#ifndef CONF_DFLL_COARSE
#define CONF_DFLL_COARSE (0x1f / 4)
#endif

// <o> Fine Value <0x0-0xFF>
// <id> dfll_arch_fine
#ifndef CONF_DFLL_FINE
#define CONF_DFLL_FINE (0x80)
#endif

// </e>

// </h>

// </e>

// <e> FDPLL0 Configuration
// <i> Indicates whether configuration for FDPLL0 is enabled or not
// <id> enable_fdpll0
#ifndef CONF_FDPLL0_CONFIG
#define CONF_FDPLL0_CONFIG 1
#endif

// <y> Reference Clock Source
// <GCLK_GENCTRL_SRC_XOSC32K"> 32kHz External Crystal Oscillator (XOSC32K)
// <GCLK_GENCTRL_SRC_XOSC0"> External Crystal Oscillator 8-48MHz (XOSC0)
// <GCLK_GENCTRL_SRC_XOSC1"> External Crystal Oscillator 8-48MHz (XOSC1)
// <GCLK_PCHCTRL_GEN_GCLK0_Val"> Generic clock generator 0
// <GCLK_PCHCTRL_GEN_GCLK1_Val"> Generic clock generator 1
// <GCLK_PCHCTRL_GEN_GCLK2_Val"> Generic clock generator 2
// <GCLK_PCHCTRL_GEN_GCLK3_Val"> Generic clock generator 3
// <GCLK_PCHCTRL_GEN_GCLK4_Val"> Generic clock generator 4
// <GCLK_PCHCTRL_GEN_GCLK5_Val"> Generic clock generator 5
// <GCLK_PCHCTRL_GEN_GCLK6_Val"> Generic clock generator 6
// <GCLK_PCHCTRL_GEN_GCLK7_Val"> Generic clock generator 7
// <GCLK_PCHCTRL_GEN_GCLK8_Val"> Generic clock generator 8
// <GCLK_PCHCTRL_GEN_GCLK9_Val"> Generic clock generator 9
// <GCLK_PCHCTRL_GEN_GCLK10_Val"> Generic clock generator 10
// <GCLK_PCHCTRL_GEN_GCLK11_Val"> Generic clock generator 11
// <i> Select the clock source.
// <id> fdpll0_ref_clock
#ifndef CONF_FDPLL0_GCLK
#define CONF_FDPLL0_GCLK GCLK_PCHCTRL_GEN_GCLK5_Val
#endif

// <h> Digital Phase Locked Loop Control
// <q> Enable
// <i> Indicates whether Digital Phase Locked Loop is enabled or not
// <id> fdpll0_arch_enable
#ifndef CONF_FDPLL0_ENABLE
#define CONF_FDPLL0_ENABLE 1
#endif

// <q> On Demand Control
// <i> Indicates whether On Demand Control is enabled or not
// <id> fdpll0_arch_ondemand
#ifndef CONF_FDPLL0_ONDEMAND
#define CONF_FDPLL0_ONDEMAND 0
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> fdpll0_arch_runstdby
#ifndef CONF_FDPLL0_RUNSTDBY
#define CONF_FDPLL0_RUNSTDBY 0
#endif

// <o> Loop Divider Ratio Fractional Part <0x0-0x1F>
// <id> fdpll0_ldrfrac
#ifndef CONF_FDPLL0_LDRFRAC
#define CONF_FDPLL0_LDRFRAC 0x0
#endif

// <o> Loop Divider Ratio Integer Part <0x0-0x1FFF>
// <id> fdpll0_ldr
#ifndef CONF_FDPLL0_LDR
#define CONF_FDPLL0_LDR 59
#endif

// <o> Clock Divider <0x0-0x7FF>
// <id> fdpll0_clock_div
#ifndef CONF_FDPLL0_DIV
#define CONF_FDPLL0_DIV 0x0
#endif

// <q> DCO Filter Enable
// <i> Indicates whether DCO Filter Enable is enabled or not
// <id> fdpll0_arch_dcoen
#ifndef CONF_FDPLL0_DCOEN
#define CONF_FDPLL0_DCOEN 0
#endif

// <o> Sigma-Delta DCO Filter Selection <0x0-0x7>
// <id> fdpll0_clock_dcofilter
#ifndef CONF_FDPLL0_DCOFILTER
#define CONF_FDPLL0_DCOFILTER 0x0
#endif

// <q> Lock Bypass
// <i> Indicates whether Lock Bypass is enabled or not
// <id> fdpll0_arch_lbypass
#ifndef CONF_FDPLL0_LBYPASS
#define CONF_FDPLL0_LBYPASS 0
#endif

// <o> Lock Time
// <0x0=>No time-out, automatic lock
// <0x4=>The Time-out if no lock within 800 us
// <0x5=>The Time-out if no lock within 900 us
// <0x6=>The Time-out if no lock within 1 ms
// <0x7=>The Time-out if no lock within 11 ms
// <id> fdpll0_arch_ltime
#ifndef CONF_FDPLL0_LTIME
#define CONF_FDPLL0_LTIME 0x0
#endif

// <o> Reference Clock Selection
// <0x0=>GCLK clock reference
// <0x1=>XOSC32K clock reference
// <0x2=>XOSC0 clock reference
// <0x3=>XOSC1 clock reference
// <id> fdpll0_arch_refclk
#ifndef CONF_FDPLL0_REFCLK
#define CONF_FDPLL0_REFCLK 0x0
#endif

// <q> Wake Up Fast
// <i> Indicates whether Wake Up Fast is enabled or not
// <id> fdpll0_arch_wuf
#ifndef CONF_FDPLL0_WUF
#define CONF_FDPLL0_WUF 0
#endif

// <o> Proportional Integral Filter Selection <0x0-0xF>
// <id> fdpll0_arch_filter
#ifndef CONF_FDPLL0_FILTER
#define CONF_FDPLL0_FILTER 0x0
#endif

// </h>
// </e>
// <e> FDPLL1 Configuration
// <i> Indicates whether configuration for FDPLL1 is enabled or not
// <id> enable_fdpll1
#ifndef CONF_FDPLL1_CONFIG
#define CONF_FDPLL1_CONFIG 0
#endif

// <y> Reference Clock Source
// <GCLK_GENCTRL_SRC_XOSC32K"> 32kHz External Crystal Oscillator (XOSC32K)
// <GCLK_GENCTRL_SRC_XOSC0"> External Crystal Oscillator 8-48MHz (XOSC0)
// <GCLK_GENCTRL_SRC_XOSC1"> External Crystal Oscillator 8-48MHz (XOSC1)
// <GCLK_PCHCTRL_GEN_GCLK0_Val"> Generic clock generator 0
// <GCLK_PCHCTRL_GEN_GCLK1_Val"> Generic clock generator 1
// <GCLK_PCHCTRL_GEN_GCLK2_Val"> Generic clock generator 2
// <GCLK_PCHCTRL_GEN_GCLK3_Val"> Generic clock generator 3
// <GCLK_PCHCTRL_GEN_GCLK4_Val"> Generic clock generator 4
// <GCLK_PCHCTRL_GEN_GCLK5_Val"> Generic clock generator 5
// <GCLK_PCHCTRL_GEN_GCLK6_Val"> Generic clock generator 6
// <GCLK_PCHCTRL_GEN_GCLK7_Val"> Generic clock generator 7
// <GCLK_PCHCTRL_GEN_GCLK8_Val"> Generic clock generator 8
// <GCLK_PCHCTRL_GEN_GCLK9_Val"> Generic clock generator 9
// <GCLK_PCHCTRL_GEN_GCLK10_Val"> Generic clock generator 10
// <GCLK_PCHCTRL_GEN_GCLK11_Val"> Generic clock generator 11
// <i> Select the clock source.
// <id> fdpll1_ref_clock
#ifndef CONF_FDPLL1_GCLK
#define CONF_FDPLL1_GCLK GCLK_GENCTRL_SRC_XOSC32K
#endif

// <h> Digital Phase Locked Loop Control
// <q> Enable
// <i> Indicates whether Digital Phase Locked Loop is enabled or not
// <id> fdpll1_arch_enable
#ifndef CONF_FDPLL1_ENABLE
#define CONF_FDPLL1_ENABLE 0
#endif

// <q> On Demand Control
// <i> Indicates whether On Demand Control is enabled or not
// <id> fdpll1_arch_ondemand
#ifndef CONF_FDPLL1_ONDEMAND
#define CONF_FDPLL1_ONDEMAND 0
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> fdpll1_arch_runstdby
#ifndef CONF_FDPLL1_RUNSTDBY
#define CONF_FDPLL1_RUNSTDBY 0
#endif

// <o> Loop Divider Ratio Fractional Part <0x0-0x1F>
// <id> fdpll1_ldrfrac
#ifndef CONF_FDPLL1_LDRFRAC
#define CONF_FDPLL1_LDRFRAC 0xd
#endif

// <o> Loop Divider Ratio Integer Part <0x0-0x1FFF>
// <id> fdpll1_ldr
#ifndef CONF_FDPLL1_LDR
#define CONF_FDPLL1_LDR 0x5b7
#endif

// <o> Clock Divider <0x0-0x7FF>
// <id> fdpll1_clock_div
#ifndef CONF_FDPLL1_DIV
#define CONF_FDPLL1_DIV 0x0
#endif

// <q> DCO Filter Enable
// <i> Indicates whether DCO Filter Enable is enabled or not
// <id> fdpll1_arch_dcoen
#ifndef CONF_FDPLL1_DCOEN
#define CONF_FDPLL1_DCOEN 0
#endif

// <o> Sigma-Delta DCO Filter Selection <0x0-0x7>
// <id> fdpll1_clock_dcofilter
#ifndef CONF_FDPLL1_DCOFILTER
#define CONF_FDPLL1_DCOFILTER 0x0
#endif

// <q> Lock Bypass
// <i> Indicates whether Lock Bypass is enabled or not
// <id> fdpll1_arch_lbypass
#ifndef CONF_FDPLL1_LBYPASS
#define CONF_FDPLL1_LBYPASS 0
#endif

// <o> Lock Time
// <0x0=>No time-out, automatic lock
// <0x4=>The Time-out if no lock within 800 us
// <0x5=>The Time-out if no lock within 900 us
// <0x6=>The Time-out if no lock within 1 ms
// <0x7=>The Time-out if no lock within 11 ms
// <id> fdpll1_arch_ltime
#ifndef CONF_FDPLL1_LTIME
#define CONF_FDPLL1_LTIME 0x0
#endif

// <o> Reference Clock Selection
// <0x0=>GCLK clock reference
// <0x1=>XOSC32K clock reference
// <0x2=>XOSC0 clock reference
// <0x3=>XOSC1 clock reference
// <id> fdpll1_arch_refclk
#ifndef CONF_FDPLL1_REFCLK
#define CONF_FDPLL1_REFCLK 0x1
#endif

// <q> Wake Up Fast
// <i> Indicates whether Wake Up Fast is enabled or not
// <id> fdpll1_arch_wuf
#ifndef CONF_FDPLL1_WUF
#define CONF_FDPLL1_WUF 0
#endif

// <o> Proportional Integral Filter Selection <0x0-0xF>
// <id> fdpll1_arch_filter
#ifndef CONF_FDPLL1_FILTER
#define CONF_FDPLL1_FILTER 0x0
#endif

// </h>
// </e>

// <<< end of configuration section >>>

#endif // HPL_OSCCTRL_CONFIG_H

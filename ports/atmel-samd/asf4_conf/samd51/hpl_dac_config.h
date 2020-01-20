/* Auto-generated config file hpl_dac_config.h */
#ifndef HPL_DAC_CONFIG_H
#define HPL_DAC_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Basic configuration
// <o> Reference Selection
// <0x00=> Unbuffered external voltage reference
// <0x01=> Voltage supply
// <0x02=> Buffered external voltage reference
// <0x03=> Internal bandgap reference
// <id> dac_arch_refsel
#ifndef CONF_DAC_REFSEL
#define CONF_DAC_REFSEL 0
#endif

// <q> Differential mode
// <i> Indicates whether the differential mode is enabled or not
// <id> dac_arch_diff
#ifndef CONF_DAC_DIFF
#define CONF_DAC_DIFF 0
#endif
// </h>

// <e> Advanced Configuration
// <id> dac_advanced_settings
#ifndef CONF_DAC_ADVANCED_CONFIG
#define CONF_DAC_ADVANCED_CONFIG 0
#endif

// <q> Debug Run
// <i> Indicate whether running when CPU is halted
// <id> adc_arch_dbgrun
#ifndef CONF_DAC_DBGRUN
#define CONF_DAC_DBGRUN 1
#endif

// <h> Channel 0 configuration
// <q> Left Adjusted Data
// <i> Indicate how the data is adjusted in the Data and Data Buffer register
// <id> dac0_arch_leftadj
#ifndef CONF_DAC0_LEFTADJ
#define CONF_DAC0_LEFTADJ 1
#endif

// <o> Current control
// <0=> GCLK_DAC <= 1.2MHz (100kSPS)
// <1=> 1.2MHz < GCLK_DAC <= 6MHz (500kSPS)
// <2=> 6MHz < GCLK_DAC <= 12MHz (1MSPS)
// <i> This defines the current in output buffer according to conversion rate
// <id> dac0_arch_cctrl
#ifndef CONF_DAC0_CCTRL
#define CONF_DAC0_CCTRL 0
#endif

// <q> Run in standby
// <i> Indicates whether the DAC channel will continue running in standby sleep mode or not
// <id> dac0_arch_runstdby
#ifndef CONF_DAC0_RUNSTDBY
#define CONF_DAC0_RUNSTDBY 0
#endif

// <q> Dithering Mode
// <i> Indicate whether dithering mode is enabled
// <id> dac0_arch_ditrher
#ifndef CONF_DAC0_DITHER
#define CONF_DAC0_DITHER 0
#endif

// <o> Refresh period <0x00-0xFF>
// <i> This defines the refresh period. If it is 0, the refresh mode is disabled, else the refresh period is: value * 500us
// <id> dac0_arch_refresh
#ifndef CONF_DAC0_REFRESH
#define CONF_DAC0_REFRESH 2
#endif
// </h>
// <h> Channel 1 configuration
// <q> Left Adjusted Data
// <i> Indicate how the data is adjusted in the Data and Data Buffer register
// <id> dac1_arch_leftadj
#ifndef CONF_DAC1_LEFTADJ
#define CONF_DAC1_LEFTADJ 1
#endif

// <o> Current control
// <0=> GCLK_DAC <= 1.2MHz (100kSPS)
// <1=> 1.2MHz < GCLK_DAC <= 6MHz (500kSPS)
// <2=> 6MHz < GCLK_DAC <= 12MHz (1MSPS)
// <i> This defines the current in output buffer according to conversion rate
// <id> dac1_arch_cctrl
#ifndef CONF_DAC1_CCTRL
#define CONF_DAC1_CCTRL 0
#endif

// <q> Run in standby
// <i> Indicates whether the DAC channel will continue running in standby sleep mode or not
// <id> dac1_arch_runstdby
#ifndef CONF_DAC1_RUNSTDBY
#define CONF_DAC1_RUNSTDBY 0
#endif

// <q> Dithering Mode
// <i> Indicate whether dithering mode is enabled
// <id> dac1_arch_ditrher
#ifndef CONF_DAC1_DITHER
#define CONF_DAC1_DITHER 0
#endif

// <o> Refresh period <0x00-0xFF>
// <i> This defines the refresh period. If it is 0, the refresh mode is disabled, else the refresh period is: value * 500us
// <id> dac1_arch_refresh
#ifndef CONF_DAC1_REFRESH
#define CONF_DAC1_REFRESH 2
#endif
// </h>

// <h> Event configuration
// <o> Inversion of DAC 0 event
// <0=> Detection on rising edge pf the input event
// <1=> Detection on falling edge pf the input event
// <i> This defines the edge detection of the input event
// <id> dac_arch_invei0
#ifndef CONF_DAC_INVEI0
#define CONF_DAC_INVEI0 0
#endif

// <q> Data Buffer of DAC 0 Empty Event Output
// <i> Indicate whether Data Buffer Empty Event is enabled and generated when the Data Buffer register is empty or not
// <id> dac_arch_emptyeo_0
#ifndef CONF_DAC_EMPTYEO0
#define CONF_DAC_EMPTYEO0 0
#endif

// <q> Start Conversion Event Input DAC 0
// <i> Indicate whether Start input event is enabled
// <id> dac_arch_startei_0
#ifndef CONF_DAC_STARTEI0
#define CONF_DAC_STARTEI0 0
#endif
// <o> Inversion of DAC 1 event
// <0=> Detection on rising edge pf the input event
// <1=> Detection on falling edge pf the input event
// <i> This defines the edge detection of the input event
// <id> dac_arch_invei1
#ifndef CONF_DAC_INVEI1
#define CONF_DAC_INVEI1 0
#endif

// <q> Data Buffer of DAC 1 Empty Event Output
// <i> Indicate whether Data Buffer Empty Event is enabled and generated when the Data Buffer register is empty or not
// <id> dac_arch_emptyeo_1
#ifndef CONF_DAC_EMPTYEO1
#define CONF_DAC_EMPTYEO1 0
#endif

// <q> Start Conversion Event Input DAC 1
// <i> Indicate whether Start input event is enabled
// <id> dac_arch_startei_1
#ifndef CONF_DAC_STARTEI1
#define CONF_DAC_STARTEI1 0
#endif

// </h>
// </e>

// <<< end of configuration section >>>

#endif // HPL_DAC_CONFIG_H

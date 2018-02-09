/* Auto-generated config file hpl_dac_config.h */
#ifndef HPL_DAC_CONFIG_H
#define HPL_DAC_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Basic configuration
// <o> Reference Selection
// <0x00=> Internal 1.0v reference
// <0x01=> AVCC
// <0x02=> External reference
// <id> dac_arch_refsel
#ifndef CONF_DAC_REFSEL
#define CONF_DAC_REFSEL 1
#endif
// </h>

// <e> Advanced Configuration
// <id> dac_advanced_settings
#ifndef CONF_DAC_ADVANCED_CONFIG
#define CONF_DAC_ADVANCED_CONFIG 0
#endif

// <q> Run in standby
// <i> Indicates whether the DAC will continue running in standby sleep mode or not
// <id> dac_arch_runstdby
#ifndef CONF_DAC_RUNSTDBY
#define CONF_DAC_RUNSTDBY 0
#endif

// <q> Bypass DATABUF Write Protection
// <i> Indicate whether DATABUF write protection is bypass
// <id> dac_arch_bdwp
#ifndef CONF_DAC_BDWP
#define CONF_DAC_BDWP 0
#endif

// <q> Voltage Pump Disable
// <i> Indicate whether voltage pump is disable or not
// <id> dac_arch_vpd
#ifndef CONF_DAC_VPD
#define CONF_DAC_VPD 0
#endif

// <q> Left Adjusted Data
// <i> Indicate how the data is adjusted in the Data and Data Buffer register
// <id> dac_arch_leftadj
#ifndef CONF_DAC_LEFTADJ
#define CONF_DAC_LEFTADJ 1
#endif

// <q> Internal Output Enable
// <i> Indicate whether internal output is enable or not
// <id> dac_arch_ioen
#ifndef CONF_DAC_IOEN
#define CONF_DAC_IOEN 0
#endif

// <q> External Output Enable
// <i> Indicate whether external output is enable or not
// <id> dac_arch_eoen
#ifndef CONF_DAC_EOEN
#define CONF_DAC_EOEN 1
#endif

// </e>

// <h> Event configuration
// <q> Data Buffer Empty Event Output
// <i> Indicate whether Data Buffer Empty Event is enabled and generated when the Data Buffer register is empty or not
// <id> dac_arch_emptyeo
#ifndef CONF_DAC_EMPTYEO
#define CONF_DAC_EMPTYEO 0
#endif

// <q> Start Conversion on Event Input
// <i> Indicate whether Start Conversion is enabled and data are loaded from the Data Buffer register to the Data register upon event reception or not
// <id> dac_arch_startei
#ifndef CONF_DAC_STARTEI
#define CONF_DAC_STARTEI 0
#endif
// </h>

// <<< end of configuration section >>>

#endif // HPL_DAC_CONFIG_H

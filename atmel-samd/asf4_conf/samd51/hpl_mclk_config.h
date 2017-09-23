/* Auto-generated config file hpl_mclk_config.h */
#ifndef HPL_MCLK_CONFIG_H
#define HPL_MCLK_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

#include <peripheral_clk_config.h>

// <e> System Configuration
// <i> Indicates whether configuration for system is enabled or not
// <id> enable_cpu_clock
#ifndef CONF_SYSTEM_CONFIG
#define CONF_SYSTEM_CONFIG 1
#endif

// <h> Basic settings
// <y> CPU Clock source
// <GCLK_PCHCTRL_GEN_GCLK0_Val"> Generic clock generator 0
// <i> This defines the clock source for the CPU
// <id> cpu_clock_source
#ifndef CONF_CPU_SRC
#define CONF_CPU_SRC GCLK_PCHCTRL_GEN_GCLK0_Val
#endif

// <y> CPU Clock Division Factor
// <MCLK_CPUDIV_DIV_DIV1_Val"> 1
// <MCLK_CPUDIV_DIV_DIV2_Val"> 2
// <MCLK_CPUDIV_DIV_DIV4_Val"> 4
// <MCLK_CPUDIV_DIV_DIV8_Val"> 8
// <MCLK_CPUDIV_DIV_DIV16_Val"> 16
// <MCLK_CPUDIV_DIV_DIV32_Val"> 32
// <MCLK_CPUDIV_DIV_DIV64_Val"> 64
// <MCLK_CPUDIV_DIV_DIV128_Val"> 128
// <i> Prescalar for CPU clock
// <id> cpu_div
#ifndef CONF_MCLK_CPUDIV
#define CONF_MCLK_CPUDIV MCLK_CPUDIV_DIV_DIV1_Val
#endif
// <y> Low Power Clock Division
// <MCLK_LPDIV_LPDIV_DIV1_Val"> Divide by 1
// <MCLK_LPDIV_LPDIV_DIV2_Val"> Divide by 2
// <MCLK_LPDIV_LPDIV_DIV4_Val"> Divide by 4
// <MCLK_LPDIV_LPDIV_DIV8_Val"> Divide by 8
// <MCLK_LPDIV_LPDIV_DIV16_Val"> Divide by 16
// <MCLK_LPDIV_LPDIV_DIV32_Val"> Divide by 32
// <MCLK_LPDIV_LPDIV_DIV64_Val"> Divide by 64
// <MCLK_LPDIV_LPDIV_DIV128_Val"> Divide by 128
// <id> mclk_arch_lpdiv
#ifndef CONF_MCLK_LPDIV
#define CONF_MCLK_LPDIV MCLK_LPDIV_LPDIV_DIV4_Val
#endif

// <y> Backup Clock Division
// <MCLK_BUPDIV_BUPDIV_DIV1_Val"> Divide by 1
// <MCLK_BUPDIV_BUPDIV_DIV2_Val"> Divide by 2
// <MCLK_BUPDIV_BUPDIV_DIV4_Val"> Divide by 4
// <MCLK_BUPDIV_BUPDIV_DIV8_Val"> Divide by 8
// <MCLK_BUPDIV_BUPDIV_DIV16_Val"> Divide by 16
// <MCLK_BUPDIV_BUPDIV_DIV32_Val"> Divide by 32
// <MCLK_BUPDIV_BUPDIV_DIV64_Val"> Divide by 64
// <MCLK_BUPDIV_BUPDIV_DIV128_Val"> Divide by 128
// <id> mclk_arch_bupdiv
#ifndef CONF_MCLK_BUPDIV
#define CONF_MCLK_BUPDIV MCLK_BUPDIV_BUPDIV_DIV8_Val
#endif
// <y> High-Speed Clock Division
// <MCLK_HSDIV_DIV_DIV1_Val"> Divide by 1
// <id> mclk_arch_hsdiv
#ifndef CONF_MCLK_HSDIV
#define CONF_MCLK_HSDIV MCLK_HSDIV_DIV_DIV1_Val
#endif
// </h>

// <h> NVM Settings
// <o> NVM Wait States
// <i> These bits select the number of wait states for a read operation.
// <0=> 0
// <1=> 1
// <2=> 2
// <3=> 3
// <4=> 4
// <5=> 5
// <6=> 6
// <7=> 7
// <8=> 8
// <9=> 9
// <10=> 10
// <11=> 11
// <12=> 12
// <13=> 13
// <14=> 14
// <15=> 15
// <id> nvm_wait_states
#ifndef CONF_NVM_WAIT_STATE
#define CONF_NVM_WAIT_STATE 0
#endif

// </h>

// </e>

// <<< end of configuration section >>>

#endif // HPL_MCLK_CONFIG_H

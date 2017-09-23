/* Auto-generated config file hpl_pm_config.h */
#ifndef HPL_PM_CONFIG_H
#define HPL_PM_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

#include <peripheral_clk_config.h>

// <e> System Configuration
// <i> Indicates whether configuration for system is enabled or not
// <id> enable_cpu_clock
#ifndef CONF_SYSTEM_CONFIG
#define CONF_SYSTEM_CONFIG 1
#endif

// <h> CPU Clock Settings
// <y> CPU Clock source
// <GCLK_CLKCTRL_GEN_GCLK0_Val"> Generic clock generator 0
// <i> This defines the clock source for the CPU
// <id> cpu_clock_source
#ifndef CONF_CPU_SRC
#define CONF_CPU_SRC GCLK_CLKCTRL_GEN_GCLK0_Val
#endif

// <y> CPU clock Prescalar
// <PM_CPUSEL_CPUDIV_DIV1_Val"> 1
// <PM_CPUSEL_CPUDIV_DIV2_Val"> 2
// <PM_CPUSEL_CPUDIV_DIV4_Val"> 4
// <PM_CPUSEL_CPUDIV_DIV8_Val"> 8
// <PM_CPUSEL_CPUDIV_DIV16_Val"> 16
// <PM_CPUSEL_CPUDIV_DIV32_Val"> 32
// <PM_CPUSEL_CPUDIV_DIV64_Val"> 64
// <PM_CPUSEL_CPUDIV_DIV128_Val"> 128
// <i> Prescalar for Main CPU clock
// <id> cpu_div
#ifndef CONF_CPU_DIV
#define CONF_CPU_DIV PM_CPUSEL_CPUDIV_DIV1_Val
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

// <h> APBA Clock Select
// <y> APBA clock prescalar
// <PM_APBASEL_APBADIV_DIV1"> 1
// <PM_APBASEL_APBADIV_DIV2"> 2
// <PM_APBASEL_APBADIV_DIV4"> 4
// <PM_APBASEL_APBADIV_DIV8"> 8
// <PM_APBASEL_APBADIV_DIV16"> 16
// <PM_APBASEL_APBADIV_DIV32"> 32
// <PM_APBASEL_APBADIV_DIV64"> 64
// <PM_APBASEL_APBADIV_DIV128"> 128
// <i> APBA clock prescalar
// <id> apba_div
#ifndef CONF_APBA_DIV
#define CONF_APBA_DIV PM_APBASEL_APBADIV_DIV1
#endif
// </h>

#if CONF_APBA_DIV < CONF_CPU_DIV
#warning APBA DIV cannot less than CPU DIV
#endif

// <h> APBB Clock Select
// <y> APBB clock prescalar
// <PM_APBBSEL_APBBDIV_DIV1"> 1
// <PM_APBBSEL_APBBDIV_DIV2"> 2
// <PM_APBBSEL_APBBDIV_DIV4"> 4
// <PM_APBBSEL_APBBDIV_DIV8"> 8
// <PM_APBBSEL_APBBDIV_DIV16"> 16
// <PM_APBBSEL_APBBDIV_DIV32"> 32
// <PM_APBBSEL_APBBDIV_DIV64"> 64
// <PM_APBBSEL_APBBDIV_DIV128"> 128
// <i> APBB clock prescalar
// <id> apbb_div
#ifndef CONF_APBB_DIV
#define CONF_APBB_DIV PM_APBBSEL_APBBDIV_DIV1
#endif
// </h>

#if CONF_APBB_DIV < CONF_CPU_DIV
#warning APBB DIV cannot less than CPU DIV
#endif

// <h> APBC Clock Select
// <y> APBC clock prescalar
// <PM_APBCSEL_APBCDIV_DIV1"> 1
// <PM_APBCSEL_APBCDIV_DIV2"> 2
// <PM_APBCSEL_APBCDIV_DIV4"> 4
// <PM_APBCSEL_APBCDIV_DIV8"> 8
// <PM_APBCSEL_APBCDIV_DIV16"> 16
// <PM_APBCSEL_APBCDIV_DIV32"> 32
// <PM_APBCSEL_APBCDIV_DIV64"> 64
// <PM_APBCSEL_APBCDIV_DIV128"> 128
// <i> APBC clock prescalar
// <id> apbc_div
#ifndef CONF_APBC_DIV
#define CONF_APBC_DIV PM_APBCSEL_APBCDIV_DIV1
#endif
// </h>

#if CONF_APBC_DIV < CONF_CPU_DIV
#warning APBC DIV cannot less than CPU DIV
#endif

// </e>

// <<< end of configuration section >>>

#endif // HPL_PM_CONFIG_H

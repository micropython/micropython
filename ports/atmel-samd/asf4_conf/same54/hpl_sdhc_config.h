/* Auto-generated config file hpl_sdhc_config.h */
#ifndef HPL_SDHC_CONFIG_H
#define HPL_SDHC_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

#include "peripheral_clk_config.h"

#ifndef CONF_BASE_FREQUENCY
#define CONF_BASE_FREQUENCY CONF_SDHC0_FREQUENCY
#endif

// <o> Clock Generator Select
// <0=> Divided Clock mode
// <1=> Programmable Clock mode
// <i> This defines the clock generator mode in the SDCLK Frequency Select field
// <id> sdhc_clk_gsel
#ifndef CONF_SDHC0_CLK_GEN_SEL
#define CONF_SDHC0_CLK_GEN_SEL 0
#endif

// <<< end of configuration section >>>

#endif // HPL_SDHC_CONFIG_H

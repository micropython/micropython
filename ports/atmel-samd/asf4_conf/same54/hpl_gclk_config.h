// CircuitPython SAMD51 clock tree:
// DFLL48M (with USBCRM on to sync with external USB ref) -> GCLK1, GCLK5, GCLK6
//   GCLK1 (48MHz) -> 48 MHz peripherals
//   GCLK5 (48 MHz divided down to 2 MHz) -> DPLL0
//     DPLL0 (multiplied up to 120 MHz) -> GCLK0, GCLK4 (output for monitoring)
//   GCLK6 (48 MHz divided down to 12 MHz) -> DAC

// We'd like to use XOSC32K as a ref for DFLL48M on boards with a 32kHz crystal,
// but haven't figured that out yet.

// Used in hpl/core/hpl_init.c to define which clocks should be initialized first.
// Not clear why all these need to be specified, but it doesn't work properly otherwise.

// #define CIRCUITPY_GCLK_INIT_1ST (1 << 0 | 1 << 1 | 1 << 3 | 1 <<5)
#define CIRCUITPY_GCLK_INIT_1ST 0xffff

/* Auto-generated config file hpl_gclk_config.h */
#ifndef HPL_GCLK_CONFIG_H
#define HPL_GCLK_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <e> Generic clock generator 0 configuration
// <i> Indicates whether generic clock 0 configuration is enabled or not
// <id> enable_gclk_gen_0
#ifndef CONF_GCLK_GENERATOR_0_CONFIG
#define CONF_GCLK_GENERATOR_0_CONFIG 1
#endif

// <h> Generic Clock Generator Control
// <y> Generic clock generator 0 source// <GCLK_GENCTRL_SRC_XOSC0"> External Crystal Oscillator 8-48MHz (XOSC0)
// <GCLK_GENCTRL_SRC_XOSC1"> External Crystal Oscillator 8-48MHz (XOSC1)
// <GCLK_GENCTRL_SRC_GCLKIN"> Generic clock generator input pad
// <GCLK_GENCTRL_SRC_GCLKGEN1"> Generic clock generator 1
// <GCLK_GENCTRL_SRC_OSCULP32K"> 32kHz Ultra Low Power Internal Oscillator (OSCULP32K)
// <GCLK_GENCTRL_SRC_XOSC32K"> 32kHz External Crystal Oscillator (XOSC32K)
// <GCLK_GENCTRL_SRC_DFLL"> Digital Frequency Locked Loop (DFLL48M)
// <GCLK_GENCTRL_SRC_DPLL0"> Digital Phase Locked Loop (DPLL0)
// <GCLK_GENCTRL_SRC_DPLL1"> Digital Phase Locked Loop (DPLL1)
// <i> This defines the clock source for generic clock generator 0
// <id> gclk_gen_0_oscillator
#ifndef CONF_GCLK_GEN_0_SOURCE
#define CONF_GCLK_GEN_0_SOURCE GCLK_GENCTRL_SRC_DPLL0
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> gclk_arch_gen_0_runstdby
#ifndef CONF_GCLK_GEN_0_RUNSTDBY
#define CONF_GCLK_GEN_0_RUNSTDBY 0
#endif

// <q> Divide Selection
// <i> Indicates whether Divide Selection is enabled or not
// <id> gclk_gen_0_div_sel
#ifndef CONF_GCLK_GEN_0_DIVSEL
#define CONF_GCLK_GEN_0_DIVSEL 0
#endif

// <q> Output Enable
// <i> Indicates whether Output Enable is enabled or not
// <id> gclk_arch_gen_0_oe
#ifndef CONF_GCLK_GEN_0_OE
#define CONF_GCLK_GEN_0_OE 1
#endif

// <q> Output Off Value
// <i> Indicates whether Output Off Value is enabled or not
// <id> gclk_arch_gen_0_oov
#ifndef CONF_GCLK_GEN_0_OOV
#define CONF_GCLK_GEN_0_OOV 0
#endif

// <q> Improve Duty Cycle
// <i> Indicates whether Improve Duty Cycle is enabled or not
// <id> gclk_arch_gen_0_idc
#ifndef CONF_GCLK_GEN_0_IDC
#define CONF_GCLK_GEN_0_IDC 0
#endif

// <q> Generic Clock Generator Enable
// <i> Indicates whether Generic Clock Generator Enable is enabled or not
// <id> gclk_arch_gen_0_enable
#ifndef CONF_GCLK_GEN_0_GENEN
#define CONF_GCLK_GEN_0_GENEN 1
#endif
// </h>

// <h> Generic Clock Generator Division
// <o> Generic clock generator 0 division <0x0000-0xFFFF>
// <id> gclk_gen_0_div
#ifndef CONF_GCLK_GEN_0_DIV
#define CONF_GCLK_GEN_0_DIV 1
#endif
// </h>
// </e>

// <e> Generic clock generator 1 configuration
// <i> Indicates whether generic clock 1 configuration is enabled or not
// <id> enable_gclk_gen_1
#ifndef CONF_GCLK_GENERATOR_1_CONFIG
#define CONF_GCLK_GENERATOR_1_CONFIG 1
#endif

// <h> Generic Clock Generator Control
// <y> Generic clock generator 1 source// <GCLK_GENCTRL_SRC_XOSC0"> External Crystal Oscillator 8-48MHz (XOSC0)
// <GCLK_GENCTRL_SRC_XOSC1"> External Crystal Oscillator 8-48MHz (XOSC1)
// <GCLK_GENCTRL_SRC_GCLKIN"> Generic clock generator input pad
// <GCLK_GENCTRL_SRC_OSCULP32K"> 32kHz Ultra Low Power Internal Oscillator (OSCULP32K)
// <GCLK_GENCTRL_SRC_XOSC32K"> 32kHz External Crystal Oscillator (XOSC32K)
// <GCLK_GENCTRL_SRC_DFLL"> Digital Frequency Locked Loop (DFLL48M)
// <GCLK_GENCTRL_SRC_DPLL0"> Digital Phase Locked Loop (DPLL0)
// <GCLK_GENCTRL_SRC_DPLL1"> Digital Phase Locked Loop (DPLL1)
// <i> This defines the clock source for generic clock generator 1
// <id> gclk_gen_1_oscillator
#ifndef CONF_GCLK_GEN_1_SOURCE
#define CONF_GCLK_GEN_1_SOURCE GCLK_GENCTRL_SRC_DFLL
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> gclk_arch_gen_1_runstdby
#ifndef CONF_GCLK_GEN_1_RUNSTDBY
#define CONF_GCLK_GEN_1_RUNSTDBY 0
#endif

// <q> Divide Selection
// <i> Indicates whether Divide Selection is enabled or not
// <id> gclk_gen_1_div_sel
#ifndef CONF_GCLK_GEN_1_DIVSEL
#define CONF_GCLK_GEN_1_DIVSEL 0
#endif

// <q> Output Enable
// <i> Indicates whether Output Enable is enabled or not
// <id> gclk_arch_gen_1_oe
#ifndef CONF_GCLK_GEN_1_OE
#define CONF_GCLK_GEN_1_OE 1
#endif

// <q> Output Off Value
// <i> Indicates whether Output Off Value is enabled or not
// <id> gclk_arch_gen_1_oov
#ifndef CONF_GCLK_GEN_1_OOV
#define CONF_GCLK_GEN_1_OOV 0
#endif

// <q> Improve Duty Cycle
// <i> Indicates whether Improve Duty Cycle is enabled or not
// <id> gclk_arch_gen_1_idc
#ifndef CONF_GCLK_GEN_1_IDC
#define CONF_GCLK_GEN_1_IDC 0
#endif

// <q> Generic Clock Generator Enable
// <i> Indicates whether Generic Clock Generator Enable is enabled or not
// <id> gclk_arch_gen_1_enable
#ifndef CONF_GCLK_GEN_1_GENEN
#define CONF_GCLK_GEN_1_GENEN 1
#endif
// </h>

// <h> Generic Clock Generator Division
// <o> Generic clock generator 1 division <0x0000-0xFFFF>
// <id> gclk_gen_1_div
#ifndef CONF_GCLK_GEN_1_DIV
#define CONF_GCLK_GEN_1_DIV 1
#endif
// </h>
// </e>

// <e> Generic clock generator 2 configuration
// <i> Indicates whether generic clock 2 configuration is enabled or not
// <id> enable_gclk_gen_2
#ifndef CONF_GCLK_GENERATOR_2_CONFIG
#define CONF_GCLK_GENERATOR_2_CONFIG 1
#endif

// <h> Generic Clock Generator Control
// <y> Generic clock generator 2 source// <GCLK_GENCTRL_SRC_XOSC0"> External Crystal Oscillator 8-48MHz (XOSC0)
// <GCLK_GENCTRL_SRC_XOSC1"> External Crystal Oscillator 8-48MHz (XOSC1)
// <GCLK_GENCTRL_SRC_GCLKIN"> Generic clock generator input pad
// <GCLK_GENCTRL_SRC_GCLKGEN1"> Generic clock generator 1
// <GCLK_GENCTRL_SRC_OSCULP32K"> 32kHz Ultra Low Power Internal Oscillator (OSCULP32K)
// <GCLK_GENCTRL_SRC_XOSC32K"> 32kHz External Crystal Oscillator (XOSC32K)
// <GCLK_GENCTRL_SRC_DFLL"> Digital Frequency Locked Loop (DFLL48M)
// <GCLK_GENCTRL_SRC_DPLL0"> Digital Phase Locked Loop (DPLL0)
// <GCLK_GENCTRL_SRC_DPLL1"> Digital Phase Locked Loop (DPLL1)
// <i> This defines the clock source for generic clock generator 2
// <id> gclk_gen_2_oscillator
#ifndef CONF_GCLK_GEN_2_SOURCE
#define CONF_GCLK_GEN_2_SOURCE GCLK_GENCTRL_SRC_OSCULP32K
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> gclk_arch_gen_2_runstdby
#ifndef CONF_GCLK_GEN_2_RUNSTDBY
#define CONF_GCLK_GEN_2_RUNSTDBY 0
#endif

// <q> Divide Selection
// <i> Indicates whether Divide Selection is enabled or not
// <id> gclk_gen_2_div_sel
#ifndef CONF_GCLK_GEN_2_DIVSEL
#define CONF_GCLK_GEN_2_DIVSEL 1
#endif

// <q> Output Enable
// <i> Indicates whether Output Enable is enabled or not
// <id> gclk_arch_gen_2_oe
#ifndef CONF_GCLK_GEN_2_OE
#define CONF_GCLK_GEN_2_OE 0
#endif

// <q> Output Off Value
// <i> Indicates whether Output Off Value is enabled or not
// <id> gclk_arch_gen_2_oov
#ifndef CONF_GCLK_GEN_2_OOV
#define CONF_GCLK_GEN_2_OOV 0
#endif

// <q> Improve Duty Cycle
// <i> Indicates whether Improve Duty Cycle is enabled or not
// <id> gclk_arch_gen_2_idc
#ifndef CONF_GCLK_GEN_2_IDC
#define CONF_GCLK_GEN_2_IDC 0
#endif

// <q> Generic Clock Generator Enable
// <i> Indicates whether Generic Clock Generator Enable is enabled or not
// <id> gclk_arch_gen_2_enable
#ifndef CONF_GCLK_GEN_2_GENEN
#define CONF_GCLK_GEN_2_GENEN 1
#endif
// </h>

// <h> Generic Clock Generator Division
// <o> Generic clock generator 2 division <0x0000-0xFFFF>
// <id> gclk_gen_2_div
#ifndef CONF_GCLK_GEN_2_DIV
#define CONF_GCLK_GEN_2_DIV 4
#endif
// </h>
// </e>

// <e> Generic clock generator 3 configuration
// <i> Indicates whether generic clock 3 configuration is enabled or not
// <id> enable_gclk_gen_3
#ifndef CONF_GCLK_GENERATOR_3_CONFIG
#define CONF_GCLK_GENERATOR_3_CONFIG 0
#endif

// <h> Generic Clock Generator Control
// <y> Generic clock generator 3 source// <GCLK_GENCTRL_SRC_XOSC0"> External Crystal Oscillator 8-48MHz (XOSC0)
// <GCLK_GENCTRL_SRC_XOSC1"> External Crystal Oscillator 8-48MHz (XOSC1)
// <GCLK_GENCTRL_SRC_GCLKIN"> Generic clock generator input pad
// <GCLK_GENCTRL_SRC_GCLKGEN1"> Generic clock generator 1
// <GCLK_GENCTRL_SRC_OSCULP32K"> 32kHz Ultra Low Power Internal Oscillator (OSCULP32K)
// <GCLK_GENCTRL_SRC_XOSC32K"> 32kHz External Crystal Oscillator (XOSC32K)
// <GCLK_GENCTRL_SRC_DFLL"> Digital Frequency Locked Loop (DFLL48M)
// <GCLK_GENCTRL_SRC_DPLL0"> Digital Phase Locked Loop (DPLL0)
// <GCLK_GENCTRL_SRC_DPLL1"> Digital Phase Locked Loop (DPLL1)
// <i> This defines the clock source for generic clock generator 3
// <id> gclk_gen_3_oscillator
#ifndef CONF_GCLK_GEN_3_SOURCE
#define CONF_GCLK_GEN_3_SOURCE GCLK_GENCTRL_SRC_XOSC32K
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> gclk_arch_gen_3_runstdby
#ifndef CONF_GCLK_GEN_3_RUNSTDBY
#define CONF_GCLK_GEN_3_RUNSTDBY 0
#endif

// <q> Divide Selection
// <i> Indicates whether Divide Selection is enabled or not
// <id> gclk_gen_3_div_sel
#ifndef CONF_GCLK_GEN_3_DIVSEL
#define CONF_GCLK_GEN_3_DIVSEL 0
#endif

// <q> Output Enable
// <i> Indicates whether Output Enable is enabled or not
// <id> gclk_arch_gen_3_oe
#ifndef CONF_GCLK_GEN_3_OE
#define CONF_GCLK_GEN_3_OE 0
#endif

// <q> Output Off Value
// <i> Indicates whether Output Off Value is enabled or not
// <id> gclk_arch_gen_3_oov
#ifndef CONF_GCLK_GEN_3_OOV
#define CONF_GCLK_GEN_3_OOV 0
#endif

// <q> Improve Duty Cycle
// <i> Indicates whether Improve Duty Cycle is enabled or not
// <id> gclk_arch_gen_3_idc
#ifndef CONF_GCLK_GEN_3_IDC
#define CONF_GCLK_GEN_3_IDC 0
#endif

// <q> Generic Clock Generator Enable
// <i> Indicates whether Generic Clock Generator Enable is enabled or not
// <id> gclk_arch_gen_3_enable
#ifndef CONF_GCLK_GEN_3_GENEN
#define CONF_GCLK_GEN_3_GENEN 0
#endif
// </h>

// <h> Generic Clock Generator Division
// <o> Generic clock generator 3 division <0x0000-0xFFFF>
// <id> gclk_gen_3_div
#ifndef CONF_GCLK_GEN_3_DIV
#define CONF_GCLK_GEN_3_DIV 1
#endif
// </h>
// </e>

// <e> Generic clock generator 4 configuration
// <i> Indicates whether generic clock 4 configuration is enabled or not
// <id> enable_gclk_gen_4
#ifndef CONF_GCLK_GENERATOR_4_CONFIG
#define CONF_GCLK_GENERATOR_4_CONFIG 1
#endif

// <h> Generic Clock Generator Control
// <y> Generic clock generator 4 source// <GCLK_GENCTRL_SRC_XOSC0"> External Crystal Oscillator 8-48MHz (XOSC0)
// <GCLK_GENCTRL_SRC_XOSC1"> External Crystal Oscillator 8-48MHz (XOSC1)
// <GCLK_GENCTRL_SRC_GCLKIN"> Generic clock generator input pad
// <GCLK_GENCTRL_SRC_GCLKGEN1"> Generic clock generator 1
// <GCLK_GENCTRL_SRC_OSCULP32K"> 32kHz Ultra Low Power Internal Oscillator (OSCULP32K)
// <GCLK_GENCTRL_SRC_XOSC32K"> 32kHz External Crystal Oscillator (XOSC32K)
// <GCLK_GENCTRL_SRC_DFLL"> Digital Frequency Locked Loop (DFLL48M)
// <GCLK_GENCTRL_SRC_DPLL0"> Digital Phase Locked Loop (DPLL0)
// <GCLK_GENCTRL_SRC_DPLL1"> Digital Phase Locked Loop (DPLL1)
// <i> This defines the clock source for generic clock generator 4
// <id> gclk_gen_4_oscillator
#ifndef CONF_GCLK_GEN_4_SOURCE
#define CONF_GCLK_GEN_4_SOURCE GCLK_GENCTRL_SRC_DPLL0
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> gclk_arch_gen_4_runstdby
#ifndef CONF_GCLK_GEN_4_RUNSTDBY
#define CONF_GCLK_GEN_4_RUNSTDBY 0
#endif

// <q> Divide Selection
// <i> Indicates whether Divide Selection is enabled or not
// <id> gclk_gen_4_div_sel
#ifndef CONF_GCLK_GEN_4_DIVSEL
#define CONF_GCLK_GEN_4_DIVSEL 0
#endif

// <q> Output Enable
// <i> Indicates whether Output Enable is enabled or not
// <id> gclk_arch_gen_4_oe
#ifndef CONF_GCLK_GEN_4_OE
#define CONF_GCLK_GEN_4_OE 1
#endif

// <q> Output Off Value
// <i> Indicates whether Output Off Value is enabled or not
// <id> gclk_arch_gen_4_oov
#ifndef CONF_GCLK_GEN_4_OOV
#define CONF_GCLK_GEN_4_OOV 0
#endif

// <q> Improve Duty Cycle
// <i> Indicates whether Improve Duty Cycle is enabled or not
// <id> gclk_arch_gen_4_idc
#ifndef CONF_GCLK_GEN_4_IDC
#define CONF_GCLK_GEN_4_IDC 0
#endif

// <q> Generic Clock Generator Enable
// <i> Indicates whether Generic Clock Generator Enable is enabled or not
// <id> gclk_arch_gen_4_enable
#ifndef CONF_GCLK_GEN_4_GENEN
#define CONF_GCLK_GEN_4_GENEN 1
#endif
// </h>

// <h> Generic Clock Generator Division
// <o> Generic clock generator 4 division <0x0000-0xFFFF>
// <id> gclk_gen_4_div
#ifndef CONF_GCLK_GEN_4_DIV
#define CONF_GCLK_GEN_4_DIV 1
#endif
// </h>
// </e>

// <e> Generic clock generator 5 configuration
// <i> Indicates whether generic clock 5 configuration is enabled or not
// <id> enable_gclk_gen_5
#ifndef CONF_GCLK_GENERATOR_5_CONFIG
#define CONF_GCLK_GENERATOR_5_CONFIG 1
#endif

// <h> Generic Clock Generator Control
// <y> Generic clock generator 5 source// <GCLK_GENCTRL_SRC_XOSC0"> External Crystal Oscillator 8-48MHz (XOSC0)
// <GCLK_GENCTRL_SRC_XOSC1"> External Crystal Oscillator 8-48MHz (XOSC1)
// <GCLK_GENCTRL_SRC_GCLKIN"> Generic clock generator input pad
// <GCLK_GENCTRL_SRC_GCLKGEN1"> Generic clock generator 1
// <GCLK_GENCTRL_SRC_OSCULP32K"> 32kHz Ultra Low Power Internal Oscillator (OSCULP32K)
// <GCLK_GENCTRL_SRC_XOSC32K"> 32kHz External Crystal Oscillator (XOSC32K)
// <GCLK_GENCTRL_SRC_DFLL"> Digital Frequency Locked Loop (DFLL48M)
// <GCLK_GENCTRL_SRC_DPLL0"> Digital Phase Locked Loop (DPLL0)
// <GCLK_GENCTRL_SRC_DPLL1"> Digital Phase Locked Loop (DPLL1)
// <i> This defines the clock source for generic clock generator 5
// <id> gclk_gen_5_oscillator
#ifndef CONF_GCLK_GEN_5_SOURCE
#define CONF_GCLK_GEN_5_SOURCE GCLK_GENCTRL_SRC_DFLL
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> gclk_arch_gen_5_runstdby
#ifndef CONF_GCLK_GEN_5_RUNSTDBY
#define CONF_GCLK_GEN_5_RUNSTDBY 0
#endif

// <q> Divide Selection
// <i> Indicates whether Divide Selection is enabled or not
// <id> gclk_gen_5_div_sel
#ifndef CONF_GCLK_GEN_5_DIVSEL
#define CONF_GCLK_GEN_5_DIVSEL 0
#endif

// <q> Output Enable
// <i> Indicates whether Output Enable is enabled or not
// <id> gclk_arch_gen_5_oe
#ifndef CONF_GCLK_GEN_5_OE
#define CONF_GCLK_GEN_5_OE 1
#endif

// <q> Output Off Value
// <i> Indicates whether Output Off Value is enabled or not
// <id> gclk_arch_gen_5_oov
#ifndef CONF_GCLK_GEN_5_OOV
#define CONF_GCLK_GEN_5_OOV 0
#endif

// <q> Improve Duty Cycle
// <i> Indicates whether Improve Duty Cycle is enabled or not
// <id> gclk_arch_gen_5_idc
#ifndef CONF_GCLK_GEN_5_IDC
#define CONF_GCLK_GEN_5_IDC 0
#endif

// <q> Generic Clock Generator Enable
// <i> Indicates whether Generic Clock Generator Enable is enabled or not
// <id> gclk_arch_gen_5_enable
#ifndef CONF_GCLK_GEN_5_GENEN
#define CONF_GCLK_GEN_5_GENEN 1
#endif
// </h>

// <h> Generic Clock Generator Division
// <o> Generic clock generator 5 division <0x0000-0xFFFF>
// <id> gclk_gen_5_div
#ifndef CONF_GCLK_GEN_5_DIV
#define CONF_GCLK_GEN_5_DIV 24
#endif
// </h>
// </e>

// <e> Generic clock generator 6 configuration
// <i> Indicates whether generic clock 6 configuration is enabled or not
// <id> enable_gclk_gen_6
#ifndef CONF_GCLK_GENERATOR_6_CONFIG
#define CONF_GCLK_GENERATOR_6_CONFIG 1
#endif

// <h> Generic Clock Generator Control
// <y> Generic clock generator 6 source// <GCLK_GENCTRL_SRC_XOSC0"> External Crystal Oscillator 8-48MHz (XOSC0)
// <GCLK_GENCTRL_SRC_XOSC1"> External Crystal Oscillator 8-48MHz (XOSC1)
// <GCLK_GENCTRL_SRC_GCLKIN"> Generic clock generator input pad
// <GCLK_GENCTRL_SRC_GCLKGEN1"> Generic clock generator 1
// <GCLK_GENCTRL_SRC_OSCULP32K"> 32kHz Ultra Low Power Internal Oscillator (OSCULP32K)
// <GCLK_GENCTRL_SRC_XOSC32K"> 32kHz External Crystal Oscillator (XOSC32K)
// <GCLK_GENCTRL_SRC_DFLL"> Digital Frequency Locked Loop (DFLL48M)
// <GCLK_GENCTRL_SRC_DPLL0"> Digital Phase Locked Loop (DPLL0)
// <GCLK_GENCTRL_SRC_DPLL1"> Digital Phase Locked Loop (DPLL1)
// <i> This defines the clock source for generic clock generator 6
// <id> gclk_gen_6_oscillator
#ifndef CONF_GCLK_GEN_6_SOURCE
#define CONF_GCLK_GEN_6_SOURCE GCLK_GENCTRL_SRC_DFLL
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> gclk_arch_gen_6_runstdby
#ifndef CONF_GCLK_GEN_6_RUNSTDBY
#define CONF_GCLK_GEN_6_RUNSTDBY 0
#endif

// <q> Divide Selection
// <i> Indicates whether Divide Selection is enabled or not
// <id> gclk_gen_6_div_sel
#ifndef CONF_GCLK_GEN_6_DIVSEL
#define CONF_GCLK_GEN_6_DIVSEL 0
#endif

// <q> Output Enable
// <i> Indicates whether Output Enable is enabled or not
// <id> gclk_arch_gen_6_oe
#ifndef CONF_GCLK_GEN_6_OE
#define CONF_GCLK_GEN_6_OE 0
#endif

// <q> Output Off Value
// <i> Indicates whether Output Off Value is enabled or not
// <id> gclk_arch_gen_6_oov
#ifndef CONF_GCLK_GEN_6_OOV
#define CONF_GCLK_GEN_6_OOV 0
#endif

// <q> Improve Duty Cycle
// <i> Indicates whether Improve Duty Cycle is enabled or not
// <id> gclk_arch_gen_6_idc
#ifndef CONF_GCLK_GEN_6_IDC
#define CONF_GCLK_GEN_6_IDC 1
#endif

// <q> Generic Clock Generator Enable
// <i> Indicates whether Generic Clock Generator Enable is enabled or not
// <id> gclk_arch_gen_6_enable
#ifndef CONF_GCLK_GEN_6_GENEN
#define CONF_GCLK_GEN_6_GENEN 1
#endif
// </h>

// <h> Generic Clock Generator Division
// <o> Generic clock generator 6 division <0x0000-0xFFFF>
// <id> gclk_gen_6_div
#ifndef CONF_GCLK_GEN_6_DIV
#define CONF_GCLK_GEN_6_DIV 4
#endif
// </h>
// </e>

// <e> Generic clock generator 7 configuration
// <i> Indicates whether generic clock 7 configuration is enabled or not
// <id> enable_gclk_gen_7
#ifndef CONF_GCLK_GENERATOR_7_CONFIG
#define CONF_GCLK_GENERATOR_7_CONFIG 0
#endif

// <h> Generic Clock Generator Control
// <y> Generic clock generator 7 source// <GCLK_GENCTRL_SRC_XOSC0"> External Crystal Oscillator 8-48MHz (XOSC0)
// <GCLK_GENCTRL_SRC_XOSC1"> External Crystal Oscillator 8-48MHz (XOSC1)
// <GCLK_GENCTRL_SRC_GCLKIN"> Generic clock generator input pad
// <GCLK_GENCTRL_SRC_GCLKGEN1"> Generic clock generator 1
// <GCLK_GENCTRL_SRC_OSCULP32K"> 32kHz Ultra Low Power Internal Oscillator (OSCULP32K)
// <GCLK_GENCTRL_SRC_XOSC32K"> 32kHz External Crystal Oscillator (XOSC32K)
// <GCLK_GENCTRL_SRC_DFLL"> Digital Frequency Locked Loop (DFLL48M)
// <GCLK_GENCTRL_SRC_DPLL0"> Digital Phase Locked Loop (DPLL0)
// <GCLK_GENCTRL_SRC_DPLL1"> Digital Phase Locked Loop (DPLL1)
// <i> This defines the clock source for generic clock generator 7
// <id> gclk_gen_7_oscillator
#ifndef CONF_GCLK_GEN_7_SOURCE
#define CONF_GCLK_GEN_7_SOURCE GCLK_GENCTRL_SRC_XOSC1
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> gclk_arch_gen_7_runstdby
#ifndef CONF_GCLK_GEN_7_RUNSTDBY
#define CONF_GCLK_GEN_7_RUNSTDBY 0
#endif

// <q> Divide Selection
// <i> Indicates whether Divide Selection is enabled or not
// <id> gclk_gen_7_div_sel
#ifndef CONF_GCLK_GEN_7_DIVSEL
#define CONF_GCLK_GEN_7_DIVSEL 0
#endif

// <q> Output Enable
// <i> Indicates whether Output Enable is enabled or not
// <id> gclk_arch_gen_7_oe
#ifndef CONF_GCLK_GEN_7_OE
#define CONF_GCLK_GEN_7_OE 0
#endif

// <q> Output Off Value
// <i> Indicates whether Output Off Value is enabled or not
// <id> gclk_arch_gen_7_oov
#ifndef CONF_GCLK_GEN_7_OOV
#define CONF_GCLK_GEN_7_OOV 0
#endif

// <q> Improve Duty Cycle
// <i> Indicates whether Improve Duty Cycle is enabled or not
// <id> gclk_arch_gen_7_idc
#ifndef CONF_GCLK_GEN_7_IDC
#define CONF_GCLK_GEN_7_IDC 0
#endif

// <q> Generic Clock Generator Enable
// <i> Indicates whether Generic Clock Generator Enable is enabled or not
// <id> gclk_arch_gen_7_enable
#ifndef CONF_GCLK_GEN_7_GENEN
#define CONF_GCLK_GEN_7_GENEN 0
#endif
// </h>

// <h> Generic Clock Generator Division
// <o> Generic clock generator 7 division <0x0000-0xFFFF>
// <id> gclk_gen_7_div
#ifndef CONF_GCLK_GEN_7_DIV
#define CONF_GCLK_GEN_7_DIV 1
#endif
// </h>
// </e>

// <e> Generic clock generator 8 configuration
// <i> Indicates whether generic clock 8 configuration is enabled or not
// <id> enable_gclk_gen_8
#ifndef CONF_GCLK_GENERATOR_8_CONFIG
#define CONF_GCLK_GENERATOR_8_CONFIG 0
#endif

// <h> Generic Clock Generator Control
// <y> Generic clock generator 8 source// <GCLK_GENCTRL_SRC_XOSC0"> External Crystal Oscillator 8-48MHz (XOSC0)
// <GCLK_GENCTRL_SRC_XOSC1"> External Crystal Oscillator 8-48MHz (XOSC1)
// <GCLK_GENCTRL_SRC_GCLKIN"> Generic clock generator input pad
// <GCLK_GENCTRL_SRC_GCLKGEN1"> Generic clock generator 1
// <GCLK_GENCTRL_SRC_OSCULP32K"> 32kHz Ultra Low Power Internal Oscillator (OSCULP32K)
// <GCLK_GENCTRL_SRC_XOSC32K"> 32kHz External Crystal Oscillator (XOSC32K)
// <GCLK_GENCTRL_SRC_DFLL"> Digital Frequency Locked Loop (DFLL48M)
// <GCLK_GENCTRL_SRC_DPLL0"> Digital Phase Locked Loop (DPLL0)
// <GCLK_GENCTRL_SRC_DPLL1"> Digital Phase Locked Loop (DPLL1)
// <i> This defines the clock source for generic clock generator 8
// <id> gclk_gen_8_oscillator
#ifndef CONF_GCLK_GEN_8_SOURCE
#define CONF_GCLK_GEN_8_SOURCE GCLK_GENCTRL_SRC_XOSC1
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> gclk_arch_gen_8_runstdby
#ifndef CONF_GCLK_GEN_8_RUNSTDBY
#define CONF_GCLK_GEN_8_RUNSTDBY 0
#endif

// <q> Divide Selection
// <i> Indicates whether Divide Selection is enabled or not
// <id> gclk_gen_8_div_sel
#ifndef CONF_GCLK_GEN_8_DIVSEL
#define CONF_GCLK_GEN_8_DIVSEL 0
#endif

// <q> Output Enable
// <i> Indicates whether Output Enable is enabled or not
// <id> gclk_arch_gen_8_oe
#ifndef CONF_GCLK_GEN_8_OE
#define CONF_GCLK_GEN_8_OE 0
#endif

// <q> Output Off Value
// <i> Indicates whether Output Off Value is enabled or not
// <id> gclk_arch_gen_8_oov
#ifndef CONF_GCLK_GEN_8_OOV
#define CONF_GCLK_GEN_8_OOV 0
#endif

// <q> Improve Duty Cycle
// <i> Indicates whether Improve Duty Cycle is enabled or not
// <id> gclk_arch_gen_8_idc
#ifndef CONF_GCLK_GEN_8_IDC
#define CONF_GCLK_GEN_8_IDC 0
#endif

// <q> Generic Clock Generator Enable
// <i> Indicates whether Generic Clock Generator Enable is enabled or not
// <id> gclk_arch_gen_8_enable
#ifndef CONF_GCLK_GEN_8_GENEN
#define CONF_GCLK_GEN_8_GENEN 0
#endif
// </h>

// <h> Generic Clock Generator Division
// <o> Generic clock generator 8 division <0x0000-0xFFFF>
// <id> gclk_gen_8_div
#ifndef CONF_GCLK_GEN_8_DIV
#define CONF_GCLK_GEN_8_DIV 1
#endif
// </h>
// </e>

// <e> Generic clock generator 9 configuration
// <i> Indicates whether generic clock 9 configuration is enabled or not
// <id> enable_gclk_gen_9
#ifndef CONF_GCLK_GENERATOR_9_CONFIG
#define CONF_GCLK_GENERATOR_9_CONFIG 0
#endif

// <h> Generic Clock Generator Control
// <y> Generic clock generator 9 source// <GCLK_GENCTRL_SRC_XOSC0"> External Crystal Oscillator 8-48MHz (XOSC0)
// <GCLK_GENCTRL_SRC_XOSC1"> External Crystal Oscillator 8-48MHz (XOSC1)
// <GCLK_GENCTRL_SRC_GCLKIN"> Generic clock generator input pad
// <GCLK_GENCTRL_SRC_GCLKGEN1"> Generic clock generator 1
// <GCLK_GENCTRL_SRC_OSCULP32K"> 32kHz Ultra Low Power Internal Oscillator (OSCULP32K)
// <GCLK_GENCTRL_SRC_XOSC32K"> 32kHz External Crystal Oscillator (XOSC32K)
// <GCLK_GENCTRL_SRC_DFLL"> Digital Frequency Locked Loop (DFLL48M)
// <GCLK_GENCTRL_SRC_DPLL0"> Digital Phase Locked Loop (DPLL0)
// <GCLK_GENCTRL_SRC_DPLL1"> Digital Phase Locked Loop (DPLL1)
// <i> This defines the clock source for generic clock generator 9
// <id> gclk_gen_9_oscillator
#ifndef CONF_GCLK_GEN_9_SOURCE
#define CONF_GCLK_GEN_9_SOURCE GCLK_GENCTRL_SRC_XOSC1
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> gclk_arch_gen_9_runstdby
#ifndef CONF_GCLK_GEN_9_RUNSTDBY
#define CONF_GCLK_GEN_9_RUNSTDBY 0
#endif

// <q> Divide Selection
// <i> Indicates whether Divide Selection is enabled or not
// <id> gclk_gen_9_div_sel
#ifndef CONF_GCLK_GEN_9_DIVSEL
#define CONF_GCLK_GEN_9_DIVSEL 0
#endif

// <q> Output Enable
// <i> Indicates whether Output Enable is enabled or not
// <id> gclk_arch_gen_9_oe
#ifndef CONF_GCLK_GEN_9_OE
#define CONF_GCLK_GEN_9_OE 0
#endif

// <q> Output Off Value
// <i> Indicates whether Output Off Value is enabled or not
// <id> gclk_arch_gen_9_oov
#ifndef CONF_GCLK_GEN_9_OOV
#define CONF_GCLK_GEN_9_OOV 0
#endif

// <q> Improve Duty Cycle
// <i> Indicates whether Improve Duty Cycle is enabled or not
// <id> gclk_arch_gen_9_idc
#ifndef CONF_GCLK_GEN_9_IDC
#define CONF_GCLK_GEN_9_IDC 0
#endif

// <q> Generic Clock Generator Enable
// <i> Indicates whether Generic Clock Generator Enable is enabled or not
// <id> gclk_arch_gen_9_enable
#ifndef CONF_GCLK_GEN_9_GENEN
#define CONF_GCLK_GEN_9_GENEN 0
#endif
// </h>

// <h> Generic Clock Generator Division
// <o> Generic clock generator 9 division <0x0000-0xFFFF>
// <id> gclk_gen_9_div
#ifndef CONF_GCLK_GEN_9_DIV
#define CONF_GCLK_GEN_9_DIV 1
#endif
// </h>
// </e>

// <e> Generic clock generator 10 configuration
// <i> Indicates whether generic clock 10 configuration is enabled or not
// <id> enable_gclk_gen_10
#ifndef CONF_GCLK_GENERATOR_10_CONFIG
#define CONF_GCLK_GENERATOR_10_CONFIG 0
#endif

// <h> Generic Clock Generator Control
// <y> Generic clock generator 10 source// <GCLK_GENCTRL_SRC_XOSC0"> External Crystal Oscillator 8-48MHz (XOSC0)
// <GCLK_GENCTRL_SRC_XOSC1"> External Crystal Oscillator 8-48MHz (XOSC1)
// <GCLK_GENCTRL_SRC_GCLKIN"> Generic clock generator input pad
// <GCLK_GENCTRL_SRC_GCLKGEN1"> Generic clock generator 1
// <GCLK_GENCTRL_SRC_OSCULP32K"> 32kHz Ultra Low Power Internal Oscillator (OSCULP32K)
// <GCLK_GENCTRL_SRC_XOSC32K"> 32kHz External Crystal Oscillator (XOSC32K)
// <GCLK_GENCTRL_SRC_DFLL"> Digital Frequency Locked Loop (DFLL48M)
// <GCLK_GENCTRL_SRC_DPLL0"> Digital Phase Locked Loop (DPLL0)
// <GCLK_GENCTRL_SRC_DPLL1"> Digital Phase Locked Loop (DPLL1)
// <i> This defines the clock source for generic clock generator 10
// <id> gclk_gen_10_oscillator
#ifndef CONF_GCLK_GEN_10_SOURCE
#define CONF_GCLK_GEN_10_SOURCE GCLK_GENCTRL_SRC_XOSC1
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> gclk_arch_gen_10_runstdby
#ifndef CONF_GCLK_GEN_10_RUNSTDBY
#define CONF_GCLK_GEN_10_RUNSTDBY 0
#endif

// <q> Divide Selection
// <i> Indicates whether Divide Selection is enabled or not
// <id> gclk_gen_10_div_sel
#ifndef CONF_GCLK_GEN_10_DIVSEL
#define CONF_GCLK_GEN_10_DIVSEL 0
#endif

// <q> Output Enable
// <i> Indicates whether Output Enable is enabled or not
// <id> gclk_arch_gen_10_oe
#ifndef CONF_GCLK_GEN_10_OE
#define CONF_GCLK_GEN_10_OE 0
#endif

// <q> Output Off Value
// <i> Indicates whether Output Off Value is enabled or not
// <id> gclk_arch_gen_10_oov
#ifndef CONF_GCLK_GEN_10_OOV
#define CONF_GCLK_GEN_10_OOV 0
#endif

// <q> Improve Duty Cycle
// <i> Indicates whether Improve Duty Cycle is enabled or not
// <id> gclk_arch_gen_10_idc
#ifndef CONF_GCLK_GEN_10_IDC
#define CONF_GCLK_GEN_10_IDC 0
#endif

// <q> Generic Clock Generator Enable
// <i> Indicates whether Generic Clock Generator Enable is enabled or not
// <id> gclk_arch_gen_10_enable
#ifndef CONF_GCLK_GEN_10_GENEN
#define CONF_GCLK_GEN_10_GENEN 0
#endif
// </h>

// <h> Generic Clock Generator Division
// <o> Generic clock generator 10 division <0x0000-0xFFFF>
// <id> gclk_gen_10_div
#ifndef CONF_GCLK_GEN_10_DIV
#define CONF_GCLK_GEN_10_DIV 1
#endif
// </h>
// </e>

// <e> Generic clock generator 11 configuration
// <i> Indicates whether generic clock 11 configuration is enabled or not
// <id> enable_gclk_gen_11
#ifndef CONF_GCLK_GENERATOR_11_CONFIG
#define CONF_GCLK_GENERATOR_11_CONFIG 0
#endif

// <h> Generic Clock Generator Control
// <y> Generic clock generator 11 source// <GCLK_GENCTRL_SRC_XOSC0"> External Crystal Oscillator 8-48MHz (XOSC0)
// <GCLK_GENCTRL_SRC_XOSC1"> External Crystal Oscillator 8-48MHz (XOSC1)
// <GCLK_GENCTRL_SRC_GCLKIN"> Generic clock generator input pad
// <GCLK_GENCTRL_SRC_GCLKGEN1"> Generic clock generator 1
// <GCLK_GENCTRL_SRC_OSCULP32K"> 32kHz Ultra Low Power Internal Oscillator (OSCULP32K)
// <GCLK_GENCTRL_SRC_XOSC32K"> 32kHz External Crystal Oscillator (XOSC32K)
// <GCLK_GENCTRL_SRC_DFLL"> Digital Frequency Locked Loop (DFLL48M)
// <GCLK_GENCTRL_SRC_DPLL0"> Digital Phase Locked Loop (DPLL0)
// <GCLK_GENCTRL_SRC_DPLL1"> Digital Phase Locked Loop (DPLL1)
// <i> This defines the clock source for generic clock generator 11
// <id> gclk_gen_11_oscillator
#ifndef CONF_GCLK_GEN_11_SOURCE
#define CONF_GCLK_GEN_11_SOURCE GCLK_GENCTRL_SRC_XOSC1
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> gclk_arch_gen_11_runstdby
#ifndef CONF_GCLK_GEN_11_RUNSTDBY
#define CONF_GCLK_GEN_11_RUNSTDBY 0
#endif

// <q> Divide Selection
// <i> Indicates whether Divide Selection is enabled or not
// <id> gclk_gen_11_div_sel
#ifndef CONF_GCLK_GEN_11_DIVSEL
#define CONF_GCLK_GEN_11_DIVSEL 0
#endif

// <q> Output Enable
// <i> Indicates whether Output Enable is enabled or not
// <id> gclk_arch_gen_11_oe
#ifndef CONF_GCLK_GEN_11_OE
#define CONF_GCLK_GEN_11_OE 0
#endif

// <q> Output Off Value
// <i> Indicates whether Output Off Value is enabled or not
// <id> gclk_arch_gen_11_oov
#ifndef CONF_GCLK_GEN_11_OOV
#define CONF_GCLK_GEN_11_OOV 0
#endif

// <q> Improve Duty Cycle
// <i> Indicates whether Improve Duty Cycle is enabled or not
// <id> gclk_arch_gen_11_idc
#ifndef CONF_GCLK_GEN_11_IDC
#define CONF_GCLK_GEN_11_IDC 0
#endif

// <q> Generic Clock Generator Enable
// <i> Indicates whether Generic Clock Generator Enable is enabled or not
// <id> gclk_arch_gen_11_enable
#ifndef CONF_GCLK_GEN_11_GENEN
#define CONF_GCLK_GEN_11_GENEN 0
#endif
// </h>

// <h> Generic Clock Generator Division
// <o> Generic clock generator 11 division <0x0000-0xFFFF>
// <id> gclk_gen_11_div
#ifndef CONF_GCLK_GEN_11_DIV
#define CONF_GCLK_GEN_11_DIV 1
#endif
// </h>
// </e>

// <<< end of configuration section >>>

#endif // HPL_GCLK_CONFIG_H

/* Auto-generated config file hpl_evsys_config.h */
#ifndef HPL_EVSYS_CONFIG_H
#define HPL_EVSYS_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <e> Channel 0 settings
// <id> evsys_channel_setting_0
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_0
#define CONF_EVSYS_CHANNEL_SETTINGS_0 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_0
#ifndef CONF_EDGSEL_0
#define CONF_EDGSEL_0 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_0
#ifndef CONF_PATH_0
#define CONF_PATH_0 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_0
#ifndef CONF_EVGEN_0
#define CONF_EVGEN_0 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_0
#ifndef CONF_OVR_0
#define CONF_OVR_0 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_0
#ifndef CONF_EVD_0
#define CONF_EVD_0 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_0
#ifndef CONF_ONDEMAND_0
#define CONF_ONDEMAND_0 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_0
#ifndef CONF_RUNSTDBY_0
#define CONF_RUNSTDBY_0 0
#endif

// </e>

// <e> Channel 1 settings
// <id> evsys_channel_setting_1
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_1
#define CONF_EVSYS_CHANNEL_SETTINGS_1 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_1
#ifndef CONF_EDGSEL_1
#define CONF_EDGSEL_1 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_1
#ifndef CONF_PATH_1
#define CONF_PATH_1 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_1
#ifndef CONF_EVGEN_1
#define CONF_EVGEN_1 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_1
#ifndef CONF_OVR_1
#define CONF_OVR_1 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_1
#ifndef CONF_EVD_1
#define CONF_EVD_1 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_1
#ifndef CONF_ONDEMAND_1
#define CONF_ONDEMAND_1 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_1
#ifndef CONF_RUNSTDBY_1
#define CONF_RUNSTDBY_1 0
#endif

// </e>

// <e> Channel 2 settings
// <id> evsys_channel_setting_2
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_2
#define CONF_EVSYS_CHANNEL_SETTINGS_2 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_2
#ifndef CONF_EDGSEL_2
#define CONF_EDGSEL_2 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_2
#ifndef CONF_PATH_2
#define CONF_PATH_2 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_2
#ifndef CONF_EVGEN_2
#define CONF_EVGEN_2 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_2
#ifndef CONF_OVR_2
#define CONF_OVR_2 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_2
#ifndef CONF_EVD_2
#define CONF_EVD_2 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_2
#ifndef CONF_ONDEMAND_2
#define CONF_ONDEMAND_2 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_2
#ifndef CONF_RUNSTDBY_2
#define CONF_RUNSTDBY_2 0
#endif

// </e>

// <e> Channel 3 settings
// <id> evsys_channel_setting_3
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_3
#define CONF_EVSYS_CHANNEL_SETTINGS_3 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_3
#ifndef CONF_EDGSEL_3
#define CONF_EDGSEL_3 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_3
#ifndef CONF_PATH_3
#define CONF_PATH_3 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_3
#ifndef CONF_EVGEN_3
#define CONF_EVGEN_3 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_3
#ifndef CONF_OVR_3
#define CONF_OVR_3 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_3
#ifndef CONF_EVD_3
#define CONF_EVD_3 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_3
#ifndef CONF_ONDEMAND_3
#define CONF_ONDEMAND_3 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_3
#ifndef CONF_RUNSTDBY_3
#define CONF_RUNSTDBY_3 0
#endif

// </e>

// <e> Channel 4 settings
// <id> evsys_channel_setting_4
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_4
#define CONF_EVSYS_CHANNEL_SETTINGS_4 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_4
#ifndef CONF_EDGSEL_4
#define CONF_EDGSEL_4 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_4
#ifndef CONF_PATH_4
#define CONF_PATH_4 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_4
#ifndef CONF_EVGEN_4
#define CONF_EVGEN_4 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_4
#ifndef CONF_OVR_4
#define CONF_OVR_4 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_4
#ifndef CONF_EVD_4
#define CONF_EVD_4 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_4
#ifndef CONF_ONDEMAND_4
#define CONF_ONDEMAND_4 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_4
#ifndef CONF_RUNSTDBY_4
#define CONF_RUNSTDBY_4 0
#endif

// </e>

// <e> Channel 5 settings
// <id> evsys_channel_setting_5
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_5
#define CONF_EVSYS_CHANNEL_SETTINGS_5 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_5
#ifndef CONF_EDGSEL_5
#define CONF_EDGSEL_5 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_5
#ifndef CONF_PATH_5
#define CONF_PATH_5 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_5
#ifndef CONF_EVGEN_5
#define CONF_EVGEN_5 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_5
#ifndef CONF_OVR_5
#define CONF_OVR_5 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_5
#ifndef CONF_EVD_5
#define CONF_EVD_5 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_5
#ifndef CONF_ONDEMAND_5
#define CONF_ONDEMAND_5 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_5
#ifndef CONF_RUNSTDBY_5
#define CONF_RUNSTDBY_5 0
#endif

// </e>

// <e> Channel 6 settings
// <id> evsys_channel_setting_6
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_6
#define CONF_EVSYS_CHANNEL_SETTINGS_6 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_6
#ifndef CONF_EDGSEL_6
#define CONF_EDGSEL_6 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_6
#ifndef CONF_PATH_6
#define CONF_PATH_6 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_6
#ifndef CONF_EVGEN_6
#define CONF_EVGEN_6 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_6
#ifndef CONF_OVR_6
#define CONF_OVR_6 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_6
#ifndef CONF_EVD_6
#define CONF_EVD_6 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_6
#ifndef CONF_ONDEMAND_6
#define CONF_ONDEMAND_6 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_6
#ifndef CONF_RUNSTDBY_6
#define CONF_RUNSTDBY_6 0
#endif

// </e>

// <e> Channel 7 settings
// <id> evsys_channel_setting_7
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_7
#define CONF_EVSYS_CHANNEL_SETTINGS_7 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_7
#ifndef CONF_EDGSEL_7
#define CONF_EDGSEL_7 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_7
#ifndef CONF_PATH_7
#define CONF_PATH_7 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_7
#ifndef CONF_EVGEN_7
#define CONF_EVGEN_7 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_7
#ifndef CONF_OVR_7
#define CONF_OVR_7 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_7
#ifndef CONF_EVD_7
#define CONF_EVD_7 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_7
#ifndef CONF_ONDEMAND_7
#define CONF_ONDEMAND_7 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_7
#ifndef CONF_RUNSTDBY_7
#define CONF_RUNSTDBY_7 0
#endif

// </e>

// <e> Channel 8 settings
// <id> evsys_channel_setting_8
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_8
#define CONF_EVSYS_CHANNEL_SETTINGS_8 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_8
#ifndef CONF_EDGSEL_8
#define CONF_EDGSEL_8 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_8
#ifndef CONF_PATH_8
#define CONF_PATH_8 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_8
#ifndef CONF_EVGEN_8
#define CONF_EVGEN_8 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_8
#ifndef CONF_OVR_8
#define CONF_OVR_8 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_8
#ifndef CONF_EVD_8
#define CONF_EVD_8 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_8
#ifndef CONF_ONDEMAND_8
#define CONF_ONDEMAND_8 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_8
#ifndef CONF_RUNSTDBY_8
#define CONF_RUNSTDBY_8 0
#endif

// </e>

// <e> Channel 9 settings
// <id> evsys_channel_setting_9
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_9
#define CONF_EVSYS_CHANNEL_SETTINGS_9 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_9
#ifndef CONF_EDGSEL_9
#define CONF_EDGSEL_9 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_9
#ifndef CONF_PATH_9
#define CONF_PATH_9 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_9
#ifndef CONF_EVGEN_9
#define CONF_EVGEN_9 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_9
#ifndef CONF_OVR_9
#define CONF_OVR_9 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_9
#ifndef CONF_EVD_9
#define CONF_EVD_9 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_9
#ifndef CONF_ONDEMAND_9
#define CONF_ONDEMAND_9 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_9
#ifndef CONF_RUNSTDBY_9
#define CONF_RUNSTDBY_9 0
#endif

// </e>

// <e> Channel 10 settings
// <id> evsys_channel_setting_10
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_10
#define CONF_EVSYS_CHANNEL_SETTINGS_10 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_10
#ifndef CONF_EDGSEL_10
#define CONF_EDGSEL_10 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_10
#ifndef CONF_PATH_10
#define CONF_PATH_10 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_10
#ifndef CONF_EVGEN_10
#define CONF_EVGEN_10 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_10
#ifndef CONF_OVR_10
#define CONF_OVR_10 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_10
#ifndef CONF_EVD_10
#define CONF_EVD_10 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_10
#ifndef CONF_ONDEMAND_10
#define CONF_ONDEMAND_10 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_10
#ifndef CONF_RUNSTDBY_10
#define CONF_RUNSTDBY_10 0
#endif

// </e>

// <e> Channel 11 settings
// <id> evsys_channel_setting_11
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_11
#define CONF_EVSYS_CHANNEL_SETTINGS_11 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_11
#ifndef CONF_EDGSEL_11
#define CONF_EDGSEL_11 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_11
#ifndef CONF_PATH_11
#define CONF_PATH_11 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_11
#ifndef CONF_EVGEN_11
#define CONF_EVGEN_11 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_11
#ifndef CONF_OVR_11
#define CONF_OVR_11 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_11
#ifndef CONF_EVD_11
#define CONF_EVD_11 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_11
#ifndef CONF_ONDEMAND_11
#define CONF_ONDEMAND_11 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_11
#ifndef CONF_RUNSTDBY_11
#define CONF_RUNSTDBY_11 0
#endif

// </e>

// <e> Channel 12 settings
// <id> evsys_channel_setting_12
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_12
#define CONF_EVSYS_CHANNEL_SETTINGS_12 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_12
#ifndef CONF_EDGSEL_12
#define CONF_EDGSEL_12 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_12
#ifndef CONF_PATH_12
#define CONF_PATH_12 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_12
#ifndef CONF_EVGEN_12
#define CONF_EVGEN_12 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_12
#ifndef CONF_OVR_12
#define CONF_OVR_12 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_12
#ifndef CONF_EVD_12
#define CONF_EVD_12 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_12
#ifndef CONF_ONDEMAND_12
#define CONF_ONDEMAND_12 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_12
#ifndef CONF_RUNSTDBY_12
#define CONF_RUNSTDBY_12 0
#endif

// </e>

// <e> Channel 13 settings
// <id> evsys_channel_setting_13
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_13
#define CONF_EVSYS_CHANNEL_SETTINGS_13 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_13
#ifndef CONF_EDGSEL_13
#define CONF_EDGSEL_13 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_13
#ifndef CONF_PATH_13
#define CONF_PATH_13 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_13
#ifndef CONF_EVGEN_13
#define CONF_EVGEN_13 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_13
#ifndef CONF_OVR_13
#define CONF_OVR_13 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_13
#ifndef CONF_EVD_13
#define CONF_EVD_13 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_13
#ifndef CONF_ONDEMAND_13
#define CONF_ONDEMAND_13 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_13
#ifndef CONF_RUNSTDBY_13
#define CONF_RUNSTDBY_13 0
#endif

// </e>

// <e> Channel 14 settings
// <id> evsys_channel_setting_14
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_14
#define CONF_EVSYS_CHANNEL_SETTINGS_14 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_14
#ifndef CONF_EDGSEL_14
#define CONF_EDGSEL_14 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_14
#ifndef CONF_PATH_14
#define CONF_PATH_14 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_14
#ifndef CONF_EVGEN_14
#define CONF_EVGEN_14 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_14
#ifndef CONF_OVR_14
#define CONF_OVR_14 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_14
#ifndef CONF_EVD_14
#define CONF_EVD_14 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_14
#ifndef CONF_ONDEMAND_14
#define CONF_ONDEMAND_14 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_14
#ifndef CONF_RUNSTDBY_14
#define CONF_RUNSTDBY_14 0
#endif

// </e>

// <e> Channel 15 settings
// <id> evsys_channel_setting_15
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_15
#define CONF_EVSYS_CHANNEL_SETTINGS_15 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_15
#ifndef CONF_EDGSEL_15
#define CONF_EDGSEL_15 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_15
#ifndef CONF_PATH_15
#define CONF_PATH_15 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_15
#ifndef CONF_EVGEN_15
#define CONF_EVGEN_15 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_15
#ifndef CONF_OVR_15
#define CONF_OVR_15 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_15
#ifndef CONF_EVD_15
#define CONF_EVD_15 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_15
#ifndef CONF_ONDEMAND_15
#define CONF_ONDEMAND_15 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_15
#ifndef CONF_RUNSTDBY_15
#define CONF_RUNSTDBY_15 0
#endif

// </e>

// <e> Channel 16 settings
// <id> evsys_channel_setting_16
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_16
#define CONF_EVSYS_CHANNEL_SETTINGS_16 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_16
#ifndef CONF_EDGSEL_16
#define CONF_EDGSEL_16 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_16
#ifndef CONF_PATH_16
#define CONF_PATH_16 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_16
#ifndef CONF_EVGEN_16
#define CONF_EVGEN_16 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_16
#ifndef CONF_OVR_16
#define CONF_OVR_16 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_16
#ifndef CONF_EVD_16
#define CONF_EVD_16 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_16
#ifndef CONF_ONDEMAND_16
#define CONF_ONDEMAND_16 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_16
#ifndef CONF_RUNSTDBY_16
#define CONF_RUNSTDBY_16 0
#endif

// </e>

// <e> Channel 17 settings
// <id> evsys_channel_setting_17
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_17
#define CONF_EVSYS_CHANNEL_SETTINGS_17 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_17
#ifndef CONF_EDGSEL_17
#define CONF_EDGSEL_17 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_17
#ifndef CONF_PATH_17
#define CONF_PATH_17 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_17
#ifndef CONF_EVGEN_17
#define CONF_EVGEN_17 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_17
#ifndef CONF_OVR_17
#define CONF_OVR_17 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_17
#ifndef CONF_EVD_17
#define CONF_EVD_17 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_17
#ifndef CONF_ONDEMAND_17
#define CONF_ONDEMAND_17 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_17
#ifndef CONF_RUNSTDBY_17
#define CONF_RUNSTDBY_17 0
#endif

// </e>

// <e> Channel 18 settings
// <id> evsys_channel_setting_18
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_18
#define CONF_EVSYS_CHANNEL_SETTINGS_18 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_18
#ifndef CONF_EDGSEL_18
#define CONF_EDGSEL_18 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_18
#ifndef CONF_PATH_18
#define CONF_PATH_18 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_18
#ifndef CONF_EVGEN_18
#define CONF_EVGEN_18 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_18
#ifndef CONF_OVR_18
#define CONF_OVR_18 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_18
#ifndef CONF_EVD_18
#define CONF_EVD_18 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_18
#ifndef CONF_ONDEMAND_18
#define CONF_ONDEMAND_18 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_18
#ifndef CONF_RUNSTDBY_18
#define CONF_RUNSTDBY_18 0
#endif

// </e>

// <e> Channel 19 settings
// <id> evsys_channel_setting_19
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_19
#define CONF_EVSYS_CHANNEL_SETTINGS_19 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_19
#ifndef CONF_EDGSEL_19
#define CONF_EDGSEL_19 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_19
#ifndef CONF_PATH_19
#define CONF_PATH_19 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_19
#ifndef CONF_EVGEN_19
#define CONF_EVGEN_19 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_19
#ifndef CONF_OVR_19
#define CONF_OVR_19 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_19
#ifndef CONF_EVD_19
#define CONF_EVD_19 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_19
#ifndef CONF_ONDEMAND_19
#define CONF_ONDEMAND_19 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_19
#ifndef CONF_RUNSTDBY_19
#define CONF_RUNSTDBY_19 0
#endif

// </e>

// <e> Channel 20 settings
// <id> evsys_channel_setting_20
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_20
#define CONF_EVSYS_CHANNEL_SETTINGS_20 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_20
#ifndef CONF_EDGSEL_20
#define CONF_EDGSEL_20 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_20
#ifndef CONF_PATH_20
#define CONF_PATH_20 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_20
#ifndef CONF_EVGEN_20
#define CONF_EVGEN_20 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_20
#ifndef CONF_OVR_20
#define CONF_OVR_20 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_20
#ifndef CONF_EVD_20
#define CONF_EVD_20 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_20
#ifndef CONF_ONDEMAND_20
#define CONF_ONDEMAND_20 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_20
#ifndef CONF_RUNSTDBY_20
#define CONF_RUNSTDBY_20 0
#endif

// </e>

// <e> Channel 21 settings
// <id> evsys_channel_setting_21
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_21
#define CONF_EVSYS_CHANNEL_SETTINGS_21 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_21
#ifndef CONF_EDGSEL_21
#define CONF_EDGSEL_21 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_21
#ifndef CONF_PATH_21
#define CONF_PATH_21 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_21
#ifndef CONF_EVGEN_21
#define CONF_EVGEN_21 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_21
#ifndef CONF_OVR_21
#define CONF_OVR_21 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_21
#ifndef CONF_EVD_21
#define CONF_EVD_21 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_21
#ifndef CONF_ONDEMAND_21
#define CONF_ONDEMAND_21 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_21
#ifndef CONF_RUNSTDBY_21
#define CONF_RUNSTDBY_21 0
#endif

// </e>

// <e> Channel 22 settings
// <id> evsys_channel_setting_22
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_22
#define CONF_EVSYS_CHANNEL_SETTINGS_22 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_22
#ifndef CONF_EDGSEL_22
#define CONF_EDGSEL_22 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_22
#ifndef CONF_PATH_22
#define CONF_PATH_22 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_22
#ifndef CONF_EVGEN_22
#define CONF_EVGEN_22 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_22
#ifndef CONF_OVR_22
#define CONF_OVR_22 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_22
#ifndef CONF_EVD_22
#define CONF_EVD_22 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_22
#ifndef CONF_ONDEMAND_22
#define CONF_ONDEMAND_22 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_22
#ifndef CONF_RUNSTDBY_22
#define CONF_RUNSTDBY_22 0
#endif

// </e>

// <e> Channel 23 settings
// <id> evsys_channel_setting_23
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_23
#define CONF_EVSYS_CHANNEL_SETTINGS_23 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_23
#ifndef CONF_EDGSEL_23
#define CONF_EDGSEL_23 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_23
#ifndef CONF_PATH_23
#define CONF_PATH_23 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_23
#ifndef CONF_EVGEN_23
#define CONF_EVGEN_23 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_23
#ifndef CONF_OVR_23
#define CONF_OVR_23 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_23
#ifndef CONF_EVD_23
#define CONF_EVD_23 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_23
#ifndef CONF_ONDEMAND_23
#define CONF_ONDEMAND_23 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_23
#ifndef CONF_RUNSTDBY_23
#define CONF_RUNSTDBY_23 0
#endif

// </e>

// <e> Channel 24 settings
// <id> evsys_channel_setting_24
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_24
#define CONF_EVSYS_CHANNEL_SETTINGS_24 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_24
#ifndef CONF_EDGSEL_24
#define CONF_EDGSEL_24 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_24
#ifndef CONF_PATH_24
#define CONF_PATH_24 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_24
#ifndef CONF_EVGEN_24
#define CONF_EVGEN_24 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_24
#ifndef CONF_OVR_24
#define CONF_OVR_24 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_24
#ifndef CONF_EVD_24
#define CONF_EVD_24 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_24
#ifndef CONF_ONDEMAND_24
#define CONF_ONDEMAND_24 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_24
#ifndef CONF_RUNSTDBY_24
#define CONF_RUNSTDBY_24 0
#endif

// </e>

// <e> Channel 25 settings
// <id> evsys_channel_setting_25
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_25
#define CONF_EVSYS_CHANNEL_SETTINGS_25 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_25
#ifndef CONF_EDGSEL_25
#define CONF_EDGSEL_25 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_25
#ifndef CONF_PATH_25
#define CONF_PATH_25 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_25
#ifndef CONF_EVGEN_25
#define CONF_EVGEN_25 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_25
#ifndef CONF_OVR_25
#define CONF_OVR_25 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_25
#ifndef CONF_EVD_25
#define CONF_EVD_25 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_25
#ifndef CONF_ONDEMAND_25
#define CONF_ONDEMAND_25 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_25
#ifndef CONF_RUNSTDBY_25
#define CONF_RUNSTDBY_25 0
#endif

// </e>

// <e> Channel 26 settings
// <id> evsys_channel_setting_26
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_26
#define CONF_EVSYS_CHANNEL_SETTINGS_26 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_26
#ifndef CONF_EDGSEL_26
#define CONF_EDGSEL_26 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_26
#ifndef CONF_PATH_26
#define CONF_PATH_26 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_26
#ifndef CONF_EVGEN_26
#define CONF_EVGEN_26 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_26
#ifndef CONF_OVR_26
#define CONF_OVR_26 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_26
#ifndef CONF_EVD_26
#define CONF_EVD_26 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_26
#ifndef CONF_ONDEMAND_26
#define CONF_ONDEMAND_26 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_26
#ifndef CONF_RUNSTDBY_26
#define CONF_RUNSTDBY_26 0
#endif

// </e>

// <e> Channel 27 settings
// <id> evsys_channel_setting_27
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_27
#define CONF_EVSYS_CHANNEL_SETTINGS_27 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_27
#ifndef CONF_EDGSEL_27
#define CONF_EDGSEL_27 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_27
#ifndef CONF_PATH_27
#define CONF_PATH_27 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_27
#ifndef CONF_EVGEN_27
#define CONF_EVGEN_27 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_27
#ifndef CONF_OVR_27
#define CONF_OVR_27 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_27
#ifndef CONF_EVD_27
#define CONF_EVD_27 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_27
#ifndef CONF_ONDEMAND_27
#define CONF_ONDEMAND_27 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_27
#ifndef CONF_RUNSTDBY_27
#define CONF_RUNSTDBY_27 0
#endif

// </e>

// <e> Channel 28 settings
// <id> evsys_channel_setting_28
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_28
#define CONF_EVSYS_CHANNEL_SETTINGS_28 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_28
#ifndef CONF_EDGSEL_28
#define CONF_EDGSEL_28 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_28
#ifndef CONF_PATH_28
#define CONF_PATH_28 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_28
#ifndef CONF_EVGEN_28
#define CONF_EVGEN_28 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_28
#ifndef CONF_OVR_28
#define CONF_OVR_28 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_28
#ifndef CONF_EVD_28
#define CONF_EVD_28 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_28
#ifndef CONF_ONDEMAND_28
#define CONF_ONDEMAND_28 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_28
#ifndef CONF_RUNSTDBY_28
#define CONF_RUNSTDBY_28 0
#endif

// </e>

// <e> Channel 29 settings
// <id> evsys_channel_setting_29
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_29
#define CONF_EVSYS_CHANNEL_SETTINGS_29 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_29
#ifndef CONF_EDGSEL_29
#define CONF_EDGSEL_29 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_29
#ifndef CONF_PATH_29
#define CONF_PATH_29 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_29
#ifndef CONF_EVGEN_29
#define CONF_EVGEN_29 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_29
#ifndef CONF_OVR_29
#define CONF_OVR_29 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_29
#ifndef CONF_EVD_29
#define CONF_EVD_29 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_29
#ifndef CONF_ONDEMAND_29
#define CONF_ONDEMAND_29 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_29
#ifndef CONF_RUNSTDBY_29
#define CONF_RUNSTDBY_29 0
#endif

// </e>

// <e> Channel 30 settings
// <id> evsys_channel_setting_30
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_30
#define CONF_EVSYS_CHANNEL_SETTINGS_30 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_30
#ifndef CONF_EDGSEL_30
#define CONF_EDGSEL_30 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_30
#ifndef CONF_PATH_30
#define CONF_PATH_30 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_30
#ifndef CONF_EVGEN_30
#define CONF_EVGEN_30 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_30
#ifndef CONF_OVR_30
#define CONF_OVR_30 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_30
#ifndef CONF_EVD_30
#define CONF_EVD_30 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_30
#ifndef CONF_ONDEMAND_30
#define CONF_ONDEMAND_30 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_30
#ifndef CONF_RUNSTDBY_30
#define CONF_RUNSTDBY_30 0
#endif

// </e>

// <e> Channel 31 settings
// <id> evsys_channel_setting_31
#ifndef CONF_EVSYS_CHANNEL_SETTINGS_31
#define CONF_EVSYS_CHANNEL_SETTINGS_31 0
#endif

// <y> Edge detection
// <i> Indicates whether channel is enabled in standby sleep mode
// <EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val"> No event output when using the resynchronized or synchronous path
// <EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val"> Event is detected on the rising edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val"> Event is detected on the falling edge of the signal from event generator
// <EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val"> Event is detected on the rising and falling edge of the signal from event generator
// <id> evsys_edgsel_31
#ifndef CONF_EDGSEL_31
#define CONF_EDGSEL_31 EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val
#endif

// <y> Path selection
// <i> Indicates which path for the event signal is chosen
// <EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val"> Synchronous path
// <EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val"> Resynchronized path
// <EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val"> Asynchronous path
// <id> evsys_path_31
#ifndef CONF_PATH_31
#define CONF_PATH_31 EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val
#endif

// <o> Event generator
// <i> Determines event generator for channel
// <0x0=>No event generator
// <0x1=>XOSC Clock Failure 0
// <0x2=>XOSC Clock Failure 1
// <0x3=>XOSC32K Clock Failure
// <0x4=>RTC period 0
// <0x5=>RTC period 1
// <0x6=>RTC period 2
// <0x7=>RTC period 3
// <0x8=>RTC period 4
// <0x9=>RTC period 5
// <0xA=>RTC period 6
// <0xB=>RTC period 7
// <0xC=>RTC compare 0
// <0xD=>RTC compare 1
// <0xE=>RTC compare 2
// <0xF=>RTC compare 3
// <0x10=>RTC Tamper Detection
// <0x11=>RTC overflow
// <0x12=>EIC external interrupt 0
// <0x13=>EIC external interrupt 1
// <0x14=>EIC external interrupt 2
// <0x15=>EIC external interrupt 3
// <0x16=>EIC external interrupt 4
// <0x17=>EIC external interrupt 5
// <0x18=>EIC external interrupt 6
// <0x19=>EIC external interrupt 7
// <0x1A=>EIC external interrupt 8
// <0x1B=>EIC external interrupt 9
// <0x1C=>EIC external interrupt 10
// <0x1D=>EIC external interrupt 11
// <0x1E=>EIC external interrupt 12
// <0x1F=>EIC external interrupt 13
// <0x20=>EIC external interrupt 14
// <0x21=>EIC external interrupt 15
// <0x22=>DMAC channel 0
// <0x23=>DMAC channel 1
// <0x24=>DMAC channel 2
// <0x25=>DMAC channel 3
// <0x28=>PAC access error
// <0x29=>TCC0 overflow
// <0x2A=>TCC0 trig
// <0x2B=>TCC0 counter
// <0x2C=>TCC0 match/capture 0
// <0x2D=>TCC0 match/capture 1
// <0x2E=>TCC0 match/capture 2
// <0x2F=>TCC0 match/capture 3
// <0x30=>TCC0 match/capture 4
// <0x31=>TCC0 match/capture 5
// <0x32=>TCC1 overflow
// <0x33=>TCC1 trig
// <0x34=>TCC1 counter
// <0x35=>TCC1 match/capture 0
// <0x36=>TCC1 match/capture 1
// <0x37=>TCC1 match/capture 2
// <0x38=>TCC1 match/capture 3
// <0x39=>TCC2 overflow
// <0x3A=>TCC2 trig
// <0x3B=>TCC2 counter
// <0x3C=>TCC2 match/capture 0
// <0x3D=>TCC2 match/capture 1
// <0x3E=>TCC2 match/capture 2
// <0x3F=>TCC3 overflow
// <0x40=>TCC3 trig
// <0x41=>TCC3 counter
// <0x42=>TCC3 match/capture 0
// <0x43=>TCC3 match/capture 1
// <0x44=>TCC4 overflow
// <0x45=>TCC4 trig
// <0x46=>TCC4 counter
// <0x47=>TCC4 match/capture 0
// <0x48=>TCC4 match/capture 1
// <0x49=>TC0 overflow
// <0x4A=>TC0 match/capture 0
// <0x4B=>TC0 match/capture 1
// <0x4C=>TC1 overflow
// <0x4D=>TC1 match/capture 0
// <0x4E=>TC1 match/capture 1
// <0x4F=>TC2 overflow
// <0x50=>TC2 match/capture 0
// <0x51=>TC2 match/capture 1
// <0x52=>TC3 overflow
// <0x53=>TC3 match/capture 0
// <0x54=>TC3 match/capture 1
// <0x55=>TC4 overflow
// <0x56=>TC4 match/capture 0
// <0x57=>TC4 match/capture 1
// <0x58=>TC5 overflow
// <0x59=>TC5 match/capture 0
// <0x5A=>TC5 match/capture 1
// <0x5B=>TC6 overflow
// <0x5C=>TC6 match/capture 0
// <0x5D=>TC6 match/capture 1
// <0x5E=>TC7 overflow
// <0x5F=>TC7 match/capture 0
// <0x60=>TC7 match/capture 1
// <0x61=>PDEC overflow
// <0x62=>PDEC error
// <0x63=>PDEC direction
// <0x64=>PDEC VLC
// <0x65=>PDEC match channel 0
// <0x66=>PDEC match channel 1
// <0x67=>ADC0 result ready
// <0x68=>ADC0 window monitor
// <0x69=>ADC1 result ready
// <0x6A=>ADC1 window monitor
// <0x6B=>AC comparator 0
// <0x6C=>AC comparator 1
// <0x6D=>AC window 0
// <0x6E=>DAC data buffer empty 0
// <0x6F=>DAC data buffer empty 1
// <0x70=>DAC result ready 0
// <0x71=>DAC result ready 1
// <0x73=>TRNG Data Ready
// <0x74=>CCL LUT output 0
// <0x75=>CCL LUT output 1
// <0x76=>CCL LUT output 2
// <0x77=>CCL LUT output 3
// <id> evsys_evgen_31
#ifndef CONF_EVGEN_31
#define CONF_EVGEN_31 0
#endif

// <q> Overrun channel interrupt
// <i> Indicates whether overrun channel interrupt is enabled or not
// <id> evsys_ovr_31
#ifndef CONF_OVR_31
#define CONF_OVR_31 0
#endif

// <q> Event detected interrupt
// <i> Indicates whether event detected interrupt is enabled or not
// <id> evsys_evd_31
#ifndef CONF_EVD_31
#define CONF_EVD_31 0
#endif

// <q> On demand clock
// <i> Indicates whether clock for channel is requested on demand or not
// <id> evsys_ondemand_31
#ifndef CONF_ONDEMAND_31
#define CONF_ONDEMAND_31 0
#endif

// <q> Run is standby mode
// <i> Indicates whether channel is enabled in standby sleep mode
// <id> evsys_runstdby_31
#ifndef CONF_RUNSTDBY_31
#define CONF_RUNSTDBY_31 0
#endif

// </e>

// <h> RTC events
// <o> Channel selection for RTC Tamper Event
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_0
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_0
#define CONF_CHANNEL_0 0
#endif
//</h>

// <h> PORT events
// <o> Channel selection for PORT event 0
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_1
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_1
#define CONF_CHANNEL_1 0
#endif

// <o> Channel selection for PORT event 1
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_2
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_2
#define CONF_CHANNEL_2 0
#endif

// <o> Channel selection for PORT event 2
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_3
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_3
#define CONF_CHANNEL_3 0
#endif

// <o> Channel selection for PORT event 3
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_4
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_4
#define CONF_CHANNEL_4 0
#endif
//</h>

// <h> DMAC events
// <o> Channel selection for DMAC channel 0
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_5
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_5
#define CONF_CHANNEL_5 0
#endif

// <o> Channel selection for DMAC channel 1
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_6
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_6
#define CONF_CHANNEL_6 0
#endif

// <o> Channel selection for DMAC channel 2
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_7
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_7
#define CONF_CHANNEL_7 0
#endif

// <o> Channel selection for DMAC channel 3
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_8
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_8
#define CONF_CHANNEL_8 0
#endif

// <o> Channel selection for DMAC channel 4
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_9
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_9
#define CONF_CHANNEL_9 0
#endif

// <o> Channel selection for DMAC channel 5
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_10
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_10
#define CONF_CHANNEL_10 0
#endif

// <o> Channel selection for DMAC channel 6
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_11
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_11
#define CONF_CHANNEL_11 0
#endif

// <o> Channel selection for DMAC channel 7
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_12
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_12
#define CONF_CHANNEL_12 0
#endif
//</h>

// <h> Reserved
#ifndef CONF_CHANNEL_13
#define CONF_CHANNEL_13 0
#endif

#ifndef CONF_CHANNEL_14
#define CONF_CHANNEL_14 0
#endif

#ifndef CONF_CHANNEL_15
#define CONF_CHANNEL_15 0
#endif

#ifndef CONF_CHANNEL_16
#define CONF_CHANNEL_16 0
#endif
//</h>

// <h> TCC events
// <o> Channel selection for TCC0 Event 0
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_17
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_17
#define CONF_CHANNEL_17 0
#endif

// <o> Channel selection for TCC0 Event 1
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_18
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_18
#define CONF_CHANNEL_18 0
#endif

// <o> Channel selection for TCC0 match/capture 0
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_19
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_19
#define CONF_CHANNEL_19 0
#endif

// <o> Channel selection for TCC0 match/capture 1
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_20
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_20
#define CONF_CHANNEL_20 0
#endif

// <o> Channel selection for TCC0 match/capture 2
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_21
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_21
#define CONF_CHANNEL_21 0
#endif

// <o> Channel selection for TCC0 match/capture 3
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_22
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_22
#define CONF_CHANNEL_22 0
#endif

// <o> Channel selection for TCC0 match/capture 4
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_23
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_23
#define CONF_CHANNEL_23 0
#endif

// <o> Channel selection for TCC0 match/capture 5
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_24
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_24
#define CONF_CHANNEL_24 0
#endif

// <o> Channel selection for TCC1 Event 0
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_25
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_25
#define CONF_CHANNEL_25 0
#endif

// <o> Channel selection for TCC1 Event 1
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_26
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_26
#define CONF_CHANNEL_26 0
#endif

// <o> Channel selection for TCC1 match/capture 0
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_27
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_27
#define CONF_CHANNEL_27 0
#endif

// <o> Channel selection for TCC1 match/capture 1
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_28
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_28
#define CONF_CHANNEL_28 0
#endif

// <o> Channel selection for TCC1 match/capture 2
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_29
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_29
#define CONF_CHANNEL_29 0
#endif

// <o> Channel selection for TCC1 match/capture 3
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_30
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_30
#define CONF_CHANNEL_30 0
#endif

// <o> Channel selection for TCC2 Event 0
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_31
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_31
#define CONF_CHANNEL_31 0
#endif

// <o> Channel selection for TCC2 Event 1
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_32
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_32
#define CONF_CHANNEL_32 0
#endif

// <o> Channel selection for TCC2 match/capture 0
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_33
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_33
#define CONF_CHANNEL_33 0
#endif

// <o> Channel selection for TCC2 match/capture 1
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_34
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_34
#define CONF_CHANNEL_34 0
#endif

// <o> Channel selection for TCC2 match/capture 2
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_35
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_35
#define CONF_CHANNEL_35 0
#endif

// <o> Channel selection for TCC3 Event 0
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_36
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_36
#define CONF_CHANNEL_36 0
#endif

// <o> Channel selection for TCC3 Event 1
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_37
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_37
#define CONF_CHANNEL_37 0
#endif

// <o> Channel selection for TCC3 match/capture 0
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_38
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_38
#define CONF_CHANNEL_38 0
#endif

// <o> Channel selection for TCC3 match/capture 1
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_39
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_39
#define CONF_CHANNEL_39 0
#endif

// <o> Channel selection for TCC4 Event 0
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_40
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_40
#define CONF_CHANNEL_40 0
#endif

// <o> Channel selection for TCC4 Event 1
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_41
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_41
#define CONF_CHANNEL_41 0
#endif

// <o> Channel selection for TCC4 match/capture 0
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_42
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_42
#define CONF_CHANNEL_42 0
#endif

// <o> Channel selection for TCC4 match/capture 1
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_43
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_43
#define CONF_CHANNEL_43 0
#endif
//</h>

// <h> TC events
// <o> Channel selection for TC0 event
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_44
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_44
#define CONF_CHANNEL_44 0
#endif

// <o> Channel selection for TC1 event
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_45
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_45
#define CONF_CHANNEL_45 0
#endif

// <o> Channel selection for TC2 event
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_46
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_46
#define CONF_CHANNEL_46 0
#endif

// <o> Channel selection for TC3 event
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_47
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_47
#define CONF_CHANNEL_47 0
#endif

// <o> Channel selection for TC4 event
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_48
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_48
#define CONF_CHANNEL_48 0
#endif

// <o> Channel selection for TC5 event
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_49
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_49
#define CONF_CHANNEL_49 0
#endif

// <o> Channel selection for TC6 event
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_50
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_50
#define CONF_CHANNEL_50 0
#endif

// <o> Channel selection for TC7 event
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_51
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_51
#define CONF_CHANNEL_51 0
#endif
//</h>

// <h> PDEC events
// <o> Channel selection for PDEC event 0
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_52
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_52
#define CONF_CHANNEL_52 0
#endif

// <o> Channel selection for PDEC event 1
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_53
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_53
#define CONF_CHANNEL_53 0
#endif

// <o> Channel selection for PDEC event 2
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_54
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_54
#define CONF_CHANNEL_54 0
#endif
//</h>

// <h> ADC events
// <o> Channel selection for ADC0 start conversion event
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_55
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_55
#define CONF_CHANNEL_55 0
#endif

// <o> Channel selection for ADC0 flush event
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_56
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_56
#define CONF_CHANNEL_56 0
#endif

// <o> Channel selection for ADC1 start conversion event
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_57
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_57
#define CONF_CHANNEL_57 0
#endif

// <o> Channel selection for ADC1 flush event
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_58
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_58
#define CONF_CHANNEL_58 0
#endif
//</h>

// <h> AC events
// <o> Channel selection for Start comparator event 0
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_59
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_59
#define CONF_CHANNEL_59 0
#endif

// <o> Channel selection for Start comparator event 1
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_60
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_60
#define CONF_CHANNEL_60 0
#endif
//</h>

// <h> DAC events
// <o> Channel selection for DAC Start event 0
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_61
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_61
#define CONF_CHANNEL_61 0
#endif

// <o> Channel selection for DAC Start event 1
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_62
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_62
#define CONF_CHANNEL_62 0
#endif
//</h>

// <h> CCL events
// <o> Channel selection for CCL input event 0
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_63
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_63
#define CONF_CHANNEL_63 0
#endif

// <o> Channel selection for CCL input event 1
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_64
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_64
#define CONF_CHANNEL_64 0
#endif

// <o> Channel selection for CCL input event 2
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_65
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_65
#define CONF_CHANNEL_65 0
#endif

// <o> Channel selection for CCL input event 3
// <0x0=>No channel output selected
// <0x1=>Channel 0
// <0x2=>Channel 1
// <0x3=>Channel 2
// <0x4=>Channel 3
// <0x5=>Channel 4
// <0x6=>Channel 5
// <0x7=>Channel 6
// <0x8=>Channel 7
// <0x9=>Channel 8
// <0xA=>Channel 9
// <0xB=>Channel 10
// <0xC=>Channel 11
// <0xD=>Channel 12
// <0xE=>Channel 13
// <0xF=>Channel 14
// <0x10=>Channel 15
// <0x11=>Channel 16
// <0x12=>Channel 17
// <0x13=>Channel 18
// <0x14=>Channel 19
// <0x15=>Channel 20
// <0x16=>Channel 21
// <0x17=>Channel 22
// <0x18=>Channel 23
// <0x19=>Channel 24
// <0x1A=>Channel 25
// <0x1B=>Channel 26
// <0x1C=>Channel 27
// <0x1D=>Channel 28
// <0x1E=>Channel 29
// <0x1F=>Channel 30
// <0x20=>Channel 31
// <id> evsys_channel_66
// <i> Indicates which channel is chosen for user
#ifndef CONF_CHANNEL_66
#define CONF_CHANNEL_66 0
#endif
//</h>

// <<< end of configuration section >>>

#endif // HPL_EVSYS_CONFIG_H

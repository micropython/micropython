/* Auto-generated config file hpl_adc_config.h */
#ifndef HPL_ADC_CONFIG_H
#define HPL_ADC_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

#ifndef CONF_ADC_0_ENABLE
#define CONF_ADC_0_ENABLE 1
#endif

// <h> Basic Configuration

// <o> Conversion Result Resolution
// <0x0=>12-bit
// <0x1=>16-bit (averaging must be enabled)
// <0x2=>10-bit
// <0x3=>8-bit
// <i>  Defines the bit resolution for the ADC sample values (RESSEL)
// <id> adc_resolution
#ifndef CONF_ADC_0_RESSEL
#define CONF_ADC_0_RESSEL 0x0
#endif

// <o> Reference Selection
// <0x0=>Internal bandgap reference
// <0x2=>1/2 VDDANA (only for VDDANA > 2.0V)
// <0x3=>VDDANA
// <0x4=>External reference A
// <0x5=>External reference B
// <0x6=>External reference C
// <i> Select the reference for the ADC (REFSEL)
// <id> adc_reference
#ifndef CONF_ADC_0_REFSEL
#define CONF_ADC_0_REFSEL 0x0
#endif

// <o> Prescaler configuration
// <0x0=>Peripheral clock divided by 2
// <0x1=>Peripheral clock divided by 4
// <0x2=>Peripheral clock divided by 8
// <0x3=>Peripheral clock divided by 16
// <0x4=>Peripheral clock divided by 32
// <0x5=>Peripheral clock divided by 64
// <0x6=>Peripheral clock divided by 128
// <0x7=>Peripheral clock divided by 256
// <i> These bits define the ADC clock relative to the peripheral clock (PRESCALER)
// <id> adc_prescaler
#ifndef CONF_ADC_0_PRESCALER
#define CONF_ADC_0_PRESCALER 0x3
#endif

// <q> Free Running Mode
// <i> When enabled, the ADC is in free running mode and a new conversion will be initiated when a previous conversion completes. (FREERUN)
// <id> adc_freerunning_mode
#ifndef CONF_ADC_0_FREERUN
#define CONF_ADC_0_FREERUN 0
#endif

// <q> Differential Mode
// <i> In differential mode, the voltage difference between the MUXPOS and MUXNEG inputs will be converted by the ADC. (DIFFMODE)
// <id> adc_differential_mode
#ifndef CONF_ADC_0_DIFFMODE
#define CONF_ADC_0_DIFFMODE 0
#endif

// <o> Positive Mux Input Selection
// <0x00=>ADC AIN0 pin
// <0x01=>ADC AIN1 pin
// <0x02=>ADC AIN2 pin
// <0x03=>ADC AIN3 pin
// <0x04=>ADC AIN4 pin
// <0x05=>ADC AIN5 pin
// <0x06=>ADC AIN6 pin
// <0x07=>ADC AIN7 pin
// <0x08=>ADC AIN8 pin
// <0x09=>ADC AIN9 pin
// <0x0A=>ADC AIN10 pin
// <0x0B=>ADC AIN11 pin
// <0x0C=>ADC AIN12 pin
// <0x0D=>ADC AIN13 pin
// <0x0E=>ADC AIN14 pin
// <0x0F=>ADC AIN15 pin
// <0x18=>1/4 scaled core supply
// <0x19=>1/4 Scaled VBAT Supply
// <0x1A=>1/4 scaled I/O supply
// <0x1B=>Bandgap voltage
// <0x1C=>Temperature reference (PTAT)
// <0x1D=>Temperature reference (CTAT)
// <0x1E=>DAC Output
// <i> These bits define the Mux selection for the positive ADC input. (MUXPOS)
// <id> adc_pinmux_positive
#ifndef CONF_ADC_0_MUXPOS
#define CONF_ADC_0_MUXPOS 0x0
#endif

// <o> Negative Mux Input Selection
// <0x00=>ADC AIN0 pin
// <0x01=>ADC AIN1 pin
// <0x02=>ADC AIN2 pin
// <0x03=>ADC AIN3 pin
// <0x04=>ADC AIN4 pin
// <0x05=>ADC AIN5 pin
// <0x06=>ADC AIN6 pin
// <0x07=>ADC AIN7 pin
// <0x18=>Internal ground
// <0x19=>I/O ground
// <i> These bits define the Mux selection for the negative ADC input. (MUXNEG)
// <id> adc_pinmux_negative
#ifndef CONF_ADC_0_MUXNEG
#define CONF_ADC_0_MUXNEG 0x0
#endif

// </h>

// <e> Advanced Configuration
// <id> adc_advanced_settings
#ifndef CONF_ADC_0_ADVANCED
#define CONF_ADC_0_ADVANCED 0
#endif

// <q> Run in standby
// <i> Indicates whether the ADC will continue running in standby sleep mode or not (RUNSTDBY)
// <id> adc_arch_runstdby
#ifndef CONF_ADC_0_RUNSTDBY
#define CONF_ADC_0_RUNSTDBY 0
#endif

// <q>Debug Run
// <i> If enabled, the ADC is running if the CPU is halted by an external debugger. (DBGRUN)
// <id> adc_arch_dbgrun
#ifndef CONF_ADC_0_DBGRUN
#define CONF_ADC_0_DBGRUN 0
#endif

// <q> On Demand Control
// <i> Will keep the ADC peripheral running if requested by other peripherals (ONDEMAND)
// <id> adc_arch_ondemand
#ifndef CONF_ADC_0_ONDEMAND
#define CONF_ADC_0_ONDEMAND 0
#endif

// <q> Left-Adjusted Result
// <i> When enabled, the ADC conversion result is left-adjusted in the RESULT register. The high byte of the 12-bit result will be present in the upper part of the result register. (LEFTADJ)
// <id> adc_arch_leftadj
#ifndef CONF_ADC_0_LEFTADJ
#define CONF_ADC_0_LEFTADJ 0
#endif

// <q> Reference Buffer Offset Compensation Enable
// <i> The accuracy of the gain stage can be increased by enabling the reference buffer offset compensation. This will decrease the input impedance and thus increase the start-up time of the reference. (REFCOMP)
// <id> adc_arch_refcomp
#ifndef CONF_ADC_0_REFCOMP
#define CONF_ADC_0_REFCOMP 0
#endif

// <q>Comparator Offset Compensation Enable
// <i> This bit indicates whether the Comparator Offset Compensation is enabled or not (OFFCOMP)
// <id> adc_arch_offcomp
#ifndef CONF_ADC_0_OFFCOMP
#define CONF_ADC_0_OFFCOMP 0
#endif

// <q> Digital Correction Logic Enabled
// <i> When enabled, the ADC conversion result in the RESULT register is then corrected for gain and offset based on the values in the GAINCAL and OFFSETCAL registers. (CORREN)
// <id> adc_arch_corren
#ifndef CONF_ADC_0_CORREN
#define CONF_ADC_0_CORREN 0
#endif

// <o> Offset Correction Value <0-4095>
// <i> If the digital correction logic is enabled (CTRLB.CORREN = 1), these bits define how the ADC conversion result is compensated for offset error before being written to the Result register. (OFFSETCORR)
// <id> adc_arch_offsetcorr
#ifndef CONF_ADC_0_OFFSETCORR
#define CONF_ADC_0_OFFSETCORR 0
#endif

// <o> Gain Correction Value <0-4095>
// <i> If the digital correction logic is enabled (CTRLB.CORREN = 1), these bits define how the ADC conversion result is compensated for gain error before being written to the result register. (GAINCORR)
// <id> adc_arch_gaincorr
#ifndef CONF_ADC_0_GAINCORR
#define CONF_ADC_0_GAINCORR 0
#endif

// <o> Adjusting Result / Division Coefficient <0-7>
// <i> These bits define the division coefficient in 2n steps. (ADJRES)
// <id> adc_arch_adjres
#ifndef CONF_ADC_0_ADJRES
#define CONF_ADC_0_ADJRES 0x0
#endif

// <o.0..10> Number of Samples to be Collected
// <0x0=>1 sample
// <0x1=>2 samples
// <0x2=>4 samples
// <0x3=>8 samples
// <0x4=>16 samples
// <0x5=>32 samples
// <0x6=>64 samples
// <0x7=>128 samples
// <0x8=>256 samples
// <0x9=>512 samples
// <0xA=>1024 samples
// <i> Define how many samples should be added together.The result will be available in the Result register (SAMPLENUM)
// <id> adc_arch_samplenum
#ifndef CONF_ADC_0_SAMPLENUM
#define CONF_ADC_0_SAMPLENUM 0x0
#endif

// <o> Sampling Time Length <0-63>
// <i> These bits control the ADC sampling time in number of half CLK_ADC cycles, depending of the prescaler value, thus controlling the ADC input impedance. (SAMPLEN)
// <id> adc_arch_samplen
#ifndef CONF_ADC_0_SAMPLEN
#define CONF_ADC_0_SAMPLEN 0
#endif

// <o> Window Monitor Mode
// <0x0=>No window mode
// <0x1=>Mode 1: RESULT above lower threshold
// <0x2=>Mode 2: RESULT beneath upper threshold
// <0x3=>Mode 3: RESULT inside lower and upper threshold
// <0x4=>Mode 4: RESULT outside lower and upper threshold
// <i> These bits enable and define the window monitor mode. (WINMODE)
// <id> adc_arch_winmode
#ifndef CONF_ADC_0_WINMODE
#define CONF_ADC_0_WINMODE 0x0
#endif

// <o> Window Monitor Lower Threshold <0-65535>
// <i> If the window monitor is enabled, these bits define the lower threshold value. (WINLT)
// <id> adc_arch_winlt
#ifndef CONF_ADC_0_WINLT
#define CONF_ADC_0_WINLT 0
#endif

// <o> Window Monitor Upper Threshold <0-65535>
// <i> If the window monitor is enabled, these bits define the lower threshold value. (WINUT)
// <id> adc_arch_winut
#ifndef CONF_ADC_0_WINUT
#define CONF_ADC_0_WINUT 0
#endif

// <o> Bitmask for positive input sequence <0-4294967295>
// <i> Use this parameter to input the bitmask for positive input sequence control (refer to datasheet for the device).
// <id> adc_arch_seqen
#ifndef CONF_ADC_0_SEQEN
#define CONF_ADC_0_SEQEN 0x0
#endif

// </e>

// <e> Event Control
// <id> adc_arch_event_settings
#ifndef CONF_ADC_0_EVENT_CONTROL
#define CONF_ADC_0_EVENT_CONTROL 0
#endif

// <q> Window Monitor Event Out
// <i> Enables event output on window event (WINMONEO)
// <id> adc_arch_winmoneo
#ifndef CONF_ADC_0_WINMONEO
#define CONF_ADC_0_WINMONEO 0
#endif

// <q> Result Ready Event Out
// <i> Enables event output on result ready event (RESRDEO)
// <id> adc_arch_resrdyeo
#ifndef CONF_ADC_0_RESRDYEO
#define CONF_ADC_0_RESRDYEO 0
#endif

// <q> Invert flush Event Signal
// <i> Invert the flush event input signal (FLUSHINV)
// <id> adc_arch_flushinv
#ifndef CONF_ADC_0_FLUSHINV
#define CONF_ADC_0_FLUSHINV 0
#endif

// <q> Trigger Flush On Event
// <i> Trigger an ADC pipeline flush on event (FLUSHEI)
// <id> adc_arch_flushei
#ifndef CONF_ADC_0_FLUSHEI
#define CONF_ADC_0_FLUSHEI 0
#endif

// <q> Invert Start Conversion Event Signal
// <i> Invert the start conversion event input signal (STARTINV)
// <id> adc_arch_startinv
#ifndef CONF_ADC_0_STARTINV
#define CONF_ADC_0_STARTINV 0
#endif

// <q> Trigger Conversion On Event
// <i> Trigger a conversion on event. (STARTEI)
// <id> adc_arch_startei
#ifndef CONF_ADC_0_STARTEI
#define CONF_ADC_0_STARTEI 0
#endif

// </e>

// <<< end of configuration section >>>

#endif // HPL_ADC_CONFIG_H

======================
ADC Synchronous driver
======================

An ADC (Analog-to-Digital Converter) converts analog signals to digital values.
A reference signal with a known voltage level is quantified into equally
sized chunks, each representing a digital value from 0 to the highest number
possible with the bit resolution supported by the ADC. The input voltage
measured by the ADC is compared against these chunks and the chunk with the
closest voltage level defines the digital value that can be used to represent
the analog input voltage level.

Usually an ADC can operate in either differential or single-ended mode.
In differential mode two signals (V+ and V-) are compared against each other
and the resulting digital value represents the relative voltage level between
V+ and V-. This means that if the input voltage level on V+ is lower than on
V- the digital value is negative, which also means that in differential
mode one bit is lost to the sign. In single-ended mode only V+ is compared
against the reference voltage, and the resulting digital value can only be
positive, but the full bit-range of the ADC can be used.

Usually multiple resolutions are supported by the ADC, lower resolution can
reduce the conversion time, but lose accuracy.

Some ADCs has a gain stage on the input lines which can be used to increase the
dynamic range. The default gain value is usually x1, which means that the
conversion range is from 0V to the reference voltage.
Applications can change the gain stage, to increase or reduce the conversion
range.

The window mode allows the conversion result to be compared to a set of
predefined threshold values. Applications can use callback function to monitor
if the conversion result exceeds predefined threshold value.

Usually multiple reference voltages are supported by the ADC, both internal and
external with difference voltage levels. The reference voltage have an impact
on the accuracy, and should be selected to cover the full range of the analog
input signal and never less than the expected maximum input voltage.

There are two conversion modes supported by ADC, single shot and free running.
In single shot mode the ADC only make one conversion when triggered by the
application, in free running mode it continues to make conversion from it
is triggered until it is stopped by the application. When window monitoring,
the ADC should be set to free running mode.

Features
--------
* Initialization and de-initialization
* Support multiple Conversion Mode, Single or Free run
* Start ADC Conversion
* Read Conversion Result

Applications
------------
* Measurement of internal sensor. E.g., MCU internal temperature sensor value.
* Measurement of external sensor. E.g., Temperature, humidity sensor value.
* Sampling and measurement of a signal. E.g., sinusoidal wave, square wave.

Dependencies
------------
* ADC hardware

Concurrency
-----------
N/A

Limitations
-----------
N/A

Knows issues and workarounds
----------------------------
N/A


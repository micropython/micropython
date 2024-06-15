# PWM testing

This directory contains tools for testing PWM behavior in CircuitPython.

1.  [CircuitPython PWM test code](code.py) is run on the board to be tested.
2.  As the code runs, the state of the PWM signal is logged by a logic analyzer (here Saleae Logic Pro 8).
3.  Data collected by the logic analyzer is analyzed and plotted into a PNG image by [CPython code](duty.py).

Here is a sample plot with key features annotated:

<img src="pwm_plot.explainer.png">



<details open>
<summary>

# PWM expected behavior

</summary>

(1)  A PWM signal has a period (defined as the time between rising edges) and a duty cycle (defined as the ratio of the time between a rising edge and the next falling edge, divided by the period).  In a typical application the PWM period will be constant while the duty cycle changes frequently.

(2)  An exception to (1) is that CircuitPython explicitly supports duty cycles of 0% and 100%, where the signal stays constant at a low/high level.  In the CP API duty cycle is always specified as a 16-bit value, where 0 corresponds to 0%, 0xFFFF corresponds to 100%, and values in between scale accordingly.

(3)  While in the PWM API the duty cycle is specified as a 16-bit value and the period is specified by a 32-bit frequency value, a given processor may not be able to provide a signal with that precision, but will do its best to approximate what is asked for to the best of its ability.  The actual PWM duty and frequency settings resulting from the requested parameters can be obtained from the API.

(4)  The user can set the duty cycle and frequency (if initialized with `variable_frequency=True`) at any time.  Changes in duty cycle and frequency should appear in the PWM signal as soon as possible after the set function is invoked.

(5)  Changes in duty cycle should ideally never result in output glitches - that is, the duty cycle of output PWM should never take on a value which has not been set by the user.

(6)  Changes in frequency may (and will usually) result in a transient glitch in duty cycle.  PWM hardware is generally not designed for glitch-free frequency transitions.

(7)  PWM frequency and duty cycle should be jitter-free.

(8)  Setting of PWM frequency and duty cycle should be non-blocking operations which return control to the caller as soon as possible.

(9)  As a corollary to (2), PWM settings of 0 and 0xFFFF should be the ONLY settings which result in always low/always high PWM.
 Other settings should always provide an oscillating signal.
</details>

# Test method

To test all of the elements of expected behavior listed above, I exercised the PWM API and captured the resulting output signal on a logic analyzer (Seleae Logic Pro 8).  Logic analyzer data was then analyzed and plotted.

Here is the CP code.  It loops through a list of PWM frequencies, staying at each frequency for a specified interval, typically 1 second.    At each frequency it repeatedly cycles through a list of duty cycles in a tight loop, updating the duty cycle as frequently as it is able.
For the tests I show here, I used a set of 13 frequencies logarithmically spaced between 1000 Hz and 10 MHz, and duty cycles of 1/9, 2/9, 3/9, 4/9, 5/9, 6/9, 7/9, 8/9, selected to stay away from "special numbers" like 0, 65535, and powers of two.
<details>
<summary> CircuitPython code</summary>

```python
import board
import pwmio
import random
import time
import microcontroller
import os
import sys

cr10 = 10**(1/3)

freqs = [int(f) for f in [1e3, 1e3*cr10, 1e3*cr10*cr10,
                          1e4, 1e4*cr10,
                          1e4*cr10*cr10,
                          1e5, 1e5*cr10, 1e5*cr10*cr10,
                          1e6, 1e6*cr10, 1e6*cr10*cr10,
                          10000000]]

top = 65536
duties = [int(top * frac) for frac in [ 1/9, 8/9, 2/9, 7/9, 3/9, 6/9, 4/9, 5/9 ]]
# duties = [int(top * frac) for frac in [ 1/9 ]]

freq_duration = 1.0
duty_duration = 0.00000001
start_duty = int(65535*0.1)

print('\n\n')
board_name = sys.implementation[2]

pins = {"RP2040-Zero": (("GP15", ""),),
        "Grand Central": (("D51", "TCC"), ("A15", "TC")),
        "Metro M0": (("A2", "TC"), ("A3", "TCC")),
        "ESP32-S3-DevKit": (("IO6", ""),), # marked D5 on board for XIAO-ESP32-s3
        "XIAO nRF52840": (("D9", ""),),
       }

for board_key in pins:
    if board_key in board_name:
        pins_to_use = pins[board_key]
        break

while True:
    for pin_name, pin_type in pins_to_use:
        pin = getattr(board, pin_name)
        print('title="', end="")
        print(f"{board_name} at {microcontroller.cpu.frequency} Hz, pin {pin_name}", end="")
        if len(pin_type) > 0:
            print(f" ({pin_type})", end="")
        print('",')
        print(f'subtitle="{freq_duration:0.1f}s per frequency",')
        print(f'version="{sys.version}",')
        print('freq_calls=', end="")
        pwm = pwmio.PWMOut(pin, variable_frequency=True)
        t0 = time.monotonic()
        duty_time = t0 + duty_duration
        print('(', end='')
        for freq in freqs:
            i = 0
            try:
                pwm.frequency = freq
            except ValueError:
                break;
            pwm.duty_cycle = start_duty
            freq_time = t0 + freq_duration
            duty_time = t0 + duty_duration
            while time.monotonic() < freq_time:
                for duty in duties:
                    while time.monotonic() < duty_time and time.monotonic() < freq_time:
                        pass
                    pwm.duty_cycle = duty
                    duty_time += duty_duration
                    i += 1
                    if time.monotonic() > freq_time:
                        break
            t0 = freq_time
            print(f'({freq}, {i/freq_duration:.0f}), ', end='')
        print(')')
        print('done.')
        pwm.deinit()
        time.sleep(5)
```
</details>

Here is the Python code used to analyze the captured logic analyzer data (typically ~300MB).  The plots generated display the frequency and duty cycle of every captured PWM cycle (typically 16M points).

<details>
<summary>Data analysis code</summary>

```python
import math
import numpy as np
from PIL import Image
import matplotlib.pyplot as plt
from PIL import Image
from PIL import ImageFont
from PIL import ImageDraw

def read(filename, image_filename=None, height=500, width=500, f_min=100, f_max=1e8, title='', subtitle='', version='',
         duty_labels=(0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9), freq_calls=tuple(), margin=0.01,
         duty_color = (255, 0, 0), freq_color = (0, 255, 0), calls_color = (0, 255, 255), title_color = (255, 255, 255)):
    """Read a one channel logic analyzer raw csv data file and generate a plot visualizing the PWM signal
       captured in the file.  Each line of the file is a <time, level> pair indicating the times (in seconds)
       at which the signal transitioned to that level.  For example:
                              1.726313020,0
                              1.726313052,1
                              1.726313068,0
                              1.727328804,1
    """
    rising_edge = None
    falling_edge = None
    pixels = np.zeros((height, width, 3), dtype=np.uint8) * 255
    t0 = None
    t1 = None
    with open(filename, 'r') as f:
        for line in f:
            t, val = line.split(',')
            try:
                t = float(t)
                val = int(val)
            except ValueError:
                val=None
            if val is not None:
                if t0 is None or t<t0:
                    t0 = t
                if t1 is None or t>t1:
                    t1 = t
    with open(filename, 'r') as f:
        pts = 0
        f_log_max = int(math.log10(f_max))
        f_log_min = int(math.log10(f_min))
        f_log_span = f_log_max - f_log_min
        for line in f:
            t, val = line.split(',')
            try:
                t = float(t)
                val = int(val)
            except ValueError:
                val=None
            if val==1:
                if falling_edge is not None and rising_edge is not None:
                    period = t - rising_edge
                    frequency = 1/period
                    duty_cycle = (falling_edge - rising_edge) / period
                    x = int(width * (t - t0)/(t1 - t0))
                    y_duty = int((1 - duty_cycle) * height)
                    y_freq = int(height * (1 - (math.log10(frequency) - f_log_min) / f_log_span))
                    x = max(0, min(x, width - 1))
                    y_duty = max(0, min(y_duty, height - 1))
                    y_freq = max(0, min(y_freq, height - 1))
                    pixels[y_duty, x] = duty_color
                    pixels[y_freq, x] = freq_color
                    pts += 1
                rising_edge = t
            elif val==0:
                falling_edge = t
        image = Image.fromarray(pixels)
        draw = ImageDraw.Draw(image)
        draw.text((width*margin, height * (1 - margin)), 'Duty cycle', duty_color, anchor='lb')
        draw.text((width * 0.5, height * (1 - margin)), 'Call throughput', calls_color, anchor='mb')
        draw.text((width*(1-margin), height * (1 - margin)), 'PWM frequency', freq_color, anchor='rb')

        for duty in duty_labels:
            draw.text((width*margin, height * (1 - duty)), f'{int(100*duty):d}%', duty_color, anchor='lm')
        for exponent in range(f_log_min+1, f_log_max):
            draw.text((width*(1-margin), height * (1 - (exponent - f_log_min) / (f_log_span))),
                      str(10**exponent) + ' Hz', freq_color, anchor='rm')
        for freq, count in freq_calls:
            draw.text((width * 0.5, height * (1 - (math.log10(freq) - f_log_min) / (f_log_span))),
                      f'{count} Hz', calls_color, anchor='mm')
        subtitle += f', showing {pts} PWM cycles'
        draw.text((width*0.5, height * margin), title, title_color, anchor='mm')
        draw.text((width*0.5, height * 4 * margin), version, title_color, anchor='mm')
        draw.text((width*0.5, height * 8 * margin), subtitle, title_color, anchor='mm')
        image.show()
        if image_filename is not None:
            image.save(image_filename)
    return image
```
</details>

The plots this generates are pretty dense.  Here's one, measured on pin 51 of a Metro M4 Grand Central board, annotated to show what is going on:

<img src="https://github.com/timchinowsky/circuitpython/assets/5445541/a374d6a9-77ea-4541-933b-f775b5fa0f6a" />

The PWM performance here looks pretty great except for the thing that got me started on this in the first place - the call to set the PWM duty cycle blocks for a time equal to the PWM period.

# Measurements on samd21/51 port

Investigation of PWM on the CP samd21/51 port is complicated by the port's inclusion of two different processor families, each of which have two different types of PWM peripherals (TC and TCC).  Moreover, the TC peripheral for the samd21 is not identical to that of the samd51, as is hinted at by these datasheet excerpts:

<details>
<summary>Datasheet summaries of samd21 and samd51 TC and TCC peripherals</summary>
<img src="https://github.com/timchinowsky/circuitpython/assets/5445541/ba9b38a0-72b1-47b5-a457-8cd8325b1c7a" />
</details>

The description of TC for the samd51 notably adds mention of double-buffering, while the samd21/51 descriptions for TCC are the same.

For samd21 and samd51 boards, TC and TCC were chosen by selecting pins which support only one or the other.  Here the pins are outlined in red:

<details>
<summary>Pins implementing TC and TCC on samd21/51</summary>
<img src="https://github.com/timchinowsky/circuitpython/assets/5445541/ca7baa7d-d1be-4b72-922e-4d5ce918be15" />
</details>

Here is an analysis of data gathered from each of the four types of PWM peripheral using the software described above:

|          | samd21 | samd51 |
| ------  | ----------- | ---------- |
| TC    |  ![all_samd21_tc](https://github.com/timchinowsky/circuitpython/assets/5445541/03d23303-f346-4e3b-a14e-7fdf1c0dc44f) | ![all_samd51_tc](https://github.com/timchinowsky/circuitpython/assets/5445541/9de21857-812f-4081-9fa3-68ebb3125ec7)  |
| TCC  | ![all_samd21_tcc](https://github.com/timchinowsky/circuitpython/assets/5445541/ccc69f9e-0ed7-4401-bdd0-6588a71c6b5d) | ![all_samd51_tcc](https://github.com/timchinowsky/circuitpython/assets/5445541/529b4500-0fbf-4357-ad67-9a2d7353ffa9) |

Key findings:

* Data from both samd51 peripherals looks good, except for the blocking issue (variation in execution time with PWM frequency)
* Data from samd21 TCC looks good, except for errant frequency measurements at the top two frequencies (pointed out with white arrow).  Unlike samd51, execution time does not depend on PWM frequency.
* Data from samd21 TC has a lot of weirdness at all frequencies.

# Code analysis








, https://github.com/adafruit/circuitpython/issues/1644 and to port `audiopwmio` to samd21/51.



<details>
<summary>Datasheet excerpts</summary>
<img src="https://github.com/timchinowsky/circuitpython/assets/5445541/50651bef-e2a2-4a6b-8982-f5adad296f5b" />
</details>

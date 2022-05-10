# The MIT License (MIT)
#
# Copyright (c) 2017, 2018 Scott Shawcroft for Adafruit Industries
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
"""
`adafruit_rtttl`
====================================================

Play notes to a digialio pin using ring tone text transfer language (rtttl).

* Author(s): Scott Shawcroft
"""

__version__ = "2.4.2"
__repo__ = "https://github.com/adafruit/Adafruit_CircuitPython_RTTTL"

import sys
import time
import pwmio

AUDIOIO_AVAILABLE = False
try:
    import audioio
    from adafruit_waveform import sine

    AUDIOIO_AVAILABLE = True
    try:
        import audiocore
    except ImportError:
        audiocore = audioio
except ImportError:
    pass

PIANO = {
    "4c": 261.626,
    "4c#": 277.183,
    "4d": 293.665,
    "4d#": 311.127,
    "4e": 329.628,
    "4f": 349.228,
    "4f#": 369.994,
    "4g": 391.995,
    "4g#": 415.305,
    "4a": 440,
    "4a#": 466.164,
    "4b": 493.883,
    "5c": 523.251,
    "5c#": 554.365,
    "5d": 587.330,
    "5d#": 622.254,
    "5e": 659.255,
    "5f": 698.456,
    "5f#": 739.989,
    "5g": 783.991,
    "5g#": 830.609,
    "5a": 880,
    "5a#": 932.328,
    "5b": 987.767,
    "6c": 1046.50,
    "6c#": 1108.73,
    "6d": 1174.66,
    "6d#": 1244.51,
    "6e": 1318.51,
    "6f": 1396.91,
    "6f#": 1479.98,
    "6g": 1567.98,
    "6g#": 1661.22,
    "6a": 1760,
    "6a#": 1864.66,
    "6b": 1975.53,
    "7c": 2093,
    "7c#": 2217.46,
}


def _parse_note(note, duration=2, octave="6"):
    note = note.strip()
    piano_note = None
    note_duration = duration
    if note[0].isdigit() and note[1].isdigit():
        note_duration = int(note[:2])
        piano_note = note[2]
    elif note[0].isdigit():
        note_duration = int(note[0])
        piano_note = note[1]
    else:
        piano_note = note[0]
    if "." in note:
        note_duration *= 1.5
    if "#" in note:
        piano_note += "#"
    note_octave = octave
    if note[-1].isdigit():
        note_octave = note[-1]
    piano_note = note_octave + piano_note
    return piano_note, note_duration


def _get_wave(tune, octave):
    """Returns the proper waveform to play the song along with the minimum
    frequency in the song.
    """
    min_freq = 13000

    for note in tune.split(","):
        piano_note, _ = _parse_note(note, octave=octave)
        if piano_note[-1] != "p" and PIANO[piano_note] < min_freq:
            min_freq = PIANO[piano_note]
    return sine.sine_wave(16000, min_freq), min_freq


# pylint: disable-msg=too-many-arguments
def _play_to_pin(tune, base_tone, min_freq, duration, octave, tempo):
    """Using the prepared input send the notes to the pin
    """
    pwm = isinstance(base_tone, pwmio.PWMOut)
    for note in tune.split(","):
        piano_note, note_duration = _parse_note(note, duration, octave)
        if piano_note in PIANO:
            if pwm:
                base_tone.frequency = int(PIANO[piano_note])
                base_tone.duty_cycle = 2 ** 15
            else:
                # AudioOut interface changed in CP 3.x
                if sys.implementation.version[0] >= 3:
                    pitch = int(PIANO[piano_note])
                    sine_wave = sine.sine_wave(16000, pitch)
                    sine_wave_sample = audiocore.RawSample(sine_wave)
                    base_tone.play(sine_wave_sample, loop=True)
                else:
                    base_tone.frequency = int(16000 * (PIANO[piano_note] / min_freq))
                    base_tone.play(loop=True)

        time.sleep(4 / note_duration * 60 / tempo)
        if pwm:
            base_tone.duty_cycle = 0
        else:
            base_tone.stop()
        time.sleep(0.02)


# pylint: disable-msg=too-many-arguments
def play(pin, rtttl, octave=None, duration=None, tempo=None):
    """Play notes to a digialio pin using ring tone text transfer language (rtttl).
    :param ~digitalio.DigitalInOut pin: the speaker pin
    :param rtttl: string containing rtttl
    :param int octave: represents octave number (default 6 starts at middle c)
    :param int duration: length of notes (default 4 quarter note)
    :param int tempo: how fast (default 63 beats per minute)
    """
    _, defaults, tune = rtttl.lower().split(":")
    for default in defaults.split(","):
        if default[0] == "d" and not duration:
            duration = int(default[2:])
        elif default[0] == "o" and not octave:
            octave = default[2:]
        elif default[0] == "b" and not tempo:
            tempo = int(default[2:])
    if not octave:
        octave = 6
    if not duration:
        duration = 4
    if not tempo:
        tempo = 63

    base_tone = None
    min_freq = 440
    if AUDIOIO_AVAILABLE:
        wave, min_freq = _get_wave(tune, octave)
        try:
            # AudioOut interface changed in CP 3.x; a waveform if now pass
            # directly to .play(), generated for each note in _play_to_pin()
            if sys.implementation.version[0] >= 3:
                base_tone = audioio.AudioOut(pin)
            else:
                base_tone = audioio.AudioOut(pin, wave)
        except ValueError:
            # No DAC on the pin so use PWM.
            pass

    # Fall back to PWM
    if not base_tone:
        base_tone = pwmio.PWMOut(pin, duty_cycle=0, variable_frequency=True)

    _play_to_pin(tune, base_tone, min_freq, duration, octave, tempo)

    base_tone.deinit()

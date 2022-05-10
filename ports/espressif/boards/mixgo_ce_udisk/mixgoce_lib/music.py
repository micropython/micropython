"""
Music

CircuitPython library for Music - MixGoCE
=======================================================

Small Cabbage
20210721
"""


import time
from mixgoce import version

class Music:
    def __init__(self, pin, duration='4', octave='6', tempo='63'):
        import pwmio
        self.pin = pin
        self.pwm = pwmio.PWMOut(pin, duty_cycle=0, variable_frequency=True)
        self.duration = duration
        self.octave = octave
        self.tempo = tempo

    def set_duration(self, duration='4'):
        self.duration = duration

    def set_octave(self, octave='6'):
        self.octave = octave

    def sef_tempo(self, tempo='63'):
        self.tempo = tempo

    def set_duration_tempo(self, duration, tempo):#这项等于原来的set_tempo
        self.duration = duration
        self.tempo = tempo

    def reset(self):
        self.set_duration()
        self.set_octave()
        self.set_tempo()

    def get_duration(self):
        return self.duration

    def get_octave(self):
        return self.octave

    def get_tempo(self):
        return (self.tempo)

    def play_demo(self, demo):
        import adafruit_rtttl
        if self.pwm:
            self.pwm.deinit()
        adafruit_rtttl.play(self.pin, demo)

    def play_demo_print(self, demo):
        import adafruit_rtttl
        if self.pwm:
            self.pwm.deinit()
        adafruit_rtttl.play(self.pin, demo)
        _, defaults, tune = demo.lower().split(":")
        print(tune)

    def play(self, note, duration=None):
        import pwmio
        if self.pwm:
            self.pwm.deinit()
        self.pwm = pwmio.PWMOut(self.pin, duty_cycle=0, variable_frequency=True)
        self.pwm.frequency = int(note)
        self.pwm.duty_cycle = 2 ** 15
        if duration:
            # print(1)
            time.sleep(duration/1000)
            self.pwm.duty_cycle = 0
    
    def stop(self):
        self.pwm.duty_cycle = 0
        self.pwm.deinit()

BA_DING = "itchy:d=4,o=4,b=300:4b5,2g5"
JUMP_UP = "itchy:d=4,o=5,b=300:c,d,e,f,g"
JUMP_DOWN = "itchy:d=4,o=5,b=300:g,f,e,d,c"
POWER_UP = "itchy:d=4,o=4,b=300:4g4,4c5,4e5,2g5,4e5,1g5"
POWER_DOWN = "itchy:d=4,o=4,b=300:4g5,4d#5,4c5,2g4,4b4,1c5"
DADADADUM = "itchy:d=4,o=4,b=200:2p,g,g,g,1d#,p,f,f,f,1d"
#ENTERTAINER = "itchy:d=4,o=4,b=300:4d4,d#4,e4,2c5,4e4,2c5,4e4,1c5,c5,d5,d#5,e5,c5,d5,2e5,b4,2d5,1c5"
BIRTHDAY = "itchy:d=4,o=4,b=180:6c,12c,4d,4c,4f,2e,6c,12c,4d,4c,4g,2f,6c,12c,4c5,4a,4f,4e,4d,6a#,12a#,4a,4f,4g,2f"
#BLUES = "itchy:d=4,o=4,b=400:2c,4e,4g,4a,4a#,4a,4g,4e,2c,4e,4g,4a,4a#,4a,4g,4e,4f,4a,2c,4d,4d#,4d,4c,4a,2c,4e,4g,4a,4a#,4a,4g,4e,4g,4b,4d,4f,4f,4a,4c,4d#,2c,e,g,e,g,f,e,d"
#PYTHON = "itchy:d=4,o=4,b=380:d5,4b4,p,b4,b4,a#4,b4,g5,p,d5,d5,r,b4,c5,p,c5,c5,p,d5,1e5,p,c5,a4,p,a4,a4,g#4,a4,f#5,p,e5,e5,p,c5,b4,p,b4,b4,p,c5,1d5,p,d5,4b4,p,b4,b4,a#4,b4,b5,p,g5,g5,p,d5,c#5,p,a5,a5,p,a5,1a5,p,g5,f#5,p,a5,a5,g#5,a5,e5,p,a5,a5,g#5,a5,d5,p,c#5,d5,p,c#5,2d5,3p"

import board

if version:#new
    buzzer = Music(board.IO40)
else:#old
    from digitalio import DigitalInOut, Direction

    buzzer = Music(board.IO17)
    spk_en = DigitalInOut(board.IO40)
    spk_en.direction = Direction.OUTPUT
    spk_en.value = True
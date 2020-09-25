# Dev by Sonthaya Nongnuch

from machine import Pin, PWM
from time import sleep

note_map = {
    "C4": 261,
	"C#4": 277,
	"D4": 293,
	"Eb4": 311,
	"E4": 329,
	"F4": 349,
	"F#4": 369,
	"G4": 391,
	"G#4": 415,
	"A4": 440,
	"Bb4": 466,
	"B4": 493,
	"C5": 523,
	"C#5": 554,
	"D5": 587,
	"Eb5": 622,
	"E5": 659,
	"F5": 698,
	"F#5": 740,
	"G5": 784,
	"G#5": 831,
	"A5": 880,
	"Bb5": 932,
	"B5": 988,
	"C6": 1046,
	"C#6": 1109,
	"D6": 1175,
	"Eb6": 1244,
	"E6": 1318, 
	"F6": 1396,
	"F#6": 1480,
	"G6": 1568,
	"G#6": 1661,
	"A6": 1760,
	"Bb6": 1865,
	"B6": 1976,
	"C7": 2093,
    "SIL": 0
}

__buzzer = PWM(Pin(13), freq=2000, duty=0)
volume = 50
bpm = 120

def tone(freq=2093, duration=0.5):
    __buzzer.freq(int(freq))
    __buzzer.duty(int(volume / 100 * 512))
    sleep(duration)
    __buzzer.duty(0)

def on(freq=2000):
    __buzzer.freq(int(freq))
    __buzzer.duty(int(volume / 100 * 512))

def off():
    __buzzer.duty(0)

def note(notes, duration=4):
    quarter_delay = (60 * 1000) / bpm
    delay = quarter_delay * duration
    delay = delay / 1000 # mS -> S
    for note in notes.split(" "):
        tone(note_map[note], delay)


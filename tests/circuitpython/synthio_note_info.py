from synthio import Synthesizer, Note, Envelope
from audiocore import get_buffer

s = Synthesizer()
n = Note(440, envelope=Envelope())
print("{} {:.2f}".format(*s.note_info(n)))
s.press(n)
print("press")
for _ in range(9):
    print("{} {:.2f}".format(*s.note_info(n)))
    get_buffer(s)

s.release(n)
print("release")
for _ in range(11):
    print("{} {:.2f}".format(*s.note_info(n)))
    get_buffer(s)

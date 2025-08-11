import mactypes
import textedit
import toolboxevent
import deskmgr

teJustCenter = 1
textRect = mactypes.Rect(135, 10, 270, 487)
text = bytearray(b"\r".join([b"Hello, World", b"This is MicroPython calling TETextBox", b"", b"Click to exit"]))
textedit.TETextBox(text, len(text), textRect, textedit.teJustCenter)


while not toolboxevent.Button():
    deskmgr.SystemTask()  # scott added - slows it down on fast machines

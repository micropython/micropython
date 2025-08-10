import mkapitest as mactypes
import mkapitest as textedit

teJustCenter = 1
textRect = mactypes.Rect(135, 10, 270, 487)
text = bytearray(b"\n".join([b"Hello, World", b"This is another line"]))
textedit.TETextBox(text, len(text), textRect, teJustCenter)

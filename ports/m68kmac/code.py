# m68k-micropython window creation demo

import eventmgr
import mactypes
import qd
import uctypes
import windowmgr
import random

scrn = qd.qdGlobals().screenBits


def pstr(s):
    b = mactypes.Str255()
    b[0] = len(s)
    for i in range(len(s)):
        b[i + 1] = ord(s[i])
    return uctypes.struct(b, mactypes.ConstStringPtr)


ev = eventmgr.EventRecord()

NIL_WINDOW = uctypes.struct(0, qd.GrafPtr)
ABOVE_ALL_WINDOWS = uctypes.struct(-1, qd.GrafPtr)

title = pstr("Hello World 11")
r = mactypes.Rect()
r[:] = scrn.bounds
r.top += 80
r.left += 100
qd.InsetRect(r, 25, 25)

w = windowmgr.NewWindow(NIL_WINDOW, r, title, True, 0, ABOVE_ALL_WINDOWS, True, 0)
qd.SetPort(w)

g = qd.qdGlobals()

barbell = qd.NewRgn()
tempRect = mactypes.Rect()
qd.OpenRgn()
qd.SetRect(tempRect, 20, 20, 30, 50)
qd.FrameOval(tempRect)
qd.SetRect(tempRect, 25, 30, 85, 40)
qd.FrameRect(tempRect)
qd.SetRect(tempRect, 80, 20, 90, 50)
qd.FrameOval(tempRect)
qd.CloseRgn(barbell)

r[:] = barbell[0].rgnBBox
print(f"region size {barbell[0].rgnSize} bbox {r.top},{r.left}..{r.right},{r.bottom}")
qd.FillRgn(barbell, g.black)

qd.DisposeRgn(barbell)

input("hit enter to exit")

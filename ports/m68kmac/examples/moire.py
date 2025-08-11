# m68k-micropython window creation demo

import eventmgr
import mactypes
import qd
import uctypes
import windowmgr


def let(dst, src):
    memoryview(dst)[:] = memoryview(src)[:]


def pstr(s):
    b = mactypes.Str255()
    b[0] = len(s)
    for i in range(len(s)):
        b[i + 1] = ord(s[i])
    return b


ev = eventmgr.EventRecord()

NIL_WINDOW = uctypes.struct(0, qd.GrafPort)
ABOVE_ALL_WINDOWS = uctypes.struct(-1, qd.GrafPort)

title = pstr("Hello World")
r = mactypes.Rect()
scrn = qd.qdGlobals().screenBits
let(r, scrn.bounds)
r.top += 80
qd.InsetRect(r, 25, 25)

w = windowmgr.NewWindow(NIL_WINDOW, r, title, True, 0, ABOVE_ALL_WINDOWS, False, 0)
windowmgr.ShowWindow(w)
let(r, w.portRect)
qd.SetPort(w)

mid_x = (r.left + r.right) // 2
mid_y = (r.top + r.bottom) // 2
for i in range(r.left, r.right, 2):
    qd.MoveTo(mid_x, r.bottom)
    qd.LineTo(i, r.top)

qd.MoveTo(mid_x, mid_y)
input("hit enter to exit")

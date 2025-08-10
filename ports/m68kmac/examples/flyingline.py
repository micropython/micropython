"""
FlyingLine
from "Macintosh C Programming Primer"
written by Dave Mark & Cartwright Reed
(c)1992 by Addison-Wesley Publishing Company

Ported to Micropython m68k by Scott Small
"""

# TODO: make this more Pythonic. It's pretty much a straight port of the C code
#  right now, heavily relying on global variables.

# Imports
import deskmgr
import mactypes
import menumgr
import qd
import random
import time
import toolboxevent
import uctypes
import windowmgr


# Globals
kNumLines = 50          # Try 100 or 150!
gDeltaLeft = 2          # These four
gDeltaTop = 3           # are the
gDeltaRight = 6         # key to
gDeltaBottom = 3        # FlyingLine!
gLines = [mactypes.Rect()]
gOldMBarHeight = None


def main():
    """
    Application entry point
    """
    WindowInit()
    LinesInit()
    MainLoop()


def pstr(s):
    """
    Converts a Python string into a Pascal string
    """
    b = mactypes.Str255()
    b[0] = len(s)
    for i in range(len(s)):
        b[i + 1] = ord(s[i])
    return uctypes.struct(b, mactypes.ConstStringPtr)


def WindowInit():
    """
    Initializes the full-screen window used to display the app
    """
    global gOldMBarHeight

    gOldMBarHeight = menumgr.LMGetMBarHeight()
    newMBarHeight = menumgr.LMGetMBarHeight()
    newMBarHeight[1] = 0
    menumgr.LMSetMBarHeight(newMBarHeight)

    NIL_WINDOW = uctypes.struct(0, qd.GrafPtr)
    kMoveToFront = uctypes.struct(-1, qd.GrafPtr)
    kEmptyTitle = pstr("")
    kVisible = True
    kNoGoAway = False
    kNilRefCon = 0

    screenBits = qd.qdGlobals().screenBits
    window = windowmgr.NewWindow(
        NIL_WINDOW,
        screenBits.bounds,
        kEmptyTitle,
        kVisible,
        windowmgr.plainDBox,
        kMoveToFront,
        kNoGoAway,
        kNilRefCon,
    )

    mBarRect = mactypes.Rect(
        screenBits.bounds.top,
        screenBits.bounds.left,
        screenBits.bounds.top + gOldMBarHeight[1],
        screenBits.bounds.right,
    )

    mBarRgn = qd.NewRgn()
    qd.RectRgn(mBarRgn, mBarRect)
    qd.UnionRgn(window.visRgn, mBarRgn, window.visRgn)
    qd.DisposeRgn(mBarRgn)
    qd.SetPort(window)
    qd.FillRect(window.portRect, qd.qdGlobals().black)  # Change black to ltGray
    qd.PenMode(qd.patXor)                               # <-- and comment out this line!


def LinesInit():
    """
    Initializes the lines that will be moving around
    """
    global gLines, kNumLines

    qd.HideCursor()
    random.seed(time.time())
    RandomRect(gLines[0])
    DrawLine(0)

    for i in range(1, kNumLines):
        gLines.append(
            mactypes.Rect(
                gLines[i - 1].top, gLines[i - 1].left, gLines[i - 1].bottom, gLines[i - 1].right
            )
        )
        RecalcLine(i)
        DrawLine(i)


def MainLoop():
    """
    Application main loop - runs continuously after initialization until mouse button is pressed
    """
    global gLines, gOldMBarHeight, kNumLines

    while not toolboxevent.Button():
        DrawLine(kNumLines - 1)
        for i in range(kNumLines-1, 0, -1):
            gLines[i] = mactypes.Rect(
                gLines[i - 1].top, gLines[i - 1].left, gLines[i - 1].bottom, gLines[i - 1].right
            )
        RecalcLine(0)
        DrawLine(0)
        DrawMsg()
        deskmgr.SystemTask()  # scott added - slows it down on fast machines
    menumgr.LMSetMBarHeight(gOldMBarHeight)


def RandomRect(rect):
    """
    Takes a Rect and assigns it random coordinates within the application window
    """
    window = windowmgr.FrontWindow()

    rect.left = Randomize(window.portRect.right - window.portRect.left)
    rect.right = Randomize(window.portRect.right - window.portRect.left)
    rect.top = Randomize(window.portRect.bottom - window.portRect.top)
    rect.bottom = Randomize(window.portRect.bottom - window.portRect.top)


def Randomize(theRange):
    """
    Generates a random number
    """
    kRandomUpperLimit = 32768

    randomNumber = random.randrange(kRandomUpperLimit)
    return (randomNumber * theRange) // kRandomUpperLimit


def RecalcLine(i):
    """
    Updates the position of a line
    """
    global gLines, gDeltaTop, gDeltaBottom, gDeltaLeft, gDeltaRight

    window = windowmgr.FrontWindow()

    gLines[i].top += gDeltaTop
    if (gLines[i].top < window.portRect.top) or (gLines[i].top > window.portRect.bottom):
        gDeltaTop *= -1
        gLines[i].top += 2 * gDeltaTop

    gLines[i].bottom += gDeltaBottom
    if (gLines[i].bottom < window.portRect.top) or (gLines[i].bottom > window.portRect.bottom):
        gDeltaBottom *= -1
        gLines[i].bottom += 2 * gDeltaBottom

    gLines[i].left += gDeltaLeft
    if (gLines[i].left < window.portRect.left) or (gLines[i].left > window.portRect.right):
        gDeltaLeft *= -1
        gLines[i].left += 2 * gDeltaLeft

    gLines[i].right += gDeltaRight
    if (gLines[i].right < window.portRect.left) or (gLines[i].right > window.portRect.right):
        gDeltaRight *= -1
        gLines[i].right += 2 * gDeltaRight


def DrawLine(i):
    """
    Draws a line to the screen
    """
    global gLines

    qd.MoveTo(gLines[i].left, gLines[i].top)
    qd.LineTo(gLines[i].right, gLines[i].bottom)


def DrawMsg():
    """
    Draws a message to the screen
    (not part of original FlyingLine demo - Scott added)
    """
    qd.TextFace(qd.bold)
    qd.TextSize(18)
    qd.TextMode(3)
    qd.MoveTo(24, 24)
    qd.DrawString(pstr("Hello, Micropython!"))
    qd.MoveTo(24, 48)
    qd.DrawString(pstr("Click Mouse to Exit"))


main()

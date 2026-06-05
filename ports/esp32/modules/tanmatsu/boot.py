from machine import SDCard
import uos
import display
import badgebsp
import mascot

sd = SDCard()
uos.mount(sd, "/sd")
display.orientation(270)
app = badgebsp.get_bootsel_arg()
if app and not app == "/sd/apps/nl.mansoft.micropython":
    if app.endswith("/__init__.py"):
        app = app[:len("/__init__.py")]
    index = app.rindex('/')
    directory = app[:index]
    app = app[index + 1:]
    sys.path.append(directory)
    __import__(app)
else:
    display.drawFill(0xFFFFFF)
    snek_info = display.pngInfo(mascot.snek)
    display.drawPng((display.width() - snek_info[0]) // 2, (display.height() - snek_info[1]) // 2 - (snek_info[1] // 2) - 10, mascot.snek)
    text = "Interactive Python shell now active on USB console"
    display.drawText((display.width() - display.getTextWidth(text)) // 2, (display.height() - snek_info[1]) // 2 + (snek_info[1] // 2) + 10, text, 0x000000)
    display.flush()

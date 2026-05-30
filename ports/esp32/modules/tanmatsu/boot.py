from machine import SDCard
import uos
import display
import badgebsp

sd = SDCard()
uos.mount(sd, "/sd")
display.orientation(270)
app = badgebsp.get_bootsel_arg()
if app and not app == "shell":
    index = app.rindex('/')
    directory = app[:index]
    app = app[index + 1:]
    sys.path.append(directory)
    __import__(app)

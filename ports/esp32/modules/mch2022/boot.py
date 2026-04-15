import machine, sys, system, time, display, mascot
import _device as device
import rtcmem
import esp32

rtcmem.write(0,0)
rtcmem.write(1,0)

device.prepareForWakeup()

app = rtcmem.read_string()

#if not app:
#	app = 'dashboard.launcher'

if app and not app == "shell":
	try:
		if app.startswith("/internal"):
			# /internal is mounted on / in Python
			app = app[len("/internal"):]
		if app.endswith("/__init__.py"):
			app = app[:len("/__init__.py")]
		print("Starting app '{}'...".format(app))
		system.__current_app__ = app
		if app:
			__import__(app)
	except KeyboardInterrupt:
		system.launcher()
	except BaseException as e:
		sys.print_exception(e)
		ignore = 0
		try:
			ignore = nvs.get_i32("ignore_crash")
		except Exception as e:
			ignore = 0
		if not ignore:
			print("Fatal exception in the running app!")
			system.crashedWarning()
			time.sleep(3)
			system.launcher()
else:
    display.drawFill(0xFFFFFF)
    snek_info = display.pngInfo(mascot.snek)
    display.drawPng((display.width() - snek_info[0]) // 2, (display.height() - snek_info[1]) // 2 - (snek_info[1] // 2) - 10, mascot.snek)
    text = "Interactive Python shell now active on USB console"
    display.drawText((display.width() - display.getTextWidth(text)) // 2, (display.height() - snek_info[1]) // 2 + (snek_info[1] // 2) + 10, text, 0x000000)
    display.flush()

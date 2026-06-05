import machine, sys, system, time
import _device as device
import rtcmem
import esp32

rtcmem.write(0,0)
rtcmem.write(1,0)

device.prepareForWakeup()

## Polyfill legacy APIs
def legacy_nvs_getstr(namespace, key): 
	print("machine.nvs_getstr() is deprecated, use valuestore.load(<namespace>, <key>) instead")
	return valuestore.load(namespace, key).decode("utf-8")
	
def legacy_nvs_setstr(namespace, key, value): 
	print("machine.nvs_setstr() is deprecated, use valuestore.save(<namespace>, <key>, <value>) instead")
	return valuestore.save(namespace, key, value)

def legacy_nvs_getint(namespace, key): 
	print("machine.nvs_getint() is deprecated, use valuestore.load(<namespace>, <key>) instead")
	return int(valuestore.load(namespace, key))
	
def legacy_nvs_setint(namespace, key, value): 
	print("machine.nvs_setint() is deprecated, use valuestore.save(<namespace>, <key>, <value>) instead")
	return valuestore.save(namespace, key, value)

# machine.nvs_getstr = legacy_nvs_getstr
# machine.nvs_setstr = legacy_nvs_setstr
# machine.nvs_getint = legacy_nvs_getint
# machine.nvs_setint = legacy_nvs_setint
## End polyfills


__chk_recovery = False
nvs = esp32.NVS("system")
try:
	fc_level = nvs.get_i32('factory_checked')
except Exception as e:
	fc_level = 0
recovery_button = None

if fc_level >= 3:
	try:
		import buttons
		try:
			#Use the START button if available
			recovery_button = buttons.BTN_START
		except:
			#Else use the B button
			recovery_button = buttons.BTN_B
		__chk_recovery = machine.wake_reason() == (7, 0) and buttons.value(recovery_button)
	except:
		pass

#Application starting
if __chk_recovery:
	app = "dashboard.recovery"
else:
	app = rtcmem.read_string()
	if not app:
		if fc_level < 3:
			app = "factory_checks"
		else:
			app = rtcmem.read_string()
			if not app:
				app = 'dashboard.home'
        del fc_level

del __chk_recovery
del recovery_button

if app and not app == "shell":
	try:
		print("Starting app '%s'..." % app)
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

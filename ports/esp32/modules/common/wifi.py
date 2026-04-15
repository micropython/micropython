import network, time, machine, nvs, consts
import ntp as ntpsync
import esp32

_STA_IF = network.WLAN(network.STA_IF)
_AP_IF  = network.WLAN(network.AP_IF)

_DEFAULT_TIMEOUT  = nvs.nvs_getint("system", "wifi.timeout") or 20
_DEFAULT_SSID     = nvs.nvs_getstr("system", "wifi.ssid")
_DEFAULT_PASSWORD = nvs.nvs_getstr("system", "wifi.password")

if not _DEFAULT_SSID:
	_DEFAULT_SSID     = consts.WIFI_SSID
	_DEFAULT_PASSWORD = consts.WIFI_PASSWORD

# STATION MODE
# ------------

def connect(*args):
	'''
	Connect to a WiFi network
	:param ssid: optional, ssid of network to connect to
	:param password: optional, password of network to connect to
	'''
	_STA_IF.active(True)
	if len(args) == 0:
		if _DEFAULT_PASSWORD:
			_STA_IF.connect(_DEFAULT_SSID, _DEFAULT_PASSWORD)
		else:
			_STA_IF.connect(_DEFAULT_SSID)
	elif len(args) == 1:
		_STA_IF.connect(args[0])
	elif len(args) == 2:
		_STA_IF.connect(args[0], args[1])
	else:
		raise Exception('Expected either 0 (default network), 1 (ssid) or 2 (ssid, password) parameters.')

def disconnect():
	'''
	Disconnect from the WiFi network
	'''
	if status():
		_STA_IF.disconnect()

def stop():
	'''
	Disconnect from the WiFi network and disable the station interface
	'''
	_STA_IF.disconnect()
	_STA_IF.active(False)

def status():
	'''
	Connection status of the station interface
	:return: boolean, connected
	'''
	return _STA_IF.isconnected()

def wait(duration=_DEFAULT_TIMEOUT):
	'''
	Wait until connection has been made to a network using the station interface
	:return: boolean, connected
	'''
	t = int(duration*10)
	while not status():
		if t <= 0:
			break
		t -= 1
		time.sleep(0.1)
	return status()

def scan():
	'''
	Scan for WiFi networks
	:return: list, wifi networks [SSID, BSSID, CHANNEL, RSSI, AUTHMODE1, AUTHMODE2, HIDDEN]
	'''
	_STA_IF.active(True)
	return _STA_IF.scan()

def ifconfig(newConfig=None):
	'''
	Get or set the interface configuration of the station interface
	:return: tuple, (ip, subnet, gateway, dns)
	'''
	if newConfig:
		return _STA_IF.ifconfig(newConfig)
	else:
		return _STA_IF.ifconfig()

# ACCESS POINT MODE
# -----------------

def accesspoint_start(ssid, password=None):
	'''
	Create a WiFi access point
	:param ssid: SSID of the network
	:param password: Password of the network (optional)
	'''
	if password and len(password) < 8:
		raise Exception("Password too short: must be at least 8 characters long")
	_AP_IF.active(True)
	if password:
		_AP_IF.config(essid=ssid, authmode=network.AUTH_WPA2_PSK, password=password)
	else:
		_AP_IF.config(essid=ssid, authmode=network.AUTH_OPEN)

def accesspoint_status():
	'''
	Accesspoint status
	:return: boolean, active
	'''
	return _AP_IF.active()

def accesspoint_stop():
	'''
	Disable the accesspoint
	'''
	_AP_IF.active(False)

def accesspoint_ifconfig(newConfig=None):
	'''
	Get or set the interface configuration of the accesspoint interface
	:return: tuple, (ip, subnet, gateway, dns)
	'''
	if newConfig:
		return _AP_IF.ifconfig(newConfig)
	else:
		return _AP_IF.ifconfig()

# EXTRAS
# -----------------

def ntp(onlyIfNeeded=True, server='pool.ntp.org'):
	'''
	Synchronize the system clock with NTP
	:return: boolean, synchronized
	'''
	if onlyIfNeeded and time.time() > 1482192000:
		return True #RTC is already set, sync not needed
	rtc = machine.RTC()
	if not status():
		return False # Not connected to a WiFi network
	return ntpsync.set_NTP_time()

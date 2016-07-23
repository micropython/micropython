# TiLDA Badge Bootstrap script
# Automatically downloads the app library to the badge via wifi
import network, pyb, usocket, machine, os, json, ugfx

def download(path, target):
	host = "api.badge.emfcamp.org"
	ai = usocket.getaddrinfo(host, 80) # ToDo: Change to 443 once we have https
	s = usocket.socket()
	s.connect(ai[0][4]) # .wrap_socket(s) # enable HTTPS
	s.send('GET /%s HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n' % (path, host))

	with open(target, 'wb') as f:
		state = 1
		hbuf = b"";
		remaining = None;
		while True:
			buf = s.recv(1024)
			if state == 1: # Status
				nl = buf.find(b"\n")
				if nl > -1:
					hbuf += buf[:nl - 1]
					status = hbuf.split(b' ')[1]
					if status != b"200":
						raise Exception("Invalid status " + str(status))

					state = 2
					hbuf = b"";
					buf = buf[nl + 1:]
				else:
					hbuf += buf

			if state == 2: # Headers
				hbuf += buf
				nl = hbuf.find(b"\n")
				while nl > -1:
					if nl < 2:
						if remaining == None:
							raise Exception("No Content-Length")
						buf = hbuf[2:]
						hbuf = None
						state = 3
						break

					header = hbuf[:nl - 1].decode("utf8").split(':', 3)
					if header[0] == "Content-Length":
						remaining = int(header[1].strip())

					hbuf = hbuf[nl + 1:]
					nl = hbuf.find(b"\n")

			if state == 3: # Content
				f.write(buf)
				remaining -= len(buf)
				if remaining < 1:
					break

			pyb.delay(50)

	s.close()

ugfx.init()
ugfx.clear(ugfx.BLACK)
ugfx.text(0, 0, "Downloading TiLDA software", ugfx.RED)
ugfx.text(0, 30, "Should this not work, try again by", ugfx.WHITE)
ugfx.text(0, 50, "pressing the reset button at the back", ugfx.WHITE)
ugfx.text(0, 100, "Please wait...", ugfx.WHITE)

def message(lines):
	y = 150
	ugfx.area(0, y, ugfx.width(), ugfx.height() - y, ugfx.BLACK)
	for line in lines:
		ugfx.text(0, y, line, ugfx.WHITE)
		y += 20

for d in ["apps", "apps/app_library", "lib"]:
	try:
		os.mkdir(d)
	except OSError as e:
		print(e)

w = {}
try:
	if "wifi.json" in os.listdir():
		with open("wifi.json") as f:
			w = json.loads(f.read())
except ValueError as e:
	print(e)

if not ("ssid" in w and "pw" in w):
	message(["Couldn't find a valid wifi.json :(", "More information:", "badge.emfcamp.org/TiLDA_MK3/wifi"])
	while True: pyb.wfi()

wifi_ssid = w["ssid"]

message(["Connecting to '%s'" % (wifi_ssid), "Update wifi.json if this is incorrect", "More information:", "badge.emfcamp.org/TiLDA_MK3/wifi"])
n = network.CC3100()
if ("pw" in w) and w["pw"]:
	n.connect(wifi_ssid, w["pw"], timeout=10)
else:
	n.connect(wifi_ssid, timeout=10)
while not n.is_connected():
	n.update()
	pyb.delay(100)

try:
	libs = ["wifi", "buttons", "http_client", "filesystem", "dialogs", "database", "app"]
	for i, lib in enumerate(libs):
		message(["Downloading library: %s (%d/%d)" % (lib, i + 1, len(libs))])
		download("/firmware/master/lib/%s.py" % (lib), "lib/%s.py" % (lib))

	message(["Downloading app library"])
	download("/firmware/master/apps/app_library/main.py", "apps/app_library/main.py")

finally:
	os.sync()
machine.reset()

### Description: Update the badge's time via NTP
### License: MIT

import socket

NTP_DELTA = 2208988800
NTP_HOST = "pool.ntp.org"
NTP_PORT = 123


def get_NTP_time():
	NTP_QUERY = bytearray(48)
	NTP_QUERY[0] = 0x1b
	addr = socket.getaddrinfo(NTP_HOST, NTP_PORT)[0][-1]
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	s.sendto(NTP_QUERY, addr)

	# Setting timeout for receiving data. Because we're using UDP,
	# there's no need for a timeout on send.
	s.settimeout(2)
	msg = None
	try:
		msg = s.recv(48)
	except OSError:
		pass
	finally:
		s.close()

	if msg is None:
		return None

	import struct
	val = struct.unpack("!I", msg[40:44])[0]
	return val - NTP_DELTA


def set_NTP_time():
	import time
	from machine import RTC
	print("Setting time from NTP")

	t = get_NTP_time()
	if t is None:
		print("Could not set time from NTP")
		return False

	tm = time.localtime(t)
	tm = tm[0:8]

	rtc = RTC()
	rtc.init(tm)
	#rtc.datetime(tm)

	return True


def local_time_available():
	import time
	current = time.localtime()
	# If the time has been synced via NTP already,
	# the year is no longer set to 1970 like it is after cold boot.
	return current[0] != 1970
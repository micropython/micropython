import urequests
import utime
import urandom
import network
import untplib
from machine import RTC
from machine import Pin

DB = "counters-87809";
if not DB:
    print('not Firebase DB key specified, skip sending data');

ACCESS_TOKEN="AIzaSyBiZak8OSV1wu0ENz7fUdHdQ0S4qRFc0QU";

def wifiConnect():
	sta_if = network.WLAN(network.STA_IF);
	if not sta_if.isconnected():
		print('connecting to network...');
		sta_if.active(True);
		sta_if.connect('Smart_box-142', 'natalia31081980');
		while not sta_if.isconnected():
			pass;
	print('network config:', sta_if.ifconfig());

def setTime():
	c=untplib.NTPClient();
	resp=c.request('0.uk.pool.ntp.org', version=3, port=123);
	print("Offset is ", resp.offset);
	rtc = RTC();
	print("Adjusting clock by ", resp.offset, "seconds");
	rtc.init(utime.localtime(utime.time() + resp.offset));

def sendToFirebase():
	for a in range(5):
		timestamp = int(utime.time());
		value1 = urandom.getrandbits(9);
		value2 = urandom.getrandbits(9);
		value3 = urandom.getrandbits(9);
		data = {"timestamp":timestamp,"6123464":value1,"11443721":value2,"11444802":value3,"electricity":0};
		response = urequests.post('https://' + DB + '.firebaseio.com/.json', json=data);
		print(data);
		#print(response.status_code);
		utime.sleep(1);

wifiConnect();
setTime();
sendToFirebase();

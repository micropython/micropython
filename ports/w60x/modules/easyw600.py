import binascii
import network
import utime
import w600

def scan():
    sta_if = network.WLAN(network.STA_IF)
    sta_if.active(True)
    for i in sta_if.scan():
        print("%32s"%(bytes(i[0]).decode('ascii')+'\t'),bytes(binascii.hexlify(i[1])).decode('ascii')+"\t",str(i[2])+"\t",str(i[3])+"\t",str(i[4])+"\t",i[5])


def oneshot():
    sta_if = network.WLAN(network.STA_IF)
    sta_if.active(True)
    sta_if.oneshot(1)
    print("wait conneting...")
    while (1):
        if (sta_if.isconnected()):
            break;
        else:
            utime.sleep(1)
    print("connected, ip is " + sta_if.ifconfig()[0])


def connect(ssid=None, password=None):
    sta_if = network.WLAN(network.STA_IF)
    sta_if.active(True)
    sta_if.connect(ssid, password)
    print("conneting... ")
    while (1):
        if (sta_if.isconnected()):
            break;
        else:
            utime.sleep(1)
    print("connected, ip is " + sta_if.ifconfig()[0])


def disconnect():
    sta_if = network.WLAN(network.STA_IF)
    sta_if.disconnect()


def createap(ssid="w600_softap", passwd=None):
    ap_if = network.WLAN(network.AP_IF)
    ap_if.active(True)
    if passwd is None:
        ap_if.config(essid=ssid,authmode=network.AUTH_OPEN,channel=11)
    else:
        ap_if.config(essid=ssid,password=passwd,authmode=network.AUTH_WPA2_PSK_AES,channel=11)
    print("softap working, ip is 192.168.43.1")


def closeap():
    ap_if = network.WLAN(network.AP_IF)
    ap_if.active(False)


def ftpserver():
    w600.run_ftpserver(port=21,username="root",password="root")
    print("ftp server port is 21, username is root, password is root")


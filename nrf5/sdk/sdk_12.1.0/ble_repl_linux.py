from bluepy.btle import Scanner, DefaultDelegate, Peripheral, UUID
import struct

class NotificationDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)

    def handleNotification(self, cHandle, data):
	print data

def find_mac_by_name(name):
    scanner = Scanner(iface=1)
    devices = scanner.scan(5.0)

    found_device = None
    for dev in devices:
        print "Device %s (%s), RSSI=%d dB" % (dev.addr, dev.addrType, dev.rssi)
        for (adtype, desc, value) in dev.getScanData():
            if (desc == "Short Local Name"):
                if value == name:
                     found_device = dev
                     break
        
        if found_device:
            break
    return dev

dev = find_mac_by_name("micr")
peri = Peripheral(dev)
peri.setDelegate(NotificationDelegate())

# service = peri.getServiceByUUID(UUID("6e400001-b5a3-f393-e0a9-e50e24dcca9e"))

rx_char = peri.getCharacteristics(uuid=UUID("6e400002-b5a3-f393-e0a9-e50e24dcca9e"))[0]
tx_char = peri.getCharacteristics(uuid=UUID("6e400003-b5a3-f393-e0a9-e50e24dcca9e"))[0]

# enable cccd
cccd =    peri.writeCharacteristic(0x0e, struct.pack('<bb', 0x01, 0x00))

while True:
    if peri.waitForNotifications(0.1):
        pass  

    c = raw_input().encode('utf-8')
    rx_char.write(c)


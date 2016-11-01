import esp
import uctypes
import network
import lwip


def main():

    ROM = uctypes.bytearray_at(0x40200000, 16)
    fid = esp.flash_id()

    print("FlashROM:")
    print("Flash ID: %x (Vendor: %x Device: %x)" % (fid, fid & 0xff, fid & 0xff00 | fid >> 16))

    print("Flash bootloader data:")
    SZ_MAP = {0: "512KB", 1: "256KB", 2: "1MB", 3: "2MB", 4: "4MB"}
    FREQ_MAP = {0: "40MHZ", 1: "26MHZ", 2: "20MHz", 0xf: "80MHz"}
    print("Byte @2: %02x" % ROM[2])
    print("Byte @3: %02x (Flash size: %s Flash freq: %s)" % (ROM[3], SZ_MAP.get(ROM[3] >> 4, "?"), FREQ_MAP.get(ROM[3] & 0xf)))
    print("Firmware checksum:")
    print(esp.check_fw())

    print("\nNetworking:")
    print("STA ifconfig:", network.WLAN(network.STA_IF).ifconfig())
    print("AP ifconfig:", network.WLAN(network.AP_IF).ifconfig())
    print("Free WiFi driver buffers of type:")
    for i, comm in enumerate(("1,2 TX", "4 Mngmt TX(len: 0x41-0x100)", "5 Mngmt TX (len: 0-0x40)", "7", "8 RX")):
        print("%d: %d (%s)" % (i, esp.esf_free_bufs(i), comm))
    print("lwIP PCBs:")
    lwip.print_pcbs()


main()

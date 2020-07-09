# GNSS sample for pca10090

import network
import socket

n = network.NRF91()

at = socket.socket(socket.AF_LTE, socket.SOCK_DGRAM, socket.PROTO_AT)
at.send("AT+CFUN=4")  # Flight mode.
at.recv(20)

# Enable LTE + GNSS, persistently stored.
# Mask of "0, 0, 1, 0" also possible if
# only GNSS is of interest.
at.send("AT%XSYSTEMMODE=1,0,1,0")
at.recv(20)

# Antenna tuning for pca10090 GNSS.
at.send("AT%XMAGPIO=1,0,0,1,1,1574,1577")
at.recv(20)

# Implicit CFUN=1. GNSS sockets can respond, and LTE starts to connect.
# Returns when LTE link has been established. Alternativly, set a timeout
# on connect; n.connect(seconds).
n.connect()

gps = socket.socket(socket.AF_LOCAL, socket.SOCK_DGRAM, socket.PROTO_GNSS)
gps.setsockopt(socket.SOL_GNSS, socket.SO_GNSS_FIX_RETRY, bytearray([0, 0]))
gps.setsockopt(socket.SOL_GNSS, socket.SO_GNSS_FIX_INTERVAL, bytearray([1, 0]))

nmea_mask = (
    socket.GNSS_NMEA_CONFIG_GGA_FLAG
    | socket.GNSS_NMEA_CONFIG_GLL_FLAG
    | socket.GNSS_NMEA_CONFIG_GSA_FLAG
    | socket.GNSS_NMEA_CONFIG_GSV_FLAG
    | socket.GNSS_NMEA_CONFIG_RMC_FLAG
)

gps.setsockopt(socket.SOL_GNSS, socket.SO_GNSS_NMEA_MASK, bytearray([nmea_mask, 0]))
gps.setsockopt(socket.SOL_GNSS, socket.SO_GNSS_START, bytearray([0, 0, 0, 0]))
count = 0
while 1:
    count += 1
    bytes = gps.recv(580)
    if bytes[0] == 2:
        print("NMEA:%s" % str(bytes[8:], "utf-8"))

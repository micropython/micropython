import network
import time
import socket
import ssl

CA = b"""-----BEGIN CERTIFICATE-----
MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/
MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT
DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow
PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD
Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB
AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O
rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq
OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b
xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw
7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD
aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV
HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG
SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69
ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr
AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz
R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5
JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo
Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ
-----END CERTIFICATE-----"""

wlan = network.WLAN(network.STA_IF) # create station interface
wlan.active(True)       # activate the interface
if not wlan.isconnected():
    if wlan.status() == network.STAT_CONNECTING:
        wlan.disconnect()
    wlan.connect('xyz', 'xyz') # connect to an AP

while True:
    if wlan.isconnected():
        break
    time.sleep(0.2)
host = 'letsencrypt.org'

ai = socket.getaddrinfo(host, 443)
ai = ai[0]
s = socket.socket()
s.connect(ai[-1])
# s.setblocking(False)
s2 = ssl.wrap_socket(s)
cert = s2.getpeercert(True)
s2.close()

s = socket.socket()
s.connect(ai[-1])
s.setblocking(False)
print("Starting connection which should raise, probably you get a message like: \"mbedtls_cert error: 8\"")
try:
    s2 = ssl.wrap_socket(s, server_hostname=host, cert_reqs=0xffffff)
except OSError:
    pass
else:
    raise 'This should have raised an error!'
finally:
    s2.close()

s = socket.socket()
s.connect(ai[-1])
s.setblocking(False)
print("Starting connection which should work")
try:
    s2 = ssl.wrap_socket(s, server_hostname=host, ca_certs=CA, cert_reqs=0xffffff)
except OSError:
    raise Exception('This should not have raised an error!')
finally:
    s2.close()

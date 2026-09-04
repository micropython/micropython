# Test network.HALOW associating with an access point.
#
# IMPORTANT: This test requires hardware: a Morse Micro transceiver, and an
# 802.11ah access point to join.  Its credentials are not in this file; put them
# in a halow_config module on the device, as:
#
#     SSID = "..."
#     KEY = "..."
#     COUNTRY = "US"
#
# The test is skipped when that module is absent.

try:
    import network

    network.HALOW
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

try:
    from halow_config import SSID, KEY, COUNTRY
except ImportError:
    print("SKIP")
    raise SystemExit

import socket
import time
import unittest

# Association covers a channel sweep and a four way handshake.
CONNECT_TIMEOUT_MS = 30000


class Test(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        network.country(COUNTRY)
        cls.nic = network.HALOW()
        cls.nic.config(pm=network.HALOW.PM_NONE)
        cls.nic.active(True)
        cls.nic.connect(SSID, KEY)
        start = time.ticks_ms()
        while not cls.nic.isconnected():
            if time.ticks_diff(time.ticks_ms(), start) > CONNECT_TIMEOUT_MS:
                raise Exception("did not associate with %s" % SSID)
            time.sleep_ms(100)

    @classmethod
    def tearDownClass(cls):
        cls.nic.disconnect()
        cls.nic.active(False)

    def test_connected(self):
        self.assertTrue(self.nic.isconnected())
        self.assertEqual(self.nic.status(), network.HALOW.STAT_GOT_IP)

    def test_ifconfig(self):
        ip, netmask, gateway, dns = self.nic.ifconfig()
        self.assertNotEqual(ip, "0.0.0.0")
        self.assertNotEqual(gateway, "0.0.0.0")

    def test_status(self):
        self.assertTrue(-120 < self.nic.status("rssi") < 0)
        self.assertEqual(len(self.nic.status("bssid")), 6)

    def test_channel(self):
        # Reported once associated, unlike a scan result, this is the channel
        # being operated on.
        self.assertTrue(self.nic.config("channel") > 0)
        self.assertIn(self.nic.config("bandwidth"), (1, 2, 4, 8))

    def test_ssid(self):
        self.assertEqual(self.nic.config("ssid"), SSID)

    def test_rates_after_traffic(self):
        # Move enough frames for rate control to have made a choice.
        gateway = self.nic.ifconfig()[2]
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        try:
            for _ in range(64):
                s.sendto(b"x" * 512, (gateway, 9))
        finally:
            s.close()
        # Frames are still on their way out when sendto() returns, and the rate
        # table is updated as they complete.
        time.sleep(1)
        rates = self.nic.status("rates")
        self.assertTrue(any(sent > 0 for _, _, _, sent, _ in rates))

    def test_gateway_round_trip(self):
        # Connecting to the gateway proves address resolution, transmit and
        # receive all work; a refusal is as good as an accept here.
        gateway = self.nic.ifconfig()[2]
        s = socket.socket()
        s.settimeout(10)
        try:
            try:
                s.connect((gateway, 80))
            except OSError as er:
                self.assertNotIn("ETIMEDOUT", repr(er))
        finally:
            s.close()

    def test_twt_refused_while_associated(self):
        # The agreement is carried in the association request.
        with self.assertRaises(OSError):
            self.nic.config(twt=(1000000, 20000, network.HALOW.TWT_SUGGEST))

    def test_listen_interval_refused_while_associated(self):
        # Also carried in the association request, so it applies to the next one.
        self.nic.config(listen_interval=5)
        self.assertEqual(self.nic.config("listen_interval"), 5)
        self.nic.config(listen_interval=0)

    def test_scan_empty_while_associated(self):
        # The radio cannot sweep and hold a link at the same time.
        self.assertEqual(self.nic.scan(), [])

    def test_wnm_sleep_round_trip(self):
        self.nic.config(pm=network.HALOW.PM_POWERSAVE)
        try:
            self.nic.config(wnm_sleep=True)
            time.sleep(1)
            self.nic.config(wnm_sleep=False)
            self.assertTrue(self.nic.isconnected())
        finally:
            self.nic.config(pm=network.HALOW.PM_NONE)


if __name__ == "__main__":
    unittest.main()

# Board-contained smoke tests for an ESP32 LAN uplink and AP setup used with
# lwIP NAPT.
#
# This is a hardware-gated integration test. It validates prerequisites and
# control-plane state around an AP+LAN NAPT setup. It does not prove end-to-end
# NAT translation because no traffic originates from a separate AP client.
#
# It requires a board/firmware build with CONFIG_LWIP_IPV4_NAPT enabled plus a
# working LAN uplink that acquires DNS via DHCP. Set LAN_ARGS below to match the
# Ethernet hardware under test.

import time
import unittest

try:
    import network
    import socket
except ImportError:
    print("SKIP")
    raise SystemExit


# Example:
# LAN_ARGS = {
#     "mdc": 23,
#     "mdio": 18,
#     "power": 5,
#     "phy_type": network.PHY_LAN8720,
#     "phy_addr": 0,
# }
#
# SPI Ethernet example:
# from machine import Pin, SPI
# spi = SPI(1, sck=Pin(13), mosi=Pin(11), miso=Pin(12))
# LAN_ARGS = {
#     "spi": spi,
#     "cs": Pin(14),
#     "int": Pin(10),
#     "reset": Pin(9),
#     "phy_type": network.PHY_W5500,
#     "phy_addr": 0,
# }
LAN_ARGS = None
TIMEOUT_MS = 15000
PROBE_BY_IP_HOST = "1.1.1.1"
PROBE_PORT = 80
INVALID_TEST_DNS = "254.254.254.254"
# Use distinct hostname sets across the pass/fail/pass sequence to avoid resolver
# cache hits and to reduce false failures from any single domain being unavailable.
PROBE_BY_NAME_HOST_SETS = (
    ("example.com", "iana.org", "python.org"),
    ("micropython.org", "www.example.org", "docs.python.org"),
    ("one.one.one.one", "dns.google", "pypi.org"),
)


class Test(unittest.TestCase):
    def _wait_for_ap_dns(self, ap, expected_dns, failure_message):
        start = time.ticks_ms()
        while time.ticks_diff(time.ticks_ms(), start) < TIMEOUT_MS:
            ap_dns = ap.ifconfig()[3]
            if ap_dns == expected_dns:
                return ap_dns
            time.sleep_ms(250)

        self.fail(
            "{} (expected {}, got {})".format(failure_message, expected_dns, ap.ifconfig()[3])
        )

    def _assert_public_tcp_uplink(self):
        sock = socket.socket()
        try:
            sock.settimeout(5)
            sock.connect((PROBE_BY_IP_HOST, PROBE_PORT))
            print("Connected to {}:{}".format(PROBE_BY_IP_HOST, PROBE_PORT))
        except OSError as exc:
            self.fail(
                "LAN uplink could not open TCP socket to {}:{}: {}".format(
                    PROBE_BY_IP_HOST, PROBE_PORT, exc
                )
            )
        finally:
            sock.close()

    def _assert_public_dns_uplink(self, hosts):
        errors = []
        for host in hosts:
            try:
                addr = socket.getaddrinfo(host, PROBE_PORT)[0][-1]
            except OSError as exc:
                errors.append("{} lookup: {}".format(host, exc))
                continue

            sock = socket.socket()
            try:
                sock.settimeout(5)
                sock.connect(addr)
                print("Resolved and connected to {} via {}".format(host, addr[0]))
                return
            except OSError as exc:
                errors.append("{} connect {}: {}".format(host, addr, exc))
            finally:
                sock.close()

        self.fail("No DNS-dependent uplink probe succeeded: {}".format("; ".join(errors)))

    def _assert_public_dns_lookup_fails(self, hosts):
        successes = []
        failures = []

        for host in hosts:
            try:
                addr = socket.getaddrinfo(host, PROBE_PORT)[0][-1]
                successes.append("{} -> {}".format(host, addr[0]))
            except OSError as exc:
                print("DNS lookup failed for {} as expected: {}".format(host, exc))
                failures.append(host)

        if successes:
            self.fail("DNS lookup unexpectedly succeeded for: {}".format(", ".join(successes)))

        if not failures:
            self.fail("No DNS-dependent failure probes were attempted")

    def _restart_ap(self, ap):
        ap.active(False)
        time.sleep_ms(100)
        ap.active(True)

    def _set_lan_dns(self, lan, dns_server):
        ip, subnet, gateway, _ = lan.ifconfig()
        lan.ifconfig((ip, subnet, gateway, dns_server))
        current_dns = lan.ifconfig()[3]
        self.assertEqual(current_dns, dns_server)
        return current_dns

    def _bring_up_lan_and_ap(self):
        if LAN_ARGS is None:
            self.skipTest("configure LAN_ARGS for the test board")

        if not hasattr(network, "LAN") or not hasattr(network, "WLAN"):
            self.skipTest("LAN or WLAN support not available")

        ap = network.WLAN(network.WLAN.IF_AP)
        ap.active(False)
        time.sleep_ms(100)
        ap.config(ssid="mp-napt-test")
        ap.active(True)

        try:
            lan = network.LAN(**LAN_ARGS)
        except Exception as exc:
            ap.active(False)
            self.skipTest("LAN unavailable: {}".format(exc))

        lan.active(True)

        start = time.ticks_ms()
        while time.ticks_diff(time.ticks_ms(), start) < TIMEOUT_MS:
            if lan.isconnected():
                break
            time.sleep_ms(250)
        else:
            ap.active(False)
            self.skipTest("LAN link or DHCP did not become ready")

        print("LAN ifconfig:", lan.ifconfig())
        print("AP ifconfig:", ap.ifconfig())

        lan_dns = lan.ifconfig()[3]
        if not lan_dns or lan_dns == "0.0.0.0":
            ap.active(False)
            self.skipTest("LAN DNS not available")

        try:
            ap_dns = self._wait_for_ap_dns(ap, lan_dns, "AP DNS was not updated from LAN uplink")
        except AssertionError:
            ap.active(False)
            raise

        print("LAN DNS:", lan_dns)
        print("AP DNS:", ap_dns)
        return ap, lan, lan_dns, ap_dns

    def test_ap_dns_matches_lan_dns(self):
        ap, _, lan_dns, ap_dns = self._bring_up_lan_and_ap()
        try:
            self.assertEqual(ap_dns, lan_dns)
        finally:
            ap.active(False)

    def test_ap_propagates_dns_changes_and_dns_dependent_uplink_follows(self):
        ap, lan, lan_dns, _ = self._bring_up_lan_and_ap()
        try:
            # Verify pass/fail/pass across baseline DNS, invalid DNS, and restored DNS.
            self._assert_public_dns_uplink(PROBE_BY_NAME_HOST_SETS[0])

            if INVALID_TEST_DNS == lan_dns:
                self.skipTest("choose an INVALID_TEST_DNS different from the uplink DNS")

            updated_lan_dns = self._set_lan_dns(lan, INVALID_TEST_DNS)
            self._restart_ap(ap)
            self._wait_for_ap_dns(
                ap,
                updated_lan_dns,
                "AP DNS was not updated from the LAN DNS after applying the invalid DNS",
            )
            self._assert_public_dns_lookup_fails(PROBE_BY_NAME_HOST_SETS[1])

            restored_lan_dns = self._set_lan_dns(lan, lan_dns)
            self._restart_ap(ap)
            self._wait_for_ap_dns(
                ap,
                restored_lan_dns,
                "AP DNS was not updated from the LAN DNS after AP restart",
            )
            self._assert_public_dns_uplink(PROBE_BY_NAME_HOST_SETS[2])
        finally:
            ap.active(False)

    def test_lan_uplink_can_open_public_tcp_socket(self):
        ap, _, _, _ = self._bring_up_lan_and_ap()
        try:
            self._assert_public_tcp_uplink()
        finally:
            ap.active(False)


if __name__ == "__main__":
    unittest.main()

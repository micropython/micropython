# Test network.HALOW, the parts that need no access point.
#
# IMPORTANT: This test requires hardware: a Morse Micro transceiver on the bus
# the board is configured for.  It brings the interface up but does not
# associate, so nothing else is needed.  See network_halow_sta.py for the tests
# that do need a network.

try:
    import network

    network.HALOW
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

import unittest

COUNTRY = "US"


class Test(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        network.country(COUNTRY)
        cls.nic = network.HALOW()
        cls.nic.active(True)

    @classmethod
    def tearDownClass(cls):
        cls.nic.active(False)

    def test_singleton(self):
        self.assertIs(network.HALOW(), network.HALOW(network.HALOW.IF_STA))

    def test_constants(self):
        # Security modes are distinct, and 802.11ah has no WPA2-PSK.
        self.assertNotEqual(network.HALOW.SEC_OPEN, network.HALOW.SEC_SAE)
        self.assertNotEqual(network.HALOW.SEC_OPEN, network.HALOW.SEC_OWE)
        self.assertNotEqual(network.HALOW.PM_NONE, network.HALOW.PM_POWERSAVE)

    def test_active(self):
        self.assertTrue(self.nic.active())

    def test_mac(self):
        mac = self.nic.config("mac")
        self.assertEqual(len(mac), 6)
        self.assertNotEqual(mac, b"\x00" * 6)
        self.assertNotEqual(mac, b"\xff" * 6)

    def test_version(self):
        morselib, firmware, chip = self.nic.config("version")
        self.assertTrue(morselib)
        self.assertTrue(firmware)
        self.assertTrue(chip)

    def test_scan_returns_tuples(self):
        # A sweep covers every channel of the regulatory domain, so it takes a
        # few seconds; what is found depends on the environment.
        for net in self.nic.scan():
            ssid, bssid, channel, rssi, security, hidden = net
            self.assertEqual(len(bssid), 6)
            self.assertTrue(-120 < rssi < 0)
            self.assertIn(
                security,
                (network.HALOW.SEC_OPEN, network.HALOW.SEC_OWE, network.HALOW.SEC_SAE),
            )

    def test_scan_takes_no_arguments(self):
        with self.assertRaises(TypeError):
            self.nic.scan(ssid="anything")

    def test_radio_settings(self):
        before = [self.nic.config(k) for k in ("ampdu", "sgi", "subbands")]
        self.nic.config(ampdu=False, sgi=False, subbands=False)
        self.assertFalse(self.nic.config("ampdu"))
        self.assertFalse(self.nic.config("sgi"))
        self.assertFalse(self.nic.config("subbands"))
        self.nic.config(ampdu=before[0], sgi=before[1], subbands=before[2])

    def test_thresholds(self):
        self.nic.config(rts_threshold=512, fragment_threshold=1024)
        self.assertEqual(self.nic.config("rts_threshold"), 512)
        self.assertEqual(self.nic.config("fragment_threshold"), 1024)
        self.nic.config(rts_threshold=0, fragment_threshold=0)

    def test_listen_interval(self):
        self.nic.config(listen_interval=5)
        self.assertEqual(self.nic.config("listen_interval"), 5)
        self.nic.config(listen_interval=0)

    def test_power_management(self):
        self.nic.config(pm=network.HALOW.PM_POWERSAVE)
        self.assertEqual(self.nic.config("pm"), network.HALOW.PM_POWERSAVE)
        self.nic.config(pm=network.HALOW.PM_NONE)
        self.assertEqual(self.nic.config("pm"), network.HALOW.PM_NONE)

    def test_duty_cycle(self):
        limit, mode, burst_left, burst_window = self.nic.status("duty_cycle")
        # In hundredths of a percent, so a full duty cycle is 10000.
        self.assertTrue(0 < limit <= 10000)
        self.nic.config(duty_cycle=network.HALOW.DUTY_CYCLE_BURST)
        self.assertEqual(self.nic.config("duty_cycle"), network.HALOW.DUTY_CYCLE_BURST)
        self.nic.config(duty_cycle=network.HALOW.DUTY_CYCLE_SPREAD)
        self.assertEqual(self.nic.config("duty_cycle"), network.HALOW.DUTY_CYCLE_SPREAD)

    def test_txpower(self):
        before = self.nic.config("txpower")
        self.nic.config(txpower=10)
        self.assertEqual(self.nic.config("txpower"), 10)
        self.nic.config(txpower=before)

    def test_status_reports_a_known_state(self):
        self.assertIn(
            self.nic.status(),
            (
                network.HALOW.STAT_IDLE,
                network.HALOW.STAT_CONNECTING,
                network.HALOW.STAT_NOIP,
                network.HALOW.STAT_GOT_IP,
                network.HALOW.STAT_CONNECT_FAIL,
            ),
        )

    def test_health_check(self):
        self.nic.config(health_check=(30000, 120000))
        # A minimum above the maximum is not a usable interval.
        with self.assertRaises(OSError):
            self.nic.config(health_check=(120000, 30000))

    def test_rates(self):
        for mcs, bandwidth, gi, sent, success in self.nic.status("rates"):
            self.assertTrue(0 <= mcs <= 10)
            self.assertIn(bandwidth, (1, 2, 4, 8))
            self.assertTrue(success <= sent)

    def test_scan_refused_while_dozing(self):
        # A dozing transceiver cannot hear probe responses.
        self.nic.config(pm=network.HALOW.PM_POWERSAVE)
        try:
            with self.assertRaises(OSError):
                self.nic.scan()
        finally:
            self.nic.config(pm=network.HALOW.PM_NONE)

    def test_wnm_sleep_refused_without_power_save(self):
        with self.assertRaises(OSError):
            self.nic.config(wnm_sleep=True)

    def test_wnm_powerdown_setting(self):
        self.nic.config(wnm_powerdown=True)
        self.assertTrue(self.nic.config("wnm_powerdown"))
        self.nic.config(wnm_powerdown=False)
        self.assertFalse(self.nic.config("wnm_powerdown"))

    def test_fixed_rate(self):
        # Pin the rate, then hand it back to rate control.
        self.nic.config(fixed_rate=(0, 1, 1))
        self.nic.config(fixed_rate=(-1, -1, -1))

    def test_ioctl_rejects_a_bad_command(self):
        with self.assertRaises(OSError):
            self.nic.ioctl(None, bytearray(32))

    def test_unknown_config_key(self):
        with self.assertRaises(ValueError):
            self.nic.config("not_a_parameter")

    def test_settings_reject_out_of_range_values(self):
        # These reach fields narrower than a Python integer, where a negative or
        # oversized value would wrap rather than be refused: txpower=-5 read
        # back as 65531, which the transceiver takes as no limit at all.
        for key, value in (
            ("txpower", -5),
            ("txpower", 1 << 20),
            ("listen_interval", -1),
            ("listen_interval", 100000),
        ):
            with self.assertRaises(ValueError):
                self.nic.config(**{key: value})

    def test_twt_interval_is_not_a_machine_word(self):
        # An hour between wakes is a legitimate request and does not fit in a
        # machine word, so converting it as one raised OverflowError before the
        # interface could see it.  Accepting it is the assertion.
        self.nic.config(twt=(3600000000, 100000))

    def test_twt_rejects_a_window_longer_than_its_interval(self):
        with self.assertRaises(OSError):
            self.nic.config(twt=(1000, 100000))

    def test_rates_guard_interval_is_one_bit(self):
        for _, _, gi, _, _ in self.nic.status("rates"):
            self.assertIn(gi, (0, 1))

    def test_config_after_deactivate(self):
        # Taking the interface down shuts the transceiver down but leaves the
        # driver initialised, and the cached settings used to be applied anyway
        # -- straight into a morselib that was no longer running, which answered
        # EAGAIN.  Settings made while down are meant to be kept for the next
        # time it comes up.
        try:
            self.nic.active(False)
            self.nic.config(pm=network.HALOW.PM_NONE)
            self.nic.config(rts_threshold=512)
            self.assertEqual(self.nic.config("rts_threshold"), 512)
        finally:
            self.nic.active(True)
            self.nic.config(rts_threshold=0)

    def test_use_after_deinit_is_refused_not_fatal(self):
        # deinit() hands the driver's memory pool back to the collector.  The
        # scan cache was left pointing into it, so the next sweep wrote into
        # memory Python had already been given -- which a soft reset reaches
        # too, since it tears the driver down the same way.  Restores the
        # interface either way, so the rest of the suite still has one.
        try:
            self.nic.scan()
            self.nic.deinit()
            self.assertEqual(self.nic.scan(), [])
            self.assertFalse(self.nic.active())
        finally:
            self.nic.active(True)


if __name__ == "__main__":
    unittest.main()

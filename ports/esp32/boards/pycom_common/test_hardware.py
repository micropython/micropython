"""
Hardware validation script for Pycom LoPy/LoPy4 boards.

Run from REPL:
    import test_hardware
    test_hardware.run()

Or run individual tests:
    test_hardware.test_pins()
    test_hardware.test_wifi()
    test_hardware.test_lora()
"""

import sys
import machine
import time


def _pass(name):
    print("[PASS]", name)


def _fail(name, reason=""):
    print("[FAIL]", name, reason)


def _skip(name, reason=""):
    print("[SKIP]", name, reason)


def test_repl():
    """Verify REPL is running and basic Python works."""
    assert 1 + 1 == 2
    assert "hello"[0] == "h"
    _pass("REPL basic Python")


def test_pins():
    """Test GPIO output on P-pins (non-LoRa, non-special pins)."""
    # Test a few safe GPIO pins (P3=GPIO4, P8=GPIO2, P11=GPIO22)
    test_gpios = ["P3", "P8", "P11"]
    for name in test_gpios:
        p = machine.Pin(name, machine.Pin.OUT)
        p.value(1)
        assert p.value() == 1
        p.value(0)
        assert p.value() == 0
    _pass("GPIO output (P3, P8, P11)")


def test_wifi():
    """Test WiFi scan."""
    import network

    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    time.sleep_ms(500)
    networks = wlan.scan()
    wlan.active(False)
    if len(networks) > 0:
        _pass("WiFi scan ({} APs found)".format(len(networks)))
    else:
        _fail("WiFi scan", "no APs found (may be RF shielded)")


def test_ble():
    """Test BLE activation."""
    try:
        import bluetooth

        ble = bluetooth.BLE()
        ble.active(True)
        time.sleep_ms(200)
        active = ble.active()
        ble.active(False)
        if active:
            _pass("BLE active")
        else:
            _fail("BLE active")
    except Exception as e:
        _fail("BLE", str(e))


def test_neopixel():
    """Test NeoPixel RGB LED on GPIO0."""
    try:
        from neopixel import NeoPixel

        np = NeoPixel(machine.Pin(0, machine.Pin.OUT), 1)
        for color in [(50, 0, 0), (0, 50, 0), (0, 0, 50), (0, 0, 0)]:
            np[0] = color
            np.write()
            time.sleep_ms(200)
        _pass("NeoPixel RGB (GPIO0)")
    except Exception as e:
        _fail("NeoPixel", str(e))


def test_filesystem():
    """Test filesystem read/write persistence."""
    try:
        with open("/test_hw.tmp", "w") as f:
            f.write("test123")
        with open("/test_hw.tmp", "r") as f:
            data = f.read()
        import os

        os.remove("/test_hw.tmp")
        if data == "test123":
            _pass("Filesystem r/w")
        else:
            _fail("Filesystem r/w", "data mismatch")
    except Exception as e:
        _fail("Filesystem", str(e))


def test_psram():
    """Test PSRAM availability (LoPy4 only)."""
    import gc

    free = gc.mem_free()
    if free > 2000000:  # >2MB indicates PSRAM
        _pass("PSRAM detected ({} bytes free)".format(free))
    else:
        _skip("PSRAM", "not detected ({} bytes free)".format(free))


def test_lora_chip():
    """Test SX127x chip detection via SPI using named pins."""
    spi = machine.SPI(
        1,
        baudrate=10000000,
        polarity=0,
        phase=0,
        sck=machine.Pin("LORA_SCLK"),
        mosi=machine.Pin("LORA_MOSI"),
        miso=machine.Pin("LORA_MISO"),
    )
    cs = machine.Pin("LORA_CS", machine.Pin.OUT, value=1)

    # Read version register (0x42)
    cs.value(0)
    spi.write(bytearray([0x42 & 0x7F]))
    version = spi.read(1)[0]
    cs.value(1)
    spi.deinit()

    if version == 0x22:
        _pass("LoRa chip SX1272 detected (version=0x{:02x})".format(version))
    elif version == 0x12:
        _pass("LoRa chip SX1276 detected (version=0x{:02x})".format(version))
    else:
        _fail("LoRa chip detection", "unknown version 0x{:02x}".format(version))


def test_lora_tx():
    """Test LoRa transmit (sends a short test packet)."""
    try:
        from sx127x import SX127x

        spi = machine.SPI(
            1,
            baudrate=10000000,
            polarity=0,
            phase=0,
            sck=machine.Pin("LORA_SCLK"),
            mosi=machine.Pin("LORA_MOSI"),
            miso=machine.Pin("LORA_MISO"),
        )
        # Try to get LORA_RESET pin; not all boards have it
        try:
            reset = machine.Pin("LORA_RESET")
        except ValueError:
            reset = None

        radio = SX127x(spi, machine.Pin("LORA_CS"), machine.Pin("LORA_DIO0"), reset)
        radio.set_frequency(868000000)
        radio.set_spreading_factor(7)
        radio.set_bandwidth(125000)
        radio.set_tx_power(2)
        radio.send(b"PYCOM_HW_TEST")
        radio.sleep()
        _pass("LoRa TX (868 MHz SF7)")
    except Exception as e:
        _fail("LoRa TX", str(e))


def run():
    """Run all hardware validation tests."""
    print("=" * 40)
    print("Pycom Hardware Validation")
    print("Board:", sys.implementation._machine)
    print("=" * 40)

    tests = [
        test_repl,
        test_pins,
        test_wifi,
        test_ble,
        test_neopixel,
        test_filesystem,
        test_psram,
        test_lora_chip,
        test_lora_tx,
    ]

    for test in tests:
        try:
            test()
        except Exception as e:
            _fail(test.__name__, str(e))

    print("=" * 40)
    print("Done.")

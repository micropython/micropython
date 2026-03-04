"""
CSI-based turbulence monitor example for ESP32.

This example demonstrates how to use CSI data to calculate turbolence by analyzing
changes in the channel state information over time.

The basic principle:
- CSI data represents the channel response between transmitter and receiver
- Physical objects and environmental changes affect the Wi-Fi signal propagation
- Channel variations cause changes in CSI amplitude across subcarriers
- By calculating the standard deviation of subcarrier amplitudes, we measure spatial turbulence

Requirements:
- ESP32, ESP32-S2, ESP32-S3, ESP32-C3, ESP32-C5, or ESP32-C6
- Active WiFi connection with consistent traffic
- CSI enabled in firmware build
- Router/AP should be in a fixed position

Usage:
    # From MicroPython project root directory:
    mpremote connect /dev/cu.usbmodem11401 run examples/csi/csi_turbulence_monitor.py

Author: Francesco Pace <francesco.pace@gmail.com>
"""

import network
import time
import math
import gc

# Configuration
WIFI_SSID = "your-ssid"  # CHANGE THESE!
WIFI_PASSWORD = "your-password"  # CHANGE THESE!
CSI_BUFFER_SIZE = 16
SELECTED_SUBCARRIERS = [47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58]


def connect_wifi():
    """Connect to WiFi"""
    print(f"Connecting to WiFi '{WIFI_SSID}'...")

    wlan = network.WLAN(network.WLAN.IF_STA)
    if wlan.active():
        wlan.active(False)
        time.sleep(1)

    wlan.active(True)
    time.sleep(3)

    # Configure WiFi protocol for 802.11b/g/n only to get 64 subcarriers
    wlan.config(protocol=network.MODE_11B | network.MODE_11G | network.MODE_11N)
    wlan.config(bandwidth=wlan.BW_HT20)  # HT20 for stable CSI
    wlan.config(promiscuous=False)  # CSI from connected AP only

    # Enable CSI capture
    wlan.csi_enable(buffer_size=CSI_BUFFER_SIZE)
    print(f"✅ CSI enabled (buffer: {CSI_BUFFER_SIZE})\n")

    wlan.connect(WIFI_SSID, WIFI_PASSWORD)

    timeout = 30
    while not wlan.isconnected() and timeout > 0:
        time.sleep(1)
        timeout -= 1

    if not wlan.isconnected():
        raise Exception("WiFi connection failed")

    print(f"✅ Connected - IP: {wlan.ifconfig()[0]}")

    # Disable power management to avoid packet loss
    wlan.config(pm=wlan.PM_NONE)

    return wlan


def calculate_turbulence(csi_data, subcarriers):
    """Calculate spatial turbulence (standard deviation of subcarrier amplitudes)"""
    if len(csi_data) < 2:
        return 0.0

    sum_amp = 0.0
    sum_sq = 0.0
    count = 0

    for sc_idx in subcarriers:
        i = sc_idx * 2
        if i + 1 < len(csi_data):
            real = csi_data[i]
            imag = csi_data[i + 1]
            amplitude = math.sqrt(real * real + imag * imag)
            sum_amp += amplitude
            sum_sq += amplitude * amplitude
            count += 1

    if count < 2:
        return 0.0

    mean = sum_amp / count
    variance = (sum_sq / count) - (mean * mean)
    return math.sqrt(max(0.0, variance))


def main():
    print("\n" + "=" * 60)
    print("Turbulence Monitor")
    print("=" * 60 + "\n")

    wlan = connect_wifi()

    packets = 0
    csi_length_logged = False
    print("Starting CSI processing...\n")

    try:
        while True:
            # Read a frame (returns tuple or None)
            # Tuple order: (rssi, channel, mac, timestamp, local_timestamp, data,
            #               rate, sig_mode, mcs, cwb, smoothing, not_sounding,
            #               aggregation, stbc, fec_coding, sgi, noise_floor, ampdu_cnt,
            #               secondary_channel, ant, sig_len, rx_state, agc_gain, fft_gain)
            frame = wlan.csi_read()

            if frame:
                csi_data = frame[5]  # data field
                data_len = len(csi_data)

                # Log CSI data length on first packet
                if not csi_length_logged:
                    print(f"ℹ️  CSI data length: {data_len} bytes ({data_len // 2} subcarriers)\n")
                    csi_length_logged = True

                # Use only first 128 bytes (or less if data is shorter)
                csi_data = csi_data[:128]

                turbulence = calculate_turbulence(csi_data, SELECTED_SUBCARRIERS)

                packets += 1
                print(f"📊 Pkts: {packets:6d} | Turbulence: {turbulence:6.2f}")

                if packets % 100 == 0:
                    gc.collect()
            else:
                time.sleep_us(100)

    except KeyboardInterrupt:
        print("\n\nStopping...")

    finally:
        wlan.csi_disable()
        print(f"\nTotal packets: {packets}")
        print("=" * 60)


if __name__ == "__main__":
    main()

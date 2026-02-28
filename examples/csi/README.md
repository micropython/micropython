# ESP32 CSI (Channel State Information) Examples

This directory contains examples demonstrating how to use the Wi-Fi CSI (Channel State Information) feature on ESP32 devices.

## What is CSI?

Channel State Information (CSI) provides detailed information about the Wi-Fi channel state by analyzing physical layer signals. CSI data contains amplitude and phase information for each subcarrier, enabling applications such as:

- **Motion Detection**: Detect movement by analyzing changes in CSI patterns (advanced applications)
- **Indoor Localization**: Determine position based on signal propagation characteristics
- **Gesture Recognition**: Recognize hand gestures through CSI variations
- **Presence Detection**: Detect if someone is in a room
- **Channel Analysis**: Monitor Wi-Fi channel quality and interference

## Requirements

### Hardware
- ESP32, ESP32-S2, ESP32-S3, ESP32-C3, ESP32-C5, or ESP32-C6
- Active Wi-Fi connection (CSI requires an active connection to capture frames)

### Firmware
- MicroPython with CSI support enabled (`MICROPY_PY_NETWORK_WLAN_CSI`)
- ESP-IDF with CSI enabled (`CONFIG_ESP_WIFI_CSI_ENABLED=y`)

**Pre-configured boards**: The ``ESP32_GENERIC_C6`` and ``ESP32_GENERIC_S3`` boards with ``CSI`` variant have
CSI support pre-enabled and optimized for CSI applications. These boards are
recommended for CSI development as they include optimized Wi-Fi buffer settings
and other CSI-specific configurations.

To use CSI on other boards, you need to:
1. Enable CSI in your build configuration by adding ``CONFIG_ESP_WIFI_CSI_ENABLED=y``
   to your ``sdkconfig.base`` or a custom ``sdkconfig.board`` file
2. Ensure ``MICROPY_PY_NETWORK_WLAN_CSI`` is enabled in the MicroPython build

**Important: Wi-Fi Traffic Requirement**

CSI data is extracted from Wi-Fi packets received by the ESP32. **Without active traffic, no CSI frames will be captured.** To test CSI functionality, you need to generate traffic directed to the ESP32.

**Generating Test Traffic:**

From another device on the same network, use ping to generate traffic:

```bash
# Replace <esp32_ip> with your ESP32's IP address
ping -i 0.1 <esp32_ip>
```

**Traffic Frequency Guidelines:**

- **More frequent packets** (e.g., `-i 0.05` = 50ms interval):
  - Generates more CSI data
  - Better for real-time analysis
  - Increases computational load on the device

**Note:** The ESP32 must receive packets (not just send them). Traffic from the router/AP to the ESP32 is what generates CSI frames.

## Running Examples

Execute examples directly using `mpremote run`. **Important:** Run the command from the MicroPython project root directory:

```bash
# Make sure you're in the MicroPython project root directory
cd /path/to/micropython

# Then run the examples
mpremote connect /dev/cu.usbmodem11401 run examples/csi/csi_basic.py
mpremote connect /dev/cu.usbmodem11401 run examples/csi/csi_turbulence_monitor.py
```

**Note:** 
- Replace `/dev/cu.usbmodem11401` with your device's serial port. On Linux it's typically `/dev/ttyUSB0`, on Windows it's `COM3` (or similar).
- The path `examples/csi/csi_basic.py` is relative to the MicroPython project root directory.

**Before running:** Update the WiFi credentials (`WIFI_SSID` and `WIFI_PASSWORD`) in the example files.

## Examples

### 1. `csi_basic.py` - Basic CSI Capture

A simple example showing how to:
- Connect to Wi-Fi
- Enable CSI capture
- Read and display CSI frames

**Usage:**
```bash
# From the MicroPython project root directory
mpremote connect /dev/cu.usbmodem11401 run examples/csi/csi_basic.py
```

**What it does:**
- Connects to Wi-Fi
- Enables CSI with a 32-frame buffer
- Continuously reads and displays CSI frame information
- Shows RSSI, channel, MAC address, CSI data length, and more

**Best for:** Learning the basics of CSI capture and understanding the data format.

### 2. `csi_turbulence_monitor.py` - Turbulence Monitoring

Demonstrates how to calculate spatial turbulence using CSI data by analyzing
the standard deviation of subcarrier amplitudes.

**Usage:**
```bash
# From the MicroPython project root directory
mpremote connect /dev/cu.usbmodem11401 run examples/csi/csi_turbulence_monitor.py
```

**What it does:**
- Calculates spatial turbulence from CSI amplitude variance
- Monitors selected subcarriers
- Displays real-time turbulence values
- Processes frames continuously with minimal overhead

**Best for:** Understanding how CSI can be used to measure channel variations
and signal propagation characteristics.

**Key Features:**
- Uses selected subcarriers for focused analysis (center frequencies are most stable)
- Calculates spatial turbulence as standard deviation of amplitudes
- Efficient processing with garbage collection every 100 packets
- Real-time display of turbulence values

**Tips:**
- Works best with consistent Wi-Fi traffic (ping every 100-200ms)
- Position router and ESP32 in fixed locations
- Turbulence values indicate channel variation (higher = more variation)
- Adjust `SELECTED_SUBCARRIERS` to focus on different frequency ranges

## API Reference

The CSI API is integrated into `network.WLAN`:

```python
import network
wlan = network.WLAN(network.WLAN.IF_STA)
wlan.active(True)
wlan.connect("SSID", "password")

# Enable CSI capture
wlan.csi_enable(buffer_size=32)  # Optional: configure buffer size

# Read a frame (returns tuple or None)
# Tuple order: (rssi, channel, mac, timestamp, local_timestamp, data,
#               rate, sig_mode, mcs, cwb, smoothing, not_sounding,
#               aggregation, stbc, fec_coding, sgi, noise_floor, ampdu_cnt,
#               secondary_channel, ant, sig_len, rx_state, agc_gain, fft_gain)
frame = wlan.csi_read()
if frame:
    rssi = frame[0]           # Received signal strength in dBm
    channel = frame[1]        # Wi-Fi channel number
    mac = frame[2]            # Source MAC address (6 bytes)
    timestamp = frame[3]      # Timestamp in microseconds
    data = frame[5]           # CSI raw data (I/Q components)

# Check available frames
available = wlan.csi_available()

# Check dropped frames
dropped = wlan.csi_dropped()

# Disable CSI
wlan.csi_disable()
```

### CSI Frame Structure

Each frame returned by `csi_read()` is a tuple with 24 fields (in order):

- **0 - rssi** (int): Received signal strength in dBm
- **1 - channel** (int): Wi-Fi channel number
- **2 - mac** (bytes): Source MAC address (6 bytes)
- **3 - timestamp** (int): Timestamp in microseconds
- **4 - local_timestamp** (int): Local timestamp from Wi-Fi hardware
- **5 - data** (bytearray): CSI raw data (I/Q components as int8_t values)
- **6 - rate** (int): Data rate
- **7 - sig_mode** (int): Signal mode (legacy, HT, VHT)
- **8 - mcs** (int): Modulation and Coding Scheme index
- **9 - cwb** (int): Channel bandwidth
- **10 - smoothing** (int): Smoothing applied
- **11 - not_sounding** (int): Not sounding frame
- **12 - aggregation** (int): Aggregation
- **13 - stbc** (int): STBC
- **14 - fec_coding** (int): FEC coding
- **15 - sgi** (int): Short GI
- **16 - noise_floor** (int): Background noise level in dBm
- **17 - ampdu_cnt** (int): AMPDU count
- **18 - secondary_channel** (int): Secondary channel
- **19 - ant** (int): Antenna
- **20 - sig_len** (int): Signal length
- **21 - rx_state** (int): RX state
- **22 - agc_gain** (int): AGC gain value, unsigned 0-255 (ESP32-S3/C3/C5/C6 only, 0 on other platforms)
- **23 - fft_gain** (int): FFT scaling gain, signed -128 to 127 (ESP32-S3/C3/C5/C6 only, 0 on other platforms)

### Configuration Options

`csi_enable()` accepts optional keyword arguments:

- **`buffer_size`** (int, default=16): Number of frames to store in buffer (1-1024)
  - Each frame uses ~540 bytes of RAM
  - Larger buffer = less frame drops but more RAM usage
  - Recommended: 32-128 for most applications

- **`legacy_ltf`** (bool, default=False): Enable Legacy Long Training Field CSI
  - Note: On modern API devices (C5/C6), legacy CSI is always enabled internally

- **`ht_ltf`** (bool, default=True): Enable HT Long Training Field CSI (PRIMARY - best SNR)

- **`stbc`** (bool, default=False): Enable STBC HT-LTF2 CSI (legacy API) or HE-STBC CSI (modern API, C6 only)

- **`capture_ack`** (bool, default=False): Enable capture of 802.11 ACK frames (adds noise, not useful)

- **`scale`** (int or None, default=None): CSI data scaling value
  - ``None`` or ``0``: Automatic scaling (recommended)
  - ``1-15``: Manual scaling with the specified shift value

For most applications, the default settings work well. Only adjust if you need specific CSI characteristics.

## Understanding CSI Data

### CSI Data Format

The `data` field contains I/Q (In-phase/Quadrature) components as `int8_t` values:
- Pairs of values: `[I0, Q0, I1, Q1, I2, Q2, ...]`
- Each pair represents one subcarrier
- Typical length: 52-128 bytes (26-64 subcarriers for HT20)

### Calculating Amplitude

For turbulence monitoring and analysis, you often need amplitude:

```python
def calculate_amplitude(csi_data):
    amplitudes = []
    for i in range(0, len(csi_data) - 1, 2):
        i_val = csi_data[i]
        q_val = csi_data[i + 1] if i + 1 < len(csi_data) else 0
        amplitude = math.sqrt(i_val * i_val + q_val * q_val)
        amplitudes.append(amplitude)
    return amplitudes
```

### Buffer Management

CSI frames arrive asynchronously from the Wi-Fi hardware. The circular buffer handles this:

- **Producer**: Wi-Fi hardware writes frames (ISR context)
- **Consumer**: Python code reads frames at its own pace
- **Buffer overflow**: If Python is too slow, frames are dropped (tracked by `csi_dropped()`)

**Memory calculation:**
```
RAM usage ≈ buffer_size × 540 bytes
Example: 64 frames × 540 bytes = ~34 KB
```

## Troubleshooting

### No frames received

- **Check Wi-Fi connection**: CSI requires an active connection
- **Check for traffic**: CSI is captured from received packets - ensure there's Wi-Fi activity
- **Verify firmware**: Make sure CSI is enabled in build configuration
  - If using ``ESP32_GENERIC_C6`` or ``ESP32_GENERIC_S3`` with ``CSI`` variant, CSI is pre-enabled
  - For other boards, ensure ``CONFIG_ESP_WIFI_CSI_ENABLED=y`` is set
- **Check buffer**: Use `csi_available()` to see if frames are queued

### Too many dropped frames

- **Increase buffer size**: Use `buffer_size` parameter in `csi_enable()`
- **Read faster**: Process frames more frequently in your loop
- **Reduce processing**: Do less work per frame

### Turbulence values seem incorrect

- **Traffic**: Ensure consistent Wi-Fi traffic is active (ping every 100-200ms)
- **Subcarriers**: Adjust `SELECTED_SUBCARRIERS` - center frequencies (47-58) are typically most stable
- **Environment**: Works best in stable environments with fixed router/ESP32 positions
- **Processing**: Check for dropped frames - may indicate buffer overflow

### Memory issues

- **Reduce buffer size**: Use smaller `buffer_size` (minimum: 1)
- **Free memory**: Close other applications, reduce heap usage
- **Frame processing**: Process and discard frames quickly

## Platform-Specific Notes

### Gain Lock Support

| Platform | Gain Lock | Notes |
|----------|-----------|-------|
| ESP32-S3 | ✅ Supported | Full AGC/FFT control |
| ESP32-C3 | ✅ Supported | Full AGC/FFT control |
| ESP32-C5 | ✅ Supported | Full AGC/FFT control |
| ESP32-C6 | ✅ Supported | Full AGC/FFT control |
| ESP32 | ❌ Not available | Use software gain compensation |
| ESP32-S2 | ❌ Not available | Use software gain compensation |

On unsupported platforms, `csi_force_gain()` raises `NotImplementedError`. Use `csi_gain_lock_supported()` to check.

### ESP32-C6 / ESP32-C5

- Uses newer ESP-IDF 5.x CSI API structure
- Full gain lock support
- **Recommended board**: Use ``ESP32_GENERIC_C6`` board with ``CSI`` variant

### ESP32-S3 / ESP32-C3

- Uses legacy ESP-IDF CSI API structure
- Full `rx_ctrl` structure available
- Full gain lock support
- **Recommended board**: Use ``ESP32_GENERIC_S3`` or ``ESP32_GENERIC_C3`` board with ``CSI`` variant

### ESP32 (Original)

- Uses legacy ESP-IDF CSI API structure
- Full `rx_ctrl` structure available
- **No gain lock support** - use software gain compensation for stable CSI
- **Recommended board**: Use ``ESP32_GENERIC`` board with ``CSI`` variant

### ESP32-S2

- Uses legacy ESP-IDF CSI API structure
- **No gain lock support** - use software gain compensation

## Further Reading

- [ESP-IDF Wi-Fi CSI Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_wifi.html#wi-fi-channel-state-information)
- ESPectre project (Wi-Fi-based motion detection system): https://github.com/francescopace/espectre
- Research papers on Wi-Fi sensing and CSI-based analysis

## License

These examples are part of the MicroPython project and are licensed under the MIT License.


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

## Examples

### 1. `csi_basic.py` - Basic CSI Capture

A simple example showing how to:
- Connect to Wi-Fi
- Enable CSI capture
- Read and display CSI frames

**Usage:**
```python
import examples.csi.csi_basic
```

**What it does:**
- Connects to Wi-Fi
- Enables CSI with a 32-frame buffer
- Continuously reads and displays CSI frame information
- Shows RSSI, channel, MAC address, CSI data length, and more

**Best for:** Learning the basics of CSI capture and understanding the data format.

### 2. `csi_turbolence_monitor.py` - Turbulence Monitoring

Demonstrates how to calculate spatial turbulence using CSI data by analyzing
the standard deviation of subcarrier amplitudes.

**Usage:**
```python
import examples.csi.csi_turbolence_monitor
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
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wlan.connect("SSID", "password")

# Enable CSI capture
wlan.csi_enable(buffer_size=32)  # Optional: configure buffer size

# Read a frame (returns dict or None)
frame = wlan.csi_read()

# Check available frames
available = wlan.csi_available()

# Check dropped frames
dropped = wlan.csi_dropped()

# Disable CSI
wlan.csi_disable()
```

### CSI Frame Structure

Each frame returned by `csi_read()` is a dictionary with:

- **`rssi`** (int): Received signal strength in dBm
- **`channel`** (int): Wi-Fi channel number
- **`mac`** (bytes): Source MAC address (6 bytes)
- **`timestamp`** (int): Timestamp in microseconds
- **`data`** (bytearray): CSI raw data (I/Q components)
- **`rate`**, **`mcs`**, **`sig_mode`**: Wi-Fi transmission parameters
- **`noise_floor`** (int): Background noise level
- Additional metadata fields (see code for full list)

### Configuration Options

`csi_enable()` accepts optional keyword arguments:

- **`buffer_size`** (int, default=16): Number of frames to store in buffer (1-1024)
  - Each frame uses ~552 bytes of RAM
  - Larger buffer = less frame drops but more RAM usage
  - Recommended: 32-128 for most applications

- **`lltf_en`** (bool, default=True): Enable Legacy Long Training Field CSI
- **`htltf_en`** (bool, default=True): Enable HT Long Training Field CSI
- **`stbc_htltf2_en`** (bool, default=True): Enable STBC HT-LTF2 CSI
- **`ltf_merge_en`** (bool, default=True): Merge L-LTF and HT-LTF data
- **`channel_filter_en`** (bool, default=True): Enable channel filter
- **`manu_scale`** (bool, default=False): Manual scaling mode
- **`shift`** (int, default=0): Shift value for manual scaling (0-15)

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
RAM usage ≈ buffer_size × 552 bytes
Example: 64 frames × 552 bytes = ~35 KB
```

## Troubleshooting

### No frames received

- **Check Wi-Fi connection**: CSI requires an active connection
- **Check for traffic**: CSI is captured from received packets - ensure there's Wi-Fi activity
- **Verify firmware**: Make sure CSI is enabled in build configuration
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

### ESP32-C6 / ESP32-C5 (Wi-Fi 6)

- Supports 802.11ax (Wi-Fi 6) CSI capture
- Uses new ESP-IDF 5.x CSI API
- Enhanced CSI capabilities with HE-LTF support

### ESP32 / ESP32-S2 / ESP32-S3 / ESP32-C3

- Supports 802.11b/g/n CSI capture
- Uses legacy CSI API
- Full `rx_ctrl` structure available

## Further Reading

- [ESP-IDF Wi-Fi CSI Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_wifi.html#wi-fi-channel-state-information)
- ESPectre project (Wi-Fi-based motion detection system): https://github.com/francescopace/espectre
- Research papers on Wi-Fi sensing and CSI-based analysis

## License

These examples are part of the MicroPython project and are licensed under the MIT License.


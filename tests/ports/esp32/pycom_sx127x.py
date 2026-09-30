# Test the SX127x LoRa driver on Pycom LoPy/LoPy4 boards.
# This test requires a Pycom board with SX1272 or SX1276 radio.

try:
    from machine import Pin
except ImportError:
    print("SKIP")
    raise SystemExit

# Check for LORA_CS pin name (only defined on Pycom boards)
try:
    Pin("LORA_CS")
except ValueError:
    print("SKIP")
    raise SystemExit

from machine import SPI
from sx127x import SX127x

# Set up SPI bus using named pins
spi = SPI(
    1,
    baudrate=10000000,
    polarity=0,
    phase=0,
    sck=Pin("LORA_SCLK"),
    mosi=Pin("LORA_MOSI"),
    miso=Pin("LORA_MISO"),
)

# Try to get reset pin (not all boards have it)
try:
    reset_pin = Pin("LORA_RESET", Pin.OUT)
except ValueError:
    reset_pin = None

# Test 1: Chip detection and initialization
radio = SX127x(
    spi=spi,
    cs_pin=Pin("LORA_CS"),
    dio0_pin=Pin("LORA_DIO0"),
    reset_pin=reset_pin,
)
print("chip:", radio._chip in (1272, 1276))
print("init: OK")

# Test 2: Frequency setting (868 MHz)
radio.set_frequency(868000000)
print("freq: OK")

# Test 3: Spreading factor
for sf in range(6, 13):
    radio.set_spreading_factor(sf)
print("sf 6-12: OK")

# Test SF out of range
try:
    radio.set_spreading_factor(5)
except ValueError:
    print("sf 5: ValueError")
try:
    radio.set_spreading_factor(13)
except ValueError:
    print("sf 13: ValueError")

# Test 4: Bandwidth (common values supported by both chips)
for bw in [125000, 250000, 500000]:
    radio.set_bandwidth(bw)
print("bw: OK")

# Test invalid BW
try:
    radio.set_bandwidth(99999)
except ValueError:
    print("bw invalid: ValueError")

# Test 5: Coding rate
for cr in range(5, 9):
    radio.set_coding_rate(cr)
print("cr 5-8: OK")

try:
    radio.set_coding_rate(4)
except ValueError:
    print("cr 4: ValueError")

# Test 6: TX power
radio.set_tx_power(2)
radio.set_tx_power(14)
radio.set_tx_power(17)
radio.set_tx_power(20)
print("tx_power: OK")

# Test 7: Preamble, sync word, CRC, implicit header
radio.set_preamble_length(8)
radio.set_sync_word(0x12)
radio.set_crc(True)
radio.set_crc(False)
radio.set_implicit_header(True)
radio.set_implicit_header(False)
print("config: OK")

# Test 8: RSSI and SNR properties (just verify they don't error)
_ = radio.rssi
_ = radio.snr
print("rssi/snr: OK")

# Test 9: Sleep and standby
radio.sleep()
radio.standby()
print("power modes: OK")

print("all tests passed")

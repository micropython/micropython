"""
SX1272/SX1276 LoRa radio driver for MicroPython.

Unified driver handling register-level differences between SX1272 and SX1276.
Based on Semtech datasheets DS_SX1272/73_V4 and DS_SX1276-7-8-9_W_APP_V7.
"""

import time
from machine import Pin
from micropython import const

# Common registers
_REG_FIFO = const(0x00)
_REG_OP_MODE = const(0x01)
_REG_FRF_MSB = const(0x06)
_REG_FRF_MID = const(0x07)
_REG_FRF_LSB = const(0x08)
_REG_PA_CONFIG = const(0x09)
_REG_OCP = const(0x0B)
_REG_LNA = const(0x0C)
_REG_FIFO_ADDR_PTR = const(0x0D)
_REG_FIFO_TX_BASE_ADDR = const(0x0E)
_REG_FIFO_RX_BASE_ADDR = const(0x0F)
_REG_FIFO_RX_CURRENT_ADDR = const(0x10)
_REG_IRQ_FLAGS_MASK = const(0x11)
_REG_IRQ_FLAGS = const(0x12)
_REG_RX_NB_BYTES = const(0x13)
_REG_PKT_SNR_VALUE = const(0x19)
_REG_PKT_RSSI_VALUE = const(0x1A)
_REG_MODEM_CONFIG_1 = const(0x1D)
_REG_MODEM_CONFIG_2 = const(0x1E)
_REG_SYMB_TIMEOUT_LSB = const(0x1F)
_REG_PREAMBLE_MSB = const(0x20)
_REG_PREAMBLE_LSB = const(0x21)
_REG_PAYLOAD_LENGTH = const(0x22)
_REG_MAX_PAYLOAD_LENGTH = const(0x23)
_REG_MODEM_CONFIG_3 = const(0x26)
_REG_DETECTION_OPTIMIZE = const(0x31)
_REG_DETECTION_THRESHOLD = const(0x37)
_REG_SYNC_WORD = const(0x39)
_REG_DIO_MAPPING_1 = const(0x40)
_REG_VERSION = const(0x42)
_REG_PA_DAC = const(0x4D)

# Modes
_MODE_LONG_RANGE = const(0x80)
_MODE_SLEEP = const(0x00)
_MODE_STDBY = const(0x01)
_MODE_TX = const(0x03)
_MODE_RX_CONTINUOUS = const(0x05)
_MODE_RX_SINGLE = const(0x06)

# IRQ flags
_IRQ_TX_DONE = const(0x08)
_IRQ_RX_DONE = const(0x40)
_IRQ_RX_TIMEOUT = const(0x80)
_IRQ_PAYLOAD_CRC_ERROR = const(0x20)

# PA config
_PA_BOOST = const(0x80)

# Expected chip version register values
_VERSION_SX1272 = const(0x22)
_VERSION_SX1276 = const(0x12)

# Bandwidth encoding tables
_BW_TABLE_SX1276 = {
    7800: 0,
    10400: 1,
    15600: 2,
    20800: 3,
    31250: 4,
    41700: 5,
    62500: 6,
    125000: 7,
    250000: 8,
    500000: 9,
}
_BW_TABLE_SX1272 = {
    125000: 0,
    250000: 1,
    500000: 2,
}

# FXOSC / 2^19
_FSTEP = 61.03515625  # Hz per step


class SX127x:
    """Low-level driver for SX1272 and SX1276 LoRa transceivers."""

    def __init__(self, spi, cs_pin, dio0_pin, reset_pin=-1, chip=None):
        """
        Args:
            spi: machine.SPI instance, already initialized
            cs_pin: machine.Pin for chip select (active low)
            dio0_pin: machine.Pin for DIO0 interrupt
            reset_pin: machine.Pin for hardware reset, or -1 if not available
            chip: 1272 or 1276; if None, auto-detect from version register
        """
        self._spi = spi
        self._cs = cs_pin
        self._cs.init(Pin.OUT, value=1)
        self._dio0 = dio0_pin
        self._dio0.init(Pin.IN)
        self._reset_pin = reset_pin

        self._frequency = 0
        self._tx_power = 14
        self._irq_callback = None

        # Hardware reset if available
        self.reset()

        # Detect chip variant
        version = self._read_reg(_REG_VERSION)
        if chip is not None:
            self._chip = chip
        elif version == _VERSION_SX1272:
            self._chip = 1272
        elif version == _VERSION_SX1276:
            self._chip = 1276
        else:
            raise RuntimeError("Unknown SX127x chip version: 0x{:02x}".format(version))

        # Put into sleep mode (LoRa)
        self._write_reg(_REG_OP_MODE, _MODE_LONG_RANGE | _MODE_SLEEP)
        time.sleep_ms(10)

        # Set FIFO base addresses
        self._write_reg(_REG_FIFO_TX_BASE_ADDR, 0)
        self._write_reg(_REG_FIFO_RX_BASE_ADDR, 0)

        # Set LNA boost
        self._write_reg(_REG_LNA, self._read_reg(_REG_LNA) | 0x03)

        # Set auto AGC
        if self._chip == 1276:
            self._write_reg(_REG_MODEM_CONFIG_3, 0x04)  # AGC auto on
        else:
            # SX1272: AGC auto on is bit 2 of ModemConfig2 (register 0x1E)
            val = self._read_reg(_REG_MODEM_CONFIG_2)
            self._write_reg(_REG_MODEM_CONFIG_2, val | 0x04)

        # Standby mode
        self.standby()

    def reset(self):
        """Perform hardware or soft reset."""
        if self._reset_pin != -1 and self._reset_pin is not None:
            pin = self._reset_pin
            if not isinstance(pin, Pin):
                pin = Pin(pin, Pin.OUT)
            pin.value(0)
            time.sleep_ms(10)
            pin.value(1)
            time.sleep_ms(10)
        else:
            # Soft reset: write mode register to reset state
            # Switch to FSK sleep then back to LoRa sleep
            self._write_reg(_REG_OP_MODE, 0x00)  # FSK sleep
            time.sleep_ms(10)
            self._write_reg(_REG_OP_MODE, _MODE_LONG_RANGE | _MODE_SLEEP)
            time.sleep_ms(10)

    def standby(self):
        """Enter standby mode."""
        self._write_reg(_REG_OP_MODE, _MODE_LONG_RANGE | _MODE_STDBY)

    def sleep(self):
        """Enter sleep mode."""
        self._write_reg(_REG_OP_MODE, _MODE_LONG_RANGE | _MODE_SLEEP)

    def set_frequency(self, freq_hz):
        """Set carrier frequency in Hz."""
        self._frequency = int(freq_hz)
        frf = int(freq_hz / _FSTEP)
        self._write_reg(_REG_FRF_MSB, (frf >> 16) & 0xFF)
        self._write_reg(_REG_FRF_MID, (frf >> 8) & 0xFF)
        self._write_reg(_REG_FRF_LSB, frf & 0xFF)

    def set_spreading_factor(self, sf):
        """Set spreading factor (6-12)."""
        if sf < 6 or sf > 12:
            raise ValueError("SF must be 6-12")

        # Detection optimize and threshold for SF6
        if sf == 6:
            self._write_reg(_REG_DETECTION_OPTIMIZE, 0xC5)
            self._write_reg(_REG_DETECTION_THRESHOLD, 0x0C)
        else:
            self._write_reg(_REG_DETECTION_OPTIMIZE, 0xC3)
            self._write_reg(_REG_DETECTION_THRESHOLD, 0x0A)

        val = self._read_reg(_REG_MODEM_CONFIG_2)
        self._write_reg(_REG_MODEM_CONFIG_2, (val & 0x0F) | (sf << 4))

    def set_bandwidth(self, bw_hz):
        """Set signal bandwidth in Hz."""
        if self._chip == 1276:
            if bw_hz not in _BW_TABLE_SX1276:
                raise ValueError("Unsupported BW for SX1276")
            bw_val = _BW_TABLE_SX1276[bw_hz]
            val = self._read_reg(_REG_MODEM_CONFIG_1)
            self._write_reg(_REG_MODEM_CONFIG_1, (val & 0x0F) | (bw_val << 4))
        else:
            if bw_hz not in _BW_TABLE_SX1272:
                raise ValueError("Unsupported BW for SX1272")
            bw_val = _BW_TABLE_SX1272[bw_hz]
            val = self._read_reg(_REG_MODEM_CONFIG_1)
            self._write_reg(_REG_MODEM_CONFIG_1, (val & 0x3F) | (bw_val << 6))

    def set_coding_rate(self, cr):
        """Set coding rate denominator (5-8, i.e. 4/5 to 4/8)."""
        if cr < 5 or cr > 8:
            raise ValueError("CR must be 5-8")
        cr_val = cr - 4
        val = self._read_reg(_REG_MODEM_CONFIG_1)
        if self._chip == 1276:
            self._write_reg(_REG_MODEM_CONFIG_1, (val & 0xF1) | (cr_val << 1))
        else:
            self._write_reg(_REG_MODEM_CONFIG_1, (val & 0xC7) | (cr_val << 3))

    def set_preamble_length(self, length):
        """Set preamble length in symbols."""
        self._write_reg(_REG_PREAMBLE_MSB, (length >> 8) & 0xFF)
        self._write_reg(_REG_PREAMBLE_LSB, length & 0xFF)

    def set_sync_word(self, sw):
        """Set sync word (0x12 = private, 0x34 = LoRaWAN public)."""
        self._write_reg(_REG_SYNC_WORD, sw)

    def set_tx_power(self, level, use_pa_boost=True):
        """Set transmit power in dBm."""
        self._tx_power = level
        if use_pa_boost:
            if level > 17:
                # Enable +20dBm on PA_BOOST
                level = min(level, 20)
                self._write_reg(_REG_PA_DAC, 0x87)
                self.set_ocp(140)
                self._write_reg(_REG_PA_CONFIG, _PA_BOOST | (level - 5))
            else:
                self._write_reg(_REG_PA_DAC, 0x84)  # default
                self.set_ocp(100)
                level = max(2, min(level, 17))
                self._write_reg(_REG_PA_CONFIG, _PA_BOOST | (level - 2))
        else:
            # RFO pin
            level = max(0, min(level, 14))
            self._write_reg(_REG_PA_CONFIG, 0x70 | level)

    def set_ocp(self, ma):
        """Set over-current protection trim in mA."""
        if ma <= 120:
            ocp_trim = int((ma - 45) / 5)
        elif ma <= 240:
            ocp_trim = int((ma + 30) / 10)
        else:
            ocp_trim = 27
        self._write_reg(_REG_OCP, 0x20 | (ocp_trim & 0x1F))

    def set_implicit_header(self, implicit):
        """Enable or disable implicit header mode."""
        val = self._read_reg(_REG_MODEM_CONFIG_1)
        if self._chip == 1276:
            if implicit:
                self._write_reg(_REG_MODEM_CONFIG_1, val | 0x01)
            else:
                self._write_reg(_REG_MODEM_CONFIG_1, val & 0xFE)
        else:
            if implicit:
                self._write_reg(_REG_MODEM_CONFIG_1, val | 0x04)
            else:
                self._write_reg(_REG_MODEM_CONFIG_1, val & 0xFB)

    def set_crc(self, enable):
        """Enable or disable CRC checking."""
        if self._chip == 1276:
            val = self._read_reg(_REG_MODEM_CONFIG_2)
            if enable:
                self._write_reg(_REG_MODEM_CONFIG_2, val | 0x04)
            else:
                self._write_reg(_REG_MODEM_CONFIG_2, val & 0xFB)
        else:
            val = self._read_reg(_REG_MODEM_CONFIG_1)
            if enable:
                self._write_reg(_REG_MODEM_CONFIG_1, val | 0x02)
            else:
                self._write_reg(_REG_MODEM_CONFIG_1, val & 0xFD)

    def send(self, data):
        """Transmit data (bytes). Blocks until TX_DONE."""
        self.standby()

        # Set FIFO pointer to TX base
        self._write_reg(_REG_FIFO_ADDR_PTR, 0)
        # Write payload
        self._write_buf(_REG_FIFO, data)
        # Set payload length
        self._write_reg(_REG_PAYLOAD_LENGTH, len(data))

        # Clear IRQ flags
        self._write_reg(_REG_IRQ_FLAGS, 0xFF)

        # Map DIO0 to TX_DONE
        self._write_reg(_REG_DIO_MAPPING_1, 0x40)

        # Start TX
        self._write_reg(_REG_OP_MODE, _MODE_LONG_RANGE | _MODE_TX)

        # Wait for TX_DONE
        while not (self._read_reg(_REG_IRQ_FLAGS) & _IRQ_TX_DONE):
            time.sleep_ms(1)

        # Clear IRQ flags
        self._write_reg(_REG_IRQ_FLAGS, 0xFF)
        self.standby()

    def recv(self, timeout_ms=0):
        """
        Receive a packet.

        Args:
            timeout_ms: 0 = single RX with register timeout, >0 = poll for ms

        Returns:
            bytes or None if timeout/CRC error
        """
        self.standby()

        # Clear IRQ flags
        self._write_reg(_REG_IRQ_FLAGS, 0xFF)

        # Map DIO0 to RX_DONE
        self._write_reg(_REG_DIO_MAPPING_1, 0x00)

        if timeout_ms > 0:
            # Continuous RX, poll with Python timeout
            self._write_reg(_REG_OP_MODE, _MODE_LONG_RANGE | _MODE_RX_CONTINUOUS)
            start = time.ticks_ms()
            while time.ticks_diff(time.ticks_ms(), start) < timeout_ms:
                irq = self._read_reg(_REG_IRQ_FLAGS)
                if irq & _IRQ_RX_DONE:
                    break
                time.sleep_ms(1)
            else:
                self.standby()
                return None
        else:
            # Single RX with hardware timeout
            self._write_reg(_REG_OP_MODE, _MODE_LONG_RANGE | _MODE_RX_SINGLE)
            while True:
                irq = self._read_reg(_REG_IRQ_FLAGS)
                if irq & (_IRQ_RX_DONE | _IRQ_RX_TIMEOUT):
                    break
                time.sleep_ms(1)
            if irq & _IRQ_RX_TIMEOUT:
                self._write_reg(_REG_IRQ_FLAGS, 0xFF)
                self.standby()
                return None

        # Check CRC
        irq = self._read_reg(_REG_IRQ_FLAGS)
        self._write_reg(_REG_IRQ_FLAGS, 0xFF)

        if irq & _IRQ_PAYLOAD_CRC_ERROR:
            self.standby()
            return None

        # Read payload
        nb_bytes = self._read_reg(_REG_RX_NB_BYTES)
        rx_addr = self._read_reg(_REG_FIFO_RX_CURRENT_ADDR)
        self._write_reg(_REG_FIFO_ADDR_PTR, rx_addr)
        payload = self._read_buf(_REG_FIFO, nb_bytes)

        self.standby()
        return bytes(payload)

    def on_recv(self, callback):
        """
        Set up asynchronous receive via DIO0 interrupt.

        callback(payload_bytes) is called when a packet is received.
        Pass None to disable.
        """
        if callback is None:
            self._dio0.irq(handler=None)
            self._irq_callback = None
            self.standby()
            return

        self._irq_callback = callback
        self.standby()

        # Clear IRQ flags
        self._write_reg(_REG_IRQ_FLAGS, 0xFF)

        # Map DIO0 to RX_DONE
        self._write_reg(_REG_DIO_MAPPING_1, 0x00)

        # Set up IRQ
        self._dio0.irq(trigger=Pin.IRQ_RISING, handler=self._handle_irq)

        # Start continuous RX
        self._write_reg(_REG_OP_MODE, _MODE_LONG_RANGE | _MODE_RX_CONTINUOUS)

    def _handle_irq(self, pin):
        """DIO0 interrupt handler — reads IRQ flags to disambiguate."""
        irq = self._read_reg(_REG_IRQ_FLAGS)
        self._write_reg(_REG_IRQ_FLAGS, 0xFF)

        if irq & _IRQ_RX_DONE and not (irq & _IRQ_PAYLOAD_CRC_ERROR):
            nb_bytes = self._read_reg(_REG_RX_NB_BYTES)
            rx_addr = self._read_reg(_REG_FIFO_RX_CURRENT_ADDR)
            self._write_reg(_REG_FIFO_ADDR_PTR, rx_addr)
            payload = self._read_buf(_REG_FIFO, nb_bytes)
            if self._irq_callback:
                self._irq_callback(bytes(payload))

    @property
    def rssi(self):
        """Last packet RSSI in dBm."""
        raw = self._read_reg(_REG_PKT_RSSI_VALUE)
        if self._chip == 1272:
            return -139 + raw
        else:
            # SX1276 HF port (>862 MHz)
            if self._frequency >= 862000000:
                return -157 + raw
            else:
                return -164 + raw

    @property
    def snr(self):
        """Last packet SNR in dB."""
        raw = self._read_reg(_REG_PKT_SNR_VALUE)
        if raw > 127:
            raw -= 256
        return raw * 0.25

    # --- SPI register access ---

    def _read_reg(self, addr):
        self._cs.value(0)
        self._spi.write(bytearray([addr & 0x7F]))
        result = self._spi.read(1)
        self._cs.value(1)
        return result[0]

    def _write_reg(self, addr, value):
        self._cs.value(0)
        self._spi.write(bytearray([addr | 0x80, value]))
        self._cs.value(1)

    def _read_buf(self, addr, length):
        self._cs.value(0)
        self._spi.write(bytearray([addr & 0x7F]))
        result = self._spi.read(length)
        self._cs.value(1)
        return result

    def _write_buf(self, addr, data):
        self._cs.value(0)
        self._spi.write(bytearray([addr | 0x80]) + data)
        self._cs.value(1)

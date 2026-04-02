"""
LoRaWAN 1.0.x MAC layer for MicroPython.

Implements Class A device operation with OTAA and ABP activation.
Runs on top of the SX127x raw LoRa driver.

Usage (OTAA):
    from lora import LoRa
    from lorawan import LoRaWAN

    radio = LoRa(frequency=868100000, sf=7, bw=125000)
    wan = LoRaWAN(radio, mode=LoRaWAN.OTAA,
                  dev_eui=bytes.fromhex('...'),
                  app_eui=bytes.fromhex('...'),
                  app_key=bytes.fromhex('...'))
    wan.join()
    wan.send(1, b'Hello')

Usage (ABP):
    wan = LoRaWAN(radio, mode=LoRaWAN.ABP,
                  dev_addr=0x01234567,
                  nwk_skey=bytes.fromhex('...'),
                  app_skey=bytes.fromhex('...'))
    wan.send(1, b'Hello')
"""

from micropython import const
import struct
import time
import os
from lorawan_crypto import (
    aes_cmac,
    aes_ctr_encrypt,
    aes_ctr_decrypt,
    compute_mic,
    derive_session_keys,
)

# LoRaWAN MHDR message types
_MTYPE_JOIN_REQUEST = const(0x00)
_MTYPE_JOIN_ACCEPT = const(0x20)
_MTYPE_UNCONFIRMED_UP = const(0x40)
_MTYPE_UNCONFIRMED_DOWN = const(0x60)
_MTYPE_CONFIRMED_UP = const(0x80)
_MTYPE_CONFIRMED_DOWN = const(0xA0)

# Direction constants
_DIR_UP = const(0)
_DIR_DOWN = const(1)

# LoRaWAN data rate table for EU868
_DR_TABLE_EU868 = {
    0: (12, 125000),
    1: (11, 125000),
    2: (10, 125000),
    3: (9, 125000),
    4: (8, 125000),
    5: (7, 125000),
}

# Default RX2 parameters (EU868)
_RX2_FREQ = 869525000
_RX2_DR = 0  # SF12/125kHz

# Class A timing
_JOIN_ACCEPT_DELAY1 = 5000  # ms
_JOIN_ACCEPT_DELAY2 = 6000  # ms
_RECEIVE_DELAY1 = 1000  # ms
_RECEIVE_DELAY2 = 2000  # ms


class LoRaWAN:
    """LoRaWAN 1.0.x Class A device."""

    OTAA = 0
    ABP = 1

    def __init__(
        self,
        radio,
        mode=0,
        dev_eui=None,
        app_eui=None,
        app_key=None,
        dev_addr=None,
        nwk_skey=None,
        app_skey=None,
    ):
        """
        Initialize LoRaWAN MAC layer.

        Args:
            radio: LoRa radio instance (from lora module)
            mode: LoRaWAN.OTAA or LoRaWAN.ABP
            dev_eui: 8-byte Device EUI (OTAA)
            app_eui: 8-byte Application EUI / JoinEUI (OTAA)
            app_key: 16-byte Application Key (OTAA)
            dev_addr: 4-byte Device Address as int (ABP)
            nwk_skey: 16-byte Network Session Key (ABP)
            app_skey: 16-byte Application Session Key (ABP)
        """
        self._radio = radio
        self._mode = mode
        self._joined = False

        # Frame counters
        self._fcnt_up = 0
        self._fcnt_down = 0

        # RX1 delay and data rate offset
        self._rx1_delay = _RECEIVE_DELAY1
        self._rx1_dr_offset = 0

        if mode == self.OTAA:
            if not all([dev_eui, app_eui, app_key]):
                raise ValueError("OTAA requires dev_eui, app_eui, app_key")
            self._dev_eui = dev_eui
            self._app_eui = app_eui
            self._app_key = app_key
            self._dev_addr = None
            self._nwk_skey = None
            self._app_skey = None
        elif mode == self.ABP:
            if not all([dev_addr is not None, nwk_skey, app_skey]):
                raise ValueError("ABP requires dev_addr, nwk_skey, app_skey")
            self._dev_addr = dev_addr
            self._nwk_skey = nwk_skey
            self._app_skey = app_skey
            self._joined = True
        else:
            raise ValueError("mode must be OTAA or ABP")

    @property
    def joined(self):
        """Whether the device has joined the network."""
        return self._joined

    @property
    def dev_addr(self):
        """Device address (available after join)."""
        return self._dev_addr

    def join(self, timeout=30000):
        """
        Perform OTAA join procedure.

        Args:
            timeout: Total timeout in ms for join attempts (default 30s)

        Raises:
            RuntimeError: If join fails after timeout
        """
        if self._mode != self.OTAA:
            raise RuntimeError("join() only for OTAA mode")

        # Configure radio for join
        self._radio._radio.set_sync_word(0x34)  # LoRaWAN public sync word

        start = time.ticks_ms()
        while time.ticks_diff(time.ticks_ms(), start) < timeout:
            dev_nonce = os.urandom(2)
            join_req = self._build_join_request(dev_nonce)

            # Send join request
            self._radio._radio.send(join_req)
            tx_end = time.ticks_ms()

            # RX1 window: JoinAcceptDelay1
            accept = self._rx_window(tx_end, _JOIN_ACCEPT_DELAY1)
            if accept:
                if self._process_join_accept(accept, dev_nonce):
                    self._joined = True
                    return True

            # RX2 window: JoinAcceptDelay2 at RX2 frequency/DR
            sf, bw = _DR_TABLE_EU868[_RX2_DR]
            saved_freq = self._radio._radio._frequency
            self._radio._radio.set_frequency(_RX2_FREQ)
            self._radio._radio.set_spreading_factor(sf)
            self._radio._radio.set_bandwidth(bw)

            accept = self._rx_window(tx_end, _JOIN_ACCEPT_DELAY2)

            # Restore original settings
            self._radio._radio.set_frequency(saved_freq)
            self._radio._radio.set_spreading_factor(7)
            self._radio._radio.set_bandwidth(125000)

            if accept:
                if self._process_join_accept(accept, dev_nonce):
                    self._joined = True
                    return True

            # Backoff before retry
            time.sleep_ms(1000)

        raise RuntimeError("Join failed: timeout")

    def send(self, port, payload, confirmed=False):
        """
        Send an uplink frame.

        Args:
            port: FPort (1-223)
            payload: Data bytes to send
            confirmed: If True, send confirmed uplink

        Returns:
            Downlink data bytes or None
        """
        if not self._joined:
            raise RuntimeError("Not joined")

        if port < 1 or port > 223:
            raise ValueError("FPort must be 1-223")

        mtype = _MTYPE_CONFIRMED_UP if confirmed else _MTYPE_UNCONFIRMED_UP
        frame = self._build_data_frame(mtype, port, payload)

        # Send frame
        self._radio._radio.set_sync_word(0x34)
        self._radio._radio.send(frame)
        tx_end = time.ticks_ms()

        self._fcnt_up += 1

        # Class A: open RX1, then RX2
        # RX1
        rx_data = self._rx_window(tx_end, self._rx1_delay)
        if rx_data:
            return self._process_downlink(rx_data)

        # RX2
        sf, bw = _DR_TABLE_EU868[_RX2_DR]
        saved_freq = self._radio._radio._frequency
        self._radio._radio.set_frequency(_RX2_FREQ)
        self._radio._radio.set_spreading_factor(sf)
        self._radio._radio.set_bandwidth(bw)

        rx_data = self._rx_window(tx_end, _RECEIVE_DELAY2)

        self._radio._radio.set_frequency(saved_freq)
        self._radio._radio.set_spreading_factor(7)
        self._radio._radio.set_bandwidth(125000)

        if rx_data:
            return self._process_downlink(rx_data)

        return None

    def _rx_window(self, tx_end, delay_ms):
        """Open a receive window at tx_end + delay_ms."""
        now = time.ticks_ms()
        wait = time.ticks_diff(tx_end + delay_ms, now)
        if wait > 0:
            time.sleep_ms(wait)
        return self._radio._radio.recv(timeout_ms=1000)

    def _build_join_request(self, dev_nonce):
        """Build LoRaWAN join-request message."""
        mhdr = bytes([_MTYPE_JOIN_REQUEST | 0x00])  # Major=0
        # AppEUI and DevEUI in little-endian
        body = mhdr + self._app_eui[::-1] + self._dev_eui[::-1] + dev_nonce
        mic = aes_cmac(self._app_key, body)[:4]
        return body + mic

    def _process_join_accept(self, data, dev_nonce):
        """Process and validate a join-accept message."""
        if len(data) < 17:
            return False

        # Decrypt join-accept (the entire payload after MHDR is encrypted)
        mhdr = data[0]
        if (mhdr & 0xE0) != _MTYPE_JOIN_ACCEPT:
            return False

        from ucryptolib import aes

        encrypted = data[1:]
        decrypted = bytearray()
        for i in range(0, len(encrypted), 16):
            block = encrypted[i : i + 16]
            if len(block) < 16:
                block = block + b"\x00" * (16 - len(block))
            decrypted += aes(self._app_key, 1).encrypt(block)

        # Parse: AppNonce(3) | NetID(3) | DevAddr(4) | DLSettings(1) | RxDelay(1) [| CFList]
        app_nonce = bytes(decrypted[0:3])
        net_id = bytes(decrypted[3:6])
        self._dev_addr = struct.unpack("<I", decrypted[6:10])[0]
        dl_settings = decrypted[10]
        rx_delay = decrypted[11]

        # Verify MIC
        mic_msg = bytes([mhdr]) + bytes(decrypted[:-4])
        mic_calc = aes_cmac(self._app_key, mic_msg)[:4]
        mic_recv = bytes(decrypted[-4:])

        if mic_calc != mic_recv:
            return False

        # Parse DLSettings
        self._rx1_dr_offset = (dl_settings >> 4) & 0x07

        # Parse RxDelay
        if rx_delay == 0:
            self._rx1_delay = _RECEIVE_DELAY1
        else:
            self._rx1_delay = rx_delay * 1000

        # Derive session keys
        self._nwk_skey, self._app_skey = derive_session_keys(
            self._app_key, app_nonce, net_id, dev_nonce
        )

        # Reset frame counters
        self._fcnt_up = 0
        self._fcnt_down = 0

        return True

    def _build_data_frame(self, mtype, port, payload):
        """Build a LoRaWAN data frame (uplink)."""
        mhdr = bytes([mtype | 0x00])

        # FHDR: DevAddr(4) | FCtrl(1) | FCnt(2)
        fctrl = 0x00  # No ADR, no ACK, no FOptsLen
        fhdr = struct.pack("<IBH", self._dev_addr, fctrl, self._fcnt_up & 0xFFFF)

        fport = bytes([port])

        # Encrypt payload
        enc_payload = aes_ctr_encrypt(
            self._app_skey, payload, self._dev_addr, self._fcnt_up, _DIR_UP
        )

        # Assemble frame (without MIC)
        frame = mhdr + fhdr + fport + enc_payload

        # Compute and append MIC
        mic = compute_mic(
            self._nwk_skey, frame, self._dev_addr, self._fcnt_up, _DIR_UP, len(frame)
        )

        return frame + mic

    def _process_downlink(self, data):
        """Process a downlink frame, return decrypted payload or None."""
        if len(data) < 12:
            return None

        mhdr = data[0]
        mtype = mhdr & 0xE0
        if mtype not in (_MTYPE_UNCONFIRMED_DOWN, _MTYPE_CONFIRMED_DOWN):
            return None

        # Parse FHDR
        dev_addr = struct.unpack("<I", data[1:5])[0]
        if dev_addr != self._dev_addr:
            return None

        fctrl = data[5]
        fopts_len = fctrl & 0x0F
        fcnt_low = struct.unpack("<H", data[6:8])[0]

        # Reconstruct full fcnt (assume no rollover for now)
        fcnt = (self._fcnt_down & 0xFFFF0000) | fcnt_low
        if fcnt < self._fcnt_down:
            return None  # Replay protection

        fhdr_len = 8 + fopts_len
        frame_no_mic = data[:-4]
        mic_recv = data[-4:]

        # Verify MIC
        mic_calc = compute_mic(
            self._nwk_skey, frame_no_mic, dev_addr, fcnt, _DIR_DOWN, len(frame_no_mic)
        )
        if mic_calc != mic_recv:
            return None

        self._fcnt_down = fcnt + 1

        # Extract payload
        if len(data) <= fhdr_len + 1 + 4:
            return b""  # No payload (MAC commands only or empty)

        fport = data[fhdr_len]
        enc_payload = data[fhdr_len + 1 : -4]

        # Decrypt
        if fport == 0:
            key = self._nwk_skey
        else:
            key = self._app_skey

        return aes_ctr_decrypt(key, enc_payload, dev_addr, fcnt, _DIR_DOWN)

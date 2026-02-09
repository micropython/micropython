"""Reusable aioble OTA transfer helper.

This helper manages the BLE transport aspects of an OTA update by parsing a
fixed header, validating CRC32 checksums, and forwarding chunk payloads to the
transport-agnostic writer defined in ``ota_ble``. It pauses the host BLE class'
regular RX/TX loops while the transfer is in progress so acknowledgements and
errors can be sent immediately.
"""

import binascii
import struct
import uasyncio as asyncio
import aioble
import machine
import time

from app.ota_ble import OTABinaryWriter, OTAError

HEADER_FORMAT = "<HBIHI"
HEADER_SIZE = struct.calcsize(HEADER_FORMAT)
MAGIC_WORD = 0xB17E
CMD_DATA = 0x01
CMD_FINALIZE = 0x02
CMD_ABORT = 0x03


class AiobleOTAError(Exception):
    pass


class _OtaProtocolError(Exception):
    def __init__(self, code: str, detail: str | None = None):
        self.code = code
        self.detail = detail
        super().__init__(code if detail is None else f"{code}/{detail}")


class AiobleOTA:
    """Coordinates BLE streaming with the low-level OTA writer."""

    def __init__(
        self,
        ble,
        *,
        writer: OTABinaryWriter | None = None,
        status_cb=None,
        magic: int = MAGIC_WORD,
    ):
        self.ble = ble
        self.writer = writer or OTABinaryWriter()
        self.status_cb = status_cb
        self.magic = magic
        self._active = False
        self._expected_seq = 0
        self._session_lock = asyncio.Lock()

    @property
    def active(self) -> bool:
        return self._active

    async def start_session(self, total_bytes: int, sha_hex: str) -> None:
        async with self._session_lock:
            if self._active:
                await self._notify_status("ota/error/busy")
                return
            self._active = True
            await self.ble.pause_transport_tasks()
            try:
                self.writer.begin(length=total_bytes, sha_hex=sha_hex)
                self._expected_seq = 0
                # Single required gate for the host: do not send frames until this.
                await self._notify_status("ota/status/stream")
                await self._stream_frames()
                await self._notify_status("ota/status/verifying")
                part = self.writer.finalize(reboot=False)
                await self._notify_status(f"ota/status/complete/{part}")
                await self._notify_status("ota/status/rebooting")
                # Give the notify a moment to flush before reset.
                await asyncio.sleep_ms(200)
                machine.reset()
            except (_OtaProtocolError, AiobleOTAError, OTAError) as exc:
                self.writer.abort()
                await self._notify_error(exc)
            finally:
                self._active = False
                await self.ble.resume_transport_tasks()

    async def _stream_frames(self) -> None:
        last_progress_ms = time.ticks_ms()
        while True:
            try:
                _, packet = await self.ble.rx_char.written()
            except aioble.DeviceDisconnectedError as exc:
                raise _OtaProtocolError("disconnect") from exc
            cmd, seq, payload = self._decode_frame(packet)
            if cmd == CMD_DATA:
                if seq != self._expected_seq:
                    raise _OtaProtocolError("seq")
                self.writer.append(payload)
                self._expected_seq += 1
                progress = self.writer.received_bytes
                total = self.writer.expected_bytes or 0
                now = time.ticks_ms()
                if total and progress >= total:
                    await self._notify_status(f"ota/progress/{progress}/{total}")
                    last_progress_ms = now
                elif time.ticks_diff(now, last_progress_ms) >= 1000:
                    await self._notify_status(f"ota/progress/{progress}/{total}")
                    last_progress_ms = now
            elif cmd == CMD_FINALIZE:
                if seq != self._expected_seq:
                    raise _OtaProtocolError("seq")
                return
            elif cmd == CMD_ABORT:
                raise _OtaProtocolError("host_abort")
            else:
                raise _OtaProtocolError("unknown_cmd")

    def _decode_frame(self, packet: bytes | bytearray | memoryview):
        if len(packet) < HEADER_SIZE:
            raise _OtaProtocolError("frame_short")
        magic, cmd, seq, length, crc = struct.unpack_from(HEADER_FORMAT, packet)
        if magic != self.magic:
            raise _OtaProtocolError("magic")
        payload = packet[HEADER_SIZE:HEADER_SIZE + length]
        if len(payload) != length:
            raise _OtaProtocolError("len")
        calc_crc = binascii.crc32(payload) & 0xFFFFFFFF
        if calc_crc != crc:
            raise _OtaProtocolError("crc")
        return cmd, seq, payload

    async def _notify_status(self, message: str) -> None:
        if not self.status_cb or not self.ble.connected:
            return
        try:
            payload = message.encode() if isinstance(message, str) else message
            self.status_cb(payload)
        except Exception:
            pass
        await asyncio.sleep(0)

    async def _notify_error(self, exc: Exception) -> None:
        code = "internal"
        detail = None
        if isinstance(exc, _OtaProtocolError):
            code = exc.code
            detail = exc.detail
        elif isinstance(exc, AiobleOTAError):
            code = "internal"
        elif isinstance(exc, OTAError):
            msg = str(exc)
            # Map OTA writer failures to stable codes.
            if "SHA mismatch" in msg:
                code = "sha"
            elif "expected" in msg and "Received" in msg:
                code = "size"
            elif "Firmware larger" in msg:
                code = "size"
            elif "Unaligned" in msg:
                code = "flash"
            else:
                code = "flash"
        suffix = "" if detail is None else f"/{detail}"
        await self._notify_status(f"ota/error/{code}{suffix}")

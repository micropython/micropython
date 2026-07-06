#!/usr/bin/env python3
"""
Receive ESP32-C3 continuous ADC UDP audio packets and save decoded PCM as WAV.

Packet header, little-endian, 8 bytes:
  <H B B H h
  seq, codec, step_index, sample_rate_hz, predictor

Codec 0: PCM16 payload, little-endian signed 16-bit samples.
Codec 1: IMA-style ADPCM4 payload; predictor is sample 0, nibbles are low-first.

Decoded sample count is derived from the UDP datagram length:
  PCM16  -> payload_bytes / 2
  ADPCM4 -> 1 + payload_bytes * 2

The sender pads ADPCM4 packets to odd decoded sample counts when necessary, so
ADPCM4 packet length is self-delimiting without a sample_count header field.
"""

from __future__ import annotations

import argparse
import socket
import struct
import sys
import time
import wave
from dataclasses import dataclass
from typing import Iterable

CODEC_PCM16 = 0
CODEC_ADPCM4 = 1
HEADER = struct.Struct("<HBBHh")  # seq, codec, step_index, sample_rate_hz, predictor
DEFAULT_SAMPLES_PER_PACKET = 320

IMA_STEP_TABLE = [
    7, 8, 9, 10, 11, 12, 13, 14,
    16, 17, 19, 21, 23, 25, 28, 31,
    34, 37, 41, 45, 50, 55, 60, 66,
    73, 80, 88, 97, 107, 118, 130, 143,
    157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658,
    724, 796, 876, 963, 1060, 1166, 1282, 1411,
    1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024,
    3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484,
    7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794,
    32767,
]

IMA_INDEX_TABLE = [
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
]


def clip_i16(x: int) -> int:
    return max(-32768, min(32767, x))


def decode_ima_nibble(code: int, predictor: int, step_index: int) -> tuple[int, int]:
    step = IMA_STEP_TABLE[step_index]
    diffq = step >> 3
    if code & 4:
        diffq += step
    if code & 2:
        diffq += step >> 1
    if code & 1:
        diffq += step >> 2

    predictor = predictor - diffq if (code & 8) else predictor + diffq
    predictor = clip_i16(predictor)

    step_index += IMA_INDEX_TABLE[code & 0x0F]
    step_index = max(0, min(88, step_index))
    return predictor, step_index


def sample_count_from_payload_len(codec: int, payload_len: int) -> int:
    if codec == CODEC_PCM16:
        if payload_len & 1:
            raise ValueError(f"PCM16 payload has odd byte length: {payload_len}")
        return payload_len // 2
    if codec == CODEC_ADPCM4:
        return 1 + payload_len * 2
    raise ValueError(f"unknown codec {codec}")


def decode_adpcm4_into(
    payload: memoryview,
    predictor: int,
    step_index: int,
    out: bytearray,
) -> memoryview:
    if not 0 <= step_index <= 88:
        raise ValueError(f"bad ADPCM step index {step_index}")

    sample_count = sample_count_from_payload_len(CODEC_ADPCM4, len(payload))
    needed_out = sample_count * 2
    if len(out) < needed_out:
        raise ValueError("internal ADPCM output buffer too small")

    struct.pack_into("<h", out, 0, predictor)
    out_pos = 2
    for i in range(sample_count - 1):
        b = payload[i >> 1]
        code = (b & 0x0F) if (i & 1) == 0 else ((b >> 4) & 0x0F)
        predictor, step_index = decode_ima_nibble(code, predictor, step_index)
        struct.pack_into("<h", out, out_pos, predictor)
        out_pos += 2

    return memoryview(out)[:needed_out]


def decode_pcm16(payload: memoryview) -> memoryview:
    sample_count_from_payload_len(CODEC_PCM16, len(payload))
    return payload


@dataclass
class Stats:
    packets: int = 0
    lost_packets: int = 0
    reordered_or_old_packets: int = 0
    bad_packets: int = 0
    samples: int = 0
    first_codec: int | None = None
    sample_rate: int | None = None
    sample_rate_mismatches: int = 0


def seq_distance(expected: int, got: int) -> int:
    return (got - expected) & 0xFFFF


def codec_name(codec: int) -> str:
    if codec == CODEC_ADPCM4:
        return "ADPCM4"
    if codec == CODEC_PCM16:
        return "PCM16"
    return f"unknown({codec})"


def write_silence_frames(wf: wave.Wave_write, frames: int, chunk_frames: int = 4096) -> None:
    zero_chunk = b"\x00\x00" * min(frames, chunk_frames)
    while frames > 0:
        n = min(frames, chunk_frames)
        wf.writeframes(zero_chunk[: n * 2])
        frames -= n


def receive_to_wav(
    bind: str,
    port: int,
    sample_rate_override: int | None,
    out_path: str,
    timeout: float,
    max_seconds: float | None,
    fill_loss: bool,
    max_loss_fill_packets: int,
) -> Stats:
    if sample_rate_override is not None and sample_rate_override <= 0:
        raise ValueError("sample-rate override must be positive")
    if not 0 < port <= 65535:
        raise ValueError("port must be in 1..65535")
    if timeout <= 0:
        raise ValueError("timeout must be positive")
    if max_seconds is not None and max_seconds <= 0:
        raise ValueError("max seconds must be positive")
    if max_loss_fill_packets < 0:
        raise ValueError("max loss fill packets cannot be negative")

    stats = Stats(sample_rate=sample_rate_override)
    expected_seq: int | None = None
    last_samples_per_packet = DEFAULT_SAMPLES_PER_PACKET
    deadline = None if max_seconds is None else time.monotonic() + max_seconds
    adpcm_out = bytearray((DEFAULT_SAMPLES_PER_PACKET + 1) * 2)
    wf: wave.Wave_write | None = None
    warned_rate_mismatch = False

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        sock.bind((bind, port))
        sock.settimeout(timeout)

        if sample_rate_override is None:
            print(f"Listening on {bind}:{port}; WAV sample rate will come from the first valid packet.")
        else:
            print(f"Listening on {bind}:{port}; writing {out_path} at override rate {sample_rate_override} Hz.")
        print("Stop with Ctrl+C, --max-seconds, or a zero-length datagram from the ESP32.")

        while True:
            if deadline is not None and time.monotonic() >= deadline:
                print("Reached max seconds; stopping.")
                break

            try:
                datagram, addr = sock.recvfrom(2048)
            except socket.timeout:
                continue

            if not datagram:
                print("Received zero-length stop datagram.")
                break

            if len(datagram) < HEADER.size:
                stats.bad_packets += 1
                continue

            seq, codec, step_index, header_sample_rate, predictor = HEADER.unpack_from(datagram)
            payload = memoryview(datagram)[HEADER.size:]

            if codec not in (CODEC_PCM16, CODEC_ADPCM4):
                stats.bad_packets += 1
                if stats.first_codec is None:
                    print(f"Ignoring first packet from {addr}: unknown codec {codec}", file=sys.stderr)
                continue

            if header_sample_rate <= 0:
                stats.bad_packets += 1
                print(f"Bad packet {seq}: invalid header sample rate {header_sample_rate}", file=sys.stderr)
                continue

            if sample_rate_override is not None:
                effective_sample_rate = sample_rate_override
                if header_sample_rate != sample_rate_override:
                    stats.sample_rate_mismatches += 1
                    if not warned_rate_mismatch:
                        warned_rate_mismatch = True
                        print(
                            f"Warning: packet sample rate is {header_sample_rate} Hz, "
                            f"but WAV override is {sample_rate_override} Hz; using override.",
                            file=sys.stderr,
                        )
            else:
                effective_sample_rate = stats.sample_rate or header_sample_rate
                if stats.sample_rate is not None and header_sample_rate != stats.sample_rate:
                    stats.sample_rate_mismatches += 1
                    if not warned_rate_mismatch:
                        warned_rate_mismatch = True
                        print(
                            f"Warning: packet sample rate changed from {stats.sample_rate} Hz "
                            f"to {header_sample_rate} Hz; keeping the WAV rate fixed.",
                            file=sys.stderr,
                        )

            if wf is None:
                stats.sample_rate = effective_sample_rate
                wf = wave.open(out_path, "wb")
                wf.setnchannels(1)
                wf.setsampwidth(2)
                wf.setframerate(effective_sample_rate)
                print(f"Writing {out_path} at {effective_sample_rate} Hz")

            if stats.first_codec is None:
                stats.first_codec = codec
                print(f"First valid packet from {addr}, codec={codec_name(codec)}")

            if expected_seq is not None and seq != expected_seq:
                gap = seq_distance(expected_seq, seq)
                if 0 < gap < 32768:
                    stats.lost_packets += gap
                    print(f"Packet gap: expected {expected_seq}, got {seq}; missing {gap}")
                    if fill_loss:
                        fill_packets = min(gap, max_loss_fill_packets)
                        silence_frames = fill_packets * last_samples_per_packet
                        write_silence_frames(wf, silence_frames)
                        stats.samples += silence_frames
                        if fill_packets < gap:
                            print(
                                f"Skipped silence fill for {gap - fill_packets} packets "
                                f"to avoid a huge WAV allocation."
                            )
                else:
                    stats.reordered_or_old_packets += 1
                    print(f"Ignoring old/reordered packet: expected {expected_seq}, got {seq}")
                    continue

            try:
                sample_count = sample_count_from_payload_len(codec, len(payload))
                if codec == CODEC_PCM16:
                    pcm = decode_pcm16(payload)
                else:
                    needed = sample_count * 2
                    if len(adpcm_out) < needed:
                        adpcm_out = bytearray(needed)
                    pcm = decode_adpcm4_into(payload, predictor, step_index, adpcm_out)
            except (ValueError, struct.error) as exc:
                stats.bad_packets += 1
                print(f"Bad packet {seq}: {exc}", file=sys.stderr)
                continue

            wf.writeframes(pcm)
            stats.packets += 1
            stats.samples += sample_count
            if sample_count:
                last_samples_per_packet = sample_count
            expected_seq = (seq + 1) & 0xFFFF

    except KeyboardInterrupt:
        print("Interrupted.")
    finally:
        if wf is not None:
            wf.close()
        sock.close()

    return stats


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Receive ESP32-C3 continuous ADC UDP audio and save WAV.")
    parser.add_argument("--bind", default="0.0.0.0", help="local address to bind, default: 0.0.0.0")
    parser.add_argument("--port", type=int, default=5000, help="UDP port, default: 5000")
    parser.add_argument(
        "--sample-rate",
        type=int,
        default=None,
        help="optional WAV sample-rate override; by default the receiver uses the packet header sample rate",
    )
    parser.add_argument("--out", default="capture.wav", help="output WAV file, default: capture.wav")
    parser.add_argument("--timeout", type=float, default=1.0, help="socket timeout in seconds, default: 1.0")
    parser.add_argument("--max-seconds", type=float, default=None, help="optional recording duration limit")
    parser.add_argument("--no-fill-loss", action="store_true", help="do not insert silence for missing UDP packets")
    parser.add_argument(
        "--max-loss-fill-packets",
        type=int,
        default=200,
        help="maximum missing packets to fill with silence per gap, default: 200",
    )
    return parser


def main(argv: Iterable[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)

    try:
        stats = receive_to_wav(
            bind=args.bind,
            port=args.port,
            sample_rate_override=args.sample_rate,
            out_path=args.out,
            timeout=args.timeout,
            max_seconds=args.max_seconds,
            fill_loss=not args.no_fill_loss,
            max_loss_fill_packets=args.max_loss_fill_packets,
        )
    except (OSError, ValueError) as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 2

    duration = stats.samples / stats.sample_rate if stats.sample_rate else 0.0
    print(
        f"Done. packets={stats.packets}, lost={stats.lost_packets}, "
        f"old/reordered={stats.reordered_or_old_packets}, bad={stats.bad_packets}, "
        f"rate={stats.sample_rate or 'unknown'} Hz, rate_mismatches={stats.sample_rate_mismatches}, "
        f"samples={stats.samples}, duration={duration:.2f}s"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

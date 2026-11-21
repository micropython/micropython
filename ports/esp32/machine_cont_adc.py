#!/usr/bin/env python3
"""
Receive ESP32-C3 continuous ADC UDP audio packets and save decoded PCM as WAV.

It can also read an existing WAV file with --input-wav/--in-wav, convert it
to mono PCM16, optionally apply the same receiver-side IIR filtering, and
write a new WAV file without using UDP.

Recommended offline denoise chain for ESP32-C3 ADC speech recordings:
  --denoise-noise FILE   Use a separate noise-only WAV file as the noise profile.
                         The script removes the noise-file DC/bias offset, applies
                         STFT spectral noise reduction, then applies a speech
                         band-pass filter. This mode is for --input-wav processing.

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

Optional receiver-side IIR filtering:
  --iir-noise FILE       Generate an IIR notch-bank filter from a noise-only WAV file.
  --iir-coeffs FILE      Load IIR coefficients directly from JSON or text.
  --iir-coeff-count N    Number of notches/coefficients to generate from noise, default: 64.
  --iir-coeff-out FILE   Save generated coefficients for reuse.

Coefficient file formats accepted by --iir-coeffs:
  1) JSON direct-form IIR:
       {"b": [b0, b1, ...], "a": [1, a1, ...]}

  2) JSON biquad cascade:
       {"sections": [{"b": [b0,b1,b2], "a": [1,a1,a2]}, ...]}
       or {"sections": [[b0,b1,b2,a1,a2], ...]}

  3) Plain text, one biquad per line:
       b0 b1 b2 a1 a2
       b0 b1 b2 a0 a1 a2

  4) Plain text or JSON flat numeric list:
       64 64 64
     This is interpreted as a cascade of ESP-style first-order smoothers:
       y += (x - y) / coeff
"""

from __future__ import annotations

import argparse
import json
import math
import os
import re
import socket
import struct
import sys
import time
import wave
from dataclasses import dataclass
from typing import Iterable, Sequence

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


def clip_i16(x: int | float) -> int:
    return max(-32768, min(32767, int(round(x))))


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


@dataclass
class IIRBuildOptions:
    coeff_file: str | None = None
    noise_file: str | None = None
    coeff_out_file: str | None = None
    coeff_count: int = 64
    notch_radius: float = 0.995
    min_frequency_hz: float = 20.0
    max_frequency_hz: float | None = None


@dataclass
class DenoiseOptions:
    noise_file: str | None = None
    strength: float = 1.35
    floor: float = 0.06
    frame_size: int = 1024
    hop_size: int = 256
    gain_smoothing: float = 0.80
    remove_dc: bool = True
    bandpass_low_hz: float = 100.0
    bandpass_high_hz: float = 3500.0
    bandpass_sections: int = 2


class PCM16IIRFilter:
    """Base class for streaming int16 little-endian IIR filters."""

    description: str = "IIR filter"

    def process_sample(self, sample: float) -> float:
        raise NotImplementedError

    def process_pcm16le(self, pcm: bytes | bytearray | memoryview) -> bytes:
        if len(pcm) & 1:
            raise ValueError("PCM16 buffer has odd byte length")
        out = bytearray(len(pcm))
        out_pos = 0
        for (sample,) in struct.iter_unpack("<h", pcm):
            filtered = self.process_sample(float(sample))
            struct.pack_into("<h", out, out_pos, clip_i16(filtered))
            out_pos += 2
        return bytes(out)


class DirectFormIIR(PCM16IIRFilter):
    """Direct-form I IIR with arbitrary-length b/a coefficients."""

    def __init__(self, b: Sequence[float], a: Sequence[float], description: str = "direct-form IIR") -> None:
        if not b:
            raise ValueError("IIR numerator coefficient list b cannot be empty")
        if not a:
            raise ValueError("IIR denominator coefficient list a cannot be empty")
        if abs(a[0]) < 1e-30:
            raise ValueError("IIR denominator a[0] cannot be zero")

        a0 = float(a[0])
        self.b = [float(v) / a0 for v in b]
        self.a = [float(v) / a0 for v in a]
        self.x_hist = [0.0] * max(0, len(self.b) - 1)
        self.y_hist = [0.0] * max(0, len(self.a) - 1)
        self.description = description

    def process_sample(self, sample: float) -> float:
        y = self.b[0] * sample
        for i, coeff in enumerate(self.b[1:]):
            y += coeff * self.x_hist[i]
        for i, coeff in enumerate(self.a[1:]):
            y -= coeff * self.y_hist[i]

        if self.x_hist:
            self.x_hist[1:] = self.x_hist[:-1]
            self.x_hist[0] = sample
        if self.y_hist:
            self.y_hist[1:] = self.y_hist[:-1]
            self.y_hist[0] = y
        return y


class FirstOrderSmootherCascade(PCM16IIRFilter):
    """Cascade of y += (x - y) / coeff sections."""

    def __init__(self, coeffs: Sequence[float]) -> None:
        if not coeffs:
            raise ValueError("smoother coefficient list cannot be empty")
        self.coeffs = [float(c) for c in coeffs]
        for c in self.coeffs:
            if c < 1.0:
                raise ValueError("smoother coefficients must be >= 1")
        self.state = [0.0] * len(self.coeffs)
        self.initialized = [False] * len(self.coeffs)
        self.description = f"{len(self.coeffs)} first-order smoother section(s)"

    def process_sample(self, sample: float) -> float:
        y = sample
        for i, coeff in enumerate(self.coeffs):
            if not self.initialized[i]:
                self.state[i] = y
                self.initialized[i] = True
            else:
                self.state[i] += (y - self.state[i]) / coeff
            y = self.state[i]
        return y


class BiquadCascadeIIR(PCM16IIRFilter):
    """Cascade of normalized second-order IIR sections using DF-II transposed state."""

    def __init__(self, sections: Sequence[tuple[Sequence[float], Sequence[float]]], description: str = "biquad IIR cascade") -> None:
        if not sections:
            raise ValueError("biquad section list cannot be empty")

        normalized: list[tuple[list[float], list[float]]] = []
        for idx, (b, a) in enumerate(sections):
            if len(b) != 3 or len(a) != 3:
                raise ValueError(f"biquad section {idx} must have 3 b and 3 a coefficients")
            if abs(a[0]) < 1e-30:
                raise ValueError(f"biquad section {idx} has zero a[0]")
            a0 = float(a[0])
            normalized.append(([float(v) / a0 for v in b], [float(v) / a0 for v in a]))

        self.sections = normalized
        self.z1 = [0.0] * len(normalized)
        self.z2 = [0.0] * len(normalized)
        self.description = description

    def process_sample(self, sample: float) -> float:
        y = sample
        for i, (b, a) in enumerate(self.sections):
            x = y
            y = b[0] * x + self.z1[i]
            self.z1[i] = b[1] * x - a[1] * y + self.z2[i]
            self.z2[i] = b[2] * x - a[2] * y
        return y


_NUMBER_RE = re.compile(r"[-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?")


def parse_number_list(text: str) -> list[float]:
    return [float(m.group(0)) for m in _NUMBER_RE.finditer(text)]


def normalize_biquad_from_numbers(values: Sequence[float]) -> tuple[list[float], list[float]]:
    if len(values) == 5:
        b0, b1, b2, a1, a2 = values
        return [b0, b1, b2], [1.0, a1, a2]
    if len(values) == 6:
        b0, b1, b2, a0, a1, a2 = values
        return [b0, b1, b2], [a0, a1, a2]
    raise ValueError("a biquad must contain 5 values (b0 b1 b2 a1 a2) or 6 values (b0 b1 b2 a0 a1 a2)")


def is_number_list(value: object) -> bool:
    return isinstance(value, list) and all(isinstance(x, (int, float)) for x in value)


def json_section_to_biquad(section: object) -> tuple[list[float], list[float]]:
    if isinstance(section, dict):
        if "b" in section and "a" in section:
            b = [float(v) for v in section["b"]]
            a = [float(v) for v in section["a"]]
            if len(b) != 3 or len(a) != 3:
                raise ValueError("JSON biquad section b/a arrays must each contain 3 coefficients")
            return b, a
        if "coeffs" in section:
            return normalize_biquad_from_numbers([float(v) for v in section["coeffs"]])
        raise ValueError("JSON biquad section must contain b/a arrays or coeffs")
    if is_number_list(section):
        return normalize_biquad_from_numbers([float(v) for v in section])
    raise ValueError("invalid JSON biquad section")


def notch_biquad(frequency_hz: float, sample_rate_hz: int, radius: float) -> tuple[list[float], list[float]]:
    if not 0.0 < frequency_hz < sample_rate_hz / 2.0:
        raise ValueError(f"notch frequency {frequency_hz} Hz is outside 0..Nyquist")
    if not 0.0 < radius < 1.0:
        raise ValueError("notch radius must be between 0 and 1")

    w0 = 2.0 * math.pi * frequency_hz / sample_rate_hz
    c = math.cos(w0)
    b = [1.0, -2.0 * c, 1.0]
    a = [1.0, -2.0 * radius * c, radius * radius]

    # Normalize each section to unity DC gain so a large cascade does not
    # unintentionally attenuate the whole recording outside the notches.
    sum_b = b[0] + b[1] + b[2]
    sum_a = a[0] + a[1] + a[2]
    if abs(sum_b) > 1e-30:
        gain = sum_a / sum_b
        b = [v * gain for v in b]
    return b, a


def load_iir_filter_from_json(data: object, target_sample_rate: int, source_name: str) -> PCM16IIRFilter:
    if isinstance(data, dict):
        source_rate = data.get("sample_rate")
        if source_rate is not None:
            try:
                source_rate_int = int(source_rate)
                if source_rate_int != target_sample_rate:
                    print(
                        f"Warning: coefficient file sample_rate={source_rate_int} Hz, "
                        f"recording rate={target_sample_rate} Hz.",
                        file=sys.stderr,
                    )
            except (TypeError, ValueError):
                print(f"Warning: ignoring non-integer coefficient sample_rate={source_rate!r}", file=sys.stderr)

        if "sections" in data:
            sections_raw = data["sections"]
            if not isinstance(sections_raw, list):
                raise ValueError("JSON sections must be a list")

            # If the file came from this script and stores notch frequencies,
            # re-design the biquads for the current target sample rate.
            can_redesign = all(
                isinstance(s, dict) and "frequency_hz" in s and "r" in s for s in sections_raw
            )
            if can_redesign:
                sections = [
                    notch_biquad(float(s["frequency_hz"]), target_sample_rate, float(s["r"]))
                    for s in sections_raw
                    if 0.0 < float(s["frequency_hz"]) < target_sample_rate / 2.0
                ]
                return BiquadCascadeIIR(
                    sections,
                    description=f"{len(sections)} generated notch biquad section(s) from {source_name}",
                )

            sections = [json_section_to_biquad(section) for section in sections_raw]
            return BiquadCascadeIIR(sections, description=f"{len(sections)} loaded biquad section(s) from {source_name}")

        if "b" in data and "a" in data:
            b = [float(v) for v in data["b"]]
            a = [float(v) for v in data["a"]]
            return DirectFormIIR(b, a, description=f"direct-form IIR from {source_name}: len(b)={len(b)}, len(a)={len(a)}")

        if "smoothing" in data:
            coeffs = [float(v) for v in data["smoothing"]]
            return FirstOrderSmootherCascade(coeffs)

        raise ValueError("JSON coefficient file must contain sections, b/a, or smoothing")

    if is_number_list(data):
        values = [float(v) for v in data]
        if len(values) in (5, 6):
            return BiquadCascadeIIR([normalize_biquad_from_numbers(values)], description=f"1 loaded biquad section from {source_name}")
        return FirstOrderSmootherCascade(values)

    if isinstance(data, list):
        sections = [json_section_to_biquad(section) for section in data]
        return BiquadCascadeIIR(sections, description=f"{len(sections)} loaded biquad section(s) from {source_name}")

    raise ValueError("unsupported JSON coefficient file format")


def load_iir_filter_from_text(text: str, target_sample_rate: int, source_name: str) -> PCM16IIRFilter:
    lines: list[str] = []
    for raw_line in text.splitlines():
        line = raw_line.split("#", 1)[0].split(";", 1)[0].strip()
        if line:
            lines.append(line)

    if not lines:
        raise ValueError("coefficient file is empty")

    b_values: list[float] | None = None
    a_values: list[float] | None = None
    unlabeled_lines: list[str] = []

    for line in lines:
        if ":" in line:
            key, value = line.split(":", 1)
            key = key.strip().lower()
            numbers = parse_number_list(value)
            if key in ("b", "num", "numerator", "feedforward"):
                b_values = numbers
            elif key in ("a", "den", "denominator", "feedback"):
                a_values = numbers
            else:
                raise ValueError(f"unknown coefficient line label {key!r}")
        else:
            unlabeled_lines.append(line)

    if b_values is not None or a_values is not None:
        if b_values is None or a_values is None:
            raise ValueError("labeled text coefficient file needs both b: and a: lines")
        return DirectFormIIR(
            b_values,
            a_values,
            description=f"direct-form IIR from {source_name}: len(b)={len(b_values)}, len(a)={len(a_values)}",
        )

    per_line = [parse_number_list(line) for line in unlabeled_lines]
    if per_line and all(len(values) in (5, 6) for values in per_line):
        sections = [normalize_biquad_from_numbers(values) for values in per_line]
        return BiquadCascadeIIR(sections, description=f"{len(sections)} loaded biquad section(s) from {source_name}")

    flat_values = parse_number_list("\n".join(unlabeled_lines))
    if not flat_values:
        raise ValueError("coefficient file does not contain numeric coefficients")
    return FirstOrderSmootherCascade(flat_values)


def load_iir_filter_from_file(path: str, target_sample_rate: int) -> PCM16IIRFilter:
    with open(path, "r", encoding="utf-8") as f:
        text = f.read()

    stripped = text.lstrip()
    if stripped.startswith("{") or stripped.startswith("["):
        data = json.loads(text)
        return load_iir_filter_from_json(data, target_sample_rate, os.path.basename(path))
    return load_iir_filter_from_text(text, target_sample_rate, os.path.basename(path))


def read_wav_mono_samples(path: str) -> tuple[int, list[float]]:
    with wave.open(path, "rb") as wf:
        channels = wf.getnchannels()
        sampwidth = wf.getsampwidth()
        sample_rate = wf.getframerate()
        frame_count = wf.getnframes()
        raw = wf.readframes(frame_count)

    if channels <= 0:
        raise ValueError("WAV file has no channels")
    if frame_count <= 0:
        raise ValueError("WAV file has no samples")

    samples: list[float] = []

    if sampwidth == 1:
        # WAV 8-bit PCM is unsigned.
        vals = [float(b - 128) * 256.0 for b in raw]
    elif sampwidth == 2:
        vals = [float(v[0]) for v in struct.iter_unpack("<h", raw)]
    elif sampwidth == 3:
        vals = []
        for i in range(0, len(raw), 3):
            b0, b1, b2 = raw[i], raw[i + 1], raw[i + 2]
            value = b0 | (b1 << 8) | (b2 << 16)
            if value & 0x800000:
                value -= 0x1000000
            vals.append(float(value) / 256.0)
    elif sampwidth == 4:
        vals = [float(v[0]) / 65536.0 for v in struct.iter_unpack("<i", raw)]
    else:
        raise ValueError(f"unsupported WAV sample width: {sampwidth} bytes")

    if len(vals) % channels != 0:
        raise ValueError("WAV data length is not divisible by channel count")

    for i in range(0, len(vals), channels):
        samples.append(sum(vals[i : i + channels]) / channels)

    return sample_rate, samples


def power_of_two_at_most(value: int) -> int:
    if value < 1:
        return 0
    return 1 << (value.bit_length() - 1)


def fft_inplace(values: list[complex]) -> None:
    n = len(values)
    if n == 0 or n & (n - 1):
        raise ValueError("FFT length must be a power of two")

    j = 0
    for i in range(1, n):
        bit = n >> 1
        while j & bit:
            j ^= bit
            bit >>= 1
        j ^= bit
        if i < j:
            values[i], values[j] = values[j], values[i]

    size = 2
    while size <= n:
        half = size >> 1
        theta = -2.0 * math.pi / size
        w_m = complex(math.cos(theta), math.sin(theta))
        for start in range(0, n, size):
            w = 1.0 + 0.0j
            for k in range(start, start + half):
                t = w * values[k + half]
                u = values[k]
                values[k] = u + t
                values[k + half] = u - t
                w *= w_m
        size <<= 1


def find_noise_peak_frequencies(
    samples: Sequence[float],
    sample_rate: int,
    count: int,
    min_frequency_hz: float,
    max_frequency_hz: float | None,
) -> list[tuple[float, float]]:
    if count <= 0:
        raise ValueError("IIR coefficient count must be positive")
    if sample_rate <= 0:
        raise ValueError("noise sample rate must be positive")
    if len(samples) < 512:
        raise ValueError("noise sample file is too short; need at least 512 samples")

    max_fft_size = 65536
    nfft = power_of_two_at_most(min(len(samples), max_fft_size))
    if nfft < 512:
        raise ValueError("noise sample file is too short after FFT sizing")

    # Use a centered block to avoid startup transients in the noise sample.
    offset = max(0, (len(samples) - nfft) // 2)
    block = [float(x) for x in samples[offset : offset + nfft]]
    mean = sum(block) / len(block)

    # Hann window; power normalization is unnecessary because we only rank peaks.
    spectrum = []
    for n, x in enumerate(block):
        w = 0.5 - 0.5 * math.cos(2.0 * math.pi * n / (nfft - 1))
        spectrum.append(complex((x - mean) * w, 0.0))

    fft_inplace(spectrum)
    bin_hz = sample_rate / nfft
    nyquist = sample_rate / 2.0
    max_hz = max_frequency_hz if max_frequency_hz is not None else nyquist * 0.95
    max_hz = min(max_hz, nyquist * 0.999)
    min_bin = max(1, int(math.ceil(min_frequency_hz / bin_hz)))
    max_bin = min((nfft // 2) - 1, int(math.floor(max_hz / bin_hz)))
    if min_bin >= max_bin:
        raise ValueError("invalid frequency range for noise-derived filter")

    powers = [0.0] * (max_bin + 1)
    for i in range(min_bin, max_bin + 1):
        v = spectrum[i]
        powers[i] = (v.real * v.real) + (v.imag * v.imag)

    candidates: list[tuple[float, int]] = []
    for i in range(min_bin + 1, max_bin):
        if powers[i] > powers[i - 1] and powers[i] >= powers[i + 1]:
            candidates.append((powers[i], i))

    if not candidates:
        # Broadband noise may not have sharp local maxima. Fall back to the
        # strongest bins so the user still gets a deterministic filter.
        candidates = [(powers[i], i) for i in range(min_bin, max_bin + 1)]

    candidates.sort(reverse=True)
    min_spacing_bins = max(1, int(round(5.0 / bin_hz)))
    selected: list[tuple[float, float]] = []
    selected_bins: list[int] = []
    for power, bin_index in candidates:
        if len(selected) >= count:
            break
        if any(abs(bin_index - prev) < min_spacing_bins for prev in selected_bins):
            continue
        selected_bins.append(bin_index)
        selected.append((bin_index * bin_hz, power))

    selected.sort(key=lambda item: item[0])
    return selected


def save_generated_iir_coefficients(
    path: str,
    target_sample_rate: int,
    source_noise_file: str,
    radius: float,
    peaks: Sequence[tuple[float, float]],
) -> None:
    sections = []
    for frequency_hz, power in peaks:
        b, a = notch_biquad(frequency_hz, target_sample_rate, radius)
        sections.append(
            {
                "frequency_hz": frequency_hz,
                "power": power,
                "r": radius,
                "b": b,
                "a": a,
            }
        )

    payload = {
        "type": "biquad_cascade",
        "sample_rate": target_sample_rate,
        "source_noise_file": source_noise_file,
        "sections": sections,
    }
    with open(path, "w", encoding="utf-8") as f:
        json.dump(payload, f, indent=2)
        f.write("\n")


def build_iir_filter_from_noise(
    path: str,
    target_sample_rate: int,
    coeff_count: int,
    radius: float,
    min_frequency_hz: float,
    max_frequency_hz: float | None,
    coeff_out_file: str | None,
) -> PCM16IIRFilter:
    noise_rate, noise_samples = read_wav_mono_samples(path)
    peak_count = coeff_count
    target_max_hz = (target_sample_rate / 2.0) * 0.95
    noise_max_hz = max_frequency_hz if max_frequency_hz is not None else target_max_hz
    noise_max_hz = min(noise_max_hz, (noise_rate / 2.0) * 0.95, target_max_hz)

    peaks = find_noise_peak_frequencies(
        noise_samples,
        noise_rate,
        peak_count,
        min_frequency_hz=min_frequency_hz,
        max_frequency_hz=noise_max_hz,
    )
    if not peaks:
        raise ValueError("could not find noise peaks to build IIR filter")

    sections = [notch_biquad(freq_hz, target_sample_rate, radius) for freq_hz, _ in peaks]

    if coeff_out_file is not None:
        save_generated_iir_coefficients(coeff_out_file, target_sample_rate, path, radius, peaks)
        print(f"Saved generated IIR coefficients to {coeff_out_file}")

    first_freq = peaks[0][0]
    last_freq = peaks[-1][0]
    return BiquadCascadeIIR(
        sections,
        description=(
            f"{len(sections)} noise-derived notch biquad section(s) "
            f"from {os.path.basename(path)} ({first_freq:.1f}..{last_freq:.1f} Hz)"
        ),
    )


def build_iir_filter(options: IIRBuildOptions | None, target_sample_rate: int) -> PCM16IIRFilter | None:
    if options is None:
        return None
    if options.coeff_file and options.noise_file:
        raise ValueError("use either --iir-coeffs or --iir-noise, not both")
    if options.coeff_count <= 0:
        raise ValueError("--iir-coeff-count must be positive")
    if not 0.0 < options.notch_radius < 1.0:
        raise ValueError("--iir-notch-radius must be between 0 and 1")
    if options.min_frequency_hz < 0:
        raise ValueError("--iir-min-frequency must be non-negative")
    if options.max_frequency_hz is not None and options.max_frequency_hz <= options.min_frequency_hz:
        raise ValueError("--iir-max-frequency must be greater than --iir-min-frequency")

    if options.coeff_file:
        return load_iir_filter_from_file(options.coeff_file, target_sample_rate)
    if options.noise_file:
        return build_iir_filter_from_noise(
            path=options.noise_file,
            target_sample_rate=target_sample_rate,
            coeff_count=options.coeff_count,
            radius=options.notch_radius,
            min_frequency_hz=options.min_frequency_hz,
            max_frequency_hz=options.max_frequency_hz,
            coeff_out_file=options.coeff_out_file,
        )
    if options.coeff_out_file:
        raise ValueError("--iir-coeff-out only makes sense with --iir-noise")
    return None


def validate_denoise_options(options: DenoiseOptions | None) -> None:
    if options is None:
        return
    if not options.noise_file:
        raise ValueError("--denoise-noise needs a noise-only WAV file")
    if options.strength < 0.0:
        raise ValueError("--denoise-strength cannot be negative")
    if not 0.0 <= options.floor <= 1.0:
        raise ValueError("--denoise-floor must be in 0..1")
    if options.frame_size < 128 or (options.frame_size & (options.frame_size - 1)):
        raise ValueError("--denoise-frame-size must be a power of two and at least 128")
    if options.hop_size <= 0 or options.hop_size > options.frame_size:
        raise ValueError("--denoise-hop-size must be in 1..frame_size")
    if not 0.0 <= options.gain_smoothing < 1.0:
        raise ValueError("--denoise-gain-smoothing must be in 0..less-than-1")
    if options.bandpass_low_hz < 0.0:
        raise ValueError("--denoise-bandpass-low cannot be negative")
    if options.bandpass_high_hz < 0.0:
        raise ValueError("--denoise-bandpass-high cannot be negative")
    if options.bandpass_sections < 0:
        raise ValueError("--denoise-bandpass-sections cannot be negative")


def mean_value(samples: Sequence[float]) -> float:
    return sum(float(x) for x in samples) / len(samples) if samples else 0.0


def rms_value(samples: Sequence[float]) -> float:
    if not samples:
        return 0.0
    return math.sqrt(sum(float(x) * float(x) for x in samples) / len(samples))


def hann_window(length: int) -> list[float]:
    if length <= 1:
        return [1.0] * max(0, length)
    return [0.5 - 0.5 * math.cos(2.0 * math.pi * n / (length - 1)) for n in range(length)]


def ifft_inplace(values: list[complex]) -> None:
    n = len(values)
    for i, value in enumerate(values):
        values[i] = value.conjugate()
    fft_inplace(values)
    inv_n = 1.0 / n
    for i, value in enumerate(values):
        values[i] = value.conjugate() * inv_n


def stft_noise_profile(
    noise_samples: Sequence[float],
    frame_size: int,
    hop_size: int,
    window: Sequence[float],
) -> list[float]:
    """Return the mean magnitude spectrum of a noise-only sample."""
    if not noise_samples:
        raise ValueError("noise sample file has no samples")

    mag_sum = [0.0] * frame_size
    frame_count = 0
    n = len(noise_samples)
    start = 0
    while start < n or frame_count == 0:
        spectrum = [0j] * frame_size
        for i in range(frame_size):
            idx = start + i
            sample = float(noise_samples[idx]) if idx < n else 0.0
            spectrum[i] = complex(sample * window[i], 0.0)
        fft_inplace(spectrum)
        for i, value in enumerate(spectrum):
            mag_sum[i] += abs(value)
        frame_count += 1
        start += hop_size

    inv = 1.0 / frame_count
    return [v * inv for v in mag_sum]


def spectral_noise_reduce(
    samples: Sequence[float],
    noise_samples: Sequence[float],
    frame_size: int,
    hop_size: int,
    strength: float,
    floor: float,
    gain_smoothing: float,
) -> list[float]:
    """Stationary STFT spectral subtraction with Hann overlap-add.

    It preserves phase and reduces bins whose magnitude is close to the
    noise-only profile. This is intentionally simple and portable; it is best
    for stationary ADC hiss/whine rather than rapidly changing background noise.
    """
    if not samples:
        return []

    window = hann_window(frame_size)
    noise_mag = stft_noise_profile(noise_samples, frame_size, hop_size, window)

    original_len = len(samples)
    # Center the first and last real samples inside several analysis windows.
    # Without this padding, the tiny edge values of the Hann window can cause
    # large normalization spikes at the beginning/end of the output.
    pad = frame_size
    padded = [0.0] * pad + [float(x) for x in samples] + [0.0] * pad
    n = len(padded)
    out = [0.0] * (n + frame_size)
    norm = [0.0] * (n + frame_size)
    prev_gain: list[float] | None = None
    eps = 1e-12

    for start in range(0, n, hop_size):
        spectrum = [0j] * frame_size
        for i in range(frame_size):
            idx = start + i
            sample = padded[idx] if idx < n else 0.0
            spectrum[i] = complex(sample * window[i], 0.0)

        fft_inplace(spectrum)

        gains = [1.0] * frame_size
        for i, value in enumerate(spectrum):
            mag = abs(value)
            gain = 1.0 - strength * (noise_mag[i] / (mag + eps))
            if gain < floor:
                gain = floor
            elif gain > 1.0:
                gain = 1.0
            if prev_gain is not None:
                gain = gain_smoothing * prev_gain[i] + (1.0 - gain_smoothing) * gain
            gains[i] = gain
            spectrum[i] = value * gain
        prev_gain = gains

        ifft_inplace(spectrum)
        for i, value in enumerate(spectrum):
            idx = start + i
            win = window[i]
            out[idx] += value.real * win
            norm[idx] += win * win

    reconstructed: list[float] = []
    for i in range(n):
        if norm[i] > 1e-12:
            reconstructed.append(out[i] / norm[i])
        else:
            reconstructed.append(out[i])
    return reconstructed[pad : pad + original_len]


def rbj_lowpass_biquad(cutoff_hz: float, sample_rate_hz: int, q: float = 0.7071067811865476) -> tuple[list[float], list[float]]:
    if not 0.0 < cutoff_hz < sample_rate_hz / 2.0:
        raise ValueError("low-pass cutoff must be between 0 and Nyquist")
    w0 = 2.0 * math.pi * cutoff_hz / sample_rate_hz
    c = math.cos(w0)
    alpha = math.sin(w0) / (2.0 * q)
    b = [(1.0 - c) * 0.5, 1.0 - c, (1.0 - c) * 0.5]
    a = [1.0 + alpha, -2.0 * c, 1.0 - alpha]
    return b, a


def rbj_highpass_biquad(cutoff_hz: float, sample_rate_hz: int, q: float = 0.7071067811865476) -> tuple[list[float], list[float]]:
    if not 0.0 < cutoff_hz < sample_rate_hz / 2.0:
        raise ValueError("high-pass cutoff must be between 0 and Nyquist")
    w0 = 2.0 * math.pi * cutoff_hz / sample_rate_hz
    c = math.cos(w0)
    alpha = math.sin(w0) / (2.0 * q)
    b = [(1.0 + c) * 0.5, -(1.0 + c), (1.0 + c) * 0.5]
    a = [1.0 + alpha, -2.0 * c, 1.0 - alpha]
    return b, a


def apply_speech_bandpass(
    samples: Sequence[float],
    sample_rate_hz: int,
    low_hz: float,
    high_hz: float,
    sections_per_edge: int,
) -> list[float]:
    """Apply a practical speech band-pass using cascaded biquad HP/LP sections."""
    if sections_per_edge <= 0:
        return [float(x) for x in samples]
    if sample_rate_hz <= 0:
        raise ValueError("sample rate must be positive")

    nyquist = sample_rate_hz / 2.0
    sections: list[tuple[list[float], list[float]]] = []
    if low_hz > 0.0:
        if low_hz >= nyquist:
            raise ValueError("denoise band-pass low cutoff is at or above Nyquist")
        for _ in range(sections_per_edge):
            sections.append(rbj_highpass_biquad(low_hz, sample_rate_hz))
    if high_hz > 0.0:
        if high_hz >= nyquist:
            high_hz = nyquist * 0.95
        if low_hz > 0.0 and high_hz <= low_hz:
            raise ValueError("denoise band-pass high cutoff must be above low cutoff")
        for _ in range(sections_per_edge):
            sections.append(rbj_lowpass_biquad(high_hz, sample_rate_hz))

    if not sections:
        return [float(x) for x in samples]

    filt = BiquadCascadeIIR(sections, description="speech band-pass")
    return [filt.process_sample(float(sample)) for sample in samples]


def apply_recommended_denoise_chain(
    samples: Sequence[float],
    sample_rate_hz: int,
    options: DenoiseOptions,
) -> list[float]:
    """ESP32-C3 ADC speech cleanup: DC removal, spectral denoise, speech band-pass."""
    validate_denoise_options(options)
    if options.noise_file is None:
        raise ValueError("--denoise-noise is required")

    noise_rate, noise_samples = read_wav_mono_samples(options.noise_file)
    if noise_rate != sample_rate_hz:
        raise ValueError(
            f"noise WAV sample rate is {noise_rate} Hz, but input WAV is {sample_rate_hz} Hz; "
            "resample one file first"
        )
    if len(noise_samples) < options.frame_size // 2:
        raise ValueError("noise sample is too short for denoise profile")

    working = [float(x) for x in samples]
    noise_working = [float(x) for x in noise_samples]

    dc = mean_value(noise_working) if options.remove_dc else 0.0
    if options.remove_dc:
        working = [x - dc for x in working]
        noise_working = [x - dc for x in noise_working]

    before_rms = rms_value(working)
    noise_rms = rms_value(noise_working)
    print(
        f"Applying recommended denoise chain: DC offset={dc:.2f} PCM counts, "
        f"noise RMS after DC={noise_rms:.2f}, STFT frame={options.frame_size}, hop={options.hop_size}, "
        f"strength={options.strength}, floor={options.floor}"
    )

    reduced = spectral_noise_reduce(
        samples=working,
        noise_samples=noise_working,
        frame_size=options.frame_size,
        hop_size=options.hop_size,
        strength=options.strength,
        floor=options.floor,
        gain_smoothing=options.gain_smoothing,
    )

    filtered = apply_speech_bandpass(
        reduced,
        sample_rate_hz=sample_rate_hz,
        low_hz=options.bandpass_low_hz,
        high_hz=options.bandpass_high_hz,
        sections_per_edge=options.bandpass_sections,
    )
    after_rms = rms_value(filtered)
    if options.bandpass_sections > 0:
        print(
            f"Applied speech band-pass: {options.bandpass_low_hz:g}..{options.bandpass_high_hz:g} Hz "
            f"({options.bandpass_sections} biquad section(s) per edge)."
        )
    print(f"Denoise chain RMS: before={before_rms:.2f}, after={after_rms:.2f}")
    return filtered


def seq_distance(expected: int, got: int) -> int:
    return (got - expected) & 0xFFFF


def codec_name(codec: int) -> str:
    if codec == CODEC_ADPCM4:
        return "ADPCM4"
    if codec == CODEC_PCM16:
        return "PCM16"
    return f"unknown({codec})"


def write_silence_frames(
    wf: wave.Wave_write,
    frames: int,
    chunk_frames: int = 4096,
    iir_filter: PCM16IIRFilter | None = None,
) -> None:
    zero_chunk = b"\x00\x00" * min(frames, chunk_frames)
    while frames > 0:
        n = min(frames, chunk_frames)
        chunk = zero_chunk[: n * 2]
        if iir_filter is not None:
            chunk = iir_filter.process_pcm16le(chunk)
        wf.writeframes(chunk)
        frames -= n


def process_wav_file_to_wav(
    in_path: str,
    sample_rate_override: int | None,
    out_path: str,
    iir_options: IIRBuildOptions | None = None,
    denoise_options: DenoiseOptions | None = None,
) -> Stats:
    """Read a WAV file, optionally filter it, and write mono PCM16 WAV output.

    The input WAV is mixed down to mono when needed and converted to signed
    16-bit PCM samples. If --sample-rate is provided, no resampling is done;
    the output WAV header is written with the override rate.
    """
    if sample_rate_override is not None and sample_rate_override <= 0:
        raise ValueError("sample-rate override must be positive")

    input_sample_rate, samples = read_wav_mono_samples(in_path)
    effective_sample_rate = sample_rate_override or input_sample_rate

    if sample_rate_override is None:
        print(f"Reading {in_path}; writing {out_path} at input rate {effective_sample_rate} Hz.")
    else:
        print(
            f"Reading {in_path}; writing {out_path} at override rate "
            f"{effective_sample_rate} Hz (input file is {input_sample_rate} Hz)."
        )

    if denoise_options is not None:
        # Denoising is offline and file-based. It uses the true input sample
        # rate because --sample-rate only changes the output WAV header; it
        # does not resample the audio.
        samples = apply_recommended_denoise_chain(samples, input_sample_rate, denoise_options)

    iir_filter = build_iir_filter(iir_options, effective_sample_rate)
    if iir_filter is not None:
        print(f"Applying receiver-side {iir_filter.description}")

    stats = Stats(
        packets=0,
        samples=len(samples),
        first_codec=CODEC_PCM16,
        sample_rate=effective_sample_rate,
        sample_rate_mismatches=int(sample_rate_override is not None and sample_rate_override != input_sample_rate),
    )

    with wave.open(out_path, "wb") as wf:
        wf.setnchannels(1)
        wf.setsampwidth(2)
        wf.setframerate(effective_sample_rate)

        chunk_samples = 4096
        for start in range(0, len(samples), chunk_samples):
            chunk = samples[start : start + chunk_samples]
            pcm = bytearray(len(chunk) * 2)
            out_pos = 0
            for sample in chunk:
                struct.pack_into("<h", pcm, out_pos, clip_i16(sample))
                out_pos += 2

            if iir_filter is not None:
                wf.writeframes(iir_filter.process_pcm16le(pcm))
            else:
                wf.writeframes(pcm)

    return stats


def receive_to_wav(
    bind: str,
    port: int,
    sample_rate_override: int | None,
    out_path: str,
    timeout: float,
    max_seconds: float | None,
    fill_loss: bool,
    max_loss_fill_packets: int,
    iir_options: IIRBuildOptions | None = None,
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
    iir_filter: PCM16IIRFilter | None = None
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
                iir_filter = build_iir_filter(iir_options, effective_sample_rate)
                if iir_filter is not None:
                    print(f"Applying receiver-side {iir_filter.description}")
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
                        write_silence_frames(wf, silence_frames, iir_filter=iir_filter)
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
                pcm_to_write: bytes | memoryview
                if iir_filter is not None:
                    pcm_to_write = iir_filter.process_pcm16le(pcm)
                else:
                    pcm_to_write = pcm
            except (ValueError, struct.error) as exc:
                stats.bad_packets += 1
                print(f"Bad packet {seq}: {exc}", file=sys.stderr)
                continue

            wf.writeframes(pcm_to_write)
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
    parser = argparse.ArgumentParser(description="Receive ESP32-C3 continuous ADC UDP audio, or process a WAV file, and save WAV.")
    parser.add_argument(
        "--input-wav",
        "--in-wav",
        dest="input_wav",
        default=None,
        metavar="WAV",
        help="read audio from this WAV file instead of listening for UDP packets",
    )
    parser.add_argument(
        "--denoise-noise",
        default=None,
        metavar="WAV",
        help=(
            "noise-only WAV profile for the recommended offline denoise chain; "
            "used with --input-wav"
        ),
    )
    parser.add_argument(
        "--denoise-strength",
        type=float,
        default=1.35,
        help="spectral noise-reduction strength, default: 1.35",
    )
    parser.add_argument(
        "--denoise-floor",
        type=float,
        default=0.06,
        help="minimum spectral gain to avoid over-gating, default: 0.06",
    )
    parser.add_argument(
        "--denoise-frame-size",
        type=int,
        default=1024,
        help="STFT frame size, power of two, default: 1024",
    )
    parser.add_argument(
        "--denoise-hop-size",
        type=int,
        default=256,
        help="STFT hop size, default: 256",
    )
    parser.add_argument(
        "--denoise-gain-smoothing",
        type=float,
        default=0.80,
        help="temporal smoothing for spectral gains, default: 0.80",
    )
    parser.add_argument(
        "--denoise-no-dc",
        action="store_true",
        help="do not subtract the DC/bias offset measured from the noise WAV",
    )
    parser.add_argument(
        "--denoise-bandpass-low",
        type=float,
        default=100.0,
        help="speech band-pass low cutoff in Hz, default: 100",
    )
    parser.add_argument(
        "--denoise-bandpass-high",
        type=float,
        default=3500.0,
        help="speech band-pass high cutoff in Hz, default: 3500",
    )
    parser.add_argument(
        "--denoise-bandpass-sections",
        type=int,
        default=2,
        help="number of HP and LP biquad sections per band-pass edge; 0 disables band-pass, default: 2",
    )
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
    parser.add_argument(
        "--iir-noise",
        default=None,
        metavar="WAV",
        help="noise-only WAV used to generate a receiver-side IIR notch-bank filter",
    )
    parser.add_argument(
        "--iir-coeffs",
        default=None,
        metavar="FILE",
        help="load receiver-side IIR coefficients from JSON or text instead of generating them from noise",
    )
    parser.add_argument(
        "--iir-coeff-out",
        default=None,
        metavar="FILE",
        help="write generated noise-derived IIR coefficients to this JSON file for reuse",
    )
    parser.add_argument(
        "--iir-coeff-count",
        type=int,
        default=64,
        help="number of noise-derived notch coefficients/sections to generate, default: 64",
    )
    parser.add_argument(
        "--iir-notch-radius",
        type=float,
        default=0.995,
        help="IIR notch pole radius for noise-derived filters; closer to 1 is narrower, default: 0.995",
    )
    parser.add_argument(
        "--iir-min-frequency",
        type=float,
        default=20.0,
        help="lowest frequency considered when generating a noise-derived filter, default: 20 Hz",
    )
    parser.add_argument(
        "--iir-max-frequency",
        type=float,
        default=None,
        help="highest frequency considered when generating a noise-derived filter, default: 95%% of Nyquist",
    )
    return parser


def main(argv: Iterable[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)

    iir_options = IIRBuildOptions(
        coeff_file=args.iir_coeffs,
        noise_file=args.iir_noise,
        coeff_out_file=args.iir_coeff_out,
        coeff_count=args.iir_coeff_count,
        notch_radius=args.iir_notch_radius,
        min_frequency_hz=args.iir_min_frequency,
        max_frequency_hz=args.iir_max_frequency,
    )
    if not (iir_options.coeff_file or iir_options.noise_file or iir_options.coeff_out_file):
        iir_options = None

    denoise_options = None
    if args.denoise_noise:
        denoise_options = DenoiseOptions(
            noise_file=args.denoise_noise,
            strength=args.denoise_strength,
            floor=args.denoise_floor,
            frame_size=args.denoise_frame_size,
            hop_size=args.denoise_hop_size,
            gain_smoothing=args.denoise_gain_smoothing,
            remove_dc=not args.denoise_no_dc,
            bandpass_low_hz=args.denoise_bandpass_low,
            bandpass_high_hz=args.denoise_bandpass_high,
            bandpass_sections=args.denoise_bandpass_sections,
        )

    try:
        if denoise_options is not None and not args.input_wav:
            raise ValueError("--denoise-noise is an offline WAV-file mode; use it together with --input-wav")

        if args.input_wav:
            stats = process_wav_file_to_wav(
                in_path=args.input_wav,
                sample_rate_override=args.sample_rate,
                out_path=args.out,
                iir_options=iir_options,
                denoise_options=denoise_options,
            )
        else:
            stats = receive_to_wav(
                bind=args.bind,
                port=args.port,
                sample_rate_override=args.sample_rate,
                out_path=args.out,
                timeout=args.timeout,
                max_seconds=args.max_seconds,
                fill_loss=not args.no_fill_loss,
                max_loss_fill_packets=args.max_loss_fill_packets,
                iir_options=iir_options,
            )
    except (OSError, ValueError, json.JSONDecodeError, wave.Error) as exc:
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

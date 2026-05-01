#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2024 Andrew Leech
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import zlib

# Minimum file size to attempt compression testing (bytes).
# Below this, decompression setup overhead on device isn't worth it.
MIN_COMPRESS_SIZE = 256

# Compression ratio threshold: use deflate only if compressed/original < this.
# 0.80 means require at least 20% size reduction.
MIN_COMPRESS_RATIO = 0.80

# Chunk sizes for each encoding method.
# Larger chunks = fewer exec() round trips over serial.
DEFLATE_CHUNK_SIZE = 4096  # Compressed chunks are smaller on wire
BASE64_CHUNK_SIZE = 2048  # Still benefits from fewer round trips

# Device-side decompression window size (512 bytes, minimal RAM).
DEFLATE_WBITS = 9
# Host-side compression: negative = raw deflate (no zlib header).
DEFAULT_WBITS = -DEFLATE_WBITS

# Sample size for compression ratio testing.
COMPRESS_SAMPLE_SIZE = 4096


def compress_chunk(data, wbits=DEFAULT_WBITS):
    """Compress a single chunk using raw deflate.

    Each chunk is independently compressed/decompressible, which is required
    for per-chunk transfer where each exec() call decompresses one chunk.

    Args:
        data: Bytes to compress.
        wbits: Window bits for deflate. Negative = raw deflate (no header).
               Default -9 = 512-byte window, minimal device RAM usage.

    Returns:
        Compressed bytes in raw deflate format.
    """
    compressor = zlib.compressobj(wbits=wbits)
    return compressor.compress(data) + compressor.flush()


def test_compression_ratio(data, sample_size=COMPRESS_SAMPLE_SIZE):
    """Test compression ratio on a data sample.

    Compresses a prefix of the data to estimate overall compressibility
    without processing the entire file.

    Args:
        data: The full data to test.
        sample_size: Max bytes to sample from start of data.

    Returns:
        Ratio of compressed/original size (0.0-1.0+). Lower = better compression.
    """
    sample = data[:sample_size]
    if not sample:
        return 1.0
    compressed = compress_chunk(sample)
    return len(compressed) / len(sample)

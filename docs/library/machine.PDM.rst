.. currentmodule:: machine
.. _machine.PDM:

class PDM -- Digital Microphone Interface
==========================================

PDM (Pulse Density Modulation) is a digital microphone interface commonly used in
embedded systems. This class provides access to PDM microphones on Zephyr-based
boards through the Zephyr DMIC (Digital Microphone) subsystem.

Currently, the PDM class supports blocking mode operation. Non-blocking and asyncio
modes are planned for future implementation.

Constructors
------------

.. class:: PDM(id, *, rate=16000, bits=16, format=PDM.MONO, ibuf=8192)

   Construct a PDM object with the given arguments:

   - *id* identifies the PDM peripheral. It can be a device name string (e.g., ``"pdm0"``)
     or an integer (where ``0`` maps to ``"dmic0"``).

   - *rate* sets the sample rate in Hz. Common values: 8000, 16000, 32000, 48000.
     Default: 16000.

   - *bits* sets the bit depth (16 or 32). Default: 16.

   - *format* specifies the channel configuration:

     - ``PDM.MONO`` - Single channel (default)
     - ``PDM.STEREO`` - Two channels (left and right)

   - *ibuf* sets the internal ring buffer size in bytes. Larger buffers reduce the risk
     of data loss but use more RAM. Default: 8192.

Methods
-------

.. method:: PDM.init(*, rate=16000, bits=16, format=PDM.MONO, ibuf=8192)

   Initialize or reconfigure the PDM interface with the given parameters.
   See the constructor for parameter descriptions.

.. method:: PDM.deinit()

   Deinitialize the PDM interface, stop audio capture, and free resources.

.. method:: PDM.readinto(buf)

   Read audio samples into *buf*, which must be a writeable buffer (e.g., ``bytearray``).
   Blocks until the buffer is completely filled.

   Returns the number of bytes read into the buffer.

Constants
---------

.. data:: PDM.MONO
          PDM.STEREO

   Channel format constants:

   - ``PDM.MONO`` - Single channel audio
   - ``PDM.STEREO`` - Dual channel audio (interleaved left/right samples)

Configuration
-------------

Enabling PDM Support
~~~~~~~~~~~~~~~~~~~~

Add to your board configuration file (e.g., ``boards/xiao_ble_nrf52840_sense.conf``)::

    # Enable MicroPython PDM class
    CONFIG_MICROPY_PY_MACHINE_PDM=y

This automatically enables the required Zephyr subsystems:

- ``CONFIG_AUDIO`` - Audio subsystem
- ``CONFIG_AUDIO_DMIC`` - Digital microphone support
- Platform-specific drivers (e.g., ``CONFIG_AUDIO_DMIC_NRFX_PDM`` for nRF52840)

Device Tree Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~

Enable the PDM peripheral in your board's device tree overlay::

    &pdm0 {
        status = "okay";
    };

    // Enable microphone power regulator (if needed)
    / {
        msm261d3526hicpm-c-en {
            regulator-boot-on;
        };
    };

After configuration changes, rebuild with ``west build -p -b your_board``.

Examples
--------

Simple Audio Capture
~~~~~~~~~~~~~~~~~~~~~

Basic audio capture that blocks until the buffer is full::

    from machine import PDM

    # Initialize PDM at 16kHz, 16-bit, mono
    pdm = PDM("pdm0", rate=16000, bits=16, format=PDM.MONO)
    buf = bytearray(1024)

    # Blocks until buf is full
    num_bytes = pdm.readinto(buf)
    print(f"Read {num_bytes} bytes")

    # Process audio samples
    for i in range(0, num_bytes, 2):
        sample = int.from_bytes(buf[i:i+2], 'little', True)
        print(f"Sample: {sample}")

    pdm.deinit()

Recording to WAV File
~~~~~~~~~~~~~~~~~~~~~

Capture 2 seconds of audio and save as a WAV file::

    from machine import PDM
    import struct
    import time

    SAMPLE_RATE = 16000
    DURATION_SEC = 2
    TOTAL_BYTES = SAMPLE_RATE * 2 * DURATION_SEC  # 16-bit = 2 bytes/sample

    pdm = PDM("pdm0", rate=SAMPLE_RATE, bits=16, format=PDM.MONO)

    # Discard first 500ms to avoid startup transients
    discard_buf = bytearray(SAMPLE_RATE)
    pdm.readinto(discard_buf)
    time.sleep_ms(100)

    with open("audio.wav", "wb") as f:
        # WAV header
        f.write(b"RIFF")
        f.write(struct.pack("<I", 36 + TOTAL_BYTES))
        f.write(b"WAVE")
        f.write(b"fmt ")
        f.write(struct.pack("<I", 16))  # fmt chunk size
        f.write(struct.pack("<H", 1))   # PCM
        f.write(struct.pack("<H", 1))   # Mono
        f.write(struct.pack("<I", SAMPLE_RATE))
        f.write(struct.pack("<I", SAMPLE_RATE * 2))
        f.write(struct.pack("<H", 2))   # Block align
        f.write(struct.pack("<H", 16))  # Bits per sample
        f.write(b"data")
        f.write(struct.pack("<I", TOTAL_BYTES))

        # Record audio
        buf = bytearray(4096)
        bytes_recorded = 0
        while bytes_recorded < TOTAL_BYTES:
            to_read = min(4096, TOTAL_BYTES - bytes_recorded)
            buf_view = memoryview(buf)[:to_read]
            num = pdm.readinto(buf_view)
            f.write(buf_view[:num])
            bytes_recorded += num

    pdm.deinit()
    print("Recording saved to audio.wav")

Stereo Capture
~~~~~~~~~~~~~~

Capture and deinterleave stereo audio (requires hardware with two microphones)::

    from machine import PDM

    pdm = PDM("pdm0", rate=16000, bits=16, format=PDM.STEREO)
    buf = bytearray(2048)  # 4 bytes per stereo frame (2x 16-bit)

    num_bytes = pdm.readinto(buf)

    # Deinterleave left and right channels
    for i in range(0, num_bytes, 4):
        left = int.from_bytes(buf[i:i+2], 'little', True)
        right = int.from_bytes(buf[i+2:i+4], 'little', True)
        print(f"L={left}, R={right}")

    pdm.deinit()

Important Notes
---------------

Microphone Startup Time
~~~~~~~~~~~~~~~~~~~~~~~~

PDM microphones typically require 100-500ms to stabilize after power-on. During this
period, the audio may contain:

- Initial noise/garbage data (~400ms)
- DC offset transients (~200ms)

For clean recordings, discard the first 500ms of captured audio or wait before starting
the recording.

DC Offset
~~~~~~~~~

Many PDM microphones have a DC offset (signal not centered at zero). This is normal
and can be removed by:

1. **Software**: Subtract the average value from each sample
2. **Audio tools**: Use "Normalize" or "Remove DC Offset" in Audacity
3. **Hardware**: Enable the nRF52840's built-in high-pass filter (if available)

Memory Usage
~~~~~~~~~~~~

The PDM implementation uses:

- **Ring buffer**: Size specified by ``ibuf`` parameter (default 8KB)
- **DMIC buffers**: 4 blocks × 4KB = 16KB for hardware DMA
- **Total**: ~24KB + Python object overhead

Performance Considerations
~~~~~~~~~~~~~~~~~~~~~~~~~~

- **Sample rate**: Higher rates (48kHz) require faster processing and larger buffers
- **Buffer size**: Increase ``ibuf`` to reduce underruns in heavy processing scenarios
- **Processing time**: In blocking mode, process data quickly to avoid blocking the microphone

Troubleshooting
---------------

PDM Device Not Found
~~~~~~~~~~~~~~~~~~~~

**Error**: ``ValueError: PDM device not found``

**Solutions**:

- Verify device name (try ``"pdm0"`` or ``0``)
- Check device tree has ``&pdm0 { status = "okay"; };``
- Confirm ``CONFIG_MICROPY_PY_MACHINE_PDM=y`` in board config
- Ensure ``CONFIG_AUDIO_DMIC=y`` is enabled (automatic with PDM config)

PDM Device Not Ready
~~~~~~~~~~~~~~~~~~~~

**Error**: ``RuntimeError: PDM device not ready``

**Solutions**:

- Check pin configuration in device tree overlay
- Verify clock source is configured (``clock-source = "PCLK32M";``)
- Ensure microphone power regulator is enabled (if required)

No Audio or All Zeros
~~~~~~~~~~~~~~~~~~~~~~

**Symptoms**: Buffer contains all zeros or constant values

**Solutions**:

- Check microphone power supply (regulator-boot-on in device tree)
- Verify PDM clock and data pins match hardware
- Try different sample rates (8000, 16000, 32000)
- Wait 500ms after initialization before recording
- Test with loud sounds (clapping, talking loudly)

Buffer Underruns
~~~~~~~~~~~~~~~~

**Symptoms**: Gaps or clicks in audio

**Solutions**:

- Increase ``ibuf`` size (try 16384 or 32768)
- Process data faster in blocking mode
- Reduce sample rate if CPU is overloaded

Implementation Details
----------------------

The PDM class interfaces with Zephyr's DMIC subsystem, which handles:

- Hardware configuration and DMA setup
- Memory slab allocation for audio buffers
- PDM clock generation and data sampling

The MicroPython layer provides:

- Ring buffer for decoupling hardware from Python code
- Blocking mode operation
- Simple Python API following machine.I2S patterns

See Also
--------

- :ref:`machine.I2S` - Similar API for I2S audio
- `Zephyr DMIC Documentation <https://docs.zephyrproject.org/latest/hardware/peripherals/audio/dmic.html>`_

# PDM_PCM Audio Validation

This directory contains the tools to validate the `PDM_PCM` peripheral on the PSOC Edge E84 kit by recording audio on the target and playing it back on the host.

## Prerequisites

### Hardware
- PSOC Edge E84 Kit with MicroPython firmware flashed
- Microphone connected to pins `P8_5` (SCK) and `P8_6` (DATA)

> [!NOTE]  These pins are currently hardcoded for the available KIT_PSE84_AI board. You can modify the recording script `record-target.py` to support other pin configurations as needed. They can also be added to the configurable parameters in the CLI script in the future.

### Software (Linux / WSL)

**mpremote** — used to communicate with the MicroPython target:
```bash
pip install mpremote
```

**ffmpeg / ffplay** — used to play back the recorded audio:
```bash
sudo apt update
sudo apt install ffmpeg
```

## Usage

All commands are run from this directory:

```bash
cd tests/ports/psoc-edge/audio
```

### Full flow — record, load, and play in one step

```bash
python record-audio.py
```

With custom parameters:
```bash
python record-audio.py -r 16000 -b 16 -f 2 -m 0 -t 5 -o audio.raw -d a0
```

### Step by step

**1. Record** — uploads the config and runs the recording script on the target:
```bash
python record-audio.py record -r 16000 -b 16 -f 2 -m 0 -t 5
```

**2. Load** — copies the raw audio file from the target to the host:
```bash
python record-audio.py load
```

> [!TIP]  The file transfer over the serial interface can be unreliable and may fail without needing to re-record. If the full flow fails at the load step, you can retry just the transfer independently with `load`, then play back the result with `play`, without repeating the recording.

**3. Play** — plays back the audio on the host:
```bash
python record-audio.py play
```

**4. Clean** — removes audio files from both host and target:
```bash
python record-audio.py clean
```

### CLI reference

| Argument | Short | Description | Default | Choices |
|---|---|---|---|---|
| `--device` | `-d` | mpremote device name | `a0` | any |
| `--audio-file` | `-o` | Output file name | `audio.raw` | any |
| `--audio-script` | `-s` | Target recording script | `record-target.py` | any |
| `--time-sec` | `-t` | Recording duration (seconds) | `5` | any int |
| `--sample-rate` | `-r` | Sample rate in Hz | `16000` | `8000 16000 32000 44100 48000` |
| `--bits-per-sample` | `-b` | Bits per sample | `16` | `16 32` |
| `--format` | `-f` | Audio format | `2` | `1`=MONO, `2`=STEREO |
| `--mode` | `-m` | I/O mode | `0` | `0`=blocking, `1`=non-blocking |

---

## Validation matrix

Mark each combination as validated (`✅`), failed (`❌`), or not tested (leave blank).

| Sample Rate | Bits per Sample | Format | Mode | Validated |
|---|---|---|---|---|
| 8000 | 16 | MONO | blocking | |
| 8000 | 16 | MONO | non-blocking | |
| 8000 | 16 | STEREO | blocking | |
| 8000 | 16 | STEREO | non-blocking | |
| 8000 | 32 | MONO | blocking | |
| 8000 | 32 | MONO | non-blocking | |
| 8000 | 32 | STEREO | blocking | |
| 8000 | 32 | STEREO | non-blocking | |
| 16000 | 16 | MONO | blocking | |
| 16000 | 16 | MONO | non-blocking | |
| 16000 | 16 | STEREO | blocking | `✅` |
| 16000 | 16 | STEREO | non-blocking | `✅` |
| 16000 | 32 | MONO | blocking | |
| 16000 | 32 | MONO | non-blocking | |
| 16000 | 32 | STEREO | blocking | |
| 16000 | 32 | STEREO | non-blocking | |
| 32000 | 16 | MONO | blocking | |
| 32000 | 16 | MONO | non-blocking | |
| 32000 | 16 | STEREO | blocking | |
| 32000 | 16 | STEREO | non-blocking | |
| 32000 | 32 | MONO | blocking | |
| 32000 | 32 | MONO | non-blocking | |
| 32000 | 32 | STEREO | blocking | |
| 32000 | 32 | STEREO | non-blocking | |
| 44100 | 16 | MONO | blocking | |
| 44100 | 16 | MONO | non-blocking | |
| 44100 | 16 | STEREO | blocking | |
| 44100 | 16 | STEREO | non-blocking | |
| 44100 | 32 | MONO | blocking | |
| 44100 | 32 | MONO | non-blocking | |
| 44100 | 32 | STEREO | blocking | |
| 44100 | 32 | STEREO | non-blocking | |
| 48000 | 16 | MONO | blocking | |
| 48000 | 16 | MONO | non-blocking | |
| 48000 | 16 | STEREO | blocking | |
| 48000 | 16 | STEREO | non-blocking | |
| 48000 | 32 | MONO | blocking | |
| 48000 | 32 | MONO | non-blocking | |
| 48000 | 32 | STEREO | blocking | |
| 48000 | 32 | STEREO | non-blocking | |

Program your board using the **AmebaFlash** tool from the
[ameba-rtos SDK](https://github.com/Ameba-AIoT/ameba-rtos).

### Before flashing

Close any open serial terminal (SecureCRT, miniterm, mpremote) connected
to the board before running the flash command, otherwise the port will be
busy and flashing will fail.

### Enter download mode

The board attempts to enter download mode automatically via the DTR/RTS
signals of the USB-to-UART chip. If automatic entry fails, enter manually:

Hold the **Download KEY**, press and release the **Reset KEY**, then
release the **Download KEY**. The board is now in flash download mode.

### Install the flash tool

Clone the ameba-rtos SDK and install dependencies:

```bash
git clone https://github.com/Ameba-AIoT/ameba-rtos.git
```

**Linux / WSL:**
```bash
pip install pyserial
```

**Windows:**
```bash
pip install pyserial pyDes
```

The flash tool is at `ameba-rtos/tools/ameba/Flash/AmebaFlash.py`.
A pre-built Windows executable is at `ameba-rtos/tools/ameba/Flash/dist/AmebaFlash.exe`.

### Flash the firmware (Linux / WSL)

Download `firmware.bin` from this page, then flash it (replace `/dev/ttyUSB0`
with your serial port):

```bash
cd ameba-rtos/tools/ameba/Flash

python3 AmebaFlash.py \
    --download \
    --port /dev/ttyUSB0 \
    --baudrate 1500000 \
    --profile Devices/Profiles/RTL8721Dx.rdev \
    --image /path/to/firmware.bin \
    --start-address 0x8000000 \
    --memory-type nor
```

**WSL with usbipd:** attach the board to WSL first, then use the same
command with `/dev/ttyUSB0`. The USB connection remains stable during
flashing because the USB-to-UART chip is powered independently from the
target MCU.

### Flash the firmware (Windows)

```bash
cd ameba-rtos\tools\ameba\Flash

python AmebaFlash.py \
    --download \
    --port COM3 \
    --baudrate 1500000 \
    --profile Devices\Profiles\RTL8721Dx.rdev \
    --image \path\to\firmware.bin \
    --start-address 0x8000000 \
    --memory-type nor
```

Replace `COM3` with your actual port name (check Device Manager).

### Flash using the SDK build system

If you have the full ameba-rtos-m port checked out, you can build and
flash directly:

```bash
# From the port directory (after building)
make deploy PORT=/dev/ttyUSB0

# With a custom baud rate
make deploy PORT=/dev/ttyUSB0 BAUD=115200
```

### Troubleshooting

**Port busy:** Make sure no serial terminal is connected to the board.

**Download mode timeout:** The automatic DTR/RTS entry failed. Enter
download mode manually (hold Download KEY, press Reset, release both),
then re-run the flash command immediately.

**Flash error mid-way:** A transient communication error occurred. The
board will reboot automatically; wait a few seconds for the USB device to
reappear, then retry.

**Slow or unreliable:** Try a lower baud rate:

```bash
python3 AmebaFlash.py --download --port /dev/ttyUSB0 --baudrate 115200 ...
```

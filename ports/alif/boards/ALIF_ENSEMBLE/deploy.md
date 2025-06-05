### Alif Security Toolkit

This board can be programmed via the SE UART interface using the Alif Security Toolkit. MicroPython uses a custom version of the toolkit, which can be downloaded from [here](https://github.com/micropython/alif-security-toolkit/) or cloned:

```bash
git clone https://github.com/micropython/alif-security-toolkit/
```

---

### Update the SE Firmware (Optional)

If needed, update the SE firmware to match the version used by MicroPython. Ensure you have the correct port, part number, and chip revision.

```bash
cd alif-security-toolkit/toolkit
python updateSystemPackage.py --port /dev/ttyACM0 --cfg-part AE722F80F55D5XX --cfg-rev B4
```

**Note:** The board must be power-cycled after this step.

---

### Deploy MicroPython

Download (or build) the firmware package, then deploy it:

```bash
python alif-security-toolkit/toolkit/app-write-mram.py --port /dev/ttyACM0 --cfg-part AE722F80F55D5XX --cfg-rev B4 --pad --images file:build-ALIF_ENSEMBLE/application_package.ds
```

---

### Build MicroPython from Source

Alternatively, you can build the firmware from source. The MicroPython repository includes all necessary tools.

#### Steps:

1. Clone the MicroPython repository:
   ```bash
   git clone https://github.com/micropython/micropython.git
   ```

2. Install the Arm GNU toolchain (version 13.3.Rel1).

3. Add the toolchain to your system `PATH`.

4. Connect the SE USB cable.

5. Build the firmware:
   ```bash
   cd micropython
   make -j$(nproc) -C mpy-cross
   cd ports/alif
   make -j$(nproc) BOARD=ALIF_ENSEMBLE submodules
   make -j$(nproc) BOARD=ALIF_ENSEMBLE
   ```

6. Deploy the firmware:
   ```bash
   make -j$(nproc) BOARD=ALIF_ENSEMBLE deploy
   ```

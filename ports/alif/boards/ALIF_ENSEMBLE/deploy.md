### Alif Security Toolkit

This board can be programmed via the SE UART interface using the Alif Security
Toolkit. MicroPython uses a custom version of the toolkit, which can be downloaded
from [here](https://github.com/micropython/alif-security-toolkit/) or cloned:

```bash
git clone https://github.com/micropython/alif-security-toolkit/
```

---

### Update the SE Firmware (Optional)

If needed, update the SE firmware to match the version used by MicroPython. Ensure
you have the correct port, part number, and chip revision.

```bash
python alif-security-toolkit/toolkit/updateSystemPackage.py --port /dev/ttyACM0 --cfg-part AE722F80F55D5LS --cfg-rev B4
```

**Note:** The board must be power-cycled after this step.

---

### Deploy MicroPython

Download (or build) the firmware package, unzip it, then deploy it:

```bash

python alif-security-toolkit/toolkit/app-write-mram.py --port /dev/ttyACM0 --cfg-part AE722F80F55D5LS --cfg-rev B4 --pad --images file:build-ALIF_ENSEMBLE/application_package.ds
```

The MicroPython REPL is available on the second USB serial port, eg `/dev/ttyACM1`.

---

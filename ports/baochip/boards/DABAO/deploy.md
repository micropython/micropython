## Flashing

The Dabao's USB-C connects directly to the Baochip-1x's on-chip USB
controller; boot1 (in RRAM) provides a USB-CDC + MSC interface for
firmware updates.

### From a running MicroPython REPL

Call `machine.bootloader()` to enter boot1, then run `make deploy`:

```bash
DABAO_PORT=/dev/ttyACM0 make BOARD=DABAO deploy
```

Or drag `build-DABAO/firmware.uf2` onto the MSC volume that appears.

### From a stopped or unknown state

Hold the **PROG** button, tap **RESET**, hold PROG for ~1 second, then
release.  The chip enters boot1 regardless of the current firmware
state.  Then run `make deploy` as above, or drag the `.uf2` onto the
USB drive.

`make deploy` uses the SDK's `serial_flash.py --persistent` by default,
which flashes over the CDC channel and sets bootwait to disabled so the
chip auto-boots user firmware on every subsequent reset without pausing
in boot1.

To re-enter boot1 for the next flash cycle, use either path above:
`machine.bootloader()` from the REPL, or PROG+RESET.  Pass `PERSIST=0`
to `make deploy` to leave bootwait enabled instead (the chip will enter
boot1 on every reset until disabled again):

```bash
DABAO_PORT=/dev/ttyACM0 PERSIST=0 make BOARD=DABAO deploy
```

# Examples

These examples, originally from [e-radionica/Inkplate/Examples/Inkplate6](https://github.com/e-radionicacom/Inkplate-micropython/tree/master/Examples/Inkplate6) are _not_ frozen into the board firmware; they're here for reference. Execute them on your Inkplate to try them out.

- basicBW.py -> demonstrates basic drawing capabilities, as well as drawing some images.
- basicGrayscale.py -> demonstrates basic drawing capabilities, as well as drawing some images.
- exampleNetwork.py -> demonstrates connection to WiFi network while drawing the HTTP request response on the screen.
- exampleSd.py -> demonstrates reading files and images from SD card.
- batteryAndTemperatureRead.py -> demonstrates how to read temperature and voltage from internal sensors.
- touchpads.py -> demonstrates how to use built in touchpads.

## To execute the examples
The recommended way to execute the examples is with `mpremote`. Install `mpremote` with `pipx` (`pipx install mpremote`) then run a script on the Inkplate, eg:

```bash
> mpremote connect [SERIALPORT] run basicBW.py
```
## 1. Deploy the MicroPython firmware to the iMX RT1011 Nano Kit.

The iMX RT1011 Nano Kit comes pre-installed with a UF2 bootloader.
It can be started by pushing reset twice. Then the bootloader drive will
appear. If that does not happen or the bootloader was lost, you can
reinstall the bootloader using the procedure below.

Once the bootloader is installed and started, you can install MicroPython
by copying the .uf2 version of the firmware file to the bootloader
drive. When the firmware is installed, the drive will disappear.

## 2. Reinstall the bootloader iMX RT1011 Nano Kit.

1. Get the NXP program sdphost for your operating system, e.g. from
https://github.com/adafruit/tinyuf2/tree/master/ports/mimxrt10xx/sdphost.
You can also get them from the NXP web sites.

2. Get the UF2 boot-loader package https://github.com/adafruit/tinyuf2/releases/tag/0.21.0
and extract the file  tinyuf2-makerdiary_rt1011-0.21.0.bin. You may as
well go for a newer release.

Now you have all files at hand that you will need for updating.

1. Push and hold the "USR/BT" button, then press "RST", and release both buttons.

2. Run the commands:

```
sudo ./sdphost -u 0x1fc9,0x0145 -- write-file 0x20206400 tinyuf2-makerdiary_rt1011-0.21.0.bin
sudo ./sdphost -u 0x1fc9,0x0145 -- jump-address 0x20207000
```
Wait until a drive icon appears on the computer (or mount it explicitly).
At this point the bootloader is installed. You can now copy the
MicroPython .uf2 file to the board.

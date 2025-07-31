The Olimex RT1011 board is delivered without firmware. The best option to
install MicroPython is installing a UF2 bootstrap loader first, which then can be
used to install and update MicroPython. The bootloader has to be installed
only once.

For initial deployment of the bootloader a few preparation steps are required, which
have to be done once.

1. Get the NXP program sdphost for your operating system, e.g. from
https://github.com/adafruit/tinyuf2/tree/master/ports/mimxrt10xx/sdphost.
You can also get them from the NXP web sites.

2. Get the UF2 boot-loader package https://github.com/adafruit/tinyuf2/releases/tag/0.21.0/tinyuf2-imxrt1010_evk-0.20.1.zip and extract the files `tinyuf2-imxrt1010_evk-0.21.0.bin`
. You may as well go for a newer release.

Now you have all files at hand that you will need for updating.

1. Get the firmware file you want to upload  with the .uf2 extension from the MicroPython download page.

2. Push and hold the "Boot" button, then press "Reset", and release both buttons.

3. Run the commands:

```
sudo ./sdphost -u 0x1fc9,0x0145 -- write-file 0x20206400 tinyuf2-imxrt1010_evk-0.21.0.bin
sudo ./sdphost -u 0x1fc9,0x0145 -- jump-address 0x20207000
```
Wait until a drive icon appears on the computer (or mount it explicitly). Then the UF2 bootloader
is permanently installed.

Using sudo is Linux specific. You may not need it at all, if the access rights are set properly,
and you will not need it for Windows.

4. Once the upload of the bootloader is finished, push Reset twice.

The bootloader should start and show a drive icon.

5. Copy the .uf2 version of MicroPython to this drive to install or update MicroPython.

A LED on the board will start flickering, indicating that the upload is ongoing. Once the upload
is complete, the drive icon will disappear. Wait until the LED stops flickering. In rare cases there
may be an error message coming up, especially when there are only few or no changes in
the firmware file. Then just repeat the copy.

Once the UF2 bootloader is installed, only steps 4 and 5 are required to deploy MicroPython. If
MicroPython is already installed, the bootloader can as well be invoked by calling
`machine.bootloader()`.

If at any time the flash content is corrupted you can always start over from the beginning.

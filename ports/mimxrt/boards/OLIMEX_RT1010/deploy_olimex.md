For initial deployment of the firmware a few preparation steps are required, which
have to be done once.

1. Get the files ufconv.py and uf2families.json from the micropython/tools directory,
e.g. at https://github.com/micropython/micropython/tree/master/tools.

2. Get the NXP program sdphost for your operating system, e.g. from
https://github.com/adafruit/tinyuf2/tree/master/ports/mimxrt10xx/sdphost.
You can also get them from the NXP web sites.

3. Get the UF2 boot-loader package https://github.com/adafruit/tinyuf2/releases/download/0.9.0/tinyuf2-imxrt1010_evk-0.9.0.zip
and extract the file tinyuf2-imxrt1010_evk-0.9.0.bin.

Now you have all files at hand that you will need for updating.

1. Get the firmware you want to upload from the MicroPython download page.

2. Push and hold the "Boot" button, then press "Reset", and release both buttons.

3. Run the commands:

```
sudo ./sdphost -u 0x1fc9,0x0145 -- write-file 0x20206400 tinyuf2-imxrt1010_evk-0.9.0.bin
sudo ./sdphost -u 0x1fc9,0x0145 -- jump-address 0x20207000
```
Wait until a drive icon appears on the computer (or mount it explicitly), and then run:
```
python3 uf2conv.py <firmware_xx.yy.zz.hex> --base 0x60000400 -f 0x4fb2d5bd
```
You can put all of that in a script. Just add a short wait before the 3rd command to let the drive connect.

4. Once the upload is finished, push Reset again.

Using sudo is Linux specific. You may not need it at all, if the access rights are set properly,
and you will not need it for Windows.

Once the generic boot-loader is available, this procedure is only required for the first
firmware load or in case the flash is corrupted and the existing firmware is not functioning
any more.

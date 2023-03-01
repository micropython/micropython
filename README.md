Building the Firmware
=======================
Prerequisites
```
sudo apt update
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential
sudo pip3 install rshell
sudo usermod -a -G dialout $USER
make -C mpy-cross
cd ports/rp2
make BOARD=PICO_W submodules
```
Build Firmware
```
cd ports/rp2
make BOARD=PICO_W
ls build-PICO_W/*.uf2
```
Load Firwmare onto Pico W
- put pico into boot mode by holding down BOOTSEL while plugging it into your computer
```
cd ports/rp2
sudo mkdir /mnt/pico
sudo blkid -o list | grep RPI
sudo mount /dev/sda1 /mnt/pico
sudo cp build-PICO_W/firmware.uf2 /mnt/pico
sudo sync
```
- power cycle the board

Connect to Python Session
=======================
```
screen /dev/ttyACM0
```
or
```
sudo rshell
repl
```

Loading the Project Code
=======================
Prerequisites
- Pico is loaded with micropython firmware and plugged into USB

Loading the Project Code
```
sudo rshell
rm /pyboard/*            # "Factory Reset" (Does not reset firmware)
cp subway/*.py /pyboard/ # Don't overwrite config
cp subway/*    /pyboard/ # Overwrite config
repl
# CTRL+D() to restart board. Runs main.py on startup
```

First Boot
=======================
- connect to "Live Subway Map" Wifi Network
- navigate to http://192.168.4.1/
- provide SSID and Password of your LAN
- connect back to your LAN
- device should have gotten assigned a DHCP address on your LAN
    - for now, it just says Hello
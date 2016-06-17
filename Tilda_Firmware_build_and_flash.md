# Tilda

## Dependencies
### ```git```, ```make```, etc
I will just assume you have this

### ```arm-none-eabi-gcc```
https://launchpad.net/gcc-arm-embedded/+download
You will have to make the ```arm-none-eabi-gcc``` from the bin directory available to your system (add to PATH, symlink or copy)

### ```pyusb```
```sudo pip install pyusb```

## How to flash your tilda

```bash
# Create a working directory somewhere
mkdir tilda
cd tilda

# Clone this repo
git clone git@github.com:emfcamp/micropython.git
git checkout board_v3_correct_cmsis_cc3100_ugfx

# Clone the ugfx repo
git clone git@bitbucket.org:Tectu/ugfx.git

# There's a bug in ugfx, comment line 368 out
# ToDo: Maybe fix this upstream? Missing #if GINPUT_NEEDS_MOUSE ?
vim ugfx/src/gwin/gwin_widget.c

# Move into the stmha directory
cd micropython/stmha

# Now we can build the firmware and flash it to the badge
# You have to boot the badge into dfu mode by pressing down the center
# joystick button while pressing the reset button to trigger a reboot
make BOARD=STM32L475_EMFBADGE --deploy
```


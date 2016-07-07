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

# Switch to our work branch
cd micropython
git checkout tilda-master

# Now we can build the firmware and flash it to the badge
# You have to boot the badge into dfu mode by pressing down the center
# joystick button while pressing the reset button to trigger a reboot
make -C stmhal BOARD=STM32L475_EMFBADGE --deploy
```


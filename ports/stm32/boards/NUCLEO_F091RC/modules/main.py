# frozen main.py
#
# By having a standard location like `boards/<board>/modules` the `boot`, `main` and other python scripts can be
# conveniently stored.
# Setting a different path for the `modules` allow easy recompilation of only the relevant files and allows for
# quick compilation of a new `firmware.*` to upload to your device
#
# In this case: `make BOARD=NUCLEO_F091RC FROZEN_MPY_DIR=boards/NUCLEO_F091RC/modules`

print("main: running from flash")


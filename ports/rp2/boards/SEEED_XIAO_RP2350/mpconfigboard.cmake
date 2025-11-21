# cmake file for Seeed XIAO RP2350
set(PICO_BOARD "seeed_xiao_rp2350")

# To change the gpio count for QFN-80
# set(PICO_NUM_GPIOS 48)

# Seeed XIAO RP2350 uses RP2350A with limited memory, reduce stack size 
set(PICO_STACK_SIZE 0x1000)

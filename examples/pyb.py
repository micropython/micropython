# pyboard testing functions for CPython
import time

def delay(n):
    #time.sleep(float(n) / 1000)
    pass

rand_seed = 1
def rand():
    global rand_seed
    # for these choice of numbers, see P L'Ecuyer, "Tables of linear congruential generators of different sizes and good lattice structure"
    rand_seed = (rand_seed * 653276) % 8388593
    return rand_seed

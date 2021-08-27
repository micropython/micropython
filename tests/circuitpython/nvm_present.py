import skip_if

# TODO(tannewt): Remove this when we add nvm support to 3.x
skip_if.always()
skip_if.board_not_in("metro_m0_express", "feather_m0_express", "circuitplayground_express")

import microcontroller
import random

nvm = microcontroller.nvm

len(nvm)

single = random.randint(0, 255)
nvm[1] = single
assert nvm[1] == single

nvm[0] = single
assert nvm[0] == single

b = bytearray()
for i in range(10):
    b.append(random.randint(0, 255))

microcontroller.nvm[10:20] = b
assert microcontroller.nvm[10:20] == b

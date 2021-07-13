import skip_if

skip_if.board_not_in("gemma_m0", "trinket_m0")

import microcontroller

assert microcontroller.nvm == None

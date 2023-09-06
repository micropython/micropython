# Minimal _boot.py for the 512kiB variant. Does not set up a block device or
# filesystem. Other variants use esp8266/modules/_boot.py.

import gc

gc.threshold((gc.mem_free() + gc.mem_alloc()) // 4)

import gc

gc.threshold((gc.mem_free() + gc.mem_alloc()) // 4)

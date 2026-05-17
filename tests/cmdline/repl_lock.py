import micropython
micropython.heap_lock()
1+1
micropython.heap_lock()
None # Cause the repl's line storage to be enlarged ----------------
micropython.heap_lock()
raise SystemExit

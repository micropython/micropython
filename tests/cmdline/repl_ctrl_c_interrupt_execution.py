# Test Ctrl-C interrupts executing code
# The test framework sends lines sequentially without waiting for completion,
# so Ctrl-C is sent while sleep(30) is still running, triggering SIGINT.
import time
time.sleep(30)
{\x03}
print('repl still responds')

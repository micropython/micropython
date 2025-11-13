# Test Ctrl-C interrupts executing code
import time
time.sleep(30)
{\x03}
print('interrupted')

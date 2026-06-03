# sigint: deliver via controlling terminal
# Test that Ctrl-C (SIGINT) interrupts blocking code execution.
# MicroPython restores original terminal mode (ISIG on) during
# execution, so the PTY terminal driver generates SIGINT from \x03.
import time
time.sleep(10)
{\x03}
print('repl still responds')

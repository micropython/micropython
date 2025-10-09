# Test that Ctrl-C interrupts running code

# Note: This test cannot actually test Ctrl-C during execution because
# the test framework sends complete lines then waits for output.
# For a working test, see tests/ports/unix/test_ctrl_c_interrupt.py

# Test paste mode Ctrl-C (which does work in the test framework)
{\x05}
print('paste mode')
{\x03}

# Verify REPL works
print('REPL works')

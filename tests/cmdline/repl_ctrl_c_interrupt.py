# Test that Ctrl-C interrupts running code

# Test paste mode Ctrl-C cancel
{\x05}
print('paste mode')
{\x03}

# Verify REPL works
print('REPL works')

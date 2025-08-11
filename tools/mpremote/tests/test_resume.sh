#!/bin/bash
set -e

# The eval command will continue the state of the exec (default resume behavior).
echo -----
$MPREMOTE exec "a = 'hello'" eval "a"

# With new default behavior, `a` will persist across sessions.
echo -----
$MPREMOTE eval "a"

# Variables persist by default (no resume command needed).
echo -----
$MPREMOTE exec "a = 'persists'"
$MPREMOTE eval "a"

# Explicit soft-reset clears variables.
echo -----
$MPREMOTE exec "a = 'soft-reset'" eval "a" soft-reset eval "1+1" eval "a" || true

# A disconnect will no longer reset state.
echo -----
$MPREMOTE eval "1+2" disconnect eval "2+3"

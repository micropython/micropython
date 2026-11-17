#!/bin/bash
set -e

# Interpreter state is carried from one command to the next.
echo -----
$MPREMOTE soft-reset exec "a = 'hello'" eval "a"

# It is also carried between separate invocations of mpremote.
echo -----
$MPREMOTE eval "a"

echo -----
$MPREMOTE exec "a = 'persists'"
$MPREMOTE eval "a"

# The "resume" command is accepted for backwards compatibility and does nothing.
echo -----
$MPREMOTE resume eval "a"

# An explicit soft-reset clears the interpreter state.
echo -----
$MPREMOTE exec "a = 'soft-reset'" eval "a" soft-reset eval "1+1" eval "'a' in globals()" || true

# A disconnect does not clear the interpreter state.
echo -----
$MPREMOTE eval "1+2" disconnect eval "2+3"

#!/bin/bash
set -e

# The eval command will continue the state of the exec.
echo -----
$MPREMOTE exec "a = 'hello'" eval "a"

# Automatic soft reset. `a` will trigger NameError.
echo -----
$MPREMOTE eval "a" || true

# Resume will skip soft reset.
echo -----
$MPREMOTE exec "a = 'resume'"
$MPREMOTE resume eval "a"

# The eval command will continue the state of the exec.
echo -----
$MPREMOTE exec "a = 'soft-reset'" eval "a" soft-reset eval "1+1" eval "a" || true

# A disconnect will trigger auto-reconnect.
echo -----
$MPREMOTE eval "1+2" disconnect eval "2+3"

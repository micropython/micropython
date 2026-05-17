#!/bin/bash
set -e

$MPREMOTE exec "print('mpremote')"

$MPREMOTE exec "print('before sleep'); import time; time.sleep(0.1); print('after sleep')"
$MPREMOTE exec --no-follow "print('before sleep'); import time; time.sleep(0.1); print('after sleep')"
sleep 0.3

$MPREMOTE eval "1+2"
$MPREMOTE eval "[{'a': 'b'}, (1,2,3,), True]"

cat << EOF > /tmp/run.py
print("run")
EOF

$MPREMOTE run /tmp/run.py

cat << EOF > /tmp/run.py
import time
for i in range(3):
    time.sleep(0.1)
    print("run")
EOF
$MPREMOTE run --no-follow /tmp/run.py
sleep 0.5

#!/bin/bash
set -e

# Special ErrorFS so the test can induce arbitrary filesystem errors.
cat << EOF > "${TMP}/fs.py"
import os, vfs, errno

class ErrorFS:
    def mount(self, *a, **k):
        pass
    def umount(self, *a, **k):
        pass
    def chdir(self, *a, **k):
        pass
    def open(self, *a, **k):
        raise self.error

fs = ErrorFS()
vfs.mount(fs, '/fs')
os.chdir('/fs')
EOF

$MPREMOTE run "${TMP}/fs.py"

echo -----
$MPREMOTE resume exec "fs.error = Exception()"
(
  $MPREMOTE resume cat :Exception.py || echo "expect error"
) 2> >(head -n1 >&2) # discard traceback specifics but keep main error message

for errno in ENOENT EISDIR EEXIST ENODEV EINVAL EPERM EOPNOTSUPP ; do
echo -----
$MPREMOTE resume exec "fs.error = OSError(errno.$errno, '')"
$MPREMOTE resume cat :$errno.py || echo "expect error"
done

echo -----
$MPREMOTE resume exec "vfs.umount('/fs')"

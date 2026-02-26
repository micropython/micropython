#!/bin/bash
set -e

# Test Unicode and special character handling in mpremote.
# These tests cover:
# - Properly escaping quotes for commands (transport.py)
# - Command expansion handling `=` (main.py)
# - UTF-8 display and encoding fixes (console.py, transport.py)
# - wr_bytes string to bytes conversion (transport_serial.py)

# Create a RAM disk for testing
# Get the test directory (where this script and ramdisk.py are located)
TEST_DIR=$(dirname $0)

# Initialize ramdisk for file tests
$MPREMOTE run "${TEST_DIR}/ramdisk.py"

echo -----
# Test UTF-8 output via exec (tests stdout_write_bytes UTF-8 handling)
$MPREMOTE resume exec "print('Hello 世界')"
$MPREMOTE resume exec "print('Emoji: 🎉🐍')"
$MPREMOTE resume exec "print('Accents: café, naïve, résumé')"

echo -----
# Test Files with single quotes in names (tests _quote_path in transport.py)
$MPREMOTE resume touch ":quote'test.txt"
$MPREMOTE resume ls :
$MPREMOTE resume rm ":quote'test.txt"
$MPREMOTE resume ls :

echo -----
# Test Writing and reading UTF-8 content (tests transport and wr_bytes)
cat << EOF > "${TMP}/unicode_content.txt"
Hello 世界
Emoji: 🎉🐍
Café résumé naïve
EOF
$MPREMOTE resume cp "${TMP}/unicode_content.txt" :
$MPREMOTE resume cat :unicode_content.txt
$MPREMOTE resume sha256sum :unicode_content.txt
cat "${TMP}/unicode_content.txt" | sha256sum

echo -----
# This test writes UTF-8 content via mount and reads it back
# https://github.com/micropython/micropython/issues/13055
cat << EOF > "${TMP}/write_utf_to_mounted.py"
with open("foo.txt", 'w', encoding='utf-8') as file:
    file.write("🔢 Data" + '\n')
    file.write("🔢 Data" + '\n')

with open("foo.txt", 'r', encoding='utf-8') as file:
    data = file.read()
    print(data)
EOF
$MPREMOTE mount ${TMP} run "${TMP}/write_utf_to_mounted.py"


echo -----
# Test Command expansion with `=` sign (tests main.py fix)
# The `=` in arguments should not cause errors
$MPREMOTE resume exec "x = 'a=b'; print(x)"

echo -----
# Test eval with Unicode strings  
$MPREMOTE resume eval "'Hello 世界'"
$MPREMOTE resume eval "repr('日本語')"

echo -----
# Test UTF-8 display on Windows
# Testing mpremote on windows with bash is not straightforward,
# but we can at least run the command that was problematic.
$MPREMOTE resume exec "print('Hello World'); print('你好'); print('=' * 20)"

echo -----

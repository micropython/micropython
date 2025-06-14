#!/bin/bash

# Comprehensive Bluetooth Multi-Test Runner
# This script runs all Bluetooth multi-tests and generates a detailed report

set +e  # Don't exit on errors - continue running all tests

# Configuration
REPORT_FILE="BLUETOOTH_MULTITEST_RESULTS.md"
LOG_DIR="test_logs"
DEVICE1="/dev/ttyACM1"
DEVICE2="/dev/ttyACM0"
TEST_RUNNER="./tests/run-multitests.py"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Create log directory
mkdir -p "$LOG_DIR"

# Initialize report
cat > "$REPORT_FILE" << EOF
# Bluetooth Multi-Test Results Report

**Date:** $(date)  
**Firmware:** BTstack with bond persistence fixes  
**Device:** Raspberry Pi Pico2 W  
**Test Environment:** Two devices on $DEVICE2 and $DEVICE1  

## Summary

This report documents the results of running all Bluetooth multi-tests after implementing the critical bond persistence fix in BTstack. The fix addressed the root cause where \`mp_bluetooth_gap_on_set_secret()\` was failing when no IRQ handler was registered, blocking TLV storage operations.

---

## Test Results

EOF

# Test counters
total_tests=0
passed_tests=0
failed_tests=0

# Function to run a single test
run_test() {
    local test_file="$1"
    local test_name="$2"
    local category="$3"
    
    echo -e "${BLUE}Running: $test_name${NC}"
    
    # Create log file
    local log_file="$LOG_DIR/$(basename "$test_file" .py).log"
    
    # Add test header to report
    cat >> "$REPORT_FILE" << EOF

#### $test_name

\`\`\`bash
$TEST_RUNNER -i pyb:$DEVICE1 -i pyb:$DEVICE2 -t $test_file
\`\`\`

EOF
    
    # Run the test and capture result
    local exit_code=0
    if timeout 60 $TEST_RUNNER -i pyb:$DEVICE1 -i pyb:$DEVICE2 -t "$test_file" > "$log_file" 2>&1; then
        echo -e "${GREEN}✅ PASSED${NC}: $test_name"
        echo "**Result:** ✅ **PASSED**" >> "$REPORT_FILE"
        ((passed_tests++))
    else
        exit_code=$?
        echo -e "${RED}❌ FAILED${NC}: $test_name (exit code: $exit_code)"
        echo "**Result:** ❌ **FAILED** (exit code: $exit_code)" >> "$REPORT_FILE"
        ((failed_tests++))
    fi
    
    # Add test output to report (truncated if too long)
    echo "" >> "$REPORT_FILE"
    echo '<details>' >> "$REPORT_FILE"
    echo '<summary>Show detailed output</summary>' >> "$REPORT_FILE"
    echo "" >> "$REPORT_FILE"
    echo '```' >> "$REPORT_FILE"
    
    # Truncate very long outputs
    if [[ $(wc -l < "$log_file") -gt 100 ]]; then
        echo "# Output truncated - showing first 50 and last 50 lines" >> "$REPORT_FILE"
        head -50 "$log_file" >> "$REPORT_FILE"
        echo "..." >> "$REPORT_FILE"
        echo "# ... (output truncated) ..." >> "$REPORT_FILE"
        echo "..." >> "$REPORT_FILE"
        tail -50 "$log_file" >> "$REPORT_FILE"
    else
        cat "$log_file" >> "$REPORT_FILE"
    fi
    
    echo '```' >> "$REPORT_FILE"
    echo '</details>' >> "$REPORT_FILE"
    echo "" >> "$REPORT_FILE"
    
    ((total_tests++))
}

# Function to add category header
add_category() {
    local category="$1"
    local description="$2"
    
    echo -e "\n${YELLOW}=== $category ===${NC}"
    cat >> "$REPORT_FILE" << EOF

### $category

$description

EOF
}

echo -e "${BLUE}Starting Bluetooth Multi-Test Suite${NC}"
echo "Report will be saved to: $REPORT_FILE"
echo "Detailed logs will be saved to: $LOG_DIR/"

# Phase 1: Critical Bond Persistence Tests
add_category "🔥 Phase 1: Critical Bond Persistence Tests" \
    "Tests that verify the core bond persistence functionality after our fixes."

# Local MicroPython bond tests
run_test "./tests/multi_bluetooth/ble_gap_pair_bond.py" \
    "Test 1: ble_gap_pair_bond.py" \
    "Basic pairing with bonding"

run_test "./tests/multi_bluetooth/ble_gap_pair_bond_reconnect.py" \
    "Test 2: ble_gap_pair_bond_reconnect.py" \
    "Bond persistence across BLE restart"

run_test "./tests/multi_bluetooth/ble_gap_pair_bond_persist.py" \
    "Test 3: ble_gap_pair_bond_persist.py" \
    "Bond persistence across device reboot"

# aioble bond tests
run_test "~/micropython/lib/micropython-lib/micropython/bluetooth/aioble/multitests/ble_bond_persist.py" \
    "Test 4: aioble ble_bond_persist.py" \
    "aioble bond persistence across restart"

run_test "~/micropython/lib/micropython-lib/micropython/bluetooth/aioble/multitests/ble_bond_reboot.py" \
    "Test 5: aioble ble_bond_reboot.py" \
    "aioble bond persistence across reboot"

# Phase 2: Basic Functionality Tests
add_category "🧪 Phase 2: Basic Functionality Tests" \
    "Tests to ensure basic Bluetooth functionality still works after our changes."

run_test "./tests/multi_bluetooth/ble_gap_pair.py" \
    "Test 6: ble_gap_pair.py" \
    "Basic pairing without bonding"

run_test "./tests/multi_bluetooth/ble_gap_connect.py" \
    "Test 7: ble_gap_connect.py" \
    "Basic connection establishment"

run_test "./tests/multi_bluetooth/ble_characteristic.py" \
    "Test 8: ble_characteristic.py" \
    "Basic characteristic operations"

run_test "~/micropython/lib/micropython-lib/micropython/bluetooth/aioble/multitests/ble_pair.py" \
    "Test 9: aioble ble_pair.py" \
    "Basic aioble pairing"

# Phase 3: Advanced Features (sample)
add_category "🚀 Phase 3: Advanced Features (Sample)" \
    "Sample of advanced Bluetooth features to check for regressions."

run_test "./tests/multi_bluetooth/ble_gattc_discover_services.py" \
    "Test 10: ble_gattc_discover_services.py" \
    "GATT service discovery"

run_test "./tests/multi_bluetooth/ble_subscribe.py" \
    "Test 11: ble_subscribe.py" \
    "Notifications and indications"

run_test "~/micropython/lib/micropython-lib/micropython/bluetooth/aioble/multitests/ble_notify.py" \
    "Test 12: aioble ble_notify.py" \
    "aioble notifications"

# Generate final summary
cat >> "$REPORT_FILE" << EOF

---

## Final Summary

**Total Tests Run:** $total_tests  
**Passed:** $passed_tests  
**Failed:** $failed_tests  
**Success Rate:** $(( passed_tests * 100 / total_tests ))%

### Test Categories Breakdown

#### 🔥 Critical Bond Persistence Tests (5 tests)
These tests verify that our BTstack bond persistence fix is working correctly.

#### 🧪 Basic Functionality Tests (4 tests)  
These tests ensure our changes don't break basic Bluetooth operations.

#### 🚀 Advanced Features Sample (3 tests)
Sample of advanced features to check for regressions.

### All Available Tests

For reference, here are all the Bluetooth multi-tests available:

#### Local MicroPython Tests (\`tests/multi_bluetooth/\`)
EOF

# List all local tests
find tests/multi_bluetooth/ -name "*.py" | sort | while read test; do
    echo "- \`$(basename "$test")\`" >> "$REPORT_FILE"
done

cat >> "$REPORT_FILE" << EOF

#### aioble Tests (\`lib/micropython-lib/.../multitests/\`)
EOF

# List all aioble tests
find ~/micropython/lib/micropython-lib/micropython/bluetooth/aioble/multitests/ -name "*.py" 2>/dev/null | sort | while read test; do
    echo "- \`$(basename "$test")\`" >> "$REPORT_FILE"
done

cat >> "$REPORT_FILE" << EOF

### How to Run Individual Tests

To run any specific test:

\`\`\`bash
# Local MicroPython tests
./tests/run-multitests.py -i pyb:$DEVICE1 -i pyb:$DEVICE2 -t ./tests/multi_bluetooth/TEST_NAME.py

# aioble tests  
./tests/run-multitests.py -i pyb:$DEVICE1 -i pyb:$DEVICE2 -t ~/micropython/lib/micropython-lib/micropython/bluetooth/aioble/multitests/TEST_NAME.py
\`\`\`

### How to Run All Tests

\`\`\`bash
# Run this comprehensive test suite
./run_all_bluetooth_tests.sh
\`\`\`

---

**Generated on:** $(date)  
**Test Duration:** Started at script launch time  
**Environment:** MicroPython BTstack with bond persistence fixes
EOF

# Print final summary
echo -e "\n${BLUE}=== TEST SUMMARY ===${NC}"
echo -e "Total Tests: $total_tests"
echo -e "${GREEN}Passed: $passed_tests${NC}"
echo -e "${RED}Failed: $failed_tests${NC}"
echo -e "Success Rate: $(( passed_tests * 100 / total_tests ))%"
echo -e "\nDetailed report saved to: $REPORT_FILE"
echo -e "Individual test logs saved to: $LOG_DIR/"

# Exit with error if any tests failed
if [[ $failed_tests -gt 0 ]]; then
    echo -e "\n${RED}Some tests failed. Check the report for details.${NC}"
    exit 1
else
    echo -e "\n${GREEN}All tests passed!${NC}"
    exit 0
fi
EOF
#!/bin/bash

# Quick Bond Persistence Test Runner
# Runs only the critical bond persistence tests

set +e  # Don't exit on errors - continue running all tests

# Configuration
DEVICE1="/dev/ttyACM1"
DEVICE2="/dev/ttyACM0"
TEST_RUNNER="./tests/run-multitests.py"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Test counters
total_tests=0
passed_tests=0
failed_tests=0

echo -e "${BLUE}Running Critical Bond Persistence Tests${NC}"
echo "========================================"

run_test() {
    local test_file="$1"
    local test_name="$2"
    
    echo -e "\n${YELLOW}Running: $test_name${NC}"
    echo "Command: $TEST_RUNNER -i pyb:$DEVICE1 -i pyb:$DEVICE2 -t $test_file"
    
    ((total_tests++))
    
    if timeout 60 $TEST_RUNNER -i pyb:$DEVICE1 -i pyb:$DEVICE2 -t "$test_file"; then
        echo -e "${GREEN}✅ PASSED${NC}: $test_name"
        ((passed_tests++))
    else
        echo -e "${RED}❌ FAILED${NC}: $test_name"
        ((failed_tests++))
    fi
}

# Critical bond persistence tests
echo -e "\n${BLUE}Phase 1: Local MicroPython Bond Tests${NC}"
run_test "./tests/multi_bluetooth/ble_gap_pair_bond.py" "Basic pairing with bonding"
run_test "./tests/multi_bluetooth/ble_gap_pair_bond_reconnect.py" "Bond persistence across BLE restart"
run_test "./tests/multi_bluetooth/ble_gap_pair_bond_persist.py" "Bond persistence across device reboot"

echo -e "\n${BLUE}Phase 2: aioble Bond Tests${NC}"
run_test "~/micropython/lib/micropython-lib/micropython/bluetooth/aioble/multitests/ble_bond_persist.py" "aioble bond persistence across restart"
run_test "~/micropython/lib/micropython-lib/micropython/bluetooth/aioble/multitests/ble_bond_reboot.py" "aioble bond persistence across reboot"

# Summary
echo -e "\n${BLUE}========================================"
echo -e "BOND PERSISTENCE TEST SUMMARY"
echo -e "========================================${NC}"
echo -e "Total Tests: $total_tests"
echo -e "${GREEN}Passed: $passed_tests${NC}"
echo -e "${RED}Failed: $failed_tests${NC}"
echo -e "Success Rate: $(( passed_tests * 100 / total_tests ))%"

if [[ $failed_tests -gt 0 ]]; then
    echo -e "\n${RED}❌ Some bond persistence tests failed${NC}"
    echo -e "This indicates the bond persistence fix may not be working correctly."
    exit 1
else
    echo -e "\n${GREEN}✅ All bond persistence tests passed!${NC}"
    echo -e "The bond persistence fix is working correctly."
    exit 0
fi
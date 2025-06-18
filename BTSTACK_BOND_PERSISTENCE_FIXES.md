# BTstack Bond Persistence Fixes Summary

## Overview
This document summarizes the fixes made to resolve BTstack bond persistence issues in MicroPython.

## Core Issue
BTstack's bond persistence was failing because `mp_bluetooth_gap_on_set_secret()` was returning false when no Python-level IRQ handler was registered. This prevented BTstack from storing bond data in its TLV (Tag-Length-Value) storage system.

## Key Fixes

### 1. Core BTstack Fix (extmod/modbluetooth.c)
- Modified `mp_bluetooth_gap_on_set_secret()` to return true when no IRQ handler is registered
- This allows BTstack to use its internal TLV storage for bond persistence
- Fix: `if (result == mp_const_none) { return true; }`

### 2. Test Suite Improvements

#### aioble Test Fix (lib/micropython-lib/micropython/bluetooth/aioble/multitests/ble_pair_bond_persist.py)
- Fixed connection API: Changed `aioble.connect()` to proper `device.connect()` pattern
- Added support for both NimBLE (error 261) and BTstack (error 5) error codes
- Ensures test works with both BLE stacks

#### Bond Lifecycle Test (tests/multi_bluetooth/ble_gap_pair_bond_lifecycle.py)
- Created comprehensive test for bond persistence across multiple restarts
- Fixed crashes by:
  - Creating BLE instances locally in each test instance
  - Re-registering services after each BLE restart (handles become invalid)
  - Proper initialization order: config, irq, then active
  - Moving MAC address retrieval after BLE activation
- Added file-based secret storage with JSON format

#### Test Consolidation
- Reduced from 4 redundant tests to 2 essential tests
- Removed duplicates while maintaining comprehensive coverage

## Test Results
- Simple pairing test: Working correctly
- aioble bond persistence test: Working with both NimBLE and BTstack
- Bond lifecycle test: Fixed crashes, now tests persistence across 4 restarts

## Technical Details

### BTstack TLV Storage
- BTstack uses TLV (Tag-Length-Value) format for persistent storage
- Stores ER/IR keys (Encryption Root/Identity Root) for BLE security
- Requires IRQ handler to be set BEFORE `ble.active(1)` for bond loading

### Error Code Differences
- NimBLE: Error 261 (0x105 = BLE_HS_ERR_ATT_BASE + ATT_ERROR_INSUFFICIENT_AUTHENTICATION)
- BTstack: Error 5 (0x05 = ATT_ERROR_INSUFFICIENT_AUTHENTICATION)

### Service Registration
- Services must be re-registered after each BLE restart
- Characteristic handles become invalid after `ble.active(0)`

## Commits Made
1. Fixed `mp_bluetooth_gap_on_set_secret()` to support BTstack bond persistence
2. Updated aioble test with proper connection API and error code handling
3. Created comprehensive bond lifecycle test with crash fixes
4. Fixed BLE initialization order in lifecycle test

## Future Considerations
- Consider adding documentation about BTstack bond persistence requirements
- May want to add more tests for edge cases (e.g., bond deletion, multiple devices)
- Could enhance error messages to indicate when bond persistence fails
# Bluetooth Multi-Test Results Report

**Date:** Thu Jun 12 17:37:58 AEST 2025  
**Firmware:** BTstack with bond persistence fixes  
**Device:** Raspberry Pi Pico2 W  
**Test Environment:** Two devices on /dev/ttyACM0 and /dev/ttyACM1  

## Summary

This report documents the results of running all Bluetooth multi-tests after implementing the critical bond persistence fix in BTstack. The fix addressed the root cause where `mp_bluetooth_gap_on_set_secret()` was failing when no IRQ handler was registered, blocking TLV storage operations.

---

## Test Results


### 🔥 Phase 1: Critical Bond Persistence Tests

Tests that verify the core bond persistence functionality after our fixes.


#### Test 1: ble_gap_pair_bond.py

```bash
./tests/run-multitests.py -i pyb:/dev/ttyACM1 -i pyb:/dev/ttyACM0 -t ./tests/multi_bluetooth/ble_gap_pair_bond.py
```

**Result:** ❌ **FAILED** (exit code: 1)

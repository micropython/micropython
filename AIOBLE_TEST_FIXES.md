# aioble Test Fixes Required

**Issue:** The `ble_pair_bond_*.py` test files are calling `aioble.shutdown()` which doesn't exist.

**Files that need fixing:**
- `lib/micropython-lib/micropython/bluetooth/aioble/multitests/ble_pair_bond_reboot.py`
- `lib/micropython-lib/micropython/bluetooth/aioble/multitests/ble_pair_bond_persist.py`

**Error:**
```
AttributeError: 'module' object has no attribute 'shutdown'
```

**Fixes Applied:**

1. ✅ Changed all instances of `aioble.shutdown()` to `aioble.stop()`
2. ✅ Added missing `import aioble.security` 
3. ✅ Fixed `encrypt=True` parameter by using `EncryptedCharacteristic` class
4. ❌ Still failing: `aioble.connect()` method doesn't exist

**Status:** Partially fixed locally, but these are in the micropython-lib submodule

**Additional Issues Found:**
- Tests try to call `aioble.connect()` which doesn't exist  
- May need to use `Device.connect()` pattern instead
- Tests may be for older/different aioble API version

**Note:** These changes should be committed to the micropython-lib repository separately.

**Difference between the files:**
- `ble_bond_*.py` files - Use `aioble.stop()` (correct)
- `ble_pair_bond_*.py` files - Were using `aioble.shutdown()` (incorrect, now fixed)

Both sets of files test bond persistence but with slightly different approaches.
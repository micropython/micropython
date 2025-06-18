# Bond Persistence Test Issue

## Problem
The bond persistence lifecycle test fails after BLE restart. While initial pairing works correctly, after restarting BLE with `ble.active(0)` and `ble.active(1)`, the connection immediately drops without establishing encryption.

## Symptoms
1. Initial pairing works - both devices connect, pair, and establish encryption
2. 3 secrets are saved (not 2 as originally expected)
3. After BLE restart, connection is established but immediately drops
4. No encryption update event is received after restart
5. The peripheral times out waiting for encryption update (IRQ 28)

## Key Observations
1. The aioble test (`ble_pair_bond_persist.py`) works correctly
2. The aioble test uses file-based secret storage
3. The aioble test doesn't fully restart BLE - it uses `aioble.stop()` which is a higher-level operation
4. BTstack may require bonds to be loaded BEFORE `ble.active(1)` is called

## Attempted Fixes
1. ✅ Fixed `mp_bluetooth_gap_on_set_secret()` to return true when no handler
2. ✅ Fixed IRQ_GET_SECRET handler to support indexed lookups
3. ✅ Updated test expectations for 3 secrets instead of 2
4. ❌ In-memory secret storage doesn't persist across BLE restart
5. ❌ File-based storage with shared file causes conflicts between instances

## Differences from aioble Test
1. aioble registers IRQ handler globally via `register_irq_handler()`
2. aioble uses `aioble.stop()` instead of `ble.active(0)`
3. aioble maintains persistent file storage
4. aioble's security module handles secret save/load automatically

## Next Steps
1. Investigate if BTstack requires bonds to be present in TLV storage before activation
2. Consider if the test needs to simulate a true device restart differently
3. May need to ensure secrets are properly synced to BTstack's TLV storage
4. Could potentially use separate test instances to avoid file conflicts

## Technical Details
- BTstack uses TLV (Tag-Length-Value) storage for bonds
- The `le_device_db_tlv` module manages the bond database
- IRQ handlers must be set BEFORE `ble.active(1)` for bond loading
- Secret types stored: ER/IR keys plus additional BTstack-specific data
# BTstack Bond Loading Diagnostic Questions

**Date:** 2025-06-13  
**Issue:** Bonds are successfully stored but not loaded/restored during BLE restart  
**Status:** Need to investigate bond loading mechanism

## BTstack Initialization & Bond Loading

1. **When should bond loading happen?** 
   - During `mp_bluetooth_init()`?
   - After stack startup?
   - During the first connection attempt?

2. **TLV vs Database Loading**: 
   - Are you seeing any `TLV_GET` calls for bond data (tags like `0x42544400`-`0x4254440f`) during BLE restart?
   - Or are those only happening during the initial empty database scan?

3. **ER/IR Key Restoration**: 
   - When BLE restarts, are the ER/IR keys (tags `0x534d4552`/`0x534d4952`) being loaded back from storage?
   - Or are new ones being generated each time?

## BTstack Configuration & Flow

4. **Device Database vs TLV**: 
   - Is BTstack using `le_device_db_tlv_configure()` properly? 
   - Should there be explicit calls to load the device database during init?

5. **Stack State**: 
   - Does the bond loading need to happen at a specific BTstack state (like `HCI_STATE_WORKING`)?
   - Or can it happen earlier?

6. **Address Consistency**: 
   - Are the Bluetooth addresses staying consistent across restarts? 
   - Bond data is tied to specific addresses.

## Implementation Details

7. **Automatic vs Manual Loading**: 
   - Should BTstack automatically load bonds from TLV during `le_device_db_tlv_configure()`?
   - Or do we need manual `le_device_db_tlv_get_*()` calls?

8. **Timing vs Pairing State**: 
   - Is there a difference between the TLV operations during initial pairing (working) vs during reconnection attempts (failing)?

9. **Alternative Storage**: 
   - Have you tried BTstack's examples like `le_device_db_tlv_test.c` to see how they handle bond loading?

## Additional Investigation Areas

10. **Bond Storage Format**:
    - What exact data is stored in each TLV tag?
    - Is the format compatible with what BTstack expects on load?

11. **Security Manager State**:
    - Is the Security Manager (SM) properly initialized before bond loading?
    - Are there missing SM configuration steps?

12. **Event Sequence**:
    - What events should trigger bond loading?
    - Is there a missing event handler or callback?

## Test Results Summary

- ✅ Initial pairing and bonding works
- ✅ TLV_STORE operations succeed
- ✅ Encryption established on first connection
- ❌ Bonds not restored after BLE restart
- ❌ Reconnection attempts timeout
- ❌ Missing encryption update events on reconnection

## Next Steps

- Enable TLV debug logging during BLE restart to see read attempts
- Compare BTstack example code for bond loading patterns
- Check if bonds are tied to addresses that change on restart
- Investigate if manual bond loading calls are needed
- Review BTstack documentation for bond persistence requirements
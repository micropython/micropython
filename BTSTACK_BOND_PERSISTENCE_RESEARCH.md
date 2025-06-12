# BTstack Bond Persistence Deep Research Investigation

## Current Status and Problem Statement

Following the successful implementation of automatic ER/IR key generation via TLV in BTstack, we have discovered that while initial pairing works correctly, bond data persistence across BLE instance restarts is not functioning as expected. This document outlines a comprehensive research plan to investigate and resolve bond persistence issues.

## Confirmed Working Components

1. **TLV Infrastructure**: BTstack TLV callbacks are properly implemented in `modbluetooth_btstack.c`
2. **ER/IR Key Generation**: BTstack automatically generates and stores cryptographically secure ER/IR keys via TLV tags 'SMER'/'SMIR' 
3. **Initial Pairing**: Device pairing and bonding works correctly (verified by `ble_gap_pair_bond.py` test)
4. **Secret Storage**: MicroPython's secret storage system (`mp_bluetooth_gap_on_get_secret`/`mp_bluetooth_gap_on_set_secret`) is functional

## Problem Manifestation

The `ble_gap_pair_bond_reconnect.py` test reveals:
- Initial pairing succeeds with `_IRQ_ENCRYPTION_UPDATE 1 0 1`
- After BLE instance restart, devices connect but encryption update times out
- This suggests bond data (LTK, IRK, CSRK) is not being properly restored

## BTstack Documentation Findings

### Architecture and Configuration Requirements

**BTstack Bond Storage Architecture** (from lib/btstack documentation):
- **Application Layer**: Uses `le_device_db.h` and `btstack_link_key_db.h` APIs
- **Storage Layer**: Implements TLV (Tag-Value-Length) storage via `btstack_tlv.h`
- **Hardware Layer**: Platform-specific flash implementation via `hal_flash_bank.h`

**Required Configuration Directives** (from BTstack manual):
```c
#define NVM_NUM_DEVICE_DB_ENTRIES  // Max number of LE Device DB entries that can be stored
#define NVM_NUM_LINK_KEYS         // Max number of Classic Link Keys that can be stored  
#define MAX_NR_LE_DEVICE_DB_ENTRIES // Max number of items in LE Device DB (RAM cache)

// Flash Configuration (if needed)
#define ENABLE_TLV_FLASH_EXPLICIT_DELETE_FIELD  // For flash that can't overwrite with zero
#define ENABLE_TLV_FLASH_WRITE_ONCE            // For write-once flash memory
```

**BTstack TLV Tag Format**:
- **LE Device DB**: Tags formatted as `'BTD' + index` (0x42544400 + index)
- **Link Keys**: Tags formatted as `'BTL' + index` (0x42544C00 + index)
- **GATT Server**: Tags formatted as `'BTM' + index` for client characteristic configs

**Standard BTstack Setup Pattern** (from posix examples):
```c
// 1. Initialize TLV implementation
tlv_impl = btstack_tlv_posix_init_instance(&tlv_context, tlv_db_path);

// 2. Set global TLV instance
btstack_tlv_set_instance(tlv_impl, &tlv_context);

// 3. Configure device databases
#ifdef ENABLE_CLASSIC
hci_set_link_key_db(btstack_link_key_db_tlv_get_instance(tlv_impl, &tlv_context));
#endif

#ifdef ENABLE_BLE  
le_device_db_tlv_configure(tlv_impl, &tlv_context);
#endif
```

**Available TLV Implementations**:
- `btstack_tlv_posix.c` - File-based storage for POSIX systems
- `btstack_tlv_esp32.c` - ESP32 NVS storage
- `btstack_tlv_flash_bank.c` - Generic flash bank implementation
- `btstack_tlv_windows.c` - Windows registry storage
- `btstack_tlv_none.c` - No-op implementation

## Research Investigation Areas

### 1. BTstack Bond Database Analysis

**Objective**: Understand how BTstack manages bond storage and retrieval

**Key Research Questions**:
- How does BTstack's `le_device_db_tlv.c` interact with TLV storage for bond data?
- What TLV tags does BTstack use for storing bond information beyond ER/IR keys?
- When and how does BTstack attempt to load existing bonds during initialization?
- What is the relationship between ER/IR keys and actual bond data (LTK, IRK, CSRK)?

**Files to Investigate**:
```
lib/btstack/src/ble/le_device_db_tlv.c
lib/btstack/src/ble/sm.c  
lib/btstack/src/btstack_tlv.c
lib/btstack/src/ble/le_device_db.h
lib/btstack/test/le_device_db_tlv/le_device_db_tlv_test.cpp  # Complete test reference
```

**Debugging Approach**:
- Add extensive debug logging to TLV get/store/delete operations
- Log all BTstack TLV tag operations with tag names and data
- Trace `le_device_db_tlv_*` function calls during pairing and reconnection
- Monitor when BTstack calls `btstack_tlv_get_instance()` vs our `btstack_tlv_set_instance()`

### 2. TLV Tag Mapping and Bond Data Flow

**Objective**: Map the complete flow of bond data through TLV storage

**Key Research Questions**:
- What are all the TLV tags BTstack uses for bond storage? (Beyond 'SMER'/'SMIR')
- How are device-specific bonds identified? (By BD_ADDR? By internal index?)
- What bond data elements are stored: LTK, IRK, CSRK, key sizes, authentication info?
- Is there a bond database index or lookup mechanism we're missing?

**Implementation Strategy**:
```c
// Enhanced TLV debugging in modbluetooth_btstack.c
static int btstack_tlv_mp_get_tag(void *context, uint32_t tag, uint8_t *buffer, uint32_t buffer_size) {
    char tag_str[5] = {(tag>>24)&0xFF, (tag>>16)&0xFF, (tag>>8)&0xFF, tag&0xFF, 0};
    DEBUG_printf("TLV_GET: tag=0x%08x (%s) size=%u\n", tag, tag_str, buffer_size);
    
    // Add logging of buffer contents on successful retrieval
    const uint8_t *data;
    size_t data_len;
    if (mp_bluetooth_gap_on_get_secret(0, 0, (uint8_t *)&tag, sizeof(tag), &data, &data_len)) {
        DEBUG_printf("TLV_GET: found %zu bytes\n", data_len);
        // Log first few bytes for analysis
        for (int i = 0; i < min(8, data_len); i++) {
            DEBUG_printf(" %02x", data[i]);
        }
        DEBUG_printf("\n");
    } else {
        DEBUG_printf("TLV_GET: tag not found\n");
    }
    // ... existing implementation
}
```

### 3. BLE Instance Lifecycle and Bond Restoration

**Objective**: Understand the timing and sequence of bond restoration

**Key Research Questions**:
- When during BLE initialization does BTstack attempt to load bonds?
- Is our `ble.active(0)` / `ble.active(1)` sequence properly preserving bond data?
- Does BTstack require specific initialization order for bond restoration?
- Are there BTstack configuration flags that affect bond persistence?

**Areas to Investigate**:
- BTstack initialization sequence in `mp_bluetooth_init()`
- HCI state transitions and when bond loading occurs
- Effect of `sm_init()` timing on bond restoration
- Impact of `le_device_db_init()` on existing bond data

### 4. MicroPython Secret Storage Integration

**Objective**: Verify MicroPython's secret storage is working correctly for bond data

**Key Research Questions**:
- Are all bond-related TLV operations successfully reaching MicroPython's secret storage?
- Is secret storage persistent across `ble.active()` cycles?
- Are there size limitations or data format issues affecting bond storage?
- How does secret storage handle multiple bonds (multiple paired devices)?

**Testing Strategy**:
```python
# Add secret storage introspection to tests
def debug_secret_storage():
    # Log all secrets stored during pairing
    # Verify secrets persist after BLE restart
    # Check secret data integrity
    pass
```

### 5. BTstack Bond Database Configuration

**Objective**: Ensure BTstack's bond database is properly configured

**Key Research Questions**:
- Is `le_device_db_tlv_configure()` being called with correct parameters?
- Are there BTstack compile-time flags affecting bond persistence?
- Does BTstack's TLV implementation require specific initialization order?
- Are there missing BTstack API calls for bond restoration?

**Configuration Investigation**:
```c
// In mp_bluetooth_init(), verify proper bond DB setup
le_device_db_tlv_configure(&btstack_tlv_mp, NULL);
le_device_db_init();

// Check if we need additional calls:
// le_device_db_set_local_bd_addr()?
// Additional SM configuration?
```

### 6. Reference Implementation Analysis

**Objective**: Study working BTstack bond persistence implementations

**Key Research Questions**:
- How do other BTstack integrations handle bond persistence?
- Are there reference implementations with working TLV bond storage?
- What can we learn from BTstack's own test/example code?

**Critical Findings from BTstack Documentation**:

**Flash Implementation Requirements** (from hal_flash_bank.h):
- **Two banks**: One active, one for migration/defragmentation
- **Bank header**: 8-byte header with "BTstack" magic and epoch counter
- **Entry format**: 32-bit tag + 32-bit length + data + optional delete field

**Integration Requirements for MicroPython**:
1. **TLV Implementation**: Choose or create platform-specific TLV storage
2. **Configuration**: Set NVM limits in `btstack_config.h`
3. **Initialization**: Configure TLV instance and device database during stack startup
4. **Memory**: Allocate RAM cache for device database entries

**Comprehensive Test Reference** (le_device_db_tlv_test.cpp):
- Shows complete bond storage/retrieval test cases
- Demonstrates encryption parameter management
- Includes CSRK (Connection Signature Resolving Key) handling
- Provides flash bank memory simulation setup

**Files to Study**:
```
lib/btstack/example/
lib/btstack/test/le_device_db_tlv/le_device_db_tlv_test.cpp  # Complete reference
lib/btstack/port/posix-h4/main.c  # Standard initialization pattern
lib/btstack/platform/
```

### 7. Advanced Debugging Implementation

**Objective**: Create comprehensive debugging tools for bond persistence

**Implementation Plan**:

```c
// Add to modbluetooth_btstack.c
#ifdef DEBUG_BOND_PERSISTENCE
static void debug_dump_tlv_contents(void) {
    DEBUG_printf("=== TLV Bond Database Dump ===\n");
    // Enumerate and dump all stored TLV entries
    // Show tag names, sizes, and data snippets
}

static void debug_bond_state(const char *event) {
    DEBUG_printf("=== Bond State at %s ===\n", event);
    // Log current bond count, device database state
    // Show loaded bonds and their properties
}
#endif
```

**Test Enhancement**:
```python
# Enhanced bond persistence test with detailed logging
def test_bond_persistence_detailed():
    # Log secret storage state at each step
    # Dump TLV contents before/after pairing
    # Verify specific bond data elements
    # Test multiple device bonds
    pass
```

### 8. Systematic Testing Matrix

**Objective**: Create comprehensive test scenarios for bond persistence

**Test Scenarios**:
1. **Single Device Bond Persistence**:
   - Pair → BLE restart → Reconnect
   - Verify encryption without re-pairing

2. **Multiple Device Bonds**:
   - Pair with Device A → Pair with Device B → BLE restart → Reconnect to both
   - Verify bond isolation and correct key retrieval

3. **Bond Data Integrity**:
   - Verify LTK, IRK, CSRK persistence
   - Test authentication level preservation
   - Verify key size and security properties

4. **Edge Cases**:
   - Bond storage during low memory conditions
   - Concurrent pairing operations
   - Bond deletion and cleanup

**Success Criteria**:
- Reconnection works without re-pairing
- Encrypted characteristics readable without authentication
- Multiple bonds maintained correctly
- Bond data survives BLE instance restart

## Key Investigation Priorities Based on BTstack Documentation

### Immediate Action Items

1. **Verify Configuration**: Check if BTstack configuration includes required NVM limits:
   ```c
   #define NVM_NUM_DEVICE_DB_ENTRIES 16
   #define MAX_NR_LE_DEVICE_DB_ENTRIES 16
   ```

2. **Initialization Order**: Verify BTstack setup follows standard pattern:
   ```c
   // Current: btstack_tlv_set_instance(&btstack_tlv_mp, NULL);
   // Required: le_device_db_tlv_configure(&btstack_tlv_mp, NULL);
   ```

3. **TLV Tag Analysis**: Monitor for BTstack's standard bond tags:
   - `'BTD' + index` (0x42544400 + index) for LE Device DB entries
   - Verify our custom implementation handles these correctly

4. **Test Against Reference**: Compare with comprehensive test in `le_device_db_tlv_test.cpp`

### Expected Outcomes

### Phase 1: Understanding (Week 1-2)
- Complete mapping of BTstack bond data flow using documentation insights
- Verification against BTstack's standard initialization pattern
- Analysis of TLV tag usage compared to documented format
- Configuration validation against BTstack requirements

### Phase 2: Implementation (Week 3-4)
- Fix identified bond persistence issues based on BTstack patterns
- Enhanced debugging infrastructure following BTstack examples
- Implementation alignment with documented TLV tag format

### Phase 3: Validation (Week 5)
- All bond persistence tests passing using BTstack-compliant implementation
- Documentation of bond storage mechanisms matching BTstack architecture
- Performance validation against BTstack reference implementations

## Technical Artifacts

**Documentation Outputs**:
1. `BTSTACK_BOND_FLOW.md` - Complete bond data flow diagram
2. `TLV_TAG_REFERENCE.md` - All BTstack TLV tags and their purposes
3. `BOND_TESTING_GUIDE.md` - Comprehensive test procedures

**Code Outputs**:
1. Enhanced TLV debugging infrastructure
2. Fixed bond persistence implementation
3. Comprehensive bond persistence test suite
4. Bond management utilities for debugging

## Success Metrics

1. **Functional**: `ble_gap_pair_bond_reconnect.py` test passes consistently
2. **Performance**: Bond restoration completes within acceptable timeframes
3. **Reliability**: Bond data survives multiple BLE restart cycles
4. **Scalability**: Multiple concurrent bonds maintained correctly

This research investigation will provide the foundation for robust, persistent BLE bonding in MicroPython's BTstack implementation, completing the work started with automatic ER/IR key generation.
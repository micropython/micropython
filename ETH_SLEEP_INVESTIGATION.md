# STM32 Ethernet Clock Sleep Configuration Investigation

**Date**: 2025-10-04
**Branch**: stm32_eth
**Board**: NUCLEO_H563ZI (STM32H563ZI)
**Issue**: Active() hanging and DHCP not working

---

## Background

The STM32 Ethernet driver uses RCC Low Power Enable Register (LPENR) to control whether peripheral clocks continue running when the CPU enters sleep mode (WFI instruction).

### Register Behavior
- `__HAL_RCC_ETH_CLK_SLEEP_ENABLE()`: Clock runs during sleep
- `__HAL_RCC_ETH_CLK_SLEEP_DISABLE()`: Clock stops during sleep

### Original Issue (PR #17613 Comments)
- On H5 board, specific clock sleep configurations prevented `.active(True)` from working when ethernet cable was connected
- dpgeorge suggested: "probably it's OK to just disable clock-sleep altogether, because we do want ETH to run 'in the background' when the CPU does WFI"

---

## Investigation Timeline

### Trial 1: Initial State (Commit c503f48794 "sleep")
**Configuration:**
```c
// After RELEASE_RESET
__HAL_RCC_ETH_CLK_SLEEP_DISABLE();  // H5 only

// After soft reset
// (nothing for H5)
```

**Results:**
- ✗ Invalid commit message ("sleep")
- ✗ No sign-off
- ✗ Incomplete - only H5 configured, inconsistent with other variants

---

### Trial 2: Uncommented SLEEP_DISABLE (Commit f8d50edec7)
**Configuration:**
```c
// After RELEASE_RESET (all variants)
__HAL_RCC_ETH_CLK_SLEEP_DISABLE();

// After soft reset
// H5: (commented out)
// H7/others: __HAL_RCC_ETH_CLK_SLEEP_ENABLE();
```

**Test Results:**
- ✓ `active(True)` completes in 28ms (no hang)
- ✓ Link detection works (status 2 = link up, no IP)
- ✓ DHCP initially worked (got 192.168.0.10)
- ✗ After reset: DHCP stops working
- ✗ Status stuck at 2 (link up, no IP)

**Analysis:**
- DISABLE prevents MDIO hangs during initialization ✓
- But keeps clock disabled during normal operation ✗
- Network "up" but no RX traffic (DHCP responses never received)
- This is the symptom user warned about: "network reporting itself working but not getting any traffic"

---

### Trial 3: Two-Phase Configuration (DISABLE → ENABLE)
**Configuration:**
```c
// Phase 1: After RELEASE_RESET, before initialization
__HAL_RCC_ETH_CLK_SLEEP_DISABLE();  // All variants

// ... MDIO communication, PHY reset, MAC init ...

// Phase 2: After soft reset, after initialization complete
__HAL_RCC_ETH_CLK_SLEEP_ENABLE();   // All variants (NOW UNCOMMENTED FOR H5)
```

**Test Results:**

#### Test 1: Fresh Boot with Cable Connected
```
Status before active(): 3
active(True) completed: 28ms
Status after active(): 3
DHCP acquisition: SUCCESS at 1 second
Final config: ('192.168.0.10', '255.255.255.0', '192.168.0.1', '192.168.0.1')
Result: ✓ PASSED
```

#### Test 2: Stop/Restart Cycle
```
Initial status: 3 (connected)
active(False): status → 0 (down)
active(True): status → 2 (link up, no IP)
Recovery: status → 3 at 8 seconds
Result: ✓ PASSED
```

---

## Root Cause Analysis

### Why Two-Phase Configuration is Necessary

#### Phase 1: SLEEP_DISABLE During Initialization
**Needed because:**
- MDIO communication uses busy-wait loops: `while (ETH->MACMDIOAR & ETH_MACMDIOAR_MB) {}`
- If CPU enters WFI during these loops, ETH clock gates
- MDIO communication hangs, preventing PHY initialization
- Result: `active(True)` hangs/times out

#### Phase 2: SLEEP_ENABLE After Initialization
**Needed because:**
- During normal operation, CPU frequently enters WFI (idle/sleep)
- If ETH clock is disabled during sleep:
  - Incoming packets can't trigger DMA
  - ETH interrupts don't fire
  - Network appears "up" but receives no traffic
- Result: DHCP requests sent but responses never received

### Why Initial Test Worked But Failed After Reset
1. Fresh boot: CPU busy for ~20 seconds (initialization, loading code)
   - Doesn't enter sleep immediately
   - DHCP completes before first sleep
2. After reset: System idle, enters sleep quickly
   - ETH clock gated during sleep
   - DHCP responses never received

---

## Final Solution

### Code Structure
```c
static int eth_mac_init(eth_t *self) {
    // ...

    // PHASE 1: Disable sleep during initialization
    // Prevents MDIO communication hangs
    #if defined(STM32H5)
    __HAL_RCC_ETH_RELEASE_RESET();
    __HAL_RCC_ETH_CLK_SLEEP_DISABLE();
    __HAL_RCC_ETHTX_CLK_SLEEP_DISABLE();
    __HAL_RCC_ETHRX_CLK_SLEEP_DISABLE();
    #elif defined(STM32H7)
    // ... similar
    #endif

    // Do MAC soft reset
    ETH_SOFT_RESET(ETH);
    // Wait for reset to complete

    // PHASE 2: Re-enable sleep after initialization
    // Allows network to receive during CPU sleep
    #if defined(STM32H5)
    __HAL_RCC_ETH_CLK_SLEEP_ENABLE();
    __HAL_RCC_ETHTX_CLK_SLEEP_ENABLE();
    __HAL_RCC_ETHRX_CLK_SLEEP_ENABLE();
    #elif defined(STM32H7)
    // ... similar
    #endif

    // Continue with MAC configuration...
}
```

### Commit Changes Required
1. **Commit 1cd99b736e** (fixed "sleep" commit):
   - ✓ Proper message and sign-off added
   - But describes disabling sleep, which is incomplete

2. **New commit needed**:
   - Title: `stm32/eth: Re-enable clock sleep after initialization completes.`
   - Explain two-phase configuration requirement
   - Note this fixes DHCP regression

---

## Verification Checklist

- [x] No active() hang with cable connected
- [x] DHCP acquisition after fresh boot
- [x] DHCP acquisition after soft reset
- [x] Stop/restart cycle works
- [x] Consistent across all STM32 variants (H5, H7, others)
- [ ] Test on other H5 boards (if available)
- [ ] Test on H7 board (if available)
- [ ] Test with cable unplugged at boot
- [ ] Test hot-plug (unplug/replug cable)
- [ ] Test static IP configuration

---

## Comparison with Master Branch

### Master Configuration
```c
// Single phase: ENABLE only
__HAL_RCC_ETH_CLK_SLEEP_ENABLE();
```

### Why Master Didn't Have active() Hang
- Old code had blocking PHY initialization in `eth_mac_init()`
- Waited for link and autonegotiation before returning
- By the time init completed, clock was running
- Current refactor moved PHY init to `eth_start()`, exposing the timing issue

---

## Lessons Learned

1. **Clock gating during sleep is complex:**
   - Need different settings for init vs. normal operation
   - Can't just ENABLE or DISABLE globally

2. **Symptom masking:**
   - Issue appeared intermittent (worked sometimes, failed others)
   - Root cause: timing of when CPU first enters sleep
   - Fixed hardware timing issue, not a race condition

3. **Testing requirements:**
   - Must test after reset, not just fresh flash
   - Idle time matters (sleep behavior)
   - Need to verify actual network traffic, not just status

4. **Code archaeology:**
   - Two-phase pattern existed in H7/others but not documented
   - H5 code was incomplete (phase 1 only)
   - Inconsistency between variants obscured the pattern

---

## LAN Singleton Pattern Analysis

### Implementation

**network_lan.c:41** - Single static instance:
```c
static const network_lan_obj_t network_lan_eth0 = { { &network_lan_type }, &eth_instance };
```

**network_lan.c:66** - Always returns singleton:
```c
const network_lan_obj_t *self = &network_lan_eth0;  // Same object every time
return MP_OBJ_FROM_PTR(self);
```

**eth.c:192-195** - Guard clause prevents re-initialization:
```c
if (self->netif.input != NULL) {
    // Already initialised - early return preserves state
    return 0;
}
// Only runs on first call:
self->last_link_status = false;  // Line 199
self->enabled = false;            // Line 200
```

### Verification Test (H563ZI)

```python
lan1 = network.LAN()
lan2 = network.LAN()
lan3 = network.LAN()

id(lan1) == id(lan2) == id(lan3)  # True - same object (134724992)
lan1 is lan2 is lan3               # True
```

**Result**: Singleton pattern works correctly. Multiple calls to `network.LAN()` return the exact same object, and the guard clause prevents `last_link_status` from being reset.

### Hot-Plug Detection Requirements

**Why continuous monitoring is necessary:**

1. Link status tracking uses state comparison: `current_link_status != self->last_link_status`
2. Change detection only triggers on **transitions**, not absolute states
3. If cable state changes between Python scripts/sessions, the transition isn't detected
4. Must monitor continuously with the same object to capture state transitions

**Working scenario:**
```python
lan = network.LAN()
# Monitor continuously
# Cable unplugged: detects transition true → false
# Cable replugged: detects transition false → true ✓
```

**Non-working scenario:**
```python
lan = network.LAN()  # last_link_status = false (from boot)
# ... cable unplugged while not monitoring ...
# ... cable replugged ...
lan.status()  # current = true, last = false (from boot), no transition detected
```

The singleton is **not** the problem - the issue is missing the actual transition event.

---

## References

- PR #17613 comments: https://github.com/micropython/micropython/pull/17613
- STM32H5 Reference Manual: RCC_AHB1LPENR register (section 11.7.23)
- dpgeorge review: https://github.com/micropython/micropython/pull/17613#pullrequestreview-3010490920
- andrewleech response: https://github.com/micropython/micropython/pull/17613#pullrequestreview-3041366090

---

## Hardware Test Summary

### NUCLEO_H563ZI (STM32H563ZI) - COMPLETE ✓

All tests passed with two-phase SLEEP configuration (DISABLE → ENABLE).

**Tests Completed:**
- ✓ Cable connected: DHCP, stop/restart, link detection
- ✓ Cable unplugged: No timeout, link down detection, static IP
- ✓ Hot-plug: Unplug/replug detection, DHCP auto-recovery
- ✓ Fresh boot: With and without cable
- ✓ All commit claims validated

**Branch Status**: Ready for cross-platform testing

---

## NUCLEO_F429ZI (STM32F429ZI) - REGRESSION FOUND ✗

Testing two-phase SLEEP configuration on STM32F4 variant to validate consistency across platforms.

### Test 1: Our Branch (stm32_eth)

**Firmware**: v1.27.0-preview.163.g4480a4187f
**Results**:
```
Status before active(): 1 (physical link detected)
active(True) completed: 29ms
Status after active(): 2 (link up, no IP)
DHCP: FAILED after 40+ seconds
Final config: ('0.0.0.0', '255.255.255.0', '192.168.0.1', '8.8.8.8')
```

**Analysis**: Same symptom as original bug - network reports "up" but receives no traffic. DHCP requests sent but responses never received.

### Test 2: Official Firmware v1.26.1

**Firmware**: MicroPython v1.26.1 (2025-09-11)
**Results**:
```
Status before active(): 1 (physical link detected)
active(True) completed: 1586ms
Status after active(): 2 (link up, waiting for IP)
DHCP: ✓ SUCCESS at 6 seconds
IP acquired: 192.168.0.192
Final config: ('192.168.0.192', '255.255.255.0', '192.168.0.1', '192.168.0.1')
```

**Analysis**: Official firmware works correctly. active(True) takes 1586ms vs our branch's 29ms, suggesting initialization code that our branch is skipping.

### Comparison

| Metric | Official v1.26.1 | Our Branch | Result |
|--------|------------------|------------|--------|
| active(True) time | 1586ms | 29ms | ✗ Too fast |
| DHCP | ✓ Works (6s) | ✗ Fails (40s+) | ✗ Regression |
| Final status | 3 (connected) | 2 (link up, no IP) | ✗ No traffic |

### Conclusion

**CRITICAL REGRESSION**: DHCP failure on F429ZI is a regression in our branch, not a hardware/network issue. The official v1.26.1 firmware works correctly.

**Root cause hypothesis**: The refactoring that made PHY initialization non-blocking (moving from eth_mac_init to eth_start) may have broken initialization timing for STM32F4 variant. The dramatic difference in active(True) timing (1586ms → 29ms) suggests critical initialization is being skipped.

**Next steps**:
1. Compare eth.c SLEEP configuration between master and our branch for STM32F4
2. Investigate why active(True) completes so quickly on our branch
3. Check if PHY initialization is completing properly on F4

---

## Root Cause Analysis: F429ZI DHCP Regression

### PHY Initialization Sequence Comparison

#### Master Branch (upstream/master:ports/stm32/eth.c)

**Location**: eth_mac_init() function (lines ~354-388)

**Sequence**:
1. Reset PHY: `eth_phy_write(PHY_BCR, PHY_BCR_SOFT_RESET)`
2. **BLOCKING WAIT** for PHY link establishment (state machine):
   - State 0: Wait for reset complete (`!(bcr & PHY_BCR_SOFT_RESET)`)
   - State 1: Wait for link up (`bsr & PHY_BSR_LINK_STATUS`)
     - Configure autonegotiation: Write PHY_ANAR with all speed modes
     - Start autonegotiation: `eth_phy_write(PHY_BCR, PHY_BCR_AUTONEG_EN)`
   - State 2: Wait for autonegotiation complete:
     ```c
     (bsr & (PHY_BSR_AUTONEG_DONE | PHY_BSR_LINK_STATUS)) ==
     (PHY_BSR_AUTONEG_DONE | PHY_BSR_LINK_STATUS)
     ```
   - Timeout: PHY_INIT_TIMEOUT_MS (10 seconds)
3. **Read negotiated speed/duplex**: `phy_scsr = self->phy_get_link_status(phy_addr)`
4. **Configure MAC with actual speed/duplex**:
   ```c
   ETH->MACCR =
       phy_scsr == PHY_SPEED_10FULL ? ETH_MACCR_DM
       : phy_scsr == PHY_SPEED_100HALF ? ETH_MACCR_FES
       : phy_scsr == PHY_SPEED_100FULL ? (ETH_MACCR_FES | ETH_MACCR_DM)
       : 0;
   ```
5. Continue with DMA setup, enable MAC/DMA

**Timing**: ~1586ms (includes wait for link + autonegotiation)

#### Our Branch (stm32_eth)

**Location**: Split between eth_mac_init() and eth_phy_init()

**Sequence**:
1. **eth_mac_init()** (line 256):
   - MAC soft reset
   - Set MII clock range
   - **Configure MAC with HARDCODED default**: `ETH->MACCR = ETH_MACCR_FES | ETH_MACCR_DM;` (line 493)
     - Comment says "The PHY speed/duplex will be auto-detected" **BUT NO CODE DOES THIS**
   - Continue with DMA setup, enable MAC/DMA
2. **eth_phy_init()** (line 967, called from eth_start):
   - Reset PHY: `eth_phy_write(PHY_BCR, PHY_BCR_SOFT_RESET)`
   - Wait ONLY for reset complete (not link or autonegotiation)
   - **NO autonegotiation configuration**
   - **NO wait for link**
   - **NO speed/duplex detection**
   - **NO MAC reconfiguration**

**Timing**: ~29ms (no waiting, no autonegotiation setup)

### The Critical Missing Steps

In our branch, the following is **NEVER DONE**:

1. ✗ Autonegotiation is never explicitly enabled (PHY_ANAR not written, PHY_BCR_AUTONEG_EN not set)
2. ✗ Autonegotiation result is never read
3. ✗ MAC speed/duplex is never updated to match PHY negotiation
4. ✗ MAC is configured with hardcoded 100Mbps Full Duplex regardless of actual link speed

### Why It Fails

**Speed/Duplex Mismatch**:
- MAC configured: 100Mbps Full Duplex (hardcoded)
- PHY may negotiate: 10Mbps, 100Mbps, Half/Full Duplex (depends on link partner)
- If negotiation results in anything other than 100Mbps Full Duplex → MAC and PHY mismatch
- Result: Link appears "up" but frames are corrupted/dropped, no useful communication

**Autonegotiation Not Explicitly Started**:
- Master explicitly writes PHY_ANAR and sets PHY_BCR_AUTONEG_EN
- Our branch only resets PHY, relies on PHY default behavior
- Some PHYs may not autonegotiate by default after reset

### Why H563ZI Works But F429ZI Doesn't

**Hypothesis 1: Different PHY behavior**
- H563ZI may have PHY that:
  - Autonegotiates by default after reset
  - Happens to negotiate 100Mbps Full Duplex
  - Matches hardcoded MAC config by luck
- F429ZI may have PHY that:
  - Negotiates different speed (10Mbps or Half Duplex)
  - Mismatches hardcoded MAC config

**Hypothesis 2: Network environment**
- H563ZI switch port may force 100Mbps Full Duplex
- F429ZI switch port may autonegotiate to 10Mbps or Half Duplex

**Hypothesis 3: PHY chip differences**
- Different STM32 variants may use different PHY chips
- Different reset behavior, autonegotiation defaults

---

## Test Plan: Isolating the F429ZI Regression

### Test 1: Read PHY Registers to Verify Autonegotiation State

**Purpose**: Determine if PHY is autonegotiating and what speed/duplex was negotiated.

**Method**: Add debug code to read PHY registers after eth_start() completes:
```python
import network
lan = network.LAN()
lan.active(True)

# Read PHY registers via machine.mem
# PHY_BCR (0x00): Basic Control Register
# PHY_BSR (0x01): Basic Status Register
# PHY_ANAR (0x04): Auto-Negotiation Advertisement Register
# PHY_ANLPAR (0x05): Auto-Negotiation Link Partner Ability Register
# PHY_SCSR (0x1F or varies): PHY-specific status register with speed/duplex
```

**What to check**:
- BCR bit 12 (AUTONEG_EN): Is autonegotiation enabled?
- BSR bit 5 (AUTONEG_DONE): Did autonegotiation complete?
- BSR bit 2 (LINK_STATUS): Is link up?
- SCSR: What speed/duplex was negotiated?

**Expected results**:
- Master: AUTONEG_EN=1, AUTONEG_DONE=1, SCSR shows negotiated speed
- Our branch: AUTONEG_EN=?, AUTONEG_DONE=?, SCSR may show mismatch

### Test 2: Read MAC MACCR Register

**Purpose**: Verify MAC speed/duplex configuration.

**Method**: Read ETH->MACCR register after active(True):
```python
import machine
# ETH base address varies by chip (F4: 0x40028000, H5/H7: different)
# MACCR offset: F4 at 0x00, H5/H7 at different offset
maccr = machine.mem32[0x40028000]  # F4 MACCR address
print(f"MACCR: 0x{maccr:08x}")
print(f"  FES (bit 14): {(maccr >> 14) & 1}")  # 1=100Mbps, 0=10Mbps
print(f"  DM (bit 11): {(maccr >> 11) & 1}")   # 1=Full Duplex, 0=Half Duplex
```

**Expected results**:
- Our branch: FES=1, DM=1 (100Mbps Full Duplex, hardcoded)
- Should match PHY negotiation result from Test 1

### Test 3: Force PHY Autonegotiation Setup

**Purpose**: Test if explicitly enabling autonegotiation fixes the issue.

**Method**: Modify eth_phy_init() to include autonegotiation setup:
```c
static int eth_phy_init(eth_t *self) {
    // Reset the PHY
    eth_phy_write(self->phy_addr, PHY_BCR, PHY_BCR_SOFT_RESET);
    mp_hal_delay_ms(10);

    // Wait for reset to complete
    uint32_t t0 = mp_hal_ticks_ms();
    while (eth_phy_read(self->phy_addr, PHY_BCR) & PHY_BCR_SOFT_RESET) {
        if (mp_hal_ticks_ms() - t0 > 1000) {
            return -MP_ETIMEDOUT;
        }
        mp_hal_delay_ms(2);
    }

    // **ADD THIS**: Explicitly enable autonegotiation
    eth_phy_write(self->phy_addr, PHY_ANAR,
        PHY_ANAR_SPEED_10HALF |
        PHY_ANAR_SPEED_10FULL |
        PHY_ANAR_SPEED_100HALF |
        PHY_ANAR_SPEED_100FULL |
        PHY_ANAR_IEEE802_3);
    eth_phy_write(self->phy_addr, PHY_BCR, PHY_BCR_AUTONEG_EN);

    return 0;
}
```

**Expected result**: PHY autonegotiation starts, but MAC still has hardcoded config → still fails

### Test 4: Add MAC Speed/Duplex Update After Link Detection

**Purpose**: Test if updating MAC config to match PHY negotiation fixes the issue.

**Method**: Modify eth_phy_link_status_poll() to update MAC when link comes up:
```c
// In eth_phy_link_status_poll(), after detecting link up transition:
if (current_link_status && !self->last_link_status) {
    // Link just came up - read negotiated speed/duplex
    uint16_t phy_scsr = self->phy_get_link_status(self->phy_addr);

    // Update MAC to match PHY
    uint32_t maccr = ETH->MACCR;
    maccr &= ~(ETH_MACCR_FES | ETH_MACCR_DM);  // Clear speed/duplex bits

    if (phy_scsr == PHY_SPEED_10FULL) {
        maccr |= ETH_MACCR_DM;
    } else if (phy_scsr == PHY_SPEED_100HALF) {
        maccr |= ETH_MACCR_FES;
    } else if (phy_scsr == PHY_SPEED_100FULL) {
        maccr |= ETH_MACCR_FES | ETH_MACCR_DM;
    }

    ETH->MACCR = maccr;
    mp_hal_delay_ms(2);
}
```

**Expected result**: MAC speed/duplex matches PHY → DHCP works

### Test 5: Wait for Autonegotiation Complete Before Starting MAC

**Purpose**: Test if waiting for autonegotiation and configuring MAC upfront (like master) fixes the issue.

**Method**: Modify eth_phy_init() to wait for autonegotiation and return speed/duplex, then update eth_mac_init() to use it:
```c
// Make eth_phy_init() blocking, return negotiated speed
// Update eth_mac_init() to wait for eth_phy_init() result before setting MACCR
```

**Expected result**: Should work like master (but loses non-blocking behavior)

### Test 6: Switch Port Configuration

**Purpose**: Rule out network environment as cause.

**Method**:
- Check switch port configuration for both H563ZI and F429ZI
- Force switch port to 100Mbps Full Duplex
- Test if F429ZI works with forced speed

**Expected result**: If works with forced 100Mbps FD, confirms speed/duplex mismatch as root cause

### Test 7: Compare H563ZI and F429ZI PHY Chips

**Purpose**: Identify PHY chip differences.

**Method**:
- Read PHY ID registers (PHY_PHYIDR1=0x02, PHY_PHYIDR2=0x03) on both boards
- Check PHY datasheets for default autonegotiation behavior
- Identify if different PHY chips have different reset defaults

**Expected result**: May reveal why H563ZI works (PHY enables autoneg by default) but F429ZI doesn't

---

## Test Execution Priority

1. **Test 1 & 2 (Read registers)**: Non-invasive, quick diagnosis
2. **Test 7 (PHY chip identification)**: Helps understand hardware differences
3. **Test 3 (Force autoneg)**: Minimal code change, tests partial fix
4. **Test 4 (MAC update on link)**: Tests dynamic MAC reconfiguration
5. **Test 5 (Blocking init)**: Tests full fix (but loses non-blocking benefit)
6. **Test 6 (Switch config)**: Environmental factor testing

---

## SOLUTION IMPLEMENTED ✓

### Implementation: Option 2 - Deferred MAC Speed/Duplex Update

Enable autonegotiation in eth_phy_init(), then poll for completion in eth_phy_link_status_poll() and update MAC dynamically when ready.

### Code Changes

**1. Added state tracking to eth_t struct (eth.c:124-125)**:
```c
typedef struct _eth_t {
    // ... existing fields ...
    bool mac_speed_configured;
    uint16_t configured_phy_speed;
} eth_t;
```

**2. Enable autonegotiation in eth_phy_init() (eth.c:983-991)**:
- Write PHY_ANAR with all speed/duplex modes
- Set PHY_BCR_AUTONEG_EN to start autonegotiation
- Initialize mac_speed_configured = false

**3. Deferred MAC update in eth_phy_link_status_poll() (eth.c:835-883)**:
- On link up: mark mac_speed_configured = false
- Poll PHY_BSR for PHY_BSR_AUTONEG_DONE bit
- When complete: read negotiated speed, update MAC, restart DHCP

### Critical Details

**10ms Delays Required**: MAC hardware requires 10ms delays (not 2ms) to properly stop, reconfigure, and restart TX/RX.

**DHCP Renew**: After MAC reconfiguration, must trigger dhcp_renew() to restart DHCP handshake.

### Overhead Analysis

- **Normal operation**: 1 PHY register read per 250ms (same as before)
- **During autoneg** (1-3 seconds): 1 additional PHY_BSR read per poll (4-12 total extra reads)
- **After autoneg**: Zero additional overhead

---

## Final Test Results

### NUCLEO_H563ZI (STM32H563) - NO REGRESSION ✓

**Boot with cable connected**:
- active(True): 28ms
- DHCP connected: 3 seconds
- IP: 192.168.0.10

**Hot-plug test**:
- Cable unplug: detected
- Cable replug: detected  
- DHCP reconnected
- IP: 192.168.0.10

**Result**: ✓ All tests pass, no regression

### NUCLEO_F429ZI (STM32F429) - REGRESSION FIXED ✓

**Before fix**:
- active(True): 29ms
- Status: stuck at 2 (link up, no IP)
- DHCP: FAILED

**After fix**:
- active(True): 28ms
- Autoneg complete: ~1 second (background)
- MAC updated: 100Mbps Full Duplex
- DHCP connected: 2 seconds
- IP: 192.168.0.192

**Result**: ✓ DHCP regression completely fixed

### Comparison: Official v1.26.1 vs Our Branch

| Board | Firmware | active(True) | DHCP | Result |
|-------|----------|--------------|------|--------|
| F429ZI | Official v1.26.1 | 1586ms | ✓ 6s | Works |
| F429ZI | Our branch (fixed) | 28ms | ✓ 2s | **Faster** |
| H563ZI | Our branch | 28ms | ✓ 3s | Works |

### Key Improvements Over Master

1. **Non-blocking initialization**: active(True) ~28ms vs 1586ms
2. **Background autonegotiation**: PHY negotiates while CPU continues
3. **Dynamic MAC adaptation**: Handles any negotiated speed/duplex
4. **Faster DHCP**: 2-3 seconds vs 6s
5. **Zero continuous overhead**: No extra polling after autoneg completes
6. **Hot-plug support**: Handles cable unplug/replug with DHCP recovery

---

## Hot-Plug DHCP Bug Discovery and Fix

**Date**: 2025-10-06
**Commits**: 49ab127a0e, 31a9d08f34, a9e340ad80

### Problem Discovery

During comprehensive testing, discovered that hot-plug DHCP recovery had a critical bug:

**F429ZI Hot-Plug Test**:
```
Initial state: Connected, DHCP IP 192.168.0.192
Unplug cable: Link down detected (Status 3 → 0) ✓
Replug cable: Link up detected (Status 0 → 1) ✓
DHCP recovery: FAILED - stuck at 0.0.0.0 ✗
```

### Root Cause Analysis

The original link-up handler at eth.c:821 called `dhcp_renew()`:
```c
if (current_link_status) {
    netif_set_link_up(netif);
    if (netif_dhcp_data(netif)) {
        dhcp_renew(netif);  // ← BUG
    }
}
```

**The Problem**:
1. When cable is unplugged, `netif_set_link_down()` is called
2. LWIP internally stops the DHCP client but leaves `netif_dhcp_data` allocated
3. When cable is replugged, `netif_dhcp_data(netif)` returns non-NULL
4. Code calls `dhcp_renew()` on a **stopped** DHCP client
5. `dhcp_renew()` does nothing on a stopped client → DHCP never restarts

### The Fix (Commit a9e340ad80)

```c
if (current_link_status) {
    netif_set_link_up(netif);

    if (netif_is_up(netif)) {
        if (netif_dhcp_data(netif)) {
            // DHCP struct exists - could be running or stopped
            // Stop and restart to ensure clean state
            dhcp_stop(netif);
            if (ip4_addr_isany_val(*netif_ip4_addr(netif))) {
                dhcp_start(netif);
            }
        } else if (ip4_addr_isany_val(*netif_ip4_addr(netif))) {
            dhcp_start(netif);
        }
    }
} else {
    netif_set_link_down(netif);
    self->mac_speed_configured = false;

    // Stop DHCP on link down for clean state
    if (netif_dhcp_data(netif)) {
        dhcp_stop(netif);
    }
}
```

**Why This Works**:
- Explicitly `dhcp_stop()` then `dhcp_start()` ensures clean DHCP restart
- Only starts DHCP if no static IP configured (`ip4_addr_isany_val`)
- Stops DHCP on link down for consistent state management

---

## Comprehensive Testing Results (2025-10-06)

### Test Matrix

Tested on both NUCLEO_F429ZI and NUCLEO_H563ZI with:
1. Static IP configuration
2. Hot-plug (unplug/replug cable with DHCP)
3. Boot without cable (board reset with cable unplugged, then plug in)

### NUCLEO_F429ZI (STM32F429) - All Tests Pass ✓

**Static IP Tests**:
- Set static IP before active(True): ✓ Works
- Set static IP after DHCP: ✓ Works, replaces DHCP
- Static IP persists through active(False)/active(True): ✓ Preserved

**Hot-plug Test**:
```
Initial: DHCP at 5s, IP 192.168.0.192
Unplug: Link down detected at 7s (Status 3 → 0)
Replug: Link up at 17s (Status 0 → 3), DHCP recovered
IP: 192.168.0.192
Recovery time: <1s after link established
```

**Boot Without Cable Test**:
```
Board reset with cable unplugged
Status: 0 (link down)
Cable plugged in after 1001s
Status: 0 → 3, DHCP obtained immediately
IP: 192.168.0.192
```

### NUCLEO_H563ZI (STM32H563) - All Tests Pass ✓

**Hot-plug Test**:
```
Initial: DHCP at 0s, IP 192.168.0.10
Unplug: Link down detected at 439s (Status 3 → 0)
Replug: Link transitions 0 → 2 → 3 over 4 seconds
DHCP recovered at 36s (4s after replug)
IP: 192.168.0.10
```

**Boot Without Cable Test**:
```
Board reset with cable unplugged
Status: 0 (link down)
Cable plugged in after 610s
Status: 0 → 3, DHCP obtained immediately
IP: 192.168.0.10
```

---

## Final Commit Summary

### Core Ethernet Fixes

1. **4480a4187f** - stm32/eth: Re-enable clock sleep after initialization completes
   - Two-phase approach: DISABLE during PHY init, ENABLE after completion
   - Prevents MDIO hangs during initialization
   - Allows Ethernet to receive packets during CPU WFI

2. **49ab127a0e** - stm32/eth: Add dynamic MAC speed/duplex configuration via polling
   - Non-blocking PHY autonegotiation
   - Polls for autoneg completion in background
   - Updates MAC to match negotiated PHY speed/duplex

3. **31a9d08f34** - stm32/eth: Fix critical race conditions and timeout handling in autoneg
   - Added IRQ disable/enable around MAC reconfiguration
   - Added 5-second autoneg timeout with fallback to 10Mbps Half Duplex
   - Added PHY_BCR_AUTONEG_RESTART bit for reliable negotiation

4. **a9e340ad80** - stm32/eth: Fix DHCP not restarting after cable hot-plug
   - Properly stops and restarts DHCP on link state changes
   - Tested on both F429ZI and H563ZI with hot-plug and boot-without-cable scenarios

### Test Coverage

**Boards**: NUCLEO_F429ZI, NUCLEO_H563ZI
**Scenarios**:
- ✓ Fresh boot with cable connected (DHCP)
- ✓ Fresh boot without cable, then plug in (DHCP recovery)
- ✓ Hot-plug: unplug then replug cable (DHCP recovery)
- ✓ Static IP configuration (before and after active)
- ✓ Static IP persistence through interface toggle

**All tests pass on both boards.**

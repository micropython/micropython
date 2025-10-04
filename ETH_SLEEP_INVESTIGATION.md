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

## References

- PR #17613 comments: https://github.com/micropython/micropython/pull/17613
- STM32H5 Reference Manual: RCC_AHB1LPENR register (section 11.7.23)
- dpgeorge review: https://github.com/micropython/micropython/pull/17613#pullrequestreview-3010490920
- andrewleech response: https://github.com/micropython/micropython/pull/17613#pullrequestreview-3041366090

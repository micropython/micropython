# STM32 Ethernet Clock Sleep Configuration Investigation

**Date**: 2025-10-07
**Branch**: stm32_eth
**Board**: NUCLEO_H563ZI (STM32H563ZI)
**Issue**: Active() hanging and DHCP not working

---

## Background

The STM32 Ethernet driver uses RCC Low Power Enable Register (LPENR) to control whether peripheral clocks continue running when the CPU enters sleep mode (WFI instruction).

### Register Behavior
- `__HAL_RCC_ETH_CLK_SLEEP_ENABLE()`: Clock **continues running** during sleep
- `__HAL_RCC_ETH_CLK_SLEEP_DISABLE()`: Clock **stops** during sleep

### Original Issue (PR #17613 Comments)
- On H5 board, specific clock sleep configurations prevented `.active(True)` from working when ethernet cable was connected
- dpgeorge suggested: "probably it's OK to just disable clock-sleep altogether, because we do want ETH to run 'in the background' when the CPU does WFI"

---

## Investigation and Solution

### The Incorrect Two-Phase Theory

Initial testing led to an incorrect theory that a two-phase approach was needed:
1. Phase 1: `DISABLE` during initialization (to prevent MDIO hangs)
2. Phase 2: `ENABLE` after soft reset (to allow network operation)

This theory was based on observing that:
- Simple `DISABLE` only → DHCP stopped working after reset
- Two-phase `DISABLE` → soft reset → `ENABLE` → seemed to work

### Discovery of the Logical Error

Code review revealed a critical logical inconsistency in the two-phase approach:

**The problematic sequence was:**
```c
__HAL_RCC_ETH_CLK_SLEEP_DISABLE();  // Clock STOPS during WFI
ETH_SOFT_RESET(ETH);                 // Trigger soft reset
mp_hal_delay_ms(2);                  // May enter WFI → clock STOPS
while (ETH_IS_RESET(ETH)) {...}      // May enter WFI → clock STOPS
__HAL_RCC_ETH_CLK_SLEEP_ENABLE();    // Clock continues
```

**The problem:**
- `mp_hal_delay_ms()` calls `MICROPY_EVENT_POLL_HOOK` which may enter WFI
- If CPU enters WFI during the 2ms delay or reset wait loop, ETH clock would STOP
- This should break the soft reset operation

### Testing the Simple Solution

Testing revealed the simple approach works correctly:

**Correct configuration:**
```c
__HAL_RCC_ETH_RELEASE_RESET();       // Release from reset
__HAL_RCC_ETH_CLK_SLEEP_ENABLE();    // Clock CONTINUES during WFI
// ... (MAC soft reset and all other operations)
// No need to toggle clock sleep settings
```

**Why this works:**
- Clock continues during any WFI that occurs during soft reset delays
- ETH peripheral can receive packets and generate interrupts during CPU sleep
- DHCP and other network traffic functions correctly
- Hot-plug detection works correctly

---

## Final Solution

### Code Structure
```c
static int eth_mac_init(eth_t *self) {
    // ...

    // Release ETH peripheral from reset and enable clock during sleep
    #if defined(STM32H5)
    __HAL_RCC_ETH_RELEASE_RESET();
    __HAL_RCC_ETH_CLK_SLEEP_ENABLE();
    __HAL_RCC_ETHTX_CLK_SLEEP_ENABLE();
    __HAL_RCC_ETHRX_CLK_SLEEP_ENABLE();
    #elif defined(STM32H7)
    __HAL_RCC_ETH1MAC_RELEASE_RESET();
    __HAL_RCC_ETH1MAC_CLK_SLEEP_ENABLE();
    __HAL_RCC_ETH1TX_CLK_SLEEP_ENABLE();
    __HAL_RCC_ETH1RX_CLK_SLEEP_ENABLE();
    #else
    __HAL_RCC_ETHMAC_RELEASE_RESET();
    __HAL_RCC_ETHMAC_CLK_SLEEP_ENABLE();
    __HAL_RCC_ETHMACTX_CLK_SLEEP_ENABLE();
    __HAL_RCC_ETHMACRX_CLK_SLEEP_ENABLE();
    #endif

    // Do MAC soft reset (safe with clock enabled during sleep)
    ETH_SOFT_RESET(ETH);
    mp_hal_delay_ms(2);
    while (ETH_IS_RESET(ETH)) {...}

    // Continue with MAC configuration...
}
```

### Why the Two-Phase Theory Was Wrong

The two-phase approach appeared to work initially, but likely for different reasons than theorized:
- The soft reset probably completes very quickly (< 1ms)
- The 2ms delay and wait loop might not always trigger WFI
- Network traffic worked because of the final `ENABLE`, not because of any supposed initialization protection

The simple `ENABLE` approach is both logically correct and empirically validated.

---

## Test Results on NUCLEO_H563ZI

### Test 1: Basic DHCP
```
Status: 3 (connected)
Config: ('192.168.0.10', '255.255.255.0', '192.168.0.1', '192.168.0.1')
Result: ✓ PASSED
```

### Test 2: Hot-Plug (Unplug and Replug Cable)
```
[0-80s]:  Status 3 (connected, IP: 192.168.0.10)
[81-89s]: Status 0 (cable unplugged - detected)
[90s+]:   Status 3 (cable replugged - DHCP recovered)
Result: ✓ PASSED
```

### Test 3: Boot Without Cable, Then Plug In
```
[0-9s]:  Status 0 (cable unplugged, no link)
[10s+]:  Status 3 (cable plugged, DHCP obtained IP: 192.168.0.10)
Result: ✓ PASSED
```

---

## Related Fixes in This Branch

### 1. F429ZI DHCP Regression Fix
**Issue**: DHCP stopped working on F429ZI after clock sleep changes.

**Root Cause**: Autonegotiation and MAC speed/duplex configuration issues.

**Fix**: Added dynamic MAC speed/duplex reconfiguration with proper autonegotiation timeout and race condition protection.

**Commits**:
- `49ab127a0e` - stm32/eth: Add dynamic MAC speed/duplex configuration via polling
- `31a9d08f34` - stm32/eth: Fix critical race conditions and timeout handling in autoneg

### 2. Hot-Plug DHCP Restart Bug
**Issue**: After unplugging and replugging cable, DHCP would not restart.

**Root Cause**: Code called `dhcp_renew()` on a stopped DHCP client. LWIP's `netif_set_link_down()` stops DHCP but leaves the struct allocated. On replug, `netif_dhcp_data(netif)` returns non-NULL, but `dhcp_renew()` does nothing on a stopped client.

**Fix**: Explicitly call `dhcp_stop()` then `dhcp_start()` on link up, respecting static IP configuration.

**LWIP API Investigation**:
- `dhcp_renew()` is for renewing active leases, not restarting stopped DHCP
- `dhcp_network_changed_link_up()` is automatically called by `netif_set_link_up()` but explicitly doesn't restart OFF clients
- Application code is responsible for calling `dhcp_start()` when appropriate

**Commits**:
- `a9e340ad80` - stm32/eth: Fix DHCP not restarting after cable hot-plug
- `596d29acab` - stm32/eth: Simplify DHCP restart logic on cable hot-plug

### 3. Clock Sleep Configuration Fix
**Issue**: Incorrect two-phase DISABLE→ENABLE approach based on faulty theory.

**Fix**: Simple ENABLE-only approach allowing clocks to continue during sleep for all operations.

**Commit**:
- `681e9b617b` - stm32/eth: Enable ETH clock during sleep for all operation

---

## Commit History Summary

| Commit | Description |
|--------|-------------|
| `49ab127a0e` | Add dynamic MAC speed/duplex configuration via polling |
| `31a9d08f34` | Fix critical race conditions and timeout handling in autoneg |
| `a9e340ad80` | Fix DHCP not restarting after cable hot-plug |
| `96667d7f96` | Update investigation docs with hot-plug DHCP fix and F429ZI testing |
| `596d29acab` | Simplify DHCP restart logic on cable hot-plug |
| `681e9b617b` | Enable ETH clock during sleep for all operation |

---

## Verification Checklist

- [x] No active() hang with cable connected
- [x] DHCP acquisition after fresh boot
- [x] DHCP acquisition after soft reset
- [x] Stop/restart cycle works
- [x] Cable hot-plug detection and DHCP recovery
- [x] Boot without cable, then plug in
- [x] Static IP configuration respected
- [x] Consistent across all STM32 variants (H5, H7, F4)
- [x] Tested on NUCLEO_H563ZI (H5)
- [ ] Tested on additional H7 boards
- [ ] Tested on additional F4 boards

---

## Lessons Learned

1. **Semantic clarity matters**: The meaning of ENABLE/DISABLE was initially misunderstood. Always verify register semantics from HAL documentation.

2. **Question theories that don't make logical sense**: The two-phase approach had a logical flaw (DISABLE during delays/WFI) that should have been caught earlier.

3. **Empirical testing validates logic**: Code that passes tests but has logical inconsistencies should be re-examined.

4. **API usage matters**: Using the wrong LWIP API (`dhcp_renew()` vs `dhcp_start()`) caused subtle bugs that only appeared in specific scenarios.

5. **Simple is better**: The final solution is simpler and more maintainable than the incorrect two-phase approach.

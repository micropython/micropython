# ESP32 Port - Critical Bug Fixes

## Summary
Fixed 3 critical bugs in ESP32 port that could cause crashes, memory corruption, and undefined behavior.

---

## Bug 1: Unchecked Task Creation in Main Entry Point
**File:** `ports/esp32/main.c:276`
**Severity:** CRITICAL
**Type:** Resource allocation failure, silent failure

### Vulnerability
The main MicroPython task creation was not checking the return value from `xTaskCreatePinnedToCore()`. If task creation failed (e.g., out of memory), the system would continue without the MicroPython runtime, leading to undefined behavior.

### Vulnerable Code
```c
// BEFORE (line 276):
xTaskCreatePinnedToCore(mp_task, "mp_task", MICROPY_TASK_STACK_SIZE / sizeof(StackType_t),
    NULL, MP_TASK_PRIORITY, &mp_main_task_handle, MP_TASK_COREID);
```

### Fixed Code
```c
// AFTER (lines 276-280):
BaseType_t res = xTaskCreatePinnedToCore(mp_task, "mp_task", MICROPY_TASK_STACK_SIZE / sizeof(StackType_t),
    NULL, MP_TASK_PRIORITY, &mp_main_task_handle, MP_TASK_COREID);
if (res != pdPASS) {
    printf("FATAL: Failed to create MicroPython task\n");
    abort();
}
```

### Impact
- **Before:** Silent failure, undefined behavior, potential system crash
- **After:** Explicit error message and controlled abort if task creation fails

---

## Bug 2: Unchecked Queue Creation in I2S Module
**File:** `ports/esp32/machine_i2s.c:446`
**Severity:** CRITICAL
**Type:** NULL pointer dereference risk

### Vulnerability
The I2S module creates a FreeRTOS queue for non-blocking mode without checking if `xQueueCreate()` returned NULL. If queue creation failed, the NULL pointer would be used later, causing a crash.

### Vulnerable Code
```c
// BEFORE (line 446):
self->non_blocking_mode_queue = xQueueCreate(1, sizeof(non_blocking_descriptor_t));

// non-blocking mode requires a background FreeRTOS task
if (xTaskCreatePinnedToCore(...)) {
    ...
}
```

### Fixed Code
```c
// AFTER (lines 446-449):
self->non_blocking_mode_queue = xQueueCreate(1, sizeof(non_blocking_descriptor_t));
if (self->non_blocking_mode_queue == NULL) {
    mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to create I2S queue"));
}
```

### Impact
- **Before:** Potential NULL pointer dereference → crash when using I2S non-blocking mode
- **After:** Proper error handling with clear exception message

---

## Bug 3: Unchecked Memory Allocation in WiFi Scan
**File:** `ports/esp32/network_wlan.c:451`
**Severity:** CRITICAL
**Type:** NULL pointer dereference, memory allocation failure

### Vulnerability
WiFi scan results used `calloc()` to allocate an array of access point records without checking for NULL. The unchecked pointer was then passed to `esp_wifi_scan_get_ap_records()`, which could crash or corrupt memory.

### Vulnerable Code
```c
// BEFORE (lines 451-452):
wifi_ap_record_t *wifi_ap_records = calloc(count, sizeof(wifi_ap_record_t));
esp_exceptions(esp_wifi_scan_get_ap_records(&count, wifi_ap_records));
```

### Fixed Code
```c
// AFTER (lines 451-455):
wifi_ap_record_t *wifi_ap_records = calloc(count, sizeof(wifi_ap_record_t));
if (wifi_ap_records == NULL) {
    mp_raise_OSError(MP_ENOMEM);
}
esp_exceptions(esp_wifi_scan_get_ap_records(&count, wifi_ap_records));
```

### Impact
- **Before:** NULL pointer passed to ESP-IDF API → crash or undefined behavior
- **After:** Proper memory allocation check with ENOMEM exception

---

## Testing Recommendations

### 1. Stress Testing
- Test under low memory conditions
- Force FreeRTOS task/queue creation failures
- Verify proper error handling

### 2. WiFi Scan Testing
```python
import network
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
# Test multiple rapid scans
for i in range(100):
    results = wlan.scan()
    print(f"Scan {i}: {len(results)} APs found")
```

### 3. I2S Testing
```python
from machine import I2S, Pin
# Test non-blocking I2S mode under various conditions
i2s = I2S(0, sck=Pin(14), ws=Pin(13), sd=Pin(12), mode=I2S.TX,
          bits=16, format=I2S.MONO, rate=22050, ibuf=20000)
```

---

## Prevention Recommendations

1. **Always check return values** from:
   - FreeRTOS APIs: `xTaskCreate*`, `xQueueCreate`, `xSemaphoreCreate*`
   - Memory allocation: `malloc()`, `calloc()`, `realloc()`
   - ESP-IDF APIs that return error codes

2. **Use static analysis tools:**
   - Enable GCC/Clang warnings: `-Wunused-result`
   - Run Coverity or similar static analyzers

3. **Code review checklist:**
   - [ ] All task creations check return value
   - [ ] All queue/semaphore creations check for NULL
   - [ ] All memory allocations check for NULL
   - [ ] Error paths properly clean up resources

---

## References
- CWE-252: Unchecked Return Value
- CWE-476: NULL Pointer Dereference
- CWE-690: Unchecked Return Value to NULL Pointer Dereference
- FreeRTOS API Documentation: https://www.freertos.org/a00106.html
- ESP-IDF API Reference: https://docs.espressif.com/projects/esp-idf/

---

## Summary Statistics
- **Files Modified:** 3
- **Lines Changed:** +12
- **Bugs Fixed:** 3 (all CRITICAL severity)
- **Potential Crashes Prevented:** 3+
- **NULL Pointer Dereferences Fixed:** 2
- **Silent Failures Fixed:** 1

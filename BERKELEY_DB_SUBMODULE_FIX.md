# Berkeley DB Submodule Security Fix

## Issue
The Berkeley DB library (in `lib/berkeley-db-1.xx`) is included as a git submodule and cannot be directly modified from the main MicroPython repository.

However, a HIGH severity buffer overflow vulnerability was discovered in:
**File:** `lib/berkeley-db-1.xx/hash/ndbm.c` lines 70-71

## Vulnerability Details
**Type:** Buffer Overflow (CWE-120)
**Severity:** HIGH
**Impact:** Crash or memory corruption with long database filenames

### Vulnerable Code
```c
char path[MAXPATHLEN];
...
(void)strcpy(path, file);
(void)strcat(path, DBM_SUFFIX);
```

The code uses unsafe `strcpy()` and `strcat()` without bounds checking. If the `file` parameter length + `DBM_SUFFIX` length exceeds `MAXPATHLEN`, a buffer overflow occurs.

## Recommended Fix
Replace unsafe string operations with bounded `snprintf()`:

```c
char path[MAXPATHLEN];
...
// Use snprintf instead of strcpy/strcat for bounds checking
snprintf(path, sizeof(path), "%s%s", file, DBM_SUFFIX);
```

## Application Instructions

### Option 1: Apply Patch
A patch file has been provided: `berkeley-db-ndbm-security-fix.patch`

```bash
cd lib/berkeley-db-1.xx
git apply ../../berkeley-db-ndbm-security-fix.patch
```

### Option 2: Manual Edit
Edit `lib/berkeley-db-1.xx/hash/ndbm.c` and replace lines 70-71:

**Before:**
```c
(void)strcpy(path, file);
(void)strcat(path, DBM_SUFFIX);
```

**After:**
```c
// Use snprintf instead of strcpy/strcat for bounds checking
snprintf(path, sizeof(path), "%s%s", file, DBM_SUFFIX);
```

### Option 3: Fork and Update Submodule
1. Fork the Berkeley DB repository
2. Apply the security fix
3. Update MicroPython's submodule reference to point to your fixed fork

## Testing
After applying the fix, test with long database filenames:

```python
import btree

# Test with various filename lengths
for i in range(1, 300):
    try:
        filename = "x" * i + ".db"
        db = btree.open(filename)
        db.close()
    except Exception as e:
        print(f"Failed at length {i}: {e}")
```

## Status
- **Discovered:** 2025-11-20
- **Documented:** BUG_FIXES_SESSION2.md (Bug #5)
- **Patch Created:** berkeley-db-ndbm-security-fix.patch
- **Applied to MicroPython:** ❌ (submodule - requires manual application)

## References
- Main bug report: BUG_FIXES_SESSION2.md
- CWE-120: Buffer Copy without Checking Size of Input
- OWASP: Buffer Overflow Prevention

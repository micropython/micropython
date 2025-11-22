# Quick Reference: 20 Upstream Bug Fixes

**See full analysis:** `UPSTREAM_BUG_ANALYSIS_20_FIXES.md`

---

## CRITICAL Bugs (8) - Fix First

| # | Issue | Title | File | Lines |
|---|-------|-------|------|-------|
| 1 | #18170 | list.sort() mutation corruption | py/objlist.c | ~40 |
| 2 | #17941 | Array sorting GC crash | py/objlist.c | ~40 |
| 3 | #18168 | Bytearray memoryview corruption | py/objarray.c | ~50 |
| 4 | #18171 | Array memoryview readinto crash | py/objarray.c | ~50 |
| 5 | #17925 | String multiplication crash | py/objstr.c | ~15 |
| 6 | #17855 | UTF-8 exception crash | py/objexcept.c | ~25 |
| 7 | #17848 | VFS buffer enlargement crash | extmod/vfs_blockdev.c | ~20 |
| 8 | #17728 | super() segfault | py/objtype.c | ~10 |

**Total Lines:** ~250

---

## HIGH Severity Bugs (7) - Fix Second

| # | Issue | Title | File | Lines |
|---|-------|-------|------|-------|
| 9 | #18167 | uctypes.bytes_at segfault | extmod/moductypes.c | ~15 |
| 10 | #18166 | uctypes.bytearray_at segfault | extmod/moductypes.c | ~15 |
| 11 | #18172 | bytearray uctypes segfault | extmod/moductypes.c | ~0 |
| 12 | #18169 | Viper ptr8(0) segfault | py/emitnative.c | ~40 |
| 13 | #18144 | Range integer overflow | py/objrange.c | ~30 |
| 14 | #17818 | Websocket corrupt data crash | extmod/modwebsocket.c | ~5 |
| 15 | #17727 | BufferedWriter invalid stream | extmod/modio.c | ~8 |

**Total Lines:** ~113

---

## MEDIUM Severity Bugs (5) - Fix Last

| # | Issue | Title | File | Lines |
|---|-------|-------|------|-------|
| 16 | #17819 | socket.socket() assertion | extmod/modusocket.c | ~5 |
| 17 | #17817 | compile() assertion | py/emitbc.c | ~10 |
| 18 | #17722 | Syntax assertion | py/compile.c | ~10 |
| 19 | #17720 | Regex stack overflow | lib/re1.5/ | ~20 |
| 20 | #17726 | uctypes sizeof incorrect | extmod/moductypes.c | ~25 |

**Total Lines:** ~70

---

## Summary by Component

### Core Python (py/)
- 12 bugs
- ~200 lines of fixes
- Files: objlist.c, objarray.c, objstr.c, objexcept.c, objtype.c, objrange.c, emitnative.c, emitbc.c, compile.c

### Extension Modules (extmod/)
- 8 bugs
- ~103 lines of fixes
- Files: moductypes.c, vfs_blockdev.c, modwebsocket.c, modio.c, modusocket.c

### Total Estimated Changes
- **20 bugs**
- **~433 lines of code changes**
- **~500 lines including tests**

---

## Fix Phases

### Phase 1: Quick Wins (1-2 days)
Issues #16-20 - Simple validation fixes

### Phase 2: Memory Safety (3-4 days)
Issues #9-15 - Address validation, type checks

### Phase 3: Core Algorithms (4-5 days)
Issues #1-8 - Mutation guards, export tracking

### Phase 4: Testing (2-3 days)
Integration testing, fuzzing, documentation

**Total:** 10-14 days

---

## Files to Modify (9 core files)

1. **py/objlist.c** - Bugs #1, #2 (sorting mutation)
2. **py/objarray.c** - Bugs #3, #4, #11 (memoryview exports)
3. **py/objstr.c** - Bug #5 (string multiplication)
4. **py/objexcept.c** - Bug #6 (UTF-8 validation)
5. **py/objtype.c** - Bug #8 (super validation)
6. **py/objrange.c** - Bug #13 (range overflow)
7. **extmod/moductypes.c** - Bugs #9, #10, #11, #20 (uctypes)
8. **extmod/vfs_blockdev.c** - Bug #7 (buffer validation)
9. **extmod/modwebsocket.c** - Bug #14 (frame validation)

Plus 5 minor files (modio.c, modusocket.c, emitnative.c, emitbc.c, compile.c, re1.5/)

---

## Verification Checklist

For each bug fix:
- [ ] Reproducer test case created
- [ ] Fix implemented and tested
- [ ] Regression tests pass
- [ ] No performance degradation
- [ ] Documentation updated
- [ ] Upstream PR prepared

---

## Links

- Full Analysis: `UPSTREAM_BUG_ANALYSIS_20_FIXES.md` (954 lines)
- Upstream Issues: https://github.com/micropython/micropython/issues
- Our Existing Fixes: `RELEASE_NOTES_v1.27.1.md`

---

**Last Updated:** 2025-11-22

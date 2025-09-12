# Native Module Overhead Analysis

This document analyzes the overhead of different approaches to implementing native modules in MicroPython.

## Test Modules

Three equivalent native modules were created with the same functionality:
- `double(x)` - Returns x * 2
- `add(a, b)` - Returns a + b
- `message()` - Returns a string
- `MAGIC_NUMBER` - Constant value 42

### 1. Traditional Approach (`static_module_traditional`)
Standard native module implementation with manual QSTR definitions and direct init function.

### 2. Hybrid Macro Approach (`static_module_hybrid`)
Uses macros to reduce boilerplate in the init function while maintaining compatibility.

### 3. Auto-generation Approach (`static_module_autogen`)
Demonstrates how code generation could be used to automate module registration.

## Size Comparison

### MPY File Sizes
- Traditional: 457 bytes
- Hybrid: 447 bytes 
- Auto-gen: 449 bytes

### Object File Sizes (.o)
- Traditional: 2832 bytes
- Hybrid: 2904 bytes (+72 bytes, +2.5%)
- Auto-gen: 2832 bytes (same as traditional)

### Text Section Sizes (from linker output)
All three modules report identical text size: 320 bytes

## Code Analysis

### Traditional vs Hybrid
The hybrid approach adds a `_qstr_refs()` function to ensure QSTRs are included in the build. However, the compiler optimizes this to a single `ret` instruction (1 byte).

The main overhead comes from:
1. The `_qstr_refs` function symbol and its minimal body
2. Slightly different code alignment

The generated machine code for `mpy_init` is otherwise identical between approaches.

### Macro Benefits
The hybrid macro approach provides:
- Cleaner, more maintainable code
- Less repetition in the init function
- Same runtime performance
- Minimal size overhead (~2.5% in object file, smaller in final MPY)

### Auto-generation Considerations
The auto-generation approach can achieve:
- Zero runtime overhead (identical to traditional)
- Build-time complexity for code generation
- Potential for more sophisticated module definitions
- Better scalability for large modules

## Runtime Performance

All three approaches generate identical machine code for:
- Function implementations
- Module initialization sequence
- Function dispatch

The only difference is a single unused function in the hybrid approach that adds no runtime overhead.

## Recommendations

1. **For Simple Modules**: The traditional approach is straightforward and has minimal overhead.

2. **For Larger Modules**: The hybrid macro approach offers better maintainability with negligible overhead (~10 bytes in the final MPY).

3. **For Complex Projects**: Auto-generation can eliminate boilerplate entirely but requires build system integration.

## Conclusion

The overhead of macro-based or auto-generation approaches for native modules is minimal:
- Size impact: <3% increase in object file, <2% in final MPY
- Runtime impact: Zero (identical machine code)
- Maintainability: Significantly improved

The benefits of reduced boilerplate and improved maintainability outweigh the minimal size overhead for most use cases.
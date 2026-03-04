# Test that exception handling correctly identifies strings in all heap areas
# when MICROPY_GC_SPLIT_HEAP is enabled. This validates the fix for issue #17855.
#
# This test requires the unix coverage port built with MICROPY_GC_SPLIT_HEAP_N_HEAPS=4
# to properly exercise the multiple heap scenario.

# Check if we have gc module
try:
    import gc

    # Try to get GC info which should work on all ports
    gc.collect()
    print("GC available")
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


# Test 1: Create a string starting with 0xff (compression marker)
# This should NOT be decompressed as it's a heap-allocated string
def test_heap_string_with_compression_marker():
    # Force garbage collection to clear memory
    gc.collect()

    # Create a string that starts with 0xff (the compression marker)
    # We use eval to avoid the string being optimized into ROM
    test_str = eval("'\\xff" + "test string with compression marker" + "'")

    # Verify the string starts with 0xff
    if ord(test_str[0]) != 0xFF:
        print("ERROR: Test string doesn't start with 0xff")
        return False

    # Now raise an exception with this string
    # If the heap check is broken, this could crash trying to decompress garbage
    try:
        raise ValueError(test_str)
    except ValueError as e:
        # The exception message should be our original string, not decompressed garbage
        caught_str = str(e)
        if caught_str == test_str:
            print("PASS: String with 0xff marker in heap preserved correctly")
            return True
        else:
            print(
                "ERROR: String mismatch. Expected %s, got %s" % (repr(test_str), repr(caught_str))
            )
            return False


# Test 2: Fill heap to force allocation into later heap areas (when split heap is enabled)
def test_with_heap_pressure():
    gc.collect()

    # Allocate a bunch of objects to fill the first heap area(s)
    # This increases the chance that our test string will be in a non-first heap
    blocker = []
    for i in range(100):
        # Create various sized objects
        blocker.append(b"x" * (i * 10 + 100))
        blocker.append([i] * 20)
        blocker.append({"key": i, "data": "filler" * 10})

    # Now create our test string with compression marker
    test_str = eval("'\\xff" + "X" * 50 + "'")

    try:
        raise RuntimeError(test_str)
    except RuntimeError as e:
        if str(e) == test_str:
            print("PASS: String preserved under heap pressure")
            return True
        else:
            print("ERROR: String corrupted under heap pressure")
            return False


# Test 3: Verify that actual compressed ROM strings still work
def test_real_rom_exception():
    # Built-in exceptions use compressed ROM strings for their messages
    try:
        # This will raise a built-in exception with a ROM string message
        [].append(1, 2)  # Wrong number of arguments
    except TypeError as e:
        # The error message should be properly decompressed
        msg = str(e)
        # Just verify we got a message (content varies by build config)
        if len(msg) > 0:
            print("PASS: ROM exception messages still work")
            return True
        else:
            print("ERROR: ROM exception has empty message")
            return False


# Test 4: Test exception chaining with heap strings
def test_exception_chaining():
    test_str1 = eval("'\\xff" + "first exception" + "'")
    test_str2 = eval("'\\xff" + "second exception" + "'")

    try:
        try:
            raise ValueError(test_str1)
        except ValueError:
            raise RuntimeError(test_str2)
    except RuntimeError as e:
        if str(e) == test_str2:
            print("PASS: Exception chaining with heap strings works")
            return True
        else:
            print("ERROR: Exception chaining failed")
            return False


# Run all tests
if __name__ == "__main__":
    results = []

    results.append(test_heap_string_with_compression_marker())
    results.append(test_with_heap_pressure())
    results.append(test_real_rom_exception())
    results.append(test_exception_chaining())

    if all(results):
        print("All tests passed")
    else:
        print("Some tests failed")

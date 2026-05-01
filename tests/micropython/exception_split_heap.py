# Test that exception handling correctly identifies strings in all heap areas
# when MICROPY_GC_SPLIT_HEAP is enabled. This validates the fix for issue #17855.
#
# This test requires the unix coverage port built with MICROPY_GC_SPLIT_HEAP_N_HEAPS=4
# to properly exercise the multiple heap scenario.

try:
    import gc

    gc.collect()
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

import unittest


class TestExceptionSplitHeap(unittest.TestCase):
    def test_heap_string_with_compression_marker(self):
        # Create a string starting with 0xff (compression marker)
        # Use eval to avoid the string being optimized into ROM
        gc.collect()
        test_str = eval("'\\xff" + "test string with compression marker" + "'")
        self.assertEqual(ord(test_str[0]), 0xFF)

        # If the heap check is broken, this could crash trying to decompress garbage
        try:
            raise ValueError(test_str)
        except ValueError as e:
            self.assertEqual(str(e), test_str)

    def test_heap_pressure(self):
        # Fill heap to force allocation into later heap areas (when split heap is enabled)
        gc.collect()
        blocker = []
        for i in range(100):
            blocker.append(b"x" * (i * 10 + 100))
            blocker.append([i] * 20)
            blocker.append({"key": i, "data": "filler" * 10})

        test_str = eval("'\\xff" + "X" * 50 + "'")

        try:
            raise RuntimeError(test_str)
        except RuntimeError as e:
            self.assertEqual(str(e), test_str)

    def test_rom_exception_messages(self):
        # Verify that actual compressed ROM strings still work
        try:
            [].append(1, 2)  # Wrong number of arguments
        except TypeError as e:
            msg = str(e)
            self.assertTrue(len(msg) > 0)

    def test_exception_chaining(self):
        # Test exception chaining with heap strings
        test_str1 = eval("'\\xff" + "first exception" + "'")
        test_str2 = eval("'\\xff" + "second exception" + "'")

        try:
            try:
                raise ValueError(test_str1)
            except ValueError:
                raise RuntimeError(test_str2)
        except RuntimeError as e:
            self.assertEqual(str(e), test_str2)


if __name__ == "__main__":
    unittest.main()

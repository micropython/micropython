"""
Test script for dual-mode modules

This script tests the functionality of modules that can be compiled as both
user C modules and native modules.
"""

import sys

def test_simple_module():
    """Test the simple dual-mode module"""
    try:
        import dualmode_simple
    except ImportError:
        print("ERROR: Could not import dualmode_simple module")
        print("Make sure to either:")
        print("1. Build firmware with USER_C_MODULES=path/to/this/dir")
        print("2. Build native module: make -f Makefile.natmod")
        return False
    
    print("Testing dualmode_simple module...")
    
    # Test square function
    assert dualmode_simple.square(5) == 25
    assert dualmode_simple.square(-3) == 9
    print("✓ square() works correctly")
    
    # Test factorial function
    assert dualmode_simple.factorial(0) == 1
    assert dualmode_simple.factorial(5) == 120
    print("✓ factorial() works correctly")
    
    # Test error handling in factorial
    try:
        dualmode_simple.factorial(-1)
        assert False, "Should have raised ValueError"
    except ValueError:
        print("✓ factorial() raises ValueError for negative input")
    
    # Test concat function
    assert dualmode_simple.concat("Hello, ", "World!") == "Hello, World!"
    assert dualmode_simple.concat("Micro", "Python") == "MicroPython"
    print("✓ concat() works correctly")
    
    # Test constants
    assert dualmode_simple.PI == 31416
    assert dualmode_simple.E == 27183
    print("✓ Constants are accessible")
    
    print("All simple module tests passed!")
    return True


def test_example_module():
    """Test the more complex dual-mode module"""
    try:
        import dualmode_example
    except ImportError:
        print("ERROR: Could not import dualmode_example module")
        return False
    
    print("\nTesting dualmode_example module...")
    
    # Test add function
    assert dualmode_example.add(10, 20) == 30
    assert dualmode_example.add(-5, 5) == 0
    print("✓ add() works correctly")
    
    # Test multiply function with variable args
    assert dualmode_example.multiply() == 1
    assert dualmode_example.multiply(5) == 5
    assert dualmode_example.multiply(2, 3, 4) == 24
    print("✓ multiply() works correctly")
    
    # Test get_info function
    info = dualmode_example.get_info()
    assert isinstance(info, dict)
    assert 'version' in info
    assert 'mode' in info
    assert info['version'] == 100
    print(f"✓ get_info() returns: {info}")
    
    # Test Timer class
    timer = dualmode_example.Timer()
    print("✓ Timer() can be instantiated")
    
    # Test elapsed method
    import time
    elapsed1 = timer.elapsed()
    time.sleep(0.1)  # Sleep 100ms
    elapsed2 = timer.elapsed()
    assert elapsed2 > elapsed1
    print(f"✓ Timer.elapsed() works (elapsed: {elapsed2}ms)")
    
    # Test reset method
    timer.reset()
    elapsed3 = timer.elapsed()
    assert elapsed3 < elapsed2
    print("✓ Timer.reset() works")
    
    # Test module constants
    assert dualmode_example.VERSION == 100
    assert dualmode_example.MODE in ['native', 'firmware']
    print(f"✓ Module MODE: {dualmode_example.MODE}")
    
    print("All example module tests passed!")
    return True


def main():
    """Run all tests"""
    print("Dual-Mode Module Test Suite")
    print("=" * 40)
    
    # Test both modules
    simple_ok = test_simple_module()
    example_ok = test_example_module()
    
    print("\n" + "=" * 40)
    if simple_ok and example_ok:
        print("All tests PASSED!")
    else:
        print("Some tests FAILED!")
        sys.exit(1)


if __name__ == "__main__":
    main()
# Test for normalize_newlines functionality
# This test verifies that test framework handles various newline combinations correctly

# Note: This is more of an integration test since normalize_newlines is in the test framework
# The actual testing happens when this test is run through run-tests.py

print("Testing newline handling")
print("Line 1\r\nLine 2")  # Windows-style line ending - should be normalized
print("Line 3")            # Normal line
print("Line 4")            # Normal line
print("Line 5\nLine 6")    # Unix-style line ending - already normalized

# Test that literal \r in strings is preserved
print(repr("test\rstring"))  # Should show 'test\rstring' not 'test\nstring'

Unity Test API
==============

[![Unity Build Status](https://api.travis-ci.org/ThrowTheSwitch/Unity.png?branch=master)](https://travis-ci.org/ThrowTheSwitch/Unity)
__Copyright (c) 2007 - 2017 Unity Project by Mike Karlesky, Mark VanderVoord, and Greg Williams__

Running Tests
-------------

    RUN_TEST(func, linenum)

Each Test is run within the macro `RUN_TEST`.  This macro performs necessary setup before the test is called and handles cleanup and result tabulation afterwards.

Ignoring Tests
--------------

There are times when a test is incomplete or not valid for some reason.  At these times, TEST_IGNORE can be called.  Control will immediately be returned to the caller of the test, and no failures will be returned.

    TEST_IGNORE()

Ignore this test and return immediately

    TEST_IGNORE_MESSAGE (message)

Ignore this test and return immediately.  Output a message stating why the test was ignored.

Aborting Tests
--------------

There are times when a test will contain an infinite loop on error conditions, or there may be reason to escape from the test early without executing the rest of the test.  A pair of macros support this functionality in Unity.  The first `TEST_PROTECT` sets up the feature, and handles emergency abort cases. `TEST_ABORT` can then be used at any time within the tests to return to the last `TEST_PROTECT` call.

    TEST_PROTECT()

Setup and Catch macro

    TEST_ABORT()

Abort Test macro

Example:

    main()
    {
        if (TEST_PROTECT())
        {
            MyTest();
        }
    }

If MyTest calls `TEST_ABORT`, program control will immediately return to `TEST_PROTECT` with a return value of zero.


Unity Assertion Summary
=======================

Basic Validity Tests
--------------------

    TEST_ASSERT_TRUE(condition)

Evaluates whatever code is in condition and fails if it evaluates to false

    TEST_ASSERT_FALSE(condition)

Evaluates whatever code is in condition and fails if it evaluates to true

    TEST_ASSERT(condition)

Another way of calling `TEST_ASSERT_TRUE`

    TEST_ASSERT_UNLESS(condition)

Another way of calling `TEST_ASSERT_FALSE`

    TEST_FAIL()
    TEST_FAIL_MESSAGE(message)

This test is automatically marked as a failure.  The message is output stating why.

Numerical Assertions: Integers
------------------------------

    TEST_ASSERT_EQUAL_INT(expected, actual)
    TEST_ASSERT_EQUAL_INT8(expected, actual)
    TEST_ASSERT_EQUAL_INT16(expected, actual)
    TEST_ASSERT_EQUAL_INT32(expected, actual)
    TEST_ASSERT_EQUAL_INT64(expected, actual)

Compare two integers for equality and display errors as signed integers. A cast will be performed
to your natural integer size so often this can just be used.  When you need to specify the exact size,
like when comparing arrays, you can use a specific version:

    TEST_ASSERT_EQUAL_UINT(expected, actual)
    TEST_ASSERT_EQUAL_UINT8(expected, actual)
    TEST_ASSERT_EQUAL_UINT16(expected, actual)
    TEST_ASSERT_EQUAL_UINT32(expected, actual)
    TEST_ASSERT_EQUAL_UINT64(expected, actual)

Compare two integers for equality and display errors as unsigned integers.  Like INT, there are
variants for different sizes also.

    TEST_ASSERT_EQUAL_HEX(expected, actual)
    TEST_ASSERT_EQUAL_HEX8(expected, actual)
    TEST_ASSERT_EQUAL_HEX16(expected, actual)
    TEST_ASSERT_EQUAL_HEX32(expected, actual)
    TEST_ASSERT_EQUAL_HEX64(expected, actual)

Compares two integers for equality and display errors as hexadecimal.  Like the other integer comparisons,
you can specify the size... here the size will also effect how many nibbles are shown (for example, `HEX16`
will show 4 nibbles).

    TEST_ASSERT_EQUAL(expected, actual)

Another way of calling TEST_ASSERT_EQUAL_INT

    TEST_ASSERT_INT_WITHIN(delta, expected, actual)

Asserts that the actual value is within plus or minus delta of the expected value.  This also comes in
size specific variants.


    TEST_ASSERT_GREATER_THAN(threshold, actual)

Asserts that the actual value is greater than the threshold. This also comes in size specific variants.


    TEST_ASSERT_LESS_THAN(threshold, actual)

Asserts that the actual value is less than the threshold. This also comes in size specific variants.


Arrays
------

    _ARRAY

You can append `_ARRAY` to any of these macros to make an array comparison of that type.  Here you will
need to care a bit more about the actual size of the value being checked.  You will also specify an
additional argument which is the number of elements to compare.  For example:

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, elements)

    _EACH_EQUAL

Another array comparison option is to check that EVERY element of an array is equal to a single expected
value. You do this by specifying the EACH_EQUAL macro. For example:

    TEST_ASSERT_EACH_EQUAL_INT32(expected, actual, elements)

Numerical Assertions: Bitwise
-----------------------------

    TEST_ASSERT_BITS(mask, expected, actual)

Use an integer mask to specify which bits should be compared between two other integers.  High bits in the mask are compared, low bits ignored.

    TEST_ASSERT_BITS_HIGH(mask, actual)

Use an integer mask to specify which bits should be inspected to determine if they are all set high.  High bits in the mask are compared, low bits ignored.

    TEST_ASSERT_BITS_LOW(mask, actual)

Use an integer mask to specify which bits should be inspected to determine if they are all set low.  High bits in the mask are compared, low bits ignored.

    TEST_ASSERT_BIT_HIGH(bit, actual)

Test a single bit and verify that it is high.  The bit is specified 0-31 for a 32-bit integer.

    TEST_ASSERT_BIT_LOW(bit, actual)

Test a single bit and verify that it is low.  The bit is specified 0-31 for a 32-bit integer.

Numerical Assertions: Floats
----------------------------

    TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)

Asserts that the actual value is within plus or minus delta of the expected value.

    TEST_ASSERT_EQUAL_FLOAT(expected, actual)
    TEST_ASSERT_EQUAL_DOUBLE(expected, actual)

Asserts that two floating point values are "equal" within a small % delta of the expected value.

String Assertions
-----------------

    TEST_ASSERT_EQUAL_STRING(expected, actual)

Compare two null-terminate strings.  Fail if any character is different or if the lengths are different.

    TEST_ASSERT_EQUAL_STRING_LEN(expected, actual, len)

Compare two strings.  Fail if any character is different, stop comparing after len characters.

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, actual, message)

Compare two null-terminate strings.  Fail if any character is different or if the lengths are different. Output a custom message on failure.

    TEST_ASSERT_EQUAL_STRING_LEN_MESSAGE(expected, actual, len, message)

Compare two strings.  Fail if any character is different, stop comparing after len characters. Output a custom message on failure.

Pointer Assertions
------------------

Most pointer operations can be performed by simply using the integer comparisons above.  However, a couple of special cases are added for clarity.

    TEST_ASSERT_NULL(pointer)

Fails if the pointer is not equal to NULL

    TEST_ASSERT_NOT_NULL(pointer)

Fails if the pointer is equal to NULL

Memory Assertions
-----------------

    TEST_ASSERT_EQUAL_MEMORY(expected, actual, len)

Compare two blocks of memory.  This is a good generic assertion for types that can't be coerced into acting like
standard types... but since it's a memory compare, you have to be careful that your data types are packed.

_MESSAGE
--------

you can append _MESSAGE to any of the macros to make them take an additional argument.  This argument
is a string that will be printed at the end of the failure strings.  This is useful for specifying more
information about the problem.


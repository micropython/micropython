/* Copyright (c) 2010 James Grenning and Contributed to Unity Project
 * ==========================================
 *  Unity Project - A Test Framework for C
 *  Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
 *  [Released under MIT License. Please refer to license.txt for details]
 * ========================================== */

#ifndef UNITY_FIXTURE_H_
#define UNITY_FIXTURE_H_

#include "unity.h"
#include "unity_internals.h"
#include "unity_fixture_malloc_overrides.h"
#include "unity_fixture_internals.h"

int UnityMain(int argc, const char* argv[], void (*runAllTests)(void));


#define TEST_GROUP(group)\
    static const char* TEST_GROUP_##group = #group

#define TEST_SETUP(group) void TEST_##group##_SETUP(void);\
    void TEST_##group##_SETUP(void)

#define TEST_TEAR_DOWN(group) void TEST_##group##_TEAR_DOWN(void);\
    void TEST_##group##_TEAR_DOWN(void)


#define TEST(group, name) \
    void TEST_##group##_##name##_(void);\
    void TEST_##group##_##name##_run(void);\
    void TEST_##group##_##name##_run(void)\
    {\
        UnityTestRunner(TEST_##group##_SETUP,\
            TEST_##group##_##name##_,\
            TEST_##group##_TEAR_DOWN,\
            "TEST(" #group ", " #name ")",\
            TEST_GROUP_##group, #name,\
            __FILE__, __LINE__);\
    }\
    void  TEST_##group##_##name##_(void)

#define IGNORE_TEST(group, name) \
    void TEST_##group##_##name##_(void);\
    void TEST_##group##_##name##_run(void);\
    void TEST_##group##_##name##_run(void)\
    {\
        UnityIgnoreTest("IGNORE_TEST(" #group ", " #name ")", TEST_GROUP_##group, #name);\
    }\
    void TEST_##group##_##name##_(void)

/* Call this for each test, insider the group runner */
#define RUN_TEST_CASE(group, name) \
    { void TEST_##group##_##name##_run(void);\
      TEST_##group##_##name##_run(); }

/* This goes at the bottom of each test file or in a separate c file */
#define TEST_GROUP_RUNNER(group)\
    void TEST_##group##_GROUP_RUNNER(void);\
    void TEST_##group##_GROUP_RUNNER(void)

/* Call this from main */
#define RUN_TEST_GROUP(group)\
    { void TEST_##group##_GROUP_RUNNER(void);\
      TEST_##group##_GROUP_RUNNER(); }

/* CppUTest Compatibility Macros */
#ifndef UNITY_EXCLUDE_CPPUTEST_ASSERTS
/* Sets a pointer and automatically restores it to its old value after teardown */
#define UT_PTR_SET(ptr, newPointerValue)               UnityPointer_Set((void**)&(ptr), (void*)(newPointerValue), __LINE__)
#define TEST_ASSERT_POINTERS_EQUAL(expected, actual)   TEST_ASSERT_EQUAL_PTR((expected), (actual))
#define TEST_ASSERT_BYTES_EQUAL(expected, actual)      TEST_ASSERT_EQUAL_HEX8(0xff & (expected), 0xff & (actual))
#define FAIL(message)                                  TEST_FAIL_MESSAGE((message))
#define CHECK(condition)                               TEST_ASSERT_TRUE((condition))
#define LONGS_EQUAL(expected, actual)                  TEST_ASSERT_EQUAL_INT((expected), (actual))
#define STRCMP_EQUAL(expected, actual)                 TEST_ASSERT_EQUAL_STRING((expected), (actual))
#define DOUBLES_EQUAL(expected, actual, delta)         TEST_ASSERT_DOUBLE_WITHIN((delta), (expected), (actual))
#endif

/* You must compile with malloc replacement, as defined in unity_fixture_malloc_overrides.h */
void UnityMalloc_MakeMallocFailAfterCount(int count);

#endif /* UNITY_FIXTURE_H_ */

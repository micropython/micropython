/* Copyright (c) 2010 James Grenning and Contributed to Unity Project
 * ==========================================
 *  Unity Project - A Test Framework for C
 *  Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
 *  [Released under MIT License. Please refer to license.txt for details]
 * ========================================== */

#include "unity_fixture.h"
#include "unity_output_Spy.h"
#include <stdlib.h>
#include <string.h>

TEST_GROUP(UnityFixture);

TEST_SETUP(UnityFixture)
{
}

TEST_TEAR_DOWN(UnityFixture)
{
}

static int* pointer1 = 0;
static int* pointer2 = (int*)2;
static int* pointer3 = (int*)3;
static int int1;
static int int2;
static int int3;
static int int4;

TEST(UnityFixture, PointerSetting)
{
    TEST_ASSERT_POINTERS_EQUAL(pointer1, 0);
    UT_PTR_SET(pointer1, &int1);
    UT_PTR_SET(pointer2, &int2);
    UT_PTR_SET(pointer3, &int3);
    TEST_ASSERT_POINTERS_EQUAL(pointer1, &int1);
    TEST_ASSERT_POINTERS_EQUAL(pointer2, &int2);
    TEST_ASSERT_POINTERS_EQUAL(pointer3, &int3);
    UT_PTR_SET(pointer1, &int4);
    UnityPointer_UndoAllSets();
    TEST_ASSERT_POINTERS_EQUAL(pointer1, 0);
    TEST_ASSERT_POINTERS_EQUAL(pointer2, (int*)2);
    TEST_ASSERT_POINTERS_EQUAL(pointer3, (int*)3);
}

TEST(UnityFixture, ForceMallocFail)
{
    void* m;
    void* mfails;
    UnityMalloc_MakeMallocFailAfterCount(1);
    m = malloc(10);
    CHECK(m);
    mfails = malloc(10);
    TEST_ASSERT_POINTERS_EQUAL(0, mfails);
    free(m);
}

TEST(UnityFixture, ReallocSmallerIsUnchanged)
{
    void* m1 = malloc(10);
    void* m2 = realloc(m1, 5);
    TEST_ASSERT_POINTERS_EQUAL(m1, m2);
    free(m2);
}

TEST(UnityFixture, ReallocSameIsUnchanged)
{
    void* m1 = malloc(10);
    void* m2 = realloc(m1, 10);
    TEST_ASSERT_POINTERS_EQUAL(m1, m2);
    free(m2);
}

TEST(UnityFixture, ReallocLargerNeeded)
{
    void* m1 = malloc(10);
    void* m2;
    CHECK(m1);
    strcpy((char*)m1, "123456789");
    m2 = realloc(m1, 15);
    /* CHECK(m1 != m2); //Depends on implementation */
    STRCMP_EQUAL("123456789", m2);
    free(m2);
}

TEST(UnityFixture, ReallocNullPointerIsLikeMalloc)
{
    void* m = realloc(0, 15);
    CHECK(m != 0);
    free(m);
}

TEST(UnityFixture, ReallocSizeZeroFreesMemAndReturnsNullPointer)
{
    void* m1 = malloc(10);
    void* m2 = realloc(m1, 0);
    TEST_ASSERT_POINTERS_EQUAL(0, m2);
}

TEST(UnityFixture, CallocFillsWithZero)
{
    void* m = calloc(3, sizeof(char));
    char* s = (char*)m;
    CHECK(m);
    TEST_ASSERT_BYTES_EQUAL(0, s[0]);
    TEST_ASSERT_BYTES_EQUAL(0, s[1]);
    TEST_ASSERT_BYTES_EQUAL(0, s[2]);
    free(m);
}

static char *p1;
static char *p2;

TEST(UnityFixture, PointerSet)
{
    char c1;
    char c2;
    char newC1;
    char newC2;
    p1 = &c1;
    p2 = &c2;

    UnityPointer_Init();
    UT_PTR_SET(p1, &newC1);
    UT_PTR_SET(p2, &newC2);
    TEST_ASSERT_POINTERS_EQUAL(&newC1, p1);
    TEST_ASSERT_POINTERS_EQUAL(&newC2, p2);
    UnityPointer_UndoAllSets();
    TEST_ASSERT_POINTERS_EQUAL(&c1, p1);
    TEST_ASSERT_POINTERS_EQUAL(&c2, p2);
}

TEST(UnityFixture, FreeNULLSafety)
{
    free(NULL);
}

TEST(UnityFixture, ConcludeTestIncrementsFailCount)
{
    UNITY_UINT savedFails = Unity.TestFailures;
    UNITY_UINT savedIgnores = Unity.TestIgnores;
    UnityOutputCharSpy_Enable(1);
    Unity.CurrentTestFailed = 1;
    UnityConcludeFixtureTest(); /* Resets TestFailed for this test to pass */
    Unity.CurrentTestIgnored = 1;
    UnityConcludeFixtureTest(); /* Resets TestIgnored */
    UnityOutputCharSpy_Enable(0);
    TEST_ASSERT_EQUAL(savedFails + 1, Unity.TestFailures);
    TEST_ASSERT_EQUAL(savedIgnores + 1, Unity.TestIgnores);
    Unity.TestFailures = savedFails;
    Unity.TestIgnores = savedIgnores;
}

/*------------------------------------------------------------ */

TEST_GROUP(UnityCommandOptions);

static int savedVerbose;
static unsigned int savedRepeat;
static const char* savedName;
static const char* savedGroup;

TEST_SETUP(UnityCommandOptions)
{
    savedVerbose = UnityFixture.Verbose;
    savedRepeat = UnityFixture.RepeatCount;
    savedName = UnityFixture.NameFilter;
    savedGroup = UnityFixture.GroupFilter;
}

TEST_TEAR_DOWN(UnityCommandOptions)
{
    UnityFixture.Verbose = savedVerbose;
    UnityFixture.RepeatCount= savedRepeat;
    UnityFixture.NameFilter = savedName;
    UnityFixture.GroupFilter = savedGroup;
}


static const char* noOptions[] = {
        "testrunner.exe"
};

TEST(UnityCommandOptions, DefaultOptions)
{
    UnityGetCommandLineOptions(1, noOptions);
    TEST_ASSERT_EQUAL(0, UnityFixture.Verbose);
    TEST_ASSERT_POINTERS_EQUAL(0, UnityFixture.GroupFilter);
    TEST_ASSERT_POINTERS_EQUAL(0, UnityFixture.NameFilter);
    TEST_ASSERT_EQUAL(1, UnityFixture.RepeatCount);
}

static const char* verbose[] = {
        "testrunner.exe",
        "-v"
};

TEST(UnityCommandOptions, OptionVerbose)
{
    TEST_ASSERT_EQUAL(0, UnityGetCommandLineOptions(2, verbose));
    TEST_ASSERT_EQUAL(1, UnityFixture.Verbose);
}

static const char* group[] = {
        "testrunner.exe",
        "-g", "groupname"
};

TEST(UnityCommandOptions, OptionSelectTestByGroup)
{
    TEST_ASSERT_EQUAL(0, UnityGetCommandLineOptions(3, group));
    STRCMP_EQUAL("groupname", UnityFixture.GroupFilter);
}

static const char* name[] = {
        "testrunner.exe",
        "-n", "testname"
};

TEST(UnityCommandOptions, OptionSelectTestByName)
{
    TEST_ASSERT_EQUAL(0, UnityGetCommandLineOptions(3, name));
    STRCMP_EQUAL("testname", UnityFixture.NameFilter);
}

static const char* repeat[] = {
        "testrunner.exe",
        "-r", "99"
};

TEST(UnityCommandOptions, OptionSelectRepeatTestsDefaultCount)
{
    TEST_ASSERT_EQUAL(0, UnityGetCommandLineOptions(2, repeat));
    TEST_ASSERT_EQUAL(2, UnityFixture.RepeatCount);
}

TEST(UnityCommandOptions, OptionSelectRepeatTestsSpecificCount)
{
    TEST_ASSERT_EQUAL(0, UnityGetCommandLineOptions(3, repeat));
    TEST_ASSERT_EQUAL(99, UnityFixture.RepeatCount);
}

static const char* multiple[] = {
        "testrunner.exe",
        "-v",
        "-g", "groupname",
        "-n", "testname",
        "-r", "98"
};

TEST(UnityCommandOptions, MultipleOptions)
{
    TEST_ASSERT_EQUAL(0, UnityGetCommandLineOptions(8, multiple));
    TEST_ASSERT_EQUAL(1, UnityFixture.Verbose);
    STRCMP_EQUAL("groupname", UnityFixture.GroupFilter);
    STRCMP_EQUAL("testname", UnityFixture.NameFilter);
    TEST_ASSERT_EQUAL(98, UnityFixture.RepeatCount);
}

static const char* dashRNotLast[] = {
        "testrunner.exe",
        "-v",
        "-g", "gggg",
        "-r",
        "-n", "tttt",
};

TEST(UnityCommandOptions, MultipleOptionsDashRNotLastAndNoValueSpecified)
{
    TEST_ASSERT_EQUAL(0, UnityGetCommandLineOptions(7, dashRNotLast));
    TEST_ASSERT_EQUAL(1, UnityFixture.Verbose);
    STRCMP_EQUAL("gggg", UnityFixture.GroupFilter);
    STRCMP_EQUAL("tttt", UnityFixture.NameFilter);
    TEST_ASSERT_EQUAL(2, UnityFixture.RepeatCount);
}

static const char* unknownCommand[] = {
        "testrunner.exe",
        "-v",
        "-g", "groupname",
        "-n", "testname",
        "-r", "98",
        "-z"
};
TEST(UnityCommandOptions, UnknownCommandIsIgnored)
{
    TEST_ASSERT_EQUAL(0, UnityGetCommandLineOptions(9, unknownCommand));
    TEST_ASSERT_EQUAL(1, UnityFixture.Verbose);
    STRCMP_EQUAL("groupname", UnityFixture.GroupFilter);
    STRCMP_EQUAL("testname", UnityFixture.NameFilter);
    TEST_ASSERT_EQUAL(98, UnityFixture.RepeatCount);
}

TEST(UnityCommandOptions, GroupOrNameFilterWithoutStringFails)
{
    TEST_ASSERT_EQUAL(1, UnityGetCommandLineOptions(3, unknownCommand));
    TEST_ASSERT_EQUAL(1, UnityGetCommandLineOptions(5, unknownCommand));
    TEST_ASSERT_EQUAL(1, UnityMain(3, unknownCommand, NULL));
}

TEST(UnityCommandOptions, GroupFilterReallyFilters)
{
    UNITY_UINT saved = Unity.NumberOfTests;
    TEST_ASSERT_EQUAL(0, UnityGetCommandLineOptions(4, unknownCommand));
    UnityIgnoreTest(NULL, "non-matching", NULL);
    TEST_ASSERT_EQUAL(saved, Unity.NumberOfTests);
}

IGNORE_TEST(UnityCommandOptions, TestShouldBeIgnored)
{
    TEST_FAIL_MESSAGE("This test should not run!");
}

/*------------------------------------------------------------ */

TEST_GROUP(LeakDetection);

TEST_SETUP(LeakDetection)
{
#ifdef UNITY_EXCLUDE_STDLIB_MALLOC
    UnityOutputCharSpy_Create(200);
#else
    UnityOutputCharSpy_Create(1000);
#endif
}

TEST_TEAR_DOWN(LeakDetection)
{
    UnityOutputCharSpy_Destroy();
}

#define EXPECT_ABORT_BEGIN \
  { \
    jmp_buf TestAbortFrame;   \
    memcpy(TestAbortFrame, Unity.AbortFrame, sizeof(jmp_buf)); \
    if (TEST_PROTECT()) \
    {

#define EXPECT_ABORT_END \
    } \
    memcpy(Unity.AbortFrame, TestAbortFrame, sizeof(jmp_buf)); \
  }

/* This tricky set of defines lets us see if we are using the Spy, returns 1 if true */
#ifdef __STDC_VERSION__

#if __STDC_VERSION__ >= 199901L
#define USING_SPY_AS(a)                    EXPAND_AND_USE_2ND(ASSIGN_VALUE(a), 0)
#define ASSIGN_VALUE(a)                    VAL_##a
#define VAL_UnityOutputCharSpy_OutputChar  0, 1
#define EXPAND_AND_USE_2ND(a, b)           SECOND_PARAM(a, b, throwaway)
#define SECOND_PARAM(a, b, ...)            b
#if USING_SPY_AS(UNITY_OUTPUT_CHAR)
  #define USING_OUTPUT_SPY /* UNITY_OUTPUT_CHAR = UnityOutputCharSpy_OutputChar */
#endif
#endif /* >= 199901 */

#else  /* __STDC_VERSION__ else */
#define UnityOutputCharSpy_OutputChar 42
#if UNITY_OUTPUT_CHAR == UnityOutputCharSpy_OutputChar /* Works if no -Wundef -Werror */
  #define USING_OUTPUT_SPY
#endif
#undef UnityOutputCharSpy_OutputChar
#endif /* __STDC_VERSION__ */

TEST(LeakDetection, DetectsLeak)
{
#ifndef USING_OUTPUT_SPY
    TEST_IGNORE_MESSAGE("Build with '-D UNITY_OUTPUT_CHAR=UnityOutputCharSpy_OutputChar' to enable tests");
#else
    void* m = malloc(10);
    TEST_ASSERT_NOT_NULL(m);
    UnityOutputCharSpy_Enable(1);
    EXPECT_ABORT_BEGIN
    UnityMalloc_EndTest();
    EXPECT_ABORT_END
    UnityOutputCharSpy_Enable(0);
    Unity.CurrentTestFailed = 0;
    CHECK(strstr(UnityOutputCharSpy_Get(), "This test leaks!"));
    free(m);
#endif
}

TEST(LeakDetection, BufferOverrunFoundDuringFree)
{
#ifndef USING_OUTPUT_SPY
    TEST_IGNORE();
#else
    void* m = malloc(10);
    char* s = (char*)m;
    TEST_ASSERT_NOT_NULL(m);
    s[10] = (char)0xFF;
    UnityOutputCharSpy_Enable(1);
    EXPECT_ABORT_BEGIN
    free(m);
    EXPECT_ABORT_END
    UnityOutputCharSpy_Enable(0);
    Unity.CurrentTestFailed = 0;
    CHECK(strstr(UnityOutputCharSpy_Get(), "Buffer overrun detected during free()"));
#endif
}

TEST(LeakDetection, BufferOverrunFoundDuringRealloc)
{
#ifndef USING_OUTPUT_SPY
    TEST_IGNORE();
#else
    void* m = malloc(10);
    char* s = (char*)m;
    TEST_ASSERT_NOT_NULL(m);
    s[10] = (char)0xFF;
    UnityOutputCharSpy_Enable(1);
    EXPECT_ABORT_BEGIN
    m = realloc(m, 100);
    EXPECT_ABORT_END
    UnityOutputCharSpy_Enable(0);
    Unity.CurrentTestFailed = 0;
    CHECK(strstr(UnityOutputCharSpy_Get(), "Buffer overrun detected during realloc()"));
#endif
}

TEST(LeakDetection, BufferGuardWriteFoundDuringFree)
{
#ifndef USING_OUTPUT_SPY
    TEST_IGNORE();
#else
    void* m = malloc(10);
    char* s = (char*)m;
    TEST_ASSERT_NOT_NULL(m);
    s[-1] = (char)0x00; /* Will not detect 0 */
    s[-2] = (char)0x01;
    UnityOutputCharSpy_Enable(1);
    EXPECT_ABORT_BEGIN
    free(m);
    EXPECT_ABORT_END
    UnityOutputCharSpy_Enable(0);
    Unity.CurrentTestFailed = 0;
    CHECK(strstr(UnityOutputCharSpy_Get(), "Buffer overrun detected during free()"));
#endif
}

TEST(LeakDetection, BufferGuardWriteFoundDuringRealloc)
{
#ifndef USING_OUTPUT_SPY
    TEST_IGNORE();
#else
    void* m = malloc(10);
    char* s = (char*)m;
    TEST_ASSERT_NOT_NULL(m);
    s[-1] = (char)0x0A;
    UnityOutputCharSpy_Enable(1);
    EXPECT_ABORT_BEGIN
    m = realloc(m, 100);
    EXPECT_ABORT_END
    UnityOutputCharSpy_Enable(0);
    Unity.CurrentTestFailed = 0;
    CHECK(strstr(UnityOutputCharSpy_Get(), "Buffer overrun detected during realloc()"));
#endif
}

TEST(LeakDetection, PointerSettingMax)
{
#ifndef USING_OUTPUT_SPY
    TEST_IGNORE();
#else
    int i;
    for (i = 0; i < UNITY_MAX_POINTERS; i++) UT_PTR_SET(pointer1, &int1);
    UnityOutputCharSpy_Enable(1);
    EXPECT_ABORT_BEGIN
    UT_PTR_SET(pointer1, &int1);
    EXPECT_ABORT_END
    UnityOutputCharSpy_Enable(0);
    Unity.CurrentTestFailed = 0;
    CHECK(strstr(UnityOutputCharSpy_Get(), "Too many pointers set"));
#endif
}

/*------------------------------------------------------------ */

TEST_GROUP(InternalMalloc);
#define TEST_ASSERT_MEMORY_ALL_FREE_LIFO_ORDER(first_mem_ptr, ptr) \
    ptr = malloc(10); free(ptr);                                   \
    TEST_ASSERT_EQUAL_PTR_MESSAGE(first_mem_ptr, ptr, "Memory was stranded, free in LIFO order");


TEST_SETUP(InternalMalloc) { }
TEST_TEAR_DOWN(InternalMalloc) { }

TEST(InternalMalloc, MallocPastBufferFails)
{
#ifdef UNITY_EXCLUDE_STDLIB_MALLOC
    void* m = malloc(UNITY_INTERNAL_HEAP_SIZE_BYTES/2 + 1);
    void* n = malloc(UNITY_INTERNAL_HEAP_SIZE_BYTES/2);
    free(m);
    TEST_ASSERT_NOT_NULL(m);
    TEST_ASSERT_NULL(n);
    TEST_ASSERT_MEMORY_ALL_FREE_LIFO_ORDER(m, n);
#endif
}

TEST(InternalMalloc, CallocPastBufferFails)
{
#ifdef UNITY_EXCLUDE_STDLIB_MALLOC
    void* m = calloc(1, UNITY_INTERNAL_HEAP_SIZE_BYTES/2 + 1);
    void* n = calloc(1, UNITY_INTERNAL_HEAP_SIZE_BYTES/2);
    free(m);
    TEST_ASSERT_NOT_NULL(m);
    TEST_ASSERT_NULL(n);
    TEST_ASSERT_MEMORY_ALL_FREE_LIFO_ORDER(m, n);
#endif
}

TEST(InternalMalloc, MallocThenReallocGrowsMemoryInPlace)
{
#ifdef UNITY_EXCLUDE_STDLIB_MALLOC
    void* m = malloc(UNITY_INTERNAL_HEAP_SIZE_BYTES/2 + 1);
    void* n = realloc(m, UNITY_INTERNAL_HEAP_SIZE_BYTES/2 + 9);
    free(n);
    TEST_ASSERT_NOT_NULL(m);
    TEST_ASSERT_EQUAL(m, n);
    TEST_ASSERT_MEMORY_ALL_FREE_LIFO_ORDER(m, n);
#endif
}

TEST(InternalMalloc, ReallocFailDoesNotFreeMem)
{
#ifdef UNITY_EXCLUDE_STDLIB_MALLOC
    void* m = malloc(UNITY_INTERNAL_HEAP_SIZE_BYTES/2);
    void* n1 = malloc(10);
    void* out_of_mem = realloc(n1, UNITY_INTERNAL_HEAP_SIZE_BYTES/2 + 1);
    void* n2 = malloc(10);

    free(n2);
    if (out_of_mem == NULL) free(n1);
    free(m);

    TEST_ASSERT_NOT_NULL(m);       /* Got a real memory location */
    TEST_ASSERT_NULL(out_of_mem);  /* The realloc should have failed */
    TEST_ASSERT_NOT_EQUAL(n2, n1); /* If n1 != n2 then realloc did not free n1 */
    TEST_ASSERT_MEMORY_ALL_FREE_LIFO_ORDER(m, n2);
#endif
}

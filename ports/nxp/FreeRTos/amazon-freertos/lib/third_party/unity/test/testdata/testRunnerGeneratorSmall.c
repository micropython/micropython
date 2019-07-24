/* This Test File Is Used To Verify Many Combinations Of Using the Generate Test Runner Script */

#include <stdio.h>
#include "unity.h"
#include "Defs.h"

TEST_FILE("some_file.c")

/* Notes about prefixes:
   test     - normal default prefix. these are "always run" tests for this procedure
   spec     - normal default prefix. required to run default setup/teardown calls.
*/

/* Support for Meta Test Rig */
#define TEST_CASE(a)
void putcharSpy(int c) { (void)putchar(c);} // include passthrough for linking tests

/* Global Variables Used During These Tests */
int CounterSetup = 0;
int CounterTeardown = 0;
int CounterSuiteSetup = 0;

void setUp(void)
{
    CounterSetup = 1;
}

void tearDown(void)
{
    CounterTeardown = 1;
}

void custom_setup(void)
{
    CounterSetup = 2;
}

void custom_teardown(void)
{
    CounterTeardown = 2;
}

void test_ThisTestAlwaysPasses(void)
{
    TEST_PASS();
}

void test_ThisTestAlwaysFails(void)
{
    TEST_FAIL_MESSAGE("This Test Should Fail");
}

void test_ThisTestAlwaysIgnored(void)
{
    TEST_IGNORE_MESSAGE("This Test Should Be Ignored");
}

void spec_ThisTestPassesWhenNormalSetupRan(void)
{
    TEST_ASSERT_EQUAL_MESSAGE(1, CounterSetup, "Normal Setup Wasn't Run");
}

void spec_ThisTestPassesWhenNormalTeardownRan(void)
{
    TEST_ASSERT_EQUAL_MESSAGE(1, CounterTeardown, "Normal Teardown Wasn't Run");
}


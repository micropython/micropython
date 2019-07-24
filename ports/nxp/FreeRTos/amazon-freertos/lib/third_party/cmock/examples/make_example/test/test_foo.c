#include "unity.h"
#include "foo.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_foo_init_should_initialize_multiplier()
{
    foo_init();

    TEST_ASSERT_FALSE(1);
}

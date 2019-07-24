/* Copyright (c) 2010 James Grenning and Contributed to Unity Project
 * ==========================================
 *  Unity Project - A Test Framework for C
 *  Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
 *  [Released under MIT License. Please refer to license.txt for details]
 * ========================================== */

#include "unity_fixture.h"

static void runAllTests(void)
{
    RUN_TEST_GROUP(UnityFixture);
    RUN_TEST_GROUP(UnityCommandOptions);
    RUN_TEST_GROUP(LeakDetection);
    RUN_TEST_GROUP(InternalMalloc);
}

int main(int argc, const char* argv[])
{
    return UnityMain(argc, argv, runAllTests);
}


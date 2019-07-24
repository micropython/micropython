/* Copyright (c) 2010 James Grenning and Contributed to Unity Project
 * ==========================================
 *  Unity Project - A Test Framework for C
 *  Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
 *  [Released under MIT License. Please refer to license.txt for details]
 * ========================================== */

#include "unity_fixture.h"

TEST_GROUP_RUNNER(UnityFixture)
{
    RUN_TEST_CASE(UnityFixture, PointerSetting);
    RUN_TEST_CASE(UnityFixture, ForceMallocFail);
    RUN_TEST_CASE(UnityFixture, ReallocSmallerIsUnchanged);
    RUN_TEST_CASE(UnityFixture, ReallocSameIsUnchanged);
    RUN_TEST_CASE(UnityFixture, ReallocLargerNeeded);
    RUN_TEST_CASE(UnityFixture, ReallocNullPointerIsLikeMalloc);
    RUN_TEST_CASE(UnityFixture, ReallocSizeZeroFreesMemAndReturnsNullPointer);
    RUN_TEST_CASE(UnityFixture, CallocFillsWithZero);
    RUN_TEST_CASE(UnityFixture, PointerSet);
    RUN_TEST_CASE(UnityFixture, FreeNULLSafety);
    RUN_TEST_CASE(UnityFixture, ConcludeTestIncrementsFailCount);
}

TEST_GROUP_RUNNER(UnityCommandOptions)
{
    RUN_TEST_CASE(UnityCommandOptions, DefaultOptions);
    RUN_TEST_CASE(UnityCommandOptions, OptionVerbose);
    RUN_TEST_CASE(UnityCommandOptions, OptionSelectTestByGroup);
    RUN_TEST_CASE(UnityCommandOptions, OptionSelectTestByName);
    RUN_TEST_CASE(UnityCommandOptions, OptionSelectRepeatTestsDefaultCount);
    RUN_TEST_CASE(UnityCommandOptions, OptionSelectRepeatTestsSpecificCount);
    RUN_TEST_CASE(UnityCommandOptions, MultipleOptions);
    RUN_TEST_CASE(UnityCommandOptions, MultipleOptionsDashRNotLastAndNoValueSpecified);
    RUN_TEST_CASE(UnityCommandOptions, UnknownCommandIsIgnored);
    RUN_TEST_CASE(UnityCommandOptions, GroupOrNameFilterWithoutStringFails);
    RUN_TEST_CASE(UnityCommandOptions, GroupFilterReallyFilters);
    RUN_TEST_CASE(UnityCommandOptions, TestShouldBeIgnored);
}

TEST_GROUP_RUNNER(LeakDetection)
{
    RUN_TEST_CASE(LeakDetection, DetectsLeak);
    RUN_TEST_CASE(LeakDetection, BufferOverrunFoundDuringFree);
    RUN_TEST_CASE(LeakDetection, BufferOverrunFoundDuringRealloc);
    RUN_TEST_CASE(LeakDetection, BufferGuardWriteFoundDuringFree);
    RUN_TEST_CASE(LeakDetection, BufferGuardWriteFoundDuringRealloc);
    RUN_TEST_CASE(LeakDetection, PointerSettingMax);
}

TEST_GROUP_RUNNER(InternalMalloc)
{
    RUN_TEST_CASE(InternalMalloc, MallocPastBufferFails);
    RUN_TEST_CASE(InternalMalloc, CallocPastBufferFails);
    RUN_TEST_CASE(InternalMalloc, MallocThenReallocGrowsMemoryInPlace);
    RUN_TEST_CASE(InternalMalloc, ReallocFailDoesNotFreeMem);
}

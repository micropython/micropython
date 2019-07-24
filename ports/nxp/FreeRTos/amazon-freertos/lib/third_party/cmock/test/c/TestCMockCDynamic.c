/* ==========================================
    CMock Project - Automatic Mock Generation for C
    Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
    [Released under MIT License. Please refer to license.txt for details]
========================================== */

#include "unity.h"
#include "cmock.h"

#define TEST_MEM_INDEX_SIZE  (CMOCK_MEM_INDEX_TYPE)(sizeof(CMOCK_MEM_INDEX_TYPE))
#define TEST_MEM_INDEX_PAD   (CMOCK_MEM_INDEX_TYPE)(((CMOCK_MEM_INDEX_TYPE)sizeof(CMOCK_MEM_INDEX_TYPE) + 7) & ~7) //round up to nearest 4 byte boundary

CMOCK_MEM_INDEX_TYPE StartingSize;

void setUp(void)
{
  CMock_Guts_MemFreeAll();
  StartingSize = CMock_Guts_MemBytesFree();
  TEST_ASSERT_EQUAL(0, CMock_Guts_MemBytesUsed());
}

void tearDown(void)
{
}

void test_MemNewWillReturnNullIfGivenIllegalSizes(void)
{
  TEST_ASSERT_EQUAL_HEX( CMOCK_GUTS_NONE, CMock_Guts_MemNew(0) );

  //verify we're cleared still
  TEST_ASSERT_EQUAL(0, CMock_Guts_MemBytesUsed());
  TEST_ASSERT_EQUAL(0, CMock_Guts_MemBytesFree());
}

void test_MemNewWillNowSupportSizesGreaterThanTheDefinesCMockSize(void)
{
    TEST_ASSERT_EQUAL(0, CMock_Guts_MemBytesFree());

    TEST_ASSERT_MESSAGE(CMock_Guts_MemNew(CMOCK_MEM_SIZE - TEST_MEM_INDEX_SIZE + 1) != CMOCK_GUTS_NONE, "Should Not Have Returned CMOCK_GUTS_NONE");

    TEST_ASSERT_EQUAL(((CMOCK_MEM_INDEX_TYPE)CMOCK_MEM_SIZE + TEST_MEM_INDEX_PAD), CMock_Guts_MemBytesUsed());
    TEST_ASSERT_EQUAL((CMOCK_MEM_INDEX_TYPE)CMOCK_MEM_SIZE, CMock_Guts_MemBytesFree());
}

void test_MemChainWillReturnNullAndDoNothingIfGivenIllegalInformation(void)
{
  CMOCK_MEM_INDEX_TYPE next = CMock_Guts_MemNew(8);
  TEST_ASSERT_EQUAL(8 + TEST_MEM_INDEX_PAD, CMock_Guts_MemBytesUsed());
  TEST_ASSERT_EQUAL(StartingSize - 8 - TEST_MEM_INDEX_PAD, CMock_Guts_MemBytesFree());

  TEST_ASSERT_EQUAL_HEX( CMOCK_GUTS_NONE, CMock_Guts_MemChain(next + CMOCK_MEM_SIZE, next) );
  TEST_ASSERT_EQUAL_HEX( CMOCK_GUTS_NONE, CMock_Guts_MemChain(next, next + CMOCK_MEM_SIZE) );

  //verify we're still the same
  TEST_ASSERT_EQUAL(8 + TEST_MEM_INDEX_PAD, CMock_Guts_MemBytesUsed());
  TEST_ASSERT_EQUAL(StartingSize - 8 - TEST_MEM_INDEX_PAD, CMock_Guts_MemBytesFree());
}

void test_MemNextWillReturnNullIfGivenABadRoot(void)
{
  TEST_ASSERT_EQUAL_HEX( CMOCK_GUTS_NONE, CMock_Guts_MemNext(0) );
  TEST_ASSERT_EQUAL_HEX( CMOCK_GUTS_NONE, CMock_Guts_MemNext(2) );
  TEST_ASSERT_EQUAL_HEX( CMOCK_GUTS_NONE, CMock_Guts_MemNext( CMOCK_MEM_SIZE - 4 ) );

  //verify we're cleared still
  TEST_ASSERT_EQUAL(0, CMock_Guts_MemBytesUsed());
  TEST_ASSERT_EQUAL(StartingSize, CMock_Guts_MemBytesFree());
}

void test_ThatWeCanClaimAndChainAFewElementsTogether(void)
{
  unsigned int  i;
  CMOCK_MEM_INDEX_TYPE first = CMOCK_GUTS_NONE;
  CMOCK_MEM_INDEX_TYPE next;
  CMOCK_MEM_INDEX_TYPE element[4];

  //verify we're cleared first
  TEST_ASSERT_EQUAL(0, CMock_Guts_MemBytesUsed());
  TEST_ASSERT_EQUAL(StartingSize, CMock_Guts_MemBytesFree());

  //first element
  element[0] = CMock_Guts_MemNew(sizeof(unsigned int));
  TEST_ASSERT_MESSAGE(element[0] != CMOCK_GUTS_NONE, "Should Not Have Returned CMOCK_GUTS_NONE");
  first = CMock_Guts_MemChain(first, element[0]);
  TEST_ASSERT_EQUAL(element[0], first);
  *((unsigned int*)CMock_Guts_GetAddressFor(element[0])) = 0;

  //verify we're using the right amount of memory
  TEST_ASSERT_EQUAL(1 * (TEST_MEM_INDEX_PAD + 8), CMock_Guts_MemBytesUsed());
  TEST_ASSERT_EQUAL(StartingSize - 1 * (TEST_MEM_INDEX_PAD + 8), CMock_Guts_MemBytesFree());

  //second element
  element[1] = CMock_Guts_MemNew(sizeof(unsigned int));
  TEST_ASSERT_MESSAGE(element[1] != CMOCK_GUTS_NONE, "Should Not Have Returned CMOCK_GUTS_NONE");
  TEST_ASSERT_NOT_EQUAL(element[0], element[1]);
  TEST_ASSERT_EQUAL(first, CMock_Guts_MemChain(first, element[1]));
  *((unsigned int*)CMock_Guts_GetAddressFor(element[1])) = 1;

  //verify we're using the right amount of memory
  TEST_ASSERT_EQUAL(2 * (TEST_MEM_INDEX_PAD + 8), CMock_Guts_MemBytesUsed());
  TEST_ASSERT_EQUAL(StartingSize - 2 * (TEST_MEM_INDEX_PAD + 8), CMock_Guts_MemBytesFree());

  //third element
  element[2] = CMock_Guts_MemNew(sizeof(unsigned int));
  TEST_ASSERT_MESSAGE(element[2] != CMOCK_GUTS_NONE, "Should Not Have Returned CMOCK_GUTS_NONE");
  TEST_ASSERT_NOT_EQUAL(element[0], element[2]);
  TEST_ASSERT_NOT_EQUAL(element[1], element[2]);
  TEST_ASSERT_EQUAL(first, CMock_Guts_MemChain(first, element[2]));
  *((unsigned int*)CMock_Guts_GetAddressFor(element[2])) = 2;

  //verify we're using the right amount of memory
  TEST_ASSERT_EQUAL(3 * (TEST_MEM_INDEX_PAD + 8), CMock_Guts_MemBytesUsed());
  TEST_ASSERT_EQUAL(StartingSize - 3 * (TEST_MEM_INDEX_PAD + 8), CMock_Guts_MemBytesFree());

  //fourth element
  element[3] = CMock_Guts_MemNew(sizeof(unsigned int));
  TEST_ASSERT_MESSAGE(element[3] != CMOCK_GUTS_NONE, "Should Not Have Returned CMOCK_GUTS_NONE");
  TEST_ASSERT_NOT_EQUAL(element[0], element[3]);
  TEST_ASSERT_NOT_EQUAL(element[1], element[3]);
  TEST_ASSERT_NOT_EQUAL(element[2], element[3]);
  TEST_ASSERT_EQUAL(first, CMock_Guts_MemChain(first, element[3]));
  *((unsigned int*)CMock_Guts_GetAddressFor(element[3])) = 3;

  //verify we're using the right amount of memory
  TEST_ASSERT_EQUAL(4 * (TEST_MEM_INDEX_PAD + 8), CMock_Guts_MemBytesUsed());
  TEST_ASSERT_EQUAL(StartingSize - 4 * (TEST_MEM_INDEX_PAD + 8), CMock_Guts_MemBytesFree());

  //traverse list
  next = first;
  for (i = 0; i < 4; i++)
  {
    TEST_ASSERT_EQUAL(element[i], next);
    TEST_ASSERT_EQUAL(i, *((unsigned int*)CMock_Guts_GetAddressFor(element[i])));
    next = CMock_Guts_MemNext(next);
  }

  //verify we get a null at the end of the list
  TEST_ASSERT_EQUAL_HEX( CMOCK_GUTS_NONE, next);

  //verify we're using the right amount of memory
  TEST_ASSERT_EQUAL(4 * (TEST_MEM_INDEX_PAD + 8), CMock_Guts_MemBytesUsed());
  TEST_ASSERT_EQUAL(StartingSize - 4 * (TEST_MEM_INDEX_PAD + 8), CMock_Guts_MemBytesFree());

  //Free it all
  CMock_Guts_MemFreeAll();

  //verify we're cleared
  TEST_ASSERT_EQUAL(0, CMock_Guts_MemBytesUsed());
  TEST_ASSERT_EQUAL(StartingSize, CMock_Guts_MemBytesFree());
}

void test_ThatWeCanAskForAllSortsOfSizes(void)
{
  CMOCK_MEM_INDEX_TYPE i;
  CMOCK_MEM_INDEX_TYPE first = CMOCK_GUTS_NONE;
  CMOCK_MEM_INDEX_TYPE next;
  CMOCK_MEM_INDEX_TYPE sizes[10]          = {3,  1,  80,  5,  8,  31, 7,  911, 2,  80};
  CMOCK_MEM_INDEX_TYPE sizes_buffered[10] = {16, 16, 88,  16, 16, 40, 16, 920, 16, 88}; //includes counter
  CMOCK_MEM_INDEX_TYPE sum = 0;
  CMOCK_MEM_INDEX_TYPE cap;

  for (i = 0; i < 10; i++)
  {
    next = CMock_Guts_MemNew(sizes[i]);
    TEST_ASSERT_MESSAGE(next != CMOCK_GUTS_NONE, "Should Not Have Returned CMOCK_GUTS_NONE");

    first = CMock_Guts_MemChain(first, next);
    TEST_ASSERT_MESSAGE(first != CMOCK_GUTS_NONE, "Should Not Have Returned CMOCK_GUTS_NONE");

    sum += sizes_buffered[i];
    cap = (StartingSize > (sum + CMOCK_MEM_SIZE)) ? StartingSize : (sum + CMOCK_MEM_SIZE);
    TEST_ASSERT_EQUAL(sum, CMock_Guts_MemBytesUsed());
    TEST_ASSERT(cap >= CMock_Guts_MemBytesFree());
  }

  //verify we can still walk through the elements allocated
  next = first;
  for (i = 0; i < 10; i++)
  {
    TEST_ASSERT_MESSAGE(next != CMOCK_GUTS_NONE, "Should Not Have Returned CMOCK_GUTS_NONE");
    next = CMock_Guts_MemNext(next);
  }

  //there aren't any after that
  TEST_ASSERT_EQUAL_HEX( CMOCK_GUTS_NONE, next);
}

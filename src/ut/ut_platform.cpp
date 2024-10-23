#include "ut_platform.h"

#include "platform/os.h"
#include "platform/platform.h"
#include "platform/log.h"


void ut_platform_run()
{
    ut_memory_pool_create_destroy();
    ut_memory_pool_over_allocate();
    ut_pool_allocation_test();
}

void ut_memory_pool_create_destroy(void)
{
    memory_pool_st* unittest0;
    STANDARD_RESULT res = memory_pool_create("unittest1", 1, &unittest0);
    (void)res;
    STANDARD_ASSERT(res == 0);
    STANDARD_ASSERT(unittest0->payload_size > 1);
    pools_list_print();

    memory_pool_destroy(unittest0);
    pools_list_print();


    // destroy first to last
    memory_pool_st* unittest1;
    res = memory_pool_create("unittest1", 1, &unittest1);
    STANDARD_ASSERT(res == 0);

    memory_pool_st* unittest2;
    res = memory_pool_create("unittest2", 1, &unittest2);
    STANDARD_ASSERT(res == 0);

    memory_pool_st* unittest3;
    res = memory_pool_create("unittest3", 1, &unittest3);
    STANDARD_ASSERT(res == 0);

    pools_list_print();
    memory_pool_destroy(unittest1);
    pools_list_print();
    memory_pool_destroy(unittest2);
    pools_list_print();
    memory_pool_destroy(unittest3);
    pools_list_print();


    // destroy last to first
    memory_pool_st* unittest4;
    res = memory_pool_create("unittest4", 1, &unittest4);
    STANDARD_ASSERT(res == 0);

    memory_pool_st* unittest5;
    res = memory_pool_create("unittest5", 1, &unittest5);
    STANDARD_ASSERT(res == 0);

    memory_pool_st* unittest6;
    res = memory_pool_create("unittest6", 1, &unittest6);
    STANDARD_ASSERT(res == 0);

    pools_list_print();
    memory_pool_destroy(unittest6);
    pools_list_print();
    memory_pool_destroy(unittest5);
    pools_list_print();
    memory_pool_destroy(unittest4);
    pools_list_print();

}

void ut_memory_pool_over_allocate(void)
{
    memory_pool_st* unittest1;
    u64 size = os_page_size_get() - POOL_HEADER_SIZE_PLUS_PAD_BYTES;
    STANDARD_RESULT res = memory_pool_create("unittest1", size, &unittest1);
    (void)res;
    STANDARD_ASSERT(res == 0);
    pools_list_print();

    pool_free_zero_list_print(unittest1);
    pool_free_not_zero_list_print(unittest1);
    pool_allocation_list_print(unittest1);

    u64 full_alloc = size - ALLOCATION_HEADER_SIZE_PLUS_PAD_BYTES - ALLOCATION_FOOTER_SIZE_PLUS_PAD_BYTES;
    void* test = pool_allocate(full_alloc, unittest1, MEMORY_TAG_UNTAGGED, true);

    pool_free_zero_list_print(unittest1);
    pool_free_not_zero_list_print(unittest1);
    pool_allocation_list_print(unittest1);

    void* testfail = pool_allocate(1, unittest1, MEMORY_TAG_UNTAGGED, true);
    (void)testfail;
    STANDARD_ASSERT(testfail == NULL);

    pool_free_zero_list_print(unittest1);
    pool_free_not_zero_list_print(unittest1);
    pool_allocation_list_print(unittest1);

    pool_free(test);
    pool_free_zero_list_print(unittest1);
    pool_free_not_zero_list_print(unittest1);
    pool_allocation_list_print(unittest1);

    STANDARD_ASSERT(testfail == NULL);

    memory_pool_destroy(unittest1);
    pools_list_print();
}

void ut_pool_allocation_test(void)
{
    memory_pool_st* unittest1;
    STANDARD_RESULT res = memory_pool_create("unittest1", MiB(10), &unittest1);
    (void)res;
    STANDARD_ASSERT(res == 0);
    pools_list_print();
    pool_free_zero_list_print(unittest1);
    pool_free_not_zero_list_print(unittest1);
    pool_allocation_list_print(unittest1);

    // first to last
    void* test1 = pool_allocate(100, unittest1, MEMORY_TAG_UNTAGGED, true);
    void* test2 = pool_allocate(100, unittest1, MEMORY_TAG_UNTAGGED, true);
    void* test3 = pool_allocate(100, unittest1, MEMORY_TAG_UNTAGGED, true);
    void* test4 = pool_allocate(100, unittest1, MEMORY_TAG_UNTAGGED, true);
    void* test5 = pool_allocate(100, unittest1, MEMORY_TAG_UNTAGGED, true);
    void* test6 = pool_allocate(100, unittest1, MEMORY_TAG_UNTAGGED, true);
    void* test7 = pool_allocate(100, unittest1, MEMORY_TAG_UNTAGGED, true);
    void* test8 = pool_allocate(100, unittest1, MEMORY_TAG_UNTAGGED, true);
    void* test9 = pool_allocate(100, unittest1, MEMORY_TAG_UNTAGGED, true);
    void* test10 = pool_allocate(100, unittest1, MEMORY_TAG_UNTAGGED, true);
    pool_free_zero_list_print(unittest1);
    pool_free_not_zero_list_print(unittest1);
    pool_allocation_list_print(unittest1);
    pool_free(test1);
    pool_free(test2);
    pool_free(test3);
    pool_free(test4);
    pool_free(test5);
    pool_free(test6);
    pool_free(test7);
    pool_free(test8);
    pool_free(test9);
    pool_free(test10);

    pool_free_zero_list_print(unittest1);
    pool_free_not_zero_list_print(unittest1);
    pool_allocation_list_print(unittest1);

    // last to first
    void* test11 = pool_allocate(100, unittest1, MEMORY_TAG_UNTAGGED, true);
    void* test12 = pool_allocate(100, unittest1, MEMORY_TAG_UNTAGGED, true);
    void* test13 = pool_allocate(100, unittest1, MEMORY_TAG_UNTAGGED, true);
    void* test14 = pool_allocate(100, unittest1, MEMORY_TAG_UNTAGGED, true);
    void* test15 = pool_allocate(100, unittest1, MEMORY_TAG_UNTAGGED, true);
    void* test16 = pool_allocate(100, unittest1, MEMORY_TAG_UNTAGGED, true);
    void* test17 = pool_allocate(100, unittest1, MEMORY_TAG_UNTAGGED, true);
    void* test18 = pool_allocate(100, unittest1, MEMORY_TAG_UNTAGGED, true);
    void* test19 = pool_allocate(100, unittest1, MEMORY_TAG_UNTAGGED, true);
    void* test20 = pool_allocate(100, unittest1, MEMORY_TAG_UNTAGGED, true);
    pool_free_zero_list_print(unittest1);
    pool_free_not_zero_list_print(unittest1);
    pool_allocation_list_print(unittest1);
    pool_free(test20);
    pool_free(test19);
    pool_free(test18);
    pool_free(test17);
    pool_free(test16);
    pool_free(test15);
    pool_free(test14);
    pool_free(test13);
    pool_free(test12);
    pool_free(test11);

    pool_free_zero_list_print(unittest1);
    pool_free_not_zero_list_print(unittest1);
    pool_allocation_list_print(unittest1);

    memory_pool_destroy(unittest1);
    pools_list_print();
}

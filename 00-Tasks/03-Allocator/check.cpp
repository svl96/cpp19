#include <ctime>
#include <malloc.h>
#include "entry_point.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace Task03 {

    class Check : public ::testing::Test {
    protected:
        void* memory;
        size_t size;

        void SetUp() override {
            size = 1024;
            memory = malloc(size);
        }

        void TearDown() override {
            free(memory);
        }
    };

    TEST_F(Check, SimpleAllocate) {
        Allocator a(memory, size);
        void *ptr = a.Allocate(200);

        ASSERT_LE(memory, ptr);
        ASSERT_LE(ptr, (char*) memory + size);
    }

    TEST_F(Check, AllocateAndDeallocate) {
        Allocator a(memory, size);

        auto* ptr1 = (int*)a.Allocate(100);
        ptr1[0] = 5;
        a.Deallocate(ptr1);
        auto* ptr2 = (int*)a.Allocate(100);
        ptr2[0] = 9;
        ASSERT_EQ(ptr1[0], 9);
    }

    TEST_F(Check, AllocateTooMuch_ThrowsException) {
        Allocator a(memory, size);

        EXPECT_THROW({ a.Allocate(size + 1); }, NotEnoughMemory);
    }

    TEST_F(Check, FreeSegmentsAreMerged) {
        Allocator a(memory, size);

        void *x = a.Allocate(400);
        void *y = a.Allocate(400);
        a.Deallocate(y);
        a.Deallocate(x);

        a.Allocate(800);  // Doesn't throw
    }

    TEST_F(Check, AllocWithNoSpaceForOther) {
        Allocator a(memory, size);
        a.Allocate(1000);
        EXPECT_THROW({ a.Allocate(2); }, NotEnoughMemory);
    }

    TEST_F(Check, MergePartial) {
        Allocator a(memory, size);

        void *x = a.Allocate(900);
        a.Allocate(80);
        a.Deallocate(x);
    }

}

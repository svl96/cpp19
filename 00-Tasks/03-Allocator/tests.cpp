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

        std::vector<void*> Fill(Allocator a, size_t element_size) {
            std::vector<void*> pointers;
            void *ptr;
            for (size_t i = 0;;++i) {
                try {
                    ptr = a.Allocate(element_size);
                }
                catch (const NotEnoughMemory& e) {
                    break;
                }
                pointers.push_back(ptr);
            }
            return pointers;
        }


        double RunMemoryOverheadTest(size_t memory_size, size_t element_size) {
            auto mem = malloc(memory_size);
            Allocator a(mem, memory_size);

            auto pointers = Fill(a, element_size);
            double elements_total_size = element_size * pointers.size();
            free(mem);

            return elements_total_size / memory_size;
        }

    };

    TEST_F(Check, NoExtraDataStoredInClassFields) {
        Allocator a(memory, size);

        ASSERT_LE(sizeof(a), sizeof(void*) + sizeof(size_t));
    }

    TEST_F(Check, MemoryIsAllocatedFromGivenPool) {
        Allocator a(memory, size);

        void *ptr = a.Allocate(100);

        ASSERT_LE(memory, ptr);
        ASSERT_LE(ptr, (char*) memory + size);
    }

    TEST_F(Check, AllocateAndDeallocate) {
        Allocator a(memory, size);

        void *ptr = a.Allocate(100);
        a.Deallocate(ptr);
    }

    TEST_F(Check, AllocateTooMuch_ThrowsException) {
        Allocator a(memory, size);

        EXPECT_THROW({ a.Allocate(size + 1); }, NotEnoughMemory);
    }

    TEST_F(Check, AllocatedRegionsDontOverlap) {
        Allocator a(memory, size);

        size_t size_1 = 300;
        size_t size_2 = 500;

        char value_1 = 11;
        char value_2 = 22;

        void *ptr_1 = a.Allocate(size_1);
        void *ptr_2 = a.Allocate(size_2);
        memset(ptr_1, value_1, size_1);
        memset(ptr_2, value_2, size_2);
        for (size_t i = 0; i < size_1; ++i) {
            ASSERT_EQ((int) ((char*) ptr_1)[i], (int) value_1);
        }

        for (size_t i = 0; i < size_2; ++i) {
            ASSERT_EQ((int) ((char*) ptr_2)[i], (int) value_2);
        }

        a.Deallocate(ptr_1);
        a.Deallocate(ptr_2);
    }

    TEST_F(Check, FreeSegmentsAreMerged) {
        Allocator a(memory, size);

        void *x = a.Allocate(400);
        void *y = a.Allocate(400);
        a.Deallocate(x);
        a.Deallocate(y);

        a.Allocate(800);  // Doesn't throw
    }


    TEST_F(Check, AllocateAndFreeManyTimes) {
        Allocator a(memory, size);

        auto pointers = Fill(a, 10);
        size_t initial_count = pointers.size();
        ASSERT_GE(initial_count, (size_t) 20);

        for (auto& ptr : pointers) {
            a.Deallocate(ptr);
        }

        pointers = Fill(a, 10);

        ASSERT_EQ(pointers.size(), initial_count);
    }


    TEST_F(Check, OverheadFor1ByteObjects) {
        ASSERT_LE(RunMemoryOverheadTest(10000, 1), 0.6);
    }

}

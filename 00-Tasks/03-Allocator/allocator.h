#pragma once

#include "exceptions.h"
#include <utility>

class Allocator {
public:
    Allocator(void *memory, size_t size);
    void* Allocate(size_t size);
    void Deallocate(void *ptr);

private:
    size_t size_;
    void* mem_;

    void MergeFreeBlocks();
};
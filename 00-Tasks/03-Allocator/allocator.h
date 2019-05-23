#pragma once

#include <glob.h>
#include "exceptions.h"

class Allocator {
public:
    Allocator(void *memory, size_t size);
    void* Allocate(size_t size);
    void Deallocate(void *ptr);
};
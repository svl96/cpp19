//
// Created by svl on 5/23/19.
//

#include <cstring>
#include <iostream>
#include "allocator.h"

#define METASIZE 8

struct MetaInfo {
    size_t size;
    size_t allocated;
};



Allocator::Allocator(void *memory, size_t size) : size_(size), mem_(memory) {
    auto info = static_cast<MetaInfo*>(mem_);
    info->size = size;
    info->allocated = false;
}

void *Allocator::Allocate(size_t size) {

    // 1. Получаем новый размер, который будет выровнен, новый размер, который придется выделять.
    // 2. Находим блок в памяти, который будет подходящего размера

    size_t new_size = sizeof(MetaInfo) + size + (sizeof(size_t) - (size % sizeof(size_t))) % sizeof(size_t);


    auto ch_mem = (char*)mem_;
    size_t start = 0;
    auto info = (MetaInfo*)&(ch_mem[start]);
    while (start < size_) {
        info = (MetaInfo*)&(ch_mem[start]);
        if (!info->allocated && info->size >= new_size) {
            break;
        }
        start += info->size;
    }

    if (start >= size_) {
        throw NotEnoughMemory();
    }

    auto next = (MetaInfo*)&(ch_mem[start + new_size]);
    next->size = info->size - new_size;
    next->allocated = false;

    return info + sizeof(MetaInfo);
}

void Allocator::Deallocate(void *ptr) {
    MetaInfo* info = (MetaInfo*)(ptr) - 1;
    info->allocated = false;
    MergeFreeBlocks();
}

void Allocator::MergeFreeBlocks() {

    auto ch_mem = (char*)mem_;

    size_t offset = 0;
    auto info = (MetaInfo*)&(ch_mem[0]);

    while (offset < size_) {
        while (info->allocated) {
            offset += info->size;
            if (offset >= size_) {
                break;
            }
            info = (MetaInfo*)&(ch_mem[offset]);
        }
        if (offset >= size_) {
            break;
        }
        size_t start = offset;
        while (!info->allocated) {
            offset += info->size;
            if (offset >= size_) {
                break;
            }
            info = (MetaInfo*)&(ch_mem[offset]);
        }
        info = (MetaInfo*)&(ch_mem[start]);
        info->allocated = false;
        info->size = offset >= size_ ? size_ - start : offset;
        memset(&ch_mem[start], 0, info->size);
    }

}

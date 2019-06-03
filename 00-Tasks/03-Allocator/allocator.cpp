//
// Created by svl on 5/23/19.
//

#include <cstring>
#include <iostream>
#include "allocator.h"


Allocator::Allocator(void *memory, size_t size) : size_(size), mem_(memory) {
    auto info = static_cast<MetaInfo*>(mem_);
    info->size = size;
    info->allocated = false;
}

void *Allocator::Allocate(size_t size) {

    // 1. Получаем новый размер, который будет выровнен, новый размер, который придется выделять.
    // 2. Находим блок в памяти, который будет подходящего размера
    size_t allocate_size = sizeof(MetaInfo) + size + (sizeof(size_t) - (size % sizeof(size_t))) % sizeof(size_t);

    size_t block_start = 0;
    auto ch_mem_ptr = (char*)mem_;
    MetaInfo* info = nullptr;

    while (block_start + allocate_size <= size_) {
        info = (MetaInfo*)&(ch_mem_ptr[block_start]);
        if (!info->allocated && allocate_size <= info->size) {
            break;
        }
        block_start += info->size;
    }

    if (info == nullptr || block_start + allocate_size > size_) {
        throw NotEnoughMemory();
    }

    if (allocate_size + sizeof(MetaInfo) <= info->size) {
        auto next = (MetaInfo *) &(ch_mem_ptr[block_start + allocate_size]);
        next->size = info->size - allocate_size;
        next->allocated = false;
    } else {
        allocate_size = info->size;
    }

    info->allocated = true;
    info->size = allocate_size;

    return info + 1;
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

    while (offset < size_ && info->size != 0) {
        while (info->allocated) {
            offset += info->size;
            if (offset >= size_ || info->size < sizeof(MetaInfo)) {
                return;
            }
            info = (MetaInfo*)&(ch_mem[offset]);
        }
        if (offset >= size_) {
            break;
        }
        size_t start = offset;
        while (!info->allocated) {
            offset += info->size;
            if (offset >= size_ || info->size == 0) {
                offset = size_;
                break;
            }
            info = (MetaInfo*)&(ch_mem[offset]);
        }

        info = (MetaInfo*)&(ch_mem[start]);
        info->allocated = false;
        info->size = offset - start;
        memset(&ch_mem[start + sizeof(MetaInfo)], 0, info->size - sizeof(MetaInfo));
    }
}

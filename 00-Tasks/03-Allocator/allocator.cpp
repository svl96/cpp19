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

    size_t new_size = sizeof(MetaInfo) + size + (sizeof(int) - (size % sizeof(int))) % sizeof(int);

//    std::cout << "new size " << new_size << " meta " << sizeof(MetaInfo) << " size " << size << std::endl;

    auto ch_mem = (char*)mem_;
    size_t start = 0;
    auto info = (MetaInfo*)&(ch_mem[start]);
    while (start < size_) {
        info = (MetaInfo*)&(ch_mem[start]);
//        std::cout << "allocated " << info->allocated << std::endl;
        if (!info->allocated && info->size >= new_size) {
            break;
        }
        start += info->size;
//        std::cout << "start in " << start << std::endl;
        if (info->size == 0) {
            break;
        }
    }
//    std::cout << "start " << start << std::endl;
    if (start >= size_) {
        throw NotEnoughMemory();
    }

    info->size = new_size;
    info->allocated = true;
//    std::cout << "new " << start + new_size << std::endl;
    auto next = (MetaInfo*)&(ch_mem[start + new_size]);
    next->size = info->size - new_size;
    next->allocated = false;
//    std::cout << "next " << next->size << std::endl;
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
            if (offset >= size_ || info->size == 0) {
                break;
            }
            info = (MetaInfo*)&(ch_mem[offset]);
        }
        if (offset >= size_) {
            break;
        }
        size_t start = offset;
        std::cout << "sec while" << std::endl;
        while (!info->allocated) {
            offset += info->size;
            std::cout << "offset " << offset << std::endl;
            if (offset >= size_ || info->size == 0) {
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

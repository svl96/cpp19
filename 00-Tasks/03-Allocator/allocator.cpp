//
// Created by svl on 5/23/19.
//

#include <cstring>
#include <iostream>
#include "allocator.h"

#define METASIZE 8

struct MetaInfo {
    int size;
    int allocated;
};


Allocator::Allocator(void *memory, size_t size) : size_(size), mem_(memory) {
    auto info = static_cast<MetaInfo*>(mem_);
    info->size = size;
    info->allocated = 0;
}


std::pair<void*, size_t> Allocator::GetFreeBlock(size_t min_size) {
    auto start_int = static_cast<char*>(mem_);
    auto *block = (MetaInfo*)start_int;
    size_t pos = 0;
    size_t total_size = 0;
//    std::cout <<  "before get free " << std::endl;

    while (pos < size_ - METASIZE) {
        std::cout << "pos " << pos << std::endl;
        while (block->allocated) {
            std::cout << "first" << std::endl;
            pos += block->size + METASIZE;
            start_int += block->size + METASIZE;
            block = (MetaInfo*)start_int;
        }
        std::cout << "pos " << pos << std::endl;
        total_size = 0;
        while(!block->allocated) {
            std::cout << "sec" << std::endl;
            pos += block->size + METASIZE;
            total_size += block->size + METASIZE;
            start_int += block->size + METASIZE;
            block = (MetaInfo*)start_int;
        }
        std::cout << "pos " << pos << std::endl;
        if (total_size >= min_size + METASIZE) {
            break;
        }
    }
//    std::cout <<  "after get free " << std::endl;
    return {start_int - total_size, total_size};
}

void *Allocator::Allocate(size_t size) {
    size_t align_size = size % sizeof(int) == 0 ? size : (size / sizeof(int) + 1) * sizeof(int);

    std::cout << "meta size " << sizeof(MetaInfo) << std::endl;
    if (align_size + sizeof(MetaInfo) > size_) {
        throw NotEnoughMemory();
    }

    auto res = GetFreeBlock(align_size);

    char* start = (char *)res.first;
    size_t block_size = res.second;
//    std::cout << "align size " << align_size << " start " << start << " block " << block_size << std::endl;
    if (block_size < align_size) {
        throw NotEnoughMemory();
    }

    memset(start, 0, block_size);
    auto info = static_cast<MetaInfo*>((void *)start);

    info->size = align_size;
    info->allocated = true;

    info = static_cast<MetaInfo*>((void *)(start + (align_size / sizeof(int))+ METASIZE));
    info->size = block_size - (align_size / sizeof(int)) - METASIZE;
    info->allocated = false;

    return start + METASIZE;
}

void Allocator::Deallocate(void *ptr) {
    MetaInfo* info = static_cast<MetaInfo *>(ptr) - 1;
    info->allocated = false;
}

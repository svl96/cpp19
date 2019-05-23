//
// Created by svl on 5/23/19.
//

#include <cstring>
#include "allocator.h"

#define METASIZE 2

struct MetaInfo {
    size_t size;
    bool flag;
};



Allocator::Allocator(void *memory, size_t size) : size_(size), mem_(memory) {}


std::pair<int*, size_t> Allocator::GetFreeBlock(int* start_ptr, size_t min_size) {
    auto start_int = start_ptr;
    auto mem_int = static_cast<int*>(mem_);
    auto *block = (MetaInfo*)start_int;
    size_t pos = start_int - mem_int;
    size_t total_size = 0;

    while (pos < size_ / 4) {
        while (block->flag) {
            start_int += block->size + sizeof(block);
            block = (MetaInfo*)start_int;
        }
        total_size = 0;
        while(!block->flag) {
            total_size += block->size + sizeof(block);
        }
        if (total_size >= min_size + sizeof(block)) {
            break;
        }
    }
    return {start_int, total_size};
}

void *Allocator::Allocate(size_t size) {
    size_t size_int = size % 4 == 0 ? size / 4 : size / 4 + 1;
    auto mem_int = static_cast<int*>(mem_);
    size_t total_size = size / 4;
    size_t pos = 0;
    auto *block = (MetaInfo*)(mem_int);
    // берем стартовый блок
    // надо хранить ходить не по char* а по int*
    // чтобы у нас было нормлаьное выравнивание.
    // ищем максимально большое свободное место.

    auto res = GetFreeBlock(mem_int, size_int);

    int* start = res.first;
    size_t block_size = res.second;

    memset(start, 0, block_size);
    auto info = static_cast<MetaInfo*>((void *)start);

    info->size = size_int;
    info->flag = true;

    int* next = start + size_int + METASIZE;
    block_size -= size_int - METASIZE;

    info = static_cast<MetaInfo*>((void *)next);
    info->size = block_size;
    info->flag = false;

    return start + METASIZE;
}

void Allocator::Deallocate(void *ptr) {
    MetaInfo* info = static_cast<MetaInfo *>(ptr) - 1;
    info->flag = false;
}

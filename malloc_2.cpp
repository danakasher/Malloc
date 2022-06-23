#include <cstring>
#include "malloc_2.h"

MemoryBlocks memory_blocks = MemoryBlocks();

void *malloc_2::smalloc(size_t size) {
    if(size == 0 || size > (long) pow(10, 8)) return nullptr;

    void *program_break = memory_blocks.allocate(size);
    if(program_break == nullptr){
        return nullptr;
    }

    return (char *)program_break + sizeof(MetaData);
}

void *malloc_2::scalloc(size_t num, size_t size) {
    void *program_break = smalloc(num * size);
    if(program_break == nullptr)
        return nullptr;

    memset(program_break, 0, num*size);
    return program_break;
}

void malloc_2::sfree(void *p) {
    if(p != nullptr){
        MetaData *block = (MetaData* )((char *)p - sizeof(MetaData));
        block->is_free = true;
    }
}

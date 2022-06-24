#include <cstring>
#include "malloc_3.h"

MemoryBlocks memory_blocks = MemoryBlocks();

void *malloc_3::smalloc(size_t size) {
    if(size == 0 || size > (long) pow(10, 8)) return nullptr;

    void *program_break = memory_blocks.allocate(size);
    if(program_break == nullptr){
        return nullptr;
    }

    return (char *)program_break + sizeof(MetaData);
}

void *malloc_3::scalloc(size_t num, size_t size) {
    void *program_break = smalloc(num * size);
    if(program_break == nullptr)
        return nullptr;

    memset(program_break, 0, num*size);
    return program_break;
}

void malloc_3::sfree(void *p) {
    if(p != nullptr){
        MetaData *block = (MetaData* )((char *)p - sizeof(MetaData));
        block->is_free = true;
    }
}

void *malloc_3::srealloc(void *oldp, size_t size) {
    if(size == 0 || size > (long) pow(10, 8)) return nullptr;
    if(oldp == nullptr) return smalloc(size);

    MetaData *old_block = (MetaData* )((char *)oldp - sizeof(MetaData));
    if(old_block->size > size) return old_block;

    void *program_break = smalloc(size);
    memcpy(program_break, old_block, old_block->size);
    sfree(oldp);
    return program_break;
}

size_t malloc_3::_num_free_blocks() {
    return memory_blocks.numOfFreeBlocks();
}

size_t malloc_3::_num_free_bytes() {
    return memory_blocks.numOfFreeBytes();
}

size_t malloc_3::_num_allocated_blocks() {
    return memory_blocks.numOfAllocatedBlocks();
}

size_t malloc_3::_num_allocated_bytes() {
    return memory_blocks.numOfAllocatedBytes();
}

size_t malloc_3::_num_meta_data_bytes() {
    return memory_blocks.numOfMetaDataBytes();
}

size_t malloc_3::_size_meta_data() {
    return sizeof(MetaData);
}

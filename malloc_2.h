#ifndef MALLOC_MALLOC_2_H
#define MALLOC_MALLOC_2_H
#include  <unistd.h>
#include <cmath>

typedef struct MetaData{
    size_t size;
    bool is_free;
    MetaData *next;
    MetaData *prev;
} MetaData;

class MemoryBlocks{
private:
    MetaData *head = nullptr;
public:
    MemoryBlocks() = default;
    void* allocate(size_t size){
        MetaData *curr_block = head;
        while(curr_block != nullptr){
            if(curr_block->is_free && size <= curr_block->size){
                curr_block->is_free = false;
                return curr_block;
            }
            // If curr_block is the last in the list, exit and use it to chain the new block
            if(curr_block->next == nullptr) break;

            curr_block = curr_block->next;
        }
        void *program_break = sbrk((intptr_t) size);
        if(program_break == (void *) -1){
            return nullptr;
        }

        auto *block = (MetaData *) program_break;
        block->is_free = false;
        block->size = size;
        block->next = nullptr;
        block->prev = curr_block;

        return block;
    }
};

class malloc_2 {
    void* smalloc(size_t size);
    void* scalloc(size_t num, size_t size);
    void sfree(void *p);
    void* srealloc(void* oldp, size_t size);
};




#endif //MALLOC_MALLOC_2_H

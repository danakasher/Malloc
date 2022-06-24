#ifndef MALLOC_MALLOC_3_H
#define MALLOC_MALLOC_3_H

#include  <unistd.h>
#include <cmath>

typedef struct MetaData {
    size_t size;
    bool is_free;
    MetaData *next_by_size;
    MetaData *next_by_address;
    MetaData *prev_by_size;
    MetaData *prev_by_address;
} MetaData;

class MemoryBlocks {
private:
    MetaData *head_by_size =
public:
    MemoryBlocks() = default;

    void *allocate(size_t size) {
        MetaData *curr_block = head_by_size;
        while (curr_block != nullptr) {
            if (curr_block->is_free && size <= curr_block->size) {
                if(curr_block->size - size - sizeof(MetaData) > 128){
                    split(curr_block, size);
                }
                curr_block->is_free = false;
                return curr_block;
            }
            // If curr_block is the last in the list, exit and use it to chain the new block
            if (curr_block->next_by_size == nullptr) break;

            curr_block = curr_block->next_by_size;
        }
        void *program_break = sbrk((intptr_t) size);
        if (program_break == (void *) -1) {
            return nullptr;
        }

        auto *block = (MetaData *) program_break;
        block->is_free = false;
        block->size = size;
        insertNewBlock(block);
        insertByAddress(block);
        return block;
    }

    void removeFromSizeList(MetaData *block){
        if(block == head_by_size){
            head_by_size = block->next_by_size;
        } else {
            block->prev_by_size->next_by_size = block->next_by_size;
            block->next_by_size->prev_by_size = block->prev_by_size;
            block->next_by_size = nullptr;
            block->prev_by_size = nullptr;
        }
    }

    void removeFromAddressList(MetaData *block){
        if(block == head_by_address){
            head_by_address = block->next_by_address;
        } else {
            block->prev_by_address->next_by_address = block->next_by_address;
            block->next_by_address->prev_by_address = block->prev_by_address;
            block->next_by_address = nullptr;
            block->prev_by_address = nullptr;
        }
    }

    void split(MetaData *block, size_t size){
        size_t old_size = block->size;

        MetaData *split_block = block + block->size + sizeof(MetaData);
        split_block->size = old_size - size - sizeof(MetaData);
        split_block->is_free = true;

        split_block->prev_by_size = nullptr;
        split_block->prev_by_address = block;
        split_block->next_by_size = nullptr;
        split_block->next_by_address = block->next_by_address;
        if(split_block->next_by_address != nullptr && split_block->next_by_address->is_free){
            split_block = (MetaData *) mergeBlocks(split_block, 1);
        }

        removeFromSizeList(block);
        block->next_by_address = split_block;
        block->is_free = false;
        block->size = size;

        insertNewBlock(block);
        insertNewBlock(split_block);
    }

    void *mergeBlocks(MetaData *block, int dir){
        MetaData *merge_block, *removed_block;
        if(dir == 0){ // Left merge
            return mergeBlocks(block->prev_by_address, 1);
        } else {
            // Right merge
            merge_block = block;
            merge_block->size += merge_block->next_by_address->size;
            removed_block = merge_block->next_by_address;
            merge_block->next_by_address = merge_block->next_by_address->next_by_address;
            removeFromSizeList(removed_block);
            removeFromAddressList(removed_block);
        }
        return merge_block;
    }

    void insertNewBlock(MetaData *block){
        if(head_by_size == nullptr){
            block->prev_by_size = nullptr;
            block->next_by_size = nullptr;
            head_by_size = block;
            return;
        }
        MetaData *curr_block = head_by_size;
        if(block->size < curr_block->size || (block->size == curr_block->size && block < curr_block)){
            block->next_by_size = curr_block;
            curr_block->prev_by_size = block;
            head_by_size = block;
            return;
        }

        curr_block = head_by_size->next_by_size;
        while(curr_block != nullptr){
            if(block->size < curr_block->size || (block->size == curr_block->size && block < curr_block)){
                curr_block->prev_by_size->next_by_size = block;
                block->prev_by_size = curr_block->prev_by_size;
                curr_block->prev_by_size = block;
                block->next_by_size = curr_block;
                return;
            }
            if(curr_block->next_by_size == nullptr) break;
            curr_block = curr_block->next_by_size;
        }

        block->prev_by_size = curr_block;
        block->next_by_size = nullptr;
        curr_block->next_by_size = block;
    }

    void insertByAddress(MetaData *block){
        if(head_by_address == nullptr){
            block->prev_by_size = nullptr;
            block->next_by_size = nullptr;
            head_by_size = block;
            return;
        }
        MetaData *curr_block = head_by_address;
        if(block < curr_block){
            block->next_by_address = curr_block;
            curr_block->prev_by_address = block;
            head_by_address = block;
            return;
        }

        curr_block = head_by_address->next_by_address;
        while(curr_block != nullptr){
            if(block < curr_block){
                curr_block->prev_by_address->next_by_address = block;
                block->prev_by_address = curr_block->prev_by_address;
                curr_block->prev_by_address = block;
                block->next_by_address = curr_block;
                return;
            }
            if(curr_block->next_by_address == nullptr) break;
            curr_block = curr_block->next_by_address;
        }

        block->prev_by_address = curr_block;
        block->next_by_address = nullptr;
        curr_block->next_by_address = block;
    }

    size_t numOfFreeBlocks() {
        if (head_by_size == nullptr) return 0;
        size_t count = 0;
        MetaData *temp = head_by_size;
        while (temp != nullptr) {
            count += temp->is_free;
            temp = temp->next_by_size;
        }

        return count;
    }

    size_t numOfFreeBytes() {
        if (head_by_size == nullptr) return 0;
        size_t count = 0;
        MetaData *temp = head_by_size;
        while (temp != nullptr) {
            if (temp->is_free) {
                count += temp->size;
            }
            temp = temp->next_by_size;
        }

        return count;
    }

    size_t numOfAllocatedBlocks() {
        if (head_by_size == nullptr) return 0;
        size_t count = 0;
        MetaData *temp = head_by_size;
        while (temp != nullptr) {
            count += !temp->is_free;
            temp = temp->next_by_size;
        }

        return count;
    }

    size_t numOfAllocatedBytes() {
        if (head_by_size == nullptr) return 0;
        size_t count = 0;
        MetaData *temp = head_by_size;
        while (temp != nullptr) {
            if (!temp->is_free) {
                count += temp->size;
            }
            temp = temp->next_by_size;
        }

        return count;
    }

    size_t numOfMetaDataBytes() {
        if (head_by_size == nullptr) return 0;
        size_t count = 0;
        MetaData *temp = head_by_size;
        while (temp != nullptr) {
            count += sizeof(MetaData);
            temp = temp->next_by_size;
        }

        return count;
    }
};

class malloc_3 {
    size_t _num_free_blocks();
    size_t _num_free_bytes();
    size_t _num_allocated_blocks();
    size_t _num_allocated_bytes();
    size_t _num_meta_data_bytes();
    size_t _size_meta_data();

public:
    void *smalloc(size_t size);
    void *scalloc(size_t num, size_t size);
    void sfree(void *p);
    void *srealloc(void *oldp, size_t size);
};
#endif //MALLOC_MALLOC_3_H

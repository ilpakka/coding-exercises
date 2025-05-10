#include "mr_malloc.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>

// Memory block
typedef struct block {
    size_t size;
    int free;
    struct block* next;
} block_t;

static block_t* head = NULL;

// Find a free block that's large enough
static block_t* find_free_block(size_t size) {
    block_t* current = head;
    while (current) {
        if (current->free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Request more memory from the system
static block_t* request_space(size_t size) {
    block_t* block = sbrk(0);
    void* request = sbrk(size + sizeof(block_t));
    
    if (request == (void*)-1) {
        return NULL; // sbrk failed
    }

    block->size = size;
    block->free = 0;
    block->next = NULL;

    if (head) {
        block_t* current = head;
        while (current->next) {
            current = current->next;
        }
        current->next = block;
    } else {
        head = block;
    }

    return block;
}

void* mr_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }


    block_t* block = find_free_block(size);
    
    if (block) {
        block->free = 0;
        return (void*)(block + 1);
    }

    block = request_space(size);
    if (!block) {
        return NULL;
    }

    return (void*)(block + 1);
}

void mr_free(void* ptr) {
    if (!ptr) {
        return;
    }

    block_t* block = (block_t*)ptr - 1;
    block->free = 1;
}

void print_memory_map(void) {
    block_t* current = head;
    printf("Memory Map:\n");
    while (current) {
        printf("Block at %p: size=%zu, free=%d\n", 
               (void*)current, current->size, current->free);
        current = current->next;
    }
} 
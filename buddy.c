#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define MEMORY_SIZE 1024  // Memory pool size in KB

// block struture
typedef struct Block {
    int size;
    bool is_free;
    struct Block *buddy;
    struct Block *next;
} Block;

// Free List Array
Block *free_lists[11]; // Maximum log2(MEMORY_SIZE) + 1

// Initialization
void initialize_memory() {
    for (int i = 0; i < 11; i++) {
        free_lists[i] = NULL;
    }

    Block *initial_block = (Block *)malloc(sizeof(Block));
    initial_block->size = MEMORY_SIZE;
    initial_block->is_free = true;
    initial_block->buddy = NULL;
    initial_block->next = NULL;

    free_lists[(int)log2(MEMORY_SIZE)] = initial_block;
}

// Splitting a Block
void split_block(Block *block, int target_size) {
    while (block->size / 2 >= target_size) {
        int new_size = block->size / 2;

        Block *buddy = (Block *)malloc(sizeof(Block));
        buddy->size = new_size;
        buddy->is_free = true;
        buddy->buddy = block;
        buddy->next = block->next;

        block->size = new_size;
        block->buddy = buddy;
        block->next = buddy;
    }
}

// Allocating memory
Block *allocate_memory(int request_size) {
    int target_size = pow(2, ceil(log2(request_size)));
    int index = (int)log2(target_size);

    for (int i = index; i < 11; i++) {
        if (free_lists[i] != NULL) {
            Block *block = free_lists[i];
            free_lists[i] = block->next;

            split_block(block, target_size);
            block->is_free = false;

            int target_index = (int)log2(block->size);
            free_lists[target_index] = block->next;
            return block;
        }
    }

    printf("Memory allocation failed!\n");
    return NULL;
}
// Freeing memory
void free_memory(Block *block) {
    block->is_free = true;
    merge_buddies(block);

    int index = (int)log2(block->size);
    block->next = free_lists[index];
    free_lists[index] = block;
}

// Merging buddies
void merge_buddies(Block *block) {
    if (block->buddy && block->buddy->is_free) {
        block->size *= 2;
        block->is_free = true;
        block->next = block->buddy->next;
        free(block->buddy);
    }
}


// Printing memory state
void print_memory_state() {
    printf("Current Memory State:\n");
    for (int i = 0; i < 11; i++) {
        if (free_lists[i] != NULL) {
            printf("Blocks of size %d KB:\n", (int)pow(2, i));
            Block *current = free_lists[i];
            while (current) {
                printf("  - Block: %s\n", current->is_free ? "Free" : "Allocated");
                current = current->next;
            }
        }
    }
}

int main() {
    initialize_memory();

    printf("Initial Memory State:\n");
    print_memory_state();

    printf("\nStep 1: Allocating 100 KB\n");
    Block *block1 = allocate_memory(100);
    print_memory_state();

    printf("\nStep 2: Allocating 200 KB\n");
    Block *block2 = allocate_memory(200);
    print_memory_state();

    printf("\nStep 3: Freeing 128 KB Block\n");
    free_memory(block1);
    print_memory_state();

    printf("\nStep 4: Freeing 256 KB Block\n");
    free_memory(block2);
    print_memory_state();

    return 0;
}


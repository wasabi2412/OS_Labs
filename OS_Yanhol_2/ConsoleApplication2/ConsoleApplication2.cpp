#include <stdlib.h>
#include "iostream"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdint.h>

using namespace std;
#define ALLOCATED_S 6
#define ALLOCATED_L 16
#define LONGEST_ORDER 10
#define SHORTEST_ORDER 4
#define _MEMORYBASE  ((uintptr_t)BUDDY->pool)
#define _BUDDYOF(b, i)  (((uintptr_t)b - _MEMORYBASE) ^ (1 << (i)))
#define BUDDYOF(b, i)   ((pointer)( _BUDDYOF(b, i) + _MEMORYBASE))
typedef void* pointer;
typedef struct buddy {
    pointer freelist[LONGEST_ORDER + 2];
    uint8_t pool[(1 << LONGEST_ORDER)];
} buddy_t;

pointer bmalloc(int size);
void bfree(pointer block);
void show_result();
buddy_t* BUDDY = 0;

void buddy_init(buddy_t* buddy) {
    BUDDY = buddy;
    memset(BUDDY, 0, sizeof(buddy_t));
    BUDDY->freelist[LONGEST_ORDER] = BUDDY->pool;
}

void test001() {
    void *p1, *p2;
    p1 = bmalloc(ALLOCATED_S);
    p2 = bmalloc(ALLOCATED_L);
    bfree(p2);
    bfree(p1);
    show_result();
}

pointer bmalloc(int size) {

    int i, order;
    pointer block, buddy;
    i = 0;
    while ((1 << (i)) < size + 1)
        i++;

    order = i = (i < SHORTEST_ORDER) ? SHORTEST_ORDER : i;

    for (;; i++) {
        if (i > LONGEST_ORDER)
            return NULL;
        if (BUDDY->freelist[i])
            break;
    }
    block = BUDDY->freelist[i];
    BUDDY->freelist[i] = *(pointer*)BUDDY->freelist[i];

    while (i-- > order) {
        buddy = BUDDYOF(block, i);
        BUDDY->freelist[i] = buddy;
    }

    *(((uint8_t*)block - 1)) = order;
    return block;
}

void bfree(pointer block) {
    int i;
    pointer buddy;
    pointer* _pointer;
    i = *(((uint8_t*)block - 1));

    for (;; i++) {
        buddy = BUDDYOF(block, i);
        _pointer = &(BUDDY->freelist[i]);
        while ((*_pointer != NULL) && (*_pointer != buddy))
            _pointer = (pointer*)*_pointer;
        if (*_pointer != buddy) {
            *(pointer*)block = BUDDY->freelist[i];
            BUDDY->freelist[i] = block;
            return;
        }
        block = (block < buddy) ? block : buddy;
    }
}

static int count_blocks(int i) {
    int count = 0;
    pointer* p = &(BUDDY->freelist[i]);
    while (*p != NULL) {
        count++;
        p = (pointer*)*p;
    }
    return count;
}

static int total_free() {
    int i, NumberOfBytes = 0;
    for (i = 0; i <= LONGEST_ORDER; i++) {
        NumberOfBytes += count_blocks(i) * (1 << (i));
    }
    return NumberOfBytes;
}

static void print_block(int i) {
    pointer* _pointer = &BUDDY->freelist[i];
    while (*_pointer != NULL) {
        cout << "| Memory block: ";
        printf("| 0x%08lx                |\n", (uintptr_t)*_pointer);
        cout << "|-------------------------------------------|\n";
        _pointer = (pointer*)*_pointer;
    }
}

void show_result() {
    int iterator;
    cout << "|-------------------------------------------|\n";
    printf("| released: %d                            |\n", total_free());
    printf("| MEMORY POOL start point =  @0x%08x    |\n", (unsigned int)(uintptr_t)BUDDY->pool);
    for (iterator = 0; iterator <= LONGEST_ORDER; iterator++) {
        print_block(iterator);
    }
}

int main() {
    buddy_t* buddy = (buddy_t*)malloc(sizeof(buddy_t));
    buddy_init(buddy);
    test001();
    BUDDY = 0;
    free(buddy);
    return 0;
}

#ifndef LAZY_ALLOCATOR_H
#define LAZY_ALLOCATOR_H

#include <stdlib.h>
#include <stddef.h>

typedef struct LazyAllocatorSegment
{
    void *ptr;
    struct LazyAllocatorSegment *next;
} LazyAllocatorSegment;

typedef LazyAllocatorSegment LazyAllocator;

static inline LazyAllocatorSegment *CreateLazyAllocatorSegment(size_t size)
{
    LazyAllocatorSegment *segment = (LazyAllocatorSegment *)malloc(sizeof(LazyAllocatorSegment));
    if (!segment)
        return NULL;

    segment->ptr = malloc(size);
    if (!segment->ptr)
    {
        free(segment);
        return NULL;
    }

    segment->next = NULL;
    return segment;
}

static inline void *AllocateFromLazyAllocator(LazyAllocatorSegment **allocator, size_t size)
{
    LazyAllocatorSegment *segment = CreateLazyAllocatorSegment(size);
    if (!segment)
        return NULL;

    segment->next = *allocator;
    *allocator = segment;
    return segment->ptr;
}

static inline void *ReallocFromLazyAllocator(LazyAllocatorSegment **allocator, void *oldPtr, size_t newSize)
{
    LazyAllocatorSegment *current = *allocator;

    while (current)
    {
        if (current->ptr == oldPtr)
        {
            void *new_ptr = realloc(current->ptr, newSize);
            if (!new_ptr)
                return NULL;

            current->ptr = new_ptr;
            return new_ptr;
        }
        current = current->next;
    }
    return NULL;
}

static inline void FreeFromLazyAllocator(LazyAllocatorSegment **allocator, void *ptr)
{
    LazyAllocatorSegment *current = *allocator;
    LazyAllocatorSegment *prev = NULL;

    while (current)
    {
        if (current->ptr == ptr)
        {
            if (prev)
                prev->next = current->next;
            else
                *allocator = current->next;

            free(current->ptr);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

static inline void FreeLazyAllocator(LazyAllocatorSegment **allocator)
{
    LazyAllocatorSegment *current = *allocator;
    while (current)
    {
        LazyAllocatorSegment *next = current->next;
        free(current->ptr);
        free(current);
        current = next;
    }
    *allocator = NULL;
}

#endif // LAZY_ALLOCATOR_H


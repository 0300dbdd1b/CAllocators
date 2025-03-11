#ifndef LINEAR_ALLOCATOR_H
#define LINEAR_ALLOCATOR_H

#include <cstdint>
#include <stdlib.h>

#if defined(__x86_64__)
	#define WORD_SIZE 8
#else
	#define WORWORD_SIZE 4
#endif

typedef struct LinearAllocator
{
	char    *memory;
	size_t  capacity;
	size_t  usedMemory;
} LinearAllocator;
typedef LinearAllocator Arena;
typedef LinearAllocator ArenaAllocator;


static inline void *_align_forward(void *ptr, size_t alignment)
{
	uintptr_t 	addr	= (uintptr_t)ptr;
	uintptr_t	aligned	= (addr + (alignment - 1)) & ~(alignment - 1);
	return (void *)aligned;
}


static inline LinearAllocator *CreateLinearAllocator(size_t size)
{
	LinearAllocator *allocator = (LinearAllocator *)malloc(sizeof(LinearAllocator));
	if (!allocator) return NULL;

	allocator->memory = (char *)malloc(size);
	if (!allocator->memory)
	{
		free(allocator);
		return NULL;
	}

	allocator->capacity		= size;
	allocator->usedMemory = 0;
	return allocator;
}
#define CreateArena				CreateLinearAllocator
#define CreateArenaAllocator	CreateLinearAllocator

static inline void *AllocateFromLinearAllocator(LinearAllocator *allocator, size_t size)
{
    if (!allocator) return NULL;
    void	*curr_pos			= allocator->memory + allocator->usedMemory;
    void	*aligned_pos		= _align_forward(curr_pos, WORD_SIZE);
    size_t	alignment_offset	= (char *)aligned_pos - (char *)curr_pos;
    size_t	total_required		= alignment_offset + size;
    if (allocator->usedMemory + total_required > allocator->capacity) return NULL;
    allocator->usedMemory += total_required;
    return (aligned_pos);
}

#define AllocateFromArena			AllocateFromLinearAllocator
#define AllocateFromArenaAllocator	AllocateFromLinearAllocator

static inline void FreeSizeFromLinearAllocator(LinearAllocator *allocator, size_t size)
{
	if (!allocator || size > allocator->usedMemory)
		return;

	allocator->usedMemory -= size;
}
#define FreeSizeFromArena			FreeSizeFromLinearAllocator
#define FreeSizeFromArenaAllocator	FreeSizeFromLinearAllocator

static inline void FreeLinearAllocator(LinearAllocator *allocator)
{
	if (!allocator)
		return;

	allocator->usedMemory = 0;
}
#define FreeArena				FreeLinearAllocator
#define FreeArenaAllocator		FreeLinearAllocator

static inline void DestroyLinearAllocator(LinearAllocator *allocator)
{
	if (!allocator)
		return;

	free(allocator->memory);
	allocator->memory = NULL;
	allocator->capacity = 0;
	allocator->usedMemory = 0;
}
#define DestroyArena			DestroyLinearAllocator
#define DestroyArenaAllocator	DestroyLinearAllocator

#endif // LINEAR_ALLOCATOR_H


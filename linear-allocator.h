
#ifndef LINEAR_ALLOCATOR_H
#define LINEAR_ALLOCATOR_H

#include <stdlib.h>

typedef struct LinearAllocator
{
	char    *memory;
	size_t  capacity;
	size_t  usedMemory;
} LinearAllocator;

typedef LinearAllocator Arena;
typedef LinearAllocator ArenaAllocator;

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
#define CreateArena CreateLinearAllocator
#define CreateArenaAllocator CreateLinearAllocator

static inline void *AllocateFromLinearAllocator(LinearAllocator *allocator, size_t size)
{
	if (!allocator || size + allocator->usedMemory > allocator->capacity)
		return NULL;

	void *ptr = allocator->memory + allocator->usedMemory;
	allocator->usedMemory += size;
	return ptr;
}
#define AllocateFromArena AllocateFromLinearAllocator
#define AllocateFromArenaAllocator AllocateFromLinearAllocator

static inline void FreeSizeFromLinearAllocator(LinearAllocator *allocator, size_t size)
{
	if (!allocator || size > allocator->usedMemory)
		return;

	allocator->usedMemory -= size;
}
#define FreeSizeFromArena FreeSizeFromLinearAllocator
#define FreeSizeFromArenaAllocator FreeSizeFromLinearAllocator

static inline void FreeLinearAllocator(LinearAllocator *allocator)
{
	if (!allocator)
		return;

	allocator->usedMemory = 0;
}
#define FreeArena FreeLinearAllocator
#define FreeArenaAllocator FreeLinearAllocator

static inline void DestroyLinearAllocator(LinearAllocator *allocator)
{
	if (!allocator)
		return;

	free(allocator->memory);
	allocator->memory = NULL;
	allocator->capacity = 0;
	allocator->usedMemory = 0;
}
#define DestroyArena DestroyLinearAllocator
#define DestroyArenaAllocator DestroyLinearAllocator

#endif // LINEAR_ALLOCATOR_H


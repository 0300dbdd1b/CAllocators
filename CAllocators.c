#include "CAlloctors.h"
#include "platform.h"
#include <stdint.h>

static inline uintptr_t	_align_forward_(uintptr_t addr, uintptr_t alignment)
{
	return ((addr + (alignment - 1)) & ~(alignment - 1));
}

LinearAllocator	*CreateLineaarAllocator(U64 initialSize)
{
	LinearAllocator *allocator = malloc(sizeof(LinearAllocator));
	if (!allocator)
		return (NULL);
	allocator->memory = malloc(sizeof(U8) * initialSize);
	if (!allocator->memory)
	{
		free(allocator);
		return (NULL);
	}
	allocator->totalSize = initialSize;
	allocator->usedMemory = 0;
	return (allocator);
}

void	*AllocateFromLinearAllocatorWithAlignment(LinearAllocator *allocator, U64 size, U64 alignment)
{
	if (size == 0) return (NULL);
	uintptr_t	current 					= (uintptr_t)(allocator->memory + allocator->usedMemory);
	uintptr_t	aligned						= _align_forward_(current, alignment);
	U64			padding						= aligned - current;
	U64			totalAllocatorSizeNeeded	= allocator->usedMemory + padding + size;
	if (totalAllocatorSizeNeeded > allocator->totalSize)
	{
		U64 newSize = allocator->totalSize * LINEAR_ALLOCATOR_GROWTH_FACTOR;
		if (newSize < totalAllocatorSizeNeeded)
			newSize = totalAllocatorSizeNeeded;
		U8 *newMemory = realloc(allocator->memory, newSize);
		if (!newMemory) return (NULL);
		allocator->memory = newMemory;
		allocator->totalSize = newSize;
		current = (uintptr_t)(allocator->memory + allocator->usedMemory);
		aligned = _align_forward_(current, alignment);
		padding = aligned - current;
	}
	allocator->usedMemory += size + padding;
	return ((void *)aligned);
}

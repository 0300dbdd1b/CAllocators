#ifndef CAllocators
#define CAllocators

#include "platform.h"

#define LINEAR_ALLOCATOR_GROWTH_FACTOR 1.5
typedef	U32	ID;

typedef struct LinearAllocator
{
	U8		*memory;
	U64		totalSize;
	U64		usedMemory;
} LinearAllocator;




LinearAllocator		*CreateLineaarAllocator(U64 initialSize);
void				*AllocateFromLinearAllocatorWithAlignment(LinearAllocator *allocator, U64 size, U64 alignment);
#define AllocateFromLinearAllocator(allocator, type, count) (type *)AllocateFromLinearAllocatorWithAlignment(allocator, sizeof(type) * (count), ALIGNOF(type))

#endif // !CAllocators

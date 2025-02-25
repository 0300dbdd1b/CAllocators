#include "CAlloctors.h"


int main()
{
	LinearAllocator *alloc = CreateLineaarAllocator(10);

	int *tab = AllocateFromLinearAllocator(alloc, int, 100);

	for (int i = 0; i < 100; i++)
	{
		tab[i] = i;
		printf("%d\n", tab[i]);
	}
}

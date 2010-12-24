#include <windows.h>
#include "osal_types.h"
#include "osal_memory.h"

#include <stdlib.h>

/*---------------------------------------------------------------------------*/

void * Evmalloc(size_t size)
{
	//add debug code to check memory loss
	return malloc(size);
}

/*---------------------------------------------------------------------------*/

void Evfree(void *buffer)
{
    free(buffer);
}

/*---------------------------------------------------------------------------*/

void * Evrealloc(void *buffer, size_t size)
{
	return realloc(buffer, size);
}

/*---------------------------------------------------------------------------*/

void * Evcalloc(size_t size1, size_t size2)
{
	return calloc(size1, size2);
}


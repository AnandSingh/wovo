#include <stdarg.h> 


#include <stdio.h>

#include "osal_string.h"
#include "osal_stdio.h"



int  Evvprintf(const char * format, va_list ptr)
{
	return vprintf(format, ptr);
}

int  Evprintf(const char * format, ...)
{
	int	iRes = 0;
	va_list ptr;
	va_start(ptr, format);

	iRes = Evvprintf(format, ptr);
	
	va_end(ptr);

	return iRes;
}

void Evperror(const char * ErrMsg)
{
	Evprintf("%s", ErrMsg);
}



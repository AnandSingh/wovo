#include "osal_string.h"
#include "osal_memory.h"
#include <string.h> 
#include <stdlib.h> /* strtoul */
#include <stdarg.h> /* var args */
#include <stdio.h>  /* vsprintf */

void *Evmemset(void *s, int c, size_t n)
{
    return memset(s, c, n);
}

/*---------------------------------------------------------------------------*/

void *Evmemcpy(void *s1, const void *s2, size_t n)
{
    return memcpy(s1, s2, n);
}

/*---------------------------------------------------------------------------*/

void *Evmemmove(void *s1, const void *s2, size_t n)
{
    return memmove(s1, s2, n);
}

/*---------------------------------------------------------------------------*/

size_t Evstrlen(const char *s)
{
    return strlen(s);
}

/*---------------------------------------------------------------------------*/

char *Evstrcpy(char *s1, const char *s2)
{
    return strcpy(s1, s2);
}

/*---------------------------------------------------------------------------*/
#if 0
char *Evstrncpy(char *s1, const char *s2, size_t n)
{
    return strncpy(s1, s2, n);
}
#endif

/*---------------------------------------------------------------------------*/

char *Evstrcat(char *s1, const char *s2)
{
    return strcat(s1, s2);
}

/*---------------------------------------------------------------------------*/

int Evmemcmp(const void *s1, const void *s2, size_t n)
{
    return memcmp(s1, s2, n);
}

/*---------------------------------------------------------------------------*/

int Evstrcmp(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
}

/*---------------------------------------------------------------------------*/

int Evstrncmp(const char *s1, const char *s2, size_t n)
{
    return strncmp(s1, s2, n);
}

/*---------------------------------------------------------------------------*/
#if 0
char *Evstrstr(const char *s1, const char *s2)
{
    return strstr(s1, s2);
}
#endif

/*---------------------------------------------------------------------------*/

char *Evstrchr(const char *s, int c)
{
    return strchr(s, c);
}

/*---------------------------------------------------------------------------*/

char *Evstrrchr(const char *s, int c)
{
    return strrchr(s, c);
}

/*---------------------------------------------------------------------------*/
#if 0
char *Evstrtok(char *s1, const char *s2)
{
    return strtok(s1, s2);
}
#endif

/*---------------------------------------------------------------------------*/

int	Evsprintf(char *str, const char *format, ...)
{
    int	iRes = 0;
	va_list ptr;
	va_start(ptr, format);

    iRes = vsprintf(str, format, ptr);

    va_end(ptr);

	return iRes;
}

/*---------------------------------------------------------------------------*/

int Evvsprintf(char *str, const char *format, va_list ap)
{
	return vsprintf(str, format, ap);
}

/*---------------------------------------------------------------------------*/
#if 0
int Evsnprintf(char *str, size_t size, const char *format, ...)
{
    int	iRes = 0;
	va_list ptr;
	va_start(ptr, format);

    iRes = _vsnprintf(str, size, format, ptr);

    va_end(ptr);

	return iRes;
}
#endif
/*---------------------------------------------------------------------------*/

#if 0
int Evvsnprintf(char *str, size_t size, const char *format, va_list ap)
{
    return _vsnprintf(str, size, format, ap);
}
#endif

/*---------------------------------------------------------------------------*/

char * Evstrpbrk ( const char * str1, const char * str2 )
{
  return strpbrk (str1, str2 );
}

/*---------------------------------------------------------------------------*/
#if 0
int Evstrspn ( const char * str1, const char * str2 )
{
  return strspn (str1, str2 );
}
#endif

/*---------------------------------------------------------------------------*/
#if 0
int vsscanf(const char  *buffer, const char  *format, va_list argPtr)

{
	size_t stackSize;
	void **newStack;
    int result;
    void *savedESP;


    // Get an upper bound for the # of args
    size_t count = 0;
    const char *p = format;

    while(1)
    {
        char c = *(p++);
        if(c==0) break;
        if(c=='%' && (p[0]!='*' && p[0]!='%')) ++count;
    }

    // Make a local stack
    stackSize = (2+count)*sizeof(void*);
    newStack = (void**)Evmalloc(stackSize);

    // Fill local stack the way sscanf likes it
    newStack[0] = (void*)buffer;
    newStack[1] = (void*)format;
    Evmemcpy(newStack+2, argPtr, count*sizeof(void*));

    // Warp into system sscanf with new stack
    _asm
    {
       mov     savedESP, esp;
        mov     esp, newStack;
        call    sscanf;
        mov     esp, savedESP;
        mov     result, eax;
    }

    return result;
} 
#endif

#if 0
int Evsscanf(char *string, const char *format, ...)
{
	
	int	iRes = 0;
	va_list ptr;
	va_start(ptr, format);

    iRes = vsscanf(string, format, ptr);
    va_end(ptr);
	return iRes;
}
#endif

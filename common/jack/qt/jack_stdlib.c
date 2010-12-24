#include <stdarg.h> 

#include <windows.h>


#include "osal_stdlib.h"

/*---------------------------------------------------------------------------*/

int Everrno(void)
{
	return GetLastError();
}

/*---------------------------------------------------------------------------*/

int EvFileSys_errno(void)
{
	return GetLastError();
}

/*---------------------------------------------------------------------------*/

int EvSocket_errno(void)
{
	return 0;
	//return WSAGetLastError();
}

/*---------------------------------------------------------------------------*/

double Evatof(const char *nptr)
{
    return atof(nptr);
}

/*---------------------------------------------------------------------------*/

int Evatoi(const char *nptr)
{
    return atoi(nptr);
}

/*---------------------------------------------------------------------------*/

int64_t Evatoi64(const char *nptr)
{
    return _atoi64(nptr);
}

/*---------------------------------------------------------------------------*/

unsigned long int Evstrtoul(const char *nptr, char **endptr, int base)
{
    return strtoul(nptr, endptr, base);
}


/* The rand() function computes a sequence of pseudo-random integers in the
 * range of 0 to max_int.
 * @param max_int: Upper range for the pseudo-random integer
 */
int Evrand(int max_int)
{
	return rand() % max_int;
}

/* Initialize the pseudo-random number generator
 * @param seed: value to initialize the pseudo-random generator with. Same seed leads to same results.
 */
void Evsrand (unsigned int seed)
{
	srand(seed);
}

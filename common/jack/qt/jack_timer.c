#define _USE_32BIT_TIME_T 1
#include <windows.h>
#include <time.h>

#include "osal_types.h"
#include "osal_memory.h"
#include "osal_string.h"
#include "osal_time.h"

 /***********************************************************************
  *           DOSFS_FileTimeToUnixTime
  *
  * Convert a FILETIME format to Unix time.
  * If not NULL, 'remainder' contains the fractional part of the filetime,
  * in the range of [0..9999999] (even if time_t is negative).
  */
 Evtime_t FileTimeToUnixTime( const FILETIME *filetime, DWORD *remainder )
 {
	 /* Read the comment in the function DOSFS_UnixTimeToFileTime. */
     unsigned long a0;                  /* 16 bit, low    bits */
     unsigned long a1;                  /* 16 bit, medium bits */
     unsigned long a2;                  /* 32 bit, high   bits */
     unsigned long r;                   /* remainder of division */
     unsigned int carry;         /* carry bit for subtraction */
     int negative;               /* whether a represents a negative value */
 
     /* Copy the time values to a2/a1/a0 */
     a2 =  (unsigned long)filetime->dwHighDateTime;
     a1 = ((unsigned long)filetime->dwLowDateTime ) >> 16;
     a0 = ((unsigned long)filetime->dwLowDateTime ) & 0xffff;
 
     /* Subtract the time difference */
     if (a0 >= 32768           ) a0 -=             32768        , carry = 0;
     else                        a0 += (1 << 16) - 32768        , carry = 1;
 
     if (a1 >= 54590    + carry) a1 -=             54590 + carry, carry = 0;
     else                        a1 += (1 << 16) - 54590 - carry, carry = 1;
 
     a2 -= 27111902 + carry;
 
     /* If a is negative, replace a by (-1-a) */
     negative = (a2 >= ((unsigned long)1) << 31);
     if (negative)
     {
         /* Set a to -a - 1 (a is a2/a1/a0) */
         a0 = 0xffff - a0;
         a1 = 0xffff - a1;
         a2 = ~a2;
     }
 
     /* Divide a by 10000000 (a = a2/a1/a0), put the rest into r.
        Split the divisor into 10000 * 1000 which are both less than 0xffff. */
     a1 += (a2 % 10000) << 16;
     a2 /=       10000;
     a0 += (a1 % 10000) << 16;
     a1 /=       10000;
     r   =  a0 % 10000;
     a0 /=       10000;
 
     a1 += (a2 % 1000) << 16;
     a2 /=       1000;
     a0 += (a1 % 1000) << 16;
     a1 /=       1000;
     r  += (a0 % 1000) * 10000;
     a0 /=       1000;
 
     /* If a was negative, replace a by (-1-a) and r by (9999999 - r) */
     if (negative)
     {
         /* Set a to -a - 1 (a is a2/a1/a0) */
         a0 = 0xffff - a0;
         a1 = 0xffff - a1;
         a2 = ~a2;
 
         r  = 9999999 - r;
     }
 
     if (remainder) *remainder = r;
 
     /* Do not replace this by << 32, it gives a compiler warning and it does
        not work. */
     return ((((Evtime_t)a2) << 16) << 16) + (a1 << 16) + a0;
 }

 #ifdef EVOSAL_TESTTIME

 static struct {
	long system;
	long base;
	long mult;
} EwTest = { 0 , 0 , 0 };

int32_t EwSetTestTime(const struct Ewtm *time, long mult)
{
	EwTest.base = 0;
	EwTest.system = Ewtime(NULL);

	EwTest.mult = mult;
	if (time != NULL)
		EwTest.base = Ewmktime(time);
	else
		EwTest.base = EwTest.system;

	return(0);
}

#endif
 
/*---------------------------------------------------------------------------*/

Evtime_t Evtime(Evtime_t *timer)
{
	SYSTEMTIME	stm;
	FILETIME	ftm;
	Evtime_t	timerTemp;

	GetSystemTime(&stm);
	SystemTimeToFileTime(&stm, &ftm);

	/* AE: Should work I hope */
	timerTemp = FileTimeToUnixTime( &ftm, NULL);

	#ifdef EVOSAL_TESTTIME
	if (EwTest.base != 0)
		timerTemp = EwTest.base + (timerTemp - EwTest.system) * EwTest.mult;
	#endif
	
	if(timer)
		*timer = timerTemp;

	return timerTemp;
}


/*---------------------------------------------------------------------------*/

int	Evgettimeofday(struct Evtimeval * timer , struct Evtimezone * tz )
{
	union {
    __int64 ns100; /*time since 1 Jan 1601 in 100ns units */
    FILETIME ft;
    } now;

    GetSystemTimeAsFileTime (&now.ft);
    timer->tv_usec = (long) ((now.ns100 / 10LL) % 1000000LL);
    timer->tv_sec = (long) ((now.ns100 - 116444736000000000LL) / 10000000LL);
    return 0;
}


/* Time conversion function to/from UNIX context (EPOCH time and struct_tm) */
#define SECS_PER_HOUR 3600L
#define SECS_PER_DAY  86400L
#define	__isleap(year)	\
  ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))

static const unsigned char __vals[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, /* non-leap */
	    29,
};

Evtime_t Evmktime(struct Evtm *tim)
{
	int32_t days, secs;
	Evtime_t t;
	struct Evtm x;
	
	/* 0:sec  1:min  2:hour  3:mday  4:mon  5:year  6:wday  7:yday  8:isdst */
	register int32_t *p = (int32_t *) &x;
	register const unsigned char *s;
	int32_t d, default_dst;
	
	if (tim == NULL)
	{
		return 0;
	}
	Evmemcpy(p, tim, sizeof(struct Evtm));
	x.tm_isdst = 0;
	p[8] = 0;				/* set tm_isdst to 0. */
	default_dst = 0;
	
	d = 400;
	p[5] = (p[5] - ((p[6] = p[5]/d) * d)) + (p[7] = p[4]/12);
	if ((p[4] -= 12 * p[7]) < 0) {
		p[4] += 12;
		--p[5];
	}

	s = __vals;
	d = (p[5] += 1900);			/* Correct year.  Now between 1900 and 2300. */
	if (__isleap(d)) {
		s += 11;
	}
	
	p[7] = 0;
	d = p[4];
	while (d) {
		p[7] += *s;
		if (*s == 29) {
			s -= 11;			/* Backup to non-leap Feb. */
		}
		++s;
		--d;
	}

	d = p[5] - 1;
	days = -719163L + ((long)d)*365 + ((d/4) - (d/100) + (d/400) + p[3] + p[7]);
	secs = p[0] + 60*( p[1] + 60*((long)(p[2])) );
 DST_CORRECT:
	if (secs < 0) {
		secs += 120009600L;
		days -= 1389;
	}
	if ( ((unsigned long)(days + secs/86400L)) > 49710L) {
		t = ((Evtime_t)(-1));
		goto DONE;
	}
	secs += (days * 86400L);

	d = ((struct Evtm *)p)->tm_isdst;
	t = secs;

	if ((d < 0) && (((struct Evtm *)p)->tm_isdst != default_dst)) {

		secs -= (days * 86400L);
		goto DST_CORRECT;
	}
 DONE:
	return t;
}

static const uint16_t __mon_lengths[2][12] =
{
    /* Normal years.  */
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    /* Leap years.  */
    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};


struct Evtm * Evlocaltime_r(const Evtime_t * t, struct Evtm * tmbuf)
{
  Evtime_t offset = 0;
  int32_t days, rem;
  register int32_t y;
  register uint16_t *ip;

  if ( (t == NULL) || (tmbuf == NULL) )
		return NULL;

  days = *t / SECS_PER_DAY;
  rem = *t % SECS_PER_DAY;
  rem += offset;
  while (rem < 0)
    {
      rem += SECS_PER_DAY;
      --days;
    }
  while (rem >= SECS_PER_DAY)
    {
      rem -= SECS_PER_DAY;
      ++days;
    }
  tmbuf->tm_hour = rem / SECS_PER_HOUR;
  rem %= SECS_PER_HOUR;
  tmbuf->tm_min = rem / 60;
  tmbuf->tm_sec = rem % 60;
  /* January 1, 1970 was a Thursday.  */
  tmbuf->tm_wday = (4 + days) % 7;
  if (tmbuf->tm_wday < 0)
    tmbuf->tm_wday += 7;
  y = 1970;
  while (days >= (rem = __isleap(y) ? 366 : 365))
    {
      ++y;
      days -= rem;
    }
  while (days < 0)
    {
      --y;
      days += __isleap(y) ? 366 : 365;
    }
  tmbuf->tm_year = y - 1900;
  tmbuf->tm_yday = days;
  ip = (uint16_t*)__mon_lengths[__isleap(y)];
  for (y = 0; days >= ip[y]; ++y)
    days -= ip[y];
  tmbuf->tm_mon = y;
  tmbuf->tm_mday = days + 1;
  tmbuf->tm_isdst = 0;
  return tmbuf;
}
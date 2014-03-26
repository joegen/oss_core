#include <stdio.h>
#include <string.h>		/* memset */
#include <sys/time.h>		/* struct timeval, gettimeofday */
#include <signal.h>		/* siginterrupt, SIGALRM */
#include "dtime.h"		/* function headers */

/**
 ** dtime:
 **  returns the time of day in double format with microsecond precision
 */
double dtime ()
{
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return (tvtod (tv));
}

/**
 ** dalarm: 
 **  generates a SIGALRM signal in #time# seconds
 */
void dalarm (double time)
{
    struct itimerval it;
    memset (&it, 0, sizeof (struct itimerval));
    it.it_value = dtotv (time);
    siginterrupt (SIGALRM, 1);
    setitimer (ITIMER_REAL, &it, NULL);
}

/**
 ** dalarm:
 **  sleeps for #time# seconds
 */
void dsleep (double time)
{
    struct timeval tv;
    tv = dtotv (time);
    select (0, NULL, NULL, NULL, &tv);
}

/**
 ** dtotv: 
 **  returns the struct timeval representation of double #d#
 **
 */
struct timeval dtotv (double d)
{
    struct timeval tv;
    tv.tv_sec = (long) d;
    tv.tv_usec = (long) ((d - (double) tv.tv_sec) * 1000000.0);
    return (tv);
}

/**
 ** tvtod:
 **  returns the double representation of timeval #tv#
 */
double tvtod (struct timeval tv)
{
    return (tv.tv_sec + ((double) tv.tv_usec / 1000000.0));
}

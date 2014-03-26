/*
** $Id: semaphore.c,v 1.7 2006/06/07 09:21:29 krishnap Exp $
**
** Matthew Allen
** description: 
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include "semaphore.h"

void *sema_create (int val)
{

    Sema *s;
    int ret;

    s = (Sema *) malloc (sizeof (Sema));
    s->val = val;
    if ((ret = pthread_mutex_init (&s->lock, NULL)) != 0)
	{
	    free (s);
	    return (NULL);
	}

    if ((ret = pthread_cond_init (&s->cond, NULL)) != 0)
	{
	    pthread_mutex_destroy (&s->lock);
	    free (s);
	    return (NULL);
	}

    return ((void *) s);

}

void sema_destroy (void *v)
{

    Sema *s = (Sema *) v;

    pthread_mutex_destroy (&s->lock);
    pthread_cond_destroy (&s->cond);
    free (s);

}

int sema_p (void *v, double time)
{

    Sema *s = (Sema *) v;
    struct timespec timeout;
    struct timeval now;
    unsigned long sec, nsec;
    int ret = 0;

    pthread_mutex_lock (&s->lock);
    s->val--;
    if (s->val < 0)
	{
	    if (time <= 0.0)
		{
		    ret = pthread_cond_wait (&s->cond, &s->lock);
		}
	    else
		{
		    if (time < 0.1)
			time = 0.1;
		    gettimeofday (&now, NULL);
		    sec = (unsigned long) time;
		    nsec =
			(unsigned long) ((time - (double) sec) *
					 1000000000.0);
		    timeout.tv_sec = now.tv_sec + sec;
		    timeout.tv_nsec = (now.tv_usec * 1000) + nsec;
		    ret =
			pthread_cond_timedwait (&s->cond, &s->lock, &timeout);
		}
	}
    pthread_mutex_unlock (&s->lock);

    return (ret);

}

void sema_v (void *v)
{

    Sema *s = (Sema *) v;

    pthread_mutex_lock (&s->lock);
    s->val++;
    if (s->val <= 0)
	{
	    pthread_cond_signal (&s->cond);
	}
    pthread_mutex_unlock (&s->lock);

}

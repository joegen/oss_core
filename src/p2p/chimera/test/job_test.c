#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "job_queue.h"


/* Dummy Job 1 */
void test_print (void *in)
{
    char *c;
    c = (char *) in;
    printf (" %c \n", *c);
}

/* Dummy Job 2 */
void count_print (void *in)
{
    int i, j;
    i = *(int *) in;
    printf ("I is %d\n", i);

    for (j = 0; j < i; j++)
	printf ("%d ", j);
    printf ("\n\n");
}

/* Dummy Job 3 
 * jut burn some CPU cycle */
void busy_cycle (void *in)
{
    int *cycle;
    cycle = (int *) in;
    // printf("CYCLE is %d\n", *cycle);

    int i;
    for (i = 0; i < *cycle; i++)
	random ();
}

int main ()
{

    int i;
    int cycle = 1000;
    char c = 'z';
    List *Q;

    Q = job_queue_init (3);

    /* submit some jobs */
    for (i = 0; i < 100; i++)
	{
	    printf ("I in main %d\n", *&i);
	    job_submit (Q, count_print, (void *) &i, sizeof (int));
	}

    /* Be Busy */
    for (i = 0; i < 1000; i++)
	{
	    busy_cycle ((void *) &cycle);
	}

    /* submit some more jobs */
    for (i = 0; i < 100; i++)
	{
	    job_submit (Q, count_print, (void *) &i, sizeof (int));
	}

    job_submit (Q, test_print, (void *) &c, sizeof (char));
    job_submit (Q, test_print, (void *) &c, sizeof (char));
    job_submit (Q, test_print, (void *) &c, sizeof (char));

    printf ("\n-------End of Main--------\n");

    pthread_exit (NULL);

    return 0;
}

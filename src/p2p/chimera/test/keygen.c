#include <stdio.h>
#include <stdlib.h>
#include "jrb.h"
#include "jval.h"

main (int argc, char **argv)
{

    int i, count;
    unsigned long val;
    JRB tree;

    if (argc != 2 || sscanf (argv[1], "%d", &count) != 1)
	{
	    fprintf (stderr, "usage: keygen count\n");
	    exit (1);
	}

    srand (time (NULL));
    tree = make_jrb ();

    for (i = 0; i < count; i++)
	{

	    do
		{
		    if ((rand () % 2) == 1)
			val = RAND_MAX;
		    else
			val = 0;
		    val += rand ();
		}
	    while (jrb_find_dbl (tree, (double) val) != NULL);

	    printf ("%x\n", val);
	    jrb_insert_dbl (tree, (double) val, JNULL);

	}

}

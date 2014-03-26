#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "route.h"


int main ()
{

    ChimeraHost temp;
    ChimeraHost *next_hop;
    ChimeraHost **ret;
    char cmd[5];
    int i, count, key, running;

    printf ("Please Enter your node info\n");
    gethostinfo (&temp);
    route_init (&temp);
    printTable ();

    running = 1;
    while (running)
	{
	    printf ("CMD:");
	    fflush (stdin);
	    scanf ("%s", cmd);
	    cmd[strlen (cmd)] = 0;

	    if (strcmp (cmd, "add") == 0)
		{
		    gethostinfo (&temp);
		    route_update (&temp, 1);
		}
	    else if (strcmp (cmd, "del") == 0)
		{
		    gethostinfo (&temp);
		    route_update (&temp, 0);
		}
	    else if (strcmp (cmd, "leaf") == 0)
		{
		    printf ("COUNT: ");
		    scanf ("%d", &count);
		    ret = route_neighbors (count);

		    printf ("\n+++++++ %d closest ++++++++++\n", count);
		    for (i = 0; ret[i] != NULL && i < count; i++)
			printf ("%x ", ret[i]->key);
		    printf ("\n+++++++++++++++++++++++++++++\n");
		}
	    else if (strcmp (cmd, "look") == 0)
		{
		    printf ("KEY: ");
		    scanf ("%i", &key);
		    next_hop = route_lookup (key, 1, 1);
		    printf ("NEXT_HOP:%x \n", next_hop->key);
		}
	    else if (strcmp (cmd, "exit") == 0)
		running = 0;
	    else
		printf ("CMD could be: [add] [del] [leaf] [look] [exit]\n");
	    printTable ();
	    leafset_print ();
	    printf ("\n====================================\n");
	}

    return 0;
}

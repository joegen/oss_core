/*
** $Id: test.c,v 1.5 2006/06/07 09:21:29 krishnap Exp $
**
** Matthew Allen
** description: 
*/
#include <stdio.h>
#include <unistd.h>
#include "chimera.h"

#define USAGE "test [ -j bootstrap:port ]"
#define OPTSTR "j:"
extern char *optarg;
extern int optind;

int main (int argc, char **argv)
{

    int opt;
    char *hn;
    int port;
    Host *host = NULL;
    int i;

    while ((opt = getopt (argc, argv, OPTSTR)) != EOF)
	{
	    switch ((char) opt)
		{
		case 'j':
		    for (i = 0; optarg[i] != ':' && i < strlen (optarg); i++);
		    optarg[i] = 0;
		    hn = optarg;
		    sscanf (optarg + (i + 1), "%d", &port);
		    host = message_makehost (hn, port);
		    break;
		default:
		    fprintf (stderr, "invalid option %c\n", (char) opt);
		    fprintf (stderr, "usage: %s\n", USAGE);
		    exit (1);
		}
	}

    port = atoi (argv[optind]);
    chimera_join (port, host);

    sleep (30);

}

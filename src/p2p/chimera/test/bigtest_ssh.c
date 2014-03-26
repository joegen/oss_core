/*
** $Id: bigtest_ssh.c,v 1.4 2006/06/07 09:21:29 krishnap Exp $
**
** Matthew Allen
** description: 
*/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "message.h"
#include "log.h"
#include "jrb.h"
#include "jval.h"
#include "semaphore.h"
#include "dtime.h"

extern char *optarg;
#define OPTSTR "f:"
#define USAGE "bigtest"

int announced;
void *sem;
int hops;
unsigned long current;

typedef struct
{
    char *hnp;
    char *name;
    int port;
    unsigned long key;
    int pid;
} Host;

JRB read_hosts (char *fn, int *nhosts)
{

    FILE *fp;
    char s[256];
    char hn[128];
    int port;
    unsigned long key;
    JRB hosts;
    Host *host;

    fp = fopen (fn, "r");
    if (fp == NULL)
	{
	    perror (fn);
	    exit (1);
	}

    hosts = make_jrb ();
    *nhosts = 0;

    while (fgets (s, 256, fp) != NULL)
	{
	    sscanf (s, "%s %d %8x", hn, &port, &key);
	    host = (Host *) malloc (sizeof (Host));
	    host->name = strdup (hn);
	    host->port = port;
	    host->key = key;
	    sprintf (s, "%s:%d", hn, port);
	    host->hnp = strdup (s);
	    host->pid = 0;
	    jrb_insert_str (hosts, strdup (s), new_jval_v (host));
	    *nhosts++;
	}

    fclose (fp);

    return (hosts);

}

void hello (Message * msg)
{

    fprintf (stderr, "hello: %s\n", msg->payload);
    announced++;
    sema_v (sem);

}

void fwd (Message * msg)
{

    unsigned long cur, dest;

    sscanf (msg->payload, "%x %x", &cur, &dest);
    //fprintf(stderr, "message to %8x routed by %8x\n", dest, cur);
    if (dest == current)
	{
	    hops++;
	}

}

void del (Message * msg)
{

    unsigned long cur, dest;

    sscanf (msg->payload, "%x %x", &cur, &dest);
    //fprintf(stderr, "message to %8x routes delivered to %8x\n", dest, cur);
    if (dest == current)
	{
	    sema_v (sem);
	}

}

void start_host (Host * host, Host * join)
{

    char port[16], key[16];
    char *arg[9];
    int i;

    sprintf (port, "%d", host->port);
    sprintf (key, "%x", host->key);
    if (join != NULL)
	{
	    arg[0] = "ssh";
	    arg[1] = host->name;
	    arg[2] = "bighost";
	    arg[3] = "-j";
	    arg[4] = join->hnp;
	    arg[5] = port;
	    arg[6] = key;
	    arg[7] = NULL;
	}
    else
	{
	    arg[0] = "ssh";
	    arg[1] = host->name;
	    arg[2] = "bighost";
	    arg[3] = port;
	    arg[4] = key;
	    arg[5] = NULL;
	}


    /* for(i=0; arg[i] != NULL; i++)
       fprintf(stderr,"%s ", arg[i]);
       printf("\n"); */


    host->pid = fork ();

    if (host->pid == 0)
	{
	    execvp ("ssh", arg);
	    perror ("ssh");
	    exit (1);
	}

    sema_p (sem, 0.0);

}
void kill_hosts (char *nodelist)
{

    char *arg[7];
    int pid, i;

    arg[0] = "perl";
    arg[1] = "mcmd.pl";
    arg[2] = "-n";
    arg[3] = nodelist;
    arg[4] = "bigkill 30";
    arg[5] = "30";
    arg[6] = NULL;

    for (i = 0; i < 6; i++)
	printf ("%d - %s\n", i, arg[i]);

    pid = fork ();
    if (pid == 0)
	{
	    execvp ("perl", arg);
	    perror ("perl");
	    exit (1);
	}

}

int main (int argc, char **argv)
{

    int opt;
    char *configfile = "hosts";
    JRB hosts, node, node2;
    Host *host, *join, *me;
    int nhosts;
    int i, j;
    Message *m;
    char dest[16];
    ChimeraHost ch;
    double start;

    while ((opt = getopt (argc, argv, OPTSTR)) != EOF)
	{
	    switch ((char) opt)
		{
		case 'f':
		    configfile = optarg;
		    break;
		default:
		    fprintf (stderr, "invalid option %c\n", (char) opt);
		    fprintf (stderr, "usage: %s\n", USAGE);
		    exit (1);
		}
	}

    //  kill_hosts(argv[3]);
    //  exit(1);

    sem = sema_create (0);

    me = (Host *) malloc (sizeof (Host));
    me->name = "miller.cs.ucsb.edu";
    me->port = 11110;
    me->hnp = "miller.cs.ucsb.edu:11110";

    hosts = read_hosts (configfile, &nhosts);
    message_init (11110);
    message_handler (21, hello);
    message_handler (22, fwd);
    message_handler (23, del);
    srand (time (NULL));

    announced = 0;
    jrb_traverse (node, hosts)
    {
	if (announced == 0)
	    {
		join = NULL;
	    }
	else
	    {
		i = (rand () % announced) + 1;
		jrb_traverse (node2, hosts)
		{
		    i--;
		    if (i == 0)
			break;
		}
		join = (Host *) node2->val.v;
	    }
	host = (Host *) node->val.v;
	start_host (host, join);
	dsleep (0.5);
    }

    // kill_hosts(argv[3]);
    //  exit(1);


    for (j = 0; j < 10000; j++)
	{

	    do
		{
		    i = (rand () % announced) + 1;
		    jrb_traverse (node, hosts)
		    {
			i--;
			if (i == 0)
			    break;
		    }
		    host = (Host *) node->val.v;
		    ch.name = host->name;
		    ch.address = network_address (host->name);
		    ch.port = host->port;
		    ch.key = host->key;
		    sprintf (dest, "%x", rand ());
		    printf ("iniating route to %s from %s:%d\n", dest,
			    host->name, host->port);
		    m = message_create (0, 24, strlen (dest) + 1, dest);
		    hops = 0;
		    sscanf (dest, "%x", &current);
		}
	    while (!message_send (&ch, m));
	    start = dtime ();
	    if (sema_p (sem, 5.0) == 0)
		{
		    printf ("message %d routed in %d hops %lf seconds\n", j,
			    hops, dtime () - start);
		}
	    else
		{
		    printf ("message %d failed\n", j, hops);
		}
	    fflush (stdout);
	}

}

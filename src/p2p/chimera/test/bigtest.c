/*
** $Id: bigtest.c,v 1.24 2007/01/08 02:51:48 ravenben Exp $
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
#include "key.h"
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
Key current;
ChimeraState *state;

typedef struct
{
    char *hnp;
    char *name;
    int port;
    Key key;
    int pid;
} Host;

JRB read_hosts (char *fn, int *nhosts)
{

    FILE *fp;
    char s[256];
    char hn[128];
    int port;
    Key key;
    JRB hosts;
    Host *host;
    char keyinput[64];

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
	    sscanf (s, "%s %d %s", hn, &port, keyinput);
	    host = (Host *) malloc (sizeof (Host));
	    host->name = strdup (hn);
	    host->port = port;

	    str_to_key (keyinput, &key);
	    key_assign (&(host->key), key);
	    sprintf (s, "%s:%d", hn, port);
	    host->hnp = strdup (s);
	    host->pid = 0;
	    jrb_insert_str (hosts, strdup (s), new_jval_v (host));
	    *nhosts++;
	}

    fclose (fp);

    return (hosts);

}

void hello (ChimeraState * chstate, Message * msg)
{


    fprintf (stderr, "hello: %s\n", msg->payload);
    announced++;
    sema_v (sem);

}

void fwd (ChimeraState * chstate, Message * msg)
{

    Key cur, dest;
    char deststr[64];
    char curstr[64];


    sscanf (msg->payload, "%s %s", curstr, deststr);
    str_to_key (curstr, &cur);
    str_to_key (deststr, &dest);
    fprintf (stderr, "message to %s routed by %s\n", dest.keystr, cur.keystr);
    if (key_equal (dest, current))
	{
	    hops++;
	}

}

void del (ChimeraState * chstate, Message * msg)
{

    Key cur, dest;
    char deststr[64];
    char curstr[64];
    sscanf (msg->payload, "%s %s", curstr, deststr);
    str_to_key (curstr, &cur);
    str_to_key (deststr, &dest);
    fprintf (stderr, "message to %s routes delivered to %s\n", dest.keystr,
	     cur.keystr);

    if (key_equal (dest, current))
	{
	    sema_v (sem);
	}

}

void start_host (Host * host, Host * join)
{

    char port[16], key[160];
    char *arg[9];
    int i;

    memset (port, 0, 16);
    memset (key, 0, 160);

    sprintf (port, "%d", host->port);
    sprintf (key, "%s", host->key.keystr);
//  arg[0] = "ssh";
//  arg[1] = host->name;
    arg[0] = "./bighost";
    if (join != NULL)
	{
	    arg[1] = "-j";
	    arg[2] = join->hnp;
	    arg[3] = port;
	    arg[4] = key;
	    arg[5] = NULL;
	}
    else
	{
	    arg[1] = port;
	    arg[2] = key;
	    arg[3] = NULL;
	}


    host->pid = fork ();

    if (host->pid == 0)
	{
	    execvp ("./bighost", arg);
	    perror ("./bighost");
	    exit (1);
	}

    sema_p (sem, 0.0);

}

void key_rand (Key * key)
{
    int i;
    for (i = 0; i < 5; i++)
	{
	    key->t[i] = rand ();
	}

    key_to_str (key);

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
    char dest[160];
    char msg[256];
    Key tmp;
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

    sem = sema_create (0);
    state = (ChimeraState *) malloc (sizeof (ChimeraState));

    me = (Host *) malloc (sizeof (Host));
    me->name = "localhost";
    me->port = 11110;
    me->hnp = "localhost:11110";

    hosts = read_hosts (configfile, &nhosts);

    state->log = log_init ();
    log_direct (state->log, LOG_ERROR, stderr);
    log_direct (state->log, LOG_WARN, stderr);

    key_init ();
    state->message = message_init (state, 11110);
    message_handler (state, 21, hello, 1);
    message_handler (state, 22, fwd, 1);
    message_handler (state, 23, del, 1);
    message_handler (state, 24, NULL, 1);

    srand (time (NULL));

/* This part runs bighost in different hosts */

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
	dsleep (0.1);
    }

/* this part sends a message to two random hosts in the tree */

    for (j = 0; j < 1000; j++)
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
		    ch.address = network_address (state->network, host->name);
		    ch.port = host->port;
		    key_assign (&ch.key, host->key);
		    key_rand (&tmp);
		    sprintf (dest, "%s", tmp.keystr);
		    sprintf (msg, "%s %d", dest, j);

		    printf ("iniating route to %s from %s\n", dest,
			    host->key.keystr);


		    m = message_create (ch.key, 24, strlen (dest) + 1, dest);

		    hops = 0;
		    sscanf (dest, "%s", current.keystr);
		    str_to_key (current.keystr, &current);

		}
	    while (!message_send (state, &ch, m, TRUE));
	    start = dtime ();
	    if (sema_p (sem, 2.0) == 0)
		{
		    printf ("message %d routed in %d hops %lf seconds\n", j,
			    hops, dtime () - start);
		}
	    else
		{
		    printf ("message %d failed\n", j, hops);
		}
	    fflush (stdout);
	    dsleep (.01);
	}

}

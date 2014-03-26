/*
** Authors: 
** Rama Alebouyeh ( rama[at]cs.ucsb.edu )
** Matthew Allen ( msa[at]cs.ucsb.edu )
*/

#include <stdio.h>
#include <unistd.h>
#include "chimera.h"

#define USAGE "test [ -j bootstrap:port ] port key"
#define OPTSTR "j:"
extern char *optarg;
extern int optind;

#define TEST_CHAT 15

void test_fwd (Key ** kp, Message ** mp, ChimeraHost ** hp)
{

    Key *k = *kp;
    Message *m = *mp;
    ChimeraHost *h = *hp;

    fprintf (stderr, "Routing %s (%s) to %s via %s:%d\n",
	     (m->type == TEST_CHAT) ? ("CHAT") : ("JOIN"), m->payload,
	     k->keystr, h->name, h->port);

}

void test_del (Key * k, Message * m)
{

    fprintf (stderr, "Delivered %s (%s) to %s\n",
	     (m->type == TEST_CHAT) ? ("CHAT") : ("JOIN"), m->payload,
	     k->keystr);

    if (m->type == TEST_CHAT)
	{
	    fprintf (stderr, "** %s **\n", m->payload);
	}

}

void test_update (Key * k, ChimeraHost * h, int joined)
{

    if (joined)
	{
	    fprintf (stderr, "Node %s:%s:%d joined neighbor set\n", k->keystr,
		     h->name, h->port);
	}
    else
	{
	    fprintf (stderr, "Node %s:%s:%d leaving neighbor set\n",
		     k->keystr, h->name, h->port);
	}

}

int main (int argc, char **argv)
{
    int opt;
    char *hn = NULL;
    int port, joinport;
    ChimeraHost *host = NULL;
    char tmp[256];
    Key key, keyinput;
    int i, j;
    ChimeraState *state;

    while ((opt = getopt (argc, argv, OPTSTR)) != EOF)
	{
	    switch ((char) opt)
		{
		case 'j':
		    for (i = 0; optarg[i] != ':' && i < strlen (optarg); i++);
		    optarg[i] = 0;
		    hn = optarg;
		    sscanf (optarg + (i + 1), "%d", &joinport);
		    break;
		default:
		    fprintf (stderr, "invalid option %c\n", (char) opt);
		    fprintf (stderr, "usage: %s\n", USAGE);
		    exit (1);
		}
	}

    if ((argc - optind) != 2)
	{
	    fprintf (stderr, "usage: %s\n", USAGE);
	    exit (1);
	}

    port = atoi (argv[optind]);

    str_to_key (argv[optind + 1], &keyinput);

    state = chimera_init (port);
    if (state == NULL)
	{
	    fprintf (stderr, "unable to initialize chimera \n");
	    exit (1);
	}
    if (hn != NULL)
	{
	    host = host_get (state, hn, joinport);
	}
    chimera_forward (state, test_fwd);
    chimera_deliver (state, test_del);
    chimera_update (state, test_update);
    chimera_setkey (state, keyinput);
    chimera_register (state, TEST_CHAT, 1);

    /*char  networkdebug[256], routingdebug[256];
       sprintf(networkdebug, "LOG_NETWORKDEBUG_%s", keyinput.keystr); 
       sprintf(routingdebug, "LOG_ROUTING_%s", keyinput.keystr); 
       FILE *networklogfp= fopen(networkdebug, "w");
       FILE *routingfp= fopen(routingdebug, "w");
       log_direct(state->log, LOG_ROUTING, routingfp);
       log_direct(state->log, LOG_NETWORKDEBUG, networklogfp); */

    log_direct (state->log, LOG_WARN, stderr);
    log_direct (state->log, LOG_ROUTING, stderr);

    chimera_join (state, host);

    fprintf (stderr,
	     "** send messages to key with command <key> <message> **\n");

    while (fgets (tmp, 256, stdin) != NULL)
	{
	    if (strlen (tmp) > 2)
		{
		    for (i = 0; tmp[i] != '\n'; i++);
		    tmp[i] = 0;
		    for (i = 0; tmp[i] != ' ' && i < strlen (tmp); i++);
		    tmp[i] = 0;
		    i++;
		    str_to_key (tmp, &key);
		    fprintf (stderr, "sending key:%s data:%s len:%d\n",
			     key.keystr, tmp + i, strlen (tmp + i));
		    chimera_send (state, key, TEST_CHAT, strlen (tmp + i) + 1,
				  tmp + i);
		}
	}
}

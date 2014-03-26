/*
** $Id: bighost.c,v 1.19 2006/06/07 09:21:29 krishnap Exp $
**
** Matthew Allen
** description: 
*/
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "chimera.h"
#include "log.h"
#include <string.h>

#define USAGE "bighost [ -j bootstrap:port ] port key"
#define OPTSTR "j:"

#define DEBUG 0

extern char *optarg;
extern int optind;

#define TEST_CHAT 15

ChimeraHost *driver = NULL;
ChimeraState *state;
Key key;
int seq = -1;

void test_fwd (Key ** kp, Message ** mp, ChimeraHost ** hp)
{

    Key *k = *kp;
    Message *m = *mp;
    ChimeraHost *h = *hp;
    char s[256];
    Message *message;
    int seq;



    if (m->type == 20)
	{
	    if (DEBUG)
		printf ("FWD:%s\n", m->payload);

	    sscanf (m->payload, "%d", &seq);

	    /* key is the node key , k is the destination */
	    sprintf (s, "%s %s %d", key.keystr, k->keystr, seq);

	    message = message_create (driver->key, 22, strlen (s) + 1, s);
	    message_send (state, driver, message, TRUE);
	    free (message);
	}

}

void test_del (Key * k, Message * m)
{

    char s[256];
    Message *message;
    int seq;

    if (m->type == 20)
	{
	    if (DEBUG)
		printf ("DEL:%s\n", m->payload);

	    sscanf (m->payload, "%d", &seq);

	    sprintf (s, "%s %s %d", key.keystr, k->keystr, seq);
	    message = message_create (driver->key, 23, strlen (s) + 1, s);
	    message_send (state, driver, message, TRUE);
	    free (message);
	}

}

void test_init (ChimeraState * state, Message * m)
{

    Key dest;
    char s[256];
    int seq;

    //sscanf(m->payload, "%x", &des );

    sscanf (m->payload, "%s %d", dest.keystr, &seq);
    str_to_key (dest.keystr, &dest);
    if (DEBUG)
	{
	    printf ("INIT: %s\n", m->payload);
	    printf ("dest: %s seq: %d\n", dest.keystr, seq);
	}

    sprintf (s, "%d TEST", seq);
    chimera_send (state, dest, 20, strlen (s) + 1, s);
}


int main (int argc, char **argv)
{

    int opt;
    char *hn = NULL;
    int port, joinport;
    ChimeraHost *join = NULL;
    char tmp[256];
    int i, j;
    Message *hello;

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

    fprintf (stderr, "\n");

    if ((argc - optind) != 2)
	{
	    fprintf (stderr, "usage: %s\n", USAGE);
	    exit (1);
	}


    port = atoi (argv[optind]);

    str_to_key (argv[optind + 1], &key);

    state = chimera_init (port);
    chimera_setkey (state, key);


    if (hn != NULL)
	{
	    join = host_get (state, hn, joinport);
	}

    chimera_forward (state, test_fwd);
    chimera_deliver (state, test_del);
    chimera_register (state, 20, 1);
    chimera_register (state, 21, 1);
    chimera_register (state, 22, 1);
    chimera_register (state, 23, 1);

    log_direct (state->log, LOG_ERROR, stderr);
    log_direct (state->log, LOG_WARN, stderr);
    //  log_direct(state->log, LOG_ROUTING, stderr);  

    message_handler (state, 24, test_init, 1);

    driver = host_get (state, "localhost", 11110);

    if (join != NULL)
	{
	    chimera_join (state, join);
	    sprintf (tmp, "%d %s joining with %s:%d", port, key.keystr, hn,
		     joinport);
	}
    else
	{
	    sprintf (tmp, "%d %s starting system", port, key.keystr, hn,
		     joinport);
	}

    hello = message_create (driver->key, 21, strlen (tmp) + 1, tmp);
    message_send (state, driver, hello, TRUE);
    free (hello);

    //pthread_exit(NULL);
    while (1)
	{
	    sleep (1000);
	}
}

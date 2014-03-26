/*
** Authors: 
** Rama Alebouyeh ( rama[at]cs.ucsb.edu )
** Matthew Allen ( msa[at]cs.ucsb.edu )
*/

#include <stdio.h>
#include <unistd.h>
#include "chimera.h"
#include "dtime.h"
#include "message.h"
#include <time.h>

#define USAGE "sender port key client_key client_name client_port"
#define OPTSTR "j:"
extern char *optarg;
extern int optind;

#define TEST_CHAT 15
#define MSG_LEN 1000
#define MSG_NUM 500

void test_fwd (Key ** kp, Message ** mp, ChimeraHost ** hp)
{

    Key *k = *kp;
    Message *m = *mp;
    ChimeraHost *h = *hp;

    // fprintf(stderr,"Routing %s (%s) to %s via %s:%d\n", (m->type==TEST_CHAT)?("CHAT"):("JOIN"),m->payload, k->keystr, h->name, h->port);

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
    ChimeraHost *chost, *host = NULL;
    Key client_key;
    char tmp[256];
    Key key, keyinput;
    int i, j, counter;
    ChimeraState *state;
    char msg[MSG_LEN];
    char *cname;
    int cport;
    struct timeval tv;
    Message *m;


    counter = 0;

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

    if ((argc - optind) != 5)
	{
	    fprintf (stderr, "usage: %s\n", USAGE);
	    exit (1);
	}

    port = atoi (argv[optind]);
    str_to_key (argv[optind + 1], &keyinput);
    str_to_key (argv[optind + 2], &client_key);
    cname = argv[optind + 3];
    //  strcpy(cname, argv[optind+3]);
    cport = atoi (argv[optind + 4]);

    fprintf (stderr, "client is %s:%d  %s\n", cname, cport,
	     client_key.keystr);

    state = chimera_init (port);

    if (hn != NULL)
	{
	    host = host_get (state, hn, joinport);
	}
    if (cname != NULL)
	chost = host_get (state, cname, cport);

    chimera_forward (state, test_fwd);
    chimera_deliver (state, test_del);
    chimera_update (state, test_update);
    chimera_setkey (state, keyinput);
    chimera_register (state, TEST_CHAT, 1);
    //log_direct(state->log, LOG_NETWORKDEBUG, stderr);
    log_direct (state->log, LOG_WARN, stderr);

    chimera_join (state, host);

    for (i = 0; i < MSG_LEN -1; i++)
	msg[i] = 'X';
    msg[MSG_LEN - 1] = '\0';
    fprintf (stderr, "msg_len:   %d ", strlen (msg));
    counter = 0;
    fprintf (stderr, "**--** type start to begin the test **--**\n");

    while (fgets (tmp, 256, stdin) != NULL)
	{
	    if (strcmp (tmp, "start") == 1)
		{
		    gettimeofday (&tv, NULL);
		    fprintf (stderr, "** Start sending message  @ %s ", ctime (&tv));
		    for (i = 0; i < MSG_NUM; i++)
			{

//				m=message_create(client_key, TEST_CHAT, MSG_LEN,msg);
//				message_send(state,chost,m,FALSE);
				chimera_send (state, client_key, TEST_CHAT, MSG_LEN, msg);
				counter++;
				//fprintf(stderr, "Sent %d \n", counter);
			}
		    gettimeofday (&tv, NULL);
		    fprintf (stderr, "**Finished sending %d messages @ %s ",
			     counter, ctime (&tv));
		}
	}
}

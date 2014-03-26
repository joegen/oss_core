
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <openssl/evp.h>
#include "chimera.h"
#include "log.h"
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include "dtime.h"
#include <errno.h>

#define USAGE "bighost [ -j bootstrap:port ] port key"
#define OPTSTR "j:"

#define DEBUG 0
#define NUM_HOST 120

extern char *optarg;
extern int optind;

ChimeraHost *driver = NULL;
ChimeraState *state;
Key key;
Key destinations[100];
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
	    //  fprintf(stderr,"message %d to %x delivered to %x",seq,k,key);

	    /*  sprintf(s, "%x %x %d", key, *k, seq );
	       message = message_create(0, 22, strlen(s)+1, s);
	       message_send(state, driver, message,TRUE);
	       free(message); */
	}

}

void test_del (Key * k, Message * m)
{

    char s[256];
    Message *message;
    int seq;
    Key dest;
    //  unsigned long dest;



    if (m->type == 20)
	{
	    if (DEBUG)
		printf ("DEL:%s\n", m->payload);

	    sscanf (m->payload, "%d %s", &seq, dest.keystr);
	    str_to_key (dest.keystr, &dest);
	    fprintf (stderr, "message %d to %s delivered to %s\n", seq,
		     dest.keystr, key.keystr);

	    if (seq == 100)
		{
		    sprintf (s, "%s %s %d", key.keystr, dest.keystr, seq);
		    message =
			message_create (driver->key, 23, strlen (s) + 1, s);
		    message_send (state, driver, message, TRUE);
		    free (message);
		}
	    fflush (stderr);
	}
}

void test_init (ChimeraState * state, Message * m)
{

    Key dest;
    char s[64];
    int seq;

    //  sscanf(m->payload, "%x", &dest);
    sscanf (m->payload, "%s %d", dest.keystr, &seq);
    str_to_key (dest.keystr, &dest);
    if (DEBUG)
	{
	    printf ("INIT: %s\n", m->payload);
	    printf ("dest: %s seq: %d\n", dest.keystr, seq);
	}


    sprintf (s, "%d TEST", seq);
    //chimera_send(dest, 20, 5, "TEST");
    chimera_send (state, dest, 20, strlen (s) + 1, s);
}


char *sha1_keygen2 (char *key, char *digest)
{
    EVP_MD_CTX mdctx;
    const EVP_MD *md;
    unsigned char *md_value;
    int md_len, i;
    char digit[10];
    char *tmp;

    md_value = (unsigned char *) malloc (EVP_MAX_MD_SIZE);

    OpenSSL_add_all_digests ();

    md = EVP_get_digestbyname ("sha1");

    EVP_MD_CTX_init (&mdctx);
    EVP_DigestInit_ex (&mdctx, md, NULL);
    EVP_DigestUpdate (&mdctx, key, strlen (key));
    EVP_DigestFinal_ex (&mdctx, md_value, &md_len);
    EVP_MD_CTX_cleanup (&mdctx);

    digest = (char *) malloc (KEY_SIZE / BASE_B + 1);
    memset (digest, 0, (KEY_SIZE / BASE_B + 1));

    tmp = digest;
    for (i = 0; i < md_len; i++)
	{
	    if (power (2, BASE_B) == BASE_16)
		{
		    convert_base16 (md_value[i], digit);
		}
	    else if (power (2, BASE_B) == BASE_4)
		{
		    convert_base4 (md_value[i], digit);
		}
	    else if (power (2, BASE_B) == BASE_2)
		{
		    convert_base2 (md_value[i], digit);
		}

	    strcat (tmp, digit);
	    tmp = tmp + strlen (digit);
	}

    free (md_value);

    tmp = '\0';
    return (digest);
}

void load_destinations ()
{
    int i;
    char *digest;
    char input[64];

    for (i = 0; i < 100; i++)
	{
	    sprintf (input, "Random:%d", i + 11111);
	    digest = sha1_keygen2 (input, digest);
	    str_to_key (digest, &destinations[i]);
	}

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
    char dest[16];
    char msg[200];
    char m[200];
    ChimeraHost ch;
    double wtime;
    int type;
    int x;

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

    if ((argc - optind) != 4)
	{
	    fprintf (stderr, "usage: %s\n", USAGE);
	    exit (1);
	}
    port = atoi (argv[optind]);
    str_to_key (argv[optind + 1], &key);
    wtime = atof (argv[optind + 2]);
    type = atoi (argv[optind + 3]);


    //  printf("port:%d key:%s wtime:%.2f\n",port,key.keystr,wtime);      

    state = chimera_init (port);
    chimera_setkey (state, key);
    if (hn != NULL)
	{
	    join = host_get (state, hn, joinport);
	}

    chimera_forward (state, test_fwd);
    chimera_deliver (state, test_del);
    chimera_register (state, 20, 1);

    // log_direct(state->log, LOG_DEBUG, stderr);
    //log_direct(state->log,LOG_NETWORKDEBUG, stderr);
    //log_direct(state->log,LOG_WARN, stderr);
    chimera_register (state, 21, 1);
    chimera_register (state, 22, 1);
    chimera_register (state, 23, 1);
    message_handler (state, 24, test_init, 1);

    load_destinations ();
    driver = host_get (state, "marrow", 11110);

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


    dsleep (wtime);

    if (type == 1 || type == 2)
	{
	    for (i = 0; i < 100; i++)
		{

		    fprintf (stderr, "initiating msg %d to %s from %s\n", i,
			     destinations[i].keystr, key.keystr);
		    sprintf (msg, "%d %s", i, destinations[i].keystr);

		    chimera_send (state, destinations[i], 20,
				  strlen (msg) + 1, msg);

		    fflush (stderr);
		    dsleep (2);
		}
	}
    //dsleep(1000);

    dsleep (50);
    if (type == 1)
	{
	    for (i = 0; i < 100; i++)
		{

		    fprintf (stderr, "initiating msg %d to %s from %s\n",
			     i + 100, destinations[i].keystr, key.keystr);
		    sprintf (msg, "%d %s", i + 100, destinations[i].keystr);

		    chimera_send (state, destinations[i], 20,
				  strlen (msg) + 1, msg);

		    fflush (stderr);
		    dsleep (2);
		}
	}
    if (type == 3)
	{
	    while (1)
		{
		    for (i = 0; i < 100000; i++)
			{
			    x++;
			    x--;
			}
		    dsleep (0.01);

		}
	}
    pthread_exit (NULL);
}

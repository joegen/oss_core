/*
 ** Author: Krishna Puttaswamy
 ** This test is to store and retrieve blocks in dht.
 **   Bring up a network of specified size, insert blocks into it and finally retrieve the blocks
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "dht.h"
#include "chimera.h"
#include "message.h"
#include "log.h"
#include "key.h"
#include "jrb.h"
#include "jval.h"
#include "semaphore.h"
#include "dtime.h"

extern char *optarg;
extern int optind;
#define OPTSTR "f:"
#define USAGE "./dhttest malice_percentage"
#define EXECUTABLE_NAME "./dht"

int malice_percentage;
int announced;
void *sem;
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
		Host *host = (Host *) malloc (sizeof (Host));
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

void redirect_to_chimera(ChimeraState * chstate, Message * msg)
{
	fprintf(stderr, " inside redirect: \n");
    chimera_route (chstate, &msg->dest, msg, NULL);
}

void hello (ChimeraState * chstate, Message * msg)
{
	fprintf (stderr, "inside hello: %s\n", msg->payload);
	announced++;
	sema_v (sem);
}

void start_host (Host * host, Host * join)
{
	char port[16], key[160];
	char ssh[100] = "ssh";
	memset (port, 0, 16);
	memset (key, 0, 160);
	char *arg[9];
	int i;

	char currentHost[256];
	gethostname(currentHost, 100); // get the currenthostname
	//fprintf(stderr, "The hostname is %s \n", currentHost);

	sprintf (port, "%d", host->port);
	sprintf (key, "%s", host->key.keystr);
	//  arg[0] = "ssh";
	//  arg[1] = host->name;
	char sshcommand[1000];
	int rand_num = rand()%100;
	int malicious = 0;
	// for now, all nodes are malicious only of type MALICE_ALL_BOOTSTRAP
	if (rand_num < malice_percentage)
		malicious = MALICE_ALL_BOOTSTRAP;
	char malice[10];
	memset (malice, 0, 10);
	sprintf(malice, "%d", malicious);


	arg[0] = EXECUTABLE_NAME;
	if (join != NULL)
	{
		arg[1] = "-j";
		arg[2] = join->hnp;
		arg[3] = port;
		arg[4] = key;
        arg[5] = malice;
		arg[6] = NULL;
		sprintf(sshcommand, "%s %s %s %s %s %s", arg[0], arg[1], arg[2], arg[3], arg[4], arg[5]);
	}
	else
	{
		arg[1] = port;
		arg[2] = key;
		arg[3] = malice;
		arg[4] = NULL;
		sprintf(sshcommand, "%s %s %s %s", arg[0], arg[1], arg[2], arg[3]);
	}

	//:fprintf(stderr, "nodehost is %s and currenthost is %s \n", host->name, currentHost);
	if (strcmp(host->name, "localhost")==0 || strcmp(host->name, currentHost) == 0)
	{
	}
	else
	{
		arg[0] = ssh;
		arg[1] = host->name;
		arg[2] = sshcommand;
		arg[3] = NULL;
		//	fprintf(stderr, "Ssh command is %s \n", sshcommand);
	}
	host->pid = fork ();

	if (host->pid == 0)
	{
		if (strcmp(arg[0], "ssh") == 0)
		{
//		    fprintf(stderr, "The ssh command to be executed on %s is %s \n", arg[1], arg[2]);
			execvp("/usr/bin/ssh", arg);
		}
		else
		{
			execvp (EXECUTABLE_NAME, arg);
		}

		fprintf(stderr, "ERROR ERROR ERROR!!!! THE EXECVP RETURNED in the driver \n");
		perror (EXECUTABLE_NAME);
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


bool read_from_file(FILE *fp, char *block)
{
	// assumption is that the file contains each entry of size entry_size
	if (fgets(block, ENTRY_SIZE, fp) == NULL)
		return false;

	int i = 0;
	// replace new line -- just to making the output pretty
	int len = strlen(block);
	for(i = 0; i < len; i++)
		if(block[i] == '\n')
		{
			block[i] = '\0';
			break;;
		}

	return true;
}

int main (int argc, char **argv)
{
	int opt;
	char *configfile = HOSTS_FILE;
	JRB hosts, node, node2;
	Host *host, *join, *me;
	int nhosts;
	char dest[160];
	char msg[256];
	Key tmp;
	ChimeraHost ch;
	double start;
	int i = 0;

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
    if ((argc - optind) != 1)
	{
	    fprintf (stderr, "usage: %s\n", USAGE);
	    exit (1);
	}

    malice_percentage = atoi (argv[optind]);
	fprintf(stdout, "The network is supposed to have %d percentage of malice \n", malice_percentage);
	fprintf(stdout, "The blocks file should have %d blocks \n", TEST_NETWORK_SIZE * BLOCKS_PER_FILE * FILES_PER_NODE);

	FILE *blockFP = fopen(BLOCKS_FILE, "r");
	int count = 0;
	// assuming that the contents of base16 keys that are not more than 100 bytes long
	char block[100];
	while(fgets(block, 100, blockFP)!= NULL)
	{
		count++;
	}
	fclose(blockFP);

	// if there is not enuf blocks, exit
	if (count != (TEST_NETWORK_SIZE * BLOCKS_PER_FILE * FILES_PER_NODE))
	{
		fprintf(stderr, "The file doesn't contain enough IDs... please regenerate the file  \n");
		exit(1);
	}

	sem = sema_create (0);
	state = (ChimeraState *) malloc (sizeof (ChimeraState));

	hosts = read_hosts (configfile, &nhosts);

	state->log = log_init ();
	log_direct (state->log, LOG_ERROR, stderr);
	log_direct (state->log, LOG_WARN, stderr);

	key_init ();
	state->message = message_init (state, 11110);
	message_handler (state, HELLO_MESSAGE, hello, 1);
	//TODO: verify the statement below
	message_handler (state, GET_COMMAND, redirect_to_chimera, 1);
	message_handler (state, PUT_COMMAND, redirect_to_chimera, 1);
	message_handler (state, DHT_DUMP_STATE, redirect_to_chimera, 1);

	srand (time (NULL));

	/* This part runs cashmere in different hosts */

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
		dsleep (0.05);
	}
	dsleep(61);

	int  k = 0;
	blockFP = fopen(BLOCKS_FILE, "r");
	while(1)
	{
		if (read_from_file(blockFP, block) == false)
			break;

		i = (rand() % announced) + 1;
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

		// insert a block
		Message *m = message_create (ch.key, PUT_COMMAND, strlen(block) + 1, block);
		message_send (state, &ch, m, TRUE);
		k++;
	}
	fclose(blockFP);
	fprintf(stdout, "Inserted %d blocks into the DHT ...now looking them up\n", k);

	blockFP = fopen(BLOCKS_FILE, "r");
	int breakout = 0;
	count = 0;
	while(1)
	{
		if (breakout == 1)
			break;

		i = (rand() % announced) + 1;
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

		// each node will lookup alls the blocks for the avg. number of files on each block
		int k = 0;
		for (k = 0; k < FILES_PER_NODE; k++)
		{
			if ( breakout == 1) break;

			int j = 0;
			for (j = 0; j < BLOCKS_PER_FILE; j++)
			{
				if (read_from_file(blockFP, block) == false)
				{
					breakout = 1;
					break;
				}

				fprintf(stderr, "Looking for %s \n", block);
				Message *m = message_create (ch.key, GET_COMMAND, strlen(block) + 1, block);
				message_send (state, &ch, m, TRUE);
				count++;
			}
			dsleep(.1);
		}
		dsleep(.2);
	}
	fclose(blockFP);
	fprintf(stdout, "Lookedup %d blocks into the DHT \n", count);

	// dump hashtable statistics of each node at the end of expt
	jrb_traverse (node, hosts)
	{
		host = (Host *) node->val.v;
		ch.name = host->name;
		ch.address = network_address (state->network, host->name);
		ch.port = host->port;
		key_assign (&ch.key, host->key);
		Message *m = message_create (ch.key, DHT_DUMP_STATE, strlen(host->name) + 1, host->name);
		message_send (state, &ch, m, TRUE);
	}
}

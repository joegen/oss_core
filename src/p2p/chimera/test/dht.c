/*
** Krishna Puttaswamy
** A simple DHT application on top of Chimera
** supports put/get with replications; the put/get is only for blockids, this is not a complete dht that stores blocks
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "dht.h"
#include "message.h"
#include "log.h"
#include "key.h"
#include "jrb.h"
#include "jval.h"
#include "dtime.h"
#include "route.h"

#define USAGE "dht [ -j bootstrap:port ] port key malice_type"
#define OPTSTR "j:"

#define RAND() (lrand48())
#define SEED(s) (srand48(s))

extern char *optarg;
extern int optind;

int malice_type = 0;
ChimeraHost *driver = NULL;
ChimeraState *state;
Key key;
char mykey[200];
int seq = -1;
int my_put_count = 0, my_get_count = 0;
int ignore_dht_get_nonroot = 0, ignore_dht_get_root = 0, ignore_dht_put_nonroot = 0, ignore_dht_put_root = 0;
int found_blocks = 0, missing_blocks = 0;

Key randomKeys[TEST_NETWORK_SIZE];
int randomKeysCount = 0;

char hashTable[HASH_TABLE_SIZE][ENTRY_SIZE];
int hashTableSize = 0;

// put the block in the dht
void dht_put(char *block)
{
	// current implementation is dummy... its just storing the block's Id
	if (hashTableSize >= HASH_TABLE_SIZE)
		fprintf(stderr, "ERROR: the hashtable is full \n");
	else
	{
        strcpy(hashTable[hashTableSize++], block);
//	    fprintf(stdout, "putting %s \n", block);
	}
}

bool dht_get(char *blockId)
{
	int i = 0;
	for( i = 0; i < hashTableSize; i++)
		if (strcmp(blockId, hashTable[i]) == 0)
			return true;

	return false;
}

void forward_handler (Key ** kp, Message ** mp, ChimeraHost ** hp)
{
	Message *m = *mp;
	if (m->type == DHT_GET_ROOT || m->type == DHT_PUT_ROOT || m->type == DHT_GET_NONROOT || m->type == DHT_PUT_NONROOT)
	{ 
		DhtMessage *dhtmsg = (DhtMessage *) (m->payload);
		dhtmsg->hops++;

		// if malicious, change the next-hop to yourself
		if (malice_type == MALICE_ALL_BOOTSTRAP && strcmp(mykey, m->source.keystr))
		{
			str_to_key(mykey, &(mp[0]->dest)); // set the destination to self
    		hp[0] = ((ChimeraGlobal *)state->chimera)->me; // and set next hop to self
			dhtmsg->hijacked = true; //set the bool to true..this means that the message is hijacked and should be treated differently when delivered
		}
		fprintf(stderr, "source: %s hijacker: %s \n", m->source.keystr, mykey);
	}
}

int getRandomNumber ()
{
    struct timeval tm;
    gettimeofday (&tm, NULL);
    SEED (tm.tv_sec + tm.tv_usec);
    return RAND () % 1000;
}

Key** get_right_leafset(ChimeraState *state, int *rsize)
{
	int i = 0;
	Key **rightLeafKeys;
    RouteGlobal *routeglob = (RouteGlobal *) state->route;

    pthread_mutex_lock (&routeglob->lock);
    *rsize = leafset_size (routeglob->rightleafset);
	rightLeafKeys = (Key **)malloc(*rsize * sizeof(Key *));
	for (i = 0; i < *rsize; i++)
	{
		rightLeafKeys[i] = (Key *) malloc(sizeof(Key));
		key_assign(rightLeafKeys[i],  routeglob->rightleafset[i]->key);
	}
    pthread_mutex_unlock (&routeglob->lock);

	return rightLeafKeys;
}

void broadcast_to_replicas(int msg_type, int size, char *payload)
{
	// send it to the DHT_REPLICATION number of ppl in your right leafset
	// The assumption is that there are DHT_REPLICATION number of ppl in your right leafset
	RouteGlobal *routeglob = (RouteGlobal *) state->route;
	int rsize;
	Key **rightleafkeys = get_right_leafset(state, &rsize);
	int i = 0;
	for (i = 0; i < rsize && i < DHT_REPLICATION; i++)
	{
		Key tempKey;
		key_assign(&tempKey, *rightleafkeys[i]);

		if (msg_type == DHT_PUT_NONROOT)
		{
			chimera_send (state, tempKey,
					DHT_PUT_NONROOT, size, payload);
		}
		else if (msg_type == DHT_GET_NONROOT)
		{
			chimera_send (state, tempKey,
					DHT_GET_NONROOT, size, payload);
		}
		else
		{
			fprintf(stderr, "ERROR: wrong message type in broadcast to replicas \n");
		}
	}
}

void printResult(char *blockId, int hops)
{
	if (dht_get(blockId) == true)
	{
		fprintf(stderr, "Found the block %s in %d hops \n", blockId, hops);
		found_blocks++;
	}
	else
	{
		fprintf(stderr, "Block %s is missing even after %d hops \n", blockId, hops);
		missing_blocks++;
	}
	fflush(stderr);
}

// take care of the messages delivered to a node
void delivery_handler(Key * key, Message * msg)
{
    if(msg->type == DHT_GET_ROOT || msg->type == DHT_GET_NONROOT)
	{
		my_get_count++;
	}
	if(msg->type == DHT_PUT_ROOT || msg->type == DHT_PUT_NONROOT)
	{
		my_put_count++;
	}
	
	if (msg->type == DHT_GET_ROOT || msg->type == DHT_PUT_ROOT || msg->type == DHT_GET_NONROOT || msg->type == DHT_PUT_NONROOT)
	{ 
		// only if the message is hijacked, ignore it
		DhtMessage *dhtmessage = (DhtMessage *)msg->payload;
		if (dhtmessage->hijacked == true)
		{
			if (msg->type == DHT_PUT_ROOT)
			{
				ignore_dht_put_root++;
			}
			else if (msg->type == DHT_PUT_NONROOT)
			{
				ignore_dht_put_nonroot++;
			}
			else if (msg->type == DHT_GET_ROOT)
			{
				ignore_dht_get_root++;
				// reply to the source 
			}
			else if (msg->type == DHT_GET_NONROOT)
			{
				ignore_dht_get_nonroot++;
				// reply to the source 
			}
			return;
		}
	}

 	Key newKey;
    if (msg->type == PUT_COMMAND)
	{
		DhtMessage *send = get_new_dhtmessage(msg->payload);
		str_to_key(msg->payload, &newKey);
		chimera_send(state, newKey, DHT_PUT_ROOT, sizeof(DhtMessage), (char *)send);
	}
    if (msg->type == GET_COMMAND)
	{
		DhtMessage *send = get_new_dhtmessage(msg->payload);
		str_to_key(msg->payload, &newKey);
		chimera_send(state, newKey, DHT_GET_ROOT, sizeof(DhtMessage), (char *)send);
	}
    else if (msg->type == DHT_PUT_ROOT)
	{
		DhtMessage *dhtmsg = (DhtMessage *) msg->payload;
		dht_put(dhtmsg->blockId);
		broadcast_to_replicas(DHT_PUT_NONROOT, msg->size, msg->payload);
	}
    else if (msg->type == DHT_PUT_NONROOT)
	{
	    log_message (state->log, LOG_DATA,
			 "calling dht_put_nonroot \n");
		DhtMessage *dhtmsg = (DhtMessage *) msg->payload;
		dht_put(dhtmsg->blockId);
	}
    else if (msg->type == DHT_GET_ROOT)
	{
		DhtMessage *dhtmsg = (DhtMessage *) msg->payload;
		printResult(dhtmsg->blockId, dhtmsg->hops);
		broadcast_to_replicas(DHT_GET_NONROOT, msg->size, msg->payload);
	}
	else if (msg->type == DHT_GET_NONROOT)
	{
		DhtMessage *dhtmsg = (DhtMessage *) msg->payload;
		printResult(dhtmsg->blockId, dhtmsg->hops);
	}
	else if (msg->type == DHT_DUMP_STATE)
	{
		fprintf(stderr, "%s has %d entries in its hashtable \n", mykey, hashTableSize);
		fprintf(stderr, "my_get_count: %d my_put_count: %d \n", my_get_count, my_put_count);
		fprintf(stderr, "ignored counters  dht_get_root: %d dht_get_nonroot: %d dht_put_root: %d dht_put_nonroot: %d \n",
				ignore_dht_get_root, ignore_dht_get_nonroot, ignore_dht_put_root, ignore_dht_put_nonroot);
		fprintf(stderr, "found_blocks: %d missing_blocks: %d \n", found_blocks, missing_blocks);
		fflush(stderr);
	}
}

void read_hosts_entries()
{
	FILE *fp = fopen (HOSTS_FILE, "r");
	if (fp == NULL)
	{
		perror ("unable to open the hosts file \n");
		exit (1);
	}
	char hn[200], keyinput[200];
	int port;
	char s[1000];
	randomKeysCount = 0;

	while (fgets (s, 256, fp) != NULL)
	{
		if (randomKeysCount == TEST_NETWORK_SIZE-1) break;

		sscanf (s, "%s %d %s", hn, &port, keyinput);
		str_to_key (keyinput, &randomKeys[randomKeysCount]);
		randomKeysCount++;
	}
	fclose (fp);
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

    if ((argc - optind) != 3)
	{
	    fprintf (stderr, "usage: %s\n", USAGE);
	    exit (1);
	}

    port = atoi (argv[optind]);
    str_to_key (argv[optind + 1], &key);
	malice_type = atoi(argv[optind + 2]);
    state = chimera_init (port);
    chimera_setkey (state, key);
	strcpy(mykey, key.keystr);
	srand (time (NULL));

    if (hn != NULL)
	{
	    join = host_get (state, hn, joinport);
	}

	if (malice_type != MALICE_NONE)
	{
		fprintf(stderr, "%s is malicious of type %d \n", mykey, malice_type);
	}

    chimera_register (state, HELLO_MESSAGE, 1);
    chimera_register (state, PUT_COMMAND, 1);
    chimera_register (state, GET_COMMAND, 1);
    chimera_register (state, DHT_GET_ROOT, 1);
    chimera_register (state, DHT_GET_NONROOT, 1);
    chimera_register (state, DHT_PUT_NONROOT, 1);
    chimera_register (state, DHT_PUT_ROOT, 1);
    chimera_register (state, DHT_DUMP_STATE, 1);

    char networkdebug[256], routingdebug[256];
    //      sprintf(networkdebug, "LOG_NETWORKDEBUG_%s", keyinput.keystr); 
    //sprintf (routingdebug, "LOG_ROUTING_%s", key.keystr);
    //      FILE *networklogfp= fopen(networkdebug, "w");
    //FILE *routingfp = fopen (routingdebug, "w");
    //log_direct (state->log, LOG_ROUTING, routingfp);
    //      log_direct(state->log, LOG_NETWORKDEBUG, networklogfp);

    char cashmeredebug[256];
    sprintf (cashmeredebug, "logs/LOG_DATA%s", key.keystr);
    FILE *cashmerefp = fopen (cashmeredebug, "w");
    log_direct (state->log, LOG_DATA, cashmerefp);
    //log_direct (state->log, LOG_WARN, stderr);
    //log_direct (state->log, LOG_ERROR, stderr);

    read_hosts_entries();
    chimera_deliver (state, delivery_handler);
	chimera_forward (state, forward_handler);

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

    hello =
	message_create (driver->key, HELLO_MESSAGE, strlen (tmp) + 1, tmp);
    while(!message_send (state, driver, hello, TRUE));
    free (hello);

    while (1)
	{
	    sleep (1000);
	}
}

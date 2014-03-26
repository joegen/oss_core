
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <openssl/evp.h>
#include "message.h"
#include "log.h"
#include "host.h"
#include "jrb.h"
#include "jval.h"
#include "semaphore.h"
#include <sys/types.h>
#include <signal.h>
#include "dtime.h"
#include <errno.h>


extern int errno;
extern char *optarg;

#define OPTSTR "f:"
#define USAGE "monitor"
#define DEBUG 0
#define NON_STABLE 2		//evry other NON_STABLE instance will be non_stable
#define INTERVAL 0.3

int announced;
void *sem_msg;
void *sem_join;
int k_seq;
int hops, first;
int nhosts;
int nstable, nnon_stable;
ChimeraState *state;
Key current;
Key destinations[100];

JRB stable, non_stable;
//Host **stable_arr;
//Host **non_stable_arr;

pthread_mutex_t lock;
pthread_attr_t attr;
pthread_t tid;

void *network_churn ();

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
	    sscanf (s, "%s %d %s", hn, &port, key.keystr);
	    str_to_key (key.keystr, &key);
	    host = (Host *) malloc (sizeof (Host));
	    host->name = strdup (hn);
	    host->port = port;
	    key_assign (&host->key, key);
	    //    host->key = key;
	    sprintf (s, "%s:%d", hn, port);
	    host->hnp = strdup (s);
	    host->pid = 0;
	    jrb_insert_str (hosts, strdup (s), new_jval_v (host));
	    (*nhosts)++;
	}

    fclose (fp);

    return (hosts);
}

void create_trees (JRB hosts)
{
    int i = 0;
    JRB node;
    Host *host;

    nstable = 0;
    nnon_stable = 0;

    stable = make_jrb ();
    non_stable = make_jrb ();

    jrb_traverse (node, hosts)
    {

	host = (Host *) node->val.v;
	if (i++ % NON_STABLE == 0)
	    {
		jrb_insert_str (non_stable, host->hnp, new_jval_v (host));
		nnon_stable++;
	    }
	else
	    {
		jrb_insert_str (stable, host->hnp, new_jval_v (host));
		nstable++;
	    }
    }
}

void print_tree (JRB t)
{
    int i;
    Host *host;
    JRB node;

    jrb_traverse (node, t)
    {
	host = (Host *) node->val.v;
	fprintf (stderr, "key:%s pid:%d\n", host->key.keystr, host->pid);
    }
    fflush (stderr);
}


void hello (ChimeraState * chstate, Message * msg)
{

    fprintf (stdout, "hello: %s\n", msg->payload);
    announced++;
    // sema_v(sem_join);
    fflush (stdout);
}

void fwd (ChimeraState * chstate, Message * msg)
{

    Key cur, dest;
    int seq;

    sscanf (msg->payload, "%s %s %d", cur.keystr, dest.keystr, &seq);
    str_to_key (cur.keystr, &cur);
    str_to_key (dest.keystr, &dest);
    if (DEBUG)
	{
	    printf ("BIG-FWD:%s\n", msg->payload);
	    fprintf (stderr, "message %d seq %d to %s routed by %s\n", seq,
		     k_seq, dest.keystr, cur.keystr);
	}

    if (key_equal (dest, current))
	{
	    hops++;
	}
}

void del (ChimeraState * chstate, Message * msg)
{

    Key cur, dest;
    int seq;

    pthread_mutex_lock (&lock);
    sscanf (msg->payload, "%s %s %d", cur.keystr, dest.keystr, &seq);
    str_to_key (cur.keystr, &cur);
    str_to_key (dest.keystr, &dest);

    if (seq == 100 & first == 1)
	{
	    fprintf (stderr, "+++++++++++++++++++++++++++++++++++++++\n");
	    first = 0;
	    if (pthread_create (&tid, &attr, network_churn, NULL) != 0)
		{
		    fprintf (stderr, "pthread_mutex_init: %s",
			     strerror (errno));
		    exit (0);
		}
	    pthread_mutex_unlock (&lock);
	}

    //if(DEBUG){
    //printf("BIG-DEL:%s\n", msg->payload);
    //printf(stderr, "message %d seq %d to %8x delivered to %8x\n", seq,k_seq,dest, cur);
    // }

    /*
       if(dest == current) {
       fprintf(stderr, "message %d seq %d to %8x delivered to %8x\n", seq,k_seq,dest, cur);
       sema_v(sem_msg);
       }
       else {
       fprintf(stderr, "==== message %d seq %d to %8x delivered to %8x\n", seq,k_seq,dest, cur);
       fprintf(stderr, "==== msg:%d seq:%d current:%8x dest:%8x cur:%8x\n", seq,k_seq,current,dest,cur);
       }
     */

    fflush (stdout);
}

void start_host (Host * host, Host * join, float num, int active)
{

    char port[16];
    char *arg[9];
    char num_str[9];
    char active_str[5];
    int i;

    sprintf (port, "%d", host->port);
    //  sprintf(key, "%x", host->key);
    sprintf (num_str, "%.2f", num);
    sprintf (active_str, "%d", active);

//  arg[0] = "ssh";
//  arg[1] = host->name;
    arg[0] = "./bignode";
    if (join != NULL)
	{
	    fprintf (stderr, "have -j in the beginning \n");
	    arg[1] = "-j";
	    arg[2] = join->hnp;
	    arg[3] = port;
	    arg[4] = host->key.keystr;
	    arg[5] = num_str;
	    arg[6] = active_str;
	    arg[7] = NULL;
	}
    else
	{
	    fprintf (stderr, "no bootstrap node\n");
	    arg[1] = port;
	    arg[2] = host->key.keystr;
	    arg[3] = num_str;
	    arg[4] = active_str;
	    arg[5] = NULL;
	}

    //  for(i=0; arg[i] != NULL; i++)
    //    printf("%s ", arg[i]);
    //  printf("\n");


    host->pid = fork ();

    if (host->pid == 0)
	{
	    execvp ("./bignode", arg);
	    perror ("./bignode");
	    exit (1);
	}

    //sema_p(sem_join, 0.0);
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
    char *configfile = "hosts";
    JRB hosts, node, node2;
    Host *host, *join, *me;
    int i, j;
    int k = 0;
    Message *m;
    char msg[160];
    ChimeraHost ch;
    int status = 0;
    float wtime;
    int z;

    first = 1;
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

    sem_msg = sema_create (0);
    sem_join = sema_create (0);
    state = (ChimeraState *) malloc (sizeof (ChimeraState));

    me = (Host *) malloc (sizeof (Host));
    me->name = "localhost";
    me->port = 11110;
    me->hnp = "localhost:11110";

    load_destinations ();
    hosts = read_hosts (configfile, &nhosts);

    state->log = log_init ();
    key_init ();
    state->message = message_init (state, 11110);
    message_handler (state, 21, hello, 1);
    message_handler (state, 22, fwd, 1);
    message_handler (state, 23, del, 1);
    srand (time (NULL));


    /* This part runs bighost in different hosts */
    z = 1;
    nstable = 0;
    nnon_stable = 0;
    announced = 0;
    stable = make_jrb ();
    non_stable = make_jrb ();

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
	wtime = (nhosts - announced) * INTERVAL;
	if (z++ % NON_STABLE == 0)
	    {
		//      start_host(host, join, wtime,2);  
		start_host (host, join, wtime, 1);
		jrb_insert_str (non_stable, host->hnp, new_jval_v (host));
		nnon_stable++;
	    }
	else
	    {
		start_host (host, join, wtime, 1);
		jrb_insert_str (stable, host->hnp, new_jval_v (host));
		nstable++;
	    }
	dsleep (INTERVAL);
    }

    fprintf (stderr, "The number of stables is %d and non-stables is %d \n",
	     stable, non_stable);
    fprintf (stderr, " STABLES \n");
    print_tree (stable);
    fprintf (stderr, " NON-STABLES \n");
    print_tree (non_stable);
    fflush (stderr);

    //this part sends a message to two random hosts in the tree 
    if (pthread_attr_init (&attr) != 0)
	{
	    fprintf (stderr, "pthread_attr_init: %s", strerror (errno));
	    exit (0);
	}
    if (pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM) != 0)
	{
	    fprintf (stderr, "pthread_attr_setscope: %s", strerror (errno));
	    exit (0);
	}
    if (pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED) != 0)
	{
	    fprintf (stderr, "pthread_attr_setdetachstate: %s",
		     strerror (errno));
	    exit (0);
	}

    if (pthread_mutex_init (&lock, NULL) != 0)
	{
	    fprintf (stderr, "pthread_mutex_init: %s", strerror (errno));
	    exit (0);
	}

    while (1)
	{
	    for (i = 0; i < 10000; i++)
		;
	    dsleep (0.5);
	}
    //wait_pid(-1, );
}

void *network_churn ()
{

    JRB hosts, node, node2;
    Host *host, *join;
    int i = 1;
    int l = 1;

    jrb_traverse (node, non_stable)
    {
	host = (Host *) node->val.v;
	// kill 2/3 of the unstable node this mean 20% of all nodes 
	if (l-- % 3 != 0)
	    {
		fprintf (stdout, "Killing (%d) %s\n", host->pid,
			 host->key.keystr);
		kill (host->pid, SIGKILL);
		host->pid = 0;
	    }
	else
	    {
		fprintf (stdout, "No Killing a node \n");
	    }
	fflush (stdout);
    }

    dsleep (50);
    srand (time (NULL));

    while (1)
	{

	    //    fprintf(stderr," nnon_stable:%d nstable:%d\n",nnon_stable,nstable);

	    // pick a live random node from unstable stack to remove from the network

	    do
		{
		    i = rand () % nnon_stable;
		    jrb_traverse (node, non_stable)
		    {
			if (i-- == 0 || i < 0)
			    break;
		    }
		    host = (Host *) node->val.v;
		    //fprintf(stderr, "HOST TO KILL1: pid=%d key=%x i=%d\n",host->pid,host->key,i);
		}
	    while (host->pid == 0);

	    fprintf (stderr, "Killing2 (%d) %s\n", host->pid,
		     host->key.keystr);
	    kill (host->pid, SIGKILL);
	    host->pid = 0;
	    fflush (stdout);


	    //dsleep(10/nnon_stable );
	    dsleep (1);

	    // pick a random node from stable stack to join to 
	    i = rand () % nstable;
	    jrb_traverse (node2, stable)
	    {
		if (i-- == 0)
		    break;
	    }
	    join = (Host *) node2->val.v;
	    fprintf (stderr, "HOST TO JOIN: pid=%d key=%x\n", join->pid,
		     join->key);

	    if (join == NULL)
		{
		    fprintf (stderr, " ERROR in joining host \n");
		    fflush (stderr);
		    exit (0);
		}

	    // pick a dead random node from unstable stack to add to network 
	    do
		{
		    i = rand () % nnon_stable;
		    jrb_traverse (node, non_stable)
		    {
			if (i-- == 0 || i < 0)
			    break;
		    }
		    host = (Host *) node->val.v;
		}
	    while (host->pid != 0);

	    host = (Host *) node->val.v;
//    fprintf(stderr, "HOST TO JOIN: pid=%d key=%x i=%d\n",host->pid,host->key,i);
	    start_host (host, join, 0.01, 3);

//    dsleep(5/nnon_stable);

	}

}

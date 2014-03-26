/*
** $Id: network.c,v 1.30 2007/04/04 00:04:49 krishnap Exp $
**
** Matthew Allen
** description: 
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <netdb.h>
#include <pthread.h>
#include "network.h"
#include "host.h"
#include "message.h"
#include "log.h"
#include "semaphore.h"
#include "jrb.h"
#include "jval.h"
#include "dtime.h"

extern int errno;
#define SEND_SIZE NETWORK_PACK_SIZE

typedef struct
{
    int sock;
    JRB waiting;
    unsigned long seqstart, seqend;
    pthread_mutex_t lock;
	JRB retransmit;
} NetworkGlobal;

// allocate a new pointer and return it
PQEntry* get_new_pqentry()
{
	PQEntry* entry = (PQEntry *)malloc(sizeof(PQEntry));
	entry->desthost = NULL;
	entry->data = NULL;
	entry->datasize = 0;
	entry->retry = 0;
	entry->seqnum = 0;
	entry->transmittime = 0.0;

	return entry;
}

AckEntry* get_new_ackentry()
{
	AckEntry *entry = (AckEntry *)(malloc(sizeof(AckEntry)));
	entry->acked = 0;
	entry->acktime = 0.0;

	return entry;
}

/** network_address:
 ** returns the ip address of the #hostname#
 */
unsigned long network_address (void *networkglobal, char *hostname)
{

    int is_addr;
    struct hostent *he;
    unsigned long addr;
    unsigned long local;
    int i;
    NetworkGlobal *netglob = (NetworkGlobal *) networkglobal;


    /* apparently gethostbyname does not portably recognize ip addys */

#ifdef SunOS
    is_addr = inet_addr (hostname);
    if (is_addr == -1)
	is_addr = 0;
    else
	{
	    memcpy (&addr, (struct in_addr *) &is_addr, sizeof (addr));
	    is_addr = inet_addr ("127.0.0.1");
	    memcpy (&local, (struct in_addr *) &is_addr, sizeof (addr));
	    is_addr = 1;
	}
#else
    is_addr = inet_aton (hostname, (struct in_addr *) &addr);
    inet_aton ("127.0.0.1", (struct in_addr *) &local);
#endif

    pthread_mutex_lock (&(netglob->lock));
    if (is_addr)
	he = gethostbyaddr ((char *) &addr, sizeof (addr), AF_INET);
    else
	he = gethostbyname (hostname);

    if (he == NULL)
	{
	    pthread_mutex_unlock (&(netglob->lock));
	    return (0);
	}

    /* make sure the machine is not returning localhost */

    addr = *(unsigned long *) he->h_addr_list[0];
    for (i = 1; he->h_addr_list[i] != NULL && addr == local; i++)
	addr = *(unsigned long *) he->h_addr_list[i];
    pthread_mutex_unlock (&(netglob->lock));

    return (addr);

}

/** network_init:
 ** initiates the networking layer by creating socket and bind it to #port# 
 */

void *network_init (void *logs, int port)
{
    int sd;
    int ret;
    struct sockaddr_in saddr;
    int one;
    NetworkGlobal *ng;

    ng = (NetworkGlobal *) malloc (sizeof (NetworkGlobal));

    /* create socket */
    sd = socket (AF_INET, SOCK_DGRAM, 0);
    if (sd < 0)
	{
	    if (LOGS)
	      log_message (logs, LOG_ERROR, "network: socket: %s\n",
			   strerror (errno));
	    return (NULL);
	}
    if (setsockopt (sd, SOL_SOCKET, SO_REUSEADDR, (void *) &one, sizeof (one))
	== -1)
	{
	    if (LOGS)
	      log_message (logs, LOG_ERROR, "network: setsockopt: %s\n: ",
			   strerror (errno));
	    close (sd);
	    return (NULL);
	}

    /* attach socket to #port#. */
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl (INADDR_ANY);
    saddr.sin_port = htons ((short) port);
    if (bind (sd, (struct sockaddr *) &saddr, sizeof (saddr)) < 0)
	{
	    if (LOGS)
	      log_message (logs, LOG_ERROR, "network: bind: %s:\n",
			   strerror (errno));
	    close (sd);
	    return (NULL);
	}

    if ((ret = pthread_mutex_init (&(ng->lock), NULL)) != 0)
	{
	    if (LOGS)
	      log_message (logs, LOG_ERROR,
			   "network: pthread_mutex_init: %s:\n",
			   strerror (ret));
	    close (sd);
	    return (NULL);
	}

    ng->sock = sd;
    ng->waiting = make_jrb();
    ng->seqstart = 0;
    ng->seqend = 0;
	ng->retransmit = make_jrb();

    return ((void *) ng);
}

/** Never returns. Keep retransmitting the failed packets.
 **/
void *retransmit_packets(void *state)
{
	JRB pqnode, node;
	ChimeraState *chstate = (ChimeraState *) state;
	NetworkGlobal *ng = (NetworkGlobal *) chstate->network;
	double now = 0;
	PQEntry* pqentry;

	//JRB tempjrb = make_jrb(); -- to try doing send outside the lock block
	while(1)
	{
		// wake up, get all the packets to be transmitted by now, send them again or delete them from the priqueue
		now = dtime();
		int resend = 0;

		pthread_mutex_lock (&(ng->lock));
		for(pqnode = jrb_first(ng->retransmit); pqnode != jrb_nil(ng->retransmit) && pqnode->key.d <= now; pqnode = jrb_next(pqnode))
		{
			pqentry = (PQEntry *) pqnode->val.v;
			//kpkp - debug enable
			//	fprintf(stderr, "processing a packet with retransmit time %f; looking for seqnum %d\n", pqnode->key.d, pqentry->seqnum);

			node = jrb_find_int (ng->waiting, pqentry->seqnum);
			if (node != NULL)
			{
				AckEntry *ackentry = (AckEntry *)node->val.v;
				if(ackentry->acked == 0) // means, if the packet is not yet acknowledged
				{
					// packet is not acked
					//kpkp - debug enable
					//			fprintf(stderr, "packet %d is not acked... retransmitting for %dth time\n", node->key.i, pqentry->retry);

					resend = 1;
					//jrb_insert_dbl(tempjrb, pqnode->key.d, new_jval_v(pqentry));-- to try doing send outside the lock block
					double transmittime = dtime();
					network_resend(state, pqentry->desthost, pqentry->data, pqentry->datasize, 1, pqentry->seqnum, &transmittime); 
					pqentry->retry = pqentry->retry + 1;
					if (pqentry->retry < MAX_RETRY)
					{
						PQEntry *newentry = get_new_pqentry();
						newentry->desthost = pqentry->desthost;
						newentry->data = pqentry->data;
						newentry->datasize = pqentry->datasize;
						newentry->retry = pqentry->retry;
						newentry->seqnum = pqentry->seqnum;
						newentry->transmittime = transmittime;

						jrb_insert_dbl (ng->retransmit, (transmittime+RETRANSMIT_INTERVAL), new_jval_v (newentry));
					}
					else
					{
						// max retransmission has expired -- update the host stats, free up the resources
						host_update_stat (pqentry->desthost, 0);

						free(pqentry->data);
					}
					// delete this node
					jrb_delete_node(pqnode);
				}
				else
				{
					//kpkp - enable
					//			fprintf(stderr, "packet %d is acked... deleting from DS\n", node->key.i);
					// packet is acked;
					// update the host latency and the success measurements
					host_update_stat (pqentry->desthost, 1);
					double latency = ackentry->acktime - pqentry->transmittime; 
					if(latency > 0)
					{
						//kpkp - debug enable
						//fprintf(stderr, "latency for one host is %f \n", latency);
						if (pqentry->desthost->latency == 0.0)
						{
							pqentry->desthost->latency = latency;
						}
						else
						{
							pqentry->desthost->latency =
								(0.9 * pqentry->desthost->latency) + (0.1 * latency);
						}
					}
					jrb_delete_node(node);
					jrb_delete_node(pqnode);
				}
			}
			else
			{
				//kpkp - debug enable
				//		fprintf(stderr, "didn't find the seqnum in the DS \n");
			}
		}
		pthread_mutex_unlock (&(ng->lock));

		/* -- to try doing send outside the lock block
		   if (resend != 0)
		   for(pqnode = jrb_first(tempjrb); pqnode != jrb_nil(tempjrb); pqnode = jrb_next(pqnode))
		   {
		   pqentry = (PQEntry *) pqnode->val.v;
		   network_resend(state, pqentry->desthost, pqentry->data, pqentry->datasize, 1, pqentry->seqnum); 
		   jrb_delete_node(pqnode);
		   }
		 */

		sleep(RETRANSMIT_THREAD_SLEEP);
	}
}

/**
 ** network_activate: 
 ** NEVER RETURNS. Puts the network layer into listen mode. This thread
 ** manages acknowledgements, delivers incomming messages to the message
 ** handler, and drives the network layer. It should only be called once.
 */
void *network_activate (void *state)
{
    fd_set fds, thisfds;
    int ret, retack;
    char data[SEND_SIZE];
    struct sockaddr_in from;
    int socklen = sizeof (from);
    unsigned long ack, seq;
    JRB node;
    ChimeraState *chstate = (ChimeraState *) state;
    NetworkGlobal *ng = (NetworkGlobal *) chstate->network;

    FD_ZERO (&fds);
    FD_SET (ng->sock, &fds);

    while (1)
	{
	    /* block until information becomes available */
	    memcpy (&thisfds, &fds, sizeof (fd_set));
	    ret = select (ng->sock + 1, &thisfds, NULL, NULL, NULL);
	    if (ret < 0)
		{
		  if (LOGS)
		    log_message (chstate->log, LOG_ERROR,
				 "network: select: %s\n", strerror (errno));
		    continue;
		}

	    /* receive the new data */
	    ret =
		recvfrom (ng->sock, data, SEND_SIZE, 0,
			  (struct sockaddr *) &from, &socklen);
	    if (ret < 0)
		{
		  if (LOGS)
		    log_message (chstate->log, LOG_ERROR,
				 "network: recvfrom: %s\n", strerror (errno));
		    continue;
		}
	    memcpy (&ack, data, sizeof (unsigned long));
	    ack = ntohl (ack);
	    memcpy (&seq, data + sizeof (unsigned long),
		    sizeof (unsigned long));
	    seq = ntohl (seq);

	    /* process acknowledgement */
	    if (ack == 0)
		{
		  if (LOGS)
		    log_message (chstate->log, LOG_NETWORKDEBUG,
				 "network_activate: received ack seq=%d from %s:%d\n",
				 seq, inet_ntoa (from.sin_addr),
				 from.sin_port);

		    pthread_mutex_lock (&(ng->lock));
		    node = jrb_find_int (ng->waiting, seq);
		    if (node != NULL)
			{
				AckEntry *entry = (AckEntry *)node->val.v;
				entry->acked = 1;
				entry->acktime = dtime();
		//kpkp - enable
		//		fprintf(stderr, "Received an ack for packet %d at time entry->acktime %f\n", seq, entry->acktime);
			}
		    pthread_mutex_unlock (&(ng->lock));
		}

	    /* process receive and send acknowledgement */
	    else if (ack == 1)
		{
		  if (LOGS)
		    log_message (chstate->log, LOG_NETWORKDEBUG,
				 "network_activate: received message seq=%d  data:%s\n",
				 seq, data + (2 * sizeof (unsigned long)));
		    ack = htonl (0);
		    memcpy (data, &ack, sizeof (unsigned long));
		    retack =
			sendto (ng->sock, data, 2 * sizeof (unsigned long), 0,
				(struct sockaddr *) &from, sizeof (from));
		    if (retack < 0)
			{
			  if (LOGS)
			    log_message (chstate->log, LOG_ERROR,
					 "network: sendto: %s\n",
					 strerror (errno));
			  continue;
			}
		    if (LOGS)
		      log_message (chstate->log, LOG_NETWORKDEBUG,
				   "network_activate: sent out ack for  message seq=%d\n",
				   seq);
		    message_received (state,
				      data + (2 * sizeof (unsigned long)),
				      ret - (2 * sizeof (unsigned long)));
		}
	    else if (ack == 2)
		{
		    message_received (state,
				      data + (2 * sizeof (unsigned long)),
				      ret - (2 * sizeof (unsigned long)));
		}
	    else
		{
		  if (LOGS)
		    log_message (chstate->log, LOG_ERROR,
				 "network: received unrecognized message ack=%d seq=%d\n",
				 ack, seq);
		}
	}
}

/**
 ** network_send: host, data, size
 ** Sends a message to host, updating the measurement info.
 */
int network_send (void *state, ChimeraHost * host, char *data, int size,
		  unsigned long ack)
{
    struct sockaddr_in to;
    int ret, retval;
    unsigned long seq, seqnumbackup, ntype;
	int sizebackup;
    char s[SEND_SIZE];
    void *semaphore;
    JRB node;
	JRB priqueue;
    double start;
    ChimeraState *chstate = (ChimeraState *) state;
    NetworkGlobal *ng;

    ng = (NetworkGlobal *) chstate->network;

    if (size > NETWORK_PACK_SIZE)
	{
	    if (LOGS)
	      log_message (chstate->log, LOG_ERROR,
			   "network_send: cannot send data over %lu bytes!\n",
			   NETWORK_PACK_SIZE);
	    return (0);
	}
    if (ack != 1 && ack != 2)
	{
	    if (LOGS)
	      log_message (chstate->log, LOG_ERROR,
			   "network_send: FAILED, unexpected message ack property %i !\n", ack);
	    return (0);
	}
    memset (&to, 0, sizeof (to));
    to.sin_family = AF_INET;
    to.sin_addr.s_addr = host->address;
    to.sin_port = htons ((short) host->port);

    AckEntry *ackentry = get_new_ackentry();
	sizebackup = size;
    /* get sequence number and initialize acknowledgement indicator*/
	pthread_mutex_lock (&(ng->lock));
	node = jrb_insert_int (ng->waiting, ng->seqend, new_jval_v(ackentry));
	seqnumbackup = ng->seqend;
	seq = htonl (ng->seqend);
	ng->seqend++;		/* needs to be fixed to modplus */
	pthread_mutex_unlock (&(ng->lock));

    /* create network header */
    ntype = htonl (ack);
    memcpy (s, &ntype, sizeof (unsigned long));
    memcpy (s + sizeof (unsigned long), &seq, sizeof (unsigned long));
    memcpy (s + (2 * sizeof (unsigned long)), data, size);
    size += (2 * sizeof (unsigned long));

    /* send data */
    seq = ntohl (seq);
    if (LOGS)
      log_message (chstate->log, LOG_NETWORKDEBUG,
		   "network_send: sending message seq=%d ack=%d to %s:%d  data:%s\n",
		   seq, ack, host->name, host->port, data);
    start = dtime ();

    ret = sendto (ng->sock, s, size, 0, (struct sockaddr *) &to, sizeof (to));
    if (LOGS)
      log_message (chstate->log, LOG_NETWORKDEBUG,
		   "network_send: sent message: %s\n", s);

    if (ret < 0)
	{
	    if (LOGS)
	      log_message (chstate->log, LOG_ERROR,
			   "network_send: sendto: %s\n", strerror (errno));
	    host_update_stat (host, 0);
	    return (0);
	}

    if (ack == 1)
	{
		// insert a record into the priority queue with the following information:
		// key: starttime + next retransmit time
		// other info: destination host, seq num, data, data size
		PQEntry *pqrecord = get_new_pqentry();
		pqrecord->desthost = host;
		pqrecord->data = data;
		pqrecord->datasize = sizebackup;
		pqrecord->retry = 0;
		pqrecord->seqnum = seqnumbackup;
		pqrecord->transmittime = start;

		pthread_mutex_lock (&(ng->lock));
		priqueue = jrb_insert_dbl (ng->retransmit, (start+RETRANSMIT_INTERVAL), new_jval_v (pqrecord));
		pthread_mutex_unlock (&(ng->lock));

		//kpkp - enable
	    // fprintf(stderr, "network_send: sent seq=%d; inserted entry into the retransmit priqueue with time %f\n", pqrecord->seqnum, start+1);

	    // wait for ack  
		/* // This code is for the semaphore implmentation -- this should be deleted once 
		   // the priority queue is well tested and stabilized

	    if (LOGS)
	      log_message (chstate->log, LOG_NETWORKDEBUG,
			   "network_send: waiting for acknowledgement for seq=%d\n",
			   seq);
	    retval = sema_p (semaphore, TIMEOUT);
	    if (LOGS) {
	      if (retval != 0)
		log_message (chstate->log, LOG_NETWORKDEBUG,
			     "network_send: acknowledgement timer seq=%d TIMEDOUT\n",
			     seq);
	      else
		log_message (chstate->log, LOG_NETWORKDEBUG,
			     "network_send: acknowledgement for seq=%d received\n",
			     seq);
	    }
	    pthread_mutex_lock (&(ng->lock));
	    sema_destroy (semaphore);
	    jrb_delete_node (node);
	    pthread_mutex_unlock (&(ng->lock));

	    if (retval != 0)
		{
		    host_update_stat (host, 0);
		    return (0);
		}

	    /// update latency info 
	    if (host->latency == 0.0)
		{
		    host->latency = dtime () - start;
		}
	    else
		{
		    host->latency =
			(0.9 * host->latency) + (0.1 * (dtime () - start));
		}
		*/
	}

    return (1);
}

/**
 ** Resends a message to host
 */
int network_resend (void *state, ChimeraHost *host, char *data, int size, int ack, unsigned long seqnum, double *transtime)
{
	struct sockaddr_in to;
	int ret, retval;
	char s[SEND_SIZE];
	double start;
    ChimeraState *chstate = (ChimeraState *) state;
    NetworkGlobal *ng = (NetworkGlobal *) chstate->network;

	memset (&to, 0, sizeof (to));
	to.sin_family = AF_INET;
	to.sin_addr.s_addr = host->address;
	to.sin_port = htons ((short) host->port);

	unsigned long seq = htonl (seqnum);

	/* create network header */
	unsigned long ntype = htonl (ack);
	memcpy (s, &ntype, sizeof (unsigned long));
	memcpy (s + sizeof (unsigned long), &seq, sizeof (unsigned long));
	memcpy (s + (2 * sizeof (unsigned long)), data, size);
	size += (2 * sizeof (unsigned long));

	/* send data */
	seq = ntohl (seq);
	if (LOGS)
		log_message (chstate->log, LOG_NETWORKDEBUG,
				"network_resend: resending message seq=%d ack=%d to %s:%d  data:%s\n",
				seq, ack, host->name, host->port, data);

	*transtime = dtime();
	ret = sendto (ng->sock, s, size, 0, (struct sockaddr *) &to, sizeof (to));
	if (LOGS)
		log_message (chstate->log, LOG_NETWORKDEBUG,
				"network_resend: sent message: %s\n", s);
	if (ret < 0)
	{
		if (LOGS)
			log_message (chstate->log, LOG_ERROR,
					"network_resend: sendto: %s\n", strerror (errno));
		host_update_stat (host, 0);
		return (0);
	}

// kpkp
	//fprintf(stderr, "network_resend: resending message seq=%d ack=%d to %s:%d  datalen:%d\n", seq, ack, host->name, host->port, strlen(data));
	return (1);
}

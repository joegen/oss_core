/*
** $Id: message.c,v 1.37 2007/04/04 00:04:49 krishnap Exp $
**
** Matthew Allen
** description: 
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include "chimera.h"
#include "message.h"
#include "job_queue.h"
#include "log.h"
#include "network.h"
#include "jval.h"

/* message on the wire is encoded in the following way:
** [ type ] [ size ] [ key ] [ data ] */

#define HEADER_SIZE (sizeof(unsigned long) + sizeof(unsigned long) + KEY_SIZE/BASE_B + 1)

typedef struct
{
    JRB handlers;
    void *jobq;
    pthread_attr_t attr;
    pthread_mutex_t lock;
} MessageGlobal;

typedef struct
{
    int ack;
    messagehandler_t handler;
    int priority;
    int reply;
} MessageProperty;


/** 
 ** message_create: 
 ** creates the message to the destination #dest# the message format would be like:
 **  [ type ] [ size ] [ key ] [ data ]. It return the created message structure.
 ** 
 */
Message *message_create (Key dest, int type, int size, char *payload)
{

    Message *message;
    message = (Message *) malloc (sizeof (Message));
    key_assign (&message->dest, dest);
    message->type = type;
    message->size = size;
    message->payload = malloc (sizeof (char) * size);
    memcpy (message->payload, payload, size);

    return (message);
}

/** 
 ** message_free:
 ** free the message and the payload
 */
void message_free (Message * msg)
{
    free (msg->payload);
    free (msg);
}

/**
 ** message_init: chstate, port
 ** Initialize messaging subsystem on port and returns the MessageGlobal * which 
 ** contains global state of message subsystem.
 ** message_init also initiate the network subsystem
 */
void *message_init (void *chstate, int port)
{
    pthread_t tid;
    MessageGlobal *mg;
    ChimeraState *state = (ChimeraState *) chstate;

    mg = (MessageGlobal *) malloc (sizeof (MessageGlobal));
    state->message = (void *) mg;

    mg->handlers = make_jrb ();
    state->network = network_init (state->log, port);

    if (state->network == NULL)
	{
	    return (NULL);
	}

    if (pthread_attr_init (&mg->attr) != 0)
	{
	    if (LOGS)
	      log_message (state->log, LOG_ERROR, "pthread_attr_init: %s",
			   strerror (errno));
	    return (NULL);
	}
    if (pthread_attr_setscope (&mg->attr, PTHREAD_SCOPE_SYSTEM) != 0)
	{
	    if (LOGS)
	      log_message (state->log, LOG_ERROR, "pthread_attr_setscope: %s",
			   strerror (errno));
	    return (NULL);
	}
    if (pthread_attr_setdetachstate (&mg->attr, PTHREAD_CREATE_DETACHED) != 0)
	{
	    if (LOGS)
	      log_message (state->log, LOG_ERROR,
			   "pthread_attr_setdetachstate: %s", strerror (errno));
	    return (NULL);
	}

    if (pthread_mutex_init (&mg->lock, NULL) != 0)
	{
	    if (LOGS)
	      log_message (state->log, LOG_ERROR, "pthread_mutex_init: %s",
			   strerror (errno));
	    return (NULL);
	}

    mg->jobq = job_queue_init (10);

    if (pthread_create (&tid, &mg->attr, network_activate, (void *) state) != 0)
	{
	    if (LOGS)
	      log_message (state->log, LOG_ERROR, "pthread_create for network_activate: %s",
			   strerror (errno));
	    return (NULL);
	}

	if (pthread_create (&tid, &mg->attr, retransmit_packets, (void *) state) != 0)
	{
		if (LOGS)
			log_message (state->log, LOG_ERROR, "pthread_create failed for retransmit_packets: %s",
					strerror (errno));
		return (NULL);
	}

    return ((void *) mg);
}

/**
 ** message_receiver:
 ** is called by the message_received func. this function will find the proper 
 ** handler for the message type and passe the handler and its required args to 
 ** the job_queue
 */
void message_receiver (void *chstate, Message * message)
{

    JRB node;
    MessageProperty *msgprop;
    messagehandler_t func;
    ChimeraState *state = (ChimeraState *) chstate;
    MessageGlobal *msgglob = (MessageGlobal *) state->message;

    /* find message handler */
    pthread_mutex_lock (&msgglob->lock);
    node = jrb_find_int (msgglob->handlers, message->type);
    if (node == NULL)
	{
	    if (LOGS)
	      log_message (state->log, LOG_ERROR,
			   "received unrecognized message tpye %d\n",
			   message->type);
	    pthread_mutex_unlock (&msgglob->lock);
	    message_free (message);
	    return;
	}
    msgprop = node->val.v;
    func = msgprop->handler;
    if (func == NULL)
	{
	    if (LOGS)
	      log_message (state->log, LOG_ERROR,
			   "no message handler has registered for type %d\n",
			   message->type);
	    pthread_mutex_unlock (&msgglob->lock);
	    message_free (message);
	    return;
	}
    pthread_mutex_unlock (&msgglob->lock);

    /* call the handler */
    func (chstate, message);
    message_free (message);
}

/** 
 ** message_received:
 ** is called by network_activate and will be passed received data and size from socket
 **
 */
void message_received (void *chstate, char *data, int size)
{

    unsigned long msgtype;
    unsigned long msgsize;
    //  unsigned long msgdest;
    Key msgdest;
    Message *message;
    ChimeraState *state = (ChimeraState *) chstate;
    MessageGlobal *msgglob = (MessageGlobal *) state->message;
    JobArgs *jargs;

    /* message format on the wire 
     * [ type ] [ size ] [ key ] [ data ] 
     */

    jargs = (JobArgs *) malloc (sizeof (JobArgs));


    /* decode message and create Message structure */
    memcpy (&msgtype, data, sizeof (unsigned long));
    msgtype = ntohl (msgtype);
    memcpy (&msgsize, data + sizeof (unsigned long), sizeof (unsigned long));
    msgsize = ntohl (msgsize);

    str_to_key (data + (2 * sizeof (unsigned long)), &msgdest);

    message =
	message_create (msgdest, (int) msgtype, (int) msgsize,
			data + HEADER_SIZE);

    jargs->state = state;
    jargs->msg = message;

    job_submit (msgglob->jobq, message_receiver, (void *) jargs, 0);
}

/**
 ** registers the handler function #func# with the message type #type#,
 ** it also defines the acknowledgment requirement for this type 
 */
void message_handler (void *chstate, int type, messagehandler_t func, int ack)
{

    JRB node;
    ChimeraState *state = (ChimeraState *) chstate;
    MessageGlobal *msgglob = (MessageGlobal *) state->message;
    MessageProperty *msgprop =
	(MessageProperty *) malloc (sizeof (MessageProperty));

    msgprop->handler = func;
    msgprop->ack = ack;

    /* add message handler function into the set of all handlers */
    pthread_mutex_lock (&msgglob->lock);
    node = jrb_find_int (msgglob->handlers, type);
    /* don't allow duplicates */
    if (node != NULL)
	{
	    if (LOGS)
	      log_message (state->log, LOG_WARN,
			   "message handler already registered with %d\n",
			   type);
	    pthread_mutex_unlock (&msgglob->lock);
	    return;
	}
    jrb_insert_int (msgglob->handlers, type, new_jval_v (msgprop));

    pthread_mutex_unlock (&msgglob->lock);
}

/**
 ** message_send:
 ** send the message to destination #host# the retry arg indicates to the network
 ** layer if this message should be ackd or not
 */
int message_send (void *chstate, ChimeraHost * host, Message * message,
		  Bool retry)
{
	char *data;
	unsigned long size, type, ack;
	int i, ret = 0;
	ChimeraState *state = (ChimeraState *) chstate;
	MessageGlobal *msgglob = (MessageGlobal *) state->message;
	JRB node;
	MessageProperty *msgprop;

	/* message format on the wire 
	 * [ type ] [ size ] [ key ] [ data ] 
	 */

	if (host == NULL)
		return (0);

	size = HEADER_SIZE + message->size;
	data = (char *) malloc (sizeof (char) * size);

	/* encode the message */
	type = htonl ((unsigned long) message->type);
	memcpy (data, &type, sizeof (unsigned long));
	size = htonl ((unsigned long) message->size);
	memcpy (data + sizeof (unsigned long), &size, sizeof (unsigned long));
	memcpy (data + (2 * sizeof (unsigned long)),
			get_key_string (&message->dest),
			strlen (get_key_string (&message->dest)));
	memcpy (data + HEADER_SIZE, message->payload, message->size);
	size = HEADER_SIZE + message->size;	/*reset due to htonl */

	/* get the message properties */
	pthread_mutex_lock (&msgglob->lock);
	node = jrb_find_int (msgglob->handlers, message->type);
	if (node == NULL)
	{
		if (LOGS)
			log_message (state->log, LOG_ERROR,
					"fail to send unrecognized message tpye %d\n",
					message->type);
		pthread_mutex_unlock (&msgglob->lock);
		return 0;
	}
	msgprop = node->val.v;
	pthread_mutex_unlock (&msgglob->lock);

	/* send the message */
	if (!retry)
		ret = network_send (state, host, data, size, msgprop->ack);
	else
		ret = network_send (state, host, data, size, msgprop->ack);
	return (ret);
}

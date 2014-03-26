/*
** $Id: chat.c,v 1.5 2006/06/07 09:21:29 krishnap Exp $
**
** Matthew Allen
** description: 
*/
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "message.h"

#define JOIN_MESSAGE  1
#define CHAT_MESSAGE  2
#define SHOUT_MESSAGE 3

Host *them = NULL;

void handle_join (Message * message)
{

    int i;

    for (i = 0; i < message->size; i++)
	{
	    if (message->payload[i] == ':')
		break;
	}
    message->payload[i] = 0;
    i++;
    them = message_makehost (message->payload, atoi (message->payload + i));
    printf ("%s:%d joined\n", them->name, them->port);

}

void handle_chat (Message * message)
{

    printf ("%s", message->payload);

}

void handle_shout (Message * message)
{

    int i;

    for (i = 0; i < message->size; i++)
	{
	    if (message->payload[i] >= 'a' && message->payload[i] <= 'z')
		message->payload[i] += ('A' - 'a');
	}
    printf ("%s", message->payload);

}

int main (int argc, char **argv)
{

    char tmp[256];
    pthread_t tid;
    Message *message;

    if (argc != 2 && argc != 4)
	{
	    fprintf (stderr, "what the hell are you doing?\n");
	    fprintf (stderr, "usage: chat port [ partner port ]\n");
	    exit (1);
	}

    message_init (atoi (argv[1]));
    message_handler (JOIN_MESSAGE, handle_join);
    message_handler (CHAT_MESSAGE, handle_chat);
    message_handler (SHOUT_MESSAGE, handle_shout);

    if (argc == 4)
	{			/* joining host */
	    them = message_makehost (argv[2], atoi (argv[3]));
	    gethostname (tmp, 256);
	    sprintf (tmp + strlen (tmp), ":%s", argv[1]);
	    message = message_create (JOIN_MESSAGE, strlen (tmp) + 1, tmp);
	    message_send (them, message);
	}

    while (fgets (tmp, 256, stdin) != NULL)
	{
	    tmp[1] = 0;
	    if (them == NULL)
		{
		    printf ("no one else is conncted!\n");
		    continue;
		}
	    message = message_create (0, strlen (tmp + 2) + 1, tmp + 2);
	    if (strcmp (tmp, "C") == 0)
		{
		    message->type = CHAT_MESSAGE;
		    message_send (them, message);
		}
	    else if (strcmp (tmp, "S") == 0)
		{
		    message->type = SHOUT_MESSAGE;
		    message_send (them, message);
		}
	    else
		{
		    printf ("I have no idea what '%s' means.\n", tmp);
		}
	    free (message);
	}

}

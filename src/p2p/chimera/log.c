/*
** $Id: log.c,v 1.15 2006/09/05 06:09:35 krishnap Exp $
**
** Matthew Allen
** description: 
*/

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include "log.h"

extern FILE *stdin;


void *log_init ()
{
    static FILE **log_fp;
    log_fp = (FILE **) malloc (sizeof (FILE *) * LOG_COUNT);
    memset (log_fp, 0, sizeof (FILE *) * LOG_COUNT);
    return ((void *) log_fp);
}

void log_message (void *logs, int type, char *format, ...)
{
    va_list ap;
    FILE **log_fp = (FILE **) logs;

    if (log_fp !=NULL && log_fp[type] != NULL)
	{
	    va_start (ap, format);
	    vfprintf (log_fp[type], format, ap);
	    fflush (log_fp[type]);	// this is needed to get the contents logged
	}
}

void log_direct (void *logs, int type, FILE * fp)
{
    FILE **log_fp = (FILE **) logs;

    if (fp == NULL)
	{
	    fprintf (stderr,
		     "The file pointer given to log_direct is NULL; No messges would be printed to the file \n");
	}

    log_fp[type] = fp;
}

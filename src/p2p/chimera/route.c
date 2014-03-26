#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include "route.h"
#include "host.h"
#include "jrb.h"
#include "jval.h"
#include "log.h"

void leafset_update (ChimeraState * state, ChimeraHost * host, int joined,
		     ChimeraHost ** deleted, ChimeraHost ** added);
int leafset_size (ChimeraHost ** arr);
void leafset_range_update (RouteGlobal * routglob, Key * rrange,
			   Key * lrange);
void leafset_insert (ChimeraState * state, ChimeraHost * host,
		     int right_or_left, ChimeraHost ** deleted,
		     ChimeraHost ** added);
void leafset_delete (ChimeraState * state, ChimeraHost * host,
		     int right_or_left, ChimeraHost ** deleted);
void leafset_print (ChimeraState * state);
void sort_hosts (void *logs, ChimeraHost ** hosts, Key key, int size);
void sort_hosts_key (void *logs, ChimeraHost ** hosts, Key key, int size);
ChimeraHost *find_closest_key (void *logs, ChimeraHost ** hosts, Key key,
			       int size);
int power (int base, int n);
int hexalpha_to_int (int c);


void route_keyupdate (void *routeglob, ChimeraHost * me)
{

    RouteGlobal *rg = (RouteGlobal *) routeglob;
    rg->me = me;
    sprintf (rg->keystr, "%s", get_key_string (&rg->me->key));
}


/** route_init:
** Ininitiates routing table and leafsets 
*/
void *route_init (ChimeraHost * me)
{

    int i, j, k;
    RouteGlobal *rg;
    rg = (RouteGlobal *) malloc (sizeof (RouteGlobal));

    /* allocate memory for routing table */
    rg->table =
	(ChimeraHost ****) malloc (sizeof (struct ChimeraHost ***) * MAX_ROW);
    for (i = 0; i < MAX_ROW; i++)
	{
	    rg->table[i] =
		(ChimeraHost ***) malloc (sizeof (ChimeraHost **) * MAX_COL);
	    for (j = 0; j < MAX_COL; j++)
		{
		    rg->table[i][j] =
			(ChimeraHost **) malloc (sizeof (ChimeraHost *) *
						 MAX_ENTRY);
		    for (k = 0; k < MAX_ENTRY; k++)
			rg->table[i][j][k] = NULL;
		}
	}

    rg->keystr = (char *) malloc (sizeof (char) * (MAX_ROW + 1));
    route_keyupdate ((void *) rg, me);

    key_assign (&(rg->Rrange), me->key);
    key_assign (&(rg->Lrange), me->key);

    /* allocate memory for leafsets */
    rg->leftleafset =
	(ChimeraHost **) malloc (sizeof (ChimeraHost *) *
				 ((LEAFSET_SIZE / 2) + 1));
    rg->rightleafset =
	(ChimeraHost **) malloc (sizeof (ChimeraHost *) *
				 ((LEAFSET_SIZE / 2) + 1));
    for (i = 0; i < (LEAFSET_SIZE / 2) + 1; i++)
	{
	    rg->leftleafset[i] = NULL;
	    rg->rightleafset[i] = NULL;
	}

    pthread_mutex_init (&rg->lock, NULL);

    return ((void *) rg);
}

/** route_get_table: 
 ** return the entire routing table
 */
ChimeraHost **route_get_table (ChimeraState * state)
{
    ChimeraHost **ret;
    int i, j, l, k, count;
    RouteGlobal *routeglob = (RouteGlobal *) state->route;

    pthread_mutex_lock (&routeglob->lock);

    count = 0;

    for (i = 0; i < MAX_ROW; i++)
	for (j = 0; j < MAX_COL; j++)
	    for (l = 0; l < MAX_ENTRY; l++)
		if (routeglob->table[i][j][l] != NULL)
		    count++;
    ret = (ChimeraHost **) malloc (sizeof (ChimeraHost *) * (count + 1));
    k = 0;
    for (i = 0; i < MAX_ROW; i++)
	for (j = 0; j < MAX_COL; j++)
	    for (l = 0; l < MAX_ENTRY; l++)
		if (routeglob->table[i][j][l] != NULL)
		    {
			ret[k++] =
			    host_get (state, routeglob->table[i][j][l]->name,
				      routeglob->table[i][j][l]->port);
		    }
    ret[k] = NULL;
    pthread_mutex_unlock (&routeglob->lock);
    return ret;

}

/** route_row_lookup:key 
 ** return the row in the routing table that matches the longest prefix with #key# 
 */
ChimeraHost **route_row_lookup (ChimeraState * state, Key key)
{
    ChimeraHost **ret;
    int i, j, k, l, count;
    RouteGlobal *routeglob = (RouteGlobal *) state->route;

    pthread_mutex_lock (&routeglob->lock);


    i = key_index (state->log, routeglob->me->key, key);

    /* find out the number of hosts exists in the matching row */
    count = 0;
    for (j = 0; j < MAX_COL; j++)
	for (l = 0; l < MAX_ENTRY; l++)
	    if (routeglob->table[i][j][l] != NULL)
		count++;

    ret = (ChimeraHost **) malloc (sizeof (ChimeraHost *) * (count + 2));
    k = 0;
    for (j = 0; j < MAX_COL; j++)
	for (l = 0; l < MAX_ENTRY; l++)
	    if (routeglob->table[i][j][l] != NULL)
		ret[k++] =
		    host_get (state, routeglob->table[i][j][l]->name,
			      routeglob->table[i][j][l]->port);

    ret[k++] = host_get (state, routeglob->me->name, routeglob->me->port);
    ret[k] = NULL;

    pthread_mutex_unlock (&routeglob->lock);

    return ret;
}

/** route_lookup:
 ** returns an array of #count# nodes that are acceptable next hops for a
 ** message being routed to #key#. #is_save# is ignored for now.
 */
ChimeraHost **route_lookup (ChimeraState * state, Key key, int count,
			    int is_safe)
{
    int i, j, k, Lsize, Rsize;
    int index = 0, match_col = 0, next_hop = 0, size = 0;
    ChimeraHost *leaf, *tmp, *min;
    Key dif1, dif2;
    ChimeraHost **ret, **hosts;
    RouteGlobal *routeglob = (RouteGlobal *) state->route;

    pthread_mutex_lock (&routeglob->lock);
    if (LOGS) {
      log_message(state->log, LOG_ROUTING, "%s: lookup for key %s!\n",
		  get_key_string (&routeglob->me->key), get_key_string (&key));
    }

    /*calculate the leafset and table size */
    Lsize = leafset_size (routeglob->leftleafset);
    Rsize = leafset_size (routeglob->rightleafset);

    /* if the key is in the leafset range route through leafset */
    /* the additional 2 ChimeraHosts pointed by the #hosts# are to consider the node itself and NULL at the end */
    if (count == 1
	&& key_between (state->log, &key, &routeglob->Lrange,
			&routeglob->Rrange))
	{
	  if (LOGS) {
	    log_message (state->log, LOG_ROUTING,
			 "Routing through leafset\n");
	  }
	    hosts =
		(ChimeraHost **) malloc (sizeof (ChimeraHost *) *
					 (LEAFSET_SIZE + 2));
	    memset ((void *) hosts, 0,
		    (sizeof (ChimeraHost *) * (LEAFSET_SIZE + 2)));
	    ret =
		(ChimeraHost **) malloc (sizeof (ChimeraHost *) *
					 (count + 1));
	    memset ((void *) ret, 0, (sizeof (ChimeraHost *) * (count + 1)));

	    hosts[index++] = routeglob->me;
	  if (LOGS) {
	    log_message (state->log, LOG_ROUTING, "ME: (%s, %d, %s) \n",
			 routeglob->me->name, routeglob->me->port,
			 get_key_string (&routeglob->me->key));
	  }
	    /* look left */
	    for (i = 0; i < Lsize; i++)
		{
		    leaf = routeglob->leftleafset[i];
		    if (LOGS) {
		      log_message (state->log, LOG_ROUTING,
				   "Left_leafset[%d]: (%s, %d, %s) \n", i,
				   leaf->name, leaf->port,
				   get_key_string (&leaf->key));
		    }
		    hosts[index++] = leaf;
		}

	    /* look right */
	    for (i = 0; i < Rsize; i++)
		{
		    leaf = routeglob->rightleafset[i];
		    if (LOGS) {
		      log_message (state->log, LOG_ROUTING,
				   "Right_leafset[%d]: (%s, %d, %s) \n", i,
				   leaf->name, leaf->port,
				   get_key_string (&leaf->key));
		    }
		    hosts[index++] = leaf;
		}
	    hosts[index] = NULL;

	    min = find_closest_key (state, hosts, key, index);

	    ret[0] = min;
	    ret[1] = NULL;
	    if (LOGS) 
	      log_message (state->log, LOG_ROUTING, "++NEXT_HOP = %s\n",
			   get_key_string (&ret[0]->key));
	    free (hosts);
	    pthread_mutex_unlock (&routeglob->lock);
	    return (ret);
	}

    /* check to see if there is a matching next hop (for fast routing) */
    i = key_index (state->log, routeglob->me->key, key);
    match_col = hexalpha_to_int (get_key_string (&key)[i]);

    for (k = 0; k < MAX_ENTRY; k++)
	if (routeglob->table[i][match_col][k] != NULL
	    && routeglob->table[i][match_col][k]->success_avg > BAD_LINK)
	    {
		next_hop = 1;
		tmp = routeglob->table[i][match_col][k];
		break;
	    }

    if (next_hop == 1 && count == 1)
	{
	    for (k = 0; k < MAX_ENTRY; k++)
		{
		    if (routeglob->table[i][match_col][k] != NULL)
			if ((routeglob->table[i][match_col][k]->success_avg >
			     tmp->success_avg)
			    || (routeglob->table[i][match_col][k]->
				success_avg == tmp->success_avg
				&& routeglob->table[i][match_col][k]->
				latency < tmp->latency))
			    tmp = routeglob->table[i][match_col][k];
		}
	    ret =
		(ChimeraHost **) malloc (sizeof (ChimeraHost *) *
					 (count + 1));
	    ret[0] = host_get (state, tmp->name, tmp->port);
	    ret[1] = NULL;
	    if (LOGS)
	      log_message (state->log, LOG_ROUTING,
			   "Routing through Table(%s), NEXT_HOP=%s\n",
			   get_key_string (&routeglob->me->key),
			   get_key_string (&ret[0]->key));
	    pthread_mutex_unlock (&routeglob->lock);
	    return (ret);
	}

    /* if there is no matching next hop we have to find the best next hop */
    /* brute force method to solve count requirements */

    hosts =
	(ChimeraHost **) malloc (sizeof (ChimeraHost *) *
				 (LEAFSET_SIZE + 1 + (MAX_COL * MAX_ENTRY)));
    memset ((void *) hosts, 0,
	    (sizeof (ChimeraHost *) *
	     (LEAFSET_SIZE + 1 + (MAX_COL * MAX_ENTRY))));
    if (LOGS)
      log_message (state->log, LOG_ROUTING,
		   "Routing to next closest key I know of: \n");
    leaf = routeglob->me;
    if (LOGS)
      log_message (state->log, LOG_ROUTING, "+me: (%s, %d, %s)\n", leaf->name,
		   leaf->port, get_key_string (&leaf->key));
    hosts[index++] = routeglob->me;

    /* look left */
    for (i = 0; i < Lsize; i++)
	{
	    leaf = routeglob->leftleafset[i];
	    if (LOGS)
	      log_message (state->log, LOG_ROUTING,
			   "+Left_leafset[%d]: (%s, %d, %s) \n", i, leaf->name,
			   leaf->port, get_key_string (&leaf->key));
	    hosts[index++] = leaf;
	}

    /* look right */
    for (i = 0; i < Rsize; i++)
	{
	    leaf = routeglob->rightleafset[i];
	    if (LOGS)
	      log_message (state->log, LOG_ROUTING,
			   "+Right_leafset[%d]: (%s, %d, %s)\n", i, leaf->name,
			   leaf->port, get_key_string (&leaf->key));
	    hosts[index++] = leaf;
	}

    /* find the longest prefix match */
    i = key_index (state->log, routeglob->me->key, key);

    for (j = 0; j < MAX_COL; j++)
	for (k = 0; k < MAX_ENTRY; k++)
	    if (routeglob->table[i][j][k] != NULL
		&& routeglob->table[i][j][k]->success_avg > BAD_LINK)
		{
		    leaf = routeglob->table[i][j][k];
		    if (LOGS)
		      log_message (state->log, LOG_ROUTING,
				   "+Table[%d][%d][%d]: (%s, %d, %s)\n", i, j,
				   k, leaf->name, leaf->port,
				   get_key_string (&leaf->key));
		    hosts[index++] = leaf;
		}

    hosts[index] = NULL;

    ret = (ChimeraHost **) malloc (sizeof (ChimeraHost *) * (count + 1));

	if (count == 1)
	{
		ret[0] = find_closest_key (state, hosts, key, index);
		ret[1] = NULL;
	}
	else
	{
		sort_hosts (state->log, hosts, key, index);
		/* find the best #count# entries that we looked at... could be much better */
		for (i = 0, j = 0; hosts[i] != NULL && (i - j) < count; i++)
		{
			tmp = hosts[i];

			if ((i - j) > 0 && key_equal (ret[(i - j) - 1]->key, tmp->key))
			{
				j++;
				continue;
			}
			if (LOGS)
			  log_message (state->log, LOG_ROUTING,
				       "++Result[%d]: (%s, %d, %s)\n", i, tmp->name,
				       tmp->port, get_key_string (&tmp->key));
			ret[i - j] = host_get (state, tmp->name, tmp->port);
		}
		ret[i - j] = NULL;
	}

    /*  to prevent bouncig */
    if (count == 1)
	{
	    key_distance (state->log, &dif1, &key, &ret[0]->key);
	    key_distance (state->log, &dif2, &key, &routeglob->me->key);
	    if (key_equal (dif1, dif2))
		ret[0] = routeglob->me;
	}
    free (hosts);
    pthread_mutex_unlock (&routeglob->lock);

    return (ret);
}

/** sort_hosts_key:
 ** Sorts #hosts# based on their key distance from #Key#, closest node first
 */
void sort_hosts_key (void *logs, ChimeraHost ** hosts, Key key, int size)
{
    int i, j;
    ChimeraHost *tmp;
    Key dif1;
    Key dif2;
    for (i = 0; i < size; i++)
	{
	    for (j = i + 1; j < size; j++)
		{
		    if (hosts[i] != NULL && hosts[j] != NULL)
			{
			    key_distance (logs, &dif1, &hosts[i]->key, &key);
			    key_distance (logs, &dif2, &hosts[j]->key, &key);
			    if (key_comp (&dif2, &dif1) < 0)
				{
				    tmp = hosts[i];
				    hosts[i] = hosts[j];
				    hosts[j] = tmp;
				}
			}
		}
	}
}

/** find_closest_key:
 ** finds the closest node in the array of #hosts# to #key# and put that in min.
 */
ChimeraHost *find_closest_key (void *state, ChimeraHost ** hosts, Key key,
			       int size)
{

    int i, j;
    Key dif;
    Key mindif;
    ChimeraHost *min, *tmp;
    ChimeraState *chs = (ChimeraState *) state;

    if (size == 0)
	{
	    min = NULL;
	    return;
	}

    else
	{
	    min = hosts[0];
	    key_distance (chs->log, &mindif, &hosts[0]->key, &key);
	}

    for (i = 0; i < size; i++)
	{

	    if (hosts[i] != NULL)
		{
		    key_distance (chs->log, &dif, &hosts[i]->key, &key);

		    if (key_comp (&dif, &mindif) < 0)
			{
			    min = hosts[i];
			    key_assign (&mindif, dif);
			}
		}
	}
    tmp = host_get (chs, min->name, min->port);
    return (min);
}

/** sort_hosts:
 ** Sorts #hosts# based on common prefix match and key distance from #Key#
 */
void sort_hosts (void *logs, ChimeraHost ** hosts, Key key, int size)
{
    int i, j;
    ChimeraHost *tmp;
    Key dif1;
    Key dif2;
    int pmatch1 = 0;
    int pmatch2 = 0;


    for (i = 0; i < size; i++)
	{
	    for (j = i + 1; j < size; j++)
		{
		    if (hosts[i] != NULL && hosts[j] != NULL)
			{
			    pmatch1 = key_index (logs, key, hosts[i]->key);
			    pmatch2 = key_index (logs, key, hosts[j]->key);
			    if (pmatch2 > pmatch1)
				{
				    tmp = hosts[i];
				    hosts[i] = hosts[j];
				    hosts[j] = tmp;
				}
			    else if (pmatch1 == pmatch2)
				{

				    key_distance (logs, &dif1, &hosts[i]->key,
						  &key);
				    key_distance (logs, &dif2, &hosts[j]->key,
						  &key);
				    if (key_comp (&dif2, &dif1) < 0)
					{
					    tmp = hosts[i];
					    hosts[i] = hosts[j];
					    hosts[j] = tmp;
					}
				}
			}

		}
	}
}

int leafset_size (ChimeraHost ** arr)
{
    int i = 0;
    for (i = 0; arr[i] != NULL; i++);
    return i;
}


/**
 ** leafset_range_update:
 ** updates the leafset range whenever a node leaves or joins to the leafset
 **
 */
void leafset_range_update (RouteGlobal * routeglob, Key * rrange,
			   Key * lrange)
{
    int i, j;

    /* right range */
    for (i = 0; routeglob->rightleafset[i] != NULL; i++)
	key_assign (rrange, routeglob->rightleafset[i]->key);
    if (i == 0)
	key_assign (lrange, routeglob->me->key);

    /* left range */
    if (routeglob->leftleafset[0] != NULL)
	key_assign (lrange, routeglob->leftleafset[0]->key);
    else
	key_assign (lrange, routeglob->me->key);
}


/**
 ** leafset_update:
 ** this function is called whenever a route_update is called the joined
 ** is 1 if the node has joined and 0 if a node is leaving. 
 ** 
 */
void leafset_update (ChimeraState * state, ChimeraHost * host, int joined,
		     ChimeraHost ** deleted, ChimeraHost ** added)
{

    int Lsize = 0;
    int Rsize = 0;
    Key midpoint;
    RouteGlobal *routeglob = (RouteGlobal *) state->route;

    Lsize = leafset_size (routeglob->leftleafset);
    Rsize = leafset_size (routeglob->rightleafset);

    key_midpoint (state->log, &midpoint, routeglob->me->key);

    if (joined)
	{
		/* key falls in the right side of node */
		if (Rsize < LEAFSET_SIZE / 2
				|| key_between (state->log, &host->key,
					&routeglob->me->key,
					&routeglob->rightleafset[Rsize -
					1]->key))
		{
			/* insert in Right leafset */
			leafset_insert (state, host, 1, deleted, added);
		}
		/* key falls in the left side of the node */
		if (Lsize < LEAFSET_SIZE / 2
				|| key_between (state->log, &host->key,
					&routeglob->leftleafset[Lsize - 1]->key,
					&routeglob->me->key))
		{
			/* insert in Left leafset */
			leafset_insert (state, host, 0, deleted, added);
		}
	}
    else
	{
	    if (key_between
		(state->log, &host->key, &routeglob->me->key, &midpoint))
		{
		    leafset_delete (state, host, 1, deleted);
		}
	    else
		leafset_delete (state, host, 0, deleted);
	}

}

extern void chimera_update_upcall (ChimeraState * state, Key * k,
				   ChimeraHost * h, int joined);

/** 
 ** leafset_delete:
 ** removes the #deleted# node from leafset
 **
 */
void leafset_delete (ChimeraState * state, ChimeraHost * host,
		     int right_or_left, ChimeraHost ** deleted)
{
    int i = 0, size;
    int match = 0;
    ChimeraHost **p;
    RouteGlobal *routeglob = (RouteGlobal *) state->route;

    /*insert in right leafset */
    if (right_or_left == 1)
	{
	    size = leafset_size (routeglob->rightleafset);
	    p = routeglob->rightleafset;
	}
    /*insert in left leafset */
    else
	{
	    size = leafset_size (routeglob->leftleafset);
	    p = routeglob->leftleafset;
	}


    for (i = 0; i < size && !(key_equal (p[i]->key, host->key)); i++);
    if (i < size)
	{
	    *deleted = p[i];
	    match = 1;
	}

    /* shift leafset members to not have a hole in the leafset */
    if (match)
	{
	    do
		{
		    p[i] = p[i + 1];
		    i++;
		}
	    while (i < size - 1);
	    p[i] = NULL;
	}


}

/**
 **leafset_insert:
 ** inserts the added node tot the leafset and removes the deleted from the leafset
 ** the deleted node is NULL if the new added node will not cause a node to leave the leafset.
 */
void leafset_insert (ChimeraState * state, ChimeraHost * host,
		     int right_or_left, ChimeraHost ** deleted,
		     ChimeraHost ** added)
{

    int i = 0, size;
    ChimeraHost **p;
    ChimeraHost *tmp1, *tmp2;
    ChimeraHost *input = host;
    Key dif1, dif2;
    RouteGlobal *routeglob = (RouteGlobal *) state->route;

    /*inert in right leafset */
    if (right_or_left == 1)
	{
	    size = leafset_size (routeglob->rightleafset);
	    p = routeglob->rightleafset;
	}
    /*insert in left leafset */
    else
	{
	    size = leafset_size (routeglob->leftleafset);
	    p = routeglob->leftleafset;
	}

    if (size == 0)
	{
	    p[0] = input;
	    *added = input;
	}

    else
	{
		// to avoid duplicate entries in the same leafset
		if (key_equal (p[i]->key, input->key))
		{
			host_release (state, input);
			return;
		}

		int foundKeyPos = 0;
		if (right_or_left == 1)
		{
			foundKeyPos = key_between(state->log, &host->key, &routeglob->me->key, &p[i]->key);
		}
		else
		{
			foundKeyPos = key_between(state->log, &host->key, &p[i]->key, &routeglob->me->key);
		}

		while ((i < size) && !foundKeyPos)
		{
		    if (key_equal (p[i]->key, input->key))
			{
			    host_release (state, input);
			    return;
			}
		    i++;
		    if (i < size)
			{
				if (right_or_left == 1)
				{
					foundKeyPos = key_between(state->log, &host->key, &routeglob->me->key, &p[i]->key);
				}
				else
				{
					foundKeyPos = key_between(state->log, &host->key, &p[i]->key, &routeglob->me->key);
				}
			}
		}

	    tmp1 = input;
	    *added = input;

	    while (i < LEAFSET_SIZE / 2)
		{
		    tmp2 = p[i];
		    p[i++] = tmp1;
		    tmp1 = tmp2;
		}

	    /* there is leftover */
	    if (tmp2 != NULL && size == LEAFSET_SIZE / 2)
		*deleted = tmp2;
	}
}

/** route_neighbors: 
** returns an array of #count# neighbor nodes with priority to closer nodes 
*/
ChimeraHost **route_neighbors (ChimeraState * state, int count)
{
    int i = 0, j = 0, Rsize = 0, Lsize = 0, index = 0;
    int ret_size;
    ChimeraHost *tmp;
    ChimeraHost **hosts =
	(ChimeraHost **) malloc (sizeof (ChimeraHost *) * (LEAFSET_SIZE + 1));
    ChimeraHost **ret =
	(ChimeraHost **) malloc (sizeof (ChimeraHost *) * (count + 1));
    RouteGlobal *routeglob = (RouteGlobal *) state->route;

    pthread_mutex_lock (&routeglob->lock);

    Lsize = leafset_size (routeglob->leftleafset);
    Rsize = leafset_size (routeglob->rightleafset);

    if (count > Rsize + Lsize)
	{
	    ret_size = Rsize + Lsize;
	}
    else
	ret_size = count;

    /* creat a jrb of leafset pointers sorted on distance */
    for (i = 0; i < Lsize; i++)
	{
	    tmp = routeglob->leftleafset[i];
	    hosts[index++] = tmp;
	}

    for (i = 0; i < Rsize; i++)
	{
	    tmp = routeglob->rightleafset[i];
	    hosts[index++] = tmp;
	}

    hosts[index] = NULL;
    /* sort aux */
    sort_hosts (state->log, hosts, routeglob->me->key, index);

    for (i = 0; i < ret_size; i++)
	{
	    tmp = hosts[i];
	    ret[i] = host_get (state, tmp->name, tmp->port);
	}

    ret[i] = NULL;

    free (hosts);
    pthread_mutex_unlock (&routeglob->lock);

    return ret;
}


/** route_update:
** updated the routing table in regard to #host#. If the host is joining
** the network (and #joined# == 1), then it is added to the routing table
** if it is appropriate. If it is leaving the network (and #joined# == 0),
** then it is removed from the routing tables 
*/

void route_update (ChimeraState * state, ChimeraHost * host, int joined)
{
    int i, j, k, found, pick;
    ChimeraHost *tmp, *deleted = NULL, *added = NULL;
    RouteGlobal *routeglob = (RouteGlobal *) state->route;


    pthread_mutex_lock (&routeglob->lock);

    if (key_equal (routeglob->me->key, host->key))
	{
	    pthread_mutex_unlock (&routeglob->lock);
	    return;
	}

    i = key_index (state->log, routeglob->me->key, host->key);
    j = hexalpha_to_int (get_key_string (&host->key)[i]);


    /*join */
    if (joined)
	{
	    found = 0;
	    for (k = 0; k < MAX_ENTRY; k++)
		{
		    if (routeglob->table[i][j][k] == NULL)
			{
			    routeglob->table[i][j][k] =
				host_get (state, host->name, host->port);
			    leafset_update (state, host, joined, &deleted,
					    &added);
			    found = 1;
			    break;
			}
		    else if (routeglob->table[i][j][k] != NULL
			     && key_equal (routeglob->table[i][j][k]->key,
					   host->key))
			{
			    pthread_mutex_unlock (&routeglob->lock);
			    return;

			}
		}

	    /* the entry array is full we have to get rid of one */
	    /* replace the new node with the node with the highest latncy in the entry array */
	    if (!found)
		{
		    pick = 0;
		    for (k = 1; k < MAX_ENTRY; k++)
			{
			    if (routeglob->table[i][j][pick]->success_avg >
				routeglob->table[i][j][k]->success_avg)
				pick = k;
			}
		    host_release (state, routeglob->table[i][j][pick]);
		    routeglob->table[i][j][pick] =
			host_get (state, host->name, host->port);
		    leafset_update (state, host, joined, &deleted, &added);
		}
	}

    /*delete */
    else
	{
	    for (k = 0; k < MAX_ENTRY; k++)
		if (routeglob->table[i][j][k] != NULL
		    && key_equal (routeglob->table[i][j][k]->key, host->key))
		    {
			host_release (state, routeglob->table[i][j][k]);
			routeglob->table[i][j][k] = NULL;
			break;
		    }

	    leafset_update (state, host, joined, &deleted, &added);

	}

    if (deleted != NULL)
	{
	    leafset_range_update (routeglob, &(routeglob->Rrange),
				  &(routeglob->Lrange));
	    chimera_update_upcall (state, &(deleted->key), deleted, 0);
	}
    if (added != NULL)
	{
	    leafset_range_update (routeglob, &(routeglob->Rrange),
				  &(routeglob->Lrange));
	    chimera_update_upcall (state, &(added->key), added, 1);
	}
    pthread_mutex_unlock (&routeglob->lock);

    fflush (stderr);
}

void gethostinfo (ChimeraHost * CHost)
{

    char tmp[64];
    CHost->name = (char *) malloc (sizeof (char) * 64);
    printf ("Enter key: ");
    scanf ("%s", tmp);
    str_to_key (tmp, &(CHost->key));
    printf ("Key :");
    key_print (CHost->key);
}

int hexalpha_to_int (int c)
{
    char hexalpha[] = "0123456789abcdef";
    int i;
    int answer = 0;

    for (i = 0; answer == 0 && hexalpha[i] != '\0'; i++)
	{
	    if (hexalpha[i] == c)
		{
		    answer = i;
		}
	}

    return answer;
}

int power (int base, int n)
{
    int i, p = 1;

    for (i = 1; i <= n; i++)
	p = p * base;
    return p;
}



void leafset_print (ChimeraState * state)
{
    int i;
    int Lsize, Rsize;
    RouteGlobal *routeglob = (RouteGlobal *) state->route;

    Lsize = leafset_size (routeglob->leftleafset);
    Rsize = leafset_size (routeglob->rightleafset);

    fprintf (stderr, "LEFT: ");
    for (i = 0; i < Lsize; i++)
	fprintf (stderr, "%s ",
		 get_key_string (&routeglob->leftleafset[i]->key));
    fprintf (stderr, "\nRIGHT: ");
    for (i = 0; i < Rsize; i++)
	fprintf (stderr, "%s ",
		 get_key_string (&routeglob->rightleafset[i]->key));
    fprintf (stderr, "\n");
}

void printTable (ChimeraState * state)
{

    int i, j, k;
    RouteGlobal *routeglob = (RouteGlobal *) state->route;

    /* print the table */

    fprintf (stderr,
	     "------------------------------- TABLE-------------------------------\n");
    for (i = 0; i < MAX_ROW; i++)
	{
	    for (j = 0; j < MAX_COL; j++)
		{
		    for (k = 0; k < MAX_ENTRY; k++)
			if (routeglob->table[i][j][k] != NULL)
			    fprintf (stderr, "%s ",
				     get_key_string (&routeglob->
						     table[i][j][k]->key));
			else
			    fprintf (stderr,
				     "00000000 00000000 00000000 00000000 00000000");
		}
	    fprintf (stderr, "\n");
	}
    fprintf (stderr,
	     "----------------------------------------------------------------------\n");
}

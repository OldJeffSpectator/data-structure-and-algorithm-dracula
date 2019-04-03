////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// map.c: an implementation of a Map type
// You can change this as much as you want!
//
// 2017-11-30	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include "map.h"
#include "places.h"


static void add_connections_normal (map *);
static void add_connections_rail (map *g);
static void add_connection (map *, location_t, location_t, transport_t,int);
static inline bool is_sentinel_edge (connection);

static map_adj *adjlist_insert (map_adj *, location_t, transport_t,int);
static bool adjlist_contains (map_adj *, location_t, transport_t,int);


// Create a new empty graph (for a map)
// #Vertices always same as NUM_PLACES
map *map_new (void)
{
	map *g = malloc (sizeof *g);
	if (g == NULL) err (EX_OSERR, "couldn't allocate Map");

	(*g) = (map) {
		.n_vertices = NUM_MAP_LOCATIONS,
		.n_edges = 0,
		.connections = { NULL }
	};

	add_connections_normal (g);
	add_connections_rail(g);
	return g;
}

// Remove an existing graph
void map_drop (map *g)
{
	assert (g != NULL);

	for (size_t i = 0; i < g->n_vertices; i++) {
		map_adj *curr = g->connections[i];
		while (curr != NULL) {
			map_adj *next = curr->next;
			free (curr);
			curr = next;
		}
	}
	free (g);
}

// Display content of Map/Graph
void map_show (map *g)
{
	assert (g != NULL);

	printf ("V=%zu, E=%zu\n", g->n_vertices, g->n_edges);
	for (size_t i = 0; i < g->n_vertices; i++)
		for (map_adj *curr = g->connections[i];
			 curr != NULL; curr = curr->next){
				if(curr->rail_distance <2)continue;
				printf (
				"%s connects to %s by %s with rail distance %d\n",
				location_get_name ((location_t) i),
				location_get_name (curr->v),
				transport_to_s (curr->type),curr->rail_distance
				);
			 }
			 
}

// Return count of nodes
size_t map_nv (map *g)
{
	assert (g != NULL);
	return g->n_vertices;
}

// Return count of edges of a particular type
size_t map_ne (map *g, transport_t type)
{
	assert (g != NULL);
	assert (valid_transport_p (type) || type == ANY);

	size_t n_edges = 0;

	for (size_t i = 0; i < g->n_vertices; i++)
		for (map_adj *curr = g->connections[i];
			 curr != NULL; curr = curr->next)
			if (curr->type == type || type == ANY)
				n_edges++;

	return n_edges;
}

/// Add edges to Graph representing map of Europe
static void add_connections_normal (map *g)
{
	assert (g != NULL);

	for (size_t i = 0; ! is_sentinel_edge (CONNECTIONS[i]); i++){
		if(CONNECTIONS[i].t == RAIL){
			//add level one rail connection
			add_connection (
			g,
			CONNECTIONS[i].v, CONNECTIONS[i].w,
			CONNECTIONS[i].t,1// rail_distance is 1
			);
		}else{
			add_connection (
			g,
			CONNECTIONS[i].v, CONNECTIONS[i].w,
			CONNECTIONS[i].t,0// rail distance is 0
			);
		}
	}
		
}
/// add the rail connections
static void add_connections_rail (map *g)
{
	assert (g != NULL);
	// add second level rail connection
	// for every locations i
	for(size_t i =0; i <g->n_vertices;i++){
		// for all the locations x that i connected to 
		for(map_adj* curr = g->connections[i];curr != NULL; curr = curr->next){
			//if x is connected to i with a rail distance 1
			if(curr->rail_distance == 1){
				// for each location s that x connected to 
				for(map_adj* curr2 = g->connections[curr->v]; curr2 != NULL; curr2 = curr2->next){
					// if s is not i, and s is within the distance 1 in rail
					// add connection with s and i 
					if( curr2->v != PLACES[i].id && curr2->rail_distance == 1){
							add_connection(g,curr2->v,PLACES[i].id,RAIL,2);// add connection with rail distance 2
						// for each location that s connected to withh in rail distance 1
						//add the third level connection
						for(map_adj* curr3 = g->connections[curr2->v]; curr3 != NULL; curr3 = curr3->next){
							if( curr3->v != PLACES[i].id && curr3->v!= curr->v && curr3->rail_distance == 1){
								add_connection(g,curr3->v,PLACES[i].id,RAIL,3);// add connection with rail distance 3
							}
						}
					}
				}
			}
		}
	}
	/*
	add third level rail connection
	for(size_t i =0; i <g->n_vertices;i++){
		// for all the locations x that i connected to 
		for(map_adj* curr = g->connections[i];curr != NULL; curr = curr->next){
			//if x is connected to i with a rail distance 2
			if(curr->rail_distance == 2){
				// for each location s that x connected to 
				for(map_adj* curr2 = g->connections[curr->v]; curr2 != NULL; curr2 = curr2->next){
					// if s is not i, and s is within the distance 1 in rail
					// add connection with s and i 
					if( curr2->v != PLACES[i].id && curr2->rail_distance == 1){
							add_connection(g,curr2->v,PLACES[i].id,RAIL,3);// add connection with rail distance 3
					}
				}
			}
		}
	}
	*/

		
}


/// Add a new edge to the Map/Graph
static void add_connection (
	map *g,
	location_t start,
	location_t end,
	transport_t type,
	int rail_distance)
{
	assert (g != NULL);
	assert (start != end);
	assert (valid_location_p (start));
	assert (valid_location_p (end));
	assert (valid_transport_p (type));
	assert (rail_distance >= 0 && rail_distance <=3);
	// don't add edges twice
	if (adjlist_contains (g->connections[start], end, type,rail_distance)) return;

	g->connections[start] = adjlist_insert (g->connections[start], end, type,rail_distance);
	g->connections[end]   = adjlist_insert (g->connections[end], start, type,rail_distance);
	g->n_edges++;
}

/// Is this the magic 'sentinel' edge?
static inline bool is_sentinel_edge (connection x)
{
	return x.v == -1 && x.w == -1 && x.t == ANY;
}

/// Insert a node into an adjacency list.
static map_adj *adjlist_insert (map_adj *list, location_t v, transport_t type,int rail_distance)
{
	assert (valid_location_p (v));
	assert (valid_transport_p (type));

	map_adj *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate map_adj");
	(*new) = (map_adj){ .v = v, .type = type,  .rail_distance = rail_distance, .next = list};
	return new;
}

/// Does this adjacency list contain a particular value?
static bool adjlist_contains (map_adj *list, location_t v, transport_t type,int rail_distance)
{
	assert (valid_location_p (v));
	assert (valid_transport_p (type));

	for (map_adj *curr = list; curr != NULL; curr = curr->next)
		if (curr->v == v && curr->type == type && curr ->rail_distance == rail_distance)
			return true;
	return false;
}


























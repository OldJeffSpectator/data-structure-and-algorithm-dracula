////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// hunter_view.c: the HunterView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sysexits.h>
#include <string.h>
#include "game.h"
#include "game_view.h"
#include "hunter_view.h"
#include <stdio.h>
#define DRACULA 4
#define RECORD_POINT 30
#define NO_RESRICT -1
#include "map.h" 
#include "queue.h"
#include "queue_location.h"
#include "helper.h"
typedef struct hunter_view {
	Queue_location dracula_trail;
	location_t dracula_location;
	GameView gv;
	player_message last_message;
	size_t turn;
	location_t possible_location;
	Map euro;
} hunter_view;



void current_location(hunter_view* hv,char *past_plays);
hunter_view *hv_new (char *past_plays, player_message messages[])
{
	
	hunter_view *new = malloc (sizeof *new);
	new->euro = map_new();
	new->gv = gv_new(past_plays, messages);
	new->dracula_trail = queue_location_new();
	new->dracula_location = UNKNOWN_LOCATION;
	new->possible_location = UNKNOWN_LOCATION;
	if (new == NULL) err (EX_OSERR, "couldn't allocate HunterView");
	current_location(new,past_plays);
	if(new->turn == 0){
		new->last_message[0] = '\0';
	}else{
		//size_t len = strlen(past_plays);
		if(new->turn % 5 == 0){// lordgaldming's turn
			
			strcpy(new->last_message,messages[new->turn -2]);
		}else{
			strcpy(new->last_message,messages[new->turn -1]);
		}
	

	}
	return new;
}

void current_location(hunter_view* hv,char *past_plays){
	unsigned int len = strlen(past_plays);

	

	hv->turn = (len+1)/8;
	if(hv->turn>=5){

		if(hv->turn % 5 != 0){// not goldming's turn
			if(past_plays[len-4]!='.'){
				char abbrev[3]= {'\0','\0','\0'};
				abbrev[0]=past_plays[len-6];
				abbrev[1]=past_plays[len-5];
				hv->possible_location = location_find_by_abbrev(abbrev);
			}
		}else{
			if(past_plays[len-12]!= '.'){
				char abbrev[3]= {'\0','\0','\0'};
				abbrev[0]=past_plays[len-14];
				abbrev[1]=past_plays[len-13];
				hv->possible_location = location_find_by_abbrev(abbrev);
			}			
		}
	}	



	for(unsigned int n=4 *8; n<len; n+=(8*NUM_PLAYERS)){
		char move[3]= {past_plays[n+1],past_plays[n+2],'\0'}; 
		
		location_t tmp = location_find_by_abbrev (move);
		location_t prev = hv->dracula_location;
		hv->dracula_location = tmp;
		// case to update exact location
		if( tmp == UNKNOWN_LOCATION &&move[0] =='D'){
				
			int pos = atoi(&(move[1]));
			switch(pos){
				case 1:hv->dracula_location =
							DOUBLE_BACK_1;break;
				case 2:hv->dracula_location =
							DOUBLE_BACK_2;break;
				case 3:hv->dracula_location =
							DOUBLE_BACK_3;break;
				case 4:hv->dracula_location =
							DOUBLE_BACK_4;break;
				case 5:hv->dracula_location =
							DOUBLE_BACK_5;break;
			}
		
			assert(queue_location_size(hv->dracula_trail) != 0);
		
			assert(pos>=1 && pos <= 5);
			Node_location curr = hv->dracula_trail->head;
			for (int a =queue_location_size(hv->dracula_trail);
						a>=1&& curr!= NULL;
						a--, curr = curr->next){
				if( a == pos && valid_location_p(curr->item) ){
					hv->dracula_location = curr->item;
					break;
				}
			}
		} else if(strcmp(move,"C?")==0){
			//city unkown
			hv->dracula_location = CITY_UNKNOWN;
			
		} else if(strcmp(move,"S?")==0) {
			// sea unkown
			hv->dracula_location= SEA_UNKNOWN;
			
		} else if (strcmp(move,"HI")== 0){
		// Hide 
			if(valid_location_p(prev)) hv->dracula_location = prev;
			else  hv->dracula_location = HIDE;
		} else if(strcmp(move,"TP")==0) {
			// teleport
				hv->dracula_location = CASTLE_DRACULA;
		} 
		if(queue_location_size(hv->dracula_trail)== TRAIL_SIZE) queue_location_de(hv->dracula_trail);
		queue_location_en(hv->dracula_trail,hv->dracula_location);

	}
}




void hv_drop (hunter_view *hv)
{
	map_drop(hv->euro);
	queue_location_drop(hv->dracula_trail);
	gv_drop(hv->gv);
	free (hv);
}


char* hv_get_last_message(hunter_view *hv){
	char* str = malloc(sizeof(char)*(MESSAGE_SIZE+1));
	strcpy(str,hv->last_message);
	return str;
}


round_t hv_get_round (hunter_view *hv)
{
	
	return gv_get_round(hv->gv);
}

enum player hv_get_player (hunter_view *hv)
{
	
	return gv_get_player(hv->gv);
}

int hv_get_score (hunter_view *hv)
{
	return gv_get_score(hv->gv);
}

int hv_get_health (hunter_view *hv, enum player player)
{
	
	return gv_get_health(hv->gv,player);
}

location_t hv_get_location (hunter_view *hv, enum player player)
{
	if(player == PLAYER_DRACULA){
		return hv->dracula_location;
	}
	location_t curr = gv_get_location(hv->gv,player);
	
	return curr; 
}
location_t hv_get_dracula_possible(hunter_view *hv)
{
	return hv->possible_location;
}

void hv_get_trail (
	hunter_view *hv, enum player player,
	location_t trail[TRAIL_SIZE])
{
	gv_get_history(hv->gv,player,trail);
}

location_t *hv_get_dests (
	hunter_view *hv, size_t *n_locations,
	bool road, bool rail, bool sea)
{
	
	*n_locations = 0;
	return hv_get_dests_player(hv,n_locations,hv_get_player(hv),road,rail,sea);
	
}

location_t *hv_get_dests_player (
	hunter_view *hv, size_t *n_locations, enum player player,
	bool road, bool rail, bool sea)
{
	
	*n_locations = 0;
	round_t round = hv_get_round(hv);
	int playerNo_curr = get_player_number(hv_get_player(hv));
	int k = get_player_number(player);
	if(k < playerNo_curr ){
		round ++;// past player
	}
	location_t* result =gv_get_connections(hv->gv,n_locations,
							hv_get_location(hv,player),
							player,round,
							road,rail,sea);
	if( player != PLAYER_DRACULA) return result;
	//return result;
	size_t t =0;
	location_t* real=NULL;
	location_t special = (queue_location_size(hv->dracula_trail)==6)?
								hv->dracula_trail->head->item:hv->dracula_location;
	for( unsigned int n =0;n<*n_locations;n++){
		assert(valid_location_p (result[n]) );
		if(queue_location_in(hv->dracula_trail,result[n]) == true){
			if(!(special == result[n] ||result[n] == hv->dracula_location)){
				continue;
			}
		}
		
		//continue;
		real = add_connection(real,&t,result[n]);
	}
	//printf("%d\n",t);
	(*n_locations) = t;
	
	free(result);
	return real;
	
}






// finds shortest path to destination and returns the next move 
// required to reach it
// use basic bfs search (with modified map which can also be used for rail connections searching)
location_t hv_find_shortest_path (location_t start, location_t end,hunter_view* hv,int round, int playerNo) 
{
	Map m = hv->euro;
	assert(valid_location_p(start)&& valid_location_p(end));
	assert(round <= 366);
	assert(playerNo<=3);
	location_t spt[NUM_MAP_LOCATIONS];
	bool visited[NUM_MAP_LOCATIONS]; 

	// initialise arrays
	for (int i = 0; i < NUM_MAP_LOCATIONS; i++) {
		spt[i] = NOWHERE;
		visited[i] = false; 
	}
	int rail_index = (round+playerNo) % 4;
	Queue q = newQueue(); 
	enQueue (q, start,rail_index); 
	visited[start] = true; 

	while (! QueueisEmpty (q)) {
		int index = -1;
		location_t location_tmp = UNKNOWN_LOCATION;
		deQueue(q,&location_tmp,&index);
		assert(index != -1);
		assert(location_tmp!= UNKNOWN_LOCATION);
		// to all the location that the current conneted to
		for(map_adj* curr = m->connections[location_tmp];curr != NULL;curr = curr->next){
			if(visited[curr->v]|| curr->rail_distance > index)continue;
			// distance out of bonnd or have been visited before
			spt[curr->v]= location_tmp;
			visited[curr->v]= true;
			enQueue(q,curr->v,increment_rail_index(index));
		}
	}

	dropQueue (q); 

	if( spt[end]== UNKNOWN_LOCATION){
		return UNKNOWN_LOCATION;// START and end is not connected
	}
	location_t next_dest = spt[end];
   	if(next_dest == start ){
        return end;
    }
   	while( next_dest != start && next_dest != UNKNOWN_LOCATION){ 
		if( spt[next_dest] == start || spt[next_dest] == UNKNOWN_LOCATION)
			break;


		next_dest = spt[next_dest];
   	}


	return next_dest;

}




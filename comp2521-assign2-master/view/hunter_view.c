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
// #include "map.h" ... if you decide to use the Map ADT



typedef struct node_hv* Node_hv;
typedef struct queue_hv* Queue_hv;
struct queue_hv{
   Node_hv head;  
   Node_hv tail;
	int max;
   int num;
};
struct node_hv{
   Node_hv next;
   location_t item;
};
typedef struct hunter_view {
	Queue_hv dracula_trail;
	location_t dracula_location;
	GameView gv;
} hunter_view;
int hunter_number_hv(enum player player);
bool queue_hv_in(Queue_hv q, location_t n);
Queue_hv queue_hv_new(int max);
location_t queue_hv_de(Queue_hv q);
void queue_hv_en(Queue_hv q,location_t v);
int queue_hv_size(Queue_hv q);
void queue_hv_drop(Queue_hv q);
location_t* add_connection_hv(location_t* dest,size_t *n_location,location_t new);
void current_location(hunter_view* hv,char *past_plays);
hunter_view *hv_new (char *past_plays, player_message messages[])
{
	
	hunter_view *new = malloc (sizeof *new);
	new->gv = gv_new(past_plays, messages);
	new->dracula_trail = queue_hv_new(6);
	new->dracula_location = UNKNOWN_LOCATION;
	if (new == NULL) err (EX_OSERR, "couldn't allocate HunterView");
	current_location(new,past_plays);
	return new;
}

void current_location(hunter_view* hv,char *past_plays){
	unsigned int len = strlen(past_plays);
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
		
			assert(queue_hv_size(hv->dracula_trail) != 0);
		
			assert(pos>=1 && pos <= 5);
			Node_hv curr = hv->dracula_trail->head;
			for (int a =queue_hv_size(hv->dracula_trail);
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

		queue_hv_en(hv->dracula_trail,hv->dracula_location);

	}
}




void hv_drop (hunter_view *hv)
{
	queue_hv_drop(hv->dracula_trail);
	gv_drop(hv->gv);
	free (hv);
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
	int playerNo_curr = hunter_number_hv(hv_get_player(hv));
	int k = hunter_number_hv(player);
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
	location_t special = (queue_hv_size(hv->dracula_trail)==6)?
								hv->dracula_trail->head->item:hv->dracula_location;
	for( unsigned int n =0;n<*n_locations;n++){
		assert(valid_location_p (result[n]) );
		if(queue_hv_in(hv->dracula_trail,result[n]) == true){
			if(!(special == result[n] ||result[n] == hv->dracula_location)){
				continue;
			}
		}
		
		//continue;
		real = add_connection_hv(real,&t,result[n]);
	}
	//printf("%d\n",t);
	(*n_locations) = t;
	
	free(result);
	return real;
	
}

location_t* add_connection_hv(location_t* dest,size_t *n_location,location_t new){
	
	if(*n_location == 0){
		dest= malloc(sizeof(location_t));
		
	}else{
		dest = realloc(dest,(sizeof(location_t))*((*n_location)+1));
	
	}
	dest[(*n_location)] = new;
	(*n_location)++;
	return dest;


}


////////////////////
// queue_hv functions

int queue_hv_size(Queue_hv q){
   return q->num;

}


void queue_hv_drop(Queue_hv q){
	while(queue_hv_size(q) > 0){

		queue_hv_de(q);
	}
	free(q);
}
void queue_hv_en(Queue_hv q,location_t v){
   
   Node_hv new = malloc(sizeof(struct node_hv));
   new->item = v;
   new->next = NULL;
   if(q->num == q->max &&q->max!= NO_RESRICT ){
			queue_hv_de(q);
	}
   
   if( q->num == 0){
      q->head = new;
      q->tail = new;
      
   }else{
		
      q->tail->next = new;
      q->tail = new;
   
   }
   q->num++;
}
location_t queue_hv_de(Queue_hv q){
	assert(q->head != NULL);
   Node_hv tmp = q->head;
   location_t v = tmp->item;
	if( q->head == q->tail){
		q->tail = NULL;
	}
   q->head = q->head->next;
	
   q->num--;
   free(tmp);
   return v;
   

}

Queue_hv queue_hv_new(int max){
   Queue_hv new = malloc(sizeof(struct queue_hv));
   new->num = 0;
	new->max = max;
   new->head = NULL;
   new->tail = NULL;
   return new;
}
bool queue_hv_in(Queue_hv q, location_t n){
	for(Node_hv curr = q->head; curr != NULL ; curr = curr->next){
		if(curr->item == n){
			return true;
		}
	}
	return false;

}
int hunter_number_hv(enum player player){
	switch(player){
		case PLAYER_LORD_GODALMING:return 0;
      case PLAYER_DR_SEWARD:return 1;
      case PLAYER_VAN_HELSING:return 2;
      case PLAYER_MINA_HARKER:return 3;
		case PLAYER_DRACULA: return 4;
	}
	return -1;
}
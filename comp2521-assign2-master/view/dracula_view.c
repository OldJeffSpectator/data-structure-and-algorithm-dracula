////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// dracula_view.c: the DraculaView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <string.h>
#include "dracula_view.h"
#include "game.h"
#include "game_view.h"
#define DRACULA 4
#define RECORD_POINT 30
#define NO_RESRICT -1
#define VAMPIRE UNKNOWN_LOCATION 
// #include "map.h" ... if you decide to use the Map ADT
typedef struct node_dv* Node_dv;
typedef struct queue_dv* Queue_dv;
struct queue_dv{
   Node_dv head;  
   Node_dv tail;
	int max;
   int num;
};
struct node_dv{
   Node_dv next;
   location_t item;
};
bool queue_dv_in(Queue_dv q, location_t n);
Queue_dv queue_dv_new(int max);
location_t queue_dv_de(Queue_dv q);
void queue_dv_en(Queue_dv q,location_t v);
int queue_dv_size(Queue_dv q);
void queue_dv_drop(Queue_dv q);


typedef struct dracula_view {
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	GameView gv;
	Queue_dv dracula_trap;
	Queue_dv dracula_trail;
	location_t vampire_location;
	location_t current_location[NUM_PLAYERS];
	location_t previous_location[NUM_PLAYERS];
	int health[NUM_PLAYERS-1];//HUNTER'S health

} dracula_view;
int trap_counter_dv(char* tmp);
void trail_trap_simulator(dracula_view* dv,char* past_plays);
void current_location_handler(dracula_view* dv, int playerNo, char* move);
void trap_encounter_dv(dracula_view* dv,int hunter_no,int x);
void send_to_hospital_dv(dracula_view* dv,int hunter_no);
void trap_placed_dv(dracula_view* dv);
void vampire_place_dv(dracula_view* dv);
void vampire_mature_dv(dracula_view* dv);
void trap_vanish_dv(dracula_view* dv);
void encounter_check_dv(bool encounter[3],char* past_plays);
void vampire_encounter_dv(dracula_view* dv);
dracula_view *dv_new (char *past_plays, player_message messages[])
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	dracula_view *dv= malloc (sizeof *dv);
	if (dv== NULL) err (EX_OSERR, "couldn't allocate DraculaView");
	dv->gv = gv_new(past_plays,messages);
	//hunter's health and location
	for( int n=0; n<NUM_PLAYERS-1; n++){
		dv->current_location[n] = UNKNOWN_LOCATION;
		dv->health[n] = GAME_START_HUNTER_LIFE_POINTS;
		dv->previous_location[n] = UNKNOWN_LOCATION;
	}
	dv->previous_location[DRACULA] = UNKNOWN_LOCATION;
	dv->current_location[DRACULA] = UNKNOWN_LOCATION;
	dv->vampire_location = UNKNOWN_LOCATION;
	dv->dracula_trap = queue_dv_new(6);
	dv->dracula_trail = queue_dv_new(6);

	trail_trap_simulator(dv,past_plays);
	return dv;
}


void trail_trap_simulator(dracula_view* dv,char* past_plays){
	int turn =0;
	unsigned int len = strlen(past_plays);
	for(unsigned int n=0; n<len; n+=8,turn++){
      // get the current playerNo 
      int playerNo = turn %5;
      // get the move of the last play
      char move[3]= {past_plays[n+1],past_plays[n+2],'\0'}; 

      // update current posoition
		current_location_handler(dv,playerNo,move);
      
      
      // Dracula's turn
      if(past_plays[n] == 'D'){
			

		
			if(past_plays[n+3]=='T'){
				trap_placed_dv(dv);
			}
			if(past_plays[n+4]=='V'){
				vampire_place_dv(dv);
			} 



         if(past_plays[n+5] == 'V'){
            vampire_mature_dv(dv);
         }else if( past_plays[n+5] =='M'){
				trap_vanish_dv(dv);
			}

      
      } else { 
         //hunter's turn
         
         bool tvd[3]= {false,false,false};
			char tmp[5] = {past_plays[n+3],past_plays[n+4],past_plays[n+5],past_plays[n+6],'\0'};

			encounter_check_dv(tvd,tmp);
			// encounter all the trap first;
			if(tvd[0] == true) {
				int n_trap = trap_counter_dv(tmp);	
				trap_encounter_dv(dv,playerNo,n_trap);
			}
			// then vampire
			if(tvd[1] == true) vampire_encounter_dv(dv);
			
				
				
			
         
         
         
         
			

         // the end of ACTION phase
			// regain the health by resting
         if(dv->previous_location[playerNo]== dv->current_location[playerNo] ){
				dv->health[playerNo] += LIFE_GAIN_REST;
				// can not be higher than the start life
				if( dv->health[playerNo] > GAME_START_HUNTER_LIFE_POINTS){
					dv->health[playerNo] = GAME_START_HUNTER_LIFE_POINTS;
				}
			}
			
         
         
         
         
         
         
         
      
         
         
         
         
      
         
         
         
          
         
      }
      
   }


}
void current_location_handler(dracula_view* dv, int playerNo, char* move){

	if(playerNo == DRACULA){
		
		dv->previous_location[playerNo] = dv->current_location[playerNo];
		dv->current_location[playerNo] = location_find_by_abbrev (move);
		// DOUBLE 	back
		// update to the exact loaction
		if( dv->current_location[playerNo] == UNKNOWN_LOCATION
			&& move[0] == 'D'){
			
				
			int pos = atoi(&(move[1]));
			switch(pos){
				case 1:dv->current_location[playerNo] =
							DOUBLE_BACK_1;break;
				case 2:dv->current_location[playerNo] =
							DOUBLE_BACK_2;break;
				case 3:dv->current_location[playerNo] = 
							DOUBLE_BACK_3;break;
				case 4:dv->current_location[playerNo] =
							DOUBLE_BACK_4;break;
				case 5:dv->current_location[playerNo] = 
							DOUBLE_BACK_5;break;
			}
		
			assert(queue_dv_size(dv->dracula_trail) != 0);
			
			assert(pos>=1 && pos <= 5);
			Node_dv curr = dv->dracula_trail->head;


			for (int a =queue_dv_size(dv->dracula_trail);
						a>=1&& curr!= NULL;
						a--, curr = curr->next){
				if( a == pos){
					queue_dv_en(dv->dracula_trail,curr->item);
					dv->current_location[playerNo] = curr->item;
					break;
				}
			}



		} else if (strcmp(move,"HI")== 0){
			// Hide 
			assert(queue_dv_size(dv->dracula_trail) != 0);
			dv->current_location[playerNo]  = dv->dracula_trail->tail->item;
			queue_dv_en(dv->dracula_trail,
						dv->dracula_trail->tail->item);
			
		} else if(strcmp(move,"TP")==0) {
			// teleport
			dv->current_location[playerNo] = CASTLE_DRACULA;
			queue_dv_en(dv->dracula_trail,
						CASTLE_DRACULA);
			// normally
			// the above will never be reached in dracula view
			// just in case
		} else if(strcmp(move,"C?")==0){
			//city unkown
			dv->current_location[playerNo] = CITY_UNKNOWN;
			queue_dv_en(dv->dracula_trail,
						CITY_UNKNOWN);
		} else if(strcmp(move,"S?")==0) {
			// sea unkown
			dv->current_location[playerNo] = SEA_UNKNOWN;
			queue_dv_en(dv->dracula_trail,
						SEA_UNKNOWN);
		} else{
			queue_dv_en(dv->dracula_trail,
						dv->current_location[playerNo]);
		
		}
		
		
		
	}else{
		dv->previous_location[playerNo] = dv->current_location[playerNo];
		dv->current_location[playerNo] = location_find_by_abbrev (move);
		//send to hospital
		if( dv->current_location[playerNo] == HOSPITAL_LOCATION&& dv->health[playerNo] == 0){
				dv->health[playerNo] = GAME_START_HUNTER_LIFE_POINTS;
		}
	}
}
void trap_encounter_dv(dracula_view* dv,int hunter_no,int x){
	assert(x != 0);
	location_t hunter_location = dv->current_location[hunter_no];
	Node_dv next = NULL;
	Node_dv prev = NULL;
	Queue_dv q = dv->dracula_trap;
	int count =0;
/*
	Node_dv h =dv->dracula_trap->head;
	// for delete head



	while(h->item == hunter_location && h->item != VAMPIRE && count < x){

		count++;
		dv->health[hunter_no] -= LIFE_LOSS_TRAP_ENCOUNTER;
		if( q->head == q->tail){
			q->tail = NULL;
		}

		q->head = q->head->next;
		q->num--;
		free(h);
		h = q->head;
		if( h == NULL) break;
	}

	while( h != NULL){
		

	}*/


	for(Node_dv curr = q->head; curr!= NULL&& dv->health[hunter_no]>0&&count < x; ){
		next = curr->next;

		//encounter a trap
		// and cant be vampire, has to be a trap
		if( curr->item == hunter_location){
			assert(curr->item != UNKNOWN_LOCATION);
			count++;
			// lose health point
			dv->health[hunter_no] -= LIFE_LOSS_TRAP_ENCOUNTER;
			// remove the head

			Node_dv tmp= curr;
			// update the head
			if( tmp == q->head && tmp ==q->tail){
				
				q->head = NULL;
				q->tail = NULL;
				q->num --;
				free(curr);
				break;
				
			} else if (tmp !=q->head && tmp ==q->tail){
				q->tail = prev;
				q->tail->next = NULL;
				q->num --;
				free(curr);
				break;
			}else if( tmp ==q->head && tmp !=q->tail ) {
				prev = NULL;
				q->head = q->head->next;
				q->num --;
				free(curr);
				
			}else{
				prev->next = curr->next;
				q->num --;
				free(curr);
			}



		}else{
			prev = curr;
			
		}
		curr = next;
	}
	if( dv->health[hunter_no]<=0){
		send_to_hospital_dv(dv,hunter_no);

	}


}

void send_to_hospital_dv(dracula_view* dv,int hunter_no){
   // switch the location
   dv->current_location[hunter_no] = HOSPITAL_LOCATION;
   // regain health
   dv->health[hunter_no] = 0;
}

void trap_placed_dv(dracula_view* dv){
	queue_dv_en(dv->dracula_trap,dv->current_location[4]);
}



void vampire_place_dv(dracula_view* dv){
	queue_dv_en(dv->dracula_trap,VAMPIRE);
	dv->vampire_location = dv->current_location[4];
}
void vampire_mature_dv(dracula_view* dv){
	//somehow delete the vampire from the list
	dv->vampire_location = UNKNOWN_LOCATION;
}
void vampire_encounter_dv(dracula_view* dv){
	//somehow delete the vampire from the list

	if(dv->dracula_trap->head == NULL) return;
	if(dv->dracula_trap->head == dv->dracula_trap->tail){
		assert(dv->dracula_trap->head->item == VAMPIRE);
		Node_dv tmp =dv->dracula_trap->head;
		dv->dracula_trap->head= NULL;
		dv->dracula_trap->tail= NULL;
		
		free(tmp);
	}else if(dv->dracula_trap->head->item == VAMPIRE) {
		Node_dv tmp =dv->dracula_trap->head;
		dv->dracula_trap->head = dv->dracula_trap->head->next;
		
		free(tmp);
	}else{
		bool d = false;
		for( Node_dv curr = dv->dracula_trap->head; 
			curr != NULL && curr != dv->dracula_trap->tail; 
			curr = curr->next){
			assert(curr->next != NULL);
			if( curr ->next->item == VAMPIRE){
				Node_dv tmp = curr ->next;
				curr->next = curr->next ->next;
				free(tmp);
				d = true;
				break;
			}

		}
		assert(d == true);


	}

	dv->dracula_trap->num --;
	dv->vampire_location = UNKNOWN_LOCATION;
}

void trap_vanish_dv(dracula_view* dv){
	assert(dv != NULL);
	return;
}

void encounter_check_dv(bool encounter[3],char* past_plays){
	for(int n=0; n<4; n++){
		if( past_plays[n] =='T'){
			encounter[0] = true;
		}

		
		if (past_plays[n]=='V') {
			encounter[1] = true;
		}
		if (past_plays[n]=='D') {
			encounter[2] = true;
		}
		
		
	}

}
int trap_counter_dv(char* tmp){
	int count =0;
	for(int n=0; n<4;n++){
		if(tmp[n] =='T'){
			count ++;

		}

	}
	return count;


}

//////////////////////////////////////////


void dv_drop (dracula_view *dv)
{
	queue_dv_drop(dv->dracula_trail);
	queue_dv_drop(dv->dracula_trap);
	gv_drop(dv->gv);
	free (dv);
}
//done
round_t dv_get_round (dracula_view *dv)
{
	
	return gv_get_round(dv->gv);
}
//done
int dv_get_score (dracula_view *dv)
{
	
	return gv_get_score(dv->gv);
}
//done
int dv_get_health (dracula_view *dv, enum player player)
{
	
	return gv_get_health(dv->gv,player);
}



location_t dv_get_location (dracula_view *dv, enum player player)
{
	
	if( player == PLAYER_DRACULA){
		return dv->current_location[DRACULA];
	}

	
	return gv_get_location(dv->gv,player);
}

void dv_get_player_move (
	dracula_view *dv, enum player player,
	location_t *start, location_t *end)
{
	/*
	int playerNo = -1;

	switch(player){
      case PLAYER_LORD_GODALMING:playerNo =0 ;break;
      case PLAYER_DR_SEWARD:playerNo =1 ;break;
      case PLAYER_VAN_HELSING:playerNo =2 ;break;
      case PLAYER_MINA_HARKER:playerNo =3 ;break;
      case PLAYER_DRACULA:playerNo =DRACULA ;break;

   }
	
	*start = dv->previous_location[playerNo];
	*end = dv->current_location[playerNo];
	*/

	location_t* trail = malloc(sizeof(location_t)*6);
	gv_get_history(dv->gv,player,trail);
   *end = trail[0];
   *start = trail[1];
	/*
	if(dv_get_health(dv,player) <= 0){
		 *end = HOSPITAL_LOCATION;//edge case
		 //if the player die
	}

	*/
   free(trail);

}

void dv_get_locale_info (
	dracula_view *dv, location_t where,
	int *n_traps, int *n_vamps)
{
	*n_traps =0;
	*n_vamps = 0;
	if(where == UNKNOWN_LOCATION) return;
	for( Node_dv curr = dv->dracula_trap->head; curr != NULL; curr = curr->next){
		if( curr->item == where){
			(*n_traps) ++;
		}

	}
	if( dv->vampire_location == where){
		(*n_vamps) ++;
	}
	
}

void dv_get_trail (
	dracula_view *dv, enum player player,
	location_t trail[TRAIL_SIZE])
{
	if(player != PLAYER_DRACULA){
		location_t tmp[6];
		gv_get_history(dv->gv,player,tmp);
		for(int n=0; n<TRAIL_SIZE;n++){
			trail[n] = tmp[n];
		}

	}else{
		Node_dv curr = dv->dracula_trail->head;
		for(int n =(queue_dv_size(dv->dracula_trail))-1;n>=0&& curr!= NULL;curr = curr->next,n--){
			trail[n] = curr->item;
		}
		for(int n= queue_dv_size(dv->dracula_trail); n< TRAIL_SIZE;n++){
			trail[n] = UNKNOWN_LOCATION;
		}
	

	}
}

location_t *dv_get_dests (
	dracula_view *dv, size_t *n_locations, bool road, bool sea)
{
	
	*n_locations = 0;
	size_t t=0;
	location_t* unfilted = gv_get_connections(dv->gv,&t,
									dv->current_location[DRACULA],PLAYER_DRACULA,
									dv_get_round(dv),road,false,sea);
	location_t* final = NULL;
	location_t special_valid_location = dv->current_location[DRACULA];
	if(queue_dv_size(dv->dracula_trail)==6){
		special_valid_location = dv->dracula_trail->head->item;
	}
	for(unsigned int n=0; n<t;n++){
		if(queue_dv_in(dv->dracula_trail,unfilted[n]) == false||
			unfilted[n] ==dv->current_location[DRACULA]
			|| unfilted[n] == special_valid_location){
			if(final == NULL) final = malloc(sizeof(location_t));
			else final = realloc(final,sizeof(location_t)*((*n_locations)+1));

			
			final[*n_locations] = unfilted[n];
			(*n_locations)++;

		}


	}
	

	
	free(unfilted);
	return final;
}

location_t *dv_get_dests_player (
	dracula_view *dv, size_t *n_locations, enum player player,
	bool road, bool rail, bool sea)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*n_locations = 0;
	if(player == PLAYER_DRACULA){
		return dv_get_dests (dv, n_locations,  road, sea);

	}

	return gv_get_connections(dv->gv,n_locations,gv_get_location(dv->gv,player),
									player,dv_get_round(dv)+1,
									road,rail,sea);// hunter is next turn
}





/////////////////////////////////////////////



/////////////////////////////////////////
// queue_dv functions

int queue_dv_size(Queue_dv q){
   return q->num;

}


void queue_dv_drop(Queue_dv q){
	while(queue_dv_size(q) > 0){

		queue_dv_de(q);
	}
	free(q);
}
void queue_dv_en(Queue_dv q,location_t v){
   
   Node_dv new = malloc(sizeof(struct node_dv));
   new->item = v;
   new->next = NULL;
   if(q->num == q->max &&q->max!= NO_RESRICT ){
			queue_dv_de(q);
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
location_t queue_dv_de(Queue_dv q){
	assert(q->head != NULL);
   Node_dv tmp = q->head;
   location_t v = tmp->item;
	if( q->head == q->tail){
		q->tail = NULL;
	}
   q->head = q->head->next;
	
   q->num--;
   free(tmp);
   return v;
   

}

Queue_dv queue_dv_new(int max){
   Queue_dv new = malloc(sizeof(struct queue_dv));
   new->num = 0;
	new->max = max;
   new->head = NULL;
   new->tail = NULL;
   return new;
}
bool queue_dv_in(Queue_dv q, location_t n){
	for(Node_dv curr = q->head; curr != NULL ; curr = curr->next){
		if(curr->item == n){
			return true;
		}
	}
	return false;

}

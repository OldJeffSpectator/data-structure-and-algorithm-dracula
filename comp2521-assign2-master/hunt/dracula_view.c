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
#include "map.h" 
#include "queue.h"
#include "queue_location.h"
#include "helper.h"
typedef struct dracula_view {
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	GameView gv;
	Map euro;
	Queue_location dracula_trap;
	Queue_location dracula_trail;
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
	dv->euro = map_new();
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
	dv->dracula_trap = queue_location_new();
	dv->dracula_trail = queue_location_new();

	trail_trap_simulator(dv,past_plays);
	return dv;
}

// simulate the traps encounters
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
			

		  	// place trap
			if(past_plays[n+3]=='T'){
				trap_placed_dv(dv);
			}
			// vampire
			if(past_plays[n+4]=='V'){
				vampire_place_dv(dv);
			} 


		//vampire mature
         if(past_plays[n+5] == 'V'){
            vampire_mature_dv(dv);
			//trail fade
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
// handle the real current location of the player
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
		
			assert(queue_location_size(dv->dracula_trail) != 0);
			
			assert(pos>=1 && pos <= 5);
			Node_location curr = dv->dracula_trail->head;


			for (int a =queue_location_size(dv->dracula_trail);
						a>=1&& curr!= NULL;
						a--, curr = curr->next){
				if( a == pos){
					if(queue_location_size(dv->dracula_trail)==6)queue_location_de(dv->dracula_trail);
					queue_location_en(dv->dracula_trail,curr->item);
					dv->current_location[playerNo] = curr->item;
					break;
				}
			}
			



		} else if (strcmp(move,"HI")== 0){
			// Hide 
			assert(queue_location_size(dv->dracula_trail) != 0);
			dv->current_location[playerNo]  = dv->dracula_trail->tail->item;
			if(queue_location_size(dv->dracula_trail)==6)queue_location_de(dv->dracula_trail);
			queue_location_en(dv->dracula_trail,
						dv->dracula_trail->tail->item);
			
		} else if(strcmp(move,"TP")==0) {
			// teleport
			dv->current_location[playerNo] = CASTLE_DRACULA;
			if(queue_location_size(dv->dracula_trail)==6)queue_location_de(dv->dracula_trail);
			queue_location_en(dv->dracula_trail,
						CASTLE_DRACULA);
			// normally
			
			// the above will never be reached in dracula view
			// just in case
		} else if(strcmp(move,"C?")==0){
			//city unkown
			dv->current_location[playerNo] = CITY_UNKNOWN;
			if(queue_location_size(dv->dracula_trail)==6)queue_location_de(dv->dracula_trail);
			queue_location_en(dv->dracula_trail,
						CITY_UNKNOWN);
						
		} else if(strcmp(move,"S?")==0) {
			// sea unkown
			dv->current_location[playerNo] = SEA_UNKNOWN;
			if(queue_location_size(dv->dracula_trail)==6)queue_location_de(dv->dracula_trail);
			queue_location_en(dv->dracula_trail,
						SEA_UNKNOWN);
						
		} else{
			location_t t = dv->current_location[playerNo];
			if(queue_location_size(dv->dracula_trail)==6)queue_location_de(dv->dracula_trail);
			queue_location_en(dv->dracula_trail,t);
			
			
		
		}
		
		
	}else{
		//hunter's turn
		// store the previous location
		dv->previous_location[playerNo] = dv->current_location[playerNo];
		dv->current_location[playerNo] = location_find_by_abbrev (move);
		//send to hospital
		if( dv->current_location[playerNo] == HOSPITAL_LOCATION&& dv->health[playerNo] == 0){
				dv->health[playerNo] = GAME_START_HUNTER_LIFE_POINTS;
		}
	}

	
}
// handle encounter the trap
void trap_encounter_dv(dracula_view* dv,int hunter_no,int x){
	assert(x != 0);
	location_t hunter_location = dv->current_location[hunter_no];
	
	int count =0;
	while(count < x&& dv->health[hunter_no]>0){
		dv->health[hunter_no] -= LIFE_LOSS_TRAP_ENCOUNTER;
		deleteNode(&(dv->dracula_trap->head),hunter_location,&count);
	}
	up_date_tail(dv->dracula_trap);


	if( dv->health[hunter_no]<=0){
		send_to_hospital_dv(dv,hunter_no);

	}


}
//send hunter to hospital
void send_to_hospital_dv(dracula_view* dv,int hunter_no){
   // switch the location
   dv->current_location[hunter_no] = HOSPITAL_LOCATION;
   // regain health
   dv->health[hunter_no] = 0;
}
// handle place the trap
void trap_placed_dv(dracula_view* dv){
	if(queue_location_size(dv->dracula_trap)==6)queue_location_de(dv->dracula_trap);
	queue_location_en(dv->dracula_trap,dv->current_location[4]);
}



void vampire_place_dv(dracula_view* dv){
	if(queue_location_size(dv->dracula_trap)==6)queue_location_de(dv->dracula_trap);
	queue_location_en(dv->dracula_trap,VAMPIRE);
	dv->vampire_location = dv->current_location[4];
}
void vampire_mature_dv(dracula_view* dv){
	//somehow delete the vampire from the list
	dv->vampire_location = UNKNOWN_LOCATION;
}
void vampire_encounter_dv(dracula_view* dv){
	//somehow delete the vampire from the list

	int count=0;
	deleteNode(&(dv->dracula_trap->head),VAMPIRE,&count);
	up_date_tail(dv->dracula_trap);
	
	dv->vampire_location = UNKNOWN_LOCATION;
}

void trap_vanish_dv(dracula_view* dv){
	assert(dv != NULL);
	return;
}
// check what encounter the hunter encounter
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
	
	
	


	queue_location_drop(dv->dracula_trail);
	
	queue_location_drop(dv->dracula_trap);
	gv_drop(dv->gv);
	map_drop(dv->euro);
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
	

	location_t* trail = malloc(sizeof(location_t)*6);
	gv_get_history(dv->gv,player,trail);
   *end = trail[0];
   *start = trail[1];

   free(trail);

}

void dv_get_locale_info (
	dracula_view *dv, location_t where,
	int *n_traps, int *n_vamps)
{
	*n_traps =0;
	*n_vamps = 0;
	if(where == UNKNOWN_LOCATION) return;
	for( Node_location curr = dv->dracula_trap->head; curr != NULL; curr = curr->next){
		if( curr->item == where){
			(*n_traps) ++;
		}

	}
	if( dv->vampire_location == where){
		(*n_vamps) ++;
	}
	
}
void dv_get_trail_move(DraculaView dv ,location_t trail[TRAIL_SIZE]){
	location_t tmp[6];
	gv_get_history(dv->gv,PLAYER_DRACULA,tmp);
	for(int n=0; n<TRAIL_SIZE;n++){
		trail[n] = tmp[n];
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
		Node_location curr = dv->dracula_trail->head;
		
		for(int n =(queue_location_size(dv->dracula_trail))-1;n>=0&& curr!= NULL;curr = curr->next,n--){
			trail[n] = curr->item;
		}
		for(int n= queue_location_size(dv->dracula_trail); n< TRAIL_SIZE;n++){
			trail[n] = UNKNOWN_LOCATION;
		}
	

	}
}

bool dv_db_already(DraculaView dv){
	location_t tmp[6];
	gv_get_history(dv->gv,PLAYER_DRACULA,tmp);
	for(int n=0; n<5; n++){
		
		if (tmp[n]<=DOUBLE_BACK_5 && tmp[n]>= DOUBLE_BACK_1)
			return true;
	}
	return false;
}
bool dv_hi_already(DraculaView dv){
	location_t tmp[6];
	gv_get_history(dv->gv,PLAYER_DRACULA,tmp);
	for(int n=0; n<5; n++){
		if (tmp[n]== HIDE)
			return true;
	}
	return false;
}


// get the possible move according to the original trail info
location_t *dv_get_old_dests (dracula_view *dv, size_t *n_locations, bool road, bool sea){
	size_t tmp =0;
	location_t* x = gv_get_connections(dv->gv,&tmp,
									dv->current_location[DRACULA],PLAYER_DRACULA,
									dv_get_round(dv),road,false,sea);
	*n_locations = tmp;
	return x;
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
	location_t trail[TRAIL_SIZE];
	size_t trail_n = TRAIL_SIZE-1;
	gv_get_history(dv->gv,PLAYER_DRACULA,trail);

	

	for(unsigned int n=0; n<t;n++){
		if(Exist(trail,&trail_n,unfilted[n])== false||
			unfilted[n] ==dv->current_location[DRACULA]){
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





////////////////////////////////////////////////////////////////////

// finds shortest path to destination and returns the next move 
// required to reach it
void dv_find_shortest_path (location_t start, dracula_view* gv,
			int playerNo,int round,int distance[NUM_MAP_LOCATIONS]) 
{
	Map m = gv->euro;
	assert(valid_location_p(start));
	assert(round <= 366);
	assert(playerNo<=3);

	bool visited[NUM_MAP_LOCATIONS]; 

	// initialise arrays
	for (int i = 0; i < NUM_MAP_LOCATIONS; i++) {
		distance[i] = -1;
		visited[i] = false; 
	}
	int rail_index = (round+playerNo) % 4;
	Queue q = newQueue(); 
	enQueue (q, start,rail_index); 
	visited[start] = true; 
	distance[start] = 0;
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
			distance[curr->v] = distance[index]+1;
			visited[curr->v]= true;
			enQueue(q,curr->v,increment_rail_index(index));
		}
	}

	dropQueue (q); 


}
// find the next location in the shortest path to reach the next location
location_t dv_find_next_location (location_t start, location_t end,dracula_view* dv){
	assert(valid_location_p(start));
	assert(valid_location_p(end));
	Map m = dv->euro;
	location_t spt[NUM_MAP_LOCATIONS];
	bool visited[NUM_MAP_LOCATIONS]; 
	location_t trail[TRAIL_SIZE];
	gv_get_history(dv->gv,PLAYER_DRACULA,trail);
	// initialise arrays
	for (int i = 0; i < NUM_MAP_LOCATIONS; i++) {
		spt[i] = NOWHERE;
		visited[i] = false; 
	}
	Queue q = newQueue(); 
	enQueue (q, start,0); 
	visited[start] = true; 
	size_t trail_s = TRAIL_SIZE-1;
	while (! QueueisEmpty (q)) {
		int index = -1;
		location_t location_tmp = UNKNOWN_LOCATION;
		deQueue(q,&location_tmp,&index);
		assert(index != -1);
		assert(location_tmp!= UNKNOWN_LOCATION);
		// to all the location that the current conneted to
		for(map_adj* curr = m->connections[location_tmp];curr != NULL;curr = curr->next){
			if(visited[curr->v]|| curr->rail_distance > index || Exist(trail,&trail_s,curr->v))continue;
			// distance out of bonnd or have been visited before
			// only consider the road and rail connection
			spt[curr->v]= location_tmp;
			visited[curr->v]= true;
			enQueue(q,curr->v,0);
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

// get all the reachable location
// filter out city or sea location
location_t *dv_get_reachable (dracula_view *dv, location_t start,size_t* num,bool city, bool sea){
	size_t newnum =0;
	location_t* dest = dv_get_dests(dv,&newnum,true,true);
	location_t* new= NULL;
	
	
	
	if(city == true && sea == false){
		//add only the cities
		for(size_t i =0; i< newnum; i++){
			if(dest[i] != start && land_p(dest[i])){
				//puts(location_get_name(dest[i]));
				new = add_connection(new,num,dest[i]);
			}
			
		}
		
	}else if (city == false && sea == true){
		// add only sea locations
		for(size_t i =0; i< newnum; i++){
			if(dest[i] != start && sea_p(dest[i])){
				new = add_connection(new,num,dest[i]);
			}
		}

	}else{// add both city and sea connection
		for(size_t i =0; i< newnum; i++){
			if(dest[i] != start){
				new = add_connection(new,num,dest[i]);
			}
		}
	} 

	free(dest);
	

	return new;





}











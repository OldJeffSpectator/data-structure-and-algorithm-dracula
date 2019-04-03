////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include "dracula.h"
#include "dracula_view.h"
#include "game.h"
#include "places.h"
#include "helper.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_VALID_LOCATION (MAX_MAP_LOCATION-MIN_MAP_LOCATION+1)
#define UNREACHABLE -1
#define DANGER_LIFE  (LIFE_LOSS_HUNTER_ENCOUNTER + LIFE_LOSS_HUNTER_ENCOUNTER)


void go_to (location_t des);
size_t find_safest (int* safe_place,size_t size);
bool hunter_reachable (location_t* hunter_locations[4],size_t n_hunter[4],location_t dest);
void make_first_move (DraculaView dv);


void get_all_hunter_reachable (location_t* hunter_locations[4],size_t n_hunter[4],DraculaView dv);
void drop_hunter_locations (location_t* hunter_locations[4]);
int trail_available (DraculaView dv,location_t* hunter_locations[4],size_t n_hunter[4]);

location_t decide_best_location (DraculaView dv,
			location_t hunter_location[4],
			location_t* possible_dest, 
			size_t num);


void decide_dracula_move (DraculaView dv)
{
	
	if (dv_get_round(dv)== 0) {
		// make the first move
		make_first_move (dv);
		return;
	}

	location_t dracula_location = dv_get_location (dv,PLAYER_DRACULA);
	int health = dv_get_health (dv,PLAYER_DRACULA);
	size_t n_p_dest = 0;
	location_t* possible_dest = dv_get_dests (dv,&n_p_dest,true,true);
	
	// if the health is in danger
	// im not at the castle
	// and i can reach the castle(legal move)
	if (health <= DANGER_LIFE 
			&& dracula_location != CASTLE_DRACULA 
		  	&& Exist(possible_dest,&n_p_dest,CASTLE_DRACULA) == true) {
			
		location_t des = dv_find_next_location(dracula_location,CASTLE_DRACULA,dv);
		assert(valid_location_p(des));
		go_to(des);

	} else if (sea_p (dracula_location)) {
		
		// if i am on the sea
		size_t num = 0;
		location_t* coastals = dv_get_reachable (dv,dracula_location,&num,true,false);
		
		location_t* hunter_locations[4];
		size_t hunter_num[4];
		get_all_hunter_reachable (hunter_locations,hunter_num,dv);



		// for all the costal cities i have 
		location_t my_dest = UNKNOWN_LOCATION;
		for (size_t i = 0; i < num; i++){
			if (hunter_reachable(hunter_locations,hunter_num,coastals[i]) == false) {
				my_dest = coastals[i];
				break;
			}
		}	
		
		// if they r all reachable by the hunter
		if (my_dest == UNKNOWN_LOCATION) {
			// go to the adjacent sea
			size_t num_s =0;
			location_t* adj_sea = dv_get_reachable (dv,dracula_location,&num_s,false,true);
			// forced to go to the land
			if (num_s == 0) {
				free (adj_sea);
				num_s =0;
				adj_sea = dv_get_reachable (dv,dracula_location,&num_s,true,true);
			}

			// force to double back
			if(num_s == 0){
				if(dv_db_already(dv) == false)
				register_best_play("D1","suPER pOweR");
				else//have to be TP
				register_best_play("TP","I quit");
			}else{
				go_to (adj_sea[0]);
				
			}
			free (adj_sea);
			

		} else { 
		// else 
			assert (valid_location_p (my_dest));
			go_to (my_dest);
			// go back on the land
		}
		free (coastals);
		drop_hunter_locations (hunter_locations);
				
			
	} else {
		// if i am on the land
		// for all the land city i can reach
			// if they r reachable by the hunter
				
				// if i can hide and the current city is not reachable by the hunter
					// hide
				// else if i can double back & some city cant be reachable by the hunter
					// double back to that city
				// else if i can go to the sea
					// go to the sea
				// else (no safe city in double back and hide and any other locations)
					// go to the safest i can reach
			// else go to the safest 
		
		size_t num = 0;
		location_t* my_land_dest = dv_get_reachable (dv, dracula_location, &num, true, false);
		

		
		size_t n_dest = 0;
		location_t *dest = NULL;
		location_t hunters[4];
		location_t* hunter_locations[4];
		size_t hunter_num[4];
		
		for(size_t n =0; n < 4; n++){
			hunters[n]= dv_get_location (dv,get_player_byNo(n));
		}	
		get_all_hunter_reachable (hunter_locations,hunter_num,dv);
		
		for(size_t i = 0; i < num; i++){

			if (hunter_reachable (hunter_locations, hunter_num, my_land_dest[i]) == false) {
				dest = add_connection (dest, &n_dest, my_land_dest[i]);
			}
		}
		
		if (dest == NULL) {
			
			//if they r all reachable by the hunter
			int trail_num = trail_available (dv, hunter_locations, hunter_num);
			
			//printf("%d\n",trail_num);

			
			if (dv_hi_already (dv) == false// play a hide
				&& hunter_reachable (hunter_locations,hunter_num,dracula_location)==false) {
				register_best_play ("HI","Suicidal Suicidal...");

			} else if (dv_db_already(dv) == false && trail_num != 0) {
				
				// play a double back
				char play[3] = {'D','\0','\0'};
				play[1] = ((char)(trail_num)) + '0';
				assert (play[1]>='1'&& play[1]<='5');
				register_best_play (play,"Skr");
			} else {
				// go to the sea 
				size_t sea_n = 0;
				location_t* sea_dest = dv_get_reachable (dv, dracula_location, &sea_n, false, true);
				// if  i can go to the sea
				if (sea_n != 0){
					
					go_to (decide_best_location(dv, hunters, sea_dest, sea_n));
				} else {
					// if i cant reach any sea legally
					// go to the saftest i can reach on land
					if (my_land_dest != NULL){
						go_to (decide_best_location(dv, hunters, my_land_dest, num));
					} else if (dv_hi_already(dv) == false){ //forced to play a hi
						register_best_play("HI","Suicidal Suicidal...");	
					} else if (dv_db_already(dv) == false) {// force to play a db
						char play[3] = {'D','\0','\0'};
						
						location_t t[6];
						dv_get_trail (dv,PLAYER_DRACULA,t);
						size_t tmp_n = 0;
						location_t* tmp_dest = dv_get_old_dests (dv, &tmp_n, true, true);
						for(size_t i = 0; i < 6; i++) {
							if (Exist (tmp_dest,&tmp_n,t[i])&& valid_location_p (t[i])) {
								play[1] = '1'+((char)(i));
								break;
							}
						}
						assert (play[1]>='1'&& play[1]<='5');
						register_best_play (play,"Skr");

						free (tmp_dest);
					}else{// no other options
						register_best_play ("TP","Space travel,cool thing");
					} 
					
				}

				free (sea_dest);
			}
		
		} else {
			
			// go to the saftest on-land location that the hunter cant reach
			go_to (decide_best_location (dv, hunters, dest, n_dest));
		}	
		
		free (dest);
		free (my_land_dest);
		drop_hunter_locations (hunter_locations);
		
	}
		

	free (possible_dest);

	//register_best_play ("CD", "U cant touch me\n\n\n");
}


// find the available location that i can double back to 
// in the trail
int trail_available (DraculaView dv, location_t* hunter_locations[4], size_t n_hunter[4]) 
{
	location_t trail[TRAIL_SIZE];
	size_t x = 0;
	location_t* dest = dv_get_old_dests (dv, &x, true, true);
	
	
	dv_get_trail (dv, PLAYER_DRACULA, trail);

	for (int n = 0; n <5; n++) {
		if (hunter_reachable (hunter_locations, n_hunter, trail[n]) == false
		  && valid_location_p (trail[n]) && Exist (dest, &x, trail[n])) {
			//puts(location_get_name(trail[n]));
			free (dest);
			return (n+1);
		}
	
	}
	free (dest);
	return 0;
}
// get all the locations that the hunter can reach from the current location
void get_all_hunter_reachable (location_t* hunter_locations[4], size_t n_hunter[4], DraculaView dv) 
{
	for (size_t n =0;n<4;n++) {
		hunter_locations[n]= dv_get_dests_player (dv,&(n_hunter[n]), get_player_byNo(n), true, true, false);
	}
}










// play the first move of dracula
void make_first_move (DraculaView dv)
{
	location_t hunter_locations[4];
	// get all the location that the hunter can go to
	for(size_t n =0; n <4; n++) {
		hunter_locations[n]= dv_get_location (dv,get_player_byNo(n));
	}
	location_t* dest = NULL;
	size_t n_dest = 0;
	// select from all the land location except hospital
	for (size_t n = 0 ; n < NUM_MAP_LOCATIONS;n++) {
		if(land_p (PLACES[n].id) 
			&& PLACES[n].id!= CASTLE_DRACULA 
			&& PLACES[n].id!= HOSPITAL_LOCATION)
			dest = add_connection(dest, &n_dest, PLACES[n].id);
	}

	go_to (decide_best_location(dv, hunter_locations, dest, n_dest));
	assert (dest != NULL);
	free (dest);

}
// go to that location
void go_to (location_t des) 
{
	char tmp[3];
	strcpy (tmp,location_get_abbrev(des));
	register_best_play (tmp, "U cant touch me\n\n\n");
}
// drop the location pointers
void drop_hunter_locations (location_t* hunter_locations[4]) 
{
	for (size_t n =0; n<4 ; n++){
		free (hunter_locations[n]);
	}
}


// find the safest number in the given array(smallest)
size_t find_safest (int* safe_place,size_t size) 
{
	assert (size >=1);
	if (size == 1) {
		return 0;
	}
	size_t index = 0;
	int safest = safe_place[0];
	
	for (size_t n = 1; n<size;n++) {
		if (safe_place[n]<safest && safe_place[n] != UNREACHABLE) {
			index = n;
			safest =safe_place[n];
		}
	}
	return index;
}

// whether the given location is reachable by the hunter?
bool hunter_reachable (location_t* hunter_locations[4],size_t n_hunter[4],location_t dest) 
{	
	for (size_t n =0; n<4;n++){
		for (size_t i =0; i< n_hunter[n];i++) {
			if (hunter_locations[n][i] == dest) return true;
		}
	}
	return false;
}

// most important function
location_t decide_best_location (DraculaView dv,
			location_t hunter_location[4],
			location_t* possible_dest, 
			size_t num) 
{

	int* distance_index = malloc(sizeof(int)* num);// initialise the possible distance

	int distance_tree[4][NUM_MAP_LOCATIONS]={{0},{0},{0}};
	// get the minimal spanning tree from hunter's current location
	for (int n = 0; n < 4;n++){
		dv_find_shortest_path (hunter_location[n], dv, n, dv_get_round (dv)+1, distance_tree[n]);
	}
	// for all the location that the dracula can reach/

	// calculate the sum of distance which all the hunters
	// need to travel in order to get that location
	for (size_t n = 0; n < num; n++) {
		distance_index[n] = 0;
		for(size_t i = 0; i < 4; i++){
			distance_index[n] += distance_tree[i][possible_dest[n]];
		}
	}
	// pick the location with the largest distance sunm
	int max = distance_index[0];
	size_t max_index= 0;
	for (size_t n =1; n<num;n++) {
		if (distance_index[n]>max) {
			max = distance_index[n];
			max_index = n;
		}
	}
	free (distance_index);
	return possible_dest[max_index];

}


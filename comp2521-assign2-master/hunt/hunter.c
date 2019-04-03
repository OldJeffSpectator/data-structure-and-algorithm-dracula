////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include "game.h"
#include "hunter.h"
#include "hunter_view.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "places.h"

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "queue.h"
#include "map.h"
#include "helper.h"
void make_first_move (HunterView hv);
void play_random_move (HunterView hv); 
int get_player_number (enum player p);


void make_move (HunterView hv);

void decide_hunter_move (HunterView hv)
{
	
	// FIRST ROUND PLAY
	if(hv_get_round(hv) == 0) {
		make_first_move(hv);
		return;
	}

	make_move(hv);

}


// rule:
// for each player, he will read the last message sent by the previous player
// and decide a move according to the message
// the message will start with a number...
// if it starts with 0: no previous target, free to change or make a random move
// if it starts with 1: followed by a new target
// if it starts with 2: do a research 
void make_move(HunterView hv)
{
	// get the message from the last player
	char* str = hv_get_last_message (hv);
	assert (str != NULL);
	// jusr in case, normally this case wont exist
	if (str[0] == '\0') {
		str = realloc (str,2*sizeof(char));
		str[0]='0';
		str[1]='\0';
	}

	// get the current location of dracula
	location_t target = hv_get_location (hv,PLAYER_DRACULA);
	enum player current_player = hv_get_player (hv);
	location_t my_location = hv_get_location (hv,current_player);


	// if invalid
	// get the 'possible location' based on last game
	if (valid_location_p (target)== false) {
		if(valid_location_p (hv_get_dracula_possible (hv))) {
			target = hv_get_dracula_possible(hv);
			
		}
	}
	
	// else, get the possible location from the trail if valid
	if (valid_location_p (target) == false) {
		location_t trail[TRAIL_SIZE];
		hv_get_trail(hv,PLAYER_DRACULA,trail);

		for (int n=0; n<TRAIL_SIZE;n++) {
			if (valid_location_p(trail[n])) {
				target = trail[n];
				break;
			}
		}


	}

	// else previous target
	// from the last player
	if (valid_location_p (target) == false) {
		if(str[0]== '1') {
			target = location_find_by_abbrev(&(str[1]));
		}
	}

	bool valid_current = valid_location_p (target);
	
	// if hunter is low on health 
	// do a hide
	if (hv_get_health (hv,current_player) <= LIFE_LOSS_DRACULA_ENCOUNTER){
		//keep the original message
		char message[4]={'1','\0','\0','\0'};
		char abb[3];

		if (valid_location_p(target)== true) {
			strcpy (&(message[1]),location_get_abbrev(target));
		} else if (str[0] !='0') {
			strcpy (message, str);
		} else {
			message[0] = '0';  
		}
		// do a hdie
		strcpy (abb,location_get_abbrev(my_location));
		
		register_best_play (abb,message);

	// hunter is in the current location of target, and its not dracula
	} else if (target == my_location && target != hv_get_location (hv,PLAYER_DRACULA)) {
		play_random_move (hv);

	//make a move to same location
	} else if (target == my_location && target == hv_get_location (hv,PLAYER_DRACULA)) {
		
		char message[4]={'1','\0','\0','\0'};
		char abb[3];
		strcpy (abb,location_get_abbrev (target));
		strcpy (&(message[1]),location_get_abbrev (target));
		register_best_play (abb,message);

	} else if (valid_current) {
		// no previous target,current target is valid
		// set the target up
		// find the shrotest path to the dest
		location_t t = hv_find_shortest_path (my_location,
											target,hv,hv_get_round(hv),
											get_player_number(current_player));
		
		char message[4] = {'1','\0','\0','\0'};
		char abb[3];

		strcpy (abb,location_get_abbrev (t));
		strcpy (&(message[1]),location_get_abbrev (target));
		register_best_play (abb,message);


	} else if (str[0]=='0' && !valid_current) {
		// no previous target, current target is invalid
		// make random move		
		
		play_random_move(hv);
	} else {
		// have previous target,the current one is invalid
		// go for it!

		location_t t = location_find_by_abbrev (&(str[1]));
		location_t next = hv_find_shortest_path (my_location,
											t,hv,hv_get_round (hv),
											get_player_number (current_player));
		
		if (next == my_location&& next != hv_get_location(hv,PLAYER_DRACULA)) {
			play_random_move(hv);
		} else {
			char abb[3];
			strcpy (abb,location_get_abbrev(next));
		
			register_best_play(abb,str);
		}
		
	}
	free (str);
}



// play the first turn. The cities are spread apart to cover alot of ground. 
void make_first_move (HunterView hv) 
{
	if (hv_get_player(hv)== PLAYER_LORD_GODALMING) {
		char tmp[3];
		strcpy (tmp,location_get_abbrev(MADRID));
		register_best_play (tmp, "0");

	} else if (hv_get_player(hv)== PLAYER_DR_SEWARD) {
		char tmp[3];
		strcpy (tmp,location_get_abbrev(PARIS));
		register_best_play (tmp, "0");

	} else if (hv_get_player(hv)== PLAYER_VAN_HELSING) {
		char tmp[3];
		strcpy (tmp,location_get_abbrev(BUCHAREST));
		register_best_play (tmp, "0");

	} else {
		char tmp[3];
		strcpy (tmp,location_get_abbrev(SZEGED));
		register_best_play (tmp, "0");

	}
	return;
}

// play a random valid move 
void play_random_move (HunterView hv) 
{
	size_t n_edges; 
	location_t *edges = hv_get_dests_player (hv, &n_edges, hv_get_player(hv), true, true, true); 

	


	assert(n_edges != 0);
	size_t ran = ((size_t)rand())% n_edges;
	if(n_edges == 1){

		ran =0;
	}else if(n_edges == 2){

		ran =1;

	}else{
		location_t trail[TRAIL_SIZE];
		size_t trail_n = TRAIL_SIZE;
		hv_get_trail(hv,hv_get_player(hv),trail);
		location_t* new_edges = NULL;
		size_t new_n=0;
		ran =0;
		// try not to go back to the trail
		// filter out the old messages
		while(new_edges == NULL && trail_n >=1){

			for(size_t n =0;n<n_edges;n++){
				if(Exist(trail,&trail_n,edges[n])== false){
					new_edges = add_connection(new_edges,&new_n,edges[n]);
				}
			}
			trail_n--;
		}
		// update possible dest
		if( new_edges != NULL){
			free(edges);
			edges = new_edges;
			n_edges = new_n;
		}
		// choose randomly

		ran = (((size_t)rand())% (n_edges)) ;

		
	}

	location_t curr = edges[ran];
	free (edges);
	char tmp[3];
	strcpy (tmp,location_get_abbrev(curr));
	register_best_play (tmp, "0"); 
	
}





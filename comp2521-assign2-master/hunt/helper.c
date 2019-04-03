#include "places.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "helper.h"
#include "game.h"
#include "assert.h"
location_t* add_connection(location_t* dest,size_t *n_location,location_t n){
	
	if(*n_location == 0){
		dest= malloc(sizeof(location_t));
		
	}else{
		dest = realloc(dest,(sizeof(location_t))*((*n_location)+1));
	
	}
	dest[(*n_location)] = n;
	(*n_location)++;
	return dest;


}
// HELPER function
bool Exist(location_t* dest,size_t *n_location,location_t new){
	for( size_t n=0; n<*n_location;n++){
		if(dest[n]== new)return true;
	}
	return false;
}


// get the number of player according to the player
int get_player_number (enum player p)
{
	
	switch(p){
		case PLAYER_LORD_GODALMING:return 0;
      	case PLAYER_DR_SEWARD:return 1;
      	case PLAYER_VAN_HELSING:return 2;
      	case PLAYER_MINA_HARKER:return 3;
		case PLAYER_DRACULA: return 4;
	}

	return -1;

}

int increment_rail_index(int rail_index){
	return ((rail_index +1)%4);
}

enum player get_player_byNo (size_t num)
{
	assert (num>=0 && num<=4);
	switch(num){
		case 0: return PLAYER_LORD_GODALMING;
		case 1: return PLAYER_DR_SEWARD;
		case 2: return PLAYER_VAN_HELSING;
		case 3: return PLAYER_MINA_HARKER;
		default: return PLAYER_DRACULA; 
	}
	



}


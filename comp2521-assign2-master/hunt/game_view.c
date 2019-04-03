////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// game_view.c: GameView ADT implementation
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
#include "queue_location.h"
#include "helper.h"
#define DRACULA 4
#define RECORD_POINT 30
#define NO_RESRICT -1
#define VAMPIRE UNKNOWN_LOCATION
//#include "map.h" ... if you decide to use the Map ADT

typedef struct game_view {
   size_t current_player;
   int score;
	size_t current_turn;
	round_t current_round;

	location_t previous_location[NUM_PLAYERS-1];
	location_t current_location[NUM_PLAYERS];
	location_t locations[NUM_PLAYERS][TRAIL_SIZE];// last six position
	int num_location[NUM_PLAYERS];
	Queue_location dracula_locations;// the actual last 6 position of dracula
									// the 'trail'
	int health[5];// health of players


	//char* message;
} game_view;




// functions prototypes
// for connections


bool is_sentinel_edge (connection x);

int trap_counter(char* tmp);
//else
void calculator(game_view* gv, char *past_plays);
void record_location(game_view* gv,int playerNo);
void vampire_mature(game_view* gv);
void encounter_check(bool encounter[3],char* past_plays);
location_t dracula_location(game_view* gv);
void trap_vanish(Queue_location trap_q);
void trap_placed( Queue_location trap_q, location_t new_trap);
void send_to_hospital(game_view* gv,int hunter_no);
void dracula_encounter(game_view* gv,int hunter_no);
void trap_encounter(game_view* gv,int hunter_no,Queue_location q,int x);



game_view *gv_new (char *past_plays, player_message messages[])
{
	assert( messages != NULL);
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	game_view *gv = malloc (sizeof *gv);
	if (gv == NULL) err (EX_OSERR, "couldn't allocate GameView");

	// initial the blood point && health && location
	for(int n=0; n<NUM_PLAYERS; n++){
		for(int i=0; i< TRAIL_SIZE; i++){
			gv->locations[n][i] = UNKNOWN_LOCATION;
		}
		if(n==DRACULA) gv->health[n]= GAME_START_BLOOD_POINTS	;
		else{
			gv->previous_location[n] = UNKNOWN_LOCATION;
			gv->health[n] = GAME_START_HUNTER_LIFE_POINTS;
		}
		gv->num_location[n] =0;
		gv->current_location[n] = UNKNOWN_LOCATION;

	}
	gv->dracula_locations = queue_location_new();
	//gv->message = strlen( messages+1);
	//strcpy(gv->message,messages);
	

	size_t len = strlen(past_plays)+1;
	gv->current_round =len/(8*NUM_PLAYERS);
	
	
	//initiate turn number
   
	gv->current_turn = len/8;
	// initaite score
	
	gv->score = GAME_START_SCORE;
	
	// calculate current player
	gv->current_player = (len/8)%5;


	calculator(gv,past_plays);
	
	return gv;
}

//////////////////////////////////////////////////////////////////////////////




void calculator(game_view* gv, char *past_plays){
	unsigned int turn =0;
	
   unsigned int start = 0 ;
	if(RECORD_POINT <= gv->current_turn ){
		start = gv->current_turn -RECORD_POINT;
	}


   Queue_location trap_q = queue_location_new();
   unsigned int len = strlen(past_plays);
	for(unsigned int n=0; n<len; n+=8,turn++){
      // get the current playerNo 
      int playerNo = turn %5;
      // get the move of the last play
      char move[3]= {past_plays[n+1],past_plays[n+2],'\0'}; 
      // update current posoition
		
		
		if(playerNo == DRACULA){
			location_t tmp = location_find_by_abbrev (move);
			gv->current_location[playerNo] = tmp;
			if(tmp<=70  &&  0<=tmp){
				gv->current_location[playerNo] = tmp;
				if(queue_location_size(gv->dracula_locations)== 6)
				queue_location_de(gv->dracula_locations);
				queue_location_en(gv->dracula_locations,
								gv->current_location[playerNo]);
			}else{
				// DOUBLE 	back
				if( gv->current_location[playerNo] == UNKNOWN_LOCATION
					&& move[0] == 'D'){
					
						
					int pos = atoi(&(move[1]));
					switch(pos){
						case 1:gv->current_location[playerNo] =
									DOUBLE_BACK_1;break;
						case 2:gv->current_location[playerNo] =
									DOUBLE_BACK_2;break;
						case 3:gv->current_location[playerNo] = 
									DOUBLE_BACK_3;break;
						case 4:gv->current_location[playerNo] =
									DOUBLE_BACK_4;break;
						case 5:gv->current_location[playerNo] = 
									DOUBLE_BACK_5;break;
					}
				
					assert(queue_location_size(gv->dracula_locations) != 0);
				
					assert(pos>=1 && pos <= 5);
					Node_location curr = gv->dracula_locations->head;


					for (int a =queue_location_size(gv->dracula_locations);
								a>=1&& curr!= NULL;
								a--, curr = curr->next){
						if( a == pos){
							if(queue_location_size(gv->dracula_locations)== 6)
							queue_location_de(gv->dracula_locations);
							queue_location_en(gv->dracula_locations,curr->item);
							break;
						}
					}



				} else if (strcmp(move,"HI")== 0){
					// Hide 
					assert(queue_location_size(gv->dracula_locations) != 0);
					gv->current_location[playerNo]  = HIDE;
					if(queue_location_size(gv->dracula_locations)== 6)
						queue_location_de(gv->dracula_locations);
					queue_location_en(gv->dracula_locations,
								gv->dracula_locations->tail->item);
				} else if(strcmp(move,"C?")==0){
					//city unkown
					gv->current_location[playerNo] = CITY_UNKNOWN;
					if(queue_location_size(gv->dracula_locations)== 6)
						queue_location_de(gv->dracula_locations);
					queue_location_en(gv->dracula_locations,
								CITY_UNKNOWN);
				} else if(strcmp(move,"S?")==0) {
					// sea unkown
					gv->current_location[playerNo] = SEA_UNKNOWN;
					if(queue_location_size(gv->dracula_locations)== 6)
						queue_location_de(gv->dracula_locations);
					queue_location_en(gv->dracula_locations,
								SEA_UNKNOWN);
				} else if(strcmp(move,"TP")==0) {
					// teleport
					gv->current_location[playerNo] = TELEPORT;
					if(queue_location_size(gv->dracula_locations)== 6)
						queue_location_de(gv->dracula_locations);
					queue_location_en(gv->dracula_locations,
								CASTLE_DRACULA);
				} else{
					if(queue_location_size(gv->dracula_locations)== 6)
						queue_location_de(gv->dracula_locations);
					queue_location_en(gv->dracula_locations,
								gv->current_location[playerNo]);
				}
			}	
			
		}else{
			if( gv->health[playerNo] <= 0 && gv->current_location[playerNo] == HOSPITAL_LOCATION){
				
				gv->health[playerNo] = GAME_START_HUNTER_LIFE_POINTS;
			}
			gv->previous_location[playerNo] = gv->current_location[playerNo];
			gv->current_location[playerNo] = location_find_by_abbrev (move);
			
		}
		
		
      
		//printf("%u\t%u\n",gv->current_turn, start);
		//printf("Double back is %s\n",move,location_get_name(location_find_by_abbrev("HI")));
      //update the location records
      // for history function
      if(turn >= start){
         //record the location   
                  
         record_location(gv,playerNo);
      }
      
      // Dracula's turn
      if(past_plays[n] == 'D'){
			// record the location of dracula


			// losing score for dracula's turn
			gv->score-= SCORE_LOSS_DRACULA_TURN;
			if(past_plays[n+3]=='T'){
				trap_placed(trap_q,dracula_location(gv));
			}
			if(past_plays[n+4]=='V'){
				trap_placed(trap_q,VAMPIRE);
			}



         if(past_plays[n+5] == 'V'){
            vampire_mature(gv);
         }else if( past_plays[n+5] =='M'){
				trap_vanish(trap_q);

			}

         // lose health for be at the sea
			//printf("1\t%s\n",location_get_name(dracula_location(gv)));
			if(valid_location_p (dracula_location(gv))== true ){
				if(sea_p (dracula_location(gv))==true ){
				
					gv->health[DRACULA] -= LIFE_LOSS_SEA;
				} else {
				// known to be back to the castle
					if(dracula_location(gv)  == CASTLE_DRACULA){
						
						gv->health[DRACULA] += LIFE_GAIN_CASTLE_DRACULA;
					}
					
				
				}     
			} else if (dracula_location(gv) == SEA_UNKNOWN){

				gv->health[DRACULA] -= LIFE_LOSS_SEA;
			}  

      } else { 
         //hunter's turn
         
         bool tvd[3]= {false,false,false};
			//TODO : modify this
			char tmp[5] = {past_plays[n+3],past_plays[n+4],past_plays[n+5],past_plays[n+6],'\0'};

			encounter_check(tvd,tmp);
			// encounter all the trap first;
			if(tvd[0] == true) {
				int n_trap = trap_counter(tmp);	
				trap_encounter(gv,playerNo,trap_q,n_trap);
			}
			// then vampire
			// then dracula
			if(tvd[2] == true) {
				dracula_encounter(gv,playerNo);
				
				
			}
         
         
         
         
			
			
			
			

			
         // the end of ACTION phase
			// regain the health by resting
         if(gv->previous_location[playerNo]== gv->current_location[playerNo] ){
				gv->health[playerNo] += LIFE_GAIN_REST;
				if( gv->health[playerNo] > GAME_START_HUNTER_LIFE_POINTS){
					gv->health[playerNo] = GAME_START_HUNTER_LIFE_POINTS;
				}
			}
			
         
         
         
         
         
         
         
      
         
         
         
         
      
         
         
         
          
         
      }
      
   }

	queue_location_drop(trap_q);

}
int trap_counter(char* tmp){
	int count =0;
	for(int n=0; n<4;n++){
		if(tmp[n] =='T'){
			count ++;

		}

	}
	return count;


}	
void record_location(game_view* gv,int playerNo){

	gv->locations[playerNo][gv->num_location[playerNo]] =  gv->current_location[playerNo];
   gv->num_location[playerNo]++;
  
}




void vampire_mature(game_view* gv){
   gv->score -= SCORE_LOSS_VAMPIRE_MATURES;

}



void encounter_check(bool encounter[3],char* past_plays){
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


location_t dracula_location(game_view* gv){
   if(queue_location_size(gv->dracula_locations)!= 0){
		return gv->dracula_locations->tail->item;
	}
	return UNKNOWN_LOCATION;// never get here
}
void trap_vanish(Queue_location trap_q){
	//queue_location_de(trap_q);
	assert( trap_q != NULL);
	return;

}


void trap_placed( Queue_location trap_q, location_t new_trap){
	if(queue_location_size(trap_q)== 6)
				queue_location_de(trap_q);
	queue_location_en(trap_q,new_trap);
}

void send_to_hospital(game_view* gv,int hunter_no){
   // switch the location
   gv->current_location[hunter_no] = HOSPITAL_LOCATION;
   // regain health
   gv->health[hunter_no] = 0;
   // reduce the score
   gv->score -= SCORE_LOSS_HUNTER_HOSPITAL;
}






void dracula_encounter(game_view* gv,int hunter_no){
	
	
	// if hunter isnt dead
	if(gv->health[hunter_no]>0){
		
				
		gv->health[DRACULA] -= LIFE_LOSS_HUNTER_ENCOUNTER;
		gv->health[hunter_no] -= LIFE_LOSS_DRACULA_ENCOUNTER;
		if(gv->health[hunter_no]<=0){
         send_to_hospital(gv,hunter_no);
      }

	}
      


	

}





void trap_encounter(game_view* gv,int hunter_no,Queue_location q,int x){
	assert(x!= 0);
	location_t hunter_location = gv->current_location[hunter_no];
	Node_location next = NULL;
	Node_location prev = NULL;
	int count =0;
	for(Node_location curr = q->head;count<x &&curr!= NULL&& gv->health[hunter_no]>0; curr = next){
		next = curr->next;

		//encounter a trap
		if( curr->item == hunter_location){
			count++;
			// lose health point
			gv->health[hunter_no] -= LIFE_LOSS_TRAP_ENCOUNTER;
			// remove the head

			Node_location tmp= curr;
			// update the head
			if( tmp == q->head && tmp ==q->tail){
				
				q->head = NULL;
				q->tail = NULL;
				q->num =0;
				free(curr);
				break;
				
			} else if( tmp ==q->head && tmp !=q->tail ) {
				prev = NULL;
				q->head = q->head->next;
				q->num --;
				free(curr);
				
			}else if (tmp !=q->head && tmp ==q->tail){
				q->tail = prev;
				q->tail->next = NULL;
				q->num --;
				free(curr);
				break;
			}else{
				prev->next = curr->next;
				q->num --;
				free(curr);
			}
		}else{
			prev = curr;
		}

	}
	if( gv->health[hunter_no]<=0){
		send_to_hospital(gv,hunter_no);

	}


}




//////////////////////////////////////////////////////////////////////////////

void gv_drop (game_view *gv)
{
	queue_location_drop(gv->dracula_locations);
	//free(gv->message);
	free (gv);
}

round_t gv_get_round (game_view *gv)
{
	return gv->current_round;
}

enum player gv_get_player (game_view *gv)
{
	//printf("%d\n\n",gv->current_player);
	switch(gv->current_player){
      case 0:return PLAYER_LORD_GODALMING;
      case 1:return PLAYER_DR_SEWARD;
      case 2:return PLAYER_VAN_HELSING;
      case 3:return PLAYER_MINA_HARKER;
      case DRACULA:return PLAYER_DRACULA;
   }
   
	return PLAYER_DRACULA;
}

int gv_get_score (game_view *gv)
{
	return gv->score;
	
}

int gv_get_health (game_view *gv, enum player player)
{
	int playerNo = -1;
	switch(player){
      case PLAYER_LORD_GODALMING:playerNo =0 ;break;
      case PLAYER_DR_SEWARD:playerNo =1 ;break;
      case PLAYER_VAN_HELSING:playerNo =2 ;break;
      case PLAYER_MINA_HARKER:playerNo =3 ;break;
      case PLAYER_DRACULA:playerNo =DRACULA ;break;
   }
	return gv->health[playerNo];
}

location_t gv_get_location (game_view *gv, enum player player)
{
	int playerNo = -1;
	switch(player){
      case PLAYER_LORD_GODALMING:playerNo =0 ;break;
      case PLAYER_DR_SEWARD:playerNo =1 ;break;
      case PLAYER_VAN_HELSING:playerNo =2 ;break;
      case PLAYER_MINA_HARKER:playerNo =3 ;break;
      case PLAYER_DRACULA:playerNo =DRACULA ;break;
   }
	
	return gv->current_location[playerNo];
}

void gv_get_history (
	game_view *gv, enum player player,
	location_t trail[TRAIL_SIZE])
{
	int playerNo = -1;
	switch(player){
      case PLAYER_LORD_GODALMING:playerNo =0 ;break;
      case PLAYER_DR_SEWARD:playerNo =1 ;break;
      case PLAYER_VAN_HELSING:playerNo =2 ;break;
      case PLAYER_MINA_HARKER:playerNo =3 ;break;
      case PLAYER_DRACULA:playerNo =DRACULA ;break;
   }
	
	int i =0;
	
	for(int n=gv->num_location[playerNo]-1;n>=0;n--,i++){
		trail[i]=   gv->locations[playerNo][n];
	}
	if( i != TRAIL_SIZE){
		for( ; i< TRAIL_SIZE; i++){
			trail[i] = UNKNOWN_LOCATION;
		}
	}

	
}


location_t *gv_get_connections (
	game_view *gv, size_t *n_locations,
	location_t from, enum player player, round_t round,
	bool road, bool rail, bool sea)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	
	// dracula cant travel by rail
	*n_locations =0;
	
	if(player == PLAYER_DRACULA) rail = false;

	location_t*	dest=NULL;
	if(valid_location_p(from) == false){
		return NULL;
	}
	dest = add_connection(dest,n_locations,from);
	assert(gv != NULL);

	for(int n =0 ; (!is_sentinel_edge(CONNECTIONS[n])); n++){
		connection conn = CONNECTIONS[n];
		if((conn.v == from) || (conn.w ==from) ){
			location_t to = 	(conn.v == from) ? (conn.w):(conn.v);
			// if the location has not be added
				bool e = Exist(dest,n_locations,to);
				
				// consider dracula's special rules
				if(player == PLAYER_DRACULA){
					if( to == HOSPITAL_LOCATION) continue;
				}


				if(road == true && conn.t == ROAD){
					if( e== false)
					dest = add_connection(dest,n_locations,to);

				}
				
				if ( sea == true && conn.t == BOAT){
					if( e== false)
					dest = add_connection(dest,n_locations,to);
				
				}
				
				if ( rail == true && conn.t == RAIL){
					int mod = (round + get_player_number (player))%4;
					if(mod == 0) continue;
					if( e== false)
					dest = add_connection(dest,n_locations,to);

					Queue_location q = queue_location_new();
					
					if(mod >= 2){
							
						// find all the city connected to to with in one step
						for(int i =0 ; (!is_sentinel_edge(CONNECTIONS[i])); i++){
							
							if((CONNECTIONS[i].v == to || CONNECTIONS[i].w == to)&&CONNECTIONS[i].t == RAIL){
								location_t tmp = CONNECTIONS[i].v == to?
														CONNECTIONS[i].w: CONNECTIONS[i].v;
								if(Exist(dest,n_locations,tmp) ==false){
									
									dest = add_connection(dest,n_locations,tmp);
									
								}
								queue_location_en(q,tmp);

							}
						}
					}
					// third level connection
					if ( mod == 3){
						while(queue_location_size(q)>0){
							location_t item = queue_location_de(q);
							for(int i =0 ; (!is_sentinel_edge(CONNECTIONS[i])); i++){
							
								if((CONNECTIONS[i].v == item || CONNECTIONS[i].w == item)&&CONNECTIONS[i].t == RAIL){
									location_t tmp = CONNECTIONS[i].v == item?
															CONNECTIONS[i].w: CONNECTIONS[i].v;
									if(Exist(dest,n_locations,tmp) ==false){
										dest = add_connection(dest,n_locations,tmp);
										
									}

								}
							}


						}


					}
					
					
					queue_location_drop(q);


				}

			
			
				

		}

	}


	

	return dest;
}

///////////////////////////////////////////////




/// Is this the magic 'sentinel' edge?
bool is_sentinel_edge (connection x)
{
	return x.v == -1 && x.w == -1 && x.t == ANY;
}



/////////////////////////////////////////


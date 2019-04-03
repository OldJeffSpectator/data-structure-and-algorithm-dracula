////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// test_game_view.c: test the GameView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_view.h"
void dracula_view (void);
void test_general(void);
void test_encouner (void);
bool seen(location_t target, location_t* trail, size_t size);
void test_trail_move (void);
void connection_check( void);

void hunter_view (void);


int main (void)
{
	do {////////////////////////////////////////////////////////////////
		puts ("Test basic empty initialisation");

		char *trail = "";
		player_message messages[] = {};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == PLAYER_LORD_GODALMING);
		assert (gv_get_round (gv) == 0);
		assert (gv_get_health (gv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert (gv_get_score (gv) == GAME_START_SCORE);
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == UNKNOWN_LOCATION);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula trail and basic functions");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DC?....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == PLAYER_LORD_GODALMING);
		assert (gv_get_round (gv) == 1);
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert (gv_get_location (gv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert (gv_get_location (gv, PLAYER_VAN_HELSING) == ZURICH);
		assert (gv_get_location (gv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert (gv_get_location (gv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for encountering Dracula and hunter history");

		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.... "
			"GGED...";
		player_message messages[] = {
			"Hello", "Rubbish",  "Stuff", "", "Mwahahah",
			"Aha!"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_location (gv, PLAYER_DRACULA) == GENEVA);
		assert (gv_get_health (gv, PLAYER_LORD_GODALMING) == 5);
		assert (gv_get_health (gv, PLAYER_DRACULA) == 30);
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == GENEVA);

		location_t history[TRAIL_SIZE];
		gv_get_history (gv, PLAYER_DRACULA, history);
		assert (history[0] == GENEVA);
		assert (history[1] == UNKNOWN_LOCATION);

		gv_get_history (gv, PLAYER_LORD_GODALMING, history);
		assert (history[0] == GENEVA);
		assert (history[1] == STRASBOURG);
		assert (history[2] == UNKNOWN_LOCATION);

		gv_get_history (gv, PLAYER_DR_SEWARD, history);
		assert (history[0] == ATLANTIC_OCEAN);
		assert (history[1] == UNKNOWN_LOCATION);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula doubling back at sea, "
			  "and losing blood points (Hunter View)");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DS?.... "
			"GST.... SST.... HST.... MST.... DD1....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == 0);
		assert (gv_get_health (gv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS - (2 * LIFE_LOSS_SEA));
		assert (gv_get_location (gv, PLAYER_DRACULA) == DOUBLE_BACK_1);

		location_t history[TRAIL_SIZE];
		gv_get_history (gv, PLAYER_DRACULA, history);
		assert (history[0] == DOUBLE_BACK_1);
		assert (history[1] == SEA_UNKNOWN);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula doubling back at sea, "
			  "and losing blood points (Dracula View)");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DEC.... "
			"GST.... SST.... HST.... MST.... DD1....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == 0);
		assert (gv_get_health (gv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS - (2 * LIFE_LOSS_SEA));
		assert (gv_get_location (gv, PLAYER_DRACULA) == DOUBLE_BACK_1);

		location_t history[TRAIL_SIZE];
		gv_get_history (gv, PLAYER_DRACULA, history);
		assert (history[0] == DOUBLE_BACK_1);
		assert (history[1] == ENGLISH_CHANNEL);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for connections");
		char *trail = "";
		player_message messages[] = {};
		GameView gv = gv_new (trail, messages);

		do {
			puts ("Checking Galatz road connections");
			size_t n_edges;
			location_t *edges = gv_get_connections (
				gv, &n_edges,
				GALATZ, PLAYER_LORD_GODALMING, 0,
				true, false, false
			);

			bool seen[NUM_MAP_LOCATIONS] = {false};
			for (size_t i = 0; i < n_edges; i++)
				seen[edges[i]] = true;

			assert (n_edges == 5);
			assert (seen[GALATZ]);
			assert (seen[CONSTANTA]);
			assert (seen[BUCHAREST]);
			assert (seen[KLAUSENBURG]);
			assert (seen[CASTLE_DRACULA]);

			free (edges);
		} while (0);

		do {
			puts ("Checking Ionian Sea sea connections");
			size_t n_edges;
			location_t *edges = gv_get_connections (
				gv, &n_edges,
				IONIAN_SEA, PLAYER_LORD_GODALMING, 0,
				false, false, true
			);

			bool seen[NUM_MAP_LOCATIONS] = {false};
			for (size_t i = 0; i < n_edges; i++)
				seen[edges[i]] = true;

			assert (n_edges == 7);
			assert (seen[IONIAN_SEA]);
			assert (seen[BLACK_SEA]);
			assert (seen[ADRIATIC_SEA]);
			assert (seen[TYRRHENIAN_SEA]);
			assert (seen[ATHENS]);
			assert (seen[VALONA]);
			assert (seen[SALONICA]);

			free (edges);
		} while (0);

		do {
			puts ("Checking Athens rail connections (none)");
			size_t n_edges;
			location_t *edges = gv_get_connections (
				gv, &n_edges,
				ATHENS, PLAYER_LORD_GODALMING, 0,
				false, true, false
			);

			assert (n_edges == 1);
			assert (edges[0] == ATHENS);

			free (edges);
		} while (0);

		puts ("passed");
		gv_drop (gv);
	} while (0);
	dracula_view ();
	hunter_view ();
	return EXIT_SUCCESS;
}









void dracula_view (void)
{
	test_general();
	test_encouner ();
	test_trail_move ();
	connection_check();
}





void test_general(void){
		puts("test for round&health&score functions");
	{

		puts ("\tTest for dracula lose point for be at the sea");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DNS.... " 
			"GST.... SAO.... HZU.... MBB.... DD1.... "
			"GST.... SAO.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_round (gv) == 2);
		
		assert (	gv_get_health (gv, PLAYER_DRACULA) == 
					GAME_START_BLOOD_POINTS- LIFE_LOSS_SEA*2);
					// lose blood twice for being at the sea
		assert (	gv_get_health (gv, PLAYER_LORD_GODALMING) == 
					GAME_START_HUNTER_LIFE_POINTS);
					// cant exceed his owm limit
		
		assert( gv_get_score(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN *2);
		

		assert(gv_get_location(gv,PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(gv_get_location(gv,PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(gv_get_location(gv,PLAYER_VAN_HELSING) == ZURICH);
		assert(gv_get_location(gv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(gv_get_location(gv,PLAYER_DRACULA) == DOUBLE_BACK_1);
		

		puts ("passed");
		gv_drop (gv);

	}
	/*
	{
		puts ("\tTest for dracula recover himself by going to calstle dracula after losing blood");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DNS.... " 
			"GST.... SAO.... HZU.... MBB.... DNS.... "
			"GST.... SAO.... HZU.... MBB.... DCD.V.. "
			"GST.... SAO.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_round (gv) == 3);
		
		assert (gv_get_health(gv, PLAYER_MINA_HARKER) == GAME_START_HUNTER_LIFE_POINTS);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS- LIFE_LOSS_SEA*2 + 
																									LIFE_GAIN_CASTLE_DRACULA);
		assert (gv_get_score(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN *3);


		assert(gv_get_location(gv,PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(gv_get_location(gv,PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(gv_get_location(gv,PLAYER_VAN_HELSING) == ZURICH);
		assert(gv_get_location(gv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(gv_get_location(gv,PLAYER_DRACULA) == CASTLE_DRACULA);

		puts ("passed");
		gv_drop (gv);


	}	*/
	{
		puts ("\tTest for dracula recover himself by going to calstle dracula without losing blood");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DCD.V.. "
			"GST.... SAO.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_round (gv) == 1);
		
		assert (gv_get_health(gv, PLAYER_MINA_HARKER) == GAME_START_HUNTER_LIFE_POINTS);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS+ 
																							LIFE_GAIN_CASTLE_DRACULA);
		assert (gv_get_score(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN );


		assert(gv_get_location(gv,PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(gv_get_location(gv,PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(gv_get_location(gv,PLAYER_VAN_HELSING) == ZURICH);
		assert(gv_get_location(gv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(gv_get_location(gv,PLAYER_DRACULA) == CASTLE_DRACULA);

		puts ("passed");
		gv_drop (gv);

	}
	// TEST FOR ENCOUNTER
	{
		puts ("\tTest for dracula encounter ONE hunter on the land");

		char *trail =
			"GPR.... SBD.... HZU.... MBB.... DVI.V.. "
			"GVIVD.. SBD.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_round (gv) == 1);
		
		assert (gv_get_health(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS
																		-LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS- 
																							LIFE_LOSS_HUNTER_ENCOUNTER);
		assert (gv_get_score(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN );

		assert(gv_get_location(gv,PLAYER_LORD_GODALMING) == VIENNA);
		assert(gv_get_location(gv,PLAYER_DR_SEWARD) == BUDAPEST);
		assert(gv_get_location(gv,PLAYER_VAN_HELSING) == ZURICH);
		assert(gv_get_location(gv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(gv_get_location(gv,PLAYER_DRACULA) == VIENNA);

		puts ("passed");
		gv_drop (gv);

	}
	{
		
		puts ("\tTest for dracula encounter 2 hunter on the land");

		char *trail =
			"GPR.... SBD.... HZU.... MBB.... DVI.V.. "
			"GVIVD.. SVID... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_round (gv) == 1);
		
		assert (gv_get_health(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS
																		-LIFE_LOSS_DRACULA_ENCOUNTER);

		assert (gv_get_health(gv,PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS 
																		-LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS- 
																		2*LIFE_LOSS_HUNTER_ENCOUNTER);
		assert (gv_get_score(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN );


		assert(gv_get_location(gv,PLAYER_LORD_GODALMING) == VIENNA);
		assert(gv_get_location(gv,PLAYER_DR_SEWARD) == VIENNA);
		assert(gv_get_location(gv,PLAYER_VAN_HELSING) == ZURICH);
		assert(gv_get_location(gv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(gv_get_location(gv,PLAYER_DRACULA) == VIENNA);

		puts ("passed");
		gv_drop (gv);

	}

	{
		puts ("\tTest for dracula encounter hunter on the sea");

		char *trail =
			"GED.... SBD.... HZU.... MBB.... DNS.... "
			"GNS.... SBD.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_round (gv) == 1);
		// does not affect anything
		assert (gv_get_health(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS- LIFE_LOSS_SEA);
		assert (gv_get_score(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN );



		assert(gv_get_location(gv,PLAYER_LORD_GODALMING) == NORTH_SEA);
		assert(gv_get_location(gv,PLAYER_DR_SEWARD) == BUDAPEST);
		assert(gv_get_location(gv,PLAYER_VAN_HELSING) == ZURICH);
		assert(gv_get_location(gv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(gv_get_location(gv,PLAYER_DRACULA) == NORTH_SEA);

		puts ("passed");
		gv_drop (gv);
	}
	{
		puts("\tTest for dracula encounter hunter after a hide move");
		char* trail =
			"GPR.... SBD.... HZU.... MBB.... DVI.V.. "
			"GPR.... SBD.... HZU.... MBB.... DHIT... "
			"GVITVD. SBD.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);


		assert (gv_get_round (gv) == 2);
		assert (gv_get_health(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS
																			-LIFE_LOSS_TRAP_ENCOUNTER
																			-LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS- LIFE_LOSS_HUNTER_ENCOUNTER);
		assert (gv_get_score(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN*2 );

		assert(gv_get_location(gv,PLAYER_LORD_GODALMING) == VIENNA);
		assert(gv_get_location(gv,PLAYER_DR_SEWARD) == BUDAPEST);
		assert(gv_get_location(gv,PLAYER_VAN_HELSING) == ZURICH);
		assert(gv_get_location(gv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(gv_get_location(gv,PLAYER_DRACULA) == HIDE);


		puts ("passed");
		gv_drop (gv);
	}
	{
		puts("\tTest for dracula encounter hunter after a double back move");
		char* trail =
			"GPR.... SBD.... HZU.... MBB.... DVI.V.. "
			"GPR.... SBD.... HZU.... MBB.... DD1T... "
			"GVITVD. SBD.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_round (gv) == 2);
		assert (gv_get_health(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS
																			-LIFE_LOSS_TRAP_ENCOUNTER
																			-LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS- LIFE_LOSS_HUNTER_ENCOUNTER);
		assert (gv_get_score(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN*2 );

		assert(gv_get_location(gv,PLAYER_LORD_GODALMING) == VIENNA);
		assert(gv_get_location(gv,PLAYER_DR_SEWARD) == BUDAPEST);
		assert(gv_get_location(gv,PLAYER_VAN_HELSING) == ZURICH);
		assert(gv_get_location(gv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(gv_get_location(gv,PLAYER_DRACULA) == DOUBLE_BACK_1);


		puts ("passed");
		gv_drop (gv);
	}
	{
		puts("\tTest for dracula encounter hunter after a teleport move");
		char* trail =
			"GGA.... SBD.... HZU.... MBB.... DVI.V.. "
			"GGA.... SBD.... HZU.... MBB.... DTPT... "
			"GCDT.D. SBD.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_round (gv) == 2);
		assert (gv_get_health(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS
																			-LIFE_LOSS_TRAP_ENCOUNTER
																			-LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS+LIFE_GAIN_CASTLE_DRACULA
																			- LIFE_LOSS_HUNTER_ENCOUNTER);
		assert (gv_get_score(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN*2 );

		assert(gv_get_location(gv,PLAYER_LORD_GODALMING) == CASTLE_DRACULA);
		assert(gv_get_location(gv,PLAYER_DR_SEWARD) == BUDAPEST);
		assert(gv_get_location(gv,PLAYER_VAN_HELSING) == ZURICH);
		assert(gv_get_location(gv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(gv_get_location(gv,PLAYER_DRACULA) == TELEPORT);


		puts ("passed");
		gv_drop (gv);
	}
	{
		puts("\tTest for HUNTER vanish a vampire");
		char* trail =
			"GPR.... SBD.... HZU.... MBB.... DVI.V.. "
			"GPR.... SBD.... HZU.... MBB.... DTPT... "
			"GVIV... SBD.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_round (gv) == 2);
		assert (gv_get_health(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS
																			);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS +LIFE_GAIN_CASTLE_DRACULA);
		assert (gv_get_score(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN*2 );

		assert(gv_get_location(gv,PLAYER_LORD_GODALMING) == VIENNA);
		assert(gv_get_location(gv,PLAYER_DR_SEWARD) == BUDAPEST);
		assert(gv_get_location(gv,PLAYER_VAN_HELSING) == ZURICH);
		assert(gv_get_location(gv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(gv_get_location(gv,PLAYER_DRACULA) == TELEPORT);


		puts ("passed");
		gv_drop (gv);
	}
	{
		puts("\tTest for hunter encounter three traps at the same time");
		char* trail =
			"GPR.... SBD.... HZU.... MBB.... DVI.V.. "
			"GPR.... SBD.... HZU.... MBB.... DZAT... "
			"GPR.... SBD.... HZU.... MBB.... DHIT... "
			"GPR.... SBD.... HZU.... MBB.... DD1T... "
			"GPR.... SBD.... HZU.... MBB.... DPRT... "
			"GZATTT. SBD.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_round (gv) == 5);
		assert (gv_get_health(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS
																			-LIFE_LOSS_TRAP_ENCOUNTER*3
																			);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert (gv_get_score(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN*5 );



		assert(gv_get_location(gv,PLAYER_LORD_GODALMING) == ZAGREB);
		assert(gv_get_location(gv,PLAYER_DR_SEWARD) == BUDAPEST);
		assert(gv_get_location(gv,PLAYER_VAN_HELSING) == ZURICH);
		assert(gv_get_location(gv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(gv_get_location(gv,PLAYER_DRACULA) == PRAGUE);

		puts ("passed");
		gv_drop (gv);
	}

	{
		puts("\tTest for hunter encounter first traps\n"
					"\tbefore encounter the second\n"
					"\t");
		char* trail =

			"GPR.... SPR.... HZU.... MBB.... DVI.V.. "
			"GVIVD.. SVID... HZU.... MBB.... DPRT... "
			"GPRTD.. SPRD... HZU.... MBB.... DVIT... "
			"GJM.... SVIT... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_round (gv) == 3);
		assert (gv_get_health(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
		// be sent the hospital
		assert (gv_get_health(gv, PLAYER_DR_SEWARD) <= 0);// die in that turn
		// encounter hunters for four times
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS- 4*LIFE_LOSS_HUNTER_ENCOUNTER);
		assert (gv_get_score(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN*3-SCORE_LOSS_HUNTER_HOSPITAL*2 );



		assert(gv_get_location(gv,PLAYER_LORD_GODALMING) == HOSPITAL_LOCATION);

		printf("%d %s\n",gv_get_health(gv, PLAYER_DR_SEWARD),location_get_name(gv_get_location(gv,PLAYER_DR_SEWARD)));
		assert(gv_get_location(gv,PLAYER_DR_SEWARD) == HOSPITAL_LOCATION);
		assert(gv_get_location(gv,PLAYER_VAN_HELSING) == ZURICH);
		assert(gv_get_location(gv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(gv_get_location(gv,PLAYER_DRACULA) == VIENNA);


		puts ("passed");
		gv_drop (gv);
	}

	{
		puts("\tTest for hunter go to the hospital intentionally to hospital");
		char* trail =
			"GVI.... SBD.... HZU.... MBB.... DZA.V.. "
			"GZAVD.. SBD.... HZU.... MBB.... DHIT... "
			"GJM.... SBD.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_round (gv) == 2);
		assert (gv_get_health(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS-LIFE_LOSS_DRACULA_ENCOUNTER
																			);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS -LIFE_LOSS_HUNTER_ENCOUNTER);
		assert (gv_get_score(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN*2 );

		assert(gv_get_location(gv,PLAYER_LORD_GODALMING) == HOSPITAL_LOCATION);
		assert(gv_get_location(gv,PLAYER_DR_SEWARD) == BUDAPEST);
		assert(gv_get_location(gv,PLAYER_VAN_HELSING) == ZURICH);
		assert(gv_get_location(gv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(gv_get_location(gv,PLAYER_DRACULA) == HIDE);


		puts ("passed");
		gv_drop (gv);
	}

	


}

void test_encouner (void){



	{
		puts ("Test for vampire mature");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DZA.V.. "
			"GST.... SAO.... HZU.... MBB.... DD1T... "
			"GST.... SAO.... HZU.... MBB.... DHIT... "
			"GST.... SAO.... HZU.... MBB.... DBDT... "
			"GST.... SAO.... HZU.... MBB.... DVIT... "
			"GST.... SAO.... HZU.... MBB.... DPRT... "
			"GST.... SAO.... HZU.... MBB.... DBR..V. "
			"GST.... SAO.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);
		assert(gv_get_score(gv)==GAME_START_SCORE- 7* SCORE_LOSS_DRACULA_TURN- 
										SCORE_LOSS_VAMPIRE_MATURES);
		gv_drop (gv);
	}
	{
		puts ("Test for vampire mature and trap vanish");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DZA.V.. "
			"GST.... SAO.... HZU.... MBB.... DD1T... "
			"GST.... SAO.... HZU.... MBB.... DHIT... "
			"GST.... SAO.... HZU.... MBB.... DBDT... "
			"GST.... SAO.... HZU.... MBB.... DVIT... "
			"GST.... SAO.... HZU.... MBB.... DPRT... "
			"GST.... SAO.... HZU.... MBB.... DBRT.V. "
			"GST.... SAO.... HZU.... MBB.... DHAT.M. "
			"GST.... SAO.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);
		assert(gv_get_score(gv)==GAME_START_SCORE- 8* SCORE_LOSS_DRACULA_TURN- 
										SCORE_LOSS_VAMPIRE_MATURES);
		{
			location_t history[TRAIL_SIZE];
			gv_get_history (gv,PLAYER_DRACULA,history);
			assert(history[5] == HIDE);
			assert(history[4] == BUDAPEST);
			assert(history[3] == VIENNA);
			assert(history[2] == PRAGUE);
			assert(history[1] == BERLIN);
			assert(history[0] == HAMBURG);
		}
		gv_drop (gv);
	}
	{
		puts ("Test random");

		char *trail =
			"GZU.... SSJ.... HSJ.... MBB.... DZA.V.. "
			"GZU.... SSJ.... HSJ.... MBB.... DD1T... "
			"GZU.... SSJ.... HSJ.... MBB.... DMUT... "
			"GZU.... SSJ.... HSJ.... MBB.... DVIT... "
			"GZU.... SSJ.... HSJ.... MBB.... DHIT... "
			"GZU.... SSJ.... HSJ.... MBB.... DPRT... "
			"GMUT... SZATV.. HSJ.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);
		assert(gv_get_score(gv)==GAME_START_SCORE- 6* SCORE_LOSS_DRACULA_TURN);
		
		{
			location_t history[TRAIL_SIZE];
			gv_get_history (gv,PLAYER_DRACULA,history);
			assert(history[5] == ZAGREB);
			assert(history[4] == DOUBLE_BACK_1);
			assert(history[3] == MUNICH);
			assert(history[2] == VIENNA);
			assert(history[1] == HIDE);
			assert(history[0] == PRAGUE);
			
		}
		{
			location_t history[TRAIL_SIZE];
			gv_get_history (gv,PLAYER_LORD_GODALMING,history);
			assert(history[5] == ZURICH);
			assert(history[4] == ZURICH);
			assert(history[3] == ZURICH);
			assert(history[2] == ZURICH);
			assert(history[1] == ZURICH);
			assert(history[0] == MUNICH);
			
		}
		gv_drop (gv);
	}
}

void test_trail_move (void){


	{
		puts ("\tTest for dracula first move");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DZA.V.. "
			"GST.... SAO.... HZU.... MBB.... DHIT... "
			"GST.... SAO.... HZU.... MBB.... DD1T... "
			"GST.... SAO.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);
		
		assert( gv_get_location(gv,PLAYER_DRACULA) == DOUBLE_BACK_1);
		{
			location_t history[TRAIL_SIZE];
			gv_get_history (gv,PLAYER_DRACULA,history);
			assert(history[2] == ZAGREB);
			assert(history[1] == HIDE);
			assert(history[0] == DOUBLE_BACK_1);

			for( int n=3; n<TRAIL_SIZE; n++){
				assert(history[n] == UNKNOWN_LOCATION);
			}
		}
		

		puts ("passed");
		gv_drop (gv);
	}
	

	{
		puts("\tTest for hunter encounter first traps\n"
					"\tbefore encounter the second\n"
					"\t");
		char* trail =

			"GPR.... SPR.... HZU.... MBB.... DVI.V.. "
			"GVIVD.. SVID... HZU.... MBB.... DPRT... "
			"GPRTD.. SPRD... HZU.... MBB.... DD2T... "
			"GSZ.... SVIT... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);

		{
			location_t history[TRAIL_SIZE];
			gv_get_history (gv,PLAYER_LORD_GODALMING,history);
			assert(history[3] == PRAGUE);
			assert(history[2] == VIENNA);
			assert(history[1] == PRAGUE);
			assert(history[0] == SZEGED);

			for( int n=4; n<TRAIL_SIZE; n++){
				assert(history[n] == UNKNOWN_LOCATION);
			}
		}

		{
			location_t history[TRAIL_SIZE];
			gv_get_history (gv,PLAYER_DR_SEWARD,history);
			assert(history[3] == PRAGUE);
			assert(history[2] == VIENNA);
			assert(history[1] == PRAGUE);
			assert(history[0] == VIENNA);
			assert(gv_get_location(gv,PLAYER_DR_SEWARD) == HOSPITAL_LOCATION);
			for( int n=4; n<TRAIL_SIZE; n++){
				assert(history[n] == UNKNOWN_LOCATION);
			}
		}


		puts ("passed");
		gv_drop (gv);
	}


}

bool seen(location_t target, location_t* trail, size_t size){
	for(size_t n=0; n<size;n++){
		if ( trail[n] == target) return true;
	}
	return false;

}


void connection_check( void){
	
	{
		puts("\tTest for general both hunter and dracula");
		char* trail ="GPR.... SVI.... HVI.... MVI....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);

		{
			{
				size_t num;
				location_t* dest = gv_get_connections(gv,&num,gv_get_location(gv,PLAYER_DRACULA),
                                                   PLAYER_DRACULA,gv_get_round(gv)
                                                   ,false,false,false);
				assert(num == 0);
				assert(dest == NULL);
			}
			{
				size_t num;
				location_t* dest = gv_get_connections(gv,&num,gv_get_location(gv,PLAYER_DRACULA),
                                                   PLAYER_DRACULA,gv_get_round(gv)
                                                   ,true,false,false);
				assert(num == 0);
				assert(dest == NULL);
			}
			{
				size_t num;
				location_t* dest = gv_get_connections(gv,&num,gv_get_location(gv,PLAYER_DRACULA),
                                                   PLAYER_DRACULA,gv_get_round(gv)
                                                   ,false,false,true);
				assert(num == 0);
				assert(dest == NULL);
			}
			{
				size_t num;
				location_t* dest = gv_get_connections(gv,&num,gv_get_location(gv,PLAYER_DRACULA),
                                                   PLAYER_DRACULA,gv_get_round(gv)
                                                   ,true,true,true);
				assert(num == 0);
				assert(dest == NULL);
			}
		}

		{
			puts("\t rail 0 connection");
			size_t num;
			// Round num is 0 , player is 0 , rail is 0
			location_t* dest = gv_get_connections(gv,&num,gv_get_location(gv,PLAYER_LORD_GODALMING),
                                                PLAYER_LORD_GODALMING,gv_get_round(gv),
                                                true,true,true);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++)
				seen[dest[i]] = true;

			assert (num == 4); 
			assert (seen[PRAGUE]); 
			assert (seen[BERLIN]); 
			assert (seen[NUREMBURG]);
			assert (seen[VIENNA]); 
			free(dest);
		}

		{
			puts("\t rail 1 connection");
			size_t num;
			// Round num is 0 , player is 0 , rail is 0
			location_t* dest = gv_get_connections(gv,&num,gv_get_location(gv,PLAYER_DR_SEWARD),
                                                PLAYER_DR_SEWARD,gv_get_round(gv),
                                                true,true,true);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++)
				seen[dest[i]] = true;

			assert (num == 6); 
			assert (seen[VIENNA]); 
			assert (seen[PRAGUE]); 
			assert (seen[BUDAPEST]);
			assert (seen[MUNICH]); 
			assert (seen[ZAGREB]); 
			assert (seen[VENICE]); 
			free(dest);
		}

		{
			puts("\t rail 2 connection");
			size_t num;
			// Round num is 0 , player is 0 , rail is 0
			location_t* dest = gv_get_connections(gv,&num,gv_get_location(gv,PLAYER_VAN_HELSING),
                                                PLAYER_VAN_HELSING,gv_get_round(gv),
                                                true,true,false);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++)
				seen[dest[i]] = true;

			assert (num == 8); 
			// ROAD 
			assert (seen[VIENNA]); 
			assert (seen[PRAGUE]); 
			assert (seen[BUDAPEST]);
			assert (seen[MUNICH]); 
			assert (seen[ZAGREB]); 
			// RAIL 1
			assert (seen[VENICE]); 
			// RAIL 2
			assert (seen[SZEGED]);
			assert (seen[BERLIN]);
			free(dest);
		}

		{
			puts("\t rail 3 connection");
			size_t num;
			// Round num is 0 , player is 0 , rail is 0
			location_t* dest = gv_get_connections(gv,&num,gv_get_location(gv,PLAYER_MINA_HARKER),
                                                PLAYER_MINA_HARKER,gv_get_round(gv),
                                                true,true,false);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++)
				seen[dest[i]] = true;

			assert (num == 12); 
			// ROAD 
			assert (seen[VIENNA]); 
			assert (seen[PRAGUE]); 
			assert (seen[BUDAPEST]);
			assert (seen[MUNICH]); 
			assert (seen[ZAGREB]); 
			// RAIL 1
			assert (seen[VENICE]); 
			// RAIL 2
			assert (seen[SZEGED]);
			assert (seen[BERLIN]);
			// rail 3
			assert (seen[HAMBURG]);
			assert (seen[LEIPZIG]);
			assert (seen[BELGRADE]);
			assert (seen[BUCHAREST]);
			free(dest);
		}
		gv_drop (gv);
	}
	{
		puts("hunter connection test: first round");
		
		char* trail =
		"GMA.... SMA.... HMA.... MMA....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);
		{
			puts("\tNO rail connection for lord goldming (0 + 0)%%4 =0");
			size_t num;
			
			location_t* dest = gv_get_connections(gv,&num,gv_get_location(gv,PLAYER_LORD_GODALMING),
                                                PLAYER_LORD_GODALMING,gv_get_round(gv),
                                                true,true,true);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			free(dest);
			assert (num == 7);
			assert (seen[MADRID]);
			assert (seen[LISBON]); 
			assert (seen[CADIZ]);
			assert (seen[GRANADA]);
			assert (seen[ALICANTE]);
			assert (seen[SARAGOSSA]);
			assert (seen[SANTANDER]);      
		}
		{
			puts("\tno extra rail connection for dr s (0 + 1)%%4 =1");
			size_t num;
			
			location_t* dest = gv_get_connections(gv,&num,gv_get_location(gv,PLAYER_DR_SEWARD),
                                                PLAYER_DR_SEWARD,gv_get_round(gv),
                                                true,true,true);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			// doesnt change anything..
			assert (num == 7);
			assert (seen[MADRID]);
			assert (seen[LISBON]); 
			assert (seen[CADIZ]);
			assert (seen[GRANADA]);
			assert (seen[ALICANTE]);
			assert (seen[SARAGOSSA]);
			assert (seen[SANTANDER]);      
			free(dest);
		}
		{
			puts("\textra connection for van h (0 + 2)%%4 =2");
			size_t num;
			
			location_t* dest = gv_get_connections(gv,&num,gv_get_location(gv,PLAYER_VAN_HELSING),
                                                PLAYER_VAN_HELSING,gv_get_round(gv),
                                                true,true,true);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			// doesnt change anything..
			assert (num == 9);
			assert (seen[MADRID]);
			assert (seen[LISBON]); 
			assert (seen[CADIZ]);
			assert (seen[GRANADA]);
			assert (seen[ALICANTE]);
			assert (seen[SARAGOSSA]);
			assert (seen[SANTANDER]);      
			// rail 2
			assert (seen[BORDEAUX]);
			assert (seen[BARCELONA]);
			free(dest);
		}
		{
			puts("\textra connection for mina (0 + 3)%%4 =3");
			size_t num;
			
			location_t* dest = gv_get_connections(gv,&num,gv_get_location(gv,PLAYER_MINA_HARKER),
                                                PLAYER_MINA_HARKER,gv_get_round(gv),
                                                true,true,true);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			// doesnt change anything..
			assert (num == 10);
			assert (seen[MADRID]);
			assert (seen[LISBON]); 
			assert (seen[CADIZ]);
			assert (seen[GRANADA]);
			assert (seen[ALICANTE]);
			assert (seen[SARAGOSSA]);
			assert (seen[SANTANDER]);      
			// rail 2
			assert (seen[BORDEAUX]);
			assert (seen[BARCELONA]);
			// rail 3
			assert (seen[PARIS]);
			free(dest);
		}
		gv_drop (gv);


	}
	
	{
		puts("hunter connection test: SECOND round, port city");
		
		char* trail =
		"GAL.... SAL.... HAL.... MAL.... DBI.V.. "
		"GAL.... SAL.... HAL.... MAL....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		GameView gv = gv_new (trail, messages);
		{
			puts("\tonly road connection");
			size_t num;
			
			location_t* dest = gv_get_connections(gv,&num,gv_get_location(gv,PLAYER_LORD_GODALMING),
                                                PLAYER_LORD_GODALMING,gv_get_round(gv),
                                                true,false,false);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			assert (num == 4);

			assert (seen[ALICANTE]); 
			assert (seen[GRANADA]);
			assert (seen[MADRID]);
			assert (seen[SARAGOSSA]);
			free(dest);
			
		}
		{
			puts("\tonly rail");
			size_t num;
			
			location_t* dest = gv_get_connections(gv,&num,gv_get_location(gv,PLAYER_LORD_GODALMING),
                                                PLAYER_LORD_GODALMING,gv_get_round(gv),
                                                false,true,false);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			assert (num == 3);

			assert (seen[ALICANTE]); 
			assert (seen[MADRID]);
			assert (seen[BARCELONA]);
			free(dest);
			
		}
		{
			puts("\tonly sea");
			size_t num;
			
			location_t* dest = gv_get_connections(gv,&num,gv_get_location(gv,PLAYER_LORD_GODALMING),
                                                PLAYER_LORD_GODALMING,gv_get_round(gv),
                                                false,false,true);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			assert (num == 2);

			assert (seen[ALICANTE]); 
			assert (seen[MEDITERRANEAN_SEA]);
			free(dest);
	
			
		}
		{
			puts("\tall three connection");
			size_t num;
			
			location_t* dest = gv_get_connections(gv,&num,gv_get_location(gv,PLAYER_LORD_GODALMING),
                                                PLAYER_LORD_GODALMING,gv_get_round(gv),
                                                true,true,true);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			assert (num == 6);

			assert (seen[ALICANTE]); 
			assert (seen[MEDITERRANEAN_SEA]);
			assert (seen[MADRID]);
			assert (seen[BARCELONA]);
			assert (seen[GRANADA]);
			assert (seen[SARAGOSSA]);
			free(dest);

		}


		{
			puts("\tall three connection : rail distance 2 from alicante");
			size_t num;
			
			location_t* dest = gv_get_connections(gv,&num,gv_get_location(gv,PLAYER_DR_SEWARD),
                                                PLAYER_DR_SEWARD,gv_get_round(gv),
                                                true,true,true);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			assert (num == 8);

			assert (seen[ALICANTE]); 
			assert (seen[MEDITERRANEAN_SEA]);
			assert (seen[MADRID]);
			assert (seen[BARCELONA]);
			assert (seen[GRANADA]);
			assert (seen[SARAGOSSA]);
			// rail 2
			assert (seen[LISBON]);
			assert (seen[SANTANDER]);
			free(dest);
			

		}
		{
			puts("\tall three connection : rail distance 3 from alicante");
			size_t num;
			
			location_t* dest = gv_get_connections(gv,&num,gv_get_location(gv,PLAYER_VAN_HELSING),
                                                PLAYER_VAN_HELSING,gv_get_round(gv),
                                                true,true,true);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			assert (num == 9);

			assert (seen[ALICANTE]); 
			assert (seen[MEDITERRANEAN_SEA]);
			assert (seen[MADRID]);
			assert (seen[BARCELONA]);
			assert (seen[GRANADA]);
			assert (seen[SARAGOSSA]);
			// rail 2
			assert (seen[LISBON]);
			assert (seen[SANTANDER]);
			// rail 3
			assert (seen[BORDEAUX]);
			free(dest);
			
		}
		{
			puts("\trail only : rail distance 3 from alicante");
			size_t num;
			
			location_t* dest = gv_get_connections(gv,&num,gv_get_location(gv,PLAYER_VAN_HELSING),
                                                PLAYER_VAN_HELSING,gv_get_round(gv),
                                                false,true,false);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			assert (num == 7);

			assert (seen[ALICANTE]); 
	
			assert (seen[MADRID]);
			assert (seen[BARCELONA]);
			// rail 2
			assert (seen[LISBON]);
			assert (seen[SANTANDER]);
			assert (seen[SARAGOSSA]);
			// rail 3
			assert (seen[BORDEAUX]);
			free(dest);
			
		}
		gv_drop (gv);
		
	}


	




}







/////////////////////////////////////////////////////////////////////////////




















void hunter_view (void)
{
	


// ======================= LOCATION BASED TESTS ============================ 

// ### BASIC TESTS ### 

	{
		puts ("Checking Rome total possible moves (5)");

		char *trail =
			"GRO...."; 
		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, gv_get_location(gv,PLAYER_LORD_GODALMING),PLAYER_LORD_GODALMING,gv_get_round(gv), true, true, true);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 5); // including Rome itself
		assert (seen[ROME]); 
		assert (edges[0] == ROME); 
		assert (seen[NAPLES]);
		assert (seen[BARI]); 
		assert (seen[TYRRHENIAN_SEA]);

		puts ("passed");
		free (edges);
		gv_drop (gv);


	} 



	{
	puts ("Checking Rome road connections (4)");

		char *trail =
			"GRO...."; 
		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_LORD_GODALMING),
         PLAYER_LORD_GODALMING,gv_get_round(gv),
             true, false, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 4); // including Rome itself
		assert (seen[ROME]); 
		assert (seen[FLORENCE]);
		assert (seen[NAPLES]); 
		assert (seen[BARI]); 
		assert (!seen[TYRRHENIAN_SEA]);

		puts ("passed");
		free (edges);
		gv_drop (gv);


	}



	{
	puts ("Checking Rome sea connections (2)");

		char *trail =
			"GRO...."; 
		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_LORD_GODALMING),
         PLAYER_LORD_GODALMING,gv_get_round(gv),false, false, true);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 2); // including Rome itself
		assert (seen[ROME]); 
		assert (!seen[FLORENCE]);
		assert (!seen[NAPLES]); 
		assert (!seen[BARI]); 
		assert (seen[TYRRHENIAN_SEA]);

		puts ("passed");
		free (edges);
		gv_drop (gv);


	}


	{
	puts ("Checking Madrid total possible moves (7)");

		char *trail =
			"GMA...."; 
		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_LORD_GODALMING),
         PLAYER_LORD_GODALMING,gv_get_round(gv),true, true, true);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 7); // including Madrid itself
		assert (seen[MADRID]); 
		assert (seen[SANTANDER]);
		assert (seen[ALICANTE]); 
		assert (seen[GRANADA]); 
		assert (seen[CADIZ]);
		assert (seen[LISBON]);
		assert (seen[SARAGOSSA]);



		puts ("passed");
		free (edges);
		gv_drop (gv);


	}



	{
	puts ("Checking Madrid sea connections (1)");

		char *trail =
			"GMA...."; 
		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_LORD_GODALMING),
         PLAYER_LORD_GODALMING,gv_get_round(gv), false, false, true);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 1); // including Madrid itself
		assert (seen[MADRID]); 
		assert (!seen[SANTANDER]);
		assert (!seen[ALICANTE]); 
		assert (!seen[GRANADA]); 
		assert (!seen[CADIZ]);
		assert (!seen[LISBON]);
		assert (!seen[SARAGOSSA]);



		puts ("passed");
		free (edges);
		gv_drop (gv);


	}


	{
	puts ("Checking Madrid road connections (7)");

		char *trail =
			"GMA...."; 
		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_LORD_GODALMING),
         PLAYER_LORD_GODALMING,gv_get_round(gv),true, false, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 7); // including Madrid itself
		assert (seen[MADRID]); 
		assert (seen[SANTANDER]);
		assert (seen[ALICANTE]); 
		assert (seen[GRANADA]); 
		assert (seen[CADIZ]);
		assert (seen[LISBON]);
		assert (seen[SARAGOSSA]);



		puts ("passed");
		free (edges);
		gv_drop (gv);


	}



	{
	puts ("Checking Madrid rail connections (rail unavailable)");

		char *trail =
			"GMA...."; 
		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_LORD_GODALMING),
         PLAYER_LORD_GODALMING,gv_get_round(gv), false, true, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 1); // including Madrid itself
		assert (seen[MADRID]); 
		assert (!seen[SANTANDER]);
		assert (!seen[ALICANTE]); 
		assert (!seen[GRANADA]); 
		assert (!seen[CADIZ]);
		assert (!seen[LISBON]);
		assert (!seen[SARAGOSSA]);



		puts ("passed");
		free (edges);
		gv_drop (gv);


	}

	{
	puts ("Checking Madrid rail connections (one rail move available)");

		char *trail =
			"GMA.... SAO.... HMN.... MPR.... DVIV... "
			"GMA...."; 
		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_LORD_GODALMING),
         PLAYER_LORD_GODALMING,gv_get_round(gv),false, true, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 5); // including Madrid itself
		assert (seen[MADRID]); 
		assert (seen[SANTANDER]);
		assert (seen[ALICANTE]); 
		assert (!seen[GRANADA]); 
		assert (!seen[CADIZ]);
		assert (seen[LISBON]);
		assert (seen[SARAGOSSA]);



		puts ("passed");
		free (edges);
		gv_drop (gv);



	}

	


	{
	puts ("Checking Bucharest rail connections (rail unavailable)");

		char *trail =
			"GBC...."; 
		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_LORD_GODALMING),
         PLAYER_LORD_GODALMING,gv_get_round(gv),false, true, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 1); // including Bucharest itself
		assert (seen[BUCHAREST]); 
		assert (!seen[GALATZ]);
		assert (!seen[CONSTANTA]); 
		assert (!seen[SOFIA]); 
		assert (!seen[SZEGED]);
		assert (!seen[KLAUSENBURG]);



		puts ("passed");
		free (edges);
		gv_drop (gv);


	}

	{
	puts ("Checking Bucharest rail connections (one rail move available)");

		char *trail =
			"GBC.... SAO.... HMN.... MPR.... DVIV... "
			"GBC...."; 
		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_LORD_GODALMING),
         PLAYER_LORD_GODALMING,gv_get_round(gv),false, true, false);

		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 4); // including Bucharst itself
		assert (seen[BUCHAREST]); 
		assert (seen[GALATZ]);
		assert (seen[CONSTANTA]); 
		assert (!seen[SOFIA]); 
		assert (seen[SZEGED]);
		assert (!seen[KLAUSENBURG]);



		puts ("passed");
		free (edges);
		gv_drop (gv);



	}

	{
	puts ("Checking Manchester total possible moves (4)");

		char *trail =
			"GMN...."; 
		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_LORD_GODALMING),
         PLAYER_LORD_GODALMING,gv_get_round(gv),true, true, true);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 4); // including Manchester itself
		assert (seen[MANCHESTER]); 
		assert (seen[LIVERPOOL]);
		assert (seen[EDINBURGH]); 
		assert (seen[LONDON]); 
		assert (!seen[NORTH_SEA]);
		assert (!seen[IRISH_SEA]);
		assert (!seen[ENGLISH_CHANNEL]);



		puts ("passed");
		free (edges);
		gv_drop (gv);

	}

	{

	puts ("Checking Manchester rail connections (rail is unavailable) (none)");

		char *trail =
			"GMN...."; 
		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_LORD_GODALMING),
         PLAYER_LORD_GODALMING,gv_get_round(gv), false, true, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 1); // including Manchester itself
		assert (seen[MANCHESTER]); 
		assert (!seen[LIVERPOOL]);
		assert (!seen[EDINBURGH]); 
		assert (!seen[LONDON]); 
		assert (!seen[NORTH_SEA]);
		assert (!seen[IRISH_SEA]);
		assert (!seen[ENGLISH_CHANNEL]);



		puts ("passed");
		free (edges);
		gv_drop (gv);

	}


	{


	puts ("Checking Manchester rail connections (one rail move is available) (3)");

		char *trail =
			"GMN.... SMN.... HMN.... MMN.... DC?.V.. "; 

		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_LORD_GODALMING),
         PLAYER_LORD_GODALMING,gv_get_round(gv),false, true, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 4); // including Manchester itself
		assert (seen[MANCHESTER]); 
		assert (seen[LIVERPOOL]);
		assert (seen[EDINBURGH]); 
		assert (seen[LONDON]); 
		assert (!seen[NORTH_SEA]);
		assert (!seen[IRISH_SEA]);
		assert (!seen[ENGLISH_CHANNEL]);



		puts ("passed");
		free (edges);
		gv_drop (gv);







	}

	{



	puts ("Checking Paris rail connections (rail is unavailable)");

		char *trail =
			"GPA...."; 

		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_LORD_GODALMING),
         PLAYER_LORD_GODALMING,gv_get_round(gv),false, true, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 1); // including Manchester itself
		assert (seen[PARIS]); 
		assert (!seen[BORDEAUX]);
		assert (!seen[LE_HAVRE]); 
		assert (!seen[BRUSSELS]); 
		assert (!seen[COLOGNE]);
		assert (!seen[MARSEILLES]);
		assert (!seen[SARAGOSSA]);
		assert (!seen[FRANKFURT]);




		puts ("passed");
		free (edges);
		gv_drop (gv);



	}


	{



	puts ("Checking Paris rail connections (one move rail is available)");

		char *trail =
			"GPA.... SPA...."; 

		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_DR_SEWARD),
         PLAYER_DR_SEWARD,gv_get_round(gv),false, true, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 5); // including Paris itself
		assert (seen[PARIS]); 
		assert (seen[BORDEAUX]);
		assert (seen[LE_HAVRE]); 
		assert (seen[BRUSSELS]); 
		assert (!seen[COLOGNE]);
		assert (seen[MARSEILLES]);
		assert (!seen[SARAGOSSA]);
		assert (!seen[FRANKFURT]);




		puts ("passed");
		free (edges);
		gv_drop (gv);



	}



	{



	puts ("Checking Paris rail connections (two move rail is available)");

		char *trail =
			"GPA.... SPA.... HPA...."; 

		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_VAN_HELSING),
         PLAYER_VAN_HELSING,gv_get_round(gv),false, true, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 7); // including Paris itself
		assert (seen[PARIS]); 
		assert (seen[BORDEAUX]);
		assert (seen[LE_HAVRE]); 
		assert (seen[BRUSSELS]); 
		assert (seen[COLOGNE]);
		assert (seen[MARSEILLES]);
		assert (seen[SARAGOSSA]);
		assert (!seen[BARCELONA]);
		assert (!seen[FRANKFURT]);
		assert (!seen[MADRID]);





		puts ("passed");
		free (edges);
		gv_drop (gv);


	}


	{


	puts ("Checking Paris rail connections (three move rail is available)");

		char *trail =
			"GPA.... SPA.... HPA.... MPA...."; 

		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_MINA_HARKER),
         PLAYER_MINA_HARKER,gv_get_round(gv), false, true, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 10); // including Paris itself
		assert (seen[PARIS]); 
		assert (seen[BORDEAUX]);
		assert (seen[LE_HAVRE]); 
		assert (seen[BRUSSELS]); 
		assert (seen[COLOGNE]);
		assert (seen[MARSEILLES]);
		assert (seen[SARAGOSSA]);
		assert (seen[BARCELONA]);
		assert (seen[FRANKFURT]);
		assert (seen[MADRID]);


		puts ("passed");
		free (edges);
		gv_drop (gv);




	}

	{


	puts ("Hunter is only allowed one move if considering both rail & road (one rail move available)");

		char *trail =
			"GPA.... SPA...."; 

		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_DR_SEWARD),
         PLAYER_DR_SEWARD,gv_get_round(gv), true, true, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 9); // including Paris itself
		assert (seen[PARIS]); 
		assert (seen[BORDEAUX]);
		assert (seen[LE_HAVRE]); 
		assert (seen[BRUSSELS]); 
		assert (seen[CLERMONT_FERRAND]); 
		assert (seen[GENEVA]); 
		assert (!seen[COLOGNE]);
		assert (seen[MARSEILLES]);
		assert (!seen[SARAGOSSA]);
		assert (!seen[FRANKFURT]);




		puts ("passed");
		free (edges);
		gv_drop (gv);



	}

	{



	puts ("Checking Vienna rail connections (rail is unavailable)");

		char *trail =
			"GVI...."; 

		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_LORD_GODALMING),
         PLAYER_LORD_GODALMING,gv_get_round(gv), false, true, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 1); // including Vienna itself
		assert (seen[VIENNA]); 
		assert (!seen[BUDAPEST]);
		assert (!seen[PRAGUE]); 
		assert (!seen[VENICE]); 
		assert (!seen[SZEGED]);
		assert (!seen[BERLIN]);
		assert (!seen[HAMBURG]);
		assert (!seen[LEIPZIG]);




		puts ("passed");
		free (edges);
		gv_drop (gv);



	}


	{



	puts ("Checking Vienna rail connections (one move rail is available)");

		char *trail =
			"GVI.... SVI...."; 

		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_DR_SEWARD),
         PLAYER_DR_SEWARD,gv_get_round(gv),  false, true, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 4); // including Vienna itself
		assert (seen[VIENNA]); 
		assert (seen[BUDAPEST]);
		assert (seen[PRAGUE]); 
		assert (seen[VENICE]); 
		assert (!seen[SZEGED]);
		assert (!seen[BERLIN]);
		assert (!seen[HAMBURG]);
		assert (!seen[LEIPZIG]);




		puts ("passed");
		free (edges);
		gv_drop (gv);



	}



	{



	puts ("Checking Vienna rail connections (two move rail is available)");

		char *trail =
			"GVI.... SVI.... HVI...."; 

		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_VAN_HELSING),
         PLAYER_VAN_HELSING,gv_get_round(gv),  false, true, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 6); // including Vienna itself
		assert (seen[VIENNA]); 
		assert (seen[BUDAPEST]);
		assert (seen[PRAGUE]); 
		assert (seen[VENICE]); 
		assert (seen[SZEGED]);
		assert (!seen[BUCHAREST]); 
		assert (!seen[BELGRADE]); 
		assert (seen[BERLIN]);
		assert (!seen[HAMBURG]);
		assert (!seen[LEIPZIG]);





		puts ("passed");
		free (edges);
		gv_drop (gv);


	}


	{


	puts ("Checking Vienna rail connections (three move rail is available)");

		char *trail =
			"GVI.... SVI.... HVI.... MVI...."; 

		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_MINA_HARKER),
         PLAYER_MINA_HARKER,gv_get_round(gv), false, true, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 10); // including Vienna itself
		assert (seen[VIENNA]); 
		assert (seen[BUDAPEST]);
		assert (seen[PRAGUE]); 
		assert (seen[VENICE]); 
		assert (seen[SZEGED]);
		assert (seen[BUCHAREST]); 
		assert (seen[BELGRADE]); 
		assert (seen[BERLIN]);
		assert (seen[HAMBURG]);
		assert (seen[LEIPZIG]);


		puts ("passed");
		free (edges);
		gv_drop (gv);




	}

	{

	puts ("Checking Vienna rail + road connections (three move rail is available)");

		char *trail =
			"GVI.... SVI.... HVI.... MVI...."; 

		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_MINA_HARKER),
         PLAYER_MINA_HARKER,gv_get_round(gv),true, true, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++) {
			seen[edges[i]] = true;
		}

		assert (n_edges == 12); // including Vienna itself
		assert (seen[VIENNA]); 
		assert (seen[BUDAPEST]);
		assert (seen[PRAGUE]); 
		assert (seen[VENICE]); 
		assert (seen[SZEGED]);
		assert (seen[MUNICH]);
		assert (seen[ZAGREB]); 
		assert (seen[BUCHAREST]); 
		assert (seen[BELGRADE]); 
		assert (seen[BERLIN]);
		assert (seen[HAMBURG]);
		assert (seen[LEIPZIG]);


		puts ("passed");
		free (edges);
		gv_drop (gv);


	}



	{
	puts ("Dracula should not be able to take a rail");

		char *trail =
			"GVI.... SZA.... HMA.... MAO.... DPAV... "; 

		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_DRACULA),
         PLAYER_DRACULA,gv_get_round(gv), true, true, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 7); // including Manchester itself
		assert (seen[PARIS]); 
		assert (seen[LE_HAVRE]);
		assert (seen[NANTES]); 
		assert (seen[CLERMONT_FERRAND]); 
		assert (!seen[BORDEAUX]);
		assert (!seen[MARSEILLES]);
		assert (seen[BRUSSELS]);



		puts ("passed");
		free (edges);
		gv_drop (gv);

	}

	{
	puts ("Dracula get_connections should not include cities in"
		  " Dracula's trail (since he cannot backtrack unless Double back)");

		char *trail =
			"GVI.... SZA.... HMA.... MAO.... DPAV... "
			"GVI.... SZA.... HMA.... MAO.... DCFT... "; 

		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_DRACULA),
         PLAYER_DRACULA,gv_get_round(gv),  true, false, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 7); // including Clermont itself
		assert (seen[CLERMONT_FERRAND]); 
		assert (seen[PARIS]);
		assert (seen[NANTES]); 
		assert (seen[TOULOUSE]); 
		assert (seen[MARSEILLES]);
		assert (seen[GENEVA]);
		assert (seen[BORDEAUX]);



		puts ("passed");
		free (edges);
		gv_drop (gv);

	}




	{
	puts ("Checking Bay of Biscay total possible moves (6)");

		char *trail =
			"GBB...."; 
		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_LORD_GODALMING),
         PLAYER_LORD_GODALMING,gv_get_round(gv),  false, false, true);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 5); // including Madrid itself
		assert (seen[BAY_OF_BISCAY]); 
		assert (seen[NANTES]);
		assert (seen[BORDEAUX]); 
		assert (seen[SANTANDER]); 
		assert (seen[ATLANTIC_OCEAN]);



		puts ("passed");
		free (edges);
		gv_drop (gv);


	}




	{
	puts ("Checking Mediteranian Sea total possible moves (7)");

		char *trail =
			"GMS...."; 
		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_LORD_GODALMING),
         PLAYER_LORD_GODALMING,gv_get_round(gv),  true, true, true);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 7); // including mediternian sea itself
		assert (seen[MEDITERRANEAN_SEA]); 
		assert (seen[ALICANTE]);
		assert (seen[BARCELONA]); 
		assert (seen[MARSEILLES]); 
		assert (seen[CAGLIARI]);
		assert (seen[TYRRHENIAN_SEA]);
		assert (seen[ATLANTIC_OCEAN]);
		assert (!seen[GRANADA]); 



		puts ("passed");
		free (edges);
		gv_drop (gv);


	} 

	{

	puts ("Checking Adriatic Sea total possible moves (4)");

		char *trail =
			"GAS...."; 
		player_message messages[] = {
			"Hello"};

		GameView gv = gv_new (trail, messages);

		size_t n_edges;
		location_t *edges = gv_get_connections (
			gv, &n_edges, 
         gv_get_location(gv,PLAYER_LORD_GODALMING),
         PLAYER_LORD_GODALMING,gv_get_round(gv), true, true, true);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 4); // including mediternian sea itself
		assert (seen[ADRIATIC_SEA]); 
		assert (seen[IONIAN_SEA]);
		assert (seen[BARI]); 
		assert (seen[VENICE]); 
		assert (!seen[NAPLES]);
		assert (!seen[TYRRHENIAN_SEA]);
		assert (!seen[ROME]);
		assert (!seen[GRANADA]); 



		puts ("passed");
		free (edges);
		gv_drop (gv);



	}



// ==========================================================================

// ========================= ENCOUNTER TESTS  =============================== 


	{
		puts ("Test for dracula encounter ONE hunter on the land");

		char *trail =
			"GPR.... SBD.... HZU.... MBB.... DVI.V.. "
			"GVIVD..";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};

		GameView gv = gv_new (trail, messages);

		assert (gv_get_round (gv) == 1);
		assert (gv_get_health(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - LIFE_LOSS_HUNTER_ENCOUNTER);
		assert (gv_get_score(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN );

		puts ("passed");
		gv_drop (gv);

	}

	{
		puts ("Test for dracula encounter TWO hunters on the land");

		char *trail =
			"GPR.... SBD.... HZU.... MBB.... DVI.V.. "
			"GVIVD.. SVIVD..";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};

		GameView gv = gv_new (trail, messages);

		assert (gv_get_round (gv) == 1);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 2*LIFE_LOSS_HUNTER_ENCOUNTER);
		assert (gv_get_health(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (gv_get_health(gv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (gv_get_score(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN );



		puts ("passed");
		gv_drop (gv);
		
	}

	{

		puts ("Test for dracula encounter FOUR hunters on the land");

		char *trail =
			"GPR.... SBD.... HZU.... MBB.... DVI.V.. "
			"GVIVD.. SVID... HVID... MVID...";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};

		GameView gv = gv_new (trail, messages);

		assert (gv_get_round (gv) == 1);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 4*LIFE_LOSS_HUNTER_ENCOUNTER);
		assert (gv_get_health(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (gv_get_health(gv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (gv_get_health(gv, PLAYER_MINA_HARKER) == GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (gv_get_health(gv, PLAYER_VAN_HELSING) == GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (gv_get_score(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN );
		assert (gv_get_health (gv, PLAYER_DRACULA) <= 0); 



		puts ("passed");
		gv_drop (gv);


	}  

	{
		puts ("Test for dracula & hunter encounter at sea (no encounter occurs)");

		char *trail =
			"GIR.... SBD.... HZU.... MBB.... DAO.... " // assume we know dracula location
			"GAO....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};

		GameView gv = gv_new (trail, messages);
		location_t history[TRAIL_SIZE]; 

		gv_get_history (gv, PLAYER_DRACULA, history); 

		// Test history
		assert (history[0] == ATLANTIC_OCEAN); 
		assert (history[1] == UNKNOWN_LOCATION); 
		assert (history[2] == UNKNOWN_LOCATION);  
		assert (history[3] == UNKNOWN_LOCATION); 
		assert (history[4] == UNKNOWN_LOCATION); 
		assert (history[5] == UNKNOWN_LOCATION);  

		assert (gv_get_health (gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS); 
		assert (gv_get_health(gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - LIFE_LOSS_SEA);
		assert (gv_get_score(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN );

		puts ("passed");
		gv_drop (gv);

	}

	{
		puts("Testing hunter eliminating immature vampire");
		char* trail =
			"GPR.... SBD.... HZU.... MBB.... DVI.V.. "
			"GPR.... SBD.... HZU.... MBB.... DTPT... "
			"GVI.V..";

		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};

		GameView gv = gv_new (trail, messages);
		location_t history[TRAIL_SIZE]; 

		gv_get_history (gv, PLAYER_LORD_GODALMING, history); 

		// Testing history
		assert (history[0] == VIENNA); 
		assert (history[1] == PRAGUE); 
		assert (history[2] == PRAGUE);  
		assert (history[3] == UNKNOWN_LOCATION); 
		assert (history[4] == UNKNOWN_LOCATION); 
		assert (history[5] == UNKNOWN_LOCATION);  

		// Testing stats
		assert (gv_get_round (gv) == 2);
		assert (gv_get_health(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS +LIFE_GAIN_CASTLE_DRACULA);
		assert (gv_get_score(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN*2 );

		puts ("passed");
		gv_drop (gv);


	} 


	{
	





	} 


// ========================= HOSPITAL TESTS  =============================== 

	{
		puts("Hunter loses all life points and teleported to the hospital." 
		" History should not update until that hunters next turn");


		char *trail =
			"GPR.... SBD.... HZU.... MBB.... DVIV... "
			"GVIVD.. SBD.... HZU.... MBB.... DBDT... "
			"GBDTD.. SBD.... HZU.... MBB.... DKLT... "; 
			
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};

		GameView gv = gv_new (trail, messages);
		location_t history[TRAIL_SIZE]; 

		// Testing history
		gv_get_history(gv, PLAYER_LORD_GODALMING, history); 

		assert (history[0] == BUDAPEST);
		assert (history[1] == VIENNA); 
		assert (history[2] == PRAGUE); 
		assert (history[3] == UNKNOWN_LOCATION); 
		assert (history[4] == UNKNOWN_LOCATION);  
		assert (history[5] == UNKNOWN_LOCATION); 

		gv_get_history(gv, PLAYER_DRACULA, history); 

		assert (history[0] == KLAUSENBURG);
		assert (history[1] == BUDAPEST); 
		assert (history[2] == VIENNA); 
		assert (history[3] == UNKNOWN_LOCATION); 
		assert (history[4] == UNKNOWN_LOCATION);  
		assert (history[5] == UNKNOWN_LOCATION); 	

		// Testing stats
		assert (gv_get_health (gv, PLAYER_LORD_GODALMING) == 0); 
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 2*LIFE_LOSS_HUNTER_ENCOUNTER); 
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == ST_JOSEPH_AND_ST_MARYS); 
		assert (gv_get_score(gv) == GAME_START_SCORE - (3*SCORE_LOSS_DRACULA_TURN + SCORE_LOSS_HUNTER_HOSPITAL)); 
		


		puts ("passed");
		gv_drop (gv);


	}

	
	{
		puts("Hunter who was teleported to hospital recovers life points in following turn");


		char *trail =
			"GPR.... SBD.... HZU.... MBB.... DVIV... "
			"GVIVD.. SBD.... HZU.... MBB.... DBDT... "
			"GBDTD.. SBD.... HZU.... MBB.... DKLT... "
			"GSZ....";

		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};

		GameView gv = gv_new (trail, messages);
		location_t history[TRAIL_SIZE]; 

		// Testing history
		gv_get_history(gv, PLAYER_LORD_GODALMING, history); 

		assert (history[0] == SZEGED);
		assert (history[1] == BUDAPEST); 
		assert (history[2] == VIENNA); 
		assert (history[3] == PRAGUE); 
		assert (history[4] == UNKNOWN_LOCATION);  
		assert (history[5] == UNKNOWN_LOCATION); 

		gv_get_history(gv, PLAYER_DRACULA, history); 

		assert (history[0] == KLAUSENBURG);
		assert (history[1] == BUDAPEST); 
		assert (history[2] == VIENNA); 
		assert (history[3] == UNKNOWN_LOCATION); 
		assert (history[4] == UNKNOWN_LOCATION);  
		assert (history[5] == UNKNOWN_LOCATION); 	

		// Testing stats
		assert (gv_get_health (gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS); 
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == SZEGED); 
		assert (gv_get_score(gv) == GAME_START_SCORE - (3*SCORE_LOSS_DRACULA_TURN + SCORE_LOSS_HUNTER_HOSPITAL)); 
		


		puts ("passed");
		gv_drop (gv);


	}

	{
		puts ("Hunter who has lost some life points does not recover hp upon moving"
			  " (not teleporting) to hospital"); 



		char *trail =
			"GPR.... SBD.... HZU.... MBB.... DC?V... "
			"GVIVD.. SBD.... HZU.... MBB.... DBDT... "
			"GZA.... SBD.... HZU.... MBB.... DKLT... "
			"GJM.... SBD.... HZU.... MBB.... DKLT... "; 
			
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};

		GameView gv = gv_new (trail, messages);
		location_t history[TRAIL_SIZE]; 

		// Testing history
		gv_get_history(gv, PLAYER_LORD_GODALMING, history); 

		assert (history[0] == ST_JOSEPH_AND_ST_MARYS);
		assert (history[1] == ZAGREB); 
		assert (history[2] == VIENNA); 
		assert (history[3] == PRAGUE); 
		assert (history[4] == UNKNOWN_LOCATION);  
		assert (history[5] == UNKNOWN_LOCATION); 


		// Testing stats
		assert (gv_get_health (gv, PLAYER_LORD_GODALMING) == 5); 
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - LIFE_LOSS_HUNTER_ENCOUNTER); 
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == ST_JOSEPH_AND_ST_MARYS); 
		assert (gv_get_score(gv) == GAME_START_SCORE - 4*SCORE_LOSS_DRACULA_TURN); 
		

		gv_drop (gv);

	}

	{
	
		char *trail =
			"GPR.... SBD.... HZU.... MBB.... DVIV... "
			"GVIVD.. SBD.... HZU.... MBB.... DBDT... "
			"GZA.... SBD.... HZU.... MBB.... DKLT... "
			"GJM.... SBD.... HZU.... MBB.... DKLT... "
			"GZA...."; 
			
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};

		GameView gv = gv_new (trail, messages);
		location_t history[TRAIL_SIZE]; 

		// Testing history
		gv_get_history(gv, PLAYER_LORD_GODALMING, history); 

		assert (history[0] == ZAGREB);
		assert (history[1] == ST_JOSEPH_AND_ST_MARYS); 
		assert (history[2] == ZAGREB); 
		assert (history[3] == VIENNA); 
		assert (history[4] == PRAGUE);  
		assert (history[5] == UNKNOWN_LOCATION); 


		// Testing stats
		assert (gv_get_health (gv, PLAYER_LORD_GODALMING) == 5); 
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - LIFE_LOSS_HUNTER_ENCOUNTER); 
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == ZAGREB); 
		assert (gv_get_score(gv) == GAME_START_SCORE - 4*SCORE_LOSS_DRACULA_TURN); 

		puts ("passed");
		gv_drop (gv); 

	}

	{

	}




/* ############# UNFINISHED #########################
		puts("Multiple hunters lose all life points and are teleported to the hospital");

		char *trail =
			"GPR.... SBD.... HZA.... MBB.... DVIV... "
			"GVIVD.. SVIVD.. HVIVD.. MBB.... DBDT... "
			"GZA.... SZA.... HZA.... MBB.... DKLT... "
			"GBDT... SBDT... HBDT... MBB.... DGAT... "
			"GKLT... SKLT... HKLT... MBB.... DCD.... "
			"GGAT... SGAT... HGAT... MBB.... DN2.... ";

		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};

		GameView gv = gv_new (trail, messages);
		location_t history[TRAIL_SIZE]; 

		// Testing history

			// - LORD GODALMING - 
		gv_get_history(gv, PLAYER_LORD_GODALMING, history); 

		assert (history[0] == GALATZ);
		assert (history[1] == KLAUSENBURG); 
		assert (history[2] == BUDAPEST); 
		assert (history[3] == ZAGREB); 
		assert (history[4] == VIENNA);  
		assert (history[5] == PRAGUE); 


			// - DR SEWARD - 
		gv_get_history(gv, PLAYER_DR_SEWAR
		assert (history[0] == GALATZ);
		assert (history[1] == KLAUSENBURG); 
		assert (history[2] == BUDAPEST); 
		assert (history[3] == ZAGREB); 
		assert (history[4] == VIENNA);  
		assert (history[5] == BUDAPEST); 

			// - VAN HELSING - 
		gv_get_history(gv, PLAYER_VAN_HELSING, history); 

		assert (history[0] == GALATZ);
		assert (history[1] == KLAUSENBURG); 
		assert (history[2] == BUDAPEST); 
		assert (history[3] == ZAGREB); 
		assert (history[4] == VIENNA);  
		assert (history[5] == ZAGREB); 

			// - DRACULA - 
		gv_get_history(gv, PLAYER_DRACULA, history); 

		assert (history[0] == UNKNOWN_LOCATION);
		assert (history[1] == CASTLE_DRACULA); 
		assert (history[2] == GALATZ); 
		assert (history[3] == KLAUSENBURG); 
		assert (history[4] == BUDAPEST);  
		assert (history[5] == VIENNA); 

		// Testing stats
		assert (gv_get_health (gv, PLAYER_LORD_GODALMING) <= 0); 
		printf("%d\n", gv_get_health(gv, PLAYER_DR_SEWARD)); 
		assert (gv_get_health (gv, PLAYER_DR_SEWARD) <= 0); 
		assert (gv_get_health (gv, PLAYER_VAN_HELSING) <= 0); 
		assert (gv_get_health (gv, PLAYER_DRACULA == (GAME_START_BLOOD_POINTS - 3*LIFE_LOSS_HUNTER_ENCOUNTER))); 

		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == ST_JOSEPH_AND_ST_MARYS); 
		assert (gv_get_location (gv, PLAYER_DR_SEWARD) == ST_JOSEPH_AND_ST_MARYS); 
		assert (gv_get_location (gv, PLAYER_VAN_HELSING) == ST_JOSEPH_AND_ST_MARYS); 



		puts ("passed");
		gv_drop (gv);

*/ 

// ========================= TRAIL TESTS  =============================== 


	{
		puts ("Dracula double back to a known city");

		char *trail =
			"GPR.... SBD.... HZU.... MBB.... DVIV... "
			"GVIVD.. SVID... HZU.... MBB.... DC?T... "
			"GZA.... SZA.... HZU.... MBB.... DC?T... "
			"GJM.... SVI.... HZU.... MBB.... DD3.... "
			"GZA.... SVID..."; 
			
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};

		GameView gv = gv_new (trail, messages);
		location_t history[TRAIL_SIZE]; 

		// Testing history
		gv_get_history(gv, PLAYER_DRACULA, history); 

		// history should give most recent move as double back 3 but 
		// get_location should give the exact location since the
		// double back location is known
		assert (history[0] == DOUBLE_BACK_3);
		assert (history[1] == CITY_UNKNOWN); 
		assert (history[2] == CITY_UNKNOWN); 
		assert (history[3] == VIENNA); 
		assert (history[4] == UNKNOWN_LOCATION);  
		assert (history[5] == UNKNOWN_LOCATION); 


		// Testing stats
		assert (gv_get_health (gv, PLAYER_LORD_GODALMING) == 5); 
		assert (gv_get_health (gv, PLAYER_DR_SEWARD) == 4); 
		assert (gv_get_location (gv, PLAYER_DR_SEWARD) == VIENNA); 

		assert (gv_get_location (gv, PLAYER_DRACULA) == DOUBLE_BACK_3); 
		assert (gv_get_health (gv, PLAYER_DRACULA) == (GAME_START_BLOOD_POINTS - 3*LIFE_LOSS_HUNTER_ENCOUNTER)); 
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == ZAGREB); 
		assert (gv_get_score(gv) == GAME_START_SCORE - 4*SCORE_LOSS_DRACULA_TURN ); 

		puts ("passed");
		gv_drop (gv); 

	}




	{

		puts ("Dracula double back to an unknown city");

		char *trail =
			"GPR.... SBD.... HZU.... MBB.... DC?V... "
			"GPR.... SBD.... HZU.... MBB.... DC?T... "
			"GZA.... SZA.... HZU.... MBB.... DC?T... "
			"GJM.... SVI.... HZU.... MBB.... DD3.... "
			"GZA.... SVI...."; 
			
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};

		GameView gv = gv_new (trail, messages);
		location_t history[TRAIL_SIZE]; 

		// Testing history
		gv_get_history(gv, PLAYER_DRACULA, history); 

		// both history and get_location should return double_back 3
		// since the double back city is unknown
		assert (history[0] == DOUBLE_BACK_3);
		assert (history[1] == CITY_UNKNOWN); 
		assert (history[2] == CITY_UNKNOWN); 
		assert (history[3] == CITY_UNKNOWN); 
		assert (history[4] == UNKNOWN_LOCATION);  
		assert (history[5] == UNKNOWN_LOCATION); 


		// Testing stats
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == ZAGREB); 
		assert (gv_get_location (gv, PLAYER_DR_SEWARD) == VIENNA); 

		assert (gv_get_location (gv, PLAYER_DRACULA) == DOUBLE_BACK_3); 
		assert (gv_get_score(gv) == GAME_START_SCORE - 4*SCORE_LOSS_DRACULA_TURN ); 

		puts ("passed");
		gv_drop (gv); 



	}


// ========================================================================= 


	//return EXIT_SUCCESS;
}



	// ==================== TEST CHECKLIST =================================
	// * Hunter moving to hospital vs tp. In first case should not recover hp

	// * Dracula is defeated
	// * Dracula wins
	// * hunter loses all life points
	// * multiple hunters lose all life points
	// * multiple hunters encounter dracula at once
	// * hunter encounters dracula at location where an immature vampire sleeps
	// * same as above test but with multiple hunters on that location
	// * hunters are in the same location as dracula at sea
	// * hunter encounters a trap
	// * multiple hunters encounter a trap at once (different locations)
	// * multiple hunters encounter a trap at once (same location)
	// * hunter and dracula both lose all life points in the same encounter
	// * hunter encounters dracula at castle dracula
	// * hunter is waiting at castle dracula when dracula teleports

	// =====================================================================

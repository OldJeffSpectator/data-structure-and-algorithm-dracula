////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// test_dracula_view.c: test the DraculaView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-02	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// testing rule
// for each cases: test in the order below
// 1. round Number
// 2. score
// 3. health
// 4. location of player
// 5. move info
// 6. encounter info
// 7. trail
// 8. dest & dest_player
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dracula_view.h"
void test_general(void);
void test_encouner (void);
bool seen(location_t target, location_t* trail, size_t size);
void test_trail_move (void);
void connection_check( void);
int main (void)
{
	
	do {////////////////////////////////////////////////////////////////
		puts ("Test for basic functions, "
			  "just before Dracula's first move");

		char *trail =
			"GST.... SAO.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);

		assert (dv_get_round (dv) == 0);
		assert (dv_get_location (dv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert (dv_get_location (dv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert (dv_get_location (dv, PLAYER_VAN_HELSING) == ZURICH);
		assert (dv_get_location (dv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert (dv_get_location (dv, PLAYER_DRACULA) == UNKNOWN_LOCATION);
		assert (dv_get_health (dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		
		{
			puts ("\tTest get move at first round for hunter");
			location_t start ,end;
			dv_get_player_move(dv,PLAYER_LORD_GODALMING,&start,&end);
			assert(start == UNKNOWN_LOCATION);
			assert(end == STRASBOURG);
		}
		{
			puts ("\tTest get move at first round for dracula before he made a move");
			location_t start ,end;
			dv_get_player_move(dv,PLAYER_DRACULA,&start,&end);
			assert(start == UNKNOWN_LOCATION);
			assert(end == UNKNOWN_LOCATION);
		}
		{
			puts ("\tTest UNKOWNLOCATION for get locale info");
			int n_traps,n_vamps;
			location_t where = UNKNOWN_LOCATION;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 0 && n_vamps == 0);
		}

		puts ("passed");
		dv_drop (dv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for encountering Dracula and hunter history");

		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.V.. "
			"GGEVD.. SAO.... HCD.... MAO....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "" };
		DraculaView dv = dv_new (trail, messages);

		assert (dv_get_round (dv) == 1);
		assert (dv_get_location (dv, PLAYER_DRACULA) == GENEVA);
		assert (dv_get_health (dv, PLAYER_LORD_GODALMING) == 5);
		assert (dv_get_health (dv, PLAYER_DRACULA) == 30);
		assert (dv_get_location (dv, PLAYER_LORD_GODALMING) == GENEVA);


		{
			puts ("\tTest get move after legal moves");
			location_t start ,end;
			dv_get_player_move(dv,PLAYER_LORD_GODALMING,&start,&end);
			assert(start == STRASBOURG);
			assert(end == GENEVA);
		}

		{
			puts ("\tTest get move at first round for dracula AFTER he made a move");
			location_t start ,end;
			dv_get_player_move(dv,PLAYER_DRACULA,&start,&end);
			assert(start == UNKNOWN_LOCATION);
			assert(end == GENEVA);
		}
		{
			puts ("\tTest VAMPIRE got killed  for get locale info");
			int n_traps,n_vamps;
			location_t where = GENEVA;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 0 && n_vamps == 0);
		}
		location_t history[TRAIL_SIZE];
		dv_get_trail (dv, PLAYER_DRACULA, history);
		assert (history[0] == GENEVA);
		assert (history[2] == UNKNOWN_LOCATION);

		dv_get_trail (dv, PLAYER_LORD_GODALMING, history);
		assert (history[0] == GENEVA);
		assert (history[1] == STRASBOURG);
		assert (history[2] == UNKNOWN_LOCATION);

		dv_get_trail (dv, PLAYER_DR_SEWARD, history);
		assert (history[0] == ATLANTIC_OCEAN);
		assert (history[1] == ATLANTIC_OCEAN);
		assert (history[2] == UNKNOWN_LOCATION);

		puts ("passed");
		dv_drop (dv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula leaving minions");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DED.V.. "
			"GST.... SST.... HST.... MST.... DMNT... "
			"GST.... SST.... HST.... MST....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Drop a V",
			"Party in Strasbourg", "Party", "Party", "Party"};
		DraculaView dv = dv_new (trail, messages);

		int nT, nV;
		dv_get_locale_info (dv, EDINBURGH, &nT, &nV);
		assert (nT == 0 && nV == 1);
		dv_get_locale_info (dv, MANCHESTER, &nT, &nV);
		assert (nT == 1 && nV == 0);
		assert (dv_get_location (dv, PLAYER_DRACULA) == MANCHESTER);

		{
			puts ("\tTest get move after legal moves");
			location_t start ,end;
			dv_get_player_move(dv,PLAYER_LORD_GODALMING,&start,&end);
			assert(start == STRASBOURG);
			assert(end == STRASBOURG);
		}

		{
			puts ("\tTest get move at first round for dracula AFTER he made a move");
			location_t start ,end;
			dv_get_player_move(dv,PLAYER_DRACULA,&start,&end);
			assert(start == EDINBURGH);
			assert(end == MANCHESTER);
		}


		location_t history[TRAIL_SIZE];
		dv_get_trail (dv, PLAYER_DRACULA, history);
		assert (history[0] == MANCHESTER);
		assert (history[1] == EDINBURGH);
		assert (history[2] == UNKNOWN_LOCATION);

		dv_get_trail (dv, PLAYER_MINA_HARKER, history);
		assert (history[0] == STRASBOURG);
		assert (history[1] == STRASBOURG);
		assert (history[2] == GENEVA);
		assert (history[3] == UNKNOWN_LOCATION);

		puts ("passed");
		dv_drop (dv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Checking Galatz road connections");
		char *trail = "GGA....";
		player_message messages[] = {"Gone to Galatz"};
		DraculaView dv = dv_new (trail, messages);

		size_t n_edges;
		location_t *edges = dv_get_dests_player (
			dv, &n_edges, PLAYER_LORD_GODALMING, true, false, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};
		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 5);
		assert (seen[GALATZ]);
		assert (seen[CONSTANTA]);
		assert (seen[BUCHAREST]);
		assert (seen[KLAUSENBURG]);
		assert (seen[CASTLE_DRACULA]);

		puts ("passed");
		free (edges);
		dv_drop (dv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Checking Ionian Sea sea connections");
		char *trail = "GIO....";
		player_message messages[] = {"Sailing the Ionian"};
		DraculaView dv = dv_new (trail, messages);

		size_t n_edges;
		location_t *edges = dv_get_dests_player (
			dv, &n_edges, PLAYER_LORD_GODALMING, false, false, true);
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

		puts ("passed");
		free (edges);
		dv_drop (dv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Checking Athens rail connections (none)");

		char *trail = "GAT....";
		player_message messages[] = {"Leaving Athens by train"};
		DraculaView dv = dv_new (trail, messages);

		size_t n_edges;
		location_t *edges = dv_get_dests_player (
			dv, &n_edges, PLAYER_LORD_GODALMING, false, true, false);

		assert (n_edges == 1);
		assert (edges[0] == ATHENS);

		puts ("passed");
		free (edges);
		dv_drop (dv);
	} while (0);
/////////////////////////////
// own tests

	test_general();
	test_encouner ();
	test_trail_move ();
	connection_check();
	return EXIT_SUCCESS;




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
		DraculaView dv = dv_new (trail, messages);

		assert (dv_get_round (dv) == 2);
		
		assert (	dv_get_health (dv, PLAYER_DRACULA) == 
					GAME_START_BLOOD_POINTS- LIFE_LOSS_SEA*2);
					// lose blood twice for being at the sea
		assert (	dv_get_health (dv, PLAYER_LORD_GODALMING) == 
					GAME_START_HUNTER_LIFE_POINTS);
					// cant exceed his owm limit
		
		assert( dv_get_score(dv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN *2);
		

		assert(dv_get_location(dv,PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(dv_get_location(dv,PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(dv_get_location(dv,PLAYER_VAN_HELSING) == ZURICH);
		assert(dv_get_location(dv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(dv_get_location(dv,PLAYER_DRACULA) == NORTH_SEA);

		{
			puts ("\tTest SEA  for get locale info");
			int n_traps,n_vamps;
			location_t where = NORTH_SEA;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 0 && n_vamps == 0);
		}
		

		puts ("passed");
		dv_drop (dv);

	}/*
	{
		puts ("\tTest for dracula recover himself by going to calstle dracula after losing blood");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DNS.... " 
			"GST.... SAO.... HZU.... MBB.... DHI.... "
			"GST.... SAO.... HZU.... MBB.... DCD.V.. "
			"GST.... SAO.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);

		assert (dv_get_round (dv) == 3);
		
		assert (dv_get_health(dv, PLAYER_MINA_HARKER) == GAME_START_HUNTER_LIFE_POINTS);
		assert (dv_get_health (dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS- LIFE_LOSS_SEA*2 + 
																									LIFE_GAIN_CASTLE_DRACULA);
		assert (dv_get_score(dv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN *3);


		assert(dv_get_location(dv,PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(dv_get_location(dv,PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(dv_get_location(dv,PLAYER_VAN_HELSING) == ZURICH);
		assert(dv_get_location(dv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(dv_get_location(dv,PLAYER_DRACULA) == CASTLE_DRACULA);
		{
			puts ("\tTest cd for get locale info");
			int n_traps,n_vamps;
			location_t where = CASTLE_DRACULA;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 0 && n_vamps == 1);
		}

		puts ("passed");
		dv_drop (dv);


	}	*/
	{
		puts ("\tTest for dracula recover himself by going to calstle dracula without losing blood");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DCD.V.. "
			"GST.... SAO.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);

		assert (dv_get_round (dv) == 1);
		
		assert (dv_get_health(dv, PLAYER_MINA_HARKER) == GAME_START_HUNTER_LIFE_POINTS);
		assert (dv_get_health (dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS+ 
																							LIFE_GAIN_CASTLE_DRACULA);
		assert (dv_get_score(dv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN );


		assert(dv_get_location(dv,PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(dv_get_location(dv,PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(dv_get_location(dv,PLAYER_VAN_HELSING) == ZURICH);
		assert(dv_get_location(dv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(dv_get_location(dv,PLAYER_DRACULA) == CASTLE_DRACULA);

		{
			puts ("\tTest cd for get locale info");
			int n_traps,n_vamps;
			location_t where = CASTLE_DRACULA;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 0 && n_vamps == 1);
		}
		puts ("passed");
		dv_drop (dv);

	}
	// TEST FOR ENCOUNTER
	{
		puts ("\tTest for dracula encounter ONE hunter on the land");

		char *trail =
			"GPR.... SBD.... HZU.... MBB.... DVI.V.. "
			"GVIVD.. SBD.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);

		assert (dv_get_round (dv) == 1);
		
		assert (dv_get_health(dv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS
																		-LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (dv_get_health (dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS- 
																							LIFE_LOSS_HUNTER_ENCOUNTER);
		assert (dv_get_score(dv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN );

		assert(dv_get_location(dv,PLAYER_LORD_GODALMING) == VIENNA);
		assert(dv_get_location(dv,PLAYER_DR_SEWARD) == BUDAPEST);
		assert(dv_get_location(dv,PLAYER_VAN_HELSING) == ZURICH);
		assert(dv_get_location(dv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(dv_get_location(dv,PLAYER_DRACULA) == VIENNA);

		{
			puts ("\tTest VIENA for get locale info");
			int n_traps,n_vamps;
			location_t where = VIENNA;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 0 && n_vamps == 0);
		}
		puts ("passed");
		dv_drop (dv);

	}
	{
		
		puts ("\tTest for dracula encounter 2 hunter on the land");

		char *trail =
			"GPR.... SBD.... HZU.... MBB.... DVI.V.. "
			"GVIVD.. SVID... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);

		assert (dv_get_round (dv) == 1);
		
		assert (dv_get_health(dv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS
																		-LIFE_LOSS_DRACULA_ENCOUNTER);

		assert (dv_get_health(dv,PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS 
																		-LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (dv_get_health (dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS- 
																		2*LIFE_LOSS_HUNTER_ENCOUNTER);
		assert (dv_get_score(dv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN );


		assert(dv_get_location(dv,PLAYER_LORD_GODALMING) == VIENNA);
		assert(dv_get_location(dv,PLAYER_DR_SEWARD) == VIENNA);
		assert(dv_get_location(dv,PLAYER_VAN_HELSING) == ZURICH);
		assert(dv_get_location(dv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(dv_get_location(dv,PLAYER_DRACULA) == VIENNA);

		puts ("passed");
		dv_drop (dv);

	}

	{
		puts ("\tTest for dracula encounter hunter on the sea");

		char *trail =
			"GED.... SBD.... HZU.... MBB.... DNS.... "
			"GNS.... SBD.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);

		assert (dv_get_round (dv) == 1);
		// does not affect anything
		assert (dv_get_health(dv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
		assert (dv_get_health (dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS- LIFE_LOSS_SEA);
		assert (dv_get_score(dv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN );



		assert(dv_get_location(dv,PLAYER_LORD_GODALMING) == NORTH_SEA);
		assert(dv_get_location(dv,PLAYER_DR_SEWARD) == BUDAPEST);
		assert(dv_get_location(dv,PLAYER_VAN_HELSING) == ZURICH);
		assert(dv_get_location(dv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(dv_get_location(dv,PLAYER_DRACULA) == NORTH_SEA);

		puts ("passed");
		dv_drop (dv);
	}
	{
		puts("\tTest for dracula encounter hunter after a hide move");
		char* trail =
			"GPR.... SBD.... HZU.... MBB.... DVI.V.. "
			"GPR.... SBD.... HZU.... MBB.... DHIT... "
			"GVITVD. SBD.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);


		{
			puts ("\t\tTest get move after legal moves");
			location_t start ,end;
			dv_get_player_move(dv,PLAYER_LORD_GODALMING,&start,&end);
			assert(start == PRAGUE);
			assert(end == VIENNA);
		}

		{
			puts ("\t\tTest get move for dracula \n\t\tAFTER he made a HIDE");
			location_t start ,end;
			dv_get_player_move(dv,PLAYER_DRACULA,&start,&end);
			assert(start == VIENNA);
			assert(end == HIDE);
		}
		assert (dv_get_round (dv) == 2);
		assert (dv_get_health(dv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS
																			-LIFE_LOSS_TRAP_ENCOUNTER
																			-LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (dv_get_health (dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS- LIFE_LOSS_HUNTER_ENCOUNTER);
		assert (dv_get_score(dv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN*2 );

		assert(dv_get_location(dv,PLAYER_LORD_GODALMING) == VIENNA);
		assert(dv_get_location(dv,PLAYER_DR_SEWARD) == BUDAPEST);
		assert(dv_get_location(dv,PLAYER_VAN_HELSING) == ZURICH);
		assert(dv_get_location(dv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(dv_get_location(dv,PLAYER_DRACULA) == VIENNA);

		{
			puts ("\tTest VIENA for get locale info");
			int n_traps,n_vamps;
			location_t where = VIENNA;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 0 && n_vamps == 0);
		}
		{
			puts ("\tTest other location for get locale info");
			int n_traps,n_vamps;
			location_t where = ZURICH;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 0 && n_vamps == 0);
		}
		puts ("passed");
		dv_drop (dv);
	}
	{
		puts("\tTest for dracula encounter hunter after a double back move");
		char* trail =
			"GPR.... SBD.... HZU.... MBB.... DVI.V.. "
			"GPR.... SBD.... HZU.... MBB.... DD1T... "
			"GVITVD. SBD.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);

		assert (dv_get_round (dv) == 2);
		assert (dv_get_health(dv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS
																			-LIFE_LOSS_TRAP_ENCOUNTER
																			-LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (dv_get_health (dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS- LIFE_LOSS_HUNTER_ENCOUNTER);
		assert (dv_get_score(dv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN*2 );

		assert(dv_get_location(dv,PLAYER_LORD_GODALMING) == VIENNA);
		assert(dv_get_location(dv,PLAYER_DR_SEWARD) == BUDAPEST);
		assert(dv_get_location(dv,PLAYER_VAN_HELSING) == ZURICH);
		assert(dv_get_location(dv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(dv_get_location(dv,PLAYER_DRACULA) == VIENNA);
		{
			puts ("\t\tTest get move for dracula \n\t\tAFTER he made a double back");
			location_t start ,end;
			dv_get_player_move(dv,PLAYER_DRACULA,&start,&end);
			assert(start == VIENNA);
			assert(end == DOUBLE_BACK_1);
		}

		puts ("passed");
		dv_drop (dv);
	}
	{
		puts("\tTest for dracula encounter hunter after a teleport move");
		char* trail =
			"GGA.... SBD.... HZU.... MBB.... DVI.V.. "
			"GGA.... SBD.... HZU.... MBB.... DTPT... "
			"GCDT.D. SBD.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);

		assert (dv_get_round (dv) == 2);
		assert (dv_get_health(dv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS
																			-LIFE_LOSS_TRAP_ENCOUNTER
																			-LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (dv_get_health (dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS+LIFE_GAIN_CASTLE_DRACULA
																			- LIFE_LOSS_HUNTER_ENCOUNTER);
		assert (dv_get_score(dv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN*2 );

		assert(dv_get_location(dv,PLAYER_LORD_GODALMING) == CASTLE_DRACULA);
		assert(dv_get_location(dv,PLAYER_DR_SEWARD) == BUDAPEST);
		assert(dv_get_location(dv,PLAYER_VAN_HELSING) == ZURICH);
		assert(dv_get_location(dv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(dv_get_location(dv,PLAYER_DRACULA) == CASTLE_DRACULA);
		{
			puts ("\tTest other location for get locale info");
			int n_traps,n_vamps;
			location_t where = VIENNA;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 0 && n_vamps == 1);
		}
		{
			puts ("\tTest other location for get locale info");
			int n_traps,n_vamps;
			location_t where = CASTLE_DRACULA;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 0 && n_vamps == 0);
		}

		{
			puts ("\t\tTest get move for dracula \n\t\tAFTER he made a TP");
			location_t start ,end;
			dv_get_player_move(dv,PLAYER_DRACULA,&start,&end);
			assert(start == VIENNA);
			assert(end == TELEPORT);
		}

		puts ("passed");
		dv_drop (dv);
	}
	{
		puts("\tTest for HUNTER vanish a vampire");
		char* trail =
			"GPR.... SBD.... HZU.... MBB.... DVI.V.. "
			"GPR.... SBD.... HZU.... MBB.... DTPT... "
			"GVIV... SBD.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);

		assert (dv_get_round (dv) == 2);
		assert (dv_get_health(dv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS
																			);
		assert (dv_get_health (dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS +LIFE_GAIN_CASTLE_DRACULA);
		assert (dv_get_score(dv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN*2 );

		assert(dv_get_location(dv,PLAYER_LORD_GODALMING) == VIENNA);
		assert(dv_get_location(dv,PLAYER_DR_SEWARD) == BUDAPEST);
		assert(dv_get_location(dv,PLAYER_VAN_HELSING) == ZURICH);
		assert(dv_get_location(dv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(dv_get_location(dv,PLAYER_DRACULA) == CASTLE_DRACULA);


		puts ("passed");
		dv_drop (dv);
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
		DraculaView dv = dv_new (trail, messages);

		assert (dv_get_round (dv) == 5);
		assert (dv_get_health(dv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS
																			-LIFE_LOSS_TRAP_ENCOUNTER*3
																			);
		assert (dv_get_health (dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert (dv_get_score(dv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN*5 );


		{
			puts ("\t\tTest get move  for dracula \n\t\tAFTER he made a HI and DB");
			location_t start ,end;
			dv_get_player_move(dv,PLAYER_DRACULA,&start,&end);
			assert(start == DOUBLE_BACK_1);
			assert(end == PRAGUE);
		}
		assert(dv_get_location(dv,PLAYER_LORD_GODALMING) == ZAGREB);
		assert(dv_get_location(dv,PLAYER_DR_SEWARD) == BUDAPEST);
		assert(dv_get_location(dv,PLAYER_VAN_HELSING) == ZURICH);
		assert(dv_get_location(dv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(dv_get_location(dv,PLAYER_DRACULA) == PRAGUE);

		puts ("passed");
		dv_drop (dv);
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
		DraculaView dv = dv_new (trail, messages);

		assert (dv_get_round (dv) == 3);
		assert (dv_get_health(dv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
		// be sent the hospital
		assert (dv_get_health(dv, PLAYER_DR_SEWARD) <= 0);// die in that turn
		// encounter hunters for four times
		assert (dv_get_health (dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS- 4*LIFE_LOSS_HUNTER_ENCOUNTER);
		assert (dv_get_score(dv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN*3-SCORE_LOSS_HUNTER_HOSPITAL*2 );

		{
			puts ("\tTest other location for get locale info");
			int n_traps,n_vamps;
			location_t where = VIENNA;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 0 && n_vamps == 0);
		}
		{
			puts ("\tTest other location for get locale info");
			int n_traps,n_vamps;
			location_t where = PRAGUE;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 0 && n_vamps == 0);
		}

		assert(dv_get_location(dv,PLAYER_LORD_GODALMING) == HOSPITAL_LOCATION);

		printf("%d %s\n",dv_get_health(dv, PLAYER_DR_SEWARD),location_get_name(dv_get_location(dv,PLAYER_DR_SEWARD)));
		assert(dv_get_location(dv,PLAYER_DR_SEWARD) == HOSPITAL_LOCATION);
		assert(dv_get_location(dv,PLAYER_VAN_HELSING) == ZURICH);
		assert(dv_get_location(dv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(dv_get_location(dv,PLAYER_DRACULA) == VIENNA);
		{
			puts ("\t\tTest get move for hunter after he die \n\t\tand revived");
			location_t start ,end;
			dv_get_player_move(dv,PLAYER_LORD_GODALMING,&start,&end);
			assert(start == PRAGUE);
			assert(end == ST_JOSEPH_AND_ST_MARYS);
		}
		{
			puts ("\t\tTest get move for hunter after he die \n\t\t");
			location_t start ,end;
			dv_get_player_move(dv,PLAYER_DR_SEWARD,&start,&end);
			assert(start == PRAGUE);
			assert(end == VIENNA);
		}

		puts ("passed");
		dv_drop (dv);
	}

	{
		puts("\tTest for hunter go to the hospital intentionally to hospital");
		char* trail =
			"GVI.... SBD.... HZU.... MBB.... DZA.V.. "
			"GZAVD.. SBD.... HZU.... MBB.... DHIT... "
			"GJM.... SBD.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);

		assert (dv_get_round (dv) == 2);
		assert (dv_get_health(dv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS-LIFE_LOSS_DRACULA_ENCOUNTER
																			);
		assert (dv_get_health (dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS -LIFE_LOSS_HUNTER_ENCOUNTER);
		assert (dv_get_score(dv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN*2 );

		assert(dv_get_location(dv,PLAYER_LORD_GODALMING) == HOSPITAL_LOCATION);
		assert(dv_get_location(dv,PLAYER_DR_SEWARD) == BUDAPEST);
		assert(dv_get_location(dv,PLAYER_VAN_HELSING) == ZURICH);
		assert(dv_get_location(dv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(dv_get_location(dv,PLAYER_DRACULA) == ZAGREB);
		{
			puts ("\tTest other location for get locale info");
			int n_traps,n_vamps;
			location_t where = ZAGREB;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 1 && n_vamps == 0);
		}
		{
			puts ("\t\tTest get move for hunter after he go to hospital\n\t\t");
			location_t start ,end;
			dv_get_player_move(dv,PLAYER_LORD_GODALMING,&start,&end);
			assert(start == ZAGREB);
			assert(end == ST_JOSEPH_AND_ST_MARYS);
		}

		puts ("passed");
		dv_drop (dv);
	}

	


}

void test_encouner (void){

	{
		puts ("Test for puting 3 encounters in one city");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DZA.V.. "
			"GST.... SAO.... HZU.... MBB.... DD1T... "
			"GST.... SAO.... HZU.... MBB.... DHIT... "
			"GST.... SAO.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);

		{
			puts ("\tTest 3 encounters info");
			int n_traps,n_vamps;
			location_t where = ZAGREB;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 2 && n_vamps == 1);
		}
		dv_drop (dv);
	}
	{
		puts ("Test for puting 3 encounters in TWO city");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DZA.V.. "
			"GST.... SAO.... HZU.... MBB.... DD1T... "
			"GST.... SAO.... HZU.... MBB.... DHIT... "
			"GST.... SAO.... HZU.... MBB.... DBDT... "
			"GST.... SAO.... HZU.... MBB.... DVIT... "
			"GST.... SAO.... HZU.... MBB.... DPRT... "
			"GST.... SAO.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);

		{
			puts ("\tTest 3 encounters info");
			int n_traps,n_vamps;
			location_t where = ZAGREB;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 2 && n_vamps == 1);
		}
		{
			puts ("\tTest 3 encounters info");
			int n_traps,n_vamps;
			location_t where = VIENNA;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 1 && n_vamps == 0);
		}
		dv_drop (dv);
	}
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
		DraculaView dv = dv_new (trail, messages);
		assert(dv_get_score(dv)==GAME_START_SCORE- 7* SCORE_LOSS_DRACULA_TURN- 
										SCORE_LOSS_VAMPIRE_MATURES);
		{
			puts ("\tTest 3 encounters info");
			int n_traps,n_vamps;
			location_t where = ZAGREB;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 2 && n_vamps == 0);
		}
		dv_drop (dv);
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
		DraculaView dv = dv_new (trail, messages);
		assert(dv_get_score(dv)==GAME_START_SCORE- 8* SCORE_LOSS_DRACULA_TURN- 
										SCORE_LOSS_VAMPIRE_MATURES);
		{
			puts ("\tTest 3 encounters info");
			int n_traps,n_vamps;
			location_t where = ZAGREB;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 1 && n_vamps == 0);
		}
		{
			puts ("\tTest 3 encounters info");
			int n_traps,n_vamps;
			location_t where = VIENNA;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 1 && n_vamps == 0);
		}
		{
			puts ("\tTest 3 encounters info");
			int n_traps,n_vamps;
			location_t where = BUDAPEST;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 1 && n_vamps == 0);
		}
		{
			location_t history[TRAIL_SIZE];
			dv_get_trail (dv,PLAYER_DRACULA,history);
			assert(history[5] == ZAGREB);
			assert(history[4] == BUDAPEST);
			assert(history[3] == VIENNA);
			assert(history[2] == PRAGUE);
			assert(history[1] == BERLIN);
			assert(history[0] == HAMBURG);
		}
		dv_drop (dv);
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
		DraculaView dv = dv_new (trail, messages);
		assert(dv_get_score(dv)==GAME_START_SCORE- 6* SCORE_LOSS_DRACULA_TURN);
		{
			puts ("\tTest 3 encounters info");
			int n_traps,n_vamps;
			location_t where = ZAGREB;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 0 && n_vamps == 0);
		}
		{
			puts ("\tTest 3 encounters info");
			int n_traps,n_vamps;
			location_t where = VIENNA;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 2 && n_vamps == 0);
		}
		{
			puts ("\tTest 3 encounters info");
			int n_traps,n_vamps;
			location_t where = MUNICH;
			dv_get_locale_info(dv,where,&n_traps,&n_vamps);
			assert(n_traps == 0 && n_vamps == 0);
		}
		{
			location_t history[TRAIL_SIZE];
			dv_get_trail (dv,PLAYER_DRACULA,history);
			assert(history[5] == ZAGREB);
			assert(history[4] == ZAGREB);
			assert(history[3] == MUNICH);
			assert(history[2] == VIENNA);
			assert(history[1] == VIENNA);
			assert(history[0] == PRAGUE);
			
		}
		{
			location_t history[TRAIL_SIZE];
			dv_get_trail (dv,PLAYER_LORD_GODALMING,history);
			assert(history[5] == ZURICH);
			assert(history[4] == ZURICH);
			assert(history[3] == ZURICH);
			assert(history[2] == ZURICH);
			assert(history[1] == ZURICH);
			assert(history[0] == MUNICH);
			
		}
		dv_drop (dv);
	}
}

void test_trail_move (void){
	{
		puts ("\tTest for no dracula move");

		char *trail =
			"GST.... SAO.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);
		
		{
			location_t history[TRAIL_SIZE];
			dv_get_trail (dv,PLAYER_DRACULA,history);
			for( int n=0; n<TRAIL_SIZE; n++){
				assert(history[n] == UNKNOWN_LOCATION);
			}
		}
		{
			location_t history[TRAIL_SIZE];
			dv_get_trail (dv,PLAYER_LORD_GODALMING,history);
			history[0] = STRASBOURG; 
			for( int n=1; n<TRAIL_SIZE; n++){
				assert(history[n] == UNKNOWN_LOCATION);
			}
		}
		{
			puts ("\tTest get move at first round for dracula before he made a move");
			location_t start ,end;
			dv_get_player_move(dv,PLAYER_DRACULA,&start,&end);
			assert(start == UNKNOWN_LOCATION);
			assert(end == UNKNOWN_LOCATION);
		}
		puts ("passed");
		dv_drop (dv);
	}

	{
		puts ("\tTest for dracula first move");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DZA.V.. "
			"GST.... SAO.... HZU.... MBB.... DHIT... "
			"GST.... SAO.... HZU.... MBB.... DD1T... "
			"GST.... SAO.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);
		
		{
			location_t history[TRAIL_SIZE];
			dv_get_trail (dv,PLAYER_DRACULA,history);
			assert(history[2] == ZAGREB);
			assert(history[1] == ZAGREB);
			assert(history[0] == ZAGREB);

			for( int n=3; n<TRAIL_SIZE; n++){
				assert(history[n] == UNKNOWN_LOCATION);
			}
		}
		{
			puts ("\tTest get move at first round for dracula before he made a move");
			location_t start ,end;
			dv_get_player_move(dv,PLAYER_DRACULA,&start,&end);
			assert(start == HIDE);
			assert(end == DOUBLE_BACK_1);
		}

		puts ("passed");
		dv_drop (dv);
	}
	{
		puts ("\tTest for dracula first move");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DZA.V.. "
			"GST.... SAO.... HZU.... MBB.... DHIT... "
			"GST.... SAO.... HZU.... MBB.... DD1T... "
			"GST.... SAO.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);
		
		{
			location_t history[TRAIL_SIZE];
			dv_get_trail (dv,PLAYER_DRACULA,history);
			assert(history[2] == ZAGREB);
			assert(history[1] == ZAGREB);
			assert(history[0] == ZAGREB);

			for( int n=3; n<TRAIL_SIZE; n++){
				assert(history[n] == UNKNOWN_LOCATION);
			}
		}


		puts ("passed");
		dv_drop (dv);
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
		DraculaView dv = dv_new (trail, messages);

		{
			location_t history[TRAIL_SIZE];
			dv_get_trail (dv,PLAYER_LORD_GODALMING,history);
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
			dv_get_trail (dv,PLAYER_DR_SEWARD,history);
			assert(history[3] == PRAGUE);
			assert(history[2] == VIENNA);
			assert(history[1] == PRAGUE);
			assert(history[0] == VIENNA);
			assert(dv_get_location(dv,PLAYER_DR_SEWARD) == HOSPITAL_LOCATION);
			for( int n=4; n<TRAIL_SIZE; n++){
				assert(history[n] == UNKNOWN_LOCATION);
			}
		}
		{
			puts ("\tTest FOR HUNTER died and revived");
			location_t start ,end;
			dv_get_player_move(dv,PLAYER_LORD_GODALMING,&start,&end);
			assert(start == PRAGUE);
			assert(end == SZEGED);
		}
		{
			puts ("\tTest FOR HUNTER died ");
			location_t start ,end;
			dv_get_player_move(dv,PLAYER_DR_SEWARD,&start,&end);
			assert(start == PRAGUE);
			assert(end == VIENNA);
		}
		{
			puts ("\tTest get move at first round for dracula before he made a move");
			location_t start ,end;
			dv_get_player_move(dv,PLAYER_DRACULA,&start,&end);
			assert(start == PRAGUE);
			assert(end == DOUBLE_BACK_2);
		}

		puts ("passed");
		dv_drop (dv);
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
		DraculaView dv = dv_new (trail, messages);
		
		{
			{
				size_t num;
				location_t* dest = dv_get_dests(dv,&num,false,false);
				assert(num == 0);
				assert(dest == NULL);
				
			}
			{
				size_t num;
				location_t* dest = dv_get_dests(dv,&num,true,false);
				assert(num == 0);
				assert(dest == NULL);
				
			}
			{
				size_t num;
				location_t* dest = dv_get_dests(dv,&num,false,true);
				assert(num == 0);
				assert(dest == NULL);
				
			}
			{
				size_t num;
				location_t* dest = dv_get_dests(dv,&num,true,true);
				assert(num == 0);
				assert(dest == NULL);
			}
		}

		{
			puts("\t rail 1 connection");
			size_t num;
			// Round num is 0 , player is 0 , rail is 0
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_LORD_GODALMING,true,true,true);
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
			puts("\t rail 2 connection");
			size_t num;
			// Round num is 0 , player is 0 , rail is 0
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_DR_SEWARD,true,true,true);
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
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_VAN_HELSING,true,true,false);
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

		{
			puts("\t rail 0 connection");
			size_t num;
			// Round num is 0 , player is 3 , round is 1,rail is 0
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_MINA_HARKER,true,true,false);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++)
				seen[dest[i]] = true;

			assert (num == 5); 
			assert (seen[VIENNA]); 
			assert (seen[PRAGUE]); 
			assert (seen[ZAGREB]); 
			assert (seen[MUNICH]);
			assert (seen[BUDAPEST]);
			free(dest);
		}
		dv_drop(dv);
	}
	{
		puts("hunter connection test: first round");
		
		char* trail =
		"GMA.... SMA.... HMA.... MMA....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);
		{
			puts("\tNO rail connection for lord goldming (0 + 1)%%4 =1");
			size_t num;
			
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_LORD_GODALMING,true,true,true);
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
			puts("\tno extra rail connection for dr s (1+ 1)%%4 =2");
			size_t num;
			
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_DR_SEWARD,true,true,true);
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
			puts("\textra connection for van h (1 + 2)%%4 =3");
			size_t num;
			
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_VAN_HELSING,true,true,true);
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
		{
			puts("\textra connection for mina (1 + 3)%%4 =0");
			size_t num;
			
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_MINA_HARKER,true,true,true);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
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
		dv_drop(dv);

	}
	
	{
		puts("hunter connection test: SECOND round, port city");
		
		char* trail =
		"GAL.... SAL.... HAL.... MAL.... DBI.V.. "
		"GAL.... SAL.... HAL.... MAL....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);
		{
			size_t num;
			
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_DRACULA,false,false,false);
			assert (num == 1);
			bool seen[NUM_MAP_LOCATIONS] = {false};
			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			assert (seen[BARI]); 
			free(dest);
		}
		{
			puts("\tonly road connection");
			size_t num;
			
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_LORD_GODALMING,true,false,false);
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
			// 2 + 0 % 2 =2
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_LORD_GODALMING,false,true,false);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			assert (num == 6);

			assert (seen[ALICANTE]); 
			assert (seen[MADRID]);
			assert (seen[BARCELONA]);
			assert (seen[LISBON]);
			assert (seen[SANTANDER]);
			assert (seen[SARAGOSSA]);
			free(dest);
			
		}
		{
			puts("\tonly sea");
			size_t num;
			
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_LORD_GODALMING,false,false,true);
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
			
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_LORD_GODALMING,true,true,true);
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
			assert (seen[LISBON]);
			assert (seen[SANTANDER]);
			assert (seen[SARAGOSSA]);
			free(dest);
		}


		{
			puts("\tall three connection : rail distance 3 from alicante");
			size_t num;
			
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_DR_SEWARD,true,true,true);
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
			// RAIL 3
			assert (seen[BORDEAUX]);
			free(dest);
			

		}
		{
			puts("\tall three connection : rail distance 0 from alicante");
			size_t num;
			
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_VAN_HELSING,true,true,true);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			assert (num == 5);

			assert (seen[ALICANTE]); 
			assert (seen[MEDITERRANEAN_SEA]);
			assert (seen[MADRID]);
			assert (seen[GRANADA]);
			assert (seen[SARAGOSSA]);
			
			free(dest);
		}
		{
			puts("\trail only : rail distance 0 from alicante");
			size_t num;
			
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_VAN_HELSING,false,true,false);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			assert (num == 1);
			assert (seen[ALICANTE]); 
			free(dest);
		}
		dv_drop(dv);
	}


	{
		puts("Dracula connection1");
		
		char* trail =
		"GVI.... SVI.... HVI.... MVI.... DMN.V.. "
		"GVI.... SVI.... HVI.... MVI.... DLOT... "
		"GVI.... SVI.... HVI.... MVI.... DECT... "
		"GVI.... SVI.... HVI.... MVI.... DAOT... "
		"GVI.... SVI.... HVI.... MVI.... DIRT... "
		"GVI.... SVI.... HVI.... MVI.... DSWT... "
		"GVI.... SVI.... HVI.... MVI....";
	
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);
		{
			puts("\tNO CONNECTION");
			size_t num;
			
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_DRACULA,true,true,true);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			assert (num == 2);

			assert (seen[SWANSEA]); 
			assert (seen[LIVERPOOL]); 
			free(dest);
		}
		dv_drop(dv);

	}
	{
		puts("Dracula connection2");
		
		char* trail =
		"GVI.... SVI.... HVI.... MVI.... DMN.V.. "
		"GVI.... SVI.... HVI.... MVI.... DLOT... "
		"GVI.... SVI.... HVI.... MVI.... DECT... "
		"GVI.... SVI.... HVI.... MVI.... DAOT... "
		"GVI.... SVI.... HVI.... MVI.... DIRT... "
		"GVI.... SVI.... HVI.... MVI.... DSWT... "
		"GVI.... SVI.... HVI.... MVI....";
	
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);
		{
			puts("\tALL CONNECTION");
			size_t num;
			
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_DRACULA,true,true,true);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			assert (num == 2);

			assert (seen[SWANSEA]); 
			assert (seen[LIVERPOOL]); 
			free(dest);
		}
		{
			puts("\tNO SEA CONNECTION3");
			size_t num;
			
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_DRACULA,true,true,false);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			assert (num == 2);

			assert (seen[SWANSEA]); 
			assert (seen[LIVERPOOL]); 
			free(dest);
		}
		dv_drop(dv);
	}
	{
		puts("Dracula connection4");
		
		char* trail =
		"GVI.... SVI.... HVI.... MVI.... DCOV... "
		"GVI.... SVI.... HVI.... MVI.... DFRT... "
		"GVI.... SVI.... HVI.... MVI.... DSTT... "
		"GVI.... SVI.... HVI.... MVI.... DZUT... "
		"GVI.... SVI.... HVI.... MVI.... DHIT... "
		"GVI.... SVI.... HVI.... MVI.... DMIT... "
		"GVI.... SVI.... HVI.... MVI.... DD4T.V. "
		"GVI.... SVI.... HVI.... MVI....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);
		{
			puts("\tALL CONNECTION");
			size_t num;
			
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_DRACULA,true,true,true);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			assert (num == 8);
			assert( seen[ STRASBOURG]== true);
			// current position
			assert( seen[ GENEVA]== true);
			assert( seen[ PARIS]== true);
			assert( seen[ BRUSSELS]== true);
			assert( seen[ COLOGNE]== true);
			assert( seen[ FRANKFURT]== true);
			// special case when the 6th position can be consider
			assert( seen[ NUREMBURG]== true);
			assert( seen[ MUNICH]== true);		
			free(dest);
		}
		dv_drop(dv);
	}
	{
		puts("Dracula connection5");
		
		char* trail =
		"GVI.... SVI.... HVI.... MVI.... DSNV... "
		"GVI.... SVI.... HVI.... MVI....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);
		{
			
			size_t num;
			puts("\tonly sea");
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_DRACULA,false,false,true);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			assert (num == 2);
			assert( seen[BAY_OF_BISCAY]== true);
			assert( seen[SANTANDER]== true);
			free(dest);
		}
		{
			
			size_t num;
			puts("\tall connection");
			location_t* dest = dv_get_dests_player(dv,&num,PLAYER_DRACULA,true,false,true);
			bool seen[NUM_MAP_LOCATIONS] = {false};

			for (size_t i = 0; i < num; i++){
				seen[dest[i]] = true;
			}
			assert (num == 5);
			assert( seen[BAY_OF_BISCAY]== true);
			assert( seen[SANTANDER]== true);
			assert( seen[SARAGOSSA]== true);
			assert( seen[MADRID]== true);
			assert( seen[LISBON]== true);
			free(dest);
		}
		dv_drop(dv);
	}
	{
		puts("Dracula cant go to hospital");
		
		char* trail =
		"GVI.... SVI.... HVI.... MVI.... DSZV... "
		"GVI.... SVI.... HVI.... MVI....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);
		size_t num;
		location_t* dest = dv_get_dests_player(dv,&num,PLAYER_DRACULA,true,false,true);
		bool seen[NUM_MAP_LOCATIONS] = {false};
		for (size_t i = 0; i < num; i++){
			seen[dest[i]] = true;
		}
		assert (num == 5);
		assert( seen[SZEGED]== true);
		assert( seen[BELGRADE]== true);
		assert( seen[ZAGREB]== true);
		assert( seen[BUDAPEST]== true);
		assert( seen[KLAUSENBURG]== true);

		free(dest);
		dv_drop(dv);
	}




}



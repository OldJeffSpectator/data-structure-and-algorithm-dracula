////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// test_hunter_view.c: test the HunterView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-02	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hunter_view.h"

int main (void)
{
	do {////////////////////////////////////////////////////////////////
		puts ("Test basic empty initialisation");
		char *trail = "";
		player_message messages[] = {}; 
		HunterView hv = hv_new (trail, messages);

		assert (hv_get_player (hv) == PLAYER_LORD_GODALMING);
		assert (hv_get_round (hv) == 0);
		assert (hv_get_health (hv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
		assert (hv_get_health (hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert (hv_get_score (hv) == GAME_START_SCORE);
		assert (hv_get_location (hv, PLAYER_LORD_GODALMING) == UNKNOWN_LOCATION);

		puts ("passed");
		hv_drop (hv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula trail and basic functions");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DC?....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah" };
		HunterView hv = hv_new (trail, messages);

		assert (hv_get_player (hv) == PLAYER_LORD_GODALMING);
		assert (hv_get_round (hv) == 1);
		assert (hv_get_location (hv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert (hv_get_location (hv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert (hv_get_location (hv, PLAYER_VAN_HELSING) == ZURICH);
		assert (hv_get_location (hv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert (hv_get_location (hv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert (hv_get_health (hv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS);

		puts ("passed");
		hv_drop (hv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for encountering Dracula and hunter history");
		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.... "
			"GGED...";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!" };
		HunterView hv = hv_new (trail, messages);

		assert (hv_get_location (hv, PLAYER_DRACULA) == GENEVA);
		assert (hv_get_health (hv, PLAYER_LORD_GODALMING) == 5);
		assert (hv_get_health (hv, PLAYER_DRACULA) == 30);
		assert (hv_get_location (hv, PLAYER_LORD_GODALMING) == GENEVA);

		location_t history[TRAIL_SIZE];
		hv_get_trail (hv, PLAYER_DRACULA, history);
		assert (history[0] == GENEVA);
		assert (history[1] == UNKNOWN_LOCATION);

		hv_get_trail (hv, PLAYER_LORD_GODALMING, history);
		assert (history[0] == GENEVA);
		assert (history[1] == STRASBOURG);
		assert (history[2] == UNKNOWN_LOCATION);

		hv_get_trail (hv, PLAYER_DR_SEWARD, history);
		assert (history[0] == ATLANTIC_OCEAN);
		assert (history[1] == UNKNOWN_LOCATION);

		puts ("passed");
		hv_drop (hv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula doubling back at sea, "
			  "and losing blood points (Hunter View)");
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DS?.... "
			"GST.... SST.... HST.... MST.... DD1....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go" };
		HunterView hv = hv_new (trail, messages);

		assert (hv_get_player (hv) == 0);
		assert (hv_get_location (hv, PLAYER_DRACULA) == DOUBLE_BACK_1);
		assert (hv_get_health (hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 4);

		location_t history[TRAIL_SIZE];
		hv_get_trail (hv, PLAYER_DRACULA, history);
		assert (history[0] == DOUBLE_BACK_1);
		assert (history[1] == SEA_UNKNOWN);

		puts ("passed");
		hv_drop (hv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Checking Galatz road connections");
		char *trail = "GGA....";
		player_message messages[] = {"Gone to Galatz"};
		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, true, false, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};
		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 5); // including Rome itself
		assert (seen[GALATZ]);
		assert (seen[CONSTANTA]);
		assert (seen[BUCHAREST]);
		assert (seen[KLAUSENBURG]);
		assert (seen[CASTLE_DRACULA]);

		puts ("passed");
		free (edges);
		hv_drop (hv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Checking Ionian Sea sea connections");
		char *trail = "GIO....";
		player_message messages[] = {"Sailing the Ionian"};
		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, false, false, true);
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
		hv_drop (hv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Checking Athens rail connections (none)");

		char *trail = "GAT....";
		player_message messages[] = {"Leaving Athens by train"};
		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, false, true, false);

		assert (n_edges == 1);
		assert (edges[0] == ATHENS);

		puts ("passed");
		free (edges);
		hv_drop (hv);
	} while (0);





// ======================= LOCATION BASED TESTS ============================ 


	{
		puts ("get_dests_player when no moves have been made"); 



		char *trail =
			""; 
		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, true, true, true);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 0); 
		assert (edges == NULL); 

		puts ("passed");
		free (edges);
		hv_drop (hv);









	}

	{
		puts ("Checking Rome total possible moves (5)");

		char *trail =
			"GRO...."; 
		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, true, true, true);
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
		hv_drop (hv);


	} 



	{
	puts ("Checking Rome road connections (4)");

		char *trail =
			"GRO...."; 
		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, true, false, false);
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
		hv_drop (hv);


	}



	{
	puts ("Checking Rome sea connections (2)");

		char *trail =
			"GRO...."; 
		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, false, false, true);
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
		hv_drop (hv);


	}


	{
	puts ("Checking Madrid total possible moves (7)");

		char *trail =
			"GMA...."; 
		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, true, true, true);
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
		hv_drop (hv);


	}



	{
	puts ("Checking Madrid sea connections (1)");

		char *trail =
			"GMA...."; 
		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, false, false, true);
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
		hv_drop (hv);


	}


	{
	puts ("Checking Madrid road connections (7)");

		char *trail =
			"GMA...."; 
		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, true, false, false);
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
		hv_drop (hv);


	}



	{
	puts ("Checking Madrid rail connections (rail unavailable)");

		char *trail =
			"GMA.... SAO.... HMN.... MMA.... DC?V...";

		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_MINA_HARKER, false, true, false);
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
		hv_drop (hv);


	}

	{
	puts ("Checking Madrid rail connections (one rail move available)");

		char *trail =
			"GMA.... SAO.... HMN.... MPR.... DVIV...";

		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, false, true, false);
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
		hv_drop (hv);



	}

	


	{
	puts ("Checking Bucharest rail connections (rail unavailable)");

		char *trail =
			"GMA.... SAO.... HMN.... MBC.... DC?V...";

		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_MINA_HARKER, false, true, false);
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
		hv_drop (hv);


	}

	{
	puts ("Checking Bucharest rail connections (one rail move available)");

		char *trail =
			"GBC.... SAO.... HMN.... MPR.... DVIV...";
			
		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, false, true, false);

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
		hv_drop (hv);



	}

	{
	puts ("Checking Manchester total possible moves (4)");

		char *trail =
			"GMN...."; 

		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, true, true, true);
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
		hv_drop (hv);

	}

	{

	puts ("Checking Manchester rail connections (rail is unavailable) (none)");

		char *trail =
			"GMA.... SAO.... HMN.... MMN.... DC?V...";
			
		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_MINA_HARKER, false, true, false);
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
		hv_drop (hv);

	}


	{


	puts ("Checking Manchester rail connections (one rail move is available) (3)");

		char *trail =
			"GMN.... SMN.... HMN.... MMN.... DC?.V.."; 

		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, false, true, false);
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
		hv_drop (hv);







	}

	{



	puts ("Checking Paris rail connections (rail is unavailable)");

		char *trail =
			"GMA.... SAO.... HMN.... MPA.... DC?V...";

		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_MINA_HARKER, false, true, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 1); // including Paris itself
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
		hv_drop (hv);



	}


	{



	puts ("Checking Paris rail connections (one move rail is available)");

		char *trail =
			"GPA.... SPA...."; 

		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, false, true, false);
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
		hv_drop (hv);



	}



	{



	puts ("Checking Paris rail connections (two move rail is available)");

		char *trail =
			"GPA.... SPA.... HPA...."; 

		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_DR_SEWARD, false, true, false);
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
		hv_drop (hv);


	}


	{


	puts ("Checking Paris rail connections (three move rail is available)");

		char *trail =
			"GPA.... SPA.... HPA.... MPA.... DC?V..."; 

		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_VAN_HELSING, false, true, false);
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
		hv_drop (hv);




	}

	{


	puts ("Hunter is only allowed one move if considering both rail & road (one rail move available)");

		char *trail =
			"GPA.... SPA...."; 

		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, true, true, false);
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
		hv_drop (hv);



	}

	{



	puts ("Checking Vienna rail connections (rail is unavailable)");

		char *trail =
			"GMA.... SAO.... HMN.... MVI.... DC?V...";

		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_MINA_HARKER, false, true, false);
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
		hv_drop (hv);



	}


	{



	puts ("Checking Vienna rail connections (one move rail is available)");

		char *trail =
			"GVI.... SVI...."; 

		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, false, true, false);
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
		hv_drop (hv);



	}



	{



	puts ("Checking Vienna rail connections (two move rail is available)");

		char *trail =
			"GVI.... SVI.... HVI...."; 

		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_DR_SEWARD, false, true, false);
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
		hv_drop (hv);


	}


	{


	puts ("Checking Vienna rail connections (three move rail is available)");

		char *trail =
			"GVI.... SVI.... HVI.... MVI.... DC?V..."; 

		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_VAN_HELSING, false, true, false);
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
		hv_drop (hv);




	}

	{

	puts ("Checking Vienna rail + road connections (three move rail is available)");

		char *trail =
			"GVI.... SVI.... HVI.... MVI.... DC?V..."; 

		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_VAN_HELSING, true, true, false);
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
		hv_drop (hv);


	}



	{
	puts ("Dracula should not be able to take a rail");

		char *trail =
			"GVI.... SZA.... HMA.... MAO.... DPAV..."; 

		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_DRACULA, true, true, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 7); // including Paris itself
		assert (seen[PARIS]); 
		assert (seen[LE_HAVRE]);
		assert (seen[NANTES]); 
		assert (seen[CLERMONT_FERRAND]); 
		assert (!seen[BORDEAUX]);
		assert (!seen[MARSEILLES]);
		assert (seen[BRUSSELS]);



		puts ("passed");
		free (edges);
		hv_drop (hv);

	}

	{
	puts ("Dracula get_connections should not include cities in"
		  " Dracula's trail (since he cannot backtrack unless Double back)");

		char *trail =
			"GVI.... SZA.... HMA.... MAO.... DPAV... "
			"GVI.... SZA.... HMA.... MAO.... DCFT..."; 

		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_DRACULA, true, false, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 6); // including Clermont itself
		assert (seen[CLERMONT_FERRAND]); 
		assert (!seen[PARIS]);
		assert (seen[NANTES]); 
		assert (seen[TOULOUSE]); 
		assert (seen[MARSEILLES]);
		assert (seen[GENEVA]);
		assert (seen[BORDEAUX]);



		puts ("passed");
		free (edges);
		hv_drop (hv);

	}




	{
	puts ("Checking Bay of Biscay total possible moves (6)");

		char *trail =
			"GBB...."; 
		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, false, false, true);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 5); // including Bay of Biscay itself
		assert (seen[BAY_OF_BISCAY]); 
		assert (seen[NANTES]);
		assert (seen[BORDEAUX]); 
		assert (seen[SANTANDER]); 
		assert (seen[ATLANTIC_OCEAN]);



		puts ("passed");
		free (edges);
		hv_drop (hv);


	}




	{
	puts ("Checking Mediteranian Sea total possible moves (7)");

		char *trail =
			"GMS...."; 
		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, true, true, true);
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
		hv_drop (hv);


	} 

	{

	puts ("Checking Adriatic Sea total possible moves (4)");

		char *trail =
			"GAS...."; 
		player_message messages[] = {
			"Hello"};

		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, true, true, true);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 4); // including Adratic sea itself
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
		hv_drop (hv);



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

		HunterView hv = hv_new (trail, messages);

		assert (hv_get_round (hv) == 1);
		assert (hv_get_health(hv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (hv_get_health (hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - LIFE_LOSS_HUNTER_ENCOUNTER);
		assert (hv_get_score(hv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN );

		puts ("passed");
		hv_drop (hv);

	}

	{
		puts ("Test for dracula encounter TWO hunters on the land");

		char *trail =
			"GPR.... SBD.... HZU.... MBB.... DVI.V.. "
			"GVIVD.. SVIVD..";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};

		HunterView hv = hv_new (trail, messages);

		assert (hv_get_round (hv) == 1);
		assert (hv_get_health (hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 2*LIFE_LOSS_HUNTER_ENCOUNTER);
		assert (hv_get_health(hv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (hv_get_health(hv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (hv_get_score(hv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN );



		puts ("passed");
		hv_drop (hv);
		
	}

	{

		puts ("Test for dracula encounter FOUR hunters on the land");

		char *trail =
			"GPR.... SBD.... HZU.... MBB.... DVI.V.. "
			"GVIVD.. SVID... HVID... MVID... DC?T... ";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};

		HunterView hv = hv_new (trail, messages);

		assert (hv_get_round (hv) == 2);
		assert (hv_get_health (hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 4*LIFE_LOSS_HUNTER_ENCOUNTER);
		assert (hv_get_health(hv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (hv_get_health(hv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (hv_get_health(hv, PLAYER_MINA_HARKER) == GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (hv_get_health(hv, PLAYER_VAN_HELSING) == GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER);
		assert (hv_get_score(hv) == GAME_START_SCORE - 2* SCORE_LOSS_DRACULA_TURN );
		assert (hv_get_health (hv, PLAYER_DRACULA) <= 0); 



		puts ("passed");
		hv_drop (hv);


	}  

	{
		puts ("Test for dracula & hunter encounter at sea (no encounter occurs)");

		char *trail =
			"GIR.... SBD.... HZU.... MBB.... DAO.... " // assume we know dracula location
			"GAO....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};

		HunterView hv = hv_new (trail, messages);
		location_t history[TRAIL_SIZE]; 

		hv_get_trail (hv, PLAYER_DRACULA, history); 

		// Test history
		assert (history[0] == ATLANTIC_OCEAN); 
		assert (history[1] == UNKNOWN_LOCATION); 
		assert (history[2] == UNKNOWN_LOCATION);  
		assert (history[3] == UNKNOWN_LOCATION); 
		assert (history[4] == UNKNOWN_LOCATION); 
		assert (history[5] == UNKNOWN_LOCATION);  

		assert (hv_get_health (hv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS); 
		assert (hv_get_health(hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - LIFE_LOSS_SEA);
		assert (hv_get_score(hv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN );

		puts ("passed");
		hv_drop (hv);

	}

	{
		puts("Testing hunter eliminating immature vampire");
		char* trail =
			"GPR.... SBD.... HZU.... MBB.... DVI.V.. "
			"GPR.... SBD.... HZU.... MBB.... DTPT... "
			"GVI.V..";

		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};

		HunterView hv = hv_new (trail, messages);
		location_t history[TRAIL_SIZE]; 

		hv_get_trail (hv, PLAYER_LORD_GODALMING, history); 

		// Testing history
		assert (history[0] == VIENNA); 
		assert (history[1] == PRAGUE); 
		assert (history[2] == PRAGUE);  
		assert (history[3] == UNKNOWN_LOCATION); 
		assert (history[4] == UNKNOWN_LOCATION); 
		assert (history[5] == UNKNOWN_LOCATION);  

		// Testing stats
		assert (hv_get_round (hv) == 2);
		assert (hv_get_health(hv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
		assert (hv_get_health (hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS +LIFE_GAIN_CASTLE_DRACULA);
		assert (hv_get_score(hv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN*2 );

		puts ("passed");
		hv_drop (hv);


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
			"GBDTD.. SBD.... HZU.... MBB.... DKLT..."; 
			
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};

		HunterView hv = hv_new (trail, messages);
		location_t history[TRAIL_SIZE]; 

		// Testing history
		hv_get_trail(hv, PLAYER_LORD_GODALMING, history); 

		assert (history[0] == BUDAPEST);
		assert (history[1] == VIENNA); 
		assert (history[2] == PRAGUE); 
		assert (history[3] == UNKNOWN_LOCATION); 
		assert (history[4] == UNKNOWN_LOCATION);  
		assert (history[5] == UNKNOWN_LOCATION); 

		hv_get_trail(hv, PLAYER_DRACULA, history); 

		assert (history[0] == KLAUSENBURG);
		assert (history[1] == BUDAPEST); 
		assert (history[2] == VIENNA); 
		assert (history[3] == UNKNOWN_LOCATION); 
		assert (history[4] == UNKNOWN_LOCATION);  
		assert (history[5] == UNKNOWN_LOCATION); 	

		// Testing stats
		assert (hv_get_health (hv, PLAYER_LORD_GODALMING) == 0); 
		assert (hv_get_health (hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 2*LIFE_LOSS_HUNTER_ENCOUNTER); 
		assert (hv_get_location (hv, PLAYER_LORD_GODALMING) == ST_JOSEPH_AND_ST_MARYS); 
		assert (hv_get_score(hv) == GAME_START_SCORE - (3*SCORE_LOSS_DRACULA_TURN + SCORE_LOSS_HUNTER_HOSPITAL)); 
		


		puts ("passed");
		hv_drop (hv);


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

		HunterView hv = hv_new (trail, messages);
		location_t history[TRAIL_SIZE]; 

		// Testing history
		hv_get_trail(hv, PLAYER_LORD_GODALMING, history); 

		assert (history[0] == SZEGED);
		assert (history[1] == BUDAPEST); 
		assert (history[2] == VIENNA); 
		assert (history[3] == PRAGUE); 
		assert (history[4] == UNKNOWN_LOCATION);  
		assert (history[5] == UNKNOWN_LOCATION); 

		hv_get_trail(hv, PLAYER_DRACULA, history); 

		assert (history[0] == KLAUSENBURG);
		assert (history[1] == BUDAPEST); 
		assert (history[2] == VIENNA); 
		assert (history[3] == UNKNOWN_LOCATION); 
		assert (history[4] == UNKNOWN_LOCATION);  
		assert (history[5] == UNKNOWN_LOCATION); 	

		// Testing stats
		assert (hv_get_health (hv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS); 
		assert (hv_get_location (hv, PLAYER_LORD_GODALMING) == SZEGED); 
		assert (hv_get_score(hv) == GAME_START_SCORE - (3*SCORE_LOSS_DRACULA_TURN + SCORE_LOSS_HUNTER_HOSPITAL)); 
		


		puts ("passed");
		hv_drop (hv);


	}

	{
		puts ("Hunter who has lost some life points does not recover hp upon moving"
			  " (not teleporting) to hospital"); 



		char *trail =
			"GPR.... SBD.... HZU.... MBB.... DC?V... "
			"GVIVD.. SBD.... HZU.... MBB.... DBDT... "
			"GZA.... SBD.... HZU.... MBB.... DKLT... "
			"GJM.... SBD.... HZU.... MBB.... DKLT..."; 
			
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};

		HunterView hv = hv_new (trail, messages);
		location_t history[TRAIL_SIZE]; 

		// Testing history
		hv_get_trail(hv, PLAYER_LORD_GODALMING, history); 

		assert (history[0] == ST_JOSEPH_AND_ST_MARYS);
		assert (history[1] == ZAGREB); 
		assert (history[2] == VIENNA); 
		assert (history[3] == PRAGUE); 
		assert (history[4] == UNKNOWN_LOCATION);  
		assert (history[5] == UNKNOWN_LOCATION); 


		// Testing stats
		assert (hv_get_health (hv, PLAYER_LORD_GODALMING) == 5); 
		assert (hv_get_health (hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - LIFE_LOSS_HUNTER_ENCOUNTER); 
		assert (hv_get_location (hv, PLAYER_LORD_GODALMING) == ST_JOSEPH_AND_ST_MARYS); 
		assert (hv_get_score(hv) == GAME_START_SCORE - 4*SCORE_LOSS_DRACULA_TURN); 
		

		hv_drop (hv);

	}

	{
	
		puts("Hunter does not recover health upon moving out of hospital area if they walked"
			 " into it"); 

		char *trail =
			"GPR.... SBD.... HZU.... MBB.... DVIV... "
			"GVIVD.. SBD.... HZU.... MBB.... DBDT... "
			"GZA.... SBD.... HZU.... MBB.... DKLT... "
			"GJM.... SBD.... HZU.... MBB.... DKLT... "
			"GZA...."; 
			
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};

		HunterView hv = hv_new (trail, messages);
		location_t history[TRAIL_SIZE]; 

		// Testing history
		hv_get_trail(hv, PLAYER_LORD_GODALMING, history); 

		assert (history[0] == ZAGREB);
		assert (history[1] == ST_JOSEPH_AND_ST_MARYS); 
		assert (history[2] == ZAGREB); 
		assert (history[3] == VIENNA); 
		assert (history[4] == PRAGUE);  
		assert (history[5] == UNKNOWN_LOCATION); 


		// Testing stats
		assert (hv_get_health (hv, PLAYER_LORD_GODALMING) == 5); 
		assert (hv_get_health (hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - LIFE_LOSS_HUNTER_ENCOUNTER); 
		assert (hv_get_location (hv, PLAYER_LORD_GODALMING) == ZAGREB); 
		assert (hv_get_score(hv) == GAME_START_SCORE - 4*SCORE_LOSS_DRACULA_TURN); 

		puts ("passed");
		hv_drop (hv); 

	}


	{

	puts("Dracula cannot go to the hospital");


		char *trail =
			"GPR.... SBD.... HZU.... MBB.... DZAV...";

		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};


		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_DRACULA, true, false, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};

		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 6); // including Zagreb itself
		assert (seen[ZAGREB]); 
		assert (!seen[ST_JOSEPH_AND_ST_MARYS]);
		assert (seen[SZEGED]); 
		assert (seen[BUDAPEST]); 
		assert (seen[VIENNA]);
		assert (seen[MUNICH]);
		assert (seen[SARAJEVO]);
		free(edges);
		hv_drop (hv);


	}






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

		HunterView hv = hv_new (trail, messages);
		location_t history[TRAIL_SIZE]; 

		// Testing history
		hv_get_trail(hv, PLAYER_DRACULA, history); 

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
		assert (hv_get_health (hv, PLAYER_LORD_GODALMING) == 5); 
		assert (hv_get_health (hv, PLAYER_DR_SEWARD) == 4); 
		assert (hv_get_location (hv, PLAYER_DR_SEWARD) == VIENNA); 

		assert (hv_get_location (hv, PLAYER_DRACULA) == VIENNA); 
		assert (hv_get_health (hv, PLAYER_DRACULA) == (GAME_START_BLOOD_POINTS - 3*LIFE_LOSS_HUNTER_ENCOUNTER)); 
		assert (hv_get_location (hv, PLAYER_LORD_GODALMING) == ZAGREB); 
		assert (hv_get_score(hv) == GAME_START_SCORE - 4*SCORE_LOSS_DRACULA_TURN ); 

		puts ("passed");
		hv_drop (hv); 

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

		HunterView hv = hv_new (trail, messages);
		location_t history[TRAIL_SIZE]; 

		// Testing history
		hv_get_trail(hv, PLAYER_DRACULA, history); 

		// both history and get_location should return double_back 3
		// since the double back city is unknown
		assert (history[0] == DOUBLE_BACK_3);
		assert (history[1] == CITY_UNKNOWN); 
		assert (history[2] == CITY_UNKNOWN); 
		assert (history[3] == CITY_UNKNOWN); 
		assert (history[4] == UNKNOWN_LOCATION);  
		assert (history[5] == UNKNOWN_LOCATION); 


		// Testing stats
		assert (hv_get_location (hv, PLAYER_LORD_GODALMING) == ZAGREB); 
		assert (hv_get_location (hv, PLAYER_DR_SEWARD) == VIENNA); 

		assert (hv_get_location (hv, PLAYER_DRACULA) == DOUBLE_BACK_3); 
		assert (hv_get_score(hv) == GAME_START_SCORE - 4*SCORE_LOSS_DRACULA_TURN ); 

		puts ("passed");
		hv_drop (hv); 



	}




	return EXIT_SUCCESS;
}



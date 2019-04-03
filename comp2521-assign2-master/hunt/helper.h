
#include "places.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "game.h"
#include "assert.h"

location_t* add_connection(location_t* dest,size_t *n_location,location_t n);
bool Exist(location_t* ,size_t *,location_t);
int get_player_number (enum player p);

int increment_rail_index(int rail_index);

enum player get_player_byNo (size_t num);




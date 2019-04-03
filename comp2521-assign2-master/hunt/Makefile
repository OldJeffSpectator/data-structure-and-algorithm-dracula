########################################################################
# COMP2521 19t0 // The Fury of Dracula // the Hunt
# hunt/Makefile: build your Dracula and Hunter AIs
#
# 2018-12-31	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>

CC	= 2521 3c

# do not change the following line
BINS	= dracula hunter test_game_view test_hunter_view test_dracula_view

# add any other *.o files that your solution requires
# (and add their dependencies below after dracula_view.o)
# if you're not using map.o or places.o, you can remove them
OBJS	= game_view.o map.o places.o queue.o queue_location.o helper.o

# add whatever system libraries you need here (e.g. -lm)
LIBS	=

all: ${BINS}

dracula:	player_dracula.o dracula.o dracula_view.o ${OBJS} ${LIBS}
hunter:		player_hunter.o hunter.o hunter_view.o ${OBJS} ${LIBS}

########
test_game_view:		test_game_view.o game_view.o map.o  ${OBJS} ${LIBS}
test_game_view.o:	test_game_view.c game_view.h map.h places.h game.h

test_hunter_view:	test_hunter_view.o hunter_view.o game_view.o  ${OBJS} ${LIBS}
test_hunter_view.o:	test_hunter_view.c hunter_view.h game_view.h map.h places.h game.h

test_dracula_view:	test_dracula_view.o dracula_view.o game_view.o  ${OBJS} ${LIBS}
test_dracula_view.o:	test_dracula_view.c dracula_view.h game_view.h map.h places.h game.h
########


player_dracula.o: player.c dracula.h game.h dracula_view.h game_view.h places.h
	${CC} ${CFLAGS} -DI_AM_DRACULA -c $< -o $@
player_hunter.o: player.c hunter.h game.h hunter_view.h game_view.h places.h
	${CC} ${CFLAGS} -c $< -o $@

dracula.o:	dracula.c dracula.h dracula_view.h game_view.h places.h game.h helper.h
hunter.o:	hunter.c hunter.h hunter_view.h game_view.h places.h game.h helper.h

game_view.o:	game_view.c game_view.h places.h game.h queue_location.h helper.h
dracula_view.o:	dracula_view.c dracula_view.h game_view.h places.h game.h queue.h queue_location.h helper.h
hunter_view.o:	hunter_view.c game_view.h places.h hunter_view.h game.h map.h queue.h queue_location.h helper.h

map.o:		map.c map.h places.h
places.o:	places.c places.h
queue.o: 	queue.c queue.h
queue_location.o: queue_location.c queue_location.h
helper.o : helper.c helper.h


# if you use other ADTs, add dependencies for them here

.PHONY: clean
clean:
	-rm -f ${BINS} *.o core

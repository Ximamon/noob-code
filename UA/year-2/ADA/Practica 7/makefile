# Julian Hinojosa Gil, 48795869N

OPTS = -Wall -Wextra -O3 -std=c++0x
CC = g++

all:  maze_greedy

maze_greedy: maze_greedy.cc
	${CC} ${OPTS} -o maze_greedy maze_greedy.cc

tar: maze_greedy.cc makefile
	tar cvfz 48795869N.tar.gz maze_greedy.cc makefile

clean: 
	rm -rf maze_greedy maze_greedy.dSYM *~
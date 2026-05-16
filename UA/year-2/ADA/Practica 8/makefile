# Julian Hinojosa Gil, 48795869N

OPTS = -Wall -Wextra -O0 -std=c++0x -g
CC = g++

all:  maze_bt

maze_bt: maze_bt.cc
	${CC} ${OPTS} -o maze_bt maze_bt.cc

tar: maze_bt.cc makefile
	tar cvfz 48795869N.tar.gz maze_bt.cc makefile

clean: 
	rm -rf maze_bt maze_bt.dSYM *~ 48795869N.tar.gz
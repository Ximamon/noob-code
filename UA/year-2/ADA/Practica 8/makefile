# Julian Hinojosa Gil, 48795869N

OPTS = -Wall -Wextra -O3 -std=c++0x
CC = g++

all:  maze_bt

maze_bt: maze_bt.cc
	${CC} ${OPTS} -o maze_bt maze_bt.cc

tar: maze_bt.cc makefile
	tar cvfz 48795869N.tar.gz maze_bt.cc makefile

clean: 
	rm -rf maze_bt maze_bt.dSYM *~
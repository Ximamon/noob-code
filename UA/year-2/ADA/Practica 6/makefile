# Julian Hinojosa Gil, 48795869N

OPTS = -Wall -Wextra -O3 -std=c++0x
CC = g++

all:  maze

maze: maze.cc
	${CC} ${OPTS} -o maze maze.cc

tar: maze.cc maze.gpi makefile
	tar cvfz 48795869N.tar.gz maze.cc makefile

clean: 
	rm -rf maze maze.dSYM *~
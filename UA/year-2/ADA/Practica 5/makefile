# Julian Hinojosa Gil, 48795869N

OPTS = -Wall -Wextra -O3 -std=c++0x
CC = g++

all:  pow2 pow2.Msteps #graphs

graphs: pow2.Msteps pow2.gpi
	gnuplot pow2.gpi

pow2: pow2.cc
	${CC} ${OPTS} -o pow2 pow2.cc

pow2.Msteps: pow2
	./pow2 > pow2.Msteps

tar: pow2.cc pow2.gpi makefile
	tar cvfz 48795869N.tar.gz pow2.cc pow2.gpi makefile

clean: 
	rm -rf pow2 pow2.dSYM fit.log pow2.Msteps pow2.Msteps quickSort*.png heapSort*.png pow2*.png *~
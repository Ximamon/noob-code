OPTS = -Wall -Wextra -O3 -std=c++0x
CC = g++

all:  qs-vs-hs qs-vs-hs.Msteps graphs

graphs: qs-vs-hs.Msteps qs-vs-hs.gpi
	gnuplot qs-vs-hs.gpi

qs-vs-hs: qs-vs-hs.cc
	${CC} ${OPTS} -o qs-vs-hs qs-vs-hs.cc	 

qs-vs-hs.Msteps: qs-vs-hs
	./qs-vs-hs > qs-vs-hs.Msteps

tar: qs-vs-hs.cc qs-vs-hs.gpi makefile
	tar cvfz 48795869N.tar.gz qs-vs-hs.cc qs-vs-hs.gpi makefile

clean: 
	rm -rf qs-vs-hs qs-vs-hs.dSYM fit.log qs-vs-hs.Msteps qs-vs-hs.Msteps quickSort*.png *~
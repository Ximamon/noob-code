OPTS = -Wall -Wextra -O3 -std=c++0x
CC = g++

all: bubbleSort quickSort bubbleSort.CPUtimes quickSort.CPUtimes graphs

graphs: bubbleSort.CPUtimes bubbleSort.gpi quickSort.CPUtimes quickSort.gpi
	gnuplot bubbleSort.gpi
	gnuplot quickSort.gpi

bubbleSort: bubbleSort.cc
	${CC} ${OPTS} -o bubbleSort bubbleSort.cc

quickSort: quickSort.cc
	${CC} ${OPTS} -o quickSort quickSort.cc	 

bubbleSort.CPUtimes: bubbleSort
	./bubbleSort > bubbleSort.CPUtimes

quickSort.CPUtimes: quickSort
	./quickSort > quickSort.CPUtimes

tar: bubbleSort.cc bubbleSort.gpi makefile
	tar cvfz mi_DNI-NIE.tar.gz bubbleSort.cc bubbleSort.gpi makefile

clean: 
	rm -f quickSort bubbleSort bubbleSort.CPUtimes fit.log bubbleSort*.png quickSort.CPUtimes quickSort*.png *~
 

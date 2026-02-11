OPTS = -Wall -Wextra -O3 -std=c++0x
CC = g++

all:  quickSort quickSort.CPUtimes graphs  # bubbleSort bubbleSort.CPUtimes

graphs: quickSort.CPUtimes quickSort.gpi # bubbleSort.CPUtimes bubbleSort.gpi
# 	gnuplot bubbleSort.gpi
	gnuplot quickSort.gpi

# bubbleSort: bubbleSort.cc
# 	${CC} ${OPTS} -o bubbleSort bubbleSort.cc

quickSort: quickSort.cc
	${CC} ${OPTS} -o quickSort quickSort.cc	 

# bubbleSort.CPUtimes: bubbleSort
# 	./bubbleSort > bubbleSort.CPUtimes

quickSort.CPUtimes: quickSort
	./quickSort > quickSort.CPUtimes

tar: quickSort.cc quickSort.gpi makefile
	tar cvfz 48795869N.tar.gz quickSort.cc quickSort.gpi makefile

clean: 
	rm -rf quickSort quickSort.dSYM fit.log quickSort.CPUtimes quickSort*.png *~
# bubbleSort bubbleSort.CPUtimes bubbleSort*.png
 

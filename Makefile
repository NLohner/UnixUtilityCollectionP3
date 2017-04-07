FLAGS = -Wall -std=c++14 -g -O0 -pedantic-errors

all: true false

true: true.o
	g++ $(FLAGS) -o true true.o

true.o: true.cpp
	g++ $(FLAGS) -c true.cpp

false: false.o
	g++ $(FLAGS) -o false false.o

false.o: false.cpp
	g++ $(FLAGS) -c false.cpp

clean:
	rm -f true false *.o *~ /#*
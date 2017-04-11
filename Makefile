all: cal date head ln ls 

FLAGS = -Wall -std=c++14 -g -O0 -pedantic-errors

cal.o: cal.cpp
	g++ $(FLAGS) -c cal.cpp

cal: cal.o
	g++ $(FLAGS) -o cal cal.o

date.o: date.cpp
	g++ $(FLAGS) -c date.cpp

date: date.o
	g++ $(FLAGS) -o date date.o

head.o: head.cpp
	g++ $(FLAGS) -c head.cpp

head: head.o
	g++ $(FLAGS) -o head head.o

ln.o: ln.cpp
	g++ $(FLAGS) -c ln.cpp

ln: ln.o
	g++ $(FLAGS) -o ln ln.o

ls.o: ls.cpp
	g++ $(FLAGS) -c ls.cpp

ls: ls.o
	g++ $(FLAGS) -o ls ls.o

clean:
	rm -f *.o
	rm cal date head ln ls






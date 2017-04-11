FLAGS = -Wall -std=c++14 -g -O0 -pedantic-errors

all: true false env pwd tail

true: true.o
	g++ $(FLAGS) -o true true.o

true.o: true.cpp
	g++ $(FLAGS) -c true.cpp

false: false.o
	g++ $(FLAGS) -o false false.o

false.o: false.cpp
	g++ $(FLAGS) -c false.cpp

env: env.o
	g++ $(FLAGS) -o env env.o

env.o: env.cpp
	g++ $(FLAGS) -c env.cpp

pwd: pwd.o
	g++ $(FLAGS) -o pwd pwd.o

pwd.o: pwd.cpp
	g++ $(FLAGS) -c pwd.cpp

tail: tail.o
	g++ $(FLAGS) -o tail tail.o

tail.o: tail.cpp
	g++ $(FLAGS) -c tail.cpp

clean:
	rm -f true false env pwd tail *.o *~ /#*
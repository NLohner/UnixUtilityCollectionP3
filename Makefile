
all: cal date head ln ls true false env pwd tail wc mkdir

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

wc: wc.o
	g++ $(FLAGS) -o wc wc.o

wc.o: wc.cpp
	g++ $(FLAGS) -c wc.cpp

mkdir: mkdir.o
	g++ $(FLAGS) -o mkdir mkdir.o

mkdir.o: mkdir.cpp
	g++ $(FLAGS) -c mkdir.cpp

clean:
	rm -f cal date head ln ls true false env pwd tail wc mkdir *.o *~ /#*


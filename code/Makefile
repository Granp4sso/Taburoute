all: main_exe

main_exe: main.o Taburoute.o Search.o tsp_2_opt.o HGL.o
	g++ -g -o main_exe main.o Taburoute.o Search.o tsp_2_opt.o HGL.o

main.o: main.cpp Taburoute.h 
	g++ -g -c main.cpp

Taburoute.o: Taburoute.cpp Taburoute.h Search.h tsp_2_opt.h
	g++ -g -c Taburoute.cpp

Search.o: Search.cpp Search.h tsp_2_opt.h HGL.h
	g++ -g -c Search.cpp

tsp_2_opt.o: tsp_2_opt.cpp tsp_2_opt.h HGL.h 
	g++ -g -c tsp_2_opt.cpp
	
HGL.o: HGL.cpp HGL.h 
	g++ -g -c HGL.cpp

clean:
	rm -f *.o
	rm -f ./main_exe
	rm -f *~

all: edrmake

edrmake: main.o csv_reader.o edr.o static_methods.o
	g++ main.o csv_reader.o edr.o static_methods.o -o edrmake

main.o: main.cpp
	g++ -c main.cpp

csv_reader.o: csv_reader.cpp
	g++ -c csv_reader.cpp

edr.o: edr.cpp
	g++ -c edr.cpp
	
static_methods.o: static_methods.cpp
	g++ -c static_methods.cpp

clean:
	rm -rf *.o edrmake


#define the compiler
CC=g++

#define object files
edrobjects = main.o csv_reader.o edr.o static_methods.o
testobjects = testfield.o field.o stdout_visitor.o fileout_visitor.o

#default rule
all: edrmake

edrmake: $(edrobjects)
	$(CC) -o $@ $+

testit: $(testobjects)
	$(CC) -o $@ $+
	
clean:
	rm -rf *.o edrmake testit
	
# tell make how to build .o files from .cpp files:
%.o:%.cpp
	$(CC) -c $+

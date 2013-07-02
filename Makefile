#define the compiler
CC=g++

#flags
ALL_CFLAGS=

#define object files
edrobjects = main.o csv_reader.o edr.o static_methods.o

#default rule
all: edrmake

edrmake: $(edrobjects)
	$(CC) -o $@ $+

clean:
	rm -rf *.o edrmake
	
# tell make how to build .o files from .cpp files:
%.o:%.cpp
	$(CC) -c $(CPPFLAGS) $(ALL_CFLAGS) $+
	

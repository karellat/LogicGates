CC	:= g++-6
CFLAGS	:= -std=c++14
INC	:= -I includes

APPNAME	:= bin/main
SOURCES	:= $(wildcard src/*.cpp *.cpp)
OBJECTS	:= $(patsubst  src%, obj%, $(patsubst %.cpp, %.o, $(patsubst %.c, %.o, $(SOURCES))))
SRC := src 
EXAMDIR  := examples

all: init convert $(OBJECTS)
	$(CC) $(CFLAGS) $(INC) $(OBJECTS) -o $(APPNAME)
	./bin/main

%.o: %.cpp
	$(CC) $(CFLAGS) $(INC) -c $< -o $@ 

obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) $(INC) -c $< -o $@ 

clean: 
	rm -rf obj/*
	rm -rf *.o
	rm -f $(APPNAME)
init: 
	test -e bin || mkdir bin 
	test -e obj || mkdir obj
convert:
	@bash convertor $(EXAMDIR)
	@echo "Converting eol of examples" 

CXX	:= g++-6
CXXFLAGS	:= -std=c++14 
#-g3 -fuse-ld=gold -fsanitize=address 
INC	:= -I includes

APPNAME	:= bin/main
SOURCES	:= $(wildcard src/*.cpp *.cpp)
OBJECTS	:= $(patsubst  src%, obj%, $(patsubst %.cpp, %.o, $(patsubst %.c, %.o, $(SOURCES))))
SRC := src 
EXAMDIR  := examples

compile: init convert $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INC) $(OBJECTS) -o $(APPNAME)
	

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@ 

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@ 

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

test1: compile
	./bin/main -f examples/XORAND.txt examples/ADDW.txt -a


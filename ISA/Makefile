SRC1=isaserver.cpp
SRC2=isaclient.cpp
CFLAGS=-g -pedantic -Wall -std=c++14
EXECUTABLE1=isaserver
EXECUTABLE2=isaclient

ALL:
	g++ -o $(EXECUTABLE1) $(CFLAGS) $(SRC1)
	g++ -o $(EXECUTABLE2) $(CFLAGS) $(SRC2)

clean: 
	rm $(EXECUTABLE1)
	rm $(EXECUTABLE2)
debug:
	gdb --tui $(EXECUTABLE)
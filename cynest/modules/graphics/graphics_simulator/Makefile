CC=g++
CFLAGS=-c
LDFLAGS=-lGLU -lSDL
OBJECTS=main.o graphics_simulator.o socket.o
EXECUTABLE=g_simulator

all: $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(EXECUTABLE)

main.o: 
	$(CC) $(CFLAGS) -o main.cpp

graphics_simulator.o: 
	$(CC) $(CFLAGS) -o graphics_simulator.cpp
	
socket.o: 
	$(CC) $(CFLAGS) -o socket.cpp
	

	
clean:
	rm -rf *o $(EXECUTABLE)

CC=g++
FLAGS=-O3 -std=c++1y

all: HalloWorldMapServer
		
HalloWorldMapServer:
	$(CC) $(FLAGS) -pthread -o mongoose.so -c mongoose/mongoose.c 
	$(CC) $(FLAGS) -o HalloWorldMapServer HalloWorldMapServer.cpp -pthread mongoose.so -ldl
	rm mongoose.so
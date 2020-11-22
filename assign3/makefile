CC = g++
LFLAGS = 
CFLAGS =-c -Wall -std=c++11 -O2
OBJS1 = server.o trand.o
OBJS2 = client.o trand.o 
EXE1 = server
EXE2 = client
MAN1 = server_man
MAN2 = client_man
all: $(EXE1) $(EXE2) $(MAN1) $(MAN2)

$(EXE1): $(OBJS1)
	$(CC) $(OBJS1) -o $(EXE1) $(LFLAGS)

$(EXE2): $(OBJS2)
	$(CC) $(OBJS2) -o $(EXE2) $(LFLAGS)
server_man: SERVER
	man -t ./SERVER | ps2pdf - SERVER.pdf
client_man: CLIENT
	man -t ./CLIENT | ps2pdf - CLIENT.pdf

server.o: server.cpp trand.c trand.h
	g++ server.cpp -o server.o $(CFLAGS)

client.o: client.cpp trand.c trand.h
	g++ client.cpp -o client.o $(CFLAGS)

trand.o: trand.c
	g++ trand.c -o trand.o $(CFLAGS)

clean: 
	@rm -rf *.o


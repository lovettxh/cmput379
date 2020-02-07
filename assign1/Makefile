CC = g++
LFLAGS = 
CFLAGS =-c -Wall -std=c++11 -O2
OBJS = shell.o cmd_line.o process.o
EXE = shell

$(EXE): $(OBJS)
	$(CC) $(OBJS) -o $(EXE) $(LFLAGS)

shell.o: shell.cpp cmd_line.h process.h
	g++ shell.cpp -o shell.o $(CFLAGS)

process.o: process.cpp process.h
	g++ process.cpp -o process.o $(CFLAGS)

cmd_line.o: cmd_line.cpp process.h
	g++ cmd_line.cpp -o cmd_line.o $(CFLAGS)



clean: 
	@rm -rf *.o
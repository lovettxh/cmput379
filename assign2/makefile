CC = g++
LFLAGS = 
CFLAGS =-c -Wall -std=c++11 -O2
OBJS = prodcon.o trand.o thread_function.o
EXE = prodcon

$(EXE): $(OBJS)
	$(CC) $(OBJS) -pthread -o $(EXE) $(LFLAGS)

prodcon.o: prodcon.cpp trand.c trand.h thread_function.cpp thread_function.h
	g++ prodcon.cpp -pthread -o prodcon.o $(CFLAGS)

trand.o: trand.c trand.h
	g++ trand.c -pthread -o trand.o $(CFLAGS)

thread_function.o: thread_function.cpp thread_function.h
	g++ thread_function.cpp -pthread -o thread_function.o $(CFLAGS)

clean: 
	@rm -rf *.o
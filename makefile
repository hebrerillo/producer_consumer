CC     = g++
CFLAGS = -Wall -Werror -Wextra -Wno-unused-parameter -Wno-unused-variable -g -std=c++0x
LFLAGS = -pthread -lanl -lrt
TARGETS = main

%.o: %.cpp %.h
	$(CC) -c $(CFLAGS) $< -o $@

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

all:  $(TARGETS)

main: main.o
	$(CC) -o main $^ $(LFLAGS)	

main.o: main.cpp
	$(CC) $(CFLAGS) -c $^
	
clean:
	-rm *.o core* $(TARGETS)
	

CC     = g++
CFLAGS = -Wall -Werror -Wextra -Wno-unused-parameter -Wno-unused-variable -g -std=c++0x
LFLAGS = -pthread
TARGETS = main

%.o: %.cpp %.h
	$(CC) -c $(CFLAGS) $< -o $@

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

all:  $(TARGETS)

main: main.o producer.o consumer.o sharedBuffer.o manager.o
	$(CC) -o main $^ $(LFLAGS)	

main.o: main.cpp IBufferItem.h
	$(CC) $(CFLAGS) -c $^

sharedBuffer.o: sharedBuffer.cpp sharedBuffer.h IBufferItem.h
	$(CC) $(CFLAGS) -c $^

clean:
	-rm *.o core* $(TARGETS)
	

CC     = g++
CFLAGS = -Wall -Werror -Wextra -Wno-unused-parameter -Wno-unused-variable -g -std=c++0x
LFLAGS = -pthread
TARGETS = main test
GTEST = /usr/lib/libgtest.a

all: $(TARGETS)

%.o: %.cpp %.h
	$(CC) -c $(CFLAGS) $< -o $@

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

all:  $(TARGETS)

main: main.o producer.o consumer.o sharedBuffer.o manager.o IActor.o bufferItem.o
	$(CC) -o main $^ $(LFLAGS)

main.o: main.cpp bufferItem.h
	$(CC) $(CFLAGS) -c $^

sharedBuffer.o: sharedBuffer.cpp sharedBuffer.h IBufferItem.h
	$(CC) $(CFLAGS) -c $^

producer.o: producer.h producer.cpp IActor.h sharedBuffer.h
	$(CC) $(CFLAGS) -c $^

consumer.o: consumer.h consumer.cpp IActor.h sharedBuffer.h
	$(CC) $(CFLAGS) -c $^

test: test.o producer.o consumer.o sharedBuffer.o manager.o IActor.o bufferItem.o
	$(CC) -o test $^ $(LFLAGS) $(GTEST)

clean:
	-rm -f *.gch *.o core* $(TARGETS)
	

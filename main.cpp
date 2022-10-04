#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <string.h>
#include <atomic>
#include <assert.h>

#define BUFFER_SIZE 20
#define SPEED_CONSUMER 400
#define SPEED_PRODUCER 100

static int buffer[BUFFER_SIZE];
static std::atomic<bool> quitSignal;
static int currentIndex;
static std::mutex mutex;

class Producer
{
public:
    void run()
    {
        while(!quitSignal.load())
        {
            mutex.lock();
            if ((currentIndex + 1) < BUFFER_SIZE)
            {
                ++currentIndex;
                assert(buffer[currentIndex] == 0);
                buffer[currentIndex] = 1;
                std::cout << "Producing" << std::endl;
            }
            mutex.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(SPEED_PRODUCER));
        }
    }
};

class Consumer 
{
public:
    void run()
    {
        while(!quitSignal.load())
        {
            mutex.lock();
            if (currentIndex >= 0)
            {
                assert(buffer[currentIndex] == 1);
                buffer[currentIndex--] = 0;
                std::cout << "Consuming" << std::endl;
            }
            mutex.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(SPEED_CONSUMER));
        }
    }
};


int main()
{
    quitSignal = false;
    memset(buffer, 0, sizeof(buffer));
    currentIndex = -1;
    Consumer consumer;
    Producer producer;

    std::thread threadProducer(&Producer::run, &producer);
    std::thread threadConsumer(&Consumer::run, &consumer);

    int input;
    std::cin >> input;
    quitSignal.exchange(true);
    threadProducer.join();
    threadConsumer.join();

    return 0;
}
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <string.h>
#include <atomic>

#define BUFFER_SIZE 10

static int buffer[BUFFER_SIZE];
static std::atomic<bool> quitSignal;
static size_t currentIndex;

class Producer
{
public:
    void run()
    {
        while(!quitSignal.load())
        {
            
        }
    }
};

class Consumer 
{

};


int main()
{
    quitSignal = false;
    memset(buffer, BUFFER_SIZE, 0);
    currentIndex = 0;

    return 0;
}
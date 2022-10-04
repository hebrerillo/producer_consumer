#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <string.h>
#include <atomic>
#include <assert.h>
#include <condition_variable>

#define BUFFER_SIZE 20
#define SPEED_CONSUMER 100
#define SPEED_PRODUCER 100

static std::atomic<bool> quitSignal;
static int currentIndex;
static std::mutex mutex;
static std::condition_variable cv;

class Producer
{
public:
    
    Producer(int *buffer)
    {
        buffer_ = buffer;
        thread_ = std::thread(&Producer::run, this);
    }

    ~Producer()
    {
        thread_.join();
    }

private:

    void run()
    {
        while(!quitSignal.load())
        {
            std::unique_lock<std::mutex> lock(mutex);
            if ((currentIndex + 1) < BUFFER_SIZE)
            {
                ++currentIndex;
                assert(buffer_[currentIndex] == 0);
                buffer_[currentIndex] = 1;
                std::cout << "Producing" << std::endl;
                cv.notify_all();
            }
            else
            {
                std::cout << "Producer waiting until consumer consumes." << std::endl;
                cv.wait(lock, [](){
                    return (currentIndex + 1) < BUFFER_SIZE;
                });
            }
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(SPEED_PRODUCER));
        }
    }

    std::thread thread_;
    int *buffer_;

};

class Consumer 
{
public:

    Consumer(int *buffer)
    {
        buffer_ = buffer;
        thread_ = std::thread(&Consumer::run, this);
    }

    ~Consumer()
    {
        thread_.join();
    }

private:
    void run()
    {
        while(!quitSignal.load())
        {
            std::unique_lock<std::mutex> lock(mutex);
            if (currentIndex >= 0)
            {
                assert(buffer_[currentIndex] == 1);
                buffer_[currentIndex--] = 0;
                std::cout << "Consuming" << std::endl;
                cv.notify_all();
            }
            else
            {
                std::cout << "Consumer waiting until producer produces." << std::endl;
                cv.wait(lock, [](){
                    return (currentIndex >= 0);
                });
            }
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(SPEED_CONSUMER));
        }
    }

    std::thread thread_;
    int *buffer_;
};


int main()
{
    int buffer[BUFFER_SIZE];
    quitSignal = false;
    memset(buffer, 0, sizeof(buffer));
    currentIndex = -1;
    Consumer consumer(buffer);
    Producer producer(buffer);

    int input;
    std::cin >> input;
    quitSignal.exchange(true);

    return 0;
}
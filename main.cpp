#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <string.h>
#include <atomic>
#include <assert.h>
#include <condition_variable>

#define BUFFER_SIZE 20
#define SPEED_CONSUMER 200
#define SPEED_PRODUCER 100

static int currentIndex;
static std::mutex mutex;
static std::condition_variable cv;

class Producer
{
public:

    Producer()
    : quitSignal_(true)
    {}

    void start(int *buffer)
    {
        quitSignal_ = false;
        thread_ = std::thread(&Producer::run, this, buffer);
    }

    void stop()
    {
        std::unique_lock<std::mutex> lock(mutex);
        quitSignal_.exchange(true);
        cv.notify_all();
        lock.unlock();
        thread_.join();
    }

private:

    void run(int* buffer)
    {
        while(!quitSignal_.load())
        {
            std::unique_lock<std::mutex> lock(mutex);
            if ((currentIndex + 1) < BUFFER_SIZE)
            {
                ++currentIndex;
                assert(buffer[currentIndex] == 0);
                buffer[currentIndex] = 1;
                std::cout << "Producing" << std::endl;
                cv.notify_all();
            }
            else
            {
                std::cout << "Producer waiting until consumer consumes." << std::endl;
                cv.wait(lock, [this](){
                    return (currentIndex + 1) < BUFFER_SIZE || quitSignal_.load();
                });
            }
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(SPEED_PRODUCER));
        }
    }

    std::thread thread_;
    std::atomic<bool> quitSignal_;
};

class Consumer 
{
public:
    Consumer()
    : quitSignal_(true)
    {}

    void start(int* buffer)
    {
        quitSignal_ = false;
        thread_ = std::thread(&Consumer::run, this, buffer);
    }

    void stop()
    {
        std::unique_lock<std::mutex> lock(mutex);
        quitSignal_.exchange(true);
        cv.notify_all();
        lock.unlock();
        thread_.join();
    }

private:
    void run(int* buffer)
    {
        while(!quitSignal_.load())
        {
            std::unique_lock<std::mutex> lock(mutex);
            if (currentIndex >= 0)
            {
                assert(buffer[currentIndex] == 1);
                buffer[currentIndex--] = 0;
                std::cout << "Consuming" << std::endl;
                cv.notify_all();
            }
            else
            {
                std::cout << "Consumer waiting until producer produces." << std::endl;
                cv.wait(lock, [this](){
                    return (currentIndex >= 0) || quitSignal_.load();
                });
            }
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(SPEED_CONSUMER));
        }
    }

    std::thread thread_;
    std::atomic<bool> quitSignal_;
};


int main()
{
    int buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    currentIndex = -1;
    Consumer consumer;
    Producer producer;
    consumer.start(buffer);
    producer.start(buffer);

    int input;
    std::cin >> input;

    consumer.stop();
    producer.stop();

    return 0;
}
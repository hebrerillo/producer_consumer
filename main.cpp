#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <string.h>
#include <atomic>
#include <assert.h>
#include <condition_variable>

/**
 * Class that represents the shared buffer between producer and consumer.
 */
class SharedBuffer
{
public:
    static const int BUFFER_SIZE = 10;

    SharedBuffer()
    : currentIndex_(-1)
    , quitSignal_(false)
    {
        memset(buffer_, 0, sizeof(buffer_));
    }

    /**
     * Adds an element to the buffer in the 'currentIndex_' position and increases 'currentIndex_'. This is the producer role.
     *
     * @param value The value to be added.
     */
    void push(int value)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if ((currentIndex_ + 1) < BUFFER_SIZE)
        {
            ++currentIndex_;
            assert(buffer_[currentIndex_] == 0);
            buffer_[currentIndex_] = 1;
            std::cout << "Pushing value" << std::endl;
            quitCV_.notify_all();
        }
        else
        {
            std::cout << "Buffer full. Waiting for someone to consume." << std::endl;
            quitCV_.wait(lock, [this](){
                return (currentIndex_ + 1) < BUFFER_SIZE || quitSignal_.load();
            });
        }
        lock.unlock();
    }

    /**
     * Extracts the element 'currentIndex_' from the buffer and decreases 'currentIndex_'. This is the consumer role.
     *
     * @return The element in the position 'currentIndex_'.
     */
    int pop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (currentIndex_ >= 0)
        {
            assert(buffer_[currentIndex_] == 1);
            buffer_[currentIndex_--] = 0;
            std::cout << "Poping value" << std::endl;
            quitCV_.notify_all();
        }
        else
        {
            std::cout << "Buffer empty. Waiting for someone to push." << std::endl;
            quitCV_.wait(lock, [this](){
                return (currentIndex_ >= 0) || quitSignal_.load();
            });
        }
        lock.unlock();
        return 1;
    }

    /**
     * Stops the buffer from accepting and/or returning elements.
     */
    void stop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        quitSignal_.exchange(true); //If the signaling is performed without locking, Helgrind complains that the lock associated with 'quitSignal' is not held by any thread.
        quitCV_.notify_all();
        lock.unlock();
    }

    /**
     * Whether the buffer is accepting and returning elements.
     */
    bool isRunning() const
    {
        return !quitSignal_.load();
    }

private:
    int currentIndex_; //The index to push/pop elements to/from 'buffer_'.
    int buffer_[BUFFER_SIZE];
    std::mutex mutex_; //To syncrhonize accesses to 'buffer_' and 'currentIndex_'.
    std::condition_variable quitCV_;
    std::atomic<bool> quitSignal_;
};

class Producer
{
public:

    /**
     * Constructor
     *
     * @param[in/out] buffer The buffer where the producer will insert values.
     * @param[in] delay The delay that the producer will take after producing one item.
     */
    Producer(SharedBuffer& buffer, const std::chrono::milliseconds& delay)
    {
        thread_ = std::thread(&Producer::run, this, std::ref(buffer), delay);
    }

    ~Producer()
    {
        thread_.join();
    }
private:

    /**
     * Starts adding elements to the buffer 'buffer'.
     *
     * @param[in/out] buffer The buffer to insert elements to.
     * @param[in] delay The delay that the producer will take after producing one item.
     */
    void run(SharedBuffer& buffer, const std::chrono::milliseconds& delay)
    {
        while(buffer.isRunning())
        {
            buffer.push(1);
            std::this_thread::sleep_for(delay);
        }
    }

    std::thread thread_;
};

class Consumer 
{
public:

    /**
     * @param[in/out] buffer The buffer where the consumer will extract values from.
     * @param[in] delay The delay that the consumer will take after consuming one item.
     */
    Consumer(SharedBuffer& buffer, const std::chrono::milliseconds& delay)
    {
        thread_ = std::thread(&Consumer::run, this, std::ref(buffer), delay);
    }

    ~Consumer()
    {
        thread_.join();
    }

private:

    /**
     * Starts extracting elements from the buffer 'buffer'.
     *
     * @param[in/out] buffer The buffer to extract elements from.
     * @param[in] delay The delay that the consumer will take after consuming one item.
     */
    void run(SharedBuffer& buffer, const std::chrono::milliseconds& delay)
    {
        while(buffer.isRunning())
        {
            buffer.pop();
            std::this_thread::sleep_for(delay);
        }
    }

    std::thread thread_;
};


int main()
{
    SharedBuffer buffer;
    Consumer consumer(buffer, std::chrono::milliseconds(250));
    Producer producer(buffer, std::chrono::milliseconds(500));
    Producer producer2(buffer, std::chrono::milliseconds(500));

    int input;
    std::cin >> input;
    buffer.stop();

    return 0;
}
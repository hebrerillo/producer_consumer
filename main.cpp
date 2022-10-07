#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <string.h>
#include <atomic>
#include <assert.h>
#include <condition_variable>
#include <vector>

/**
 * Class that represents an item that can be inserted in the shared buffer.
 * The item can be filled by a producir by calling 'fill', or it can be emptied by a consumer by calling 'empty'.
 */
class IBufferItem
{
public:
    /**
     * Fills this item.
     */
    virtual void fill() = 0;

    /**
     * Empties this item.
     */
    virtual void empty() = 0;

    /**
     * @return true if the item is filled, false if the item is empty.
     */
    virtual operator bool () const = 0;
};

class BufferItem: public IBufferItem
{
public:
    BufferItem()
    : value_(false)
    {}

    void fill() override
    {
        assert(!value_);
        value_ = true;
    }

    void empty() override
    {
        assert(value_);
        value_ = false;
    }

    operator bool() const override
    {
        return value_;
    }

private:
    bool value_;
};

/**
 * Class that represents the shared buffer between producers and consumers.
 * 
 */
class SharedBuffer
{
public:
    static const int BUFFER_SIZE = 10;

    SharedBuffer()
    : currentIndex_(-1)
    , quitSignal_(false)
    {
        buffer_.assign(BUFFER_SIZE, BufferItem());
    }

    /**
     * Adds an element to the buffer in the 'currentIndex_' position and increases 'currentIndex_'. This is the producer role.
     */
    void push()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if ((currentIndex_ + 1) < BUFFER_SIZE)
        {
            buffer_[++currentIndex_].fill();
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
            buffer_[currentIndex_--].empty();
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
    std::vector<BufferItem> buffer_;
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
            buffer.push();
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
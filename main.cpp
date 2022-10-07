#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <string.h>
#include <atomic>
#include <assert.h>
#include <condition_variable>
#include <vector>
#include <list>

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

    virtual ~IBufferItem(){}
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
 * Class that represents an entity that interacts with the buffer.
 */
class IBufferPlayer
{
public:

    /**
     * @return Whether this player is running.
     */
    virtual bool isRunning() const = 0;

    /**
     * Stops this player from interacting with the buffer.
     */
    virtual void stop() = 0;

    virtual ~IBufferPlayer(){}
};

/**
 * Class that represents the shared buffer between producers and consumers.
 * 
 */
class SharedBuffer
{
public:

    /**
     * Constructor
     *
     * @param[int/out] buffer The buffer to produce and consume items. 
     * @note Important!! All the items in the buffer should be empty.
     */
    SharedBuffer(const std::vector<IBufferItem* >& buffer)
    : currentIndex_(0)
    , buffer_(buffer)
    , quitSignal_(false)
    {}

    /**
     * Adds an element to the buffer in the 'currentIndex_' position and increases 'currentIndex_'. This is the producer role.
     *
     * @param[in] producer The producer.
     */
    void push(const IBufferPlayer* producer)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (currentIndex_ < buffer_.size())
        {
            buffer_[currentIndex_++]->fill();
            std::cout << "Pushing value" << std::endl;
            quitCV_.notify_all();
        }
        else
        {
            std::cout << "Buffer full. Waiting for someone to consume." << std::endl;
            quitCV_.wait(lock, [this, producer](){
                return currentIndex_ < buffer_.size() || quitSignal_.load() || !producer->isRunning();
            });
        }
        lock.unlock();
    }

    /**
     * Extracts the element 'currentIndex_' from the buffer and decreases 'currentIndex_'. This is the consumer role.
     *
     * @param[in] consumer The consumer
     */
    void pop(const IBufferPlayer* consumer)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (currentIndex_ > 0)
        {
            buffer_[--currentIndex_]->empty();
            std::cout << "Poping value" << std::endl;
            quitCV_.notify_all();
        }
        else
        {
            std::cout << "Buffer empty. Waiting for someone to push." << std::endl;
            quitCV_.wait(lock, [this, consumer](){
                return (currentIndex_ > 0) || quitSignal_.load() || !consumer->isRunning();
            });
        }
        lock.unlock();
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
    size_t currentIndex_; //The index of the last produced item.
    std::vector<IBufferItem* > buffer_;
    std::mutex mutex_; //To syncrhonize accesses to 'buffer_' and 'currentIndex_'.
    std::condition_variable quitCV_;
    std::atomic<bool> quitSignal_;
};

class Producer : public IBufferPlayer
{
public:

    /**
     * Constructor
     *
     * @param[in/out] buffer The buffer where the producer will insert values.
     * @param[in] delay The delay that the producer will take after producing one item.
     */
    Producer(SharedBuffer* buffer, const std::chrono::milliseconds& delay)
    : quitSignal_(false)
    {
        thread_ = std::thread(&Producer::run, this, buffer, delay);
    }

    /**
     * @return Whether this producer is running.
     */
    bool isRunning() const override
    {
        return !quitSignal_.load();
    }

    /**
     * Stops this producer from producing elements to the buffer.
     */
    void stop() override
    {
        quitSignal_.exchange(true);
        thread_.join();
    }

private:

    /**
     * Starts adding elements to the buffer 'buffer'.
     *
     * @param[in/out] buffer The buffer to insert elements to.
     * @param[in] delay The delay that the producer will take after producing one item.
     */
    void run(SharedBuffer* buffer, const std::chrono::milliseconds& delay)
    {
        while(buffer->isRunning() && !quitSignal_.load())
        {
            buffer->push(this);
            std::this_thread::sleep_for(delay);
        }
    }

    std::thread thread_;
    std::atomic<bool> quitSignal_;
};

class Consumer : public IBufferPlayer
{
public:

    /**
     * @param[in/out] buffer The buffer where the consumer will extract values from.
     * @param[in] delay The delay that the consumer will take after consuming one item.
     */
    Consumer(SharedBuffer* buffer, const std::chrono::milliseconds& delay)
    : quitSignal_(false)
    {
        thread_ = std::thread(&Consumer::run, this, buffer, delay);
    }

    /**
     * @return Whether this consumer is running.
     */
    bool isRunning() const override
    {
        return !quitSignal_.load();
    }

    /**
     * Stops this consumer from consuming elements from the buffer.
     */
    void stop() override
    {
        quitSignal_.exchange(true);
        thread_.join();
    }

private:

    /**
     * Starts extracting elements from the buffer 'buffer'.
     *
     * @param[in/out] buffer The buffer to extract elements from.
     * @param[in] delay The delay that the consumer will take after consuming one item.
     */
    void run(SharedBuffer* buffer, const std::chrono::milliseconds& delay)
    {
        while(buffer->isRunning() && !quitSignal_.load())
        {
            buffer->pop(this);
            std::this_thread::sleep_for(delay);
        }
    }

    std::thread thread_;
    std::atomic<bool> quitSignal_;
};

/**
 * Class to manage all the producers and consumers.
 */
class ProducersConsumersManager
{
public:

    /**
     * Constructor
     * @param[in/out] sharedBuffer The buffer shared among producers and consumers.
     */
    ProducersConsumersManager(SharedBuffer* sharedBuffer)
    : sharedBuffer_(sharedBuffer)
    {
        addProducer();
        addConsumer();
    }

    /**
     * Adds a producer to produce items into the buffer 'sharedBuffer_'.
     */
    void addProducer()
    {
        producers_.push_back(new Producer(sharedBuffer_, std::chrono::milliseconds(500)));
    }

    /**
     * Adds a consumer to consume items from the buffer 'sharedBuffer_'.
     */
    void addConsumer()
    {
        consumers_.push_back(new Consumer(sharedBuffer_, std::chrono::milliseconds(500)));
    }

    /**
     * Removes a consumer.
     */
    void removeConsumer()
    {
        std::list<Consumer* >::iterator consumer = consumers_.begin();
        if (consumer == consumers_.end())
        {
            return;
        }
        (*consumer)->stop();
        consumers_.erase(consumer);
        delete (*consumer);
    }

    /**
     * Removes a producer.
     */
    void removeProducer()
    {
        std::list<Producer* >::iterator producer = producers_.begin();
        if (producer == producers_.end())
        {
            return;
        }
        (*producer)->stop();
        producers_.erase(producer);
        delete (*producer);
    }

    ~ProducersConsumersManager()
    {
        for(auto consumer: consumers_)
        {
            consumer->stop();
            delete consumer;
        }

        for(auto producer: producers_)
        {
            producer->stop();
            delete producer;
        }
    }
private:
    SharedBuffer* sharedBuffer_;
    std::list<Consumer* > consumers_;
    std::list<Producer* > producers_;
};

int main()
{
    std::vector<IBufferItem* > buffer;
    for(size_t i = 0; i < 10; i++)
    {
        buffer.push_back(new BufferItem);
    }

    SharedBuffer sharedBuffer(buffer);
    ProducersConsumersManager manager(&sharedBuffer);

    int input = -1;
    while (input != 0)
    {
        std::cin >> input;
        switch(input)
        {
            case 1:
                manager.addProducer();
                break;
            case 2:
                manager.removeProducer();
                break;
            case 3:
                manager.addConsumer();
                break;
            case 4:
                manager.removeConsumer();
                break;
        };
    }
    sharedBuffer.stop();

    for(auto itemBuffer: buffer)
    {
        delete itemBuffer;
    }
    return 0;
}
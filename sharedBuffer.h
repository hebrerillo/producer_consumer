#ifndef PC_SHARED_BUFFER_H
#define PC_SHARED_BUFFER_H

#include <mutex>
#include <condition_variable>
#include <vector>
#include <list>
#include <assert.h>
#include <atomic>
#include <chrono>

class Producer;
class Consumer;

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
 * Class that represents the shared buffer between producers and consumers.
 * 
 */
class SharedBuffer
{
public:
    using ProducerIterator = std::list<Producer* >::iterator;
    using ConsumerIterator = std::list<Consumer* >::iterator;
    using ItemsBuffer = std::vector<IBufferItem* >;

    /**
     * Constructor
     *
     * @param[int/out] buffer The buffer to produce and consume items. 
     * @note Important!! All the items in the buffer should be empty.
     */
    SharedBuffer(const ItemsBuffer& buffer);

    /**
     * Adds an element to the buffer in the 'currentIndex_' position and increases 'currentIndex_'. This is the producer role.
     *
     * @param[in] producer The producer.
     * @note If the buffer is full, this call will block until a consumer consumes an item.
     */
    void produce(const Producer* producer);

    /**
     * Extracts the element 'currentIndex_' from the buffer and decreases 'currentIndex_'. This is the consumer role.
     *
     * @param[in] consumer The consumer.
     * @note If the buffer is empty, this call will block until a producer produces an item.
     */
    void consume(const Consumer* consumer);

    /**
     * Stops the buffer from accepting and/or returning elements.
     */
    void stop();

    /**
     * Whether producers and consumers can produce and consume elements respectively.
     */
    bool isRunning() const;

    /**
     * Adds a producer to produce items into the buffer 'buffer_'.
     *
     * @param[in] delay The delay the producer will take after producing an element.
     */
    void addProducer(const std::chrono::milliseconds& delay);

    /**
     * Adds a consumer to consume items from 'buffer_'.
     *
     * @param[in] delay The delay the consumer will take after consuming an element.
     */
    void addConsumer(const std::chrono::milliseconds& delay);

    /**
     * Removes a consumer.
     */
    void removeConsumer();

    /**
     * Removes a producer.
     */
    void removeProducer();

    /**
     * Removes all consumers.
     */
    void removeConsumers();

    /**
     * Removes all producers.
     */
    void removeProducers();

private:
    size_t currentIndex_; //The index of the last produced item.
    ItemsBuffer buffer_;
    std::mutex mutex_;
    std::condition_variable quitCV_;
    std::atomic<bool> quitSignal_;
    std::list<Consumer* > consumers_;
    std::list<Producer* > producers_;
};

#endif
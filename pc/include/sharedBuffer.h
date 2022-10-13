#ifndef PC_SHARED_BUFFER_H
#define PC_SHARED_BUFFER_H

#include <mutex>
#include <condition_variable>
#include <vector>
#include <list>
#include <atomic>
#include <chrono>
#include "IPC.h"
#include "IBufferItem.h"

class Producer;
class Consumer;

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
    explicit SharedBuffer(const IPC::ItemsBuffer& buffer);

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
     * Notifies that an external event happened. An example of an external event is the removal of a producer or a consumer.
     */
    void notify();

    /**
     * Whether producers and consumers can produce and consume elements respectively.
     */
    bool isRunning() const;

    /**
     * @return The index of the next item to be filled in the buffer.
     */
    size_t getCurrentIndex() const;

private:

    /**
     * Calculates the current index based on the last filled item on 'buffer_'.
     */
    void calculateCurrentIndex();

    size_t currentIndex_; //The index of the next item to be produced.
    IPC::ItemsBuffer buffer_;
    mutable std::mutex mutex_; //To synchornize accesses to 'currentIndex_' and 'buffer_'.
    std::condition_variable quitCV_;
    std::atomic<bool> quitSignal_;
};

#endif